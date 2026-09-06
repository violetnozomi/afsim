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

#ifndef WSFPARSEDEFINITIONS_HPP
#define WSFPARSEDEFINITIONS_HPP

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "UtMemory.hpp"
#include "WsfParseDebugMarkers.hpp"
#include "WsfParseGrammar.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseScriptScanner.hpp"
#include "WsfParseType.hpp"
class WsfParseNamedRule;
class WsfParseQuotedStringRule;
class WsfParseRuleReference;
class WsfParseStringRule;

//! Maintains a description of the wsf core grammar and other static data
//! required for parsing.  The lifetime of this object is required to be at
//! least as long as any WsfParser or WsfParseNodes.
class WSF_PARSER_EXPORT WsfParseDefinitions
{
public:
   WsfParseDefinitions();
   ~WsfParseDefinitions();
   WsfParseDefinitions(const WsfParseDefinitions&) = delete;
   WsfParseDefinitions(WsfParseDefinitions&&)      = default;
   WsfParseDefinitions& operator=(const WsfParseDefinitions&) = delete;
   WsfParseDefinitions& operator=(WsfParseDefinitions&&) = default;


   void AddGrammar(const std::string& aFileName, std::istream& aInput);

   bool Initialize();

   WsfParseNamedRule* GetRoot() const { return mRootRulePtr; }
   WsfParseNamedRule* GetGlobalRule() const { return mGlobalRules; }
   WsfParseType*      mBasicTypes;

   //! Returns a list of errors encountered while processing the grammar definition
   const std::list<std::string>& GetDefinitionErrors() const { return mDefinitionErrors; }

   std::string GetFullRuleName(WsfParseNamedRule& aRule);

   WsfParseStruct* GetRootStruct() { return mRootStructPtr; }

   std::string GetVersionString() { return mVersionString; }
   struct CompareCharPtr
   {
      bool operator()(const char* lhs, const char* rhs) const
      {
         for (;;)
         {
            if (*lhs < *rhs)
            {
               return true;
            }
            if (*rhs < *lhs)
            {
               return false;
            }
            if (*lhs == 0)
            {
               return false;
            }
            ++lhs;
            ++rhs;
         }
      }
   };

   std::map<std::string, std::unique_ptr<WsfParseActionFunction>> mFunctions;

   const WsfGrammar::M::Expr& GetInputGrammar() { return mInputGrammar; }

private:
   void InitializeFunctions();

   void AddDefinitionError(const std::string& aError);

   const char* MakeCharPtr(const std::string& aString);

   WsfParseActionAddress ResolveActionAddress(WsfParseValueType*&     aContainer,
                                              const WsfParseTypePath& aPath,
                                              bool                    aRelative = true);

   WsfParseActionAddress ResolveActionAddress(WsfParseValueType*& aContainer, const std::string& aAddrText);

   void                ProcessRuleParams(const WsfGrammar::M::Expr& aRule,
                                         WsfParseNamedRule&         aParentRule,
                                         WsfParseNamedRule*         aRulePtr,
                                         int                        aFirstParam);
   void                SetNoCase(WsfParseRule* aRulePtr);
   WsfParseAction*     ProcessAction(const WsfGrammar::M::Expr& aRule);
   WsfParseActionPart* ProcessActionSub(const WsfGrammar::M::Expr& aRule);
   bool                ProcessRule(const WsfGrammar::M::Expr& aRule,
                                   WsfParseNamedRule&         aContextRule,
                                   WsfParseRule*              aParentRulePtr,
                                   WsfParseRule*&             aRVal);
   void                ResolveRule(WsfParseNamedRule& aParentRule, WsfParseRule*& aRulePtr, bool aAllowReplace);
   void                InitializeRule(WsfParseRule* aRulePtr, int aSequence);

   void ResolveReference(WsfParseNamedRule& aParentRule, const std::string& aReferenceName, WsfParseRule*& aReferencedRulePtr);
   void ResolveReference(WsfParseNamedRule&     aParentRule,
                         WsfParseRuleReference& aReference,
                         WsfParseRule*&         aReplacementRulePtr);

   void ResolveBase(WsfParseStruct* aStructPtr, WsfParseNamedRule* aParentRulePtr);

   void CheckActions(WsfParseRule* aRulePtr, WsfParseNamedRule* aContextPtr);
   void CheckAction(WsfParseRule* aRulePtr, WsfParseValueType*& aContainer, WsfParseAction* aAction, size_t aPrevNodeIndex);
   void               CheckActionPart(WsfParseRule*        aRulePtr,
                                      WsfParseValueType*&  aContainer,
                                      WsfParseActionPart*& aActionPart,
                                      size_t               aPrevNodeIndex);
   WsfParseValueType* GetTypeFromName(const std::string& aName);
   WsfParseValueType* GetTypeFromName(WsfParseNamedRule& aContext, const std::string& aName);
   WsfParseValueType* GetTypeFromPath(WsfParseNamedRule& aContext, WsfParseTypePath& aPath);

   WsfParseTypeName ProcessType(WsfParseNamedRule& aParentRule, const WsfGrammar::M::Expr& aTypeRule);

   WsfParseStringRule*       mBasic_String;
   WsfParseQuotedStringRule* mBasic_QuotableString;

   WsfGrammar::M::Expr mInputGrammar;

   WsfParseNamedRule*     mRootRulePtr;
   WsfParseNamedRule*     mGlobalRules;
   WsfParseStruct*        mRootStructPtr;
   std::list<std::string> mDefinitionErrors;

   WsfParseValueType mSkipType;
   // Store templated types such as List/XYZ etc...
   std::map<std::string, WsfParseValueType*> mTemplatedTypes;

   WsfParseRuleMemory    mRuleMem;
   std::set<std::string> mStrings;
   std::string           mVersionString;
   UT_MEMORY_DEBUG_MARKER(cMDB_PARSE_DEFINITIONS);
};
#endif
