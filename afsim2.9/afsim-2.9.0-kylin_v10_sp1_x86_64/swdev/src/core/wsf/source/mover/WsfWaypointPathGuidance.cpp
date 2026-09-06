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

#include "WsfWaypointPathGuidance.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfMover.hpp"
#include "WsfPathGuidance.hpp"
#include "WsfPlatform.hpp"
#include "WsfWaypoint.hpp"

namespace
{
const double cWAYPOINT_PATH_GUIDANCE_ACCEL_LIMIT = 6.0 * UtEarth::cACCEL_OF_GRAVITY;
}

WsfWaypointPathGuidance::WsfWaypointPathGuidance()
   : WsfPathGuidance()
   , mMaxSpeed(std::numeric_limits<double>::max())
   , mMinSpeed(0.0)
   , mMaxLinearAccel(cWAYPOINT_PATH_GUIDANCE_ACCEL_LIMIT)
   , mMaxClimbRate(0.0)
   , mMaxFlightPathAngle(0.0)
   , mMaxAltitude(std::numeric_limits<double>::max())
   , mMinAltitude(-std::numeric_limits<double>::max())
   , mTurnRateLimit(0.0)
   , mRollAngleLimit(0.0)
   , mBodyVertLimit(0.0)
   , mMaxRadialAccel(cWAYPOINT_PATH_GUIDANCE_ACCEL_LIMIT)
   , mRollRateLimit(0.0)
   , mHeadingPursuitGain(5.0)
{
}

// virtual
// WsfWaypointPathGuidance::~WsfWaypointPathGuidance()
//{
// }

// virtual
WsfPathGuidance* WsfWaypointPathGuidance::Clone() const
{
   return new WsfWaypointPathGuidance(*this);
}

// virtual
// bool WsfWaypointPathGuidance::Initialize(double aSimTime)
//{
//    // Anything else need to go here?
//    return WsfPathGuidance::Initialize(aSimTime, aMoverPtr);
// }

// virtual
double WsfWaypointPathGuidance::GetAltitude() const
{
   // Limit the returned altitude to the constraints.
   double tgtAlt = WsfPathGuidance::GetAltitude();
   return std::max(std::min(mMaxAltitude, tgtAlt), mMinAltitude);
}

// virtual
double WsfWaypointPathGuidance::GetSpeed() const
{
   double speedValue = 0.0;
   // Limit the returned speed to the constraint.
   if (SpeedIsSet())
   {
      speedValue = WsfPathGuidance::GetSpeed();
   }
   speedValue = std::min(mMaxSpeed, speedValue);
   speedValue = std::max(mMinSpeed, speedValue);
   return speedValue;
}

// virtual
bool WsfWaypointPathGuidance::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((command == "maximum_linear_acceleration") ||
       (command == "default_linear_acceleration")) // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxLinearAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaxLinearAccel, 0.0);
   }
   else if ((command == "maximum_radial_acceleration") ||
            (command == "default_radial_acceleration")) // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxRadialAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaxRadialAccel, 0.0);
   }
   else if ((command == "maximum_climb_rate") || (command == "default_climb_rate") || // for backward compatibility
            (command == "default_dive_rate"))                                         // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxClimbRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxClimbRate, 0.0);
   }
   else if (command == "turn_rate_limit")
   {
      aInput.ReadValueOfType(mTurnRateLimit, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mTurnRateLimit, 0.0);
   }
   else if (command == "bank_angle_limit")
   {
      aInput.ReadValueOfType(mRollAngleLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(mRollAngleLimit, 5.0 * UtMath::cRAD_PER_DEG, 85.0 * UtMath::cRAD_PER_DEG);
   }
   else if (command == "heading_pursuit_gain")
   {
      aInput.ReadValue(mHeadingPursuitGain);
      aInput.ValueGreater(mHeadingPursuitGain, 0.0);
   }
   else if (command == "body_g_limit")
   {
      aInput.ReadValueOfType(mBodyVertLimit, UtInput::cACCELERATION);
      aInput.ValueGreater(mBodyVertLimit, UtEarth::cACCEL_OF_GRAVITY);
   }
   else if (command == "roll_rate_limit")
   {
      aInput.ReadValueOfType(mRollRateLimit, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mRollRateLimit, 0.0);
   }
   else if (command == "maximum_flight_path_angle")
   {
      aInput.ReadValueOfType(mMaxFlightPathAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMaxFlightPathAngle, 0.0, UtMath::cPI_OVER_2);
   }
   else if (command == "maximum_altitude")
   {
      aInput.ReadValueOfType(mMaxAltitude, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaxAltitude, mMinAltitude);
   }
   else if (command == "minimum_altitude")
   {
      aInput.ReadValueOfType(mMinAltitude, UtInput::cLENGTH);
      aInput.ValueLessOrEqual(mMinAltitude, mMaxAltitude);
   }
   else if (command == "maximum_speed")
   {
      aInput.ReadValueOfType(mMaxSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMaxSpeed, 0.0);
   }
   else if (command == "minimum_speed")
   {
      aInput.ReadValueOfType(mMinSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinSpeed, 0.0);
   }
   else
   {
      myCommand = WsfPathGuidance::ProcessInput(aInput);
   }

   return myCommand;
}

double WsfWaypointPathGuidance::CalcLateralAccelLimit(double aCurrentSpeed)
{
   // For each valid (non-zero) constraint, change it into the same units (m/s^2),
   // and then select the most critical (smallest non-zero) one:
   double accel = std::numeric_limits<double>::max();

   if (mRollAngleLimit != 0.0)
   {
      accel = std::min(accel, UtEarth::cACCEL_OF_GRAVITY * tan(mRollAngleLimit));
   }

   if (TurnRateIsSet())
   {
      accel = std::min(accel, aCurrentSpeed * GetTurnRate());
   }

   if ((mTurnRateLimit != 0.0) && (aCurrentSpeed > 1.0))
   {
      accel = std::min(accel, aCurrentSpeed * mTurnRateLimit);
   }

   if (mBodyVertLimit != 0.0)
   {
      double acVal = UtEarth::cACCEL_OF_GRAVITY / mBodyVertLimit;
      if (fabs(acVal) > 1.0)
      {
         acVal = UtMath::Sign(acVal);
      }
      accel = std::min(accel, mBodyVertLimit * sin(acos(acVal)));
   }

   // Here, we use the waypoint-provided value (if any).
   if (RadialAccelIsSet() && GetRadialAccel() > 0)
   {
      accel = std::min(accel, GetRadialAccel());
   }

   // If we have not used any value, or we clamped the radial acceleration to zero,
   // this just won't do, so revert back to the default max value.
   if ((accel <= 0.0) || (accel == std::numeric_limits<double>::max()))
   {
      accel = mMaxRadialAccel;
   }

   return accel;
}

//! This method is written especially for WsfWaypointMover use.
void WsfWaypointPathGuidance::GetWPTargetInfo(double  aVelNED[3],
                                              double& aPlatformHeading,
                                              double& aPlatformPitch,
                                              double& aPlatformRoll,
                                              double& aHeadingChangeMag,
                                              double& aHdgChgDirection,
                                              double& aGroundSpeed,
                                              double& aTotalSpeed,
                                              double& aGroundDistToTgt,
                                              double& aTotalDistToTgt)
{
   static const double cFOREVER = std::numeric_limits<double>::max();

   assert(TurnIsDetermined());

   // A little bit of a misnomer/simplification/approximation used here
   // is that the roll angle is actual, but provided the platform is in
   // motion, the heading and pitch are based upon VELOCITY, not actual
   // body orientation (we ignore any aerodynamic angle of attack).

   double trueHdg, truePitch;
   GetMover()->GetPlatform()->GetOrientationNED(trueHdg, truePitch, aPlatformRoll);
   GetMover()->GetPlatform()->GetVelocityNED(aVelNED);
   aTotalSpeed  = UtVec3d::Magnitude(aVelNED);
   aGroundSpeed = sqrt(aVelNED[0] * aVelNED[0] + aVelNED[1] * aVelNED[1]);
   if (aGroundSpeed > WsfPath::cNEGLIGIBLE_SPEED)
   {
      aPlatformHeading = atan2(aVelNED[1], aVelNED[0]);
      aPlatformPitch   = atan2(-aVelNED[2], aGroundSpeed);
   }
   else
   {
      aPlatformHeading = trueHdg;
      aPlatformPitch   = truePitch;
   }
   aPlatformPitch   = UtMath::NormalizeAngleMinusPi_Pi(aPlatformPitch);
   aPlatformHeading = UtMath::NormalizeAngleMinusPi_Pi(aPlatformHeading);

   // There are several possibilities:
   // 1) We are traveling to a particular (Lat, Lon) waypoint (translated into WCS),
   // 2) We are traveling to a specified heading (already placed into the mTgtHdgNorth variable),
   // 3) We are blindly extrapolating a fixed heading for a certain distance or time.
   double targetHeading = 0.0;
   if (LatLonIsSet())
   {
      double nedOffset[3];
      double alt = 0.0;
      if (AltIsSet())
      {
         alt = GetAltitude();
      }
      GetMover()->GetPlatform()->ConvertLLAToNED(GetLatitude(), GetLongitude(), alt, nedOffset);
      targetHeading    = atan2(nedOffset[1], nedOffset[0]);
      aGroundDistToTgt = sqrt(nedOffset[0] * nedOffset[0] + nedOffset[1] * nedOffset[1]);
      aTotalDistToTgt  = UtVec3d::Magnitude(nedOffset);
   }
   else if (HeadingIsSet())
   {
      assert(TurnIsDetermined());
      targetHeading = ChosenHeading();

      // We cannot know the distance this in this context...
      aGroundDistToTgt = cFOREVER;
      aTotalDistToTgt  = cFOREVER;
   }

   double tgtBearing = UtMath::NormalizeAngleMinusPi_Pi(targetHeading - aPlatformHeading);
   aHeadingChangeMag = fabs(tgtBearing);

   // If we are forced to turn a particular direction, then the
   // turn magnitude may be greater than 180 degrees
   if (ChosenDirection() == WsfPath::cTURN_DIR_LEFT)
   {
      aHdgChgDirection = -1.0;
      if (tgtBearing > 0.0)
      {
         aHeadingChangeMag = UtMath::cTWO_PI - aHeadingChangeMag;
      }
   }
   else if (ChosenDirection() == WsfPath::cTURN_DIR_RIGHT)
   {
      aHdgChgDirection = 1.0;
      if (tgtBearing < 0.0)
      {
         aHeadingChangeMag = UtMath::cTWO_PI - aHeadingChangeMag;
      }
   }
   else // if Turn Dir = SHORTEST
   {
      if (tgtBearing >= 0.0)
      {
         aHdgChgDirection = 1.0;
      }
      else
      {
         aHdgChgDirection = -1.0;
      }
   }

   // Once we start the big turn, and close to within "close enough", then we need to
   // get out of the way, and let the application close to zero heading delta.
   if (aHeadingChangeMag < WsfPath::cHEADING_FUZZ)
   {
      ClearChosenDir();
   }
}

//! Get the radial acceleration to be used for turning FROM the specified waypoint.
//! This overrides the base class method to impose additional constraints that are provided
//! by this class.
// virtual
double WsfWaypointPathGuidance::GetRadialAccelAtPoint(const WsfWaypoint& aPoint) const
{
   // For each valid (non-zero) constraint, change it into the same units (m/s^2),
   // and then select the most critical (smallest non-zero) one:
   double accel = std::numeric_limits<double>::max();

   if (mRollAngleLimit != 0.0)
   {
      accel = std::min(accel, UtEarth::cACCEL_OF_GRAVITY * tan(mRollAngleLimit));
   }

   double speed = aPoint.GetSpeed();
   if (speed == WsfPath::cDOUBLE_NOT_SET)
   {
      speed = GetMover()->GetPlatform()->GetSpeed();
   }

   // (JAJ) I never did find a place where 'WsfPathGuidance::SetTurnRate' was ever called.
   // I'm not even sure it would be applicable because the turn rate would be one that should
   // be used because it is the turn rate to turn to the point in question, not from the point.

   // if (TurnRateIsSet())
   //{
   //    accel = std::min(accel, speed * GetTurnRate());
   // }

   if ((mTurnRateLimit != 0.0) && (speed > 1.0))
   {
      accel = std::min(accel, speed * mTurnRateLimit);
   }

   if (mBodyVertLimit != 0.0)
   {
      double acVal = UtEarth::cACCEL_OF_GRAVITY / mBodyVertLimit;
      if (fabs(acVal) > 1.0)
      {
         acVal = UtMath::Sign(acVal);
      }
      accel = std::min(accel, mBodyVertLimit * sin(acos(acVal)));
   }

   // Use the waypoint-provided value (if any).
   if (aPoint.GetRadialAccel() != WsfPath::cDOUBLE_NOT_SET)
   {
      accel = std::min(accel, aPoint.GetRadialAccel());
   }

   // If we have not used any value, or we clamped the radial acceleration to zero,
   // this just won't do, so revert back to the default max value.
   if ((accel == 0.0) || (accel == std::numeric_limits<double>::max()))
   {
      accel = mMaxRadialAccel;
   }

   return accel;
}
