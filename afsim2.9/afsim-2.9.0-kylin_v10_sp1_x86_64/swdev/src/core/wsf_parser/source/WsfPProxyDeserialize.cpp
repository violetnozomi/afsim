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

//
// This file contains code to traverse a parse tree and execute actions that build up the proxy values.
//
//
#include "WsfPProxyDeserialize.hpp"

#include <cassert>
#include <map>

#include "UtLog.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyDeserializeContext.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfPProxyType.hpp"
#include "WsfParseAlternate.hpp"
#include "WsfParseDefinitions.hpp"
#include "WsfParseIndex.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseSequence.hpp"
#include "WsfParseTypeLoadRule.hpp"

WsfPProxyDeserialize::WsfPProxyDeserialize(WsfPProxyRegistry* aRegistry, WsfPProxyIndex* aIndex)
   : mObserver(nullptr)
   , mNoAbortSwitch(false)
   , mDelayLoading(false)
   , mRegistryPtr(aRegistry)
   , mIndexPtr(aIndex)
{
   mTraceBuilder = nullptr;
   mAbortSwitch  = &mNoAbortSwitch;
}

void WsfPProxyDeserialize::Deserialize(WsfPProxyUndoValue& aContainer, WsfParseNode* aNodePtr)
{
   mProxyRoot    = aContainer;
   mBasicRoot    = WsfPProxyUndoValue(mRegistryPtr->GetBasicRoot());
   mDelayLoading = false;

   {
      WsfPProxyDeserializeContext ctx(this, aContainer);
      ctx.mCurrentNodePtr = aNodePtr;
      mDelayLoading       = false;
      ctx.ApplyActions();
   }

   // Do delay loading
   {
      mDelayLoading = true;

      std::sort(mDelayNodes.begin(), mDelayNodes.end());

      for (auto& delayNode : mDelayNodes)
      {
         WsfPProxyDeserializeContext ctx(this, aContainer);
         ctx.mCurrentNodePtr = delayNode.second;
         ctx.ApplyActions();
      }

      mDelayLoading = false;
   }
   mProxyRoot.Get().SetInherited(false);
}

void WsfPProxyDeserialize::AddValue(WsfPProxyValue& aProxyValue)
{
   // Print a warning when this vector is getting large
   if (mIntermediateValues.size() > 1000000)
   {
      auto logger = ut::log::warning() << "Large number of intermediate values in proxy deserializer.";
      logger.AddNote() << "Value: " << aProxyValue.ValueToString();

      if (aProxyValue.GetType())
      {
         logger.AddNote() << "Kind: " << aProxyValue.GetType()->mTypeKind;
      }
   }

   mIntermediateValues.push_back(aProxyValue);
}

// Executes commands marked with "initially" in the grammar file.
void WsfPProxyDeserialize::ExecuteInitialActions(WsfPProxyStructValue aStructInstance, WsfParseStruct* aRulePtr)
{
   if (!aRulePtr->mInitialActions.empty())
   {
      WsfPProxyUndoValue          structVal(aStructInstance);
      WsfPProxyDeserializeContext ctx(this, structVal);
      ctx.mCurrentNodePtr = nullptr;

      for (auto& initialAction : aRulePtr->mInitialActions)
      {
         WsfPProxyDeserializeContext newCtx(ctx);
         newCtx.DoActions(initialAction, nullptr);
      }
   }
}

WsfPProxyDeserialize::~WsfPProxyDeserialize()
{
   std::sort(mIntermediateValues.begin(), mIntermediateValues.end());

   mIntermediateValues.erase(std::unique(mIntermediateValues.begin(), mIntermediateValues.end()),
                             mIntermediateValues.end());

   for (auto& intermediateValue : mIntermediateValues)
   {
      intermediateValue.Delete();
   }
}
