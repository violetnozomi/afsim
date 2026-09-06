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

#ifndef WSFPPROXYDESERIALIZE_HPP
#define WSFPPROXYDESERIALIZE_HPP
class WsfParseNode;
#include "WsfPProxyI.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyUndo.hpp"

class WsfPProxyDeserializeContext;

class WSF_PARSER_EXPORT WsfPProxyDeserialize
{
public:
   WsfPProxyDeserialize(WsfPProxyRegistry* aRegistry, WsfPProxyIndex* aIndex = nullptr);
   ~WsfPProxyDeserialize();
   void ExecuteInitialActions(WsfPProxyStructValue aStructInstance, WsfParseStruct* aRulePtr);

   void Deserialize(WsfPProxyUndoValue& aContainer, WsfParseNode* aNodePtr);


   void AddValue(WsfPProxyValue& aProxyValue);

   WsfPProxyUndoValue& GetBasicRoot() { return mBasicRoot; }
   WsfPProxyUndoValue  GetRoot() const { return mProxyRoot; }

   WsfPProxyRegistry* GetRegistry() const { return mRegistryPtr; }
   WsfPProxyIndex*    GetIndex() const { return mIndexPtr; }

   // Pointer to a boolean that triggers an abort on deserialization.
   volatile bool* mAbortSwitch;

   bool IsDelayLoading() const { return mDelayLoading; }

   WsfPProxyDeserializeTracerI*  mTraceBuilder;
   WsfPProxyDeserializeObserver* mObserver;
   // wsf core does 'delay loading' on some types, automatically sorting the parsing of types
   // to read base types first.  The proxy does the same thing here by keeping a list of nodes
   // which should be processed after others.
   using DelayNode = std::pair<size_t, WsfParseNode*>;
   std::vector<DelayNode> mDelayNodes;

private:
   void          BeginNode(WsfPProxyDeserializeContext& aContext);
   void          EndNode(WsfPProxyDeserializeContext& aContext);
   volatile bool mNoAbortSwitch;
   bool          mDelayLoading;

   WsfPProxyUndoValue                     mProxyRoot;
   WsfPProxyUndoValue                     mBasicRoot;
   WsfPProxyRegistry*                     mRegistryPtr;
   WsfPProxyIndex*                        mIndexPtr;
   std::map<WsfPProxyPath, WsfPProxyPath> mRenameMapping;
   std::vector<WsfPProxyValue>            mIntermediateValues;
};

#endif
