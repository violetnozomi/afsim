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

#include "ext/WsfExtEntityDeadReckon.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "ext/WsfExtInterface.hpp"
WsfExtEntityDeadReckon::WsfExtEntityDeadReckon(WsfExtInterface* aExtInterfacePtr)
   : WsfExtEntityDeadReckonSetup(aExtInterfacePtr->mDR_Setup)
   , mExternalEntitiesRequired(false)
   , mHeartbeatRequired(false)
   , mInitialized(false)
   , mUseSimpleExtrapolations(false)
   , mUseInitialDistrubutionInterval(false)
   , mHeartbeatTimerOverride(0.0)
   , mInitialDistributionStart(0.0)
   , mExtInterfacePtr(aExtInterfacePtr)
{
}

void WsfExtEntityDeadReckon::Initialize()
{
   WsfSimulation& sim        = mExtInterfacePtr->GetSimulation();
   mInitialDistributionStart = sim.GetSimTime();
   if (!mDR_ServiceRequested)
   {
      mCallbacks.Clear();
      return;
   }
   if (mMoverUpdateTimer == 0.0)
   {
      mMoverUpdateTimer = 1.0E9;
   }
   mInitialDistributionInterval = mHeartbeatTimer;
   if (!sim.IsEventStepSimulation() || !mUseInitialDistrubutionInterval)
   {
      mInitialDistributionInterval = 0;
   }
   mCallbacks.Clear();
   const WsfSimulation* simPtr = &(mExtInterfacePtr->GetSimulation());
   mCallbacks += WsfObserver::PlatformDeleted(simPtr).Connect(&WsfExtEntityDeadReckon::HandlePlatformDeleted, this);
   mCallbacks += WsfObserver::PlatformAdded(simPtr).Connect(&WsfExtEntityDeadReckon::HandlePlatformAdded, this);
   mCallbacks += WsfObserver::MoverUpdated(simPtr).Connect(&WsfExtEntityDeadReckon::HandleMoverUpdated, this);
   for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
   {
      WsfPlatform* platformPtr = sim.GetPlatformEntry(i);
      HandlePlatformAdded(sim.GetSimTime(), platformPtr);
   }
   mInitialized = true;
}

//! Request that WsfExtEntityDeadReckon is used.  If this method isn't called, we assume we can do no work.
//! @param aHeartbeatRequired When 'true', WsfExtEntityDeadReckon will enforce updating the dead reckon state
//!                           at a minimum of every heartbeat timer interval.
void WsfExtEntityDeadReckon::RequestService(bool aHeartbeatRequired)
{
   mHeartbeatRequired = mHeartbeatRequired || aHeartbeatRequired;

   // Reinitialize if required
   if (!mDR_ServiceRequested && !mInitialized)
   {
      mDR_ServiceRequested = true;
      Initialize();
   }
   else
   {
      mDR_ServiceRequested = true;
   }
}

bool WsfExtEntityDeadReckon::ProcessInput(UtInput& aInput)
{
   return false;
}

//! Forces an update of the dead reckoning for a specific platform.  The caller should ensure
//! the platform is updated to the current time before calling this method.
void WsfExtEntityDeadReckon::ForceUpdate(double aSimTime, WsfPlatform* aPlaformPtr, bool aForceSend)
{
   if ((mDeadReckonData.find(aPlaformPtr->GetIndex()) != mDeadReckonData.end()) &&
       (mDeadReckonData[aPlaformPtr->GetIndex()].mInitialized))
   {
      UpdateDeadReckon(aSimTime, aPlaformPtr, aForceSend);
   }
}

void WsfExtEntityDeadReckon::HandlePlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (mDeadReckonData.find(aPlatformPtr->GetIndex()) == mDeadReckonData.end())
   {
      mDeadReckonData[aPlatformPtr->GetIndex()] = PlatformDR();
   }
   if (mExternalEntitiesRequired || !aPlatformPtr->IsExternallyControlled())
   {
      PlatformDR& dr  = mDeadReckonData[aPlatformPtr->GetIndex()];
      dr              = PlatformDR(aSimTime, aPlatformPtr);
      dr.mInitialized = true;
      if ((mHeartbeatRequired && (mHeartbeatTimer > 0.0)) || (mMoverUpdateTimer > 0.0))
      {
         dr.mHeartbeatTimer = CalculateHeartbeatTimer(mHeartbeatTimer, mExtInterfacePtr->GetRandom().Uniform(0.9, 1.1));
         double initialHeartbeatTime = aSimTime;
         if (initialHeartbeatTime - mInitialDistributionStart < mInitialDistributionInterval &&
             mUseInitialDistrubutionInterval)
         {
            initialHeartbeatTime =
               CalculateHeartbeatTimer(mInitialDistributionInterval, mExtInterfacePtr->GetRandom().Uniform<double>()) +
               mExtInterfacePtr->GetDeferredConnectionTime() + mInitialDistributionStart;
         }
         else
         {
            initialHeartbeatTime = mExtInterfacePtr->GetDeferredConnectionTime();
         }
         if (initialHeartbeatTime < aSimTime)
         {
            initialHeartbeatTime = aSimTime;
         }
         mExtInterfacePtr->GetSimulation().AddEvent(
            ut::make_unique<HeartbeatEvent>(initialHeartbeatTime, this, aPlatformPtr->GetIndex()));
      }
   }
}

void WsfExtEntityDeadReckon::HandlePlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   mDeadReckonData.erase(aPlatformPtr->GetIndex());
}

void WsfExtEntityDeadReckon::HandleMoverUpdated(double aSimTime, WsfMover* aMoverPtr)
{
   UpdateDeadReckon(aSimTime, aMoverPtr->GetPlatform(), false);
}

void WsfExtEntityDeadReckon::UpdateDeadReckon(double aSimTime, WsfPlatform* aPlatformPtr, bool aForceUpdate)
{
   if (!mExternalEntitiesRequired && aPlatformPtr->IsExternallyControlled())
   {
      return;
   }

   if ((mDeadReckonData.find(aPlatformPtr->GetIndex()) == mDeadReckonData.end()))
   {
      return;
   }

   PlatformDR& dr = mDeadReckonData[aPlatformPtr->GetIndex()];
   dr.mIsUpdated  = true;
   bool force     = aForceUpdate;
   if (!force && mHeartbeatRequired)
   {
      force = (aSimTime - dr.mLastSendTime) > dr.mHeartbeatTimer;
   }

   bool isAccurate = IsStateAccurate(aSimTime, dr, force);

   // Set the update type
   dr.mLastUpdateType = cNO_CHANGE;
   if (!isAccurate)
   {
      dr.mLastUpdateType = force ? cHEARTBEAT : cDR_CHANGE;
   }

   // Notify observers
   MoverUpdated(aSimTime, dr);
   if (!isAccurate)
   {
      DeadReckonChanged(aSimTime, dr);
   }
}

double WsfExtEntityDeadReckon::HandleHeartbeat(double aSimTime, size_t aPlatformIndex)
{
   if (mDeadReckonData.find(aPlatformIndex) == mDeadReckonData.end())
   {
      return -1.0;
   }

   double nextUpdateTime = -1.0;

   PlatformDR& dr = mDeadReckonData[aPlatformIndex];

   double heartbeatTimer          = dr.mHeartbeatTimer;
   double lastTimeEntityStateSent = dr.mLastSendTime;
   double lastTimeMoverUpdated    = dr.mLastTimeMoverUpdate;
   bool   heartbeatIntervalExpired =
      (mHeartbeatRequired) && (aSimTime >= (lastTimeEntityStateSent + heartbeatTimer - 1.0E-6));
   bool moverUpdateIntervalExpired = (aSimTime >= (lastTimeMoverUpdated + mMoverUpdateTimer - 1.0E-6));
   bool updateRequired             = (heartbeatIntervalExpired) || (moverUpdateIntervalExpired);
   // double lastPlatformUpdateTime = dr.mPlatformPtr->GetLastUpdateTime();
   dr.mIsUpdated = false;
   if (updateRequired)
   {
      dr.mPlatformPtr->Update(aSimTime);
   }

   // Now we must check the validity of the mPlatformPtr again, because
   // the platform update may result in a GetSimulation()->GetObserver().PlatformDeleted call,
   // which in turn deletes the dis platform object.
   if (dr.mPlatformPtr != nullptr)
   {
      if (heartbeatIntervalExpired)
      {
         // Re-randomize the heartbeat timer for this platform +/- 10%
         dr.mHeartbeatTimer = CalculateHeartbeatTimer(mHeartbeatTimer, mExtInterfacePtr->GetRandom().Uniform(0.9, 1.1));
         // The above call to WsfPlatform::Update may not result in a call to
         // GetSimulation()->GetObserver().MoverUpdated. If the mover update time is not modified then MoverUpdated will
         // be called explicitly. if (lastPlatformUpdateTime == dr.mPlatformPtr->GetLastUpdateTime())
         if (!dr.mIsUpdated)
         {
            UpdateDeadReckon(aSimTime, dr.mPlatformPtr, heartbeatIntervalExpired);
         }
      }
      else if (updateRequired && !dr.mIsUpdated)
      {
         UpdateDeadReckon(aSimTime, dr.mPlatformPtr, heartbeatIntervalExpired);
      }

      if (aSimTime >= mExtInterfacePtr->GetDeferredConnectionTime())
      {
         nextUpdateTime = dr.mLastTimeMoverUpdate + mMoverUpdateTimer;
      }
      else
      {
         nextUpdateTime = mExtInterfacePtr->GetDeferredConnectionTime();
      }
      if (mHeartbeatRequired)
      {
         double nextHeartbeatUpdate = dr.mPlatformPtr->GetLastUpdateTime() + dr.mHeartbeatTimer;
         nextUpdateTime             = std::min(nextHeartbeatUpdate, nextUpdateTime);
      }
      nextUpdateTime = std::max(nextUpdateTime, aSimTime + 1.0E-3);
   }

   return nextUpdateTime;
}
bool WsfExtEntityDeadReckon::IsStateAccurate(double aSimTime, PlatformDR& aState, bool aForceUpdate)
{
   // Determine if the PDU needs to be sent according to the DIS criteria

   WsfPlatform* platformPtr = aState.mPlatformPtr;
   // if (platformPtr->IsPaused() || platformPtr->IsStopped())

   bool   isAccurate = !aForceUpdate;
   double aDT        = aSimTime - aState.mLastSendTime;

   if (!platformPtr->IsExternallyMoved())
   {
      // Dead reckon from the last sent values and determine if the current values are within limits
      DR_State extrapState;
      aState.mSentState.Extrapolate(aDT, *platformPtr, extrapState);
      aState.mUpdatedState.LoadFromPlatform(aSimTime, *platformPtr);
      aState.mLastTimeMoverUpdate = aSimTime;
      if (isAccurate)
      {
         DR_State& currentState = aState.mUpdatedState;

         double dx    = currentState.mLocationWCS[0] - extrapState.mLocationWCS[0];
         double dy    = currentState.mLocationWCS[1] - extrapState.mLocationWCS[1];
         double dz    = currentState.mLocationWCS[2] - extrapState.mLocationWCS[2];
         double dxyz2 = (dx * dx) + (dy * dy) + (dz * dz);

         double dpsi   = fabs(currentState.mOrientationWCS[0] - extrapState.mOrientationWCS[0]);
         double dtheta = fabs(currentState.mOrientationWCS[1] - extrapState.mOrientationWCS[1]);
         double dphi   = fabs(currentState.mOrientationWCS[2] - extrapState.mOrientationWCS[2]);
         if (dxyz2 > (mEntityPositionThreshold * mEntityPositionThreshold))
         {
            isAccurate = false;
         }
         else if ((dpsi > mEntityOrientationThreshold) || (dtheta > mEntityOrientationThreshold) ||
                  (dphi > mEntityOrientationThreshold))
         {
            isAccurate = false;
         }
      }
   }
   if (!isAccurate)
   {
      aState.mLastSendTime = aSimTime;
      aState.mSentState    = aState.mUpdatedState;
   }
   return isAccurate;
}

void WsfExtEntityDeadReckon::RequireExternalEntities(bool aExternalEntitiesRequired)
{
   if (!mExternalEntitiesRequired && aExternalEntitiesRequired)
   {
      mExternalEntitiesRequired = true;
      WsfSimulation& sim        = mExtInterfacePtr->GetSimulation();
      for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
      {
         WsfPlatform* platformPtr = sim.GetPlatformEntry(i);
         if (platformPtr->IsExternallyControlled())
         {
            HandlePlatformAdded(sim.GetSimTime(), platformPtr);
         }
      }
   }
}

void WsfExtEntityDeadReckon::DR_State::Extrapolate(double aDeltaTime, WsfPlatform& aPlatform, DR_State& aExtrapState)
{
   double dt      = aDeltaTime;
   double halfDt2 = 0.5 * dt * dt;
   for (int i = 0; i < 3; ++i)
   {
      aExtrapState.mAccelerationWCS[i] = mAccelerationWCS[i];
      aExtrapState.mOrientationWCS[i]  = mOrientationWCS[i] + (mAngularRateWCS[i] * dt);
      aExtrapState.mLocationWCS[i]     = mLocationWCS[i] + (mVelocityWCS[i] * dt) + (mAccelerationWCS[i] * halfDt2);
      aExtrapState.mVelocityWCS[i]     = mVelocityWCS[i] + (mAccelerationWCS[i] * dt);
   }
}

WsfExtEntityDeadReckon::PlatformDR::PlatformDR(double aSimTime, WsfPlatform* aPlatformPtr)
   : mPlatformPtr(aPlatformPtr)
{
   mInitialized         = false;
   mLastSendTime        = -1.0E9;
   mLastTimeMoverUpdate = aSimTime;
   mSentState.LoadFromPlatform(aSimTime, *aPlatformPtr);
   mUpdatedState = mSentState;
}

WsfExtEntityDeadReckon::PlatformDR::PlatformDR()
   : mLastUpdateType(WsfExtEntityDeadReckon::cNO_CHANGE)
   , mPlatformPtr(nullptr)
{
}

//! Calculate the heartbeat timer interval.
//! This method calculates a random interval used to update the platform's mover.
//! If the minimum mover timestep is nonzero, the value is further modified to be an integral number of minimum mover timesteps.
double WsfExtEntityDeadReckon::CalculateHeartbeatTimer(double aAverageHeartbeatTimer, double aMultiplier) const
{
   double heartbeatTimer       = aAverageHeartbeatTimer * aMultiplier;
   double minimumMoverTimestep = mExtInterfacePtr->GetSimulation().GetMinimumMoverTimestep();
   if (minimumMoverTimestep > 0.0)
   {
      if (heartbeatTimer < minimumMoverTimestep)
      {
         heartbeatTimer = minimumMoverTimestep;
      }
      else // set to the integral number of minimum timesteps in heartbeatTimer
      {
         heartbeatTimer = std::floor(heartbeatTimer / minimumMoverTimestep) * minimumMoverTimestep;
      }
   }
   return heartbeatTimer;
}

WsfExtEntityDeadReckon::DR_State::DR_State()
{
   for (size_t i = 0; i < 3; ++i)
   {
      mLocationWCS[i]        = 0.0;
      mVelocityWCS[i]        = 0.0;
      mAccelerationWCS[i]    = 0.0;
      mOrientationWCS[i]     = 0.0;
      mPastOrientationWCS[i] = 0.0;
      mAngularRateWCS[i]     = 0.0;
      mPastAngularRateWCS[i] = 0.0;
   }
}

void WsfExtEntityDeadReckon::DR_State::LoadFromPlatform(double aSimTime, WsfPlatform& aPlatform)
{
   aPlatform.GetLocationWCS(mLocationWCS);
   aPlatform.GetOrientationWCS(mOrientationWCS[0], mOrientationWCS[1], mOrientationWCS[2]);
   if (aPlatform.IsStopped() || aPlatform.IsPaused())
   {
      for (int i = 0; i < 3; ++i)
      {
         mVelocityWCS[i] = mAccelerationWCS[i] = 0.0;
      }
   }
   else
   {
      aPlatform.GetVelocityWCS(mVelocityWCS);
      aPlatform.GetAccelerationWCS(mAccelerationWCS);
      // This is important due to thresholds on mover update times, it may not update to quite
      // the right time.  This will allow us to estimate the platform's state more closely.
      if (aPlatform.GetMover() != nullptr)
      {
         double lastUpdateTime = aPlatform.GetMover()->GetLastUpdateTime();
         if (lastUpdateTime < aSimTime)
         {
            double dt      = aSimTime - lastUpdateTime;
            double halfDt2 = 0.5 * dt * dt;
            for (int i = 0; i < 3; ++i)
            {
               mLocationWCS[i] += (mVelocityWCS[i] * dt) + (mAccelerationWCS[i] * halfDt2);
               mVelocityWCS[i] += mAccelerationWCS[i] * dt;
               // Add on the angular rates calculated in WsfDisPlatform::UseSimpleAccelerations
               mOrientationWCS[i] += mAngularRateWCS[i] * dt;
            }
         }
      }
   }
}

// virtual
WsfEvent::EventDisposition WsfExtEntityDeadReckon::HeartbeatEvent::Execute()
{
   double nextTime = mExtPtr->HandleHeartbeat(GetTime(), mPlatformIndex);
   if (nextTime >= 0)
   {
      SetTime(nextTime);
      return cRESCHEDULE;
   }
   return cDELETE;
}

WsfExtEntityDeadReckonSetup::WsfExtEntityDeadReckonSetup()
   : mDR_ServiceRequested(false)
   , mHeartbeatTimer(5.0)
   , mMoverUpdateTimer(1.0)
   , mInitialDistributionInterval(0.0)
   , mEntityPositionThreshold(1.0)
   , mEntityOrientationThreshold(3.0 * UtMath::cRAD_PER_DEG)
{
}
