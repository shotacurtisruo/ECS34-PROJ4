#include "XMLReader.h"
#include "XMLEntity.h"
#include <expat.h>
#include <queue>
#include <vector>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> Source;
    XML_Parser Parser;
    std::queue<SXMLEntity> EntityQueue;
    std::string CharBuffer;
    bool IsEndOfData;

    static void OnStartElement(void* userData, const char* name, const char** attrs) {
        auto* impl = static_cast<SImplementation*>(userData);

        if (!impl->CharBuffer.empty()) {
            impl->EntityQueue.push({SXMLEntity::EType::CharData, impl->CharBuffer});
            impl->CharBuffer.clear();
        }

        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        for (int i = 0; attrs[i]; i += 2) {
            if (attrs[i + 1]) {
                entity.DAttributes.emplace_back(attrs[i], attrs[i + 1]);
            }
        }

        impl->EntityQueue.push(entity);
    }

    static void OnEndElement(void* userData, const char* name) {
        auto* impl = static_cast<SImplementation*>(userData);

        if (!impl->CharBuffer.empty()) {
            impl->EntityQueue.push({SXMLEntity::EType::CharData, impl->CharBuffer});
            impl->CharBuffer.clear();
        }

        impl->EntityQueue.push({SXMLEntity::EType::EndElement, name});
    }

    static void OnCharacterData(void* userData, const char* data, int len) {
        auto* impl = static_cast<SImplementation*>(userData);
        if (data && len > 0) {
            impl->CharBuffer.append(data, len);
        }
    }

    explicit SImplementation(std::shared_ptr<CDataSource> src)
        : Source(std::move(src)), IsEndOfData(false) {
        Parser = XML_ParserCreate(nullptr);
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, OnStartElement, OnEndElement);
        XML_SetCharacterDataHandler(Parser, OnCharacterData);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    bool FetchEntity(SXMLEntity& entity, bool skipCharData = false) {
        if (!EntityQueue.empty()) {
            entity = EntityQueue.front();
            EntityQueue.pop();

            return skipCharData && entity.DType == SXMLEntity::EType::CharData
                       ? FetchEntity(entity, skipCharData)
                       : true;
        }

        std::vector<char> buffer(4096);
        while (!IsEndOfData && EntityQueue.empty()) {
            size_t bytesRead = 0;

            while (bytesRead < buffer.size() && !Source->End()) {
                char ch;
                if (Source->Get(ch)) {
                    buffer[bytesRead++] = ch;
                } else {
                    break;
                }
            }

            if (bytesRead == 0) {
                IsEndOfData = true;
                return false;
            }

            if (XML_Parse(Parser, buffer.data(), bytesRead, bytesRead == 0) == XML_STATUS_ERROR) {
                return false;
            }

            if (!EntityQueue.empty()) {
                entity = EntityQueue.front();
                EntityQueue.pop();

                return skipCharData && entity.DType == SXMLEntity::EType::CharData
                           ? FetchEntity(entity, skipCharData)
                           : true;
            }
        }

        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->IsEndOfData && DImplementation->EntityQueue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity& entity, bool skipCharData) {
    return DImplementation->FetchEntity(entity, skipCharData);
}
