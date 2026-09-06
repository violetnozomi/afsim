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

#include "WsfMath3D_Mover.hpp"

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"

// =================================================================
WsfMath3D_Mover::WsfMath3D_Mover(WsfScenario& aScenario)
   : WsfRouteMover(aScenario)
   , mTurnRadius(0.0)
   , mMotion()
   , mGuidancePtr(nullptr)
   , mLocalUpdateTime(0.0)
{
   for (size_t i = 0; i < 3; ++i)
   {
      mPrepauseVelocityNED[i] = 0.0;
   }
   mGuidancePtr = new WsfPathGuidance();
}

// =================================================================
WsfMath3D_Mover::WsfMath3D_Mover(const WsfMath3D_Mover& aSrc)
   : WsfRouteMover(aSrc)
   , mTurnRadius(aSrc.mTurnRadius)
   , mMotion(aSrc.mMotion)
   , mGuidancePtr(aSrc.mGuidancePtr->Clone())
   , mLocalUpdateTime(aSrc.mLocalUpdateTime)
{
   for (size_t i = 0; i < 3; ++i)
   {
      mPrepauseVelocityNED[i] = aSrc.mPrepauseVelocityNED[i];
   }
}

// =================================================================
WsfMath3D_Mover::~WsfMath3D_Mover()
{
   delete mGuidancePtr;
}

// =================================================================
void WsfMath3D_Mover::SetExternalMotionState()
{
   GetPlatform()->SetLocationWCS(mMotion.LocWCS().GetData());
   GetPlatform()->SetVelocityNED(mMotion.VelNED().GetData());
   GetPlatform()->SetAccelerationWCS(mMotion.AccWCS().GetData());
   double heading, pitch, roll;
   mMotion.NED_Orientation(heading, pitch, roll);
   GetPlatform()->SetOrientationNED(heading, pitch, roll);
}

// =================================================================
// virtual
bool WsfMath3D_Mover::Initialize(double aSimTime)
{
   // The RouteMover initialize must occur before the mMotion Initialize,
   // or the route initial LLA will not be set properly.

   bool ok = WsfRouteMover::Initialize(aSimTime);

   if (ok)
   {
      ok = mMotion.Initialize(aSimTime, GetPlatform());
   }

   if (ok)
   {
      mTurnRadius = 100.0;
      RegisterProperty("turn_radius", mTurnRadius);
   }

   mLocalUpdateTime = aSimTime;

   return ok;
}

// =================================================================
// virtual
WsfMover* WsfMath3D_Mover::Clone() const
{
   return new WsfMath3D_Mover(*this);
}

// =================================================================
// virtual
bool WsfMath3D_Mover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   // First intercept commands that MUST be a particular value for this mover:
   if (command == "switch_on_approach")
   {
      // This is acceptable.  Do nothing.
   }
   else if (command == "switch_on_passing")
   {
      ut::log::warning() << "The 'switch_on_passing' keyword is not implemented by WSF_KINEMATIC_MOVER. Ignored.";
   }
   else if (command == "altitude_offset")
   {
      double offsetValueNotUsed;
      aInput.ReadValueOfType(offsetValueNotUsed, UtInput::cLENGTH);
      ut::log::warning() << "The 'altitude_offset' keyword is not implemented by WSF_KINEMATIC_MOVER. Ignored.";
   }
   else if (command == "on_turn_failure")
   {
      std::string other_option;
      aInput.ReadValue(other_option);
      ut::log::warning() << "The 'on_turn_failure' and sub-keyword " << other_option
                         << " are not implemented by WSF_KINEMATIC_MOVER. Ignored.";
   }
   else if (mMotion.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfRouteMover::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================
// virtual
void WsfMath3D_Mover::SetGuidanceTo(const WsfWaypoint& aNextWaypoint)
{
   // === "REACH FORWARD" Values

   // FLY-TO POINT
   double lat = aNextWaypoint.GetLat();
   double lon = aNextWaypoint.GetLon();
   double alt = 0.0;
   if (mGuidancePtr->AltIsSet())
   {
      alt = mGuidancePtr->GetAltitude();
   }
   if (aNextWaypoint.GetAlt() != WsfPath::cUSE_PREVIOUS)
   {
      alt = aNextWaypoint.GetAlt();
   }
   UtVec3dX locWCS;
   UtEntity::ConvertLLAToWCS(lat, lon, alt, locWCS.GetData());
   mMotion.SetFlyToPoint(locWCS);
}

// =================================================================
// virtual
void WsfMath3D_Mover::SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint)
{
   // === "REACH BACKWARD" Values

   double prevAlt = aPreviousWaypoint.GetAlt();
   if (prevAlt != WsfPath::cUSE_PREVIOUS)
   {
      mGuidancePtr->SetAltitude(prevAlt, WsfPath::cALT_REF_MSL);
   }

   // Acceleration Limits
   if (aPreviousWaypoint.GetLinearAccel() != WsfPath::cUSE_PREVIOUS)
   {
      double linearAccel = aPreviousWaypoint.GetLinearAccel();
      mMotion.SetMaxLinearAcceleration(fabs(linearAccel));
   }

   if (aPreviousWaypoint.GetRadialAccel() != WsfPath::cUSE_PREVIOUS)
   {
      double radialAccel = aPreviousWaypoint.GetRadialAccel();
      mMotion.SetMaxNormalAcceleration(fabs(radialAccel));
   }

   if (aPreviousWaypoint.GetSpeed() != WsfPath::cUSE_PREVIOUS)
   {
      double speed = aPreviousWaypoint.GetSpeed();
      mGuidancePtr->SetSpeed(speed);
      mMotion.SetTargetSpeed(speed);
   }
}

// =================================================================
// virtual
double WsfMath3D_Mover::TurnRadius()
{
   return mMotion.TurnRadius();
}

// =================================================================
// virtual
double WsfMath3D_Mover::TurnRadius(double aSpeed)
{
   // not implemented.  Quiet warning.
   return WsfRouteMover::TurnRadius(aSpeed);
}

// =================================================================
// virtual
bool WsfMath3D_Mover::Unpause(double aSimTime, bool aInterrupt)
{
   // Set velocity back to pre-Pause value:
   if (IsPaused())
   {
      GetPlatform()->SetVelocityNED(mPrepauseVelocityNED);
      mUnpauseTime = 0.0;
      return true;
   }
   return false;
}

// =================================================================
// virtual
void WsfMath3D_Mover::Pause(double aSimTime, double aUnpauseTime)
{
   // This function copied and pasted from WsfWaypointMover,
   // as it was not in our inheritance tree.
   mUnpauseTime = aUnpauseTime;
   GetPlatform()->GetVelocityNED(mPrepauseVelocityNED);
   double zero[3] = {0, 0, 0};
   GetPlatform()->SetVelocityWCS(zero);
   GetPlatform()->SetAccelerationWCS(zero);
   DisableAndClearUnpause();
   if (mUnpauseTime > aSimTime && mUnpauseTime != UtMath::cDOUBLE_MAX)
   {
      AddUnpauseEventFor(this, aUnpauseTime);
   }
}

// =================================================================
// virtual
void WsfMath3D_Mover::ResetPosition(double aSimTime)
{
   mMotion.Initialize(aSimTime, GetPlatform());
}

// =================================================================
// virtual
void WsfMath3D_Mover::AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr)
{
   const WsfWaypoint& wp = aTarget;

   if ((wp.GetSwitch() == WsfPath::cSWITCH_ON_TIME) || (wp.GetPointType() & WsfWaypoint::cTIME))
   {
      assert(wp.GetDistanceOrTime() != WsfPath::cDOUBLE_NOT_SET);
      assert(wp.GetDistanceOrTime() != 0.0);
      mGuidancePtr->SetSegmentEndTime(wp.GetDistanceOrTime());
   }
   else if ((wp.GetSwitch() == WsfPath::cSWITCH_ON_DISTANCE) || (wp.GetPointType() & WsfWaypoint::cDISTANCE))
   {
      assert(wp.GetDistanceOrTime() != WsfPath::cDOUBLE_NOT_SET);
      assert(wp.GetDistanceOrTime() != 0.0);
      mGuidancePtr->SetSegmentEndDist(wp.GetDistanceOrTime());
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
   }
}

// =================================================================
// virtual
bool WsfMath3D_Mover::GoToAltitude(double aSimTime,
                                   double aAltitude,
                                   double aClimbDiveRate, // ignored
                                   bool   aKeepRoute)       // ignored
{
   // Implementation:
   // Take the current fly-to point, and set it to the same
   // NED location as before, but at the desired altitude.
   UtVec3dX flyToPoint = mMotion.GetFlyToPoint();
   double   lat, lon, alt;
   UtEntity::ConvertWCSToLLA(flyToPoint.GetData(), lat, lon, alt);
   alt = aAltitude;
   UtEntity::ConvertLLAToWCS(lat, lon, alt, flyToPoint.GetData());
   mMotion.SetFlyToPoint(flyToPoint.GetData());
   return true;
}

// =================================================================
// virtual
bool WsfMath3D_Mover::GoToSpeed(double aSimTime,
                                double aSpeed,
                                double aLinearAccel, // ignored
                                bool   aKeepRoute)     // ignored
{
   mMotion.SetTargetSpeed(aSpeed);
   return true;
}

// =================================================================
// virtual
bool WsfMath3D_Mover::TurnToRelativeHeading(double                 aSimTime,
                                            double                 aHeadingChange,
                                            double                 aRadialAccel,   // ignored
                                            WsfPath::TurnDirection aTurnDirection) // ignored
{
   // Implementation:
   // Take the current fly-to point range and altitude, and re-set it
   // to the desired heading from the current platform position.
   // TREAT THE HEADING CHANGE AS A COURSE CHANGE.
   UtVec3dX flyToPoint = mMotion.GetFlyToPoint();
   UtVec3dX currentLoc = mMotion.LocWCS();
   UtDCM    xFrm       = mMotion.WCStoNED();
   UtVec3dX offsetNED  = (flyToPoint - currentLoc) * xFrm;
   double   grndRange  = offsetNED.MagXY();
   double   course     = mMotion.CourseNED();
   double   newCourse  = course + aHeadingChange;
   // Re-Use both offsetNED and flyToPoint with new values
   offsetNED.Set(grndRange * std::cos(newCourse), grndRange * std::sin(newCourse), offsetNED[2]);
   flyToPoint = currentLoc + offsetNED * xFrm.Transpose();
   mMotion.SetFlyToPoint(flyToPoint.GetData());
   return true;
}

// =================================================================
// virtual
bool WsfMath3D_Mover::TurnToHeading(double                 aSimTime,
                                    double                 aHeading,
                                    double                 aRadialAccel,   // ignored
                                    WsfPath::TurnDirection aTurnDirection) // ignored
{
   // Implementation:
   // Take the current fly-to point, and set it 10 km out in range with
   // the same altitude as before, at the desired course/heading.
   UtVec3dX flyToPoint = mMotion.GetFlyToPoint();
   UtVec3dX locWCS     = mMotion.LocWCS();
   UtDCM    wcsToNed   = mMotion.WCStoNED();
   UtVec3dX offsetNED  = (flyToPoint - locWCS) * wcsToNed;
   double   grndRng    = 10000;
   // Re-Use both offsetNED and flyToPoint with new values:
   offsetNED.Set(grndRng * std::cos(aHeading), grndRng * std::sin(aHeading), offsetNED[2]);
   flyToPoint = (locWCS + offsetNED * wcsToNed.Transpose());
   mMotion.SetFlyToPoint(flyToPoint.GetData());
   return true;
}

// =================================================================
// virtual
void WsfMath3D_Mover::UpdatePosition(double aSimTime)
{
   if (aSimTime > mLocalUpdateTime)
   {
      if (IsPaused())
      {
         mMotion.SetLastUpdateTime(aSimTime);
         SetLastUpdateTime(aSimTime);
      }
      else
      {
         mMotion.Update(aSimTime);
         mTurnRadius = mMotion.TurnRadius();

         if (mMotion.FlyToPointReached(aSimTime))
         {
            // Inform parent class of reaching the desired waypoint.
            WaypointReached(aSimTime);
         }

         SetExternalMotionState();
         SetLastUpdateTime(aSimTime);
      }
      mLocalUpdateTime = aSimTime;
   }
}
