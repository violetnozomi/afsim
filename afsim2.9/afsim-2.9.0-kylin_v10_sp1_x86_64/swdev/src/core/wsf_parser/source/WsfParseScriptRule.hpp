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

#ifndef WSFPARSESCRIPTRULE_HPP
#define WSFPARSESCRIPTRULE_HPP

#include "WsfParseRule.hpp"

//! WsfParseScriptRule collects a block of text to be parsed later by the script parser
class WsfParseScriptRule : public WsfParseRule
{
public:
   enum ScriptType
   {
      cFUNCTION,
      cIMPLICIT_FUNCTION,
      cSCRIPT_VARIABLES
   };

   WsfParseScriptRule(WsfParseRuleMemory& aMem, ScriptType aScriptType);
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;

   ScriptType mScriptType;
};

#endif
