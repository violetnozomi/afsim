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

#include "WsfParseDefinitions.hpp"

#include <cassert>
#include <sstream>

#include "UtCast.hpp"
#include "UtLexicalCast.hpp"
#include "UtStringUtil.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseBasicRules.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseScriptRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeCommandRule.hpp"
#include "WsfParseTypeLoadRule.hpp"
#include "WsfParseUtil.hpp"
#include "grammar_parse/Parser.h"

WsfParseDefinitions::WsfParseDefinitions()
   : mRootRulePtr(nullptr)
   , mRootStructPtr(nullptr)
{
   mBasicTypes                                = new WsfParseType(UtStringRef());
   mGlobalRules                               = new WsfParseNamedRule(mRuleMem);
   WsfParseRuleDictionary* dict               = mGlobalRules->GetNested();
   mBasic_String                              = new WsfParseStringRule(mRuleMem);
   mBasic_QuotableString                      = new WsfParseQuotedStringRule(mRuleMem, true);
   WsfParseQuotedStringRule* fileRefReader    = new WsfParseQuotedStringRule(mRuleMem, true);
   fileRefReader->mFileReference              = true;
   fileRefReader->mFileSearchPaths            = true;
   WsfParseQuotedStringRule* outFileRefReader = new WsfParseQuotedStringRule(mRuleMem, true);
   outFileRefReader->mFileReference           = true;
   outFileRefReader->mFileSearchPaths         = false;

   dict->Add(new WsfParseNamedRule(mRuleMem, "string", mBasic_String));
   dict->Add(new WsfParseNamedRule(mRuleMem, "ignore", mBasic_String));
   dict->Add(new WsfParseNamedRule(mRuleMem, "quotable-string", mBasic_QuotableString));
   dict->Add(new WsfParseNamedRule(mRuleMem, "file-reference", fileRefReader));
   // dict->Add(new WsfParseNamedRule("output-file-reference", fileRefReader));
   dict->Add(new WsfParseNamedRule(mRuleMem, "line-string", new WsfParseLineStringRule(mRuleMem)));
   dict->Add(new WsfParseNamedRule(mRuleMem, "real", new WsfParseRealRule(mRuleMem)));
   dict->Add(new WsfParseNamedRule(mRuleMem, "integer", new WsfParseIntRule(mRuleMem)));
   WsfParseStringRule* typenameReaderPtr = new WsfParseStringRule(mRuleMem);
   typenameReaderPtr->mFlags             = WsfParseNode::cLAZY_TYPENAME_NODE;
   dict->Add(new WsfParseNamedRule(mRuleMem, "typename", typenameReaderPtr));
   dict->Add(new WsfParseNamedRule(mRuleMem, "string-except", new WsfParseStringWithExceptionsRule(mRuleMem)));
   WsfParseStringRule* typeRefNodePtr = new WsfParseStringRule(mRuleMem);
   typeRefNodePtr->mFlags             = WsfParseNode::cLAZY_TYPE_REFERENCE_NODE;
   dict->Add(new WsfParseNamedRule(mRuleMem, "typeref", typeRefNodePtr));

   WsfParseStringRule* namedStringNodePtr = new WsfParseStringRule(mRuleMem);
   namedStringNodePtr->mFlags             = WsfParseNode::cNAMED_NODE;
   dict->Add(new WsfParseNamedRule(mRuleMem, "name", namedStringNodePtr));

   WsfParseStringRule* errorReaderPtr = new WsfParseStringRule(mRuleMem);
   errorReaderPtr->mFlags             = WsfParseNode::cERROR_INCOMPLETE;
   dict->Add(new WsfParseNamedRule(mRuleMem, "Error", errorReaderPtr));
   dict->Add(new WsfParseNamedRule(mRuleMem, "TypeCommand", new WsfParseTypeCommandRule(mRuleMem)));
   dict->Add(new WsfParseNamedRule(mRuleMem,
                                   "ScriptFunctionBlock",
                                   new WsfParseScriptRule(mRuleMem, WsfParseScriptRule::cFUNCTION)));
   dict->Add(new WsfParseNamedRule(mRuleMem,
                                   "ScriptBlock",
                                   new WsfParseScriptRule(mRuleMem, WsfParseScriptRule::cIMPLICIT_FUNCTION)));
   dict->Add(new WsfParseNamedRule(mRuleMem,
                                   "ScriptVariables",
                                   new WsfParseScriptRule(mRuleMem, WsfParseScriptRule::cSCRIPT_VARIABLES)));
}

WsfParseDefinitions::~WsfParseDefinitions()
{
   // make a list of all unique value types and delete them
   std::vector<WsfParseValueType*> types = mRuleMem.CollectTypes();
   for (std::map<std::string, WsfParseValueType*>::iterator i = mTemplatedTypes.begin(); i != mTemplatedTypes.end(); ++i)
   {
      types.push_back(i->second);
   }
   std::sort(types.begin(), types.end());
   size_t typeCount = (std::unique(types.begin(), types.end()) - types.begin());
   for (size_t i = 0; i < typeCount; ++i)
   {
      delete types[i];
   }

   delete mBasicTypes;
}

void WsfParseDefinitions::AddGrammar(const std::string& aFileName, std::istream& aInput)
{
   std::string text;
   text.assign(std::istreambuf_iterator<char>(aInput), std::istreambuf_iterator<char>());
   WsfGrammar::Scanner scanner((const unsigned char*)text.c_str(), ut::cast_to_int(text.size()));
   WsfGrammar::Parser  parser(&scanner);
   parser.mRootRule.Swap(mInputGrammar);
   parser.Parse();
   parser.mRootRule.Swap(mInputGrammar);
   for (size_t i = 0; i < parser.errors->mErrors.size(); ++i)
   {
      AddDefinitionError("Parse error in " + aFileName + ": " + parser.errors->mErrors[i]);
   }
}

void WsfParseDefinitions::SetNoCase(WsfParseRule* aRulePtr)
{
   switch (aRulePtr->Type())
   {
   case WsfParseRule::cDELIMITED:
   {
      WsfParseDelimited* del = dynamic_cast<WsfParseDelimited*>(aRulePtr);
      del->mCaseSensitive    = false;
      for (size_t i = 0; i < del->mSections.size(); ++i)
      {
         WsfParseRule* secRule = del->mSections[i].mRulePtr;
         if (secRule)
         {
            SetNoCase(secRule);
         }
      }
   }
   break;
   case WsfParseRule::cLITERAL:
      dynamic_cast<WsfParseLiteral*>(aRulePtr)->mCaseSensitive = false;
      break;
   case WsfParseRule::cSEQUENCE:
   {
      WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(aRulePtr);
      for (size_t i = 0; i < seq->Sequence().size(); ++i)
      {
         SetNoCase(seq->Sequence()[i]);
      }
   }
   break;
   case WsfParseRule::cALTERNATE:
   {
      WsfParseAlternate* alt = dynamic_cast<WsfParseAlternate*>(aRulePtr);
      for (size_t i = 0; i < alt->Alternates().size(); ++i)
      {
         SetNoCase(alt->Alternates()[i]);
      }
   }
   break;
   default:
      break;
   }
}

void WsfParseDefinitions::ProcessRuleParams(const WsfGrammar::M::Expr& aRule,
                                            WsfParseNamedRule&         aParentRule,
                                            WsfParseNamedRule*         aRulePtr,
                                            int                        aFirstParam)
{
   for (auto i = ut::cast_to_size_t(aFirstParam); i < aRule.WordCount(); ++i)
   {
      if (aRule.GetWord(i).mLabel == "symbol" && aRulePtr->Type() == WsfParseRule::cSTRUCT)
      {
         WsfParseStruct*  structPtr  = dynamic_cast<WsfParseStruct*>(aRulePtr);
         WsfParseTypeName typeName   = ProcessType(aParentRule, aRule.GetWord(i));
         WsfParseType*    newTypePtr = new WsfParseType(typeName.mPath.back());
         newTypePtr->SetRule(structPtr);
         mBasicTypes->AddType(typeName.mPath, newTypePtr);
      }
      else if (aRule.GetWord(i).mLabel == "base_type" && aRulePtr->Type() == WsfParseRule::cSTRUCT)
      {
         WsfParseStruct* structPtr = dynamic_cast<WsfParseStruct*>(aRulePtr);
         structPtr->mBaseName      = aRule.GetWord(i).mText;
      }
      else if (aRule.GetWord(i).mLabel == "output")
      {
         aRulePtr->mOutputTypeName = aRule.GetWord(i).mText;
         aRulePtr->mInputTypeName.clear();
         aRulePtr->mInputTypePtr = nullptr;
      }
      else if (aRule.GetWord(i).mLabel == "input")
      {
         aRulePtr->mInputTypeName = aRule.GetWord(i).mText;
         aRulePtr->mOutputTypeName.clear();
         aRulePtr->mOutputTypePtr = nullptr;
      }
      else if (aRule.GetWord(i).mLabel == "id")
      {
         std::stringstream iss(aRule.GetWord(i).mText);
         if (!(iss >> aRulePtr->mUserId))
         {
            std::stringstream ss;
            ss << "Error: expected integer id; while processing " << GetFullRuleName(*aRulePtr) << ".\n";
            AddDefinitionError(ss.str());
         }
      }
      else if (!aRule.GetWord(i).mLabel.empty())
      {
         std::stringstream ss;
         ss << "Error: invalid optional parameter " << aRule.GetWord(i).mLabel << " while processing "
            << GetFullRuleName(*aRulePtr) << ".\n";
         AddDefinitionError(ss.str());
      }
      else
      {
         WsfParseRule* rv = nullptr;
         if (ProcessRule(aRule.GetWord(i), *aRulePtr, nullptr, rv) && rv)
         {
            WsfParseAlternate* ruleAlt = dynamic_cast<WsfParseAlternate*>(aRulePtr->GetDefinition());
            if (rv->Type() == WsfParseRule::cSEQUENCE)
            {
               WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(rv);
               seq->mParentRulePtr   = ruleAlt;
               ruleAlt->AddAlternate(seq);
            }
            else if (rv->Type() == WsfParseRule::cALTERNATE)
            {
               WsfParseAlternate* alt = dynamic_cast<WsfParseAlternate*>(rv);
               for (size_t i = 0; i < alt->Alternates().size(); ++i)
               {
                  ruleAlt->AddAlternate(alt->Alternates()[i]);
                  alt->Alternates()[i]->mParentRulePtr = ruleAlt;
               }
            }
            else if (rv->Type() == WsfParseRule::cLITERAL || rv->Type() == WsfParseRule::cRULE_REFERENCE)
            {
               WsfParseSequence* seq = new WsfParseSequence(mRuleMem);
               seq->AddRule(rv);
               seq->mParentRulePtr = ruleAlt;
               rv->mParentRulePtr  = seq;
               ruleAlt->AddAlternate(seq);
            }
            else
            {
               std::stringstream ss;
               ss << "Error: expected a sequence '{...}' while processing " << GetFullRuleName(*aRulePtr) << ".\n";
               AddDefinitionError(ss.str());
               assert(false);
               // delete rv;
            }
         }
      }
   }
}
WsfParseTypeName WsfParseDefinitions::ProcessType(WsfParseNamedRule& aParentRule, const WsfGrammar::M::Expr& aTypeRule)
{
   if (!(aTypeRule.mType == "type" || aTypeRule.mType == "subtype"))
   {
      std::stringstream ss;
      ss << "Expected a type definition ('type' or 'subtype') while processing " << GetFullRuleName(aParentRule) << "\n";
      AddDefinitionError(ss.str());
      return WsfParseTypeName();
   }

   WsfParseTypeName type;
   if (aTypeRule.mRuleType == WsfGrammar::M::cRT_EXPRESSION)
   {
      for (size_t i = 0; i < aTypeRule.WordCount(); ++i)
      {
         const WsfGrammar::M::Expr& r = aTypeRule.GetWord(i);
         if (r.mRuleType != WsfGrammar::M::cRT_LITERAL)
         {
            std::stringstream ss;
            ss << "Expected literal in 'type' while processing " << GetFullRuleName(aParentRule) << "\n";
            AddDefinitionError(ss.str());
            return type;
         }
         else
         {
            if (r.mText.size() && r.mText[0] == '$')
            {
               std::stringstream ss(r.mText.substr(1));
               ss >> type.mOrdinal;
               if (i + 1 != aTypeRule.WordCount())
               {
                  std::stringstream ss;
                  ss << "Error: Reference '$N' must be the last section of a type\n";
                  AddDefinitionError(ss.str());
               }
            }
            type.mPath.push_back(r.mText);
         }
      }
      if (aTypeRule.mType == "subtype")
      {
         type.mNestedLookup = true;
      }
   }
   return type;
}

WsfParseActionPart* WsfParseDefinitions::ProcessActionSub(const WsfGrammar::M::Expr& aRule)
{
   if (aRule.mRuleType == WsfGrammar::M::cRT_LITERAL)
   {
      return new WsfParseActionLiteral(aRule.mText);
   }
   else if (aRule.mRuleType == WsfGrammar::M::cRT_ACTION_CALL)
   {
      if (aRule.mText == "=")
      {
         if (aRule.WordCount() == 2)
         {
            WsfParseActionAssign* ass = new WsfParseActionAssign;
            ass->mLHS_Name            = aRule.GetWord(0).mText;
            ass->mRHS                 = ProcessActionSub(aRule.GetWord(1));
            assert(ass->mRHS);
            return ass;
         }
         else
         {
            assert(false);
         }
      }
      else if (aRule.mText == "copy")
      {
         if (aRule.WordCount() == 2)
         {
            WsfParseActionCopy* cpy = new WsfParseActionCopy;
            cpy->mDstName           = aRule.GetWord(0).mText;
            cpy->mSrcName           = aRule.GetWord(1).mText;
            return cpy;
         }
         else
         {
            assert(false);
         }
      }
      else if (aRule.mText == "negate")
      {
         if (aRule.WordCount() == 1)
         {
            WsfParseActionNegate* neg = new WsfParseActionNegate;
            neg->mSubPart             = ProcessActionSub(aRule.GetWord(0));
            return neg;
         }
         return nullptr;
      }
      else if (aRule.mText == "pushBack")
      {
         if (aRule.WordCount() >= 1)
         {
            WsfParseActionListOp* lo = new WsfParseActionListOp;
            lo->mOperation           = WsfParseActionListOp::cPUSH_BACK;
            lo->mAttributeName       = aRule.GetWord(0).mText;
            if (aRule.WordCount() == 2)
            {
               lo->mPushedValuePtr = ProcessActionSub(aRule.GetWord(1));
            }
            return lo;
         }
      }
      else if (aRule.mText == "new" || aRule.mText == "mapSet")
      {
         if (aRule.WordCount() == 2)
         {
            WsfParseActionObjectMapOp* mo = new WsfParseActionObjectMapOp;
            mo->mOperation = aRule.mText == "new" ? WsfParseActionObjectMapOp::cNEW : WsfParseActionObjectMapOp::cMAP_SET;
            mo->mAttributeName = aRule.GetWord(0).mText;
            mo->mObjectName    = ProcessActionSub(aRule.GetWord(1));
            return mo;
         }
      }
      else if (aRule.mText == "get" || aRule.mText == "delete")
      {
         if (aRule.WordCount() == 2)
         {
            WsfParseActionObjectMapOp* mo = new WsfParseActionObjectMapOp;
            if (aRule.mText == "get")
            {
               mo->mOperation = WsfParseActionObjectMapOp::cGET;
            }
            else
            {
               mo->mOperation = WsfParseActionObjectMapOp::cDELETE;
            }
            mo->mAttributeName = aRule.GetWord(0).mText;
            mo->mObjectName    = ProcessActionSub(aRule.GetWord(1));
            return mo;
         }
      }
      else if (aRule.mText == "apply")
      {
         if (aRule.WordCount() == 1)
         {
            WsfParseActionObjectMapOp* mo = new WsfParseActionObjectMapOp;
            mo->mOperation                = WsfParseActionObjectMapOp::cAPPLY;
            // TODO: Currently assume apply($$) -> index=-1
            mo->mApplyOrd = -1;
            return mo;
         }
      }
      else if (aRule.mText == "skip")
      {
         if (aRule.WordCount() == 0)
         {
            return new WsfParseActionSkip;
         }
      }
      else if (aRule.mText == "push")
      {
         if (aRule.WordCount() == 1)
         {
            WsfParseActionPush* push = new WsfParseActionPush(aRule.GetWord(0).mText);
            return push;
         }
      }
      else if (aRule.mText == "print")
      {
         if (aRule.WordCount() == 1)
         {
            WsfParseActionPrint* prnt = new WsfParseActionPrint(aRule.GetWord(0).mText);
            return prnt;
         }
      }
      else
      {
         // Any unmatched name is determined to be a user-defined function
         WsfParseActionCall* callPtr = new WsfParseActionCall;
         callPtr->mFunctionName      = aRule.mText;
         for (size_t i = 0; i < aRule.WordCount(); ++i)
         {
            callPtr->mArgs.push_back(ProcessActionSub(aRule.GetWord(i)));
         }
         return callPtr;
      }
   }
   return nullptr;
}

WsfParseAction* WsfParseDefinitions::ProcessAction(const WsfGrammar::M::Expr& aRule)
{
   assert(aRule.mRuleType == WsfGrammar::M::cRT_ACTION);
   WsfParseAction* action = new WsfParseAction;
   for (size_t i = 0; i < aRule.WordCount(); ++i)
   {
      const WsfGrammar::M::Expr& stmt          = aRule.GetWord(i);
      WsfParseActionPart*        actionPartPtr = ProcessActionSub(stmt);
      if (actionPartPtr)
      {
         action->AddPart(actionPartPtr);
      }
      else
      {
         std::stringstream ss;
         ss << "Failed to process action: " << stmt.mText;
         AddDefinitionError(ss.str());
      }
   }
   return action;
}

bool WsfParseDefinitions::ProcessRule(const WsfGrammar::M::Expr& aRule,
                                      WsfParseNamedRule&         aContextRule,
                                      WsfParseRule*              aParentRulePtr,
                                      WsfParseRule*&             aRVal)
{
   switch (aRule.mRuleType)
   {
   case WsfGrammar::M::cRT_ROOT:
   {
      for (size_t i = 0; i < aRule.WordCount(); ++i)
      {
         WsfParseRule* r = nullptr;
         if (ProcessRule(aRule.GetWord(i), aContextRule, nullptr, r))
         {
            // todo?
         }
      }
   }
      return false;
      break;
   case WsfGrammar::M::cRT_SEQUENCE:
   {
      WsfParseSequence* seq = new WsfParseSequence(mRuleMem);
      for (size_t i = 0; i < aRule.WordCount(); ++i)
      {
         WsfParseRule* seqPart = nullptr;
         if (aRule.GetWord(i).mRuleType == WsfGrammar::M::cRT_ACTION)
         {
            WsfParseAction* actionPtr = ProcessAction(aRule.GetWord(i));
            seq->SetAction(seq->Sequence().size(), actionPtr);
         }
         else if (ProcessRule(aRule.GetWord(i), aContextRule, seq, seqPart) && seqPart)
         {
            seq->AddRule(seqPart);
         }
      }
      seq->mParentRulePtr = aParentRulePtr;
      aRVal               = seq;
      return true;
   }
   case WsfGrammar::M::cRT_ALTERNATE:
   {
      WsfParseAlternate* alt = new WsfParseAlternate(mRuleMem);
      aRVal                  = alt;
      for (size_t i = 0; i < aRule.WordCount(); ++i)
      {
         WsfParseRule* altPart;
         if (ProcessRule(aRule.GetWord(i), aContextRule, alt, altPart) && altPart)
         {
            if (altPart->Type() == WsfParseRule::cSEQUENCE)
            {
               alt->AddAlternate(dynamic_cast<WsfParseSequence*>(altPart));
            }
            else
            {
               // should not happen:
               assert(false);
               // delete altPart;
            }
         }
      }
      // Reduce choice of 1 alternative to a sequence.
      if (alt->Alternates().size() == 1)
      {
         aRVal = alt->Alternates()[0];
         alt->ChangeAlternates().clear();
         // delete alt;
      }
      aRVal->mParentRulePtr = aParentRulePtr;
      return true;
   }
   case WsfGrammar::M::cRT_LITERAL:
   {
      WsfParseLiteral* lit = new WsfParseLiteral(mRuleMem);
      aRVal                = lit;
      lit->mText           = aRule.mText;
      lit->mParentRulePtr  = aParentRulePtr;
      return true;
   }
   case WsfGrammar::M::cRT_ACTION:
      assert(false); // should not be possible to get here
      return false;
      break;
   case WsfGrammar::M::cRT_EXPRESSION:
   {
      std::string exprType = aRule.mType;
      // ignore aux- expressions here
      if (exprType.size() >= 4 && exprType.substr(0, 4) == "aux-")
      {
         return true;
      }
      if (exprType == "grammar-version")
      {
         mVersionString = aRule.GetWord(0).mText;
      }
      else if (exprType == "recurrence")
      {
         WsfParseRule* sub(nullptr);
         if (ProcessRule(aRule.GetWord(1), aContextRule, aParentRulePtr, sub) && sub)
         {
            WsfParseRecurrence* rec = new WsfParseRecurrence(mRuleMem);
            rec->SetRecurringRule(sub);
            rec->mParentRulePtr = aParentRulePtr;
            aRVal               = rec;

            switch (aRule.GetWord(0).mText[0])
            {
            case '*':
               rec->mCount = WsfParseRecurrence::cZERO_TO_MANY;
               break;
            case '+':
               rec->mCount = WsfParseRecurrence::cONE_TO_MANY;
               break;
            case '?':
               rec->mCount = WsfParseRecurrence::cZERO_OR_ONE;
               break;
            }
            return true;
         }
      }
      else if (exprType == "rule_ref")
      {
         WsfParseRuleReference* rref = new WsfParseRuleReference(mRuleMem);
         rref->mRuleName             = aRule.GetWord(0).mText;
         rref->mParentRulePtr        = aParentRulePtr;
         aRVal                       = rref;
         return true;
      }
      else if (exprType == "typeref")
      {
         if (aRule.WordCount() > 0)
         {
            WsfParseStringRule* r = new WsfParseStringRule(mRuleMem);
            r->mFlags             = WsfParseNode::cLAZY_TYPE_REFERENCE_NODE;
            r->mNodeType          = aRule.GetWord(0).mText;
            r->mParentRulePtr     = aParentRulePtr;
            aRVal                 = r;
            return true;
         }
         return false;
      }
      else if (exprType == "name")
      {
         if (aRule.WordCount() > 0)
         {
            WsfParseStringRule* r = new WsfParseStringRule(mRuleMem);
            r->mFlags             = WsfParseNode::cNAMED_NODE;
            r->mNodeType          = aRule.GetWord(0).mText;
            r->mParentRulePtr     = aParentRulePtr;
            aRVal                 = r;
            return true;
         }
         return false;
      }
      else if (exprType == "typename")
      {
         if (aRule.WordCount() > 0)
         {
            WsfParseStringRule* r = new WsfParseStringRule(mRuleMem);
            r->mFlags             = WsfParseNode::cLAZY_TYPENAME_NODE;
            r->mNodeType          = aRule.GetWord(0).mText;
            r->mParentRulePtr     = aParentRulePtr;
            aRVal                 = r;
            return true;
         }
         return false;
      }
      else if (exprType == "output-file-reference" || exprType == "file-reference")
      {
         WsfParseQuotedStringRule* fileRef = new WsfParseQuotedStringRule(mRuleMem, true);
         fileRef->mFileReference           = true;
         fileRef->mFileSearchPaths         = false;
         if (exprType == "file-reference")
         {
            fileRef->mFileSearchPaths = true;
         }

         if (aRule.WordCount() == 1)
         {
            fileRef->mFileType = aRule.GetWord(0).mText;
         }
         fileRef->mParentRulePtr = aParentRulePtr;
         aRVal                   = fileRef;
         return true;
      }
      else if (exprType == "nocase")
      {
         WsfParseRule* r;
         if (ProcessRule(aRule.GetWord(0), aContextRule, aParentRulePtr, r))
         {
            SetNoCase(r);
            aRVal             = r;
            r->mParentRulePtr = aParentRulePtr;
            return true;
         }
         else
         {
            std::stringstream ss;
            ss << "'nocase' requires a single argument.\n";
            AddDefinitionError(ss.str());
         }
      }
      else if (exprType == "struct")
      {
         std::string ruleName = aRule.GetWord(0).mText;
         if (aContextRule.Type() != WsfParseRule::cSTRUCT && aContextRule.mContextParent != nullptr)
         {
            std::stringstream ss;
            ss << "Can only nest structs in other structs, while adding struct " << ruleName << " to "
               << GetFullRuleName(aContextRule);
            AddDefinitionError(ss.str());
         }
         WsfParseNamedRule* r = aContextRule.FindImmediate(ruleName);
         if (!r)
         {
            r                 = new WsfParseStruct(mRuleMem, ruleName);
            r->mContextParent = &aContextRule;
            aContextRule.GetNested()->Add(r);
         }
         else
         {
            if (dynamic_cast<WsfParseStruct*>(r) == nullptr)
            {
               std::stringstream ss;
               ss << "Trying to define struct, but a non-struct already exists with that name: " + GetFullRuleName(*r)
                  << "\n";
               AddDefinitionError(ss.str());
               return false;
            }
         }
         ProcessRuleParams(aRule, aContextRule, r, 1);
      }
      else if (exprType == "value")
      {
         if (aRule.WordCount() >= 2)
         {
            std::string        ruleName = aRule.GetWord(0).mText;
            WsfParseNamedRule* r        = aContextRule.FindImmediate(ruleName);
            if (!r)
            {
               WsfParseValue* valRule  = new WsfParseValue(mRuleMem, ruleName);
               valRule->mName          = ruleName;
               valRule->mContextParent = &aContextRule;
               aContextRule.GetNested()->Add(valRule);
               r = valRule;
            }
            else
            {
               if (dynamic_cast<WsfParseValue*>(r) == nullptr)
               {
                  std::stringstream ss;
                  ss << "Trying to define value, but a non-value already exists with that name: " + GetFullRuleName(*r)
                     << "\n";
                  AddDefinitionError(ss.str());
               }
               return false;
            }
            ProcessRuleParams(aRule, aContextRule, r, 1);
            return true;
         }
      }
      else if (exprType == "enumeration")
      {
         WsfParseEnumeration*      enumPtr = new WsfParseEnumeration(mRuleMem, aRule.GetWord(0).mText);
         WsfParseEnumerationValues vals;
         if (aRule.WordCount() >= 2)
         {
            const WsfGrammar::M::Expr& w = aRule.GetWord(1);
            if ((w.mRuleType == WsfGrammar::M::cRT_EXPRESSION) && w.mType == "values")
            {
               for (size_t j = 0; j < w.WordCount(); j += 2)
               {
                  WsfParseEnumerationValues::NameValue ev;
                  ev.first = w.GetWord(j).mText;
                  std::stringstream ss(w.GetWord(j + 1).mText);
                  if (ss >> ev.second)
                  {
                     vals.mNameValues.push_back(ev);
                  }
               }
            }
         }
         ProcessRuleParams(aRule, aContextRule, enumPtr, 2);
         enumPtr->mValues = vals;
         aContextRule.GetNested()->Add(enumPtr);
         return false;
      }
      else if (exprType == "rule")
      {
         std::string        ruleName = aRule.GetWord(0).mText;
         WsfParseNamedRule* r        = aContextRule.FindImmediate(ruleName);
         if (!r)
         {
            r                 = new WsfParseNamedRule(mRuleMem);
            r->mName          = ruleName;
            r->mContextParent = &aContextRule;
            aContextRule.GetNested()->Add(r);
         }
         ProcessRuleParams(aRule, aContextRule, r, 1);
         return true;
      }
      else if (exprType == "string-except")
      {
         WsfParseStringWithExceptionsRule* se = new WsfParseStringWithExceptionsRule(mRuleMem);
         for (size_t i = 0; i < aRule.WordCount(); ++i)
         {
            se->mExceptions.insert(aRule.GetWord(i).mText);
         }
         se->mParentRulePtr = aParentRulePtr;
         aRVal              = se;
         return true;
      }
      else if (exprType == "load" || exprType == "new" || exprType == "new_replace" || exprType == "delete")
      {
         if (aRule.WordCount() > 0)
         {
            WsfParseTypeName loadType, saveType, backup;
            loadType = ProcessType(aContextRule, aRule.GetWord(0));
            if (exprType == "new" || exprType == "new_replace")
            {
               saveType = ProcessType(aContextRule, aRule.GetWord(1));
               std::swap(loadType, saveType);
            }

            for (size_t i = 1; i < aRule.WordCount(); ++i)
            {
               if (aRule.GetWord(i).mLabel == "backup")
               {
                  backup = ProcessType(aContextRule, aRule.GetWord(i));
               }
            }
            WsfParseTypeInfoData::Operation operation = WsfParseTypeInfoData::cLOAD_TYPE;
            if (exprType == "new" || exprType == "new_replace")
            {
               operation = WsfParseTypeInfoData::cCREATE_TYPE;
            }
            if (exprType == "delete")
            {
               operation = WsfParseTypeInfoData::cDELETE_TYPE;
            }
            WsfParseTypeLoadRule* rulePtr = new WsfParseTypeLoadRule(mRuleMem, operation, exprType == "new_replace");
            rulePtr->mLoadType            = loadType;
            if (!backup.mPath.empty())
            {
               rulePtr->mBackupLoadType = backup.mPath;
            }
            if (!saveType.mPath.empty())
            {
               rulePtr->mSaveType = saveType;
            }
            rulePtr->mParentRulePtr = aParentRulePtr;
            aRVal                   = rulePtr;
            return true;
         }
      }
      else if (exprType == "type" || exprType == "subtype")
      {
         ProcessType(aContextRule, aRule);
         return false;
      }
      else if (exprType == "error")
      {
         if (aRule.WordCount() == 1)
         {
            WsfParsePassthrough* pass = new WsfParsePassthrough(mRuleMem);
            pass->mKind               = WsfParsePassthrough::cERROR_RULE;
            pass->mParentRulePtr      = aParentRulePtr;
            WsfParseRule* childRule   = nullptr;
            if (ProcessRule(aRule.GetWord(0), aContextRule, pass, childRule) && childRule)
            {
               pass->SetPassthroughRule(childRule);
               aRVal = pass;
               return true;
            }
         }
         return false;
      }
      else if (exprType == "symbol")
      {
         if (aRule.WordCount() == 2)
         {
            WsfParseTypeName   typeName   = ProcessType(aContextRule, aRule.GetWord(0));
            std::string        structName = aRule.GetWord(1).mText;
            WsfParseNamedRule* rule       = aContextRule.Find(structName);
            if (dynamic_cast<WsfParseStruct*>(rule))
            {
               WsfParseStruct* structPtr  = dynamic_cast<WsfParseStruct*>(rule);
               WsfParseType*   newTypePtr = new WsfParseType(typeName.mPath.back());
               newTypePtr->SetRule(structPtr);
               mBasicTypes->AddType(typeName.mPath, newTypePtr);
            }
         }
      }
      else if (exprType == "initially")
      {
         if (aContextRule.Type() != WsfParseRule::cSTRUCT)
         {
            std::stringstream ss;
            ss << "Only 'struct' may have 'initially' inside " << GetFullRuleName(aContextRule) << "\n";
            AddDefinitionError(ss.str());
            return false;
         }
         WsfParseStruct* structPtr = (WsfParseStruct*)&aContextRule;
         if (aRule.WordCount() == 1)
         {
            WsfParseRule* result = nullptr;
            if (ProcessRule(aRule.GetWord(0), aContextRule, nullptr, result))
            {
               WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(result);
               if (seq && seq->Sequence().size() == 1 && seq->GetActionBefore(0))
               {
                  structPtr->mInitialActions.push_back(seq->TakeActionBefore(0));
               }
               else
               {
                  std::stringstream ss;
                  ss << "Expected a sequence with a single action inside " << GetFullRuleName(aContextRule) << "\n";
                  AddDefinitionError(ss.str());
                  return false;
               }
            }
         }
      }
      else if (exprType == "var")
      {
         if (aContextRule.Type() != WsfParseRule::cSTRUCT)
         {
            std::stringstream ss;
            ss << "Only 'struct' may have 'var' or 'basic', inside " << GetFullRuleName(aContextRule) << "\n";
            AddDefinitionError(ss.str());
            return false;
         }
         WsfParseStruct* structPtr = (WsfParseStruct*)&aContextRule;
         if (aRule.WordCount() >= 2)
         {
            WsfParseAttribute attr;
            attr.mType = aRule.GetWord(0).mText;
            if (attr.mType[attr.mType.size() - 1] == '&')
            {
               attr.mType.resize(attr.mType.size() - 1);
               attr.mIsPointer = true;
            }
            attr.mName = aRule.GetWord(1).mText;
            if (structPtr->FindAttribute(attr.mName) != nullptr)
            {
               std::stringstream ss;
               ss << "Attribute '" << attr.mName << "' is specified more than once, inside "
                  << GetFullRuleName(aContextRule);
               AddDefinitionError(ss.str());
               return false;
            }

            for (size_t i = 2; i < aRule.WordCount(); ++i)
            {
               const WsfGrammar::M::Expr& e = aRule.GetWord(i);
               if (e.mLabel == "default")
               {
                  attr.mDefault         = e.mText;
                  attr.mHasDefaultValue = true;
               }
               else if (e.mLabel == "fixed")
               {
                  attr.mFlags |= WsfParseAttribute::cIS_FIXED;
               }
               else
               {
                  std::stringstream ss;
                  ss << "Unknown attribute option '" << e.mLabel << "', inside " << GetFullRuleName(aContextRule);
                  AddDefinitionError(ss.str());
               }
            }
            structPtr->mAttributes.push_back(attr);
         }
      }
      else if (exprType == "delimited")
      {
         WsfParseDelimited*         delimited = new WsfParseDelimited(mRuleMem);
         WsfParseDelimited::Section section;
         WsfParseRule*              r;
         size_t                     i  = 0;
         bool                       ok = true;
         if (ProcessRule(aRule.GetWord(i), aContextRule, delimited, r) && r)
         {
            if (r->Type() != WsfParseRule::cLITERAL)
            {
               section.mRulePtr = r;
               delimited->mSections.push_back(section);
               section.mRulePtr = nullptr;
               i                = 1;
            }
         }
         bool expectLiteral = true;
         for (; i < aRule.WordCount(); ++i)
         {
            WsfParseRule* r;
            if (ProcessRule(aRule.GetWord(i), aContextRule, delimited, r) && r)
            {
               bool isLiteral = (r->Type() == WsfParseRule::cLITERAL);
               if (isLiteral != expectLiteral)
               {
                  std::stringstream ss;
                  ss << "'delimited' rule does not alternate between literal strings and rules, inside "
                     << GetFullRuleName(aContextRule) << ".\n";
                  AddDefinitionError(ss.str());
                  ok = false;
                  break;
               }
               if (isLiteral)
               {
                  section.mPreDelimiter = (dynamic_cast<WsfParseLiteral*>(r))->mText;
                  // delete r;
               }
               else
               {
                  section.mRulePtr = r;
                  delimited->mSections.push_back(section);
                  section.mRulePtr = nullptr;
                  section.mPreDelimiter.clear();
               }
            }
            expectLiteral = !expectLiteral;
         }
         if (!ok)
         {
            // delete delimited;
            return false;
         }
         else
         {
            if (!expectLiteral && !section.mPreDelimiter.empty())
            {
               delimited->mSections.push_back(section);
            }
            delimited->mParentRulePtr = aParentRulePtr;
            aRVal                     = delimited;
            return true;
         }
      }
      else if (exprType == "function")
      {
         auto f = ut::make_unique<WsfParseActionFunction>();
         f->mSignatureStrings.resize(1);
         for (size_t i = 1; i < aRule.WordCount(); ++i)
         {
            const WsfGrammar::M::Expr& e = aRule.GetWord(i);
            if (e.mLabel == "return")
            {
               f->mSignatureStrings[0] = e.mText;
            }
            else if (e.mRuleType == WsfGrammar::M::cRT_EXPRESSION && e.mType == "var" && e.WordCount() == 2)
            {
               f->mSignatureStrings.push_back(e.GetWord(0).mText);
               f->mArgumentNames.push_back(e.GetWord(1).mText);
            }
         }
         mFunctions[aRule.GetWord(0).mText] = std::move(f);
      }
      else if (exprType == "script-var")
      {
         WsfParseStruct* structPtr = aContextRule.GetStruct();
         if (!structPtr)
         {
            std::stringstream ss;
            ss << "script-var must be in a struct, while processing " << GetFullRuleName(aContextRule) << '\n';
            AddDefinitionError(ss.str());
         }
         else
         {
            if (aRule.WordCount() >= 2)
            {
               WsfParseStruct::ScriptVariable v;
               v.mIsThisType   = false;
               v.mTypeName     = aRule.GetWord(0).mText;
               v.mVariableName = aRule.GetWord(1).mText;
               for (size_t i = 2; i < aRule.WordCount(); ++i)
               {
                  if (aRule.GetWord(i).mLabel == "this")
                  {
                     v.mIsThisType = true;
                  }
               }
               structPtr->mScriptVariables.push_back(v);
            }
         }
      }
      else
      {
         std::stringstream ss;
         ss << "Unknown expression: '(" << exprType << "' while processing " << GetFullRuleName(aContextRule) << '\n';
         AddDefinitionError(ss.str());
      }
   }
   break;
   default:
      assert(false);
   }
   return false;
}

void WsfParseDefinitions::ResolveReference(WsfParseNamedRule& aParentRule,
                                           const std::string& aReferenceName,
                                           WsfParseRule*&     aReferencedRulePtr)
{
   std::string prefix, suffix;
   size_t      dot = aReferenceName.find('.');
   if (dot != std::string::npos)
   {
      prefix = aReferenceName.substr(0, dot);
      suffix = aReferenceName.substr(dot + 1);
   }
   else
   {
      prefix = aReferenceName;
   }

   aReferencedRulePtr = aParentRule.Find(prefix);

   if (!aReferencedRulePtr)
   {
      std::stringstream ss;
      ss << "Reference to non-existent rule \"" << prefix << "\", while processing " << GetFullRuleName(aParentRule)
         << ".\n";
      AddDefinitionError(ss.str());
   }
   else
   {
      if (!suffix.empty())
      {
         WsfParseNamedRule* namedPtr = dynamic_cast<WsfParseNamedRule*>(aReferencedRulePtr);
         if (namedPtr)
         {
            ResolveReference(*namedPtr, suffix, aReferencedRulePtr);
         }
         else
         {
            aReferencedRulePtr = nullptr;
            std::stringstream ss;
            ss << "Reference to rule \"" << aReferenceName << "\" is not valid.\n";
            AddDefinitionError(ss.str());
         }
      }
   }
}

void WsfParseDefinitions::ResolveReference(WsfParseNamedRule&     aParentRule,
                                           WsfParseRuleReference& aReference,
                                           WsfParseRule*&         aReplacementRulePtr)
{
   WsfParseRule* refPtr = nullptr;
   ResolveReference(aParentRule, aReference.mRuleName, refPtr);
   if (refPtr)
   {
      aReference.SetReferencedRule(refPtr);
   }
}

void WsfParseDefinitions::ResolveRule(WsfParseNamedRule& aParentRule, WsfParseRule*& aRulePtr, bool aAllowReplace)
{
   switch (aRulePtr->Type())
   {
   case WsfParseRule::cRULE_REFERENCE:
   {
      bool                   foundReference = false;
      WsfParseRuleReference* ref            = dynamic_cast<WsfParseRuleReference*>(aRulePtr);
      // Indicates the rule should not affect the proxy.
      // Transform   <:RuleName>                into:
      //             [skip()] <RuleName>
      if (!ref->mRuleName.empty() && ref->mRuleName[0] == ':' && ref->mParentRulePtr &&
          ref->mParentRulePtr->Type() == WsfParseRule::cSEQUENCE)
      {
         ref->mRuleName        = ref->mRuleName.substr(1);
         WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(ref->mParentRulePtr);
         size_t            idx = seq->FindChildIndex(ref);

         WsfParseAction* curAction = seq->GetActionBefore(idx);
         if (curAction == nullptr)
         {
            curAction = new WsfParseAction;
            seq->SetAction(idx, curAction);
         }
         curAction->mSubActions.push_back(new WsfParseActionSkip);
      }
      // If a struct's attribute is referenced, expand like this:
      // (var Bool reportsSide)
      // (var InternalLink links)
      // When var is a basic type:
      // { ...   reports_side <=reportsSide> [existingAction()]
      //-> ...   reports_side <Bool> [reportsSide=$1;existingAction()]
      // When var is a struct:
      // { ...   internal_link [existingAction()] <=links>
      //-> ...   internal_link [existingAction();push(links)] <InternalLink>
      if (!ref->mRuleName.empty() && ref->mRuleName[0] == '$')
      {
         WsfParseNamedRule* curTypePtr = aParentRule.GetInputType();
         if (curTypePtr && curTypePtr->Type() == WsfParseRule::cSTRUCT)
         {
            WsfParseStruct*    structPtr = dynamic_cast<WsfParseStruct*>(curTypePtr);
            std::string        attrName  = ref->mRuleName.substr(1);
            WsfParseAttribute* attr      = structPtr->FindAttribute(attrName);
            if (attr && attr->mTypePtr && ref->mParentRulePtr && ref->mParentRulePtr->Type() == WsfParseRule::cSEQUENCE)
            {
               WsfParseRule*      typeRulePtr  = attr->mTypePtr->GetTypeRule();
               WsfParseNamedRule* namedRulePtr = dynamic_cast<WsfParseNamedRule*>(typeRulePtr);
               ref->mRuleName                  = namedRulePtr->GetFullName();
               ref->SetReferencedRule(namedRulePtr);
               WsfParseSequence* seq       = dynamic_cast<WsfParseSequence*>(ref->mParentRulePtr);
               size_t            idx       = seq->FindChildIndex(ref);
               WsfParseAction*   actionPtr = new WsfParseAction;
               if (namedRulePtr->Type() == WsfParseRule::cSTRUCT)
               {
                  // in the case of struct, we need to [push()]
                  WsfParseActionPush* push = new WsfParseActionPush(attr->mName);
                  actionPtr->AddPart(push);
                  WsfParseAction* prevAction = seq->GetActionBefore(idx);
                  if (prevAction)
                  {
                     prevAction->Merge(*actionPtr);
                     delete actionPtr;
                  }
                  else
                  {
                     seq->SetAction(idx, actionPtr);
                  }
               }
               else
               {
                  WsfParseActionAssign* ass        = new WsfParseActionAssign;
                  ass->mLHS_Name                   = attr->mName;
                  WsfParseActionNodeReference* rhs = new WsfParseActionNodeReference;
                  rhs->mIndex                      = idx;
                  rhs->mValueTypePtr               = attr->mTypePtr;
                  ass->mRHS                        = rhs;
                  actionPtr->AddPart(ass);

                  // If previous actions were specified, add this one before those
                  WsfParseAction* prevAction = seq->GetActionBefore(idx + 1);
                  if (prevAction)
                  {
                     actionPtr->Merge(*prevAction);
                  }
                  seq->SetAction(idx + 1, actionPtr);
               }
               foundReference = true;
            }
         }
         if (!foundReference)
         {
            std::stringstream ss;
            ss << "Reference to attribute <" << ref->mRuleName << "> could not be resolved, inside "
               << GetFullRuleName(aParentRule);
            AddDefinitionError(ss.str());
         }
      }
      if (!foundReference)
      {
         WsfParseRule* replacement = nullptr;
         ResolveReference(aParentRule, *ref, replacement);
         if (replacement)
         {
            aRulePtr = replacement;
         }
      }
   }
   break;
   case WsfParseRule::cPASSTHROUGH:
   {
      if (aRulePtr->GetSubordinateRule())
      {
         WsfParseRule* sub = aRulePtr->GetSubordinateRule();
         ResolveRule(aParentRule, sub, true);
         aRulePtr->SetSubordinateRule(sub);
      }
   }
   break;
   case WsfParseRule::cRECURRENCE:
   {
      WsfParseRecurrence* rec = dynamic_cast<WsfParseRecurrence*>(aRulePtr);
      WsfParseRule*       sub = rec->GetSubordinateRule();
      ResolveRule(aParentRule, sub, true);
      rec->SetSubordinateRule(sub);
   }
   break;
   case WsfParseRule::cSEQUENCE:
   {
      WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(aRulePtr);
      for (size_t i = 0; i < seq->Sequence().size(); ++i)
      {
         ResolveRule(aParentRule, seq->ChangeSequence()[i], true);
      }
      if (aAllowReplace && !seq->HasActions() && seq->Sequence().size() == 1)
      {
         aRulePtr                 = seq->Sequence()[0];
         aRulePtr->mParentRulePtr = &aParentRule;
         seq->ChangeSequence().clear();
      }
   }
   break;
   case WsfParseRule::cALTERNATE:
   {
      WsfParseAlternate* alt = dynamic_cast<WsfParseAlternate*>(aRulePtr);
      for (size_t i = 0; i < alt->Alternates().size(); ++i)
      {
         WsfParseRule*& subPtr = alt->Alternates()[i];
         ResolveRule(aParentRule, subPtr, true);
         assert(subPtr == alt->Alternates()[i]);
      }
   }
   break;
   case WsfParseRule::cDELIMITED:
   {
      WsfParseDelimited* delim = dynamic_cast<WsfParseDelimited*>(aRulePtr);
      for (size_t i = 0; i < delim->mSections.size(); ++i)
      {
         if (delim->mSections[i].mRulePtr)
         {
            ResolveRule(aParentRule, delim->mSections[i].mRulePtr, true);
         }
      }
   }
   break;
   case WsfParseRule::cSTRUCT:
   {
      WsfParseStruct* structPtr = dynamic_cast<WsfParseStruct*>(aRulePtr);
      if (!structPtr->mBaseName.empty())
      {
         WsfParseNamedRule* basePtr = mGlobalRules->Find(structPtr->mBaseName);
         ResolveBase(structPtr, basePtr);
      }
      for (size_t i = 0; i < structPtr->mAttributes.size(); ++i)
      {
         WsfParseAttribute& attr = structPtr->mAttributes[i];
         if (attr.mTypePtr == nullptr)
         {
            attr.mTypePtr = GetTypeFromName(*structPtr, attr.mType);
            if (!attr.mTypePtr)
            {
               std::stringstream ss;
               ss << "Attribute type '" << attr.mType << "' does not resolve to a value or struct, inside "
                  << GetFullRuleName(aParentRule);
               AddDefinitionError(ss.str());
            }
         }
      }
   }
   // Fall through:
   case WsfParseRule::cNAMED_RULE:
   case WsfParseRule::cVALUE:
   {
      WsfParseNamedRule* named = dynamic_cast<WsfParseNamedRule*>(aRulePtr);
      if (!named->InitializeInputsOutputs())
      {
         std::stringstream ss;
         ss << "Can't initialize input or output type inside " << GetFullRuleName(*named);
         AddDefinitionError(ss.str());
      }
      if (named->HasNestedRules())
      {
         WsfParseRuleDictionary* dict = named->GetNested();
         for (std::map<std::string, WsfParseNamedRule*>::iterator i = dict->mNameToRule.begin();
              i != dict->mNameToRule.end();
              ++i)
         {
            WsfParseRule* subPtr = i->second;
            ResolveRule(*named, subPtr, false);
            assert(subPtr == i->second);
         }
      }
      if (named->GetDefinition())
      {
         ResolveRule(*named, named->GetDefinition(), true);
         // named->SetDefinition(def);
      }
   }
   break;
   default:
      break;
   }
}

void WsfParseDefinitions::ResolveBase(WsfParseStruct* aStructPtr, WsfParseNamedRule* aParentRulePtr)
{
   if (aParentRulePtr && aParentRulePtr->Type() == WsfParseRule::cSTRUCT &&
       aParentRulePtr != aStructPtr) // if the parent is a struct
   {
      WsfParseStruct* structPtr = dynamic_cast<WsfParseStruct*>(aParentRulePtr);
      if (!structPtr->mBaseName.empty())
      {
         WsfParseNamedRule* basePtr = mGlobalRules->Find(structPtr->mBaseName);
         ResolveBase(structPtr, basePtr); // resolve it with its parent
      }
      aStructPtr->SetBase(dynamic_cast<WsfParseStruct*>(aParentRulePtr)); // set the base after the parentage is done
   }
   else
   {
      std::stringstream ss;
      ss << "Could not find base_type: " << aStructPtr->mBaseName << " while processing " << GetFullRuleName(*aStructPtr);
      AddDefinitionError(ss.str());
   }
}

void WsfParseDefinitions::InitializeRule(WsfParseRule* aRulePtr, int aSequence)
{
   if (aRulePtr->Type() == WsfParseRule::cRULE_REFERENCE)
   {
      aRulePtr->Initialize(aSequence);
      return;
   }
   WsfParseRuleDictionary* nestedPtr = aRulePtr->GetNestedRules();
   if (nestedPtr)
   {
      for (std::map<std::string, WsfParseNamedRule*>::iterator i = nestedPtr->mNameToRule.begin();
           i != nestedPtr->mNameToRule.end();
           ++i)
      {
         InitializeRule(i->second, aSequence);
      }
   }
   std::vector<WsfParseRule*> seq = aRulePtr->GetSequence();
   std::vector<WsfParseRule*> alt = aRulePtr->GetAlternates();
   for (size_t i = 0; i < seq.size(); ++i)
   {
      InitializeRule(seq[i], aSequence);
   }
   for (size_t i = 0; i < alt.size(); ++i)
   {
      assert(aRulePtr->mParentRulePtr != nullptr);
      InitializeRule(alt[i], aSequence);
   }
   aRulePtr->Initialize(aSequence);
}

std::string WsfParseDefinitions::GetFullRuleName(WsfParseNamedRule& aRule)
{
   std::string        name   = aRule.mName;
   WsfParseNamedRule* parent = aRule.mContextParent;
   while (parent)
   {
      if (!parent->mName.empty())
      {
         name = parent->mName + "." + name;
      }
      parent = parent->mContextParent;
   }
   return name;
}

bool WsfParseDefinitions::Initialize()
{
   WsfParseRule* rval;
   ProcessRule(mInputGrammar, *mGlobalRules, nullptr, rval);
   WsfParseRule* globalRulePtr = mGlobalRules;

   // Look for a root command
   mRootRulePtr = mGlobalRules->Find("root-command");
   if (!mRootRulePtr)
   {
      AddDefinitionError("No root-command -- cannot use grammar");
      return false;
   }

   // todo: nested type assignments???
   WsfParseNamedRule* pathVarCommand = mRootRulePtr->Find("path-variable-command");
   if (pathVarCommand)
   {
      WsfParseAlternate* altPtr = dynamic_cast<WsfParseAlternate*>(pathVarCommand->GetDefinition());
      for (size_t i = 0; i < altPtr->Alternates().size(); ++i)
      {
         WsfParseSequence* seq = dynamic_cast<WsfParseSequence*>(altPtr->Alternates()[i]);
         if (seq)
         {
            seq->mSpecialCommand = true;
         }
      }
   }

   ResolveRule(*mGlobalRules, globalRulePtr, false);
   InitializeRule(mGlobalRules, 0);
   InitializeRule(mGlobalRules, 1);
   InitializeFunctions();

   // Look for a root struct
   mRootStructPtr = mGlobalRules->FindStruct("root");
   if (!mRootStructPtr)
   {
      AddDefinitionError("No root struct -- cannot use grammar");
   }
   CheckActions(mGlobalRules, mRootStructPtr);

   return mDefinitionErrors.empty();
}

WsfParseActionAddress WsfParseDefinitions::ResolveActionAddress(WsfParseValueType*& aContainer, const std::string& aAddrText)
{
   std::string pathStr  = aAddrText;
   bool        relative = true;
   if (aAddrText == "this")
   {
      return WsfParseActionAddress();
   }
   if (aAddrText.substr(0, 2) == "..")
   {
      relative = false;
      pathStr  = aAddrText.substr(2);
   }
   WsfParseTypePath path = WsfParseTypePathParse(pathStr);

   WsfParseActionAddress addr = ResolveActionAddress(aContainer, path, relative);
#ifdef WSF_PARSE_DEBUG
   addr.mDEBUG_Addr = aAddrText;
#endif
   return addr;
}

WsfParseActionAddress WsfParseDefinitions::ResolveActionAddress(WsfParseValueType*&     aContainer,
                                                                const WsfParseTypePath& aPath,
                                                                bool                    aRelative /* = true*/)
{
   WsfParseActionAddress addr;

   if (!aRelative)
   {
      aContainer = mRootStructPtr->GetType();
      // aContainer = new WsfParseValueType(mRootStructPtr);
      // aContainer = GetTypeFromName("root");  //new WsfParseValueType(mRootStructPtr);
      addr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cROOT);
   }

   for (size_t i = 0; i < aPath.size(); ++i)
   {
      // Probably a previously reported error
      if (!aContainer)
      {
         assert(!mDefinitionErrors.empty());
         return addr;
      }

      const std::string& tok = aPath[i].Get();
      if (aContainer->mStructPtr)
      {
         size_t idx = aContainer->mStructPtr->FindAttributeIndex(tok);
         if (idx != ut::npos)
         {
            addr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cATTRIBUTE, idx);
            WsfParseAttribute& attr = aContainer->mStructPtr->GetAttribute(idx);
            aContainer              = attr.mTypePtr;
         }
         else
         {
            std::stringstream ss;
            ss << "Attribute " << tok << " does not exist on " << aContainer->GetTypeName() << "\n";
            AddDefinitionError(ss.str());
         }
      }
      else if (aContainer->mValuePtr)
      {
         std::stringstream ss;
         ss << "Value type " << aContainer->mValuePtr->GetFullName() << " cannot have attributes while accessing "
            << tok << " on " << aContainer->GetTypeName() << '\n';
         AddDefinitionError(ss.str());
      }
      else if (aContainer->mKind == WsfParseValueType::cLIST)
      {
         size_t idx = ut::lexical_cast<size_t>(tok);
         addr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cATTRIBUTE, idx);
         aContainer = aContainer->mTemplateType;
      }
      else if (aContainer->mKind == WsfParseValueType::cOBJECT_MAP)
      {
         if (tok[0] == '$')
         {
            size_t lookupIndex = ut::lexical_cast<size_t>(&(tok[1]));
            addr.mAttrIndices.emplace_back(WsfParseActionAddress::EntryType::cNODE, lookupIndex);
         }
         else
         {
            addr.mAttrIndices.emplace_back(MakeCharPtr(tok));
         }
         aContainer = aContainer->mTemplateType;
      }
   }

#ifdef WSF_PARSE_DEBUG
   for (size_t i = 0; i < aPath.size(); ++i)
   {
      if (i > 0)
      {
         addr.mDEBUG_Addr += '.';
      }
      addr.mDEBUG_Addr += aPath[i].Get();
   }
#endif

   return addr;
}

void WsfParseDefinitions::CheckActionPart(WsfParseRule*        aRulePtr,
                                          WsfParseValueType*&  aContainer,
                                          WsfParseActionPart*& aActionPart,
                                          size_t               aPrevNodeIndex)
{
   if (aContainer)
   {
      switch (aActionPart->mActionPartType)
      {
      case WsfParseActionPart::cLITERAL:
      {
         WsfParseActionLiteral* lit = dynamic_cast<WsfParseActionLiteral*>(aActionPart);
         if (lit->mText.size() > 1)
         {
            // Replace $$ / $N with a node reference
            if (lit->mText[0] == '$' && aRulePtr)
            {
               size_t idx = ut::npos;
               if (lit->mText[1] == '$')
               {
                  idx = aPrevNodeIndex;
               }
               else
               {
                  std::stringstream ss(lit->mText.substr(1));
                  ss >> idx;
               }
               if (idx != ut::npos)
               {
                  std::vector<WsfParseRule*> seq = aRulePtr->GetSequence();
                  if (seq.size() > idx)
                  {
                     WsfParseActionNodeReference* newActionPtr = new WsfParseActionNodeReference;
                     newActionPtr->mIndex                      = idx;
                     WsfParseNamedRule* outRulePtr             = seq[idx]->GetOutputType();
                     if (outRulePtr)
                     {
                        newActionPtr->mValueTypePtr = outRulePtr->GetType();
                     }
                     delete aActionPart;
                     aActionPart = newActionPtr;
                  }
                  else
                  {
                     std::stringstream ss;
                     ss << "Error: Reference to " << lit->mText << " but only " << seq.size()
                        << " rules are in this sequence while while processing "
                        << GetFullRuleName(*aRulePtr->GetBestContext()) << ".\n";
                     AddDefinitionError(ss.str());
                     assert(false);
                  }
               }
            }
         }
      }
      break;
      case WsfParseActionPart::cASSIGN:
      {
         WsfParseActionAssign* ass     = dynamic_cast<WsfParseActionAssign*>(aActionPart);
         WsfParseValueType*    lhsType = aContainer;
         ass->mLHS_Address             = ResolveActionAddress(lhsType, ass->mLHS_Name);
         ass->mLHS_Type                = lhsType;
         WsfParseValueType* rhsType    = aContainer;
         CheckActionPart(aRulePtr, rhsType, ass->mRHS, aPrevNodeIndex);
         // TODO: if the RHS evaluates to a node reference, we should be checking that the assignment is valid
         //       If the RHS is a string, we can't really check here.
      }
      break;
      case WsfParseActionPart::cPUSH:
      {
         WsfParseActionPush* pushPtr  = dynamic_cast<WsfParseActionPush*>(aActionPart);
         WsfParseValueType*  attrType = aContainer;
         pushPtr->mAttributeAddr      = ResolveActionAddress(attrType, pushPtr->mAttributeName);
         if (attrType && !pushPtr->mAttributeAddr.IsEmpty())
         {
            aContainer = attrType;
         }
      }
      break;
      case WsfParseActionPart::cCOPY:
      {
         WsfParseActionCopy* cpy     = dynamic_cast<WsfParseActionCopy*>(aActionPart);
         WsfParseValueType*  lhsType = aContainer;
         cpy->mDstAddr               = ResolveActionAddress(lhsType, cpy->mDstName);
         WsfParseValueType* rhsType  = aContainer;
         cpy->mSrcAddr               = ResolveActionAddress(rhsType, cpy->mSrcName);
         cpy->mDstType               = lhsType;
         // CheckActionPart(aRulePtr, rhsType, cpy->mSrc, aPrevNodeIndex);
      }
      break;
      case WsfParseActionPart::cLIST_OP:
      {
         WsfParseActionListOp* op          = dynamic_cast<WsfParseActionListOp*>(aActionPart);
         WsfParseValueType*    listTypePtr = aContainer;
         op->mAttributeAddr                = ResolveActionAddress(listTypePtr, op->mAttributeName);
         if (op->mPushedValuePtr)
         {
            CheckActionPart(aRulePtr, aContainer, op->mPushedValuePtr, aPrevNodeIndex);
         }
         if (listTypePtr && !op->mAttributeAddr.IsEmpty())
         {
            aContainer = listTypePtr->mTemplateType;
         }
      }
      break;
      case WsfParseActionPart::cOBJECT_MAP_OP:
      {
         WsfParseActionObjectMapOp* op = dynamic_cast<WsfParseActionObjectMapOp*>(aActionPart);
         if (op->mOperation == WsfParseActionObjectMapOp::cAPPLY && aRulePtr)
         {
            std::vector<WsfParseRule*> seq         = aRulePtr->GetSequence();
            WsfParseRule*              prevNodePtr = seq[aPrevNodeIndex];
            if (prevNodePtr->Type() == WsfParseRule::cTYPE_LOAD)
            {
               WsfParseTypeLoadRule* typeRulePtr = dynamic_cast<WsfParseTypeLoadRule*>(prevNodePtr);
               if ((typeRulePtr->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE) ||
                   (typeRulePtr->GetOperation() == WsfParseTypeInfoData::cDELETE_TYPE))
               {
                  WsfParseValueType* resolvedType = aContainer;
                  WsfParseTypePath   path         = typeRulePtr->mLoadType.mPath;
                  if (typeRulePtr->mLoadType.mOrdinal == ut::npos)
                  {
                     path.pop_back();
                  }
                  if (typeRulePtr->mLoadType.mNestedLookup)
                  {
                     op->mAttributeAddr = ResolveActionAddress(resolvedType, path);
                  }
                  else
                  {
                     resolvedType       = mRootStructPtr->GetType();
                     op->mAttributeAddr = ResolveActionAddress(resolvedType, path);
                  }
                  op->mObjectMapTypePtr = resolvedType;
                  if (!op->mAttributeAddr.IsEmpty() &&
                      typeRulePtr->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE && resolvedType)
                  {
                     aContainer = resolvedType;
                  }
               }
               else if (typeRulePtr->GetOperation() == WsfParseTypeInfoData::cCREATE_TYPE)
               {
                  WsfParseValueType* resolvedType = aContainer;
                  WsfParseTypePath   path         = typeRulePtr->mSaveType.mPath;
                  if (typeRulePtr->mSaveType.mNestedLookup)
                  {
                     op->mAttributeAddr = ResolveActionAddress(resolvedType, path);
                  }
                  else
                  {
                     resolvedType = mRootStructPtr->GetType();
                     if (!resolvedType)
                     {
                        std::stringstream ss;
                        ss << "Error: Could not resolve type: " << mRootStructPtr->GetFullName() << ". \n";
                        AddDefinitionError(ss.str());
                     }
                     else
                     {
                        op->mAttributeAddr = ResolveActionAddress(resolvedType, path);
                     }
                  }
                  op->mObjectMapTypePtr = resolvedType;
                  if (!op->mAttributeAddr.IsEmpty() && resolvedType)
                  {
                     aContainer = resolvedType;
                  }
               }
            }
            else
            {
               std::stringstream ss;
               ss << "Error: Can only use apply() after type-load rules while processing "
                  << GetFullRuleName(*prevNodePtr->GetBestContext()) << ".\n";
               AddDefinitionError(ss.str());
            }
         }
         else
         {
            WsfParseValueType* mapTypePtr = aContainer;
            op->mAttributeAddr            = ResolveActionAddress(mapTypePtr, op->mAttributeName);
            if (mapTypePtr && !op->mAttributeAddr.IsEmpty())
            {
               WsfParseValueType* objectValueType = mapTypePtr;
               CheckActionPart(aRulePtr, objectValueType, op->mObjectName, aPrevNodeIndex);
               if (op->mOperation == WsfParseActionObjectMapOp::cGET || op->mOperation == WsfParseActionObjectMapOp::cNEW ||
                   op->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
               {
                  aContainer = objectValueType->mTemplateType;
               }
            }
         }
      }
      break;
      case WsfParseActionPart::cDEBUG_PRINT:
         // aContainer = aContainer;
         break;
      case WsfParseActionPart::cSKIP:
         aContainer = &mSkipType;
         break;
      case WsfParseActionPart::cCALL:
      {
         WsfParseActionCall* callPtr = dynamic_cast<WsfParseActionCall*>(aActionPart);
         auto                iter    = mFunctions.find(callPtr->mFunctionName);
         if (iter == mFunctions.end())
         {
            std::stringstream ss;
            ss << "Unknown action function '" << callPtr->mFunctionName << "'.";
            AddDefinitionError(ss.str());
         }
         else
         {
            callPtr->mFunctionPtr = iter->second.get();
            size_t argCount       = callPtr->mFunctionPtr->mArgumentNames.size();
            size_t actualArgCount = callPtr->mArgs.size();
            if (actualArgCount != argCount)
            {
               std::stringstream ss;
               ss << "Wrong argument count while calling '" << callPtr->mFunctionName << "'.  Expected " << argCount;
               AddDefinitionError(ss.str());
            }
            else
            {
               for (size_t i = 0; i < callPtr->mArgs.size(); ++i)
               {
                  CheckActionPart(aRulePtr, aContainer, callPtr->mArgs[i], aPrevNodeIndex);
                  // WsfParseValueType* expectedTypePtr = callPtr->mFunctionPtr->mSignature[i+1];
                  // WsfParseValueType* actualTypePtr = callPtr->mArgs[i]->mRHS_Type;
                  //                       if (actualTypePtr != expectedTypePtr)
                  //                       {
                  //                          std::string actualType = "none";
                  //                          if (actualTypePtr)
                  //                          {
                  //                             actualType = actualTypePtr->GetTypeName();
                  //                          }
                  //                          std::stringstream ss;
                  //                          ss << "Wrong argument type while calling '" << callPtr->mFunctionName <<
                  //                          "'.  Expected argument " << i+1 << " of type " << expectedTypePtr->mTypeName;
                  //                          ss << " but received " << actualType << ", inside " <<
                  //                          aRulePtr->GetBestContext()->GetFullName(); AddDefinitionError(ss.str());
                  //                       }
               }
            }
         }
      }
      break;
      default:
         break;
      }
   }
}

void WsfParseDefinitions::CheckAction(WsfParseRule*       aRulePtr,
                                      WsfParseValueType*& aContainer,
                                      WsfParseAction*     aAction,
                                      size_t              aPrevNodeIndex)
{
   if (!aAction)
   {
      return;
   }
   if (aContainer->mStructPtr || aContainer->mValuePtr)
   {
      for (size_t i = 0; i < aAction->mSubActions.size(); ++i)
      {
         WsfParseActionPart* sub = aAction->mSubActions[i];
         CheckActionPart(aRulePtr, aContainer, sub, aPrevNodeIndex);
      }
   }
   else
   {
      std::stringstream ss;
      ss << "Unexpected action on type " << aContainer->GetTypeName() << '.';
      AddDefinitionError(ss.str());
   }
}

void WsfParseDefinitions::CheckActions(WsfParseRule* aRulePtr, WsfParseNamedRule* aContextPtr)
{
   // if (aContextPtr == &mSkipType) return;
   WsfParseStruct* structContext = nullptr;

   // If the rule is a struct, check any actions added with the 'initially' command.
   if (aRulePtr->Type() == WsfParseRule::cSTRUCT)
   {
      WsfParseStruct* structPtr = dynamic_cast<WsfParseStruct*>(aRulePtr);
      for (size_t i = 0; i < structPtr->mInitialActions.size(); ++i)
      {
         WsfParseValueType* container = structPtr->GetType();
         CheckAction(nullptr, container, structPtr->mInitialActions[i], 0);
      }
   }

   if (aContextPtr && aContextPtr->Type() == WsfParseRule::cSTRUCT)
   {
      structContext = dynamic_cast<WsfParseStruct*>(aContextPtr);
   }
   if (aRulePtr->Type() == WsfParseRule::cRULE_REFERENCE)
   {
      WsfParseRuleReference* ref = dynamic_cast<WsfParseRuleReference*>(aRulePtr);
      if (ref->GetRulePtr())
      {
         // WsfParseStruct* refContext = ref->GetRulePtr()->GetStruct();
         WsfParseNamedRule* refContext = ref->GetRulePtr()->GetInputType();
         if (refContext != structContext && refContext)
         {
            if (refContext->Type() == WsfParseRule::cSTRUCT)
            {
               WsfParseStruct* refContextStruct = dynamic_cast<WsfParseStruct*>(refContext);
               bool            isType           = structContext && structContext->IsType(refContextStruct);
               if (!isType)
               {
                  WsfParseNamedRule* ctx = aRulePtr->GetBestContext();
                  if (ctx)
                  {
                     std::stringstream ss;
                     ss << "Referenced rule " << ref->mRuleName << " does not have the right context, while processing "
                        << GetFullRuleName(*ctx);
                     if (aContextPtr)
                     {
                        ss << "\n  Context is " << GetFullRuleName(*aContextPtr);
                     }
                     else
                     {
                        ss << "\n  No context.";
                     }
                     AddDefinitionError(ss.str());
                  }
               }
            }
         }
      }
      return;
   }

   WsfParseRuleDictionary* dict = aRulePtr->GetNestedRules();
   if (dict)
   {
      for (WsfParseRuleDictionary::NameRuleMap::iterator i = dict->mNameToRule.begin(); i != dict->mNameToRule.end(); ++i)
      {
         WsfParseNamedRule* parentContext = i->second->GetVarContext();
         if (!parentContext)
         {
            parentContext = mRootStructPtr;
         }
         CheckActions(i->second, parentContext);
      }
   }

   std::vector<WsfParseRule*> alt = aRulePtr->GetAlternates();
   std::vector<WsfParseRule*> seq = aRulePtr->GetSequence();
   WsfParseSequence*          sequenceRulePtr =
      (aRulePtr->Type() == WsfParseRule::cSEQUENCE) ? dynamic_cast<WsfParseSequence*>(aRulePtr) : nullptr;

   WsfParseNamedRule* contextPtr = aContextPtr;
   WsfParseNamedRule* outTypePtr = aRulePtr->GetOutputType();
   if (outTypePtr)
   {
      contextPtr = outTypePtr;
   }

   WsfParseValueType* contextContainer = contextPtr->GetType();
   WsfParseValueType* container        = contextContainer;
   for (size_t i = 0; i < alt.size(); ++i)
   {
      CheckActions(alt[i], contextPtr);
   }
   if (sequenceRulePtr)
   {
      WsfParseAction* a = sequenceRulePtr->GetActionBefore(0);
      CheckAction(aRulePtr, container, a, ut::npos);
      for (size_t i = 0; i < seq.size(); ++i)
      {
         WsfParseRule* r = seq[i];
         if (container->mStructPtr)
         {
            CheckActions(r, container->mStructPtr);
         }
         a         = sequenceRulePtr->GetActionBefore(i + 1);
         container = contextContainer;
         CheckAction(aRulePtr, container, a, i);
      }
   }
   else
   {
      for (size_t i = 0; i < seq.size(); ++i)
      {
         WsfParseNamedRule* typeRulePtr = container->GetTypeRule();
         if (typeRulePtr)
         {
            CheckActions(seq[i], typeRulePtr);
         }
      }
   }
}

WsfParseValueType* WsfParseDefinitions::GetTypeFromName(WsfParseNamedRule& aContext, const std::string& aName)
{
   WsfParseValueType* rval    = nullptr;
   WsfParseNamedRule* rulePtr = aContext.Find(aName);
   if (rulePtr)
   {
      rval = rulePtr->GetType();
   }
   else
   {
      std::map<std::string, WsfParseValueType*>::iterator iter = mTemplatedTypes.find(aName);
      if (iter == mTemplatedTypes.end())
      {
         bool               isList = false, isMap = false;
         std::string        templateType;
         WsfParseValueType* templateTypePtr = nullptr;
         if (aName.substr(0, 5) == "List/")
         {
            templateType = aName.substr(5);
            isList       = true;
         }
         else if (aName.substr(0, 10) == "ObjectMap/")
         {
            templateType = aName.substr(10);
            isMap        = true;
         }
         if (!templateType.empty())
         {
            templateTypePtr = GetTypeFromName(aContext, templateType);
         }
         if (templateTypePtr)
         {
            if (isList)
            {
               rval = WsfParseValueType::List(templateTypePtr);
            }
            else if (isMap)
            {
               rval = WsfParseValueType::ObjectMap(templateTypePtr);
            }
            else
            {
               assert(false);
            }
         }
         if (rval)
         {
            mTemplatedTypes[aName] = rval;
         }
      }
      else
      {
         rval = iter->second;
      }
   }
   return rval;
}

WsfParseValueType* WsfParseDefinitions::GetTypeFromName(const std::string& aName)
{
   return GetTypeFromName(*mRootRulePtr, aName);
}

WsfParseValueType* WsfParseDefinitions::GetTypeFromPath(WsfParseNamedRule& aContext, WsfParseTypePath& aPath)
{
   std::string path;
   for (size_t i = 0; i < aPath.size(); ++i)
   {
      if (i != 0)
      {
         path += '.';
      }
      path += aPath[i];
   }
   return GetTypeFromName(*mRootRulePtr, path);
}

const char* WsfParseDefinitions::MakeCharPtr(const std::string& aString)
{
   auto iter = mStrings.find(aString.c_str());
   if (iter != mStrings.end())
   {
      return iter->c_str();
   }
   auto result = mStrings.insert(aString);
   return result.first->c_str();
}

void WsfParseDefinitions::AddDefinitionError(const std::string& aError)
{
   mDefinitionErrors.push_back(aError);
}

void WsfParseDefinitions::InitializeFunctions()
{
   for (auto iter = mFunctions.begin(); iter != mFunctions.end(); ++iter)
   {
      WsfParseActionFunction& fn = *iter->second;
      for (size_t i = 0; i < fn.mSignatureStrings.size(); ++i)
      {
         WsfParseValueType* typePtr = GetTypeFromName(fn.mSignatureStrings[i]);
         if (typePtr)
         {
            fn.mSignature.push_back(typePtr);
         }
         else
         {
            std::stringstream ss;
            ss << "Error: Invalid type: " << fn.mSignatureStrings[i] << " in function " << iter->first;
            AddDefinitionError(ss.str());
         }
      }
   }
}
