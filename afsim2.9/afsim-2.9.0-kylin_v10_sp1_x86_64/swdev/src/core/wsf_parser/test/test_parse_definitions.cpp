// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UnitTestFileHelper.h"
#include "WsfParseDefinitions.hpp"

TEST(ParseDefinitionsTest, Construction)
{
   WsfParseDefinitions testDefinitions;
   ASSERT_TRUE(testDefinitions.GetDefinitionErrors().empty());
   ASSERT_NE(nullptr, testDefinitions.GetGlobalRule());
}

TEST(ParseDefinitionsTest, AddGrammar)
{
   std::string         grammarFilePath = UnitTestFilePathHelper::Instance().Path("core/wsf/grammar/wsf.ag");
   std::ifstream       grammarStream(grammarFilePath);
   WsfParseDefinitions testDefinitions;
   testDefinitions.AddGrammar("wsf.ag", grammarStream);
   ASSERT_TRUE(testDefinitions.GetDefinitionErrors().empty());
}
