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

#ifndef WSFPARSEFORMAT_HPP
#define WSFPARSEFORMAT_HPP
#include "wsf_parser_export.h"

#include <cstddef>
#include <map>
#include <set>

class WsfParseNode;
class UtTextDocument;

class WSF_PARSER_EXPORT WsfParseFormat
{
public:
   WsfParseFormat();
   void                             Format(const std::set<WsfParseNode*>& aFormatNodes);
   const std::set<UtTextDocument*>& GetModifiedDocuments() { return mModifiedDocuments; }

private:
   bool                    NodeFormatRequested(WsfParseNode* aNodePtr) const;
   void                    FormatBlockNodes(WsfParseNode* aRootNode);
   size_t&                 InsertedChars(UtTextDocument*);
   int                     mBlockIndent;
   std::set<WsfParseNode*> mFormatNodes;
   // Number of inserted chars for each file
   // may be negative!
   std::map<UtTextDocument*, size_t> mInsertedChars;
   std::set<UtTextDocument*>         mModifiedDocuments;
};
#endif
