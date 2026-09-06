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

#ifndef WSFFOLLOWER_HPP
#define WSFFOLLOWER_HPP

#include "wsf_export.h"

#include "WsfMover.hpp"

//! A specialized mover that will cause an aircraft to attempt spatial
//! synchronization with a time-tagged velocity vector located in 3D space,
//! subject to certain longitudinal and lateral speed and acceleration
//! constraints. The vector location will be extrapolated forward in time as
//! required. If for any reason the vector cannot be followed, then the
//! platform's current heading and velocity will be maintained.
//! This class is abstract, and derivations to this class will set the
//! point and velocity to follow (for formation flight with another aircraft,
//! or perhaps for following a specified track).
//!
//! The guidance followed is pure velocity pursuit.  If needed, proportional
//! navigation may be added later.

class WSF_EXPORT WsfFollower : public WsfMover
{
public:
   WsfFollower() = delete;
   WsfFollower(WsfScenario& aScenario);
   ~WsfFollower() override = default;

   //! @name Common framework methods
   //@{
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   void      Update(double aSimTime) override;
   //@}

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   bool IsInitialized() const { return mHasBeenInitialized; }

   //! @name Follower data methods
   //! These methods are used to set follower information.
   //@{
   void SetLocationLLA(double aLatitude, double aLongitude, double aAltitude);
   void SetSpeed(double aSpeed) { mInitSpeed = aSpeed; }
   void SetHeading(double aHeading) override { mInitHeading = aHeading; }
   //@}

   //! Returns true if the target information is valid
   bool GetTargetIsValid() { return mTargetIsValid; };

protected:
   // Declare default copy constructor (for Clone())
   WsfFollower(const WsfFollower& aSrc);
   WsfFollower& operator=(const WsfFollower&) = delete;

   // Sets the information on the follower
   void FollowThis(double aTimeStamp, double aLocationWCS[3], double aVelocityWCS[3]);
   void DontFollowThis() { mTargetIsValid = false; }

   // Called during update
   void AdvanceTime(double aSimTime);

   //! Extrapolates the follow information to the given time
   void UpdateFollowPosAndVel(double aSimTime);

   // Imposes turn limits on the follower
   void ImposeTurnLimits(double  aHeadingChangeMag,
                         double  aHeadingChangeDir,
                         double  aHorizGroundSpeed,
                         double& aDeltaT,
                         double& aHeading,
                         double& aRollAngle);

   // Used in the turn limit calculation
   inline double MinimumPositiveValue(double aValue1, double aValue2, double aValue3, double aValue4) const;

   //! @brief Used to output data when debugging.
   void OutputDiagnostic(double aSimTime, WsfPlatform* aPlatformPtr);
   void OutputVec3d(std::string& aLabel, double aVec[]);

private:
   //! Set to true after Initialize() function is called. Defaults to false.
   bool mHasBeenInitialized;

   //! Initial platform information for this follower
   double mInitLatitudeDeg;
   double mInitLongitudeDeg;
   double mInitAltitude;
   double mInitSpeed;
   double mInitHeading;

   //! Set to true when the FollowThis() function is called. Defaults to false.
   bool mTargetIsValid;

   //! State of platform to follow: time, position, & velocity
   double mTargetTimeStamp;
   double mTargetLocWCS[3];
   double mTargetVelWCS[3];

   //! Last location of platform to follow
   double mLastTargetTimeStamp;
   double mLastTargetLocWCS[3];

   //! Extrapolated data of platform to follow
   double mTargetExtrapTimeStamp;
   double mTargetExtrapLocWCS[3];
   double mTargetExtrapUnitLOSNED[3];
   double mTargetExtrapAlt;

   double mTargetForward;
   double mTargetFwdRate;
   double mTargetSpeed;

   //!
   double mMaxAxialAccel; // m/s^2
   double mMaxClimbRate;  // m/s
   double mMaxSpeed;      // m/s
   double mMinSpeed;      // m/s

   //! Turn Attributes:
   double mMaxRadialAccel;   // m/s^2
   double mMaxTurnRate;      // radians/s
   double mMaxBankAngle;     // radians
   double mMaxVertBodyAccel; // m/s^2
   double mMaxRollRate;      // radians/s

   double mVelPursuitGain;
};

#endif
