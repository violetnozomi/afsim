// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfParseTypeCommandRule.hpp"

#include "WsfParseIndex.hpp"
#include "WsfParseType.hpp"
#include "WsfParser.hpp"


bool WsfParseTypeCommandRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   WsfParseType* typePtr = aParser.GetParseIndex()->GetCurrentType();
   if (!typePtr)
   {
      return false;
   }
   WsfParseRule* rulePtr = typePtr->GetRule();
   if (rulePtr != nullptr)
   {
      return rulePtr->Read(aParser, aNode);
   }
   return false;
}
