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

#include "WsfFollower.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DIAG_ON
#define DO_DEBUG(A) A
#else
#define DO_DEBUG(A) /* A */
#endif

//#define DIAG_ON
#ifdef DIAG_ON
#define DO_DIAG(A) A
#else
#define DO_DIAG(A) /* A */
#endif

WsfFollower::WsfFollower(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mHasBeenInitialized(false)
   , mInitLatitudeDeg(0.0)
   , mInitLongitudeDeg(0.0)
   , mInitAltitude(0.0)
   , mInitSpeed(10.0)
   , mInitHeading(0.0)
   , mTargetIsValid(false)
   , mTargetTimeStamp(0.0)
   , mLastTargetTimeStamp(-1.0)
   , mTargetExtrapTimeStamp(0.0)
   , mTargetExtrapAlt(0.0)
   , mTargetForward(0.0)
   , mTargetFwdRate(0.0)
   , mTargetSpeed(0.0)
   , mMaxAxialAccel(0.0)
   , mMaxClimbRate(5.0)
   , mMaxSpeed(0.0)
   , mMinSpeed(0.0)
   , mMaxRadialAccel(0.0)
   , mMaxTurnRate(0.0)
   , mMaxBankAngle(0.0)
   , mMaxVertBodyAccel(0.0)
   , mMaxRollRate(0.0)
   , mVelPursuitGain(10.0)
{
   UtVec3d::Set(mTargetLocWCS, 0.0);
   UtVec3d::Set(mTargetVelWCS, 0.0);
   UtVec3d::Set(mLastTargetLocWCS, 0.0);
   UtVec3d::Set(mTargetExtrapLocWCS, 0.0);

   double one[] = {1.0, 0.0, 0.0};
   UtVec3d::Set(mTargetExtrapUnitLOSNED, one);
}

WsfFollower::WsfFollower(const WsfFollower& aSrc)
   : WsfMover(aSrc)
   , mHasBeenInitialized(aSrc.mHasBeenInitialized)
   , mInitLatitudeDeg(aSrc.mInitLatitudeDeg)
   , mInitLongitudeDeg(aSrc.mInitLongitudeDeg)
   , mInitAltitude(aSrc.mInitAltitude)
   , mInitSpeed(aSrc.mInitSpeed)
   , mInitHeading(aSrc.mInitHeading)
   , mTargetIsValid(aSrc.mTargetIsValid)
   , mTargetTimeStamp(aSrc.mTargetTimeStamp)
   , mLastTargetTimeStamp(-1.0)
   , mTargetExtrapTimeStamp(aSrc.mTargetExtrapTimeStamp)
   , mTargetExtrapAlt(aSrc.mTargetExtrapAlt)
   , mTargetForward(0.0)
   , mTargetFwdRate(0.0)
   , mTargetSpeed(0.0)
   , mMaxAxialAccel(aSrc.mMaxAxialAccel)
   , mMaxClimbRate(aSrc.mMaxClimbRate)
   , mMaxSpeed(aSrc.mMaxSpeed)
   , mMinSpeed(aSrc.mMinSpeed)
   , mMaxRadialAccel(aSrc.mMaxRadialAccel)
   , mMaxTurnRate(aSrc.mMaxTurnRate)
   , mMaxBankAngle(aSrc.mMaxBankAngle)
   , mMaxVertBodyAccel(aSrc.mMaxVertBodyAccel)
   , mMaxRollRate(aSrc.mMaxRollRate)
   , mVelPursuitGain(aSrc.mVelPursuitGain)
{
   // We only allow the cloning of non-initialized movers.
   assert(!aSrc.mHasBeenInitialized);

   UtVec3d::Set(mTargetLocWCS, aSrc.mTargetLocWCS);
   UtVec3d::Set(mTargetVelWCS, aSrc.mTargetVelWCS);

   UtVec3d::Set(mLastTargetLocWCS, 0.0);
   UtVec3d::Set(mTargetExtrapLocWCS, 0.0);
   UtVec3d::Set(mTargetExtrapUnitLOSNED, 0.0);
}

// virtual
WsfMover* WsfFollower::Clone() const
{
   return new WsfFollower(*this);
}

// ============================================================================
bool WsfFollower::Initialize(double aSimTime)
{
   assert(GetPlatform() != nullptr);

   // Initialize the base clase first
   if (!WsfMover::Initialize(aSimTime))
   {
      return false;
   }

   mHasBeenInitialized = true;

   // Make sure that we have some kind of valid
   // lateral limit to work with in turns:
   if ((mMaxRadialAccel == 0.0F) && (mMaxTurnRate == 0.0F) && (mMaxBankAngle == 0.0F) && (mMaxVertBodyAccel == 0.0F))
   {
      mMaxRadialAccel = 2.0 * UtEarth::cACCEL_OF_GRAVITY;
   }

   // Set this platform's location and orientation
   GetPlatform()->SetLocationLLA(mInitLatitudeDeg, mInitLongitudeDeg, mInitAltitude);
   GetPlatform()->SetOrientationNED(mInitHeading, 0.0, 0.0);

   // Set this platform's velocity
   double velNED[3] = {0.0};
   velNED[0]        = ::cos(mInitHeading) * mInitSpeed;
   velNED[1]        = ::sin(mInitHeading) * mInitSpeed;
   velNED[2]        = 0.0;
   GetPlatform()->SetVelocityNED(velNED);

   return true;
}

// ============================================================================
// virtual
bool WsfFollower::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "maximum_axial_acceleration")
   {
      aInput.ReadValueOfType(mMaxAxialAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaxAxialAccel, 0.0);
   }
   else if ((command == "maximum_climb_rate") || (command == "maximum_dive_rate"))
   {
      aInput.ReadValueOfType(mMaxClimbRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxClimbRate, 0.0);
   }
   else if (command == "maximum_radial_acceleration")
   {
      aInput.ReadValueOfType(mMaxRadialAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxRadialAccel, 0.0);
   }
   else if (command == "maximum_turn_rate")
   {
      aInput.ReadValueOfType(mMaxTurnRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxTurnRate, 0.0);
   }
   else if (command == "maximum_bank_angle")
   {
      aInput.ReadValueOfType(mMaxBankAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMaxBankAngle, 5.0 * UtMath::cRAD_PER_DEG, 85.0 * UtMath::cRAD_PER_DEG);
   }
   else if (command == "body_g_limit") // Input string is consistent with WsfWayPointMover
   {
      aInput.ReadValueOfType(mMaxVertBodyAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxVertBodyAccel, UtEarth::cACCEL_OF_GRAVITY);
   }
   else if (command == "maximum_roll_rate")
   {
      aInput.ReadValueOfType(mMaxRollRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxRollRate, 0.0);
   }
   else if (command == "maximum_speed")
   {
      aInput.ReadValueOfType(mMaxSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMaxSpeed, 0.0);
      if ((mMinSpeed != 0.0) && (mMaxSpeed < mMinSpeed))
      {
         throw UtInput::BadValue(aInput, "Must specify a maximum_speed equal to or greater than minimum_speed.");
      }
   }
   else if (command == "minimum_speed")
   {
      aInput.ReadValueOfType(mMinSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMinSpeed, 0.0);
      if ((mMaxSpeed != 0.0) && (mMinSpeed > mMaxSpeed))
      {
         throw UtInput::BadValue(aInput, "Must specify a minimum_speed less than or equal to maximum_speed.");
      }
   }
   else if (command == "velocity_pursuit_gain")
   {
      aInput.ReadValue(mVelPursuitGain);
      aInput.ValueGreater(mVelPursuitGain, 0.0);
   }
   else if (command == "position")
   {
      aInput.ReadValueOfType(mInitLatitudeDeg, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mInitLongitudeDeg, UtInput::cLONGITUDE);
   }
   else if (command == "altitude")
   {
      aInput.ReadValueOfType(mInitAltitude, UtInput::cLENGTH);
   }
   else if (command == "speed")
   {
      aInput.ReadValueOfType(mInitSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mInitSpeed, 0.0);
   }
   else if (command == "heading")
   {
      aInput.ReadValueOfType(mInitHeading, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mInitHeading, -UtMath::cTWO_PI);
      aInput.ValueLessOrEqual(mInitHeading, UtMath::cTWO_PI);
      while (mInitHeading < 0.0)
      {
         mInitHeading += UtMath::cTWO_PI;
      }
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// ============================================================================
// virtual
void WsfFollower::Update(double aSimTime)
{
   assert(GetPlatform() != nullptr);
   assert(mHasBeenInitialized);

   DO_DIAG({
      auto out = ut::log::debug() << "Platform Update: ";
      out.AddNote() "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "T=" << aSimTime out.AddNote() << "Last Update T = " << mLastUpdateTime;
   });

   double dT(aSimTime - mLastUpdateTime);
   if (dT > mUpdateTimeTolerance)
   {
      if (mTargetIsValid)
      {
         // Move along the route as far as possible
         AdvanceTime(aSimTime);

         if (mLastUpdateTime < aSimTime)
         {
            // Recursion.  Continue updating until we've met aSimTime
            Update(aSimTime);
         }
      }
      else
      {
         // No valid lead info to follow, just keep going at current velocity
         double increment[3] = {0.0};
         GetPlatform()->GetVelocityWCS(increment);
         UtVec3d::Multiply(increment, increment, dT);
         GetPlatform()->IncrementLocationWCS(increment);
         WsfMover::Update(aSimTime);
      }
   }
}

// ============================================================================
void WsfFollower::SetLocationLLA(double aLatitude, double aLongitude, double aAltitude)
{
   mInitLatitudeDeg  = aLatitude;
   mInitLongitudeDeg = aLongitude;
   mInitAltitude     = aAltitude;
}

// ============================================================================
void WsfFollower::FollowThis(double aTimeStamp, double aLocationWCS[3], double aVelocityWCS[3])
{
   // Save the previous time and location
   mLastTargetTimeStamp = mTargetTimeStamp;
   UtVec3d::Set(mLastTargetLocWCS, mTargetLocWCS);

   // Save the new target information
   mTargetIsValid   = true;
   mTargetTimeStamp = aTimeStamp;

   // Set the follow location and velocity
   UtVec3d::Set(mTargetLocWCS, aLocationWCS);
   UtVec3d::Set(mTargetVelWCS, aVelocityWCS);
   mTargetSpeed = UtVec3d::Magnitude(aVelocityWCS);

   // Initialize the extrapolated data
   mTargetExtrapTimeStamp = aTimeStamp;
   UtVec3d::Set(mTargetExtrapLocWCS, mTargetLocWCS);

   double latNotUsed, lonNotUsed;
   UtEntity::ConvertWCSToLLA(mTargetExtrapLocWCS, latNotUsed, lonNotUsed, mTargetExtrapAlt);

   DO_DIAG({
      auto out = ut::log::debug() << "FollowThis:";
      out.AddNote() << "T = " << mTargetTimeStamp;
      out.AddNote() << "Location LLA [deg,m]: " << latNotUsed << ", " << lonNotUsed << ", " << mTargetExtrapAlt;
      out.AddNote() << "Location WCS [m]: " << mTargetLocWCS[0] << ", " << mTargetLocWCS[1] << ", " << mTargetLocWCS[2];
      out.AddNote() << "Velocity WCS [m/s]: " << mTargetVelWCS[0] << ", " << mTargetVelWCS[1] << ", " << mTargetVelWCS[2];
      auto note = out.AddNote() << "Speed [m/s]: " << mTargetSpeed;
      if (mTargetSpeed < mMinSpeed)
      {
         note << " ** Too slow **";
      }
      if (mTargetSpeed > mMaxSpeed)
      {
         note << " ** Too fast **";
      }
   });
}

// ============================================================================
// protected virtual
void WsfFollower::AdvanceTime(double aSimTime)
{
   // Compute elapsed time since last update...
   double deltaTime = aSimTime - mLastUpdateTime;

   // Extrapolate target information
   UpdateFollowPosAndVel(aSimTime);

   // Get the current lat/lon/alt
   double latNotUsed;
   double lonNotUsed;
   double currentAlt;
   GetPlatform()->GetLocationLLA(latNotUsed, lonNotUsed, currentAlt);

   // Get this platform's current velocity vector and ground speed.
   double currentVelocityNED[3] = {0.0};
   GetPlatform()->GetVelocityNED(currentVelocityNED);
   double currentGroundSpeed =
      ::sqrt(currentVelocityNED[0] * currentVelocityNED[0] + currentVelocityNED[1] * currentVelocityNED[1]);
   double currentSpeed = UtVec3d::Normalize(currentVelocityNED);

   // Get this platform's orientation
   double heading(0.0);
   double pitch(0.0);
   double roll(0.0);
   GetPlatform()->GetOrientationNED(heading, pitch, roll);

   // Find the sin and cosine of the heading
   // This is used often in the calculation below
   double sinHeading = sin(heading);
   double cosHeading = cos(heading);

   if (currentGroundSpeed <= std::numeric_limits<double>::epsilon())
   {
      // provide a valid heading, even if velocity is zero
      UtVec3d::Set(currentVelocityNED, cosHeading, sinHeading, 0.0);
   }
   else
   {
      heading = atan2(currentVelocityNED[1], currentVelocityNED[0]);
   }

   // Update the LOS unit vector
   GetPlatform()->ConvertWCSToNED(mTargetExtrapLocWCS, mTargetExtrapUnitLOSNED);
   UtVec3d::Normalize(mTargetExtrapUnitLOSNED); // Ignore the magnitude returned

   // Don't allow too much time to elapse between updates.
   if (currentSpeed > 0.0)
   {
      deltaTime = std::min(deltaTime, std::max(0.001, 4000.0 / currentSpeed));
   }

   //
   // Altitude adjustment
   //

   // Determine if an altitude change is required
   // vDown will be the max climb rate unless close to target altitude or
   // max climb rate is zero. NED frame
   double vz(0.0);
   double deltaAlt(::fabs(mTargetExtrapAlt - currentAlt));

   if (deltaAlt > 1.0)
   {
      // We're in altitude change mode and a non-trivial change is required.
      double climbRate(mMaxClimbRate);

      // If supplied climb rate is too small; calculate a new value
      if (::fabs(climbRate) < std::numeric_limits<double>::epsilon())
      {
         double timeToTarget = mTargetForward / (0.5 * (currentSpeed + mTargetSpeed));
         climbRate           = deltaAlt / timeToTarget;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform calculating new Climb Rate.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Climb Rate: " << climbRate;
         }
      }

      // Within a time step of the target altitude
      if (deltaAlt < deltaTime * climbRate)
      {
         if (::fabs(climbRate) > std::numeric_limits<double>::epsilon())
         {
            deltaTime = deltaAlt / climbRate;
         }
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform altitude change complete.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Delta Time: " << deltaTime;
         }
      }
      // Remember that positive Z is DOWN!!!!
      vz = climbRate; // Assume decreasing altitude (increasing Z)
      if (currentAlt < mTargetExtrapAlt)
      {
         vz = -climbRate; // Increasing altitude (decreasing Z)
      }
   }
   else
   {
      // We've already hit the target altitude.  If we're off slightly in altitude we
      // just apply small correction.  Note that we don't use the prescribed climb
      // rate because such a violent change causes oscillations.
      // (Remember that +Z is DOWN!)
      vz = (currentAlt - mTargetExtrapAlt) / deltaTime;
   }

   // Return the required velocity in the z direction.
   DO_DIAG({
      auto out = ut::log::debug() << "GetVelocityForAltitudeAdjustment:";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Delta Alt: " << deltaAlt;
      out.AddNote() << "vDown: " << vz;
      out.AddNote() << "Max Climb Rate: " << mMaxClimbRate;
   });

   //
   // Axial acceleration adjustment
   //
   double vxy       = ::sqrt(std::max(currentSpeed * currentSpeed - vz * vz, 0.1));
   double targetVxy = ::sqrt(std::max(mTargetSpeed * mTargetSpeed - vz * vz, 0.1));
   mTargetFwdRate   = targetVxy - vxy;

   double locOffsetNED[3] = {0.0};
   GetPlatform()->ConvertWCSToNED(mTargetExtrapLocWCS, locOffsetNED);

   // Determine if target is close and in the rear
   mTargetForward   = cos(heading) * locOffsetNED[0] + sin(heading) * locOffsetNED[1];
   double horizDist = sqrt(locOffsetNED[0] * locOffsetNED[0] + locOffsetNED[1] * locOffsetNED[1]);

   // Determine if close to target
   double deltaTargetLocWCS[3] = {0.0};
   GetPlatform()->GetRelativeLocationWCS(mLastTargetLocWCS, deltaTargetLocWCS);

   // Based on the target's current location and velocity
   // determine if acceleration in the axial direction is required
   double accNED[3] = {0.0};
   GetPlatform()->GetAccelerationNED(accNED);
   double axy = cos(heading) * accNED[0] + sin(heading) * accNED[1];

   DO_DIAG({
      double distanceToLastTargetLoc = UtVec3d::Magnitude(deltaTargetLocWCS);
      auto   out                     = ut::log::debug() << "GetAxialAcceleration:";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Distance Forward Now [m]: " << mTargetForward;
      out.AddNote() << "Last Range [m]: " distanceToLastTargetLoc;
      out.AddNote() << "Min Speed [m/s]: " << mMinSpeed;
      out.AddNote() << "Current Speed [m/s]: " << currentSpeed;
      out.AddNote() << "Max Speed [m/s]: " << mMaxSpeed;
      out.AddNote() << "Forward Rate [m/s]: " mTargetFwdRate;
      out.AddNote() << "Target vxy [m/s]: " targetVxy;
      out.AddNote() << "vxy [m/s]: " << vxy;
      if (mTargetFwdRate < 0.0)
      {
         out << " | *** closing ***";
      }
      out.AddNote() << "Current Accel [m/s^2]: " << axy;
      out.AddNote() << "Max Axial Accel: " << mMaxAxialAccel;
   });

   // Find the time to intercept based on the current state
   // (0.5 * a(t)) * h^2 + (v(t)) * h - dx = 0
   double timeToIntercept(0.0);
   if (::fabs(axy) > std::numeric_limits<double>::epsilon())
   {
      double a = 0.5 * axy;
      double b = vxy;
      double c = -mTargetForward;

      double bSquared = b * b;
      double fourAC   = 4.0 * a * c;

      // Check for a real solution
      if (bSquared > fourAC)
      {
         double sqrtTerm = ::sqrt(bSquared - fourAC);
         double time1    = (-b + sqrtTerm) / (2.0 * a);
         double time2    = (-b - sqrtTerm) / (2.0 * a);

         // Time to intercept
         if ((time1 >= 0.0) && (time2 >= 0.0))
         {
            timeToIntercept = std::min(time1, time2);
         }
         else if ((time1 >= 0.0) && (time2 < 0.0))
         {
            timeToIntercept = time1;
         }
         else if ((time1 < 0.0) && (time2 >= 0.0))
         {
            timeToIntercept = time2;
         }
      }
   }
   // if the axial acceleration in currently small
   // then use the velocity to calculate the time to intercept
   // (v(t)) * h - dx = 0
   else if (::fabs(vxy) > std::numeric_limits<double>::epsilon())
   {
      timeToIntercept = mTargetForward / vxy;
      if (timeToIntercept < 0.0)
      {
         timeToIntercept = 0.0;
      }
   }

   bool closeToTgt = horizDist < 3000.0;
   bool overShot   = (mTargetForward < 0.0) && closeToTgt;

   DO_DIAG({
      // Intercept conditions
      bool   ttiValid        = ((mTargetFwdRate < -0.1) && (mTargetForward > 0.0));
      double timeToDecel     = ::fabs(mTargetFwdRate) / mMaxAxialAccel;
      bool   willHit         = (ttiValid) && (timeToIntercept < (timeToDecel - 3.0));
      bool   willSoon        = (ttiValid) && (timeToIntercept < 10.0);
      bool   leavingMeBehind = (mTargetForward > 0.0) && (mTargetFwdRate > 0.0);
      bool   receedingFast   = (mTargetForward < 0.0) && (!closeToTgt) && (mTargetFwdRate < 0.0);
      bool   iAmFarBehind    = mTargetForward > 1000.0;
      auto   out             = ut::log::debug() << "Intercept Conditions:" << std::boolalpha;
      out.AddNote() << "Close To Target: " << closeToTgt;
      out.AddNote() << "Over Shot: " << overShot;
      out.AddNote() << "Will Hit: " << willHit;
      out.AddNote() << "Will Soon: " << willSoon;
      out.AddNote() << "Leaving Me Behind: " << leavingMeBehind;
      out.AddNote() << "Receeding Fast: " << receedingFast;
      out.AddNote() << "I Am Far Behind: " << iAmFarBehind;
      out.AddNote() << "Time To Intercept [s]: " << timeToIntercept;
      out.AddNote() << "Time To Decel [s]: " << timeToDecel;
      if (timeToIntercept < timeToDecel)
      {
         out.AddNote() << "** pull up, pull up **";
      }
   });

   // Acceleration fine adjustment
   if (::fabs((timeToIntercept - deltaTime) / deltaTime) <= 0.1)
   {
      DO_DIAG(ut::log::debug() << "Fine adjustment.";);

      // We're only slightly off on velocity.  Insert a small acceleration to correct.
      axy = mTargetFwdRate / deltaTime;

      double deltaVxy = ::fabs(mTargetFwdRate);
      if (deltaVxy > 0.01 && deltaVxy < axy * deltaTime)
      {
         deltaTime = deltaVxy / axy;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform Speed change complete Delta Time.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Delta Time: " << deltaTime;
         }
      }
   }
   else
   {
      // Calculate a new axial acceleration
      // (0.5 * a(t)) * h^2 + (v(t)) * h - dx = 0   Solve for a(t)
      axy = 2.0 * (mTargetForward - deltaTime * vxy) / (deltaTime * deltaTime);
      DO_DIAG(ut::log::debug() << "axy1 [m/s^2]: " << axy;);

      // Scale the acceleration
      axy *= ::fabs((deltaTime - timeToIntercept) / deltaTime);
      DO_DIAG(ut::log::debug() << "axy2 [m/s^2]: " << axy;);
   }

   // Enforce the min and max
   if (axy > mMaxAxialAccel)
   {
      axy = mMaxAxialAccel;
   }
   else if (axy < -mMaxAxialAccel)
   {
      axy = -mMaxAxialAccel;
   }

   // Zero the acceleration, if we will exceed speed limits.
   if ((currentSpeed > mMaxSpeed) && (axy > 0.0))
   {
      axy = 0.0;
   }
   else if ((currentSpeed < mMinSpeed) && (axy < 0.0))
   {
      axy = 0.0;
   }

   DO_DIAG(
      auto out = ut::log::debug() << "New accel [m/s^2]: " << axy;
      if (axy > 0.0) { out.AddNote() << "accelerating"; } if (axy < 0.0) { out.AddNote() << "decelerating"; });

   //
   // Heading change
   //

   // Determine the amount and direction of heading change required.
   // Note that we must zero the vertical component of the two heading vectors
   // in order to prevent the climb/dive from figuring in to the angle change.
   currentVelocityNED[2] = 0.0;
   UtVec3d::Normalize(currentVelocityNED);
   double headingToTarget[3] = {0.0};
   UtVec3d::Set(headingToTarget, mTargetExtrapUnitLOSNED);
   headingToTarget[2] = 0.0;
   UtVec3d::Normalize(headingToTarget);

   double cosHeadingChange = UtVec3d::DotProduct(currentVelocityNED, headingToTarget);
   double headingChangeMag = ::acos(std::min(std::max(cosHeadingChange, -1.0), 1.0));
   if (overShot)
   {
      // In front of and close
      headingChangeMag = 0.0;
   }

   // The Z component of the normal vector will be positive for
   // a right turn and negative for a left turn.
   double normalVec[3] = {0.0};
   UtVec3d::CrossProduct(normalVec, currentVelocityNED, headingToTarget);
   double headingChangeDir = (normalVec[2] < 0.0) ? -1.0 : 1.0;

   ImposeTurnLimits(headingChangeMag,
                    headingChangeDir,
                    currentGroundSpeed, // inputs (pass by value)
                    deltaTime,
                    heading,
                    roll); // inputs/outputs (pass by reference)
   // (routine MAY deduce the deltaTime value!)

   // Could add yaw angle to 'heading' here'...

   pitch = ::atan2(-vz, vxy);

   // Could add angle of attack here...

   sinHeading            = sin(heading);
   cosHeading            = cos(heading);
   double velocityNED[3] = {vxy * cosHeading, vxy * sinHeading, vz};
   UtVec3d::Normalize(velocityNED);
   UtVec3d::Multiply(velocityNED, currentSpeed);

   // Convert the acceleration to the NED frame
   double accelerationNED[3] = {axy * cosHeading, axy * sinHeading, 0.0};

   // Compute distance in this time step...
   // dx = h * v(t) + 1/2 * h^2 * a(t)
   double halfDeltaTime2   = 0.5 * deltaTime * deltaTime;
   double deltaDistance[3] = {0.0};
   deltaDistance[0]        = (velocityNED[0] * deltaTime) + (accelerationNED[0] * halfDeltaTime2);
   deltaDistance[1]        = (velocityNED[1] * deltaTime) + (accelerationNED[1] * halfDeltaTime2);
   deltaDistance[2]        = (velocityNED[2] * deltaTime) + (accelerationNED[2] * halfDeltaTime2);

   // Integrate location.
   GetPlatform()->IncrementLocationNED(deltaDistance);

   // Integrate velocity
   velocityNED[0] += accelerationNED[0] * deltaTime;
   velocityNED[1] += accelerationNED[1] * deltaTime;
   velocityNED[2] += accelerationNED[2] * deltaTime;

   // Update the platform
   GetPlatform()->SetVelocityNED(velocityNED);
   GetPlatform()->SetAccelerationNED(accelerationNED);
   GetPlatform()->SetOrientationNED(heading, pitch, roll);
   mLastUpdateTime += deltaTime;

   DO_DIAG(ut::log::debug() << "Updated Follower: "; OutputDiagnostic(aSimTime, GetPlatform()););

   // If we're decelerating and we're very close to zero then we must advance
   // in order to prevent from stopping movement on the current segment.
   if ((axy <= 0.0) && (UtVec3d::MagnitudeSquared(velocityNED) < 0.01))
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Force advance because of very small velocity.";
         out.AddNote() << "v: " << UtVec3d::Magnitude(velocityNED);
         out.AddNote() << "axy: " << axy;
      }
   }
}

// ============================================================================
void WsfFollower::UpdateFollowPosAndVel(double aSimTime)
{
   // The algorithm already demands that mFollowInfoIsValid be true.
   if (aSimTime > mTargetExtrapTimeStamp)
   {
      // This routine propagates the location to fly forward to:
      double dT = aSimTime - mTargetTimeStamp;

      // Get the change in location in the WCS frame
      double deltaLocWCS[3] = {0.0};
      UtVec3d::Multiply(deltaLocWCS, mTargetVelWCS, dT);

      // Update the extrapolated data
      mTargetExtrapTimeStamp = aSimTime;

      // Find the new location
      UtVec3d::Add(mTargetExtrapLocWCS, mTargetLocWCS, deltaLocWCS);

      double latNotUsed, lonNotUsed;
      UtEntity::ConvertWCSToLLA(mTargetExtrapLocWCS, latNotUsed, lonNotUsed, mTargetExtrapAlt);
   }
}

// ============================================================================
//! Constrain current turn rate to specified limits.
//! Assumes a steady-state level flight, and fully coordinated turn.
void WsfFollower::ImposeTurnLimits(double  aHeadingChangeMag,
                                   double  aHeadingChangeDir,
                                   double  aHorizGroundSpeed,
                                   double& aDeltaT,
                                   double& aHeading,
                                   double& aRollAngle)
{
   // const double cNEGLIGIBLE_HEADING_ERR  =    0.50 * UtMath::cRAD_PER_DEG;
   // const double cNEGLIGIBLE_ROLL_ERR     =    3.00 * UtMath::cRAD_PER_DEG;
   const double cMAX_ABS_CHANGE_IN_HDG = 5.00 * UtMath::cRAD_PER_DEG;
   const double cACCEL_TOLERANCE       = 0.0001;
   // const double cTIME_TOLERANCE          =    0.0001;  // changed to mUpdateTimeTolerance
   const double cMAX_DELTA_ROLL_ANGLE    = 10.00 * UtMath::cRAD_PER_DEG;
   const double cMAX_DISTANCE_ALONG_TURN = 500.0; // meters
   const double cMAX_TIME_WHILE_TURNING  = 5.0;   // seconds
   const double cMIN_TIME_WHILE_TURNING  = 0.1;   // seconds

   if (aHorizGroundSpeed < WsfPath::cNEGLIGIBLE_SPEED)
   {
      // Don't change aDeltaT, and simply level the wings as
      // much as the roll rate will allow.T
      if (mMaxRollRate == 0)
      {
         aRollAngle = 0.0;
      }
      else
      {
         double deltaRollAngle = aDeltaT * mMaxRollRate;
         if (aRollAngle > 0.0)
         {
            if (aRollAngle > deltaRollAngle)
            {
               aRollAngle -= deltaRollAngle;
            }
            else
            {
               aRollAngle = 0.0;
            }
         }
         else if (aRollAngle < 0.0)
         {
            if (aRollAngle < -deltaRollAngle)
            {
               aRollAngle += deltaRollAngle;
            }
            else
            {
               aRollAngle = 0.0;
            }
         }
      }
      return;
   }

   // At least ONE turn constraint must be present!  Possible constraints are:
   // 1.  Max Radial (Horizontal) Acceleration
   // 2.  Max Bank Angle (x < 90.0 degrees)
   // 3.  Max Turn Rate
   // 4.  Max Body Normal Gee limit
   //        (Acting through the body local "down" direction, which
   //         includes one vertical gee to counter the effect of weight.)
   // Change each constraint into the same units.  To find out which is the most
   // critical one, translate each into an equivalent radial acceleration.

   // (Another constraint is that the max roll rate (if specified)
   //  will not permit discontinuous lateral acceleration changes.)

   double bankAngleLimitedAccel = UtEarth::cACCEL_OF_GRAVITY * tan(mMaxBankAngle);
   double turnRateLimitedAccel  = aHorizGroundSpeed * mMaxTurnRate;
   double bodyGeeLimitedAccel =
      (mMaxVertBodyAccel == 0.0F) ? 0.0 : mMaxVertBodyAccel * sin(acos(UtEarth::cACCEL_OF_GRAVITY / mMaxVertBodyAccel));
   double turnLimitedAccel =
      MinimumPositiveValue(bankAngleLimitedAccel, turnRateLimitedAccel, bodyGeeLimitedAccel, mMaxRadialAccel);

   double absDesiredVPAvgAccel = mVelPursuitGain * aHeadingChangeMag;

   // Separate the roll-rate limited case from the non-roll-rate limited,
   // as the logic is much simpler if roll angles can be instantaneously changed.
   if (mMaxRollRate == 0.0)
   {
      // There is no specified roll rate limit, so roll
      // instantly to the bank limit, or back to zero.
      double deltaT = std::min(aDeltaT, cMAX_TIME_WHILE_TURNING);

      double maxPossibleHdgChg = std::min(cMAX_ABS_CHANGE_IN_HDG, turnLimitedAccel * deltaT / aHorizGroundSpeed);

      if (aHeadingChangeMag < maxPossibleHdgChg)
      {
         // We can accomplish this turn in one step, so just do it:
         aRollAngle = 0.0;
         aHeading += aHeadingChangeDir * aHeadingChangeMag;
         aDeltaT = deltaT;
      }
      else
      {
         // We cannot accomplish this turn in one time step:
         double averageAccel   = turnLimitedAccel;
         double turnRate       = averageAccel / aHorizGroundSpeed;
         double turnTimeNeeded = maxPossibleHdgChg / turnRate;
         if (turnTimeNeeded < cMIN_TIME_WHILE_TURNING)
         {
            // Roll rates are not limited, so we can take whatever time we need
            // to roll out of the turn, to get a required average turn rate during dT.
            aDeltaT      = std::min(aDeltaT, cMIN_TIME_WHILE_TURNING);
            turnRate     = aHeadingChangeMag / aDeltaT;
            averageAccel = turnRate * aHorizGroundSpeed;
            aRollAngle   = 0.0;
         }
         else
         {
            aDeltaT    = std::min(aDeltaT, turnTimeNeeded);
            aRollAngle = atan2((averageAccel * aHeadingChangeDir), UtEarth::cACCEL_OF_GRAVITY);
         }

         // Limit the time and distance along a turn to
         // prevent abrupt changes (looks ugly in graphics).
         if (mMaxTurnRate != 0.0F)
         {
            double deltaTimeMax = sqrt(cMAX_DISTANCE_ALONG_TURN / (aHorizGroundSpeed * mMaxTurnRate));

            // Restrict the time step to defined duration.
            if (deltaTimeMax < cMIN_TIME_WHILE_TURNING)
            {
               deltaTimeMax = cMIN_TIME_WHILE_TURNING;
            }
            else if (deltaTimeMax > cMAX_TIME_WHILE_TURNING)
            {
               deltaTimeMax = cMAX_TIME_WHILE_TURNING;
            }

            aDeltaT = std::min(aDeltaT, deltaTimeMax);
         }

         // Propagate heading forward in time:
         aHeading += aHeadingChangeDir * averageAccel / aHorizGroundSpeed * aDeltaT;
      }
   }
   else // Roll Rate Limits are non-zero
   {
      // === ROLL RATE LIMITS IMPOSED ===

      double newDT = aDeltaT;

      double t0RollAngle    = aRollAngle;
      double t0LateralAccel = UtEarth::cACCEL_OF_GRAVITY * tan(t0RollAngle);

      if (aHeadingChangeMag < 0.001)
      {
         // We are almost exactly on the desired heading, with just a teeny
         // mismatch, so roll the wings as much toward zero as possible, and
         // simply accept whatever average acceleration results, given the
         // previous roll angle.

         double t1LateralAccel = 0.0;
         double deltaRoll      = newDT * mMaxRollRate;
         if (fabs(aRollAngle) < deltaRoll)
         {
            aRollAngle = 0.0;
         }
         else
         {
            double sign = (aRollAngle > 0.0) ? -1.0 : 1.0;
            aRollAngle += sign * newDT * mMaxRollRate;
            t1LateralAccel = UtEarth::cACCEL_OF_GRAVITY * tan(aRollAngle);
         }
         double deltaHeading = 0.5 * (t0LateralAccel + t1LateralAccel) * newDT / aHorizGroundSpeed;
         aHeading += aHeadingChangeDir * deltaHeading;
      }
      else
      {
         double prevAbsT1LatAccel = 0.0;
         double absT1LateralAccel = absDesiredVPAvgAccel;
         double maxBankLimit      = atan2(absDesiredVPAvgAccel, UtEarth::cACCEL_OF_GRAVITY);

         unsigned int              numIterations   = 0;
         static const unsigned int cMAX_ITERATIONS = 9;

         // Iterate until neither dT or lateral acceleration changes much:
         double oldDT = 0.0;
         while ((((absT1LateralAccel - prevAbsT1LatAccel) > cACCEL_TOLERANCE) ||
                 (fabs(newDT - oldDT) > mUpdateTimeTolerance)) &&
                (++numIterations < cMAX_ITERATIONS))
         {
            oldDT                 = newDT;
            prevAbsT1LatAccel     = absT1LateralAccel;
            double absAvgTurnRate = (fabs(t0LateralAccel) + absT1LateralAccel) / 2.0 / aHorizGroundSpeed;
            double dTNeeded       = aHeadingChangeMag / absAvgTurnRate;

            if (dTNeeded < aDeltaT)
            {
               // Very small heading change required...
               absAvgTurnRate    = aHeadingChangeMag / aDeltaT;
               absT1LateralAccel = 2.0 * aHorizGroundSpeed * absAvgTurnRate - fabs(t0LateralAccel);
               newDT             = aDeltaT;
            }
            else
            {
               // A large bank will be required, with a significant roll angle
               // Now calculate what the average lateral acceleration will be
               // for the next time step, as a function of delta T:
               double rollDT         = newDT;
               double deltaRollAngle = rollDT * mMaxRollRate;
               if (deltaRollAngle > cMAX_DELTA_ROLL_ANGLE)
               {
                  deltaRollAngle = cMAX_DELTA_ROLL_ANGLE;
                  rollDT         = cMAX_DELTA_ROLL_ANGLE / mMaxRollRate;
               }

               bool bankLimited = (fabs(aRollAngle) + deltaRollAngle) >= maxBankLimit;
               if (!bankLimited)
               {
                  double futureRollAngle = t0RollAngle;
                  double desiredAccel    = absT1LateralAccel * aHeadingChangeDir;
                  if (desiredAccel < t0LateralAccel)
                  {
                     futureRollAngle = t0RollAngle - deltaRollAngle;
                  }
                  else if (desiredAccel > t0LateralAccel)
                  {
                     futureRollAngle = t0RollAngle + deltaRollAngle;
                  }
                  newDT = std::min(newDT, rollDT);
                  absT1LateralAccel =
                     std::min(UtEarth::cACCEL_OF_GRAVITY * fabs(tan(futureRollAngle)), absDesiredVPAvgAccel);
               }

               // Once we reach a bank angle limit (as established by several
               // different constraints), hold the bank angle until we can
               // accomplish all the heading change needed.
               absAvgTurnRate = (absT1LateralAccel + fabs(t0LateralAccel)) / 2.0 / aHorizGroundSpeed;
            } // end if dTNeeded

            // Limit the time and distance along a turn to
            // prevent abrupt changes (looks ugly in graphics).
            if (mMaxTurnRate != 0.0F)
            {
               double deltaTimeMax = sqrt(cMAX_DISTANCE_ALONG_TURN / (aHorizGroundSpeed * mMaxTurnRate));

               // Restrict the time step to defined duration.
               if (deltaTimeMax < cMIN_TIME_WHILE_TURNING)
               {
                  deltaTimeMax = cMIN_TIME_WHILE_TURNING;
               }
               else if (deltaTimeMax > cMAX_TIME_WHILE_TURNING)
               {
                  deltaTimeMax = cMAX_TIME_WHILE_TURNING;
               }

               newDT = std::min(newDT, deltaTimeMax);
            }

            // Limit the time only to what is required to
            // accomplish the needed heading change.
            double neededDT = std::min(aHeadingChangeMag, cMAX_ABS_CHANGE_IN_HDG) / absAvgTurnRate;
            newDT           = std::min(newDT, neededDT);
         } // end while

         aRollAngle = atan2((absT1LateralAccel * aHeadingChangeDir), UtEarth::cACCEL_OF_GRAVITY);
         aDeltaT    = std::min(aDeltaT, newDT);

         double headingDeltaMag = (absT1LateralAccel + fabs(t0LateralAccel)) * aDeltaT / 2.0 / aHorizGroundSpeed;

         aHeading += aHeadingChangeDir * headingDeltaMag;
      } // end if headingChangeMag
   }    // end if roll rate limits == 0.0 check

   if (aHeading > UtMath::cTWO_PI)
   {
      aHeading -= UtMath::cTWO_PI;
   }
   else if (aHeading <= -UtMath::cTWO_PI)
   {
      aHeading += UtMath::cTWO_PI;
   }
}

// ============================================================================
double WsfFollower::MinimumPositiveValue(double aValue1, double aValue2, double aValue3, double aValue4) const
{
   const double cHUGE  = 1.0E10;
   double       result = cHUGE;
   if ((aValue1 > 0.0F) && (aValue1 < result))
   {
      result = aValue1;
   }
   if ((aValue2 > 0.0F) && (aValue2 < result))
   {
      result = aValue2;
   }
   if ((aValue3 > 0.0F) && (aValue3 < result))
   {
      result = aValue3;
   }
   if ((aValue4 > 0.0F) && (aValue4 < result))
   {
      result = aValue4;
   }
   return result;
}

// ============================================================================
#ifdef DIAG_ON

void WsfFollower::OutputVec3d(std::string& aLabel, double aVec[])
{
   ut::log::debug() << aLabel << " " << aVec[0] << " " << aVec[1] << " " << aVec[2];
}

void WsfFollower::OutputDiagnostic(double aSimTime, WsfPlatform* aPlatformPtr)
{
   auto out = ut::log::debug() << "T=" << aSimTime;

   if (aPlatformPtr != nullptr)
   {
      out << "== Follower [" << aPlatformPtr->GetName() << "]";

      // Location in LLA
      double dLocLLA[3] = {0.0};
      aPlatformPtr->GetLocationLLA(dLocLLA[0], dLocLLA[1], dLocLLA[2]);
      OutputVec3d("   Location    LLA [d,m]  =", dLocLLA);

      double heading(0.0);
      double pitch(0.0);
      double roll(0.0);
      aPlatformPtr->GetOrientationNED(heading, pitch, roll);
      out << "   Orientation NED [d]    ="
          << " " << (heading * UtMath::cDEG_PER_RAD) << " " << (pitch * UtMath::cDEG_PER_RAD) << " "
          << (roll * UtMath::cDEG_PER_RAD) << "\n";

      double dVelNED[3] = {0.0};
      aPlatformPtr->GetVelocityNED(dVelNED);
      OutputVec3d("   Velocity    NED [m]  =", dVelNED);

      double dVelECS[3] = {0.0};
      aPlatformPtr->GetVelocityECS(dVelECS);
      OutputVec3d("   Velocity    ECS [m/s]  =", dVelECS);

      double dAccECS[3] = {0.0};
      aPlatformPtr->GetAccelerationECS(dAccECS);
      OutputVec3d("   Accel       ECS [m/s^2]=", dAccECS);

      out << "   Speed           [m/s]  = " << aPlatformPtr->GetSpeed();
   }
}

#endif
