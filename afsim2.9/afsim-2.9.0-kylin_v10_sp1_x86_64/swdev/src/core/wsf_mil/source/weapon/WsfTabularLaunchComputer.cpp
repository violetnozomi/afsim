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

#include "WsfTabularLaunchComputer.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfDraw.hpp"
#include "WsfIntercept.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

namespace
{
// Allowable independent variable indices for table lookups.
enum
{
   cIV_LAUNCH_ALTITUDE, //<! Launcher altitude
   cIV_ALTITUDE,        //<! Altitude of target WRT to launcher
   cIV_ELEVATION,       //<! Elevation angle of target WRT to launcher
   cIV_SLANT_RANGE,     //<! Slant range from launcher to target
   cIV_GROUND_RANGE,    //<! Ground range from launcher to target
   cIV_COUNT            //<! Number of potential independent variables
};

UtTable::Parameters sTabularLaunchComputerParameters;
} // namespace

// =================================================================================================
WsfTabularLaunchComputer::WsfTabularLaunchComputer()
   : WsfLaunchComputer()
   , mTimeOfFlightPtr(nullptr)
   , mAverageSpeed(0.0)
   , mDropAdjustmentFactor(0.9)
   , mLauState()
   , mLauSpeed(0.0)
   , mTgtState()
   , mShowGraphics(false)
{
   if (sTabularLaunchComputerParameters.Empty())
   {
      // Register allowable independent variables.
      sTabularLaunchComputerParameters.AddRealParameter("launch_altitude", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE);
      sTabularLaunchComputerParameters.AddRealParameter("altitude", UtInput::cLENGTH, UtTable::NoCheck(), cIV_ALTITUDE);
      sTabularLaunchComputerParameters.AddRealParameter("elevation",
                                                        UtInput::cANGLE,
                                                        UtTable::ValueGE_LE(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                                        cIV_ELEVATION);
      sTabularLaunchComputerParameters.AddRealParameter("slant_range", UtInput::cLENGTH, UtTable::ValueGE(0.0), cIV_SLANT_RANGE);
      sTabularLaunchComputerParameters.AddRealParameter("ground_range",
                                                        UtInput::cLENGTH,
                                                        UtTable::ValueGE(0.0),
                                                        cIV_GROUND_RANGE);
   }
}

// =================================================================================================
WsfTabularLaunchComputer::WsfTabularLaunchComputer(const WsfTabularLaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mTimeOfFlightPtr(aSrc.mTimeOfFlightPtr)
   , mAverageSpeed(aSrc.mAverageSpeed)
   , mDropAdjustmentFactor(aSrc.mDropAdjustmentFactor)
   , mLauState()
   , mLauSpeed(0.0)
   , mTgtState()
   , mShowGraphics(aSrc.mShowGraphics)
{
}

// =================================================================================================
// virtual
WsfLaunchComputer* WsfTabularLaunchComputer::Clone() const
{
   return new WsfTabularLaunchComputer(*this);
}

// =================================================================================================
// virtual
bool WsfTabularLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "average_speed")
   {
      aInput.ReadValueOfType(mAverageSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mAverageSpeed, 0.0);
   }
   else if (command == "time_of_flight")
   {
      mTimeOfFlightPtr =
         UtTable::LoadInstance(aInput, UtInput::cTIME, UtTable::ValueGT(0.0), sTabularLaunchComputerParameters);
   }
   else if (command == "drop_adjustment_factor")
   {
      aInput.ReadValue(mDropAdjustmentFactor);
      aInput.ValueGreater(mDropAdjustmentFactor, 0.0);
   }
   else if (command == "show_graphics")
   {
      mShowGraphics = true;
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfTabularLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool ok = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   if (ComputerGenerationMode())
   {
      return ok;
   }

   if (mAverageSpeed <= 0.0)
   {
      ut::log::info() << "average_speed must be specified.";
      ok = false;
   }
   return ok;
}

// =================================================================================================
// virtual
double WsfTabularLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   InitializeResults(aSimTime, aTrack);

   UpdateLaunchPerception(aSimTime);
   if (UpdateTargetPerception(aSimTime, aTrack))
   {
      ComputeIntercept(aSimTime, aTrack, aLaunchDelayTime);
   }

   double timeToIntercept = cFOREVER;
   if (InterceptTimeIsValid())
   {
      timeToIntercept = mInterceptTime - aSimTime;
   }
   UpdateResults(aSimTime, timeToIntercept, aTrack);

   if (ShowResults())
   {
      std::ostringstream oss;
      auto               out = ut::log::info() << "Time to intercept results:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << mWeaponPtr->GetName();
      out.AddNote() << "Weapon Type: " << mWeaponPtr->GetType();

      oss << "T=" << aSimTime << ' ' << GetPlatform()->GetName() << '.' << mWeaponPtr->GetName() << " ("
          << mWeaponPtr->GetType() << ") Computed firing solution vs. ";
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
      if (targetPtr != nullptr)
      {
         oss << targetPtr->GetName() << " (" << targetPtr->GetType() << ")";
         out.AddNote() << "Target: " << targetPtr->GetName();
         out.AddNote() << "Target Type: " << targetPtr->GetType();
      }
      else
      {
         oss << "<UNKNOWN-TARGET>";
         out.AddNote() << "Target: <UNKNOWN-TARGET>"; // notes added to notes
      }
      if (InterceptTimeIsValid())
      {
         oss << "; TTI=" << mInterceptTime - aSimTime << " (T=" << mInterceptTime << ").";
         out.AddNote() << "TTI: " << mInterceptTime - aSimTime << " (T = " << mInterceptTime << ")";
      }
      else
      {
         oss << "; No solution.";
         out.AddNote() << "No solution.";
      }
      GetPlatform()->Comment(aSimTime, oss.str());
   }
   return timeToIntercept;
}

// =================================================================================================
// virtual
bool WsfTabularLaunchComputer::InitializeTTIData()
{
   return true; // No TTI data here... must return true to prevent base class failure.
}

// =================================================================================================
// protected
void WsfTabularLaunchComputer::ComputeIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   double lauLocWCS[3];
   mLauState.GetLocationWCS(lauLocWCS);

   double tgtLocWCS[3];
   mTgtState.GetLocationWCS(tgtLocWCS);
   double tgtVelWCS[3];
   mTgtState.GetVelocityWCS(tgtVelWCS);

   // Using an assumed average weapon velocity, compute the approximate intercept location and time-of-flight.

   double oldTTI      = -1.0;
   double oldAvgSpeed = -1.0;
   double newAvgSpeed = mAverageSpeed;
   double oldIntLocWCS[3];
   UtVec3d::Set(oldIntLocWCS, lauLocWCS);
   double newIntLocWCS[3];
   int    iterCount = 0;
   int    iterLimit = 5;
   bool   done      = false;
   while (!done)
   {
      ++iterCount;

      // Compute the intercept point based on the new guess of the average speed...
      double newTTI = WsfIntercept::Intercept(lauLocWCS, newAvgSpeed, tgtLocWCS, tgtVelWCS, newIntLocWCS);
      if (newTTI < 0.0)
      {
         break;
      }

      // Compute the changes in time-to-intercept and itercept point.
      double deltaTTI = fabs(newTTI - oldTTI);
      double deltaIntLocWCS[3];
      UtVec3d::Subtract(deltaIntLocWCS, oldIntLocWCS, newIntLocWCS);
      double deltaIntLoc = UtVec3d::Magnitude(deltaIntLocWCS);

      if (DebugEnabled())
      {
         double relLocWCS[3];
         UtVec3d::Subtract(relLocWCS, oldIntLocWCS, lauLocWCS);
         double oldIntLoc = UtVec3d::Magnitude(relLocWCS);
         UtVec3d::Subtract(relLocWCS, newIntLocWCS, lauLocWCS);
         double newIntLoc = UtVec3d::Magnitude(relLocWCS);

         auto out = ut::log::debug() << "ComputeIntercept():";
         out.AddNote() << "Iter: " << iterCount;

         { // RAII block
            auto logDebugNoteTint = out.AddNote() << "T_int:";
            logDebugNoteTint.AddNote() << "Old TTI: " << oldTTI;
            logDebugNoteTint.AddNote() << "New TTI: " << newTTI;
            logDebugNoteTint.AddNote() << "Delta TTI: " << deltaTTI;
         }
         { // RAII block
            auto logDebugNoteRint = out.AddNote() << "R_int:";
            logDebugNoteRint.AddNote() << "Old IntLoc: " << oldIntLoc;
            logDebugNoteRint.AddNote() << "New IntLoc: " << newIntLoc;
            logDebugNoteRint.AddNote() << "Delta IntLoc: " << deltaIntLoc;
         }
         { // RAII Block
            auto logDebugNoteVavg = out.AddNote() << "V_avg:";
            logDebugNoteVavg.AddNote() << "Old Avg Speed: " << oldAvgSpeed;
            logDebugNoteVavg.AddNote() << "New Avg Speed: " << newAvgSpeed;
         }
      }

      // Save the newly computed values as the current values...
      UtVec3d::Set(oldIntLocWCS, newIntLocWCS);
      oldTTI      = newTTI;
      oldAvgSpeed = newAvgSpeed;
      // Exit the iteration loop if necesary
      if ((deltaTTI < 2.0) ||       // Little change in time-to-itercept
          (deltaIntLoc < 100.0) ||  // Little change in intercept location
          (iterCount >= iterLimit)) // Iteration limit has been reached
      {
         done = true;
      }
      else
      {
         // Compute the average speed for the next iteration...
         newAvgSpeed = GetAverageSpeed(newIntLocWCS);
      }
   }

   // Use the last computed intercept location/time as the answer...

   double intLocWCS[3];
   UtVec3d::Set(intLocWCS, oldIntLocWCS);
   double intTTI = oldTTI;

   // If an intercept has been computed, adjust the intercept point to compenstate for the Earth's curvature.
   // (If a target is flying level, its instanteous velocity vector is tangent to the surface. Extrapolating
   // it would result in a greater altitude).

   // Compute the subtended angle between the initial target location and intercept location using the
   // definition of the dot product.

   double tgtLocUnitVecWCS[3];
   UtVec3d::Normalize(tgtLocUnitVecWCS, tgtLocWCS);
   double intLocUnitVecWCS[3];
   UtVec3d::Normalize(intLocUnitVecWCS, intLocWCS);
   double cosTheta = UtVec3d::DotProduct(tgtLocUnitVecWCS, intLocUnitVecWCS);
   if (cosTheta > 1.0E-4)
   {
      double intLat;
      double intLon;
      double intAlt;
      UtEntity::ConvertWCSToLLA(intLocWCS, intLat, intLon, intAlt);
      double deltaAlt = UtSphericalEarth::cEARTH_RADIUS * ((1.0 / cosTheta) - 1.0);
      intAlt          = std::max(intAlt - deltaAlt, 0.0);
      UtEntity::ConvertLLAToWCS(intLat, intLon, intAlt, intLocWCS);
   }

   // Compute the elevation to the intercept point and then compute the additional super-elevation to account
   // for gravitational drop during the flight.

   double intLocNED[3];
   mLauState.ConvertWCSToNED(intLocWCS, intLocNED);
   double Rs      = UtVec3d::Magnitude(intLocNED);                                   // slant range to intercept point
   double Rg      = sqrt(intLocNED[0] * intLocNED[0] + intLocNED[1] * intLocNED[1]); // approx ground range to intercept
   double intElev = atan2(-intLocNED[2], Rg);

   // Total drop assuming no drag.
   double dz = 0.5 * UtEarth::cACCEL_OF_GRAVITY * intTTI * intTTI;

   // Take only one-half the total drop because we assume we will follow a ballistic trajectory
   // (half up to the apogee, the other half coming down).
   dz *= 0.5;

   // Atmospheric drag causes a shallower trajectory. This is a simple compensation...
   dz *= mDropAdjustmentFactor;

   // Compute the super elevation angle and then adjust to account for the raw elevation angle.
   double superElev = atan2(dz, 0.5 * Rs);
   superElev *= std::abs(cos(intElev));

   double loftAngle = intElev + superElev;

   // Make sure loft angle is reasonable...
   if (loftAngle > 0.99 * UtMath::cPI_OVER_2)
   {
      loftAngle = 0.99 * UtMath::cPI_OVER_2;
   }
   else if (loftAngle < -0.99 * UtMath::cPI_OVER_2)
   {
      loftAngle = -0.99 * UtMath::cPI_OVER_2;
   }

   if (mShowGraphics)
   {
      ShowPoint(tgtLocWCS, 1.0, 0.0, 0.0, 15);
      ShowPoint(lauLocWCS, 0.0, 1.0, 0.0, 15);
      ShowPoint(intLocWCS, 1.0, 1.0, 1.0, 15);
      WsfDraw draw(*GetSimulation());
      draw.SetId(draw.GetNewId());
      draw.SetDuration(WsfDraw::cFOREVER);
      draw.SetColor(0.0, 1.0, 1.0);
      draw.BeginPolyline();
      draw.VertexWCS(lauLocWCS);
      draw.VertexWCS(intLocWCS);
      draw.End();
   }

   double lauBearing   = atan2(intLocNED[1], intLocNED[0]);
   double lauElevation = loftAngle;

   // Update the base class with the computed parameters...

   WsfLaunchComputer::SetInterceptPointWCS(intLocWCS);
   WsfLaunchComputer::SetInterceptTime(aSimTime + aLaunchDelayTime + oldTTI);
   WsfLaunchComputer::SetLaunchTime(aSimTime + aLaunchDelayTime);
   WsfLaunchComputer::SetLauncherBearing(lauBearing);
   WsfLaunchComputer::SetLauncherElevation(lauElevation);
   WsfLaunchComputer::SetLoftAngle(loftAngle);
}

// =================================================================================================
// protected
//! Get the average speed the weapon would fly to reach the specified target point.
//! @parameter aTgtLocWCS The location of the target point.
//! @returns   The estimated average speed.
double WsfTabularLaunchComputer::GetAverageSpeed(const double aTgtLocWCS[3])
{
   double arguments[cIV_COUNT]; // Arguments for table lookup

   double averageSpeed = mAverageSpeed;
   if (mTimeOfFlightPtr != nullptr)
   {
      // Compute values of independent variables that may be involved in the table lookup.
      double lat;
      double lon;
      mLauState.GetLocationLLA(lat, lon, arguments[cIV_LAUNCH_ALTITUDE]);
      UtEntity::ConvertWCSToLLA(aTgtLocWCS, lat, lon, arguments[cIV_ALTITUDE]);
      double tgtLocNED[3];
      mLauState.ConvertWCSToNED(aTgtLocWCS, tgtLocNED);
      double bearing;
      UtMeasurementUtil::RangeBearingElevation(tgtLocNED, arguments[cIV_SLANT_RANGE], bearing, arguments[cIV_ELEVATION]);
      double lauLocWCS[3];
      mLauState.GetLocationWCS(lauLocWCS);
      arguments[cIV_GROUND_RANGE] = UtMeasurementUtil::GroundRange(lauLocWCS, aTgtLocWCS);
      double timeOfFlight         = mTimeOfFlightPtr->Lookup(arguments);
      if (timeOfFlight > 0.0)
      {
         averageSpeed = arguments[cIV_SLANT_RANGE] / timeOfFlight;
      }
   }
   return averageSpeed;
}

// =================================================================================================
// protected
void WsfTabularLaunchComputer::ShowPoint(const double aLocWCS[3],
                                         double       aRedValue,
                                         double       aGreenValue,
                                         double       aBlueValue,
                                         int          aPointSize)
{
   WsfDraw draw(*GetSimulation());
   draw.SetId(draw.GetNewId());
   draw.SetPointSize(aPointSize);
   draw.SetDuration(WsfDraw::cFOREVER);
   draw.SetColor(aRedValue, aGreenValue, aBlueValue);
   draw.BeginPoints();
   draw.VertexWCS(aLocWCS);
   draw.VertexWCS(aLocWCS); // Need two point because of VESPA bug
   draw.End();
}

// ================================================================================================
//! Update the perception of the launch system.
void WsfTabularLaunchComputer::UpdateLaunchPerception(double aCurTime)
{
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      moverPtr->Update(aCurTime);
   }

   // Propagate the truth location and velocity to the current time.
   // (The truth values *MAY* be slightly out of date, so propagate it forward by 'dt.'.
   // In cases where called directly from UpdateGuidance, 'dt' should be zero.

   double lauLocWCS[3];
   GetPlatform()->GetLocationWCS(lauLocWCS);
   double lauVelWCS[3];
   GetPlatform()->GetVelocityWCS(lauVelWCS);
   double lauAclWCS[3];
   GetPlatform()->GetAccelerationWCS(lauAclWCS);
   mLauState.SetLocationWCS(lauLocWCS);
   mLauState.SetVelocityWCS(lauVelWCS);
   mLauState.SetAccelerationWCS(lauAclWCS);

   if (moverPtr != nullptr)
   {
      double dt = aCurTime - moverPtr->GetLastUpdateTime();
      UtVec3d::AddProduct(lauLocWCS, lauLocWCS, dt, lauVelWCS);
      UtVec3d::AddProduct(lauLocWCS, lauLocWCS, 0.5 * dt * dt, lauAclWCS);
      UtVec3d::AddProduct(lauVelWCS, lauVelWCS, dt, lauAclWCS);
   }

   // Apply navigation errors if they exist. The velocity and acceleration vectors are copied from the
   // extrapolated frame to the perceived frame using NED coordinates rather than WCS coordinates so the
   // directions remain constant in the local tangent frame.

   double locErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(locErrorWCS);
   if (!UtVec3d::Equals(locErrorWCS, 0.0))
   {
      // Get NED vectors prior to moving the frame. Note that velocity was already retrieved above.
      double lauVelNED[3];
      mLauState.GetVelocityNED(lauVelNED);
      double lauAclNED[3];
      mLauState.GetAccelerationNED(lauAclNED);

      UtVec3d::Add(lauLocWCS, lauLocWCS, locErrorWCS);
      mLauState.SetLocationWCS(lauLocWCS);
      mLauState.SetVelocityNED(lauVelNED);
      mLauState.SetAccelerationNED(lauAclNED);
   }
}

// ================================================================================================
//! Update the perception of the target.
// protected
bool WsfTabularLaunchComputer::UpdateTargetPerception(double aCurTime, const WsfTrack& aTrack)
{
   // Refresh perceived target location and velocity, if possible.

   double       tgtLocWCS[3]  = {0.0, 0.0, 0.0};
   double       tgtVelWCS[3]  = {0.0, 0.0, 0.0};
   double       tgtAclWCS[3]  = {0.0, 0.0, 0.0};
   bool         tgtStateValid = false;
   bool         useTruth      = true;
   WsfPlatform* truthPtr      = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
   if (useTruth && (truthPtr != nullptr))
   {
      // double dt = aCurTime - truthPtr->GetLastUpdateTime();
      // if (dt >= (0.999 * mMoverPtr->GetUpdateInterval()))
      //{
      //    truthPtr->Update(aCurTime);
      // }
      tgtStateValid = true;
      truthPtr->GetLocationWCS(tgtLocWCS);
      truthPtr->GetVelocityWCS(tgtVelWCS);
      truthPtr->GetAccelerationWCS(tgtAclWCS);

      // Extrapolate position forward by the difference between the simulation time and the last platform update time.
      double dt = aCurTime - truthPtr->GetLastUpdateTime();
      UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, dt, tgtVelWCS);
      UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, 0.5 * dt * dt, tgtAclWCS);
      UtVec3d::AddProduct(tgtVelWCS, tgtVelWCS, dt, tgtAclWCS);
   }

   // If the target state isn't valid, get the location from the track.
   if ((!tgtStateValid) && aTrack.GetExtrapolatedLocationWCS(aCurTime, tgtLocWCS))
   {
      tgtStateValid = true;
      UtVec3d::Set(tgtVelWCS, 0.0);
      if (aTrack.VelocityValid())
      {
         aTrack.GetVelocityWCS(tgtVelWCS);
      }
      UtVec3d::Set(tgtAclWCS, 0.0);
   }

   if (tgtStateValid)
   {
      mTgtState.SetLocationWCS(tgtLocWCS);
      mTgtState.SetVelocityWCS(tgtVelWCS);
      mTgtState.SetAccelerationWCS(tgtAclWCS);
   }
   return tgtStateValid;
}
