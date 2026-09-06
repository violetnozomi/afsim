// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvInteractionDb.hpp"

namespace
{
std::string MakeTrackMessage(bool aSensorNameValid, const std::string& aSensorName, const rv::Track_Id& aTrackNumber)
{
   std::string msg;
   if (aSensorNameValid)
   {
      msg += "\nSensor: ";
      msg += aSensorName;
   }
   msg += " \nTrackId: ";
   msg += std::to_string(aTrackNumber.localTrackNumber());
   msg += ":";
   msg += aTrackNumber.owner();
   return msg;
}
} // namespace

void rv::InteractionDb::AddPairedStartInteraction(unsigned int       aOwner,
                                                  unsigned int       aInteractor,
                                                  unsigned int       aInteractee,
                                                  unsigned int       aMessageIndex,
                                                  bool               aAtBegin,
                                                  const std::string& aMessageType,
                                                  double             aSimTime,
                                                  const std::string& aData,
                                                  bool               aCorrelateByData)
{
   mInteractionArrayMap[aOwner]
      .push(aMessageIndex, aAtBegin, aMessageType, aSimTime, true, aInteractor, aInteractee, aData, aCorrelateByData);
}

void rv::InteractionDb::AddPairedStopInteraction(unsigned int       aOwner,
                                                 unsigned int       aInteractor,
                                                 unsigned int       aInteractee,
                                                 unsigned int       aMessageIndex,
                                                 bool               aAtBegin,
                                                 const std::string& aMessageType,
                                                 double             aSimTime,
                                                 const std::string& aData,
                                                 bool               aCorrelateByData)
{
   mInteractionArrayMap[aOwner]
      .push(aMessageIndex, aAtBegin, aMessageType, aSimTime, false, aInteractor, aInteractee, aData, aCorrelateByData);
}

void rv::InteractionDb::AddUnpairedInteraction(unsigned int       aOwner,
                                               unsigned int       aInteractor,
                                               unsigned int       aInteractee,
                                               unsigned int       aMessageIndex,
                                               bool               aAtBegin,
                                               const std::string& aMessageType,
                                               double             aSimTime,
                                               const std::string& aAttackType)
{
   mUnpairedInteractionArrayMap[aOwner]
      .push(aMessageIndex, aAtBegin, aMessageType, aSimTime, true, aInteractor, aInteractee, aAttackType);
}

void rv::InteractionDb::RemovePairedInteraction(unsigned int aInteractor,
                                                unsigned int aInteractee,
                                                unsigned int aMessageIndex,
                                                bool         aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aInteractor, aInteractee, aMessageIndex, aAtBegin);
}

void rv::InteractionDb::RemoveUnpairedInteraction(unsigned int aInteractor,
                                                  unsigned int aInteractee,
                                                  unsigned int aMessageIndex,
                                                  bool         aAtBegin)
{
   RemoveMessagePrivate(mUnpairedInteractionArrayMap, aInteractor, aInteractee, aMessageIndex, aAtBegin);
}

void rv::InteractionDb::RemoveMessage(MsgMessageHop* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mUnpairedInteractionArrayMap,
                        aMsg->xmtrPlatformIndex(),
                        aMsg->rcvrPlatformIndex(),
                        aMsg->GetMessageIndex(),
                        aAtBegin);
}

void rv::InteractionDb::AddMessage(rv::MsgSensorDetectionChange* aMsg, bool aAtBegin)
{
   mInteractionArrayMap[aMsg->ownerIndex()].push(aMsg->GetMessageIndex(),
                                                 aAtBegin,
                                                 "Detect",
                                                 aMsg->simTime(),
                                                 aMsg->detected(),
                                                 aMsg->ownerIndex(),
                                                 aMsg->targetIndex(),
                                                 "\nSensor: " + aMsg->sensorName(),
                                                 true);
   if (aMsg->ownerIndex() != aMsg->targetIndex())
   {
      mInteractionArrayMap[aMsg->targetIndex()].push(aMsg->GetMessageIndex(),
                                                     aAtBegin,
                                                     "Detect",
                                                     aMsg->simTime(),
                                                     aMsg->detected(),
                                                     aMsg->ownerIndex(),
                                                     aMsg->targetIndex(),
                                                     "\nSensor: " + aMsg->sensorName(),
                                                     true);
   }
}

void rv::InteractionDb::AddMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   std::string msg = MakeTrackMessage(aMsg->sensorNameValid(), aMsg->sensorName(), aMsg->trackId());
   mInteractionArrayMap[aMsg->ownerIndex()].push(aMsg->GetMessageIndex(),
                                                 aAtBegin,
                                                 "Track",
                                                 aMsg->simTime(),
                                                 true,
                                                 aMsg->ownerIndex(),
                                                 aMsg->targetIndex(),
                                                 msg,
                                                 true);
   if (aMsg->ownerIndex() != aMsg->targetIndex())
   {
      mInteractionArrayMap[aMsg->targetIndex()].push(aMsg->GetMessageIndex(),
                                                     aAtBegin,
                                                     "Track",
                                                     aMsg->simTime(),
                                                     true,
                                                     aMsg->ownerIndex(),
                                                     aMsg->targetIndex(),
                                                     msg,
                                                     true);
   }
}

void rv::InteractionDb::AddMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   std::string msg = MakeTrackMessage(aMsg->sensorNameValid(), aMsg->sensorName(), aMsg->trackId());
   mInteractionArrayMap[aMsg->ownerIndex()].push(aMsg->GetMessageIndex(),
                                                 aAtBegin,
                                                 "Track",
                                                 aMsg->simTime(),
                                                 false,
                                                 aMsg->ownerIndex(),
                                                 aMsg->targetIndex(),
                                                 msg,
                                                 true);
   if (aMsg->ownerIndex() != aMsg->targetIndex())
   {
      mInteractionArrayMap[aMsg->targetIndex()].push(aMsg->GetMessageIndex(),
                                                     aAtBegin,
                                                     "Track",
                                                     aMsg->simTime(),
                                                     false,
                                                     aMsg->ownerIndex(),
                                                     aMsg->targetIndex(),
                                                     msg,
                                                     true);
   }
}

void rv::InteractionDb::AddMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
   std::string msg = MakeTrackMessage(false, "", aMsg->trackId());
   mInteractionArrayMap[aMsg->ownerIndex()].push(aMsg->GetMessageIndex(),
                                                 aAtBegin,
                                                 "LocalTrack",
                                                 aMsg->simTime(),
                                                 true,
                                                 aMsg->ownerIndex(),
                                                 aMsg->targetIndex(),
                                                 msg,
                                                 true);
   if (aMsg->ownerIndex() != aMsg->targetIndex())
   {
      mInteractionArrayMap[aMsg->targetIndex()].push(aMsg->GetMessageIndex(),
                                                     aAtBegin,
                                                     "LocalTrack",
                                                     aMsg->simTime(),
                                                     true,
                                                     aMsg->ownerIndex(),
                                                     aMsg->targetIndex(),
                                                     msg,
                                                     true);
   }
}

void rv::InteractionDb::AddMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
   std::string msg = MakeTrackMessage(false, "", aMsg->trackId());
   mInteractionArrayMap[aMsg->ownerIndex()].push(aMsg->GetMessageIndex(),
                                                 aAtBegin,
                                                 "LocalTrack",
                                                 aMsg->simTime(),
                                                 false,
                                                 aMsg->ownerIndex(),
                                                 aMsg->targetIndex(),
                                                 msg,
                                                 true);
   if (aMsg->ownerIndex() != aMsg->targetIndex())
   {
      mInteractionArrayMap[aMsg->targetIndex()].push(aMsg->GetMessageIndex(),
                                                     aAtBegin,
                                                     "LocalTrack",
                                                     aMsg->simTime(),
                                                     false,
                                                     aMsg->ownerIndex(),
                                                     aMsg->targetIndex(),
                                                     msg,
                                                     true);
   }
}

void rv::InteractionDb::AddMessage(rv::MsgTaskUpdate* aMsg, bool aAtBegin)
{
   const std::string msg              = "\nTask Type: " + aMsg->taskType();
   auto&             assignerBookends = mMultiInteractionArrayMap[aMsg->assignerPlatform()];
   auto              key              = std::pair<std::string, int>("Task", aMsg->taskId());
   auto              val              = assignerBookends.find(key);
   bool              start            = aMsg->state() == 0;
   if (val == assignerBookends.end())
   {
      assignerBookends.emplace(key, std::pair<MessageIndex, MessageIndex>(0U, 0U));
      val = assignerBookends.find(key);
   }
   if ((start && (val->second.first == 0U || val->second.first == aMsg->GetMessageIndex())) ||
       (!start && (val->second.second == 0U || val->second.second == aMsg->GetMessageIndex())))
   {
      if (start)
      {
         val->second.first = aMsg->GetMessageIndex();
      }
      else
      {
         val->second.second = aMsg->GetMessageIndex();
      }
      mInteractionArrayMap[aMsg->assignerPlatform()].push(aMsg->GetMessageIndex(),
                                                          aAtBegin,
                                                          "Task",
                                                          aMsg->simTime(),
                                                          start,
                                                          aMsg->assignerPlatform(),
                                                          aMsg->assigneePlatform(),
                                                          msg);
      if (aMsg->assignerPlatform() != aMsg->assigneePlatform())
      {
         auto& assigneeBookends = mMultiInteractionArrayMap[aMsg->assignerPlatform()][key];
         if (start)
         {
            assigneeBookends.first = aMsg->GetMessageIndex();
         }
         else
         {
            assigneeBookends.second = aMsg->GetMessageIndex();
         }
         mInteractionArrayMap[aMsg->assigneePlatform()].push(aMsg->GetMessageIndex(),
                                                             aAtBegin,
                                                             "Task",
                                                             aMsg->simTime(),
                                                             start,
                                                             aMsg->assignerPlatform(),
                                                             aMsg->assigneePlatform(),
                                                             msg);
      }
   }
}

void rv::InteractionDb::AddMessage(rv::MsgWeaponFired* aMsg, bool aAtBegin)
{
   mOneTimeInteractionArrayMap[aMsg->firingPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                 aAtBegin,
                                                                 "Fire",
                                                                 aMsg->simTime(),
                                                                 true,
                                                                 aMsg->firingPlatformIndex(),
                                                                 aMsg->targetPlatformIndex(),
                                                                 std::string(""));
   if (aMsg->firingPlatformIndex() != aMsg->targetPlatformIndex())
   {
      mOneTimeInteractionArrayMap[aMsg->targetPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                    aAtBegin,
                                                                    "Fire",
                                                                    aMsg->simTime(),
                                                                    true,
                                                                    aMsg->firingPlatformIndex(),
                                                                    aMsg->targetPlatformIndex(),
                                                                    std::string(""));
   }
}

void rv::InteractionDb::AddMessage(rv::MsgWeaponTerminated* aMsg, bool aAtBegin)
{
   mOneTimeInteractionArrayMap[aMsg->firingPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                 aAtBegin,
                                                                 "Fire",
                                                                 aMsg->simTime(),
                                                                 false,
                                                                 aMsg->firingPlatformIndex(),
                                                                 aMsg->targetPlatformIndex(),
                                                                 std::string(""));
   if (aMsg->firingPlatformIndex() != aMsg->targetPlatformIndex())
   {
      mOneTimeInteractionArrayMap[aMsg->targetPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                    aAtBegin,
                                                                    "Fire",
                                                                    aMsg->simTime(),
                                                                    false,
                                                                    aMsg->firingPlatformIndex(),
                                                                    aMsg->targetPlatformIndex(),
                                                                    std::string(""));
   }
}

void rv::InteractionDb::AddMessage(rv::MsgMessageReceived* aMsg, bool aAtBegin)
{
   mUnpairedInteractionArrayMap[aMsg->xmtrPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                aAtBegin,
                                                                "Message",
                                                                aMsg->simTime(),
                                                                true,
                                                                aMsg->xmtrPlatformIndex(),
                                                                aMsg->rcvrPlatformIndex(),
                                                                std::string(""));
   if (aMsg->xmtrPlatformIndex() != aMsg->rcvrPlatformIndex())
   {
      mUnpairedInteractionArrayMap[aMsg->rcvrPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                   aAtBegin,
                                                                   "Message",
                                                                   aMsg->simTime(),
                                                                   true,
                                                                   aMsg->xmtrPlatformIndex(),
                                                                   aMsg->rcvrPlatformIndex(),
                                                                   std::string(""));
   }
}

void rv::InteractionDb::RemoveMessagePrivate(std::map<int, InteractionArray>& aArray,
                                             unsigned int                     aPlatformIndex1,
                                             unsigned int                     aPlatformIndex2,
                                             unsigned int                     aMessageIndex,
                                             bool                             aAtBegin)
{
   aArray[aPlatformIndex1].pop(aMessageIndex, aAtBegin);
   if (aPlatformIndex1 != aPlatformIndex2)
   {
      aArray[aPlatformIndex2].pop(aMessageIndex, aAtBegin);
   }
}

void rv::InteractionDb::AddMessage(MsgJammingRequestInitiated* aMsg, bool aAtBegin)
{
   mInteractionArrayMap[aMsg->srcPlatform()].push(aMsg->GetMessageIndex(),
                                                  aAtBegin,
                                                  "Jam",
                                                  aMsg->simTime(),
                                                  true,
                                                  aMsg->srcPlatform(),
                                                  aMsg->target(),
                                                  std::string(""));
   if (aMsg->srcPlatform() != aMsg->target())
   {
      mInteractionArrayMap[aMsg->target()].push(aMsg->GetMessageIndex(),
                                                aAtBegin,
                                                "Jam",
                                                aMsg->simTime(),
                                                true,
                                                aMsg->srcPlatform(),
                                                aMsg->target(),
                                                std::string(""));
   }
}

void rv::InteractionDb::AddMessage(MsgJammingRequestCanceled* aMsg, bool aAtBegin)
{
   mInteractionArrayMap[aMsg->srcPlatform()].push(aMsg->GetMessageIndex(),
                                                  aAtBegin,
                                                  "Jam",
                                                  aMsg->simTime(),
                                                  false,
                                                  aMsg->srcPlatform(),
                                                  aMsg->target(),
                                                  std::string(""));
   if (aMsg->srcPlatform() != aMsg->target())
   {
      mInteractionArrayMap[aMsg->target()].push(aMsg->GetMessageIndex(),
                                                aAtBegin,
                                                "Jam",
                                                aMsg->simTime(),
                                                false,
                                                aMsg->srcPlatform(),
                                                aMsg->target(),
                                                std::string(""));
   }
}

void rv::InteractionDb::AddMessage(MsgMessageHop* aMsg, bool aAtBegin)
{
   mUnpairedInteractionArrayMap[aMsg->xmtrPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                aAtBegin,
                                                                "Message",
                                                                aMsg->simTime(),
                                                                true,
                                                                aMsg->xmtrPlatformIndex(),
                                                                aMsg->rcvrPlatformIndex(),
                                                                std::string(""));
   if (aMsg->xmtrPlatformIndex() != aMsg->rcvrPlatformIndex())
   {
      mUnpairedInteractionArrayMap[aMsg->rcvrPlatformIndex()].push(aMsg->GetMessageIndex(),
                                                                   aAtBegin,
                                                                   "Message",
                                                                   aMsg->simTime(),
                                                                   true,
                                                                   aMsg->xmtrPlatformIndex(),
                                                                   aMsg->rcvrPlatformIndex(),
                                                                   std::string(""));
   }
}

void rv::InteractionDb::RemoveMessage(rv::MsgSensorDetectionChange* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->ownerIndex(), aMsg->targetIndex(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgSensorTrackCreated* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->ownerIndex(), aMsg->targetIndex(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgSensorTrackDrop* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->ownerIndex(), aMsg->targetIndex(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgLocalTrackCreated* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->ownerIndex(), aMsg->targetIndex(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgLocalTrackDrop* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->ownerIndex(), aMsg->targetIndex(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgTaskUpdate* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap,
                        aMsg->assignerPlatform(),
                        aMsg->assigneePlatform(),
                        aMsg->GetMessageIndex(),
                        aAtBegin);
}

void rv::InteractionDb::RemoveMessage(rv::MsgMessageReceived* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mUnpairedInteractionArrayMap,
                        aMsg->xmtrPlatformIndex(),
                        aMsg->rcvrPlatformIndex(),
                        aMsg->GetMessageIndex(),
                        aAtBegin);
}

void rv::InteractionDb::RemoveMessage(MsgJammingRequestInitiated* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->srcPlatform(), aMsg->target(), aMsg->GetMessageIndex(), aAtBegin);
}

void rv::InteractionDb::RemoveMessage(MsgJammingRequestCanceled* aMsg, bool aAtBegin)
{
   RemoveMessagePrivate(mInteractionArrayMap, aMsg->srcPlatform(), aMsg->target(), aMsg->GetMessageIndex(), aAtBegin);
}

rv::InteractionDb::InteractionArray::range_pair
rv::InteractionDb::GetRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const
{
   return mInteractionArrayMap[aPlatform].GetDataInRange(aStart, aEnd, aValid);
}

rv::InteractionDb::InteractionArray::range_pair
rv::InteractionDb::GetOneTimeRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const
{
   return mOneTimeInteractionArrayMap[aPlatform].GetDataInRange(aStart, aEnd, aValid);
}

rv::InteractionDb::InteractionArray::range_pair
rv::InteractionDb::GetUnpairedRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const
{
   return mUnpairedInteractionArrayMap[aPlatform].GetDataInRange(aStart, aEnd, aValid);
}

rv::InteractionDb::InteractionArray::InteractionArray()
{
   mMinMessageIndex = mMaxMessageIndex = 0;
}

rv::InteractionDb::InteractionArray::~InteractionArray()
{
   while (!mData.empty())
   {
      mData.pop_front();
   }
}

void rv::InteractionDb::InteractionArray::push(unsigned int       aIndex,
                                               bool               aAtBegin,
                                               const std::string& aType,
                                               float              aTime,
                                               bool               aStart,
                                               int                aSource,
                                               int                aTarget,
                                               const std::string& aData,
                                               bool               aCorrelateByDataDictionary)
{
   unsigned int id = 0U;
   if (aCorrelateByDataDictionary)
   {
      auto entry = mCorrelationDictionary.find(aData);
      if (entry == mCorrelationDictionary.end())
      {
         id = static_cast<unsigned int>(mCorrelationDictionary.size()) +
              1; // add one in case there is ever a need to mix correlated with uncorrelated events
         mCorrelationDictionary[aData] = id;
      }
      else
      {
         id = entry->second;
      }
   }
   //   std::cout << "at time " << aTime << " " << aStart << " " << aSource << " " << aTarget << " " << aData << std::endl;

   if (mData.empty())
   {
      mData.push_back(new Interaction(aTime, aType, aIndex, aStart, aSource, aTarget, aData, id));
      mMinTime = mMaxTime = aTime;
      mMinMessageIndex = mMaxMessageIndex = aIndex;
   }
   else if (aAtBegin)
   {
      mMinTime         = aTime;
      mMinMessageIndex = aIndex;
      mData.push_front(new Interaction(aTime, aType, aIndex, aStart, aSource, aTarget, aData, id));
   }
   else
   {
      assert(aIndex > mMaxMessageIndex);
      double rTime = aTime;
      // this used to just be a straight assert, but the WsfExtEntityDeadReckon is not that reliable
      // I am trying a hundredth of a second threshold on it
      if (rTime < mMaxTime)
      {
         if (mMaxTime - rTime < 0.01)
         {
            rTime = mMaxTime;
         }
         else
         {
            assert(mMaxTime <= rTime);
         }
      }
      mMaxTime         = rTime;
      mMaxMessageIndex = aIndex;
      mData.push_back(new Interaction(aTime, aType, aIndex, aStart, aSource, aTarget, aData, id));
   }
}

void rv::InteractionDb::InteractionArray::pop(unsigned int aIndex, bool aAtBegin)
{
   if (aAtBegin)
   {
      assert(mData.front()->mIndex == aIndex);
      Interaction* data = mData.front();
      mData.pop_front();
      delete data;
      if (!mData.empty())
      {
         mMinTime         = mData.front()->mTime;
         mMinMessageIndex = mData.front()->mIndex;
      }
   }
   else
   {
      assert(mData.back()->mIndex == aIndex);
      Interaction* data = mData.back();
      mData.pop_back();
      delete data;
      if (!mData.empty())
      {
         mMaxTime         = mData.back()->mTime;
         mMaxMessageIndex = mData.back()->mIndex;
      }
   }
}

rv::InteractionDb::InteractionArray::const_iterator rv::InteractionDb::InteractionArray::FindFirstBefore(float aTime) const
{
   Array::const_iterator i =
      std::upper_bound(mData.begin(), mData.end(), aTime, rv::InteractionDb::InteractionArray::ComparePred());
   if (i != mData.begin())
   {
      --i;
      return i;
   }
   return mData.end();
}

// todo implement a faster version
rv::InteractionDb::InteractionArray::const_iterator rv::InteractionDb::InteractionArray::FindFirstAfter(float aTime) const
{
   if (mData.empty())
      return mData.end();
   const_iterator i = mData.end();
   --i;
   while (true)
   {
      if (aTime < (*i)->mTime)
      {
         if (i == mData.begin())
         {
            break;
         }
         --i;
      }
      else
      {
         ++i;
         break;
      }
   }
   return i;
}

rv::InteractionDb::InteractionArray::range_pair rv::InteractionDb::InteractionArray::GetDataInRange(float aStart,
                                                                                                    float aEnd,
                                                                                                    bool& aValid) const
{
   range_pair retval;
   retval.first  = FindFirstAfter(aStart);
   retval.second = FindFirstBefore(aEnd);

   if ((retval.first != mData.end()) && (retval.second != mData.end())) // found some data
   {
      if (((*retval.first)->mTime < aEnd) && ((*retval.second)->mTime > aStart)) // found some data in range
      {
         aValid = true;
         return retval;
      }
   }
   aValid        = false;
   retval.first  = mData.end();
   retval.second = mData.end();
   return retval;
}
// find first after start, find first before end
