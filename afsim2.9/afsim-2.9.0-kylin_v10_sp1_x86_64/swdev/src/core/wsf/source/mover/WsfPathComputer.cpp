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

#include "WsfPathComputer.hpp"

#include <cassert>

#include "UtCircleTangents.hpp"
#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPolynomial.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPathList.hpp"
#include "WsfWaypoint.hpp"

namespace
{
// These thresholds prevent divide by near-zero:
constexpr double cMIN_ACCELERATION    = 1.0E-7;
constexpr double cMIN_SPEED           = 1.0E-7;
constexpr double cMAXIMUM_TURN_RADIUS = UtSphericalEarth::cEARTH_RADIUS * 0.47;
double           GetAbsoluteAltitude(double aAltMSL)
{
   return aAltMSL + UtSphericalEarth::cEARTH_RADIUS;
}

double ComputeHeadingChange(double                       aCurrentHeading,
                            double                       aTargetHeading,
                            WsfPathTarget::TurnDirection aTurnDirection,
                            bool&                        aIsClockwise)
{
   double    headingChange    = aTargetHeading - aCurrentHeading;
   const int turnDirectionInt = static_cast<int>(aTurnDirection);
   if (aTurnDirection == WsfPathTarget::cTURN_DIR_SHORTEST)
   {
      headingChange = UtMath::NormalizeAngleMinusPi_Pi(headingChange);
   }
   else if (headingChange * turnDirectionInt < 0.0)
   {
      headingChange = turnDirectionInt * UtMath::NormalizeAngle0_TwoPi(turnDirectionInt * headingChange);
   }
   aIsClockwise = (headingChange > 0);
   return headingChange;
}

void ConvertNEDVectorToWCS(const double aLocationWCS[3], const double aNED[3], double aWCS[3])
{
   UtEntity tmp;
   tmp.SetLocationWCS(aLocationWCS);
   tmp.ConvertNEDVectorToWCS(aWCS, aNED);
}

void GetPerpendicularVectors(const UtVec3d& aV, UtVec3d& aNorthern, UtVec3d& aEastern)
{
   UtVec3d aboveNorthPole(0, 0, UtSphericalEarth::cEARTH_RADIUS * 2);
   aEastern.CrossProduct(aboveNorthPole, aV);
   aEastern.Normalize();
   aNorthern.CrossProduct(aV, aEastern);
   aNorthern.Normalize();
}

class TurnComputation
{
   void UpdateAxis()
   {
      mToTargetWCS = mTargetWCS - mAxisWCS;
      mToStartWCS  = mTargetWCS - mAxisWCS;
   }

public:
   TurnComputation(const WsfPathState& aState, const UtVec3d& aTargetWCS, double aRadius, bool aIsClockwise)
      : mTargetWCS(aTargetWCS)
      , mTurnFast(false)
   {
      mErrorDistance     = 0.0;
      mIsClockwise       = aIsClockwise;
      mRadius            = aRadius;
      double earthRadius = GetAbsoluteAltitude(aState.mAltitude);
      mAngleToCenter     = aRadius / earthRadius;
      if (!mIsClockwise)
      {
         mAngleToCenter = -mAngleToCenter;
      }

      UtEntity::ConvertLLAToWCS(aState.mLatitude, aState.mLongitude, aState.mAltitude, mStartWCS.GetData());
      ConvertNEDVectorToWCS(mStartWCS.GetData(), aState.mVelocityNED, mUnitVelWCS.GetData());
      mUnitVelWCS.Normalize();
      mAxisWCS = UtVec3d::Rotate(mUnitVelWCS, mAngleToCenter, mStartWCS);

      UpdateAxis();
   }

   void Reverse()
   {
      mAxisWCS     = UtVec3d::Rotate(mUnitVelWCS, -mAngleToCenter, mStartWCS);
      mIsClockwise = !mIsClockwise;
      UpdateAxis();
   }

   void GetCPA_TurnAngle(double& aTurnAngle)
   {
      UtVec3d vx, vy;
      UtVec3d toTargetWCS = mTargetWCS - mAxisWCS;
      UtVec3d toStartWCS  = mStartWCS - mAxisWCS;
      GetPerpendicularVectors(mAxisWCS, vx, vy);
      UtVec2d toStartLocal(vx.DotProduct(toStartWCS), vy.DotProduct(toStartWCS));
      UtVec2d toTargetLocal(vx.DotProduct(toTargetWCS), vy.DotProduct(toTargetWCS));
      double  angleToStart  = atan2(toStartLocal[1], toStartLocal[0]);
      double  angleToFinish = atan2(toTargetLocal[1], toTargetLocal[0]);
      aTurnAngle            = angleToFinish - angleToStart;
      if (!mIsClockwise && aTurnAngle > 0)
      {
         aTurnAngle = -UtMath::NormalizeAngle0_TwoPi(-aTurnAngle);
      }
      else if (mIsClockwise && aTurnAngle < 0)
      {
         aTurnAngle = UtMath::NormalizeAngle0_TwoPi(aTurnAngle);
      }
   }

   bool GetTurnAngle(double& aTurnAngle)
   {
      mErrorDistance = 0;
      UtVec3d vx, vy;
      GetPerpendicularVectors(mAxisWCS, vx, vy);
      UtVec3d toTargetWCS(mTargetWCS - mAxisWCS);
      double  range = toTargetWCS.Magnitude();
      UtVec2d toTargetLocal(vx.DotProduct(toTargetWCS), vy.DotProduct(toTargetWCS));
      UtVec2d tangent;
      bool    ok = UtCircleTangents::CirclePointTangent(UtVec2d(0, 0),
                                                     mRadius,
                                                     toTargetLocal,
                                                     mIsClockwise ? UtCircleTangents::cCW : UtCircleTangents::cCCW,
                                                     tangent);

      if (ok)
      {
         UtVec3d toStartWCS(mStartWCS - mAxisWCS);
         UtVec2d toStartLocal(vx.DotProduct(toStartWCS), vy.DotProduct(toStartWCS));
         double  angleToStart  = atan2(toStartLocal[1], toStartLocal[0]);
         double  angleToFinish = atan2(tangent[1], tangent[0]);
         aTurnAngle            = angleToFinish - angleToStart;
         if (!mIsClockwise && aTurnAngle > 0)
         {
            aTurnAngle = -UtMath::NormalizeAngle0_TwoPi(-aTurnAngle);
         }
         else if (mIsClockwise && aTurnAngle < 0)
         {
            aTurnAngle = UtMath::NormalizeAngle0_TwoPi(aTurnAngle);
         }
         double errorThreshold = std::min(.01 * range / mRadius, .5);
         if (fabs(aTurnAngle) > UtMath::cTWO_PI - errorThreshold)
         {
            aTurnAngle = 0.0;
         }
      }
      else
      {
         mErrorDistance = (mTargetWCS - mAxisWCS).Magnitude();
         GetCPA_TurnAngle(aTurnAngle);
      }
      return ok;
   }

   UtVec3d mTargetWCS;
   UtVec3d mStartWCS;

   UtVec3d mUnitVelWCS;
   double  mAngleToCenter;
   UtVec3d mToTargetWCS;
   UtVec3d mToStartWCS;

   double  mRadius;
   UtVec3d mAxisWCS;
   bool    mTurnFast;
   bool    mIsClockwise;
   double  mErrorDistance;
};

// Returns the turn direction that should be used to reach the target.
WsfPathTarget::TurnDirection GetTurnDirection(WsfPathTarget::TurnDirection aDirection,
                                              const UtVec3d&               aLocationWCS,
                                              UtVec3d&                     aVelocityWCS,
                                              UtVec3d&                     aTargetWCS,
                                              bool&                        aIsTurn)
{
   aIsTurn                                = true;
   WsfPathTarget::TurnDirection direction = aDirection;
   if (direction == WsfPathTarget::cTURN_DIR_SHORTEST)
   {
      UtVec3d crossed;
      crossed.CrossProduct(aVelocityWCS, aLocationWCS);
      UtVec3d toTarget  = aTargetWCS - aLocationWCS;
      double  dot       = crossed.DotProduct(toTarget);
      bool    clockwise = dot > 0.0;
      direction         = (clockwise ? WsfPathTarget::cTURN_DIR_RIGHT : WsfPathTarget::cTURN_DIR_LEFT);
      // The turn is small if the dot product is small
      aIsTurn = (dot > 1.0E-20) || (dot < -1.0E-20);
   }
   return direction;
}

} // namespace

// ================================================================================================
void WsfPathTarget::Print(ut::log::MessageStream& aStream)
{
   auto& out = aStream;
   if (mTargetType & cLOCATION)
   {
      out.AddNote() << "Lat: " << mLatitude;
      out.AddNote() << "Lon: " << mLongitude;
   }
   if (mTargetType & cALTITUDE)
   {
      out.AddNote() << "Alt: " << mAltitude << " m";
   }
   if (mTargetType & cHEADING)
   {
      out.AddNote() << "Heading: " << mHeading;
   }
   if (mTargetType & cSPEED)
   {
      out.AddNote() << "Speed: " << mSpeed << " m/s";
   }
   out.AddNote() << "Extrapolate: " << std::boolalpha << !!(mTargetType & cEXTRAPOLATE);

   if (mTargetType & cTURN_ON_APPROACH)
   {
      auto note = out.AddNote() << "Turn on Approach:";
      note.AddNote() << "Lat: " << mNextPointLat;
      note.AddNote() << "Lon: " << mNextPointLon;
      note.AddNote() << "Radial Accel: " << mNextPointRadialAccel << " m/s^2";
   }
   switch (mTurnDirection)
   {
   case cTURN_DIR_SHORTEST:
      out.AddNote() << "Turn Direction: Shortest";
      break;
   case cTURN_DIR_LEFT:
      out.AddNote() << "Turn Direction: Left";
      break;
   case cTURN_DIR_RIGHT:
      out.AddNote() << "Turn Direction: Right";
      break;
   default:
      // do nothing
      break;
   }
}

// ================================================================================================
WsfPathComputer::WsfPathComputer(ut::Random* aRandomPtr)
   : mRandomPtr(aRandomPtr)
   , mConstraints()
   , mSimTime(0.0)
   , mIntegrationTime(0.1)
   , mMaximumTurn(UtMath::cPI * 1.8)
   , mPathVarianceRadius(0.0)
   , mSpeedVariancePct(0.0)
   , mDefaultSwitch(WsfPath::cSWITCH_ON_PASSING)
   , mUsingComputedClimbRate(false)
   , mUsingComputedLinearAcceleration(false)
   , mUsingComputedSpeed(false)
   , mFlyFlightPathToTarget(false)
{
}

// ================================================================================================
void WsfPathComputer::ComputePath(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPathList)
{
   if (aTarget.mTargetType & WsfPathTarget::cLOCATION)
   {
      TurnToPoint(aState, aTarget, aPathList, 0);
   }
   else
   {
      if (aTarget.mTargetType & WsfPathTarget::cHEADING)
      {
         bool relativeTurn = (aTarget.mTargetType & WsfPathTarget::cRELATIVE_TURN) != 0;
         if (!relativeTurn)
         {
            TurnToHeading(aState, aTarget, aPathList, aTarget.mTurnDirection);
         }
         else
         {
            TurnRelative(aState, aTarget, aPathList, aTarget.mHeading);
         }
      }
      if (aTarget.mTargetType & WsfPathTarget::cEXTRAPOLATE)
      {
         WsfPathState state;
         if (!aPathList.Empty())
         {
            state = aPathList.GetEndState();
         }
         else
         {
            state = aState;
         }
         StraightOnHeading(state, aTarget, aPathList);
      }
   }

   // If we haven't figured anything out yet, put in a stub position
   if (aPathList.Empty())
   {
      WsfPathList::PauseSegment* segmentPtr = new WsfPathList::PauseSegment;
      segmentPtr->Initialize(0.0, aState);
      aPathList.Append(segmentPtr, aState);
   }
}

// ================================================================================================
//! Given the current state, the next waypoint and the waypoint after, create the 'path target'
//! for ComputePath.
//!
//! @param aCurrentState       [input] The current state.
//! @param aCurrentConstraints [input] The current path constraints.
//! @param aMoverConstraints   [input] The constraints defined in the mover by the user.
//! @param aTargetPoint        [input] A reference to the target waypoint.
//! @param aNextPointPtr       [input] A pointer to the waypoint that follows the target waypoint.
//! This is only used if the target waypoint specifies 'turn on approach', and may be 0.
//! @param aTarget             [output] The created path target.
//!
//! @note This is intended to be used in an iterative fashion as one traverses a route as it
//! maintains a persistent state.
void WsfPathComputer::CreateTarget(const WsfPathState&       aCurrentState,
                                   WsfPathConstraints&       aCurrentConstraints,
                                   const WsfPathConstraints& aMoverConstraints,
                                   const WsfWaypoint&        aTargetPoint,
                                   const WsfWaypoint*        aNextPointPtr,
                                   WsfPathTarget&            aTarget)
{
   // Keep speed and altitude targets if not changed
   aTarget.mTargetType &= (WsfPathTarget::cALTITUDE | WsfPathTarget::cSPEED);
   aTarget.mDistance = aTarget.mTime = 0.0;
   if (mUsingComputedClimbRate)
   {
      mUsingComputedClimbRate                 = false;
      aCurrentConstraints.mMaxClimbRate       = WsfPathConstraints::cLARGE_DOUBLE;
      aCurrentConstraints.mMaxFlightPathAngle = WsfPathConstraints::cDEFAULT_MAX_FLIGHT_ANGLE;
   }
   if (mUsingComputedLinearAcceleration)
   {
      mUsingComputedLinearAcceleration    = false;
      aCurrentConstraints.mMaxLinearAccel = WsfPathConstraints::cDEFAULT_MAX_LINEAR_ACCEL;
   }
   if (mUsingComputedSpeed)
   {
      mUsingComputedSpeed = false;
      aTarget.mTargetType &= ~WsfPathTarget::cSPEED;
   }

   if (aTargetPoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
   {
      aTarget.mLatitude  = aTargetPoint.GetLat();
      aTarget.mLongitude = aTargetPoint.GetLon();
      aTarget.mTime      = aTargetPoint.GetDistanceOrTime();
      aTarget.mTargetType |= WsfPathTarget::cLOCATION;

      if (mPathVarianceRadius > 0 && mRandomPtr)
      {
         WsfGeoPoint newWaypoint(aTarget.mLatitude, aTarget.mLongitude, aTarget.mAltitude);
         double      randomHeading = mRandomPtr->Uniform(0.0, 360.0);
         double      randomRadius  = mRandomPtr->Uniform(0.0, mPathVarianceRadius);
         newWaypoint.Extrapolate(randomHeading, randomRadius);

         aTarget.mLatitude  = newWaypoint.GetLat();
         aTarget.mLongitude = newWaypoint.GetLon();
      }
   }
   else
   {
      if (aTargetPoint.GetPointType() & WsfWaypoint::cDISTANCE)
      {
         aTarget.mDistance = aTargetPoint.GetDistanceOrTime();
      }
      else if (aTargetPoint.GetPointType() & WsfWaypoint::cTIME)
      {
         aTarget.mTime = aTargetPoint.GetDistanceOrTime();
      }

      if (aTargetPoint.GetHeading() != WsfPath::cDOUBLE_NOT_SET)
      {
         double heading = aTargetPoint.GetHeading();
         if (aTargetPoint.GetPointType() & WsfWaypoint::cRELATIVE_HEADING)
         {
            aTarget.mTargetType |= WsfPathTarget::cRELATIVE_TURN;
         }
         aTarget.mHeading = heading;
         aTarget.mTargetType |= WsfPathTarget::cHEADING;
      }
   }

   aTarget.mTurnDirection = (WsfPathTarget::TurnDirection)aTargetPoint.GetTurnDirection();

   if (mSpeedVariancePct > 0 && mRandomPtr)
   {
      double speedDiff = aTarget.mSpeed * mSpeedVariancePct;

      double randomSpeed = mRandomPtr->Uniform(aTarget.mSpeed - speedDiff, aTarget.mSpeed + speedDiff);
      aTarget.mSpeed     = randomSpeed;
   }

   bool switchOnApproach =
      (aTargetPoint.GetSwitch() == WsfPath::cSWITCH_ON_APPROACH) ||
      (mDefaultSwitch == WsfPath::cSWITCH_ON_APPROACH && aTargetPoint.GetSwitch() == WsfPath::cSWITCH_MOVER_DEFAULT);
   if (aNextPointPtr != nullptr && switchOnApproach &&
       (aNextPointPtr->GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE))
   {
      aTarget.mTargetType |= WsfPathTarget::cTURN_ON_APPROACH;
      aTarget.mNextPointLat = aNextPointPtr->GetLat();
      aTarget.mNextPointLon = aNextPointPtr->GetLon();

      if (aTargetPoint.GetRadialAccel() > 0)
      {
         aTarget.mNextPointRadialAccel = aTargetPoint.GetRadialAccel();
      }
      else if (aTargetPoint.GetRadialAccel() == WsfPath::cUSE_PREVIOUS)
      {
         aTarget.mNextPointRadialAccel = GetConstraints().mMaxRadialAccel;
      }
      else
      {
         aTarget.mNextPointRadialAccel = aMoverConstraints.mMaxRadialAccel;
      }
   }
   else
   {
      aTarget.mTargetType &= ~WsfPathTarget::cTURN_ON_APPROACH;
   }
   const unsigned int cLOCATION_AND_ALTITUDE = (WsfPathTarget::cLOCATION | WsfPathTarget::cALTITUDE);
   const unsigned int cLOCATION_AND_SPEED    = (WsfPathTarget::cLOCATION | WsfPathTarget::cSPEED);

   // TODO WsfPathConstraints& pathConstraints = aPathComputer.GetConstraints();
   WsfPathConstraints& pathConstraints = mConstraints;

   // If target is position/altitude && climb rate or flight angle are not constrained,
   // Compute a flight angle based on distance

   if (mFlyFlightPathToTarget || ((pathConstraints.mMaxClimbRate == WsfPathConstraints::cLARGE_DOUBLE) &&
                                  (pathConstraints.mMaxFlightPathAngle == WsfPathConstraints::cDEFAULT_MAX_FLIGHT_ANGLE) &&
                                  (cLOCATION_AND_ALTITUDE == (aTarget.mTargetType & cLOCATION_AND_ALTITUDE))))
   {
      double lat, lon, alt;
      aCurrentState.GetLocationLLA(lat, lon, alt);
      double altitudeChange = fabs(aTarget.mAltitude - alt);
      if (altitudeChange > 1)
      {
         double heading, distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, aTarget.mLatitude, aTarget.mLongitude, heading, distance);

         double flightAngle                  = atan2(altitudeChange, distance);
         pathConstraints.mMaxFlightPathAngle = flightAngle;
         mUsingComputedClimbRate             = true;
      }
   }

   if ((cLOCATION_AND_SPEED == (aTarget.mTargetType & cLOCATION_AND_SPEED)) &&
       (pathConstraints.mMaxLinearAccel == WsfPathConstraints::cDEFAULT_MAX_LINEAR_ACCEL) && (aTarget.mTime == 0.0))
   {
      double lat, lon, alt;
      aCurrentState.GetLocationLLA(lat, lon, alt);
      double speed       = aCurrentState.GetSpeed();
      double speedChange = fabs(aTarget.mSpeed - speed);
      if (speedChange > .5)
      {
         double heading, distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, aTarget.mLatitude, aTarget.mLongitude, heading, distance);
         double avgSpeed          = (speed + aTarget.mSpeed) / 2.0;
         double timeToDestination = distance / avgSpeed;
         if (timeToDestination > 1.0E-9)
         {
            pathConstraints.mMaxLinearAccel = speedChange / timeToDestination;
         }
         else
         {
            pathConstraints.mMaxLinearAccel = 1.0E9;
         }
         mUsingComputedLinearAcceleration = true;
      }
   }

   //! Routes are sometimes set up to loop on 1 point.  Handle this case explicitly with a 360 turn.
   if ((&aTargetPoint == aNextPointPtr) && (aTarget.mTargetType & WsfPathTarget::cLOCATION))
   {
      double lat, lon, alt;
      aCurrentState.GetLocationLLA(lat, lon, alt);
      if (UtVec2d((aTarget.mLatitude - lat), (aTarget.mLongitude - lon)).MagnitudeSquared() < 0.0001)
      {
         aTarget.mTargetType &= ~WsfPathTarget::cLOCATION;
         aTarget.mTargetType |= WsfPathTarget::cRELATIVE_TURN | WsfPathTarget::cHEADING;
         double turnSign  = (aTargetPoint.GetTurnDirection() == WsfPath::cTURN_DIR_RIGHT) ? 1.0 : -1.0;
         aTarget.mHeading = turnSign * UtMath::cTWO_PI;
      }
   }

   if ((aTargetPoint.GetPointType() & WsfWaypoint::cTIME_TO_POINT) && (aTargetPoint.GetDistanceOrTime() > 0.0))
   {
      // If we are trying to hit point at time, make sure we aren't paused
      if (aCurrentState.GetSpeed() <= 1.0e-3)
      {
         aTarget.mTargetType |= WsfPathTarget::cSPEED;
         aTarget.mSpeed = 1.0;
      }
   }

   ConstrainTarget(aTarget, aCurrentConstraints);
}

// ================================================================================================
//! Bind target to a valid altitude/speed
void WsfPathComputer::ConstrainTarget(WsfPathTarget& aTarget, const WsfPathConstraints& aConstraints)
{
   if (aConstraints.mIsOnGround)
   {
      aTarget.mTargetType &= ~(WsfPathTarget::cALTITUDE);
   }
   if (aTarget.mTargetType & WsfPathTarget::cALTITUDE)
   {
      aTarget.mAltitude = UtMath::Limit(aTarget.mAltitude, aConstraints.mMinAltitude, aConstraints.mMaxAltitude);
   }
   if (aTarget.mTargetType & WsfPathTarget::cSPEED)
   {
      aTarget.mSpeed = UtMath::Limit(aTarget.mSpeed, aConstraints.mMinSpeed, aConstraints.mMaxSpeed);
   }
}

// ================================================================================================
// If the target is a location, and a time is specified,
// modify the path to reach the point at the right time.
void WsfPathComputer::RevisePathHitPointAtTime(WsfPathTarget& aTarget, WsfPathList& aPath)
{
   if (aPath.Empty())
   {
      return;
   }

   WsfPathConstraints&              pathConstraints = GetConstraints();
   WsfPathList::SegmentList         segments        = aPath.GetSegments();
   WsfPathList::Segment*            segmentPtr      = segments.back();
   WsfPathList::RotationArcSegment* arcPtr          = dynamic_cast<WsfPathList::RotationArcSegment*>(segmentPtr);

   // This routine only works if the last segment of the path is a straight line.
   // The speed of the last leg of the path is modified to reach the point at the right time.
   if (arcPtr != nullptr)
   {
      double timeError      = aPath.GetDuration() - aTarget.mTime;
      double targetDuration = segmentPtr->GetDuration() - timeError;
      double speed          = arcPtr->GetInitialState().GetSpeed();
      double distance       = arcPtr->GetDuration() * speed;
      double accelerationTime(0.0);
      double targetSpeed(speed);

      // Compute the target speed, solve [ vt+a/2t^2+(v+at)(T-t)=D ]
      double acceleration = pathConstraints.mMaxLinearAccel;
      double speedUp      = UtMath::Sign(distance / speed - targetDuration);
      acceleration        = speedUp * acceleration;
      double coef[]       = {targetDuration * speed - distance, targetDuration * acceleration, -0.5 * acceleration};
      bool   possible     = UtPolynomial::SolveQuadricMinPos(coef, accelerationTime);
      accelerationTime    = possible ? accelerationTime : 1.0E10;
      targetSpeed         = std::min(speed + acceleration * accelerationTime, pathConstraints.mMaxSpeed);
      targetSpeed         = std::max(targetSpeed, pathConstraints.mMinSpeed);
      if (fabs(targetSpeed - speed) > 0.0001)
      {
         aTarget.mSpeed = targetSpeed;
         aTarget.mTargetType |= WsfPathTarget::cSPEED;
         WsfPathState state;
         WsfPathList  newPath;
         aPath.GetState(aPath.GetDuration() - arcPtr->GetDuration(), state);
         aPath.PopBack();
         ComputePath(state, aTarget, newPath);
         aPath.Append(newPath.GetDuration(), newPath);
         mUsingComputedSpeed = true;
      }
   }
}

// ================================================================================================
bool WsfPathComputer::TurnToPoint(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath, int aIterations)
{
   // Possible outcomes include:
   // 1. Successful turn to heading toward point, then fly straight to hit
   // 2. Regular turn can't intercept point
   //    a. If it's a regular point, just get as close as possible.
   //    a. If it's required, try to turn the opposite direction
   //    b. If it's optional, declare victory immediately

   bool complete(false);

   bool   turnOrient(false);
   bool   immediateOrient(false);
   double turnAngle(0.0);

   double speed  = UtVec3d::Magnitude(aState.mVelocityNED);
   double radius = GetTurnRadius(speed);

   UtVec3d targetWCS, startWCS;
   UtEntity::ConvertLLAToWCS(aState.mLatitude, aState.mLongitude, aState.mAltitude, startWCS.GetData());
   UtEntity::ConvertLLAToWCS(aTarget.mLatitude, aTarget.mLongitude, aState.mAltitude, targetWCS.GetData());
   double distanceToTarget = (startWCS - targetWCS).MagnitudeSquared();

   // Close enough already
   if (distanceToTarget < 0.01 * radius)
   {
      complete = true;
   }
   // Compute the turn if radius is reasonable
   double errorDistanceThreshold = std::max(radius * aTarget.mTurnFailureThreshold, 0.1);
   double errorDistance(0.0);
   if (!complete && radius > 0.1 && (radius < cMAXIMUM_TURN_RADIUS))
   {
      turnOrient = true;
      UtVec3d velocityWCS;
      ConvertNEDVectorToWCS(startWCS.GetData(), aState.mVelocityNED, velocityWCS.GetData());
      bool                         isTurn;
      WsfPathTarget::TurnDirection turnDirection =
         GetTurnDirection(aTarget.mTurnDirection, startWCS, velocityWCS, targetWCS, isTurn);
      bool hitPoint = false;
      if (isTurn)
      {
         // For computing turns to points at very large distances ( across the globe ), the
         // numeric accuracy begins to break down.  For these cases, just segment the path
         // and recurse. There are two checks that trigger path segmentation/recursion:
         // 1. The heading derived from the velocity vector differs from the great circle
         //    calculation by one degree or more. ( in place since v2.0 or earlier )
         // 2. The distance from the great circle calculation exceeds ~1/4 of earth's
         //    circumference. Longer distances render the arcsin-based arc length calculation
         //    in StraightToPoint() invalid because the range of asin() is -90 to +90 degrees.
         //    ( added to correct AFSIM-311 "Issues with long distance WSF_AIR_MOVER routes" )
         static const double cMAX_TURN_TO_POINT_DISTANCE = 2000000.0;
         if (distanceToTarget > cMAX_TURN_TO_POINT_DISTANCE * cMAX_TURN_TO_POINT_DISTANCE)
         {
            // ~1/4 earth circumference. The fuzz factor ensures less than 90 degrees for any surface path
            static constexpr double cMAX_STRAIGHT_TO_POINT_DISTANCE =
               0.2475 * UtMath::cTWO_PI * UtSphericalEarth::cEARTH_RADIUS;
            double maxSegmentDistance = -1; // indicates segmentation/recursion when > 0
            double heading            = -10000.0;
            if (speed > .1)
            {
               heading = atan2(aState.mVelocityNED[1], aState.mVelocityNED[0]);
            }
            double hdg, dist;
            UtSphericalEarth::GreatCircleHeadingAndDistance(aState.mLatitude,
                                                            aState.mLongitude,
                                                            aTarget.mLatitude,
                                                            aTarget.mLongitude,
                                                            hdg,
                                                            dist);
            // If current heading is not within 1 degree, add a path segment
            if (fabs(UtMath::NormalizeAngleMinusPi_Pi(heading - UtMath::cRAD_PER_DEG * hdg)) >= UtMath::cRAD_PER_DEG)
            {
               maxSegmentDistance = cMAX_TURN_TO_POINT_DISTANCE * .9;
            }
            // Else if great circle distance exceeds ~1/4 of earth circumference, add a path segment
            else if (dist > cMAX_STRAIGHT_TO_POINT_DISTANCE)
            {
               maxSegmentDistance = cMAX_STRAIGHT_TO_POINT_DISTANCE * .9;
            }

            // If current heading and distance are within limits, don't bother segmenting path -- just go straight
            if (maxSegmentDistance < 0 // Path was not segmented
                || aIterations >= 3)   // Make sure we don't iterate forever in a degenerate case
            {
               StraightToPoint(aState, aTarget, aPath, true);
               return true;
            }
            else
            {
               WsfPathTarget newTgt = aTarget;
               UtSphericalEarth::ExtrapolateGreatCirclePosition(aState.mLatitude,
                                                                aState.mLongitude,
                                                                hdg,
                                                                maxSegmentDistance,
                                                                newTgt.mLatitude,
                                                                newTgt.mLongitude);
               TurnToPoint(aState, newTgt, aPath, aIterations + 1);
               return TurnToPoint(aPath.GetEndState(), aTarget, aPath, aIterations + 1);
            }
         }

         TurnComputation turn(aState, targetWCS, radius, turnDirection == WsfPathTarget::cTURN_DIR_RIGHT);

         hitPoint           = turn.GetTurnAngle(turnAngle);
         errorDistance      = turn.mErrorDistance;
         bool pointRequired = ((aTarget.mTargetType & WsfPathTarget::cREQUIRED_POINT) != 0);
         if (aTarget.mTurnDirection == WsfPathTarget::cTURN_DIR_SHORTEST &&
             (pointRequired || (fabs(turnAngle) > UtMath::cPI)))
         {
            bool turnTooLarge  = fabs(turnAngle) > mMaximumTurn;
            bool doReverseTurn = !hitPoint && pointRequired && (errorDistanceThreshold < errorDistance);
            // We may have computed the wrong turn direction if the turn is very small
            if (turnTooLarge || doReverseTurn)
            {
               turn.Reverse();
               hitPoint      = turn.GetTurnAngle(turnAngle);
               errorDistance = turn.mErrorDistance;
               // If the turn is still too large, it could either be a point straight ahead
               // or a point just behind us.  If its ahead, turn immediately, if it's behind
               // declare victory.
               if (turnTooLarge && fabs(turnAngle) > mMaximumTurn)
               {
                  UtVec3d headingWCS;
                  ConvertNEDVectorToWCS(startWCS.GetData(), aState.mVelocityNED, headingWCS.GetData());
                  double dp       = (targetWCS - startWCS).DotProduct(headingWCS);
                  immediateOrient = (dp > 0);
                  turnOrient      = false;
                  complete        = true;
               }
            }
         }
      }
      else
      {
         immediateOrient = true;
         turnOrient      = false;
      }
      complete = complete || !hitPoint;
   }
   else if (!complete) // Bad turn radius
   {
      immediateOrient = (radius < 1);
   }

   // Turn wasn't close enough to reaching point
   if (errorDistance > errorDistanceThreshold)
   {
      if (aTarget.mTargetType & WsfPathTarget::cREQUIRED_POINT)
      {
         turnOrient = false;
         complete   = false;
      }
      else if (aTarget.mTargetType & WsfPathTarget::cOPTIONAL_POINT)
      {
         turnOrient = false;
         complete   = true;
      }
   }
   else if (turnAngle == 0.0 && !complete)
   {
      turnOrient      = false;
      immediateOrient = true;
   }

   if (turnOrient) // successful turn
   {
      // Turn toward point
      WsfPathState state;
      double       turnAngleAbs = fabs(turnAngle);
      const double cMAX_TURN_ERROR(0.005);
      if (turnAngleAbs > cMAX_TURN_ERROR && turnAngleAbs < (UtMath::cTWO_PI - cMAX_TURN_ERROR))
      {
         WsfPathTarget turnTarget(aTarget);
         turnTarget.mDistance = turnTarget.mTime = 0.0;
         TurnRelative(aState, turnTarget, aPath, turnAngle, radius);
         state = aPath.GetEndState();
      }
      else
      {
         state = aState;
      }
      if (!complete)
      {
         StraightToPoint(state, aTarget, aPath, true);
      }
   }
   else if (immediateOrient) // Fake the turn
   {
      StraightToPoint(aState, aTarget, aPath, true);
   }
   else if (!complete) // Try to hit point going straight
   {
      StraightToPoint(aState, aTarget, aPath, false);
   }

   return turnOrient;
}

// ================================================================================================
void WsfPathComputer::TurnToHeading(const WsfPathState&          aState,
                                    const WsfPathTarget&         aTarget,
                                    WsfPathList&                 aPathList,
                                    WsfPathTarget::TurnDirection aTurnDirection)
{
   bool   clockwise;
   double headingChange = ComputeHeadingChange(aState.mOrientationNED[0], aTarget.mHeading, aTurnDirection, clockwise);
   TurnRelative(aState, aTarget, aPathList, headingChange);
}

// ================================================================================================
void WsfPathComputer::TurnRelative(const WsfPathState&  aState,
                                   const WsfPathTarget& aTarget,
                                   WsfPathList&         aPathList,
                                   double               aTurnAngle,
                                   double               aTurnRadius)
{
   WsfPathState state(aState);
   double       speed  = UtVec3d::Magnitude(aState.mVelocityNED);
   double       radius = aTurnRadius;
   if (radius == -1.0)
   {
      radius = GetTurnRadius(speed);
   }

   if (speed < 0.01 || fabs(aTurnAngle) < 0.01 || radius < 0.1)
   {
      if (aTarget.mDistance > 0 || aTarget.mTime > 0)
      {
         // Turn instantly
         double heading = aTurnAngle;
         state.mOrientationNED[0] += heading;
         double n              = state.mVelocityNED[0];
         double e              = state.mVelocityNED[1];
         state.mVelocityNED[0] = n * std::cos(heading) + e * -std::sin(heading);
         state.mVelocityNED[1] = n * std::sin(heading) + e * std::cos(heading);
         StraightOnHeading(state, aTarget, aPathList);
      }
      else
      {
         WsfPathList::PauseSegment* segmentPtr = new WsfPathList::PauseSegment;
         state.mOrientationNED[0] = UtMath::NormalizeAngleMinusPi_Pi(state.mOrientationNED[0] + aTurnAngle);
         state.mVelocityNED[0]    = cos(state.mOrientationNED[0]) * speed;
         state.mVelocityNED[1]    = sin(state.mOrientationNED[0]) * speed;
         state.mVelocityNED[2]    = 0;
         segmentPtr->Initialize(0.0, state);
         aPathList.Append(segmentPtr, state);
      }
      return;
   }

   // If a turn radius is too large, this turn computation will fail.
   // Fly straight as a fallback
   if (radius > cMAXIMUM_TURN_RADIUS)
   {
      WsfPathTarget noHeadingChange = aTarget;
      noHeadingChange.mHeading      = aState.mOrientationNED[0];
      StraightOnHeading(aState, noHeadingChange, aPathList);
      return;
   }

   UtVec3d startWCS, velWCS;
   double  earthRadius   = GetAbsoluteAltitude(aState.mAltitude);
   double  angleToCenter = radius / earthRadius;
   double  turnDirection = 1.0;
   if (aTurnAngle < 0)
   {
      turnDirection = -1.0;
      angleToCenter = -angleToCenter;
   }
   UtEntity::ConvertLLAToWCS(aState.mLatitude, aState.mLongitude, aState.mAltitude, startWCS.GetData());
   ConvertNEDVectorToWCS(startWCS.GetData(), aState.mVelocityNED, velWCS.GetData());
   velWCS.Normalize();

   UtVec3d axis       = UtVec3d::Rotate(velWCS, angleToCenter, startWCS);
   double  arcLength  = turnDirection * aTurnAngle * radius;
   double  targetTime = UtMath::cDOUBLE_MAX;
   if (aTarget.mTime > 0.0)
   {
      targetTime = aTarget.mTime;
   }
   if (aTarget.mDistance > 0.0 && aTarget.mDistance < arcLength)
   {
      arcLength = aTarget.mDistance;
   }
   double durationBefore = aPathList.GetDuration();
   double targetRoll     = turnDirection * atan2((speed * speed), (UtEarth::cACCEL_OF_GRAVITY * radius));
   Rotate(axis * turnDirection, arcLength, targetTime, aTarget, aState, aPathList, radius, targetRoll);

   double duration         = aPathList.GetDuration() - durationBefore;
   double durationStraight = 0.0;
   double distanceStraight = 0.0;
   if (aTarget.mTime > 0.0)
   {
      if (duration > aTarget.mTime)
      {
         duration = aTarget.mTime;
      }
      else
      {
         durationStraight = aTarget.mTime - duration;
      }
   }
   if (aTarget.mDistance > 0.0)
   {
      if (aTarget.mDistance <= arcLength)
      {
         duration         = aTarget.mDistance / speed;
         durationStraight = 0.0;
      }
      else
      {
         distanceStraight = aTarget.mDistance - duration * speed;
      }
   }
   // If a time or distance is given, go straight until time/distance is met
   if (durationStraight > 0.0 || distanceStraight > 0.0)
   {
      WsfPathTarget target(aTarget);
      target.mTime     = durationStraight;
      target.mDistance = distanceStraight;
      StraightOnHeading(aPathList.GetEndState(), target, aPathList);
   }
}

// ================================================================================================
// Transitions without changing current heading to a target point
// @param aAllowHeadingChange If 'true', heading will change to correctly intercept target
//                            if 'false', heading will remain the same and quit when close to the target
void WsfPathComputer::StraightToPoint(const WsfPathState&  aState,
                                      const WsfPathTarget& aTarget,
                                      WsfPathList&         aPath,
                                      bool                 aAllowHeadingChange)
{
   double earthRadius = GetAbsoluteAltitude(aState.mAltitude);

   UtVec3d locationWCS;
   UtEntity::ConvertLLAToWCS(aState.mLatitude, aState.mLongitude, aState.mAltitude, locationWCS.GetData());
   double locationMagnitude = locationWCS.Magnitude();
   // Get target position with same magnitude as locationWCS
   UtVec3d targetWCS;
   UtEntity::ConvertLLAToWCS(aTarget.mLatitude, aTarget.mLongitude, aState.mAltitude, targetWCS.GetData());
   targetWCS *= locationMagnitude / targetWCS.Magnitude();
   UtVec3d toTargetWCS = targetWCS - locationWCS;

   UtVec3d rotationAxis;
   rotationAxis.CrossProduct(targetWCS, locationWCS);
   double crossLength       = rotationAxis.Normalize();
   double sinArcAngle       = crossLength / (locationMagnitude * locationMagnitude);
   double arcLengthToTarget = asin(sinArcAngle) * earthRadius;

   if (arcLengthToTarget > 0.01)
   {
      double arcLength;
      // We may be constrained to keeping the same direction
      if (!aAllowHeadingChange)
      {
         // This branch should only be necessary in strange turn failure circumstances.
         // This essentially just flies straight until we are no longer approaching the target.
         UtEntity entity;
         aState.CopyToEntity(entity);
         UtVec3d velocityWCS;
         entity.ConvertNEDVectorToWCS(velocityWCS.GetData(), aState.mVelocityNED);
         UtVec3d velocityCross, targetCross;
         targetCross.CrossProduct(locationWCS, toTargetWCS);
         velocityCross.CrossProduct(locationWCS, velocityWCS);
         double cosToTarget =
            targetCross.DotProduct(velocityCross) / (targetCross.Magnitude() * velocityCross.Magnitude());
         arcLength = arcLengthToTarget * cosToTarget;
         rotationAxis.CrossProduct(velocityWCS, locationWCS);
      }
      else
      {
         arcLength = arcLengthToTarget;
      }

      if (aTarget.mTargetType & WsfPathTarget::cTURN_ON_APPROACH)
      {
         UtVec3d nextWCS;
         UtEntity::ConvertLLAToWCS(aTarget.mNextPointLat, aTarget.mNextPointLon, aState.mAltitude, nextWCS.GetData());
         nextWCS *= locationMagnitude / nextWCS.Magnitude();
         UtVec3d toNextWCS      = nextWCS - targetWCS;
         double  distanceToNext = toNextWCS.Magnitude();
         if (distanceToNext > 0)
         {
            double angleToNext = acos(toTargetWCS.DotProduct(toNextWCS) / (toTargetWCS.Magnitude() * distanceToNext));
            double speed       = UtVec3d::Magnitude(aState.mVelocityNED);
            // Note: If the speed is changing, this calculation will not yield a correct answer.
            //       This will assume we have reached the target speed by the time the waypoint switch happens.
            if (aTarget.mTargetType & WsfPathTarget::cSPEED)
            {
               speed = aTarget.mSpeed;
            }
            double turnRadius = speed * speed / aTarget.mNextPointRadialAccel;
            // Only turn early if the angle is less than PI
            if (angleToNext < .95 * UtMath::cPI)
            {
               double turnEarlyDistance = turnRadius * tan(angleToNext / 2.0);
               // Turn early distance shouldn't be much larger than distance to next point
               turnEarlyDistance = std::min(turnEarlyDistance, distanceToNext * 2.0);
               arcLength -= turnEarlyDistance;
               if (arcLength < 0)
               {
                  arcLength = 1.0;
               }
            }
         }
      }
      Rotate(rotationAxis, arcLength, UtMath::cDOUBLE_MAX, aTarget, aState, aPath, earthRadius);
   }
}

// ================================================================================================
void WsfPathComputer::Rotate(UtVec3d              aRotationAxis,
                             double               aArcLength,
                             double               aTime,
                             const WsfPathTarget& aTarget,
                             const WsfPathState&  aState,
                             WsfPathList&         aPath,
                             double               aRotationRadius,
                             double               aTargetRoll)
{
   WsfPathState state = aState;

   UtVec3d locationWCS;
   UtEntity::ConvertLLAToWCS(state.mLatitude, state.mLongitude, state.mAltitude, locationWCS.GetData());
   double arcLength = aArcLength;

   SpeedAltitudeBreakpoint breakpoint;
   bool                    complete            = false;
   UtVec3d                 previousLocationWCS = locationWCS;
   double                  timeRemaining       = aTime;
   while (!complete)
   {
      if (ComputeSpeedAltitudeTargets(state, aTarget, breakpoint))
      {
         // If speed or altitude is changing...
         WsfPathList::RotationSegment* segmentPtr = new WsfPathList::RotationSegment;
         segmentPtr->SetRotationRadius(aRotationRadius);
         double duration        = breakpoint.mSimTime;
         double speed           = UtVec3d::Magnitude(state.mVelocityNED);
         double distanceChanged = speed * duration + breakpoint.mLinearAcceleration * duration * duration / 2.0;
         double sinFlightAngle  = std::sin(breakpoint.mFlightAngle);
         double cosFlightAngle  = std::cos(breakpoint.mFlightAngle);
         double altitudeChange  = breakpoint.mClimbRate * duration + distanceChanged * sinFlightAngle;

         double groundDistance = sqrt(distanceChanged * distanceChanged - altitudeChange * altitudeChange);

         if (groundDistance > arcLength)
         {
            // solve for duration required to hit target
            double coef[3] = {-arcLength,
                              sqrt(speed * speed - breakpoint.mClimbRate * breakpoint.mClimbRate) * cosFlightAngle,
                              (breakpoint.mLinearAcceleration * cosFlightAngle) / 2.0};
            UtPolynomial::SolveQuadricMinPos(coef, duration);
            complete = true;
         }
         else if (duration > timeRemaining)
         {
            duration = timeRemaining;
            complete = true;
         }

         segmentPtr->Initialize(duration, state);
         UtEntity::ConvertLLAToWCS(state.mLatitude, state.mLongitude, state.mAltitude, locationWCS.GetData());
         segmentPtr->SetTargetRoll(aTargetRoll);
         segmentPtr->InitializeRotation(locationWCS, aRotationAxis);
         segmentPtr->SetLinearAcceleration(breakpoint.mLinearAcceleration);
         segmentPtr->SetRollRate(mConstraints.mRollRateLimit);
         segmentPtr->SetClimbRate(breakpoint.mClimbRate);
         segmentPtr->SetFlightAngle(breakpoint.mFlightAngle);

         // When turning and changing speed, we don't have a closed-form solution.  Mark the segment as an approximation.
         if (aTargetRoll != 0.0 && breakpoint.mLinearAcceleration != 0)
         {
            segmentPtr->mIsApproximation = true;
         }

         aPath.Append(segmentPtr);
         state = aPath.GetEndState();

         arcLength -= groundDistance;

         previousLocationWCS = locationWCS;
         timeRemaining -= duration;
      }
      else // Speed & altitude are constant
      {
         double speed = UtVec3d::Magnitude(state.mVelocityNED);
         if ((mConstraints.mMaxLinearAccel > 0.0) && (aTarget.mTargetType & WsfPathTarget::cSPEED) &&
             (fabs(aTarget.mSpeed - speed) > .01))
         {
            // Snap to target speed if speed change is requested(direction is ignored)
            speed                 = aTarget.mSpeed;
            state.mVelocityNED[0] = speed;
            state.mVelocityNED[1] = state.mVelocityNED[2] = 0.0;
         }

         if (speed < 1.0E-9)
         {
            WsfPathList::PauseSegment* segmentPtr = new WsfPathList::PauseSegment;
            UtVec3d::Set(state.mVelocityNED, 0.0);
            UtVec3d::Set(state.mAccelerationNED, 0.0);
            segmentPtr->Initialize(UtMath::cDOUBLE_MAX, state);

            WsfPathState endState;
            state.mVelocityNED[0] = std::cos(state.mOrientationNED[0]) * arcLength;
            state.mVelocityNED[1] = std::sin(state.mOrientationNED[0]) * arcLength;
            UtEntity::ConvertLLAToWCS(state.mLatitude, state.mLongitude, state.mAltitude, locationWCS.GetData());
            WsfPathList::RotationArcSegment fakeSegment;
            fakeSegment.SetRotationRadius(aRotationRadius);
            fakeSegment.Initialize(1.0, state);
            fakeSegment.SetTargetRoll(aTargetRoll);
            fakeSegment.InitializeRotation(locationWCS, aRotationAxis);
            fakeSegment.SetRollRate(mConstraints.mRollRateLimit);
            fakeSegment.ComputeState(1.0, endState);
            state.mVelocityNED[0]        = 0.0;
            state.mVelocityNED[1]        = 0.0;
            endState.mVelocityNED[0]     = 0.0;
            endState.mVelocityNED[1]     = 0.0;
            endState.mAccelerationNED[0] = 0.0;
            endState.mAccelerationNED[1] = 0.0;

            aPath.Append(segmentPtr, endState);
         }
         else
         {
            UtEntity::ConvertLLAToWCS(state.mLatitude, state.mLongitude, state.mAltitude, locationWCS.GetData());
            WsfPathList::RotationArcSegment* segmentPtr = new WsfPathList::RotationArcSegment;
            segmentPtr->SetRotationRadius(aRotationRadius);

            double duration = arcLength / speed;
            if (duration > timeRemaining)
            {
               duration  = timeRemaining;
               arcLength = speed * duration;
            }

            segmentPtr->Initialize(duration, state);
            segmentPtr->SetTargetRoll(aTargetRoll);
            segmentPtr->InitializeRotation(locationWCS, aRotationAxis);
            segmentPtr->SetRollRate(mConstraints.mRollRateLimit);
            aPath.Append(segmentPtr);
         }
         complete = true;
      }
   }
}

// ================================================================================================
void WsfPathComputer::StraightOnHeading(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath)
{
   UtEntity entity;
   entity.SetLocationLLA(aState.mLatitude, aState.mLongitude, aState.mAltitude);
   double levelVelocity[3] = {aState.mVelocityNED[0], aState.mVelocityNED[1], 0.0};

   // Don't allow zero velocity
   if (levelVelocity[0] == 0 && levelVelocity[1] == 0)
   {
      double sinYaw    = std::sin(aState.mOrientationNED[0]);
      double cosYaw    = std::cos(aState.mOrientationNED[0]);
      levelVelocity[0] = cosYaw * 1.0E-9;
      levelVelocity[1] = sinYaw * 1.0E-9;
   }
   UtVec3d forwardWCS;
   entity.SetVelocityNED(levelVelocity);
   entity.GetVelocityWCS(forwardWCS.GetData());
   UtVec3d locationWCS;
   entity.GetLocationWCS(locationWCS.GetData());
   UtVec3d rotationAxis;
   rotationAxis.CrossProduct(forwardWCS, locationWCS);

   double targetTime     = UtMath::cDOUBLE_MAX;
   double targetDistance = UtMath::cDOUBLE_MAX;
   if (aTarget.mTime > 0.0)
   {
      targetTime = aTarget.mTime;
   }
   if (aTarget.mDistance > 0.0)
   {
      targetDistance = aTarget.mDistance;
   }

   Rotate(rotationAxis, targetDistance, targetTime, aTarget, aState, aPath, GetAbsoluteAltitude(aState.mAltitude));
}

// ================================================================================================
// Given speed and altitude targets, compute the time at which flight angle, climb rate, or linear acceleration changes.
bool WsfPathComputer::ComputeSpeedAltitudeTargets(const WsfPathState&      aState,
                                                  const WsfPathTarget&     aTarget,
                                                  SpeedAltitudeBreakpoint& aBreakpoint)
{
   aBreakpoint.mSimTime      = 0.0;
   bool   hasBreakpoint      = true;
   double initialSpeed       = UtVec3d::Magnitude(aState.mVelocityNED);
   double speedChange        = aTarget.mSpeed - initialSpeed;
   double altitudeChange     = aTarget.mAltitude - aState.mAltitude;
   double sinFlightPathAngle = sin(mConstraints.mMaxFlightPathAngle);
   bool   isAltitudeChange   = (aTarget.mTargetType & WsfPathTarget::cALTITUDE) != 0 && (fabs(altitudeChange) > 0.01) &&
                           (sinFlightPathAngle > cMIN_SPEED);
   bool isSpeedChange = (aTarget.mTargetType & WsfPathTarget::cSPEED) && (fabs(speedChange) > .01) &&
                        (mConstraints.mMaxLinearAccel >= cMIN_ACCELERATION);

   if (isSpeedChange && isAltitudeChange)
   {
      double linearAcceleration         = mConstraints.mMaxLinearAccel;
      double timeSpeedReached           = fabs((aTarget.mSpeed - initialSpeed) / mConstraints.mMaxLinearAccel);
      double accelerationDirection      = UtMath::Sign(aTarget.mSpeed - initialSpeed);
      double climbDirection             = UtMath::Sign(aTarget.mAltitude - aState.mAltitude);
      double speedAtClimbRateConstraint = mConstraints.mMaxClimbRate / sinFlightPathAngle;
      double acceleration               = mConstraints.mMaxLinearAccel * accelerationDirection;

      aBreakpoint.mClimbRate           = mConstraints.mMaxClimbRate * climbDirection;
      aBreakpoint.mFlightAngle         = mConstraints.mMaxFlightPathAngle * climbDirection;
      aBreakpoint.mLinearAcceleration  = acceleration;
      double timeAtClimbRateConstraint = (speedAtClimbRateConstraint - initialSpeed) / acceleration;

      if (accelerationDirection > 0)
      {
         if (timeAtClimbRateConstraint > 1.0E-3)
         {
            double distanceAtClimbRateConstraint =
               initialSpeed * timeAtClimbRateConstraint +
               mConstraints.mMaxLinearAccel * timeAtClimbRateConstraint * timeAtClimbRateConstraint / 2.0;
            double altdiff         = sinFlightPathAngle * distanceAtClimbRateConstraint;
            aBreakpoint.mClimbRate = 0.0;
            if (fabs(altitudeChange) > altdiff)
            {
               aBreakpoint.mSimTime = timeAtClimbRateConstraint;
            }
            else
            {
               double coef[3] = {climbDirection * -(aTarget.mAltitude - aState.mAltitude) / sinFlightPathAngle,
                                 initialSpeed,
                                 linearAcceleration / 2.0};
               UtPolynomial::SolveQuadricMinPos(coef, aBreakpoint.mSimTime);
            }
         }
         else
         {
            aBreakpoint.mFlightAngle = 0.0;
            aBreakpoint.mSimTime     = altitudeChange / aBreakpoint.mClimbRate;
         }
      }
      else
      {
         if (timeAtClimbRateConstraint > 1.0E-3)
         {
            // bound by climb rate, stop when not bound by climb rate
            aBreakpoint.mFlightAngle = 0.0;
            double timeAtAltitude    = fabs(altitudeChange / aBreakpoint.mClimbRate);
            aBreakpoint.mSimTime     = std::min(timeAtAltitude, timeAtClimbRateConstraint);
         }
         else
         {
            // bound by flight angle, stop when altitude is reached
            aBreakpoint.mClimbRate = 0.0;
            double sinFlightAngle  = sinFlightPathAngle;
            double coef[3] = {-fabs(altitudeChange), initialSpeed * sinFlightAngle, acceleration * sinFlightAngle / 2.0};
            if (!UtPolynomial::SolveQuadricMinPos(coef, aBreakpoint.mSimTime))
            {
               aBreakpoint.mSimTime = UtMath::cDOUBLE_MAX;
            }
         }
      }


      if (aBreakpoint.mSimTime > timeSpeedReached)
      {
         aBreakpoint.mSimTime = timeSpeedReached;
      }
   }
   else if (isSpeedChange)
   {
      double timeSpeedReached         = fabs((aTarget.mSpeed - initialSpeed) / mConstraints.mMaxLinearAccel);
      double accelerationDirection    = UtMath::Sign(aTarget.mSpeed - initialSpeed);
      double acceleration             = mConstraints.mMaxLinearAccel * accelerationDirection;
      aBreakpoint.mLinearAcceleration = acceleration;
      aBreakpoint.mClimbRate          = 0.0;
      aBreakpoint.mFlightAngle        = 0.0;
      aBreakpoint.mSimTime            = timeSpeedReached;
   }
   else if (isAltitudeChange)
   {
      double climbDirection             = UtMath::Sign(altitudeChange);
      double speedAtClimbRateConstraint = mConstraints.mMaxClimbRate / sinFlightPathAngle;
      if (initialSpeed >= speedAtClimbRateConstraint)
      {
         aBreakpoint.mClimbRate   = mConstraints.mMaxClimbRate * climbDirection;
         aBreakpoint.mFlightAngle = 0.0;
         aBreakpoint.mSimTime     = altitudeChange / aBreakpoint.mClimbRate;
      }
      else if (initialSpeed > 0.0)
      {
         aBreakpoint.mFlightAngle = mConstraints.mMaxFlightPathAngle * climbDirection;
         aBreakpoint.mClimbRate   = 0.0;
         aBreakpoint.mSimTime     = climbDirection * altitudeChange / (initialSpeed * sinFlightPathAngle);
      }
      aBreakpoint.mLinearAcceleration = 0.0;
   }
   else
   {
      hasBreakpoint = false;
   }
   if (aBreakpoint.mSimTime < 0.01)
   {
      hasBreakpoint = false;
   }
   return hasBreakpoint;
}

// ================================================================================================
//! Get the turn radius calculated based on current path constraints.
//! @param aSpeed Speed to use in turn radius calculations. (m/s)
//! @return The turn radius. (meters) (default = 0.0 m)
double WsfPathComputer::GetTurnRadius(double aSpeed)
{
   // For each valid (non-zero) constraint, calculate the turn radius
   // and then select the most critical (largest non-zero) one:
   double radius = 0.0;

   if (mConstraints.mTurnRateLimit > 0.0)
   {
      radius = std::max(radius, aSpeed / mConstraints.mTurnRateLimit);
   }

   // Roll angle limit or bank angle limit are used to constrain the max radial accel
   if (mConstraints.mMaxRadialAccel > 0.0)
   {
      radius = std::max(radius, aSpeed * aSpeed / mConstraints.mMaxRadialAccel);
   }

   if (mConstraints.mMaxTurnRate > 0.0)
   {
      radius = std::max(radius, aSpeed / mConstraints.mMaxTurnRate);
   }

   if (mConstraints.mBodyVertLimit > UtEarth::cACCEL_OF_GRAVITY)
   {
      double radialAccel =
         mConstraints.mBodyVertLimit * sin(acos(UtEarth::cACCEL_OF_GRAVITY / mConstraints.mBodyVertLimit));
      radius = std::max(radius, aSpeed * aSpeed / radialAccel);
   }

   return radius;
}
