#include "DSVWriter.h"
#include "DataSink.h"

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> sink;
    char delimiter;
    bool quoteall;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall): sink(sink), delimiter(delimiter), quoteall(quoteall) {
    }

    bool WriteRow(const std::vector<std::string> &row){
        for (size_t i = 0; i < row.size(); i++){
            if (quoteall || row[i].find(delimiter) != std::string::npos || row[i].find('"') != std::string::npos) {
                sink->Put('"');

                for (char c : row[i]) {
                    if (c == '"') {
                        sink->Put('"');
                    }
                    sink->Put(c);
                }
                sink->Put('"');
            } else {
                for (char c : row[i]) {
                    sink->Put(c);
                }
            }
            if (i != row.size() - 1) {
                sink->Put(delimiter);
            }
        }
        sink->Put('\n');
        return true;
    }

};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall) : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {
}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string> &row){
    return DImplementation->WriteRow(row);
}
