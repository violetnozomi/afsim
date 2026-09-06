// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <sstream>
#include <stdexcept>

#include <gtest/gtest.h>

#include "TextTable.hpp"


TEST(TextTable, basic)
{
   std::stringstream expected;
   expected << "+--+--+" << std::endl
            << "|C1|C2|" << std::endl
            << "+--+--+" << std::endl
            << "|a1|a2|" << std::endl
            << "|b1|b2|" << std::endl
            << "+--+--+" << std::endl;
   const std::string actual = artificer::TextTable({{"C1", 2}, {"C2", 2}}, {{"a1", "a2"}, {"b1", "b2"}}).Render();
   EXPECT_EQ(expected.str(), actual);
}

TEST(TextTable, truncated)
{
   std::stringstream expected;
   expected << "+--+--+" << std::endl
            << "|C1|C2|" << std::endl
            << "+--+--+" << std::endl
            << "|a1|a2|" << std::endl
            << "|b1|b2|" << std::endl
            << "+--+--+" << std::endl;
   const std::string actual = artificer::TextTable({{"C1", 2}, {"C2", 2}},
                                                   {{"a1 this should be truncated", "a2 this should be truncated"},
                                                    {"b1 this should be truncated", "b2 this should be truncated"}})
                                 .Render();
   EXPECT_EQ(expected.str(), actual);
}

TEST(TextTable, truncated2)
{
   std::stringstream expected;
   expected << "+----------+-----+" << std::endl
            << "|C1        |C2   |" << std::endl
            << "+----------+-----+" << std::endl
            << "|a1        |a2 th|" << std::endl
            << "|b1 this sh|b2   |" << std::endl
            << "+----------+-----+" << std::endl;
   const std::string actual =
      artificer::TextTable({{"C1", 10}, {"C2", 5}},
                           {{"a1", "a2 this should be truncated"}, {"b1 this should be truncated", "b2"}})
         .Render();
   EXPECT_EQ(expected.str(), actual);
}

TEST(TextTable, NoData)
{
   std::stringstream expected;
   // clang-format off
   expected << "+--+--+" << std::endl
            << "|C1|C2|" << std::endl
            << "+--+--+" << std::endl
            << "+--+--+" << std::endl;
   // clang-format on
   const std::string actual = artificer::TextTable({{"C1", 2}, {"C2", 2}}, {}).Render();
   EXPECT_EQ(expected.str(), actual);
}

TEST(TextTable, NoColumns)
{
   artificer::TextTable table({}, {{"a1", "a2"}, {"b1", "b2"}});
   EXPECT_THROW(table.Render(), std::runtime_error);
}

TEST(TextTable, RowSizeMismatch)
{
   std::stringstream expected;
   expected << "+--+--+" << std::endl
            << "|C1|C2|" << std::endl
            << "+--+--+" << std::endl
            << "|a1|  |" << std::endl
            << "|b1|b2|" << std::endl
            << "+--+--+" << std::endl;
   const std::string actual = artificer::TextTable({{"C1", 2}, {"C2", 2}}, {{"a1"}, {"b1", "b2", "b3"}}).Render();
   EXPECT_EQ(expected.str(), actual);
}
