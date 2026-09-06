// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include "Report.hpp"

using HeaderTest = ::testing::TestWithParam<std::tuple<std::string, bool, std::string, Header::CountType, Header::DataType>>;

TEST_P(HeaderTest, Parse)
{
   const auto& header = std::get<0>(GetParam());
   auto expectType = std::get<1>(GetParam());
   auto name = std::get<2>(GetParam());
   auto countType =  std::get<3>(GetParam());
   auto dataType = std::get<4>(GetParam());
   auto tag = Report::ParseHeaderTag(header, expectType);
   EXPECT_EQ(name, tag.name);
   EXPECT_EQ(countType, tag.countType);
   EXPECT_EQ(dataType, tag.dataType);
}

INSTANTIATE_TEST_CASE_P(ExpectType, HeaderTest, ::testing::Values(
                           std::make_tuple("time<time>", true, "time", Header::CountType::cNONE, Header::DataType::Time),
                           std::make_tuple("event<string>", true, "event", Header::CountType::cNONE, Header::DataType::String),
                           std::make_tuple("latitude<lat>", true, "latitude", Header::CountType::cNONE, Header::DataType::Lat),
                           std::make_tuple("longitude<lon>", true, "longitude", Header::CountType::cNONE, Header::DataType::Lon),
                           std::make_tuple("children_count<int>", true, "children_count", Header::CountType::cCOUNT, Header::DataType::Int),
                           std::make_tuple("child_number_#<double>", true, "child_number_#", Header::CountType::cINDEX, Header::DataType::Double),
                           std::make_tuple("elevation<angle>", true, "elevation", Header::CountType::cNONE, Header::DataType::Angle)
                        ));
INSTANTIATE_TEST_CASE_P(NoExpectType, HeaderTest, ::testing::Values(
                           std::make_tuple("time<time>", false, "time<time>", Header::CountType::cNONE, Header::DataType::String),
                           std::make_tuple("event", false, "event", Header::CountType::cNONE, Header::DataType::String),
                           std::make_tuple("latitude", false, "latitude", Header::CountType::cNONE, Header::DataType::String),
                           std::make_tuple("longitude", false, "longitude", Header::CountType::cNONE, Header::DataType::String),
                           std::make_tuple("children_count", false, "children_count", Header::CountType::cCOUNT, Header::DataType::String),
                           std::make_tuple("child_number_#", false, "child_number_#", Header::CountType::cINDEX, Header::DataType::String)
                        ));

using TokenizeTest = ::testing::TestWithParam<std::tuple<std::string, char, std::vector<std::string>>>;

TEST_P(TokenizeTest, Split)
{
   auto observed = Report::Tokenize(std::get<0>(GetParam()), std::get<1>(GetParam()));
   auto expected = std::get<2>(GetParam());
   EXPECT_EQ(expected, observed);
}

INSTANTIATE_TEST_CASE_P(Delimiter, TokenizeTest, ::testing::Values(
                           std::make_tuple("one,two,three", ',', std::vector<std::string> {"one", "two", "three"}),
                           std::make_tuple("a;b;c", ',', std::vector<std::string> {"a;b;c"}),
                           std::make_tuple("This is a test", ' ', std::vector<std::string> {"This", "is", "a", "test"}),
                           std::make_tuple("This is a test. This is also a test", '.', std::vector<std::string> {"This is a test", " This is also a test"})
                        ));

TEST(TokenizeTest, DefaultDelimiter)
{
   std::vector<std::string> observed;
   std::vector<std::string> expected = {"one", "two", "three"};
   Report::Tokenize("one,two,three", observed);
   EXPECT_EQ(expected, observed);

   expected = {"a;b;c"};
   observed.clear();
   Report::Tokenize("a;b;c", observed);
   EXPECT_EQ(expected, observed);
}
