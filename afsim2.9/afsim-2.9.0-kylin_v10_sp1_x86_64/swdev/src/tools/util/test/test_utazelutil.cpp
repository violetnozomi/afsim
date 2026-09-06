// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <gtest/gtest.h>

#include "UtAzElUtil.hpp"

TEST(UtAzElUtil, StringToEnum)
{
   std::string s1 = "Linear";
   std::string s2 = "linear";
   std::string s3 = "LiNeAr";
   std::string s4 = "lInEaR";
   std::string s5 = "Logarithmic";
   std::string s6 = "logarithmic";
   std::string s7 = "LoGaRiThMiC";
   std::string s8 = "lOgArItHmIc";

   // the following strings should fail to convert to an enum value
   std::string s9  = "cLinear";
   std::string s10 = "cLogarithmic";
   std::string s11 = "Lin";
   std::string s12 = "Log";

   using ut::azel::InterpolationType;
   InterpolationType itype;

   // the following conversions should work
   // test 1
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, "Linear"));
   EXPECT_EQ(itype, InterpolationType::cLinear);

   // test 2
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, "Logarithmic"));
   EXPECT_EQ(itype, InterpolationType::cLogarithmic);

   // test 3
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s1));
   EXPECT_EQ(itype, InterpolationType::cLinear);

   // test 4
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s2));
   EXPECT_EQ(itype, InterpolationType::cLinear);

   // test 5
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s3));
   EXPECT_EQ(itype, InterpolationType::cLinear);

   // test 6
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s4));
   EXPECT_EQ(itype, InterpolationType::cLinear);

   // test 7
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s5));
   EXPECT_EQ(itype, InterpolationType::cLogarithmic);

   // test 8
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s6));
   EXPECT_EQ(itype, InterpolationType::cLogarithmic);

   // test 9
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s7));
   EXPECT_EQ(itype, InterpolationType::cLogarithmic);

   // test 10
   EXPECT_TRUE(ut::azel::util::StringToEnum(itype, s8));
   EXPECT_EQ(itype, InterpolationType::cLogarithmic);

   // the following conversions should fail
   // test 11
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, "cLinear"));

   // test 12
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, "cLogarithmic"));

   // test 13
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, s9));

   // test 14
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, s10));

   // test 15
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, s11));

   // test 16
   EXPECT_FALSE(ut::azel::util::StringToEnum(itype, s12));
}

TEST(UtAzElUtil, EnumToString)
{
   using ut::azel::InterpolationType;
   InterpolationType it1 = InterpolationType::cLinear;
   InterpolationType it2 = InterpolationType::cLogarithmic;
   InterpolationType it3 = static_cast<InterpolationType>(3);

   EXPECT_EQ("linear", ut::azel::util::EnumToString(it1));
   EXPECT_EQ("logarithmic", ut::azel::util::EnumToString(it2));
   EXPECT_EQ("unknown", ut::azel::util::EnumToString(it3));
}
