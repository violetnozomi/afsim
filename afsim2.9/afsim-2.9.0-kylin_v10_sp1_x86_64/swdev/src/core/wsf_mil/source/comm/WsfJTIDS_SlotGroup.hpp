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

#ifndef WSFJTIDS_SLOTGROUP_HPP
#define WSFJTIDS_SLOTGROUP_HPP

#include "wsf_mil_export.h"

#include <list>
#include <string>
#include <unordered_set>
#include <vector>

namespace wsf
{
namespace comm
{
class Comm;
class SlotGroupLayerJTIDS;
} // namespace comm
} // namespace wsf

#include "UtCallback.hpp"
class UtInput;
class WsfCommandChain;
#include "WsfCommMessage.hpp"
class WsfScenario;
#include "WsfEvent.hpp"
class WsfMessage;
class WsfPlatform;
#include "WsfStringId.hpp"

//! A 'slot group' represents a collection of associated Time Slot Blocks (TSBs)
//!
//! JTIDS network design is accomplished by creating groups of TSBs called
//! 'slot groups'.  A slot group is a basically a subdivision of Network
//! Participation Group (NPG) that represents the slots used to perform a
//! function (transmit, receive, relay) within an NPG.
class WSF_MIL_EXPORT WsfJTIDS_SlotGroup
{
public:
   WsfJTIDS_SlotGroup(wsf::comm::SlotGroupLayerJTIDS* aLayerPtr,
                      const std::string&              aGroupName,
                      const std::string&              aSourceGroupName);

   WsfJTIDS_SlotGroup(const WsfJTIDS_SlotGroup& aSrc, wsf::comm::SlotGroupLayerJTIDS* aLayerPtr);

   virtual ~WsfJTIDS_SlotGroup();

   static void ClearNetGroups();

   void AddToNetwork();

   double BeginTransmitEvent(double aSimTime);

   //! @name Callback lists.
   //@{
   UtCallbackListN<void(double, const WsfMessage*, int, WsfJTIDS_SlotGroup*)> MessageSent;
   //@}

   //! Return the name ID of the slot group
   WsfStringId GetNameId() const { return mGroupName; }

   //! Return the JTIDS network number
   int GetNetwork() const { return mNetwork; }

   //! Return the JTIDS TSEC value
   int GetTSEC() const { return mTSEC; }

   //! Return the JTIDS MSEC value
   int GetMSEC() const { return mMSEC; }

   //! Return the Network Participation Group
   int GetNPG() const { return mNPG; }

   //! Returns the number of slots per frame
   int GetSlotsPerFrame() const { return mSlotsPerFrame; }

   //! Returns the number of slots allocated for transmitting
   int GetTransmitSlotsPerFrame() const { return static_cast<int>(mTransmitSlots.size()); }

   double GetNextTransmitTime(double aSimTime);

   bool             DebugEnabled() const;
   wsf::comm::Comm* GetTerminal() const;
   WsfSimulation*   GetSimulation() const;

   bool Initialize(double aSimTime);

   bool ProcessInput(UtInput& aInput);

   void PurgeQueuedMessages();

   void RemoveFromNetwork();

   bool SendMessage(double aSimTime, const WsfMessage& aMessage, WsfPlatform* aRecipientPtr, bool aSource = true);

   void StartupTransmitEvents(double aSimTime);

   double TransmitCompleteEvent(double aSimTime);

   void TransmitEventsStopped();

   class QueueEntry
   {
   public:
      using RecipientList = std::unordered_set<size_t>;
      QueueEntry(double aTimeQueued, std::unique_ptr<WsfMessage> aMessage, RecipientList&& aRecipients);
      QueueEntry(const QueueEntry& aSrc);
      ~QueueEntry();

      double              mTimeQueued;
      double              mBeginTransmitTime;
      double              mTransmitCompleteTime;
      wsf::comm::Message* mMessagePtr;
      int                 mTimeSlotsNeeded;
      int                 mTransmitSlotIndex;
      RecipientList       mRecipients;
   };

   class TransmitHeartbeatEvent : public WsfEvent
   {
   public:
      TransmitHeartbeatEvent() = default;
      TransmitHeartbeatEvent(double aSimTime)
         : WsfEvent(aSimTime)
      {
      }
      void InitializeEvent(double           aSimTime,
                           double           aEndTime,
                           double           aInterval,
                           WsfSimulation*   aSimulationPtr,
                           QueueEntry*      aEntry,
                           wsf::comm::Comm* aTerminal)
      {
         mTimeNow       = aSimTime;
         mEndTime       = aEndTime;
         mInterval      = aInterval;
         mSimulationPtr = aSimulationPtr;
         mEntry         = aEntry;
         mTerminal      = aTerminal;
      }
      EventDisposition Execute() override;

      double           mTimeNow;
      double           mEndTime;
      double           mInterval;
      WsfSimulation*   mSimulationPtr;
      QueueEntry*      mEntry;
      wsf::comm::Comm* mTerminal;
   };

private:
   enum AccessMode
   {
      cDEDICATED = -1
      // Access Modes 0 - 15 are used for contention access.
   };

   enum PackingFormat
   {
      cSTD,  // Standard double pulse ( 3x70 bits per time slot)
      cP2SP, // Packed-2 single pulse ( 6x70 bits per time slot)
      cP2DP, // Packed-2 double pulse ( 6x70 bits per time slot)
      cP4SP  // Packed-4 single pulse (12x70 bits per time slot)
   };

   //! A CommEvent is used to represent the beginning and ending of a transmission.
   class CommEvent : public WsfEvent
   {
   public:
      enum EventType
      {
         cBEGIN_TRANSMIT,
         cTRANSMIT_COMPLETE
      };

      EventDisposition Execute() override;

      static void ScheduleBeginTransmitEvent(double aSimTime, WsfSimulation& aSimulation, WsfJTIDS_SlotGroup* aGroupPtr);
      static void ScheduleTransmitCompleteEvent(double aSimTime, WsfSimulation& aSimulation, WsfJTIDS_SlotGroup* aGroupPtr);

   protected:
      CommEvent(double aSimTime, EventType aEventType, WsfJTIDS_SlotGroup* aGroupPtr);

      double              mLastStatusChangeTime;
      EventType           mEventType;
      size_t              mPlatformIndex;
      WsfJTIDS_SlotGroup* mGroupPtr;
   };

   //! This helper struct enables make_unique for CommEvent, which has a protected constructor
   struct make_unique_comm_event : CommEvent
   {
      template<typename... Args>
      make_unique_comm_event(Args&&... args)
         : CommEvent(std::forward<Args>(args)...)
      {
      }
   };

   //! A NetGroup is a list of all slot group instances that shared the
   //! same group ID, network number and TSEC value.
   class NetGroup
   {
   public:
      NetGroup(WsfStringId aGroupName, int aNetwork, int aTSEC);
      ~NetGroup() = default;

      WsfStringId                      mGroupName;
      int                              mNetwork;
      int                              mTSEC;
      std::vector<WsfJTIDS_SlotGroup*> mGroupList;
   };

   class TimeSlotBlock
   {
   public:
      TimeSlotBlock(int aOffset, int aInterval, char aType)
         : mOffset(aOffset)
         , mInterval(aInterval)
         , mType(aType)
      {
      }
      int  mOffset;
      int  mInterval;
      char mType;
   };

   void AddToTransmitQueue(double aSimTime, QueueEntry* aEntryPtr);

   int ComputeTimeSlotsNeeded(const WsfMessage& aMessage) const;

   void ComputeTransmitStartStopTime(double  aSimTime,
                                     int     aTimeSlotsNeeded,
                                     double& aStartTime,
                                     double& aStopTime,
                                     int&    aTransmitSlotIndex);

   void DeliverMessage(double aSimTime, QueueEntry& aQueueEntry);

   virtual std::string GetFullName() const;

   void ProcessPackingFormat(UtInput& aInput, PackingFormat& aPackingFormat);

   void ProcessTimeSlotBlock(UtInput& aInput, char aBlockType);

   bool ProcessNPG(UtInput& aInput);

   wsf::comm::SlotGroupLayerJTIDS* mLayerPtr;
   WsfStringId                     mGroupName;

   int           mNetwork;
   int           mTSEC;
   int           mMSEC;
   int           mNPG;
   int           mAccessMode;
   PackingFormat mPackingLimit;

   int                        mSlotsPerFrame;
   int                        mPerUnitSlotsPerFrame;
   std::vector<TimeSlotBlock> mTimeSlotBlocks;

   std::vector<int> mSlots;
   std::vector<int> mTransmitSlots;

   int                    mQueueLimit;
   std::list<QueueEntry*> mTransmitQueue;

   //! If defined then this slot group represents the paired relay slots for the specified slot group.
   WsfStringId mPairedRelaySourceName;

   //! When this is a paired relay slot group, this is the slot offset from the source slot group.
   int mPairedRelaySlotOffset;

   //! If true then relay messages from this group via the paired relay slot group.
   bool mRelay;

   //! The pointer to the paired relay slot group.
   WsfJTIDS_SlotGroup* mPairedRelayPtr;

   //! Index to the net group of which this slot group is a member
   std::vector<NetGroup>::size_type mNetGroupIndex;

   static std::vector<NetGroup> mNetGroups;

   bool mIsUsingTransmitEvents;
};

#endif
