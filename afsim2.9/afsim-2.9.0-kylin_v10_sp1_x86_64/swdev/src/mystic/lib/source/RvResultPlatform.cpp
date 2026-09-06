// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvResultPlatform.hpp"

#include <cfloat>

#include "RvEventPipeClasses.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "UtMemory.hpp"

rv::ResultPlatform::ResultPlatform(const std::string& aName, unsigned int aPlatformIndex, double aTime)
   : mPlatformIndex(aPlatformIndex)
   , mName(aName)
   , mStartTime(aTime)
{
   mMessageMap.UseDeferredPopping(rv::MsgPlatformInfo::cMESSAGE_ID);
   mMessageMap.UseDeferredPopping(rv::MsgPlatformStatus::cMESSAGE_ID);
   mMessageMap.UseDeferredPopping(rv::MsgEntityState::cMESSAGE_ID);
}

const rv::MsgEmitterModeDefinition* rv::ResultPlatform::FindSensorMode(const std::string& aModeName,
                                                                       const std::string& aSensorName) const
{
   auto jt = mSensorModeDefinition.find(std::make_pair(aSensorName, aModeName));
   if (jt == mSensorModeDefinition.end())
   {
      return nullptr;
   }
   else
   {
      return jt->second.get();
   }
}

const rv::MsgEmitterModeDefinition* rv::ResultPlatform::FindWeaponMode(const std::string& aModeName,
                                                                       const std::string& aWeaponName) const
{
   auto jt = mWeaponModeDefinition.find(std::make_pair(aWeaponName, aModeName));
   if (jt == mWeaponModeDefinition.end())
   {
      return nullptr;
   }
   else
   {
      return jt->second.get();
   }
}
/*
const rv::MsgBehaviorTreeNodeChildren * rv::ResultPlatform::FindBehaviorTreeChildren(float aSimTime) const
{
   return mBehaviorTreeNodeChildrenArray.FindFirstAfter(aSimTime);
}

const rv::MsgBehaviorTreeState * rv::ResultPlatform::FindBehaviorTreeState(float aSimTime) const
{
   if (aSimTime > 0.0)
   {
      return mBehaviorTreeStateArray.FindFirstBefore(aSimTime);
   }
   else
   {
      return mBehaviorTreeStateArray.FindFirstAfter(aSimTime);
   }
}
*/
void rv::ResultPlatform::AddSensorModeDefinition(MsgEmitterModeDefinition* aDefPtr)
{
   mSensorModeDefinition[std::make_pair(aDefPtr->partName(), aDefPtr->modeName())] =
      ut::make_unique<MsgEmitterModeDefinition>(*aDefPtr);
}

void rv::ResultPlatform::AddWeaponModeDefinition(MsgEmitterModeDefinition* aDefPtr)
{
   mWeaponModeDefinition[std::make_pair(aDefPtr->partName(), aDefPtr->modeName())] =
      ut::make_unique<MsgEmitterModeDefinition>(*aDefPtr);
}

int rv::ResultPlatform::GetStartState()
{
   if (!mTrackChangeStartState.empty())
   {
      return 2;
   }
   else if (!mDetectChangeStartState.empty())
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

std::map<float, int> rv::ResultPlatform::GetStateChanges(float aStart, float aStop, int& aStartState)
{
   bool startPassed = false;
   aStartState      = 0;
   std::map<float, int>     retval;
   std::map<size_t, size_t> stateCounter;
   std::map<int, bool>      startState;
   stateCounter[1] = mDetectChangeStartState.size();
   stateCounter[2] = mTrackChangeStartState.size();
   stateCounter[3] = 0;
   startState[1]   = false;
   startState[2]   = false;
   startState[3]   = false;

   BaseMessageArray::const_iterator      tit  = mTrackedArray.begin();
   DetectionChangeArray::const_iterator  dit  = mDetectedChangeArray.begin(); // do these arrays works as anticipated?
   std::map<double, int>::const_iterator wit  = mFiredStateMap.begin();
   bool                                  done = false;
   while (done == false)
   {
      float                     tTime = FLT_MAX;
      float                     dTime = FLT_MAX;
      float                     wTime = FLT_MAX;
      MsgSensorDetectionChange* msd   = nullptr;
      MsgBase*                  tm    = nullptr;
      if (dit != mDetectedChangeArray.end())
      {
         msd   = (MsgSensorDetectionChange*)*dit;
         dTime = msd->simTime();
      }
      if (tit != mTrackedArray.end())
      {
         tm    = (MsgBase*)*tit;
         tTime = tm->simTime();
      }
      if (wit != mFiredStateMap.end())
      {
         wTime = wit->first;
      }
      if ((dTime < tTime) && (dTime < wTime) && (msd != nullptr))
      {
         if (dTime <= aStop)
         {
            if (msd->detected())
            {
               stateCounter[1]++;
               if ((stateCounter[1] == 1) && (dTime > aStart))
               {
                  startPassed = true;
                  if (stateCounter[2] == 0)
                  {
                     retval[dTime] = 1;
                  }
                  // start detect
               }
               if (!startPassed)
               {
                  startState[1] = true;
               }
            }
            else
            {
               if (stateCounter[1] > 0)
               {
                  stateCounter[1]--;
                  if ((stateCounter[1] == 0) && (dTime > aStart))
                  {
                     startPassed = true;
                     if (stateCounter[2] == 0)
                     {
                        retval[dTime] = 0;
                     }
                  }
                  if ((!startPassed) && (stateCounter[1] == 0))
                  {
                     startState[1] = false;
                  }
               }
            }
            ++dit;
         }
      }
      else if ((tTime <= dTime) && (tTime < wTime) && (tm != nullptr))
      {
         if (tTime <= aStop) // with more iterators we will walk through all of them in order of appearance
         {
            if (tm->GetMessageId() == MsgSensorTrackCreated::cMESSAGE_ID)
            {
               stateCounter[2]++;
               if ((stateCounter[2] == 1) && (tTime > aStart))
               {
                  startPassed   = true;
                  retval[tTime] = 2;
                  // start track
               }
               if (!startPassed)
               {
                  startState[2] = true;
               }
            }
            else // tm->GetMessageId() == ViEventPipe::MsgSensorTrackDrop::cMESSAGE_ID
            {
               if (stateCounter[2] > 0)
               {
                  stateCounter[2]--;
                  if ((stateCounter[2] == 0) && (tTime > aStart))
                  {
                     startPassed = true;
                     if (stateCounter[1] > 0)
                     {
                        retval[tTime] = 1;
                     }
                     else
                     {
                        retval[tTime] = 0;
                     }
                  }
                  if ((!startPassed) && (stateCounter[2] == 0))
                  {
                     startState[2] = false;
                  }
               }
            }
            ++tit;
         }
      }
      else if ((wTime <= dTime) && (wTime <= tTime) && (wit != mFiredStateMap.end()))
      {
         if (wTime <= aStop) // with more iterators we will walk through all of them in order of appearance
         {
            if (wit->second > 0)
            {
               if ((stateCounter[3] == 0) && (wTime > aStart))
               {
                  startPassed   = true;
                  retval[wTime] = 3;
                  // start fire event
               }
               if (!startPassed)
               {
                  startState[3] = true;
               }
               stateCounter[3] += wit->second;
            }
            else
            {
               if (stateCounter[3] > 0)
               {
                  stateCounter[3] += wit->second;
                  if ((stateCounter[3] == 0) && (wTime > aStart))
                  {
                     startPassed = true;
                     if (stateCounter[2] > 0)
                     {
                        retval[wTime] = 2;
                     }
                     else if (stateCounter[1] > 0)
                     {
                        retval[wTime] = 1;
                     }
                     else
                     {
                        retval[wTime] = 0;
                     }
                  }
                  if ((!startPassed) && (stateCounter[3] == 0))
                  {
                     startState[3] = false;
                  }
               }
            }
            ++wit;
         }
      }
      if ((tTime > aStop) && (dTime > aStop) && (wTime > aStop)) // this will need to check all iterators eventually
      {
         done = true;
      }
   }
   if (startState[3])
   {
      aStartState = 3;
   }
   else if (startState[2])
   {
      aStartState = 2;
   }
   else if (startState[1])
   {
      aStartState = 1;
   }
   return retval;
}

bool rv::ResultPlatform::IsActive(double aTime) const
{
   return ((mStartTime <= aTime) && ((mEndTime >= aTime) || (mEndTime < 0)));
}

void rv::ResultPlatform::AddStartState(MsgSensorDetectionChange* aMsg)
{
   DetectionChangeIndex index(aMsg->ownerIndex(), aMsg->sensorName());
   mDetectChangeStartState[index] = *aMsg;
   mDetectChangeStartState[index].detected(true);
}

void rv::ResultPlatform::RemoveStartState(MsgSensorTrackCreated* aMsg)
{
   TrackChangeIndex index(aMsg->ownerIndex(), aMsg->trackId().localTrackNumber());
   mTrackChangeStartState.erase(index);
}

void rv::ResultPlatform::RemoveStartState(MsgSensorDetectionChange* aMsg)
{
   DetectionChangeIndex index(aMsg->ownerIndex(), aMsg->sensorName());
   mDetectChangeStartState.erase(index);
}

void rv::ResultPlatform::AddStartState(MsgSensorTrackCreated* aMsg)
{
   TrackChangeIndex index(aMsg->ownerIndex(), aMsg->trackId().localTrackNumber());
   mTrackChangeStartState[index] = *aMsg;
}

void rv::ResultPlatform::AddBaseMessage(MsgBase* aMsg, bool aAtBegin)
{
   mMessageMap.Add(aMsg, aAtBegin);
}

// Remove a message from the array.
// @param aMsg message to remove
// @param aAtBegin flag indicating removing from the beginning.
void rv::ResultPlatform::RemoveBaseMessage(MsgBase* aMsg, bool aAtBegin)
{
   mMessageMap.Remove(aMsg, aAtBegin);
}

/** Add a string sorted message to the array.
 * @param aMsg message to remove
 * @param aAtBegin flag indicating removing from the beginning. */
void rv::ResultPlatform::AddStringKeyedMessage(MsgBase* aMsg, const std::string& aKey, bool aAtBegin)
{
   mStringKeyedMessageMap[aMsg->GetMessageId()][aKey].push(aMsg, aAtBegin);
}

/** Remove a string sorted message from the array.
 * @param aMsg message to remove
 * @param aKey the key it is sorted into
 * @param aAtBegin flag indicating removing from the beginning. */
void rv::ResultPlatform::RemoveStringKeyedMessage(MsgBase* aMsg, const std::string& aKey, bool aAtBegin)
{
   mStringKeyedMessageMap[aMsg->GetMessageId()][aKey].pop(aMsg, aAtBegin);
}

void rv::ResultPlatform::AddPartMessage(MsgBase* aMsg, char aPartType, const std::string& aPartName, bool aAtBegin)
{
   mPartMap.AddMessage(aMsg, static_cast<rv::PartType::Value>(aPartType), aPartName, aAtBegin);
}

void rv::ResultPlatform::RemovePartMessage(MsgBase* aMsg, char aPartType, const std::string& aPartName, bool aAtBegin)
{
   mPartMap.RemoveMessage(aMsg, static_cast<rv::PartType::Value>(aPartType), aPartName, aAtBegin);
}

void rv::ResultPlatform::RemoveMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   mTrackedArray.pop(aMsg, aAtBegin);
   if (aAtBegin)
   {
      MsgSensorTrackCreated generated;
      generated.ownerIndex(aMsg->ownerIndex());
      generated.trackId(aMsg->trackId());
      generated.targetIndexValid(aMsg->targetIndexValid());
      generated.targetIndex(aMsg->targetIndex());
      AddStartState(&generated);
   }
}

void rv::ResultPlatform::AddMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   mTrackedArray.push(aMsg, aAtBegin);
   if (aAtBegin)
   {
      MsgSensorTrackCreated generated;
      generated.ownerIndex(aMsg->ownerIndex());
      generated.trackId(aMsg->trackId());
      generated.targetIndexValid(aMsg->targetIndexValid());
      generated.targetIndex(aMsg->targetIndex());
      RemoveStartState(&generated);
   }
}

void rv::ResultPlatform::RemoveMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   mTrackedArray.pop(aMsg, aAtBegin);
   if (aAtBegin)
   {
      AddStartState(aMsg);
   }
}

void rv::ResultPlatform::AddMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   mTrackedArray.push(aMsg, aAtBegin);
   if (aAtBegin)
   {
      RemoveStartState(aMsg);
   }
}


void rv::ResultPlatform::AddMessage(MsgWeaponFired* aMsg)
{
   mFiredStateMap[aMsg->simTime()]++;
}

void rv::ResultPlatform::AddMessage(MsgWeaponTerminated* aMsg)
{
   mFiredStateMap[aMsg->simTime()]--;
}

void rv::ResultPlatform::RemoveMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin)
{
   mDetectedChangeArray.pop(aMsg, aAtBegin);
   if (aAtBegin)
   {
      if (aMsg->detected()) // a start state has fallen off the early edge of the buffer, we need to keep track
      {
         AddStartState(aMsg);
         // add it to the start state
      }
      else // a stop state has fallen off the early edge of the buffer, we don't need to track this one anymore
      {
         RemoveStartState(aMsg);
         // if there is parallel in the start state, get rid of it
      }
   }
}

void rv::ResultPlatform::AddMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin)
{
   mDetectedChangeArray.push(aMsg, aAtBegin);
   if (aAtBegin)
   {
      if (aMsg->detected()) // a start state has come into the early edge of the buffer, we don't need to track it anymore
      {
         RemoveStartState(aMsg);
         // remove it from the start state
      }
      else // a stop state had come into the early edge of the buffer, presumably there was a correlated start
      {
         AddStartState(aMsg);
         // create a start state to put in the start state
      }
   }
}

void rv::ResultPlatform::AddMessage(MsgPlatformStatus* aMsg, bool aAtBegin)
{
   if (aMsg->removed())
   {
      mEndTime = aMsg->simTime();
   }
   AddBaseMessage(aMsg, aAtBegin);
}

void rv::ResultPlatform::RemoveMessage(MsgDetectAttempt* aMsg, bool aAtBegin)
{
   mDetectionAttemptMap[aMsg->sensorName()][aMsg->targetPlatformIndex()].pop(aMsg, aAtBegin);
}

void rv::ResultPlatform::AddMessage(MsgDetectAttempt* aMsg, bool aAtBegin)
{
   mDetectionAttemptMap[aMsg->sensorName()][aMsg->targetPlatformIndex()].push(aMsg, aAtBegin);
}

void rv::ResultPlatform::PartMap::AddMessage(MsgBase*            aMessage,
                                             rv::PartType::Value aPartType,
                                             const std::string&  aPartName,
                                             bool                aAtBegin)
{
   mPartMap[aPartType][aPartName].Add(aMessage, aAtBegin);
}

void rv::ResultPlatform::PartMap::RemoveMessage(MsgBase*            aMessage,
                                                rv::PartType::Value aPartType,
                                                const std::string&  aPartName,
                                                bool                aAtBegin)
{
   mPartMap[aPartType][aPartName].Remove(aMessage, aAtBegin);
}

const rv::ResultPlatform::PartMap::NameMap* rv::ResultPlatform::PartMap::GetParts(rv::PartType::Value& aType) const
{
   auto it = mPartMap.find(aType);
   if (it != mPartMap.end())
   {
      return &it->second;
   }
   else
   {
      return nullptr;
   }
}

bool rv::ResultPlatform::MessageMap::HasMessageOfType(int aMessageType) const
{
   auto messageMapEntry = mMap.find(aMessageType);
   if (messageMapEntry != mMap.end())
   {
      return !messageMapEntry->second.empty();
   }
   return false;
}
