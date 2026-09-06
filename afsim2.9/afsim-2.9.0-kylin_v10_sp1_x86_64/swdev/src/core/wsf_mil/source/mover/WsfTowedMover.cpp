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

#include "WsfTowedMover.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <iostream>
#include <vector>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtVec3dX.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"

WsfTowedMover::WsfTowedMover(WsfScenario& aScenario)
   : WsfOffsetMover(aScenario)
   , mReelOutRate(5.0)
   , mReelInRate(0.0)
   , mTowLength(50.0)
   , mStartReelInTime(0.0)
   , mRelativeReelInTimeAfterDeployed(0.0)
   , mStartReelInDistance(0.0)
   , mCurrentRange(0.0)
   , mLeadPlatformStateQueue()
   , mState(cSTOWED)
   , mRestoreQuantity(false)
   , mFollowLeadTrajectory(true)
   , mAzimuthRelativeToLead(0.0)
   , mElevationRelativeToLead(0.0)
{
   // Default update interval for this mover 4hz.
   // Seems to keep the towed asset stable at this speed
   SetUpdateInterval(0.25);

   mAttachmentType = WsfOffsetMover::cAT_RIGID; // tether is currently in development
}

void WsfTowedMover::OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (mFollowLeadTrajectory)
   {
      // Push lead platform's current state onto back of queue
      LeadState state;
      state.LoadFromPlatform(aSimTime, *mReferencePtr);
      mLeadPlatformStateQueue.push_back(state);
   }

   WsfOffsetMover::OnPlatformUpdated(aSimTime, aPlatformPtr);
}

// virtual
WsfMover* WsfTowedMover::Clone() const
{
   return new WsfTowedMover(*this);
}

// ============================================================================
bool WsfTowedMover::Initialize(double aSimTime)
{
   bool                 ok            = true;
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
   if (engagementPtr != nullptr)
   {
      mReferenceNameId = engagementPtr->GetFiringPlatformName();
      mReelOutRate     = std::min(mReelOutRate, engagementPtr->GetFiringPlatform()->GetSpeed());
      mState           = cREELING_OUT;
   }
   else
   {
      ut::log::error() << "WsfTowedMover could not find weapon engagement on initialization.";
      ok = false;
   }

   ok &= WsfOffsetMover::Initialize(aSimTime);

   return ok;
}

// ============================================================================
// virtual
bool WsfTowedMover::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "tow_length")
   {
      aInput.ReadValueOfType(mTowLength, UtInput::cLENGTH);
   }
   else if (command == "reel_in_speed")
   {
      aInput.ReadValueOfType(mReelInRate, UtInput::cSPEED);
      aInput.ValueGreater(mReelInRate, 0.0);
   }
   else if (command == "reel_out_speed")
   {
      aInput.ReadValueOfType(mReelOutRate, UtInput::cSPEED);
      aInput.ValueGreater(mReelOutRate, 0.0);
   }
   else if (command == "reel_in_at_absolute_time")
   {
      aInput.ReadValueOfType(mStartReelInTime, UtInput::cTIME);
   }
   else if (command == "reel_in_time_after_deployed")
   {
      aInput.ReadValueOfType(mRelativeReelInTimeAfterDeployed, UtInput::cTIME);
   }
   else if (command == "restore_quantity")
   {
      aInput.ReadValue(mRestoreQuantity);
   }
   else if (command == "follow_lead_trajectory")
   {
      aInput.ReadValue(mFollowLeadTrajectory);
   }
   else if (command == "azimuth_relative_to_lead")
   {
      aInput.ReadValueOfType(mAzimuthRelativeToLead, UtInput::cANGLE);
      aInput.ValueInClosedRange(mAzimuthRelativeToLead, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "elevation_relative_to_lead")
   {
      aInput.ReadValueOfType(mElevationRelativeToLead, UtInput::cANGLE);
      aInput.ValueInClosedRange(mElevationRelativeToLead, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// ============================================================================
// virtual
void WsfTowedMover::Update(double aSimTime)
{
   if (aSimTime - mLastUpdateTime < 0.003)
   {
      return;
   }

   ComputeTowDistance(aSimTime);

   if (mFollowLeadTrajectory)
   {
      if (mReferencePtr->GetLastUpdateTime() < aSimTime)
      {
         mReferencePtr->Update(aSimTime);
      }

      // Time offset of the towed asset is computed by taking the tether length divided by the lead platform's speed
      double    time = aSimTime - mCurrentRange / mReferencePtr->GetSpeed();
      LeadState state;
      GetLeadStateHistory(time, state);

      // Update towed asset's position, offsetting down from lead flight path
      double offsetNED[3] = {0.0, 0.0, fabs(mCurrentRange * sin(mElevationRelativeToLead))};
      double offsetWCS[3];
      mReferencePtr->ConvertNEDVectorToWCS(offsetWCS, offsetNED);
      UtVec3d::Add(state.mLocationWCS, state.mLocationWCS, offsetWCS);

      GetPlatform()->SetLocationWCS(state.mLocationWCS);
      GetPlatform()->SetVelocityWCS(state.mVelocityWCS);
      GetPlatform()->SetAccelerationWCS(state.mAccelerationWCS);
      GetPlatform()->SetOrientationNED(state.mOrientationNED[0], state.mOrientationNED[1], state.mOrientationNED[2]);
   }
   else
   {
      // Compute towed asset position relative to lead platform based on tether length
      // and desired aspect angles.
      UtVec3d::Set(mOffsetFromReferenceECS,
                   -mCurrentRange * cos(mElevationRelativeToLead) * cos(mAzimuthRelativeToLead),
                   mCurrentRange * cos(mElevationRelativeToLead) * sin(mAzimuthRelativeToLead),
                   -mCurrentRange * sin(mElevationRelativeToLead));
      mOffsetMagnitude = UtVec3d::Magnitude(mOffsetFromReferenceECS);
      WsfOffsetMover::Update(aSimTime);
   }

   UpdateVelocity();

   // Check if time to reel in
   // if reel in time has been set, and simulation time has passed the time
   // to reel in, change the state and set the distance to the current distance
   // between the lead platform and the towed asset.
   if ((mStartReelInTime > 0.0) && ((mState == cREELING_OUT) || (mState == cDEPLOYED)))
   {
      if (aSimTime >= mStartReelInTime)
      {
         mState               = cREELING_IN;
         mStartReelInDistance = mCurrentRange;
      }
   }

   mLastUpdateTime = aSimTime;
}

void WsfTowedMover::OnPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Delete the towed platform if the leader is deleted
   mReferencePtr = nullptr;
   GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
}

//! This method modifies the towed asset velocity in the case it is not fully deployed (reeling in or out).
//! If the towed asset is fixed in lead platform's body coordinates (mFollowLeadTrajectory = false), then the velocity
//! is modified by the deployment rate. If the towed asset is specified to follow the lead trajectory
//! (mFollowLeadTrajectory = true), then this velocity is modified according to the following logic: If the lead
//! platform position is L(t), then towed asset position is T(t) = L(t-u), where u is the time offset. If towed asset is
//! reeling in/out, then u is not constant but a function of time.
//! Towed asset velocity is time derivative of its position, given by T'(t) = d(T(t))/dt.
//! Then T'(t) = d(L(t-u))/dt = d(L(t-u))/d(t-u) * d(t-u)/dt = L'(t-u) * (1 - du/dt)
//! where L' is the lead velocity function, and du/dt is the derivative of the time offset of the towed asset, given by
//! d(u(t))/dt = d(tether length/lead platform speed)/dt = deployment rate/lead platform speed
void WsfTowedMover::UpdateVelocity()
{
   if (mState != cDEPLOYED)
   {
      double rate = mState == cREELING_OUT ? mReelOutRate : -mReelInRate;
      double velWCS[3];
      GetPlatform()->GetVelocityWCS(velWCS);

      if (mFollowLeadTrajectory)
      {
         UtVec3d::Multiply(velWCS, velWCS, 1.0 - rate / mReferencePtr->GetSpeed());
      }
      else
      {
         // Compute deployment velocity components in body coordinates
         double deployVelECS[3] = {rate * -cos(mElevationRelativeToLead) * cos(mAzimuthRelativeToLead),
                                   rate * cos(mElevationRelativeToLead) * sin(mAzimuthRelativeToLead),
                                   rate * -sin(mElevationRelativeToLead)};
         double deployVelWCS[3];
         // Transform velocity from body coordinates to WCS
         mReferencePtr->ConvertECSVectorToWCS(deployVelWCS, deployVelECS);
         // Add deployment velocity to lead platform's velocity
         UtVec3d::Add(velWCS, velWCS, deployVelWCS);
      }

      GetPlatform()->SetVelocityWCS(velWCS);
   }
}

void WsfTowedMover::LeadState::LoadFromPlatform(double aSimTime, WsfPlatform& aPlatform)
{
   aPlatform.GetLocationWCS(mLocationWCS);
   aPlatform.GetVelocityWCS(mVelocityWCS);
   aPlatform.GetAccelerationWCS(mAccelerationWCS);
   aPlatform.GetOrientationNED(mOrientationNED[0], mOrientationNED[1], mOrientationNED[2]);
   mTimeUpdated = aSimTime;
}

void WsfTowedMover::GetLeadStateHistory(double aTime, WsfTowedMover::LeadState& aState)
{
   auto it = std::lower_bound(mLeadPlatformStateQueue.begin(), mLeadPlatformStateQueue.end(), aTime);
   // Time is before first element in queue, no interpolation can be done. Just return first element.
   if (it == mLeadPlatformStateQueue.begin())
   {
      aState = *it;
   }
   else
   {
      auto prevIt = it - 1;
      // interpolate position, velocity, acceleration, orientation
      UtVec3dX::Interpolate(aTime,
                            prevIt->mTimeUpdated,
                            it->mTimeUpdated,
                            UtVec3dX(prevIt->mLocationWCS),
                            UtVec3dX(it->mLocationWCS))
         .Get(aState.mLocationWCS);
      UtVec3dX::Interpolate(aTime,
                            prevIt->mTimeUpdated,
                            it->mTimeUpdated,
                            UtVec3dX(prevIt->mVelocityWCS),
                            UtVec3dX(it->mVelocityWCS))
         .Get(aState.mVelocityWCS);
      UtVec3dX::Interpolate(aTime,
                            prevIt->mTimeUpdated,
                            it->mTimeUpdated,
                            UtVec3dX(prevIt->mAccelerationWCS),
                            UtVec3dX(it->mAccelerationWCS))
         .Get(aState.mAccelerationWCS);
      UtVec3dX::Interpolate(aTime,
                            prevIt->mTimeUpdated,
                            it->mTimeUpdated,
                            UtVec3dX(prevIt->mOrientationNED),
                            UtVec3dX(it->mOrientationNED))
         .Get(aState.mOrientationNED);

      // Erase elements from state queue whose time stamps are earlier than aTime
      mLeadPlatformStateQueue.erase(mLeadPlatformStateQueue.begin(), prevIt);
   }
}

//! Purpose of this routine is to compute the distance between the lead platform and towed asset.
//! @param  aSimTime The current simulation time
void WsfTowedMover::ComputeTowDistance(double aSimTime)
{
   switch (mState)
   {
   case cREELING_OUT:
   {
      mCurrentRange = std::min((aSimTime - mReferenceAttachmentTime) * mReelOutRate, mTowLength);
      if (mCurrentRange == mTowLength)
      {
         mState = cDEPLOYED;
         if (mRelativeReelInTimeAfterDeployed > 0.0)
         {
            mStartReelInTime = aSimTime + mRelativeReelInTimeAfterDeployed;
         }
      }
   }
   break;

   case cREELING_IN:
   {
      mCurrentRange = std::max(0.0, mStartReelInDistance - (aSimTime - mStartReelInTime) * mReelInRate);
      if (mCurrentRange == 0.0)
      {
         mState = cSTOWED;
         // If asset was deployed in some manner and is now stowed,
         // delete the towed asset.
         CleanUpStowedAsset(aSimTime);
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Towed asset Platform retrieval complete.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
   }
   break;
   default: // state should never be cSTOWED because towed asset is removed when stowed
            // if state is cDEPLOYED, current range should be maintained at tow length, so no need to re-compute
      break;
   }
}

//! Once reeled back onto the lead platform, remove the towed asset from the simulation.
//! Treated as an expendable at this time.
void WsfTowedMover::CleanUpStowedAsset(double aSimTime)
{
   if (mRestoreQuantity)
   {
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
      if (nullptr != engagementPtr)
      {
         WsfWeapon* weaponPtr = const_cast<WsfWeapon*>(engagementPtr->GetWeaponSystem());
         weaponPtr->SetQuantityRemaining(weaponPtr->GetInitialStateQuantityRemaining());
      }
   }
   // delete the towed asset that has been stowed.
   GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
}

const char* WsfTowedMover::GetScriptClassName() const
{
   return "WsfTowedMover";
}
