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
#ifndef WSFXIO_JOB_BOARD_PACKETREGISTRY_HPP
#define WSFXIO_JOB_BOARD_PACKETREGISTRY_HPP

#include <iostream>
#include <list>
#include <map>

#include "WsfRIPRCommon.hpp"
#include "WsfRIPRJob.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_RIPRSerializeTypes.hpp"
#include "xio/WsfXIO_SafeRIPRProc.hpp"
//#include "PakSerialize.hpp"
//#include "PakProcessor.hpp"

typedef std::map<unsigned int, WsfXIO_SafeRIPRProc> XIO_RIPRProcIdMap;

namespace ripr_xio
{
const double       sDONT_CARE_VALUE_DBL  = -1.0;
const int          sDONT_CARE_VALUE_INT  = -1;
const unsigned int sDONT_CARE_VALUE_UINT = UINT_MAX;
const WsfStringId  sDONT_CARE_VALUE_STRINGID(0);
const bool         sDONT_CARE_VALUE_BOOL = false;
} // namespace ripr_xio

#define RIPR_DEFINE_PACKET(NAME, BASE, ID) XIO_DEFINE_PACKET(NAME, BASE, ID + cXIO_FIRST_PACKET_ID_RIPR)

class WsfXIO_JobBoardPacketRegistry
{
public:
   ~WsfXIO_JobBoardPacketRegistry();
   //! Registers all packets used by RIPR XIO.  Unregistered packets cannot be sent or received.
   static void Initialize(PakProcessor& aProcessor);

protected:
private:
   WsfXIO_JobBoardPacketRegistry();
};

//! Request information about current tasks(WsfXIO_TaskInitPkt), future task changes (WsfXIO_TaskUpdatePkt),
//! and optionally override the simulation's task management logic.
class WsfXIO_JobBoardRequestPkt : public WsfXIO_RequestDataPkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_JobBoardRequestPkt, WsfXIO_RequestDataPkt, 0)
   {
      WsfXIO_RequestDataPkt::Serialize(aBuff);
      aBuff& mPlatformIndex& mRIPRProcessorName;
   }
   //! Index of the platform owning the task manager
   int32_t mPlatformIndex;
   //! Name of the task processor
   WsfStringId mRIPRProcessorName;
};

//! Defines current state of a RIPR Processor's job board.  Response to JobBoardRequestPkt.
//!
//! This struct defines a conglomerated RIPR Processor and its Job Board.
class WsfXIO_JobBoardInitPkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_JobBoardInitPkt, WsfXIO_ResponsePkt, 1)
   {
      aBuff& mWinners& mJobs& mCommanders& mJobChannelIds& mBiddingHijacked;
   }

   // the list of jobs currently in the jobboard.
   std::list<WsfRIPRJob> mJobs;

   //! The map of winning Processor Channel IDs to job IDs.
   RIPRWinnerMap mWinners;

   //! List of this Job Board's RIPR Processor commanders.
   RIPRCommanderList mCommanders;

   //! the number of job channels the parent RIPR processor supports.
   JobChannelIdList mJobChannelIds;

   //! If true, this processor cannot place bids from script calls.
   bool mBiddingHijacked;
};

namespace xio_ripr
{
enum UpdateType
{
   cJOB_ADDED,               //! RIPR processor added this job to its job board.
   cJOB_REMOVED,             //! RIPR processor removed this job from its job board.
   cJOB_WON,                 //! RIPR processor won this job.
   cJOB_CANCELED,            //! RIPR processor deleted this job from its job board.
   cJOB_COMPLETED,           //! RIPR processor received a completion for this job on its job board.
   cJOB_BID_CHANGED,         //! This Job's bid has changed.
   cJOB_PRIORITY_CHANGED,    //! This job's priority has changed.
   cJOB_MAX_WINNERS_CHANGED, //! This job's maximum number of winner's ceiling has changed.
   cCHANNEL_ID_CHANGED,      //! This RIPR processor's channel ID has changed.
   cBID_CONTROL_CHANGED,     //! This RIPR processor's bid control has been taken away or restored.
   //      cWINNERS_REALLOCATED,    //! This RIPR processor's jobs have been reallocated their winners.
};
} // namespace xio_ripr

//! Defines a change in a job.  Assignment, cancellation, or completion.  Response to WsfXIO_TaskRequestPkt.
class WsfXIO_JobBoardUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_JobBoardUpdatePkt, WsfXIO_ResponsePkt, 2)
   {
      WsfXIO_ResponsePkt::Serialize(aBuff);
      aBuff& PakSerialization::Enum(mUpdateType) & mJobPtr& mWinningBidId;
   }

   //! Type of job update
   UpdateType mUpdateType;
   //! The Job
   const WsfRIPRJob* mJobPtr;
   //! For cJOB_WON: ID (WsfRIPRProcessor::GetId(Channel)) of the RIPR processor doing the job.
   unsigned int mWinningBidId;
};

//! Defines a change in a job.  Assignment, cancellation, or completion.  Response to WsfXIO_TaskRequestPkt.
class WsfXIO_JobBoardWinnersUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_JobBoardWinnersUpdatePkt, WsfXIO_ResponsePkt, 9)
   {
      WsfXIO_ResponsePkt::Serialize(aBuff);
      aBuff& mPlatformIndex& mRIPRProcessorName& mWinners;
   }

   //! Index of the platform owning the task manager
   unsigned int mPlatformIndex;
   //! Name of the task processor
   WsfStringId mRIPRProcessorName;
   //! The map of winning Processor Channel IDs to job IDs.
   RIPRWinnerMap mWinners;
};

//! Requests a RIPR processor to perform a function against its job board.
class WsfXIO_JobBoardCommandPkt : public WsfXIO_Packet
{
public:
   static const int cPACKET_ID = cXIO_FIRST_PACKET_ID_RIPR + 3;
   WsfXIO_JobBoardCommandPkt(int aPacketId = cPACKET_ID)
      : WsfXIO_Packet(aPacketId)
      , mJobBoardPlatformIndex(sDONT_CARE_VALUE_UINT)
      , mJobBoardProcessorName(sDONT_CARE_VALUE_STRINGID)
      , mJobId(sDONT_CARE_VALUE_INT)
      , mBidderUniqueRiprId(sDONT_CARE_VALUE_UINT)
      , mBidOrPriority(sDONT_CARE_VALUE_DBL)
      , mStatus(sDONT_CARE_VALUE_STRINGID)
      , mTakeControl(sDONT_CARE_VALUE_BOOL)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_Packet::Serialize(aBuff);
      DoSerialize(aBuff);
   }

   template<typename T>
   void DoSerialize(T& aBuff)
   {
      aBuff& PakSerialization::Enum(mCommandType) &
         mJobBoardPlatformIndex& mJobBoardProcessorName& mJobId& mBidderUniqueRiprId& mBidOrPriority& mStatus& mTakeControl;
   }

   enum CommandType
   {
      //! Bid for a job on the job board.
      cJOB_BID,
      //! Delete a job from the job board.
      cJOB_CANCEL,
      //! Add the job to the job board.
      cJOB_ADD,
      //! Purge completed jobs from the job board.
      cPURGE_COMPLETED_JOBS,
      //! Change job priority of the job on the job board.
      cCHANGE_JOB_PRIORITY,
      //! Provide an update about the status of the job.
      cJOB_STATUS,
      //! Provide an update about the status of the job.
      cCHANGE_MAX_WINNERS,
      //! Take remote control of the RIPR Processor (human-in-the-loop or
      //! remote application controls its bidding.
      cTAKE_CONTROL_OR_GIVE_BACK_CONTROL,
   };

   //! Type of task update.  Required.
   CommandType mCommandType;
   //! Platform Index of the job board owner.  Required.
   unsigned int mJobBoardPlatformIndex;
   //! Name of the assigner's RIPR processor.  Required.
   WsfStringId mJobBoardProcessorName;
   //! The subject track ID.  Required.
   int mJobId;
   //! For cJOB_BID: The id of the channel the bidder is using.
   unsigned int mBidderUniqueRiprId;
   //! For cJOB_BID: Bid to make for the job.
   //! For cCHANGE_JOB_PRIORITY: Priority to assign the job.
   //! For cCHANGE_MAX_WINNERS: Number of winners to allow.
   double mBidOrPriority;
   //! For cJOB_STATUS: Status message.
   WsfStringId mStatus;
   //! For cTAKE_CONTROL_OR_GIVE_BACK_CONTROL
   bool mTakeControl;
};

//! Sent when a RIPR Processor's Channel ID list changes.
class WsfXIO_ChannelIdUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_ChannelIdUpdatePkt, WsfXIO_ResponsePkt, 4)
   {
      aBuff& PakSerialization::Enum(mUpdateType) & mPlatformIndex& mRIPRProcessorName& mJobChannelIds;
   }

   //! Type of job update
   UpdateType mUpdateType;

   //! Index of the platform owning the task manager
   unsigned int mPlatformIndex;

   //! Name of the task processor
   WsfStringId mRIPRProcessorName;

   //! the number of job channels the parent RIPR processor supports.
   JobChannelIdList mJobChannelIds;
};

//! Request information about current tasks(WsfXIO_TaskInitPkt), future task changes (WsfXIO_TaskUpdatePkt),
//! and optionally override the simulation's task management logic.
class WsfXIO_RIPRManagerRequestPkt : public WsfXIO_RequestDataPkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_RIPRManagerRequestPkt, WsfXIO_RequestDataPkt, 5)
   {
      WsfXIO_RequestDataPkt::Serialize(aBuff);
   }
};

class WsfXIO_RIPRManagerInitPkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_RIPRManagerInitPkt, WsfXIO_ResponsePkt, 6) { aBuff& mTime& mIdMap; }

   // The time the packet was created.
   double mTime;

   // the list of jobs currently in the jobboard.
   XIO_RIPRProcIdMap mIdMap;
};

class WsfXIO_RIPRManagerUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_RIPRManagerUpdatePkt, WsfXIO_ResponsePkt, 7) { aBuff& mTime& mIdMap; }

   // The time the packet was created.
   double mTime;

   // the list of jobs currently in the jobboard.
   XIO_RIPRProcIdMap mIdMap;
};

//! Sent when a RIPR Processor's Channel ID list changes.
class WsfXIO_BidControlUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   RIPR_DEFINE_PACKET(WsfXIO_BidControlUpdatePkt, WsfXIO_ResponsePkt, 8)
   {
      aBuff& PakSerialization::Enum(mUpdateType) & mPlatformIndex& mRIPRProcessorName& mBidControl;
   }

   //! Type of job update
   UpdateType mUpdateType;

   //! Index of the platform owning the task manager
   unsigned int mPlatformIndex;

   //! Name of the task processor
   WsfStringId mRIPRProcessorName;

   //! the number of job channels the parent RIPR processor supports.
   bool mBidControl;
};

#endif
