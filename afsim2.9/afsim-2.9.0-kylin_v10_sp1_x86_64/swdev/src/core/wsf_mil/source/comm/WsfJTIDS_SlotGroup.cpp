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

#include "WsfJTIDS_SlotGroup.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <sstream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtStringUtil.hpp"
#include "WsfComm.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommSlotGroupLayerJTIDS.hpp"
#include "WsfMessage.hpp"
#include "WsfMessageTable.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

std::vector<WsfJTIDS_SlotGroup::NetGroup> WsfJTIDS_SlotGroup::mNetGroups;

//! Construct a slot group
WsfJTIDS_SlotGroup::WsfJTIDS_SlotGroup(wsf::comm::SlotGroupLayerJTIDS* aLayerPtr,
                                       const std::string&              aGroupName,
                                       const std::string&              aSourceGroupName)
   : mLayerPtr(aLayerPtr)
   , mGroupName(aGroupName)
   , mNetwork(0)
   , mTSEC(0)
   , mMSEC(0)
   , mNPG(0)
   , mAccessMode(cDEDICATED)
   , mPackingLimit(cSTD)
   , mSlotsPerFrame(1)
   , mPerUnitSlotsPerFrame(-1)
   , mSlots()
   , mTransmitSlots()
   , mQueueLimit(99999)
   , mTransmitQueue()
   , mPairedRelaySourceName(nullptr)
   , mPairedRelaySlotOffset(6)
   , mRelay(false)
   , mPairedRelayPtr(nullptr)
   , mNetGroupIndex(0)
   , mIsUsingTransmitEvents(false)
{
   // Make sure the dummy net group at index 0 exists as index 0 is used to indicate
   // that the slot group is not a member of any net group.

   if (mNetGroups.empty())
   {
      mNetGroups.emplace_back(WsfStringId(nullptr), 0, 0);
   }

   // If this is a relay slot group then set the name of source slot group.
   if (!aSourceGroupName.empty())
   {
      mPairedRelaySourceName.SetString(aSourceGroupName);
   }
}

//! Copy constructor.
WsfJTIDS_SlotGroup::WsfJTIDS_SlotGroup(const WsfJTIDS_SlotGroup& aSrc, wsf::comm::SlotGroupLayerJTIDS* aLayerPtr)
   : mLayerPtr(aLayerPtr)
   , mGroupName(aSrc.mGroupName)
   , mNetwork(aSrc.mNetwork)
   , mTSEC(aSrc.mTSEC)
   , mMSEC(aSrc.mMSEC)
   , mNPG(aSrc.mNPG)
   , mAccessMode(aSrc.mAccessMode)
   , mPackingLimit(aSrc.mPackingLimit)
   , mSlotsPerFrame(aSrc.mSlotsPerFrame)
   , mPerUnitSlotsPerFrame(aSrc.mPerUnitSlotsPerFrame)
   , mTimeSlotBlocks(aSrc.mTimeSlotBlocks)
   , mSlots()
   , mTransmitSlots()
   , mQueueLimit(aSrc.mQueueLimit)
   , mTransmitQueue()
   , mPairedRelaySourceName(aSrc.mPairedRelaySourceName)
   , mPairedRelaySlotOffset(aSrc.mPairedRelaySlotOffset)
   , mRelay(aSrc.mRelay)
   , mPairedRelayPtr(nullptr)
   , mNetGroupIndex(0)
   , mIsUsingTransmitEvents(false)
{
}

WsfJTIDS_SlotGroup::~WsfJTIDS_SlotGroup()
{
   PurgeQueuedMessages();
   RemoveFromNetwork();
}

// static
void WsfJTIDS_SlotGroup::ClearNetGroups()
{
   mNetGroups.clear();
}

//! Add the slot group to the corresponding network.
void WsfJTIDS_SlotGroup::AddToNetwork()
{
   // Determine if the network group ID already exists.  If it doesn't then create it.

   mNetGroupIndex = 0;
   for (std::vector<NetGroup>::size_type i = 1; i < mNetGroups.size(); ++i)
   {
      if ((mGroupName == mNetGroups[i].mGroupName) && (mNetwork == mNetGroups[i].mNetwork) && (mTSEC == mNetGroups[i].mTSEC))
      {
         mNetGroupIndex = i;
         break;
      }
   }

   if (mNetGroupIndex == 0)
   {
      mNetGroupIndex = mNetGroups.size();
      mNetGroups.emplace_back(mGroupName, mNetwork, mTSEC);
   }

   // Make sure this group isn't already in the network group.

   std::vector<WsfJTIDS_SlotGroup*>& groupList = mNetGroups[mNetGroupIndex].mGroupList;
   if (find(groupList.begin(), groupList.end(), this) == groupList.end())
   {
      mNetGroups[mNetGroupIndex].mGroupList.push_back(this);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Added to net group.";
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "Net Group: " << mNetGroupIndex;
      }
   }
   else
   {
      auto out = ut::log::warning() << "Slot group is trying to be added to the network twice.";
      out.AddNote() << "Name: " << GetFullName();
   }
}

//! Add an entry to the transmit queue.
//!
//! @param aSimTime  The current simulation time.
//! @param aEntryPtr A pointer to the entry to be queue.
// private
void WsfJTIDS_SlotGroup::AddToTransmitQueue(double aSimTime, QueueEntry* aEntryPtr)
{
   // Determine if the entry to be added to the queue can simply be merged with an existing entry.
   for (QueueEntry* queuedPtr : mTransmitQueue)
   {
      if (aEntryPtr->mMessagePtr->SourceMessage()->GetSerialNumber() ==
          queuedPtr->mMessagePtr->SourceMessage()->GetSerialNumber())
      {
         // Merge the recipient list, eliminating duplicates.
         queuedPtr->mRecipients.insert(aEntryPtr->mRecipients.begin(), aEntryPtr->mRecipients.end());
         delete aEntryPtr;
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Slot group merged.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Name: " << GetFullName();
            out.AddNote() << "Serial Number: " << queuedPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
         }
         return;
      }
   }

   // Determine if the message is a replacement for an existing message.
   for (QueueEntry* queuedPtr : mTransmitQueue)
   {
      if (queuedPtr->mMessagePtr->SourceMessage()->CanBeReplacedBy(*aEntryPtr->mMessagePtr->SourceMessage()))
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Replacing message.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Name: " << GetFullName();
            out.AddNote() << "Old Serial Number: " << queuedPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
            out.AddNote() << "New Serial Number: " << aEntryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
         }

         // Notify that a message is being updated.

         WsfObserver::MessageUpdated(GetSimulation())(aSimTime,
                                                      GetTerminal(),
                                                      *queuedPtr->mMessagePtr->SourceMessage(),
                                                      *aEntryPtr->mMessagePtr->SourceMessage());

         delete queuedPtr->mMessagePtr;
         queuedPtr->mMessagePtr = aEntryPtr->mMessagePtr;
         aEntryPtr->mMessagePtr = nullptr;
         delete aEntryPtr;
         return;
      }
   }

   mTransmitQueue.push_back(aEntryPtr);

   // If this is the only entry on the queue then schedule the event to start the transmission.

   if (mTransmitQueue.front() == aEntryPtr)
   {
      if (aEntryPtr->mBeginTransmitTime < 0.0)
      {
         ComputeTransmitStartStopTime(aSimTime,
                                      aEntryPtr->mTimeSlotsNeeded,
                                      aEntryPtr->mBeginTransmitTime,
                                      aEntryPtr->mTransmitCompleteTime,
                                      aEntryPtr->mTransmitSlotIndex);
      }
      StartupTransmitEvents(aEntryPtr->mBeginTransmitTime);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Start transmit events.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "BTT: " << aEntryPtr->mBeginTransmitTime;
         out.AddNote() << "Serial Number: " << aEntryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
      }
   }
   else
   {
      WsfObserver::MessageQueued(
         GetSimulation())(aSimTime, GetTerminal(), *aEntryPtr->mMessagePtr->SourceMessage(), mTransmitQueue.size());
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Message Queued.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "Serial Number: " << aEntryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
      }
   }
}

//! Determine the raw number of time slots needed to transmit the message.
//! The computations factors in the packing limit assigned to the group.
//! @param  aMessage The message to be transmitted.
//! @return The number of time slots required.
// private
int WsfJTIDS_SlotGroup::ComputeTimeSlotsNeeded(const WsfMessage& aMessage) const
{
   // Calculate how many time slots are required at the standard packing.

   int bitsPerSlot     = mLayerPtr->GetBitsPerSlot();
   int timeSlotsNeeded = (aMessage.GetSizeBits() + bitsPerSlot - 1) / bitsPerSlot;

   // Adjust time slot requirement based on current packing.

   if (mPackingLimit == cP2SP)
   {
      timeSlotsNeeded = (timeSlotsNeeded + 1) / 2;
   }
   else if (mPackingLimit == cP2DP)
   {
      timeSlotsNeeded = (timeSlotsNeeded + 1) / 2;
   }
   else if (mPackingLimit == cP4SP)
   {
      timeSlotsNeeded = (timeSlotsNeeded + 3) / 4;
   }
   return timeSlotsNeeded;
}

//! Compute the start and stop time of a transmission.
// private
void WsfJTIDS_SlotGroup::ComputeTransmitStartStopTime(double  aSimTime,
                                                      int     aTimeSlotsNeeded,
                                                      double& aTransmitStartTime,
                                                      double& aTransmitStopTime,
                                                      int&    aTransmitSlotIndex)
{
   double secondsPerSlot  = mLayerPtr->GetSecondsPerSlot();
   int    slotsPerFrame   = mLayerPtr->GetSlotsPerFrame();
   double secondsPerFrame = slotsPerFrame * secondsPerSlot;

   // Compute the time offset within the current frame of the current simulation time.

   double currentFrameTimeOffset = fmod(aSimTime, secondsPerFrame);

   // Compute the simulation time at the start of the current frame.

   double frameStartTime = aSimTime - currentFrameTimeOffset;

   // Compute the slot number that corresponds to the current simulation time.
   // If the time is part way into the slot then we round up to the next slot.

   int currentSlotIndex = static_cast<int>(ceil(currentFrameTimeOffset / secondsPerSlot));
   if (currentSlotIndex >= slotsPerFrame)
   {
      currentSlotIndex = 0;
   }

   // Find the time slot for this group that is greater than or equal to the current time slot.

   std::vector<int>::size_type slotTableSize  = mTransmitSlots.size();
   std::vector<int>::size_type slotTableIndex = 0;
   for (std::vector<int>::size_type i = 0; i < slotTableSize; ++i)
   {
      if (mTransmitSlots[i] >= currentSlotIndex)
      {
         slotTableIndex = i;
         break;
      }
   }
   int nextSlotIndex = mTransmitSlots[slotTableIndex];

   // If the next slot offset is greater than or equal to the current slot offset then
   // the slot is within the current frame, otherwise it is in the next frame.

   aTransmitStartTime = frameStartTime + (nextSlotIndex * secondsPerSlot);
   if (nextSlotIndex < currentSlotIndex)
   {
      aTransmitStartTime += secondsPerFrame;
   }

   // Compute the time at which the transmission should be completed.

   int slotsElapsed = 1;
   for (int j = 1; j < aTimeSlotsNeeded; ++j)
   {
      currentSlotIndex = nextSlotIndex;
      ++slotTableIndex;
      if (slotTableIndex < slotTableSize)
      {
         // Next slot is in current frame.
         nextSlotIndex = mTransmitSlots[slotTableIndex];
         slotsElapsed += (nextSlotIndex - currentSlotIndex);
      }
      else
      {
         // Next slot is in next frame.
         slotTableIndex = 0;
         nextSlotIndex  = mTransmitSlots[slotTableIndex];
         slotsElapsed += (slotsPerFrame - currentSlotIndex + nextSlotIndex);
      }
   }

   aTransmitStopTime = aTransmitStartTime + (slotsElapsed * secondsPerSlot);

   // The transmit stop time is adjusted to be just before the end of the slot which ends the
   // transmission.  This is done for two reasons:
   //
   // a) The transmission in reality completes before the end of the slot.
   // b) If the next transmit slot immediately follows the ending slot the next slot may not get used.
   //    If the transmit stop time was used exactly as the transmit start time of the next message,
   //    the roundup in the computation of the current slot may result in wanting to start the
   //    transmission in the next slot after that!

   aTransmitStopTime -= (0.001 * secondsPerSlot);

   aTransmitSlotIndex = nextSlotIndex;
}

//! Deliver a message to all applicable recipients.
// private
void WsfJTIDS_SlotGroup::DeliverMessage(double aSimTime, QueueEntry& aQueueEntry)
{
   assert(mNetGroupIndex != 0);
   assert(!mNetGroups[mNetGroupIndex].mGroupList.empty());

   // Each slot group in the net group represents a JTIDS terminal.  In theory there could be
   // multiple JTIDS devices per platform.  For each slot group, if the corresponding terminal
   // is on a platform that is listed as a destination then we deliver the message to that terminal.

   std::vector<WsfJTIDS_SlotGroup*>& groupList = mNetGroups[mNetGroupIndex].mGroupList;
   for (WsfJTIDS_SlotGroup* groupPtr : groupList)
   {
      bool                doRelay  = false;
      WsfJTIDS_SlotGroup* relayPtr = groupPtr->mPairedRelayPtr;
      if ((groupPtr != this) && groupPtr->GetTerminal()->IsTurnedOn())
      {
         // If the terminal of this group is on a platform of interest, deliver it to this terminal...

         size_t platformIndex = groupPtr->GetTerminal()->GetPlatform()->GetIndex();
         if ((relayPtr != nullptr) || (aQueueEntry.mRecipients.find(platformIndex) != aQueueEntry.mRecipients.end()))
         {
            auto& message = *(aQueueEntry.mMessagePtr);
            // Set the destination to the of the message to that of the receiver.
            message.SourceMessage()->SetDstAddr(groupPtr->GetTerminal()->GetAddress());
            // message.SourceMessage()->SetDestination(groupPtr->GetTerminal()->GetPlatform()->GetNameId());
            message.GetResult().Reset();
            doRelay = groupPtr->GetTerminal()->Receive(aSimTime, GetTerminal(), message);
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Relaying message.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Name: " << GetFullName();
               out.AddNote() << "Status: " << doRelay;
               out.AddNote() << "Message: " << message.SourceMessage()->GetSerialNumber();
               out.AddNote() << "Recipient: " << groupPtr->GetFullName();
            }
         }
      }

      // Set up the relay if the recipient slot group has an associated relay slot group.

      if (doRelay && (relayPtr != nullptr) && relayPtr->GetTerminal()->IsTurnedOn() && (!relayPtr->mTransmitSlots.empty()))
      {
         QueueEntry* entryPtr = new QueueEntry(aQueueEntry);
         relayPtr->AddToTransmitQueue(aSimTime, entryPtr);
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Added to transmit queue.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Name: " << GetFullName();
            out.AddNote() << "Serial Number: " << entryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
            out.AddNote() << "Relayer: " << relayPtr->GetFullName();
         }
      }
   }
}

//! Get the fully qualified name of the slot group.
//! @return string of the form <platform-name>.<terminal-name>.<slot-group-name>
std::string WsfJTIDS_SlotGroup::GetFullName() const
{
   return GetTerminal()->GetPlatform()->GetName() + '.' + GetTerminal()->GetName() + '.' + mGroupName.GetString();
}

//! Standard Initialize() method.
bool WsfJTIDS_SlotGroup::Initialize(double aSimTime)
{
   bool ok = true;

   // Join the network.

   AddToNetwork();
   WsfJTIDS_SlotGroup* firstGroupPtr = mNetGroups[mNetGroupIndex].mGroupList.front();

   if (mPairedRelaySourceName == nullptr)
   {
      if (mSlotsPerFrame != firstGroupPtr->mSlotsPerFrame)
      {
         auto out = ut::log::error() << "'slots_per_frame' must be the same for common slot groups.";
         out.AddNote() << "Name: " << GetFullName();
         ok = false;
      }
      if (mSlotsPerFrame <= 0)
      {
         auto out = ut::log::error() << "No slots defined.";
         out.AddNote() << "Name: " << GetFullName();
         ok = false;
      }
      if (mPerUnitSlotsPerFrame > mSlotsPerFrame)
      {
         auto out = ut::log::error() << "'per_unit_slots_per_frame' > 'slots_per_frame'.";
         out.AddNote() << "Name: " << GetFullName();
         ok = false;
      }
   }
   else if (mPairedRelaySourceName != firstGroupPtr->mPairedRelaySourceName)
   {
      auto out = ut::log::error() << "'paired_relay_source' must be the same for common slot groups.";
      out.AddNote() << "Name: " << GetFullName();
      ok = false;
   }

   if (!ok)
   {
      RemoveFromNetwork();
      return false;
   }

   // Initialize the slot table as needed

   mSlots.clear();
   mTransmitSlots.clear();

   if (!mTimeSlotBlocks.empty())
   {
      int slotsPerFrame = static_cast<unsigned int>(mLayerPtr->GetSlotsPerFrame());
      for (auto& timeSlotBlock : mTimeSlotBlocks)
      {
         int offset   = timeSlotBlock.mOffset;
         int interval = timeSlotBlock.mInterval;
         while (offset < slotsPerFrame)
         {
            mSlots.push_back(offset);
            if (timeSlotBlock.mType != 'R')
            {
               mTransmitSlots.push_back(offset);
            }
            offset += interval;
         }
      }
      sort(mSlots.begin(), mSlots.end());
      sort(mTransmitSlots.begin(), mTransmitSlots.end());
   }

   else if (mPairedRelaySourceName != 0)
   {
      // This slot group represents the pair slots for relaying.  Locate the source slot group and
      // copy the slot group with the appropriate offset to this group.

      WsfJTIDS_SlotGroup* sourcePtr = mLayerPtr->FindGroup(mPairedRelaySourceName);
      if ((sourcePtr == nullptr) || (sourcePtr->mSlots.empty()))
      {
         { // RAII block
            auto out =
               ut::log::error() << "Source slot group for relay pair must appear before the relay pair slot group.";
            out.AddNote() << "Name: " << GetFullName();
         }
         RemoveFromNetwork();
         return false;
      }
      else
      {
         if (sourcePtr->mRelay)
         {
            sourcePtr->mPairedRelayPtr = this;
         }
         mSlots = sourcePtr->mSlots;
         for (unsigned int i = 0; i < mSlots.size(); ++i)
         {
            mSlots[i] += mPairedRelaySlotOffset;
            if (mSlots[i] >= mLayerPtr->GetSlotsPerFrame())
            {
               mSlots[i] -= mLayerPtr->GetSlotsPerFrame();
            }
         }
         sort(mSlots.begin(), mSlots.end());
         assert(!mSlots.empty());
      }
      mTransmitSlots = mSlots;
   }
   else
   {
      // If this is the first slot group to be added to the net group then generate the
      // slot tables.  If not then copy the slot table from the first entry.

      if (firstGroupPtr == this)
      {
         // This is the first slot group added so we must generate the slot allocation for this group.
         // If this is a paired slot group then we find the slot table for the source and copy it with
         // the appropriated offset.

         // Compute the time interval between each slot.

         double slotInterval = mLayerPtr->GetSlotsPerFrame() / static_cast<double>(mSlotsPerFrame);
         slotInterval        = std::max(slotInterval, 1.0);
         slotInterval *= mLayerPtr->GetSecondsPerSlot();

         // Place the first slot randomly in the first interval.

         double slotTime = GetSimulation()->GetRandom().Uniform(0.0, slotInterval);

         // Generate the slots.

         for (int slotNumber = 0; slotNumber < mSlotsPerFrame; ++slotNumber)
         {
            int slotIndex = static_cast<int>(floor(slotTime / mLayerPtr->GetSecondsPerSlot()));
            assert(slotIndex < mLayerPtr->GetSlotsPerFrame());
            if (!mSlots.empty())
            {
               assert(slotIndex != mSlots.back());
            }
            mSlots.push_back(slotIndex);
            slotTime += slotInterval;
         }
      }
      else
      {
         mSlots = firstGroupPtr->mSlots;
      }
      // Generate the transmit slot table.  If 'per_unit_slots_per_frame' is specified then
      // simply use a subset of the defined slots.  Otherwise any slot is available.

      if (mPerUnitSlotsPerFrame < 0)
      {
         mTransmitSlots = mSlots;
      }
      else if (mPerUnitSlotsPerFrame != 0)
      {
         int indexInterval = (mSlotsPerFrame + mPerUnitSlotsPerFrame - 1) / mPerUnitSlotsPerFrame;
         int indexOffset   = 0;
         std::vector<WsfJTIDS_SlotGroup*>& groupList = mNetGroups[mNetGroupIndex].mGroupList;
         for (auto& slotGroup : groupList)
         {
            if (slotGroup == this)
            {
               break;
            }
            if (slotGroup->mPerUnitSlotsPerFrame > 0)
            {
               ++indexOffset;
            }
         }

         mTransmitSlots.clear();
         for (int j = 0; j < mPerUnitSlotsPerFrame; ++j)
         {
            if (indexOffset >= mSlotsPerFrame)
            {
               indexOffset = indexOffset % mSlotsPerFrame;
            }
            mTransmitSlots.push_back(mSlots[indexOffset]);
            indexOffset += indexInterval;
         }
         sort(mTransmitSlots.begin(), mTransmitSlots.end());
      }
   }

   return ok;
}


//! Standard ProcessInput() method.
bool WsfJTIDS_SlotGroup::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "network")
   {
      aInput.ReadValue(mNetwork);
      aInput.ValueInClosedRange(mNetwork, 0, 127);
   }
   else if (command == "slots_per_frame")
   {
      aInput.ReadValue(mSlotsPerFrame);
      aInput.ValueInClosedRange(mSlotsPerFrame, 1, mLayerPtr->GetSlotsPerFrame());
   }
   else if (command == "per_unit_slots_per_frame")
   {
      aInput.ReadValue(mPerUnitSlotsPerFrame);
      aInput.ValueInClosedRange(mPerUnitSlotsPerFrame, 1, mSlotsPerFrame);
   }
   else if (command == "receive_only")
   {
      mPerUnitSlotsPerFrame = 0;
   }
   else if (command == "tsec")
   {
      aInput.ReadValue(mTSEC);
      aInput.ValueInClosedRange(mTSEC, 0, 127);
   }
   else if (command == "msec")
   {
      aInput.ReadValue(mMSEC);
      aInput.ValueInClosedRange(mMSEC, 0, 127);
   }
   else if (ProcessNPG(aInput))
   {
   }
   else if (command == "queue_limit")
   {
      aInput.ReadValue(mQueueLimit);
      aInput.ValueGreaterOrEqual(mQueueLimit, 0);
      ++mQueueLimit; // Add one to allow for the message in process of being transmitted.
   }
   else if (command == "packing_limit")
   {
      ProcessPackingFormat(aInput, mPackingLimit);
   }
   else if (command == "relay")
   {
      mRelay = true;
   }
   else if (command == "relay_slot_offset")
   {
      aInput.ReadValue(mPairedRelaySlotOffset);
      aInput.ValueGreaterOrEqual(mPairedRelaySlotOffset, 1);
   }
   else if (command == "receive_slot_block")
   {
      ProcessTimeSlotBlock(aInput, 'R');
   }
   else if (command == "transmit_slot_block")
   {
      ProcessTimeSlotBlock(aInput, 'T');
   }
   else if (command == "contention_access_mode")
   {
      aInput.ReadValue(mAccessMode);
      aInput.ValueInClosedRange(mAccessMode, 0, 15);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// private
void WsfJTIDS_SlotGroup::ProcessPackingFormat(UtInput& aInput, PackingFormat& aPackingFormat)
{
   std::string packingFormat;
   aInput.ReadValue(packingFormat);
   if ((packingFormat == "standard") || (packingFormat == "std"))
   {
      aPackingFormat = cSTD;
   }
   else if ((packingFormat == "p2sp") || (packingFormat == "packed-2-single-pulse"))
   {
      aPackingFormat = cP2SP;
   }
   else if ((packingFormat == "p2dp") || (packingFormat == "packed-2-double-pulse"))
   {
      aPackingFormat = cP2DP;
   }
   else if ((packingFormat == "p4sp") || (packingFormat == "packed-4-single-pulse"))
   {
      aPackingFormat = cP4SP;
   }
   else
   {
      throw UtInput::BadValue(aInput);
   }
}

//! Process a time slot block definition.
//!
//! Parse input of the form <set>-<index>-<rrn>
//!
//! @param aInput     The input stream.
//! @param aBlockType block type ('T' or 'R')
// private
void WsfJTIDS_SlotGroup::ProcessTimeSlotBlock(UtInput& aInput, char aBlockType)
{
   static std::string letters("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
   std::string        tsb;
   aInput.ReadValue(tsb);
   std::istringstream iss(tsb);
   char               setChar    = ' ';
   char               delimiter1 = ' ';
   char               delimiter2 = ' ';
   unsigned int       index      = 0;
   unsigned int       rrn        = 0;
   iss >> setChar >> delimiter1 >> index >> delimiter2 >> rrn; // changes iss to !good on valid inputs because it
                                                               // switches eofbit to 1 which causes iss.tellg() to
                                                               // return -1 ex: A-1-1
   long int issTellg;
   if (iss.eof() && !iss.fail())
   {
      issTellg = static_cast<long int>(iss.rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
   }
   else
   {
      issTellg = static_cast<long int>(iss.tellg());
   }
   setChar                    = toupper(setChar);
   std::string::size_type set = letters.find_first_of(toupper(setChar));
   if (iss && (issTellg == static_cast<long int>(tsb.size())) && (set != std::string::npos) &&
       (set < static_cast<unsigned int>(mLayerPtr->GetSetsPerFrame())) && (delimiter1 == '-') &&
       (index < static_cast<unsigned int>(mLayerPtr->GetSlotsPerSet())) && (delimiter2 == '-') && (rrn >= 1) && (rrn <= 15))
   {
      // Form the absolute starting offset and interval within the frame and
      // create the time slot block entry.

      int offset   = static_cast<int>((index * mLayerPtr->GetSetsPerFrame()) + set);
      int interval = static_cast<int>(1 << (15 - rrn));
      interval *= mLayerPtr->GetSetsPerFrame();
      mTimeSlotBlocks.emplace_back(offset, interval, aBlockType);
   }
   else
   {
      throw UtInput::BadValue(aInput);
   }
}

//! Purge all queued messages in the slot group.
void WsfJTIDS_SlotGroup::PurgeQueuedMessages()
{
   while (!mTransmitQueue.empty())
   {
      delete mTransmitQueue.front();
      mTransmitQueue.pop_front();
   }
}

//! Remove the slot group from the network.
void WsfJTIDS_SlotGroup::RemoveFromNetwork()
{
   if (mNetGroupIndex > 0)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Removed from net group.";
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "Net group: " << mNetGroupIndex;
      }
      std::vector<WsfJTIDS_SlotGroup*>& groupList = mNetGroups[mNetGroupIndex].mGroupList;
      auto                              iter      = find(groupList.begin(), groupList.end(), this);
      if (iter != groupList.end())
      {
         groupList.erase(iter);
      }
      mNetGroupIndex = 0;
   }
}

//! See WsfComm::SendMessage.
bool WsfJTIDS_SlotGroup::SendMessage(double aSimTime, const WsfMessage& aMessage, WsfPlatform* aRecipientPtr, bool aSource)
{
   bool ok = false;
   if ((!mTransmitSlots.empty()) && (static_cast<int>(mTransmitQueue.size()) < mQueueLimit))
   {
      ok = true;
      QueueEntry::RecipientList recipients{aRecipientPtr->GetIndex()};
      QueueEntry*               entryPtr = new QueueEntry(aSimTime, ut::clone(aMessage), std::move(recipients));
      entryPtr->mTimeSlotsNeeded         = ComputeTimeSlotsNeeded(aMessage);
      AddToTransmitQueue(aSimTime, entryPtr);
   }
   return ok;
}

//! Internal method called from WsfJTIDS_SlotGroup::CommEvent::Execute().
//!
//! This is called to begin the transmission of the top message in the queue.
//!
//! @return Simulation time at which transmission will be complete. If the value is
//!         less than zero then there was no message present.
double WsfJTIDS_SlotGroup::BeginTransmitEvent(double aSimTime)
{
   double transmitCompleteTime = -1.0;

   if (!mTransmitQueue.empty())
   {
      QueueEntry* entryPtr = mTransmitQueue.front();
      transmitCompleteTime = entryPtr->mTransmitCompleteTime;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfJTIDS_SlotGroup::BeginTransmitEvent Data Report:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "Begin Transmit: " << entryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
      }
      WsfPlatform* platformPtr       = GetTerminal()->GetPlatform();
      double       heartbeatInterval = 5.0; // non-mover
      if (platformPtr != nullptr)
      {
         if (platformPtr->GetMover() != nullptr)
         {
            heartbeatInterval = 2.0;
         }
      }
      if ((transmitCompleteTime > 0) && ((transmitCompleteTime - aSimTime) > heartbeatInterval))
      {
         auto thbEvent = ut::make_unique<TransmitHeartbeatEvent>();
         thbEvent->InitializeEvent(aSimTime, transmitCompleteTime, heartbeatInterval, GetSimulation(), entryPtr, GetTerminal());
         thbEvent->SetTime(aSimTime + heartbeatInterval);
         GetSimulation()->AddEvent(std::move(thbEvent));
      }
      WsfObserver::MessageTransmitted(GetSimulation())(aSimTime, GetTerminal(), *entryPtr->mMessagePtr->SourceMessage());
   }
   return transmitCompleteTime;
}

//! Internal method called from WsfJTIDS_Terminal::CommEvent::Execute().
//!
//! This is called to completed the transmission of the top message in the queue.
//!
//! @return Simulation time at which next transmission for this group is to begin.
//!         If the value is less than zero no messages are pending.
double WsfJTIDS_SlotGroup::TransmitCompleteEvent(double aSimTime)
{
   double beginTransmitTime = -1.0;
   // TODO: check while???
   if (!mTransmitQueue.empty())
   {
      // Get the current queue entry and deliver the message to all the intended recipients.

      QueueEntry* entryPtr = mTransmitQueue.front();
      mTransmitQueue.pop_front();
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfJTIDS_SlotGroup::TransmitCompleteEvent Data Report:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Name: " << GetFullName();
         out.AddNote() << "Transmit Complete: " << entryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
      }
      DeliverMessage(aSimTime, *entryPtr);
      MessageSent(aSimTime, entryPtr->mMessagePtr->SourceMessage().get(), entryPtr->mTransmitSlotIndex, this);
      // Notify transmission has ended; this one triggers the end transmission pdu
      WsfObserver::MessageTransmitEnded(GetSimulation())(aSimTime, GetTerminal(), *entryPtr->mMessagePtr->SourceMessage());
      delete entryPtr;

      // If there is a message queued then schedule it for transmission.

      if (!mTransmitQueue.empty())
      {
         entryPtr = mTransmitQueue.front();
         if (entryPtr->mBeginTransmitTime < 0.0)
         {
            ComputeTransmitStartStopTime(aSimTime,
                                         entryPtr->mTimeSlotsNeeded,
                                         entryPtr->mBeginTransmitTime,
                                         entryPtr->mTransmitCompleteTime,
                                         entryPtr->mTransmitSlotIndex);
         }
         beginTransmitTime = entryPtr->mBeginTransmitTime;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << GetFullName() << "Starting transmit.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Serial Number: " << entryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
            out.AddNote() << "BTT: " << entryPtr->mBeginTransmitTime;
         }
      }
   }
   return beginTransmitTime;
}

void WsfJTIDS_SlotGroup::TransmitEventsStopped()
{
   mIsUsingTransmitEvents = false;
   if (!mTransmitQueue.empty())
   {
      QueueEntry* entryPtr = mTransmitQueue.front();
      mTransmitQueue.pop_front();
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfJTIDS_SlotGroup::TransmitEventsStopped:";
         out.AddNote() << "T = " << GetSimulation()->GetSimTime();
         out.AddNote() << "Slot Group: " << GetFullName();
         out.AddNote() << "Transmit Ended: " << entryPtr->mMessagePtr->SourceMessage()->GetSerialNumber();
      }
      // Notify transmission ended
      WsfObserver::MessageTransmitEnded(
         GetSimulation())(GetSimulation()->GetSimTime(), GetTerminal(), *entryPtr->mMessagePtr->SourceMessage());
   }
}

double WsfJTIDS_SlotGroup::GetNextTransmitTime(double aSimTime)
{
   double nextTime;
   double stopTime;
   int    transmitSlotIndex;
   ComputeTransmitStartStopTime(aSimTime + mLayerPtr->GetSecondsPerSlot() / 2.0, 1, nextTime, stopTime, transmitSlotIndex);
   return nextTime;
}

void WsfJTIDS_SlotGroup::StartupTransmitEvents(double aSimTime)
{
   if (!mIsUsingTransmitEvents)
   {
      mIsUsingTransmitEvents = true;
      CommEvent::ScheduleBeginTransmitEvent(aSimTime, *GetSimulation(), this);
   }
}

bool WsfJTIDS_SlotGroup::ProcessNPG(UtInput& aInput)
{
   bool      myCommand = false;
   const int npgCount  = 32;

   if (aInput.GetCommand() == "npg")
   {
      myCommand = true;
      std::string inputString;
      aInput.ReadValue(inputString);
      std::string npg       = inputString;
      int         npgNumber = UtStringUtil::ToInt(npg);
      if (npgNumber <= 0 || npgNumber > 511)
      {
         static const char* npgList[npgCount] = {
            "",                               // 0
            "initial_entry",                  // 1
            "rtt_a",                          // 2
            "rtt_b",                          // 3
            "network_management",             // 4
            "ppli_a",                         // 5
            "ppli_b",                         // 6
            "surveillance",                   // 7
            "weapons_coordination",           // 8
            "air_control",                    // 9
            "electronic_warfare",             // 10
            "unassigned",                     // 11
            "voice_a",                        // 12
            "voice_b",                        // 13
            "indirect_ppli",                  // 14
            "",                               // 15
            "",                               // 16
            "",                               // 17
            "wc",                             // 18
            "fighter_to_fighter_dedicated",   // 19
            "fighter_to_fighter_contention",  // 20
            "engagement_coordination",        // 21
            "",                               // 22
            "",                               // 23
            "",                               // 24
            "",                               // 25
            "",                               // 26
            "joint_ppli",                     // 27
            "distributed_network_management", // 28
            "residual_message",               // 29
            "ijms_position",                  // 30
            "ijms_message"                    // 31
         };

         UtStringUtil::TrimWhiteSpace(npg);
         UtStringUtil::ToLower(npg);
         for (npgNumber = 0; (npgNumber < npgCount) && ((npgList[npgNumber][0] == 0) || (npg != npgList[npgNumber]));
              ++npgNumber)
         {
         }
         if (npgNumber == npgCount)
         {
            npgNumber = 0;
         }
      }
      if (npgNumber == 0)
      {
         throw UtInput::BadValue(aInput, "Invalid value for npg");
      }
      mNPG = npgNumber;
   }
   return myCommand;
}

bool WsfJTIDS_SlotGroup::DebugEnabled() const
{
   return mLayerPtr->DebugEnabled();
}

wsf::comm::Comm* WsfJTIDS_SlotGroup::GetTerminal() const
{
   return mLayerPtr->Parent();
}

WsfSimulation* WsfJTIDS_SlotGroup::GetSimulation() const
{
   return mLayerPtr->Parent()->GetSimulation();
}

// ****************************************************************************
// ************** Start of definition of nested class CommEvent ***************
// ****************************************************************************

WsfJTIDS_SlotGroup::CommEvent::CommEvent(double aSimTime, EventType aEventType, WsfJTIDS_SlotGroup* aGroupPtr)
   : WsfEvent(aSimTime)
   , mLastStatusChangeTime(aGroupPtr->GetTerminal()->GetLastStatusChangeTime())
   , mEventType(aEventType)
   , mPlatformIndex(aGroupPtr->GetTerminal()->GetPlatform()->GetIndex())
   , mGroupPtr(aGroupPtr)
{
}

WsfEvent::EventDisposition WsfJTIDS_SlotGroup::CommEvent::Execute()
{
   EventDisposition disposition = cDELETE;
   WsfPlatform*     platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if ((platformPtr != nullptr) && (mLastStatusChangeTime == mGroupPtr->GetTerminal()->GetLastStatusChangeTime()))
      if (platformPtr != nullptr)
      {
         if (mEventType == cBEGIN_TRANSMIT)
         {
            double nextTime = mGroupPtr->BeginTransmitEvent(GetTime());
            if (nextTime >= 0.0)
            {
               SetTime(nextTime);
               mEventType  = cTRANSMIT_COMPLETE;
               disposition = cRESCHEDULE;
            }
         }
         else if (mEventType == cTRANSMIT_COMPLETE)
         {
            double nextTime = mGroupPtr->TransmitCompleteEvent(GetTime());
            if (nextTime >= 0.0)
            {
               SetTime(nextTime);
               mEventType  = cBEGIN_TRANSMIT;
               disposition = cRESCHEDULE;
            }
         }
         if (disposition == cDELETE)
         {
            mGroupPtr->TransmitEventsStopped();
         }
      }

   return disposition;
}

// static
void WsfJTIDS_SlotGroup::CommEvent::ScheduleBeginTransmitEvent(double              aSimTime,
                                                               WsfSimulation&      aSimulation,
                                                               WsfJTIDS_SlotGroup* aGroupPtr)
{
   aSimulation.AddEvent(ut::make_unique<make_unique_comm_event>(aSimTime, cBEGIN_TRANSMIT, aGroupPtr));
}

// static
void WsfJTIDS_SlotGroup::CommEvent::ScheduleTransmitCompleteEvent(double              aSimTime,
                                                                  WsfSimulation&      aSimulation,
                                                                  WsfJTIDS_SlotGroup* aGroupPtr)
{
   aSimulation.AddEvent(ut::make_unique<make_unique_comm_event>(aSimTime, cTRANSMIT_COMPLETE, aGroupPtr));
}

// ****************************************************************************
// *************** Start of definition of nested class NetGroup ***************
// ****************************************************************************

WsfJTIDS_SlotGroup::NetGroup::NetGroup(WsfStringId aGroupName, int aNetwork, int aTSEC)
   : mGroupName(aGroupName)
   , mNetwork(aNetwork)
   , mTSEC(aTSEC)
{
}

// ****************************************************************************
// ************** Start of definition of nested class QueueEntry **************
// ****************************************************************************
//
// The QueueEntry class represents an entry in the transmission queue.

WsfJTIDS_SlotGroup::QueueEntry::QueueEntry(double aTimeQueued, std::unique_ptr<WsfMessage> aMessage, RecipientList&& aRecipients)
   : mTimeQueued(aTimeQueued)
   , mBeginTransmitTime(-1.0)
   , mTransmitCompleteTime(-1.0)
   , mMessagePtr(new wsf::comm::Message(std::move(aMessage)))
   , mTimeSlotsNeeded(0)
   , mRecipients(std::move(aRecipients))
{
}

WsfJTIDS_SlotGroup::QueueEntry::QueueEntry(const QueueEntry& aSrc)
   : mTimeQueued(aSrc.mTimeQueued)
   , mBeginTransmitTime(-1.0)
   , mTransmitCompleteTime(-1.0)
   , mMessagePtr(aSrc.mMessagePtr->Clone())
   , mTimeSlotsNeeded(aSrc.mTimeSlotsNeeded)
   , mRecipients(aSrc.mRecipients)
{
}

WsfJTIDS_SlotGroup::QueueEntry::~QueueEntry()
{
   delete mMessagePtr;
}

WsfEvent::EventDisposition WsfJTIDS_SlotGroup::TransmitHeartbeatEvent::Execute()
{
   WsfEvent::EventDisposition eventDisposition = cDELETE;
   WsfObserver::MessageTransmittedHeartbeat(GetSimulation())(GetTime(), mTerminal, *mEntry->mMessagePtr->SourceMessage());
   if ((mEndTime - GetTime()) > 2)
   {
      SetTime(GetTime() + mInterval);
      eventDisposition = cRESCHEDULE;
   }
   return eventDisposition;
}
