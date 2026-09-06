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

#include "RvBAT_Interface.hpp"

#include <QMap>
#include <QSet>

#include "RvEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"

RvBAT::Interface::Interface(QObject* parent)
   : QObject(parent)
{
}

void RvBAT::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   rv::ResultDb* db = aData.GetDb();

   if (db != nullptr)
   {
      // Wait to initialize all the BAT data until we have loaded the db.
      if (db->GetPlatformCount() >= 1 && !mBehaviorToolLoaded)
      {
         // For each platform, check if it has abt or fsm data
         for (unsigned int platformIndex = 0; platformIndex < db->GetPlatformCount(); ++platformIndex)
         {
            rv::ResultPlatform* plat = aData.FindPlatform(platformIndex);
            if (plat != nullptr)
            {
               const auto* btArray  = plat->GetArray<rv::MsgBehaviorTree>();
               const auto* fsmArray = plat->GetArray<rv::MsgStateMachine>();
               // If we have an abt for this platform, populate it in the list of trees.
               if (btArray != nullptr)
               {
                  for (auto&& kt : *btArray)
                  {
                     const rv::MsgBehaviorTree& tree        = *static_cast<rv::MsgBehaviorTree*>(kt);
                     mTrees[platformIndex][tree.id()].mName = tree.treeName();
                     // Populate the node list for this tree
                     for (const auto& node : tree.nodes())
                     {
                        mTrees[platformIndex][tree.id()].mChildren.insert(node.nodeId(), node);
                     }
                  }

                  if (mTrees.size() > 0)
                  {
                     // We are only loaded if we set data.
                     mBehaviorToolLoaded = true;
                  }
               }

               // If we have state machines for this platform, populate them.
               if (fsmArray != nullptr)
               {
                  for (auto&& kt : *fsmArray)
                  {
                     const rv::MsgStateMachine& fsm = *static_cast<rv::MsgStateMachine*>(kt);
                     // Populate the state list for this fsm
                     for (const auto& state : fsm.states())
                     {
                        mFSMs[platformIndex][fsm.fsmId()].mStates.insert(state.stateIndex(), state);
                     }
                  }

                  if (mFSMs.size() > 0)
                  {
                     // We are only loaded if we set data.
                     mBehaviorToolLoaded = true;
                  }
               }
            }
         }
      }
   }

   // Once we've loaded the initial list of platforms with behavior trees, update here.
   if (mBehaviorToolLoaded)
   {
      // Loop through each platform and check for tree state updates.
      // We want to populate every tree here, even ones not shown,
      // just in case the user swaps platforms in the combo box while paused.
      // comboBoxChanged will get called, and we won't have access to aData like we do here.
      for (size_t platformIndex : mTrees.keys())
      {
         rv::ResultPlatform* plat = aData.FindPlatform(static_cast<int>(platformIndex));
         // If the platform exists
         if (plat != nullptr)
         {
            // Begin reading and get the arrays for ABT and FSM states
            plat->BeginRead();
            const auto btArray  = plat->GetArray<rv::MsgBehaviorTreeState>();
            const auto fsmArray = plat->GetArray<rv::MsgStateMachineState>();
            // If we have ABT data to read from
            if (btArray != nullptr)
            {
               // Create a set containing all tree ids for this platform
               auto treeIds = mTrees.value(platformIndex).keys().toSet();
               // Get our starting iterator to iterate backwards from
               auto it = btArray->FindFirstIteratorBefore(aData.GetSimTime());
               if (it != btArray->end())
               {
                  // This will run while we still have messages to check for and trees to
                  // populate (treeIds non-empty). treeIds starts full with each tree id,
                  // and each id will be removed as a message is found for it.  Since we
                  // are iterating backwards this guarantees we will have the newest message
                  // for each tree id
                  while (!treeIds.isEmpty())
                  {
                     rv::MsgBehaviorTreeState* msg = dynamic_cast<rv::MsgBehaviorTreeState*>(*it);
                     // If the message exists and we haven't already found a newer message
                     if (msg != nullptr && treeIds.find(msg->treeId()) != treeIds.end())
                     {
                        // If we have a tree matching the message, then update the state ands
                        // remove this tree from the ids left to populate
                        if (mTrees[platformIndex].find(msg->treeId()) != mTrees[platformIndex].end())
                        {
                           mTrees[platformIndex][msg->treeId()].mState = msg;
                           treeIds.remove(msg->treeId());
                        }
                     }
                     // Go to the previous message if we aren't at the beginning
                     if (it == btArray->begin())
                     {
                        break;
                     }
                     --it;
                  }

                  for (auto treeId : treeIds)
                  {
                     mTrees[platformIndex][treeId].mState = nullptr;
                  }
               }
               else
               {
                  for (auto treeId : treeIds)
                  {
                     mTrees[platformIndex][treeId].mState = nullptr;
                  }
               }
            }
            // If we have FSM data to read from
            if (fsmArray != nullptr)
            {
               // Create a set containing all tree ids for this platform
               auto fsmIds = mFSMs.value(platformIndex).keys().toSet();
               // Get our starting iterator to iterate backwards from
               auto it = fsmArray->FindFirstIteratorBefore(aData.GetSimTime());
               if (it != fsmArray->end())
               {
                  // This will run while we still have messages to check for and fsms to
                  // populate (fsmIds non-empty). treeIds starts full with each fsm id,
                  // and each id will be removed as a message is found for it.  Since we
                  // are iterating backwards this guarantees we will have the newest message
                  // for each fsm id
                  while (!fsmIds.isEmpty())
                  {
                     rv::MsgStateMachineState* msg = dynamic_cast<rv::MsgStateMachineState*>(*it);
                     // If the message exists and we haven't already found a newer message
                     if (msg != nullptr && fsmIds.find(msg->fsmId()) != fsmIds.end())
                     {
                        // If we have an fsm matching the message, then update the state ands
                        // remove this tree from the ids left to populate
                        if (mFSMs[platformIndex].find(msg->fsmId()) != mFSMs[platformIndex].end())
                        {
                           mFSMs[platformIndex][msg->fsmId()].mState = msg;
                           fsmIds.remove(msg->fsmId());
                        }
                     }
                     // Go to the previous message if we aren't at the beginning
                     if (it == fsmArray->begin())
                     {
                        break;
                     }
                     --it;
                  }

                  for (auto fsmId : fsmIds)
                  {
                     mFSMs[platformIndex][fsmId].mState = nullptr;
                  }
               }
               else
               {
                  for (auto fsmId : fsmIds)
                  {
                     mFSMs[platformIndex][fsmId].mState = nullptr;
                  }
               }
            }
            plat->EndRead();
         }
      }
   }
}

void RvBAT::Interface::Reset()
{
   mBehaviorToolLoaded = false;
   mTrees.clear();
   mFSMs.clear();
}

const QString RvBAT::Interface::GetTreeNameFromId(const size_t aId) const
{
   // If we can find the name of a tree by id, return it
   for (auto trees : GetBehaviorTrees())
   {
      if (trees.find(aId) != trees.end())
      {
         return QString::fromStdString(trees.value(aId).mName);
      }
   }
   return "";
}
