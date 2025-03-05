//neeed to edi thtis file

#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>

TEST(DSVTest, PokemonReadWrite) {
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("Pokemon,Type,Level\nPikachu,Electric,25\nCharizard,Fire,36\n");
    std::shared_ptr<CStringDataSink> sink = std::make_shared<CStringDataSink>();

    CDSVReader reader(src, ',');
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row;
    while (!reader.End()) {
        if (reader.ReadRow(row)) {
            writer.WriteRow(row);
        }
    }

    EXPECT_EQ(sink->String(), "Pokemon,Type,Level\nPikachu,Electric,25\nCharizard,Fire,36\n");
}