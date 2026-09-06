// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <string>

#include <gtest/gtest.h>

#include "UtStringUtil.hpp"

TEST(UtStringUtil, ReplaceAll)
{
   std::string testStr        = "block something something end_block";
   std::string replaceStr     = "block ";
   std::string replacementStr = "/*block ";
   std::string expectedStr    = "/*block something something end_block";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   replaceStr     = " end_block";
   replacementStr = " end_block*/";
   expectedStr    = "/*block something something end_block*/";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   replaceStr     = " something";
   replacementStr = " something_else";
   expectedStr    = "/*block something_else something_else end_block*/";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   replaceStr     = " something_else";
   replacementStr = " something";
   expectedStr    = "/*block something something end_block*/";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   testStr        = "my_file_%d_%d.txt";
   replaceStr     = "%d";
   replacementStr = "1";
   expectedStr    = "my_file_1_1.txt";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   testStr        = "my_file_%d_%d.txt";
   replaceStr     = "%d";
   replacementStr = "11";
   expectedStr    = "my_file_11_11.txt";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);

   testStr        = "my_file_%d_%d.txt";
   replaceStr     = "%d";
   replacementStr = "111";
   expectedStr    = "my_file_111_111.txt";
   UtStringUtil::ReplaceAll(testStr, replaceStr, replacementStr);
   EXPECT_EQ(testStr, expectedStr);
}
