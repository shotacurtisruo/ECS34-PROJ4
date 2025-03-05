#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>
 //test for XML stuff

 TEST(XMLTest, BasicReadWrite) {
    std::shared_ptr<CStringDataSource> source = std::make_shared<CStringDataSource>("<tag>data</tag>");
    std::shared_ptr<CStringDataSink> dataSink = std::make_shared<CStringDataSink>();

    CXMLReader reader(source);
    CXMLWriter writer(dataSink);

    SXMLEntity entity;
    while (!reader.End()) {
        if (reader.ReadEntity(entity)) {
            writer.WriteEntity(entity);
        }
    }

    EXPECT_EQ(dataSink->String(), "<tag>data</tag>");
}

