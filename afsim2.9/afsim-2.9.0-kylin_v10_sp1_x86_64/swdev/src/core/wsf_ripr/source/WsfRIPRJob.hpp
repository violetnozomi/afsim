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

//***************************************************************************

//* FILE NAME:
//*   WsfRIPRJob.hpp
//*
//* PURPOSE:
//*   The WsfRIPRJob class stores information about one job that an agent can
//*   perform.  It is generally used with the WsfRIPRJobBoard.
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSFRIPRJOB_HPP
#define WSFRIPRJOB_HPP

#include <map>
#include <set>
#include <string>

#include "UtReferenceTracked.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
#include "WsfRIPRCommon.hpp"
#include "WsfRIPRJobBoard.hpp"
class WsfRIPRManager;
class WsfRIPRProcessor;
#include "script/WsfScriptProcessorClass.hpp"
class WsfSimulation;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

// WsfRIPRJob is a data class that holds information specifying what a job
// is and how it should be carried out.

class WsfRIPRJob : public WsfObject
{
public:
   WsfRIPRJob(WsfRIPRManager* aManagerPtr);
   // WsfRIPRJob(const WsfRIPRProcessor& aAssigner, std::string aJobName, std::string aJobDescription, double aPriority
   // = 0, int aMaxTakers = 1);
   WsfRIPRJob(WsfRIPRManager* aManagerPtr,
              unsigned int    aAssignerRIPRProcId,
              std::string&    aJobName,
              std::string&    aJobDescription,
              double          aPriority  = 0,
              int             aMaxTakers = 1);
   WsfRIPRJob(const WsfRIPRJob& aSrc);
   WsfRIPRJob* Clone() const override { return new WsfRIPRJob(*this); }
   ~WsfRIPRJob() override;

   WsfRIPRJob& operator=(const WsfRIPRJob& aSrc);

   void Update(double aSimTime);

   void BidJob(unsigned int aBidderRIPRProcId, double aBid, double aProgress);
   void BidJob(unsigned int aBidderRIPRProcId, double aBid);

   ripr::RIPRBidderList::iterator UnbidJob(unsigned int aQuitterRIPRProcId);

   void ClearBidders();

   double GetBid(unsigned int aRIPRProcId) const;

   ripr::RIPRBidderList GetBidders() const { return mBidders; }

   double GetBestProgress();

   void SetWinnersMax(const int aNum);
   void SetWinnersMin(const int aNum);

   void SetPriority(const double aPriority);

   void SetProgress(WsfRIPRProcessor* aBidderPtr, int aChannelIndex, double aProgress);
   void SetProgress(unsigned int aRIPRProcId, double aProgress);

   double GetProgress(unsigned int aBidderRIPRProcId);

   int GetRank(unsigned int aBidderRIPRProcId);

   int GetId() const;

   unsigned int AssignerProcessorId() const { return mAssignerProcessorId; }

   WsfStringId                 AssignerPlatformName() const;
   size_t                      AssignerPlatformIndex() const;
   WsfPlatform*                AssignerPlatform() const;
   WsfRIPRProcessor*           Assigner() const;
   WsfStringId                 AssignerProcessorName() const;
   double                      GetPriority() const { return mPriority; }
   int                         GetWinnersMax() const { return mWinnersMax; }
   int                         GetWinnersMin() const { return mWinnersMin; }
   std::string                 Description() const { return mDescription; }
   const ripr::RIPRJobDataMap& Data() const { return mData; }
   UtScriptData                Data(const std::string& aDataKey) const;

   static bool CompareBidders(const ripr::RIPRBidderPair& first, const ripr::RIPRBidderPair& second);

   static int NextId();

   static void Reset() { mNextJobId = 0; }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfObject::Serialize(aBuff);
      aBuff& mBidders& mProgress& mDescription& mData& mPriority& mWinnersMax& mBidsChanged& mBidWindowOpen&
         mAssignerProcessorId& mId& mCurrentTime;
   }

   //! Returns what this job believes to be the current time.  This is intended
   //! for internal use, but somebody could conceivably find an external use for it.
   double CurrentTime() const { return mCurrentTime; }

   void SetBidWindowOpen(const bool aValue) { mBidWindowOpen = aValue; }
   bool IsBidWindowOpen() const { return mBidWindowOpen; }

   // Job Board over comm methods
   void SendMessageSetProgress(wsf::comm::Comm* aCommPtr, WsfRIPRProcessor* aBidderPtr, int aChannelIndex, double aProgress);
   void            SendMessageBidJob(wsf::comm::Comm*  aCommPtr,
                                     WsfRIPRProcessor* aBidderPtr,
                                     int               aChannelIndex,
                                     double            aBid,
                                     double            aProgress = 0);
   void            SendMessageUnbidJob(wsf::comm::Comm* aCommPtr, WsfRIPRProcessor* aBidderPtr, int aChannelIndex);
   WsfRIPRManager* GetManager() const { return mManagerPtr; }
   WsfSimulation*  GetSimulation() const;

   WsfRIPRManager* mManagerPtr;

   //! list of pairs matching bidders (by proc id) to their bids (doubles)
   ripr::RIPRBidderList mBidders;

   //! mapping between proc ids and progress performing this job
   ripr::RIPRJobProgressMap mProgress;

   //! a human-readable string describing this job
   std::string mDescription;

   //! key/value pairs of auxiliary data attached to this job
   ripr::RIPRJobDataMap mData;

   std::set<int>& Dependencies() { return WsfRIPRJobBoard::DependenciesForJob(GetId()); };
   void           DependencyAdd(int aDependedOnJobId) { WsfRIPRJobBoard::DependencyAdd(GetId(), aDependedOnJobId); };
   void DependencyRemove(int aDependedOnJobId) { WsfRIPRJobBoard::DependencyRemove(GetId(), aDependedOnJobId); };
   bool DependencyCheck(int aDependedOnJobId) { return WsfRIPRJobBoard::DependencyCheck(GetId(), aDependedOnJobId); };
   int  DependencyCount() { return WsfRIPRJobBoard::DependencyCount(GetId()); };

   void   Touch();
   double TouchTime() { return mLastTouched; };

private:
   void Update();

   //! dirty bit set to true whenever a bid changes (bid, unbid, modify)
   bool mBidsChanged;

   //! can only bid when mBidWindowOpen is true (can unbid at any time)
   bool mBidWindowOpen;

   // priority and num winners trigger observer call on change, so must be
   // kept provide to ensure the "Set" function triggers the necessary
   // notification sequence.

   //! the priority level for this job.  only meaningful relative to priorities of other jobs on the same job board
   //! Accessed via Priority() & SetPriority()
   double mPriority;

   //! the maximum number of bidders who can win this job (though some allocation modes cause this limit to be exceeded)
   //! Accessed via GetMaxWinners() and SetMaxWinners().
   int mWinnersMax;

   //! the minimum number of winners required in order for anyone to win this job.  if mWinnersMin > 1 then job
   //! assignment will be slowed down and less optimal
   int mWinnersMin;

   //! the processor id of the WsfRIPRProcessor that created this job (typically)
   unsigned int mAssignerProcessorId;

   //! the globally unique id for this job
   //! access via JobId()
   int mId;

   //! keeps track of the current time and was created for observer calls outside
   //! the Update(time) method.
   double mCurrentTime;

   //! keeps track of when the job was last touched
   double mLastTouched;

   static int mNextJobId;
};


//! Define script methods for WsfRIPRJob.
class WsfScriptRIPRJobClass : public WsfScriptObjectClass
{
public:
   WsfScriptRIPRJobClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // See the base class for a description of Create, Clone, and Destroy.

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Create);
   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(Description);
   UT_DECLARE_SCRIPT_METHOD(GetName);
   UT_DECLARE_SCRIPT_METHOD(Priority);
   UT_DECLARE_SCRIPT_METHOD(SetPriority);
   UT_DECLARE_SCRIPT_METHOD(Winners);
   UT_DECLARE_SCRIPT_METHOD(WinnersActual);
   UT_DECLARE_SCRIPT_METHOD(WinnersMin);
   UT_DECLARE_SCRIPT_METHOD(WinnersMax);
   UT_DECLARE_SCRIPT_METHOD(Data_1);
   UT_DECLARE_SCRIPT_METHOD(Data_2);
   UT_DECLARE_SCRIPT_METHOD(SetWinnersMin);
   UT_DECLARE_SCRIPT_METHOD(SetWinnersMax);
   UT_DECLARE_SCRIPT_METHOD(SetData);
   UT_DECLARE_SCRIPT_METHOD(SetProgress_1);
   UT_DECLARE_SCRIPT_METHOD(SetProgress_2);
   UT_DECLARE_SCRIPT_METHOD(BestProgress);
   UT_DECLARE_SCRIPT_METHOD(Progress_1);
   UT_DECLARE_SCRIPT_METHOD(Progress_2);
   UT_DECLARE_SCRIPT_METHOD(BidJob_1);
   UT_DECLARE_SCRIPT_METHOD(BidJob_2);
   UT_DECLARE_SCRIPT_METHOD(BidJob_3);
   UT_DECLARE_SCRIPT_METHOD(BidJob_4);
   UT_DECLARE_SCRIPT_METHOD(UnbidJob_1);
   UT_DECLARE_SCRIPT_METHOD(UnbidJob_2);
   UT_DECLARE_SCRIPT_METHOD(ClearBidders);
   UT_DECLARE_SCRIPT_METHOD(GetBid_1);
   UT_DECLARE_SCRIPT_METHOD(GetBid_2);
   UT_DECLARE_SCRIPT_METHOD(Rank_1);
   UT_DECLARE_SCRIPT_METHOD(Rank_2);
   UT_DECLARE_SCRIPT_METHOD(Id);
   UT_DECLARE_SCRIPT_METHOD(Touch);     // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(TouchTime); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(DependenciesForJob);
   UT_DECLARE_SCRIPT_METHOD(DependencyAdd);
   UT_DECLARE_SCRIPT_METHOD(DependencyRemove);
   UT_DECLARE_SCRIPT_METHOD(DependencyCheck);
   UT_DECLARE_SCRIPT_METHOD(DependencyCount);
};

#endif
