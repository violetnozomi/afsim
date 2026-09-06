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

#ifndef WSFPPROXYSATISFY_HPP
#define WSFPPROXYSATISFY_HPP

#include "WsfPProxyBasicValue.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyPath.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyRuleTrace.hpp"
#include "WsfPProxyUndo.hpp"
#include "WsfPProxyValue.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseTypeLoadRule.hpp"

class WsfPProxyActionLookup;

class ProxyEvaluator
{
public:
   ProxyEvaluator(WsfPProxyRegistry* reg)
      : mRegistryPtr(reg)
   {
   }

   std::string StringOrdValue(WsfParseNode* aParentNodePtr, size_t aOrd)
   {
      WsfParseNode* childPtr = aParentNodePtr->GetChild(aOrd);
      if (childPtr)
      {
         return childPtr->mValue.Text();
      }
      return "";
   }
   std::string StringEval(WsfParseNode* aParentNodePtr, WsfParseActionPart& aExpr)
   {
      switch (aExpr.mActionPartType)
      {
      case WsfParseActionPart::cNODE_REFERENCE:
      {
         WsfParseActionNodeReference* ref = (WsfParseActionNodeReference*)&aExpr;
         if (ref->mValueTypePtr)
         {
            return StringOrdValue(aParentNodePtr, ref->mIndex);
         }
         return StringOrdValue(aParentNodePtr, ref->mIndex);
      }
      break;
      case WsfParseActionPart::cLITERAL:
      {
         WsfParseActionLiteral* lit = (WsfParseActionLiteral*)&aExpr;
         return lit->mText;
      }
      break;
      default:
         break;
      }
      return "";
   }

   WsfPProxyRegistry* mRegistryPtr;
};

class WsfPProxySatisfy
{
public:
   WsfPProxyValue       mDesiredRoot;
   WsfPProxyRegistry*   mRegistryPtr;
   ProxyEvaluator       mEvaluator;
   WsfPParseRuleTrace*  mTracePtr;
   WsfPParseRuleTracer* mTracerPtr;
   bool                 mRecurrenceInsertBeforeEnd;

   std::set<const WsfPProxyType*> mTypesThatNeedToBeCleared;
   // Keeps track of proxy values that have been cleared in attempt to satisfy a value
   // this is for the case of :
   // platform_type PT ...    route  x y z   end_route
   // platform P PT ...    +route  a b c   end_route
   std::set<void*> mAttemptedToClear;

   enum SatMode
   {
      cSM_NONE,
      // Attempt to satisfy a single attribute (mTargetPath)
      cSM_TARGET,
      // The target has been satisfied.  Continue operation until rules are completed successfully.
      // cSM_COMPLETE,
      // Check existing inputs to make sure they are correct
      // Prune or replace bad inputs when possible
      cSM_FIX
   };

   SatMode mSatMode;

   bool FixMode() const { return false; }

   WsfPProxyPath mTargetPath;


   struct SatPoint
   {
   public:
      enum Flags
      {
         cTARGET_COMPLETE = 1
      };

      SatPoint()
         : trace(nullptr)
         , flags(0)
      {
      }
      SatPoint(const SatPoint& aSrc)
         : current(aSrc.current)
         , desired(aSrc.desired)
         , returnValue(aSrc.returnValue)
         , trace(aSrc.trace)
         , flags(aSrc.flags)
      {
      }
      SatPoint(const SatPoint& aSrc, WsfPParseRuleTrace* aTrace)
         : current(aSrc.current)
         , desired(aSrc.desired)
         , returnValue(aSrc.returnValue)
         , trace(aTrace)
         , flags(aSrc.flags)
      {
      }
      SatPoint(WsfPProxyUndoValue  aCurrent,
               WsfPProxyValue      aDesired,
               WsfPProxyValue      aReturnValue,
               WsfPParseRuleTrace* aTrace,
               int                 aFlags = 0)
         : current(aCurrent)
         , desired(aDesired)
         , returnValue(aReturnValue)
         , trace(aTrace)
         , flags(aFlags)
      {
      }

      bool IsTargetComplete() const { return 0 != (flags & cTARGET_COMPLETE); }

      void SetTargetComplete(bool aIsComplete)
      {
         if (aIsComplete)
         {
            flags |= cTARGET_COMPLETE;
         }
         else
         {
            flags &= ~cTARGET_COMPLETE;
         }
      }

      // Normally we only recurse if a rule has utility in setting the target value
      // If this returns true, some previous rule has utility, and we just want to complete a valid trace.
      // Ex: Here we could choose either end_ token:
      //    antenna_pattern MY_PATTERN { end_antenna_pattern | end_pattern }
      // Ex: Here we need to choose the type of waypoint correctly, but the 'target' of adding a waypoint has already
      // been met.
      //    [pushBack(waypoints)] <Waypoint.any>
      bool NeedToComplete() const { return IsTargetComplete(); }

      WsfPProxyUndoValue current;
      WsfPProxyValue     desired;
      WsfPProxyValue     returnValue;
      // WsfPProxyPath        target;
      WsfPParseRuleTrace* trace;
      int                 flags;
   };

   WsfPProxyUndoValue mCurrentRoot;

   enum SatisfyResultEnum
   {
      cNOT_SATISFIABLE  = 0,
      cSATISFIABLE      = 1,
      cADDS_VALUE       = 2,
      cCHANGED_TARGET   = 4,
      cSUCCESS          = cADDS_VALUE | cSATISFIABLE,
      cSUCCESS_COMPLETE = cCHANGED_TARGET | cSUCCESS
   };

   struct SatisfyResult
   {
      SatisfyResult(SatisfyResultEnum aResult)
         : mResult(static_cast<unsigned int>(aResult))
      {
      }
      SatisfyResult()
         : mResult(cNOT_SATISFIABLE)
      {
      }
      // True if satisfiable
      operator bool() const { return 0 != (mResult & cSATISFIABLE); }
      // True if both satisfiable and adds value
      bool Success() const { return (cSUCCESS & mResult) == cSUCCESS; }

      bool TargetComplete() const { return (cSUCCESS_COMPLETE & mResult) == cSUCCESS_COMPLETE; }
      // Combine RHS with this
      SatisfyResult& operator+=(const SatisfyResult& aRhs)
      {
         mResult |= aRhs.mResult;
         return *this;
      }
      unsigned int mResult;
   };

   mutable std::ofstream debugOutput;

   WsfPProxySatisfy(WsfPProxyRegistry*   aRegistryPtr,
                    WsfPProxyValue       aDesiredRootValue,
                    WsfPProxyUndoValue   aCurrentRootValue,
                    WsfPParseRuleTracer* aTracerPtr);
   ~WsfPProxySatisfy();
   bool SatisfyRootDifferences();
   bool SatisfyBlock(WsfPParseRuleTraceContext* aContextPtr);
   bool SatisfyBlock(WsfPParseRuleTraceContext* aContextPtr, WsfPProxyValue aExistingValues);
   bool SatisfyAttribute(const WsfPProxyPath& aPath);


   WsfPProxySatisfy::SatisfyResult SatisfyAttributeInContext(const WsfPProxyPath&       aPath,
                                                             WsfPParseRuleTraceContext* aContextPtr);
   bool                            IsEqual(const WsfPProxyValue& A, const WsfPProxyValue& B);

   WsfPProxyValue LookupBasicValue(const WsfPProxyPath& aPath);
   WsfPProxyValue LookupBasicValue(const WsfParseTypePath& aPath);

   WsfPProxyUndoValue LookupCurrent(WsfPProxyUndoValue& aCurrent, const WsfParseTypePath& aAddr, bool aRelative);

   WsfPProxyUndoValue LookupCurrent(WsfPProxyUndoValue& aCurrent, const WsfParseActionAddress& aAddr);

   WsfPProxyValue Lookup(WsfPProxyValue&              aRoot,
                         const WsfPProxyValue&        c,
                         const WsfParseActionAddress& aAddr,
                         WsfPProxyPath*               aUpdatePath = nullptr);

   WsfPProxyValue LookupDesired(WsfPProxyValue               aDesiredValue,
                                const WsfParseActionAddress& aAddr,
                                WsfPProxyPath*               aUpdatePath = nullptr)
   {
      return Lookup(mDesiredRoot, aDesiredValue, aAddr, aUpdatePath);
   }

   WsfPProxyValue LookupCurrentOrBasicValue(WsfPProxyUndoValue& aCurrent, const WsfParseTypePath& aAddr, bool aRelative);

   WsfPProxyValue LookupDesired(WsfPProxyValue          aDesiredValue,
                                const WsfParseTypePath& aAddr,
                                bool                    aRelative,
                                WsfPProxyPath*          aUpdatePath);

   // An expression in an assignment made by a grammar action
   struct EquationExpr
   {
      EquationExpr(WsfPProxyValue v)
         : mExprType(cVALUE)
         , mValue(v)
         , mNodeIndex(ut::npos)
      {
      }
      EquationExpr(std::string& aFunc)
         : mExprType(cFUNCTION)
         , mFunction(aFunc)
         , mNodeIndex(ut::npos)
      {
      }
      EquationExpr(size_t aNodeIndex)
         : mExprType(cNODE_INDEX)
         , mNodeIndex(aNodeIndex)
      {
      }
      EquationExpr() {}
      static EquationExpr UnsetValue()
      {
         EquationExpr e;
         e.mExprType = cUNSET_VALUE;
         return e;
      }
      enum ExprType
      {
         // A fixed value
         cVALUE,
         cUNSET_VALUE,
         cFUNCTION,
         // This expression has the value returned by a node.
         // When an equation is solved, this is a variable
         cNODE_INDEX
      };
      void Cleanup()
      {
         mValue.Delete();
         for (size_t i = 0; i < mParameters.size(); ++i)
         {
            mParameters[i].Cleanup();
         }
      }
      ExprType                  mExprType;
      WsfPProxyValue            mValue;
      std::string               mFunction;
      size_t                    mNodeIndex;
      std::vector<EquationExpr> mParameters;
   };

   //! Represents an assignment being made in a grammar action
   struct Equation
   {
      // Index of the action group.  0 indicates before the first sequence subrule.
      size_t mActionSet;
      // WsfParseActionAddress      mLHS;
      WsfPProxyPath mLHS;
      EquationExpr  mRHS;

      void Cleanup() { mRHS.Cleanup(); }
   };

   // Builds an equation expression
   void EquationExpression(WsfParseActionPart* aExprPtr, WsfParseValueType* aLHS_TypePtr, EquationExpr& aExpr);

   EquationExpr EvalExpr(EquationExpr& aExpr);

   //! Attempts to solve an equation to determine:
   //!    -If the equation is unsolvable, returns false
   //!    -If the equation can be solved, the variable values are stored in aVariables
   //! @param aLhs    The desired value of the equation's left-hand-side.
   //! @param aRHS    The expression to evaluate
   //! @param aVariables   The variables values used in the expression.  Variables not bound to a value are null.
   bool SolveEquation(const WsfPProxyValue& aLhs, EquationExpr& aRHS, std::vector<WsfPProxyValue*>& aVariables);

   //! Attempts to solve a set of equations.
   bool SolveEquations(const WsfPProxyValue& aCurrentDesired,
                       // WsfParseActionAddress&   aPrefixAddr,
                       std::vector<Equation>&        aEquations,
                       std::vector<WsfPProxyValue*>& aVariables);

   // Given a rule sequence and a current and desired value, build the equations
   // required to solve for input values.
   bool BuildSequenceEquations(WsfParseSequence*             aSequence,
                               WsfPProxyUndoValue&           aCurrentValue,
                               const WsfPProxyValue&         aDesiredValue,
                               std::vector<Equation>&        aEquations,
                               WsfParseNode*                 aExistingNodePtr,
                               std::vector<WsfPProxyValue*>& aVariables);

   struct SolveResult
   {
      enum Disposition
      {
         cCONTINUE,
         cSTOP
      };
      SolveResult()
         : mDisposition(cCONTINUE)
      {
      }
      Disposition mDisposition;
   };

   int ApplyEquations(size_t                 aActionSetIndex,
                      size_t&                aNextEquation,
                      std::vector<Equation>& aEquations,
                      WsfPProxyUndoValue&    aCurrentValue,
                      WsfPProxyValue         aDesiredValue);

   // Execute action on aCurrentValue while keeping aNewValue pointing to the same
   // relative position in the proxy tree.
   void FollowAction(WsfParseAction*     aActionPtr,
                     WsfPParseRuleTrace* aParentTracePtr,
                     // WsfParseNode*                       aParentNodePtr,
                     WsfPProxyUndoValue& aCurrentValue,
                     WsfPProxyValue*     aDesiredValue,
                     WsfPProxyPath&      aCurrentPath,
                     // std::vector<WsfPProxyBasicValue>&   aCurrentNodeReturns,
                     std::vector<WsfPProxyValue*> aVariables,
                     SatisfyResult&               aResult);

   WsfParseNamedRule* GetRuleOutput(WsfParseRule* aRulePtr);

   // Some parse rules return values instead of operating on the current value.
   // In this case, we handle these rules as if they operated on the current value by determining
   // where the return value is assigned.
   // This method returns true if the assignment address is found.
   bool FindBindingAddress(WsfPParseRuleTrace* aTracePtr,
                           WsfPProxyUndoValue& aCurrentValue,
                           WsfPProxyValue      aDesiredValue,
                           // std::vector<WsfPProxyBasicValue>& aReturnValues,
                           WsfParseSequence*      aSeq,
                           int                    aNextSequenceChild,
                           WsfParseActionAddress& aBindAddr);

   bool FindBindingAddress2(WsfPParseRuleTrace*               aTracePtr,
                            WsfPProxyUndoValue&               aCurrentValue,
                            WsfPProxyValue                    aDesiredValue,
                            std::vector<WsfPProxyBasicValue>& aReturnValues,
                            WsfParseSequence*                 aSeq,
                            size_t                            aNextSequenceChild,
                            WsfPProxyPath&                    aBindAddr);

   SatisfyResult SatisfySequence(SatPoint pt);

   SatisfyResult SatisfyAlternate(SatPoint pt);

   //! @param aDesiredValue    The desired value to be returned by the current trace node.
   //! @param aTracePtr        The pointer to the current trace node.
   SatisfyResult SatisfyValueRule(WsfPProxyBasicValue aDesiredValue, WsfPParseRuleTrace* aTracePtr);
   SatisfyResult SatisfyRoot(SatPoint pt);
   SatisfyResult SatisfyRecurrence(SatPoint pt);

   //       SatisfyResult Scan(SatPoint pt);
   //       SatisfyResult ScanRoot(SatPoint pt);
   //       SatisfyResult ScanSequence(SatPoint pt);


   enum TypeLoadLookupResult
   {
      cTL_SUCCESS,
      cTL_COLLISION,
      cTL_LOADFAIL,
      cTL_FAILURE
   };
   TypeLoadLookupResult LookupTypeLoad(WsfParseTypeInfoData::Operation aOperation,
                                       bool                            aIsSaveType,
                                       const WsfParseTypeName&         aType,
                                       WsfPProxyUndoValue&             aCurrent,
                                       WsfPProxyValue&                 aDesired,
                                       std::string&                    aObjectName);

   SatisfyResult SatisfyTypeLoad(WsfParseRule* currentRulePtr, SatPoint pt);

   SatisfyResult Satisfy(SatPoint pt);

private:
   bool SatisfyBlockP(WsfPParseRuleTraceContext* aContextPtr, WsfPProxyValue aExistingValues);

   bool IsAttributeSatisfied(const WsfPProxyPath& aPath);

   std::unique_ptr<WsfPProxyActionLookup> mActionLookupPtr;
};


#endif
