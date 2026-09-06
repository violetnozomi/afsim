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

#ifndef WSFPPROXYDESERIALIZECONTEXT_HPP
#define WSFPPROXYDESERIALIZECONTEXT_HPP

#include "WsfPProxyBasicValue.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyPath.hpp"
#include "WsfPProxyUndo.hpp"
#include "WsfPProxyValue.hpp"
#include "WsfParseAction.hpp"
#include "WsfParseType.hpp"

class WsfParseNode;

// Stores the evaluation context.
class WsfPProxyDeserializeContext
{
public:
   enum ApplyResult
   {
      cAPPLY_SUCCESS,
      cAPPLY_SKIP,
      cAPPLY_DELAY,
      cAPPLY_PRUNE
   };

   struct ReturnValue
   {
      ReturnValue() {}
      ReturnValue(const std::string& aText)
         : mTextValue(aText)
      {
      }
      ReturnValue(WsfPProxyValue& val)
         : mProxyValue(val)
      {
      }
      ReturnValue& operator=(const WsfPProxyValue& aLhs)
      {
         mProxyValue = aLhs;
         mTextValue.clear();
         return *this;
      }
      ReturnValue& operator=(const std::string& aText)
      {
         mTextValue = aText;
         return *this;
      }
      ReturnValue& operator=(const ReturnValue& aRhs)
      {
         mTextValue = aRhs.mTextValue;
         if (mTextValue.empty())
         {
            mProxyValue = aRhs.mProxyValue;
         }
         return *this;
      }
      std::string ToString()
      {
         if (mProxyValue && mProxyValue.IsBasicType())
         {
            return WsfPProxyBasicValue(mProxyValue).ToString();
         }
         return mTextValue;
      }
      std::string    mTextValue;
      WsfPProxyValue mProxyValue;
   };

   WsfPProxyDeserializeContext(WsfPProxyDeserializeContext& aBase);

   WsfPProxyDeserializeContext(WsfPProxyDeserialize* aDeserializer, const WsfPProxyUndoValue& aContainer);
   ~WsfPProxyDeserializeContext()
   {
      for (size_t i = 0; i < mNodesReturnValues.size(); ++i)
      {
         if (mNodesReturnValues[i])
         {
            mDeserializer->AddValue(mNodesReturnValues[i]);
         }
      }
   }
   void BeginNode(WsfParseNode* aNodePtr);
   void EndNode(WsfParseNode* aNodePtr);
   // Return true if the deserialize process should be aborted.
   bool IsAborting() { return *mDeserializer->mAbortSwitch; }


   ReturnValue GetOrdValue(size_t aOrd);
   std::string GetOrdString(size_t aOrd);

   std::string NodeValue(const WsfParseNode* aValue) const;

   WsfPProxyRegistry* GetRegistry() const { return mDeserializer->GetRegistry(); }

   // Nodes are processed in sequences - aka sibling nodes.  This is the first sibling / first node in the current sequence.
   WsfParseNode* mCurrentNodePtr;
   // Because some nodes return values, we need a place to store the returns.
   // This is filled with the return value of each node in a sequence.
   // i.e. mNodesReturnValues[0] will store the return value of mCurrentNodePtr
   // Actions may reference one of these returned values using the "$1" notation.
   std::vector<WsfPProxyValue> mNodesReturnValues;

   const WsfPProxyUndoValue&     Container() const { return mContainer; }
   const WsfPProxyPath&          CurrentPath() const { return mContainer.GetPath(); }
   WsfPProxyDeserialize&         Deserializer() { return *mDeserializer; }
   WsfPProxyDeserializeObserver* Observer() { return mDeserializer->mObserver; }

   void                SetContainer(const WsfPProxyUndoValue& aContainer) { mContainer = aContainer; }
   WsfPProxyUndoValue& BasicRoot() { return mDeserializer->GetBasicRoot(); }
   //      WsfPProxyUndoValue& Root() { return mDeserializer->mProxyRoot; }
   void SetContainer(const WsfPProxyValue& aContainer) { mContainer = WsfPProxyUndoValue(aContainer); }
   void SetSkip(bool aSkip) { mSkip = aSkip; }
   bool IsSkip() { return mSkip; }
   void AddToIndex(const WsfPProxyPath& aPath, WsfParseNode* aNodePtr, WsfPProxyIndex::EntryType aEntryType);


   // Execute actions on this node and all children.
   bool ApplyActions();

   // Execute a sequence of action parts
   bool DoActions(WsfParseAction* aActions, WsfParseNode* aNodePtr);

private:
   WsfPProxyDeserializeTracerI::DeserializeAction CanContinue(WsfParseNode* aParentNodePtr, WsfParseNode* aChildNodePtr)
   {
      WsfPProxyDeserializeTracerI* tracer = GetTracer();
      if (!tracer)
      {
         return WsfPProxyDeserializeTracerI::cDA_CONTINUE;
      }
      return tracer->CanContinue(aParentNodePtr, aChildNodePtr);
   }

   void RollbackComplete(WsfParseNode* aParentNodePtr)
   {
      WsfPProxyDeserializeTracerI* tracer = GetTracer();
      if (tracer)
      {
         tracer->RollbackComplete(aParentNodePtr);
      }
   }

   WsfPProxyDeserializeTracerI* GetTracer() { return mDeserializer->mTraceBuilder; }

   WsfPProxyUndoValue Lookup(const WsfParseActionAddress& aAddr, const WsfPProxyUndoValue& aBaseContainer);

   // Lookup a value in either basic types or proxy root.
   WsfPProxyUndoValue LookupRoot(const WsfParseTypePath& aLookupPath);


   // Evaluate an action.  The action may have side-effects upon the context.
   ReturnValue Eval(WsfParseValueType* aLHS_TypePtr, WsfParseActionPart& aExpr);


   // Some nodes output values, like (value ...) nodes.
   // Prior to executing actions on these nodes, we create the value
   // to be stored as the current value
   WsfPProxyValue CreateNodeOutput(WsfParseNode* aNodePtr);


   WsfPProxyUndoValue LookupP(const WsfParseActionAddress& aAddr, const WsfPProxyUndoValue& aAddressBase);

   WsfPProxyDeserializeContext* mParent;
   WsfPProxyDeserialize*        mDeserializer;


   // The current container.  This may be any proxy value and is the current value
   // that actions are operating on.  If the container is a struct instance,
   // we might process an action like side="blue".  If the container is a basic type,
   // we might process an action like this="blue".
   WsfPProxyUndoValue mContainer;

   bool mSkip;
};


#endif
