#include "DSVReader.h"
#include "DataSource.h"

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> source;
    char Delimiter;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter) : source(src), Delimiter(delimiter) {
    }

    bool ReadRow(std::vector<std::string> &row) {

        row.clear();
        std::string field;
        char c;
        bool quoted = false;

        while (!source->End()) {
            source->Get(c);

            if (c == '"'){
                char nc;
                bool peeked = source->Peek(nc);
                if (peeked == true && nc == '"'){
                    source->Get(nc);
                    field += '"';
                } else if (quoted == false) {
                    quoted = true;
                } else {
                    quoted = false;
                }
            } else if ((c == Delimiter || c == '\n')  && !quoted){
                row.push_back(field);
                field.clear();
                if (c == '\n') {
                    return true;
                }
            } else { 
                field += c;
            }
        }
        if (!field.empty() || !row.empty()) { 
            row.push_back(field);
            return true;
        }
        return false;
    }
};

CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter) : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {
}

CDSVReader::~CDSVReader() = default;

bool CDSVReader::End() const {
    return DImplementation->source->End();
}

bool CDSVReader::ReadRow(std::vector< std::string > &row) {
   return DImplementation->ReadRow(row);
}