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

#include "WsfParseScriptRule.hpp"

#include "UtTextDocument.hpp"
#include "WsfParseScriptScanner.hpp"
#include "WsfParser.hpp"


WsfParseScriptRule::WsfParseScriptRule(WsfParseRuleMemory& aMem, ScriptType aScriptType)
   : WsfParseRule(aMem, cSCRIPT_BLOCK)
   , mScriptType(aScriptType)
{
}

bool WsfParseScriptRule::Read(WsfParser& aParser, WsfParseNode*& aNode)
{
   // Assume this is contained in a block.
   UtTextDocumentRange first = aParser.ReadWord();
   if (first.Valid())
   {
      UtTextDocumentRange last = first;
      UtTextDocumentRange next;
      while (next = aParser.ReadWord(), next.Valid() && next.mSource == first.mSource)
      {
         last = next;
      }
      aParser.UndoRead();
      UtTextDocumentRange wholeRange = first;
      wholeRange.Extend(last);
      std::string txt = wholeRange.Text();
      // TestRead(aParser, wholeRange, aNode);
      aNode = aParser.NewNode(this, "script-block", wholeRange);
      switch (mScriptType)
      {
      case cFUNCTION:
         aNode->SetFlags(WsfParseNode::cSCRIPT_FUNCTION);
         break;
      case cIMPLICIT_FUNCTION:
         aNode->SetFlags(WsfParseNode::cSCRIPT_BLOCK);
         break;
      case cSCRIPT_VARIABLES:
         aNode->SetFlags(WsfParseNode::cSCRIPT_VARIABLES);
         break;
      }
      return true;
   }
   return false;
}
