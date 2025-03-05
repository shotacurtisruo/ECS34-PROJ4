#include <gtest/gtest.h>
#include "StringUtils.h"


TEST(StringUtilsTest, SliceTest){
   EXPECT_EQ("string", StringUtils::Slice("string", 0, 6));
  
}


TEST(StringUtilsTest, Capitalize){
   EXPECT_EQ("String", StringUtils::Capitalize("string"));
}


TEST(StringUtilsTest, Upper){
   EXPECT_EQ("STRING", StringUtils::Upper("string"));
}


TEST(StringUtilsTest, Lower){
   EXPECT_EQ("string", StringUtils::Lower("STRING"));
}


TEST(StringUtilsTest, LStrip){
   EXPECT_EQ("string", StringUtils::LStrip("   string"));
}


TEST(StringUtilsTest, RStrip){
   EXPECT_EQ("string", StringUtils::RStrip("string   "));
}


TEST(StringUtilsTest, Strip){
   EXPECT_EQ("string", StringUtils::Strip("   string   "));
}


TEST(StringUtilsTest, Center){
   EXPECT_EQ("  string  ", StringUtils::Center("string", 10));
}


TEST(StringUtilsTest, LJust){
   EXPECT_EQ("string    ", StringUtils::LJust("string", 10));
}


TEST(StringUtilsTest, RJust){
   EXPECT_EQ("    string", StringUtils::RJust("string", 10));
}


TEST(StringUtilsTest, Replace){
   EXPECT_EQ("strang", StringUtils::Replace("string", "i", "a"));
}


TEST(StringUtilsTest, Split){
 
}


TEST(StringUtilsTest, Join){
//std::string Join(const std::string &str, const std::vector< std::string >&vect);
  


}


TEST(StringUtilsTest, ExpandTabs){
}
TEST(StringUtilsTest, EditDistance){
  
}