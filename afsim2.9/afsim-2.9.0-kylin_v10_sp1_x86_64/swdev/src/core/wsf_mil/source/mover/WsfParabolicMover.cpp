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

#include "WsfParabolicMover.hpp"

#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

// static
WsfStringId WsfParabolicMover::TypeId()
{
   return "WSF_PARABOLIC_MOVER";
}

WsfParabolicMover::WsfParabolicMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mLastUpdate(0.0)
   , mPlannedFlightTime(0.0)
   , mPlannedFinalLatDeg(0.0)
   , mPlannedFinalLonDeg(0.0)
   , mPlannedFinalAlt(0.0)
   , mGreatCircleDist(0.0)
   , mStraightLineDist(0.0)
   , mGroundSpeed(0.0)
   , mHeadingDeg(0.0)
   , mStartFlightTime(0.0)
   , mInitialized(false)
   , mInFlight(false)
   , mTrajType(cUNDETERMINED)
{
   const double zero[] = {0.0, 0.0, 0.0};
   UtVec3d::Set(mAccelNED, zero);
   UtVec3d::Set(mCurVelNED, zero);
   UtVec3d::Set(mTotalDeltaNED, zero);
   mAccelNED[2] = UtEarth::cACCEL_OF_GRAVITY;
}


WsfParabolicMover::WsfParabolicMover(const WsfParabolicMover& aSrc)
   : WsfMover(aSrc)
   , mLastUpdate(aSrc.mLastUpdate)
   , mPlannedFlightTime(aSrc.mPlannedFlightTime)
   , mPlannedFinalLatDeg(aSrc.mPlannedFinalLatDeg)
   , mPlannedFinalLonDeg(aSrc.mPlannedFinalLonDeg)
   , mPlannedFinalAlt(aSrc.mPlannedFinalAlt)
   , mGreatCircleDist(aSrc.mGreatCircleDist)
   , mStraightLineDist(aSrc.mStraightLineDist)
   , mGroundSpeed(aSrc.mGroundSpeed)
   , mHeadingDeg(aSrc.mHeadingDeg)
   , mStartFlightTime(aSrc.mStartFlightTime)
   , mInitialized(aSrc.mInitialized)
   , mInFlight(aSrc.mInFlight)
   , mTrajType(aSrc.mTrajType)
{
   UtVec3d::Set(mAccelNED, aSrc.mAccelNED);
   UtVec3d::Set(mCurVelNED, aSrc.mCurVelNED);
   UtVec3d::Set(mTotalDeltaNED, aSrc.mTotalDeltaNED);
}

// virtual
WsfParabolicMover::~WsfParabolicMover()
{
   if ((DebugEnabled()) && (mLastUpdate != 0.0) && (GetSimulation() != nullptr))
   {
      auto out = ut::log::debug() << "WsfParabolicMover on Platform is destructing.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Desired Time of Flight: " << mPlannedFlightTime << " s";
      out.AddNote() << "Actual Time of Flight: " << mLastUpdate - mStartFlightTime << " s";

      double actualFinalLat;
      double actualFinalLon;
      double actualFinalAlt;
      GetPlatform()->GetLocationLLA(actualFinalLat, actualFinalLon, actualFinalAlt);

      out.AddNote() << "Desired Final Altitude: " << mPlannedFinalAlt << " m";
      out.AddNote() << "Actual Final Altitude: " << actualFinalAlt << " m";

      double actualFinalWCSLoc[3];
      GetPlatform()->GetLocationWCS(actualFinalWCSLoc);

      double intendedFinalWCSLoc[3];
      UtEntity::ConvertLLAToWCS(mPlannedFinalLatDeg, mPlannedFinalLonDeg, mPlannedFinalAlt, intendedFinalWCSLoc);

      double errorVector[3];
      UtVec3d::Subtract(errorVector, actualFinalWCSLoc, intendedFinalWCSLoc);
      double offsetMag = UtVec3d::Magnitude(errorVector);

      wsf::Terrain terraFirma(GetSimulation()->GetTerrainInterface());
      float        terrainElev;
      terraFirma.GetElevInterp(actualFinalLat, actualFinalLon, terrainElev);

      out.AddNote() << "Final Actual HAT: " << actualFinalAlt - terrainElev << " meters (but may not be used).";
      out.AddNote() << "Final 3-D offset from intended position is " << offsetMag << " meters.";
   }
}

// virtual
WsfMover* WsfParabolicMover::Clone() const
{
   return new WsfParabolicMover(*this);
}

// virtual
bool WsfParabolicMover::Initialize(double aSimTime)
{
   bool ok     = WsfMover::Initialize(aSimTime);
   mLastUpdate = aSimTime;

   return ok;
}

// virtual
bool WsfParabolicMover::ProcessInput(UtInput& aInput)
{
   const std::string errorMsg("WsfParabolicMover::ProcessInput(), desired impact (Lat,Lon,Alt) already set.");

   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "tof_and_impact_lat_lon_alt")
   {
      double tof;
      double lat;
      double lon;
      double alt;
      aInput.ReadValueOfType(tof, UtInput::cTIME);
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);

      if (!SetImpactTOFAndLLA(tof, lat, lon, alt))
      {
         throw UtInput::BadValue(aInput, errorMsg);
      }
   }
   else if (command == "tof_and_impact_lat_lon")
   {
      double tof;
      double lat;
      double lon;
      aInput.ReadValueOfType(tof, UtInput::cTIME);
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);

      if (!SetImpactTOFAndLL(tof, lat, lon))
      {
         throw UtInput::BadValue(aInput, errorMsg);
      }
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }

   return myCommand;
}


bool WsfParabolicMover::SetBallistic()
{
   if (mTrajType == cUNDETERMINED)
   {
      mTrajType = cSIMPLE_GRAVITY;
      if (DebugEnabled())
      {
         ut::log::debug() << "WsfParabolicMover::SetBallistic() set Trajectory Type to Ballistic (simple gravity).";
      }
      return true;
   }
   else
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "WsfParabolicMover::SetBallistic() failed since Trajectory Type was already determined.";
      }
      return false;
   }
}

bool WsfParabolicMover::SetCurrentTarget()
{
   if (mTrajType == cUNDETERMINED)
   {
      mTrajType = cCURRENT_TARGET;
      if (DebugEnabled())
      {
         ut::log::debug() << "WsfParabolicMover::SetCurrentTarget() successful.";
      }
      return true;
   }
   else
   {
      if (DebugEnabled())
      {
         ut::log::debug()
            << "WsfParabolicMover::SetCurrentTarget() failed since Trajectory Type was already determined.";
      }
      return false;
   }
}

bool WsfParabolicMover::SetImpactTOFAndLLA(double aTOF, double aLat, double aLon, double aAlt)
{
   bool set = false;

   if (mTrajType == cUNDETERMINED)
   {
      mTrajType           = cLLA_IMPACT;
      mPlannedFlightTime  = aTOF;
      mPlannedFinalLatDeg = aLat;
      mPlannedFinalLonDeg = aLon;
      mPlannedFinalAlt    = aAlt;
      set                 = true;

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfParabolicMover set to desired impact LLA.";
         if (GetPlatform() != nullptr)
         {
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         else
         {
            out.AddNote() << "Platform: <unknown>";
         }
      }
   }

   return set;
}

bool WsfParabolicMover::SetImpactTOFAndLL(double aTOF, double aLat, double aLon)
{
   float          terrainAlt = 0;
   WsfSimulation* simPtr     = GetSimulation();
   if (simPtr != nullptr)
   {
      wsf::Terrain terraFirma(simPtr->GetTerrainInterface());
      terraFirma.GetElevInterp(aLat, aLon, terrainAlt);
   }
   return SetImpactTOFAndLLA(aTOF, aLat, aLon, double(terrainAlt));
}

void WsfParabolicMover::InitShot(double aSimTime)
{
   const WsfTrack* targetTrackPtr = nullptr;
   if (GetPlatform() != nullptr)
   {
      targetTrackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   }

   // cUNDETERMINED, cLLA_IMPACT, cSIMPLE_GRAVITY, cCURRENT_TARGET
   bool goForTarget = ((mTrajType == cUNDETERMINED) || (mTrajType == cCURRENT_TARGET));

   double targetLocWCS[3];
   if (goForTarget && (targetTrackPtr != nullptr) && (targetTrackPtr->GetExtrapolatedLocationWCS(aSimTime, targetLocWCS)))
   {
      mTrajType = cCURRENT_TARGET;
      if (DebugEnabled())
      {
         ut::log::debug() << "During WsfParabolicMover::InitShot(), a valid target track was found. Trajectory "
                             "computed to intercept.";
      }

      UtEntity::ConvertWCSToLLA(targetLocWCS, mPlannedFinalLatDeg, mPlannedFinalLonDeg, mPlannedFinalAlt);
      ToWCSLocation(targetLocWCS);
   }
   else if (mTrajType == cLLA_IMPACT)
   {
      double initLocWCS[3];
      GetPlatform()->GetLocationWCS(initLocWCS);

      double initLat;
      double initLon;
      double initAlt;
      UtEntity::ConvertWCSToLLA(initLocWCS, initLat, initLon, initAlt);
      UtSphericalEarth::GreatCircleHeadingAndDistance(initLat,
                                                      initLon,
                                                      mPlannedFinalLatDeg,
                                                      mPlannedFinalLonDeg, // inputs
                                                      mHeadingDeg,
                                                      mGreatCircleDist); // outputs

      // ====================================================================
      // BUT:  Due to curvature of the earth, this trajectory will hit long,
      // so find out what the actual straight-line distance is.

      double finalLocWCS[3];
      UtEntity::ConvertLLAToWCS(mPlannedFinalLatDeg, mPlannedFinalLonDeg, initAlt, finalLocWCS);

      double delta[3];
      UtVec3d::Subtract(delta, initLocWCS, finalLocWCS);
      mStraightLineDist = UtVec3d::Magnitude(delta);

      mGroundSpeed = mGreatCircleDist / mPlannedFlightTime; // ?? mGreatCircleDist mStraightLineDist ??

      mTotalDeltaNED[0] = cos(mHeadingDeg * UtMath::cRAD_PER_DEG) * mGreatCircleDist;
      mTotalDeltaNED[1] = sin(mHeadingDeg * UtMath::cRAD_PER_DEG) * mGreatCircleDist;
      mTotalDeltaNED[2] = mPlannedFinalAlt - initAlt;

      // ====================================================================

      GetPlatform()->GetVelocityNED(mCurVelNED);

      if (DebugEnabled())
      {
         ut::log::debug() << "WsfParabolicMover::InitShot() begin flight for an LLA impact.";
      }

      SetAccelerations();
   }
   else
   {
      // Revert to simple gravity trajectory... (TOF unknown)

      GetPlatform()->GetVelocityNED(mCurVelNED);
      mGroundSpeed = sqrt(mCurVelNED[0] * mCurVelNED[0] + mCurVelNED[1] * mCurVelNED[1]);
      if (DebugEnabled())
      {
         if (mTrajType == cUNDETERMINED)
         {
            ut::log::debug() << "WsfParabolicMover::InitShot() defaulting to Simple Gravity during begin flight.";
         }
         else if (mTrajType != cSIMPLE_GRAVITY)
         {
            ut::log::debug()
               << "WsfParabolicMover::InitShot() overriding trajectory type with Simple Gravity during begin flight.";
         }
      }
      mTrajType = cSIMPLE_GRAVITY;
   }

   mStartFlightTime = aSimTime;
   mInFlight        = true;
}

void WsfParabolicMover::ToWCSLocation(double aFinalWCSLocation[])
{
   // This method sets the following values:
   // (The last three values may vary during the trajectory, and mTOF may be overwritten later...)
   // mPlannedFinalLatDeg, mPlannedFinalLonDeg, mPlannedFinalAlt,
   // mGreatCircleDist, mTotalDeltaNED[], mPlannedFlightTime,
   // mCurVelNED[], mHeadingDeg, mGroundSpeed

   UtEntity::ConvertWCSToLLA(aFinalWCSLocation, mPlannedFinalLatDeg, mPlannedFinalLonDeg, mPlannedFinalAlt);

   double initLocWCS[3];
   GetPlatform()->GetLocationWCS(initLocWCS);

   double initLat;
   double initLon;
   double initAlt;
   UtEntity::ConvertWCSToLLA(initLocWCS, initLat, initLon, initAlt);

   UtSphericalEarth::GreatCircleHeadingAndDistance(initLat,
                                                   initLon,
                                                   mPlannedFinalLatDeg,
                                                   mPlannedFinalLonDeg, // inputs
                                                   mHeadingDeg,
                                                   mGreatCircleDist); // outputs

   // ====================================================================
   // BUT:  Due to curvature of the earth, this trajectory will hit long,
   // so find out what the actual straight-line distance is.

   double finalLocWCS[3];
   UtEntity::ConvertLLAToWCS(mPlannedFinalLatDeg, mPlannedFinalLonDeg, initAlt, finalLocWCS);

   double delta[3];
   UtVec3d::Subtract(delta, initLocWCS, finalLocWCS);
   mStraightLineDist = UtVec3d::Magnitude(delta);

   mTotalDeltaNED[0] =
      cos(mHeadingDeg * UtMath::cRAD_PER_DEG) * mGreatCircleDist; // ?? mStraightLineDist mGreatCircleDist ??
   mTotalDeltaNED[1] =
      sin(mHeadingDeg * UtMath::cRAD_PER_DEG) * mGreatCircleDist; // ?? mStraightLineDist mGreatCircleDist ??
   mTotalDeltaNED[2] = mPlannedFinalAlt - initAlt;

   // ====================================================================

   GetPlatform()->GetVelocityNED(mCurVelNED);
   mGroundSpeed       = sqrt(mCurVelNED[0] * mCurVelNED[0] + mCurVelNED[1] * mCurVelNED[1]);
   mPlannedFlightTime = mGreatCircleDist / mGroundSpeed; // ?? mGreatCircleDist mStraightLineDist
   SetAccelerations();
}

void WsfParabolicMover::SetAccelerations()
{
   // Standard stuff:  v = a * dt; d = r * dt;
   double dt  = mPlannedFlightTime;
   double dt2 = dt * dt;

   mAccelNED[0] = 2.0 * (mTotalDeltaNED[0] - mCurVelNED[0] * dt) / dt2;
   mAccelNED[1] = 2.0 * (mTotalDeltaNED[1] - mCurVelNED[1] * dt) / dt2;
   mAccelNED[2] = 2.0 * (mTotalDeltaNED[2] - mCurVelNED[2] * dt) / dt2;
}

// virtual
void WsfParabolicMover::Update(double aSimTime)
{
   double deltaT = aSimTime - mLastUpdate;

   if (deltaT > 0.0)
   {
      if (!mInFlight)
      {
         InitShot(aSimTime);
      }

      double initVelNED[3];
      UtVec3d::Set(initVelNED, mCurVelNED);

      mCurVelNED[0] += mAccelNED[0] * deltaT;
      mCurVelNED[1] += mAccelNED[1] * deltaT;
      mCurVelNED[2] += mAccelNED[2] * deltaT;

      double deltaNED[3];

      deltaNED[0] = 0.5 * (initVelNED[0] + mCurVelNED[0]) * deltaT;
      deltaNED[1] = 0.5 * (initVelNED[1] + mCurVelNED[1]) * deltaT;
      deltaNED[2] = 0.5 * (initVelNED[2] + mCurVelNED[2]) * deltaT;

      GetPlatform()->IncrementLocationNED(deltaNED);

      GetPlatform()->SetVelocityNED(mCurVelNED);

      double roll    = 0;
      double heading = atan2(mCurVelNED[1], mCurVelNED[0]);

      double mag   = UtVec3d::Magnitude(mCurVelNED);
      double pitch = asin(-mCurVelNED[2] / mag);

      GetPlatform()->SetOrientationNED(heading, pitch, roll);

      mLastUpdate = aSimTime;
   }
}
