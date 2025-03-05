#include "XMLWriter.h"
#include <vector>
#include <string>
#include <memory>

struct CXMLWriter::SImplementation
{
    std::shared_ptr<CDataSink> DDataSink;  // data sink thast for writing the output.
    std::vector<std::string> DElementList; // its the stock of open elements.

    // constructor initializes the data sink.
    SImplementation(std::shared_ptr<CDataSink> sink)
        : DDataSink(sink) {}

    // writes a plain string to the data sink
    // returns false if writing fails
    bool OutputString(const std::string_view str)
    {
        for (char tempCh : str)
        {
            if (!DDataSink->Put(tempCh))
            {
                return false;
            }
        }
        return true;
    }

    // writes an escaped version of the string (e.g., for special XML characters).
    // used switch and case
    bool StringEscaped(const std::string &str)
    {
        for (char tempCh : str)
        {
            switch (tempCh)
            {
            case '<':
                if (!OutputString("&lt;"))
                {
                    return false;
                }
                break;

            case '>':
                if (!OutputString("&gt;"))
                {
                    return false;
                }
                break;

            case '&':
                if (!OutputString("&amp;"))
                {
                    return false;
                }
                break;

            case '"':
                if (!OutputString("&quot;"))
                {
                    return false;
                }
                break;

            case '\'':
                if (!OutputString("&apos;"))
                {
                    return false;
                }
                break;

            default:
                if (!DDataSink->Put(tempCh))
                {
                    return false;
                }
            }
        }
        return true;
    }
    bool FinalizeOutput()
    {
        for (auto temps = std::make_reverse_iterator(DElementList.end());
             temps != std::make_reverse_iterator(DElementList.begin()); ++temps)
        {
            // Ensure ALL parts of the closing tag are written correctly
            if (!OutputString("</"))
            {
                return false;
            }
            if (!OutputString(*temps))
            {
                return false;
            }
            if (!OutputString(">"))
            {
                return false;
            }
        }
        DElementList.clear();
        return true;
    }
    

    // writes the provided XML entity to the output.
    bool OutputEntity(const SXMLEntity &entity)
    {
        switch (entity.DType)
        {
        case SXMLEntity::EType::StartElement:
            // write the opening tag for the element.
            if (!OutputString("<") || !OutputString(entity.DNameData))
            {
                return false;
            }
            // write all attributes for the element.
            for (const auto &attribute : entity.DAttributes)
            {
                if (!OutputString(" ") || !OutputString(attribute.first) || !OutputString("=\"") || !StringEscaped(attribute.second) || !OutputString("\""))
                {
                    return false;
                }
            }
            if (!OutputString(">"))
            {
                return false;
            }
            DElementList.push_back(entity.DNameData); // addd to the delementlist stack
            break;

        case SXMLEntity::EType::EndElement:
            // wclose the tag write it
            if (!OutputString("</") || !OutputString(entity.DNameData) || !OutputString(">"))
            {
                return false;
            }
            if (!DElementList.empty())
            {
                DElementList.pop_back(); // remove the element from the stack.
            }
            break;

        case SXMLEntity::EType::CharData:
            if (!StringEscaped(entity.DNameData))
            {
                return false;
            }
            break;

        case SXMLEntity::EType::CompleteElement:
            if (!OutputString("<") || !OutputString(entity.DNameData))
            {
                return false;
            }
            // write all attributes for the element.
            for (const auto &attribute : entity.DAttributes)
            {
                if (!OutputString(" ") || !OutputString(attribute.first) || !OutputString("=\"") || !StringEscaped(attribute.second) || !OutputString("\""))
                {
                    return false;
                }
            }
            if (!OutputString("/>"))
            {
                return false;
            }
            break;
        }
        return true;
    }
};

// constructor initializes the XMLWriter
CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink))
{
}

// destructor here
CXMLWriter::~CXMLWriter() = default;

// go tht output and write the output to the data sink
bool CXMLWriter::Flush()
{
    return DImplementation->FinalizeOutput();
}

// write the entity
bool CXMLWriter::WriteEntity(const SXMLEntity &entity)
{
    return DImplementation->OutputEntity(entity);
}