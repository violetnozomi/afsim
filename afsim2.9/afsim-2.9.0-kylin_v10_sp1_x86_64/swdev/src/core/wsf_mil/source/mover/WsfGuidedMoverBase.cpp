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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfGuidedMoverBase.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <iomanip>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLLPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtScriptData.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMoverGuidance.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
const double cUNDEFINED_DOUBLE = 1.0E+30; // This must be a large POSITIVE value
}

// ================================================================================================
WsfGuidedMoverBase::WsfGuidedMoverBase(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , RouteChanged()
   , StageIgnition()
   , StageBurnout()
   , StageSeparation()
   , mLaunchTime(-1.0)
   , mExternalState()
   , mLastAdvanceTime(-1.0)
   , mIntegrationTimestep(0.0)
   , mStageIndex(0)
   , mBoostCompleteStageIndex(0)
   , mBoostComplete(false)
   , mRoute()
   , mPhase(cPHASE_PRE_BURN)
   , mPhaseStartTime(0.0)
   , mPreIgnitionCoastTime(0.0)
   , mPreSeparationCoastTime(0.0)
   , mLaunchComputerBurnTime(0.0)
   , mGuidancePtr(nullptr)
   , mYForce(0.0)
   , mZForce(0.0)
   , mCommandedRollRate(0.0)
   , mCommandedPitchRate(0.0)
   , mCommandedYawRate(0.0)
   , mCommandedSpeed(-1.0)
   , mCommandedThrottle(1.0)
   , mCommandedEngineStopTime(cUNDEFINED_DOUBLE)
   , mCommandedPreSeparationCoastTime(-1.0)
   , mCommandedPreIgnitionCoastTime(-1.0)
   , mMinimumHeightAboveTerrain(0.0)
   , mShowStatus(false)
   , mShowTrajectory(false)
   , mMatchOwningPlatHeading(false)
   , mCheckForGroundImpact(false)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mSeaLevelPressure(0.0)
   , mFMM_Time(-1.0)
   , mFMM_Heading(0.0)
   , mTimeHistoryPath()
   , mTimeHistoryOfs()
   , mLaunchPlatformIndex(0)
   , mLaunchWeaponPtr(nullptr)
{
   UtVec3d::Set(mFMM_LocWCS, 0.0);
}

// ================================================================================================
WsfGuidedMoverBase::WsfGuidedMoverBase(const WsfGuidedMoverBase& aSrc)
   : WsfMover(aSrc)
   , RouteChanged()
   , StageIgnition()
   , StageBurnout()
   , StageSeparation()
   , mLaunchTime(-1.0)
   , mExternalState()
   , mLastAdvanceTime(-1.0)
   , mIntegrationTimestep(aSrc.mIntegrationTimestep)
   , mStageIndex(0)
   , mBoostCompleteStageIndex(0)
   , mBoostComplete(false)
   , mRoute(aSrc.mRoute)
   , mPhase(aSrc.mPhase)
   , mPhaseStartTime(aSrc.mPhaseStartTime)
   , mPreIgnitionCoastTime(aSrc.mPreIgnitionCoastTime)
   , mPreSeparationCoastTime(aSrc.mPreSeparationCoastTime)
   , mLaunchComputerBurnTime(0.0)
   , mGuidancePtr(nullptr)
   , mYForce(0.0)
   , mZForce(0.0)
   , mCommandedRollRate(aSrc.mCommandedRollRate)
   , mCommandedPitchRate(aSrc.mCommandedPitchRate)
   , mCommandedYawRate(aSrc.mCommandedYawRate)
   , mCommandedSpeed(aSrc.mCommandedSpeed)
   , mCommandedThrottle(aSrc.mCommandedThrottle)
   , mCommandedEngineStopTime(aSrc.mCommandedEngineStopTime)
   , mCommandedPreSeparationCoastTime(aSrc.mCommandedPreSeparationCoastTime)
   , mCommandedPreIgnitionCoastTime(aSrc.mCommandedPreIgnitionCoastTime)
   , mMinimumHeightAboveTerrain(aSrc.mMinimumHeightAboveTerrain)
   , mShowStatus(aSrc.mShowStatus)
   , mShowTrajectory(aSrc.mShowTrajectory)
   , mMatchOwningPlatHeading(aSrc.mMatchOwningPlatHeading)
   , mCheckForGroundImpact(aSrc.mCheckForGroundImpact)
   , mAtmosphere(aSrc.mAtmosphere)
   , mSeaLevelPressure(0.0)
   , mFMM_Time(-1.0)
   , mFMM_Heading(0.0)
   , mTimeHistoryPath(aSrc.mTimeHistoryPath)
   , mTimeHistoryOfs()
   , mLaunchPlatformIndex(0)
   , mLaunchWeaponPtr(nullptr)
{
   UtVec3d::Set(mFMM_LocWCS, 0.0);
}

// ================================================================================================
WsfGuidedMoverBase::~WsfGuidedMoverBase()
{
   if ((mLaunchTime >= 0.0) && mShowStatus)
   {
      auto out = ut::log::debug() << "Platform terminated." << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << mLastUpdateTime;
      out.AddNote() << "Since Launch: " << mLastUpdateTime - mLaunchTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      WriteKinematicStatus(out);
   }
}

// ================================================================================================
// The derived class should call this class and then perform any required initialization of stages, etc.
// It should NOT invoke 'InitiateStage' however.That is done by this class in Initialize2().
bool WsfGuidedMoverBase::Initialize(double aSimTime) // override
{
   bool ok          = WsfMover::Initialize(aSimTime);
   mLaunchTime      = aSimTime;
   mLastAdvanceTime = aSimTime;

   mExternalState = static_cast<UtEntity&>(*GetPlatform());

   // If debug is enabled then show_status is also enabled.
   mShowStatus |= DebugEnabled();

   // Get sea-level pressure for thrust calculations
   mSeaLevelPressure = mAtmosphere.Pressure(0.0);

   // If an integration_timestep is specified then the update_time_tolerance is not used.
   if (mIntegrationTimestep > 0.0)
   {
      mUpdateTimeTolerance = 0.0;
   }

   return ok;
}

// ================================================================================================
// If the derived class has overridden this method then it should call it.
// It's primary function to initiate the first stage and start the trajectory output (if requested).
bool WsfGuidedMoverBase::Initialize2(double aSimTime) // override
{
   bool ok = WsfMover::Initialize2(aSimTime);

   // Get any launch computer data that may be used from the hand-off track.
   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      // If the burn time is given as zero (not set), then this might erroneously
      // cause a transition to next stage burn.  So use a temp variable here:
      double commandedEngineStopTime = 0.0;
      WsfLaunchComputer::GetBurnTime(*trackPtr, commandedEngineStopTime);
      if (commandedEngineStopTime != 0.0)
      {
         // Convert to absolute simulation time
         mCommandedEngineStopTime = aSimTime + commandedEngineStopTime;
      }
   }

   InitiateStage(aSimTime, aSimTime, 0); // Start with the first stage

   if (!mTimeHistoryPath.empty())
   {
      std::string fileName(mTimeHistoryPath + '/' + GetPlatform()->GetName() + ".mth");
      mTimeHistoryOfs.open(fileName);
      if (mTimeHistoryOfs)
      {
         mTimeHistoryOfs << "#--TOF-- --Range- ---Alt-- --Mass-- --Speed- --Mach--"
                         << " -Fy-Cmd- -Fz-Cmd- -Fy-Act- -Fz-Act-" << std::endl;
      }
      else
      {
         auto out = ut::log::warning() << "Unable to open time history file.";
         out.AddNote() << "File: " << fileName;
      }
   }

   // If the first stage has a pre-ignition delay, capture the data needed to maintain the attachment.
   if (mPreIgnitionCoastTime > 0.0)
   {
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
      if (engagementPtr != nullptr)
      {
         mLaunchPlatformIndex           = engagementPtr->GetFiringPlatformIndex();
         WsfPlatform* launchPlatformPtr = GetSimulation()->GetPlatformByIndex(mLaunchPlatformIndex);
         if (launchPlatformPtr != nullptr)
         {
            mLaunchWeaponPtr = launchPlatformPtr->GetComponent<WsfWeapon>(engagementPtr->GetWeaponSystemName());
         }
         if ((mLaunchPlatformIndex == 0) || (mLaunchWeaponPtr == nullptr))
         {
            mLaunchPlatformIndex = 0;
            mLaunchWeaponPtr     = nullptr;
         }
      }
   }

   // If there is no pre-ignition coast time then we set the initial state now.
   if (mLaunchPlatformIndex == 0)
   {
      SetInitialState(aSimTime, aSimTime, true);
   }
   return ok;
}

// ================================================================================================
bool WsfGuidedMoverBase::ProcessInput(UtInput& aInput) // override
{
   std::unique_ptr<WsfRoute> routePtr  = nullptr;
   bool                      myCommand = true;
   std::string               command(aInput.GetCommand());

   if (WsfRouteTypes::Get(GetScenario()).LoadInstance(aInput, routePtr))
   {
      if (routePtr != nullptr)
      {
         bool routeSet = SetRoute(0.0, *routePtr); // This copies the route
         if (!routeSet)
         {
            throw UtInput::BadValue(aInput, "Unable to establish route");
         }
      }
   }
   else if (command == "integration_timestep")
   {
      // A value of zero indicates no upper bound on the timestep.
      aInput.ReadValueOfType(mIntegrationTimestep, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mIntegrationTimestep, 0.0);
   }
   else if (command == "show_status")
   {
      mShowStatus = true;
   }
   else if (command == "show_trajectory")
   {
      mShowTrajectory = true;
   }
   else if (command == "align_heading_with_velocity")
   {
      aInput.ReadValue(mMatchOwningPlatHeading);
   }
   else if (command == "check_for_ground_impact")
   {
      mCheckForGroundImpact = true;
   }
   else if (command == "time_history_path")
   {
      aInput.ReadValue(mTimeHistoryPath);
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
void WsfGuidedMoverBase::Update(double aSimTime) // override
{
   if (aSimTime == mLastUpdateTime)
   {
      return;
   }

   if (mIntegrationTimestep == 0.0)
   {
      // No integration timestep provided. Updates driven purely by the simulation.
      while (mLastAdvanceTime <= (aSimTime - mUpdateTimeTolerance))
      {
         double begTime = mLastAdvanceTime;
         double endTime = aSimTime;
         AdvanceTimeInternal(aSimTime, begTime, endTime);
         mLastAdvanceTime = endTime;
      }
   }
   else
   {
      // Integration timestep provided.
      while ((mLastAdvanceTime + mIntegrationTimestep) <= aSimTime)
      {
         double begTime       = mLastAdvanceTime;
         double endTime       = -1.0;
         double targetEndTime = std::min(mLastAdvanceTime + mIntegrationTimestep, aSimTime);
         while (endTime != targetEndTime)
         {
            endTime = targetEndTime;
            AdvanceTimeInternal(aSimTime, begTime, endTime);
            begTime = endTime;
         }
         mLastAdvanceTime = endTime;
      }
   }

   // Extrapolate the last state published by derived class forward to the current time.
   //
   // For now we only extrapolate the position by only the velocity. Using the accelerations
   // in a pure linear extrapolation can cause some issues.
   //
   // NOTE: If the derived class is using ECI coordinates then this must also do the same.
   //       Some downstream processors (e.g.: WsfGuidanceComputer) need to know the coordinate frame.

   double dt = aSimTime - mLastAdvanceTime;
   if (mExternalState.GetCoordinateFrame() == UtEntity::cCF_INERTIAL)
   {
      double locTOD[3];
      double velTOD[3];
      double aclTOD[3];
      double oriECI[3];
      mExternalState.SetTime(aSimTime);
      mExternalState.GetLocationTOD(locTOD);
      mExternalState.GetVelocityTOD(velTOD);
      mExternalState.GetAccelerationTOD(aclTOD);
      mExternalState.GetOrientationECI(oriECI[0], oriECI[1], oriECI[2]);
      UtVec3d::AddProduct(locTOD, locTOD, dt, velTOD);
      // UtVec3d::AddProduct(locTOD, locTOD, 0.5 * dt * dt, aclTOD);
      // UtVec3d::AddProduct(velTOD, velTOD, dt, aclTOD);
      GetPlatform()->SetTime(aSimTime);
      GetPlatform()->SetLocationTOD(locTOD);
      GetPlatform()->SetVelocityTOD(velTOD);
      GetPlatform()->SetAccelerationTOD(aclTOD);
   }
   else
   {
      double locWCS[3];
      double velWCS[3];
      double aclWCS[3];
      double psiWCS;
      double thetaWCS;
      double phiWCS;
      mExternalState.GetLocationWCS(locWCS);
      mExternalState.GetVelocityWCS(velWCS);
      mExternalState.GetAccelerationWCS(aclWCS);
      mExternalState.GetOrientationWCS(psiWCS, phiWCS, thetaWCS);
      UtVec3d::AddProduct(locWCS, locWCS, dt, velWCS);
      // UtVec3d::AddProduct(locWCS, locWCS, 0.5 * dt * dt, aclWCS);
      // UtVec3d::AddProduct(velWCS, velWCS, dt, aclWCS);

      // If a commanded terrain offset has been commanded and terrain is enabled, adjust the altitude
      // (smoothly, if possible) so the object doesn't hit the terrain (crude terrain following).

      if ((mMinimumHeightAboveTerrain > 0.0) && GetPlatform()->GetTerrain().IsEnabled())
      {
         double lat;
         double lon;
         double alt;
         UtEntity::ConvertWCSToLLA(locWCS, lat, lon, alt);
         float terrainHeight;
         GetPlatform()->GetTerrain().GetElevInterp(lat, lon, terrainHeight);
         double minAlt = terrainHeight + mMinimumHeightAboveTerrain;
         if (alt < minAlt)
         {
            UtEntity::ConvertLLAToWCS(lat, lon, minAlt, locWCS);
         }
      }
      GetPlatform()->SetLocationWCS(locWCS);
      GetPlatform()->SetVelocityWCS(velWCS);
      GetPlatform()->SetAccelerationWCS(aclWCS);
      GetPlatform()->SetOrientationWCS(psiWCS, phiWCS, thetaWCS);
   }

   PostUpdate(aSimTime); // Let derived class do any post-update actions

   // If requested, check for ground impact directly rather than relying on a fuse.
   if (mCheckForGroundImpact)
   {
      double lat;
      double lon;
      double alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      float terrainHeight;
      GetPlatform()->GetTerrain().GetElevInterp(lat, lon, terrainHeight);
      if ((alt - 1.0) < terrainHeight)
      {
         GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
      }
   }
   mLastUpdateTime = aSimTime;
}

// ================================================================================================
// This probably isn't needed, but if a PartBroken comes from some other source other than an ignition or
// staging failure, we want to make sure the phase gets set so the kinematics don't consider thrust.
// Note that ignition and staging failures will end up here, but we won't count on it in the future.
void WsfGuidedMoverBase::PartBroken(double aSimTime) // override
{
   mPhase                   = cPHASE_FAIL;
   mBoostCompleteStageIndex = mStageIndex;
   mBoostComplete           = true;
   WsfMover::PartBroken(aSimTime);
}

// ================================================================================================
void WsfGuidedMoverBase::AdvanceTimeInternal(double aSimTime, double aBegTime, double& aEndTime)
{
   // Copy the information from the last timestep into the platform. This is necessary because
   // the guidance computer uses the platform data and it needs the most up-to-date data.
   // At some point the interface between the mover and the guidance computer should be updated so
   // this will not be necessary.

   static_cast<UtEntity&>(*GetPlatform()) = mExternalState;
   mLastUpdateTime                        = aBegTime;

   Phase nextPhase = mPhase; // Assume no change in phase.
   if (mPhase == cPHASE_BURN)
   {
      if ((mCommandedEngineStopTime != cUNDEFINED_DOUBLE) && (aEndTime >= mCommandedEngineStopTime))
      {
         // Engine shut-down during this integration interval
         aEndTime                 = mCommandedEngineStopTime;
         nextPhase                = cPHASE_POST_BURN;
         mCommandedEngineStopTime = cUNDEFINED_DOUBLE; // Prevent subsequent calls
      }
   }
   else if (mPhase == cPHASE_PRE_BURN)
   {
      // Use the commanded pre-ignition coast time if present, otherwise use the value defined in the stage.
      double preIgnitionCoastTime =
         (mCommandedPreIgnitionCoastTime >= 0.0) ? mCommandedPreIgnitionCoastTime : mPreIgnitionCoastTime;
      if (aEndTime >= (mPhaseStartTime + preIgnitionCoastTime))
      {
         // Pre-burn coast phase will end in this integration interval.
         aEndTime                       = mPhaseStartTime + preIgnitionCoastTime;
         nextPhase                      = cPHASE_BURN;
         mCommandedPreIgnitionCoastTime = -1.0; // If used, the commanded value is cleared after satisfied
      }
   }
   else if (mPhase == cPHASE_POST_BURN)
   {
      // Use the commanded pre-separation coast time if present, otherwise use the value defined in the stage.
      double preSeparationCoastTime =
         (mCommandedPreSeparationCoastTime >= 0.0) ? mCommandedPreSeparationCoastTime : mPreSeparationCoastTime;
      if (aEndTime >= (mPhaseStartTime + preSeparationCoastTime))
      {
         // Post-burn coast phase will end in this integration interval.
         aEndTime                         = mPhaseStartTime + preSeparationCoastTime;
         nextPhase                        = cPHASE_PRE_BURN;
         mCommandedPreSeparationCoastTime = -1.0; // If used, the commanded value is cleared after satisfied.
      }
   }
   double deltaT = aEndTime - aBegTime;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform Information Update:" << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime << " (" << aBegTime - mLaunchTime << ")";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Updated DT: " << deltaT;
      out.AddNote() << "ET: " << aEndTime << " (" << aEndTime - mLaunchTime << ") ";
   }

   // Ignore trivial updates resulting from breaking up the integration interval.
   if (deltaT > 1.0E-8)
   {
      if (mLaunchPlatformIndex == 0)
      {
         AdvanceTime(aSimTime, aBegTime, aEndTime, nextPhase);
      }
      else
      {
         PrelaunchAdvanceTime(aSimTime, aEndTime);
      }
   }

   // Check to see if there is a change of phase
   if (mPhase != nextPhase)
   {
      // Ensure scripts and observers have the current state.
      static_cast<UtEntity&>(*GetPlatform()) = mExternalState;
      if (nextPhase == cPHASE_PRE_BURN)
      {
         if (SeparateStage(aSimTime, mStageIndex))
         {
            // Entering 'pre-burn' phase, which only occurs when the previous stage has completed.
            if (mShowStatus)
            {
               auto out = ut::log::info() << "Staging Message:";
               WriteStagingMessage(out, aSimTime, aEndTime, "separation", mStageIndex);
            }
            CallStagingEventScript(aSimTime, "on_stage_separation"); // Call scripts
            StageSeparation(aSimTime, aEndTime);                     // Call observers
            WsfObserver::MoverStaged(GetSimulation())(aSimTime, this);
            InitiateStage(aSimTime, aEndTime, mStageIndex + 1);
         }
         else
         {
            mBoostCompleteStageIndex = mStageIndex;
            mBoostComplete           = true;
            nextPhase                = cPHASE_FAIL;
            auto out                 = ut::log::warning() << "Staging Message:";
            WriteStagingMessage(out, aSimTime, aEndTime, "separation failure", mStageIndex);
         }
      }
      else if (nextPhase == cPHASE_BURN)
      {
         // If this is the first stage then sever the attachment to the launch platform.
         if (mStageIndex == 0)
         {
            PrelaunchComplete(aSimTime, aEndTime);
         }

         if (IgniteStage(aSimTime, mStageIndex))
         {
            // Light the fires....
            if (mShowStatus)
            {
               auto out = ut::log::info() << "Staging Message:";
               WriteStagingMessage(out, aSimTime, aEndTime, "ignition", mStageIndex);
            }
            CallStagingEventScript(aSimTime, "on_stage_ignition"); // Call scripts
            StageIgnition(aSimTime, aEndTime);                     // Call observers
         }
         else
         {
            mBoostCompleteStageIndex = mStageIndex;
            mBoostComplete           = true;
            nextPhase                = cPHASE_FAIL;
            auto out                 = ut::log::warning() << "Staging Message:";
            WriteStagingMessage(out, aSimTime, aEndTime, "ignition failure", mStageIndex);
         }
      }
      else if (nextPhase == cPHASE_POST_BURN)
      {
         // Entering 'post-burn' phase.
         //
         // Indicate if this stage terminates the boost phase
         bool prevBoostComplete = mBoostComplete;

         if (mStageIndex == mBoostCompleteStageIndex)
         {
            mBoostComplete = true;
         }
         if (mShowStatus)
         {
            auto out = ut::log::info() << "Post Burn staging message:";
            if (mStageIndex == mBoostCompleteStageIndex)
            {
               out.AddNote() << "T = " << std::fixed << std::setprecision(5) << aSimTime << " ("
                             << aEndTime - mLaunchTime << ")";
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Status: Boost phase complete.";
            }
            WriteStagingMessage(out, aSimTime, aEndTime, "burnout", mStageIndex);
         }

         CallStagingEventScript(aSimTime, "on_stage_burnout"); // Call scripts
         StageBurnout(aSimTime, aEndTime);                     // Call observers

         // Flag manipulations to assure this callback is only generated once for a given Mover.
         if (!prevBoostComplete && mBoostComplete)
         {
            WsfObserver::MoverBurnedOut(GetSimulation())(aEndTime, this);
         }
      }
      mPhase          = nextPhase;
      mPhaseStartTime = aEndTime;
   }
}
// ================================================================================================
const WsfRoute* WsfGuidedMoverBase::GetRoute() const // override
{
   return &mRoute;
}

// ================================================================================================
bool WsfGuidedMoverBase::SetRoute(double          aSimTime,
                                  const WsfRoute& aRoute) // override
{
   mRoute = aRoute;
   RouteChanged(aSimTime); // Inform callback subscribers
   if (GetSimulation() != nullptr)
   {
      WsfObserver::RouteChanged(GetSimulation())(this);
   }
   return true;
}

// ================================================================================================
bool WsfGuidedMoverBase::UpdateRoute(double          aSimTime,
                                     const WsfRoute& aRoute) // override
{
   mRoute = aRoute;
   RouteChanged(aSimTime); // Inform callback subscribers
   if (GetSimulation() != nullptr)
   {
      WsfObserver::RouteChanged(GetSimulation())(this);
   }
   return true;
}

// ================================================================================================
void WsfGuidedMoverBase::EjectStage(double aSimTime, double aPreSeparationCoastTime, double aPreIgnitionCoastTime)
{
   mCommandedEngineStopTime         = aSimTime;
   mCommandedPreSeparationCoastTime = aPreSeparationCoastTime;
   mCommandedPreIgnitionCoastTime   = aPreIgnitionCoastTime;
}

// ================================================================================================
//! A helper function to write the kinematic status of the mover to stdout.
//! This function is invoked by the mover and the guidance computer to write out a common
//! status message at important events. It is expected that the caller has already written
//! out the introductory line that indicates the event that occurred.
void WsfGuidedMoverBase::WriteKinematicStatus(ut::log::MessageStream& aStream)
{
   if (mFMM_Time < 0.0)
   {
      return; // platform has not started moving on its own
   }

   // Ground range is estimated using a spherical earth.
   double lat;
   double lon;
   double alt;
   double launchLocWCS[3];
   double weaponLocWCS[3];
   UtEntity::ConvertWCSToLLA(mFMM_LocWCS, lat, lon, alt);
   UtSphericalEarth::ConvertLLAToECEF(lat, lon, alt, launchLocWCS);
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   UtSphericalEarth::ConvertLLAToECEF(lat, lon, alt, weaponLocWCS);
   double cosTheta = UtVec3d::DotProduct(weaponLocWCS, launchLocWCS) /
                     std::max(UtVec3d::Magnitude(weaponLocWCS) * UtVec3d::Magnitude(launchLocWCS), 1.0);
   cosTheta         = std::min(std::max(cosTheta, -1.0), 1.0);
   double downRange = UtSphericalEarth::cEARTH_RADIUS * acos(cosTheta);

   double locWCS[3];
   GetPlatform()->GetLocationWCS(locWCS);
   double aclECS[3];
   GetPlatform()->GetAccelerationECS(aclECS);

   double speed = GetPlatform()->GetSpeed();
   double hdg;
   double pitch;
   double roll;
   GetPlatform()->GetOrientationNED(hdg, pitch, roll);

   aStream << std::fixed;
   aStream.AddNote() << "Lat: " << lat;
   aStream.AddNote() << "Lon: " << lon;
   aStream.AddNote() << "Alt: " << std::setprecision(0) << alt << " m";
   aStream.AddNote() << "Downrange: " << std::setprecision(0) << downRange << " m";
   aStream.AddNote() << "Mass: " << std::setprecision(2) << GetPlatform()->GetMass() << " kg";
   aStream.AddNote() << "Speed: " << std::setprecision(0) << speed << " m/s";
   aStream.AddNote() << "Mach: " << std::setprecision(2) << speed / mAtmosphere.SonicVelocity(alt);
   aStream.AddNote() << "Acceleration ECS: " << std::setprecision(2) << aclECS[0] / UtEarth::cACCEL_OF_GRAVITY << " g, "
                     << aclECS[1] / UtEarth::cACCEL_OF_GRAVITY << " g, " << aclECS[2] / UtEarth::cACCEL_OF_GRAVITY
                     << " g";
   aStream.AddNote() << "Heading: " << std::setprecision(4) << hdg * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Pitch: " << std::setprecision(4) << pitch * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Roll: " << std::setprecision(4) << roll * UtMath::cDEG_PER_RAD << " deg";
}

// ================================================================================================
// protected virtual
void WsfGuidedMoverBase::CallStagingEventScript(double aSimTime, const std::string& aScriptName)
{
   UtScript* scriptPtr = GetPlatform()->GetScriptContext().FindScript(aScriptName);
   if (scriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      UtScriptData     stageNumber(static_cast<int>(mStageIndex + 1));
      scriptArgs.push_back(stageNumber);
      GetPlatform()->GetScriptContext().ExecuteScript(aSimTime, scriptPtr, scriptRetVal, scriptArgs);
   }
}

// ================================================================================================
//! Stub method that is called when the first stage ignition actually occurs.
//! @param aSimTime The current simulation time.
//! @param aCurTime The current missile time. May be less than the simulation time.
// protected virtual
void WsfGuidedMoverBase::InitiateMotion(double aSimTime, double aCurTime)
{
   // Base class does nothing.
}

// ================================================================================================
//! The base mover only supports a single stage.
bool WsfGuidedMoverBase::SeparateStage(double aSimTime, size_t aStageIndex)
{
   return true;
}

// ================================================================================================
//! The base mover only supports a single stage.
bool WsfGuidedMoverBase::IgniteStage(double aSimTime, size_t aStageIndex)
{
   return true;
}

// ================================================================================================
// protected virtual
void WsfGuidedMoverBase::InitiateStage(double aSimTime, double aCurTime, size_t aStageIndex)
{
   // Base class only supports a single stage
   if (aStageIndex > 0)
   {
      { // RAII block
         auto out = ut::log::error() << "Platform attempting to use stage index that is too large.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Stage: " << aStageIndex + 1;
         out.AddNote() << "Max Stage: 1";
      }
      abort();
   }
   mStageIndex = aStageIndex;

   mPhase                  = cPHASE_PRE_BURN;
   mPhaseStartTime         = aCurTime;
   mPreIgnitionCoastTime   = 0.0;
   mPreSeparationCoastTime = 0.5 * DBL_MAX;
}

// ================================================================================================
//! Advance time while still attached to the launch platform.
// protected
void WsfGuidedMoverBase::PrelaunchAdvanceTime(double aSimTime, double aCurTime)
{
   // If the launching platform exists, update the weapon position so it stays locked to the
   // platform. If the launching platform has died then we must also die.
   WsfPlatform* launcherPtr = GetSimulation()->GetPlatformByIndex(mLaunchPlatformIndex);
   if (launcherPtr != nullptr)
   {
      launcherPtr->Update(aSimTime);
      SetInitialState(aSimTime, aCurTime, false);
   }
   else
   {
      // If an engagement object exists (like it should) then terminate the engagement.
      // Otherwise simply delete the platform.
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
      if (engagementPtr != nullptr)
      {
         engagementPtr->Terminate(aSimTime, WsfWeaponEngagement::cDUD);
      }
      else
      {
         GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
      }
   }
}

// ================================================================================================
//! Sever the attachment to the launch platform.
//! This is called once the 'pre-ignition coast time' for the first stage has expired.
//! It initializes the kinematic state of the weapon in preparation for free flight.
// protected
void WsfGuidedMoverBase::PrelaunchComplete(double aSimTime, double aCurTime)
{
   mLaunchPlatformIndex = 0; // Prevent further calls
   mLaunchWeaponPtr     = nullptr;

   // Set the kinematic state of this platform.
   SetInitialState(aSimTime, aCurTime, true);

   // Inform the derived class that we are no longer attached to the launching platform.
   InitiateMotion(aSimTime, aCurTime);
}

// ================================================================================================
//! Set/update the initial kinematic state of the platform.
//!
//! This method is called AT LEAST ONCE while the platform is still attached to the launching
//! platform (i.e.: on the pad or rail). While still attached to the launching platform, the state
//! of this platform is moved along with the motion of the launching platform. The final call to
//! this method is made just when this platform is to start moving on its own.
//!
//! A platform which has a non-zero value for 'pre_ignition_coast_time' for the first stage will
//! result in this routine being called repeatedly to move this platform along with the launching
//! platform. If the value is zero it will be called only once during initialization.
//!
//! @param aSimTime The current simulation time.
//! @param aCurTime The current missile time (may be less than aSimTime)
//! @param aInitiatingMotion true if this is the final call and this platform is about to start
//! moving on its own.
// protected
void WsfGuidedMoverBase::SetInitialState(double aSimTime, double aCurTime, bool aInitiatingMotion)
{
   double locWCS[3];
   double velWCS[3];
   double aclWCS[3];
   double psiWCS;
   double thetaWCS;
   double phiWCS;

   WsfWeapon* weaponPtr = mLaunchWeaponPtr;
   if (weaponPtr == nullptr)
   {
      GetPlatform()->GetLocationWCS(locWCS);
      mExternalState.SetLocationWCS(locWCS);
      GetPlatform()->GetVelocityWCS(velWCS);
      mExternalState.SetVelocityWCS(velWCS);
      GetPlatform()->GetAccelerationWCS(aclWCS);
      mExternalState.SetAccelerationWCS(aclWCS);
      GetPlatform()->GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
      mExternalState.SetOrientationWCS(psiWCS, thetaWCS, phiWCS);
   }
   else
   {
      // We're currently attached to the launch platform and we're within the pre-ignition
      // coast time of the first stage.

      weaponPtr->UpdatePosition(aSimTime);
      weaponPtr->GetLocationWCS(locWCS);
      mExternalState.SetLocationWCS(locWCS);

      // If motion is being initiated (i.e.: this is the last call while still attached to
      // the launch platform) then also apply the ejection velocity.

      weaponPtr->GetPlatform()->GetVelocityWCS(velWCS);
      if (aInitiatingMotion)
      {
         // If the launcher was a WsfExplicitWeapon then apply special processing.

         auto* expWeaponPtr = dynamic_cast<WsfExplicitWeapon*>(weaponPtr);
         if (expWeaponPtr != nullptr)
         {
            if (expWeaponPtr->GetIgnoreLaunchPlatformVelocity())
            {
               UtVec3d::Set(velWCS, 0.0);
            }

            // Get the ejection velocity, convert from PCS to WCS and add to velocity.
            double launchDeltaV_PCS[3];
            expWeaponPtr->GetLaunchDeltaV(launchDeltaV_PCS);
            double launchDeltaV_WCS[3];
            weaponPtr->ConvertPCSVectorToWCS(launchDeltaV_WCS, launchDeltaV_PCS);
            UtVec3d::Add(velWCS, velWCS, launchDeltaV_WCS);
         }

         // If the resulting velocity is close to zero then set it to something else.
         if (UtVec3d::Magnitude(velWCS) < 1.0)
         {
            // Resulting velocity is zero.  We have to give it something or things go berserk.
            double ejectionVelPCS[3] = {10.0, 0.0, 0.0};
            weaponPtr->ConvertPCSVectorToWCS(velWCS, ejectionVelPCS);
         }
         mExternalState.SetVelocityWCS(velWCS);
         UtVec3d::Set(aclWCS, 0.0);
         mExternalState.SetAccelerationWCS(aclWCS);

         // Align the vehicle along the velocity vector, with no roll.

         double velNED[3];
         mExternalState.GetVelocityNED(velNED);
         double vel = UtVec3d::Magnitude(velNED);
         if (vel > 0.001)
         {
            double heading = atan2(velNED[1], velNED[0]);
            double pitch   = asin(-velNED[2] / vel);
            double roll    = 0.0;
            mExternalState.SetOrientationNED(heading, pitch, roll);
         }
      }
      else
      {
         // We're simply being carried along with the launch platform
         mExternalState.SetVelocityWCS(velWCS);
         weaponPtr->GetPlatform()->GetAccelerationWCS(aclWCS);
         mExternalState.SetAccelerationWCS(aclWCS);
         weaponPtr->GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
         mExternalState.SetOrientationWCS(psiWCS, thetaWCS, phiWCS);
      }
   }

   if (aInitiatingMotion)
   {
      // Capture data for trajectory output
      mFMM_Time = aCurTime;
      GetPlatform()->GetLocationWCS(mFMM_LocWCS);
      double dummy;
      GetPlatform()->GetOrientationNED(mFMM_Heading, dummy, dummy);
   }
}

// ================================================================================================
//! Perform any post update actions to the platform.
//! This is called by the WsfGuidedMoverBase::Update just prior to exiting. Its main purpose is to allow
//! the derived class to perform any additional updates to the platform that could not easily be
//! See the derived class descriptions for the actions.
//! @param aSimTime The current simulation time.
void WsfGuidedMoverBase::PostUpdate(double aSimTime) {}

// ================================================================================================
// protected virtual
void WsfGuidedMoverBase::WriteStagingMessage(ut::log::MessageStream& aStream,
                                             double                  aSimTime,
                                             double                  aCurTime,
                                             const std::string&      aStatus,
                                             size_t                  aStageIndex)
{
   if (mShowStatus)
   {
      auto out = aStream.AddNote() << "Staging:" << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Since Launch: " << aCurTime - mLaunchTime << " sec";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Status: " << aStatus << " of stage " << aStageIndex + 1;

      WriteKinematicStatus(out);
   }
}

// ================================================================================================
//! Write trajectory output if requested by the user.
// protected virtual
void WsfGuidedMoverBase::WriteTrajectoryOutput(double aSimTime, double aCurTime, const double aECS_Forces[3])
{
   // Write trajectory output only if the platform has started moving on its own.
   if (mFMM_Time < 0.0)
   {
      return;
   }

   if (DebugEnabled())
   {
      auto   out = ut::log::debug() << "Trajectory Output:" << std::fixed;
      double lat;
      double lon;
      double altMSL;
      mExternalState.GetLocationLLA(lat, lon, altMSL);
      double hdg;
      double pitch;
      double roll;
      mExternalState.GetOrientationNED(hdg, pitch, roll);
      out.AddNote() << "T = " << std::setprecision(5) << aSimTime << " (" << aCurTime - mFMM_Time << ")";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Lat: " << UtLatPos(lat, UtAngle::FmtDMS + 2);
      out.AddNote() << "Lon: " << UtLonPos(lon, UtAngle::FmtDMS + 2);
      out.AddNote() << "Alt: " << std::setprecision(2) << altMSL << " m MSL";
      out.AddNote() << "V: " << mExternalState.GetSpeed();
      out.AddNote() << "Heading: " << hdg * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Roll: " << roll * UtMath::cDEG_PER_RAD << " deg";
   }

   if (mShowTrajectory || mTimeHistoryOfs.is_open())
   {
      // When ECI coordinates are being used there is initially a large lateral velocity component due to
      // Earth rotation. This shouldn't be reported as the time history file is really a WCS/LLA report.
      // In that case we just report the ECS X-axis velocity as the speed. (If we don't do this the
      // speed will be above Mach 1 right at the start!)
      double speed = 0.0;
      if (mExternalState.GetCoordinateFrame() == UtEntity::cCF_INERTIAL)
      {
         double velECS[3];
         mExternalState.GetVelocityECS(velECS);
         speed = velECS[0];
      }
      else
      {
         speed = mExternalState.GetSpeed();
      }
      double altitude = mExternalState.GetAltitude();
      double mass     = GetPlatform()->GetMass();
      double mach     = speed / mAtmosphere.SonicVelocity(altitude);
      double locWCS[3];
      mExternalState.GetLocationWCS(locWCS);
      double cosTheta = UtVec3d::DotProduct(locWCS, mFMM_LocWCS) /
                        std::max(UtVec3d::Magnitude(locWCS) * UtVec3d::Magnitude(mFMM_LocWCS), 1.0);
      cosTheta         = std::min(std::max(cosTheta, -1.0), 1.0);
      double theta     = acos(cosTheta);
      double downRange = theta * UtSphericalEarth::cEARTH_RADIUS;
      double forceToG  = 1.0 / (mass * UtEarth::cACCEL_OF_GRAVITY);
      double yForceAct = aECS_Forces[1] * forceToG;
      double zForceAct = aECS_Forces[2] * forceToG;
      if (mShowTrajectory)
      {
         auto out = ut::log::info() << "Trajectory:" << std::fixed;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Time of Flight: " << aCurTime - mFMM_Time << " sec";
         out.AddNote() << "Range: " << std::setprecision(1) << downRange << " m";
         out.AddNote() << "Altitude: " << std::setprecision(1) << altitude << " m";
         out.AddNote() << "Speed: " << std::setprecision(2) << speed << " m/s";
         out.AddNote() << "Mach: " << std::setprecision(4) << mach;
         out.AddNote() << "Y Force: " << std::setprecision(4) << yForceAct << " N";
         out.AddNote() << "Z Force: " << std::setprecision(4) << zForceAct << " N";
         out.AddNote() << "Mass: " << std::setprecision(2) << mass << " kg";
      }
      if (mTimeHistoryOfs.is_open())
      {
         double             yForceCmd    = mYForce * forceToG;
         double             zForceCmd    = mZForce * forceToG;
         std::ios::fmtflags oldFlags     = mTimeHistoryOfs.flags();
         std::streamsize    oldPrecision = mTimeHistoryOfs.precision();
         mTimeHistoryOfs.setf(std::ios_base::fixed, std::ios_base::floatfield);
         // clang-format off
         mTimeHistoryOfs << std::setw(8) << aCurTime - mFMM_Time
                         << ' ' << std::setprecision(1) << std::setw(8) << downRange
                         << ' ' << std::setprecision(1) << std::setw(8) << altitude
                         << ' ' << std::setprecision(2) << std::setw(8) << mass
                         << ' ' << std::setprecision(2) << std::setw(8) << speed
                         << ' ' << std::setprecision(4) << std::setw(8) << mach
                         << ' ' << std::setprecision(4) << std::setw(8) << yForceCmd
                         << ' ' << std::setprecision(4) << std::setw(8) << zForceCmd
                         << ' ' << std::setprecision(4) << std::setw(8) << yForceAct
                         << ' ' << std::setprecision(4) << std::setw(8) << zForceAct
                         << '\n';
         // clang-format on
         mTimeHistoryOfs.flags(oldFlags);
         mTimeHistoryOfs.precision(oldPrecision);
      }
   }
}
