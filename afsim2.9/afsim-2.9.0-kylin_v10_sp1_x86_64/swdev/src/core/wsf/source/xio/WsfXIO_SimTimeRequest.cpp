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

#include "xio/WsfXIO_SimTimeRequest.hpp"

#include "WsfRealTimeClockSource.hpp"
#include "WsfSimulation.hpp"

WsfXIO_SimTimeRequest::WsfXIO_SimTimeRequest(WsfXIO_Connection*                  aConnectionPtr,
                                             WsfXIO_RequestSimTimePkt::TimerType aTimerType,
                                             double                              aInterval,
                                             bool                                aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
{
   mRequest.mTimerInterval = aInterval;
   mRequest.mTimerType     = aTimerType;
}

void WsfXIO_SimTimeRequest::Initialized()
{
   SendRequest(mRequest);
}

WsfXIO_TimeSynchronization::WsfXIO_TimeSynchronization(WsfXIO_Connection* aConnectionPtr, WsfSimulation* aSimulationPtr)
   : WsfXIO_SimTimeRequest(aConnectionPtr)
   , mSmoothRate(true)
   , mSynPackets(true)
   , mIsInitialized(false)
   , mJumpToRemoteTime(false)
   , mSyncWithSimulation(aSimulationPtr)
{
   mTargetTimeOffset   = 0.0;
   mMinimumErrorOffset = -1.0;
   mMaximumErrorOffset = 1.0;
   SetUpdateInterval(mMaximumErrorOffset - mMinimumErrorOffset);
   WsfSimulation* simPtr = aSimulationPtr;
   if (mSyncWithSimulation && simPtr)
   {
      mClockSourcePtr = (WsfClockSource*)simPtr->GetClockSource();
   }
   else
   {
      mClockSourcePtr = new WsfRealTimeClockSource;
      ((WsfRealTimeClockSource*)mClockSourcePtr)->SetQuietMode(true);
   }
}

WsfXIO_TimeSynchronization::~WsfXIO_TimeSynchronization()
{
   // Disable packet synchronization
   if (mSyncWithSimulation)
   {
      GetConnection()->SetHasClockTranslation(false);
   }
   else
   {
      delete mClockSourcePtr;
   }
}

void WsfXIO_TimeSynchronization::SetClockSource(WsfClockSource* aClockSourcePtr)
{
   delete mClockSourcePtr;
   mClockSourcePtr     = aClockSourcePtr;
   mSyncWithSimulation = nullptr;
}

//! Sets the parameters for the synchronization.
//! @param aTargetTimeOffset Desired amount of time offset between this application and the remote application.
//! @param aMinimumTimeError Minimum value of TError before the simulation clock is advanced
//! @param aMaximumTimeError Maximum value of TError before the simulation clock is paused
//! @param aTimerUpdateInterval Time interval between clock updates.
//! @note TError = LocalTime - AdjustedRemoteTime + TargetTimeOffset
//!       AdjustedRemoteTime = (RemoteTime + InitialTimeOffset)
//!       InitialTimeOffset = (LocalTime - RemoteTime) at startup
void WsfXIO_TimeSynchronization::SetTiming(double aTargetTimeOffset,
                                           double aMinimumTimeError,
                                           double aMaximumTimeError,
                                           double aTimerUpdateInterval)
{
   mTargetTimeOffset   = aTargetTimeOffset;
   mMinimumErrorOffset = aMinimumTimeError;
   mMaximumErrorOffset = aMaximumTimeError;
   SetUpdateInterval(aTimerUpdateInterval);
}

//! Synchronize this simulation with the remote simulation
void WsfXIO_TimeSynchronization::InitializeSync(WsfXIO_SimTimePkt& aPkt)
{
   // Cannot use multiple time synchronizations for a single connection
   if (mSyncWithSimulation)
   {
      assert(!GetConnection()->HasClockTranslation());
   }

   UpdateState(aPkt);
   double simTimeNow = mClockSourcePtr->GetClock(1.0E300);
   // Use this offset to convert remote sim time to local sim time
   if (mJumpToRemoteTime)
   {
      mBaseOffset = 0;
      mClockSourcePtr->SetClock(aPkt.mSimTime);
   }
   else
   {
      mBaseOffset = simTimeNow - aPkt.mSimTime + -mTargetTimeOffset;
   }

   if (mSyncWithSimulation)
   {
      mSyncWithSimulation->SetClockRate(aPkt.mClockRate);
      GetConnection()->SetClockOffset(mBaseOffset);
      GetConnection()->SetHasClockTranslation(mSynPackets);
   }
   else
   {
      mClockSourcePtr->SetClockRate(aPkt.mClockRate);
   }

   mIsInitialized = true;
}

//! Update the local clock source to use the same clock rate and stopped state
void WsfXIO_TimeSynchronization::UpdateState(WsfXIO_SimTimePkt& aPkt)
{
   bool remotePaused = ((aPkt.mState & WsfXIO_SimTimePkt::cPAUSED) != 0);
   if (mClockSourcePtr->IsStopped() != remotePaused)
   {
      if (mSyncWithSimulation)
      {
         if (remotePaused)
         {
            mSyncWithSimulation->Pause();
         }
         else
         {
            mSyncWithSimulation->Resume();
         }
      }
      else
      {
         if (remotePaused)
         {
            mClockSourcePtr->StopClock();
         }
         else
         {
            mClockSourcePtr->StartClock();
         }
      }
   }
}

void WsfXIO_TimeSynchronization::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   WsfXIO_SimTimePkt& pkt = (WsfXIO_SimTimePkt&)aPkt;
   mLastState             = pkt;
   if (!mIsInitialized)
   {
      InitializeSync(pkt);
      return;
   }

   UpdateState(pkt);

   double adjustedRemoteTime = pkt.mSimTime + mBaseOffset;
   mClockSourcePtr->SetMaximumClock(adjustedRemoteTime + mMaximumErrorOffset + GetUpdateInterval());
   double simTimeNow         = mClockSourcePtr->GetClock(1.0E300);
   double simTimeErrorOffset = simTimeNow - adjustedRemoteTime;

   if (!mClockSourcePtr->IsStopped())
   {
      if (simTimeErrorOffset < mMinimumErrorOffset)
      {
         // Clock fallen behind
         mClockSourcePtr->StopClock();
         mClockSourcePtr->SetClock(adjustedRemoteTime + mMinimumErrorOffset);
         mClockSourcePtr->StartClock();
      }
      if (mSmoothRate && mSyncWithSimulation)
      {
         if (simTimeErrorOffset < mMinimumErrorOffset / 2.0)
         {
            mSyncWithSimulation->SetClockRate(pkt.mClockRate * 1.05);
         }
         else if (simTimeErrorOffset > mMaximumErrorOffset / 2.0)
         {
            mSyncWithSimulation->SetClockRate(pkt.mClockRate * 0.95);
         }
         else
         {
            mSyncWithSimulation->SetClockRate(pkt.mClockRate);
         }
      }
      else
      {
         if (mSyncWithSimulation)
         {
            mSyncWithSimulation->SetClockRate(pkt.mClockRate);
         }
         else
         {
            mClockSourcePtr->SetClockRate(pkt.mClockRate);
         }
      }
   }
}
