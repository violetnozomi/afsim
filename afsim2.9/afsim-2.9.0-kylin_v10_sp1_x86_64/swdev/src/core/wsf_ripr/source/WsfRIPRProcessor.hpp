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

#ifndef WSFRIPRPROCESSOR_HPP
#define WSFRIPRPROCESSOR_HPP

#include "UtCallbackHolder.hpp"
#include "UtWallClock.hpp"
#include "script/WsfScriptProcessor.hpp"
class WsfMessage;
class WsfTrack;
class WsfTrackMessage;
class WsfWeapon;
class WsfWeaponEngagement;
#include "WsfRIPRCommon.hpp"
#include "WsfTrackId.hpp"

class WsfProcessor;
class WsfRIPRJob;
class WsfRIPRJobBoard;
class UtInput;
class UtScript;
class WsfRIPRManager;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

class WsfRIPRProcessor : public WsfScriptProcessor
{
public:
   WsfRIPRProcessor(WsfScenario& aScenario, const char* aPretendName, bool aJobPassThrough = false);
   WsfRIPRProcessor(WsfScenario& aScenario, bool aJobPassThrough = false);

   WsfRIPRProcessor(WsfScenario&       aScenario,
                    const std::string& aPartClass,
                    const std::string& aPartVarName,
                    bool               aJobPassThrough = false);
   WsfRIPRProcessor& operator=(const WsfRIPRProcessor&) = delete;
   ~WsfRIPRProcessor() override;

   //! Hijack Bidding stops script calls from bidding for this processor.
   //! The reason you hijack bidding:
   //! 1) You want a human-in-the-loop to control bids.
   void BiddingHijacked(bool aValue);

   //! True means script calls to set bids for this processor are n
   bool BiddingHijacked() const { return mBiddingHijacked; }

   //! Create a clone of this object.
   //! @return A pointer to the new copy.
   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;

   void Update(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   const char* GetScriptClassName() const override;

   virtual bool RegisterScriptBiddingBlock(std::string aJobType, WsfScriptContext* aContextPtr, UtScript* aScriptPtr);

   virtual bool UnregisterScriptBiddingBlock(std::string aJobType);

   unsigned int GetId(unsigned int aIndex = 0) { return mJobChannelIds[aIndex]; }

   unsigned int NumJobChannels() const { return static_cast<unsigned int>(mJobChannelIds.size()); }
   void         SetNumJobChannels(unsigned int aNumChannels);
   int          JobChannelByProcId(unsigned int aRIPRProcId);

   //! Returns a copy of the Job Channels Id list.
   ripr::JobChannelIdList JobChannelIds() const { return mJobChannelIds; }

   std::vector<unsigned int> JobTypeChannels(std::string aJobType);
   std::vector<std::string>  ChannelJobTypes(unsigned int aChannelIndex);
   bool                      ValidJobTypeChannel(std::string aJobType, unsigned int aChannelIndex);

   double WallClock();

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   // commander stuff

   WsfRIPRProcessor* CommanderProcessor();
   WsfRIPRProcessor* CommanderProcessor(const std::string&);

   WsfPlatform* CommanderPlatform();
   WsfPlatform* CommanderPlatform(const std::string&);

   std::vector<WsfRIPRProcessor*> SubordinateProcessors();
   std::vector<WsfRIPRProcessor*> SubordinateProcessors(const std::string& aCommandChainName);

   std::vector<WsfPlatform*> SubordinatePlatforms();
   std::vector<WsfPlatform*> SubordinatePlatforms(const std::string& aCommandChainName);

   double QuerySubordinatesMaxBid(WsfRIPRJob& aJob, bool expandPassThroughAgents = true);

   std::vector<WsfPlatform*> Subordinates();
   ripr::RIPRCommanderList   Commanders();

   virtual WsfRIPRJobBoard* JobBoard();

   void SetJobPassThrough(bool aJobPassThrough);
   bool JobPassThrough() { return mJobPassThrough; }

   bool JobWindowOpen(double aSimTime);
   bool BidWindowOpen(double aSimTime);

   void SetJobWindowOpen(bool aOpen);
   void SetBidWindowOpen(bool aOpen);

   void SetJobStickiness(double aStickiness);

   double QueryBid(WsfRIPRJob& aJob);

   wsf::comm::Comm* JobBoardComm() { return mJobBoardCommPtr; }
   void             SetJobBoardComm(wsf::comm::Comm* aCommPtr) { mJobBoardCommPtr = aCommPtr; }

   // sent comm message to (probably) one of my subordinates with a job award
   void SendMessageJobAward(wsf::comm::Comm* aCommPtr, WsfRIPRProcessor* aSourcePtr, int aChannelIndex, int aJobId);

   // returns the job awarded to aProc/aChannelIndex; sends over comm if available and returns buffered job
   WsfRIPRJob* JobFor(double aSimTime, WsfRIPRProcessor* aProcPtr, int aChannelIndex);

   // get and set the jobs I've got buffered for each of my channels
   // job "-1" means no job has been assigned
   int  BufferedJobForChannel(int aChannelIndex) { return mBufferedJobs[aChannelIndex]; };
   void SetBufferedJobForChannel(int aChannelIndex, int aJob) { mBufferedJobs[aChannelIndex] = aJob; }

   // subordinate stuff

   // returns 1 if targeting, 0 otherwise
   int Targeting(const WsfTrack* aTrackPtr);

   int WeaponsActive(const WsfTrack& aTrack);
   int WeaponsActiveOfType(const WsfWeapon& aWeapon);

   int          WeaponsActive();
   WsfPlatform* ActiveWeaponPlatform(unsigned int aIndex);

   int SubsTargeting(const WsfTrack* aTrackPtr, const WsfPlatform* aExcludePlatform = nullptr);
   int SubsWeaponsActive(const WsfTrack* aTrackPtr, const WsfPlatform* aExcludePlatform = nullptr);

   int PeersWeaponsActive(const WsfTrack& aTrack);
   int PeersTargeting(const WsfTrack& aTrack);

   std::vector<WsfPlatform*> WeaponsIncoming();

   std::string  TargetName();
   WsfPlatform* TargetPlatform();
   size_t       TargetPlatformId();
   WsfTrack*    TargetTrack();

   bool SetTargetByName(std::string aTargetName);
   bool SetTargetByPlatform(WsfPlatform* aPlatformPtr);
   void SetTargetByPlatformId(size_t aTargetId);
   bool SetTargetByTrack(WsfTrack* aTargetTrackPtr);

   bool   ClearTarget();
   size_t mTargetPlatformIndex;

   // From here down taken from WsfTaskManager
   UtCallbackHolder mCallbacks;

   //! A list of active weapons fired in response to weapon fire task.
   //!
   //! An entry is created in response to the WsfObserver::WeaponFired callback.
   //! An entry is removed in response to the WsfObserver::WeaponTerminated callback.
   //!
   //! Note that entries are NOT removed when the associated task is purged.
   struct ActiveWeapon
   {
      //! The track ID of the track that was provided to WsfWeapon::Fire.
      WsfTrackId mTaskTrackId;
      //! The name of the weapon system that was fired. (NOT the name of the launched weapon).
      WsfStringId mWeaponSystemName;
      //! Platform index of the launched weapon platform.
      size_t mPlatformIndex;
      //! The serial number of the weapon engagement
      unsigned int mSerialNumber;
      //! The platform index of the target, not in WsfTaskManager
      size_t mTargetIndex;
   };

   using ActiveWeaponList     = std::list<ActiveWeapon>;
   using ActiveWeaponListIter = std::list<ActiveWeapon>::iterator;

   ActiveWeaponList mActiveWeaponList;

   //! Persistent track/weapon status data.
   //!
   //! This information is maintained on the assigners side and represents the on-going and historical
   //! data about weapon events. This information may be needed by user decision making processes.
   //!
   //! An entry exists for each specific track-and-weapon combination. Multiple fire events between the
   //! same track and weapon are merged into one entry.
   //!
   //! The entries for a given track are purged when the track is dropped.
   struct WeaponStatus
   {
   public:
      WeaponStatus(const WsfTrackId& aTrackId, size_t aAssigneeIndex, WsfStringId aWeaponSystemName)
         : mTrackId(aTrackId)
         , mAssigneeIndex(aAssigneeIndex)
         , mWeaponSystemName(aWeaponSystemName)
         , mTargetIndex(0)
         , mTimeWeaponLastFired(-1.0)
         , mTimeWeaponLastTerminated(-1.0)
         , mTotalSalvosFired(0)
         , mTotalRoundsFired(0)
         , mRoundsPending(0)
         , mRoundsActive(0)
         , mRoundsHit(0)
         , mTimeWeaponLastHit(-1)
         , mRoundsMissed(0)
         , mTimeWeaponLastMissed(-1)
      {
      }

      //! The track ID
      WsfTrackId mTrackId;

      //! The platform index of containing the weapon system.
      size_t mAssigneeIndex;

      //! The string ID of the name of the weapon system.
      WsfStringId mWeaponSystemName;

      //! The platform index of the target
      size_t mTargetIndex; // not from WsfTaskManager

      //! The simulation time when of the most recent weapon firing event for this task.
      double mTimeWeaponLastFired;

      //! The simulation time when of the most recent weapon termination event for this task.
      double mTimeWeaponLastTerminated;

      //! The number of salvos that have been fired.
      int mTotalSalvosFired;

      //! The total number of weapons that have been fired for this task.
      int mTotalRoundsFired;

      //! The number of weapons that are pending (requested but not yet in flight)
      int mRoundsPending;

      //! The number of weapons that are active (in flight).
      int mRoundsActive;

      //! The number of weapons that hit
      int mRoundsHit;

      //! The time the last weapon hit
      double mTimeWeaponLastHit;

      //! The number of weapons that missed
      int mRoundsMissed;

      //! The time the last weapon missed
      double mTimeWeaponLastMissed;
   };

   using WeaponStatusList     = std::list<WeaponStatus>;
   using WeaponStatusListIter = std::list<WeaponStatus>::iterator;
   WeaponStatusList mWeaponStatusList;

   WeaponStatusList::iterator FindWeaponStatus(const WsfTrackId& aTrackId,
                                               size_t            aAssigneeIndex,
                                               WsfStringId       aWeaponSystemName);

   //! @name Simulation observer callbacks
   //@{

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   void WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPlatformPtr);

   void WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPlatformPtr);


   bool UplinkCapable();

   bool StartUplinking(size_t receiverIndex, size_t targetIndex = 0);
   bool StartUplinking(WsfPlatform* receiverPtr, WsfTrack* targetPtr = nullptr);

   bool StopUplinking(size_t receiverIndex);
   bool StopUplinking(WsfPlatform* receiverPtr);

   bool IsUplinkingTo(int receiverIndex);
   bool IsUplinkingTo(WsfPlatform* receiverPtr);

   unsigned int    WeaponUplinkCount();
   WsfPlatform*    UplinkWeaponPlatform(unsigned int aIndex);
   WsfRIPRManager* GetManager() const { return mManagerPtr; }

protected:
   // Pointer to the RIPR manager if this processor has been added to the simulation
   WsfRIPRManager* mManagerPtr;
   // processor stuff
   ripr::JobChannelIdList mJobChannelIds;

   ripr::JobTypeChannelsMap mJobTypeChannels;
   using JobTypeIter = ripr::JobTypeChannelsMap::iterator;

   WsfRIPRProcessor(const WsfRIPRProcessor& aSrc);

   // commander stuff
   WsfRIPRJobBoard* mJobBoardPtr;
   bool             mJobPassThrough;

   // ripr job stuff
   // stores the local bidding script blocks until initialization, when they are registered into the map below
   std::vector<std::pair<std::string, UtScript*>> mBiddingBlocks;

   // holds the context & the script pointer of any named query_bid_type script blocks
   // this allows the blocks to exist on any context (hopefully just the processor)
   using QueryBidMapType = std::map<std::string, std::pair<WsfScriptContext*, UtScript*>>;
   QueryBidMapType mQueryBidTypeMap;

   // this is needed because we can't create job channels until Initialize(), not during construction
   unsigned int mDesiredJobChannels;

   UtWallClock mRiprWallClock;

   //! If true, this processor cannot bid from script calls.
   bool mBiddingHijacked;

   //! pointer to the WsfComm object to be used by mJobBoard
   wsf::comm::Comm* mJobBoardCommPtr;

   //! nameid of the WsfComm object to stick in mJobBoardComm during initialize; read during process input
   WsfStringId mJobBoardCommInput;

   //! map of which job id each channel id thinks it should currently be doing, indexed by job id
   ripr::RIPRChannelJobMap mBufferedJobs;
};

#endif
