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

#include "WsfPProxySatisfy.hpp"

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfPProxyActionLookup.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyDiff.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeLoadRule.hpp"

using UValue = WsfPProxyUndoValue;
//#define PROXY_SERIALIZE_TRACE

WsfPProxySatisfy::WsfPProxySatisfy(WsfPProxyRegistry*   aRegistryPtr,
                                   WsfPProxyValue       aDesiredRootValue,
                                   WsfPProxyUndoValue   aCurrentRootValue,
                                   WsfPParseRuleTracer* aTracerPtr)
   : mRegistryPtr(aRegistryPtr)
   , mEvaluator(aRegistryPtr)
   , mActionLookupPtr(ut::make_unique<WsfPProxyActionLookup>(aRegistryPtr))
{
   mSatMode                   = cSM_NONE;
   mRecurrenceInsertBeforeEnd = false;
   mDesiredRoot               = aDesiredRootValue;
   mCurrentRoot               = aCurrentRootValue;
   mTracerPtr                 = aTracerPtr;
   mTracePtr                  = mTracerPtr->Root();

   mTypesThatNeedToBeCleared.insert(aRegistryPtr->GetType("Route"));
}

WsfPProxySatisfy::~WsfPProxySatisfy() {}

// Returns 'true' if any changes were made while trying to satisfy this attribute.
// The attribute may remain unsatisfied
bool WsfPProxySatisfy::SatisfyAttribute(const WsfPProxyPath& aPath)
{
   mSatMode = cSM_TARGET;
#ifdef PROXY_SERIALIZE_TRACE
   { // RAII block
      auto logger = ut::log::debug() << "BEGIN SatisfyAttribute";
      logger.AddNote() << "Path: " << aPath.ToString(mDesiredRoot);
   }
#endif
   const int cMAX_ITERATIONS = 10;
   int       iteration       = 0;
   bool      madeChanges     = false;
   while (!IsAttributeSatisfied(aPath))
   {
      if (iteration++ > cMAX_ITERATIONS)
      {
         break;
      }

      mTargetPath           = aPath;
      WsfPProxyPath ctxPath = aPath;

      bool iterationMadeChanges = false;
      bool foundContext         = false;
      while (!ctxPath.Empty() && !iterationMadeChanges)
      {
         WsfPParseRuleTracer::Contexts* ctxs = mTracerPtr->FindContexts(ctxPath);
         if (ctxs)
         {
            size_t contextCount = ctxs->mContextList.size();
            for (size_t i = 0; i < contextCount && !iterationMadeChanges; ++i)
            {
               size_t                     idx = contextCount - i - 1;
               auto&                      ctx = ctxs->mContextList[idx];
               WsfPProxySatisfy::SatPoint pt;
               pt.desired           = mDesiredRoot.Lookup(ctxPath);
               pt.current           = mCurrentRoot.GetAttr(ctxPath);
               pt.trace             = ctx->mTracePtr;
               SatisfyResult result = Satisfy(pt);
               iterationMadeChanges = result.TargetComplete();
               foundContext         = true;
            }
         }
         ctxPath.Pop();
      }
      if (!iterationMadeChanges && !foundContext)
      {
         WsfPProxySatisfy::SatPoint pt;
         pt.desired           = mDesiredRoot;
         pt.current           = mCurrentRoot;
         pt.trace             = mTracePtr->AddChild(mRegistryPtr->mEntryPointRule);
         SatisfyResult result = Satisfy(pt);
         iterationMadeChanges = result.TargetComplete();
      }
      if (!iterationMadeChanges)
      {
         break;
      }
      madeChanges = true;

      {
         // If this is a complex attribute, don't keep looping.  Let Caller do another diff
         WsfPProxyValue desired(mDesiredRoot.Lookup(aPath));
         if (desired && !desired.IsBasicType())
         {
            break;
         }
      }
   }

   return madeChanges;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyAttributeInContext(const WsfPProxyPath&       aPath,
                                                                            WsfPParseRuleTraceContext* aContextPtr)
{
   mTargetPath = aPath;
   WsfPProxySatisfy::SatPoint pt;

   WsfPProxyPath ctxPath = aPath;
   while (ctxPath.size() > aContextPtr->GetAddr().size())
   {
      WsfPParseRuleTracer::Contexts* contexts = mTracerPtr->FindContexts(ctxPath);
      if (contexts)
      {
         for (size_t i = 0; i < contexts->mContextList.size(); ++i)
         {
            auto& ctxPtr = contexts->mContextList[i];
            if (!ctxPtr->mTracePtr->IsPruned() && ctxPtr->HasAncestor(aContextPtr))
            {
               pt.desired                             = mDesiredRoot.Lookup(ctxPtr->GetAddr());
               pt.current                             = mCurrentRoot.GetAttr(ctxPtr->GetAddr());
               pt.trace                               = ctxPtr->mTracePtr;
               WsfPProxySatisfy::SatisfyResult result = Satisfy(pt);
               if (result.TargetComplete())
               {
                  return result;
               }
            }
         }
      }
      ctxPath.Pop();
   }
   pt.desired = mDesiredRoot.Lookup(aContextPtr->GetAddr());
   pt.current = mCurrentRoot.GetAttr(aContextPtr->GetAddr());
   pt.trace   = aContextPtr->mTracePtr;
   return Satisfy(pt);
}

bool WsfPProxySatisfy::IsEqual(const WsfPProxyValue& A, const WsfPProxyValue& B)
{
   WsfPProxyBasicValue a(A);
   WsfPProxyBasicValue b(B);
   return a.IsValid() && b.IsValid() && a.IsEqual(b);
}

WsfPProxyValue WsfPProxySatisfy::LookupBasicValue(const WsfPProxyPath& aPath)
{
   WsfPProxyValue root = mRegistryPtr->GetBasicRoot();
   return root.Lookup(aPath);
}

WsfPProxyValue WsfPProxySatisfy::LookupBasicValue(const WsfParseTypePath& aPath)
{
   WsfPProxyValue root = mRegistryPtr->GetBasicRoot();
   for (size_t i = 0; i < aPath.size() && root; ++i)
   {
      root = root[aPath[i]];
   }
   return root;
}

WsfPProxyUndoValue WsfPProxySatisfy::LookupCurrent(WsfPProxyUndoValue& aCurrent, const WsfParseTypePath& aAddr, bool aRelative)
{
   if (!aRelative)
   {
      WsfPProxyUndoValue val(mCurrentRoot);
      for (size_t i = 0; i < aAddr.size() && val; ++i)
      {
         val.MoveToAttr(aAddr[i].Get());
      }
      return val;
   }
   else
   {
      WsfPProxyUndoValue val(aCurrent);
      for (size_t i = 0; i < aAddr.size() && val; ++i)
      {
         val.MoveToAttr(aAddr[i].Get());
      }
      return val;
   }
}

WsfPProxyValue WsfPProxySatisfy::LookupCurrentOrBasicValue(WsfPProxyUndoValue&     aCurrent,
                                                           const WsfParseTypePath& aAddr,
                                                           bool                    aRelative)
{
   WsfPProxyUndoValue val  = LookupCurrent(aCurrent, aAddr, aRelative);
   WsfPProxyValue     rval = val.Get();
   if (!rval)
   {
      rval = LookupBasicValue(aAddr);
   }
   return rval;
}

WsfPProxyUndoValue WsfPProxySatisfy::LookupCurrent(WsfPProxyUndoValue& aCurrent, const WsfParseActionAddress& aAddr)
{
   WsfPProxyUndoValue val(aCurrent);
   for (size_t i = 0; i < aAddr.mAttrIndices.size(); ++i)
   {
      const WsfParseActionAddress::Entry& e = aAddr.mAttrIndices[i];
      if (val)
      {
         if (e.mType == WsfParseActionAddress::EntryType::cROOT)
         {
            val = mCurrentRoot;
         }
         else if (e.IsNodeIndex())
         {
            if (0)
            {
               // int idx = e.GetNodeIndex();
               // if (idx < (int)aReturnValuesPtr->size() && idx >= 0)
               {
                  // return aReturnValuesPtr->at(idx);
               } // else
               {
                  assert(!"Not enough return values");
               }
            }
            else
            {
               assert(!"No return values given");
            }
         }
         else if (e.IsAttributeIndex())
         {
            val.MoveToAttr(e.mIndex);
         }
         else
         {
            val.MoveToAttr(e.mName);
         }
      }
      else
      {
         // todo: we need to decide how to handle errors
         assert(false);
      }
   }
   return val;
}

WsfPProxyValue WsfPProxySatisfy::Lookup(WsfPProxyValue&              aRoot,
                                        const WsfPProxyValue&        c,
                                        const WsfParseActionAddress& aAddr,
                                        WsfPProxyPath*               aUpdatePath /*=0*/)
{
   WsfPProxyValue val = c;
   for (size_t i = 0; i < aAddr.mAttrIndices.size(); ++i)
   {
      const WsfParseActionAddress::Entry& e = aAddr.mAttrIndices[i];
      if (val)
      {
         if (e.mType == WsfParseActionAddress::EntryType::cROOT)
         {
            if (aUpdatePath)
            {
               aUpdatePath->Clear();
            }
            val = aRoot;
         }
         else if (e.IsNodeIndex())
         {
            assert(false); // use other form of Lookup() if this is a possibility
         }
         else if (e.IsAttributeIndex())
         {
            if (aUpdatePath)
            {
               (*aUpdatePath) += e.mIndex;
            }
            val = val.GetAtIndex(e.mIndex);
         }
         else // (e.IsString())
         {
            std::string name(e.mName);
            if (aUpdatePath)
            {
               (*aUpdatePath) += name;
            }
            val = val.GetAttr(name);
         }
      }
      else
      {
         // todo: we need to decide how to handle errors
         return val;
         assert(false);
      }
   }
   return val;
}

WsfPProxyValue WsfPProxySatisfy::LookupDesired(WsfPProxyValue          aDesiredValue,
                                               const WsfParseTypePath& aAddr,
                                               bool                    aRelative,
                                               WsfPProxyPath*          aUpdatePath)
{
   WsfPProxyValue val;
   if (aRelative)
   {
      val = aDesiredValue;
   }
   else
   {
      val = mDesiredRoot;
   }
   for (size_t i = 0; i < aAddr.size() && val; ++i)
   {
      const std::string& str = aAddr[i].Get();
      if (aUpdatePath)
      {
         aUpdatePath->Push(val, str);
      }
      val = val.GetAttr(str);
   }
   return val;
}

void WsfPProxySatisfy::EquationExpression(WsfParseActionPart* aExprPtr, WsfParseValueType* aLHS_TypePtr, EquationExpr& aExpr)
{
   if (aExprPtr->mActionPartType == WsfParseActionPart::cLITERAL)
   {
      WsfParseActionLiteral* lit = (WsfParseActionLiteral*)aExprPtr;
      if (aLHS_TypePtr)
      {
         WsfPProxyType* lhsTypePtr = mRegistryPtr->GetType(aLHS_TypePtr);
         if (lhsTypePtr)
         {
            if (lhsTypePtr->IsBasicType())
            {
               WsfPProxyBasicType* lhsBasicType = (WsfPProxyBasicType*)lhsTypePtr;
               WsfPProxyBasicValue basicValue   = WsfPProxyValue::ConstructNew(lhsBasicType);
               if (lit->mText.empty())
               {
                  basicValue.SetUnset();
               }
               else
               {
                  basicValue.SetValue(lit->mText);
               }
               aExpr = EquationExpr(basicValue);
            }
            else
            {
               assert(lit->mText.empty());
               aExpr = EquationExpr::UnsetValue();
            }
         }
      }
      else
      {
         aExpr = EquationExpr(lit->mText);
      }
   }
   else if (aExprPtr->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
   {
      WsfParseActionNodeReference* nr = (WsfParseActionNodeReference*)aExprPtr;
      aExpr                           = EquationExpr(nr->mIndex);
   }
}

WsfPProxySatisfy::EquationExpr WsfPProxySatisfy::EvalExpr(EquationExpr& aExpr)
{
   // EquationExpr::cVALUE is currently the only implemented expression type.
   assert(aExpr.mExprType == EquationExpr::cVALUE);
   return aExpr;
}

// Solve an equation
// aLhs - The desired proxy value
// aRhs - The Right hand side of the assignment being made
bool WsfPProxySatisfy::SolveEquation(const WsfPProxyValue& aLhs, EquationExpr& aRHS, std::vector<WsfPProxyValue*>& aVariables)
{
   if (aRHS.mExprType == EquationExpr::cVALUE)
   {
      return IsEqual(aLhs, aRHS.mValue);
   }
   else if (aRHS.mExprType == EquationExpr::cUNSET_VALUE)
   {
      // In almost all cases, we only want to make something unset if the desired value is unset
      if (aLhs.IsUnset())
      {
         return true;
      }
      // However, there are some cases where it is impossible to simply update a value without first clearing it.

      if (((aLhs.GetList() != nullptr) || mTypesThatNeedToBeCleared.find(aLhs.GetType()) != mTypesThatNeedToBeCleared.end()))
      {
         // This moves our current state without getting closer to the desired state
         // Therefore, we can only do this a limited number of times or risk never converging on any state
         if (mAttemptedToClear.insert(aLhs.GetDataPtr()).second)
         {
            return true;
         }
      }
      return false;
   }
   else if (aRHS.mExprType == EquationExpr::cFUNCTION)
   {
      assert(!"not yet implemented");
      return false;
   }
   else if (aRHS.mExprType == EquationExpr::cNODE_INDEX)
   {
      if (aVariables[aRHS.mNodeIndex] == nullptr)
      {
         aVariables[aRHS.mNodeIndex] = new WsfPProxyValue(aLhs.Copy());
         return true;
      }
      else
      {
         return *aVariables[aRHS.mNodeIndex] == aLhs;
      }
   }
   // TODO: There are some equations we can't solve yet.
   //       one major missing piece is 'functions'
   // assert(false);
   return false;

   return true;
}

bool WsfPProxySatisfy::SolveEquations(const WsfPProxyValue&  aCurrentDesired /*, WsfParseActionAddress& aPrefixAddr*/,
                                      std::vector<Equation>& aEquations,
                                      std::vector<WsfPProxyValue*>& aVariables)
{
   bool ok = true;
   for (size_t i = 0; i < aEquations.size(); ++i)
   {
      Equation& eq = aEquations[i];
      // WsfParseActionAddress addr = eq.mLHS;

      // addr += aPrefixAddr;
      std::string    __DEBUG_PATH_STR = eq.mLHS.ToString(mDesiredRoot);
      WsfPProxyValue lhsVal           = mDesiredRoot.Lookup(eq.mLHS);
      // WsfPProxyValue lhsVal = Lookup(mDesiredRoot, aCurrentDesired, addr);
      WsfPProxyBasicValue debug___(lhsVal);
      if (!SolveEquation(lhsVal, eq.mRHS, aVariables))
      {
         ok = false;
         break;
      }
   }
   return ok;
}

bool WsfPProxySatisfy::BuildSequenceEquations(WsfParseSequence*             aSequence,
                                              WsfPProxyUndoValue&           aCurrentValue,
                                              const WsfPProxyValue&         aDesiredValue,
                                              std::vector<Equation>&        aEquations,
                                              WsfParseNode*                 aExistingNodePtr,
                                              std::vector<WsfPProxyValue*>& aVariables)
{
   WsfPProxyUndoValue current = aCurrentValue;
   WsfPProxyValue     desired = aDesiredValue;
   // WsfPProxyPath      currentPath = aCurrentValue.GetPath();
   const std::vector<WsfParseRule*>& seq = aSequence->Sequence();
   for (size_t i = 0; i < seq.size() + 1; ++i)
   {
      // WsfParseActionAddress prefixAddr;
      WsfParseAction* actionPtr = aSequence->GetActionBefore(i);
      if (!actionPtr)
      {
         continue;
      }
      for (size_t j = 0; j < actionPtr->mSubActions.size(); ++j)
      {
         WsfParseActionPart* subPtr = actionPtr->mSubActions[j];
         if (subPtr->mActionPartType == WsfParseActionPart::cASSIGN)
         {
            WsfParseActionAssign* ass = (WsfParseActionAssign*)subPtr;
            Equation              e;
            e.mActionSet = i;
            e.mLHS       = LookupCurrent(current, ass->mLHS_Address).GetPath();
            // e.mLHS += ass->mLHS_Address;
            EquationExpression(ass->mRHS, ass->mLHS_Type, e.mRHS);
            aEquations.push_back(e);
         }
         if (subPtr->mActionPartType == WsfParseActionPart::cPUSH)
         {
            WsfParseActionPush* pushPtr = (WsfParseActionPush*)subPtr;
            // prefixAddr += pushPtr->mAttributeAddr;
            current = LookupCurrent(current, pushPtr->mAttributeAddr);
            desired = LookupDesired(desired, pushPtr->mAttributeAddr);
         }
         if (subPtr->mActionPartType == WsfParseActionPart::cOBJECT_MAP_OP)
         {
            WsfParseActionObjectMapOp* mapOp = (WsfParseActionObjectMapOp*)subPtr;
            if (mapOp->mOperation == WsfParseActionObjectMapOp::cNEW ||
                mapOp->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
            {
               // prefixAddr += mapOp->mAttributeAddr;
               std::string objName = "";
               if (aExistingNodePtr)
               {
                  objName = mEvaluator.StringEval(aExistingNodePtr, *mapOp->mObjectName);
               }
               else
               {
                  bool foundMatch = false;
                  // Find an entry in the map which does not already exist
                  WsfPProxyUndoValue currentMap = LookupCurrent(current, mapOp->mAttributeAddr);
                  WsfPProxyValue     desiredMap = LookupDesired(desired, mapOp->mAttributeAddr);
                  if (currentMap && desiredMap && currentMap.GetPath().IsPrefixOf(mTargetPath) &&
                      mTargetPath.size() > currentMap.GetPath().size())
                  {
                     current                      = currentMap;
                     desired                      = desiredMap;
                     std::string targetObjectName = mTargetPath[currentMap.GetPath().size()].GetMapKey();
                     if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
                     {
                        WsfParseActionNodeReference* nodeRef = (WsfParseActionNodeReference*)mapOp->mObjectName;

                        current.MoveToAttr(targetObjectName);
                        desired = desired.GetAttr(targetObjectName);

                        aVariables[nodeRef->mIndex] =
                           new WsfPProxyValue(WsfPProxyValue::ConstructNew(mRegistryPtr->GetStringType()));
                        WsfPProxyBasicValue(*aVariables[nodeRef->mIndex]).SetValue(targetObjectName);
                        foundMatch = true;
                     }
                     else if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cLITERAL)
                     {
                        if (((WsfParseActionLiteral*)mapOp->mObjectName)->mText == targetObjectName)
                        {
                           current.MoveToAttr(targetObjectName);
                           desired    = desired.GetAttr(targetObjectName);
                           foundMatch = true;
                        }
                     }
                  }
                  //                   if (currentMap && desiredMap)
                  //                   {
                  //                      current = currentMap;
                  //                      desired = desiredMap;
                  //
                  //                      bool foundMatch = false;
                  //                      WsfPProxyObjectMap* mapPtr = desiredMap.GetObjectMap();
                  //                      const std::map<std::string, WsfPProxyValue>& desiredValues =
                  //                      mapPtr->GetValues(); for (std::map<std::string,
                  //                      WsfPProxyValue>::const_iterator iter = desiredValues.begin(); iter !=
                  //                      desiredValues.end(); ++iter)
                  //                      {
                  //                         WsfPProxyValue currentMapEntry = currentMap.Get().GetAttr(iter->first);
                  //                         if (! currentMapEntry)
                  //                         {
                  //                            // TODO: What do we do here????
                  //                            //       The node isn't really a proxy value
                  //                            aVariables[nodeRef->mIndex] = new
                  //                            WsfPProxyValue(mRegistryPtr->GetStringType()->ConstructNew());
                  //                            std::string objectName = iter->first;
                  //                            WsfPProxyBasicValue(*aVariables[nodeRef->mIndex]).SetValue(objectName);
                  //                            current.MoveToAttr(objectName);
                  //                            desired = desired.GetAttr(objectName);
                  //                            foundMatch = true;
                  //                            break;
                  //                         }
                  //                      }
                  if (!foundMatch)
                  {
                     return false;
                  }
                  //}
               }
            }
            else if (mapOp->mOperation == WsfParseActionObjectMapOp::cDELETE)
            {
               // Find an entry in the map which does not already exist
               WsfPProxyUndoValue currentMap = LookupCurrent(current, mapOp->mAttributeAddr);
               WsfPProxyValue     desiredMap = LookupDesired(desired, mapOp->mAttributeAddr);
               if (currentMap && desiredMap && currentMap.GetPath().IsPrefixOf(mTargetPath) &&
                   mTargetPath.size() == currentMap.GetPath().size() + 1)
               {
                  std::string typeToRemove = mTargetPath.Back().GetMapKey();
                  if (!desiredMap.GetAttr(typeToRemove) && currentMap.GetAttr(typeToRemove))
                  {
                     if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
                     {
                        WsfParseActionNodeReference* nodeRef = (WsfParseActionNodeReference*)mapOp->mObjectName;

                        aVariables[nodeRef->mIndex] =
                           new WsfPProxyValue(WsfPProxyValue::ConstructNew(mRegistryPtr->GetStringType()));
                     }
                  }
               }
            }
         }
      }
   }
   return true;
}

int WsfPProxySatisfy::ApplyEquations(size_t                 aActionSetIndex,
                                     size_t&                aNextEquation,
                                     std::vector<Equation>& aEquations,
                                     WsfPProxyUndoValue&    aCurrentValue,
                                     WsfPProxyValue         aDesiredValue)
{
   int changeFlags = 0;
   while (aNextEquation < aEquations.size() && aActionSetIndex <= aEquations[aNextEquation].mActionSet)
   {
      Equation&           eq      = aEquations[aNextEquation];
      WsfPProxyUndoValue  current = mCurrentRoot.GetAttr(eq.mLHS); // LookupCurrent(aCurrentValue, eq.mLHS);
      WsfPProxyBasicValue desired = mDesiredRoot.Lookup(eq.mLHS);
      // WsfPProxyBasicValue desired = LookupDesired(aDesiredValue, eq.mLHS);
      bool copyOk = true;
      if (eq.mRHS.mExprType == EquationExpr::cUNSET_VALUE)
      {
         current.SetUnset(true);
         if (current.GetPath().IsPrefixOf(mTargetPath))
         {
            changeFlags |= cCHANGED_TARGET;
         }
         changeFlags |= cADDS_VALUE;
      }
      else
      {
         WsfPProxyBasicValue basicValue(current.Get());
         if (!basicValue.IsEqual(desired))
         {
            if (mTargetPath == current.GetPath())
            {
               changeFlags |= cCHANGED_TARGET;
            }
            copyOk = current.Assign(desired.Copy());
            if (!copyOk)
            {
               current.Assign(desired.Copy());
            }
            changeFlags |= cADDS_VALUE;
         }
      }
      assert(copyOk);
      ++aNextEquation;
   }
   return changeFlags;
}

void WsfPProxySatisfy::FollowAction(WsfParseAction*              aActionPtr,
                                    WsfPParseRuleTrace*          aParentTracePtr,
                                    WsfPProxyUndoValue&          aCurrentValue,
                                    WsfPProxyValue*              aDesiredValue,
                                    WsfPProxyPath&               aCurrentPath,
                                    std::vector<WsfPProxyValue*> aVariables,
                                    SatisfyResult&               aResult)
{
   aResult = cSATISFIABLE;
   for (size_t i = 0; i < aActionPtr->mSubActions.size(); ++i)
   {
      WsfParseActionPart* act = aActionPtr->mSubActions[i];
      switch (act->mActionPartType)
      {
      case WsfParseActionPart::cPUSH:
      {
         WsfParseActionPush* push         = (WsfParseActionPush*)act;
         WsfPProxyValue      desiredValue = LookupDesired(*aDesiredValue, push->mAttributeAddr, &aCurrentPath);
         WsfPProxyUndoValue  currentValue = LookupCurrent(aCurrentValue, push->mAttributeAddr);

         if (desiredValue && currentValue.Exists())
         {
            *aDesiredValue = desiredValue;
            aCurrentValue  = currentValue;
         }
         else
         {
            aResult = cNOT_SATISFIABLE;
            break;
         }
      }
      break;
      case WsfParseActionPart::cOBJECT_MAP_OP:
      {
         WsfParseActionObjectMapOp* mapOp = (WsfParseActionObjectMapOp*)act;
         if (mapOp->mOperation == WsfParseActionObjectMapOp::cNEW || mapOp->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
         {
            WsfPProxyValue desiredMap = LookupDesired(*aDesiredValue, mapOp->mAttributeAddr, &aCurrentPath);
            // WsfPProxyObjectMap* newMapPtr = newMap.GetObjectMap();
            if (desiredMap)
            {
               UValue curMap = LookupCurrent(aCurrentValue, mapOp->mAttributeAddr);
               if (curMap)
               {
                  std::string key;
                  // If we are executing a trace with existing inputs, use the name given
                  if (aParentTracePtr->mNodePtr)
                  {
                     key = mEvaluator.StringEval(aParentTracePtr->mNodePtr, *mapOp->mObjectName);
                  }
                  // Otherwise, use a name that been selected earlier
                  else if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
                  {
                     WsfParseActionNodeReference* nodeRef = (WsfParseActionNodeReference*)mapOp->mObjectName;
                     WsfPProxyValue*              objName = aVariables[nodeRef->mIndex];
                     if (objName)
                     {
                        key = WsfPProxyBasicValue(*objName).ToString();
                     }
                  }
                  else if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cLITERAL)
                  {
                     key = ((WsfParseActionLiteral*)mapOp->mObjectName)->mText;
                  }
                  if (!key.empty())
                  {
                     WsfPProxyValue desiredValue = desiredMap.GetAttr(key);
                     if (!desiredValue)
                     {
                        aResult = cNOT_SATISFIABLE;
                        return;
                     }
                     aCurrentPath += key;
                     if (curMap.MapInsert(key, WsfPProxyValue::ConstructNew(desiredValue.GetType())))
                     {
                        aCurrentValue = curMap;
                        aCurrentValue.MoveToAttr(key);
                        *aDesiredValue = desiredValue;
                        aResult += cADDS_VALUE;
                        if (aCurrentPath.IsPrefixOf(mTargetPath))
                        {
                           aResult += cCHANGED_TARGET;
                        }
                     }
                     return;
                  }
                  else
                  {
                     aResult = cNOT_SATISFIABLE;
                     return;
                  }
               }
            }
         }
         // An apply operations can do load, create, or delete -- all depending on the grammar rule used.
         if (mapOp->mOperation == WsfParseActionObjectMapOp::cAPPLY)
         {
            // Find the rule that is being applied
            WsfParseTypeLoadRule* loadRulePtr = nullptr;
            WsfParseNode*         loadNodePtr = nullptr;
            for (size_t i = 0; i < aParentTracePtr->Children().size(); ++i)
            {
               WsfPParseRuleTrace* childTracePtr = aParentTracePtr->Children()[i];
               WsfParseRule*       rulePtr       = childTracePtr->Rule();
               if (rulePtr && rulePtr->Type() == WsfParseRule::cTYPE_LOAD)
               {
                  loadRulePtr = (WsfParseTypeLoadRule*)rulePtr;
                  loadNodePtr = childTracePtr->mNodePtr;
                  break;
               }
            }
            if (loadRulePtr)
            {
               if (WsfParseTypeInfoData::cCREATE_TYPE == loadRulePtr->GetOperation())
               {
                  // an existing input
                  if (loadNodePtr)
                  {
                     WsfParseTypeInfoData* typeInfoPtr = (WsfParseTypeInfoData*)loadNodePtr->GetAuxiliaryValue();
                     if (typeInfoPtr)
                     {
                        WsfPProxyValue desiredSave =
                           LookupDesired(*aDesiredValue,
                                         typeInfoPtr->mSaveKey,
                                         (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_SAVE) != 0,
                                         &aCurrentPath);
                        if (desiredSave)
                        {
                           WsfPProxyValue desiredLoad =
                              LookupDesired(*aDesiredValue,
                                            typeInfoPtr->mLoadKey,
                                            (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_LOAD) != 0,
                                            &aCurrentPath);
                           WsfParseTypePath saveMap      = typeInfoPtr->mSaveKey;
                           UtStringRef      loadTypeName = saveMap.back();
                           saveMap.pop_back();
                           if (!desiredLoad)
                           {
                              WsfPProxyType* loadTypePtr = mRegistryPtr->FindBasicType(typeInfoPtr->mLoadKey);
                              if (loadTypePtr)
                              {
                                 UValue currentMap =
                                    LookupCurrent(aCurrentValue,
                                                  saveMap,
                                                  (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_SAVE) != 0);
                                 if (currentMap &&
                                     currentMap.MapInsert(loadTypeName, WsfPProxyValue::ConstructNew(loadTypePtr)))
                                 {
                                    aCurrentValue  = currentMap.GetAttr(loadTypeName.Get());
                                    *aDesiredValue = desiredSave;
                                    aResult        = cSUCCESS;
                                    return;
                                 }
                              }
                           }
                        }
                     }
                  }
                  else // No existing input, choose a value to add to map
                  {
                     // The following "if" condition is commented because it
                     // breaks the saving of mover/fuel to a file. Commenting
                     // it out might break something else though.
                     // if (loadRulePtr->mSaveType.mOrdinal >= 0)
                     {
                        WsfParseTypePath savePath = loadRulePtr->mSaveType.mPath;
                        WsfParseTypePath loadPath = loadRulePtr->mLoadType.mPath;
                        if (loadRulePtr->mSaveType.mOrdinal != ut::npos)
                        {
                           WsfPParseRuleTrace* t = aParentTracePtr->Children()[loadRulePtr->mSaveType.mOrdinal];
                           savePath.pop_back();
                           savePath.push_back(t->GetText());
                        }
                        if (loadRulePtr->mLoadType.mOrdinal != ut::npos)
                        {
                           WsfPParseRuleTrace* t = aParentTracePtr->Children()[loadRulePtr->mLoadType.mOrdinal];
                           loadPath.pop_back();
                           loadPath.push_back(t->GetText());
                        }
                        WsfPProxyValue desiredSub =
                           LookupDesired(*aDesiredValue, savePath, loadRulePtr->mSaveType.mNestedLookup, &aCurrentPath);
                        if (desiredSub)
                        {
                           WsfPProxyValue currentLoad =
                              LookupCurrentOrBasicValue(aCurrentValue, loadPath, loadRulePtr->mLoadType.mNestedLookup);
                           if (currentLoad)
                           {
                              std::string objName = savePath.back();
                              savePath.pop_back();
                              UValue currentMap =
                                 LookupCurrent(aCurrentValue, savePath, loadRulePtr->mSaveType.mNestedLookup);
                              currentMap.MapInsert(objName, currentLoad.Copy());

                              aCurrentValue  = currentMap.GetAttr(objName);
                              *aDesiredValue = desiredSub;
                              aResult        = cSUCCESS;
                              if (aCurrentValue.GetPath() == mTargetPath)
                              {
                                 aResult += cCHANGED_TARGET;
                              }
                              return;
                           }
                        }
                        /*
                        if (desiredMap && desiredMap.GetObjectMap())
                        {
                           UValue currentMap = LookupCurrent(aCurrentValue,
                        savePath,loadRulePtr->mSaveType.mNestedLookup); if (currentMap)
                           {
                              if (aCurrentPath.IsPrefixOf(mTargetPath) && mTargetPath.size() > aCurrentPath.size())
                              {
                                 std::string objName = mTargetPath[aCurrentPath.size()].mMapKey;
                                 WsfPProxyObjectMap* desiredMapPtr = desiredMap.GetObjectMap();
                                 WsfPProxyValue desiredObject = desiredMap.GetObjectMap()->GetAt(objName);
                                 if (desiredObject)
                                 {

                                 }
                                 for(std::map<std::string, WsfPProxyValue>::iterator i =
                        desiredMapPtr->GetValues().begin(); i != desiredMapPtr->GetValues().end(); ++i)
                                 {
                                    WsfPProxyValue current = currentMap.Get().GetAttr(i->first);
                                    if (!current)
                                    {
                                       // found value that needs to be added...
                                       //WsfPProxyValue desiredEntry = i->second;
                                       //LookupDesired(*aDesiredValue, loadRulePtr->mLoadType.,
                        loadRulePtr->mLoadType.mNestedLookup);
                                    }
                                 }
                              }
                           }
                        }
                        */
                     }
                  }
               }
               else if (WsfParseTypeInfoData::cLOAD_TYPE == loadRulePtr->GetOperation())
               {
                  if (loadNodePtr)
                  {
                     WsfParseTypeInfoData* typeInfoPtr = (WsfParseTypeInfoData*)loadNodePtr->GetAuxiliaryValue();
                     if (typeInfoPtr)
                     {
                        WsfPProxyValue desiredLoad =
                           LookupDesired(*aDesiredValue,
                                         typeInfoPtr->mLoadKey,
                                         (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_LOAD) != 0,
                                         &aCurrentPath);
                        if (desiredLoad)
                        {
                           WsfParseTypePath loadMap      = typeInfoPtr->mLoadKey;
                           UtStringRef      loadTypeName = loadMap.back();
                           loadMap.pop_back();
                           UValue currentMap =
                              LookupCurrent(aCurrentValue,
                                            loadMap,
                                            (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_LOAD) != 0);
                           UValue currentLoaded = currentMap.GetAttr(loadTypeName);
                           if (currentLoaded)
                           {
                              aCurrentValue  = currentLoaded;
                              *aDesiredValue = desiredLoad;
                              aResult        = cSUCCESS;
                              return;
                           }
                        }
                     }
                  }
                  else // No existing input
                  {
                     WsfParseTypePath mapPath = loadRulePtr->mLoadType.mPath;
                     std::string      objName = mapPath.back();
                     mapPath.pop_back();
                     WsfPProxyValue desiredMap =
                        LookupDesired(*aDesiredValue, mapPath, loadRulePtr->mLoadType.mNestedLookup, &aCurrentPath);
                     if ((aCurrentPath.IsPrefixOf(mTargetPath) && aCurrentPath.size() < mTargetPath.size()) &&
                         (desiredMap && desiredMap.GetObjectMap()))
                     {
                        bool ok = true;
                        if (loadRulePtr->mLoadType.mOrdinal != ut::npos)
                        {
                           objName = mTargetPath[aCurrentPath.size()].GetMapKey();
                        }
                        else
                        {
                           ok = (objName == mTargetPath[aCurrentPath.size()].GetMapKey());
                        }

                        if (ok)
                        {
                           UValue currentMap = LookupCurrent(aCurrentValue, mapPath, loadRulePtr->mLoadType.mNestedLookup);
                           if (currentMap)
                           {
                              WsfPProxyValue desiredObject = desiredMap[objName];
                              if (desiredObject)
                              {
                                 UValue currentVal = currentMap.GetAttr(objName);
                                 if (currentVal)
                                 {
                                    aCurrentValue  = currentVal;
                                    *aDesiredValue = desiredObject;
                                    aCurrentPath += objName;
                                    aResult = cSUCCESS;
                                    return;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               else if (WsfParseTypeInfoData::cDELETE_TYPE == loadRulePtr->GetOperation())
               {
                  if (loadNodePtr)
                  {
                     aResult = cSUCCESS;
                     return;
                     //                         WsfParseTypeInfoData* typeInfoPtr =
                     //                         (WsfParseTypeInfoData*)loadNodePtr->GetAuxiliaryValue(); if
                     //                         (typeInfoPtr)
                     //                         {
                     //                            WsfPProxyValue desiredRemove = LookupDesired(*aDesiredValue,
                     //                            typeInfoPtr->mLoadKey, (typeInfoPtr->mFlags &
                     //                            WsfParseTypeInfoData::cNESTED_LOAD) != 0, &aCurrentPath); if
                     //                            (!desiredRemove)
                     //                            {
                     //                               WsfParseTypePath loadMap = typeInfoPtr->mLoadKey;
                     //                               UtStringRef loadTypeName = loadMap.back();
                     //                               loadMap.pop_back();
                     //                               UValue currentMap = LookupCurrent(aCurrentValue, loadMap,
                     //                               (typeInfoPtr->mFlags & WsfParseTypeInfoData::cNESTED_LOAD) != 0);
                     //                               UValue currentRemove = currentMap.GetAttr(loadTypeName);
                     //                               if (!currentRemove)
                     //                               {
                     //                                  aResult = cSUCCESS;
                     //                                  return;
                     //                               }
                     //                            }
                     //                         }
                  }
                  else // No existing input
                  {
                     WsfParseTypePath loadPath = loadRulePtr->mLoadType.mPath;
                     if (loadRulePtr->mLoadType.mOrdinal != ut::npos)
                     {
                        WsfPParseRuleTrace* t = aParentTracePtr->Children()[loadRulePtr->mLoadType.mOrdinal];
                        loadPath.pop_back();
                        loadPath.push_back(t->GetText());
                     }
                     WsfPProxyValue currentLoad =
                        LookupCurrentOrBasicValue(aCurrentValue, loadPath, loadRulePtr->mLoadType.mNestedLookup);
                     if (currentLoad)
                     {
                        std::string objName = loadPath.back();
                        loadPath.pop_back();
                        UValue currentMap = LookupCurrent(aCurrentValue, loadPath, loadRulePtr->mLoadType.mNestedLookup);
                        if (currentMap.MapDelete(objName))
                        {
                           aResult = cSUCCESS;
                           if (currentMap.GetPath().IsPrefixOf(mTargetPath) &&
                               currentMap.GetPath().size() + 1 == mTargetPath.size())
                           {
                              aResult += cCHANGED_TARGET;
                           }
                        }
                        return;
                     }
                     return;
                     //                         WsfParseTypePath mapPath = loadRulePtr->mLoadType.mPath;
                     //                         std::string objName = mapPath.back();
                     //                         mapPath.pop_back();
                     //                         WsfPProxyValue desiredMap = LookupDesired(*aDesiredValue, mapPath,
                     //                         loadRulePtr->mLoadType.mNestedLookup, &aCurrentPath); if
                     //                         ((aCurrentPath.IsPrefixOf(mTargetPath) && aCurrentPath.size() + 1 ==
                     //                         mTargetPath.size())
                     //                             && (desiredMap && !desiredMap.GetAttr(mTargetPath.Back().GetMapKey())))
                     //                         {
                     //                            aResult = cSUCCESS;
                     //                            return;
                     //                            //int stop = 4;
                     // //                            bool ok = true;
                     // //                            if (loadRulePtr->mLoadType.mOrdinal != -1)
                     // //                            {
                     // //                               objName = mTargetPath[aCurrentPath.size()].mMapKey;
                     // //                            }
                     // //                            else
                     // //                            {
                     // //                               ok = (objName == mTargetPath[aCurrentPath.size()].mMapKey);
                     // //                            }
                     // //
                     // //                            if (ok)
                     // //                            {
                     // //                               UValue currentMap = LookupCurrent(aCurrentValue, mapPath,
                     // loadRulePtr->mLoadType.mNestedLookup);
                     // //                               if (currentMap)
                     // //                               {
                     // //                                  WsfPProxyValue desiredObject = desiredMap[objName];
                     // //                                  if (desiredObject)
                     // //                                  {
                     // //                                     UValue currentVal = currentMap.GetAttr(objName);
                     // //                                     if (currentVal)
                     // //                                     {
                     // //                                        aCurrentValue = currentVal;
                     // //                                        *aDesiredValue = desiredObject;
                     // //                                        aCurrentPath += objName;
                     // //                                        aResult = cSUCCESS;
                     // //                                        return;
                     // //                                     }
                     // //                                  }
                     // //                               }
                     // //                            }
                     //                         }
                  }
               }
            }
         }
         aResult = cNOT_SATISFIABLE;
         return;
      }
      break;
      case WsfParseActionPart::cLIST_OP:
      {
         WsfParseActionListOp* listOp = (WsfParseActionListOp*)act;
         if (listOp->mOperation == WsfParseActionListOp::cPUSH_BACK)
         {
            UValue         currentList    = LookupCurrent(aCurrentValue, listOp->mAttributeAddr);
            WsfPProxyList* currentListPtr = currentList.Get().GetList();
            WsfPProxyValue desiredList    = LookupDesired(*aDesiredValue, listOp->mAttributeAddr, &aCurrentPath);
            WsfPProxyList* desiredListPtr = desiredList.GetList();
            if (desiredListPtr && currentListPtr)
            {
               if (listOp->mPushedValuePtr && listOp->mPushedValuePtr->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
               {
                  // Node return values are transformed into pass by reference.
                  // So if a node return value is to be returned by value, assume it has already been added to the list
                  // TODO: We will need to support a limited set of return by value cases in order to allow special functions
                  //          that modify return values ex:
                  //                  on_ground <bool> [is_flying = not($1)]
                  aResult += cADDS_VALUE;
               }
               else
               {
                  size_t insertIndex = currentListPtr->Size();
                  size_t desiredSize = desiredListPtr->Size();
                  if (insertIndex < desiredSize)
                  {
                     currentList.ListPush();
                     if (listOp->mPushedValuePtr)
                     {
                        if (listOp->mPushedValuePtr->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
                        {
                           // TODO: DO we want to use return node list or not?
                           // WsfParseActionNodeReference* nodeRef = (WsfParseActionNodeReference*)listOp->mPushedValuePtr;
                           // if (aCurrentNodeReturns[nodeRef->mIndex].IsValid())
                           //{
                           //    pushedPtr->GetWrite().Copy(aCurrentNodeReturns[nodeRef->mIndex]);
                           // }
                        }
                        else
                        {
                           assert(false);
                        }
                     }
                     aCurrentValue  = currentList.GetAttr(insertIndex);
                     *aDesiredValue = desiredListPtr->Get(insertIndex);
                     aResult += cADDS_VALUE;
                     if (mTargetPath.IsPrefixOf(aCurrentValue.GetPath()))
                     {
                        aResult += cCHANGED_TARGET;
                     }
                  }
                  else
                  {
                     aResult = cNOT_SATISFIABLE;
                     return;
                  }
               }
            }
            else
            {
               assert(false);
            }
         }
      }
      break;
      case WsfParseActionPart::cASSIGN:
         break;
      case WsfParseActionPart::cCOPY:
         // Copy doesn't change the current value

         break;
      case WsfParseActionPart::cSKIP:
         // TODO: need to implement this correctly
         ut::log::error() << "NOT IMPL: WsfParseActionPart::SKIP,COPY";
         aResult = cNOT_SATISFIABLE;
         return;
      case WsfParseActionPart::cDEBUG_PRINT:
         break;
      default:
         assert(!"Not Impl");
      }
   }
}

WsfParseNamedRule* WsfPProxySatisfy::GetRuleOutput(WsfParseRule* aRulePtr)
{
   if (aRulePtr->IsNamedRule())
   {
      return ((WsfParseNamedRule*)aRulePtr)->mOutputTypePtr;
   }
   if (aRulePtr->GetReaderFlags() & WsfParseRule::cIS_PASSTHROUGH)
   {
      return GetRuleOutput(aRulePtr->GetPassthrough());
   }
   return nullptr;
}
//
// bool WsfPProxySatisfy::FindBindingAddress(WsfPParseRuleTrace* aTracePtr,
//                                           WsfPProxyUndoValue& aCurrentValue,
//                                           WsfPProxyValue aDesiredValue,
//                                           /*std::vector<WsfPProxyBasicValue>& aReturnValues, */
//                                           WsfParseSequence* aSeq,
//                                           int aNextSequenceChild,
//                                           WsfParseActionAddress& aBindAddr)
// {
//    for (size_t j = aNextSequenceChild; j < aTracePtr->Children().size() + 1; ++j)
//    {
//       WsfParseAction* actionPtr = aSeq->GetActionBefore(j);
//       if (actionPtr)
//       {
//          for (size_t ai = 0; ai < actionPtr->mSubActions.size(); ++ai)
//          {
//             WsfParseActionPart* actPtr = actionPtr->mSubActions[ai];
//             if (actPtr->mActionPartType == WsfParseActionPart::cASSIGN)
//             {
//                aBindAddr = ((WsfParseActionAssign*)actPtr)->mLHS_Address;
//                return true;
//             }
//             else if (actPtr->mActionPartType == WsfParseActionPart::cLIST_OP)
//             {
//                WsfParseActionListOp* op = (WsfParseActionListOp*)actPtr;
//                if (op->mOperation == WsfParseActionListOp::cPUSH_BACK)
//                {
//                   UValue currentValueList = LookupCurrent(aCurrentValue, op->mAttributeAddr);
//                   WsfPProxyList* currentValueListPtr = currentValueList.Get().GetList();
//                   WsfPProxyValue newValueList = LookupDesired(aDesiredValue, op->mAttributeAddr);
//                   if (newValueList && currentValueListPtr)
//                   {
//                      int nextIndex = currentValueListPtr->Size();
//                      aBindAddr = op->mAttributeAddr;
//                      aBindAddr.mAttrIndices.push_back(WsfParseActionAddress::Entry(nextIndex));
//                      return true;
//                   }
//                }
//             }
//          }
//       }
//    }
//    return false;
// }


bool WsfPProxySatisfy::FindBindingAddress2(WsfPParseRuleTrace*               aTracePtr,
                                           WsfPProxyUndoValue&               aCurrentValue,
                                           WsfPProxyValue                    aDesiredValue,
                                           std::vector<WsfPProxyBasicValue>& aReturnValues,
                                           WsfParseSequence*                 aSeq,
                                           size_t                            aNextSequenceChild,
                                           WsfPProxyPath&                    aBindAddr)
{
   for (size_t j = aNextSequenceChild; j < aTracePtr->Children().size() + 1; ++j)
   {
      WsfParseAction* actionPtr = aSeq->GetActionBefore(j);
      if (actionPtr)
      {
         // Scan the actions to see if they can contain a binding
         bool hasSomeBinding = false;
         for (size_t ai = 0; ai < actionPtr->mSubActions.size(); ++ai)
         {
            WsfParseActionPart* actPtr = actionPtr->mSubActions[ai];
            switch (actPtr->mActionPartType)
            {
            case WsfParseActionPart::cASSIGN:
            {
               WsfParseActionAssign* assignActionPtr = ((WsfParseActionAssign*)actPtr);
               if (assignActionPtr->mRHS->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
               {
                  hasSomeBinding = true;
               }
            }
            default:
               break;
            }
         }
         if (!hasSomeBinding)
         {
            continue;
         }

         // If a binding does exist, we need to process all actions to arrive at the binding address
         WsfPProxyUndoValue current = aCurrentValue;
         WsfPProxyValue     desired = aDesiredValue;
         for (size_t ai = 0; ai < actionPtr->mSubActions.size(); ++ai)
         {
            WsfParseActionPart* actPtr = actionPtr->mSubActions[ai];
            switch (actPtr->mActionPartType)
            {
            case WsfParseActionPart::cASSIGN:
            {
               WsfParseActionAssign* assignActionPtr = ((WsfParseActionAssign*)actPtr);
               if (assignActionPtr->mRHS->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
               {
                  WsfParseActionNodeReference* nodeRef = (WsfParseActionNodeReference*)assignActionPtr->mRHS;
                  if (1 + nodeRef->mIndex == aNextSequenceChild)
                  {
                     WsfPProxyPath path = current.GetPath();
                     /*WsfPProxyValue val = */ LookupDesired(desired, assignActionPtr->mLHS_Address, &path);
                     aBindAddr = path;
                     return true;
                  }
               }
            }
            break;
            case WsfParseActionPart::cLIST_OP:
            {
               WsfParseActionListOp* op = (WsfParseActionListOp*)actPtr;
               if (op->mOperation == WsfParseActionListOp::cPUSH_BACK)
               {
                  current = LookupCurrent(current, op->mAttributeAddr);
                  // UValue currentValueList = LookupCurrent(aCurrentValue, op->mAttributeAddr);
                  WsfPProxyList* currentValueListPtr = current.Get().GetList();
                  WsfPProxyValue newValueList        = LookupDesired(desired, op->mAttributeAddr);
                  if (newValueList && currentValueListPtr)
                  {
                     size_t nextIndex = currentValueListPtr->Size();
                     current.MoveToAttr(nextIndex);
                     desired = newValueList;
                     desired = desired.GetAttr(nextIndex);
                  }
               }
            }
            break;
            case WsfParseActionPart::cOBJECT_MAP_OP:
            {
               WsfParseActionObjectMapOp* mapOp = (WsfParseActionObjectMapOp*)actPtr;
               if (mapOp->mOperation == WsfParseActionObjectMapOp::cNEW ||
                   mapOp->mOperation == WsfParseActionObjectMapOp::cMAP_SET)
               {
                  // prefixAddr += mapOp->mAttributeAddr;
                  std::string objName = "";
                  if (mapOp->mObjectName->mActionPartType == WsfParseActionPart::cNODE_REFERENCE)
                  {
                     // Find an entry in the map which does not already exist
                     WsfPProxyUndoValue currentMap = LookupCurrent(current, mapOp->mAttributeAddr);
                     WsfPProxyValue     desiredMap = LookupDesired(desired, mapOp->mAttributeAddr);
                     if (currentMap && desiredMap)
                     {
                        current = currentMap;
                        desired = desiredMap;
                        // The object name should have already been decided
                        WsfParseActionNodeReference* nodeRef         = (WsfParseActionNodeReference*)mapOp->mObjectName;
                        WsfPProxyValue               objectNameValue = aReturnValues[nodeRef->mIndex];
                        if (objectNameValue)
                        {
                           std::string objectName = WsfPProxyBasicValue(objectNameValue).ToString();
                           current.MoveToAttr(objectName);
                           desired = desired.GetAttr(objectName);
                        }
                     }
                  }
               }
            }
            break;
            case WsfParseActionPart::cLITERAL:
            case WsfParseActionPart::cNODE_REFERENCE:
            case WsfParseActionPart::cNEGATE:
            case WsfParseActionPart::cPUSH:
            case WsfParseActionPart::cDEBUG_PRINT:
            case WsfParseActionPart::cSKIP:
            case WsfParseActionPart::cCOPY:
            case WsfParseActionPart::cCALL:
               break;
            }
         }
      }
   }
   return false;
}

static void CleanupSatisfySequence(std::vector<WsfPProxyValue*>&            aVariables,
                                   std::vector<WsfPProxySatisfy::Equation>& aEquations)
{
   for (size_t i = 0; i < aVariables.size(); ++i)
   {
      if (aVariables[i])
      {
         aVariables[i]->Delete();
         delete aVariables[i];
      }
   }
   for (size_t i = 0; i < aEquations.size(); ++i)
   {
      aEquations[i].Cleanup();
   }
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfySequence(SatPoint pt)
{
   // 1. Create a set of equations given the actions for the rule sequence
   //    Variables in the equations are the values of child nodes,
   //       i.e. the rule:     position <Latitude> <Longitude>    [waypointType=1;lat=$1;lon=$2]
   //       has 2 variables, <Latitude> and <Longitude>
   // 2. Attempt to solve the equations.  i.e, try to find a set of inputs which satisfy the desired value.
   //       For instance, $1="1.0n", $2="2.0e"
   // 3. Step through each action block and each child rule.
   //    If a child fails to satisfy, report failure.
   //    Because we don't have nodes, don't execute actions which need to reference nodes.  Instead, directly, just
   //    assign the desired value to the current value.

   // If the sequence fails, the proxy change set needs rolled back to undo changes.

   // Sequence rules are the only rules which allow actions.  Here we need to make sure that the actions
   // taken will make progress toward changing the current proxy value to the desired one.
   WsfParseSequence* seq = (WsfParseSequence*)pt.trace->ActualRule();

#ifdef PROXY_SERIALIZE_TRACE
   WsfParseNode* debugNODE             = pt.trace->GetNearestNode();
   std::string   debugRULE_DESCRIPTION = seq->GetRuleDescription();
   if (debugRULE_DESCRIPTION == "{ platform <string> <string> <...> <Platform.instance-command>* end_platform }")
   {
      int stop = 1;
   }
   { // RAII block
      auto logger = ut::log::debug() << "Satisfy rule.";
      logger.AddNote() << "Rule Description: " << debugRULE_DESCRIPTION;
   }
#endif

   std::vector<Equation> eqs;

   std::vector<WsfPProxyValue*> variables(seq->Sequence().size());
   bool validEquationList = BuildSequenceEquations(seq, pt.current, pt.desired, eqs, pt.trace->mNodePtr, variables);
   // WsfParseActionAddress prefixAddr;
   bool solved = validEquationList && SolveEquations(pt.desired, /*prefixAddr, */ eqs, variables);
   if (solved)
   {
   }
   if (!solved)
   {
      CleanupSatisfySequence(variables, eqs);
      return cNOT_SATISFIABLE;
   }


   if (pt.trace->mNodePtr == nullptr && pt.trace->Children().empty())
   {
      for (size_t i = 0; i < seq->Sequence().size(); ++i)
      {
         WsfParseRule* subPtr = seq->Sequence()[i];
         pt.trace->AddChild(subPtr);
      }
   }

   size_t nextEquation = 0;

   WsfParseAction* action = nullptr;
   SolveResult     actionResult;
   SatisfyResult   satResult;
   WsfPProxyPath   currentPath  = pt.current.GetPath();
   UValue          currentValue = pt.current;
   // WsfPProxyChangeSet* newValue = &aCurrentDesired;
   WsfPProxyValue desiredValue = pt.desired;

   bool                             childFail = false;
   std::vector<WsfPProxyBasicValue> nodesReturnValues;
   action = seq->GetActionBefore(0);
   if (action)
   {
      SatisfyResult actionResult;
      FollowAction(action, pt.trace, currentValue, &desiredValue, currentPath, variables, actionResult);
      if (!actionResult)
      {
         CleanupSatisfySequence(variables, eqs);
         ;
         return actionResult;
      }
      satResult += actionResult;
      int applyFlags = ApplyEquations(0, nextEquation, eqs, pt.current, pt.desired);
      satResult.mResult |= applyFlags;
   }

   for (size_t i = 0; i < pt.trace->Children().size(); ++i)
   {
      WsfPProxyValue      returnValue;
      WsfPParseRuleTrace* childPtr      = pt.trace->Children()[i];
      WsfParseNamedRule*  outputTypePtr = GetRuleOutput(childPtr->ActualRule());
      if (seq && variables[i])
      {
         returnValue = *variables[i];
      }
      else if (!seq)
      {
         returnValue = pt.returnValue;
      }
      SatisfyResult childSat;
      if (outputTypePtr)
      {
         // this sub-rule returns a value and does not operate on the current value
         // to allow correct processing, we want to figure out if the returned value is
         // assigned anywhere
         // WsfParseActionAddress   assignAddr;
         WsfPProxyPath assignAddr;

         // if (FindBindingAddress(pt.trace, currentValue, desiredValue, seq, i+1, assignAddr))
         if (FindBindingAddress2(pt.trace, currentValue, desiredValue, nodesReturnValues, seq, i + 1, assignAddr))
         {
            // if (! assignAddr.IsEmpty())
            {
               WsfPProxyValue desiredSub = mDesiredRoot.Lookup(assignAddr);
               // WsfPProxyValue desiredSub = LookupDesired(desiredValue, assignAddr);
               if (!desiredSub)
               {
                  // This rule produces changes not consistent with the desired value; abort
                  CleanupSatisfySequence(variables, eqs);
                  return cNOT_SATISFIABLE;
               }
               else
               {
                  UValue curSub = mCurrentRoot.GetAttr(assignAddr);

                  // WsfPProxyChangeSet* curSub = outputProxyPtr->ConstructNew();
                  SatPoint newPt(curSub, desiredSub, returnValue, childPtr);
                  childSat = Satisfy(newPt);
                  // nodesReturnValues.push_back(curSub);
               }
            }
            // else
            //{
            ////TODO: if an input value is unrestricted, what do we do?
            // int stop=5;
            // }
         }
         else
         {
            // TODO: This could either be an error or a rule with no actions?
            childSat = cNOT_SATISFIABLE;
         }
      }
      else
      {
         SatPoint newPt(currentValue, desiredValue, returnValue, childPtr, pt.flags);
         if (satResult.TargetComplete())
         {
            newPt.flags |= SatPoint::cTARGET_COMPLETE;
         }
         childSat = Satisfy(newPt);
         nodesReturnValues.push_back(WsfPProxyValue());
      }
      currentValue = pt.current;
      desiredValue = pt.desired;
      currentPath  = pt.current.GetPath();

      action = seq->GetActionBefore(i + 1);
      if (action)
      {
         SatisfyResult actionResult;
         FollowAction(action, pt.trace, currentValue, &desiredValue, currentPath, variables, actionResult);
         if (!actionResult)
         {
            CleanupSatisfySequence(variables, eqs);
            return actionResult;
         }
         satResult += actionResult;
         int applyFlags = ApplyEquations(i + 1, nextEquation, eqs, pt.current, pt.desired);
         satResult.mResult |= applyFlags;
      }

      if (!childSat)
      {
         satResult = cNOT_SATISFIABLE;
         childFail = true;
         break;
      }
      satResult += childSat;
      if (mSatMode == cSM_TARGET && satResult.TargetComplete())
      {
         pt.SetTargetComplete(true);
      }
   }
   if (!childFail)
   {
      satResult += cSATISFIABLE;
   }
   CleanupSatisfySequence(variables, eqs);
   return satResult;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyAlternate(SatPoint pt)
{
   WsfParseAlternate* alt = (WsfParseAlternate*)pt.trace->ActualRule();

   // If the trace picks an alternate, make sure that is satisfiable and return.
   // Otherwise, Pick the first alternate which is Successful and return
   // Otherwise, Pick the first alternate which is Satisfiable and return
   // Otherwise fail.
   WsfParseRule* tracedAlternate = nullptr;
   if (pt.trace->Children().size() == 1)
   {
      tracedAlternate      = pt.trace->Children()[0]->Rule();
      SatisfyResult result = Satisfy(SatPoint(pt, pt.trace->Children()[0]));
      // TODO:
      // return result;
      if (result)
      {
         return result;
      }
      pt.trace->Children()[0]->SetPruned();
   }


   std::vector<WsfParseRule*> alternateChoices;
   if (pt.current.GetPath().IsPrefixOf(mTargetPath) && pt.current.GetPath().size() < mTargetPath.size())
   {
      const WsfPProxyKey& key = mTargetPath[pt.current.GetPath().size()];
      if (key.GetIndex() >= 0)
      {
         mActionLookupPtr->GetRulesThatModifyAttribute(alt, key.GetIndex(), alternateChoices);
      }
   }

   // if the two paths are the same size, check all the alternate choices...
   else if (pt.current.GetPath().IsPrefixOf(mTargetPath) && pt.current.GetPath().size() == mTargetPath.size())
   {
      for (auto altIter = alt->Alternates().cbegin(); altIter != alt->Alternates().cend(); ++altIter)
      {
         alternateChoices.push_back(*altIter);
      }
   }

   WsfParseRule* satSubPtr = nullptr;
   for (size_t i = 0; i < alternateChoices.size(); ++i)
   {
      WsfParseRule* altPtr = alternateChoices[i];
      if (altPtr == tracedAlternate)
      {
         continue;
      } // already tried
      WsfPParseRuleTrace* subTrace = pt.trace->AddChild(altPtr);
      subTrace->SetRule(altPtr);
      // UValue newChanges = new WsfPProxyChangeSet(aCurrentValue);
      size_t        rollbackIndex = pt.current.GetHistorySize();
      SatisfyResult result        = Satisfy(SatPoint(pt, subTrace));
      if (result.TargetComplete())
      {
         return result;
      }
      pt.current.Rollback(rollbackIndex);
      if (result && !satSubPtr)
      {
         satSubPtr = altPtr;
      }
      assert(pt.trace->LastChild() == subTrace);
      delete pt.trace->PopChild();
   }
   if (pt.NeedToComplete() || alt->Alternates().size() == 1)
   {
      const std::vector<WsfParseRule*>& alts = alt->Alternates();
      for (size_t a = 0; a < alts.size(); ++a)
      {
         WsfParseRule* altPtr = alts[a];
         if (std::find(alternateChoices.begin(), alternateChoices.end(), altPtr) != alternateChoices.end())
         {
            continue;
         }
         if (altPtr == tracedAlternate)
         {
            continue;
         } // already tried
         WsfPParseRuleTrace* subTrace = pt.trace->AddChild(altPtr);
         subTrace->SetRule(altPtr);
         // UValue newChanges = new WsfPProxyChangeSet(aCurrentValue);
         size_t        rollbackIndex = pt.current.GetHistorySize();
         SatisfyResult result        = Satisfy(SatPoint(pt, subTrace));
         if (result && (result.TargetComplete() || pt.IsTargetComplete()))
         {
            return result;
         }
         pt.current.Rollback(rollbackIndex);
         assert(pt.trace->LastChild() == subTrace);
         delete pt.trace->PopChild();
         if (result && !satSubPtr)
         {
            satSubPtr = altPtr;
         }
      }
   }
   if (satSubPtr)
   {
      SatisfyResult result = Satisfy(SatPoint(pt, pt.trace->AddChild(satSubPtr)));
      assert(result);
      return result;
   }
   return cNOT_SATISFIABLE;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyValueRule(WsfPProxyBasicValue aDesiredValue,
                                                                   WsfPParseRuleTrace* aTracePtr)
{
   WsfParseRule* currentRulePtr = aTracePtr->ActualRule();
   assert(currentRulePtr);
   // assert(currentRulePtr->Type() == WsfParseRule::cVALUE);
   if (aDesiredValue.IsUnset())
   {
      return cNOT_SATISFIABLE;
   }
   WsfPProxyBasicValue rVal(aDesiredValue);
   WsfPProxyBasicValue currentVal   = rVal.Copy();
   WsfParseNode*       valueNodePtr = aTracePtr->GetNearestNode();
   if (rVal)
   {
      WsfPProxyBasicType* typePtr = rVal.GetType();
      if (valueNodePtr)
      {
         currentVal.Read(aTracePtr->GetNearestNode());
         if (!IsEqual(currentVal, rVal))
         {
            std::string text = typePtr->Write(rVal.GetDataPtr(), currentRulePtr);
            // std::string text = WsfPProxyBasicValue(aDesiredValue).ToString();
            aTracePtr->SetInsertText(text);
         }
      }
      else
      {
         // aTracePtr->SetInsertText(aDesiredValue.ToString());
         aTracePtr->SetInsertText(typePtr->Write(rVal.GetDataPtr(), currentRulePtr));
      }
   }
   currentVal.Delete();
   return cSATISFIABLE;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyRoot(SatPoint pt)
{
   WsfParseRule* rootRulePtr = mRegistryPtr->mEntryPointRule;
   SatisfyResult rootResult;
   for (;;)
   {
      WsfPParseRuleTrace* subPtr = pt.trace->AddChild(rootRulePtr);
      WsfPProxyPath       rootPath;
      SatisfyResult       result = Satisfy(SatPoint(pt, subPtr));
      if (result.Success())
      {
         rootResult += result;
      }
      else
      {
         delete pt.trace->PopChild();
         break;
      }
      // TODO: Allow looping here!
      break;
   }
   return rootResult;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyRecurrence(SatPoint pt)
{
   //          // Don't recurse to platform.A.sensor when trying to change platform.A.side
   //          if (! pt.current.GetPath().IsPrefixOf(mTargetPath))
   //          {
   //             return cSATISFIABLE;
   //          }

   size_t              childNodeCount = 0;
   WsfParseRecurrence* rec            = (WsfParseRecurrence*)pt.trace->ActualRule();
   bool isBlock = (rec->mCount == WsfParseRecurrence::cONE_TO_MANY || rec->mCount == WsfParseRecurrence::cZERO_TO_MANY);
   bool descend = true;

   // Return early if target is complete AND this is a block (blocks can be changed very easily later
   if (pt.IsTargetComplete() && isBlock)
   //(rec->mCount == WsfParseRecurrence::cZERO_OR_ONE || rec->mCount == WsfParseRecurrence::cZERO_TO_MANY))
   {
      descend = false;
   }
   SatisfyResult recurrenceResult = cSATISFIABLE;

   if (descend)
   {
      size_t minChildren, maxChildren;
      rec->GetCountRange(minChildren, maxChildren);

      if (FixMode())
      {
         // Check existing trace children for satisfiability.
         // While the child rule is successful, add to trace
         for (size_t i = 0; i < pt.trace->Children().size(); ++i)
         {
            WsfPParseRuleTrace* subPtr = pt.trace->Children()[i];
            SatisfyResult       result = Satisfy(SatPoint(pt, subPtr));
            if (!result)
            {
               subPtr->SetPruned();
            }
            else
            {
               ++childNodeCount;
               recurrenceResult += result;
            }
         }
         if (!(childNodeCount >= minChildren && childNodeCount <= maxChildren))
         {
            pt.trace->SetPruned();
            return cNOT_SATISFIABLE;
         }
         return cSATISFIABLE;
      }


      bool done = (isBlock && recurrenceResult.TargetComplete());
      for (; !done && childNodeCount < maxChildren;)
      {
         size_t insertIndex = ut::npos;
         if (mRecurrenceInsertBeforeEnd && pt.trace->Children().size() > 0)
         {
            mRecurrenceInsertBeforeEnd = false;
            insertIndex                = pt.trace->Children().size() - 1;
         }
         WsfPParseRuleTrace* subPtr        = pt.trace->AddChild(rec->GetSubordinateRule(), insertIndex);
         size_t              rollbackIndex = pt.current.GetHistorySize();
         SatisfyResult       result        = Satisfy(SatPoint(pt, subPtr));

         bool accept = false;
         if (!isBlock)
         {
            if (childNodeCount < minChildren)
            {
               accept = result;
            }
            else
            {
               accept = result.Success();
            }
         }
         else
         {
            if (childNodeCount < minChildren)
            {
               accept = result;
            }
            else
            {
               accept = result.TargetComplete();
               done   = true;
            }
         }

         if (accept)
         {
            ++childNodeCount;
            recurrenceResult += result;
         }
         else
         {
            pt.current.Rollback(rollbackIndex);
            delete pt.trace->PopChild(insertIndex);
            break;
         }
      }
      if (childNodeCount < minChildren || childNodeCount > maxChildren)
      {
         return cNOT_SATISFIABLE;
      }
   }

   // New commands can be added to this block later:
   if (isBlock && recurrenceResult && pt.trace->GetContext() == nullptr)
   {
      pt.trace->NewContext(pt.current.GetPath());
   }

   return recurrenceResult;
}

WsfPProxySatisfy::TypeLoadLookupResult WsfPProxySatisfy::LookupTypeLoad(WsfParseTypeInfoData::Operation aOperation,
                                                                        bool                            aIsSaveType,
                                                                        const WsfParseTypeName&         aType,
                                                                        WsfPProxyUndoValue&             aCurrent,
                                                                        WsfPProxyValue&                 aDesired,
                                                                        std::string&                    aObjectName)
{
   bool             isSaveOperation  = aOperation == WsfParseTypeInfoData::cCREATE_TYPE;
   bool             mustBeTarget     = (isSaveOperation == aIsSaveType);
   WsfParseTypePath mapPath          = aType.mPath;
   bool             basicTypeLoading = false;
   if (!aIsSaveType)
   {
      WsfPProxyValue basicValue = LookupBasicValue(mapPath);
      if (basicValue)
      {
         basicTypeLoading = true;
      }
   }
   aObjectName = mapPath.back();
   mapPath.pop_back();

   if (basicTypeLoading)
   {
      return cTL_SUCCESS;
   }

   WsfPProxyPath  currentPath = aCurrent.GetPath();
   WsfPProxyValue desiredMap  = LookupDesired(aDesired, mapPath, aType.mNestedLookup, &currentPath);
   // ERROR: Here desiredMap is a struct!
   if ((!mustBeTarget || (currentPath.IsPrefixOf(mTargetPath) && currentPath.size() < mTargetPath.size())) &&
       (desiredMap && desiredMap.GetObjectMap()))
   {
      bool ok = true;
      if (aType.mOrdinal != ut::npos)
      {
         if (mustBeTarget)
         {
            aObjectName = mTargetPath[currentPath.size()].GetMapKey();
         }
         else
         {
            bool                 foundBase   = false;
            WsfPProxyStructValue desiredSave = mDesiredRoot.Lookup(mTargetPath);
            if (desiredSave)
            {
               WsfPProxyPath* basePath = desiredSave.GetBase();
               if (basePath && !basePath->Empty())
               {
                  aObjectName = basePath->Back().GetMapKey();
                  foundBase   = true;
               }
            }
            if (!foundBase)
            {
               return cTL_FAILURE;
            }
         }
      }
      else
      {
         ok = !mustBeTarget || (aType.mPath.back() == aObjectName);
      }

      if (aOperation == WsfParseTypeInfoData::cDELETE_TYPE)
      {
         if (!aObjectName.empty())
         {
            /*   There are two casese where we want to delete a type:
               1. The type isn't in the desired map
               2. The type in the desired map doesn't match the current one
            */
            UValue         currentMap   = LookupCurrent(aCurrent, mapPath, aType.mNestedLookup);
            WsfPProxyValue desiredValue = mDesiredRoot.Lookup(mTargetPath);
            UValue         currentVal   = currentMap.GetAttr(aObjectName);
            if (currentVal.Exists() && !desiredValue)
            {
               return cTL_SUCCESS;
            }
            if (currentVal.Exists() && desiredValue)
            {
               if (currentVal.Get().GetType() != desiredValue.GetType())
               {
                  return cTL_SUCCESS;
               }
            }
         }
         return cTL_FAILURE;
      }

      if (!aIsSaveType)
      {
         WsfPProxyValue basicValue = LookupBasicValue(mapPath);
         if (basicValue)
         {
            return cTL_SUCCESS;
         }
      }

      if (ok)
      {
         UValue currentMap = LookupCurrent(aCurrent, mapPath, aType.mNestedLookup);
         if (currentMap)
         {
            WsfPProxyValue desiredObject = desiredMap[aObjectName];
            if (desiredObject)
            {
               if (aIsSaveType)
               {
                  if (currentMap.GetAttr(aObjectName).Get())
                  {
                     return cTL_COLLISION;
                  }
               }
               else
               {
                  if (!currentMap.GetAttr(aObjectName).Get())
                  {
                     return cTL_LOADFAIL;
                  }
               }
               return cTL_SUCCESS;
            }
         }
      }
   }
   return cTL_FAILURE;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::SatisfyTypeLoad(WsfParseRule* currentRulePtr, SatPoint pt)
{
   WsfParseTypeLoadRule* loadPtr = (WsfParseTypeLoadRule*)currentRulePtr;
   if (pt.trace->mNodePtr)
   {
      return cSUCCESS;
   }
   else
   {
      if (loadPtr->mParentRulePtr && loadPtr->mParentRulePtr->Type() == WsfParseRule::cSEQUENCE)
      {
         WsfParseSequence* seq       = (WsfParseSequence*)loadPtr->mParentRulePtr;
         size_t            idx       = seq->FindChildIndex(loadPtr);
         WsfParseAction*   actionPtr = seq->GetActionBefore(idx + 1);
         bool              isApplied = false;
         if (actionPtr)
         {
            for (size_t i = 0; i < actionPtr->mSubActions.size(); ++i)
            {
               if (actionPtr->mSubActions[i]->mActionPartType == WsfParseActionPart::cOBJECT_MAP_OP)
               {
                  if (((WsfParseActionObjectMapOp*)actionPtr->mSubActions[i])->mOperation ==
                      WsfParseActionObjectMapOp::cAPPLY)
                  {
                     isApplied = true;
                  }
               }
            }
         }
         if (isApplied)
         {
            if (loadPtr->GetOperation() == WsfParseTypeInfoData::cLOAD_TYPE)
            {
               WsfPProxyUndoValue current = pt.current;
               WsfPProxyValue     desired = pt.desired;
               std::string        objName;
               if (cTL_SUCCESS ==
                   LookupTypeLoad(loadPtr->GetOperation(), false, loadPtr->mLoadType, current, desired, objName))
               {
                  if (loadPtr->mLoadType.mOrdinal != ut::npos)
                  {
                     pt.trace->GetParent()->Children()[loadPtr->mLoadType.mOrdinal]->SetInsertText(objName);
                  }
                  return cSATISFIABLE;
               }
            }
            else if (loadPtr->GetOperation() == WsfParseTypeInfoData::cCREATE_TYPE)
            {
               WsfPProxyUndoValue   currentLoad = pt.current;
               WsfPProxyValue       desiredLoad = pt.desired;
               std::string          loadObjectName;
               TypeLoadLookupResult loadResult =
                  LookupTypeLoad(loadPtr->GetOperation(), false, loadPtr->mLoadType, currentLoad, desiredLoad, loadObjectName);
               if (loadResult == cTL_SUCCESS)
               {
                  WsfPProxyUndoValue   currentSave = pt.current;
                  WsfPProxyValue       desiredSave = pt.desired;
                  std::string          saveObjectName;
                  TypeLoadLookupResult saveResult =
                     LookupTypeLoad(loadPtr->GetOperation(), true, loadPtr->mSaveType, currentSave, desiredSave, saveObjectName);
                  if (saveResult == cTL_SUCCESS || (saveResult == cTL_COLLISION && loadPtr->mAllowRedefinition))
                  {
                     if (loadPtr->mLoadType.mOrdinal != ut::npos)
                     {
                        pt.trace->GetParent()->Children()[loadPtr->mLoadType.mOrdinal]->SetInsertText(loadObjectName);
                     }
                     if (loadPtr->mSaveType.mOrdinal != ut::npos)
                     {
                        pt.trace->GetParent()->Children()[loadPtr->mSaveType.mOrdinal]->SetInsertText(saveObjectName);
                     }
                     return cSATISFIABLE;
                  }
               }
            }
            else if (loadPtr->GetOperation() == WsfParseTypeInfoData::cDELETE_TYPE)
            {
               WsfPProxyUndoValue   currentLoad = pt.current;
               WsfPProxyValue       desiredLoad = pt.desired;
               std::string          loadObjectName;
               TypeLoadLookupResult loadResult =
                  LookupTypeLoad(loadPtr->GetOperation(), false, loadPtr->mLoadType, currentLoad, desiredLoad, loadObjectName);
               if (loadResult == cTL_SUCCESS)
               {
                  if (loadPtr->mLoadType.mOrdinal != ut::npos)
                  {
                     pt.trace->GetParent()->Children()[loadPtr->mLoadType.mOrdinal]->SetInsertText(loadObjectName);
                  }
                  return cSATISFIABLE;
               }
            }
         }
         else
         {
            return cSATISFIABLE; // not applied; ignore
         }
      }
   }
   return cNOT_SATISFIABLE;
}

WsfPProxySatisfy::SatisfyResult WsfPProxySatisfy::Satisfy(SatPoint pt)
{
   if ((pt.trace->mNodePtr && pt.trace->mNodePtr->mRulePtr == nullptr))
   {
      return SatisfyRoot(pt);
   }

   WsfParseRule* currentRulePtr = pt.trace->ActualRule();

   // TODO: Some rules say they are passthrough rules, but they are still creating
   //       nodes.  This block should be unnecessary:
   if (currentRulePtr != pt.trace->Rule() && pt.trace->Children().size() == 1)
   {
      bool isSimpleValue =
         (currentRulePtr->Type() == WsfParseRule::cVALUE && !((WsfParseValue*)currentRulePtr)->mIsEnumeration);
      if (pt.trace->Children()[0]->Rule() == currentRulePtr && !isSimpleValue)
      {
         return Satisfy(SatPoint(pt, pt.trace->Children()[0]));
      }
   }

   assert(currentRulePtr);
   switch (currentRulePtr->Type())
   {
   case WsfParseRule::cINT:
   case WsfParseRule::cSTRING:
   case WsfParseRule::cREAL:
   {
      if (pt.returnValue)
      {
         if (pt.returnValue.IsUnset())
         {
            return cNOT_SATISFIABLE;
         }
         WsfPProxyBasicValue basicValue(pt.returnValue);
         assert(basicValue);
         std::string text = basicValue.ToString();
         assert(!text.empty());
         pt.trace->SetInsertText(text);
      }
      return cSATISFIABLE;
   }
   break;
   case WsfParseRule::cLITERAL:
   {
      if (pt.returnValue && pt.returnValue.IsBasicType())
      {
         WsfParseLiteral* lit = (WsfParseLiteral*)currentRulePtr;
         if (!lit->MatchesString(WsfPProxyBasicValue(pt.returnValue).ToString()))
         {
            return cNOT_SATISFIABLE;
         }
      }
      return cSATISFIABLE;
   }
   break;
   case WsfParseRule::cVALUE:
   {
      WsfParseValue* curValRule = (WsfParseValue*)currentRulePtr;
      if (curValRule->mIsEnumeration && pt.trace->Children().empty())
      {
         return Satisfy(SatPoint(pt, pt.trace->AddChild(curValRule->GetSubordinateRule())));
         // return Satisfy(aCurrentValue, aCurrentDesired, aReturnValue, pt.trace->Children()[0]);
         // curValRule->GetSubordinateRule();
      }
      if (!pt.returnValue && pt.current)
      {
         // This is questionable
         if (!pt.desired.IsBasicType())
         {
            return cNOT_SATISFIABLE;
         }
         SatisfyResult r = SatisfyValueRule(pt.desired, pt.trace);
         // if (r)
         //{
         // aCurrentValue->GetWrite().Copy(aCurrentDesired);
         // r.mResult |= cADDS_VALUE;
         // }
         return r;
      }
      else
      {
         return SatisfyValueRule(pt.returnValue, pt.trace);
      }
   }
   break;
   case WsfParseRule::cSEQUENCE:
   {
      size_t        rollbackCheckpoint = pt.current.GetHistorySize();
      SatisfyResult result             = SatisfySequence(pt);
      if (result)
      {
         // std::cerr << "SAT\n";
         return result;
      }
      else
      {
         // std::cerr << "Sequence not sat, delete changes: \n";
         pt.current.Rollback(rollbackCheckpoint);
         return result;
      }
   }
   break;
   case WsfParseRule::cALTERNATE:
   {
      return SatisfyAlternate(pt);
   }
   break;
   case WsfParseRule::cRECURRENCE:
   {
      return SatisfyRecurrence(pt);
   }
   break;
   case WsfParseRule::cSTRUCT:
   case WsfParseRule::cNAMED_RULE:
   {
      WsfParseNamedRule* namedPtr = (WsfParseNamedRule*)pt.trace->ActualRule();

      if (namedPtr->IsPartValueRule() && pt.returnValue)
      {
         WsfParseValue* valueRulePtr = namedPtr->GetRuleValue();
         if (!valueRulePtr->mIsEnumeration)
         {
            return SatisfyValueRule(pt.returnValue, pt.trace);
         }
      }

      WsfPParseRuleTrace* subTracePtr = pt.trace->AddChild(namedPtr->GetSubordinateRule());
      SatisfyResult       result      = Satisfy(SatPoint(pt, subTracePtr));
      if (!result)
      {
         WsfPParseRuleTrace* subTracePtr2 = pt.trace->PopChild();
         assert(subTracePtr2 == subTracePtr);
         delete subTracePtr2;
      }
      return result;
   }
   break;
   case WsfParseRule::cTYPE_LOAD:
   {
      return SatisfyTypeLoad(currentRulePtr, pt);
   }
   case WsfParseRule::cTYPE_COMMAND:
   {
      const WsfPProxyType* curTypePtr = pt.current.Get().GetType();
      if (curTypePtr && curTypePtr->IsStruct())
      {
         WsfPProxyStructType* curStructPtr   = (WsfPProxyStructType*)curTypePtr;
         WsfParseStruct*      parseStructPtr = curStructPtr->GetParseStruct();
         if (parseStructPtr)
         {
            WsfPParseRuleTrace* childTrace = pt.trace->AddChild(parseStructPtr);
            return Satisfy(SatPoint(pt, childTrace));
         }
      }
      return cNOT_SATISFIABLE;
   }
   default:
      return cNOT_SATISFIABLE;
   }
}

bool WsfPProxySatisfy::SatisfyRootDifferences()
{
   //    WsfPParseRuleTrace* tracePtr = mTracePtr->AddChild(mRegistryPtr->mEntryPointRule);
   //    tracePtr->NewContext(WsfPProxyPath());
   //    return SatisfyBlock(tracePtr->GetContext());
   for (int iteration = 0; iteration < 10; ++iteration)
   {
      WsfPProxyDiff diff;
      bool          hasChanged = false;
      diff.mDiffs.clear();
      // it is hard to follow what was going on here.  These debug blocks will output the desiredroot and
      // each iterations actual root, and then also report on the changes that are made
#ifdef DEBUG_CHANGES
      if (iteration == 0)
      {
         std::string   dname = "it_" + std::to_string(iteration) + "_desired.txt";
         std::ofstream desiredStream;
         desiredStream.open(dname);
         if (desiredStream.is_open())
         {
            mDesiredRoot.DebugOut(desiredStream);
         }
         desiredStream.flush();
         desiredStream.close();
      }
      std::string   cname = "it_" + std::to_string(iteration) + "_current.txt";
      std::ofstream currentStream;
      currentStream.open(cname);
      if (currentStream.is_open())
      {
         mCurrentRoot.Get().DebugOut(currentStream);
      }
      currentStream.flush();
      currentStream.close();
#endif
      diff.Diff(mCurrentRoot.Get(), mDesiredRoot, WsfPProxyPath());

      for (size_t i = 0; i < diff.mDiffs.size(); ++i)
      {
         if (diff.mDiffs[i].mType == WsfPProxyDiff::cCHANGED || diff.mDiffs[i].mType == WsfPProxyDiff::cADDED ||
             diff.mDiffs[i].mType == WsfPProxyDiff::cREMOVED)
         {
#ifdef DEBUG_CHANGES
            auto logger = ut::log::debug() << "Iteration: " << iteration;
            logger.AddNote() << "Path: " << diff.mDiffs[i].mPath.ToString(mDesiredRoot);
#endif
            bool change = SatisfyAttribute(diff.mDiffs[i].mPath);
            hasChanged |= change;
#ifdef DEBUG_CHANGES
            std::string debugChanges;
            if (!change)
            {
               debugChanges = "couldn't ";
            }
            switch (diff.mDiffs[i].mType)
            {
            case (WsfPProxyDiff::cCHANGED):
               debugChanges += "change";
               break;
            case (WsfPProxyDiff::cADDED):
               debugChanges += "add";
               break;
            case (WsfPProxyDiff::cREMOVED):
               debugChanges += "remove";
               break;
            }
            logger.AddNote() << debugChanges;
#endif
         }
      }
      if (!hasChanged)
      {
         break;
      }
   }
   return true;
}

bool WsfPProxySatisfy::SatisfyBlock(WsfPParseRuleTraceContext* aContextPtr, WsfPProxyValue aExistingValues)
{
   bool hasChanged = SatisfyBlockP(aContextPtr, aExistingValues);
   for (int i = 0; i < 10 && hasChanged; ++i)
   {
      if (!SatisfyBlockP(aContextPtr, aExistingValues))
      {
         break;
      }
   }
   return hasChanged;
}

// Satisfy a proxy value by adding new inputs to a specific block
// aExistingValues is the state of the proxy after deserializing the unchanged input files
bool WsfPProxySatisfy::SatisfyBlockP(WsfPParseRuleTraceContext* aContext, WsfPProxyValue aExistingValues)
{
   WsfPProxyDiff diff;
   bool          hasChanged = false;
   diff.mDiffs.clear();
   WsfPProxyPath proxyPath = aContext->GetAddr();

#ifdef DEBUG_CHANGES
   { // RAII block
      auto out  = ut::log::error() << "Beginning Root Node:";
      auto note = out.AddNote() << "Current:";
      mRegistryPtr->PrintProxy(mCurrentRoot.Get(), mCurrentRoot.Get().Lookup(proxyPath), note);
      note.Send();
      note << "Desired:";
      mRegistryPtr->PrintProxy(mDesiredRoot, mDesiredRoot.Lookup(proxyPath), note);
   }
#endif

   diff.Diff(mCurrentRoot.Get().Lookup(proxyPath), mDesiredRoot.Lookup(proxyPath), proxyPath);
   for (size_t i = 0; i < diff.mDiffs.size(); ++i)
   {
      if (diff.mDiffs[i].mType == WsfPProxyDiff::cCHANGED || diff.mDiffs[i].mType == WsfPProxyDiff::cADDED)
      {
         WsfPProxyPath& diffPath = diff.mDiffs[i].mPath;
         WsfPProxyDiff  diffExisting;
         // If our desired value is equal to the value reached after deserializing all inputs, don't change anything
         if (!diffExisting.IsEqual(mDesiredRoot.Lookup(diffPath), aExistingValues.Lookup(diffPath)))
         {
            // double-check that the attribute hasn't just been satisfied earlier in this loop
            if (/*!hasChanged || */ !diffExisting.IsEqual(mCurrentRoot.Get().Lookup(diffPath),
                                                          mDesiredRoot.Lookup(diffPath)))
            {
#ifdef PROXY_SERIALIZE_TRACE
               auto logger = ut::log::debug() << "SatisfyBlock:";
               logger.AddNote() << "Path: " << diffPath.ToString(mDesiredRoot);
#endif
               SatisfyResult result = SatisfyAttributeInContext(diff.mDiffs[i].mPath, aContext);
               if (result.Success())
               {
#ifdef PROXY_SERIALIZE_TRACE
                  logger.AddNote() << "SatisfyBlock SUCCESS:";
                  logger.AddNote() << "Path: " << diffPath.ToString(mCurrentRoot.Get());
#endif
                  hasChanged = true;
               }
               else
               {
#ifdef PROXY_SERIALIZE_TRACE
                  logger.AddNote() << "SatisfyBlock FAIL: ";
                  logger.AddNote() << "Path: " << diffPath.ToString(mCurrentRoot.Get());
#endif
               }
            }
         }
      }
   }

#ifdef DEBUG_CHANGES
   if (hasChanged)
   {
      auto out  = ut::log::debug() << "Ending Root Node:";
      auto note = out.AddNote() << "Current:";
      mRegistryPtr->PrintProxy(mCurrentRoot.Get(), mCurrentRoot.Get().Lookup(proxyPath), note);
      note.Send();
      note << "Desired:";
      mRegistryPtr->PrintProxy(mDesiredRoot, mDesiredRoot.Lookup(proxyPath), note);
   }
#endif
   return hasChanged;
}

bool WsfPProxySatisfy::SatisfyBlock(WsfPParseRuleTraceContext* aContext)
{
   WsfPProxyDiff diff;
   bool          hasChanged = false;
   diff.mDiffs.clear();
   WsfPProxyPath proxyPath = aContext->GetAddr();
   diff.Diff(mCurrentRoot.GetAttr(proxyPath).Get(), mDesiredRoot.Lookup(proxyPath), proxyPath);

   for (size_t i = 0; i < diff.mDiffs.size(); ++i)
   {
      if (diff.mDiffs[i].mType == WsfPProxyDiff::cCHANGED || diff.mDiffs[i].mType == WsfPProxyDiff::cADDED)
      {
         SatisfyResult result = SatisfyAttributeInContext(diff.mDiffs[i].mPath, aContext);
         if (result.Success())
         {
            hasChanged = true;
         }
      }
   }
   return hasChanged;
}

bool WsfPProxySatisfy::IsAttributeSatisfied(const WsfPProxyPath& aPath)
{
   WsfPProxyValue current(mCurrentRoot.GetAttr(aPath).Get());
   WsfPProxyValue desired(mDesiredRoot.Lookup(aPath));

   if (current && desired)
   {
      if (WsfPProxyBasicValue(current).IsEqual(desired))
      {
         return true;
      }
   }
   return false;
}
