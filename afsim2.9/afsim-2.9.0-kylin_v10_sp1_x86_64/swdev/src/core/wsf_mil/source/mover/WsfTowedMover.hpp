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

#ifndef WSFTOWEDMOVER_HPP
#define WSFTOWEDMOVER_HPP

#include "wsf_mil_export.h"

#include <deque>

#include "WsfOffsetMover.hpp"

//!   A specialized mover that will cause an object to be towed behind a platform, for example a decoy.
class WSF_MIL_EXPORT WsfTowedMover : public WsfOffsetMover
{
public:
   enum DeploymentState
   {
      cSTOWED      = 0,
      cREELING_OUT = 1,
      cDEPLOYED    = 2,
      cREELING_IN  = 3
   };

   WsfTowedMover(WsfScenario& aScenario);
   WsfTowedMover(const WsfTowedMover& aSrc) = default;
   WsfTowedMover& operator=(const WsfTowedMover&) = delete;
   ~WsfTowedMover() override                      = default;

   //! @name Overrides of WsfSinglePlatformObserver.
   //@{
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;
   //@}

   //! @name Common framework methods
   //@{
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   void      Update(double aSimTime) override;

   const char* GetScriptClassName() const override;
   //@}

   //! Stores the state of the lead platform with a timestamp
   struct LeadState
   {
      void LoadFromPlatform(double aSimTime, WsfPlatform& aPlatform);

      // Needed to support std::lower_bound algorithm
      bool operator<(double aTime) { return (mTimeUpdated < aTime); }

      double mLocationWCS[3];
      double mVelocityWCS[3];
      double mAccelerationWCS[3];
      double mOrientationNED[3];
      double mTimeUpdated;
   };

   double GetDistanceMeters() { return mCurrentRange; }

   DeploymentState GetState() { return mState; }

   //! Set time to reel in/out the towed asset.
   //! script method:
   //! param aAbsoluteSimTimeToReelIn   Units: seconds
   //! void SetReelInRate(double aAbsoluteSimTimeToReelIn)
   //!    OR
   //! param aRelativeTimeFromNOWToReelIn   Units: seconds
   //! void SetReelInTimeRelative(double aRelativeTimeFromNOWToReelIn)
   //!
   //! which calls:
   //! param aTime Absolute simulation time to reel in the towed asset.
   //! @param aStartReelInTime Absolute time in seconds in the future at which the towed platform starts to be reeled in.
   void SetReelInTime(double aStartReelInTime) { mStartReelInTime = aStartReelInTime; }

   //! Set time relative to the time the towed platform started to be reeled out.  Time will be used to initiate when to
   //! reel in the asset.
   //! @param aStartReelInTime Relative time to time the asset was reeled out.
   void SetReelInTimeAfterDeployment(double aStartReelInTime) { mRelativeReelInTimeAfterDeployed = aStartReelInTime; }

   //! Retrieve the time a towed asset is to be reeled in, absolute simulation time.
   double GetReelInTimeAbsolute() { return mStartReelInTime; }

   //! Retrieve the time after the towed asset is to be reeled in.  Time is relative to time at initial deployment state.
   double GetReelInTimeAfterDeployment() { return mRelativeReelInTimeAfterDeployed; }

   //! Set rate at which to reel in/out the towed asset.
   //! script method:
   //! param aReelInRate rate at which to reel in the towed asset
   //! void SetReelInRate(double aRate)
   //!
   //! which calls the internal method:
   //! @param aRate Rate at which to reel in the towed asset.
   void SetReelInRate(double aRate) { mReelInRate = aRate; }

   //! Set time to override reel out time if set initially
   //! @param aReelOutRate Rate at which to reel out the towed asset.
   void SetReelOutRate(double aReelOutRate) { mReelOutRate = aReelOutRate; }

   //! Retrieve the rate at which to reel out the towed asset.
   double GetReelOutRate() { return mReelOutRate; }

   //! Retrieve the rate at which to reel in the towed asset.
   double GetReelInRate() { return mReelInRate; }

   //! Set distance the towed asset will remain behind the parent platform once fully deployed.
   //! @param aLength Distance in meters behind the parent platform.
   void SetTowLength(double aLength) { mTowLength = aLength; }

protected:
   void OnPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr) override;

   //! Returns the lead platform's state at a time in the past.
   void GetLeadStateHistory(double aTime, LeadState& aState);

   // delete asset once it has been stowed after being in some type of deployed state (reeling out/in or deployed)
   void CleanUpStowedAsset(double aSimTime);

   void ComputeTowDistance(double aSimTime);

   //! Compute the towed asset velocity
   void UpdateVelocity();

private:
   //! Units: Meters/Second
   //! Definition: Rate at which the towed object is reeled out from the parent (lead) platform.
   double mReelOutRate;

   //! Units: Meters/Second
   //! Definition: Rate at which the towed object is reeled in from the parent (lead) platform.
   double mReelInRate;

   //! Units: Meters
   //! Definition: Maximum distance of towed asset to lead platform.
   double mTowLength;

   //! Units: Seconds
   //! Definition: Time towed asset retrieval began.
   double mStartReelInTime;

   //! Units: Seconds
   //! Definition: Time towed asset is to be reeled in after its been deployed.
   double mRelativeReelInTimeAfterDeployed;

   //! Units: Seconds
   //! Definition: Distance at which towed asset retrieval began.
   double mStartReelInDistance;

   //! Units: Meters
   //! Definition: Current distance (slant range) between towed asset and its parent or lead platform.
   double mCurrentRange;

   //! Units: NA
   //! Definition: Queue holding the lead platform's state history.
   std::deque<LeadState> mLeadPlatformStateQueue;

   //! Units: NA
   //! Definition: State of towed asset; REELING_OUT, DEPLOYED or REELING_IN
   DeploymentState mState;

   //! Units: NA
   //! Definition: If set then once the asset has been deployed and reeled back in and stowed
   //!             reset the quantity to the load-out.
   bool mRestoreQuantity;

   //! Units: NA
   //! Definition: If set, the towed asset will follow the lead platform's trajectory. The time offset of
   //!             the towed asset is computed by taking the current tow length divided by the lead platform's speed
   bool mFollowLeadTrajectory;

   //! Units: Radians
   //! Definition: Towed asset azimuth with respect to the lead platform. Value must be
   //!             in range [-90, 90] with 0 corresponding to the -X axis in the lead
   //!             platform's entity coordinate system, and positive values counter-clockwise.
   double mAzimuthRelativeToLead;

   //! Units: Radians
   //! Definition: Towed asset elevation with respect to the lead platform. Positive elevation actually corresponds
   //!             to -Z direction of the lead platform's ECS.
   double mElevationRelativeToLead;
};

#endif
