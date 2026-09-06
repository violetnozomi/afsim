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


#ifndef WSFEXTENTITYDEADRECKON_HPP
#define WSFEXTENTITYDEADRECKON_HPP

#include "wsf_export.h"

#include <map>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"

class UtInput;
class WsfMover;
class WsfPlatform;
class WsfExtInterface;


struct WSF_EXPORT WsfExtEntityDeadReckonSetup
{
   WsfExtEntityDeadReckonSetup();
   bool   mDR_ServiceRequested;
   double mHeartbeatTimer;
   double mMoverUpdateTimer;
   double mInitialDistributionInterval;

   double mEntityPositionThreshold;
   double mEntityOrientationThreshold;
};

//! Provides a central place where external interfaces can register for entity position updates
//! using a dead reckoning algorithm.  This allows for DR to be computed only once for each
//! internal platform, instead of once for each interface.
class WSF_EXPORT WsfExtEntityDeadReckon : public WsfExtEntityDeadReckonSetup
{
public:
   WsfExtEntityDeadReckon(WsfExtInterface* aExtInterfacePtr);

   void Initialize();

   bool ProcessInput(UtInput& aInput);

   void RequestService(bool aHeartbeatRequired);

   void ForceUpdate(double aSimTime, WsfPlatform* aPlatformPtr, bool aForceSend = false);

   void RequireExternalEntities(bool aExternalEntitiesRequired);

   //! Requests that the initial distribution interval is to be used.
   void UseInitialDistributionInterval() { mUseInitialDistrubutionInterval = true; }

   //! Return the heartbeat timer (seconds).
   double GetHeartbeatTimer() const { return mHeartbeatTimer; }

   //! Set the current heartbeat timer for this individual platform.
   void SetHeartbeatTimerOverride(double aTime) { mHeartbeatTimerOverride = aTime; }

   //! Return the orientation threshold (radians).
   double GetEntityOrientationThreshold() const { return mEntityOrientationThreshold; }

   //! Return the position threshold (meters).
   double GetEntityPositionThreshold() const { return mEntityPositionThreshold; }

   double GetInitialDistributionInterval() const { return mInitialDistributionInterval; }

   //! Allow the initial distribution interval to be set.
   void SetInitialDistributionInterval(double aValue) { mInitialDistributionInterval = aValue; }

   double GetMoverUpdateTimer() const { return mMoverUpdateTimer; }
   void   SetMoverUpdateTimer(double aMoverUpdateTimer) { mMoverUpdateTimer = aMoverUpdateTimer; }

   //! Stores the state of an entity
   struct DR_State
   {
      DR_State();
      void LoadFromPlatform(double aSimTime, WsfPlatform& aPlatform);
      void Extrapolate(double aDeltaTime, WsfPlatform& aPlatform, DR_State& aExtrapState);

      double mLocationWCS[3];
      double mVelocityWCS[3];
      double mAccelerationWCS[3];
      double mOrientationWCS[3];
      double mPastOrientationWCS[3];
      double mAngularRateWCS[3];
      double mPastAngularRateWCS[3];
   };

   enum UpdateType
   {
      cNO_CHANGE = 0,
      cDR_CHANGE = 1,
      cHEARTBEAT = 2
   };

   //! Maintains a platform's dead reckon state
   class PlatformDR
   {
   public:
      PlatformDR(double aSimTime, WsfPlatform* aPlatformPtr);
      PlatformDR();

      bool         mInitialized;
      bool         mIsUpdated;
      UpdateType   mLastUpdateType;
      double       mLastSendTime;
      double       mLastTimeMoverUpdate;
      double       mHeartbeatTimer;
      WsfPlatform* mPlatformPtr;
      // State before MoverUpdated was last called with 'true'
      DR_State mSentState;
      // State before MoverUpdated was last called
      DR_State mUpdatedState;
   };

   //! Invoked every time a platform's dead reckon has changed.
   //! This is guaranteed to be called at least once every heartbeat interval
   UtCallbackListN<void(double, PlatformDR&)> DeadReckonChanged;

   //! Invoked every time GetSimulation()->GetObserver().MoverUpdated is called.
   //! The third parameter is 'true' if the dead reckon state was updated
   UtCallbackListN<void(double, PlatformDR&)> MoverUpdated;

private:
   void UpdateDeadReckon(double aSimTime, WsfPlatform* aPlatformPtr, bool aForceUpdate);

   bool IsStateAccurate(double aSimTime, PlatformDR& aState, bool aForceUpdate);

   void HandlePlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);
   void HandlePlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);
   void HandleMoverUpdated(double aSimTime, WsfMover* aMoverPtr);

   double HandleHeartbeat(double aSimTime, size_t aPlatformIndex);

   double CalculateHeartbeatTimer(double aAverageHeartbeatTimer, double aMultiplier) const;

   //! Executes at a regular interval ensuring that a platform's mover is
   //! updated regularly.
   class HeartbeatEvent : public WsfEvent
   {
   public:
      HeartbeatEvent() = delete;
      HeartbeatEvent(double aSimTime, WsfExtEntityDeadReckon* aExtPtr, size_t aPlatformIndex)
         : WsfEvent(aSimTime)
         , mExtPtr(aExtPtr)
         , mPlatformIndex(aPlatformIndex)
      {
      }
      EventDisposition Execute() override;

      WsfExtEntityDeadReckon* mExtPtr;
      size_t                  mPlatformIndex;
   };

   UtCallbackHolder mCallbacks;

   std::map<size_t, PlatformDR> mDeadReckonData;
   bool                         mExternalEntitiesRequired;
   //! For interfaces like DIS, we need to send out a new state at every heartbeat
   //! This option ensures we send out a new DR state
   bool mHeartbeatRequired;

   bool   mInitialized;
   bool   mUseSimpleExtrapolations;
   bool   mUseInitialDistrubutionInterval;
   double mHeartbeatTimerOverride;
   double mInitialDistributionStart;

   WsfExtInterface* mExtInterfacePtr;
};

#endif
