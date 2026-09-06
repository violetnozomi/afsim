// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvPlatformHistoryBehaviorStateData.hpp"

RvPlatformHistory::BehaviorStateData::BehaviorStateData(rv::ResultPlatform*         aPlatform,
                                                        std::map<std::string, int>& aStateNames)
   : mStateNames(aStateNames)
{
   aPlatform->BeginRead();
   const auto dataArray = aPlatform->GetArray<rv::MsgBehaviorTree>();

   if (dataArray != nullptr)
   {
      for (auto&& kt : *dataArray)
      {
         auto tree = static_cast<rv::MsgBehaviorTree*>(kt);
         for (auto& node : tree->nodes())
         {
            mBehaviorMap.insert({node.nodeId(), node.nodeName()});
         }
      }
   }
   aPlatform->EndRead();
}

std::map<float, int> RvPlatformHistory::BehaviorStateData::GetStateChanges(float               aStart,
                                                                           float               aStop,
                                                                           int&                aStartState,
                                                                           rv::ResultPlatform* aPlatformData) const
{
   std::map<float, int> ret;
   aStartState = NO_STATE;

   auto treeState = aPlatformData->FindFirstBefore<rv::MsgBehaviorTreeState>(aStart);
   if (treeState != nullptr)
   {
      for (const auto& treeExcStateIter : treeState->execList())
      {
         if (treeExcStateIter.execState() == rv::NodeExecState::running &&
             mBehaviorMap.find(treeExcStateIter.nodeId()) != mBehaviorMap.end() &&
             mStateNames.find(mBehaviorMap.at(treeExcStateIter.nodeId())) != mStateNames.end())
         {
            aStartState = mStateNames.at(mBehaviorMap.at(treeExcStateIter.nodeId()));
         }
      }
   }
   else
   {
      aStartState = NO_STATE;
   }

   auto platformStates = aPlatformData->GetArray<rv::MsgBehaviorTreeState>();
   if (platformStates != nullptr)
   {
      for (auto platState = platformStates->FindFirstIteratorAfter(aStart);
           platState != platformStates->end() && (*platState)->simTime() <= aStop;
           ++platState)
      {
         const rv::MsgBehaviorTreeState& msg = static_cast<const rv::MsgBehaviorTreeState&>(**platState);
         int                             state{NO_STATE};
         for (const auto& treeExcStateIter : msg.execList())
         {
            if (treeExcStateIter.execState() == rv::NodeExecState::running &&
                mBehaviorMap.find(treeExcStateIter.nodeId()) != mBehaviorMap.end() &&
                mStateNames.find(mBehaviorMap.at(treeExcStateIter.nodeId())) != mStateNames.end())
            {
               state = mStateNames.at(mBehaviorMap.at(treeExcStateIter.nodeId()));
            }
         }
         ret.emplace(msg.simTime(), state);
      }
   }

   return ret;
}
