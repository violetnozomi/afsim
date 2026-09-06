// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "FiresMover.hpp"

#include "FiresLaunchComputer.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"

namespace Fires
{

FiresMover::FiresMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , FiresPath(aScenario)
   , mAzimuthErrorExpected(0.0)
   , mElevationErrorExpected(0.0)
{
}

FiresMover::FiresMover(const FiresMover& aSrc)
   : WsfMover(aSrc)
   , FiresPath(aSrc)
   , mAzimuthErrorExpected(aSrc.mAzimuthErrorExpected)
   , mElevationErrorExpected(aSrc.mElevationErrorExpected)
{
}

// virtual
FiresMover::~FiresMover() {}

WsfMover* FiresMover::Clone() const
{
   return new FiresMover(*this);
}

// virtual
bool FiresMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "remove_on_impact")
   {
      aInput.ReadValue(mRemoveOnImpact);
   }
   else if (command == "impact_range")
   {
      aInput.ReadValueOfType(mImpactRange, UtInput::cLENGTH);
   }
   else if (command == "initial_bearing")
   {
      aInput.ReadValueOfType(mInitialBearing, UtInput::cANGLE);
   }
   else if (command == "impact_location")
   {
      myCommand          = mImpactLocation.ProcessInput(aInput);
      mHasImpactLocation = true;
   }
   else if (command == "use_simple_propagation")
   {
      bool useSimplePropagation;
      aInput.ReadValue(useSimplePropagation);
      if (useSimplePropagation)
      {
         mPropagationType = cSIMPLE;
      }
      else
      {
         mPropagationType = cUNDEFINED;
      }
   }
   else if (command == "initial_time_constant")
   {
      aInput.ReadValueOfType(mTimeConstant, UtInput::cTIME);
   }
   else if (command == "debug")
   {
      aInput.ReadValue(mDebug);
   }
   else if (command == "time_of_flight")
   {
      aInput.ReadValueOfType(mImpactTime, UtInput::cTIME);
   }
   else if ((command == "maximum_ordinate") || (command == "maximum_height"))
   {
      aInput.ReadValueOfType(mMaxHeight, UtInput::cLENGTH);
   }
   else if (command == "constrain_to_simple_propagation")
   {
      aInput.ReadValue(mConstrainPropagation);
   }
   else if (command == "azimuth_error_sigma")
   {
      aInput.ReadValueOfType(mAzimuthErrorExpected, UtInput::cANGLE);
   }
   else if (command == "elevation_error_sigma")
   {
      aInput.ReadValueOfType(mElevationErrorExpected, UtInput::cANGLE);
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool FiresMover::Initialize2(double aSimTime)
{
   WsfMover::Initialize2(aSimTime);
   mFiresType = GetPlatform()->GetTypeId();
   ComputeTargetRangeBearingDeltaAlt();
   if (mAzimuthErrorExpected != 0.0)
   {
      SetAzimuthError(GetSimulation()->GetRandom().Normal(0.0, mAzimuthErrorExpected));
   }
   if (mElevationErrorExpected != 0.0)
   {
      SetElevationError(GetSimulation()->GetRandom().Normal(0.0, mElevationErrorExpected));
   }
   bool ok        = FiresPath::Initialize(aSimTime);
   mSimulationPtr = GetSimulation();
   if (ok)
   {
      Update(aSimTime);
   }
   return ok;
}

void FiresMover::Update(double aSimTime)
{
   if (mImpactRange != 0.0)
   {
      UpdateState(aSimTime);

      GetPlatform()->SetLocationWCS(mLocWCS);
      GetPlatform()->SetVelocityWCS(mVelWCS);
      GetPlatform()->SetAccelerationNED(mAccNED);
      GetPlatform()->SetOrientationNED(mHeading, mPitch, mRoll);

      // Check impact time; we want to remove the platform from the simulation.
      float  elev = 0.0;
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);

      if (mHasImpactLocation)
      {
         elev = (float)mImpactLocation.GetAlt();
      }
      else
      {
         if (GetSimulation()->GetTerrainInterface()->IsEnabled())
         {
            GetSimulation()->GetTerrainInterface()->GetGeodeticElev(lat, lon, elev);
         }
      }
      if (mRemoveOnImpact && (alt < elev) && (mVelNED[2] > 0.0))
      {
         if (mDebug)
         {
            auto logDebug = ut::log::debug() << "Removing internal fires Target.";
            logDebug.AddNote() << "Target: " << GetPlatform()->GetName();
            double targetVecWCS[3];
            double initialLocWCS[3];
            mInitialLocation.GetLocationWCS(initialLocWCS);
            UtVec3d::Subtract(targetVecWCS, mLocWCS, initialLocWCS);
            double range = UtVec3d::Magnitude(targetVecWCS);
            if (DebugEnabled())
            {
               logDebug.AddNote() << "Flight Time: " << aSimTime - mInitialTime;
               logDebug.AddNote() << "Range: " << range;
            }
            if (mHasImpactLocation)
            {
               double impactLocWCS[3];
               mImpactLocation.GetLocationWCS(impactLocWCS);
               UtVec3d::Subtract(targetVecWCS, mLocWCS, impactLocWCS);
               double rtt = UtVec3d::Magnitude(targetVecWCS);
               logDebug.AddNote() << "Range to target: " << rtt;
            }
         }
         WsfWeaponEngagement* wePtr = WsfWeaponEngagement::Find(*GetPlatform());
         if (wePtr != nullptr)
         {
            wePtr->Terminate(aSimTime, WsfWeaponEngagement::cTARGET_IMPACT);
         }
         GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
      }
   }
   WsfMover::Update(aSimTime);
}

void FiresMover::ComputeTargetRangeBearingDeltaAlt()
{
   double lat, lon, alt;
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   mInitialLocation = WsfGeoPoint(lat, lon, alt);

   double initialPointWCS[3];
   mInitialLocation.GetLocationWCS(initialPointWCS);
   mGeom.SetLocationWCS(initialPointWCS);

   // First, consider initialization from the current target track.
   if ((!mHasImpactLocation) && (mImpactRange == 0.0) && (GetPlatform()->GetTrackManager().GetCurrentTarget() != nullptr))
   {
      // Use a valid location in the current target track, if it exists.
      const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      if (trackPtr->LocationValid())
      {
         double impactLocWCS[3];
         trackPtr->GetLocationWCS(impactLocWCS);
         mImpactLocation    = WsfGeoPoint(impactLocWCS);
         mHasImpactLocation = true;
      }
   }
   if (mHasImpactLocation)
   {
      // Find the impact range
      double impactLocWCS[3];
      mImpactLocation.GetLocationWCS(impactLocWCS);
      double targetVecNED[3];
      mGeom.ConvertWCSToNED(impactLocWCS, targetVecNED);
      mDeltaAlt = -targetVecNED[2];

      // In this case the range is only the horizontal range
      targetVecNED[2] = 0.0; // no down component

      mImpactRange    = UtVec3d::Magnitude(targetVecNED);
      mInitialBearing = atan2(targetVecNED[1], targetVecNED[0]);
   }
}

} // namespace Fires
