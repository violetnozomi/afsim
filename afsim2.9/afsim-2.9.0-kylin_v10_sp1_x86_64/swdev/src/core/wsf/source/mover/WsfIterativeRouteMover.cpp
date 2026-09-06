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

#include "WsfIterativeRouteMover.hpp"

#include <iomanip>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfPathGuidance.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

WsfIterativeRouteMover::WsfIterativeRouteMover(WsfScenario& aScenario)
   : WsfRouteMover(aScenario)
   , mIsOnGround(false)
   , mIsOnRoad(false)
   , mPitchEnable(true)
   , mRecursionLevel(0)
   , mHeightOfTerrain(0.0)
   , mCumSegmentDist(0.0)
   , mCumSegmentTime(0.0)
   , mMinTurnRadius(100.0)
   , mSwitchRadius(0.0)
   , mLastForwardOffset(0.0)
   , mNextSegHeading(0.0)
   , mNextSegRadialAccel(0.0)
   , mGuidancePtr(nullptr)
{
   mConverging[0] = false;
   mConverging[1] = false;
   mConverging[2] = false;
}

WsfIterativeRouteMover::WsfIterativeRouteMover(const WsfIterativeRouteMover& aSrc)
   : WsfRouteMover(aSrc)
   , mIsOnGround(aSrc.mIsOnGround)
   , mIsOnRoad(aSrc.mIsOnRoad)
   , mPitchEnable(aSrc.mPitchEnable)
   , mRecursionLevel(aSrc.mRecursionLevel)
   , mHeightOfTerrain(0.0)
   , mCumSegmentDist(aSrc.mCumSegmentDist)
   , mCumSegmentTime(aSrc.mCumSegmentTime)
   , mMinTurnRadius(aSrc.mMinTurnRadius)
   , mSwitchRadius(aSrc.mSwitchRadius)
   , mLastForwardOffset(aSrc.mLastForwardOffset)
   , mNextSegHeading(aSrc.mNextSegHeading)
   , mNextSegRadialAccel(aSrc.mNextSegRadialAccel)
   , mGuidancePtr(aSrc.mGuidancePtr)
{
   mConverging[0] = aSrc.mConverging[0];
   mConverging[1] = aSrc.mConverging[1];
   mConverging[2] = aSrc.mConverging[2];
   if (mGuidancePtr != nullptr)
   {
      mGuidancePtr = mGuidancePtr->Clone();
      mGuidancePtr->SetMover(this);
   }
}

// virtual
WsfIterativeRouteMover::~WsfIterativeRouteMover()
{
   delete mGuidancePtr;
}

// virtual
bool WsfIterativeRouteMover::Initialize(double aSimTime)
{
   assert(GetPlatform() != nullptr);

   if (!WsfRouteMover::Initialize(aSimTime))
   {
      return false;
   }
   ClearConverging();
   mLastUpdateTime = aSimTime;
   mRecursionLevel = 0;
   return true;
}

// virtual
bool WsfIterativeRouteMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (WsfRouteMover::ProcessInput(aInput))
   {
   }
   else if ((command == "no_pitch") || (command == "pitch_disable"))
   {
      mPitchEnable = false;
   }
   else if (command == "pitch_enable")
   {
      mPitchEnable = true;
   }
   else if (command == "on_ground")
   {
      mIsOnGround = true;
   }
   else if ((command == "airborne") || (command == "in_air"))
   {
      mIsOnGround = false;
   }
   else if (command == "on_road")
   {
      mIsOnRoad = true;
   }
   else if (command == "off_road")
   {
      mIsOnRoad = false;
   }
   else if ((mGuidancePtr != nullptr) && (mGuidancePtr->ProcessInput(aInput)))
   {
      // Do nothing
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfIterativeRouteMover::UpdatePosition(double aSimTime)
{
   ++mRecursionLevel;
   if (IsPaused())
   {
   }
   else if (IsStopped())
   {
      mLastUpdateTime = aSimTime;
   }
   else if ((aSimTime - mLastUpdateTime) > mUpdateTimeTolerance)
   {
      // Move along the route as far as possible
      bool advanceToNextWaypoint = false;

      AdvanceTime(aSimTime, advanceToNextWaypoint);
      if (advanceToNextWaypoint)
      {
         WaypointReached(aSimTime);
      }

      if ((mLastUpdateTime + mUpdateTimeTolerance) < aSimTime)
      {
         // Recursion.  Continue updating until we've met aSimTime
         Update(aSimTime);
      }
   }

   --mRecursionLevel;
}

void WsfIterativeRouteMover::AdvanceTime(double aSimTime, bool& aAdvanceToNextWaypoint)
{
   bool movedAtAll = false;
   bool moved      = true;
   while ((moved) && (aSimTime > mLastUpdateTime) && (!aAdvanceToNextWaypoint))
   {
      // This while loop treatment is necessary, since the derived
      // WsfMover implementation may choose to break a large time step
      // up into many smaller ones.
      LocalUpdate(aSimTime, moved, aAdvanceToNextWaypoint);
      if (moved)
      {
         movedAtAll = true;
      }
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Iterative route mover advanced time." << std::boolalpha;
         out.AddNote() << "Moved: " << moved;
         out.AddNote() << "Advance: " << aAdvanceToNextWaypoint;
      }
   }
   if (movedAtAll)
   {
      PostMoveUpdate(mLastUpdateTime);
   }
}

void WsfIterativeRouteMover::PostMoveUpdate(double aSimTime)
{
   CalcAltitudesAndAttitude(aSimTime);
}

void WsfIterativeRouteMover::Pause(double aSimTime, double aUnpauseTime)
{
   mUnpauseTime = aUnpauseTime;
   AddUnpauseEventFor(this, aUnpauseTime);
}

void WsfIterativeRouteMover::SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint)
{
   if (!mGuidancePtr->AltIsSet() && aPreviousWaypoint.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
   {
      mGuidancePtr->SetAltitude(aPreviousWaypoint.GetAlt(), aPreviousWaypoint.GetAltRef());
   }

   if (aPreviousWaypoint.GetLinearAccel() > 0.0F)
   {
      mGuidancePtr->SetAxialAccel(aPreviousWaypoint.GetLinearAccel());
   }

   if (aPreviousWaypoint.GetClimbRate() > 0.0F)
   {
      mGuidancePtr->SetClimbRate(aPreviousWaypoint.GetClimbRate());
   }

   if (aPreviousWaypoint.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      mGuidancePtr->SetSpeed(aPreviousWaypoint.GetSpeed());
   }

   mGuidancePtr->SetRadialAccel(aPreviousWaypoint.GetRadialAccel());

   // If this point was 'turn on approach', the turn was initiated prior to actually reaching
   // this physical location. The computations assume that the shortest turn will be taken,
   // so if turn on approach was specified, set the turn direction to shortest.
   if (aPreviousWaypoint.GetSwitch() == WsfPath::cSWITCH_ON_APPROACH)
   {
      mGuidancePtr->SetTurnDir(WsfPath::cTURN_DIR_SHORTEST);
   }
}

void WsfIterativeRouteMover::SetGuidanceTo(const WsfWaypoint& aNextWaypoint)
{
   if (aNextWaypoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
   {
      double alt = GetPlatform()->GetAltitude();
      mGuidancePtr->SetLLA(aNextWaypoint.GetLat(), aNextWaypoint.GetLon(), alt, WsfPath::cALT_REF_MSL);
   }

   if (aNextWaypoint.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
   {
      mGuidancePtr->SetAltitude(aNextWaypoint.GetAlt(), GetAltRef(), aNextWaypoint.GetAltRef());
   }

   // Does not change Speed request (keeps old one) unless specified...
   if (aNextWaypoint.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      mGuidancePtr->SetSpeed(static_cast<double>(aNextWaypoint.GetSpeed()));
   }
}

void WsfIterativeRouteMover::SetConverging(bool aValue)
{
   mConverging[2] = mConverging[1];
   mConverging[1] = mConverging[0];
   mConverging[0] = aValue;
}

void WsfIterativeRouteMover::ClearConverging()
{
   mConverging[2] = false;
   mConverging[1] = false;
   mConverging[0] = false;
}

void WsfIterativeRouteMover::ResetWPSwitching()
{
   ClearConverging();
   mSwitchRadius      = 0.0;
   mLastForwardOffset = 0.0;
   mCumSegmentTime    = 0.0;
   mCumSegmentDist    = 0.0;

   // Clear turn_on_approach guidance
   mNextSegHeading     = 0.0;
   mNextSegRadialAccel = 0.0;
}

double WsfIterativeRouteMover::SegmentDistRemaining() const
{
   double result = 0.0;
   if (mGuidancePtr->SegmentDistIsSet())
   {
      result = mGuidancePtr->GetSegmentEndDist() - mCumSegmentDist;
   }
   return result;
}

double WsfIterativeRouteMover::SegmentTimeRemaining() const
{
   double result = 0.0;
   if (mGuidancePtr->SegmentTimeIsSet())
   {
      result = mGuidancePtr->GetSegmentEndTime() - mCumSegmentTime;
   }
   return result;
}

void WsfIterativeRouteMover::AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr)
{
   // When traversing a path from WsfWaypoint A to B, some of the guidance values flown are
   // provided by the current (passed) waypoint, and some by the target (future) waypoint.
   // This extracts values from the future point for guidance toward it...
   // Note that we normally don't change a previous target value for speed, altitude, etc.,
   // unless we get a newer one to override the old one with.

   // Get a clean start toward the next waypoint...
   ResetWPSwitching();

   const WsfWaypoint& wp = aTarget;

   if ((wp.GetSwitch() == WsfPath::cSWITCH_ON_TIME) || (wp.GetPointType() & WsfWaypoint::cTIME))
   {
      assert(wp.GetDistanceOrTime() != WsfPath::cDOUBLE_NOT_SET);
      assert(wp.GetDistanceOrTime() != 0.0F);
      mGuidancePtr->SetSegmentEndTime(static_cast<double>(wp.GetDistanceOrTime()));
   }
   else if ((wp.GetSwitch() == WsfPath::cSWITCH_ON_DISTANCE) || (wp.GetPointType() & WsfWaypoint::cDISTANCE))
   {
      assert(wp.GetDistanceOrTime() != WsfPath::cDOUBLE_NOT_SET);
      assert(wp.GetDistanceOrTime() != 0.0F);
      mGuidancePtr->SetSegmentEndDist(static_cast<double>(wp.GetDistanceOrTime()));
   }

   if (wp.GetPointType() & WsfWaypoint::cHEADING_MASK)
   {
      assert(wp.GetHeading() != WsfPath::cDOUBLE_NOT_SET);
      if (wp.GetPointType() & WsfWaypoint::cABSOLUTE_HEADING)
      {
         mGuidancePtr->SetHeading(wp.GetHeading(), WsfPath::cHDG_REF_NORTH);
      }
      else
      {
         mGuidancePtr->SetHeading(wp.GetHeading(), WsfPath::cHDG_REF_RELATIVE);
      }
      mGuidancePtr->SetTurnDir(wp.GetTurnDirection());
   }

   if (wp.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
   {
      // Setting the altitude will be done below.
      mGuidancePtr->SetLatLon(wp.GetLat(), wp.GetLon());

      // This triggers waypoint switching as desired.
      mGuidancePtr->SetSwitch(GetSwitch(), wp.GetSwitch());

      // If the target point is 'switch_on_approach' (commonly known as 'turn-short', calculate
      // the distance before the waypoint at which to start the turn.

      InitializeTurnOnApproach(aTarget, aNextPointPtr);
   }
}

// private
void WsfIterativeRouteMover::InitializeTurnOnApproach(const WsfWaypoint& aTarget, const WsfWaypoint* aNextTarget)
{
   // Assume not 'switch on approach'.
   mNextSegHeading     = 0.0;
   mNextSegRadialAccel = 0.0;

   const WsfWaypoint& wp = aTarget;
   if (wp.GetSwitch() != WsfPath::cSWITCH_ON_APPROACH)
   {
      return;
   }

   // There are a couple of situations where if switch_on_approach will not be honored.
   //
   // *) If the target index is the last point in the route.
   // *) The target index and the following index are determined to be the same (a one point orbit).
   // *) If the terminal points of the next segment are not both lat/lon points.
   //
   // In either of these cases we must actually disable the switch_on_approach because it will cause
   // incorrect behavior. In the first case it will not make it to the target point because the mover
   // will compute a lead point and declare a waypoint switch when it gets to the lead point. In the
   // second case it will get into an infinite loop!

   bool turnShortIsOk = false;
   if (aNextTarget != nullptr && aNextTarget != &aTarget && // not a one-point orbit
       (aTarget.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) &&
       (aNextTarget->GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE))
   {
      turnShortIsOk = true;

      double distNotUsed;
      UtSphericalEarth::GreatCircleHeadingAndDistance(aTarget.GetLat(),
                                                      aTarget.GetLon(),
                                                      aNextTarget->GetLat(),
                                                      aNextTarget->GetLon(),
                                                      mNextSegHeading,
                                                      distNotUsed);
      mNextSegHeading = UtMath::NormalizeAngle0_TwoPi(mNextSegHeading * UtMath::cRAD_PER_DEG);

      // Compute the radial acceleration to be used for the turn to start the next segment.
      mNextSegRadialAccel = GetGuidance()->GetRadialAccelAtPoint(aTarget);

      // Turning short also disables explicit turn direction.
      GetGuidance()->SetTurnDir(WsfPath::cTURN_DIR_SHORTEST);
   }

   // Cancel the switch_on_approach guidance if it is not acceptable.
   if (!turnShortIsOk)
   {
      GetGuidance()->SetSwitch(GetSwitch(), WsfPath::cSWITCH_ON_PASSING);
   }
}

//! Check for switching waypoints.  There are several possibilities, switch if we are within
//! one turn radius of our target point, switch after we are abreast of the target point,
//! switch when a certain period of time has elapsed, or after a specified distance is covered.
// virtual
bool WsfIterativeRouteMover::IsWaypointSwitchTime(double aSimTime)
{
   double savedSwitchRadius = mSwitchRadius;
   double forwardOffset     = 0.0;
   double rightOffset       = 0.0;
   double error             = 0.0;
   bool   goToNext          = false;

   WsfPath::Switch sw = GetGuidance()->GetSwitch();

   if (sw == WsfPath::cSWITCH_EXTERNAL)
   {
      return goToNext;
   }
   else if (sw == WsfPath::cSWITCH_ON_TIME)
   {
      assert(GetGuidance()->GetSegmentEndTime() > 0.0);
      error = GetGuidance()->GetSegmentEndTime() - CumSegmentTime();
      if (error <= 0.0)
      {
         goToNext = true;
      }
   }
   else if (sw == WsfPath::cSWITCH_ON_DISTANCE)
   {
      assert(GetGuidance()->GetSegmentEndDist() > 0.0);
      error = GetGuidance()->GetSegmentEndDist() - CumSegmentDist();
      if (error <= 0.0)
      {
         goToNext = true;
      }
   }
   else if (GetGuidance()->LatLonIsSet())
   {
      static const double cCLOSE = 250.0; // Assume this is "close"

      // Convert the target waypoint latitude/longitude to NED coordinates.
      // If navigation errors are present then we use the perceived location and velocity for
      // making waypoint switch decisions.

      WsfNavigationErrors* navErrorPtr  = GetPlatform()->GetComponent<WsfNavigationErrors>();
      UtEntity*            refEntityPtr = GetPlatform();
      if ((navErrorPtr != nullptr) && (navErrorPtr->GetGPS_Status() < 0))
      {
         refEntityPtr = &(navErrorPtr->GetPerceivedEntity());
      }
      double locNED[3];
      refEntityPtr->ConvertLLAToNED(GetGuidance()->GetLatitude(),
                                    GetGuidance()->GetLongitude(),
                                    0.0, // The altitude is not used.
                                    locNED);

      double velNED[3];
      refEntityPtr->GetVelocityNED(velNED);
      double heading      = 0.0;
      double horizSpeed_2 = (velNED[0] * velNED[0]) + (velNED[1] * velNED[1]);
      if (horizSpeed_2 > 1.0)
      {
         heading = atan2(velNED[1], velNED[0]);
      }
      forwardOffset  = cos(heading) * locNED[0] + sin(heading) * locNED[1];
      rightOffset    = cos(heading) * locNED[1] - sin(heading) * locNED[0];
      double bearing = atan2(rightOffset, forwardOffset);

      // TODO-JAJ WsfWaypointMover has its own mechanism for determining when to switch waypoints (see
      // TODO-JAJ WsfWaypointMover::IsWaypointMoverSwitchTime). These should probably be integrated.

      bool close = false;
      if (sw == WsfPath::cSWITCH_ON_APPROACH)
      {
         double turnLeadDist = GetTurnLeadDistance(sqrt(horizSpeed_2), heading);

         // Adjust the forward offset to the desired point by the lead amount for the turn.
         // Don't mess with the right offset...
         forwardOffset -= turnLeadDist;
         error = forwardOffset;
         close = (forwardOffset < cCLOSE);

         // If the waypoint we are coming from is too close to the target waypoint then the 'convergence'
         // flag will never get set. The absolute value of the forward offset keeps growing (the point
         // where we wanted to start the turn is behind us!) and therefore the recomputed value of the
         // switch radius keeps growing.

         if (forwardOffset < 0.0)
         {
            SetConverging(true);
         }
         else
         {
            SetConverging(false);
         }
      }
      else // if (sw == cSWITCH_ON_PASSING)
      {
         error         = forwardOffset;
         mSwitchRadius = sqrt(forwardOffset * forwardOffset + rightOffset * rightOffset);
         close         = (mSwitchRadius < cCLOSE);

         if (mSwitchRadius < savedSwitchRadius)
         {
            SetConverging(true);
         }
         else
         {
            SetConverging(false);
         }
      }

      if (close)
      {
         if (GetConverging())
         {
            if (error <= 0)
            {
               goToNext = true;
            }
            else if (fabs(bearing) > UtMath::cPI_OVER_2)
            {
               // We passed a waypoint without catching it.
               // Declare victory and go on.
               goToNext = true;
            }
         }

         // When using navigation errors or if the mover is 'jerky', the standard waypoint switch test may fail.
         // Therefore, we will the waypoint was in front of me on the previous pass but is now behind me.
         if (!goToNext)
         {
            if ((forwardOffset < 0.0) && (mLastForwardOffset >= 0.0))
            {
               goToNext = true;
            }
         }
      }
      mLastForwardOffset = forwardOffset;
   }

   return goToNext;
}

//! Get the distance from the waypoint for which a 'turn_short' (cSWITCH_ON_APPROACH) should be initiated.
//! This should only be called when the target waypoint is of type cLATITUDE_AND_LONGITUDE and specifies
//! cSWITCH_ON_APPROACH.
double WsfIterativeRouteMover::GetTurnLeadDistance(double aCurrentSpeed, double aCurrentHeading)
{
   double leadDistance = GetMinTurnRadius();
   if (mNextSegRadialAccel > 0.0)
   {
      // Compute the heading change required ...
      double curHeading    = UtMath::NormalizeAngle0_TwoPi(aCurrentHeading);
      double headingChange = UtMath::NormalizeAngleMinusPi_Pi(mNextSegHeading - curHeading);

      // Watch for a heading change of +/- 180 deg.
      double absLimit = 0.99 * UtMath::cPI;
      headingChange   = std::max(std::min(headingChange, absLimit), -absLimit);

      // Compute the turn radius to be used. The target speed is used rather than the current speed
      // because we may be a long ways away and may be accelerating or decelerating towards the point.
      double targetSpeed = aCurrentSpeed;
      if (GetGuidance()->SpeedIsSet())
      {
         targetSpeed = GetGuidance()->GetSpeed();
      }
      double turnRadius = (targetSpeed * targetSpeed) / mNextSegRadialAccel; // R = v^2 / a

      // And finally get the lead distance required.
      leadDistance = fabs(turnRadius * tan(headingChange * 0.5));
   }
   return leadDistance;
}


// protected
//! Update the height of terrain and height above terrain values, and re-orient (if needed).
void WsfIterativeRouteMover::CalcAltitudesAndAttitude(double /*aSimTime*/)
{
   double lat, lon;
   double altMsl;
   GetPlatform()->GetLocationLLA(lat, lon, altMsl);

   if (mTerrainPtr->IsEnabled())
   {
      float hot;
      mTerrainPtr->GetElevInterp(lat, lon, hot);
      mHeightOfTerrain = (double)hot;
   }
   else
   {
      mHeightOfTerrain = 0.0F;
   }

   if (mIsOnGround)
   {
      // Height above terrain (HAT) is constant between waypoints.
      altMsl = mHeightOfTerrain;
      GetPlatform()->SetLocationLLA(lat, lon, altMsl);
      mGuidancePtr->SetAltitude(altMsl, WsfPath::cALT_REF_MSL);
      SetGroundAttitude(lat, lon);
   }
}

// protected
//! Update the orientation of the platform to conform to local ground slope.
//! It is assumed this method will _not_ be called unless already on ground.
void WsfIterativeRouteMover::SetGroundAttitude(double aLatitude, double aLongitude)
{
   // Construct a local coordinate system with the proper orientation.
   double heading, pitch, roll;
   GetPlatform()->GetOrientationNED(heading, pitch, roll);

   // Start by getting the unit normal to the local terrain.
   if (mIsOnRoad && (!mPitchEnable))
   {
      // Force pitch and roll both to zero:
      pitch = 0.0;
      roll  = 0.0;
   }
   else if (mTerrainPtr->IsEnabled())
   {
      double sinHeading = sin(heading);
      double cosHeading = cos(heading);

      double localX[3] = {cosHeading, sinHeading, 0.0};
      double localY[3];
      double localZ[3]; // Note: this should be pointing up (-Z)
      mTerrainPtr->GetNormalNED(aLatitude, aLongitude, localZ);
      UtVec3d::Multiply(localZ, -1.0);

      UtVec3d::CrossProduct(localY, localZ, localX); // tentative Y = Z cross tentative X
      UtVec3d::CrossProduct(localX, localY, localZ);
      UtVec3d::Normalize(localX);
      UtVec3d::Normalize(localY);

      // Extract the local orientation
      heading = atan2(localX[1], localX[0]);
      pitch   = -atan2(localX[2], sqrt(localX[0] * localX[0] + localX[1] * localX[1]));
      roll    = atan2(localY[2], localZ[2]);

      if (mIsOnRoad)
      {
         roll = 0.0;
      }

      if (!mPitchEnable)
      {
         pitch = 0.0;
      }
   }
   GetPlatform()->SetOrientationNED(heading, pitch, roll);
}

// virtual
bool WsfIterativeRouteMover::GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute)
{
   mGuidancePtr->SetPauseTime(0.0);
   mGuidancePtr->SetAltitude(aAltitude, GetAltRef());
   mGuidancePtr->SetClimbRate(aClimbDiveRate);
   // do not have the mover start extrapolation on the current heading (it erases the commanded heading)
   // mGuidancePtr->TriggerExtrapolation();

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Iterative route mover commanded to go to altitude.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Altitude: " << aAltitude << " m";
      out.AddNote() << "Rate: " << aClimbDiveRate << " m/s";
   }

   return true;
}


// virtual
bool WsfIterativeRouteMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Iterative route mover commanded to go to location.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Latitude: " << aLatitude << " deg";
      out.AddNote() << "Longitude: " << aLongitude << " deg";
      out.AddNote() << "Altitude: " << aAltitude << " m";
   }
   WsfRoute route;
   route.Append(WsfWaypoint(aLatitude, aLongitude, aAltitude, mGuidancePtr->GetSpeed()));
   bool value = UpdateRoute(aSimTime, route);
   return value;
}


//! For this Mover only, any speed request is assumed a sea level equivalent speed,
//! and constant dynamic pressure is flown as the altitude increases or decreases.
// virtual
bool WsfIterativeRouteMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   mGuidancePtr->SetSpeed(aSpeed);
   mGuidancePtr->SetAxialAccel(aLinearAccel);
   // Here we have just changed the SPEED request, not the path flown

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Iterative route mover commanded to go to speed.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Speed: " << aSpeed << " m/s";
      out.AddNote() << "Linear Accel: " << aLinearAccel << " m/s^2";
   }

   return true;
}

// virtual
bool WsfIterativeRouteMover::TurnToHeading(double                 aSimTime,
                                           double                 aHeading,
                                           double                 aRadialAccel,
                                           WsfPath::TurnDirection aTurnDirection)
{
   mGuidancePtr->SetPauseTime(0.0);
   mGuidancePtr->ClearValid(WsfPathGuidance::cLAT_LON);
   mGuidancePtr->SetHeading(aHeading, WsfPath::cHDG_REF_NORTH);
   mGuidancePtr->SetRadialAccel(aRadialAccel);
   mGuidancePtr->SetTurnDir(aTurnDirection);
   // do not have the mover start extrapolation on the current heading (it erases the commanded heading)
   // mGuidancePtr->TriggerExtrapolation();

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Iterative route mover commanded to turn to heading.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Heading: " << aHeading * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Radial Accel: " << aRadialAccel << " m/s^2";
      if (aTurnDirection == WsfPath::cTURN_DIR_RIGHT)
      {
         out.AddNote() << "Turn Direction: Right";
      }
      else if (aTurnDirection == WsfPath::cTURN_DIR_LEFT)
      {
         out.AddNote() << "Turn Direction: Left";
      }
      else if (aTurnDirection == WsfPath::cTURN_DIR_SHORTEST)
      {
         out.AddNote() << "Turn Direction: Shortest";
      }
   }

   return true;
}

// virtual
bool WsfIterativeRouteMover::TurnToRelativeHeading(double                 aSimTime,
                                                   double                 aHeadingChange,
                                                   double                 aRadialAccel,
                                                   WsfPath::TurnDirection aTurnDirection)
{
   double hdg, pitch, roll;
   GetPlatform()->GetOrientationNED(hdg, pitch, roll);
   return TurnToHeading(aSimTime, hdg + aHeadingChange, aRadialAccel, aTurnDirection);
}
