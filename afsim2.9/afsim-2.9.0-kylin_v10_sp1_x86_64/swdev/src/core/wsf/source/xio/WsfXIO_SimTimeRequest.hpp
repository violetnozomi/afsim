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

#ifndef WSFXIO_SIMTIMEREQUEST_HPP
#define WSFXIO_SIMTIMEREQUEST_HPP

#include "wsf_export.h"

class WsfClockSource;
class WsfSimulation;
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Request.hpp"

//! Requests information about a remote simulation's clock.
//! Receives WsfXIO_SimTimePkt on a regular interval in addition to clock pause/resume/rate-change events.
class WSF_EXPORT WsfXIO_SimTimeRequest : public WsfXIO_Request
{
public:
   WsfXIO_SimTimeRequest(WsfXIO_Connection*                  aConnectionPtr,
                         WsfXIO_RequestSimTimePkt::TimerType aTimerType  = WsfXIO_RequestSimTimePkt::cSIM_TIME_INTERVAL,
                         double                              aInterval   = 1.0,
                         bool                                aIsReliable = true);

   void SetUpdateInterval(double aSimTimeInterval) { mRequest.mTimerInterval = aSimTimeInterval; }

   double GetUpdateInterval() const { return mRequest.mTimerInterval; }

   void SetTimerType(WsfXIO_RequestSimTimePkt::TimerType aTimerType) { mRequest.mTimerType = aTimerType; }

private:
   void Initialized() override;

   WsfXIO_RequestSimTimePkt mRequest;
};

//! WsfXIO_TimeSynchronization attempts to synchronize the local simulation clock with a remote simulation
//!* Controls the local clock when remote simulation pauses, resumes or changes clock rate.
//!* Upon timing errors, local clock is adjusted to reflect remote clock.
//!* Configures the connection to translate the WsfXIO_Packet::GetTimeStamp() value into local time.
//!* Timing parameters allow for a delayed synchronization where the local clock is behind the remote
//!  clock.  In this situation, received packets marked with the cSYNCHRONIZED flag will be buffered
//!  processed at the correct simulation time for the packet time stamp.
//!@note WsfXIO_TimeSynchronization is currently limited to realtime simulations
class WSF_EXPORT WsfXIO_TimeSynchronization : public WsfXIO_SimTimeRequest
{
public:
   WsfXIO_TimeSynchronization(WsfXIO_Connection* aConnectionPtr, WsfSimulation* aSimulationPtr);

   ~WsfXIO_TimeSynchronization() override;

   void JumpToRemoteTime(bool aDoJump) { mJumpToRemoteTime = aDoJump; }

   void SetTiming(double aTargetTimeOffset, double aMinimumTimeError, double aMaximumTimeError, double aTimerUpdateInterval);

   void HandleResponse(WsfXIO_ResponsePkt& aPkt) override;

   void            SetSmoothRate(bool aSmoothRate) { mSmoothRate = aSmoothRate; }
   void            SetSyncPackets(bool aDoSync) { mSynPackets = aDoSync; }
   WsfClockSource* GetClockSource() const { return mClockSourcePtr; }
   void            SetClockSource(WsfClockSource* aClockSourcePtr);

   WsfXIO_SimTimePkt mLastState;

private:
   void InitializeSync(WsfXIO_SimTimePkt& aPkt);

   void UpdateState(WsfXIO_SimTimePkt& aPkt);

   double          mBaseOffset;
   bool            mSmoothRate;
   bool            mSynPackets;
   bool            mIsInitialized;
   bool            mJumpToRemoteTime;
   WsfSimulation*  mSyncWithSimulation;
   WsfClockSource* mClockSourcePtr;
   double          mTargetTimeOffset;
   double          mMinimumErrorOffset;
   double          mMaximumErrorOffset;
};

#endif
