// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Mover.hpp"

#include <iomanip>
#include <string>

#include "UtDCM.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLLAPos.hpp"
#include "UtLLPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfCallback.hpp"
#include "WsfDateTime.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMoverGuidance.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPartObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_AeroCoreObject.hpp"
#include "WsfSixDOF_CommonController.hpp"
#include "WsfSixDOF_EventPipe.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_GuidanceComputer.hpp"
#include "WsfSixDOF_JetEngine.hpp"
#include "WsfSixDOF_Observer.hpp"
#include "WsfSixDOF_PilotManager.hpp"
#include "WsfSixDOF_PilotObject.hpp"
#include "WsfSixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "WsfSixDOF_Utils.hpp"
#include "WsfTrackManager.hpp"
#include "WsfVisualPart.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "maneuvers/WsfSixDOF_Maneuver.hpp"
#include "maneuvers/WsfSixDOF_ManeuverSequence.hpp"

wsf::six_dof::Mover::Mover(const WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mEnvironment(static_cast<const Environment*>(aScenario.FindExtension("wsf_six_dof_environment")))
   , mKinematicState(mEnvironment)
{
   SetType("WSF_SIX_DOF_MOVER");

   // Set the default message rates to off.
   mEventPipeManager["MsgSixDOF_CoreData"]        = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_Kinematic"]       = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_EngineFuel"]      = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_Autopilot"]       = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_AutopilotLimits"] = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_ControlInputs"]   = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgSixDOF_ForceMoment"]     = std::pair<double, int>(0.0, 0);
}

wsf::six_dof::Mover::~Mover()
{
   // If there are any platforms that have not been added to the simulation we need to delete them.
   while (!mNewlyJettisonedPlatforms.empty())
   {
      mNewlyJettisonedPlatforms.pop();
   }
}

// =================================================================

wsf::six_dof::Mover::Mover(const Mover& aSrc)
   : WsfMover(aSrc)
   , RouteChanged()
   , StageIgnition()
   , StageBurnout()
   , StageSeparation()
   , mEnginesOffAtStart(aSrc.mEnginesOffAtStart)
   , mThrottleIdleAtStart(aSrc.mThrottleIdleAtStart)
   , mThrottleFullAtStart(aSrc.mThrottleFullAtStart)
   , mThrottleAfterburnerAtStart(aSrc.mThrottleAfterburnerAtStart)
   , mAutopilotNoControlAtStart(aSrc.mAutopilotNoControlAtStart)
   , mEnableThrustVectoringAtStart(aSrc.mEnableThrustVectoringAtStart)
   , mEnableControlsAtStart(aSrc.mEnableControlsAtStart)
   , mMaxGees(aSrc.mMaxGees)
   , mSpeed(aSrc.mSpeed)
   , mManeuverPlane(aSrc.mManeuverPlane)
   , mYGLoad(aSrc.mYGLoad)
   , mZGLoad(aSrc.mZGLoad)
   , mCommandedYawRate(aSrc.mCommandedYawRate)
   , mCommandedPitchRate(aSrc.mCommandedPitchRate)
   , mCommandedRollRate(aSrc.mCommandedRollRate)
   , mCommandedSpeed(aSrc.mCommandedSpeed)
   , mCommandedEngineStopTime(aSrc.mCommandedEngineStopTime)
   , mMinimumHeightAboveTerrain(aSrc.mMinimumHeightAboveTerrain)
   , mLastStickX(aSrc.mLastStickX)
   , mLastStickY(aSrc.mLastStickY)
   , mLastRudder(aSrc.mLastRudder)
   , mLastThrottle(aSrc.mLastThrottle)
   , mLastSpdBrakeLeverPosition(aSrc.mLastSpdBrakeLeverPosition)
   , mLastSpoilerLeverPosition(aSrc.mLastSpoilerLeverPosition)
   , mLastFlapsLeverPosition(aSrc.mLastFlapsLeverPosition)
   , mInitialPosition_Latitude(aSrc.mInitialPosition_Latitude)
   , mInitialPosition_Longitude(aSrc.mInitialPosition_Longitude)
   , mInitialAltitude_m(aSrc.mInitialAltitude_m)
   , mInitialNEDHeading(aSrc.mInitialNEDHeading)
   , mInitialNEDPitch(aSrc.mInitialNEDPitch)
   , mInitialNEDRoll(aSrc.mInitialNEDRoll)
   , mInitialSpeed_NED_X_fps(aSrc.mInitialSpeed_NED_X_fps)
   , mInitialSpeed_NED_Y_fps(aSrc.mInitialSpeed_NED_Y_fps)
   , mInitialSpeed_NED_Z_fps(aSrc.mInitialSpeed_NED_Z_fps)
   , mValidInitialPosition(aSrc.mValidInitialPosition)
   , mValidInitialAltitude_ft(aSrc.mValidInitialAltitude_ft)
   , mValidInitialNEDHeading(aSrc.mValidInitialNEDHeading)
   , mValidInitialNEDPitch(aSrc.mValidInitialNEDPitch)
   , mValidInitialNEDRoll(aSrc.mValidInitialNEDRoll)
   , mValidInitialSpeed_NED(aSrc.mValidInitialSpeed_NED)
   , mManeuverPtr(aSrc.mManeuverPtr)
   , mStageIndex(aSrc.mStageIndex)
   , mWsfRoutePtr(aSrc.mWsfRoutePtr)
   , mDefaultWsfRoutePtr(aSrc.mDefaultWsfRoutePtr)
   , mFollowVerticalTrack(aSrc.mFollowVerticalTrack)
   , mJokerFuel_lbs(aSrc.mJokerFuel_lbs)
   , mBingoFuel_lbs(aSrc.mBingoFuel_lbs)
   , mEventPipeManager(aSrc.mEventPipeManager)
   , mVisualPartManager(aSrc.mVisualPartManager)
   , mCallbacks()
   , mTypeName(aSrc.mTypeName)
   , mBaseName(aSrc.mBaseName)
   , mDerivedFromName(aSrc.mDerivedFromName)
   , mIntegratorTypeName(aSrc.mIntegratorTypeName)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mStepSize_nanosec(aSrc.mStepSize_nanosec)
   , mLifeTime_nanosec(aSrc.mLifeTime_nanosec)
   , mTimeOfFlight_nanosec(aSrc.mTimeOfFlight_nanosec)
   , mLastIsCaptive(aSrc.mLastIsCaptive)
   , mPosRelToParent_m(aSrc.mPosRelToParent_m)
   , mAngRelToParentYPR_rad(aSrc.mAngRelToParentYPR_rad)
   , mSeparationVel_fps(aSrc.mSeparationVel_fps)
   , mSeparationOmega_rps(aSrc.mSeparationOmega_rps)
   , mSequencerList(aSrc.mSequencerList)
   , mEnvironment(aSrc.mEnvironment)
   , mMassProperties(aSrc.mMassProperties)
   , mKinematicState(aSrc.mKinematicState)
   , mFreezeFlags(aSrc.mFreezeFlags)
   , mSizeFactor(aSrc.mSizeFactor)
   , mStallAngle(aSrc.mStallAngle)
   , mStallDetectionDeltaAngle(aSrc.mStallDetectionDeltaAngle)
   , mMaxMach(aSrc.mMaxMach)
   , mMaxAlpha_deg(aSrc.mMaxAlpha_deg)
   , mMinAlpha_deg(aSrc.mMinAlpha_deg)
   , mMaxBeta_deg(aSrc.mMaxBeta_deg)
   , mFixedObject(aSrc.mFixedObject)
   , mPrepositionLat(aSrc.mPrepositionLat)
   , mPrepositionLon(aSrc.mPrepositionLon)
   , mInitialLat(aSrc.mInitialLat)
   , mInitialLon(aSrc.mInitialLon)
   , mInitialAlt_m(aSrc.mInitialAlt_m)
   , mDownRangeDist_m(aSrc.mDownRangeDist_m)
   , mThrustTerminationTime_sec(aSrc.mThrustTerminationTime_sec)
   , mThrustTerminationTimeValid(aSrc.mThrustTerminationTimeValid)
   , mCrashed(aSrc.mCrashed)
   , mEngineOn(aSrc.mEngineOn)
   , mAfterburnerOn(aSrc.mAfterburnerOn)
   , mSmoking(aSrc.mSmoking)
   , mOnFire(aSrc.mOnFire)
   , mDamageSmokeTrailingEffect(aSrc.mDamageSmokeTrailingEffect)
   , mFlamesPresent(aSrc.mFlamesPresent)
   , mIsDestroyed(aSrc.mIsDestroyed)
   , mProducesLaunchSmoke(aSrc.mProducesLaunchSmoke)
   , mLaunchSmokeTime_sec(aSrc.mLaunchSmokeTime_sec)
   , mRisingSmokePlume(aSrc.mRisingSmokePlume)
   , mLaunchFlashSmoke(aSrc.mLaunchFlashSmoke)
   , mLightDamage(aSrc.mLightDamage)
   , mHeavyDamage(aSrc.mHeavyDamage)
   , mInitSetupMode(aSrc.mInitSetupMode)
   , mUseWashInInitConditions(aSrc.mUseWashInInitConditions)
   , mTestingSimTime_nanosec(aSrc.mTestingSimTime_nanosec)
{
   if (aSrc.mGuidancePtr)
   {
      // TODO -- Review this
      // mGuidancePtr = aSrc.mGuidancePtr->Clone();
   }

   for (auto& sequencer : mSequencerList)
   {
      sequencer->SetParentVehicle(this);
   }

   mSubObjectList.clear();
   for (auto& obj : aSrc.mSubObjectList)
   {
      auto subObject = dynamic_cast<Mover*>(obj->Clone());
      if (subObject)
      {
         subObject->SetParentVehicle(this);
         mSubObjectList.push_back(std::unique_ptr<Mover>{subObject});
      }
   }
}

bool wsf::six_dof::Mover::ProcessInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   if (WsfMover::ProcessInput(aInput))
   {
   }
   else if (command == "engines_on")
   {
      bool enginesOn = true;
      aInput.ReadValue(enginesOn);
      mEnginesOffAtStart = !enginesOn;
   }
   else if (command == "throttle_idle")
   {
      bool throttleIdle = false;
      aInput.ReadValue(throttleIdle);
      mThrottleIdleAtStart = throttleIdle;
   }
   else if (command == "throttle_full")
   {
      bool throttleFull = false;
      aInput.ReadValue(throttleFull);
      mThrottleFullAtStart = throttleFull;
   }
   else if (command == "throttle_afterburner")
   {
      bool throttleAfterburner = false;
      aInput.ReadValue(throttleAfterburner);
      mThrottleAfterburnerAtStart = throttleAfterburner;
   }
   else if (command == "autopilot_no_control")
   {
      bool autopilotNoControl = false;
      aInput.ReadValue(autopilotNoControl);
      mAutopilotNoControlAtStart = autopilotNoControl;
   }
   else if (command == "wash_in_conditions")
   {
      bool wash_in = false;
      aInput.ReadValue(wash_in);
      mUseWashInInitConditions = wash_in;
   }
   else if (command == "enable_thrust_vectoring")
   {
      bool enable = false;
      aInput.ReadValue(enable);
      mEnableThrustVectoringAtStart = enable;
   }
   else if (command == "enable_controls")
   {
      bool enable = false;
      aInput.ReadValue(enable);
      mEnableControlsAtStart = enable;
   }
   else if (command == "six_dof_position")
   {
      double lat = 0.0;
      double lon = 0.0;
      aInput.ReadValue(lat);
      aInput.ReadValue(lon);
      mInitialPosition_Latitude  = lat;
      mInitialPosition_Longitude = lon;
      mValidInitialPosition      = true;
   }
   else if (command == "six_dof_alt")
   {
      double alt = 0.0;
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);
      mInitialAltitude_m       = alt;
      mValidInitialAltitude_ft = true;
   }
   else if (command == "six_dof_ned_heading")
   {
      double hdg_rad = 0.0;
      aInput.ReadValueOfType(hdg_rad, UtInput::cANGLE);
      mInitialNEDHeading      = hdg_rad;
      mValidInitialNEDHeading = true;
   }
   else if (command == "six_dof_ned_pitch")
   {
      double pitch_rad = 0.0;
      aInput.ReadValueOfType(pitch_rad, UtInput::cANGLE);
      mInitialNEDPitch      = pitch_rad;
      mValidInitialNEDPitch = true;
   }
   else if (command == "six_dof_ned_roll")
   {
      double roll_rad = 0.0;
      aInput.ReadValueOfType(roll_rad, UtInput::cANGLE);
      mInitialNEDRoll      = roll_rad;
      mValidInitialNEDRoll = true;
   }
   else if (command == "six_dof_set_velocity_ned_fps")
   {
      double spdX = 0.0;
      double spdY = 0.0;
      double spdZ = 0.0;
      aInput.ReadValue(spdX);
      aInput.ReadValue(spdY);
      aInput.ReadValue(spdZ);
      mInitialSpeed_NED_X_fps = spdX;
      mInitialSpeed_NED_Y_fps = spdY;
      mInitialSpeed_NED_Z_fps = spdZ;
      mValidInitialSpeed_NED  = true;
   }
   else if (command == "six_dof_initsetup")
   {
      mUseWashInInitConditions = true;
   }
   else if (command == "follow_vertical_track")
   {
      // If follow vertical track is not set to true, set it to true
      if (!mFollowVerticalTrack)
      {
         mFollowVerticalTrack = true;
      }
   }
   else if (command == "produces_launch_smoke")
   {
      double smokeTime_sec = 0.0;
      aInput.ReadValueOfType(smokeTime_sec, UtInput::cTIME);
      mProducesLaunchSmoke = true;
      mLaunchSmokeTime_sec = smokeTime_sec;
   }
   else if (command == "event_core_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_CoreData"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_kinematic_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_Kinematic"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_engine_fuel_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_EngineFuel"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_autopilot_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_Autopilot"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_autopilot_limits_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_AutopilotLimits"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_control_inputs_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_ControlInputs"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_force_moment_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_ForceMoment"] = std::pair<double, int>(dataRate, 0);
   }
   else
   {
      return false;
   }

   return true;
}

bool wsf::six_dof::Mover::ProcessVehicleInput(UtInput& aInput)
{
   const std::string& command     = aInput.GetCommand();
   auto               typeManager = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));

   if (command == "mass_properties")
   {
      mMassProperties.ProcessInput(aInput);
   }
   else if (command == "sequencer")
   {
      auto tempSequencer = ut::make_unique<Sequencer>();
      tempSequencer->ProcessInput(aInput);

      std::string sequencerName = tempSequencer->GetName();
      for (auto& iter : mSequencerList)
      {
         if (iter->GetName() == sequencerName)
         {
            throw UtInput::BadValue(aInput, "Duplicate sequencer name (" + sequencerName + ")");
         }
      }

      mSequencerList.push_back(std::move(tempSequencer));
   }
   else if (command == "use_spherical_earth")
   {
      bool value;
      aInput.ReadValue(value);
      mKinematicState.SetUseSphericalEarth(value);
   }
   else if (command == "use_rotating_earth")
   {
      bool value;
      aInput.ReadValue(value);
      mKinematicState.SetUseRotatingEarth(value);
   }
   else if (command == "ignore_jettisoned_objects")
   {
      bool value;
      aInput.ReadValue(value);
      mKinematicState.SetIgnoreJettisonObjects(value);
   }
   else if (command == "parent_rel_x")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cLENGTH);
      double x, y, z;
      mPosRelToParent_m.Get(x, y, z);
      mPosRelToParent_m.Set(num, y, z);
   }
   else if (command == "parent_rel_y")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cLENGTH);
      double x, y, z;
      mPosRelToParent_m.Get(x, y, z);
      mPosRelToParent_m.Set(x, num, z);
   }
   else if (command == "parent_rel_z")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cLENGTH);
      double x, y, z;
      mPosRelToParent_m.Get(x, y, z);
      mPosRelToParent_m.Set(x, y, num);
   }
   else if (command == "parent_rel_yaw")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGLE);
      double y, p, r;
      mAngRelToParentYPR_rad.Get(y, p, r);
      mAngRelToParentYPR_rad.Set(num, p, r);
   }
   else if (command == "parent_rel_pitch")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGLE);
      double y, p, r;
      mAngRelToParentYPR_rad.Get(y, p, r);
      mAngRelToParentYPR_rad.Set(y, num, r);
   }
   else if (command == "parent_rel_roll")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGLE);
      double y, p, r;
      mAngRelToParentYPR_rad.Get(y, p, r);
      mAngRelToParentYPR_rad.Set(y, p, num);
   }
   else if (command == "separation_vx")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cSPEED);
      num *= UtMath::cFT_PER_M;
      double x, y, z;
      mSeparationVel_fps.Get(x, y, z);
      mSeparationVel_fps.Set(num, y, z);
   }
   else if (command == "separation_vy")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cSPEED);
      num *= UtMath::cFT_PER_M;
      double x, y, z;
      mSeparationVel_fps.Get(x, y, z);
      mSeparationVel_fps.Set(x, num, z);
   }
   else if (command == "separation_vz")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cSPEED);
      num *= UtMath::cFT_PER_M;
      double x, y, z;
      mSeparationVel_fps.Get(x, y, z);
      mSeparationVel_fps.Set(x, y, num);
   }
   else if (command == "separation_omega_x")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
      double x, y, z;
      mSeparationOmega_rps.Get(x, y, z);
      mSeparationOmega_rps.Set(num, y, z);
   }
   else if (command == "separation_omega_y")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
      double x, y, z;
      mSeparationOmega_rps.Get(x, y, z);
      mSeparationOmega_rps.Set(x, num, z);
   }
   else if (command == "separation_omega_z")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
      double x, y, z;
      mSeparationOmega_rps.Get(x, y, z);
      mSeparationOmega_rps.Set(x, y, num);
   }
   else if (command == "size_factor_radius")
   {
      double num;
      aInput.ReadValueOfType(num, UtInput::cLENGTH);
      mSizeFactor.SetRefSizeFactorRadius_m(num);
   }
   else if (command == "size_factor_min")
   {
      double num;
      aInput.ReadValue(num);
      mSizeFactor.SetSizeFactorMin(num);
   }
   else if (command == "size_factor_max")
   {
      double num;
      aInput.ReadValue(num);
      mSizeFactor.SetSizeFactorMax(num);
   }
   else if (command == "size_factor_volume_rate_m3_per_sec")
   {
      double num;
      aInput.ReadValue(num);
      mSizeFactor.SetSizeFactorVolumeRate_m3_perSec(num);
   }
   else if (command == "size_factor_area_rate_m2_per_sec")
   {
      double num;
      aInput.ReadValue(num);
      mSizeFactor.SetSizeFactorAreaRate_m2_perSec(num);
   }
   else if (command == "size_factor_radius_rate_m_per_sec")
   {
      double num;
      aInput.ReadValue(num);
      mSizeFactor.SetSizeFactorRadiusRate_m_perSec(num);
   }
   else if (command == "fixed_object")
   {
      bool value;
      aInput.ReadValue(value);
      mFixedObject = value;
   }
   else if (command == "nominal_max_mach")
   {
      double value;
      aInput.ReadValue(value);
      mMaxMach = value;
   }
   else if (command == "nominal_max_alpha")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cANGLE);
      mMaxAlpha_deg = value * UtMath::cDEG_PER_RAD;
   }
   else if (command == "nominal_min_alpha")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cANGLE);
      mMinAlpha_deg = value * UtMath::cDEG_PER_RAD;
   }
   else if (command == "nominal_max_beta")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cANGLE);
      mMaxBeta_deg = value * UtMath::cDEG_PER_RAD;
   }
   else if (command == "integrator")
   {
      std::string value;
      aInput.ReadValue(value);
      mIntegratorTypeName = value;
   }
   else if (command == "subobject")
   {
      typeManager->ProcessSubObjectInput(aInput, mSubObjectList);
   }
   else if (command == "fuel_transfer")
   {
      if (GetPropulsionSystem() == nullptr)
      {
         ut::log::error() << "WsfSixDOF_Mover is trying to setup a fuel transfer without a PropulsionSystem in "
                             "WsfSixDOF_Mover::ProcessInput.";
         throw UtInput::UnknownCommand(aInput);
      }

      std::string xferName;
      aInput.ReadValue(xferName);

      if (!GetPropulsionSystem()->FuelTransferNameUnique(xferName))
      {
         auto out = ut::log::error() << "WsfSixDOF_Mover::ProcessInput() is trying to setup a fuel transfer which is "
                                        "not uniquely named in WsfSixDOF_Mover::ProcessInput.";
         out.AddNote() << "Transfer: " << xferName;
         throw UtInput::UnknownCommand(aInput);
      }

      std::string sourceTankName;
      std::string targetTankName;

      UtInputBlock subBlock(aInput, "end_fuel_transfer");

      while (subBlock.ReadCommand())
      {
         std::string localCommand;
         localCommand = subBlock.GetCommand();

         if (localCommand == "source_tank")
         {
            aInput.ReadValue(sourceTankName);
         }
         else if (localCommand == "target_tank")
         {
            aInput.ReadValue(targetTankName);
         }
      }

      if (!sourceTankName.empty() && !targetTankName.empty())
      {
         GetPropulsionSystem()->AddFuelTransfer(xferName, sourceTankName, targetTankName);
      }
      else
      {
         if (sourceTankName.empty())
         {
            auto out = ut::log::error() << "Unable to find source fuel tank for transfer in Mover::ProcessInput().";
            out.AddNote() << "Fuel Tank: " << sourceTankName;
            out.AddNote() << "Transfer: " << xferName;
         }
         if (targetTankName.empty())
         {
            auto out = ut::log::error() << "Unable to find target fuel tank for transfer in Mover::ProcessInput().";
            out.AddNote() << "Fuel Tank: " << targetTankName;
            out.AddNote() << "Transfer: " << xferName;
         }
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "remove_fuel_transfer")
   {
      if (GetPropulsionSystem() == nullptr)
      {
         ut::log::error()
            << "Mover is trying remove a fuel transfer without a PropulsionSystem in Mover::ProcessInput.";
         throw UtInput::UnknownCommand(aInput);
      }

      std::string xferName;
      aInput.ReadValue(xferName);

      if (!GetPropulsionSystem()->RemoveFuelTransfer(xferName))
      {
         auto out = ut::log::error() << "Cannot remove a fuel transfer in Mover::ProcessInput().";
         out.AddNote() << "Transfer: " << xferName;
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "remove_fuel_tank")
   {
      if (GetPropulsionSystem() == nullptr)
      {
         ut::log::error()
            << "Mover is trying remove a fuel transfer without a PropulsionSystem in Mover::ProcessInput.";
         throw UtInput::UnknownCommand(aInput);
      }

      std::string tankName;
      aInput.ReadValue(tankName);

      if (!GetPropulsionSystem()->RemoveFuelTankByName(tankName))
      {
         auto out = ut::log::error() << "Cannot remove a fuel transfer in Mover::ProcessInput().";
         out.AddNote() << "Fuel Tank: " << tankName;
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "modify_fuel_quantity")
   {
      if (GetPropulsionSystem() == nullptr)
      {
         ut::log::error()
            << "Mover is trying modify a fuel quantity without a PropulsionSystem in Mover::ProcessInput.";
         throw UtInput::UnknownCommand(aInput);
      }

      std::string tankName;
      aInput.ReadValue(tankName);

      UtInputBlock subBlock(aInput, "end_modify_fuel_quantity");

      while (subBlock.ReadCommand())
      {
         std::string localCommand;
         localCommand = subBlock.GetCommand();

         if (localCommand == "fuel_quantity")
         {
            double fuelQty_lbs = 0.0;
            aInput.ReadValueOfType(fuelQty_lbs, UtInput::cMASS);
            fuelQty_lbs *= UtMath::cLB_PER_KG;
            GetPropulsionSystem()->ModifyFuelQuantity(tankName, fuelQty_lbs);
         }
      }
   }
   else if (command == "remove_sequencer")
   {
      std::string seqname;
      aInput.ReadValue(seqname);

      if (!RemoveSequencer(seqname))
      {
         auto out = ut::log::error() << "Cannot remove sequencer in Mover::ProcessInput().";
         out.AddNote() << "Sequencer: " << seqname;
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else
   {
      return false;
   }

   return true;
}

void wsf::six_dof::Mover::UpdateEventPipeData(double aSimTime)
{
   // Check over out event pipe manager data to see if messages need to be sent
   for (auto& eventPipeMessage : mEventPipeManager)
   {
      // Check if this message has a message rate above 0
      if (eventPipeMessage.second.first > 0.0)
      {
         // Difference in simtime from the (number of updates sent * update rate), doing it this way should prevent
         // floating point drift/loss of precision
         double diff = (aSimTime - static_cast<double>(eventPipeMessage.second.second * eventPipeMessage.second.first));

         // If we're higher than the message timing threshold, send a message
         if (diff >= eventPipeMessage.second.first)
         {
            eventPipeMessage.second.second++;
            WsfObserver::SixDOF_SendMessage(GetSimulation())(aSimTime, *GetPlatform(), *this, eventPipeMessage.first);
         }
      }
   }
}

void wsf::six_dof::Mover::DeriveFrom(const Mover& aSrc)
{
   mDerivedFromName = aSrc.GetName();
   mBaseName        = aSrc.GetBaseName();

   mMassProperties = aSrc.mMassProperties;
   mKinematicState = aSrc.mKinematicState;

   mSequencerList.clear();
   for (auto& sequencer : aSrc.mSequencerList)
   {
      sequencer->SetParentVehicle(this);
      mSequencerList.push_back(sequencer);
   }

   mSubObjectList.clear();
   for (auto& obj : aSrc.mSubObjectList)
   {
      auto subObject = dynamic_cast<Mover*>(obj->Clone());
      if (subObject)
      {
         subObject->SetParentVehicle(this);
         mSubObjectList.push_back(std::unique_ptr<Mover>{subObject});
      }
   }

   mSizeFactor            = aSrc.mSizeFactor;
   mPosRelToParent_m      = aSrc.mPosRelToParent_m;
   mAngRelToParentYPR_rad = aSrc.mAngRelToParentYPR_rad;
   mSeparationVel_fps     = aSrc.mSeparationVel_fps;
   mSeparationOmega_rps   = aSrc.mSeparationOmega_rps;
   mFixedObject           = aSrc.mFixedObject;

   mIntegratorTypeName = aSrc.mIntegratorTypeName;

   mMaxMach      = aSrc.mMaxMach;
   mMaxAlpha_deg = aSrc.mMaxAlpha_deg;
   mMinAlpha_deg = aSrc.mMinAlpha_deg;
   mMaxBeta_deg  = aSrc.mMaxBeta_deg;
}

void wsf::six_dof::Mover::SetNamePrefix(const std::string& aPrefixName)
{
   if (mBaseName.empty())
   {
      // No base name
      SetName(aPrefixName);
   }
   else
   {
      // Using base name
      SetName(aPrefixName + "_" + mBaseName);
   }
}

void wsf::six_dof::Mover::TestingUpdate(double aAltitude_ft,
                                        double aSpeed_fps,
                                        double aPitchAngle_deg,
                                        double aStickBack,
                                        double aThrottleForward,
                                        double aFlapsDown)
{
   // Set freeze flags
   mFreezeFlags.location     = true;
   mFreezeFlags.altitude     = true;
   mFreezeFlags.fuelBurn     = true;
   mFreezeFlags.testingNoLag = true;

   PilotObject* pilot = GetActivePilotObject();

   if (pilot)
   {
      pilot->TakeTestControl();

      pilot->SetTestFlapsControllerPosition(aFlapsDown);
      pilot->SetTestStickBackControllerPosition(aStickBack);
      // The stick and rudder values are +/-1 while all others are 0-1.
      // Throttle assumes that 0 is idle, 1 is full power without
      // augmentation/afterburner (military power), and 2 is full power
      // with full augmentation/afterburner (full afterburner)
      pilot->SetTestThrottleControllerPosition(aThrottleForward);
   }

   // Clear various state parameters
   UtVec3dX temp(0.0, 0.0, 0.0);
   mKinematicState.SetOmegaBody(temp);
   mKinematicState.SetOmegaBodyDot(temp);
   mKinematicState.SetAlphaBetaDotToZero();

   // Increment testing sim time
   mTestingSimTime_nanosec += 10000000; // 0.01 sec

   // Set testing state
   SetLocationLLA(0, 0, aAltitude_ft * UtMath::cM_PER_FT);
   SetAttitudeNED_rad(0, aPitchAngle_deg * UtMath::cRAD_PER_DEG, 0);
   SetVelocityNED_mps(aSpeed_fps * UtMath::cM_PER_FT, 0, 0);

   // Be sure to update/set secondary parameters such as alpha
   mKinematicState.CalculateSecondaryParameters();

   Update(mTestingSimTime_nanosec);
}

bool wsf::six_dof::Mover::Initialize(double aSimTime)
{
   // We need to check the platform to see if it has already been given a position
   // This can occur, for example, when a weapon is launched
   WsfPlatform* parentPlatform        = GetPlatform();
   double       platformLat           = 0.0;
   double       platformLon           = 0.0;
   double       platformAlt_m         = 0.0;
   double       platformVelNED_mps[3] = {0, 0, 0};
   double       platformHeading_rad   = 0;
   double       platformPitch_rad     = 0;
   double       platformRoll_rad      = 0;
   bool         platformDataAvailable = false;
   parentPlatform->GetLocationLLA(platformLat, platformLon, platformAlt_m);

   // TODO Why do we have a special case here?
   // We check for a "zero case" for the platform location, which is a location
   // with a LLA of (0,0,0)

   if ((platformLat > -0.000001) && (platformLat < 0.000001) && (platformLon > -0.000001) && (platformLon < 0.000001) &&
       (platformAlt_m < 0.000001))
   {
      // The platform has no data
   }
   else
   {
      // The platform has data, so get it
      parentPlatform->GetVelocityNED(platformVelNED_mps);
      parentPlatform->GetOrientationNED(platformHeading_rad, platformPitch_rad, platformRoll_rad);
      platformDataAvailable = true;
   }

   mCallbacks.Add(SubobjectJettisoned.Connect(&Mover::HandleSubobjectJettison, this));

   std::string platformName = GetPlatform()->GetName();

   // Initialize the MRM object
   Initialize(aSimTime, platformName);

   parentPlatform->SetFuelMass(GetTotalFuelRemaining());
   parentPlatform->SetEmptyMass(GetEmptyWeight_kg());

   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      mCallbacks.Add(pilot->WaypointAchieved.Connect(&Mover::ExecuteTriggersCB, this));

      // TODO - Verify that removing this external speed brake control was okay and is not needed
      // CommonController* autopilot = pilot->GetCommonController();
      // if (autopilot != 0)
      // {
      //    autopilot->SetExternalSpeedBrakesControl(true);
      //    autopilot->SetSpeedBrakesPosition(0.0);
      //    //mPilotController->GetAutoPilot()->SetSpeedBrakeEnabled(false); //if needed, try later
      // }
   }

   // Setup route of "planned waypoints" if AFSIM route defined on input
   if (mWsfRoutePtr != nullptr)
   {
      mDefaultWsfRoutePtr                       = mWsfRoutePtr->Clone();
      const WsfRoute::WaypointVector& waypoints = mWsfRoutePtr->GetWaypoints();
      if (!waypoints.empty())
      {
         if (pilot != nullptr)
         {
            pilot->SetPlannedRoute(ConvertWsfRouteToSixDOF_Route(mWsfRoutePtr));
         }

         double lat0, lon0, alt0;
         waypoints.at(0).GetLocationLLA(lat0, lon0, alt0);
         SetLocationLLA(lat0, lon0, alt0);

         double heading = 0.0;
         double pitch   = 0.0;
         if (waypoints.size() > 1)
         {
            double lat1, lon1, alt1;
            waypoints.at(1).GetLocationLLA(lat1, lon1, alt1);

            // heading degrees, distance meters
            double distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(lat0, lon0, lat1, lon1, heading, distance);
            double deltaAlt = alt1 - alt0;
            pitch           = atan2(deltaAlt, distance);
         }
         SetAttitudeNED_rad(heading * UtMath::cRAD_PER_DEG, pitch, 0.0);

         // Get the velocity from the waypoint -- we assume the waypoint is using "true" airspeed, not KIAS/KCAS or Mach
         double   velocityECS[3] = {waypoints.at(0).GetSpeed(), 0.0, 0.0};
         double   velocityWCS[3];
         UtEntity temp;
         temp.SetLocationLLA(lat0, lon0, alt0);
         temp.SetOrientationNED(heading * UtMath::cRAD_PER_DEG, pitch, 0.0);
         temp.ConvertECSVectorToWCS(velocityWCS, velocityECS);

         SetVelocityWCS_mps(velocityWCS[0], velocityWCS[1], velocityWCS[2]); // mps
      }
   }
   else
   {
      // TODO So if I don't have a route, I always start at 0,0?

      // No route, so start with a default location at LLA(0,0,0) with attitude of level headed north
      SetLocationLLA(0.0, 0.0, 0.0);
      SetAttitudeNED_rad(0.0, 0.0, 0.0);
   }

   // Set any initial conditions
   if (mValidInitialPosition)
   {
      // Get the current attitude
      double hdg, pitch, roll;
      GetAttitudeNED_rad(hdg, pitch, roll);

      // Get the altitude
      double lat, lon, alt;
      GetLocationLLA(lat, lon, alt);

      // Set the new position
      SetLocationLLA(mInitialPosition_Latitude, mInitialPosition_Longitude, alt);
      // Set the attitude
      SetAttitudeNED_rad(hdg, pitch, roll);
   }
   if (mValidInitialAltitude_ft)
   {
      double lat, lon, alt;
      GetLocationLLA(lat, lon, alt);
      SetLocationLLA(lat, lon, mInitialAltitude_m);
   }
   if (mValidInitialNEDHeading)
   {
      double hdg, pitch, roll;
      GetAttitudeNED_rad(hdg, pitch, roll);
      SetAttitudeNED_rad(mInitialNEDHeading, pitch, roll);
   }
   if (mValidInitialNEDPitch)
   {
      double hdg, pitch, roll;
      GetAttitudeNED_rad(hdg, pitch, roll);
      SetAttitudeNED_rad(hdg, mInitialNEDPitch, roll);
   }
   if (mValidInitialNEDRoll)
   {
      double hdg, pitch, roll;
      GetAttitudeNED_rad(hdg, pitch, roll);
      SetAttitudeNED_rad(hdg, pitch, mInitialNEDRoll);
   }
   if (mValidInitialPosition && mValidInitialAltitude_ft)
   {
      // Get the current attitude
      double hdg, pitch, roll;
      GetAttitudeNED_rad(hdg, pitch, roll);

      // Set the new position
      SetLocationLLA(mInitialPosition_Latitude, mInitialPosition_Longitude, mInitialAltitude_m);
      // Set the attitude
      SetAttitudeNED_rad(hdg, pitch, roll);
   }
   if (mValidInitialNEDHeading && mValidInitialNEDPitch && mValidInitialNEDRoll)
   {
      // Set the new attitude
      SetAttitudeNED_rad(mInitialNEDHeading, mInitialNEDPitch, mInitialNEDRoll);
   }
   if (mValidInitialSpeed_NED)
   {
      SetVelocityNED_mps(mInitialSpeed_NED_X_fps * UtMath::cM_PER_FT,
                         mInitialSpeed_NED_Y_fps * UtMath::cM_PER_FT,
                         mInitialSpeed_NED_Z_fps * UtMath::cM_PER_FT);
   }

   if (platformDataAvailable)
   {
      SetLocationLLA(platformLat, platformLon, platformAlt_m);
      SetAttitudeNED_rad(platformHeading_rad, platformPitch_rad, platformRoll_rad);
      SetVelocityNED_mps(platformVelNED_mps[0], platformVelNED_mps[1], platformVelNED_mps[2]);
   }

   SetWashInInitConditions(mUseWashInInitConditions);

   // Set timing
   // SetUpdateInterval(0.01); we use the interval set in the mover class
   SetUpdateTimeTolerance(0); // Do not accept anything that is "close enough"

   // Generate subobjects
   for (auto& it : GetSubobjectList())
   {
      UtQuaternion q;
      UtVec3dX     v;
      std::string  str     = it->GetBaseName();
      Mover*       vehicle = it.get();
      InitializeSubobject(aSimTime, str, vehicle, q, v);
   }

   mCallbacks.Add(CrashedIntoGround.Connect(&Mover::CrashIntoGroundCB, this));

   if (mEnginesOffAtStart)
   {
      PropulsionSystem* propulsion = GetPropulsionSystem();
      if (propulsion)
      {
         propulsion->Shutdown();
      }
   }

   if (mThrottleIdleAtStart)
   {
      MoveThrottleToIdle();
   }

   if (mThrottleFullAtStart)
   {
      MoveThrottleToFull();
   }

   if (mThrottleAfterburnerAtStart)
   {
      MoveThrottleToAfterburner();
   }

   if (mAutopilotNoControlAtStart)
   {
      SetAutopilotNoControl();
   }

   if (!mEnableThrustVectoringAtStart)
   {
      EnableThrustVectoring(false);
   }

   if (!mEnableControlsAtStart)
   {
      EnableControls(false);
   }

   InitializeConditions(aSimTime);

   UpdatePlatformFromVehicle(this, GetPlatform());

   return WsfMover::Initialize(aSimTime);
}

bool wsf::six_dof::Mover::InitializeConditions(double aSimTime_sec)
{
   mKinematicState.CalculateSecondaryParameters();

   ComputeWashInConditions(aSimTime_sec);

   // Remember the starting lat/lon (used with preposition support)
   mPrepositionLat = mKinematicState.GetLat();
   mPrepositionLon = mKinematicState.GetLon();

   return true;
}

bool wsf::six_dof::Mover::InitializeSequencers(double aSimTime_sec)
{
   size_t simTime_nanosec = utils::TimeToTime(aSimTime_sec);

   for (auto& sequencer : mSequencerList)
   {
      sequencer->SetParentVehicle(this);
      sequencer->Initialize(simTime_nanosec);
      sequencer->StartUp(simTime_nanosec);

      // Adjust the naming in the sequencers to use full names
      sequencer->AdjustNaming(GetName());
   }

   return true;
}

bool wsf::six_dof::Mover::InitializeSubobjects(double aSimTime_sec)
{
   for (auto& obj : mSubObjectList)
   {
      obj->SetParentVehicle(this);
      obj->SetCaptiveState(true);
      obj->SetLastCaptiveState(true);

      obj->Initialize(aSimTime_sec, GetName());
   }

   return true;
}

void wsf::six_dof::Mover::SetupStartingLatLon()
{
   if (mKinematicState.StartingLocationInitialized())
   {
      return;
   }

   if (mIsCaptive)
   {
      // Get data based on parent and relative location
      CopyDataFromParent();
      AdjustForParentInducedEffects();
   }

   double x, y, z;
   mKinematicState.GetLocationWCS().Get(x, y, z);

   double locWCS[3] = {x, y, z};
   mKinematicState.GetUtEntity().SetLocationWCS(locWCS);

   // Get LLA
   double lat, lon, alt_m;
   mKinematicState.GetUtEntity().GetLocationLLA(lat, lon, alt_m);
   mKinematicState.SetPositionLLA(lat, lon, alt_m);

   mKinematicState.SetStartingLatLon(mKinematicState.GetLat(), mKinematicState.GetLon());
   mPrepositionLat = mKinematicState.GetLat();
   mPrepositionLon = mKinematicState.GetLon();
}

void wsf::six_dof::Mover::SetTheLastValues()
{
   mKinematicState.SetTheLastValues();
   mLastIsCaptive = mIsCaptive;
}

bool wsf::six_dof::Mover::StallWarningAlert() const
{
   if (GetAlpha_rad() > (mStallAngle - mStallDetectionDeltaAngle))
   {
      return true;
   }
   else
   {
      return false;
   }
}

wsf::six_dof::PilotObject* wsf::six_dof::Mover::GetActivePilotObject() const
{
   if (GetPilotManager() != nullptr)
   {
      return GetPilotManager()->GetActivePilot();
   }
   return nullptr;
}

void wsf::six_dof::Mover::ComputeWashInConditions(double aSimTime_sec)
{
   // This section will handle "wash-in" initial conditions, to provide steady state conditions at start
   if (mUseWashInInitConditions)
   {
      // We only perform wash-in for objects that lack a parent, so ensure there is no parent
      if (mParentVehicle != nullptr)
      {
         // We have a parent, so we will not wash-in
         auto out = ut::log::warning() << "Wash-in will not be performed for vehicle since it is not a parent object.";
         out.AddNote() << "Vehicle: " << GetName();
      }
      else // mParentVehicle is null (which is good in this case)
      {
         CalculateWashIn(aSimTime_sec);
      }
   }
}

void wsf::six_dof::Mover::Update(int64_t aSimTime_nanosec)
{
   UpdateObject(aSimTime_nanosec);

   if (mIsCaptive)
   {
      // Copy state from parent
      CopyDataFromParent();
      AdjustForParentInducedEffects();
   }

   // WARNING: While looping over subObject and calling Update() on them,the subObject may be removed from the
   // mSubObjectList due to being jettisoned. This will change the mSubObjectList container, invalidating any
   // iterator pointing to removed element. The following code takes advantage of how list iterators work, the
   // code will not work properly if the container type is changed to vector or another standard type
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* vehicle = (*subObjIter).get();
      ++subObjIter;

      if (vehicle)
      {
         vehicle->Update(aSimTime_nanosec);
      }
   }
}

void wsf::six_dof::Mover::UpdateObject(int64_t aSimTime_nanosec)
{
   // Determine the starting LLA
   SetupStartingLatLon();

   // We currently do not use the sequencerTimeEvent, but it could be used to cause an intermediate update within
   // an update frame time. Determine when the next sequencer time event will occur
   // int64_t sequencerTimeEvent_nanosec = -1;
   // bool sequencerTimeEvent = NextSequencerEventTime_nanosec(aSimTime_nanosec, sequencerTimeEvent_nanosec);

   // Similar to the sequencerTimeEvent, we may need a means to determine when the next guidance-related event will occur.
   // This is important for ballistic missiles with very precise thrust cutoff timing needs. We currently do not handle this.

   // Process sequencers for this time step (if not in setup mode)
   if (!mInitSetupMode)
   {
      for (auto& sequencer : mSequencerList)
      {
         sequencer->Update(aSimTime_nanosec);
      }
   }

   // Return if the time is the same as it was during the last update
   if (aSimTime_nanosec == mLastSimTime_nanosec)
   {
      return;
   }

   // Delta T for target time in nanosec
   int64_t deltaT_nano = aSimTime_nanosec - mLastSimTime_nanosec;

   // dT (delta time) is typically 0.01 sec
   double deltaT_sec = utils::TimeToTime(deltaT_nano);

   if ((deltaT_nano < 0) || (deltaT_sec < 0))
   {
      auto out = ut::log::error() << "Delta-T is negative.";
      out.AddNote() << "T = " << utils::TimeToTime(aSimTime_nanosec);
      out.AddNote() << "Delta-T: " << deltaT_sec << " sec";
      out.AddNote() << "Vehicle: " << GetName();
      return;
   }

   // Increment the life time and flight times (if not setting up)
   if (!mInitSetupMode)
   {
      mLifeTime_nanosec += deltaT_nano;
      if (!mIsCaptive)
      {
         mTimeOfFlight_nanosec += deltaT_nano;
      }
   }

   // Set 'last' values
   SetTheLastValues();

   // Set the 'last' sim time to the current time
   mLastSimTime_nanosec = aSimTime_nanosec;

   // Fixed objects do not move, so simply return
   if (mFixedObject)
   {
      return;
   }

   // Determine how the vehicle's size may be changing, especially for parachutes and balloons
   CalculateSizeFactor(deltaT_sec);

   // Call the integrator to update/integrate the state
   GetIntegrator()->Update(aSimTime_nanosec, deltaT_sec);

   // Update the flight controls (which will also update the control inputs)
   UpdateFlightControls(aSimTime_nanosec);

   // Pass the delta angles to the PilotManager
   if (GetPilotManager() != nullptr)
   {
      double deltaYaw_rad, deltaPitch_rad, deltaRoll_rad;
      mKinematicState.GetAngleDeltas(deltaYaw_rad, deltaPitch_rad, deltaRoll_rad);
      GetPilotManager()->InputAngleDeltasToPilotObjects(deltaYaw_rad, deltaPitch_rad, deltaRoll_rad);
   }

   // This "handles" various ground contacts, such as crashing, hard landings, etc.
   HandleGroundContact(aSimTime_nanosec);
}

bool wsf::six_dof::Mover::Initialize(double aSimTime_sec, const std::string& aPrefixName)
{
   if (mIsInitialized)
   {
      // Avoid re-initializing on jettison, as this can have adverse effects
      return true;
   }

   mIsInitialized = true;

   // Set the last time to the time at initialization
   mLastSimTime_nanosec = utils::TimeToTime(aSimTime_sec);

   // Set the full name
   SetNamePrefix(aPrefixName);

   // Initialize subobjects, propagating the assigned earth model
   InitializeSubobjects(aSimTime_sec);
   SetSphericalEarthFlagOnSubobjects(mKinematicState.UseSphericalEarth());

   // Initialize sequencers on this object
   mIsInitialized &= InitializeSequencers(aSimTime_sec);

   mSizeFactor.InitializeValues();

   // Initialize pieces specific to derived classes
   mIsInitialized &= InitializeDetails(aSimTime_sec);

   // Determine the vehicle mass/inertia, including sub-objects
   CalculateCurrentMassProperties();

   CalcStallAngle();

   return mIsInitialized;
}

void wsf::six_dof::Mover::UpdateFuelBurn(int64_t aSimTime_nanosec, double aDeltaT_sec, const KinematicState& aState)
{
   if (GetPropulsionSystem() == nullptr)
   {
      return;
   }

   // Calc the thrust vector by doing an 'update' on the propulsion object and a 'calculate' on propulsion objects of any children
   double simTime_sec = utils::TimeToTime(aSimTime_nanosec);
   UpdatePropulsionFuelBurn(simTime_sec, aDeltaT_sec, aState);
}

void wsf::six_dof::Mover::HandleGroundContact(int64_t aSimTime_nanosec)
{
   // Check if already crashed
   if (mCrashed)
   {
      // Vehicle has already crashed, just return
      return;
   }

   // If we are ignoring all crashes, we just return
   if (mTestingIgnoreAllCrashes)
   {
      // Before we return, we will clamp to sea level, if appropriate
      if (mTestingClampToSeaLevelMinAlt)
      {
         // Is the vehicle below sea level?
         if (mKinematicState.GetAlt_m() < 0)
         {
            ClampToSeaLevelMinAlt();
            mKinematicState.CalculateSecondaryParameters();
         }
      }

      return;
   }

   // Get the terrain and use its height, if possible
   double terrainHgt_m = mEnvironment->GetTerrainElevation(mKinematicState.GetLat(), mKinematicState.GetLon());

   // Check for hit with ground

   if (mKinematicState.GetAlt_m() < terrainHgt_m) // Vehicle has hit the ground
   {
      // Crash!!! -- object has hit the ground
      mKinematicState.SetAlt(terrainHgt_m);

      // Zero the velocity
      mKinematicState.ZeroVelocity();

      // Set angular velocity to zero
      double vel[3] = {0.0, 0.0, 0.0};
      mKinematicState.SetOmegaBody(vel);

      // Set crashed flag
      if (!mTestingIgnoreAllCrashes)
      {
         mCrashed = true;
         PerformCrash(aSimTime_nanosec);
      }

   } // End hit ground
}

void wsf::six_dof::Mover::PerformCrash(int64_t aSimTime_nanosec)
{
   // Zero the velocity
   double vel[3] = {0.0, 0.0, 0.0};
   mKinematicState.SetVelocityWCS(vel);
   mKinematicState.SetOmegaBody(vel);

   // Clamp to seal level, as appropriate
   if (mTestingClampToSeaLevelMinAlt)
   {
      ClampToSeaLevelMinAlt();
   }

   // Re-calculate parameters that are useful to "outside" objects
   mKinematicState.CalculateSecondaryParameters();

   // Inform objects that are listening with a callback
   CrashedIntoGround(utils::TimeToTime(aSimTime_nanosec));
}

void wsf::six_dof::Mover::CalculateSizeFactor(double dT_sec)
{
   if (!mIsCaptive)
   {
      mSizeFactor.CalculateSizeFactor(dT_sec);
   }
}

void wsf::six_dof::Mover::SetSphericalEarthFlagOnSubobjects(bool aSetting)
{
   mKinematicState.SetUseSphericalEarth(aSetting);

   // Set sub-objects
   for (auto& iter : mSubObjectList)
   {
      iter->SetSphericalEarthFlagOnSubobjects(aSetting);
   }
}

void wsf::six_dof::Mover::EnableControls(bool aEnabled)
{
   if (GetPilotManager() != nullptr)
   {
      GetPilotManager()->EnableControls(aEnabled);
   }
}

bool wsf::six_dof::Mover::ControlsEnabled() const
{
   if (GetPilotManager() != nullptr)
   {
      return GetPilotManager()->AreControlsEnabled();
   }
   return false;
}

bool wsf::six_dof::Mover::AutopilotAvailable() const
{
   if (GetPilotManager() != nullptr)
   {
      PilotObject* pilot = GetPilotManager()->GetActivePilot();
      return ((pilot != nullptr) && (pilot->GetCommonController() != nullptr));
   }
   return false;
}

void wsf::six_dof::Mover::CalcStallAngle()
{
   if (GetAeroCoreObject() != nullptr)
   {
      // Use a slow speed, but non-zero
      double mach = 0.01;

      // First find transition point
      double step           = 0.2 * UtMath::cRAD_PER_DEG; // Step using 0.2 degrees
      double transition_ang = -100.0;                     // Use a hugely negative angle (radians)
      double lastCL         = -100.0;                     // Use a hugely negative CL
      for (double ang = 0.0; ang <= 1.57; ang += step)
      {
         double CL = GetAeroCoreObject()->CalculateAeroCLArea(mach, ang);

         if (CL < lastCL)
         {
            // Transition point reached
            transition_ang = ang;
            break;
         }

         lastCL = CL;
      }

      if (transition_ang < 0.0)
      {
         // Stall point not discovered -- use 1.57 radians, approx 90 degrees
         mStallAngle = 1.57;

         // Detection angle is assumed to be 1 degree plus 10% of the stall angle
         mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

         return;
      }
      else
      {
         // Now, we check values with a much smaller step size
         // We use 3 degrees on either side
         double delta = 3.0 * UtMath::cRAD_PER_DEG;
         double start = transition_ang - delta;
         double end   = transition_ang + delta;

         step   = 0.2 * UtMath::cRAD_PER_DEG; // Step using only 0.01 degrees
         lastCL = -100.0;                     // Use a hugely negative CL

         for (double ang = start; ang <= end; ang += step)
         {
            double CL = GetAeroCoreObject()->CalculateAeroCLArea(mach, ang);

            if (CL < lastCL)
            {
               // Transition point reached -- The stall angle is the preceding angle
               mStallAngle = ang - step;

               // Detection angle is assumed to be 1 degree plus 10% of the stall angle
               mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

               return;
            }

            lastCL = CL;
         }

         // We were unable to calculate the precise stall angle (which should not happen)

         // Simply use the transition_ang
         mStallAngle = transition_ang;

         // Detection angle is assumed to be 1 degree plus 10% of the stall angle
         mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

         return;
      }
   }

   // No aero -- use 1.57 radians, approx 90 degrees
   mStallAngle = 1.57;

   // Detection angle is assumed to be 1 degree plus 10% of the stall angle
   mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;
}

void wsf::six_dof::Mover::GetLocationWCS_m(double& aWcsPosX_m, double& aWcsPosY_m, double& aWcsPosZ_m) const
{
   mKinematicState.GetLocationWCS().Get(aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m);
}

UtVec3dX wsf::six_dof::Mover::GetLocationWCS_m() const
{
   return mKinematicState.GetLocationWCS();
}

void wsf::six_dof::Mover::GetVelocityWCS_mps(double& aWcsVelX_mps, double& aWcsVelY_mps, double& aWcsVelZ_mps) const
{
   UtVec3dX velocityWCS_mps = GetVelocityWCS_mps();
   aWcsVelX_mps             = velocityWCS_mps.X();
   aWcsVelY_mps             = velocityWCS_mps.Y();
   aWcsVelZ_mps             = velocityWCS_mps.Z();
}

UtVec3dX wsf::six_dof::Mover::GetVelocityWCS_mps() const
{
   return mKinematicState.GetVelocityWCS();
}

UtVec3dX wsf::six_dof::Mover::GetVelocityNED_mps() const
{
   return mKinematicState.GetVelocityNED_mps();
}

void wsf::six_dof::Mover::GetOrientationWCS_rad(double& aWcsPsi_rad, double& aWcsTheta_rad, double& aWcsPhi_rad) const
{
   aWcsPsi_rad   = mKinematicState.GetYawWCS();
   aWcsTheta_rad = mKinematicState.GetPitchWCS();
   aWcsPhi_rad   = mKinematicState.GetRollWCS();
}

void wsf::six_dof::Mover::SetOrientationWCS_rad(double aWcsPsi_rad, double aWcsTheta_rad, double aWcsPhi_rad)
{
   UtEntity entity;
   double   wcsLoc[3];
   mKinematicState.GetLocationWCS().Get(wcsLoc);
   entity.SetLocationWCS(wcsLoc);

   // Set WCS angles
   mKinematicState.SetOrientationWCS(aWcsPsi_rad, aWcsTheta_rad, aWcsPhi_rad);

   // Set DCM
   mKinematicState.SetDCM(UtDCM(aWcsPsi_rad, aWcsTheta_rad, aWcsPhi_rad));
}

void wsf::six_dof::Mover::GetAttitudeNED_rad(double& aHeading, double& aPitch, double& aRoll)
{
   mKinematicState.GetUtEntity().GetOrientationNED(aHeading, aPitch, aRoll);
}

void wsf::six_dof::Mover::SetAttitudeNED_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad)
{
   // Set local angles
   mKinematicState.SetAttitude_rad(aHeading_rad, aPitch_rad, aRoll_rad);

   // Set the DCM
   double psi   = 0.0;
   double theta = 0.0;
   double phi   = 0.0;
   mKinematicState.GetUtEntity().GetOrientationWCS(psi, theta, phi);
   mKinematicState.SetDCM(UtDCM(psi, theta, phi));
}

void wsf::six_dof::Mover::ZeroBodyRates()
{
   mKinematicState.SetYawRate(0.0);
   mKinematicState.SetPitchRate(0.0);
   mKinematicState.SetRollRate(0.0);
}

void wsf::six_dof::Mover::GetLocationLLA(double& aLat, double& aLon, double& aAlt_m)
{
   mKinematicState.GetUtEntity().GetLocationLLA(aLat, aLon, aAlt_m);
}

void wsf::six_dof::Mover::SetLocationLLA(double aLat, double aLon, double aAlt_m)
{
   mKinematicState.SetPositionLLA(aLat, aLon, aAlt_m);
}

void wsf::six_dof::Mover::SetVelocityWCS_mps(double aWcsVelX_mps, double aWcsVelY_mps, double aWcsVelZ_mps)
{
   mKinematicState.SetVelocityWCS(UtVec3dX(aWcsVelX_mps, aWcsVelY_mps, aWcsVelZ_mps));
}

void wsf::six_dof::Mover::SetLocationWCS_m(double aWcsPosX_m, double aWcsPosY_m, double aWcsPosZ_m)
{
   mKinematicState.SetLocationWCS(UtVec3dX(aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m));
}

void wsf::six_dof::Mover::SetVelocityNED_mps(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   mKinematicState.SetVelocityNED(aVelNorth_mps, aVelEast_mps, aVelDown_mps);
}

UtVec3dX wsf::six_dof::Mover::GetAccelNED_mps2() const
{
   UtVec3dX accelWCS = mKinematicState.GetAccelerationWCS();
   UtVec3dX accelNED = mKinematicState.CalcNEDVecFromWCSVec(accelWCS);

   return accelNED;
}

bool wsf::six_dof::Mover::PendingSequencerEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTimes_nanosec)
{
   aEventTimes_nanosec.clear();
   bool valid = false;

   // Add sequencer event times to the set
   for (auto& seqIter : mSequencerList)
   {
      std::set<int64_t> tempEventTimes_nanosec;
      seqIter->PendingTimeEvents(aSimTime_nanosec, tempEventTimes_nanosec);

      for (const int64_t& eventTime : tempEventTimes_nanosec)
      {
         aEventTimes_nanosec.insert(eventTime);
         valid = true;
      }
   }

   return valid;
}

bool wsf::six_dof::Mover::PendingGuidanceControlEventTimes_nanosec(int64_t /*aSimTime_nanosec*/,
                                                                   std::set<int64_t>& aEventTimes_nanosec)
{
   // TODO -- Support guidance events (especially for ballistic missiles) when available.
   aEventTimes_nanosec.clear();
   return false;
}

bool wsf::six_dof::Mover::PendingEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTimes_nanosec)
{
   aEventTimes_nanosec.clear();
   bool valid = false;

   std::set<int64_t> sequencerEventTimes;
   if (PendingSequencerEventTimes_nanosec(aSimTime_nanosec, sequencerEventTimes))
   {
      for (const int64_t& eventTime : sequencerEventTimes)
      {
         aEventTimes_nanosec.insert(eventTime);
      }
      valid = true;
   }

   std::set<int64_t> guidanceControlEventTimes;
   if (PendingGuidanceControlEventTimes_nanosec(aSimTime_nanosec, sequencerEventTimes))
   {
      for (const int64_t& eventTime : guidanceControlEventTimes)
      {
         aEventTimes_nanosec.insert(eventTime);
      }
      valid = true;
   }

   return valid;
}

wsf::six_dof::Sequencer* wsf::six_dof::Mover::GetSequencerByName(const std::string& aName) const
{
   for (auto& sequencer : mSequencerList)
   {
      if (sequencer->GetName() == aName)
      {
         // Match found, return it
         return sequencer;
      }
   }
   return nullptr;
}

bool wsf::six_dof::Mover::ActivateSequencer(const std::string& aSequencerName)
{
   Sequencer* sequencer = GetSequencerByName(aSequencerName);
   if (sequencer == nullptr)
   {
      ut::log::error() << "Could not find sequencer.";
      return false;
   }
   return sequencer->ActivateExternally();
}

bool wsf::six_dof::Mover::ActivateSequencer(const std::string& aSubobjectName, const std::string& aSequencerName)
{
   Mover* subObject = GetSubobjectByBaseName(aSubobjectName);
   if (subObject)
   {
      // We found the subobject, so try to activate the sequencer
      return subObject->ActivateSequencer(aSequencerName);
   }

   return false;
}

void wsf::six_dof::Mover::CalculateWashIn(double aSimTime_sec)
{
   //// This section will handle "wash-in" initial conditions, to provide steady state conditions at start
   // if (mUseWashInInitConditions)
   //{
   //    // We only perform wash-in for objects that lack a parent, so ensure there is no parent
   //    if (mParentVehicle != nullptr)
   //    {
   //       // We have a parent, so we will not wash-in
   //       std::cout << "WARNING: Wash-in will not be performed for " << GetName() << " since it is not a parent
   //       object." << std::endl;
   //    }
   //    else // mParentVehicle is null (which is good in this case)
   //    {
   //  First, be sure to set the setup flag
   mInitSetupMode = true;

   // Remember the current freeze state
   bool fuelFreeze = mFreezeFlags.fuelBurn;
   bool locFreeze  = mFreezeFlags.location;
   bool altFreeze  = mFreezeFlags.altitude;
   bool spdFreeze  = mFreezeFlags.speed;
   bool lagFreeze  = mFreezeFlags.testingNoLag;

   // Utilize freeze during wash-in for fuel burn and location
   mFreezeFlags.fuelBurn = true;
   mFreezeFlags.location = true;

   // Note that we do *NOT* use freeze for "no lag testing". This is because doing so would result
   // in "missing" engine lag and the ability to determine when the thrust is stable.

   // Determine the alpha needed for level flight (1g). However, keep in mind that the g-load in
   // level flight will likely not be 1g due to a non-zero pitch angle in level flight.
   double gLoad     = 1.0;
   double alpha_deg = 0;
   CalculateAlphaAtSpecifiedGLoad_deg(gLoad, alpha_deg);

   // Get the "target" conditions
   double tgtVel_mps     = mKinematicState.GetSpeed_mps();
   double tgtAlt_ft      = mKinematicState.GetAlt_ft();
   double tgtVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

   // Get controls data
   PilotObject* activePilot           = GetActivePilotObject();
   double       throttleLeverInput    = activePilot->GetThrottleControllerPosition();
   double       stickBackControlInput = activePilot->GetStickBackControllerPosition();

   // These are used for "current conditions"
   double currentVel_mps     = mKinematicState.GetSpeed_mps();
   double currentAlt_ft      = mKinematicState.GetAlt_ft();
   double currentAlpha_deg   = mKinematicState.GetAlpha_deg();
   double currentVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

   // Remember that last conditions for the stick and throttle
   double lastStickBackControlInput = stickBackControlInput;
   double lastThrottleLeverInput    = throttleLeverInput;

   // Remember that last alpha condition
   double lastAlpha_deg = 0.0;

   // Get the initial attitude
   double hdg_rad   = 0;
   double pitch_rad = 0;
   double roll_rad  = 0;
   GetAttitudeNED_rad(hdg_rad, pitch_rad, roll_rad);

   // Adjust pitch using the needed alpha
   pitch_rad += (alpha_deg * UtMath::cRAD_PER_DEG);
   SetAttitudeNED_rad(hdg_rad, pitch_rad, roll_rad);

   // First, do a one second update
   double endTime_sec = aSimTime_sec + 1.0;
   for (double testSimTime_sec = aSimTime_sec; testSimTime_sec <= endTime_sec; testSimTime_sec += 0.01)
   {
      Update(utils::TimeToTime(testSimTime_sec));
   }

   // Calc end time using a 120 sec maximum
   double newStartTime_sec = endTime_sec;
   endTime_sec             = aSimTime_sec + 120.0;

   // These are the target limits that are used to determine if conditions have settled sufficiently
   const double deltaAltLimit_ft      = 4.0;
   const double deltaVertSpdLimit_fpm = 0.1;
   const double deltaAlphaLimit_deg   = 0.01;
   const double deltaVelLimit_mps     = 0.05 * UtMath::cMPS_PER_NMPH;
   const double deltaTimeLimit_sec    = 5.0; // Must run for at least 5 seconds
   const double deltaStickLimit       = 0.00025;
   const double deltaThrottleLimit    = 0.015;

   double testSimTime_sec = 0;
   for (testSimTime_sec = newStartTime_sec; testSimTime_sec <= endTime_sec; testSimTime_sec += 0.01)
   {
      Update(utils::TimeToTime(testSimTime_sec));

      // Update current conditions
      currentVel_mps     = mKinematicState.GetSpeed_mps();
      currentAlt_ft      = mKinematicState.GetAlt_ft();
      currentAlpha_deg   = mKinematicState.GetAlpha_deg();
      currentVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

      // Update controls data
      stickBackControlInput = activePilot->GetStickBackControllerPosition();
      throttleLeverInput    = activePilot->GetThrottleControllerPosition();

      // Determine the current "delta" values
      double deltaVel_mps     = fabs(tgtVel_mps - currentVel_mps);
      double deltaAlt_ft      = fabs(tgtAlt_ft - currentAlt_ft);
      double deltaVertSpd_fpm = fabs(tgtVertSpd_fpm - currentVertSpd_fpm);
      double deltaAlpha_deg   = fabs(currentAlpha_deg - lastAlpha_deg);
      double deltaStick       = fabs(stickBackControlInput - lastStickBackControlInput);
      double deltaThrottle    = fabs(throttleLeverInput - lastThrottleLeverInput);
      double deltaTime_sec    = testSimTime_sec - newStartTime_sec;

      if ((deltaAlt_ft < deltaAltLimit_ft) && (deltaVertSpd_fpm < deltaVertSpdLimit_fpm) &&
          (deltaAlpha_deg < deltaAlphaLimit_deg) && (deltaVel_mps < deltaVelLimit_mps) &&
          (deltaStick < deltaStickLimit) && (deltaThrottle < deltaThrottleLimit) && (deltaTime_sec > deltaTimeLimit_sec))
      {
         // Good enough, so break out
         break;
      }

      // Remember the last stick and throttle conditions
      lastStickBackControlInput = stickBackControlInput;
      lastThrottleLeverInput    = throttleLeverInput;

      // Remember the last alpha
      lastAlpha_deg = currentAlpha_deg;
   }

   // Uncomment this block for testing
   // std::cout << "  Wash-in completed in " << testSimTime_sec << " sec" << std::endl;
   // std::cout << "    Delta-Alt   = " << deltaAlt_ft << " ft (target = " << deltaAltLimit_ft << ")" << std::endl;
   // std::cout << "    Delta-VertSpeed = " << deltaVertSpd_fpm << " ft/min (target = " << deltaVertSpdLimit_fpm << ")"
   // << std::endl; std::cout << "    Delta-Alpha = " << deltaAlpha_deg << " deg (target = " << deltaAlphaLimit_deg <<
   // ")" << std::endl; std::cout << "    Delta-Speed = " << deltaVel_mps * UtMath::cNMPH_PER_MPS << " KTAS (target = "
   // << deltaVelLimit_mps * UtMath::cNMPH_PER_MPS << ")" << std::endl; std::cout << "    Stick Back  = " <<
   // stickBackControlInput << "  Delta-Stick = " << deltaStick << "  (target = " << deltaStickLimit << ")" <<
   // std::endl; std::cout << "    Throttle    = " << throttleLeverInput << "  Delta-Throttle = " << deltaThrottle << "
   // (target = " << deltaThrottleLimit << ")" << std::endl; std::cout << "    Thrust = " << currentThrust << " lbs" <<
   // std::endl;

   // Reset the sim time for this object and sub-objects
   SetLastSimTime(aSimTime_sec, true);

   // Restore freeze state
   mFreezeFlags.fuelBurn     = fuelFreeze;
   mFreezeFlags.location     = locFreeze;
   mFreezeFlags.altitude     = altFreeze;
   mFreezeFlags.speed        = spdFreeze;
   mFreezeFlags.testingNoLag = lagFreeze;

   // Be sure the secondary data is ready
   mKinematicState.CalculateSecondaryParameters();

   // Reset PID sim time
   CommonController* controller = activePilot->GetCommonController();
   if (controller)
   {
      controller->ResetAllPidTimings();
   }

   // Clear the setup flag
   mInitSetupMode = false;
}

bool wsf::six_dof::Mover::RemoveSequencer(const std::string& aName)
{
   auto iter = mSequencerList.begin();
   while (iter != mSequencerList.end())
   {
      if ((*iter)->GetName() == aName)
      {
         mSequencerList.erase(iter);
         return true;
      }
   }

   return false;
}

void wsf::six_dof::Mover::ClampToSeaLevelMinAlt()
{
   // Set altitude to sea level
   double lat   = 0.0;
   double lon   = 0.0;
   double alt_m = 0.0;
   GetLocationLLA(lat, lon, alt_m);
   SetLocationLLA(lat, lon, 0.0);
}

void wsf::six_dof::Mover::SetPrepositionAlt_m(double aAltitude_m)
{
   SetLocationLLA(mPrepositionLat, mPrepositionLon, aAltitude_m);
}

void wsf::six_dof::Mover::SetYawRelToParent_rad(double aYawAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(aYawAngle_rad, p, r);
}

void wsf::six_dof::Mover::SetPitchRelToParent_rad(double aPitchAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(y, aPitchAngle_rad, r);
}

void wsf::six_dof::Mover::SetRollRelToParent_rad(double aRollAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(y, p, aRollAngle_rad);
}

void wsf::six_dof::Mover::GetPositionRelativeToParent(UtVec3dX& aPosRelToParent_m, UtVec3dX& aAngRelToParentYPR_rad)
{
   aPosRelToParent_m      = mPosRelToParent_m;
   aAngRelToParentYPR_rad = mAngRelToParentYPR_rad;
}

void wsf::six_dof::Mover::AdjustForParentInducedEffects()
{
   // Get adjusted position
   UtVec3dX posDeltaVecWGS = mKinematicState.GetDCM().InverseTransform(mPosRelToParent_m);
   mKinematicState.SetLocationWCS(mKinematicState.GetLocationWCS() + posDeltaVecWGS);

   // Adjust linear velocity from position offset and angular velocity
   UtVec3dX parentVelocityWGS = mKinematicState.GetVelocityWCS();
   UtVec3dX parentOmegaWGS    = mKinematicState.GetDCM().InverseTransform(mKinematicState.GetOmegaBody());
   mKinematicState.SetVelocityWCS(parentVelocityWGS + parentOmegaWGS.Cross(posDeltaVecWGS));

   // Adjust DCM for relative attitude
   double relYaw, relPitch, relRoll;
   mAngRelToParentYPR_rad.Get(relYaw, relPitch, relRoll);
   UtDCM yawDCM(relYaw, 0.0, 0.0);
   UtDCM pitchDCM(0.0, relPitch, 0.0);
   UtDCM rollDCM(0.0, 0.0, relRoll);
   mKinematicState.SetDCM(rollDCM * pitchDCM * yawDCM * mKinematicState.GetDCM());

   // Need to calculate the secondary parameters
   mKinematicState.CalculateSecondaryParameters();
}

void wsf::six_dof::Mover::ImpartSeparationEffects()
{
   UtVec3dX separationVelVec = mSeparationVel_fps;
   UtVec3dX inertialVel(0.0, 0.0, 0.0);
   inertialVel = mKinematicState.GetDCM().InverseTransform(separationVelVec);

   mKinematicState.SetOmegaBody(mKinematicState.GetOmegaBody() + mSeparationOmega_rps);

   // Calc new vel
   mKinematicState.SetVelocityWCS(mKinematicState.GetVelocityWCS() + inertialVel);
}

void wsf::six_dof::Mover::AddSubObject(std::unique_ptr<Mover> aSubObject)
{
   mSubObjectList.emplace_back(std::move(aSubObject));
}

const std::list<std::unique_ptr<wsf::six_dof::Mover>>& wsf::six_dof::Mover::GetSubobjectList() const
{
   return mSubObjectList;
}

wsf::six_dof::Mover* wsf::six_dof::Mover::GetSubobjectByName(const std::string& aSubobjectName) const
{
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject && subObject->GetName() == aSubobjectName)
      {
         // Match found
         return subObject;
      }
   }

   // No match found, try children
   subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject)
      {
         Mover* childObject = subObject->GetSubobjectByName(aSubobjectName);
         if (childObject)
         {
            // Match found
            return childObject;
         }
      }
   }

   return nullptr;
}

wsf::six_dof::Mover* wsf::six_dof::Mover::GetSubobjectByBaseName(const std::string& aSubobjectName) const
{
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject && subObject->GetBaseName() == aSubobjectName)
      {
         // Match found
         return subObject;
      }
   }

   // No match found, try children
   subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject)
      {
         Mover* childObject = subObject->GetSubobjectByBaseName(aSubobjectName);
         if (childObject)
         {
            // Match found
            return childObject;
         }
      }
   }

   return nullptr;
}

std::unique_ptr<wsf::six_dof::Mover> wsf::six_dof::Mover::SeparateSubObject(Mover* aSubObject)
{
   // Confirm sub object is valid
   auto it = std::find_if(mSubObjectList.begin(),
                          mSubObjectList.end(),
                          [aSubObject](const std::unique_ptr<Mover>& subObj) { return subObj.get() == aSubObject; });
   if (it != mSubObjectList.end())
   {
      // Copy state from parent
      aSubObject->CopyDataFromParent();
      aSubObject->AdjustForParentInducedEffects();

      // Include separation velocities
      aSubObject->ImpartSeparationEffects();

      // Change subobject from captive to free
      aSubObject->SetCaptiveState(false);

      aSubObject->GetKinematicState()->CalculateSecondaryParameters();

      // Clear the subobject's parent
      aSubObject->mParentVehicle = nullptr;

      // Remove it from the subobject list
      auto separatedSubObjectPtr = move(*it);
      mSubObjectList.erase(it);
      return separatedSubObjectPtr;
   }

   return nullptr;
}

bool wsf::six_dof::Mover::JettisonSubobject(const std::string& aSubobjectName, int64_t aSimTime_nanosec)
{
   return JettisonSubobject(GetSubobjectByName(aSubobjectName), aSimTime_nanosec);
}

bool wsf::six_dof::Mover::JettisonSubobject(Mover* aSubObject, int64_t aSimTime_nanosec)
{
   // In order to jettison the object, it must be captive
   if (aSubObject != nullptr && aSubObject->IsCaptive())
   {
      auto separatedSubObject = SeparateSubObject(aSubObject);

      if (!mKinematicState.IgnoreJettisonObjects())
      {
         // Update the newly freed object
         aSubObject->Update(aSimTime_nanosec);

         // Callback to notify subscribers of the subobject jettisoned
         SubobjectJettisoned(separatedSubObject);
      }
      else
      {
         // If we are to ignore jettisoned objects,
         // go ahead and delete the object as it is no longer required.
         separatedSubObject = nullptr;
      }
      return true;
   }
   return false;
}

std::unique_ptr<wsf::six_dof::Mover> wsf::six_dof::Mover::DirectJettisonOfSubobject(Mover* aSubobject, double aSimTime_sec)
{
   // In order to jettison the object, it must be captive
   if (aSubobject != nullptr && aSubobject->IsCaptive())
   {
      auto separatedSubObject = SeparateSubObject(aSubobject);

      if (!mKinematicState.IgnoreJettisonObjects())
      {
         // Get the nanosecond version of time
         int64_t targetSimTime_nanosec = utils::TimeToTime(aSimTime_sec);

         // Update the newly freed object
         aSubobject->Update(targetSimTime_nanosec);
      }
      return separatedSubObject;
   }
   return nullptr;
}

int wsf::six_dof::Mover::GetNumberOfExternalTanks() const
{
   int  num        = 0;
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject && subObject->IsExternalFuelTank())
      {
         ++num;
      }
   }

   return num;
}

// TODO - Simplify this function, if possible
wsf::six_dof::FuelTank* wsf::six_dof::Mover::GetFuelTankByName(const std::string& aName,
                                                               const Mover*       aCallingObject,
                                                               int                aCallingLevel) const
{
   if ((aCallingLevel < 0) || (aCallingLevel > 15))
   {
      auto out = ut::log::error() << "Calling limit reached while vehicle looking for fuel tank.";
      out.AddNote() << "Call Level: " << aCallingLevel;
      out.AddNote() << "Vehicle: " << GetName();
      out.AddNote() << "Looking For: " << aName;
      out.AddNote() << "Called From: " << aCallingObject->GetName();
      return nullptr;
   }

   if (GetPropulsionSystem() != nullptr)
   {
      FuelTank* tank = GetPropulsionSystem()->GetFuelTank(aName);
      if (tank != nullptr)
      {
         return tank;
      }
   }

   // Separate the SubObject's name and the FuelTanks name
   size_t tokenLocation = aName.find('.');
   if (tokenLocation != std::string::npos)
   {
      std::string subObjName   = aName.substr(0, tokenLocation);
      std::string fuelTankName = "";
      if (tokenLocation + 1 <= aName.length())
      {
         fuelTankName = aName.substr(tokenLocation + 1);
      }

      // Try the children
      auto subObjIter = mSubObjectList.begin();
      while (subObjIter != mSubObjectList.end())
      {
         Mover* subObject = (*subObjIter).get();
         ++subObjIter;

         if (subObject && (subObject != aCallingObject) && (subObject->GetBaseName() == subObjName))
         {
            // Increment calling level
            ++aCallingLevel;

            // Big change here -- now passing 'this' pointer instead of who called this
            FuelTank* tank = subObject->GetFuelTankByName(fuelTankName, this, aCallingLevel);

            if (tank != nullptr)
            {
               return tank;
            }

            // Decrement calling level
            aCallingLevel--;
         }
      }
   }

   // Try the parent
   if (aCallingLevel <= 0)
   {
      if ((mParentVehicle != nullptr) && (mParentVehicle != aCallingObject))
      {
         // Increment calling level
         ++aCallingLevel;

         FuelTank* tank = mParentVehicle->GetFuelTankByName(aName, aCallingObject, aCallingLevel);
         if (tank != nullptr)
         {
            return tank;
         }
      }
   }

   return nullptr;
}

// TODO - Simplify this function, if possible
wsf::six_dof::ThrustProducerObject* wsf::six_dof::Mover::GetThrustProducerObjectByName(const std::string& aName,
                                                                                       const Mover* aCallingObject,
                                                                                       int          aCallingLevel) const
{
   if ((aCallingLevel < 0) || (aCallingLevel > 15))
   {
      auto out = ut::log::error() << "Calling limit reached while vehicle looking for thrust producer.";
      out.AddNote() << "Call Level: " << aCallingLevel;
      out.AddNote() << "Vehicle: " << GetName();
      out.AddNote() << "Looking For: " << aName;
      out.AddNote() << "Called From: " << aCallingObject->GetName();
      return nullptr;
   }

   if (GetPropulsionSystem() != nullptr)
   {
      ThrustProducerObject* thrustProducer = GetPropulsionSystem()->GetThrustProducerObjectByName(aName);
      if (thrustProducer != nullptr)
      {
         return thrustProducer;
      }
   }

   // Try the children
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject && subObject != aCallingObject)
      {
         // Increment calling level
         ++aCallingLevel;

         // Big change here -- now passing 'this' pointer instead of who called this
         ThrustProducerObject* thrustProducer = subObject->GetThrustProducerObjectByName(aName, this, aCallingLevel);

         if (thrustProducer != nullptr)
         {
            return thrustProducer;
         }

         // Decrement calling level
         aCallingLevel--;
      }
   }

   // Try the parent
   if (aCallingLevel <= 0)
   {
      if ((mParentVehicle != nullptr) && (mParentVehicle != aCallingObject))
      {
         // Increment calling level
         ++aCallingLevel;

         ThrustProducerObject* thrustProducer =
            mParentVehicle->GetThrustProducerObjectByName(aName, aCallingObject, aCallingLevel);
         if (thrustProducer != nullptr)
         {
            return thrustProducer;
         }
      }
   }

   return nullptr;
}

wsf::six_dof::FuelTank* wsf::six_dof::Mover::GetFuelTankByName(const std::string& aName) const
{
   return GetFuelTankByName(aName, this);
}

double wsf::six_dof::Mover::GetTotalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (GetPropulsionSystem() != nullptr)
   {
      fuelQuantity_lbs += GetPropulsionSystem()->GetInternalFuelTankCapacity_lbs();
   }

   fuelQuantity_lbs += GetExternalFuelTankCapacity_lbs();

   return fuelQuantity_lbs;
}

double wsf::six_dof::Mover::GetCurrentTotalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (GetPropulsionSystem() != nullptr)
   {
      fuelQuantity_lbs += GetPropulsionSystem()->GetCurrentInternalFuelTankQuantity_lbs();
   }

   fuelQuantity_lbs += GetCurrentExternalFuelTankQuantity_lbs();

   return fuelQuantity_lbs;
}

void wsf::six_dof::Mover::AddFuelQuantity_lbs(double aFuelToAdd_lbs)
{
   double fuelQuantityAdded_lbs = 0;

   // First, add fuel internally...
   if (GetPropulsionSystem() != nullptr)
   {
      fuelQuantityAdded_lbs = GetPropulsionSystem()->AddFuelQuantity_lbs(aFuelToAdd_lbs);
   }

   double fuelStillAvailableToAdd_lbs = aFuelToAdd_lbs - fuelQuantityAdded_lbs;

   if (fuelStillAvailableToAdd_lbs > 0)
   {
      // Determine the percent full

      double maxTotal_lbs        = GetTotalFuelTankCapacity_lbs();
      double currentTotal_lbs    = GetCurrentTotalFuelTankQuantity_lbs();
      double maxInternal_lbs     = GetInternalFuelTankCapacity_lbs();
      double currentInternal_lbs = GetCurrentInternalFuelTankQuantity_lbs();
      double maxExternal_lbs     = maxTotal_lbs - maxInternal_lbs;
      double currentExternal_lbs = currentTotal_lbs - currentInternal_lbs;

      if (maxExternal_lbs > 0)
      {
         double currentExternalAfterAdd_lbs = currentExternal_lbs + fuelStillAvailableToAdd_lbs;

         double percentFull = 100.0 * (currentExternalAfterAdd_lbs / maxExternal_lbs);

         if (percentFull < 0)
         {
            percentFull = 0.0;
         }
         if (percentFull > 100)
         {
            percentFull = 100.0;
         }

         double fuelAddedToExternal_lbs = 0;

         // Set percent full for sub-objects
         auto subObjIter = mSubObjectList.begin();
         while (subObjIter != mSubObjectList.end())
         {
            Mover* subObject = (*subObjIter).get();
            ++subObjIter;

            if (subObject && subObject->GetPropulsionSystem() != nullptr)
            {
               fuelAddedToExternal_lbs += subObject->GetPropulsionSystem()->FillAllTanks(percentFull);
            }
         }
      }
   }
}

void wsf::six_dof::Mover::SetFuelInTank_lbs(const std::string& aTankName, double aFuel_lbs)
{
   FuelTank* tank = GetFuelTankByName(aTankName);

   if (tank != nullptr)
   {
      double maxFuel_lbs = tank->GetFuelCapacity_lbs();

      double fuelToTranfer_lbs = aFuel_lbs;
      if (fuelToTranfer_lbs > maxFuel_lbs)
      {
         fuelToTranfer_lbs = maxFuel_lbs;
      }
      else if (fuelToTranfer_lbs <= 0)
      {
         fuelToTranfer_lbs = 0;
      }

      tank->SetCurrentFuelQuantity(fuelToTranfer_lbs);
   }
}

bool wsf::six_dof::Mover::IsExternalFuelTank()
{
   // Is a mover contains fuel tanks, but no engine, it can only serve as an external tank to a parent vehicle
   if (GetInternalFuelTankCapacity_lbs() > 0.0)
   {
      PropulsionSystem* propSystem = GetPropulsionSystem();
      if (propSystem != nullptr)
      {
         return (propSystem->GetNumThrustProducers() == 0);
      }
   }

   // No propulsion system, so it carries no tanks
   return false;
}

void wsf::six_dof::Mover::EmptyAllInternalTanks()
{
   if (GetPropulsionSystem() != nullptr)
   {
      GetPropulsionSystem()->EmptyAllTanks();
   }
}

void wsf::six_dof::Mover::EmptyAllExternalTanks()
{
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject != nullptr)
      {
         // Verify that it is a fuel tank of the parent before removing fuel
         if (subObject->IsExternalFuelTank())
         {
            subObject->EmptyAllInternalandExternalTanks();
         }
         else
         {
            // Avoid emptying internal tanks of subobjects, but do empty their external tanks
            subObject->EmptyAllExternalTanks();
         }
      }
   }
}

double wsf::six_dof::Mover::GetInternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (GetPropulsionSystem() != nullptr)
   {
      fuelQuantity_lbs += GetPropulsionSystem()->GetInternalFuelTankCapacity_lbs();
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::Mover::GetCurrentInternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (GetPropulsionSystem() != nullptr)
   {
      fuelQuantity_lbs += GetPropulsionSystem()->GetCurrentInternalFuelTankQuantity_lbs();
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::Mover::GetExternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      // If the subobject is acting as a fuel tank, add its total fuel capacity
      if (subObject && subObject->IsExternalFuelTank())
      {
         fuelQuantity_lbs += subObject->GetTotalFuelTankCapacity_lbs();
      }

      // If a subobject has its own engines, we assume that it will power
      // its own flight rather than contribute to its parent object's
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::Mover::GetCurrentExternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      // If the subobject is acting as a fuel tank, add its total fuel quantity
      if (subObject && subObject->IsExternalFuelTank())
      {
         fuelQuantity_lbs += subObject->GetCurrentTotalFuelTankQuantity_lbs();
      }

      // If a subobject has its own engines, we assume that it will power
      // its own flight rather than contribute to its parent object's
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::Mover::GetRemainingFuelPercent() const
{
   if (GetPropulsionSystem() != nullptr)
   {
      double currentTotalFuel_lbs = GetCurrentTotalFuelTankQuantity_lbs();
      double maxTotalFuel_lbs     = GetTotalFuelTankCapacity_lbs();

      return 100.0 * currentTotalFuel_lbs / maxTotalFuel_lbs;
   }
   return 0.0;
}

bool wsf::six_dof::Mover::AddFuelTransfer(std::string aFuelTransferName, std::string aSourceTankName, std::string aTargetTankName)
{
   if (GetPropulsionSystem() != nullptr)
   {
      return GetPropulsionSystem()->AddFuelTransferAndConnect(aFuelTransferName, aSourceTankName, aTargetTankName);
   }
   return false;
}

bool wsf::six_dof::Mover::RemoveFuelTransfer(std::string aFuelTransferName)
{
   if (GetPropulsionSystem() != nullptr)
   {
      return GetPropulsionSystem()->RemoveFuelTransfer(aFuelTransferName);
   }
   return false;
}

bool wsf::six_dof::Mover::SetFuelFeed(std::string aEngineName, std::string aFuelTankName)
{
   if (GetPropulsionSystem() != nullptr)
   {
      return GetPropulsionSystem()->SetFuelFeed(aEngineName, aFuelTankName);
   }
   return false;
}

bool wsf::six_dof::Mover::SetFuelFeed(std::string aFuelTankName)
{
   if (GetPropulsionSystem() != nullptr)
   {
      return GetPropulsionSystem()->SetFuelFeed(aFuelTankName);
   }
   return false;
}

wsf::six_dof::ThrustProducerObject* wsf::six_dof::Mover::GetThrustProducerObjectByName(const std::string& aName) const
{
   return GetThrustProducerObjectByName(aName, this);
}

double wsf::six_dof::Mover::GetNormalizedMilitaryThrust() const
{
   if (GetPropulsionSystem() != nullptr)
   {
      ThrustProducerObject* thruster = GetPropulsionSystem()->GetThrustProducerByIndex(0);
      if (thruster != nullptr)
      {
         const Engine* engine = thruster->GetEngine();
         auto          jetPtr = dynamic_cast<const JetEngine*>(engine);
         if (jetPtr != nullptr)
         {
            // Military thrust is assumed proportional to the engine rpm value
            return jetPtr->GetEnginePercentRPM() * 0.01;
         }
      }
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetNormalizedAfterburnerThrust() const
{
   if (GetPropulsionSystem() != nullptr)
   {
      ThrustProducerObject* thruster = GetPropulsionSystem()->GetThrustProducerByIndex(0);
      if (thruster != nullptr)
      {
         const Engine* engine = thruster->GetEngine();
         auto          jetPtr = dynamic_cast<const JetEngine*>(engine);
         if (jetPtr != nullptr)
         {
            // Afterburner thrust is assumed proportional to the nozzle position
            return jetPtr->GetNozzlePosition();
         }
      }
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetNormalizedTotalThrust() const
{
   if (GetPropulsionSystem() != nullptr)
   {
      double thrust = GetNormalizedMilitaryThrust() + GetNormalizedAfterburnerThrust();

      if (thrust < 0.0)
      {
         thrust = 0.0;
      }
      else if (thrust > 2.0)
      {
         thrust = 2.0;
      }

      return thrust;
   }

   return 0.0;
}

void wsf::six_dof::Mover::IgniteObject(int64_t aIgniteTimeInFrame_nanosec)
{
   // Ignite all engines in propulsion object
   if (GetPropulsionSystem() != nullptr)
   {
      GetPropulsionSystem()->Ignite(aIgniteTimeInFrame_nanosec);
   }

   // TODO -- we may want to consider ignite of sub-objects
}

void wsf::six_dof::Mover::ShutdownObject(int64_t aTerminateTime_nanosec)
{
   // Shutdown all engines in propulsion object
   if (GetPropulsionSystem() != nullptr)
   {
      GetPropulsionSystem()->Shutdown(aTerminateTime_nanosec);
   }

   // TODO -- we may want to consider shutdown of sub-objects
}

void wsf::six_dof::Mover::TerminateThrust(int64_t aTerminateTime_nanosec)
{
   // Shutdown all engines in propulsion object
   if (GetPropulsionSystem() != nullptr)
   {
      GetPropulsionSystem()->Shutdown(aTerminateTime_nanosec);
   }

   // Loop through all sub-objects and terminate thrust
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      Mover* subObject = (*subObjIter).get();
      ++subObjIter;

      if (subObject != nullptr)
      {
         subObject->TerminateThrust(aTerminateTime_nanosec);
      }
   }
}

bool wsf::six_dof::Mover::IsProducingThrust() const
{
   if (GetPropulsionSystem() == nullptr)
   {
      return false;
   }

   if (GetPropulsionSystem()->IsProducingThrust())
   {
      return true;
   }

   // Loop through all sub-objects
   for (auto& iter : mSubObjectList)
   {
      if (iter->IsProducingThrust())
      {
         return true;
      }
   }

   return false;
}

void wsf::six_dof::Mover::EnableThrustVectoring(bool aEnableThrustVectoring)
{
   if (GetPropulsionSystem() != nullptr)
   {
      GetPropulsionSystem()->EnableThrustVectoring(aEnableThrustVectoring);
   }
}

bool wsf::six_dof::Mover::HasAfterburner() const
{
   bool hab = false;
   if (GetPropulsionSystem() != nullptr)
   {
      // Loop through engines, looking for at least one jet with an afterburner
      for (size_t i = 0; i < GetPropulsionSystem()->GetNumThrustProducers(); ++i)
      {
         // Get the current engine
         ThrustProducerObject* engine = GetPropulsionSystem()->GetThrustProducerByIndex(i);

         // If engine is valid
         if ((engine != nullptr) && (engine->AfterburnerIsPresent()))
         {
            hab = true;
            break;
         }
      }
   }
   return hab;
}

bool wsf::six_dof::Mover::AnEngineIsOperating() const
{
   bool operating = false;
   if (GetPropulsionSystem() != nullptr)
   {
      operating = GetPropulsionSystem()->AnEngineIsOperating();
   }
   return operating;
}

bool wsf::six_dof::Mover::AnEngineIsSmoking() const
{
   bool engSmoking = false;
   if (GetPropulsionSystem() != nullptr)
   {
      engSmoking = GetPropulsionSystem()->AnEngineIsSmoking();
   }
   return engSmoking;
}

bool wsf::six_dof::Mover::AnEngineHasAfterburnerOn() const
{
   bool eabOn = false;
   if (GetPropulsionSystem() != nullptr)
   {
      eabOn = GetPropulsionSystem()->AnEngineHasAfterburnerOn();
   }
   return eabOn;
}

void wsf::six_dof::Mover::MakeEnginesSmoke()
{
   if (GetPropulsionSystem() != nullptr)
   {
      return GetPropulsionSystem()->MakeAnEngineSmoke(0); // Zero, all engines
   }
}

bool wsf::six_dof::Mover::ContrailTrailingEffectPresent() const
{
   bool present = false;
   if (GetPropulsionSystem() != nullptr)
   {
      present = GetPropulsionSystem()->AnEngineIsContrailing();
   }
   return present;
}

bool wsf::six_dof::Mover::RocketSmokeTrailingEffectPresent() const
{
   bool present = false;
   if (GetPropulsionSystem() != nullptr)
   {
      present = GetPropulsionSystem()->AnEngineIsEmittingSmokeTrail();
   }
   return present;
}

bool wsf::six_dof::Mover::DamageSmokeTrailingEffectPresent() const
{
   return mDamageSmokeTrailingEffect;
}

void wsf::six_dof::Mover::ActivateDamageSmokeTrailingEffect(bool aEffectActive)
{
   mDamageSmokeTrailingEffect = aEffectActive;
}

bool wsf::six_dof::Mover::FlamesArePresent() const
{
   return mFlamesPresent;
}

void wsf::six_dof::Mover::ActivateFlamesEffect(bool aFlamesPresent)
{
   mFlamesPresent = aFlamesPresent;
}

bool wsf::six_dof::Mover::WithinContrailAltitudeBand(double aAltitude_ft) const
{
   bool               wcab = false;
   const Environment* env  = GetEnvironment();
   if (env != nullptr)
   {
      wcab = env->IsWithinContrailBand(UtMath::cM_PER_FT * aAltitude_ft);
   }
   return wcab;
}

void wsf::six_dof::Mover::SetHeading(double aHeading)
{
   double heading = 0.0;
   double pitch   = 0.0;
   double roll    = 0.0;
   GetAttitudeNED_rad(heading, pitch, roll);
   SetAttitudeNED_rad(aHeading, pitch, roll);
}

void wsf::six_dof::Mover::SetPitch(double aPitch)
{
   double heading = 0.0;
   double pitch   = 0.0;
   double roll    = 0.0;
   GetAttitudeNED_rad(heading, pitch, roll);
   SetAttitudeNED_rad(heading, aPitch, roll);
}

void wsf::six_dof::Mover::SetRoll(double aRoll)
{
   double heading = 0.0;
   double pitch   = 0.0;
   double roll    = 0.0;
   GetAttitudeNED_rad(heading, pitch, roll);
   SetAttitudeNED_rad(heading, pitch, aRoll);
}

void wsf::six_dof::Mover::SetOrientationNED(double aHeading, double aPitch, double aRoll)
{
   SetAttitudeNED_rad(aHeading, aPitch, aRoll);
}

void wsf::six_dof::Mover::GetStateData(double& aSimTime_sec,
                                       double& aLat,
                                       double& aLon,
                                       double& aAlt_m,
                                       double& aLocalHeading_deg,
                                       double& aLocalPitch_deg,
                                       double& aLocalRoll_deg,
                                       double& aAlpha_deg,
                                       double& aBeta_deg,
                                       double& aSpeed_kts,
                                       double& aVertSpeed_fpm,
                                       double& aYawRate_dps,
                                       double& aPitchRate_dps,
                                       double& aRollRate_dps) const
{
   aSimTime_sec = mLastUpdateTime;

   GetPlatform()->GetLocationLLA(aLat, aLon, aAlt_m);

   double hdgNED_rad   = 0.0;
   double pitchNED_rad = 0.0;
   double rollNED_rad  = 0.0;
   GetPlatform()->GetOrientationNED(hdgNED_rad, pitchNED_rad, rollNED_rad);

   aLocalHeading_deg = hdgNED_rad * UtMath::cDEG_PER_RAD;
   aLocalPitch_deg   = pitchNED_rad * UtMath::cDEG_PER_RAD;
   aLocalRoll_deg    = rollNED_rad * UtMath::cDEG_PER_RAD;

   double velNED[3];
   GetPlatform()->GetVelocityNED(velNED);

   aVertSpeed_fpm = (-velNED[2]) * UtMath::cFT_PER_M * 60.0;
   aSpeed_kts     = GetPlatform()->GetSpeed() * UtMath::cNMPH_PER_MPS;
   aAlpha_deg     = 0.0;
   aBeta_deg      = 0.0;
   aYawRate_dps   = 0.0;
   aPitchRate_dps = 0.0;
   aRollRate_dps  = 0.0;
}

size_t wsf::six_dof::Mover::GetPlannedRouteIndex() const
{
   PilotObject* pilot = GetActivePilotObject();
   return pilot->GetPlannedWaypointIndex();
}

size_t wsf::six_dof::Mover::GetTempRouteIndex() const
{
   PilotObject* pilot = GetActivePilotObject();
   return pilot->GetTempWaypointIndex();
}

void wsf::six_dof::Mover::Update(double aSimTime)
{
   UpdateEventPipeData(aSimTime);

   if (mManeuverPtr != nullptr)
   {
      mManeuverPtr->Update(aSimTime);
   }

   // Get the pilot object
   PilotObject* pilot = GetActivePilotObject();

   if (mGuidancePtr != nullptr)
   {
      double aBegTime = mLastUpdateTime;
      double aEndTime = aSimTime;

      // The second argument should be the time for by the current state of the weapon (aBegTime),
      // not the time to which we are trying to advance (aEndTime).
      mGuidancePtr->UpdateGuidance(aSimTime, aBegTime, aEndTime);

      auto guidanceComputer = static_cast<GuidanceComputer*>(mGuidancePtr);

      // We need a pilot object
      if (pilot != nullptr)
      {
         // The guidance generates the following commands:
         // mYGLoad, mZGLoad and mCommandedYawRate, mCommandedPitchRate, and mCommandedRollRate
         // At the present time, only mYGLoad, mZGLoad are used.

         CommonController* autopilot = pilot->GetCommonController();
         AutopilotAction*  activity  = autopilot->GetCurrentActivity();

         if (Control::IsBankToTurn(autopilot->GetControlMethod()))
         {
            double pitchGLoad    = -mZGLoad;
            double bankAngle_deg = UtMath::cDEG_PER_RAD * atan2(mYGLoad, -mZGLoad);

            activity->SetVerticalChannelMode(Vertical::PitchGLoad);
            activity->SetPitchGLoad_g(static_cast<float>(pitchGLoad));

            activity->SetLateralChannelMode(Lateral::Bank);
            activity->SetBank_deg(static_cast<float>(bankAngle_deg));
         }
         else
         {
            double yawGLoad   = mYGLoad;
            double pitchGLoad = -mZGLoad;

            activity->SetVerticalChannelMode(Vertical::PitchGLoad);
            activity->SetPitchGLoad_g(static_cast<float>(pitchGLoad));

            activity->SetLateralChannelMode(Lateral::YawGLoad);
            activity->SetYawGLoad_g(static_cast<float>(yawGLoad));
         }

         double commandedFlightPath = guidanceComputer->GetCommandedFlightPathAngle();
         double commandedAltitude   = guidanceComputer->GetCommandedAltitude();
         double commandedSpeed      = guidanceComputer->GetCommandedSpeed();

         if (commandedAltitude != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
         {
            activity->SetVerticalChannelMode(Vertical::Altitude);
            activity->SetAltitudeMSL_ft(static_cast<float>(UtMath::cFT_PER_M * commandedAltitude));
         }
         else if (commandedFlightPath != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
         {
            activity->SetVerticalChannelMode(Vertical::FltPathAng);
            activity->SetFlightPathAngle_deg(static_cast<float>(commandedFlightPath));
         }

         if (commandedSpeed != WsfGuidanceComputer::cUNDEFINED_DOUBLE)
         {
            activity->SetSpeedChannelMode(Speed::FPS);
            activity->SetSpeed_fps(static_cast<float>(UtMath::cFT_PER_M * commandedSpeed));
         }

         if (guidanceComputer->GetAllowRouteFollowing())
         {
            activity->SetVerticalChannelMode(Vertical::Waypoint);
            activity->SetLateralChannelMode(Lateral::Waypoint);
            activity->SetSpeedChannelMode(Speed::Waypoint);
         }

         autopilot->SetCurrentActivity(activity);
      }
   }

   // Get the nanosecond version of time
   int64_t targetSimTime_nanosec = utils::TimeToTime(aSimTime);

   // Check for no appreciable advance of time -- if so, return
   int64_t deltaT_nanosec = targetSimTime_nanosec - mLastSimTime_nanosec;
   if (deltaT_nanosec < utils::cEPSILON_SIMTIME_NANOSEC)
   {
      return;
   }

   // Are there events we need to be aware of between now and the requested time step? Keep going through each
   // event in sequence until we have no events between the current time and the end of the requested time step.
   std::set<int64_t> eventSimTimes_nano;
   int64_t           simTime_nano = mLastSimTime_nanosec;

   if (PendingEventTimes_nanosec(targetSimTime_nanosec, eventSimTimes_nano))
   {
      // Loop through the list of event times
      for (const int64_t& eventTime_nano : eventSimTimes_nano)
      {
         int64_t naturalTimeStepsToEvent = (eventTime_nano - mLastSimTime_nanosec) / mStepSize_nanosec;

         for (int64_t i = 0; i < naturalTimeStepsToEvent; ++i)
         {
            // Increment one time step and update
            simTime_nano += mStepSize_nanosec;
            Update(simTime_nano);
         }

         // If we have any remaining time, carry out a reduced-step integration
         if ((eventTime_nano - simTime_nano) > utils::cEPSILON_SIMTIME_NANOSEC)
         {
            simTime_nano = eventTime_nano;
            Update(simTime_nano);
         }
      }
   }

   int64_t naturalTimeStepsRemaining = (targetSimTime_nanosec - simTime_nano) / mStepSize_nanosec;

   for (int64_t i = 0; i < naturalTimeStepsRemaining; ++i)
   {
      // Increment one time step and update
      simTime_nano += mStepSize_nanosec;
      Update(simTime_nano);
   }

   // If we have any remaining time, carry out a reduced-step integration
   if ((targetSimTime_nanosec - simTime_nano) >= utils::cEPSILON_SIMTIME_NANOSEC)
   {
      Update(targetSimTime_nanosec);
   }

   // Set the last update time
   double lastUpdateTime = utils::TimeToTime(simTime_nano);

   WsfMover::mLastUpdateTime = lastUpdateTime;

   // Update the platform state
   UpdatePlatformFromVehicle(this, GetPlatform());

   // Check for staging events and create new objects as needed
   CheckForNewlyJettisonedPlatforms(lastUpdateTime);

   UpdateAppearance(lastUpdateTime);
}

void wsf::six_dof::Mover::UpdateAppearance(double aSimTime)
{
   // Set various "appearance" parameters

   WsfPlatform* platform = GetPlatform();

   // Trailing Effect -- This denotes contrailing or missile smoke trail
   // LSB = 7, 2 bits (none, small, medium, large)
   if (!platform->ScriptOverride(7, 2))
   {
      // Check for types of smoke. Priority as follows:
      // damage smoke, rocket smoke, contrail
      if (DamageSmokeTrailingEffectPresent())
      {
         platform->SetAppearance(7, 2, 3); // 3 - Large
      }
      else if (RocketSmokeTrailingEffectPresent())
      {
         platform->SetAppearance(7, 2, 2); // 2 - Medium
      }
      else if (ContrailTrailingEffectPresent())
      {
         platform->SetAppearance(7, 2, 1); // 1 - Small
      }
      else
      {
         // No trailing effects
         platform->SetAppearance(7, 2, 0);
      }
   }

   // Smoke -- This denotes engine smoke, a rising smoke plume, or both
   // LSB = 5, 2 bits (none, rising, engine, both)
   if (!platform->ScriptOverride(5, 2))
   {
      if (AnEngineIsSmoking())
      {
         if (mRisingSmokePlume)
         {
            // both
            platform->SetAppearance(5, 2, 3);
         }
         else
         {
            // engine
            platform->SetAppearance(5, 2, 2);
         }
      }
   }
   else
   {
      if (mRisingSmokePlume)
      {
         // rising
         platform->SetAppearance(5, 2, 1);
      }
      else
      {
         // none
         platform->SetAppearance(5, 2, 0);
      }
   }

   // Damage -- This denotes various damage or a destroyed condition
   // LSB = 3, 2 bits (none, light damage, heavy damage, destroyed)
   if (!platform->ScriptOverride(3, 2))
   {
      if (mIsDestroyed)
      {
         platform->SetAppearance(3, 2, 3);
      }
      else if (mHeavyDamage)
      {
         platform->SetAppearance(3, 2, 2);
      }
      else if (mLightDamage)
      {
         platform->SetAppearance(3, 2, 1);
      }
      else
      {
         // No damage
         platform->SetAppearance(3, 2, 0);
      }
   }

   // Flaming -- This denotes that flames are present
   // LSB = 15, 1 bit (none, flames present)
   if (FlamesArePresent())
   {
      platform->SetAppearance(15, 1, 1);
   }
   else
   {
      platform->SetAppearance(15, 1, 0);
   }

   // Powerplant -- This denotes is engine is operating
   // LSB = 22, 1 bit (off, on)
   if (AnEngineIsOperating())
   {
      platform->SetAppearance(22, 1, 1);
   }
   else
   {
      platform->SetAppearance(22, 1, 0);
   }

   // NOTE: Bit 16 is shared and depends on the domain type, such as air, land, or even munition.

   // Launch -- This denotes launch smoke (produced by the munition, not the launcher)
   // LSB = 16, 1 bit
   if (mProducesLaunchSmoke)
   {
      if (GetTimeOfFlight_sec() <= mLaunchSmokeTime_sec)
      {
         platform->SetAppearance(16, 1, 1);
         mLaunchFlashSmoke = true;
      }
      else
      {
         platform->SetAppearance(16, 1, 0);
         mLaunchFlashSmoke = false;
      }
   }
   else
   {
      mLaunchFlashSmoke = false;

      // Afterburner -- This denotes is afterburner is operating
      // LSB = 16, 1 bit (off, on)
      if (AnEngineHasAfterburnerOn())
      {
         platform->SetAppearance(16, 1, 1);
      }
      else
      {
         platform->SetAppearance(16, 1, 0);
      }
   }

   if (GetDamageFactor() > 0.01)
   {
      // Some damage present

      // Check for heavy damage
      if (GetDamageFactor() > 0.5)
      {
         if (!mHeavyDamage)
         {
            mLightDamage = false;
            mHeavyDamage = true;
         }
      }
      else
      {
         if (!mLightDamage)
         {
            mLightDamage = true;
            mHeavyDamage = false;
         }
      }
   }
}

void wsf::six_dof::Mover::UpdatePlatformFromVehicle(Mover* aVehicle, WsfPlatform* aPlatform)
{
   if ((aVehicle == nullptr) || (aPlatform == nullptr))
   {
      return;
   }

   double latitude(0.0);
   double longitude(0.0);
   double altitude_m(0.0);
   aVehicle->GetLocationLLA(latitude, longitude, altitude_m);

   double wcsVelX_mps(0.0);
   double wcsVelY_mps(0.0);
   double wcsVelZ_mps(0.0);
   aVehicle->GetVelocityWCS_mps(wcsVelX_mps, wcsVelY_mps, wcsVelZ_mps);

   double wcsYaw_rad(0.0);
   double wcsPitch_rad(0.0);
   double wcsRoll_rad(0.0);
   aVehicle->GetOrientationWCS_rad(wcsYaw_rad, wcsPitch_rad, wcsRoll_rad);

   UtVec3dX accelNED = aVehicle->GetAccelNED_mps2();

   double wcsVel[3] = {wcsVelX_mps, wcsVelY_mps, wcsVelZ_mps};
   aPlatform->SetLocationLLA(latitude, longitude, altitude_m);
   aPlatform->SetVelocityWCS(wcsVel);
   aPlatform->SetOrientationWCS(wcsYaw_rad, wcsPitch_rad, wcsRoll_rad);
   aPlatform->SetAccelerationNED(accelNED.GetData());

   // Update Fuel data
   aPlatform->SetFuelMass(GetTotalFuelRemaining());
   aPlatform->SetEmptyMass(GetEmptyWeight_kg());
}

bool wsf::six_dof::Mover::ExecuteTriggersCB(unsigned int aPointIndex)
{
   return ExecuteTriggers(GetSimulation()->GetSimTime(), aPointIndex);
}

//! Executes any actions tied to the waypoint at aPointIndex
bool wsf::six_dof::Mover::ExecuteTriggers(double aSimTime, unsigned int aPointIndex)
{
   bool tookAction = false;

   // Route and Waypoint can not include any WSF classes; since WsfWaypoint::mCallbackPtr is a WsfObject*
   // we can not pass this functionality into SixDOF. We are required to handle this functionality at the Mover
   // level. To accomplish this, we store the mWsfRoute and refer back to it when needed. This requires the indices of
   // the SixDOF Route and WsfRoute to be the same (they should be).
   if (aPointIndex < mWsfRoutePtr->GetSize())
   {
      WsfWaypoint& waypoint = (*mWsfRoutePtr)[aPointIndex];

      // Check for a script present for the waypoint that we just reached
      WsfStringId scriptId(waypoint.GetScriptId());
      if (scriptId != nullptr)
      {
         if (GetPlatform()->HasScript(scriptId))
         {
            GetPlatform()->ExecuteScript(aSimTime, scriptId.GetString());
            tookAction = true;
         }
         else
         {
            tookAction = WsfCallback::ExecuteAndNotify(aSimTime, GetPlatform(), scriptId);
         }
      }

      WsfCallback* callbackPtr = dynamic_cast<WsfCallback*>(waypoint.GetCallback());
      if (callbackPtr != nullptr)
      {
         // Note: This looks strange, but we have to clone the callback because it's associated with a waypoint that might
         //       be deleted by the callback.  For example, if someone performs a reroute, the current route will be deleted
         //       along with the waypoint and this callback.
         WsfCallback* cbPtr = callbackPtr->Clone();
         cbPtr->Execute(aSimTime, GetPlatform());
         delete cbPtr;
         tookAction = true;
      }
   }

   return tookAction;
}

void wsf::six_dof::Mover::CrashIntoGroundCB(double aSimTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform has fatally crashed.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   WsfObserver::CrashedIntoGround(GetSimulation())(aSimTime, GetPlatform());

   // TODO -- Deleting the platform when it crashes is probably not the behavior we want
   GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
}

//! A helper function to write the kinematic status of the mover to stdout. This function is invoked by the mover
//! and the guidance computer to write out a common status message at important events. It is expected that the
//! caller has already written out the introductory line that indicates the event that occurred.
void wsf::six_dof::Mover::WriteKinematicStatus(ut::log::MessageStream& aStream)
{
   if (mFMM_Time < 0.0)
   {
      return; // platform has not started moving on its own
   }

   // Ground range is estimated using a spherical earth.
   double lat, lon, alt;
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

   const Environment* env   = GetEnvironment();
   double             speed = GetPlatform()->GetSpeed();
   double             mach  = env->CalcMachFromFps(alt, speed * UtMath::cFT_PER_M);

   double hdg   = 0.0;
   double pitch = 0.0;
   double roll  = 0.0;
   GetPlatform()->GetOrientationNED(hdg, pitch, roll);

   aStream << std::fixed;
   aStream.AddNote() << "Lat: " << lat;
   aStream.AddNote() << "Lon: " << lon;
   aStream.AddNote() << "Alt: " << std::setprecision(0) << alt << " m";
   aStream.AddNote() << "Downrange: " << std::setprecision(0) << downRange << " m";
   aStream.AddNote() << "Mass: " << std::setprecision(2) << GetPlatform()->GetMass() << " kg";
   aStream.AddNote() << "Speed: " << std::setprecision(0) << speed << " m/s";
   aStream.AddNote() << "Mach: " << std::setprecision(2) << mach;
   aStream.AddNote() << "Acceleration ECS: " << std::setprecision(2) << aclECS[0] / UtEarth::cACCEL_OF_GRAVITY << " g, "
                     << aclECS[1] / UtEarth::cACCEL_OF_GRAVITY << " g, " << aclECS[2] / UtEarth::cACCEL_OF_GRAVITY
                     << " g";
   aStream.AddNote() << "Heading: " << std::setprecision(4) << hdg * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Pitch: " << std::setprecision(4) << pitch * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Roll: " << std::setprecision(4) << roll * UtMath::cDEG_PER_RAD << " deg";
}

void wsf::six_dof::Mover::CheckForNewlyJettisonedPlatforms(double aSimTime)
{
   while (!mNewlyJettisonedPlatforms.empty())
   {
      WsfPlatform* platform = mNewlyJettisonedPlatforms.front();
      GetSimulation()->AddPlatform(aSimTime, platform);
      mNewlyJettisonedPlatforms.pop();
   }
}

void wsf::six_dof::Mover::InitializeSubobject(double              aSimTime,
                                              std::string&        aRootName,
                                              Mover*              aObject,
                                              const UtQuaternion& aRotation,
                                              const UtVec3dX&     aOffset)
{
   UtVec3dX relativePosition;
   UtVec3dX relativeOrientation;
   aObject->GetPositionRelativeToParent(relativePosition, relativeOrientation);

   UtVec3dX relativePositionRotated;
   aRotation.Rotate(relativePosition.GetData(),
                    relativePositionRotated.GetData()); // apply the local rotation to the offset
   relativePositionRotated += aOffset;

   UtVec3dX subobjectLocationECS = relativePositionRotated;

   UtQuaternion qOut =
      aRotation * UtQuaternion(relativeOrientation.Get(0), relativeOrientation.Get(1), relativeOrientation.Get(2));

   double psi;
   double theta;
   double phi;
   qOut.Get(psi, theta, phi);

   auto        typeManager  = static_cast<const TypeManager*>(&GetScenario().GetExtension("wsf_six_dof"));
   std::string platformName = typeManager->GetVehiclePlatformTypeMatch(aObject->GetDerivedFromName());
   if (!platformName.empty())
   {
      WsfPlatform* platformPtr = GetScenario().GetPlatformTypes().Find(platformName);
      ;
      if (platformPtr)
      {
         const std::string& icon = platformPtr->GetIcon();
         if (!icon.empty())
         {
            WsfVisualPart* vp   = new WsfVisualPart(GetScenario());
            std::string    name = aObject->GetBaseName();
            vp->SetName(name);
            vp->SetIcon(icon);
            vp->SetLocation(subobjectLocationECS.GetData());
            vp->SetYaw(psi);
            vp->SetPitch(theta);
            vp->SetRoll(phi);
            vp->SetInitiallyTurnedOn(true);
            if (GetPlatform()->AddComponent(vp))
            {
               vp->Initialize(aSimTime);
               // this is  a way to keep track of this to remove on jettison (tied to top level subobject)
               mVisualPartManager[aRootName].push_back(vp);
            }
            else
            {
               delete vp;
            }
         }
      }
   }

   for (auto& it : aObject->GetSubobjectList())
   {
      // Recursively add subobject's subobjects and multiply the matrices in
      InitializeSubobject(aSimTime, aRootName, it.get(), qOut, subobjectLocationECS);
   }
}

void wsf::six_dof::Mover::CopyDataFromParent()
{
   mKinematicState = *(mParentVehicle->GetKinematicState());
}

void wsf::six_dof::Mover::HandleSubobjectJettison(std::unique_ptr<Mover>& aJettisonedObject)
{
   if (aJettisonedObject != nullptr)
   {
      WsfPlatform* platform = CommonSubobjectJettisonTasks(std::move(aJettisonedObject));

      // Since this is a callback, we do not have access to sim time, so
      //  we store this until Update() is called from the simulation.
      if (platform != nullptr)
      {
         mNewlyJettisonedPlatforms.push(platform);
      }
   }
}

void wsf::six_dof::Mover::SetDamageFactor(double aDamageFactor)
{
   if (aDamageFactor >= 0.0)
   {
      if (aDamageFactor <= 1.0)
      {
         if (aDamageFactor > GetDamageFactor())
         {
            SetDamageFactor(aDamageFactor);
         }
      }
   }

   if (GetDamageFactor() > 0.5)
   {
      mHeavyDamage = true;
      mLightDamage = false;
   }
   else if (GetDamageFactor() > 0.001)
   {
      mLightDamage = true;
      mHeavyDamage = false;
   }
   else
   {
      mHeavyDamage = false;
      mLightDamage = false;
   }
}

void wsf::six_dof::Mover::ResetPosition(double aSimTime)
{
   double lat, lon, alt;
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   SetLocationLLA(lat, lon, alt);
}

WsfPlatform* wsf::six_dof::Mover::DirectJettisonSubobjectPlatform(Mover* aJettisonedObject, double aSimTime_sec)
{
   WsfPlatform* platform   = nullptr;
   auto         jettisoned = DirectJettisonOfSubobject(aJettisonedObject, aSimTime_sec);

   if (jettisoned)
   {
      platform = CommonSubobjectJettisonTasks(std::move(jettisoned));
   }

   return platform;
}

WsfPlatform* wsf::six_dof::Mover::CommonSubobjectJettisonTasks(std::unique_ptr<Mover> aJettisonedObject)
{
   if (aJettisonedObject != nullptr)
   {
      for (auto&& it : mVisualPartManager[aJettisonedObject->GetBaseName()])
      {
         WsfVisualPart* vp = it;
         vp->TurnOff(GetSimulation()->GetSimTime());
         vp->GetPlatform()->DeleteComponent(vp->GetName(), cWSF_COMPONENT_VISUAL);
      }
      mVisualPartManager.erase(aJettisonedObject->GetBaseName());

      const std::string derivedFromObjectType = aJettisonedObject->GetDerivedFromName();

      TypeManager& typeManager         = static_cast<TypeManager&>(GetScenario().GetExtension("wsf_six_dof"));
      std::string  desiredPlatformName = typeManager.GetVehiclePlatformTypeMatch(derivedFromObjectType);

      if (desiredPlatformName.empty())
      {
         auto out = ut::log::error() << "Cannot find a platform match for the Mover.";
         out.AddNote() << "Vehicle Type: " << derivedFromObjectType;
         return nullptr;
      }

      // Try to get a pointer to a platform of the desired type
      WsfPlatform* newPlatform = dynamic_cast<WsfPlatform*>(GetScenario().CloneType("platform_type", desiredPlatformName));
      if (newPlatform)
      {
         // Notify subscribers
         mCallbacks.Add(aJettisonedObject->SubobjectJettisoned.Connect(&Mover::HandleSubobjectJettison, this));

         // Copy relevant data from the prototype mover to the jettisoned mover
         aJettisonedObject->SetUpdateInterval(newPlatform->GetMover()->GetUpdateInterval());

         // Push data to the platform
         newPlatform->SetName(aJettisonedObject->GetName());
         UpdatePlatformFromVehicle(aJettisonedObject.get(), newPlatform);
         newPlatform->SetMover(aJettisonedObject.release());

         return newPlatform;
      }
      else
      {
         // The desired object type does not exist
         auto out = ut::log::error() << "The required platform does not exist.";
         out.AddNote() << "Platform: " << desiredPlatformName;
         return nullptr;
      }
   }

   return nullptr;
}

wsf::six_dof::Route* wsf::six_dof::Mover::ConvertWsfRouteToSixDOF_Route(const WsfRoute* aWsfRoute)
{
   double previous_alt = 0.0;

   Route* convertedRoute = new Route();
   for (int i = 0; i < static_cast<int>(aWsfRoute->GetSize()); ++i)
   {
      auto convertedWaypoint = ut::make_unique<Waypoint>();

      const WsfWaypoint& waypoint = aWsfRoute->GetWaypointAt(i);

      if (waypoint.GetAlt() != WsfPath::cUSE_PREVIOUS)
      {
         convertedWaypoint->SetLLA(UtLLAPos(waypoint.GetLat(), waypoint.GetLon(), waypoint.GetAlt()));
         previous_alt = waypoint.GetAlt();
      }
      else
      {
         convertedWaypoint->SetLLA(UtLLAPos(waypoint.GetLat(), waypoint.GetLon(), previous_alt));
      }
      if (waypoint.GetSpeed() != WsfPath::cUSE_PREVIOUS)
      {
         convertedWaypoint->SetSpeed(Waypoint::SPD_TYPE_TAS_KNOTS,
                                     waypoint.GetSpeed() * static_cast<float>(UtMath::cNMPH_PER_MPS));
      }
      if (waypoint.GetRadialAccel() != WsfPath::cUSE_PREVIOUS)
      {
         convertedWaypoint->SetMaxTurnG(Waypoint::TURN_G_TYPE_LATERAL,
                                        (waypoint.GetRadialAccel() / UtEarth::cACCEL_OF_GRAVITY));
      }
      if (waypoint.GetLabelId() != 0)
      {
         convertedWaypoint->SetLabel(waypoint.GetLabelId());
      }
      if (waypoint.GetGotoId() != 0)
      {
         convertedWaypoint->SetGoTo(waypoint.GetGotoId());
      }
      if (waypoint.GetSwitch() != WsfPath::cSWITCH_MOVER_DEFAULT)
      {
         convertedWaypoint->SetWaypointOnPassing((waypoint.GetSwitch() == WsfPath::cSWITCH_ON_PASSING));
      }
      convertedWaypoint->SetFollowVerticalTrack(mFollowVerticalTrack);
      convertedWaypoint->SetId(i);

      convertedRoute->AddWaypointToRouteEnd(convertedWaypoint);
   }

   // Build up the segment map
   convertedRoute->ComputeSegmentMap();

   return convertedRoute;
}

double wsf::six_dof::Mover::GetTotalFuelCapacity() const
{
   return GetTotalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
}

void wsf::six_dof::Mover::AddFuel(double aFuelMass_kg)
{
   AddFuelQuantity_lbs(aFuelMass_kg * UtMath::cLB_PER_KG);
}

double wsf::six_dof::Mover::GetInternalFuelCapacity() const
{
   return GetInternalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
}

double wsf::six_dof::Mover::GetExternalFuelCapacity() const
{
   return GetExternalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
}

double wsf::six_dof::Mover::GetTotalFuelRemaining() const
{
   return GetCurrentTotalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
}

double wsf::six_dof::Mover::GetInternalFuelRemaining() const
{
   return GetCurrentInternalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
}

double wsf::six_dof::Mover::GetExternalFuelRemaining() const
{
   return GetCurrentExternalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
}

double wsf::six_dof::Mover::GetFuelTankCapacity(const std::string& aTankName) const
{
   const FuelTank* fueltank = GetFuelTankByName(aTankName);
   if (fueltank != nullptr)
   {
      return fueltank->GetFuelCapacity_lbs() * UtMath::cKG_PER_LB;
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetFuelInTank(const std::string& aTankName) const
{
   const FuelTank* fueltank = GetFuelTankByName(aTankName);
   if (fueltank != nullptr)
   {
      return fueltank->GetCurrentFuelQuantity_lbs() * UtMath::cKG_PER_LB;
   }

   return 0.0;
}

void wsf::six_dof::Mover::SetFuelInTank(const std::string& aTankName, double aFuel_kg)
{
   SetFuelInTank_lbs(aTankName, aFuel_kg * UtMath::cLB_PER_KG);
}

double wsf::six_dof::Mover::GetTotalFuelFlow() const
{
   double fuelFlow_pph = GetTotalVehicleFuelFlow_pph();
   return fuelFlow_pph * UtMath::cKG_PER_LB / 3600.0; // Convert from lbs/hr to kg/sec
}

void wsf::six_dof::Mover::EmptyAllInternalandExternalTanks()
{
   EmptyAllInternalTanks();
   EmptyAllExternalTanks();
}

void wsf::six_dof::Mover::StartupEngines(double /*aSimTime_sec*/)
{
   IgniteObject(0);
}

void wsf::six_dof::Mover::ShutdownEngines(double /*aSimTime_sec*/)
{
   ShutdownObject(0);
}

bool wsf::six_dof::Mover::JokerFuelReached()
{
   if (GetCurrentTotalFuelTankQuantity_lbs() < mJokerFuel_lbs)
   {
      return true;
   }
   return false;
}

bool wsf::six_dof::Mover::BingoFuelReached()
{
   if (GetCurrentTotalFuelTankQuantity_lbs() < mBingoFuel_lbs)
   {
      return true;
   }
   return false;
}

void wsf::six_dof::Mover::SetYandZGLoads(double aYGLoad, double aZGLoad)
{
   mYGLoad = aYGLoad;
   mZGLoad = aZGLoad;
}

std::string wsf::six_dof::Mover::GetActivePilot() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      return pilot->GetPilotType();
   }

   return "NONE";
}

wsf::six_dof::Lateral::Mode wsf::six_dof::Mover::GetAutopilotLateralModeEnum() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return action->GetLateralChannelMode();
      }
   }

   return Lateral::Undefined;
}

std::string wsf::six_dof::Mover::GetAutopilotLateralMode() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return LateralModeToString(action->GetLateralChannelMode());
      }
   }

   return "Undefined";
}

float wsf::six_dof::Mover::GetAutopilotLateralModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotLateralModeEnum())
   {
   case Lateral::Mode::Heading:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetHeading_deg();
      break;
   }
   case Lateral::Mode::Bank:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetBank_deg();
      break;
   }
   case Lateral::Mode::RollRate:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetRollRate_dps();
      break;
   }
   case Lateral::Mode::DeltaRoll:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetRollDelta_deg();
      break;
   }
   case Lateral::Mode::YawGLoad:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetYawGLoad_g();
      break;
   }
   case Lateral::Mode::YawRate:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetYawRate_dps();
      break;
   }
   case Lateral::Mode::Beta:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetBank_deg();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

wsf::six_dof::Vertical::Mode wsf::six_dof::Mover::GetAutopilotVerticalModeEnum() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return action->GetVerticalChannelMode();
      }
   }

   return Vertical::Undefined;
}

std::string wsf::six_dof::Mover::GetAutopilotVerticalMode() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return VerticalModeToString(action->GetVerticalChannelMode());
      }
   }

   return "Undefined";
}

float wsf::six_dof::Mover::GetAutopilotVerticalModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotVerticalModeEnum())
   {
   case Vertical::Mode::Altitude:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetAltitudeMSL_m();
      break;
   }
   case Vertical::Mode::VertSpeed:
   {
      value = static_cast<float>(UtMath::cM_PER_FT * 60.0) *
              GetActivePilotObject()->GetCurrentAction()->GetVerticalRate_fpm();
      break;
   }
   case Vertical::Mode::PitchGLoad:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetPitchGLoad_g();
      break;
   }
   case Vertical::Mode::PitchAng:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetPitchAngle_deg();
      break;
   }
   case Vertical::Mode::FltPathAng:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetFlightPathAngle_deg();
      break;
   }
   case Vertical::Mode::DeltaPitch:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetDeltaPitch_deg();
      break;
   }
   case Vertical::Mode::Alpha:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetAlpha_deg();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

wsf::six_dof::Speed::Mode wsf::six_dof::Mover::GetAutopilotSpeedModeEnum() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return action->GetSpeedChannelMode();
      }
   }

   return Speed::Undefined;
}

std::string wsf::six_dof::Mover::GetAutopilotSpeedMode() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      AutopilotAction* action = pilot->GetCurrentAction();
      if (action != nullptr)
      {
         return SpeedModeToString(action->GetSpeedChannelMode());
      }
   }

   return "Undefined";
}

float wsf::six_dof::Mover::GetAutopilotSpeedModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotSpeedModeEnum())
   {
   case Speed::Mode::ForwardAccel:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetForwardAcceleration_g();
      break;
   }
   case Speed::Mode::KIAS:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetCalibratedAirSpeed_KCAS();
      break;
   }
   case Speed::Mode::KTAS:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetTrueAirSpeed_KTAS();
      break;
   }
   case Speed::Mode::Mach:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetMach();
      break;
   }
   case Speed::Mode::FPS:
   {
      value = GetActivePilotObject()->GetCurrentAction()->GetSpeed_fps();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

std::string wsf::six_dof::Mover::LateralModeToString(Lateral::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case Lateral::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case Lateral::Mode::Point:
   {
      modeStr = "Point";
      break;
   }
   case Lateral::Mode::Heading:
   {
      modeStr = "Heading";
      break;
   }
   case Lateral::Mode::YawRate:
   {
      modeStr = "Yaw Rate";
      break;
   }
   case Lateral::Mode::YawGLoad:
   {
      modeStr = "Yaw G Load";
      break;
   }
   case Lateral::Mode::Bank:
   {
      modeStr = "Bank";
      break;
   }
   case Lateral::Mode::DeltaRoll:
   {
      modeStr = "Delta Roll";
      break;
   }
   case Lateral::Mode::RollRate:
   {
      modeStr = "Roll Rate";
      break;
   }
   case Lateral::Mode::Beta:
   {
      modeStr = "Beta";
      break;
   }
   default:
   {
      modeStr = "Undefined";
      break;
   }
   }

   return modeStr;
}

std::string wsf::six_dof::Mover::VerticalModeToString(Vertical::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case Vertical::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case Vertical::Mode::Point:
   {
      modeStr = "Point";
      break;
   }
   case Vertical::Mode::Altitude:
   {
      modeStr = "Altitude";
      break;
   }
   case Vertical::Mode::VertSpeed:
   {
      modeStr = "VertSpeed";
      break;
   }
   case Vertical::Mode::PitchAng:
   {
      modeStr = "Pitch Angle";
      break;
   }
   case Vertical::Mode::PitchRate:
   {
      modeStr = "Pitch Rate";
      break;
   }
   case Vertical::Mode::FltPathAng:
   {
      modeStr = "Flight Path Angle";
      break;
   }
   case Vertical::Mode::DeltaPitch:
   {
      modeStr = "Delta Pitch";
      break;
   }
   case Vertical::Mode::PitchGLoad:
   {
      modeStr = "Pitch G Load";
      break;
   }
   case Vertical::Mode::Alpha:
   {
      modeStr = "Alpha";
      break;
   }
   default:
   {
      modeStr = "Undefined";
      break;
   }
   }

   return modeStr;
}

std::string wsf::six_dof::Mover::SpeedModeToString(Speed::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case Speed::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case Speed::Mode::ForwardAccel:
   {
      modeStr = "Forward G Load";
      break;
   }
   case Speed::Mode::FPS:
   {
      modeStr = "FPS";
      break;
   }
   case Speed::Mode::KIAS:
   {
      modeStr = "KIAS";
      break;
   }
   case Speed::Mode::KTAS:
   {
      modeStr = "KTAS";
      break;
   }
   case Speed::Mode::Mach:
   {
      modeStr = "Mach";
      break;
   }
   default:
   {
      modeStr = "Undefined";
      break;
   }
   }

   return modeStr;
}

double wsf::six_dof::Mover::GetStickBackPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetStickBackControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetStickRightPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetStickRightControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetRudderRightPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetRudderRightControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetThrottleMilitaryPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetThrottleMilitaryControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetThrottleAfterburnerPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetThrottleAfterburnerControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetThrustReverserControlPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetThrustReverserControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetSpeedBrakeControlPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetSpeedBrakesControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetLandingGearLeverPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetLandingGearControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetFlapsLeverPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetFlapsControllerPosition();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetSpoilersLeverPosition() const
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      return controller->GetSpoilersControllerPosition();
   }

   return 0.0;
}

void wsf::six_dof::Mover::TakeDirectControlInput()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      pilot->TakeExternalDirectControl();
   }
}

void wsf::six_dof::Mover::ReleaseDirectControlInput()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      pilot->ReleaseExternalDirectControl();
   }
}

void wsf::six_dof::Mover::SetDirectControlInputs(double aStickX, double aStickY, double aRudder, double aThrottleLeverPosition)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      mLastStickX   = aStickX;
      mLastStickY   = aStickY;
      mLastRudder   = aRudder;
      mLastThrottle = aThrottleLeverPosition;
      pilot->SetExternalDirectControlData(mLastStickX,
                                          mLastStickY,
                                          mLastRudder,
                                          mLastThrottle,
                                          mLastSpdBrakeLeverPosition,
                                          mLastSpoilerLeverPosition,
                                          mLastFlapsLeverPosition);
   }
}

bool wsf::six_dof::Mover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot GoToLocation().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   UtLLAPos newPt(aLatitude, aLongitude, aAltitude);

   pilot->FlyAtPoint(newPt);

   return true;
}

bool wsf::six_dof::Mover::TurnToHeading(double                 aSimTime,
                                        double                 aHeading_rad,
                                        double                 aRadialAccel_mps2,
                                        WsfPath::TurnDirection aTurnDirection)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot TurnToHeading().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   CommonController::eAutopilotTurnDir direction = CommonController::TD_DEFAULT;
   if (aTurnDirection == WsfPath::cTURN_DIR_LEFT)
   {
      direction = CommonController::TD_LEFT;
   }
   else if (aTurnDirection == WsfPath::cTURN_DIR_RIGHT)
   {
      direction = CommonController::TD_RIGHT;
   }

   pilot->TurnToHeading(aHeading_rad, aRadialAccel_mps2, direction);

   return true;
}

bool wsf::six_dof::Mover::GoToSpeed(double aSimTime, double aSpeed_mps, double aLinearAccel_mps2, bool aKeepRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot GoToSpeed().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   pilot->GoToSpeed(aSpeed_mps * UtMath::cNMPH_PER_MPS, aLinearAccel_mps2);

   return true;
}

bool wsf::six_dof::Mover::GoToAltitude(double aSimTime, double aAltitude_m, double aClimbDiveRate_mps, bool aKeepRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot GoToAltitude().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   pilot->GoToAltitude(aAltitude_m, aClimbDiveRate_mps);

   return true;
}

bool wsf::six_dof::Mover::TurnToRelativeHeading(double                 aSimTime,
                                                double                 aHeadingChange_rad,
                                                double                 aRadialAccel_mps2,
                                                WsfPath::TurnDirection aTurnDirection)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot TurnToRelativeHeading().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   CommonController::eAutopilotTurnDir direction = CommonController::TD_DEFAULT;
   if (aTurnDirection == WsfPath::cTURN_DIR_LEFT)
   {
      direction = CommonController::TD_LEFT;
   }
   else if (aTurnDirection == WsfPath::cTURN_DIR_RIGHT)
   {
      direction = CommonController::TD_RIGHT;
   }

   double yawRad, pitRad, rolRad;
   GetPlatform()->GetOrientationNED(yawRad, pitRad, rolRad);

   pilot->TurnToHeading(yawRad + aHeadingChange_rad, aRadialAccel_mps2, direction);

   return true;
}

bool wsf::six_dof::Mover::ReturnToRoute(double aSimTime)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot ReturnToRoute().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   pilot->ReturnToPlannedWaypoints();
   mWsfRoutePtr = mDefaultWsfRoutePtr->Clone();
   WsfObserver::RouteChanged(GetSimulation())(this);

   return true;
}

bool wsf::six_dof::Mover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot UpdateRoute().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   if (mWsfRoutePtr != &aRoute)
   {
      mWsfRoutePtr = aRoute.Clone();
   }

   pilot->FlyTempWaypoints(ConvertWsfRouteToSixDOF_Route(mWsfRoutePtr));

   WsfObserver::RouteChanged(GetSimulation())(this);
   return true;
}

bool wsf::six_dof::Mover::GoToWaypoint(double, unsigned int aDestination)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      // We need (aDestination + 1) because SixDOF routes prepend the location of the platform to the route,
      // so it has one more waypoint than a standard WsfRoute.
      if (pilot->IsOnPlannedRoute())
      {
         return pilot->SetPlannedWaypointIndex(static_cast<int>(aDestination) + 1);
      }
      else
      {
         return pilot->SetTempWaypointIndex(static_cast<int>(aDestination) + 1);
      }
   }

   return false;
}

bool wsf::six_dof::Mover::SetDefaultRoute(const WsfRoute& aRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot SetDefaultRoute().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }
   if (mDefaultWsfRoutePtr != &aRoute)
   {
      mDefaultWsfRoutePtr = aRoute.Clone();
   }
   pilot->SetPlannedRoute(ConvertWsfRouteToSixDOF_Route(mDefaultWsfRoutePtr));

   return true;
}

bool wsf::six_dof::Mover::SetTempRoute(const WsfRoute& aRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot == nullptr)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::warning() << "Cannot SetTempRoute().";
         out.AddNote() << "PilotObject is not available.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      return false;
   }

   if (mWsfRoutePtr != &aRoute)
   {
      mWsfRoutePtr = aRoute.Clone();
   }
   pilot->SetTempRoute(ConvertWsfRouteToSixDOF_Route(mWsfRoutePtr));

   return true;
}

bool wsf::six_dof::Mover::SetRoute(double aSimTime, const WsfRoute& aRoute)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      bool success = true;
      success &= SetTempRoute(aRoute);
      success &= pilot->SetPositionToTempWaypoint(0);
      return success;
   }
   else
   {
      auto out = ut::log::error() << "Cannot SetRoute().";
      out.AddNote() << "PilotObject is not available.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      return false;
   }
}

int wsf::six_dof::Mover::RoutePointIndex() const
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      // We need the (...Index() - 1) because SixDOF routes prepend the location of the platform to the route,
      // so it has one more waypoint than a standard WsfRoute.
      if (pilot->IsOnPlannedRoute())
      {
         size_t plannedIndex = pilot->GetPlannedWaypointIndex();
         return (plannedIndex == 0) ? 0 : static_cast<int>(plannedIndex - 1);
      }
      size_t tempIndex = pilot->GetTempWaypointIndex();
      return (tempIndex == 0) ? 0 : static_cast<int>(tempIndex - 1);
   }

   return 0;
}

bool wsf::six_dof::Mover::FlyRates(const UtVec3d& aAngularRates, const UtVec3d& aLongitudinalAccel)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      return pilot->FlyRates(aAngularRates[0], aAngularRates[1], aAngularRates[2], aLongitudinalAccel[0]);
   }

   return false;
}

bool wsf::six_dof::Mover::FlyHeadingSpeedAltitude(double aHeading, double aSpeed, double aAltitude, double aMaxGees, double aMaxClimb)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      return pilot->FlyHeadingSpeedAltitude(UtMath::cRAD_PER_DEG * aHeading,
                                            UtMath::cNMPH_PER_MPS * aSpeed,
                                            UtMath::cFT_PER_M * aAltitude,
                                            aMaxGees,
                                            (UtMath::cFT_PER_M / 60.0) * aMaxClimb);
   }

   return false;
}

bool wsf::six_dof::Mover::TurnInPlaneWithSpeed(const UtVec3d& aPlane, double aGees, double aSpeed)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      UtVec3dX directionVectorNED = GetVelocityNED_mps();
      directionVectorNED.Normalize();

      UtVec3dX planeNormalNED(aPlane.GetData());
      planeNormalNED.Normalize();

      UtVec3dX maneuverRotationY = planeNormalNED.Cross(directionVectorNED);
      UtVec3dX maneuverRotationZ = directionVectorNED.Cross(maneuverRotationY);

      double rotationMatrixData[3][3] = {{directionVectorNED.X(), directionVectorNED.Y(), directionVectorNED.Z()},
                                         {maneuverRotationY.X(), maneuverRotationY.Y(), maneuverRotationY.Z()},
                                         {maneuverRotationZ.X(), maneuverRotationZ.Y(), maneuverRotationZ.Z()}};

      UtDCM maneuverRotationMatrix(rotationMatrixData);

      UtVec3dX accelerationVec(0.0, aGees, 0.0);
      accelerationVec = maneuverRotationMatrix.Transpose() * accelerationVec;

      double rollDelta_rad = atan2(accelerationVec.Z(), accelerationVec.Y());
      double rollDelta_deg = UtMath::cDEG_PER_RAD * rollDelta_rad;

      // Set the modes
      AutopilotAction* action = pilot->GetCurrentAction();
      action->SetRollDelta_deg(static_cast<float>(rollDelta_deg));
      action->SetLateralChannelMode(Lateral::DeltaRoll);

      action->SetPitchGLoad_g(static_cast<float>(aGees));
      action->SetVerticalChannelMode(Vertical::PitchGLoad);

      action->SetTrueAirSpeed_KTAS(static_cast<float>(UtMath::cNMPH_PER_MPS * aSpeed));
      action->SetSpeedChannelMode(Speed::KTAS);

      pilot->GetCommonController()->SetCurrentActivity(action);

      return true;
   }

   return false;
}

bool wsf::six_dof::Mover::TurnInPlaneWithThrottle(const UtVec3d& aPlane, double aGees, double aThrottle)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      UtVec3dX directionVectorNED = GetVelocityNED_mps();
      directionVectorNED.Normalize();

      UtVec3dX planeNormalNED(aPlane.GetData());
      planeNormalNED.Normalize();

      UtVec3dX maneuverRotationY = planeNormalNED.Cross(directionVectorNED);
      UtVec3dX maneuverRotationZ = directionVectorNED.Cross(maneuverRotationY);

      double rotationMatrixData[3][3] = {{directionVectorNED.X(), directionVectorNED.Y(), directionVectorNED.Z()},
                                         {maneuverRotationY.X(), maneuverRotationY.Y(), maneuverRotationY.Z()},
                                         {maneuverRotationZ.X(), maneuverRotationZ.Y(), maneuverRotationZ.Z()}};

      UtDCM maneuverRotationMatrix(rotationMatrixData);

      UtVec3dX accelerationVec(0.0, aGees, 0.0);
      accelerationVec = maneuverRotationMatrix.Transpose() * accelerationVec;

      double rollDelta_rad = atan2(accelerationVec.Z(), accelerationVec.Y());
      double rollDelta_deg = UtMath::cDEG_PER_RAD * rollDelta_rad;

      // Set the modes
      AutopilotAction* action = pilot->GetCurrentAction();
      action->SetRollDelta_deg(static_cast<float>(rollDelta_deg));
      action->SetLateralChannelMode(Lateral::DeltaRoll);

      action->SetPitchGLoad_g(static_cast<float>(aGees));
      action->SetVerticalChannelMode(Vertical::PitchGLoad);

      action->SetThrottle(static_cast<float>(aThrottle - 1.0));
      action->SetSpeedChannelMode(Speed::Throttle);

      pilot->GetCommonController()->SetCurrentActivity(action);

      return true;
   }

   return false;
}

bool wsf::six_dof::Mover::FlyVectorWithSpeed(const UtVec3d& aVelocityVector, double /* aMaxGees */, double aSpeed)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      UtVec3dX velocityNED(aVelocityVector.GetData());

      double heading_rad         = velocityNED.Azimuth();
      double flightPathAngle_rad = velocityNED.Elevation();

      // Set the modes
      AutopilotAction* action = pilot->GetCurrentAction();
      action->SetHeading_rad(static_cast<float>(heading_rad));
      action->SetLateralChannelMode(Lateral::Heading);

      action->SetFlightPathAngle_deg(static_cast<float>(UtMath::cDEG_PER_RAD * flightPathAngle_rad));
      action->SetVerticalChannelMode(Vertical::FltPathAng);

      action->SetTrueAirSpeed_KTAS(static_cast<float>(UtMath::cNMPH_PER_MPS * aSpeed));
      action->SetSpeedChannelMode(Speed::KTAS);

      pilot->GetCommonController()->SetCurrentActivity(action);

      return true;
   }

   return false;
}

bool wsf::six_dof::Mover::FlyVectorWithThrottle(const UtVec3d& aVelocityVector, double /* aMaxGees */, double aThrottle)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      UtVec3dX velocityNED(aVelocityVector.GetData());

      double heading_rad         = velocityNED.Azimuth();
      double flightPathAngle_rad = velocityNED.Elevation();

      // Set the modes
      AutopilotAction* action = pilot->GetCurrentAction();
      action->SetHeading_rad(static_cast<float>(heading_rad));
      action->SetLateralChannelMode(Lateral::Heading);

      action->SetFlightPathAngle_deg(static_cast<float>(UtMath::cDEG_PER_RAD * flightPathAngle_rad));
      action->SetVerticalChannelMode(Vertical::FltPathAng);

      action->SetThrottle(static_cast<float>(aThrottle - 1.0));
      action->SetSpeedChannelMode(Speed::Throttle);

      pilot->GetCommonController()->SetCurrentActivity(action);

      return true;
   }

   return false;
}

void wsf::six_dof::Mover::CommandSlice(double aTargetHeading_deg, double aTargetAltitude_m, double aTargetMach)
{
   double targetAltitude_ft = UtMath::cFT_PER_M * aTargetAltitude_m;

   double currentAltitude_ft = GetAltitudeMSL_ft();
   double currentHeading_deg = GetHeading_deg();
   double currentMach        = GetMach();

   double deltaAltitude_ft = targetAltitude_ft - currentAltitude_ft;
   double deltaHeading_deg = UtMath::NormalizeAngleMinus180_180(aTargetHeading_deg - currentHeading_deg);

   if (currentAltitude_ft < 5000.0 || std::abs(deltaHeading_deg) < 15.0)
   {
      if (aTargetMach == 0.0)
      {
         // BrawlerMover uses initial Mach as the second argument here. We have scenarios that begin
         // with aircraft parked on the tarmac, so we cannot make the same assumption.
         aTargetMach = std::min(1.1 * currentMach, 0.9);
      }

      const wsf::six_dof::Environment* env             = GetEnvironment();
      double                           targetSpeed_fps = env->CalcFpsFromMach(GetAltitudeMSL_m(), aTargetMach);

      double commandedRateOfClimb_fpm = 0.0;

      if (currentMach < 0.8)
      {
         commandedRateOfClimb_fpm = UtMath::Limit(-6.0 * (currentAltitude_ft - 5000.0), 10000.0);
      }
      else
      {
         commandedRateOfClimb_fpm = UtMath::Limit(6.0 * deltaAltitude_ft, 20000.0);
      }

      double commandedRateOfClimb_fps = commandedRateOfClimb_fpm / 60.0;

      double heading = UtMath::cRAD_PER_DEG * aTargetHeading_deg;
      double fpa     = sin(commandedRateOfClimb_fps / targetSpeed_fps);

      UtVec3dX direction(heading, fpa, targetSpeed_fps, true);
      direction.Normalize();

      FlyVectorWithSpeed(direction, MaxPotentialManeuverGLoad(), UtMath::cM_PER_FT * targetSpeed_fps);
   }
   else
   {
      double commandedBankAngle_deg  = 0.0;
      double commandedVerticalLoad_g = 0.0;

      double bankAngleMaximum_deg = 90.0 + UtMath::Lerp(GetPitch_deg(), -30.0, 0.0, 0.0, 45.0);
      double targetGeeBaseline    = MaxPotentialManeuverGLoad();

      if (std::abs(deltaHeading_deg) < 90.0)
      {
         commandedBankAngle_deg  = bankAngleMaximum_deg;
         commandedVerticalLoad_g = targetGeeBaseline + UtMath::Lerp(currentMach, 0.85, 1.2, 0.0, 3.0);
      }
      else
      {
         if (GetPitch_deg() < -20.0)
         {
            commandedBankAngle_deg = std::min(110.0 / 90.0 * std::abs(deltaHeading_deg), bankAngleMaximum_deg);
         }
         else
         {
            commandedBankAngle_deg = bankAngleMaximum_deg;
         }
         commandedVerticalLoad_g = targetGeeBaseline + UtMath::Lerp(currentMach, 0.85, 1.2, 0.0, 2.0);
      }

      double commandedBankAngle_rad = UtMath::cRAD_PER_DEG * commandedBankAngle_deg;

      double commandedGeesUp    = commandedVerticalLoad_g * cos(commandedBankAngle_rad) - 1.0;
      double commandedGeesRight = commandedVerticalLoad_g * sin(commandedBankAngle_rad);

      double bankCommand_rad = std::abs(atan2(commandedGeesRight, commandedGeesUp)) * UtMath::Sign(deltaHeading_deg);
      double bankCommand_deg = UtMath::cDEG_PER_RAD * bankCommand_rad;

      double currentRoll_rad = GetRoll_rad();
      double rolldelta_rad   = UtMath::AngleDifference(currentRoll_rad, bankCommand_rad);
      if (fabs(UtMath::cDEG_PER_RAD * rolldelta_rad) > 90.0)
      {
         commandedVerticalLoad_g = 0.0;
      }
      else
      {
         commandedVerticalLoad_g *= cos(rolldelta_rad);
      }

      // The BrawlerMover implementation converts the g and roll delta command into a maneuver plane,
      // which the Brawler mover then navigates. SixDOF appears to behave more reliably, and closer to
      // the behavior of BrawlerMover, if we simply command the roll and pull directly.

      PilotObject* pilot = GetActivePilotObject();
      if (pilot)
      {
         AutopilotAction* action = pilot->GetCurrentAction();
         action->SetBank_deg(static_cast<float>(bankCommand_deg));
         action->SetLateralChannelMode(Lateral::Bank);

         action->SetPitchGLoad_g(static_cast<float>(commandedVerticalLoad_g));
         action->SetVerticalChannelMode(Vertical::PitchGLoad);

         action->SetThrottle(static_cast<float>(2.0));
         action->SetSpeedChannelMode(Speed::Throttle);

         pilot->GetCommonController()->SetCurrentActivity(action);
      }
   }
}

void wsf::six_dof::Mover::CommandLevelTurn(double aTargetHeading_deg, double aTargetMach, double /* aTargetGees */)
{
   wsf::six_dof::PilotObject* pilot = GetActivePilotObject();

   if (pilot != nullptr)
   {
      PilotObject* pilot = GetActivePilotObject();
      if (pilot)
      {
         // The target g argument is unused, but could be introduced in per-action autopilot limits
         // if (aTargetGees == 0.0) { aTargetGees = MaxPotentialManeuverGLoad(); }

         AutopilotAction* action = pilot->GetCurrentAction();
         action->SetHeading_deg(static_cast<float>(aTargetHeading_deg));
         action->SetLateralChannelMode(Lateral::Heading);

         action->SetFlightPathAngle_deg(static_cast<float>(0.0));
         action->SetVerticalChannelMode(Vertical::FltPathAng);

         action->SetMach(static_cast<float>(aTargetMach));
         action->SetSpeedChannelMode(Speed::Mach);

         pilot->GetCommonController()->SetCurrentActivity(action);
      }
   }
}

void wsf::six_dof::Mover::EnableAutopilot(bool aState)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      pilot->EnableAutopilot(aState);
   }
}

void wsf::six_dof::Mover::ActivateSimpleManualPilot()
{
   PilotManager* pilotMgr = GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeSimpleManualPilotActive();
   }
}

void wsf::six_dof::Mover::ActivateAugmentedManualPilot()
{
   PilotManager* pilotMgr = GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeAugmentedManualPilotActive();
   }
}

void wsf::six_dof::Mover::ActivateSyntheticPilot()
{
   PilotManager* pilotMgr = GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeSyntheticPilotActive();
   }
}

void wsf::six_dof::Mover::RevertLimitsToDefaults()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      CommonController* autopilot = controller->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->RevertLimitsAndSettingsToDefaults();
      }
   }
}

double wsf::six_dof::Mover::GetPitchGLoadMin()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.pitchGLoad_Min);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetPitchGLoadMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.pitchGLoad_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetAlphaMin()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.alpha_Min);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetAlphaMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.alpha_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetPitchRateMin()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.pitchRate_Min);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetPitchRateMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.pitchRate_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetVerticalSpeedMin()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.vertSpd_Min);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetVerticalSpeedMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.vertSpd_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetYawGLoadMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.yawGLoad_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetBetaMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.beta_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetYawRateMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.yawRate_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetRollRateMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.rollRate_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetBankAngleMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.bankAngle_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetForwardAccelMin()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.forwardAccel_Min);
      }
   }
   return 0.0;
}

double wsf::six_dof::Mover::GetForwardAccelMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.forwardAccel_Max);
      }
   }
   return 0.0;
}

void wsf::six_dof::Mover::SetPitchGLoadMin(double aGees)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMinPitchGLimit(aGees);
      }
   }
}

void wsf::six_dof::Mover::SetPitchGLoadMax(double aGees)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxPitchGLimit(aGees);
      }
   }
}

void wsf::six_dof::Mover::SetAlphaMin(double aAlpha)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMinAlphaLimit(aAlpha);
      }
   }
}

void wsf::six_dof::Mover::SetAlphaMax(double aAlpha)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxAlphaLimit(aAlpha);
      }
   }
}

void wsf::six_dof::Mover::SetPitchRateMin(double aPitchRate)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMinPitchRateLimit(aPitchRate);
      }
   }
}

void wsf::six_dof::Mover::SetPitchRateMax(double aPitchRate)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxPitchRateLimit(aPitchRate);
      }
   }
}

void wsf::six_dof::Mover::SetVerticalSpeedMin(double aVertSpeed)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMinVertSpeedLimit(aVertSpeed);
      }
   }
}

void wsf::six_dof::Mover::SetVerticalSpeedMax(double aVertSpeed)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxVertSpeedLimit(aVertSpeed);
      }
   }
}

void wsf::six_dof::Mover::SetYawGLoadMax(double aYawGLoad)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxYawGLimit(aYawGLoad);
      }
   }
}

void wsf::six_dof::Mover::SetBetaMax(double aBeta)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxBetaLimit(aBeta);
      }
   }
}

void wsf::six_dof::Mover::SetYawRateMax(double aYawRate)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxYawRateLimit(aYawRate);
      }
   }
}

void wsf::six_dof::Mover::SetRollRateMax(double aRollRate)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxRollRateLimit(aRollRate);
      }
   }
}

void wsf::six_dof::Mover::SetBankAngleMax(double aBankAngle)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxBankAngleLimit(aBankAngle);
      }
   }
}

void wsf::six_dof::Mover::SetForwardAccelMin(double aForwardAccel)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMinForwardAccelLimit(aForwardAccel);
      }
   }
}

void wsf::six_dof::Mover::SetForwardAccelMax(double aForwardAccel)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxForwardAccelLimit(aForwardAccel);
      }
   }
}

void wsf::six_dof::Mover::SetTurnRollInMultiplier(double aValue)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentTurnRollInMultiplier(aValue);
      }
   }
}

void wsf::six_dof::Mover::SetRouteAllowableAngleError(double aValue_rad)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetRouteAllowableAngleError_rad(aValue_rad);
      }
   }
}

double wsf::six_dof::Mover::GetCurrentTurnRollInMultiplier()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.turnRollInMultiplier);
      }
   }

   return 1.0;
}

double wsf::six_dof::Mover::GetDefaultTurnRollInMultiplier()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         return autopilot->GetDefaultTurnRollInMultiplier();
      }
   }

   return 1.0;
}

double wsf::six_dof::Mover::GetCurrentRouteAllowableAngleError()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.routeAllowableAngleError_rad);
      }
   }

   return 1.0;
}

double wsf::six_dof::Mover::GetDefaultRouteAllowableAngleError()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         return autopilot->GetDefaultRouteAllowableAngleError_rad();
      }
   }

   return 1.0;
}

wsf::six_dof::Maneuver* wsf::six_dof::Mover::GetCurrentManeuver() const
{
   Maneuver* retval{nullptr};
   if (GetManeuverSequence() != nullptr)
   {
      retval = GetManeuverSequence()->GetCurrentManeuver();
   }
   return retval;
}

wsf::six_dof::ManeuverSequence* wsf::six_dof::Mover::GetManeuverSequence() const
{
   return mManeuverPtr.get();
}

void wsf::six_dof::Mover::ExecuteManeuver(double aSimTime, std::unique_ptr<wsf::six_dof::Maneuver>&& aManeuverPtr)
{
   std::unique_ptr<ManeuverSequence> sqnPtr{nullptr};
   if (aManeuverPtr->IsSequence())
   {
      sqnPtr.reset(static_cast<ManeuverSequence*>(aManeuverPtr.release()));
   }
   else
   {
      sqnPtr = ut::make_unique<ManeuverSequence>(); // Formerly sqnPtr.reset(new ManeuverSequence());
      sqnPtr->Append(std::move(aManeuverPtr));      // This may not be needed
   }
   ExecuteManeuverSequence(aSimTime, std::move(sqnPtr));
}

void wsf::six_dof::Mover::ExecuteManeuverSequence(double                                            aSimTime,
                                                  std::unique_ptr<wsf::six_dof::ManeuverSequence>&& aSequencePtr)
{
   mManeuverPtr = std::move(aSequencePtr);
   mManeuverPtr->Assign(aSimTime, this);
}

void wsf::six_dof::Mover::CancelManeuvers()
{
   if (mManeuverPtr)
   {
      mManeuverPtr->Cancel();
   }
}

void wsf::six_dof::Mover::GetInitialState(double aLocWCS[3], double aVelWCS[3], double aOrientationNED[3]) const
{
   if (mWsfRoutePtr != nullptr)
   {
      const WsfRoute::WaypointVector& waypoints = mWsfRoutePtr->GetWaypoints();
      if (!waypoints.empty())
      {
         double lat0, lon0, alt0;
         waypoints.at(0).GetLocationLLA(lat0, lon0, alt0);
         WsfGeoPoint point{lat0, lon0, alt0};
         point.GetLocationWCS(aLocWCS);

         double heading = 0.0;
         double pitch   = 0.0;
         if (waypoints.size() > 1)
         {
            double lat1, lon1, alt1;
            waypoints.at(1).GetLocationLLA(lat1, lon1, alt1);

            double distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(lat0, lon0, lat1, lon1, heading, distance);
            pitch = atan2(alt1 - alt0, distance);
         }
         aOrientationNED[0] = heading * UtMath::cRAD_PER_DEG;
         aOrientationNED[1] = pitch;
         aOrientationNED[2] = 0.0;

         double   velECS[3] = {static_cast<double>(waypoints.at(0).GetSpeed()), 0.0, 0.0};
         UtEntity temp;
         temp.SetLocationLLA(lat0, lon0, alt0);
         temp.SetOrientationNED(aOrientationNED[0], aOrientationNED[1], aOrientationNED[2]);
         temp.ConvertECSVectorToWCS(aVelWCS, velECS);
      }
   }
}

void wsf::six_dof::Mover::SetInitialLatLon(double aLat, double aLon)
{
   mInitialPosition_Latitude  = aLat;
   mInitialPosition_Longitude = aLon;
   mValidInitialPosition      = true;
}

void wsf::six_dof::Mover::SetInitialAlt(double aAlt_m)
{
   mInitialAltitude_m       = aAlt_m;
   mValidInitialAltitude_ft = true;
}

void wsf::six_dof::Mover::SetInitialNEDHeading(double aHeading_rad)
{
   mInitialNEDHeading      = aHeading_rad;
   mValidInitialNEDHeading = true;
}

void wsf::six_dof::Mover::SetInitialNEDPitch(double aPitch_rad)
{
   mInitialNEDPitch      = aPitch_rad;
   mValidInitialNEDPitch = true;
}

void wsf::six_dof::Mover::SetInitialNEDRoll(double aRoll_rad)
{
   mInitialNEDRoll      = aRoll_rad;
   mValidInitialNEDRoll = true;
}

void wsf::six_dof::Mover::SetInitialNEDVelocity(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   mInitialSpeed_NED_X_fps = aVelNorth_mps * UtMath::cFT_PER_M;
   mInitialSpeed_NED_Y_fps = aVelEast_mps * UtMath::cFT_PER_M;
   mInitialSpeed_NED_Z_fps = aVelDown_mps * UtMath::cFT_PER_M;
   mValidInitialSpeed_NED  = true;
}

double wsf::six_dof::Mover::GetTotalThrust()
{
   return GetCurrentThrust_lbs() * UtMath::cNT_PER_LB;
}

double wsf::six_dof::Mover::GetTotalForwardThrust()
{
   return GetCurrentForwardThrust_lbs() * UtMath::cNT_PER_LB;
}

bool wsf::six_dof::Mover::GetAfterburnerOn() const
{
   return AnEngineHasAfterburnerOn();
}

double wsf::six_dof::Mover::GetEngineThrust(const std::string& aEngineName) const
{
   const ThrustProducerObject* engine = GetThrustProducerObjectByName(aEngineName);
   if (engine != nullptr)
   {
      return engine->GetForwardThrust_lbs() * UtMath::cNT_PER_LB;
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetEngineFuelFlowRate(const std::string& aEngineName) const
{
   const ThrustProducerObject* engine = GetThrustProducerObjectByName(aEngineName);
   if (engine != nullptr)
   {
      return engine->GetFuelBurnRate_pph() * UtMath::cKG_PER_LB / 3600.0; // Convert to kg/sec
   }

   return 0.0;
}

bool wsf::six_dof::Mover::GetEngineAfterburnerOn(const std::string& aEngineName) const
{
   const ThrustProducerObject* engine = GetThrustProducerObjectByName(aEngineName);
   if (engine != nullptr)
   {
      return engine->AfterburnerOn();
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetTotalFuelFlowRate() const
{
   return GetTotalVehicleFuelFlow_pph() * UtMath::cKG_PER_LB / 3600.0; // Convert to kg/sec
}

void wsf::six_dof::Mover::SetAutopilotPitchAngle(double aPitchAngle_deg)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotPitchAngle(aPitchAngle_deg);
   }
}

void wsf::six_dof::Mover::SetAutopilotFlightPathAngle(double aFlightPathAngle_deg)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotFlightPathAngle(aFlightPathAngle_deg);
   }
}

void wsf::six_dof::Mover::SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotVerticalSpeed(aVerticalSpeed_fpm);
   }
}

void wsf::six_dof::Mover::SetAutopilotAltitude(double aAltitude_ft)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotAltitude(aAltitude_ft);
   }
}

void wsf::six_dof::Mover::SetAutopilotRollAngle(double aRollAngle_deg)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotRollAngle(aRollAngle_deg);
   }
}

void wsf::six_dof::Mover::SetAutopilotRollRate(double aRollRate_dps)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotRollRate(aRollRate_dps);
   }
}

void wsf::six_dof::Mover::SetPitchGLoad(double aGLoad)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetPitchGLoad(aGLoad);
   }
}

void wsf::six_dof::Mover::SetAutopilotPitchRate(double aPitchRate_dps)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotPitchRate(aPitchRate_dps);
   }
}

void wsf::six_dof::Mover::SetAutopilotDeltaPitch(double DeltaPitchAngle_deg)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotDeltaPitch(DeltaPitchAngle_deg);
   }
}

void wsf::six_dof::Mover::SetAutopilotDeltaRoll(double DeltaRollAngle_deg)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotDeltaRoll(DeltaRollAngle_deg);
   }
}

void wsf::six_dof::Mover::SetAutopilotSpeedKTAS(double aSpeed_KTAS)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotSpeedKTAS(aSpeed_KTAS);
   }
}

void wsf::six_dof::Mover::SetAutopilotSpeedKCAS(double aSpeed_KCAS)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotSpeedKCAS(aSpeed_KCAS);
   }
}

void wsf::six_dof::Mover::SetAutopilotSpeedMach(double aSpeed_Mach)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotSpeedMach(aSpeed_Mach);
   }
}

void wsf::six_dof::Mover::SetAutopilotThrottle(double aThrottle)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotThrottle(aThrottle);
   }
}

void wsf::six_dof::Mover::SetAutopilotLateralWaypointMode()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotLateralWaypointMode();
   }
}

void wsf::six_dof::Mover::SetAutopilotVerticalWaypointMode()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotVerticalWaypointMode();
   }
}

void wsf::six_dof::Mover::SetAutopilotSpeedWaypointMode()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotSpeedWaypointMode();
   }
}

void wsf::six_dof::Mover::SetAutopilotWaypointMode()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotWaypointMode();
   }
}

void wsf::six_dof::Mover::SetAutopilotNoControl()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->SetAutopilotNoControl();
   }
}

void wsf::six_dof::Mover::EnableDirectThrottleInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->EnableDirectThrottleInput();
   }
}

void wsf::six_dof::Mover::ReleaseDirectThrottleInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->ReleaseDirectThrottleInput();
   }
}

void wsf::six_dof::Mover::MoveThrottleToIdle()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->MoveThrottleToIdle();
   }
}

void wsf::six_dof::Mover::MoveThrottleToFull()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->MoveThrottleToFull();
   }
}

void wsf::six_dof::Mover::MoveThrottleToAfterburner()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->MoveThrottleToAfterburner();
   }
}

void wsf::six_dof::Mover::SetDirectThrottleInput(double aThrottleLeverPosition)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      mLastThrottle = aThrottleLeverPosition;
      controller->SetDirectThrottleInput(mLastThrottle);
   }
}

void wsf::six_dof::Mover::EnableDirectStickRightInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->EnableDirectStickRightInput();
   }
}

void wsf::six_dof::Mover::EnableDirectStickBackInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->EnableDirectStickBackInput();
   }
}

void wsf::six_dof::Mover::EnableDirectRudderRightInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->EnableDirectRudderRightInput();
   }
}

void wsf::six_dof::Mover::ReleaseDirectStickRightInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->ReleaseDirectStickRightInput();
   }
}

void wsf::six_dof::Mover::ReleaseDirectStickBackInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->ReleaseDirectStickBackInput();
   }
}

void wsf::six_dof::Mover::ReleaseDirectRudderRightInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->ReleaseDirectRudderRightInput();
   }
}

void wsf::six_dof::Mover::SetDirectStickRightInput(double aStickRightPosition)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      mLastStickX = aStickRightPosition;
      controller->SetDirectStickRightInput(mLastStickX);
   }
}

void wsf::six_dof::Mover::SetDirectStickBackInput(double aStickBackPosition)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      mLastStickY = aStickBackPosition;
      controller->SetDirectStickBackInput(mLastStickY);
   }
}

void wsf::six_dof::Mover::SetDirectRudderRightInput(double aRudderRightPosition)
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      mLastStickY = aRudderRightPosition;
      controller->SetDirectRudderRightInput(mLastStickY);
   }
}

void wsf::six_dof::Mover::EnableDirectSpeedBrakeInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->EnableDirectSpeedBrakeInput();
   }
}

void wsf::six_dof::Mover::ReleaseDirectSpeedBrakeInput()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->ReleaseDirectSpeedBrakeInput();
   }
}

void wsf::six_dof::Mover::OpenSpeedBrake()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->OpenSpeedBrake();
   }
}

void wsf::six_dof::Mover::CloseSpeedBrake()
{
   PilotObject* controller = GetActivePilotObject();
   if (controller != nullptr)
   {
      controller->CloseSpeedBrake();
   }
}

double wsf::six_dof::Mover::GetTotalWeight_lbs() const
{
   return GetCurrentWeight_lbs();
}

double wsf::six_dof::Mover::GetTotalWeight_kg() const
{
   return GetCurrentWeight_lbs() * UtMath::cKG_PER_LB;
}

void wsf::six_dof::Mover::SetDestroyed()
{
   mIsDestroyed = true;

   ut::Random& rand = GetSimulation()->GetRandom();

   double pitchInputModifier = rand.Uniform(-1.0, 1.0);
   double rollInputModifier  = rand.Uniform(-1.0, 1.0);

   // 70% chance of being on fire
   bool   isOnFire = false;
   double fireVal  = rand.Uniform(0.0, 1.0);
   if (fireVal < 0.7)
   {
      isOnFire = true;
   }

   // 70% chance of having engine smoke
   bool   engineIsSmoking = false;
   double smokeVal        = rand.Uniform(0.0, 1.0);
   if (smokeVal < 0.7)
   {
      engineIsSmoking = true;
   }

   // 50% chance of trailing smoke
   bool   isTrailingSmoke = false;
   double trailVal        = rand.Uniform(0.0, 1.0);
   if (trailVal < 0.5)
   {
      isTrailingSmoke = true;
   }

   // Set various fire and smoke parameters, but only if true
   // (We don't want to turn off smoke that already exists)
   if (isOnFire)
   {
      ActivateFlamesEffect(isOnFire);
   }
   if (engineIsSmoking)
   {
      MakeEnginesSmoke();
   }
   if (isTrailingSmoke)
   {
      ActivateDamageSmokeTrailingEffect(isTrailingSmoke);
   }

   // Tell the pilot manager that we're destroyed
   PilotManager* pilotMgr = GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->SetDestroyed(pitchInputModifier, rollInputModifier);
   }
}

double wsf::six_dof::Mover::GetCLMaxAtMach(double aMach) const
{
   const CommonController* controller = GetPilotManager()->GetSpecialCommonController();
   if (controller != nullptr)
   {
      double clMax = 0;
      if (controller->GetCLMaxMach(aMach, clMax))
      {
         return clMax;
      }
   }

   return 0.0;
}

double wsf::six_dof::Mover::GetAlphaAtCLMaxAtMach_deg(double aMach) const
{
   const CommonController* controller = GetPilotManager()->GetSpecialCommonController();
   if (controller != nullptr)
   {
      double alphaMax_deg = 0;
      if (controller->GetAlphaMaxMach(aMach, alphaMax_deg))
      {
         return alphaMax_deg;
      }
   }

   return 0.0;
}
