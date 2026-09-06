// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtLexicalCast.hpp"

TEST(UtLexicalCast, IntegerConversion)
{
   EXPECT_THROW(ut::lexical_cast<int>("10.42"), ut::bad_lexical_cast);
   EXPECT_THROW(ut::lexical_cast<int>("1e4"), ut::bad_lexical_cast);
   EXPECT_EQ(ut::lexical_cast<int>("12345"), std::stoi("12345"));
   EXPECT_EQ(ut::lexical_cast<std::string>(12345), std::to_string(12345));
}
