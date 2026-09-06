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

#include "WsfSixDOF_PilotObject.hpp"

#include <cstring>

#include "UtException.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_Utils.hpp"

wsf::six_dof::PilotObject::PilotObject(const PilotObject& aSrc)
   : Object(aSrc)
   , WaypointAchieved()
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mAutopilotControls(aSrc.mAutopilotControls)
   , mAutopilotActionPtr(aSrc.mAutopilotActionPtr)
   , mPlannedRoutePtr(aSrc.mPlannedRoutePtr)
   , mCurrentPlannedWaypointIndex(aSrc.mCurrentPlannedWaypointIndex)
   , mTempActionPtr(aSrc.mTempActionPtr)
   , mTempRoutePtr(aSrc.mTempRoutePtr)
   , mCurrentTempWaypointIndex(aSrc.mCurrentTempWaypointIndex)
   , mControlAugmentationStickBack(aSrc.mControlAugmentationStickBack)
   , mControlAugmentationStickRight(aSrc.mControlAugmentationStickRight)
   , mControlAugmentationRudderRight(aSrc.mControlAugmentationRudderRight)
   , mPitchControlAugmentationFactor_g(aSrc.mPitchControlAugmentationFactor_g)
   , mYawControlAugmentationFactor_g(aSrc.mYawControlAugmentationFactor_g)
   , mYawControlAugmentationFactor_deg(aSrc.mYawControlAugmentationFactor_deg)
   , mRollControlAugmentationFactor_dps(aSrc.mRollControlAugmentationFactor_dps)
   , mPitchTrimFactor(aSrc.mPitchTrimFactor)
   , mRollTrimFactor(aSrc.mRollTrimFactor)
   , mYawTrimFactor(aSrc.mYawTrimFactor)
   , mNoseUpTrimDeltaT_sec(aSrc.mNoseUpTrimDeltaT_sec)
   , mRollRightTrimDeltaT_sec(aSrc.mRollRightTrimDeltaT_sec)
   , mYawRightTrimDeltaT_sec(aSrc.mYawRightTrimDeltaT_sec)
   , mTrimNoseUp(aSrc.mTrimNoseUp)
   , mTrimRollRight(aSrc.mTrimRollRight)
   , mTrimYawRight(aSrc.mTrimYawRight)
   , mPitchControlMapping(aSrc.mPitchControlMapping)
   , mRollControlMapping(aSrc.mRollControlMapping)
   , mYawControlMapping(aSrc.mYawControlMapping)
   , mControlOverrideStickBack(aSrc.mControlOverrideStickBack)
   , mControlOverrideStickRight(aSrc.mControlOverrideStickRight)
   , mControlOverrideRudderRight(aSrc.mControlOverrideRudderRight)
   , mControlOverrideThrottle(aSrc.mControlOverrideThrottle)
   , mControlOverrideSpeedBrakes(aSrc.mControlOverrideSpeedBrakes)
   , mControlAugmentationModeActive(aSrc.mControlAugmentationModeActive)
   , mPitchStabilityAugmentationModeActive(aSrc.mPitchStabilityAugmentationModeActive)
   , mRollStabilityAugmentationModeActive(aSrc.mRollStabilityAugmentationModeActive)
   , mYawStabilityAugmentationModeActive(aSrc.mYawStabilityAugmentationModeActive)
   , mParentControl(aSrc.mParentControl)
   , mManualControl(aSrc.mManualControl)
   , mExternalDirectControl(aSrc.mExternalDirectControl)
   , mAutopilotEnabled(aSrc.mAutopilotEnabled)
   , mControlsEnabled(aSrc.mControlsEnabled)
   , mTestControl(aSrc.mTestControl)
   , mIsDestroyed(aSrc.mIsDestroyed)
{
   if (mPlannedRoutePtr)
   {
      if (aSrc.mPlannedPrevWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedPrevWptDataPtr);
         mPlannedPrevWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mPlannedCurrWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedCurrWptDataPtr);
         mPlannedCurrWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mPlannedNextWptDataPtr != nullptr)
      {
         size_t index           = aSrc.mPlannedRoutePtr->GetWaypointIndex(aSrc.mPlannedNextWptDataPtr);
         mPlannedNextWptDataPtr = mPlannedRoutePtr->GetWaypointAtIndex(index);
      }
   }

   if (mTempRoutePtr)
   {
      if (aSrc.mTempPrevWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempPrevWptDataPtr);
         mTempPrevWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mTempCurrWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempCurrWptDataPtr);
         mTempCurrWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (aSrc.mTempNextWptDataPtr != nullptr)
      {
         size_t index        = aSrc.mTempRoutePtr->GetWaypointIndex(aSrc.mTempNextWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }
   }
}

void wsf::six_dof::PilotObject::UpdateControlInputsUsingCommonControllerData(int64_t aSimTime_nanosec)
{
   if (GetCommonController() == nullptr)
   {
      return;
   }

   if (ControlModeAutopilotActive())
   {
      if (mTempRoutePtr != nullptr && mTempActionPtr != nullptr)
      {
         // Temp activity is being flown, check if its a route
         if (GetCommonController()->GetWaypointAchieved())
         {
            if (mTempRoutePtr->GetNumberOfWaypoints() >= 2)
            {
               mTempPrevWptDataPtr = mTempCurrWptDataPtr;
               mTempCurrWptDataPtr = mTempNextWptDataPtr;
               mTempNextWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

               if (mTempPrevWptDataPtr != nullptr)
               {
                  mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                                  mTempCurrWptDataPtr,
                                                  mTempNextWptDataPtr,
                                                  mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                                  mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));
                  GetCommonController()->SetCurrentActivity(mTempActionPtr);

                  mCurrentTempWaypointIndex = mTempRoutePtr->GetWaypointIndex(mTempCurrWptDataPtr);

                  int id = mTempPrevWptDataPtr->GetId();
                  if (id != -1)
                  {
                     WaypointAchieved(id);
                  }
               }
            }
         }
      }
      else
      {
         // Check if any active channel is in waypoint mode
         // TODO -- Remove this for now -- need to investigate
         // if( (mAutopilotControlData.autopilotHorizontalModeActive &&
         // (mAutopilotControlData.autopilotHorizontalMode == AutopilotDataTypes::ARCM_WPT)) ||
         // (mAutopilotControlData.autopilotVerticalModeActive &&
         // (mAutopilotControlData.autopilotVerticalMode == AutopilotDataTypes::APCM_WPT)) ||
         // (mAutopilotControlData.autopilotSpeedModeActive &&
         // (mAutopilotControlData.autopilotSpeedMode == AutopilotDataTypes::ASCM_WPT)) )

         // At least one active channel is in waypoint mode, so determine if we need to go to the next waypoint
         if (GetCommonController()->GetWaypointAchieved())
         {
            // if( (mCurrentPlannedWaypointIndex >= 1) && (mCurrentPlannedWaypointIndex <
            // (int)mPlannedWaypointList.size()-2) )
            // {
            // // Move to next waypoint
            // ++mCurrentPlannedWaypointIndex;
            //
            // mPlannedWaypointData.SetPreviousWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex-1)->waypoint));
            // mPlannedWaypointData.SetCurrentWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->waypoint));
            // mPlannedWaypointData.SetNextWaypoint(*(mPlannedWaypointList.at(mCurrentPlannedWaypointIndex+1)->waypoint));
            //
            // // We set the speed based on speed-type from the waypoint speed pairing -- only one of these speeds
            // should be > zero. if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_ktas > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(Waypoint::SPD_TYPE_TAS_KNOTS,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_ktas );
            // }
            // else if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_kias > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(Waypoint::SPD_TYPE_CAS_KNOTS,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_kias );
            // }
            // else if( mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_mach > 0 )
            // {
            // mPlannedWaypointData.SetSpeed(Waypoint::SPD_TYPE_MACH,
            // mPlannedWaypointList.at(mCurrentPlannedWaypointIndex)->speed_mach );
            // }
            //
            // mPlannedWaypointData.SetWaypointSwitchingType(Waypoint::WP_ON_APPROACH);
            //
            // mAutopilotActivityPtr->SetNavWaypoint(mPlannedWaypointData);
            // GetCommonController()->SetCurrentActivity(mAutopilotActivityPtr);
            // }

            mPlannedPrevWptDataPtr = mPlannedCurrWptDataPtr;
            mPlannedCurrWptDataPtr = mPlannedNextWptDataPtr;

            if (mPlannedRoutePtr != nullptr)
            {
               mPlannedNextWptDataPtr = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

               if (mPlannedPrevWptDataPtr != nullptr)
               {
                  mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                                       mPlannedCurrWptDataPtr,
                                                       mPlannedNextWptDataPtr,
                                                       mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr),
                                                       mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));
                  GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);

                  mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);

                  int id = mPlannedPrevWptDataPtr->GetId();
                  if (id != -1)
                  {
                     WaypointAchieved(id);
                  }
               }
            }
            else
            {
               mPlannedNextWptDataPtr = nullptr;
            }
         }
      }

      // Update the autopilot
      GetCommonController()->Update(mAutopilotControls, aSimTime_nanosec);

      // Set the controls data
      SetControlDataWithAutopilotControls();
   }
   else if (ControlModeControlAugmentationActive())
   {
      // Create a new temp activity if one does not already exist
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = ut::make_unique<AutopilotAction>();
      }

      Control::Method controlMethod     = GetCommonController()->GetControlMethod();
      bool            yawToTurnControl  = Control::IsYawToTurn((controlMethod));
      bool            bankToTurnControl = Control::IsBankToTurn((controlMethod));

      if (ControlModeStabilityAugmentationActive())
      {
         // Autopilot won't fully take control, but will contribute stabilizing inputs

         // Set the modes
         if (mPitchStabilityAugmentationModeActive)
         {
            mTempActionPtr->SetPitchRate_dps(0);
            mTempActionPtr->SetVerticalChannelMode(Vertical::PitchRate);
         }

         if (mYawStabilityAugmentationModeActive)
         {
            if (yawToTurnControl)
            {
               mTempActionPtr->SetYawRate_dps(0);
               mTempActionPtr->SetLateralChannelMode(Lateral::YawRate);
            }
            if (bankToTurnControl)
            {
               // Uncomment the following to use yaw rate in SAS
               mTempActionPtr->SetYawRate_dps(0);
               mTempActionPtr->SetStabilizingChannelMode(Lateral::YawRate);

               // Uncomment the following to use beta in SAS
               // mTempActionPtr->SetBeta_deg(0);
               // mTempActionPtr->SetStabilizingChannelMode(MRM::Lateral::Beta);
            }
         }

         if (mRollStabilityAugmentationModeActive)
         {
            mTempActionPtr->SetRollRate_dps(0);

            if (yawToTurnControl)
            {
               mTempActionPtr->SetStabilizingChannelMode(Lateral::RollRate);
            }
            if (bankToTurnControl)
            {
               mTempActionPtr->SetLateralChannelMode(Lateral::RollRate);
            }
         }

         mTempActionPtr->SetSpeedChannelMode(Speed::Undefined);

         // TODO - Do we need to do this?
         GetCommonController()->SetCurrentActivity(mTempActionPtr);

         // Update the autopilot
         GetCommonController()->Update(mAutopilotControls, aSimTime_nanosec);

         // Set the controls data
         SetControlDataWithStabilityAugmentationControls();
      }
      else
      {
         // The AP will control using manual stick inputs

         // Utilize the augmentation stick inputs modified by the augmenetation factors to
         // get the pitch g-load and roll rate.

         float pitchGLoad = static_cast<float>(mControlAugmentationStickBack * mPitchControlAugmentationFactor_g);
         mTempActionPtr->SetVerticalChannelMode(Vertical::PitchGLoad);

         float rollRate_dps = static_cast<float>(mControlAugmentationStickRight * mRollControlAugmentationFactor_dps);

         // Set the roll channel using roll rate based on stick
         mTempActionPtr->SetRollRate_dps(rollRate_dps);

         // Yaw uses the same augmentation factor as pitch,
         // which may or may not be a good idea.
         if (yawToTurnControl)
         {
            // No g-bias for skid-to-turn vehicles
            mTempActionPtr->SetPitchGLoad_g(pitchGLoad);

            float yawGLoad = static_cast<float>(mControlAugmentationRudderRight * mYawControlAugmentationFactor_g);
            mTempActionPtr->SetYawGLoad_g(yawGLoad);
            mTempActionPtr->SetLateralChannelMode(Lateral::YawGLoad);

            // By setting the roll channel to Undefined, we allow the appropriate roll-nulling control
            mTempActionPtr->SetStabilizingChannelMode(Lateral::Undefined);
         }
         else if (bankToTurnControl)
         {
            mTempActionPtr->SetPitchGLoad_g(pitchGLoad + 1.0f);

            float beta_deg = static_cast<float>(mControlAugmentationRudderRight * mYawControlAugmentationFactor_deg);
            mTempActionPtr->SetBeta_deg(beta_deg);
            mTempActionPtr->SetStabilizingChannelMode(Lateral::Beta);

            mTempActionPtr->SetLateralChannelMode(Lateral::RollRate);
         }

         mTempActionPtr->SetSpeedChannelMode(Speed::Undefined);

         // TODO - Do we need to do this?
         GetCommonController()->SetCurrentActivity(mTempActionPtr);

         // Update the autopilot
         GetCommonController()->Update(mAutopilotControls, aSimTime_nanosec);

         // Set the controls data
         SetControlDataWithControlAugmentationControls();
      }
   }
}

bool wsf::six_dof::PilotObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "inherit_controls")
   {
      aInput.ReadValue(mParentControl);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void wsf::six_dof::PilotObject::Update(int64_t aSimTime_nanosec)
{
   // Calculate the delta-T
   int64_t dT_nanosec = aSimTime_nanosec - mLastSimTime_nanosec;

   if (dT_nanosec <= 0)
   {
      // If same time, simply return
      return;
   }

   mLastSimTime_nanosec = aSimTime_nanosec;

   // Handle disabled controls
   if (ControlModeDisabledActive())
   {
      ZeroDisabledControlData();
   }
   else if (mParentControl)
   {
      InheritParentControlData();
   }
   else
   {
      UpdateControlInputsUsingCommonControllerData(aSimTime_nanosec);
   }

   ManageFrozenControls();
}

void wsf::six_dof::PilotObject::InputAngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad)
{
   if (GetCommonController())
   {
      GetCommonController()->AngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }
}

void wsf::six_dof::PilotObject::SetTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                                         double aRollRightTrimDeltaT_sec,
                                                         double aYawRightTrimDeltaT_sec)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mNoseUpTrimDeltaT_sec += aNoseUpTrimDeltaT_sec;
   mRollRightTrimDeltaT_sec += aRollRightTrimDeltaT_sec;
   mYawRightTrimDeltaT_sec += aYawRightTrimDeltaT_sec;
}

bool wsf::six_dof::PilotObject::InitializeCommonController()
{
   if (GetCommonController())
   {
      GetCommonController()->SetParentVehicle(mParentVehicle);
      GetCommonController()->SetControlAugmentationModeActive(mControlAugmentationModeActive);

      if (mAutopilotActionPtr == nullptr)
      {
         mAutopilotActionPtr = new AutopilotAction;
      }
      GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);

      // TODO -- Do we want to initialize the mTempActivityPtr?
   }

   return true;
}

void wsf::six_dof::PilotObject::SetPrePositionTAS(double aAltitude_ft, double aKTAS)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mParentVehicle->ZeroKinematicsDuringTesting();
   mParentVehicle->SetPrepositionAlt_m(aAltitude_ft * UtMath::cM_PER_FT);

   double speed_fps = UtMath::cFPS_PER_NMPH * aKTAS;
   double speed_mps = UtMath::cM_PER_FT * speed_fps;

   mParentVehicle->SetVelocityNED_mps(speed_mps, 0.0, 0.0);
   mParentVehicle->SetOrientationNED(0.0, 0.0, 0.0);

   if (GetCommonController())
   {
      GetCommonController()->ResetAccumulatedPidData();
   }
}

void wsf::six_dof::PilotObject::SetPrePositionCAS(double aAltitude_ft, double aKCAS)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   double ktas = mParentVehicle->GetEnvironment()->CalcKtasFromKcas(aAltitude_ft * UtMath::cM_PER_FT, aKCAS);

   SetPrePositionTAS(aAltitude_ft, ktas);
}

void wsf::six_dof::PilotObject::SetPrePositionMach(double aAltitude_ft, double aMach)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   double ktas = mParentVehicle->GetEnvironment()->CalcKtasFromMach(aAltitude_ft * UtMath::cM_PER_FT, aMach);

   SetPrePositionTAS(aAltitude_ft, ktas);
}

void wsf::six_dof::PilotObject::SetPrePositionQ(double aAltitude_ft, double aDynamicPressure_lbft2)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   double speed_fps = mParentVehicle->GetEnvironment()->CalcFpsFromDynamicPressure(aAltitude_ft * UtMath::cM_PER_FT,
                                                                                   aDynamicPressure_lbft2);
   double ktas      = UtMath::cNMPH_PER_FPS * speed_fps;

   SetPrePositionTAS(aAltitude_ft, ktas);
}

void wsf::six_dof::PilotObject::SetObjectState(double aLat,
                                               double aLon,
                                               double aAlt_m,
                                               double aNEDVelN_mps,
                                               double aNEDVelE_mps,
                                               double aNEDVelD_mps,
                                               double aLocalHeading_rad,
                                               double aLocalPitch_rad,
                                               double aLocalRoll_rad)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   // Set conditions
   mParentVehicle->SetLocationLLA(aLat, aLon, aAlt_m);
   mParentVehicle->SetAttitudeNED_rad(aLocalHeading_rad, aLocalPitch_rad, aLocalRoll_rad);
   mParentVehicle->SetVelocityNED_mps(aNEDVelN_mps, aNEDVelE_mps, aNEDVelD_mps);

   CommonController* commonController = GetCommonController();
   if (commonController)
   {
      commonController->ResetAccumulatedPidData();
   }
}

void wsf::six_dof::PilotObject::SetControlDataWithAutopilotControls()
{
   if (ControlModeAutopilotActive())
   {
      LoadControlDataWithAutopilotControlData();
   }
}

void wsf::six_dof::PilotObject::SetControlDataWithControlAugmentationControls()
{
   if (ControlModeControlAugmentationActive())
   {
      LoadControlDataWithAutopilotControlData();
   }
}

void wsf::six_dof::PilotObject::SetControlDataWithStabilityAugmentationControls()
{
   if (ControlModeStabilityAugmentationActive())
   {
      LoadControlDataWithAutopilotStabilityData();
   }
}

void wsf::six_dof::PilotObject::EnableAutopilot(bool aState)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mAutopilotEnabled = aState;
}

wsf::six_dof::AutopilotAction* wsf::six_dof::PilotObject::GetCurrentAction() const
{
   if (mTempActionPtr != nullptr)
   {
      return mTempActionPtr;
   }
   return mAutopilotActionPtr;
}

void wsf::six_dof::PilotObject::GetAutopilotData(AutopilotData* aData)
{
   if ((GetCommonController() == nullptr) || !mAutopilotEnabled)
   {
      aData->autopilotEnabled = false;

      aData->navWaypointParameters.navWaypointDataValid     = false;
      aData->navWaypointParameters.navWaypointLegType       = Nav::LegUndefined;
      aData->navWaypointParameters.navWaypointSwitchingType = Nav::SwitchUndefined;

      aData->navWaypointParameters.navWaypointPrevLat = 0.0f;
      aData->navWaypointParameters.navWaypointPrevLon = 0.0f;
      aData->navWaypointParameters.navWaypointPrevAlt = 0.0f;

      aData->navWaypointParameters.navWaypointCurrLat = 0.0f;
      aData->navWaypointParameters.navWaypointCurrLon = 0.0f;
      aData->navWaypointParameters.navWaypointCurrAlt = 0.0f;

      aData->navWaypointParameters.navWaypointNextLat = 0.0f;
      aData->navWaypointParameters.navWaypointNextLon = 0.0f;
      aData->navWaypointParameters.navWaypointNextAlt = 0.0f;

      aData->navWaypointParameters.navWaypointAimHeading_rad   = 0.0f;
      aData->navWaypointParameters.navWaypointStartTurnHdg_rad = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRadius_m     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnAngle_rad    = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRefPtLat     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnRefPtLon     = 0.0f;
      aData->navWaypointParameters.navWaypointTurnCenterLat    = 0.0f;
      aData->navWaypointParameters.navWaypointTurnCenterLon    = 0.0f;

      return;
   }

   // Show AP as enabled
   aData->autopilotEnabled = true;

   double aimHeading_rad;
   double startTurnHdg_rad;
   double turnRadius_m;
   double turnAngle_rad;
   double turnRefPtLat;
   double turnRefPtLon;
   bool   turnCW;
   bool   turning;
   double turnCenterLat;
   double turnCenterLon;

   GetCommonController()->GetAutoPilotTurnData(aimHeading_rad,
                                               startTurnHdg_rad,
                                               turnRadius_m,
                                               turnAngle_rad,
                                               turnRefPtLat,
                                               turnRefPtLon,
                                               turnCW,
                                               turning,
                                               turnCenterLat,
                                               turnCenterLon);

   aData->navWaypointParameters.navWaypointAimHeading_rad   = static_cast<float>(aimHeading_rad);
   aData->navWaypointParameters.navWaypointStartTurnHdg_rad = static_cast<float>(startTurnHdg_rad);
   aData->navWaypointParameters.navWaypointTurnRadius_m     = static_cast<float>(turnRadius_m);
   aData->navWaypointParameters.navWaypointTurnAngle_rad    = static_cast<float>(turnAngle_rad);
   aData->navWaypointParameters.navWaypointTurnRefPtLat     = static_cast<float>(turnRefPtLat);
   aData->navWaypointParameters.navWaypointTurnRefPtLon     = static_cast<float>(turnRefPtLon);
   aData->navWaypointParameters.navWaypointTurnCenterLat    = static_cast<float>(turnCenterLat);
   aData->navWaypointParameters.navWaypointTurnCenterLon    = static_cast<float>(turnCenterLon);

   if (mTempActionPtr)
   {
      aData->navWaypointParameters.navWaypointDataValid = true;

      if (mTempPrevWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointPrevLat = static_cast<float>(mTempPrevWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointPrevLon = static_cast<float>(mTempPrevWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointPrevAlt = static_cast<float>(mTempPrevWptDataPtr->GetLLA().GetAlt());
      }
      if (mTempCurrWptDataPtr != nullptr)
      {
         if (mTempCurrWptDataPtr->FollowHorizontalTrack())
         {
            aData->navWaypointParameters.navWaypointLegType = Nav::FollowTrack;
         }
         else
         {
            aData->navWaypointParameters.navWaypointLegType = Nav::DirectTo;
         }

         if (!mTempCurrWptDataPtr->WaypointOnPassing())
         {
            aData->navWaypointParameters.navWaypointSwitchingType = Nav::OnApproach;
         }
         else
         {
            aData->navWaypointParameters.navWaypointSwitchingType = Nav::OnPassing;
         }

         aData->navWaypointParameters.navWaypointCurrLat = static_cast<float>(mTempCurrWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointCurrLon = static_cast<float>(mTempCurrWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointCurrAlt = static_cast<float>(mTempCurrWptDataPtr->GetLLA().GetAlt());
      }
      if (mTempNextWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointNextLat = static_cast<float>(mTempNextWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointNextLon = static_cast<float>(mTempNextWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointNextAlt = static_cast<float>(mTempNextWptDataPtr->GetLLA().GetAlt());
      }
   }
   else
   {
      aData->navWaypointParameters.navWaypointDataValid = true;

      if (mPlannedPrevWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointPrevLat = static_cast<float>(mPlannedPrevWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointPrevLon = static_cast<float>(mPlannedPrevWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointPrevAlt = static_cast<float>(mPlannedPrevWptDataPtr->GetLLA().GetAlt());
      }
      if (mPlannedCurrWptDataPtr != nullptr)
      {
         if (mPlannedCurrWptDataPtr->FollowHorizontalTrack())
         {
            aData->navWaypointParameters.navWaypointLegType = Nav::FollowTrack;
         }
         else
         {
            aData->navWaypointParameters.navWaypointLegType = Nav::DirectTo;
         }

         if (!mPlannedCurrWptDataPtr->WaypointOnPassing())
         {
            aData->navWaypointParameters.navWaypointSwitchingType = Nav::OnApproach;
         }
         else
         {
            aData->navWaypointParameters.navWaypointSwitchingType = Nav::OnPassing;
         }

         aData->navWaypointParameters.navWaypointCurrLat = static_cast<float>(mPlannedCurrWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointCurrLon = static_cast<float>(mPlannedCurrWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointCurrAlt = static_cast<float>(mPlannedCurrWptDataPtr->GetLLA().GetAlt());
      }
      if (mPlannedNextWptDataPtr != nullptr)
      {
         aData->navWaypointParameters.navWaypointNextLat = static_cast<float>(mPlannedNextWptDataPtr->GetLLA().GetLat());
         aData->navWaypointParameters.navWaypointNextLon = static_cast<float>(mPlannedNextWptDataPtr->GetLLA().GetLon());
         aData->navWaypointParameters.navWaypointNextAlt = static_cast<float>(mPlannedNextWptDataPtr->GetLLA().GetAlt());
      }
   }
}

bool wsf::six_dof::PilotObject::GetAutopilotPidGainData(Pid::Type   aTableType,
                                                        size_t&     aNumElements,
                                                        PidGainData aPidGainData[])
{
   if (GetCommonController() == nullptr)
   {
      return false;
   }

   return GetCommonController()->GetAutopilotPidGainData(aTableType, aNumElements, aPidGainData);
}

void wsf::six_dof::PilotObject::GetAutopilotPidValueData(AutopilotPidGroupValueData* aData)
{
   if (GetCommonController() == nullptr)
   {
      return;
   }

   GetCommonController()->GetAutopilotPidValues(aData);
}

void wsf::six_dof::PilotObject::EnforceControlLimits()
{
   EnforceSingleControlLimit(mAutopilotControls.stickBack, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.stickRight, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.rudderRight, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.throttleMilitary, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.throttleAfterburner, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorYaw, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorPitch, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.thrustVectorRoll, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.speedBrake, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.noseWheelSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.nwsSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.wheelBrakeLeft, 0.0, 1.0);
   EnforceSingleControlLimit(mAutopilotControls.wheelBrakeRight, 0.0, 1.0);
}

void wsf::six_dof::PilotObject::EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue)
{
   if (aValue < aMinValue)
   {
      aValue = aMinValue;
   }
   if (aValue > aMaxValue)
   {
      aValue = aMaxValue;
   }
}

void wsf::six_dof::PilotObject::SetPlannedRoute(Route* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mPlannedRoutePtr = aRoute;

   if (mPlannedRoutePtr != nullptr)
   {
      KinematicState* state = mParentVehicle->GetKinematicState();

      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 1)
      {
         const Waypoint* first = mPlannedRoutePtr->GetFirstElement();

         state->SetPositionLLA(first->GetLLA().GetLat(), first->GetLLA().GetLat(), first->GetLLA().GetAlt());

         double flightPathAngle_deg = state->GetFlightPathAngle_deg();
         double bearing_deg         = state->GetBearing_deg();

         // We set the speed based on speed-type from the waypoint speed pairing -- only one of these speeds should be > zero.
         if (first->GetSpeed().type == Waypoint::SPD_TYPE_TAS_KNOTS)
         {
            state->SetSpeed_ktas(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }
         else if (first->GetSpeed().type == Waypoint::SPD_TYPE_CAS_KNOTS)
         {
            state->SetSpeed_kias(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }
         else if (first->GetSpeed().type == Waypoint::SPD_TYPE_MACH)
         {
            state->SetSpeed_Mach(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }
         else if (first->GetSpeed().type == Waypoint::SPD_TYPE_FPS)
         {
            state->SetSpeed_fps(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }
         else if (first->GetSpeed().type == Waypoint::SPD_TYPE_MPH)
         {
            state->SetSpeed_mph(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }
         else if (first->GetSpeed().type == Waypoint::SPD_TYPE_MPS)
         {
            state->SetSpeed_mps(first->GetSpeed().val, bearing_deg, flightPathAngle_deg);
         }

         mPlannedPrevWptDataPtr = first;
      }

      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 2)
      {
         const Waypoint* first  = mPlannedRoutePtr->GetFirstElement();
         const Waypoint* second = mPlannedRoutePtr->GetNextWaypoint(first);

         double heading_deg, distance;

         UtSphericalEarth::GreatCircleHeadingAndDistance(first->GetLLA().GetLat(),
                                                         first->GetLLA().GetLon(),
                                                         second->GetLLA().GetLat(),
                                                         second->GetLLA().GetLon(),
                                                         heading_deg,
                                                         distance);
         double deltaAlt = second->GetLLA().GetAlt() - first->GetLLA().GetAlt();

         double pitchRad = atan2(deltaAlt, distance);
         double rollRad  = 0;

         state->SetAttitude_rad(heading_deg * UtMath::cRAD_PER_DEG, pitchRad, rollRad);
         state->SetSpeed_ktas(state->GetSpeed_ktas(), heading_deg, pitchRad * UtMath::cDEG_PER_RAD);

         mPlannedPrevWptDataPtr = first;
         mPlannedCurrWptDataPtr = second;
      }

      if (mPlannedRoutePtr->GetNumberOfWaypoints() >= 3)
      {
         const Waypoint* first  = mPlannedRoutePtr->GetFirstElement();
         const Waypoint* second = mPlannedRoutePtr->GetNextWaypoint(first);
         const Waypoint* third  = mPlannedRoutePtr->GetNextWaypoint(second);

         mPlannedPrevWptDataPtr = first;
         mPlannedCurrWptDataPtr = second;
         mPlannedNextWptDataPtr = third;
      }

      if (mAutopilotActionPtr == nullptr)
      {
         mAutopilotActionPtr = ut::make_unique<AutopilotAction>();
      }
      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr),
                                           mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));

      if (GetCommonController() != nullptr)
      {
         GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);
      }

      mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
   }
}

void wsf::six_dof::PilotObject::SetTempRoute(Route* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mTempRoutePtr = aRoute;
}

void wsf::six_dof::PilotObject::FlyTempWaypoints(Route* aRoute)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (GetCommonController())
   {
      if (aRoute->GetNumberOfWaypoints() >= 2)
      {
         auto     prev   = ut::make_unique<Waypoint>();
         UtLLAPos prevPt = mParentVehicle->GetKinematicState()->GetCurrentPositionLLA();

         prev->SetLLA(prevPt);

         Route* newRoute = aRoute;
         newRoute->AddWaypointToRouteStart(prev);
         newRoute->ComputeSegmentMap();

         mTempRoutePtr = newRoute;

         mTempPrevWptDataPtr = mTempRoutePtr->GetFirstElement();
         mTempCurrWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempPrevWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

         if (mTempActionPtr == nullptr)
         {
            mTempActionPtr = ut::make_unique<AutopilotAction>();
         }

         if (mTempPrevWptDataPtr != nullptr && mTempCurrWptDataPtr != nullptr)
         {
            mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                            mTempCurrWptDataPtr,
                                            mTempNextWptDataPtr,
                                            mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                            mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));
            GetCommonController()->SetCurrentActivity(mTempActionPtr);

            mCurrentTempWaypointIndex = mTempRoutePtr->GetWaypointIndex(mTempCurrWptDataPtr);
         }
      }
      else // Store route, but should we do anything else?
      {
         mTempRoutePtr = aRoute;
      }
   }
}

bool wsf::six_dof::PilotObject::SetPlannedWaypointIndex(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mPlannedRoutePtr != nullptr) && (aIndex < mPlannedRoutePtr->GetNumberOfWaypoints()))
   {
      mCurrentPlannedWaypointIndex = aIndex;
      mPlannedCurrWptDataPtr       = mPlannedRoutePtr->GetWaypointAtIndex(aIndex);
      mPlannedNextWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

      // Use the current position to transition to a new current waypoint
      KinematicState* state = mParentVehicle->GetKinematicState();

      auto transitionWaypoint = ut::make_unique<Waypoint>(state->GetLat(), state->GetLon(), state->GetAlt_m());
      transitionWaypoint->SetSpeed(mPlannedPrevWptDataPtr->GetSpeed());
      std::unique_ptr<Route::sRouteSegment> transitionSegment =
         Route::CalcSegmentGeometry(transitionWaypoint.get(), mPlannedCurrWptDataPtr);

      std::unique_ptr<Route::sRouteSegment> nextSegment =
         mPlannedRoutePtr->CalcSegmentGeometry(mPlannedCurrWptDataPtr, mPlannedNextWptDataPtr);

      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           transitionSegment.get(),
                                           nextSegment.get());

      GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);

      // Add the transition waypoint and segment into the transition route so that
      // they are properly destroyed;
      mTransitionRoute.AddSegment(transitionWaypoint.get(), transitionSegment);
      mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);

      return true;
   }

   return false;
}

bool wsf::six_dof::PilotObject::SetPositionToPlannedWaypoint(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mPlannedRoutePtr != nullptr) && (aIndex < mPlannedRoutePtr->GetNumberOfWaypoints()))
   {
      // Set waypoint data
      mCurrentPlannedWaypointIndex = aIndex + 1;
      mPlannedPrevWptDataPtr       = mPlannedRoutePtr->GetWaypointAtIndex(aIndex);
      mPlannedCurrWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedPrevWptDataPtr);
      mPlannedNextWptDataPtr       = mPlannedRoutePtr->GetNextWaypoint(mPlannedCurrWptDataPtr);

      Route::sRouteSegment* currSegment = mPlannedRoutePtr->GetRouteSegment(mPlannedPrevWptDataPtr);
      Route::sRouteSegment* nextSegment = mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr);

      // Set object state
      double lat   = mPlannedPrevWptDataPtr->GetLLA().GetLat();
      double lon   = mPlannedPrevWptDataPtr->GetLLA().GetLon();
      double alt_m = mPlannedPrevWptDataPtr->GetLLA().GetAlt();

      UtVec3d normDirectionVec;
      currSegment->trackVector.Normalize(normDirectionVec);

      double heading_rad = currSegment->trackStartHdg_rad;
      double pitch_rad   = currSegment->slope_rad;
      double speed_fps   = 0.0;
      double speed_mps   = 0.0;

      const Environment* env = mParentVehicle->GetEnvironment();

      Waypoint::sSpeed speed = mPlannedCurrWptDataPtr->GetSpeed();

      if (speed.type == Waypoint::SPD_TYPE_CAS_KNOTS)
      {
         double speed_kias = speed.val;
         speed_fps         = env->CalcFpsFromKcas(alt_m, speed_kias);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == Waypoint::SPD_TYPE_TAS_KNOTS)
      {
         double speed_ktas = speed.val;
         speed_fps         = UtMath::cFPS_PER_NMPH * speed_ktas;
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == Waypoint::SPD_TYPE_MACH)
      {
         double mach = speed.val;
         speed_fps   = env->CalcFpsFromMach(alt_m, mach);
         speed_mps   = speed_fps * UtMath::cM_PER_FT;
      }
      else
      {
         // error -- no speed specified
         return false;
      }

      normDirectionVec = normDirectionVec * speed_mps * -1.0;

      SetObjectState(lat, lon, alt_m, normDirectionVec[0], normDirectionVec[1], normDirectionVec[2], heading_rad, pitch_rad, 0.0);

      // Set autopilot activity
      mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                           mPlannedCurrWptDataPtr,
                                           mPlannedNextWptDataPtr,
                                           currSegment,
                                           nextSegment);

      GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);

      return true;
   }

   return false;
}

bool wsf::six_dof::PilotObject::SetTempWaypointIndex(size_t aIndex)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   if ((mTempRoutePtr != nullptr) && (aIndex < mTempRoutePtr->GetNumberOfWaypoints()))
   {
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = ut::make_unique<AutopilotAction>();
      }

      mCurrentTempWaypointIndex = aIndex;
      mTempCurrWptDataPtr       = mTempRoutePtr->GetWaypointAtIndex(aIndex);
      mTempNextWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

      KinematicState* state = mParentVehicle->GetKinematicState();

      // Use the current position to transition to a new current waypoint
      auto transitionWaypoint = ut::make_unique<Waypoint>(state->GetLat(), state->GetLon(), state->GetAlt_m());
      transitionWaypoint->SetSpeed(mTempPrevWptDataPtr->GetSpeed());

      std::unique_ptr<Route::sRouteSegment> transitionSegment =
         Route::CalcSegmentGeometry(transitionWaypoint.get(), mTempCurrWptDataPtr);

      std::unique_ptr<Route::sRouteSegment> nextSegment =
         mTempRoutePtr->CalcSegmentGeometry(mTempCurrWptDataPtr, mTempNextWptDataPtr);

      mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                      mTempCurrWptDataPtr,
                                      mTempNextWptDataPtr,
                                      transitionSegment.get(),
                                      nextSegment.get());

      GetCommonController()->SetCurrentActivity(mTempActionPtr);

      // Add the transition waypoint and segment into the transition route so that
      // they are properly destroyed
      mTransitionRoute.AddSegment(transitionWaypoint.get(), transitionSegment);
      mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);

      return true;
   }

   return false;
}

bool wsf::six_dof::PilotObject::SetPositionToTempWaypoint(size_t aIndex)
{
   if ((mTempRoutePtr != nullptr) && (aIndex < mTempRoutePtr->GetNumberOfWaypoints()))
   {
      if (mTempActionPtr == nullptr)
      {
         mTempActionPtr = ut::make_unique<AutopilotAction>();
      }

      // Set waypoint data
      mCurrentTempWaypointIndex = aIndex + 1;
      mTempPrevWptDataPtr       = mTempRoutePtr->GetWaypointAtIndex(aIndex);
      mTempCurrWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempPrevWptDataPtr);
      mTempNextWptDataPtr       = mTempRoutePtr->GetNextWaypoint(mTempCurrWptDataPtr);

      Route::sRouteSegment* currSegment = mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr);
      Route::sRouteSegment* nextSegment = mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr);

      // Set object state
      double lat   = mTempPrevWptDataPtr->GetLLA().GetLat();
      double lon   = mTempPrevWptDataPtr->GetLLA().GetLon();
      double alt_m = mTempPrevWptDataPtr->GetLLA().GetAlt();

      UtVec3d normDirectionVec;
      currSegment->trackVector.Normalize(normDirectionVec);

      double heading_rad = currSegment->trackStartHdg_rad;
      double pitch_rad   = currSegment->slope_rad;
      double speed_fps   = 0.0;
      double speed_mps   = 0.0;

      const Environment* env = mParentVehicle->GetEnvironment();

      Waypoint::sSpeed speed = mTempCurrWptDataPtr->GetSpeed();

      if (speed.type == Waypoint::SPD_TYPE_CAS_KNOTS)
      {
         double speed_kias = speed.val;
         speed_fps         = env->CalcFpsFromKcas(alt_m, speed_kias);
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == Waypoint::SPD_TYPE_TAS_KNOTS)
      {
         double speed_ktas = speed.val;
         speed_fps         = UtMath::cFPS_PER_NMPH * speed_ktas;
         speed_mps         = speed_fps * UtMath::cM_PER_FT;
      }
      else if (speed.type == Waypoint::SPD_TYPE_MACH)
      {
         double mach = speed.val;
         speed_fps   = env->CalcFpsFromMach(alt_m, mach);
         speed_mps   = speed_fps * UtMath::cM_PER_FT;
      }
      else
      {
         // error -- no speed specified
         return false;
      }

      normDirectionVec = normDirectionVec * speed_mps * -1.0;

      SetObjectState(lat, lon, alt_m, normDirectionVec[0], normDirectionVec[1], normDirectionVec[2], heading_rad, pitch_rad, 0.0);

      // Set autopilot activity
      mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr, mTempCurrWptDataPtr, mTempNextWptDataPtr, currSegment, nextSegment);

      GetCommonController()->SetCurrentActivity(mTempActionPtr);

      return true;
   }

   return false;
}

void wsf::six_dof::PilotObject::FlyAtPoint(const UtLLAPos& aPt)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   KinematicState* state = mParentVehicle->GetKinematicState();

   auto prev = ut::make_unique<Waypoint>(state->GetLat(), state->GetLon(), state->GetAlt_m());
   auto curr = ut::make_unique<Waypoint>(aPt.GetLat(), aPt.GetLon(), aPt.GetAlt());

   curr->SetSpeed(Waypoint::SPD_TYPE_TAS_KNOTS, state->GetSpeed_ktas());
   curr->SetWaypointOnPassing(true);

   mTempPrevWptDataPtr = prev.get();
   mTempCurrWptDataPtr = curr.get();
   mTempNextWptDataPtr = nullptr;

   mTempRoutePtr = ut::make_unique<Route>();
   mTempRoutePtr->AddWaypointToRouteEnd(prev);
   mTempRoutePtr->AddWaypointToRouteEnd(curr);
   mTempRoutePtr->ComputeSegmentMap();

   mTempActionPtr->SetNavWaypoints(mTempPrevWptDataPtr,
                                   mTempCurrWptDataPtr,
                                   mTempNextWptDataPtr,
                                   mTempRoutePtr->GetRouteSegment(mTempPrevWptDataPtr),
                                   mTempRoutePtr->GetRouteSegment(mTempCurrWptDataPtr));

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::ReturnToPlannedWaypoints()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mPlannedRoutePtr != nullptr && mPlannedRoutePtr->GetNumberOfWaypoints() >= 3)
   {
      if (mTempActionPtr != nullptr)
      {
         KinematicState* state = mParentVehicle->GetKinematicState();

         // Transition point is the current location
         auto transitionWaypoint = ut::make_unique<Waypoint>(state->GetLat(), state->GetLon(), state->GetAlt_m());

         std::unique_ptr<Route::sRouteSegment> transitionSegment =
            Route::CalcSegmentGeometry(transitionWaypoint.get(), mPlannedCurrWptDataPtr);

         // Left the planned route while traveling to current waypoint, so set the
         // previous waypoint to the current position and return to the last planned
         // waypoint that was being followed
         mPlannedPrevWptDataPtr = transitionWaypoint.get();

         if (mPlannedPrevWptDataPtr != nullptr && mPlannedCurrWptDataPtr != nullptr)
         {
            mAutopilotActionPtr->SetNavWaypoints(mPlannedPrevWptDataPtr,
                                                 mPlannedCurrWptDataPtr,
                                                 mPlannedNextWptDataPtr,
                                                 transitionSegment.get(),
                                                 mPlannedRoutePtr->GetRouteSegment(mPlannedCurrWptDataPtr));

            GetCommonController()->SetCurrentActivity(mAutopilotActionPtr);
            mCurrentPlannedWaypointIndex = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
         }

         // Add the transition waypoint and segment into the transition route so
         // that they are properly destroyed
         mTransitionRoute.AddSegment(transitionWaypoint.get(), transitionSegment);
         mTransitionRoute.AddWaypointToRouteEnd(transitionWaypoint);

         // Set the user action to null
         mTempActionPtr = nullptr;
      }
      else
      {
         // already on way-points
      }
   }
   else
   {
      // can not fly way-points
   }
}

//! radial accel & turn direction are ignored for now
void wsf::six_dof::PilotObject::TurnToHeading(double aHeading_rad,                 // radians
                                              double,                              // aRadialAccel,   //meters/sec^2
                                              CommonController::eAutopilotTurnDir) // aTurn)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to heading
   mTempActionPtr->SetHeading_rad((float)aHeading_rad);
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::GoToSpeed(double aSpeed, // knots true air speed
                                          double)        // aLinearAccel) // meters/sec^2
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KTAS
   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(aSpeed));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::GoToAltitude(double aAltitude, // meters
                                             double)           // aClimbRate)  // meters/sec
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to alt
   mTempActionPtr->SetAltitudeMSL_ft(static_cast<float>(aAltitude * UtMath::cFT_PER_M));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Altitude);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

bool wsf::six_dof::PilotObject::FlyRates(double aRollRate,     // deg/sec
                                         double aPitchRate,    // deg/sec
                                         double aYawRate,      // deg/sec
                                         double aAcceleration) // m/sec^2
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes
   if (Control::IsBankToTurn(GetCommonController()->GetControlMethod()))
   {
      mTempActionPtr->SetRollRate_dps(static_cast<float>(aRollRate));
      mTempActionPtr->SetLateralChannelMode(Lateral::RollRate);
   }
   else if (Control::IsYawToTurn(GetCommonController()->GetControlMethod()))
   {
      mTempActionPtr->SetYawRate_dps(static_cast<float>(aYawRate));
      mTempActionPtr->SetLateralChannelMode(Lateral::YawRate);
   }

   mTempActionPtr->SetPitchRate_dps(static_cast<float>(aPitchRate));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchRate);

   double acceleration_gees = aAcceleration / utils::cREFERENCE_GRAV_ACCEL_MPS2;
   mTempActionPtr->SetForwardAcceleration_g(static_cast<float>(acceleration_gees));
   mTempActionPtr->SetSpeedChannelMode(Speed::ForwardAccel);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool wsf::six_dof::PilotObject::FlyHeadingSpeedAltitude(double aHeading_rad, // rad
                                                        double aSpeed,       // knots (true air speed)
                                                        double aAltitude,    // feet
                                                        double,              // aMaxGees,  // Gs
                                                        double)              // aMaxClimb) // feet/minute
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   mTempActionPtr->SetHeading_rad(static_cast<float>(aHeading_rad));
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft(static_cast<float>(aAltitude));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(aSpeed));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool wsf::six_dof::PilotObject::FlySpecificTurn(double aRollError, // degrees
                                                double aGees,      // Gs
                                                double aSpeed)     // knots (true air speed)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   // Do we need some limit here?
   // if (aGees < 0.0) { aGees = 0.0; }
   // if (aGees > 8.0) { aGees = 8.0; }

   mTempActionPtr->SetRollDelta_deg(static_cast<float>(aRollError));
   mTempActionPtr->SetLateralChannelMode(Lateral::DeltaRoll);

   mTempActionPtr->SetPitchGLoad_g(static_cast<float>(aGees));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchGLoad);

   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(aSpeed));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool wsf::six_dof::PilotObject::FlySpecificVector(double aRollError,  // degrees
                                                  double aPitchError, // degrees
                                                  double,             // aMaxGees,   // Gs
                                                  double aSpeed)      // knots (true air speed)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the modes

   mTempActionPtr->SetRollDelta_deg(static_cast<float>(aRollError));
   mTempActionPtr->SetLateralChannelMode(Lateral::DeltaRoll);

   mTempActionPtr->SetDeltaPitch_deg(static_cast<float>(aPitchError));
   mTempActionPtr->SetVerticalChannelMode(Vertical::DeltaPitch);

   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(aSpeed));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);

   // TODO -- Need a means to set a temp g-load to support aMaxGees

   // We need to reset the delta angles
   GetCommonController()->ResetAllDeltaAngles();

   GetCommonController()->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool wsf::six_dof::PilotObject::FlyYawPitchAccel(double aYawAccel_g, double aPitchAccel_g)
{
   if (mIsDestroyed)
   {
      return false;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical and lateral modes

   mTempActionPtr->SetYawGLoad_g(static_cast<float>(aYawAccel_g));
   mTempActionPtr->SetLateralChannelMode(Lateral::YawGLoad);

   mTempActionPtr->SetPitchGLoad_g(static_cast<float>(aPitchAccel_g));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchGLoad);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);

   return true;
}

bool wsf::six_dof::PilotObject::GetAfterburnerEnabled() const
{
   if (this->GetCommonController())
   {
      const AutopilotLimitsAndSettings& apControls = GetCommonController()->GetCurrentLimitsAndSettings();
      return static_cast<double>(apControls.enableAfterburnerAutoControl);
   }

   return true;
}

void wsf::six_dof::PilotObject::SetAfterburnerEnabled(bool aEnabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->GetCommonController())
   {
      GetCommonController()->SetAfterburnerEnabled(aEnabled);
   }
}

double wsf::six_dof::PilotObject::GetAfterburnerThreshold() const
{
   if (this->GetCommonController())
   {
      const AutopilotLimitsAndSettings& apControls = GetCommonController()->GetCurrentLimitsAndSettings();
      return static_cast<double>(apControls.afterburnerThreshold);
   }

   // No value avail, so return default of 1.0
   return 1.0;
}

void wsf::six_dof::PilotObject::SetAfterburnerThreshold(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->GetCommonController())
   {
      GetCommonController()->SetAfterburnerThreshold(aValue);
   }
}

bool wsf::six_dof::PilotObject::GetSpeedBrakeEnabled() const
{
   if (this->GetCommonController())
   {
      const AutopilotLimitsAndSettings& apControls = GetCommonController()->GetCurrentLimitsAndSettings();
      return static_cast<double>(apControls.enableSpeedBrakeAutoControl);
   }

   return true;
}

void wsf::six_dof::PilotObject::SetSpeedBrakeEnabled(bool aEnabled)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->GetCommonController())
   {
      GetCommonController()->SetSpeedBrakeEnabled(aEnabled);
   }
}

double wsf::six_dof::PilotObject::GetSpeedBrakeThreshold() const
{
   if (this->GetCommonController())
   {
      const AutopilotLimitsAndSettings& apControls = GetCommonController()->GetCurrentLimitsAndSettings();
      return static_cast<double>(apControls.speedBrakeThreshold);
   }

   // No value avail, so return default of 1.0
   return 0.0;
}

void wsf::six_dof::PilotObject::SetSpeedBrakeThreshold(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->GetCommonController())
   {
      GetCommonController()->SetSpeedBrakeThreshold(aValue);
   }
}

double wsf::six_dof::PilotObject::GetTurnRollInMultiplier()
{
   if (this->GetCommonController())
   {
      const AutopilotLimitsAndSettings& apControls = GetCommonController()->GetCurrentLimitsAndSettings();
      return static_cast<double>(apControls.turnRollInMultiplier);
   }

   // No value avail, so return default of 1.0
   return 0.0;
}

void wsf::six_dof::PilotObject::SetTurnRollInMultiplier(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (this->GetCommonController())
   {
      GetCommonController()->SetCurrentTurnRollInMultiplier(aValue);
   }
}

bool wsf::six_dof::PilotObject::UsingManualControl() const
{
   return mManualControl;
}

void wsf::six_dof::PilotObject::TakeManualControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mManualControl = true;
}

void wsf::six_dof::PilotObject::ReleaseManualControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mManualControl = false;
}

void wsf::six_dof::PilotObject::TakeExternalDirectControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mExternalDirectControl = true;
}

void wsf::six_dof::PilotObject::ReleaseExternalDirectControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mExternalDirectControl = false;
}

void wsf::six_dof::PilotObject::SetTestThrottleControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleData(aThrottlePosition);
   }
}

void wsf::six_dof::PilotObject::SetTestThrottleMilitaryControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleMilitaryData(aThrottlePosition);
   }
}

void wsf::six_dof::PilotObject::SetTestThrottleAfterburnerControllerPosition(double aThrottlePosition)
{
   if (mTestControl)
   {
      SetThrottleAfterburnerData(aThrottlePosition);
   }
}

void wsf::six_dof::PilotObject::EnableDirectThrottleInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideThrottle = true;
}

void wsf::six_dof::PilotObject::MoveThrottleToIdle()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(0.0);
      SetThrottleAfterburnerData(0.0);
   }
}

void wsf::six_dof::PilotObject::MoveThrottleToFull()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(1.0);
      SetThrottleAfterburnerData(0.0);
   }
}

void wsf::six_dof::PilotObject::MoveThrottleToAfterburner()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleMilitaryData(1.0);
      SetThrottleAfterburnerData(1.0);
   }
}

void wsf::six_dof::PilotObject::SetDirectThrottleInput(double aThrottleLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideThrottle)
   {
      SetThrottleData(aThrottleLeverPosition);
   }
}

void wsf::six_dof::PilotObject::ReleaseDirectThrottleInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideThrottle = false;
}

void wsf::six_dof::PilotObject::EnableDirectStickBackInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideStickBack = true;
}

void wsf::six_dof::PilotObject::ReleaseDirectStickBackInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideStickBack = false;
}

void wsf::six_dof::PilotObject::EnableDirectStickRightInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideStickRight = true;
}

void wsf::six_dof::PilotObject::ReleaseDirectStickRightInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideStickRight = false;
}

void wsf::six_dof::PilotObject::EnableDirectRudderRightInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideRudderRight = true;
}

void wsf::six_dof::PilotObject::ReleaseDirectRudderRightInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideRudderRight = false;
}

void wsf::six_dof::PilotObject::EnableDirectSpeedBrakeInput()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlOverrideSpeedBrakes = true;
}

void wsf::six_dof::PilotObject::ReleaseDirectSpeedBrakeInput()
{
   mControlOverrideSpeedBrakes = false;
}

void wsf::six_dof::PilotObject::SetControlAugmentationModeActive(bool aCASIsActive)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mControlAugmentationModeActive = aCASIsActive;

   if (GetCommonController())
   {
      GetCommonController()->SetControlAugmentationModeActive(mControlAugmentationModeActive);
   }
}

bool wsf::six_dof::PilotObject::ControlAugmentationModeIsActive() const
{
   return mControlAugmentationModeActive;
}

void wsf::six_dof::PilotObject::SetExternalDirectControlTrimManualControlData(double aNoseUpTrimDeltaT_sec,
                                                                              double aRollRightTrimDeltaT_sec,
                                                                              double aYawRightTrimDeltaT_sec)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   mNoseUpTrimDeltaT_sec += aNoseUpTrimDeltaT_sec;
   mRollRightTrimDeltaT_sec += aRollRightTrimDeltaT_sec;
   mYawRightTrimDeltaT_sec += aYawRightTrimDeltaT_sec;
}


void wsf::six_dof::PilotObject::TakeTestControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed
   mTestControl = true;
}

void wsf::six_dof::PilotObject::ReleaseTestControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed
   mTestControl = false;
}

void wsf::six_dof::PilotObject::GetPlannedWaypoints(AutopilotWaypointDataList* aWaypointDataList) const
{
   if (mAutopilotActionPtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   int num = 0;
   if (mPlannedRoutePtr != nullptr)
   {
      for (auto& waypoint : mPlannedRoutePtr->GetRoute())
      {
         if (num >= 50)
         {
            break;
         }

         aWaypointDataList->waypointListData[num].lat   = static_cast<float>(waypoint->GetLLA().GetLat());
         aWaypointDataList->waypointListData[num].lon   = static_cast<float>(waypoint->GetLLA().GetLon());
         aWaypointDataList->waypointListData[num].alt_m = static_cast<float>(waypoint->GetLLA().GetAlt());

         if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_CAS_KNOTS)
         {
            aWaypointDataList->waypointListData[num].speed_kias = static_cast<float>(waypoint->GetSpeed().val);
            aWaypointDataList->waypointListData[num].speed_ktas = 0;
            aWaypointDataList->waypointListData[num].speed_mach = 0;
         }
         else if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_TAS_KNOTS)
         {
            aWaypointDataList->waypointListData[num].speed_kias = 0;
            aWaypointDataList->waypointListData[num].speed_ktas = static_cast<float>(waypoint->GetSpeed().val);
            aWaypointDataList->waypointListData[num].speed_mach = 0;
         }
         else if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_MACH)
         {
            aWaypointDataList->waypointListData[num].speed_kias = 0;
            aWaypointDataList->waypointListData[num].speed_ktas = 0;
            aWaypointDataList->waypointListData[num].speed_mach = static_cast<float>(waypoint->GetSpeed().val);
         }

         ++num;
      }
   }
   else
   {
      // No planned route, so return zeros
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   aWaypointDataList->currentWaypointIndex = static_cast<unsigned char>(mCurrentPlannedWaypointIndex);
   aWaypointDataList->waypointListNumItems = static_cast<unsigned char>(num);
}

void wsf::six_dof::PilotObject::GetTempWaypoints(AutopilotWaypointDataList* aWaypointDataList) const
{
   // If we lack an action, simply return
   if (mTempActionPtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   // If we lack a route, simply return
   if (mTempRoutePtr == nullptr)
   {
      aWaypointDataList->currentWaypointIndex = 0;
      aWaypointDataList->waypointListNumItems = 0;
      return;
   }

   int num = 0;

   for (auto& waypoint : mTempRoutePtr->GetRoute())
   {
      if (num >= 50)
      {
         break;
      }

      aWaypointDataList->waypointListData[num].lat   = static_cast<float>(waypoint->GetLLA().GetLat());
      aWaypointDataList->waypointListData[num].lon   = static_cast<float>(waypoint->GetLLA().GetLon());
      aWaypointDataList->waypointListData[num].alt_m = static_cast<float>(waypoint->GetLLA().GetAlt());

      if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_CAS_KNOTS)
      {
         aWaypointDataList->waypointListData[num].speed_kias = static_cast<float>(waypoint->GetSpeed().val);
         aWaypointDataList->waypointListData[num].speed_ktas = 0;
         aWaypointDataList->waypointListData[num].speed_mach = 0;
      }
      else if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_TAS_KNOTS)
      {
         aWaypointDataList->waypointListData[num].speed_kias = 0;
         aWaypointDataList->waypointListData[num].speed_ktas = static_cast<float>(waypoint->GetSpeed().val);
         aWaypointDataList->waypointListData[num].speed_mach = 0;
      }
      else if (waypoint->GetSpeed().type == Waypoint::SPD_TYPE_MACH)
      {
         aWaypointDataList->waypointListData[num].speed_kias = 0;
         aWaypointDataList->waypointListData[num].speed_ktas = 0;
         aWaypointDataList->waypointListData[num].speed_mach = static_cast<float>(waypoint->GetSpeed().val);
      }

      ++num;
   }
   aWaypointDataList->currentWaypointIndex = static_cast<unsigned char>(mCurrentTempWaypointIndex);
   aWaypointDataList->waypointListNumItems = static_cast<unsigned char>(num);
}

void wsf::six_dof::PilotObject::GetCurrentWaypoints(AutopilotWaypointDataList* aWaypointDataList) const
{
   if (mTempActionPtr != nullptr)
   {
      GetTempWaypoints(aWaypointDataList);
      return;
   }
   else if (mAutopilotActionPtr != nullptr)
   {
      GetPlannedWaypoints(aWaypointDataList);
      return;
   }

   // If we're here, we must lack any waypoints
   aWaypointDataList->currentWaypointIndex = 0;
   aWaypointDataList->waypointListNumItems = 0;
}

void wsf::six_dof::PilotObject::SetAutopilotPitchAngle(double aPitchAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetPitchAngle_deg(static_cast<float>(aPitchAngle_deg));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchAng);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotPitchRate(double aPitchRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch rate
   mTempActionPtr->SetPitchRate_dps(static_cast<float>(aPitchRate_dps));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchRate);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotFlightPathAngle(double aFlightPathAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to gamma
   mTempActionPtr->SetFlightPathAngle_deg(static_cast<float>(aFlightPathAngle_deg));
   mTempActionPtr->SetVerticalChannelMode(Vertical::FltPathAng);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotVerticalSpeed(double aVerticalSpeed_fpm)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to vertical speed
   mTempActionPtr->SetVerticalRate_fpm(static_cast<float>(aVerticalSpeed_fpm));
   mTempActionPtr->SetVerticalChannelMode(Vertical::VertSpeed);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotAltitude(double aAltitude_ft)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to alt
   mTempActionPtr->SetAltitudeMSL_ft(static_cast<float>(aAltitude_ft));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Altitude);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotRollAngle(double aRollAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to roll angle
   mTempActionPtr->SetBank_deg(static_cast<float>(aRollAngle_deg));
   mTempActionPtr->SetLateralChannelMode(Lateral::Bank);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotRollRate(double aRollRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to pitch angle
   mTempActionPtr->SetRollRate_dps(static_cast<float>(aRollRate_dps));
   mTempActionPtr->SetLateralChannelMode(Lateral::RollRate);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetPitchGLoad(double aGLoad)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetPitchGLoad_g(static_cast<float>(aGLoad));
   mTempActionPtr->SetVerticalChannelMode(Vertical::PitchGLoad);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotAlpha(double aAlpha_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetAlpha_deg(static_cast<float>(aAlpha_deg));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Alpha);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotDeltaPitch(double aDeltaPitchAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetDeltaPitch_deg(static_cast<float>(aDeltaPitchAngle_deg));
   mTempActionPtr->SetVerticalChannelMode(Vertical::DeltaPitch);

   GetCommonController()->ResetDeltaPitchAngle();
   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotDeltaRoll(double aDeltaRollAngle_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetRollDelta_deg(static_cast<float>(aDeltaRollAngle_deg));
   mTempActionPtr->SetLateralChannelMode(Lateral::DeltaRoll);

   GetCommonController()->ResetDeltaRollAngle();
   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotRollHeading(double aRollHeading_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetHeading_deg(static_cast<float>(aRollHeading_deg));
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotYawHeading(double aYawHeading_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetHeading_deg(static_cast<float>(aYawHeading_deg));
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotYawRate(double aYawRate_dps)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetYawRate_dps(static_cast<float>(aYawRate_dps));
   mTempActionPtr->SetLateralChannelMode(Lateral::YawRate);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotBeta(double aBeta_deg)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetBeta_deg(static_cast<float>(aBeta_deg));
   mTempActionPtr->SetLateralChannelMode(Lateral::Beta);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotYawGLoad(double aGLoad)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to pitch angle
   mTempActionPtr->SetYawGLoad_g(static_cast<float>(aGLoad));
   mTempActionPtr->SetLateralChannelMode(Lateral::YawGLoad);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotSpeedKTAS(double aSpeed_KTAS)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KTAS
   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(aSpeed_KTAS));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotSpeedKCAS(double aSpeed_KCAS)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to KCAS
   mTempActionPtr->SetCalibratedAirSpeed_KCAS(static_cast<float>(aSpeed_KCAS));
   mTempActionPtr->SetSpeedChannelMode(Speed::KIAS);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotSpeedMach(double aSpeed_Mach)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to mach
   mTempActionPtr->SetMach(static_cast<float>(aSpeed_Mach));
   mTempActionPtr->SetSpeedChannelMode(Speed::Mach);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotThrottle(double aThrottle)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to mach
   mTempActionPtr->SetThrottle(static_cast<float>(aThrottle));
   mTempActionPtr->SetSpeedChannelMode(Speed::Throttle);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotLateralWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the lateral mode to waypoint
   mTempActionPtr->SetLateralChannelMode(Lateral::Waypoint);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotVerticalWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the vertical mode to waypoint
   mTempActionPtr->SetVerticalChannelMode(Vertical::Waypoint);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotSpeedWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set the speed mode to waypoint
   mTempActionPtr->SetSpeedChannelMode(Speed::Waypoint);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

void wsf::six_dof::PilotObject::SetAutopilotWaypointMode()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   SetAutopilotLateralWaypointMode();
   SetAutopilotVerticalWaypointMode();
   SetAutopilotSpeedWaypointMode();
}

void wsf::six_dof::PilotObject::SetAutopilotNoControl()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   // Set the mTempActionPtr if appropriate
   SetTempActionToPlannedDataOrCurrentConditions();

   // Set all modes to undefined/no-control
   mTempActionPtr->SetLateralChannelMode(Lateral::Undefined);
   mTempActionPtr->SetVerticalChannelMode(Vertical::Undefined);
   mTempActionPtr->SetSpeedChannelMode(Speed::Undefined);

   GetCommonController()->SetCurrentActivity(mTempActionPtr);
}

bool wsf::six_dof::PilotObject::ClonePlannedActionToTempAction()
{
   if (!GetCommonController())
   {
      return false;
   }

   // Return if we do not have valid "planned" data
   if (!mAutopilotActionPtr)
   {
      return false;
   }

   // Clone the planned data to the temp data
   mTempActionPtr = mAutopilotActionPtr->Clone();

   if (mPlannedRoutePtr)
   {
      mTempRoutePtr = mPlannedRoutePtr->Clone();

      if (mPlannedPrevWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedPrevWptDataPtr);
         mTempPrevWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (mPlannedCurrWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedCurrWptDataPtr);
         mTempCurrWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      if (mPlannedNextWptDataPtr != nullptr)
      {
         size_t index        = mPlannedRoutePtr->GetWaypointIndex(mPlannedNextWptDataPtr);
         mTempNextWptDataPtr = mTempRoutePtr->GetWaypointAtIndex(index);
      }

      mCurrentTempWaypointIndex = mCurrentPlannedWaypointIndex;
   }

   return true;
}

void wsf::six_dof::PilotObject::SetTempActionToHoldCurrentConditions()
{
   if (GetCommonController() == nullptr)
   {
      return;
   }

   if (mTempActionPtr == nullptr)
   {
      mTempActionPtr = ut::make_unique<AutopilotAction>();
   }

   KinematicState* state = mParentVehicle->GetKinematicState();

   // Use current heading, altitude, and speed
   mTempActionPtr->SetHeading_rad(static_cast<float>(state->GetLocalHeading_rad()));
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft(static_cast<float>(state->GetAlt_ft()));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(state->GetSpeed_ktas()));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);
}

void wsf::six_dof::PilotObject::CreateTempActionIfNeeded()
{
   if (GetCommonController() == nullptr)
   {
      return;
   }

   if (mTempActionPtr != nullptr)
   {
      return;
   }

   mTempActionPtr = ut::make_unique<AutopilotAction>();

   KinematicState* state = mParentVehicle->GetKinematicState();

   // Use current heading, altitude, and speed
   mTempActionPtr->SetHeading_rad(static_cast<float>(state->GetLocalHeading_rad()));
   mTempActionPtr->SetLateralChannelMode(Lateral::Heading);

   mTempActionPtr->SetAltitudeMSL_ft(static_cast<float>(state->GetAlt_ft()));
   mTempActionPtr->SetVerticalChannelMode(Vertical::Altitude);

   mTempActionPtr->SetTrueAirSpeed_KTAS(static_cast<float>(state->GetSpeed_ktas()));
   mTempActionPtr->SetSpeedChannelMode(Speed::KTAS);
}

void wsf::six_dof::PilotObject::SetTempActionToPlannedDataOrCurrentConditions()
{
   if (GetCommonController() == nullptr)
   {
      return;
   }

   if (mTempActionPtr != nullptr)
   {
      return;
   }

   if (ClonePlannedActionToTempAction())
   {
      return;
   }

   SetTempActionToHoldCurrentConditions();
}

bool wsf::six_dof::PilotObject::ControlModeTestingActive() const
{
   return mTestControl;
}

bool wsf::six_dof::PilotObject::ControlModeDisabledActive() const
{
   if (mTestControl)
   {
      return false;
   }

   return !mControlsEnabled;
}

bool wsf::six_dof::PilotObject::ControlModeAutopilotActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   return mAutopilotEnabled;
}

bool wsf::six_dof::PilotObject::ControlModeManualActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (mAutopilotEnabled)
   {
      return false;
   }

   return mManualControl;
}

bool wsf::six_dof::PilotObject::ControlModeExternalDirectActive() const
{
   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (mManualControl)
   {
      return false;
   }

   if (!mControlAugmentationModeActive)
   {
      if (mAutopilotEnabled)
      {
         return false;
      }
   }

   return mExternalDirectControl;
}

bool wsf::six_dof::PilotObject::ControlModeControlAugmentationActive() const
{
   if (GetCommonController() == nullptr)
   {
      return false;
   }

   if (mTestControl)
   {
      return false;
   }

   if (!mControlsEnabled)
   {
      return false;
   }

   if (mAutopilotEnabled)
   {
      return false;
   }

   if (!mManualControl)
   {
      return false;
   }

   return mControlAugmentationModeActive;
}

bool wsf::six_dof::PilotObject::ControlModeStabilityAugmentationActive() const
{
   bool anyStabilityAugmentationActive = mPitchStabilityAugmentationModeActive || mYawStabilityAugmentationModeActive ||
                                         mRollStabilityAugmentationModeActive;
   return ControlModeControlAugmentationActive() && anyStabilityAugmentationActive;
}

void wsf::six_dof::PilotObject::SetDestroyed()
{
   mIsDestroyed = true;
}
