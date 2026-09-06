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

#include "WsfP6DOF_Mover.hpp"

#include <iomanip>
#include <string>

#include "P6DofFuelTank.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofUtils.hpp"
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
#include "WsfP6DOF_EventPipe.hpp"
#include "WsfP6DOF_ObjectManager.hpp"
#include "WsfP6DOF_Observer.hpp"
#include "WsfP6DOF_TypeManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPartObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfVisualPart.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "maneuvers/WsfManeuver.hpp"
#include "maneuvers/WsfManeuverSequence.hpp"

namespace
{
const double cP6DOF_MOVER_UNDEFINED_DOUBLE = 1.0E+30; // This must be a large POSITIVE value

/** Check an input for frequency units (hz) and convert to seconds if so. */
void CheckForFrequencyUnits(double& value, UtInput& aInput)
{
   std::string units;
   aInput.ReadValue(value);
   aInput.ReadValue(units);
   UtStringUtil::ToLower(units);
   UtInput::ValueType valueType = UtInput::GetValueTypeOf(units);
   // If the user input a frequency instead of seconds, convert it for them
   if (valueType == UtInput::ValueType::cFREQUENCY)
   {
      // If our value us not 0, convert it to seconds.
      if (value != 0.0)
      {
         value = (1.0 / value);
      }
   }
}
} // namespace

WsfP6DOF_Mover::WsfP6DOF_Mover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , RouteChanged()
   , StageIgnition()
   , StageBurnout()
   , StageSeparation()
   , mVectorToTarget_NED()
   , mMaxGees(0.0)
   , mSpeed(0.0)
   , mManeuverPlane()
   , mDrawPtr(nullptr)
   , mP6DofVehiclePtr(nullptr)
   , mP6DofVehicleTypePtr(nullptr)
   , mPilotManagerPtr(nullptr)
   , mP6DofVehicleCopyPtr(nullptr)
   , mUseWashInInitConditions(false)
   , mFollowVerticalTrack(false)
   , mInitialPosition_Latitude(0)
   , mInitialPosition_Longitude(0)
   , mInitialAltitude_m(0)
   , mInitialNEDHeading(0)
   , mInitialNEDPitch(0)
   , mInitialNEDRoll(0)
   , mTargetPosition_Latitude(0)
   , mTargetPosition_Longitude(0)
   , mInitialSpeed_NED_X_fps(0)
   , mInitialSpeed_NED_Y_fps(0)
   , mInitialSpeed_NED_Z_fps(0)
   , mValidInitialPosition(false)
   , mValidInitialAltitude_ft(false)
   , mValidInitialNEDHeading(false)
   , mValidInitialNEDPitch(false)
   , mValidInitialNEDRoll(false)
   , mValidInitialSpeed_NED(false)
   , mLastStickX(0.0)
   , mLastStickY(0.0)
   , mLastRudder(0.0)
   , mLastThrottle(0.0)
   , mLastSpdBrakeLeverPosition(0.0)
   , mLastSpoilerLeverPosition(0.0)
   , mLastFlapsLeverPosition(0.0)
   , mLastLandingGearLeverPosition(0.0)
   , mLastThrustReverserLeverPosition(0.0)
   , mLastThrustVectorYawRightPosition(0.0)
   , mLastThrustVectorPitchUpPosition(0.0)
   , mLastThrustVectorRollRightPosition(0.0)
   , mLastNoseWheelSteeringRightPosition(0.0)
   , mLastWheelBrakeLeftPosition(0.0)
   , mLastWheelBrakeRightPosition(0.0)
   , mLastNWS_Enabled(false)
   , mWsfRoutePtr(nullptr)
   , mDefaultWsfRoutePtr(nullptr)
   , mGuidancePtr(nullptr)
   , mYGLoad(0.0)
   , mZGLoad(0.0)
   , mCommandedYawRate(0.0)
   , mCommandedPitchRate(0.0)
   , mCommandedRollRate(0.0)
   , mCommandedSpeed(-1.0)
   , mCommandedEngineStopTime(cP6DOF_MOVER_UNDEFINED_DOUBLE)
   , mMinimumHeightAboveTerrain(0.0)
   , mStageIndex(0)
   , mFMM_Time(-1.0)
   , mFMM_Heading(0.0)
   , mEnginesOffAtStart(false)
   , mLandingGearDownAtStart(false)
   , mParkingBrakeOnAtStart(false)
   , mThrottleIdleAtStart(false)
   , mThrottleFullAtStart(false)
   , mThrottleAfterburnerAtStart(false)
   , mTaxiModeEnabledAtStart(false)
   , mNWSEnabledAtStart(false)
   , mAutopilotNoControlAtStart(false)
   , mEnableThrustVectoringAtStart(true)
   , mEnableControlsAtStart(true)
   , mProducesLaunchSmoke(false)
   , mLaunchSmokeTime_sec(0.0)
   , mRisingSmokePlume(false)
   , mLaunchFlashSmoke(false)
   , mLightDamage(false)
   , mHeavyDamage(false)
   , mDestroyed(false)
   , mDamageFactor(false)
   , mJokerFuel_lbs(0.0)
   , mBingoFuel_lbs(0.0)
   , mEventPipeManager()
   , mCallbacks()
   , mVisualPartManager()
   , mManeuverPtr{nullptr}
   , mTestSupportObjectPtr(ut::make_unique<WsfP6DOF_MoverTestObject>(this))
{
   UtVec3d::Set(mFMM_LocWCS, 0.0);

   mCurrentTarget.isValid                      = false;
   mCurrentTarget.targetName[0]                = 0;
   mCurrentTarget.targetAziumth_deg            = 0;
   mCurrentTarget.targetElevation_deg          = 0;
   mCurrentTarget.targetSlantRange_nm          = 0;
   mCurrentTarget.guidanceYawAccelCommand_g    = 0;
   mCurrentTarget.guidancePitchAccelCommand_g  = 0;
   mCurrentTarget.guidanceGBias_g              = 0;
   mCurrentTarget.guidanceMaxG                 = 0;
   mCurrentTarget.guidanceFlightPathAngle_deg  = 0;
   mCurrentTarget.autopilotYawGLoadCommand_g   = 0;
   mCurrentTarget.autopilotPitchGLoadCommand_g = 0;
   mCurrentTarget.autopilotRollRateCommand_dps = 0;

   // Set the default message rates to off.
   mEventPipeManager["MsgP6dofCoreData"]        = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofKinematic"]       = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofEngineFuel"]      = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofAutopilot"]       = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofAutopilotLimits"] = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofControlInputs"]   = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofControlSurfaces"] = std::pair<double, int>(0.0, 0);
   mEventPipeManager["MsgP6dofForceMoment"]     = std::pair<double, int>(0.0, 0);
}

// =================================================================
WsfP6DOF_Mover::~WsfP6DOF_Mover()
{
   // If there are any platforms that have not been added to the simulation we need to delete them.
   while (!mNewlyJettisonedPlatforms.empty())
   {
      delete mNewlyJettisonedPlatforms.front();
      mNewlyJettisonedPlatforms.pop();
   }

   mP6DofVehicleTypePtr = nullptr; // do not delete this pointer, object owned by type manager
   if (mDrawPtr != nullptr)
   {
      delete mDrawPtr;
   }
   if (mP6DofVehicleCopyPtr != nullptr)
   {
      delete mP6DofVehicleCopyPtr;
   }
   if (mWsfRoutePtr != nullptr)
   {
      delete mWsfRoutePtr;
   }
   if (mDefaultWsfRoutePtr != nullptr)
   {
      delete mDefaultWsfRoutePtr;
   }
}

// =================================================================
WsfP6DOF_Mover::WsfP6DOF_Mover(const WsfP6DOF_Mover& aSrc)
   : WsfMover(aSrc)
   , RouteChanged()
   , StageIgnition()
   , StageBurnout()
   , StageSeparation()
   , mVectorToTarget_NED(aSrc.mVectorToTarget_NED)
   , mMaxGees(aSrc.mMaxGees)
   , mSpeed(aSrc.mSpeed)
   , mManeuverPlane(aSrc.mManeuverPlane)
   , mDrawPtr(nullptr)
   , mP6DofVehiclePtr(aSrc.mP6DofVehiclePtr)
   , mP6DofVehicleTypePtr(nullptr)
   , mPilotManagerPtr(nullptr)
   , mP6DofVehicleCopyPtr(nullptr)
   , mUseWashInInitConditions(aSrc.mUseWashInInitConditions)
   , mFollowVerticalTrack(aSrc.mFollowVerticalTrack)
   , mNewlyJettisonedPlatforms()
   , // TODO We probably don't need to copy these, But should we?
   mInitialPosition_Latitude(aSrc.mInitialPosition_Latitude)
   , mInitialPosition_Longitude(aSrc.mInitialPosition_Longitude)
   , mInitialAltitude_m(aSrc.mInitialAltitude_m)
   , mInitialNEDHeading(aSrc.mInitialNEDHeading)
   , mInitialNEDPitch(aSrc.mInitialNEDPitch)
   , mInitialNEDRoll(aSrc.mInitialNEDRoll)
   , mTargetPosition_Latitude(aSrc.mTargetPosition_Latitude)
   , mTargetPosition_Longitude(aSrc.mTargetPosition_Longitude)
   , mInitialSpeed_NED_X_fps(aSrc.mInitialSpeed_NED_X_fps)
   , mInitialSpeed_NED_Y_fps(aSrc.mInitialSpeed_NED_Y_fps)
   , mInitialSpeed_NED_Z_fps(aSrc.mInitialSpeed_NED_Z_fps)
   , mValidInitialPosition(aSrc.mValidInitialPosition)
   , mValidInitialAltitude_ft(aSrc.mValidInitialAltitude_ft)
   , mValidInitialNEDHeading(aSrc.mValidInitialNEDHeading)
   , mValidInitialNEDPitch(aSrc.mValidInitialNEDPitch)
   , mValidInitialNEDRoll(aSrc.mValidInitialNEDRoll)
   , mValidInitialSpeed_NED(aSrc.mValidInitialSpeed_NED)
   , mLastStickX(aSrc.mLastStickX)
   , mLastStickY(aSrc.mLastStickY)
   , mLastRudder(aSrc.mLastRudder)
   , mLastThrottle(aSrc.mLastThrottle)
   , mLastSpdBrakeLeverPosition(aSrc.mLastSpdBrakeLeverPosition)
   , mLastSpoilerLeverPosition(aSrc.mLastSpoilerLeverPosition)
   , mLastFlapsLeverPosition(aSrc.mLastFlapsLeverPosition)
   , mLastLandingGearLeverPosition(aSrc.mLastLandingGearLeverPosition)
   , mLastThrustReverserLeverPosition(aSrc.mLastThrustReverserLeverPosition)
   , mLastThrustVectorYawRightPosition(aSrc.mLastThrustVectorYawRightPosition)
   , mLastThrustVectorPitchUpPosition(aSrc.mLastThrustVectorPitchUpPosition)
   , mLastThrustVectorRollRightPosition(aSrc.mLastThrustVectorRollRightPosition)
   , mLastNoseWheelSteeringRightPosition(aSrc.mLastNoseWheelSteeringRightPosition)
   , mLastWheelBrakeLeftPosition(aSrc.mLastWheelBrakeLeftPosition)
   , mLastWheelBrakeRightPosition(aSrc.mLastWheelBrakeRightPosition)
   , mLastNWS_Enabled(aSrc.mLastNWS_Enabled)
   , mWsfRoutePtr(nullptr)
   , mDefaultWsfRoutePtr(nullptr)
   , mGuidancePtr(nullptr)
   , mYGLoad(aSrc.mYGLoad)
   , mZGLoad(aSrc.mZGLoad)
   , mCommandedYawRate(aSrc.mCommandedYawRate)
   , mCommandedPitchRate(aSrc.mCommandedPitchRate)
   , mCommandedRollRate(aSrc.mCommandedRollRate)
   , mCommandedSpeed(aSrc.mCommandedSpeed)
   , mCommandedEngineStopTime(aSrc.mCommandedEngineStopTime)
   , mMinimumHeightAboveTerrain(aSrc.mMinimumHeightAboveTerrain)
   , mStageIndex(aSrc.mStageIndex)
   , mFMM_Time(-1.0)
   , mFMM_Heading(0.0)
   , mCurrentTarget(aSrc.mCurrentTarget)
   , mEnginesOffAtStart(aSrc.mEnginesOffAtStart)
   , mLandingGearDownAtStart(aSrc.mLandingGearDownAtStart)
   , mParkingBrakeOnAtStart(aSrc.mParkingBrakeOnAtStart)
   , mThrottleIdleAtStart(aSrc.mThrottleIdleAtStart)
   , mThrottleFullAtStart(aSrc.mThrottleFullAtStart)
   , mThrottleAfterburnerAtStart(aSrc.mThrottleAfterburnerAtStart)
   , mTaxiModeEnabledAtStart(aSrc.mTaxiModeEnabledAtStart)
   , mNWSEnabledAtStart(aSrc.mNWSEnabledAtStart)
   , mAutopilotNoControlAtStart(aSrc.mAutopilotNoControlAtStart)
   , mEnableThrustVectoringAtStart(aSrc.mEnableThrustVectoringAtStart)
   , mEnableControlsAtStart(aSrc.mEnableControlsAtStart)
   , mProducesLaunchSmoke(aSrc.mProducesLaunchSmoke)
   , mLaunchSmokeTime_sec(aSrc.mLaunchSmokeTime_sec)
   , mRisingSmokePlume(aSrc.mRisingSmokePlume)
   , mLaunchFlashSmoke(aSrc.mLaunchFlashSmoke)
   , mLightDamage(aSrc.mLightDamage)
   , mHeavyDamage(aSrc.mHeavyDamage)
   , mDestroyed(aSrc.mDestroyed)
   , mDamageFactor(aSrc.mDamageFactor)
   , mJokerFuel_lbs(aSrc.mJokerFuel_lbs)
   , mBingoFuel_lbs(aSrc.mBingoFuel_lbs)
   , mEventPipeManager(aSrc.mEventPipeManager)
   , mCallbacks()
   , mVisualPartManager(aSrc.mVisualPartManager)
   , mManeuverPtr{nullptr}
   , mTestSupportObjectPtr(ut::make_unique<WsfP6DOF_MoverTestObject>(this))
{
   UtVec3d::Set(mFMM_LocWCS, 0.0);

   if (aSrc.mP6DofVehicleTypePtr)
   {
      mP6DofVehicleTypePtr = aSrc.mP6DofVehicleTypePtr;
   }

   if (mP6DofVehiclePtr != nullptr)
   {
      mPilotManagerPtr = mP6DofVehiclePtr->GetPilotManager();
   }

   if (aSrc.mWsfRoutePtr)
   {
      mWsfRoutePtr = aSrc.mWsfRoutePtr->Clone();
   }

   if (aSrc.mDefaultWsfRoutePtr)
   {
      mDefaultWsfRoutePtr = aSrc.mDefaultWsfRoutePtr->Clone();
   }

   if (aSrc.mGuidancePtr)
   {
      // TODO -- Review this
      // mGuidancePtr = aSrc.mGuidancePtr->Clone();
   }

   if (aSrc.mManeuverPtr)
   {
      mManeuverPtr.reset(aSrc.mManeuverPtr->Clone());
   }
}

// =================================================================
WsfMover* WsfP6DOF_Mover::Clone() const
{
   return new WsfP6DOF_Mover(*this);
}

// =================================================================
bool WsfP6DOF_Mover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   std::unique_ptr<WsfRoute> routePtr(nullptr);
   if (WsfRouteTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, routePtr) && routePtr != nullptr)
   {
      if (mP6DofVehicleTypePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "P6DofVehicleType not specified yet!");
      }

      P6DofPilotObject* pilot = mP6DofVehicleTypePtr->PilotObject();
      if (pilot == nullptr)
      {
         throw UtInput::BadValue(aInput, "P6DofVehicleType was not defined with a P6DofPilotObject, can not used route!");
      }

      mWsfRoutePtr = routePtr.release();
      if (mPilotManagerPtr != nullptr)
      {
         P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
         if (pilot != nullptr)
         {
            pilot->SetPlannedRoute(ConvertWsfRouteToP6DofRoute(mWsfRoutePtr));
         }
      }
   }
   else if (command == "p6dof_object_type")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      WsfP6DOF_TypeManager*   mgrPtr      = (WsfP6DOF_TypeManager*)&GetScenario().GetExtension("wsf_p6dof");
      const P6DofVehicleType* tempObjType = mgrPtr->GetObjectTypeByName(typeName);
      if (tempObjType == nullptr)
      {
         throw UtInput::BadValue(aInput, "Could not find requested P6DOF object type!");
      }
      mP6DofVehicleTypePtr = tempObjType;
   }
   else if (command == "engines_on")
   {
      bool enginesOn = true;
      aInput.ReadValue(enginesOn);
      mEnginesOffAtStart = !enginesOn;
   }
   else if (command == "landing_gear_down")
   {
      bool gearDown = false;
      aInput.ReadValue(gearDown);
      mLandingGearDownAtStart = gearDown;
   }
   else if (command == "parking_brake_on")
   {
      bool brakeSet = false;
      aInput.ReadValue(brakeSet);
      mParkingBrakeOnAtStart = brakeSet;
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
   else if (command == "taxi_mode_enabled")
   {
      bool taxiMode = false;
      aInput.ReadValue(taxiMode);
      mTaxiModeEnabledAtStart = taxiMode;
   }
   else if (command == "nws_enabled")
   {
      bool nwsEnabled = false;
      aInput.ReadValue(nwsEnabled);
      mNWSEnabledAtStart = nwsEnabled;
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
   else if (command == "p6dof_position")
   {
      double lat = 0.0;
      double lon = 0.0;
      aInput.ReadValue(lat);
      aInput.ReadValue(lon);
      mInitialPosition_Latitude  = lat;
      mInitialPosition_Longitude = lon;
      mValidInitialPosition      = true;
   }
   else if (command == "p6dof_alt")
   {
      double alt = 0.0;
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);
      mInitialAltitude_m       = alt;
      mValidInitialAltitude_ft = true;
   }
   else if (command == "p6dof_ned_heading")
   {
      double hdg_rad = 0.0;
      aInput.ReadValueOfType(hdg_rad, UtInput::cANGLE);
      mInitialNEDHeading      = hdg_rad;
      mValidInitialNEDHeading = true;
   }
   else if (command == "p6dof_ned_pitch")
   {
      double pitch_rad = 0.0;
      aInput.ReadValueOfType(pitch_rad, UtInput::cANGLE);
      mInitialNEDPitch      = pitch_rad;
      mValidInitialNEDPitch = true;
   }
   else if (command == "p6dof_ned_roll")
   {
      double roll_rad = 0.0;
      aInput.ReadValueOfType(roll_rad, UtInput::cANGLE);
      mInitialNEDRoll      = roll_rad;
      mValidInitialNEDRoll = true;
   }
   else if (command == "p6dof_set_velocity_ned_fps")
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
   else if (command == "p6dof_initsetup")
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
      mEventPipeManager["MsgP6dofCoreData"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_kinematic_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofKinematic"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_engine_fuel_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofEngineFuel"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_autopilot_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofAutopilot"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_autopilot_limits_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofAutopilotLimits"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_control_inputs_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofControlInputs"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_control_surfaces_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofControlSurfaces"] = std::pair<double, int>(dataRate, 0);
   }
   else if (command == "event_force_moment_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgP6dofForceMoment"] = std::pair<double, int>(dataRate, 0);
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================
bool WsfP6DOF_Mover::Initialize(double aSimTime)
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

   // TODO The case where mP6DofVehicleTypePtr, does not seem to throw an error message to the console to inform user,
   // if this check fails, It does not appear it will fail gracefully
   if (mP6DofVehicleTypePtr && mP6DofVehiclePtr == nullptr)
   {
      // Create instance
      mP6DofVehiclePtr = new P6DofVehicle(mP6DofVehicleTypePtr->GetScenario()); // TODO -- Who is responsible for deleting
                                                                                // this?, they need to unregister first
      mCallbacks.Add(mP6DofVehiclePtr->SubobjectJettisoned.Connect(&WsfP6DOF_Mover::HandleSubobjectJettison, this));

      std::string platformName = GetPlatform()->GetName();

      // Initialize the P6DOF object
      mP6DofVehiclePtr->Initialize(aSimTime, mP6DofVehicleTypePtr, platformName);

      parentPlatform->SetFuelMass(GetTotalFuelRemaining());
      parentPlatform->SetEmptyMass(GetEmptyWeight_kg());

      // Get a pointer to the pilot/controller
      if (mPilotManagerPtr == nullptr)
      {
         mPilotManagerPtr = mP6DofVehiclePtr->GetPilotManager();
      }

      P6DofPilotObject* pilot = nullptr;
      if (mPilotManagerPtr != nullptr)
      {
         pilot = mPilotManagerPtr->GetActivePilot();
      }

      pilot = mP6DofVehiclePtr->GetPilotObject();

      if (pilot != nullptr)
      {
         mCallbacks.Add(pilot->WaypointAchieved.Connect(&WsfP6DOF_Mover::ExecuteTriggersCB, this));

         // TODO - Verify that removing this external speed brake control was okay and is not needed
         // P6DofCommonController* autopilot = pilot->GetCommonController();
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
               pilot->SetPlannedRoute(ConvertWsfRouteToP6DofRoute(mWsfRoutePtr));
            }

            double lat0, lon0, alt0;
            waypoints.at(0).GetLocationLLA(lat0, lon0, alt0);
            mP6DofVehiclePtr->SetLocationLLA(lat0, lon0, alt0);

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
            mP6DofVehiclePtr->SetAttitudeNED(heading * UtMath::cRAD_PER_DEG, pitch, 0.0);

            // Get the velocity from the waypoint -- we assume the waypoint is using "true" airspeed, not KIAS/KCAS or Mach
            double   velocityECS[3] = {waypoints.at(0).GetSpeed(), 0.0, 0.0};
            double   velocityWCS[3];
            UtEntity temp;
            temp.SetLocationLLA(lat0, lon0, alt0);
            temp.SetOrientationNED(heading * UtMath::cRAD_PER_DEG, pitch, 0.0);
            temp.ConvertECSVectorToWCS(velocityWCS, velocityECS);

            mP6DofVehiclePtr->SetVelocityWCS(velocityWCS[0], velocityWCS[1], velocityWCS[2]); // mps
         }
      }
      else
      {
         // TODO So if I don't have a route, I always start at 0,0?

         // No route, so start with a default location at LLA(0,0,0) with attitude of level headed north
         mP6DofVehiclePtr->SetLocationLLA(0.0, 0.0, 0.0);
         mP6DofVehiclePtr->SetAttitudeNED(0.0, 0.0, 0.0);
      }

      // Set any initial conditions
      if (mValidInitialPosition)
      {
         // Get the current attitude
         double hdg, pitch, roll;
         mP6DofVehiclePtr->GetAttitudeNED(hdg, pitch, roll);

         // Get the altitude
         double lat, lon, alt;
         mP6DofVehiclePtr->GetLocationLLA(lat, lon, alt);

         // Set the new position
         mP6DofVehiclePtr->SetLocationLLA(mInitialPosition_Latitude, mInitialPosition_Longitude, alt);
         // Set the attitude
         mP6DofVehiclePtr->SetAttitudeNED(hdg, pitch, roll);
      }
      if (mValidInitialAltitude_ft)
      {
         double lat, lon, alt;
         mP6DofVehiclePtr->GetLocationLLA(lat, lon, alt);
         mP6DofVehiclePtr->SetLocationLLA(lat, lon, mInitialAltitude_m);
      }
      if (mValidInitialNEDHeading)
      {
         double hdg, pitch, roll;
         mP6DofVehiclePtr->GetAttitudeNED(hdg, pitch, roll);
         mP6DofVehiclePtr->SetAttitudeNED(mInitialNEDHeading, pitch, roll);
      }
      if (mValidInitialNEDPitch)
      {
         double hdg, pitch, roll;
         mP6DofVehiclePtr->GetAttitudeNED(hdg, pitch, roll);
         mP6DofVehiclePtr->SetAttitudeNED(hdg, mInitialNEDPitch, roll);
      }
      if (mValidInitialNEDRoll)
      {
         double hdg, pitch, roll;
         mP6DofVehiclePtr->GetAttitudeNED(hdg, pitch, roll);
         mP6DofVehiclePtr->SetAttitudeNED(hdg, pitch, mInitialNEDRoll);
      }
      if (mValidInitialPosition && mValidInitialAltitude_ft)
      {
         // Get the current attitude
         double hdg, pitch, roll;
         mP6DofVehiclePtr->GetAttitudeNED(hdg, pitch, roll);

         // Set the new position
         mP6DofVehiclePtr->SetLocationLLA(mInitialPosition_Latitude, mInitialPosition_Longitude, mInitialAltitude_m);
         // Set the attitude
         mP6DofVehiclePtr->SetAttitudeNED(hdg, pitch, roll);
      }
      if (mValidInitialNEDHeading && mValidInitialNEDPitch && mValidInitialNEDRoll)
      {
         // Set the new attitude
         mP6DofVehiclePtr->SetAttitudeNED(mInitialNEDHeading, mInitialNEDPitch, mInitialNEDRoll);
      }
      if (mValidInitialSpeed_NED)
      {
         mP6DofVehiclePtr->SetVelocityNED(mInitialSpeed_NED_X_fps * UtMath::cM_PER_FT,
                                          mInitialSpeed_NED_Y_fps * UtMath::cM_PER_FT,
                                          mInitialSpeed_NED_Z_fps * UtMath::cM_PER_FT);
      }

      if (platformDataAvailable)
      {
         mP6DofVehiclePtr->SetLocationLLA(platformLat, platformLon, platformAlt_m);
         mP6DofVehiclePtr->SetAttitudeNED(platformHeading_rad, platformPitch_rad, platformRoll_rad);
         mP6DofVehiclePtr->SetVelocityNED(platformVelNED_mps[0], platformVelNED_mps[1], platformVelNED_mps[2]);
      }

      mP6DofVehiclePtr->SetWashInInitConditions(mUseWashInInitConditions);

      // Set timing
      // SetUpdateInterval(0.01); we use the interval set in the mover class
      SetUpdateTimeTolerance(0); // Do not accept anything that is "close enough"

      // Generate subobjects
      for (auto& it : mP6DofVehiclePtr->GetSubobjectList())
      {
         UtQuaternion  q;
         UtVec3d       v(0, 0, 0);
         std::string   str     = it->GetBaseName();
         P6DofVehicle* vehicle = it;
         InitializeSubobject(aSimTime, str, vehicle, q, v);
      }
   }

   mDrawPtr = new WsfDraw(*GetSimulation());

   mCallbacks.Add(mP6DofVehiclePtr->CrashedIntoGround.Connect(&WsfP6DOF_Mover::CrashIntoGroundCB, this));

   if (mP6DofVehiclePtr)
   {
      if (mEnginesOffAtStart)
      {
         P6DofPropulsionSystem* propulsion = mP6DofVehiclePtr->GetPropulsionSystem();
         if (propulsion)
         {
            propulsion->Shutdown();
         }
      }

      if (mLandingGearDownAtStart)
      {
         mP6DofVehiclePtr->SetLandingGearPositionAtStart(1.0);
      }
      else
      {
         mP6DofVehiclePtr->SetLandingGearPositionAtStart(0.0);
      }

      if (mParkingBrakeOnAtStart)
      {
         EnableDirectBraking();
         SetParkingBrake();
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

      if (mTaxiModeEnabledAtStart)
      {
         SetTaxiMode(true);
      }

      if (mNWSEnabledAtStart)
      {
         SetEnableNWS(true);
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

      mP6DofVehiclePtr->InitializeConditions(aSimTime);
   }
   UpdatePlatformFromP6DofVehicle(mP6DofVehiclePtr, GetPlatform());

   return WsfMover::Initialize(aSimTime);
}

void WsfP6DOF_Mover::InitializeSubobject(double              aSimTime,
                                         std::string&        aRootName,
                                         P6DofVehicle*       aObject,
                                         const UtQuaternion& aRotation,
                                         const UtVec3d&      aOffset)
{
   UtVec3dX p;
   UtVec3dX o;
   aObject->GetPositionRelativeToParent(p, o);

   double xyzIn[3];
   p.Get(xyzIn);
   double xyzOut[3];
   aRotation.Rotate(xyzIn, xyzOut); // apply the local rotation to the offset
   xyzOut[0] += aOffset.Get(0);
   xyzOut[1] += aOffset.Get(1);
   xyzOut[2] += aOffset.Get(2);
   UtQuaternion qOut = aRotation * UtQuaternion(o.Get(0), o.Get(1), o.Get(2));
   double       ptp[3];
   qOut.Get(ptp[0], ptp[1], ptp[2]);

   WsfP6DOF_TypeManager& typeManager = static_cast<WsfP6DOF_TypeManager&>(GetScenario().GetExtension("wsf_p6dof"));
   std::string platformName = typeManager.GetP6DofVehiclePlatformTypeMatch(aObject->GetDerivedFromObjectName());
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
            vp->SetLocation(xyzOut);
            vp->SetYaw(ptp[0]);
            vp->SetPitch(ptp[1]);
            vp->SetRoll(ptp[2]);
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
      InitializeSubobject(aSimTime,
                          aRootName,
                          it,
                          qOut,
                          UtVec3dX(xyzOut[0], xyzOut[1], xyzOut[2])); // recursively add subobject's subobjects and
                                                                      // multiply the matrices
   }
}

std::string WsfP6DOF_Mover::LateralModeToString(P6DOF::Lateral::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case P6DOF::Lateral::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case P6DOF::Lateral::Mode::Point:
   {
      modeStr = "Point";
      break;
   }
   case P6DOF::Lateral::Mode::Heading:
   {
      modeStr = "Heading";
      break;
   }
   case P6DOF::Lateral::Mode::YawRate:
   {
      modeStr = "Yaw Rate";
   }
   case P6DOF::Lateral::Mode::YawGLoad:
   {
      modeStr = "Yaw G Load";
      break;
   }
   case P6DOF::Lateral::Mode::Bank:
   {
      modeStr = "Bank";
      break;
   }
   case P6DOF::Lateral::Mode::DeltaRoll:
   {
      modeStr = "Delta Roll";
      break;
   }
   case P6DOF::Lateral::Mode::RollRate:
   {
      modeStr = "Roll Rate";
      break;
   }
   case P6DOF::Lateral::Mode::Beta:
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

std::string WsfP6DOF_Mover::VerticalModeToString(P6DOF::Vertical::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case P6DOF::Vertical::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case P6DOF::Vertical::Mode::Point:
   {
      modeStr = "Point";
      break;
   }
   case P6DOF::Vertical::Mode::Altitude:
   {
      modeStr = "Altitude";
      break;
   }
   case P6DOF::Vertical::Mode::VertSpeed:
   {
      modeStr = "VertSpeed";
      break;
   }
   case P6DOF::Vertical::Mode::PitchAng:
   {
      modeStr = "Pitch Angle";
      break;
   }
   case P6DOF::Vertical::Mode::PitchRate:
   {
      modeStr = "Pitch Rate";
      break;
   }
   case P6DOF::Vertical::Mode::FltPathAng:
   {
      modeStr = "Flight Path Angle";
      break;
   }
   case P6DOF::Vertical::Mode::DeltaPitch:
   {
      modeStr = "Delta Pitch";
      break;
   }
   case P6DOF::Vertical::Mode::PitchGLoad:
   {
      modeStr = "Pitch G Load";
      break;
   }
   case P6DOF::Vertical::Mode::Alpha:
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

std::string WsfP6DOF_Mover::SpeedModeToString(P6DOF::Speed::Mode aMode) const
{
   std::string modeStr = "";
   switch (aMode)
   {
   case P6DOF::Speed::Mode::Waypoint:
   {
      modeStr = "Waypoint";
      break;
   }
   case P6DOF::Speed::Mode::ForwardAccel:
   {
      modeStr = "Forward Acceleration";
      break;
   }
   case P6DOF::Speed::Mode::FPS:
   {
      modeStr = "FPS";
      break;
   }
   case P6DOF::Speed::Mode::KIAS:
   {
      modeStr = "KIAS";
      break;
   }
   case P6DOF::Speed::Mode::KTAS:
   {
      modeStr = "KTAS";
      break;
   }
   case P6DOF::Speed::Mode::Mach:
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

bool WsfP6DOF_Mover::ExecuteTriggersCB(unsigned int aPointIndex)
{
   return ExecuteTriggers(GetSimulation()->GetSimTime(), aPointIndex);
}

//! Executes any actions tied to the waypoint at aPointIndex
bool WsfP6DOF_Mover::ExecuteTriggers(double aSimTime, unsigned int aPointIndex)
{
   bool tookAction = false;

   // P6DofRoute and P6DofWaypoint can not including any WSF classes, since WsfWaypoint::mCallbackPtr is a WsfObject*
   // we can not pass this functionality into P6Dof.  We are required to handle this functionality at the WsfP6Dof_Mover
   // level.  To accomplish this, we store the mWsfRoute and refer back to it when needed.  This requires the indices of
   // the P6DofRoute and WsfRoute to be the same (they should be).
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
         // Note: This looks strange, but we have to clone the callback because it's associated with a waypoint that
         // might be deleted by the callback.  For example, if someone performs a reroute, the current route will be
         // deleted along with the waypoint and this callback.
         WsfCallback* cbPtr = callbackPtr->Clone();
         cbPtr->Execute(aSimTime, GetPlatform());
         delete cbPtr;
         tookAction = true;
      }
   }

   return tookAction;
}

void WsfP6DOF_Mover::CrashIntoGroundCB(double aSimTime)
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

void WsfP6DOF_Mover::Update(double aSimTime)
{
   // Check over out event pipe manager data to see if messages need to be sent
   for (auto& eventPipeMessage : mEventPipeManager)
   {
      // Check if this message has a message rate above 0
      if (eventPipeMessage.second.first > 0.0)
      {
         // Difference in simtime from the (number of updates sent * update rate), doing it this way should
         // prevent floating point drift/loss of precision
         double diff = (aSimTime - (double)(eventPipeMessage.second.second * eventPipeMessage.second.first));

         // If we're higher than the message timing threshold, send a message
         if (diff >= eventPipeMessage.second.first)
         {
            eventPipeMessage.second.second++;
            WsfObserver::P6DofSendMessage(GetSimulation())(aSimTime, *GetPlatform(), *this, eventPipeMessage.first);
         }
      }
   }

   if (!mP6DofVehiclePtr)
   {
      return;
   }

   if (mManeuverPtr != nullptr)
   {
      mManeuverPtr->Update(aSimTime);
   }

   // Get the pilot object
   P6DofPilotObject* pilot = nullptr;
   if (mPilotManagerPtr != nullptr)
   {
      pilot = mPilotManagerPtr->GetActivePilot();
   }

   if (mGuidancePtr != nullptr)
   {
      double aBegTime = mLastUpdateTime;
      double aEndTime = aSimTime;

      // The second argument should be the time for by the current state of the weapon (aBegTime),
      // not the time to which we are trying to advance (aEndTime).
      mGuidancePtr->UpdateGuidance(aSimTime, aBegTime, aEndTime);

      // We need a pilot object
      if (pilot != nullptr)
      {
         // The guidance generates the following commands:
         // mYGLoad, mZGLoad and mCommandedYawRate, mCommandedPitchRate, and mCommandedRollRate
         // At the present time, only mYGLoad, mZGLoad are used.

         P6DofCommonController* autopilot = pilot->GetCommonController();
         P6DofAutopilotAction*  activity  = autopilot->GetCurrentActivity();

         double yawGLoad = mYGLoad;

         double pitchGLoad = mZGLoad;

         pitchGLoad *= -1.0;

         activity->SetVerticalChannelMode(P6DOF::Vertical::PitchGLoad);
         activity->SetPitchGLoad_g((float)pitchGLoad);
         autopilot->SetCurrentActivity(activity);

         activity->SetLateralChannelMode(P6DOF::Lateral::YawGLoad);
         activity->SetYawGLoad_g((float)yawGLoad);
         autopilot->SetCurrentActivity(activity);
      }
   }

   mP6DofVehiclePtr->Update(aSimTime);

   // Set the last update time
   WsfMover::mLastUpdateTime = aSimTime;

   // Update the platform state
   UpdatePlatformFromP6DofVehicle(mP6DofVehiclePtr, GetPlatform());

   // Check for staging events and create new objects as needed
   CheckForNewlyJettisonedPlatforms(aSimTime);

   // Send various data (tracking, targeting, etc) to P6DOF object
   P6DOF::TrackData trackData[10];
   WsfPlatform*     platform = GetPlatform();
   if (platform != nullptr)
   {
      WsfTrackManager& trackMgr = platform->GetTrackManager();
      // const WsfTrack*        trackPtr  = trackMgr.GetCurrentTarget();
      const WsfLocalTrackList& trackList = trackMgr.GetTrackList();
      platform->SetFuelMass(GetTotalFuelRemaining());
      platform->SetEmptyMass(GetEmptyWeight_kg());

      for (auto& i : trackData)
      {
         i.sensorName[0] = 0;
         i.targetName[0] = 0;
         i.targetLat     = 0;
         i.targetLon     = 0;
         i.targetAlt_m   = 0;
      }

      int          numTracks       = 0;
      unsigned int numTracksInList = trackList.GetTrackCount();

      for (unsigned int trkNum = 0; trkNum < numTracksInList; ++trkNum)
      {
         const WsfTrack* currentTrack = trackList.GetTrackEntry(trkNum);

         double tgtLat   = 0;
         double tgtLon   = 0;
         double tgtAlt_m = 0;
         currentTrack->GetLocationLLA(tgtLat, tgtLon, tgtAlt_m);

         // double az_deg  = UtMath::cDEG_PER_RAD * currentTrack->GetSensorAzimuth();
         // double el_deg  = UtMath::cDEG_PER_RAD * currentTrack->GetSensorElevation();
         double az_deg = UtMath::cDEG_PER_RAD * currentTrack->GetBearing();
         if (az_deg < 0.0)
         {
            az_deg += 360.0;
         }
         if (az_deg > 360.0)
         {
            az_deg -= 360.0;
         }
         // double el_deg  = UtMath::cDEG_PER_RAD * currentTrack->GetElevation();
         // double rng_nm  = UtMath::cNM_PER_M * currentTrack->GetRange();

         double lat, lon, alt_m = 0;
         currentTrack->GetLocationLLA(lat, lon, alt_m);

         WsfStringId strId = currentTrack->GetSensorNameId();
         std::string sensorStr;
         if (!strId.IsNull())
         {
            sensorStr = strId.GetString();
         }

         strId = currentTrack->GetTargetName();
         std::string targetStr;
         if (!strId.IsNull())
         {
            targetStr = strId.GetString();
         }

         trackData[numTracks].sensorName = sensorStr;
         trackData[numTracks].targetName = targetStr;

         trackData[numTracks].targetLat   = lat;
         trackData[numTracks].targetLon   = lon;
         trackData[numTracks].targetAlt_m = alt_m;

         ++numTracks;

         if (numTracks >= 10)
         {
            break;
         } // Quit once we have 10 tracks
      }

      mP6DofVehiclePtr->SetPrimaryTargetData(mCurrentTarget, numTracks, trackData);
   }

   UpdateAppearance(aSimTime);
}

void WsfP6DOF_Mover::UpdateAppearance(double aSimTime)
{
   mP6DofVehiclePtr->UpdateAppearance();

   // Set various "appearance" parameters

   WsfPlatform* platform = GetPlatform();

   // Trailing Effect -- This denotes contrailing or missile smoke trail
   // LSB = 7, 2 bits (none, small, medium, large)
   if (!platform->ScriptOverride(7, 2))
   {
      // Check for types of smoke. Priority as follows:
      // damage smoke, rocket smoke, contrail
      if (mP6DofVehiclePtr->DamageSmokeTrailingEffectPresent())
      {
         platform->SetAppearance(7, 2, 3); // 3 - Large
      }
      else if (mP6DofVehiclePtr->RocketSmokeTrailingEffectPresent())
      {
         platform->SetAppearance(7, 2, 2); // 2 - Medium
      }
      else if (mP6DofVehiclePtr->ContrailTrailingEffectPresent())
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
      if (mP6DofVehiclePtr->AnEngineIsSmoking())
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
      if (mDestroyed)
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
   if (mP6DofVehiclePtr->FlamesArePresent())
   {
      platform->SetAppearance(15, 1, 1);
   }
   else
   {
      platform->SetAppearance(15, 1, 0);
   }

   // Powerplant -- This denotes is engine is operating
   // LSB = 22, 1 bit (off, on)
   if (mP6DofVehiclePtr->AnEngineIsOperating())
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
      if (mP6DofVehiclePtr->GetTimeOfFlight_sec() <= mLaunchSmokeTime_sec)
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
      if (mP6DofVehiclePtr->AnEngineHasAfterburnerOn())
      {
         platform->SetAppearance(16, 1, 1);
      }
      else
      {
         platform->SetAppearance(16, 1, 0);
      }
   }

   if (mDamageFactor > 0.01)
   {
      // Some damage present

      // Check for heavy damage
      if (mDamageFactor > 0.5)
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

void WsfP6DOF_Mover::CheckForNewlyJettisonedPlatforms(double aSimTime)
{
   while (!mNewlyJettisonedPlatforms.empty())
   {
      GetSimulation()->AddPlatform(aSimTime, mNewlyJettisonedPlatforms.front());
      mNewlyJettisonedPlatforms.pop();
   }
}

WsfPlatform* WsfP6DOF_Mover::CommonSubobjectJettisonTasks(std::unique_ptr<P6DofVehicle> aJettisonedObject)
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

      std::string derivedFromObjectType = aJettisonedObject->GetDerivedFromObjectName();

      WsfP6DOF_TypeManager& typeManager = static_cast<WsfP6DOF_TypeManager&>(GetScenario().GetExtension("wsf_p6dof"));
      std::string           desiredPlatformName = typeManager.GetP6DofVehiclePlatformTypeMatch(derivedFromObjectType);

      if (desiredPlatformName.empty())
      {
         auto out = ut::log::error() << "Cannot find a platform match for P6DofVehicleType.";
         out.AddNote() << "Vehicle Type: " << derivedFromObjectType;
         return nullptr;
      }

      WsfP6DOF_Mover* newMover = nullptr;
      // Try to get a pointer to a platform of the desired type
      WsfPlatform* newWsfPlatform =
         dynamic_cast<WsfPlatform*>(GetScenario().CloneType("platform_type", desiredPlatformName));
      if (newWsfPlatform)
      {
         newWsfPlatform->SetName(aJettisonedObject->GetName());

         // Push data to the platform
         UpdatePlatformFromP6DofVehicle(aJettisonedObject.get(), newWsfPlatform);

         // Get Pointer to the mover
         WsfMover* mover = newWsfPlatform->GetMover();
         if (mover->IsA_TypeOf("WSF_P6DOF_MOVER"))
         {
            newMover = dynamic_cast<WsfP6DOF_Mover*>(mover);
            mCallbacks.Add(aJettisonedObject->SubobjectJettisoned.Connect(&WsfP6DOF_Mover::HandleSubobjectJettison, this));
         }
         else
         {
            auto out = ut::log::error() << "The jettisoned platform does not have a p6dof mover.";
            out.AddNote() << "Platform: " << desiredPlatformName;
            delete newWsfPlatform;
            return nullptr;
         }
      }
      else
      {
         // The desired object type does not exist
         auto out = ut::log::error() << "The required platform does not exist.";
         out.AddNote() << "Platform: " << desiredPlatformName;
         return nullptr;
      }

      if (newMover != nullptr) //(newWsfPlatform != 0 has to be true for newMover != 0 to be true
      {
         // Configure the new mover
         newMover->mLastUpdateTime = aJettisonedObject->GetLastUpdateTime_sec();
         newMover->SetP6DofVehicle(std::move(aJettisonedObject));

         double updateInterval = GetUpdateInterval();
         if (updateInterval <= 0.000000001)
         {
            updateInterval = 0.01;
         }
         newMover->SetUpdateInterval(updateInterval); // Typically 0.01 sec

         // Success, return the pointer
         return newWsfPlatform;
      }
      else
      {
         auto out = ut::log::error() << "Null platform in WsfP6DOF_Mover::CommonSubobjectJettisonTasks().";
         delete newWsfPlatform;
         return nullptr;
      }
   }

   return nullptr;
}

void WsfP6DOF_Mover::HandleSubobjectJettison(std::unique_ptr<P6DofVehicle>& aJettisonedObject)
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

P6DofRoute* WsfP6DOF_Mover::ConvertWsfRouteToP6DofRoute(const WsfRoute* aWsfRoute)
{
   double previous_alt = 0.0;

   P6DofRoute* p6dofRoute = new P6DofRoute();
   for (int i = 0; i < (int)aWsfRoute->GetSize(); ++i)
   {
      P6DofWaypoint* p6dofWaypoint = new P6DofWaypoint();

      const WsfWaypoint& waypoint = aWsfRoute->GetWaypointAt(i);

      if (waypoint.GetAlt() != WsfPath::cUSE_PREVIOUS)
      {
         p6dofWaypoint->SetLLA(UtLLAPos(waypoint.GetLat(), waypoint.GetLon(), waypoint.GetAlt()));
         previous_alt = waypoint.GetAlt();
      }
      else
      {
         p6dofWaypoint->SetLLA(UtLLAPos(waypoint.GetLat(), waypoint.GetLon(), previous_alt));
      }
      if (waypoint.GetSpeed() != WsfPath::cUSE_PREVIOUS)
      {
         p6dofWaypoint->SetSpeed(P6DofWaypoint::SPD_TYPE_TAS_KNOTS, waypoint.GetSpeed() * (float)UtMath::cNMPH_PER_MPS);
      }
      if (waypoint.GetRadialAccel() != WsfPath::cUSE_PREVIOUS)
      {
         p6dofWaypoint->SetMaxTurnG(P6DofWaypoint::TURN_G_TYPE_LATERAL,
                                    (waypoint.GetRadialAccel() / UtEarth::cACCEL_OF_GRAVITY));
      }
      if (waypoint.GetLabelId() != 0)
      {
         p6dofWaypoint->SetLabel(waypoint.GetLabelId());
      }
      if (waypoint.GetGotoId() != 0)
      {
         p6dofWaypoint->SetGoTo(waypoint.GetGotoId());
      }
      if (waypoint.GetSwitch() != WsfPath::cSWITCH_MOVER_DEFAULT)
      {
         p6dofWaypoint->SetWaypointOnPassing((waypoint.GetSwitch() == WsfPath::cSWITCH_ON_PASSING));
      }
      p6dofWaypoint->SetFollowVerticalTrack(mFollowVerticalTrack);
      p6dofWaypoint->SetId(i);

      p6dofRoute->AddWaypointToRouteEnd(p6dofWaypoint);
   }

   // Build up the segment map
   p6dofRoute->ComputeSegmentMap();

   return p6dofRoute;
}

void WsfP6DOF_Mover::UpdatePlatformFromP6DofVehicle(P6DofVehicle* aP6DofVehicle, UtEntity* aPlatform)
{
   if ((aP6DofVehicle == nullptr) || (aPlatform == nullptr))
   {
      return;
   }

   double wcsPosX_m(0.0);
   double wcsPosY_m(0.0);
   double wcsPosZ_m(0.0);
   aP6DofVehicle->GetLocationWCS(wcsPosX_m, wcsPosY_m, wcsPosZ_m);

   double wcsVelX_mps(0.0);
   double wcsVelY_mps(0.0);
   double wcsVelZ_mps(0.0);
   aP6DofVehicle->GetVelocityWCS(wcsVelX_mps, wcsVelY_mps, wcsVelZ_mps);

   double wcsYaw_rad(0.0);
   double wcsPitch_rad(0.0);
   double wcsRoll_rad(0.0);
   aP6DofVehicle->GetOrientationWCS(wcsYaw_rad, wcsPitch_rad, wcsRoll_rad);

   UtVec3dX accelNED = aP6DofVehicle->GetAccelNED_mps2();

   double wcsPos[3] = {wcsPosX_m, wcsPosY_m, wcsPosZ_m};
   double wcsVel[3] = {wcsVelX_mps, wcsVelY_mps, wcsVelZ_mps};
   aPlatform->SetLocationWCS(wcsPos);
   aPlatform->SetVelocityWCS(wcsVel);
   aPlatform->SetOrientationWCS(wcsYaw_rad, wcsPitch_rad, wcsRoll_rad);
   aPlatform->SetAccelerationNED(accelNED.GetData());
}

// TODO - Needs a struct for state data defined
void WsfP6DOF_Mover::GetStateData(double& aSimTime,
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
   aSimTime = mLastUpdateTime;

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

size_t WsfP6DOF_Mover::GetPlannedRouteIndex() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      return pilot->GetPlannedWaypointIndex();
   }
   return (size_t)0;
}

size_t WsfP6DOF_Mover::GetTempRouteIndex() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      return pilot->GetTempWaypointIndex();
   }
   return (size_t)0;
}

void WsfP6DOF_Mover::SetHeading(double aHeading)
{
   if (mP6DofVehiclePtr)
   {
      double heading = 0.0;
      double pitch   = 0.0;
      double roll    = 0.0;
      mP6DofVehiclePtr->GetAttitudeNED(heading, pitch, roll);
      mP6DofVehiclePtr->SetAttitudeNED(aHeading, pitch, roll);
   }
}

void WsfP6DOF_Mover::SetPitch(double aPitch)
{
   if (mP6DofVehiclePtr)
   {
      double heading = 0.0;
      double pitch   = 0.0;
      double roll    = 0.0;
      mP6DofVehiclePtr->GetAttitudeNED(heading, pitch, roll);
      mP6DofVehiclePtr->SetAttitudeNED(heading, aPitch, roll);
   }
}

void WsfP6DOF_Mover::SetRoll(double aRoll)
{
   if (mP6DofVehiclePtr)
   {
      double heading = 0.0;
      double pitch   = 0.0;
      double roll    = 0.0;
      mP6DofVehiclePtr->GetAttitudeNED(heading, pitch, roll);
      mP6DofVehiclePtr->SetAttitudeNED(heading, pitch, aRoll);
   }
}

void WsfP6DOF_Mover::SetOrientationNED(double aHeading, double aPitch, double aRoll)
{
   if (mP6DofVehiclePtr)
   {
      mP6DofVehiclePtr->SetAttitudeNED(aHeading, aPitch, aRoll);
   }
}

void WsfP6DOF_Mover::SetP6DofVehicle(std::unique_ptr<P6DofVehicle> aP6DofVehicle)
{
   mP6DofVehiclePtr = std::move(aP6DofVehicle);
   if (mP6DofVehiclePtr != nullptr)
   {
      mPilotManagerPtr = mP6DofVehiclePtr->GetPilotManager();
   }
}

std::string WsfP6DOF_Mover::GetP6DofVehicleTypeName() const
{
   return mP6DofVehicleTypePtr->GetName();
}

std::string WsfP6DOF_Mover::GetP6DofVehicleTypeDerivedFromName() const
{
   return mP6DofVehicleTypePtr->GetDerivedFromName();
}

std::string WsfP6DOF_Mover::GetP6DofVehicleTypeSubobjectBaseName() const
{
   return mP6DofVehicleTypePtr->GetSubobjectBaseName();
}

double WsfP6DOF_Mover::GetVerticalSpeed()
{
   return mP6DofVehiclePtr->GetVertSpeed_fpm() * UtMath::cM_PER_FT / 60.0;
}

double WsfP6DOF_Mover::GetYawRate()
{
   return mP6DofVehiclePtr->GetYawRate_dps() * UtMath::cRAD_PER_DEG;
}

double WsfP6DOF_Mover::GetPitchRate()
{
   return mP6DofVehiclePtr->GetPitchRate_dps() * UtMath::cRAD_PER_DEG;
}

double WsfP6DOF_Mover::GetRollRate()
{
   return mP6DofVehiclePtr->GetRollRate_dps() * UtMath::cRAD_PER_DEG;
}

const double WsfP6DOF_Mover::GetFlightPathAngle_rad()
{
   return mP6DofVehiclePtr->GetFlightPathAngle_rad();
}

double WsfP6DOF_Mover::GetFlightPathAngle()
{
   return mP6DofVehiclePtr->GetFlightPathAngle_rad() * UtMath::cDEG_PER_RAD;
}

double WsfP6DOF_Mover::GetGLoad()
{
   return -mP6DofVehiclePtr->GetNz_g(); // G-load is negative Nz
}

void WsfP6DOF_Mover::TakeDirectControlInput()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         pilot->TakeExternalDirectControl();
      }
   }
}

void WsfP6DOF_Mover::EnableAutopilot(bool aState)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         pilot->EnableAutopilot(aState);
      }
   }
}

void WsfP6DOF_Mover::ReleaseDirectControlInput()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         pilot->ReleaseExternalDirectControl();
      }
   }
}

void WsfP6DOF_Mover::SetDirectControlInputs(double aStickX, double aStickY, double aRudder, double aThrottleLeverPosition)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
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
}

bool WsfP6DOF_Mover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot GoToLocation().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      UtLLAPos newPt(aLatitude, aLongitude, aAltitude);

      pilot->FlyAtPoint(newPt);

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::TurnToHeading(double                 aSimTime,
                                   double                 aHeading,     // radians
                                   double                 aRadialAccel, // meters/second^2
                                   WsfPath::TurnDirection aTurnDirection)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot TurnToHeading().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      P6DofCommonController::eAutopilotTurnDir direction = P6DofCommonController::TD_DEFAULT;
      if (aTurnDirection == WsfPath::cTURN_DIR_LEFT)
      {
         direction = P6DofCommonController::TD_LEFT;
      }
      else if (aTurnDirection == WsfPath::cTURN_DIR_RIGHT)
      {
         direction = P6DofCommonController::TD_RIGHT;
      }

      pilot->TurnToHeading(aHeading, aRadialAccel, direction);

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::GoToSpeed(double aSimTime,
                               double aSpeed, // meters/sec
                               double aLinearAccel,
                               bool) // aKeepRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot GoToSpeed().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      pilot->GoToSpeed(aSpeed * UtMath::cNMPH_PER_MPS, aLinearAccel);

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::GoToAltitude(double aSimTime,
                                  double aAltitude, // meters
                                  double aClimbDiveRate,
                                  bool) // aKeepRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot GoToAltitude().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      pilot->GoToAltitude(aAltitude, aClimbDiveRate);

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::TurnToRelativeHeading(double                 aSimTime,
                                           double                 aHeadingChange, // radians
                                           double                 aRadialAccel,   // meters/sec^2
                                           WsfPath::TurnDirection aTurnDirection)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot TurnToRelativeHeading().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      P6DofCommonController::eAutopilotTurnDir direction = P6DofCommonController::TD_DEFAULT;
      if (aTurnDirection == WsfPath::cTURN_DIR_LEFT)
      {
         direction = P6DofCommonController::TD_LEFT;
      }
      else if (aTurnDirection == WsfPath::cTURN_DIR_RIGHT)
      {
         direction = P6DofCommonController::TD_RIGHT;
      }

      double yawRad, pitRad, rolRad;
      GetPlatform()->GetOrientationNED(yawRad, pitRad, rolRad);

      pilot->TurnToHeading(yawRad + aHeadingChange, aRadialAccel, direction);

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::ReturnToRoute(double aSimTime)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot ReturnToRoute().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
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

   return false;
}

bool WsfP6DOF_Mover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot UpdateRoute().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      if (mWsfRoutePtr != &aRoute)
      {
         delete mWsfRoutePtr;
         mWsfRoutePtr = aRoute.Clone();
      }

      pilot->FlyTempWaypoints(ConvertWsfRouteToP6DofRoute(mWsfRoutePtr));

      WsfObserver::RouteChanged(GetSimulation())(this);
      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::GoToWaypoint(double, // aSimTime,
                                  unsigned int aDestination)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         // We need (aDestination + 1) because P6DOF routes prepend the location of the platform to the route,
         // so it has one more waypoint than a standard WsfRoute.
         if (pilot->IsOnPlannedRoute())
         {
            return pilot->SetPlannedWaypointIndex((int)aDestination + 1);
         }
         else
         {
            return pilot->SetTempWaypointIndex((int)aDestination + 1);
         }
      }
   }

   return false;
}

bool WsfP6DOF_Mover::SetDefaultRoute(const WsfRoute& aRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot SetDefaultRoute().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }
      if (mDefaultWsfRoutePtr != &aRoute)
      {
         delete mDefaultWsfRoutePtr;
         mDefaultWsfRoutePtr = aRoute.Clone();
      }
      pilot->SetPlannedRoute(ConvertWsfRouteToP6DofRoute(mDefaultWsfRoutePtr));

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::SetTempRoute(const WsfRoute& aRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::warning() << "Cannot SetTempRoute().";
            out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         return false;
      }

      if (mWsfRoutePtr != &aRoute)
      {
         delete mWsfRoutePtr;
         mWsfRoutePtr = aRoute.Clone();
      }
      pilot->SetTempRoute(ConvertWsfRouteToP6DofRoute(mWsfRoutePtr));

      return true;
   }

   return false;
}

bool WsfP6DOF_Mover::SetRoute(double, // aSimTime,
                              const WsfRoute& aRoute)
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
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
         out.AddNote() << "P6DofPilotObject from the P6DofVehicleType is not available.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         return false;
      }
   }

   return false;
}

int WsfP6DOF_Mover::RoutePointIndex() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         // We need the (...Index() - 1) because P6DOF routes prepend the location of the platform to the route,
         // so it has one more waypoint than a standard WsfRoute.
         if (pilot->IsOnPlannedRoute())
         {
            size_t plannedIndex = pilot->GetPlannedWaypointIndex();
            return (plannedIndex == 0) ? 0 : static_cast<int>(plannedIndex - 1);
         }
         size_t tempIndex = pilot->GetTempWaypointIndex();
         return (tempIndex == 0) ? 0 : static_cast<int>(tempIndex - 1);
      }
   }
   return 0;
}

void WsfP6DOF_Mover::SetYandZGLoads(double aYGLoad, double aZGLoad)
{
   mYGLoad = aYGLoad;
   mZGLoad = aZGLoad;
}

//! A helper function to write the kinematic status of the mover to stdout. This function is invoked by the mover
//! and the guidance computer to write out a common status message at important events. It is expected that the
//! caller has already written out the introductory line that indicates the event that occurred.
void WsfP6DOF_Mover::WriteKinematicStatus(ut::log::MessageStream& aStream)
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

   double           speed = GetPlatform()->GetSpeed();
   double           hdg;
   double           pitch;
   double           roll;
   double           speedOfSound_fps = 0;
   P6DofAtmosphere* atm              = mP6DofVehiclePtr->GetScenario()->GetAtmosphere();
   if (atm == nullptr)
   {
      // Use a default value of 1,000 ft/sec
      speedOfSound_fps = 1000.0 * UtMath::cFT_PER_M;
   }
   else
   {
      speedOfSound_fps = atm->SpeedOfSoundAtAltitude_fps(alt * UtMath::cFT_PER_M);
   }

   GetPlatform()->GetOrientationNED(hdg, pitch, roll);

   aStream << std::fixed;
   aStream.AddNote() << "Lat: " << lat;
   aStream.AddNote() << "Lon: " << lon;
   aStream.AddNote() << "Alt: " << std::setprecision(0) << alt << " m";
   aStream.AddNote() << "Downrange: " << std::setprecision(0) << downRange << " m";
   aStream.AddNote() << "Mass: " << std::setprecision(2) << GetPlatform()->GetMass() << " kg";
   aStream.AddNote() << "Speed: " << std::setprecision(0) << speed << " m/s";
   aStream.AddNote() << "Mach: " << std::setprecision(2) << speed * UtMath::cFT_PER_M / speedOfSound_fps;
   aStream.AddNote() << "Acceleration ECS: " << std::setprecision(2);
   aStream.AddNote() << aclECS[0] / UtEarth::cACCEL_OF_GRAVITY << " g, ";
   aStream.AddNote() << aclECS[1] / UtEarth::cACCEL_OF_GRAVITY << " g, ";
   aStream.AddNote() << aclECS[2] / UtEarth::cACCEL_OF_GRAVITY << " g";
   aStream.AddNote() << "Heading: " << std::setprecision(4) << hdg * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Pitch: " << std::setprecision(4) << pitch * UtMath::cDEG_PER_RAD << " deg";
   aStream.AddNote() << "Roll: " << std::setprecision(4) << roll * UtMath::cDEG_PER_RAD << " deg";
}

void WsfP6DOF_Mover::ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofLandingGear* gear = mP6DofVehiclePtr->GetLandingGear();
      if (gear == nullptr)
      {
         auto out = ut::log::warning() << "Cannot apply force to P6Dof mover. No landing gear defined.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      else
      {
         gear->ApplyExternalForce(aForceMagnitude_lbs, aAngle_deg * UtMath::cRAD_PER_DEG);
      }
   }
}

void WsfP6DOF_Mover::RemoveExternalForce()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofLandingGear* gear = mP6DofVehiclePtr->GetLandingGear();
      if (gear == nullptr)
      {
         auto out = ut::log::warning() << "Cannot remove force from P6Dof mover. No landing gear defined.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      else
      {
         gear->RemoveExternalForce();
      }
   }
}

void WsfP6DOF_Mover::EnableDirectBraking()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->EnableDirectBraking();
      }
      else
      {
         auto out = ut::log::warning() << "No pilot in EnableDirectBraking.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }
   else
   {
      auto out = ut::log::warning() << "No vehicle in EnableDirectBraking.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
}

void WsfP6DOF_Mover::ReleaseDirectBraking()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->ReleaseDirectBraking();
      }
   }
}

void WsfP6DOF_Mover::SetParkingBrake()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->SetParkingBrake();
      }
      else
      {
         auto out = ut::log::warning() << "No pilot in SetParkingBrake.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }
}

void WsfP6DOF_Mover::ApplyLeftGearBrake(double aValue)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->ApplyLeftGearBrake(aValue);
      }
   }
}

void WsfP6DOF_Mover::ApplyRightGearBrake(double aValue)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->ApplyRightGearBrake(aValue);
      }
   }
}

void WsfP6DOF_Mover::ReleaseWheelBrakes()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();
      if (pilot != nullptr)
      {
         mP6DofVehiclePtr->GetPilotObject()->ReleaseParkingBrake();
      }
   }
}

void WsfP6DOF_Mover::AddFuel(double aFuelMass_kg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->AddFuelQuantity_lbs(aFuelMass_kg * UtMath::cLB_PER_KG);
   }
}

double WsfP6DOF_Mover::GetTotalFuelCapacity() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetTotalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetInternalFuelCapacity() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetInternalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetExternalFuelCapacity() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetExternalFuelTankCapacity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetTotalFuelRemaining() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentTotalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetInternalFuelRemaining() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentInternalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetExternalFuelRemaining() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentExternalFuelTankQuantity_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetFuelTankCapacity(const std::string& aTankName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      const P6DofFuelTank* fueltank = mP6DofVehiclePtr->GetFuelTankByName(aTankName);
      if (fueltank != nullptr)
      {
         return fueltank->GetFuelCapacity_lbs() * UtMath::cKG_PER_LB;
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetFuelInTank(const std::string& aTankName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      const P6DofFuelTank* fueltank = mP6DofVehiclePtr->GetFuelTankByName(aTankName);
      if (fueltank != nullptr)
      {
         return fueltank->GetCurrentFuelQuantity_lbs() * UtMath::cKG_PER_LB;
      }
   }
   return 0.0;
}

void WsfP6DOF_Mover::SetFuelInTank(const std::string& aTankName, double aFuel_kg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->SetFuelInTank_lbs(aTankName, aFuel_kg * UtMath::cLB_PER_KG);
   }
}

double WsfP6DOF_Mover::GetTotalFuelFlow() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      double fuelFlow_pph = mP6DofVehiclePtr->GetTotalVehicleFuelFlow_pph();
      return fuelFlow_pph * UtMath::cKG_PER_LB / 3600.0; // Convert from lbs/hr to kg/sec
   }
   return 0.0;
}

void WsfP6DOF_Mover::EmptyAllInternalandExternalTanks()
{
   EmptyAllInternalTanks();
   EmptyAllExternalTanks();
}

void WsfP6DOF_Mover::EmptyAllInternalTanks()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->EmptyAllInternalTanks();
   }
}

void WsfP6DOF_Mover::EmptyAllExternalTanks()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->EmptyAllExternalTanks();
   }
}

void WsfP6DOF_Mover::SetJokerFuelState(double aJokerFuel_kg)
{
   mJokerFuel_lbs = aJokerFuel_kg * UtMath::cLB_PER_KG;
}

void WsfP6DOF_Mover::SetBingoFuelState(double aBingoFuel_kg)
{
   mBingoFuel_lbs = aBingoFuel_kg * UtMath::cLB_PER_KG;
}

double WsfP6DOF_Mover::GetJokerFuelState()
{
   return mJokerFuel_lbs * UtMath::cKG_PER_LB;
}

double WsfP6DOF_Mover::GetBingoFuelState()
{
   return mBingoFuel_lbs * UtMath::cKG_PER_LB;
}

bool WsfP6DOF_Mover::JokerFuelReached()
{
   if (mP6DofVehiclePtr->GetCurrentTotalFuelTankQuantity_lbs() < mJokerFuel_lbs)
   {
      return true;
   }
   return false;
}

bool WsfP6DOF_Mover::BingoFuelReached()
{
   if (mP6DofVehiclePtr->GetCurrentTotalFuelTankQuantity_lbs() < mBingoFuel_lbs)
   {
      return true;
   }
   return false;
}

void WsfP6DOF_Mover::EnableControls(bool aEnabled)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->EnableControls(aEnabled);
   }
}

bool WsfP6DOF_Mover::ControlsEnabled() const
{
   if (mP6DofVehiclePtr == nullptr)
   {
      return false;
   }
   return mP6DofVehiclePtr->ControlsEnabled();
}

void WsfP6DOF_Mover::StartupEngines(double aTimeSec)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->IgniteObject(0);
   }
}

void WsfP6DOF_Mover::ShutdownEngines(double aTimeSec)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->ShutdownObject(0);
   }
}

double WsfP6DOF_Mover::GetTotalWeight_lbs() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentWeight_lbs();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetTotalWeight_kg() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentWeight_lbs() * UtMath::cKG_PER_LB;
   }
   return 0.0;
}

void WsfP6DOF_Mover::RetractLandingGear()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      // The landing gear will now be down, but we need to be sure that there will not
      // be a command at the start to retract the gear.
      pilot->SetLandingGearControlPosition(0.0);
   }
}

void WsfP6DOF_Mover::LowerLandingGear()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      // The landing gear will now be down, but we need to be sure that there will not
      // be a command at the start to retract the gear.
      pilot->SetLandingGearControlPosition(1.0);
   }
}

void WsfP6DOF_Mover::SetFlapsPosition(double aPosition)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      pilot->SetFlapsControlPosition(aPosition);
   }
}

void WsfP6DOF_Mover::SetSpoilersPosition(double aPosition)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      pilot->SetSpoilersControlPosition(aPosition);
   }
}

void WsfP6DOF_Mover::SetThrustReverserPosition(double aPosition)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      pilot->SetThrustReverserControlPosition(aPosition);
   }
}

void WsfP6DOF_Mover::SetEnableNWS(bool aNWS_Enabled)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* pilot = mP6DofVehiclePtr->GetPilotObject();

      // If we lack a pilot controller, simply return
      if (pilot == nullptr)
      {
         return;
      }

      pilot->SetEnableNWS(aNWS_Enabled);
   }
}

bool WsfP6DOF_Mover::GetWeightOnWheels() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofLandingGear* gear = mP6DofVehiclePtr->GetLandingGear();
      if (gear != nullptr)
      {
         return gear->WeightOnWheels();
      }
   }
   return false;
}

bool WsfP6DOF_Mover::GetWeightOnNoseWheel() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofLandingGear* gear = mP6DofVehiclePtr->GetLandingGear();
      if (gear == nullptr)
      {
         auto out =
            ut::log::warning() << "Cannot process weight on nose wheel in P6Dof Mover. No landing gear defined.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      else
      {
         return gear->WeightOnNoseWheel();
      }
   }
   return false;
}

bool WsfP6DOF_Mover::IsProducingThrust() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->IsProducingThrust();
   }
   return false;
}

double WsfP6DOF_Mover::GetTotalThrust() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentThrust_lbs() * UtMath::cNT_PER_LB;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetTotalForwardThrust() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetCurrentForwardThrust_lbs() * UtMath::cNT_PER_LB;
   }
   return 0.0;
}

bool WsfP6DOF_Mover::GetAfterburnerOn() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->AnEngineHasAfterburnerOn();
   }
   return false;
}

double WsfP6DOF_Mover::GetTotalFuelFlowRate() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetTotalVehicleFuelFlow_pph() * UtMath::cKG_PER_LB / 3600.0; // Convert to kg/sec
   }
   return 0.0;
}

bool WsfP6DOF_Mover::AddFuelTransfer(std::string aFuelTransferName, std::string aSourceTankName, std::string aTargetTankName)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->AddFuelTransfer(aFuelTransferName, aSourceTankName, aTargetTankName);
   }
   return false;
}

bool WsfP6DOF_Mover::RemoveFuelTransfer(std::string aFuelTransferName)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->RemoveFuelTransfer(aFuelTransferName);
   }
   return false;
}

bool WsfP6DOF_Mover::SetFuelFeed(std::string aEngineName, std::string aFuelTankName)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->SetFuelFeed(aEngineName, aFuelTankName);
   }
   return false;
}

bool WsfP6DOF_Mover::SetFuelFeed(std::string aFuelTankName)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->SetFuelFeed(aFuelTankName);
   }
   return false;
}

double WsfP6DOF_Mover::GetEngineThrust(const std::string& aEngineName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      const P6DofThrustProducerObject* engine = mP6DofVehiclePtr->GetThrustProducerObjectByName(aEngineName);
      if (engine != nullptr)
      {
         return engine->GetForwardThrust_lbs() * UtMath::cNT_PER_LB;
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetEngineFuelFlowRate(const std::string& aEngineName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      const P6DofThrustProducerObject* engine = mP6DofVehiclePtr->GetThrustProducerObjectByName(aEngineName);
      if (engine != nullptr)
      {
         return engine->GetFuelBurnRate_pph() * UtMath::cKG_PER_LB / 3600.0; // Convert to kg/sec
      }
   }
   return 0.0;
}

bool WsfP6DOF_Mover::GetEngineAfterburnerOn(const std::string& aEngineName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      const P6DofThrustProducerObject* engine = mP6DofVehiclePtr->GetThrustProducerObjectByName(aEngineName);
      if (engine != nullptr)
      {
         return engine->AfterburnerOn();
      }
   }
   return false;
}

void WsfP6DOF_Mover::EnableThrustVectoring(bool aEnableThrustVectoring)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->EnableThrustVectoring(aEnableThrustVectoring);
   }
}

void WsfP6DOF_Mover::RevertLimitsToDefaults()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->RevertLimitsAndSettingsToDefaults();
         }
      }
   }
}

double WsfP6DOF_Mover::GetPitchGLoadMin()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.pitchGLoad_Min);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetPitchGLoadMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.pitchGLoad_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAlphaMin()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.alpha_Min);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAlphaMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.alpha_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetPitchRateMin()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.pitchRate_Min);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetPitchRateMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.pitchRate_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetVerticalSpeedMin()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.vertSpd_Min);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetVerticalSpeedMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.vertSpd_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetYawGLoadMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.yawGLoad_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetBetaMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.beta_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetYawRateMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.yawRate_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetRollRateMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.rollRate_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetBankAngleMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.bankAngle_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetForwardAccelMin()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.forwardAccel_Min);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetForwardAccelMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.forwardAccel_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetTaxiSpeedMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.taxiSpeed_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetTaxiYawRateMax()
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            const P6DOF::AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
            return static_cast<double>(apControls.taxiYawRate_Max);
         }
      }
      return 0.0;
   }
   return 0.0;
}

void WsfP6DOF_Mover::ActivateManualPilot()
{
   P6DofPilotManager* pilotMgr = mP6DofVehiclePtr->GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeManualPilotActive();
   }
}

void WsfP6DOF_Mover::ActivateSyntheticPilot()
{
   P6DofPilotManager* pilotMgr = mP6DofVehiclePtr->GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeSyntheticPilotActive();
   }
}

void WsfP6DOF_Mover::ActivateHardwareAutopilot()
{
   P6DofPilotManager* pilotMgr = mP6DofVehiclePtr->GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeHardwareAutopilotActive();
   }
}

void WsfP6DOF_Mover::ActivateGuidanceAutopilot()
{
   P6DofPilotManager* pilotMgr = mP6DofVehiclePtr->GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->MakeGuidanceAutopilotActive();
   }
}

void WsfP6DOF_Mover::SetPitchGLoadMin(double aGees)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMinPitchGLimit(aGees);
         }
      }
   }
}

void WsfP6DOF_Mover::SetPitchGLoadMax(double aGees)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxPitchGLimit(aGees);
         }
      }
   }
}

void WsfP6DOF_Mover::SetAlphaMin(double aAlpha)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMinAlphaLimit(aAlpha);
         }
      }
   }
}

void WsfP6DOF_Mover::SetAlphaMax(double aAlpha)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxAlphaLimit(aAlpha);
         }
      }
   }
}

void WsfP6DOF_Mover::SetPitchRateMin(double aPitchRate)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMinPitchRateLimit(aPitchRate);
         }
      }
   }
}

void WsfP6DOF_Mover::SetPitchRateMax(double aPitchRate)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxPitchRateLimit(aPitchRate);
         }
      }
   }
}

void WsfP6DOF_Mover::SetVerticalSpeedMin(double aVertSpeed)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMinVertSpeedLimit(aVertSpeed);
         }
      }
   }
}

void WsfP6DOF_Mover::SetVerticalSpeedMax(double aVertSpeed)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxVertSpeedLimit(aVertSpeed);
         }
      }
   }
}

void WsfP6DOF_Mover::SetYawGLoadMax(double aYawGLoad)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxYawGLimit(aYawGLoad);
         }
      }
   }
}

void WsfP6DOF_Mover::SetBetaMax(double aBeta)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxBetaLimit(aBeta);
         }
      }
   }
}

void WsfP6DOF_Mover::SetYawRateMax(double aYawRate)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxYawRateLimit(aYawRate);
         }
      }
   }
}

void WsfP6DOF_Mover::SetRollRateMax(double aRollRate)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxRollRateLimit(aRollRate);
         }
      }
   }
}

void WsfP6DOF_Mover::SetBankAngleMax(double aBankAngle)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxBankAngleLimit(aBankAngle);
         }
      }
   }
}

double WsfP6DOF_Mover::GetBankAngleMax() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            return autopilot->GetCurrentLimitsAndSettings().bankAngle_Max;
         }
      }
   }
   return -1.0;
}

void WsfP6DOF_Mover::SetForwardAccelMin(double aForwardAccel)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMinForwardAccelLimit(aForwardAccel);
         }
      }
   }
}

void WsfP6DOF_Mover::SetForwardAccelMax(double aForwardAccel)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxForwardAccelLimit(aForwardAccel);
         }
      }
   }
}

void WsfP6DOF_Mover::SetTaxiSpeedMax(double aSpeedMax)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxTaxiSpeedLimit(aSpeedMax);
         }
      }
   }
}

void WsfP6DOF_Mover::SetTaxiYawRateMax(double aSpeedMax)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentMaxTaxiYawRateLimit(aSpeedMax);
         }
      }
   }
}

void WsfP6DOF_Mover::SetTaxiMode(bool aTaxiModeEnabled)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetTaxiMode(aTaxiModeEnabled);
         }
      }
   }
}

void WsfP6DOF_Mover::SetTaxiRadius(double aTaxiRadius_ft)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetDesiredTaxiRadius(aTaxiRadius_ft);
         }
      }
   }
}

void WsfP6DOF_Mover::SetTurnRollInMultiplier(double aValue)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetCurrentTurnRollInMultiplier(aValue);
         }
      }
   }
}

double WsfP6DOF_Mover::GetCurrentTurnRollInMultiplier()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            return autopilot->GetCurrentTurnRollInMultiplier();
         }
      }
   }
   return 1.0;
}

double WsfP6DOF_Mover::GetDefaultTurnRollInMultiplier()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            return autopilot->GetDefaultTurnRollInMultiplier();
         }
      }
   }
   return 1.0;
}

void WsfP6DOF_Mover::SetRouteAllowableAngleError(double aValue_rad)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            autopilot->SetRouteAllowableAngleError_rad(aValue_rad);
         }
      }
   }
}

double WsfP6DOF_Mover::GetCurrentRouteAllowableAngleError()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            return autopilot->GetCurrentRouteAllowableAngleError_rad();
         }
      }
   }
   return 1.0;
}

double WsfP6DOF_Mover::GetDefaultRouteAllowableAngleError()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         P6DofCommonController* autopilot = controller->GetCommonController();
         if (autopilot != nullptr)
         {
            return autopilot->GetDefaultRouteAllowableAngleError_rad();
         }
      }
   }
   return 1.0;
}

void WsfP6DOF_Mover::SetAutopilotPitchAngle(double aPitchAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotPitchAngle(aPitchAngle_deg);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotFlightPathAngle(double aFlightPathAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotFlightPathAngle(aFlightPathAngle_deg);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotVerticalSpeed(aVerticalSpeed_fpm);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotAltitude(double aAltitude_ft)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotAltitude(aAltitude_ft);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotRollAngle(double aRollAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotRollAngle(aRollAngle_deg);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotRollRate(double aRollRate_dps)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotRollRate(aRollRate_dps);
      }
   }
}

void WsfP6DOF_Mover::SetPitchGLoad(double aGLoad)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetPitchGLoad(aGLoad);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotPitchRate(double aPitchRate_dps)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotPitchRate(aPitchRate_dps);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotDeltaPitch(double DeltaPitchAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotDeltaPitch(DeltaPitchAngle_deg);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotDeltaRoll(double DeltaRollAngle_deg)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotDeltaRoll(DeltaRollAngle_deg);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotSpeedKTAS(double aSpeed_KTAS)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotSpeedKTAS(aSpeed_KTAS);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotSpeedKCAS(double aSpeed_KCAS)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotSpeedKCAS(aSpeed_KCAS);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotSpeedMach(double aSpeed_Mach)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotSpeedMach(aSpeed_Mach);
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotLateralWaypointMode()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotLateralWaypointMode();
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotVerticalWaypointMode()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotVerticalWaypointMode();
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotSpeedWaypointMode()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotSpeedWaypointMode();
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotWaypointMode()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotWaypointMode();
      }
   }
}

void WsfP6DOF_Mover::SetAutopilotNoControl()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->SetAutopilotNoControl();
      }
   }
}

void WsfP6DOF_Mover::EnableDirectThrottleInput()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->EnableDirectThrottleInput();
      }
   }
}

void WsfP6DOF_Mover::ReleaseDirectThrottleInput()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->ReleaseDirectThrottleInput();
      }
   }
}

void WsfP6DOF_Mover::MoveThrottleToIdle()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->MoveThrottleToIdle();
      }
   }
}

void WsfP6DOF_Mover::MoveThrottleToFull()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->MoveThrottleToFull();
      }
   }
}

void WsfP6DOF_Mover::MoveThrottleToAfterburner()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->MoveThrottleToAfterburner();
      }
   }
}

void WsfP6DOF_Mover::EnableDirectSpeedBrakeInput()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->EnableDirectSpeedBrakeInput();
      }
   }
}

void WsfP6DOF_Mover::ReleaseDirectSpeedBrakeInput()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->ReleaseDirectSpeedBrakeInput();
      }
   }
}

double WsfP6DOF_Mover::GetStickBackPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetStickBackControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetStickRightPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetStickRightControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetRudderRightPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetRudderRightControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetThrottleMilitaryPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetThrottleMilitaryControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetThrottleAfterburnerPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetThrottleAfterburnerControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetThrustReverserControlPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetThrustReverserControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetSpeedBrakeControlPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetSpeedBrakesControllerPosition();
      }
   }
   return 0.0;
}

void WsfP6DOF_Mover::OpenSpeedBrake()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->OpenSpeedBrake();
      }
   }
}

void WsfP6DOF_Mover::CloseSpeedBrake()
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         controller->CloseSpeedBrake();
      }
   }
}

bool WsfP6DOF_Mover::ActivateSequencer(const std::string& aSequencerName)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->ActivateSequencer(aSequencerName);
   }
   return false;
}

double WsfP6DOF_Mover::GetMinimumPotentialThrust_lbs() const
{
   return mP6DofVehiclePtr->GetMinimumPotentialThrust_lbs();
}

double WsfP6DOF_Mover::GetMaximumPotentialThrust_lbs() const
{
   return mP6DofVehiclePtr->GetMaximumPotentialThrust_lbs();
}

double WsfP6DOF_Mover::GetAlpha_rad() const
{
   return mP6DofVehiclePtr->GetAlpha_rad();
}

double WsfP6DOF_Mover::GetAlpha_deg() const
{
   return mP6DofVehiclePtr->GetAlpha_deg();
}

double WsfP6DOF_Mover::GetBeta_rad() const
{
   return mP6DofVehiclePtr->GetBeta_rad();
}

double WsfP6DOF_Mover::GetBeta_deg() const
{
   return mP6DofVehiclePtr->GetBeta_deg();
}

double WsfP6DOF_Mover::GetAlphaDot_rps() const
{
   return mP6DofVehiclePtr->GetAlphaDot_rps();
}

double WsfP6DOF_Mover::GetAlphaDot_dps() const
{
   return mP6DofVehiclePtr->GetAlphaDot_dps();
}

double WsfP6DOF_Mover::GetBetaDot_rps() const
{
   return mP6DofVehiclePtr->GetBetaDot_rps();
}

double WsfP6DOF_Mover::GetBetaDot_dps() const
{
   return mP6DofVehiclePtr->GetBetaDot_dps();
}

UtVec3dX WsfP6DOF_Mover::GetCg() const
{
   return mP6DofVehiclePtr->GetCg();
}

double WsfP6DOF_Mover::GetHeading_rad() const
{
   return mP6DofVehiclePtr->GetHeading_rad();
}

double WsfP6DOF_Mover::GetHeading() const
{
   return mP6DofVehiclePtr->GetHeading_deg();
}

double WsfP6DOF_Mover::GetPitch_rad() const
{
   return mP6DofVehiclePtr->GetPitch_rad();
}

double WsfP6DOF_Mover::GetPitch() const
{
   return mP6DofVehiclePtr->GetPitch_deg();
}

double WsfP6DOF_Mover::GetRoll_rad() const
{
   return mP6DofVehiclePtr->GetRoll_rad();
}

double WsfP6DOF_Mover::GetRoll() const
{
   return mP6DofVehiclePtr->GetRoll_deg();
}

double WsfP6DOF_Mover::GetLift_nt() const
{
   return mP6DofVehiclePtr->GetLift_lbs() * UtMath::cNT_PER_LB;
}

double WsfP6DOF_Mover::GetLift_lbs() const
{
   return mP6DofVehiclePtr->GetLift_lbs();
}

double WsfP6DOF_Mover::GetDrag_nt() const
{
   return mP6DofVehiclePtr->GetDrag_lbs() * UtMath::cNT_PER_LB;
}

double WsfP6DOF_Mover::GetDrag_lbs() const
{
   return mP6DofVehiclePtr->GetDrag_lbs();
}

double WsfP6DOF_Mover::GetSideForce_nt() const
{
   return mP6DofVehiclePtr->GetSideForce_lbs() * UtMath::cNT_PER_LB;
}

double WsfP6DOF_Mover::GetSideForce_lbs() const
{
   return mP6DofVehiclePtr->GetSideForce_lbs();
}

double WsfP6DOF_Mover::GetThrustMagnitude_nt() const
{
   return mP6DofVehiclePtr->GetThrustMagnitude_lbs() * UtMath::cNT_PER_LB;
}

double WsfP6DOF_Mover::GetThrustMagnitude_lbs() const
{
   return mP6DofVehiclePtr->GetThrustMagnitude_lbs();
}

double WsfP6DOF_Mover::GetYawMoment_nm() const
{
   return mP6DofVehiclePtr->GetYawMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

double WsfP6DOF_Mover::GetYawMoment_ftlbs() const
{
   return mP6DofVehiclePtr->GetYawMoment_ftlbs();
}

double WsfP6DOF_Mover::GetPitchMoment_nm() const
{
   return mP6DofVehiclePtr->GetPitchMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

double WsfP6DOF_Mover::GetPitchMoment_ftlbs() const
{
   return mP6DofVehiclePtr->GetPitchMoment_ftlbs();
}

double WsfP6DOF_Mover::GetRollMoment_nm() const
{
   return mP6DofVehiclePtr->GetRollMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

double WsfP6DOF_Mover::GetRollMoment_ftlbs() const
{
   return mP6DofVehiclePtr->GetRollMoment_ftlbs();
}

double WsfP6DOF_Mover::GetSpeed_fps() const
{
   return mP6DofVehiclePtr->GetSpeed_fps();
}

double WsfP6DOF_Mover::GetSpeed_KCAS() const
{
   return mP6DofVehiclePtr->GetKCAS();
}

double WsfP6DOF_Mover::GetSpeed_KIAS() const
{
   return mP6DofVehiclePtr->GetKIAS();
}

double WsfP6DOF_Mover::GetSpeed_KTAS() const
{
   return mP6DofVehiclePtr->GetKTAS();
}

double WsfP6DOF_Mover::GetSpeed_Mach() const
{
   return mP6DofVehiclePtr->GetMach();
}

double WsfP6DOF_Mover::GetCurrentWeight_lbs() const
{
   return mP6DofVehiclePtr->GetCurrentWeight_lbs();
}

double WsfP6DOF_Mover::GetCurrentWeight_kg() const
{
   return mP6DofVehiclePtr->GetCurrentWeight_lbs() * UtMath::cKG_PER_LB;
}

double WsfP6DOF_Mover::GetEmptyWeight_lbs() const
{
   return mP6DofVehiclePtr->GetEmptyWeight_lbs();
}

double WsfP6DOF_Mover::GetEmptyWeight_kg() const
{
   return mP6DofVehiclePtr->GetEmptyWeight_lbs() * UtMath::cKG_PER_LB;
}

double WsfP6DOF_Mover::GetDesignWeightForMaxG_lbs() const
{
   return mP6DofVehiclePtr->GetEmptyWeight_lbs();
}

double WsfP6DOF_Mover::GetAlt_m() const
{
   return mP6DofVehiclePtr->GetAlt_m();
}

double WsfP6DOF_Mover::GetWingArea_sqft() const
{
   return mP6DofVehiclePtr->GetWingArea_sqft();
}

double WsfP6DOF_Mover::GetNx_g() const
{
   return mP6DofVehiclePtr->GetNx_g();
}

double WsfP6DOF_Mover::GetNy_g() const
{
   return mP6DofVehiclePtr->GetNy_g();
}

double WsfP6DOF_Mover::GetNz_g() const
{
   return mP6DofVehiclePtr->GetNz_g();
}

size_t WsfP6DOF_Mover::GetControlSurfaceHandle(const std::string& aControlSurfaceName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         return fltControls->GetControlSurfaceHandle(aControlSurfaceName);
      }
   }
   return 0;
}

size_t WsfP6DOF_Mover::GetControlSurfaceHandleContainingString(const std::string& aString) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         return fltControls->GetControlSurfaceHandleContainingString(aString);
      }
   }
   return 0;
}

size_t WsfP6DOF_Mover::GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1,
                                                                   const std::string& aString2) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         return fltControls->GetControlSurfaceHandleContainingTwoStrings(aString1, aString2);
      }
   }
   return 0;
}

double WsfP6DOF_Mover::GetAngleOfControlSurface(std::string aControlSurfaceName) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         size_t handle = fltControls->GetControlSurfaceHandle(aControlSurfaceName);

         if (handle > 0)
         {
            return fltControls->GetControlSurfaceAngle_deg(handle);
         }
      }
   }

   return 0.0;
}

// Returns the position (deg) of the specified control surface (based on handle),
// or returns zero if no surface exists
double WsfP6DOF_Mover::GetAngleOfControlSurface(size_t aHandle) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         if (aHandle > 0)
         {
            return fltControls->GetControlSurfaceAngle_deg(aHandle);
         }
      }
   }

   return 0.0;
}

// Returns a normalized (either +/-1 or 0-1) angle of the specified control surface (based on handle),
// or returns zero if no surface exists
double WsfP6DOF_Mover::GetNormalizedAngleOfControlSurface(size_t aHandle) const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         if (aHandle > 0)
         {
            return fltControls->GetControlSurfaceValueNormalized(aHandle);
         }
      }
   }

   return 0.0;
}

std::vector<std::string> WsfP6DOF_Mover::GetListOfControlSurfaceNames() const
{
   std::vector<std::string> surfaceNameList;

   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofFlightControlSystem* fltControls = mP6DofVehiclePtr->GetFlightControls();
      if (fltControls != nullptr)
      {
         fltControls->GetControlSurfaceNames(surfaceNameList);
      }
   }

   return surfaceNameList;
}

double WsfP6DOF_Mover::GetAileronsBothPosition() const
{
   return GetAngleAileronRight();
}

double WsfP6DOF_Mover::GetStabilatorLeftPosition() const
{
   return GetAngleStabilizerLeft();
}

double WsfP6DOF_Mover::GetStabilatorRightPosition() const
{
   return GetAngleStabilizerRight();
}

double WsfP6DOF_Mover::GetRudderPosition() const
{
   return GetAngleRudder();
}

double WsfP6DOF_Mover::GetLandingGearPosition() const
{
   return GetAngleLandingGear();
}

double WsfP6DOF_Mover::GetLandingGearLeverPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetLandingGearControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetSpeedBrakePosition_rad() const
{
   return GetSpeedBrakePosition() * UtMath::cRAD_PER_DEG;
}

double WsfP6DOF_Mover::GetSpeedBrakePosition() const
{
   return GetAngleSpeedBrake();
}

double WsfP6DOF_Mover::GetFlapsPosition() const
{
   return GetAngleFlapLeft();
}

double WsfP6DOF_Mover::GetFlapsLeverPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetFlapsControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetSpoilersPosition() const
{
   return GetAngleSpoilerLeft();
}

double WsfP6DOF_Mover::GetSpoilersLeverPosition() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      P6DofPilotObject* controller = mP6DofVehiclePtr->GetPilotObject();
      if (controller != nullptr)
      {
         return controller->GetSpoilersControllerPosition();
      }
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNoseWheelAngle() const
{
   return GetAngleOfControlSurface("Nose_Wheel_Angle_Rgt");
}

double WsfP6DOF_Mover::GetAngleAileronLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleAileronLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleAileronRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleAileronRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleFlapLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleFlapLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleFlapRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleFlapRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleSpoilerLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleSpoilerLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleSpoilerRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleSpoilerRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleStabilizerLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleStabilizerLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleStabilizerRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleStabilizerRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleElevator() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleElevator();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleRudderLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleRudderLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleRudder() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleRudder();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleRudderRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleRudderRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleSpeedBrake() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleSpeedBrake();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleLandingGear() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleLandingGear();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleLandingGearNose() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleLandingGearNose();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleLandingGearMainLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleLandingGearMainLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetAngleLandingGearMainRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetAngleLandingGearMainRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedAileronLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedAileronLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedAileronRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedAileronRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedFlapLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedFlapLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedFlapRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedFlapRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedSpoilerLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedSpoilerLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedSpoilerRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedSpoilerRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedStabilizerLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedStabilizerLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedStabilizerRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedStabilizerRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedElevator() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedElevator();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedRudderLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedRudderLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedRudder() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedRudder();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedRudderRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedRudderRight();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedSpeedBrake() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedSpeedBrake();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedLandingGear() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedLandingGear();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedLandingGearNose() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedLandingGearNose();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedLandingGearMainLeft() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedLandingGearMainLeft();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetNormalizedLandingGearMainRight() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetNormalizedLandingGearMainRight();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::AileronLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->AileronLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::AileronRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->AileronRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::FlapLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->FlapLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::FlapRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->FlapRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::SpoilerLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->SpoilerLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::SpoilerRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->SpoilerRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::StabilizerLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->StabilizerLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::StabilizerRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->StabilizerRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::ElevatorValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->ElevatorValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::RudderLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->RudderLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::RudderValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->RudderValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::RudderRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->RudderRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::SpeedBrakeValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->SpeedBrakeValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::LandingGearValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->LandingGearValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::LandingGearNoseValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->LandingGearNoseValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::LandingGearMainLeftValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->LandingGearMainLeftValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::LandingGearMainRightValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->LandingGearMainRightValid();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::ThrottleValid() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->ThrottleValid();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetStickRightInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetStickRightInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetStickBackInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetStickBackInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetRudderRightInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetRudderRightInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetThrottleInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetThrottleInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetSpeedBrakeInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetSpeedBrakeInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetSpoilerInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetSpoilerInput();
   }
   return 0.0;
}

double WsfP6DOF_Mover::GetLandingGearInput() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetLandingGearInput();
   }
   return 0.0;
}

bool WsfP6DOF_Mover::StallWarningAlert() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->StallWarningAlert();
   }
   return false;
}

void WsfP6DOF_Mover::GetAeroParametersAtAltitude(double  aAltitude_ft,
                                                 double  aSpeed_fps,
                                                 double& aAirDensity,
                                                 double& aDynamicPressure_psf,
                                                 double& aStaticPressure_psf,
                                                 double& aSpeedOfSound_fps,
                                                 double& aMach) const
{
   P6DofAtmosphere* atm = mP6DofVehiclePtr->GetScenario()->GetAtmosphere();

   if (atm == nullptr)
   {
      aAirDensity          = 0;
      aDynamicPressure_psf = 0;
      aStaticPressure_psf  = 0;
      aSpeedOfSound_fps    = 0;
      aMach                = 0;
      return;
   }

   atm->GetAeroParametersAtAltitude(aAltitude_ft, aSpeed_fps, aAirDensity, aDynamicPressure_psf, aStaticPressure_psf, aMach);

   aSpeedOfSound_fps = atm->SpeedOfSoundAtAltitude_fps(aAltitude_ft);
}

double WsfP6DOF_Mover::GetDynamicPressure_psf() const
{
   return mP6DofVehiclePtr->GetDynamicPressure_psf();
}

double WsfP6DOF_Mover::GetCLMaxAtMach(double aMach) const
{
   const P6DofCommonController* controller = mPilotManagerPtr->GetSpecialCommonController();
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

double WsfP6DOF_Mover::GetAlphaAtCLMaxAtMach_deg(double aMach) const
{
   const P6DofCommonController* controller = mPilotManagerPtr->GetSpecialCommonController();
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

std::string WsfP6DOF_Mover::GetActivePilot() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         return pilot->GetPilotType();
      }
   }

   return "NONE";
}

P6DOF::Lateral::Mode WsfP6DOF_Mover::GetAutopilotLateralModeEnum() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return action->GetLateralChannelMode();
         }
      }
   }

   return P6DOF::Lateral::Undefined;
}

std::string WsfP6DOF_Mover::GetAutopilotLateralMode() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return LateralModeToString(action->GetLateralChannelMode());
         }
      }
   }

   return "Undefined";
}

float WsfP6DOF_Mover::GetAutopilotLateralModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotLateralModeEnum())
   {
   case P6DOF::Lateral::Mode::Heading:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetHeading_deg();
      break;
   }
   case P6DOF::Lateral::Mode::Bank:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetBank_deg();
      break;
   }
   case P6DOF::Lateral::Mode::RollRate:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetRollRate_dps();
      break;
   }
   case P6DOF::Lateral::Mode::DeltaRoll:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetRollDelta_deg();
      break;
   }
   case P6DOF::Lateral::Mode::YawGLoad:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetYawGLoad_g();
      break;
   }
   case P6DOF::Lateral::Mode::YawRate:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetYawRate_dps();
      break;
   }
   case P6DOF::Lateral::Mode::Beta:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetBank_deg();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

P6DOF::Vertical::Mode WsfP6DOF_Mover::GetAutopilotVerticalModeEnum() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return action->GetVerticalChannelMode();
         }
      }
   }

   return P6DOF::Vertical::Undefined;
}

std::string WsfP6DOF_Mover::GetAutopilotVerticalMode() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return VerticalModeToString(action->GetVerticalChannelMode());
         }
      }
   }

   return "Undefined";
}

float WsfP6DOF_Mover::GetAutopilotVerticalModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotVerticalModeEnum())
   {
   case P6DOF::Vertical::Mode::Altitude:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetAltitudeMSL_ft();
      break;
   }
   case P6DOF::Vertical::Mode::VertSpeed:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetVerticalRate_fpm();
      break;
   }
   case P6DOF::Vertical::Mode::PitchGLoad:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetPitchGLoad_g();
      break;
   }
   case P6DOF::Vertical::Mode::PitchAng:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetPitchAngle_deg();
      break;
   }
   case P6DOF::Vertical::Mode::FltPathAng:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetFlightPathAngle_deg();
      break;
   }
   case P6DOF::Vertical::Mode::DeltaPitch:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetDeltaPitch_deg();
      break;
   }
   case P6DOF::Vertical::Mode::Alpha:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetAlpha_deg();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

P6DOF::Speed::Mode WsfP6DOF_Mover::GetAutopilotSpeedModeEnum() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return action->GetSpeedChannelMode();
         }
      }
   }

   return P6DOF::Speed::Undefined;
}

std::string WsfP6DOF_Mover::GetAutopilotSpeedMode() const
{
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofAutopilotAction* action = pilot->GetCurrentAction();
         if (action != nullptr)
         {
            return SpeedModeToString(action->GetSpeedChannelMode());
         }
      }
   }

   return "Undefined";
}

float WsfP6DOF_Mover::GetAutopilotSpeedModeValue() const
{
   float value = 0.0f;

   switch (GetAutopilotSpeedModeEnum())
   {
   case P6DOF::Speed::Mode::ForwardAccel:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetForwardAcceleration_g();
      break;
   }
   case P6DOF::Speed::Mode::KIAS:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetCalibratedAirSpeed_KCAS();
      break;
   }
   case P6DOF::Speed::Mode::KTAS:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetTrueAirSpeed_KTAS();
      break;
   }
   case P6DOF::Speed::Mode::Mach:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetMach();
      break;
   }
   case P6DOF::Speed::Mode::FPS:
   {
      value = mPilotManagerPtr->GetActivePilot()->GetCurrentAction()->GetSpeed_fps();
      break;
   }
   default:
   {
      break;
   }
   }

   return value;
}

void WsfP6DOF_Mover::SetDamageFactor(double aDamageFactor)
{
   if (aDamageFactor >= 0.0)
   {
      if (aDamageFactor <= 1.0)
      {
         if (aDamageFactor > mDamageFactor)
         {
            mDamageFactor = aDamageFactor;
         }
      }
   }

   if (mDamageFactor > 0.5)
   {
      mHeavyDamage = true;
      mLightDamage = false;
   }
   else if (mDamageFactor > 0.001)
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

void WsfP6DOF_Mover::SetDestroyed()
{
   mDestroyed = true;

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

   GetP6DofVehicle()->SetDestroyed(pitchInputModifier, rollInputModifier, isOnFire, engineIsSmoking, isTrailingSmoke);
}

bool WsfP6DOF_Mover::GetEngineIsOperating() const
{
   return GetP6DofVehicle()->AnEngineIsOperating();
}

bool WsfP6DOF_Mover::GetEngineIsSmoking() const
{
   return GetP6DofVehicle()->AnEngineIsSmoking();
}

bool WsfP6DOF_Mover::GetEngineAfterburnerIsOn() const
{
   return GetP6DofVehicle()->AnEngineHasAfterburnerOn();
}

void WsfP6DOF_Mover::MakeEnginesSmoke(bool aEngineSmoking)
{
   GetP6DofVehicle()->MakeEnginesSmoke();
}

bool WsfP6DOF_Mover::GetContrailTrailingEffect() const
{
   return GetP6DofVehicle()->ContrailTrailingEffectPresent();
}

bool WsfP6DOF_Mover::GetRocketSmokeTrailingEffect() const
{
   return GetP6DofVehicle()->RocketSmokeTrailingEffectPresent();
}

bool WsfP6DOF_Mover::GetDamageSmokeTrailingEffect() const
{
   return GetP6DofVehicle()->DamageSmokeTrailingEffectPresent();
}

void WsfP6DOF_Mover::SetDamageSmokeTrailingEffect(bool aEffectActive)
{
   GetP6DofVehicle()->ActivateDamageSmokeTrailingEffect(aEffectActive);
}

bool WsfP6DOF_Mover::GetRisingSmokePlumeIsPresent() const
{
   return mRisingSmokePlume;
}

void WsfP6DOF_Mover::SetRisingSmokePlumeIsPresent(bool aSmoking)
{
   mRisingSmokePlume = aSmoking;
}

bool WsfP6DOF_Mover::GetLaunchFlashSmokeIsPresent() const
{
   return mLaunchFlashSmoke;
}

void WsfP6DOF_Mover::SetLaunchFlashSmokeIsPresent(bool aLaunchFlashSmoke)
{
   mLaunchFlashSmoke = aLaunchFlashSmoke;
}

bool WsfP6DOF_Mover::GetFlamesArePresent() const
{
   return GetP6DofVehicle()->FlamesArePresent();
}

void WsfP6DOF_Mover::SetFlamesArePresent(bool aFlaming)
{
   GetP6DofVehicle()->ActivateFlamesEffect(aFlaming);
}

bool WsfP6DOF_Mover::GetIsLightlyDamaged() const
{
   return mLightDamage;
}

void WsfP6DOF_Mover::SetIsLightlyDamaged()
{
   mLightDamage = true;
}

bool WsfP6DOF_Mover::GetIsHeavilyDamaged() const
{
   return mHeavyDamage;
}

void WsfP6DOF_Mover::SetIsHeavilyDamaged()
{
   mHeavyDamage = true;
}

bool WsfP6DOF_Mover::GetIsDestroyed()
{
   return mDestroyed;
}

void WsfP6DOF_Mover::SetTestingIgnoreAllCrashes(bool aValue)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->SetTestingIgnoreAllCrashes(aValue);
   }
}

bool WsfP6DOF_Mover::GetTestingIgnoreAllCrashes() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetTestingIgnoreAllCrashes();
   }

   return false;
}

void WsfP6DOF_Mover::SetTestingClampToSealLevelMinAlt(bool aValue)
{
   if (mP6DofVehiclePtr != nullptr)
   {
      mP6DofVehiclePtr->SetTestingClampToSealLevelMinAlt(aValue);
   }
}

bool WsfP6DOF_Mover::GetTestingClampToSealLevelMinAlt() const
{
   if (mP6DofVehiclePtr != nullptr)
   {
      return mP6DofVehiclePtr->GetTestingClampToSealLevelMinAlt();
   }

   return false;
}

void WsfP6DOF_Mover::OnPlatformBrokenEvent(double aSimTime)
{
   // WsfP6DOF_Mover prevents itself from being turned off by overriding this function and doing nothing
}

WsfManeuver* WsfP6DOF_Mover::GetCurrentManeuver() const
{
   WsfManeuver* retval{nullptr};
   if (GetManeuverSequence() != nullptr)
   {
      retval = GetManeuverSequence()->GetCurrentManeuver();
   }
   return retval;
}

WsfManeuverSequence* WsfP6DOF_Mover::GetManeuverSequence() const
{
   return mManeuverPtr.get();
}

void WsfP6DOF_Mover::ExecuteManeuver(double aSimTime, std::unique_ptr<WsfManeuver>&& aManeuverPtr)
{
   std::unique_ptr<WsfManeuverSequence> sqnPtr{nullptr};
   if (aManeuverPtr->IsSequence())
   {
      sqnPtr.reset(static_cast<WsfManeuverSequence*>(aManeuverPtr.release()));
   }
   else
   {
      sqnPtr = ut::make_unique<WsfManeuverSequence>(); // Formerly sqnPtr.reset(new WsfManeuverSequence());
      sqnPtr->Append(std::move(aManeuverPtr));         // This may not be needed
   }
   ExecuteManeuverSequence(aSimTime, std::move(sqnPtr));
}

void WsfP6DOF_Mover::ExecuteManeuverSequence(double aSimTime, std::unique_ptr<WsfManeuverSequence>&& aSequencePtr)
{
   mManeuverPtr = std::move(aSequencePtr);
   mManeuverPtr->Assign(aSimTime, this);
}

void WsfP6DOF_Mover::CancelManeuvers()
{
   if (mManeuverPtr)
   {
      mManeuverPtr->Cancel();
   }
}

void WsfP6DOF_Mover::GetInitialState(double aLocWCS[3], double aVelWCS[3], double aOrientationNED[3]) const
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

void WsfP6DOF_Mover::SetInitialLatLon(double aLat, double aLon)
{
   mInitialPosition_Latitude  = aLat;
   mInitialPosition_Longitude = aLon;
   mValidInitialPosition      = true;
}

void WsfP6DOF_Mover::SetInitialAlt(double aAlt_m)
{
   mInitialAltitude_m       = aAlt_m;
   mValidInitialAltitude_ft = true;
}

void WsfP6DOF_Mover::SetInitialNEDHeading(double aHeading_rad)
{
   mInitialNEDHeading      = aHeading_rad;
   mValidInitialNEDHeading = true;
}

void WsfP6DOF_Mover::SetInitialNEDPitch(double aPitch_rad)
{
   mInitialNEDPitch      = aPitch_rad;
   mValidInitialNEDPitch = true;
}

void WsfP6DOF_Mover::SetInitialNEDRoll(double aRoll_rad)
{
   mInitialNEDRoll      = aRoll_rad;
   mValidInitialNEDRoll = true;
}

void WsfP6DOF_Mover::SetInitialNEDVelocity(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   mInitialSpeed_NED_X_fps = aVelNorth_mps * UtMath::cFT_PER_M;
   mInitialSpeed_NED_Y_fps = aVelEast_mps * UtMath::cFT_PER_M;
   mInitialSpeed_NED_Z_fps = aVelDown_mps * UtMath::cFT_PER_M;
   mValidInitialSpeed_NED  = true;
}

void WsfP6DOF_Mover::ResetPosition(double aSimTime)
{
   double lat, lon, alt;
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   mP6DofVehiclePtr->SetLocationLLA(lat, lon, alt);
}

WsfPlatform* WsfP6DOF_Mover::DirectJettisonSubobjectPlatform(P6DofVehicle* aJettisonedObject, double aSimTime_sec)
{
   auto jettisoned = mP6DofVehiclePtr->DirectJettisonOfSubobject(aJettisonedObject, aSimTime_sec);

   WsfPlatform* platform = nullptr;
   if (jettisoned)
   {
      platform = CommonSubobjectJettisonTasks(std::move(jettisoned));
   }

   return platform;
}
