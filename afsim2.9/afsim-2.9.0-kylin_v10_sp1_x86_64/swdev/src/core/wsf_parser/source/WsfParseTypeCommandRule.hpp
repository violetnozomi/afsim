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

#ifndef WSFPARSETYPECOMMANDRULE_HPP
#define WSFPARSETYPECOMMANDRULE_HPP

#include "WsfParseRule.hpp"

class WsfParseTypeCommandRule : public WsfParseRule
{
public:
   WsfParseTypeCommandRule(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, WsfParseRule::cTYPE_COMMAND)
   {
   }
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
};

#endif
