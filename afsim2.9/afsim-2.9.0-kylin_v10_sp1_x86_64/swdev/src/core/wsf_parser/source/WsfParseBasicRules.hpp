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

#ifndef WSFPARSEBASICRULES_HPP
#define WSFPARSEBASICRULES_HPP

#include <set>

#include "WsfParseGrammar.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseRule.hpp"

//
// class WSF_PARSER_EXPORT IntegerRule : public Rule
// {
//    public:
//       IntegerRule() : Rule(cINT)
//       {
//          mReaderFlags |= cIS_SINGLE_TOKEN;
//       }
//       virtual bool Read(WsfParser&     aParser,
//                         WsfParseNode*& aNode);
//       virtual bool ReadRange(WsfParser&           aParser,
//                              UtTextDocumentRange& aRange,
//                              WsfParseNode*&       aNode,
//                              bool                 aCreateNode);
// };

class WSF_PARSER_EXPORT WsfParseRealRule : public WsfParseRule
{
public:
   WsfParseRealRule(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cREAL)
   {
      mReaderFlags |= cIS_SINGLE_TOKEN;
   }
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
};

class WSF_PARSER_EXPORT WsfParseIntRule : public WsfParseRule
{
public:
   WsfParseIntRule(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cINT)
   {
      mReaderFlags |= cIS_SINGLE_TOKEN;
   }
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
};

class WSF_PARSER_EXPORT WsfParseStringRule : public WsfParseRule
{
public:
   WsfParseStringRule(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cSTRING)
      , mFlags(0)
   {
      mReaderFlags |= cIS_SINGLE_TOKEN;
   }
   std::string mNodeType;
   int         mFlags;
   bool        Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
};

class WSF_PARSER_EXPORT WsfParseStringWithExceptionsRule : public WsfParseStringRule
{
public:
   WsfParseStringWithExceptionsRule(WsfParseRuleMemory& aMem)
      : WsfParseStringRule(aMem)
   {
   }
   std::set<std::string> mExceptions;
   bool                  Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;
};

class WSF_PARSER_EXPORT WsfParseLineStringRule : public WsfParseRule
{
public:
   WsfParseLineStringRule(WsfParseRuleMemory& aMem)
      : WsfParseRule(aMem, cLINE_STRING)
   {
   }
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
};

class WSF_PARSER_EXPORT WsfParseQuotedStringRule : public WsfParseRule
{
public:
   WsfParseQuotedStringRule(WsfParseRuleMemory& aMem, bool aQuotesOptional)
      : WsfParseRule(aMem, cQUOTED_STRING)
      , mFileReference(false)
      , mFileSearchPaths(false)
   {
      mQuotesOptional = aQuotesOptional;
   }
   bool        Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   void        SetFileReference(const std::string& aFilePath, WsfParser& aParser, WsfParseNode* aNodePtr);
   bool        mQuotesOptional;
   bool        mFileReference;
   bool        mFileSearchPaths;
   UtStringRef mFileType;
};

class WSF_PARSER_EXPORT WsfParseDelimited : public WsfParseRule
{
public:
   WsfParseDelimited(WsfParseRuleMemory& aMem);
   struct Section
   {
      Section()
         : mRulePtr(nullptr)
      {
      }
      std::string mPreDelimiter;
      // WsfParseRuleReference   mRule;
      WsfParseRule* mRulePtr;
      // WsfParseReader*   mReaderPtr;
   };
   bool Read(WsfParser& aParser, WsfParseNode*& aNode) override;
   bool ReadRange(WsfParser& aParser, UtTextDocumentRange& aRange, WsfParseNode*& aNode, bool aCreateNode) override;

   // These are members to avoid malloc's every time the reader executes
   // No state is actually saved
   std::vector<Section> mSections;
   bool                 mCaseSensitive;
   WsfParseNode*        mSubnodes[10];
   UtTextDocumentRange  mWords[10];
};

#endif
