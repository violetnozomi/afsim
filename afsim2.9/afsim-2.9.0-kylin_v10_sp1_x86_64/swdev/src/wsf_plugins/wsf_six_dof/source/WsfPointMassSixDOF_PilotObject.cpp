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

#include "WsfPointMassSixDOF_PilotObject.hpp"

#include <cstring>

#include "UtException.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfPointMassSixDOF_FlightControlSystem.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfPointMassSixDOF_PropulsionSystem.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_Environment.hpp"

wsf::six_dof::PointMassPilotObject::PointMassPilotObject(const PointMassPilotObject& aSrc)
   : PilotObject(aSrc)
   , mCommonController(aSrc.mCommonController)
   , mStdStickBack(aSrc.mStdStickBack)
   , mStdStickRight(aSrc.mStdStickRight)
   , mStdRudderRight(aSrc.mStdRudderRight)
   , mStdThrottleMil(aSrc.mStdThrottleMil)
   , mStdThrottleAB(aSrc.mStdThrottleAB)
   , mStdSpeedBrakesOut(aSrc.mStdSpeedBrakesOut)
   , mStdFlapsDown(aSrc.mStdFlapsDown)
   , mStdSpoilersOut(aSrc.mStdSpoilersOut)
{
   if (mCommonController)
   {
      // Set the CAS and SAS modes
      mCommonController->SetControlAugmentationModeActive(mControlAugmentationModeActive);
      mCommonController->SetPitchStabilityAugmentationModeActive(mPitchStabilityAugmentationModeActive);
      mCommonController->SetYawStabilityAugmentationModeActive(mYawStabilityAugmentationModeActive);
      mCommonController->SetRollStabilityAugmentationModeActive(mRollStabilityAugmentationModeActive);
   }
}

bool wsf::six_dof::PointMassPilotObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (PilotObject::ProcessInput(aInput))
   {
   }
   else if (command == "autopilot_config")
   {
      mCommonController = new PointMassCommonController(aInput, command);

      if (mCommonController == nullptr)
      {
         auto out = ut::log::error() << "Unable to create mCommonController in PointMassPilotObject::ProcessInput().";
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool wsf::six_dof::PointMassPilotObject::Initialize(int64_t) // aSimTime_nanosec)
{
   InitializeCommonController();

   return true;
}

void wsf::six_dof::PointMassPilotObject::ManageFrozenControls()
{
   FreezeFlags* freeze = mParentVehicle->GetFreezeFlags();
   if (freeze == nullptr)
   {
      ut::log::error() << "Null Freeze Flags in PointMassPilotObject::Update().";
      return;
   }

   if (freeze->speed)
   {
      // Speed is frozen, set the throttle at MIL power and retract brakes and spoilers
      mStdThrottleMil    = 1.0f;
      mStdThrottleAB     = 0.0f;
      mStdSpeedBrakesOut = 0.0f;
      mStdFlapsDown      = 0.0f;
      mStdSpoilersOut    = 0.0f;
   }

   if (freeze->pitch)
   {
      // Pitch is frozen, zero stick-back and pitch TVC
      mStdStickBack = 0.0f;
   }

   if (freeze->roll)
   {
      // Roll is frozen, zero stick-back and roll TVC
      mStdStickRight = 0.0f;
   }

   if (freeze->yaw)
   {
      // Roll is frozen, zero stick-back and yaw TVC
      mStdRudderRight = 0.0f;
   }
}

void wsf::six_dof::PointMassPilotObject::LoadControlDataWithAutopilotControlData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Now we go through the "standard" controls and set the control input values
   // using the common controller (autopilot) data, where appropriate.

   mStdStickRight  = static_cast<float>(mAutopilotControls.stickRight);
   mStdStickBack   = static_cast<float>(mAutopilotControls.stickBack);
   mStdRudderRight = static_cast<float>(mAutopilotControls.rudderRight);

   if (ControlModeControlAugmentationActive())
   {
      // For CAS, we do not use the common controller data for throttle or speed brakes
   }
   else
   {
      // Only set data with common controller data if override is not active
      if (!mControlOverrideThrottle)
      {
         mStdThrottleMil = static_cast<float>(mAutopilotControls.throttleMilitary);
         mStdThrottleAB  = static_cast<float>(mAutopilotControls.throttleAfterburner);
      }

      // Only set data with common controller data if override is not active
      if (!mControlOverrideSpeedBrakes)
      {
         mStdSpeedBrakesOut = static_cast<float>(mAutopilotControls.speedBrake);
      }
   }

   // mStdThrustReverserPtr -- Do not use common controller data
   // mStdThrustVectoringYawPtr -- Not explicitly controlled in MRM
   // mStdThrustVectoringPitchPtr -- Not explicitly controlled in MRM
   // mStdThrustVectoringRollPtr -- Not explicitly controlled in MRM
   // mStdNoseWheelSteeringPtr -- Not controlled in MRM
   // mStdNWS_SteeringPtr -- Not controlled in MRM
   // mStdWheelBrakeLeftPtr -- Not controlled in MRM
   // mStdWheelBrakeRightPtr -- Not controlled in MRM

   // Do not use common controller data for mStdFlapsDownPtr, mStdSpoilersOutPtr,
   // mStdLandingGearDownPtr, and mStdNWS_EnabledPtr
}

void wsf::six_dof::PointMassPilotObject::LoadControlDataWithAutopilotStabilityData()
{
   // Be sure that controls are within limits
   EnforceControlLimits();

   // Go through the stick controls and blend the autopilot and manual responses.
   // Inspired by the A-10's SAS construction, SAS may contribute up to 25% stick
   // input in any axis. The remainder is provided by the stick inputs.

   const double autopilotMagnitudeLimit = 0.25;

   if (GetCommonController()->RollStabilityAugmentationModeIsActive())
   {
      double autopilotMagnitude = std::abs(mAutopilotControls.stickRight);
      autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
      double stickMagnitude     = 1.0 - autopilotMagnitude;

      mStdStickRight = static_cast<float>(mControlAugmentationStickRight * stickMagnitude +
                                          mAutopilotControls.stickRight * autopilotMagnitude);
   }

   if (GetCommonController()->PitchStabilityAugmentationModeIsActive())
   {
      double autopilotMagnitude = std::abs(mAutopilotControls.stickBack);
      autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
      double stickMagnitude     = 1.0 - autopilotMagnitude;

      mStdStickBack = static_cast<float>(mControlAugmentationStickBack * stickMagnitude +
                                         mAutopilotControls.stickBack * autopilotMagnitude);
   }

   if (GetCommonController()->YawStabilityAugmentationModeIsActive())
   {
      double autopilotMagnitude = std::abs(mAutopilotControls.rudderRight);
      autopilotMagnitude        = std::min(autopilotMagnitude, autopilotMagnitudeLimit);
      double stickMagnitude     = 1.0 - autopilotMagnitude;

      mStdRudderRight = static_cast<float>(mControlAugmentationRudderRight * stickMagnitude +
                                           mAutopilotControls.rudderRight * autopilotMagnitude);
   }
}

void wsf::six_dof::PointMassPilotObject::EnforceControlLimits()
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

void wsf::six_dof::PointMassPilotObject::SetThrottleData(double aThrottleLeverPosition)
{
   // The aThrottleLeverPosition uses 0.0=IDLE, 1.0=MIL, and 2.0=AB
   {
      double tempThrottle = aThrottleLeverPosition;

      if (tempThrottle > 1.0)
      {
         // Using AB
         tempThrottle    = tempThrottle - 1.0;
         mStdThrottleMil = 1.0f;
         mStdThrottleAB  = static_cast<float>(tempThrottle);
      }
      else
      {
         // Not using AB
         mStdThrottleMil = static_cast<float>(tempThrottle);
         mStdThrottleAB  = 0.0f;
      }
   }

   // Ensure values are normalized (0.0 to 1.0)

   if (mStdThrottleMil < 0.0)
   {
      mStdThrottleMil = 0.0;
   }
   else if (mStdThrottleMil > 1.0)
   {
      mStdThrottleMil = 1.0;
   }

   if (mStdThrottleAB < 0.0)
   {
      mStdThrottleAB = 0.0;
   }
   else if (mStdThrottleAB > 1.0)
   {
      mStdThrottleAB = 1.0;
   }
}

void wsf::six_dof::PointMassPilotObject::SetThrottleMilitaryData(double aThrottleLeverPosition)
{
   mStdThrottleMil = static_cast<float>(aThrottleLeverPosition);

   // Ensure values are normalized (0.0 to 1.0)
   if (mStdThrottleMil < 0.0)
   {
      mStdThrottleMil = 0.0;
   }
   else if (mStdThrottleMil > 1.0)
   {
      mStdThrottleMil = 1.0;
   }
}

void wsf::six_dof::PointMassPilotObject::SetThrottleAfterburnerData(double aThrottleLeverPosition)
{
   mStdThrottleAB = static_cast<float>(aThrottleLeverPosition);

   // Ensure values are normalized (0.0 to 1.0)
   if (mStdThrottleAB < 0.0)
   {
      mStdThrottleAB = 0.0;
   }
   else if (mStdThrottleAB > 1.0)
   {
      mStdThrottleAB = 1.0;
   }
}

void wsf::six_dof::PointMassPilotObject::SetManualControlData(double aStickRightPosition,
                                                              double aStickAftPosition,
                                                              double aThrottleLeverPosition,
                                                              double aSpdBrakeLeverPosition,
                                                              double aSpoilerLeverPosition,
                                                              double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      mTrimRollRight = mRollTrimFactor * mRollRightTrimDeltaT_sec;
      if (mTrimRollRight < -1.0)
      {
         mTrimRollRight = -1.0;
      }
      if (mTrimRollRight > 1.0)
      {
         mTrimRollRight = 1.0;
      }

      aStickRightPosition += mTrimRollRight;
      if (aStickRightPosition < -1.0)
      {
         aStickRightPosition = -1.0;
      }
      if (aStickRightPosition > 1.0)
      {
         aStickRightPosition = 1.0;
      }

      if (mRollControlMapping != nullptr)
      {
         aStickRightPosition = mRollControlMapping->Lookup(aStickRightPosition);
      }

      mStdStickRight = static_cast<float>(aStickRightPosition);

      // Set augmented control input value
      mControlAugmentationStickRight = aStickRightPosition;

      mTrimNoseUp = mPitchTrimFactor * mNoseUpTrimDeltaT_sec;
      if (mTrimNoseUp < -1.0)
      {
         mTrimNoseUp = -1.0;
      }
      if (mTrimNoseUp > 1.0)
      {
         mTrimNoseUp = 1.0;
      }

      aStickAftPosition += mTrimNoseUp;
      if (aStickAftPosition < -1.0)
      {
         aStickAftPosition = -1.0;
      }
      if (aStickAftPosition > 1.0)
      {
         aStickAftPosition = 1.0;
      }

      if (mPitchControlMapping != nullptr)
      {
         aStickAftPosition = mPitchControlMapping->Lookup(aStickAftPosition);
      }

      mStdStickBack = static_cast<float>(aStickAftPosition);

      // Set augmented control input value
      mControlAugmentationStickBack = aStickAftPosition;

      SetThrottleData(aThrottleLeverPosition);

      mStdSpeedBrakesOut = static_cast<float>(aSpdBrakeLeverPosition);
      mStdSpoilersOut    = static_cast<float>(aSpoilerLeverPosition);
      mStdFlapsDown      = static_cast<float>(aFlapsLeverPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::SetManualControlData(double aStickRightPosition,
                                                              double aStickAftPosition,
                                                              double aRudderRightPosition,
                                                              double aThrottleLeverPosition,
                                                              double aSpdBrakeLeverPosition,
                                                              double aSpoilerLeverPosition,
                                                              double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition);

      mTrimYawRight = mYawTrimFactor * mYawRightTrimDeltaT_sec;
      if (mTrimYawRight < -1.0)
      {
         mTrimYawRight = -1.0;
      }
      if (mTrimYawRight > 1.0)
      {
         mTrimYawRight = 1.0;
      }

      aRudderRightPosition += mTrimYawRight;
      if (aRudderRightPosition < -1.0)
      {
         aRudderRightPosition = -1.0;
      }
      if (aRudderRightPosition > 1.0)
      {
         aRudderRightPosition = 1.0;
      }

      if (mYawControlMapping != nullptr)
      {
         aRudderRightPosition = mYawControlMapping->Lookup(aRudderRightPosition);
      }

      mStdRudderRight = static_cast<float>(aRudderRightPosition);

      // Set augmented control input value
      mControlAugmentationRudderRight = aRudderRightPosition;
   }
}

void wsf::six_dof::PointMassPilotObject::SetManualControlData(double aStickRightPosition,
                                                              double aStickAftPosition,
                                                              double aRudderRightPosition,
                                                              double aThrottleLeverPosition,
                                                              double aSpdBrakeLeverPosition,
                                                              double aSpoilerLeverPosition,
                                                              double aFlapsLeverPosition,
                                                              double /*aLandingGearLeverPosition*/)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aRudderRightPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition);

      // mStdLandingGearDown = (float)aLandingGearLeverPosition;
   }
}

void wsf::six_dof::PointMassPilotObject::SetManualControlData(double aStickRightPosition,
                                                              double aStickAftPosition,
                                                              double aRudderRightPosition,
                                                              double aThrottleLeverPosition,
                                                              double aThrustReverserLeverPosition,
                                                              double /*aThrustVectorYawRightPosition*/,
                                                              double /*aThrustVectorPitchUpPosition*/,
                                                              double /*aThrustVectorRollRightPosition*/,
                                                              double aSpdBrakeLeverPosition,
                                                              double aSpoilerLeverPosition,
                                                              double aFlapsLeverPosition,
                                                              double aLandingGearLeverPosition,
                                                              double /*aNoseWheelSteeringRightPosition*/,
                                                              double /*aWheelBrakeLeftPosition*/,
                                                              double /*aWheelBrakeRightPosition*/,
                                                              bool /*aNWS_Enabled*/)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeManualActive())
   {
      SetManualControlData(aStickRightPosition,
                           aStickAftPosition,
                           aRudderRightPosition,
                           aThrottleLeverPosition,
                           aSpdBrakeLeverPosition,
                           aSpoilerLeverPosition,
                           aFlapsLeverPosition,
                           aLandingGearLeverPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                                      double aStickAftPosition,
                                                                      double aThrottleLeverPosition,
                                                                      double aSpdBrakeLeverPosition,
                                                                      double aSpoilerLeverPosition,
                                                                      double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      mTrimRollRight = 0.1 * mRollRightTrimDeltaT_sec;
      if (mTrimRollRight < -1.0)
      {
         mTrimRollRight = -1.0;
      }
      if (mTrimRollRight > 1.0)
      {
         mTrimRollRight = 1.0;
      }

      aStickRightPosition += mTrimRollRight;
      if (aStickRightPosition < -1.0)
      {
         aStickRightPosition = -1.0;
      }
      if (aStickRightPosition > 1.0)
      {
         aStickRightPosition = 1.0;
      }

      if (mRollControlMapping != nullptr)
      {
         aStickRightPosition = mRollControlMapping->Lookup(aStickRightPosition);
      }

      mStdStickRight = static_cast<float>(aStickRightPosition);

      // Set augmented control input value
      mControlAugmentationStickRight = aStickRightPosition;

      mTrimNoseUp = 0.1 * mNoseUpTrimDeltaT_sec;
      if (mTrimNoseUp < -1.0)
      {
         mTrimNoseUp = -1.0;
      }
      if (mTrimNoseUp > 1.0)
      {
         mTrimNoseUp = 1.0;
      }

      aStickAftPosition += mTrimNoseUp;
      if (aStickAftPosition < -1.0)
      {
         aStickAftPosition = -1.0;
      }
      if (aStickAftPosition > 1.0)
      {
         aStickAftPosition = 1.0;
      }

      if (mPitchControlMapping != nullptr)
      {
         aStickAftPosition = mPitchControlMapping->Lookup(aStickAftPosition);
      }

      mStdStickBack = static_cast<float>(aStickAftPosition);

      // Set augmented control input value
      mControlAugmentationStickBack = aStickAftPosition;

      SetThrottleData(aThrottleLeverPosition);

      mStdSpeedBrakesOut = static_cast<float>(aSpdBrakeLeverPosition);
      mStdSpoilersOut    = static_cast<float>(aSpoilerLeverPosition);
      mStdFlapsDown      = static_cast<float>(aFlapsLeverPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                                      double aStickAftPosition,
                                                                      double aRudderRightPosition,
                                                                      double aThrottleLeverPosition,
                                                                      double aSpdBrakeLeverPosition,
                                                                      double aSpoilerLeverPosition,
                                                                      double aFlapsLeverPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition);

      mTrimYawRight = 0.1 * mYawRightTrimDeltaT_sec;
      if (mTrimYawRight < -1.0)
      {
         mTrimYawRight = -1.0;
      }
      if (mTrimYawRight > 1.0)
      {
         mTrimYawRight = 1.0;
      }

      aRudderRightPosition += mTrimYawRight;
      if (aRudderRightPosition < -1.0)
      {
         aRudderRightPosition = -1.0;
      }
      if (aRudderRightPosition > 1.0)
      {
         aRudderRightPosition = 1.0;
      }

      mStdRudderRight = static_cast<float>(aRudderRightPosition);

      // Set augmented control input value
      mControlAugmentationRudderRight = aRudderRightPosition;
   }
}

void wsf::six_dof::PointMassPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                                      double aStickAftPosition,
                                                                      double aRudderRightPosition,
                                                                      double aThrottleLeverPosition,
                                                                      double aSpdBrakeLeverPosition,
                                                                      double aSpoilerLeverPosition,
                                                                      double aFlapsLeverPosition,
                                                                      double /*aLandingGearLeverPosition*/)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aRudderRightPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition);

      // mStdLandingGearDown = (float)aLandingGearLeverPosition;
   }
}

void wsf::six_dof::PointMassPilotObject::SetExternalDirectControlData(double aStickRightPosition,
                                                                      double aStickAftPosition,
                                                                      double aRudderRightPosition,
                                                                      double aThrottleLeverPosition,
                                                                      double aThrustReverserLeverPosition,
                                                                      double /*aThrustVectorYawRightPosition*/,
                                                                      double /*aThrustVectorPitchUpPosition*/,
                                                                      double /*aThrustVectorRollRightPosition*/,
                                                                      double aSpdBrakeLeverPosition,
                                                                      double aSpoilerLeverPosition,
                                                                      double aFlapsLeverPosition,
                                                                      double aLandingGearLeverPosition,
                                                                      double /*aNoseWheelSteeringRightPosition*/,
                                                                      double /*aWheelBrakeLeftPosition*/,
                                                                      double /*aWheelBrakeRightPosition*/,
                                                                      bool /*aNWS_Enabled*/)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (ControlModeExternalDirectActive())
   {
      SetExternalDirectControlData(aStickRightPosition,
                                   aStickAftPosition,
                                   aRudderRightPosition,
                                   aThrottleLeverPosition,
                                   aSpdBrakeLeverPosition,
                                   aSpoilerLeverPosition,
                                   aFlapsLeverPosition,
                                   aLandingGearLeverPosition);
   }
}

double wsf::six_dof::PointMassPilotObject::GetStickBackControllerPosition() const
{
   return static_cast<double>(mStdStickBack);
}

void wsf::six_dof::PointMassPilotObject::SetTestStickBackControllerPosition(double aStickAftPosition)
{
   if (mTestControl)
   {
      mStdStickBack = static_cast<float>(aStickAftPosition);
   }
}

double wsf::six_dof::PointMassPilotObject::GetStickRightControllerPosition() const
{
   return static_cast<double>(mStdStickRight);
}

void wsf::six_dof::PointMassPilotObject::SetTestStickRightControllerPosition(double aStickRightPosition)
{
   if (mTestControl)
   {
      mStdStickRight = static_cast<float>(aStickRightPosition);
   }
}

double wsf::six_dof::PointMassPilotObject::GetRudderRightControllerPosition() const
{
   return static_cast<double>(mStdRudderRight);
}

void wsf::six_dof::PointMassPilotObject::SetTestRudderRightControllerPosition(double aRudderRightPosition)
{
   if (mTestControl)
   {
      mStdRudderRight = static_cast<float>(aRudderRightPosition);
   }
}

double wsf::six_dof::PointMassPilotObject::GetSpeedBrakesControllerPosition() const
{
   return static_cast<double>(mStdSpeedBrakesOut);
}

void wsf::six_dof::PointMassPilotObject::SetTestSpeedBrakesControllerPosition(double aSpeedBrakesPosition)
{
   if (mTestControl)
   {
      mStdSpeedBrakesOut = static_cast<float>(aSpeedBrakesPosition);
   }
}

double wsf::six_dof::PointMassPilotObject::GetFlapsControllerPosition() const
{
   return static_cast<double>(mStdFlapsDown);
}

void wsf::six_dof::PointMassPilotObject::SetTestFlapsControllerPosition(double aFlapsPosition)
{
   if (mTestControl)
   {
      float value = static_cast<float>(aFlapsPosition);
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdFlapsDown = value;
   }
}

void wsf::six_dof::PointMassPilotObject::SetTestSpoilersControllerPosition(double aSpoilersPosition)
{
   if (mTestControl)
   {
      float value = static_cast<float>(aSpoilersPosition);
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdSpeedBrakesOut = value;
   }
}

void wsf::six_dof::PointMassPilotObject::SetTestLandingGearControllerPosition(double /*aLandingGearPosition*/)
{
   return; // No landing gear on MRM
}

double wsf::six_dof::PointMassPilotObject::GetThrottleMilitaryControllerPosition() const
{
   return mStdThrottleMil;
}

double wsf::six_dof::PointMassPilotObject::GetThrottleAfterburnerControllerPosition() const
{
   return mStdThrottleAB;
}

double wsf::six_dof::PointMassPilotObject::GetThrottleControllerPosition() const
{
   double throttleMil = 0;
   double throttleAB  = 0;

   throttleMil = mStdThrottleMil;
   throttleAB  = mStdThrottleAB;

   // The throttle position has 1.0=MIL and 2.0=AB

   double throttle = 0;

   if (throttleAB > 0)
   {
      throttle = 1.0 + throttleAB;
   }
   else
   {
      throttle = throttleMil;
   }

   return throttle;
}

void wsf::six_dof::PointMassPilotObject::SetDirectStickBackInput(double aStickBackPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideStickBack)
   {
      aStickBackPosition = UtMath::Limit(aStickBackPosition, 1.0);

      if (mPitchControlMapping != nullptr)
      {
         aStickBackPosition = mPitchControlMapping->Lookup(aStickBackPosition);
      }

      mStdStickBack = static_cast<float>(aStickBackPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::SetDirectStickRightInput(double aStickRightPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideStickRight)
   {
      aStickRightPosition = UtMath::Limit(aStickRightPosition, 1.0);

      if (mRollControlMapping != nullptr)
      {
         aStickRightPosition = mRollControlMapping->Lookup(aStickRightPosition);
      }

      mStdStickRight = static_cast<float>(aStickRightPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::SetDirectRudderRightInput(double aRudderRightPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideRudderRight)
   {
      aRudderRightPosition = UtMath::Limit(aRudderRightPosition, 1.0);

      if (mYawControlMapping != nullptr)
      {
         aRudderRightPosition = mYawControlMapping->Lookup(aRudderRightPosition);
      }

      mStdRudderRight = static_cast<float>(aRudderRightPosition);
   }
}

void wsf::six_dof::PointMassPilotObject::OpenSpeedBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      mStdSpeedBrakesOut = 1.0f;
   }
}

void wsf::six_dof::PointMassPilotObject::CloseSpeedBrake()
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      mStdSpeedBrakesOut = 0.0f;
   }
}

void wsf::six_dof::PointMassPilotObject::SetDirectSpeedBrakesInput(double aValue)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   if (mControlOverrideSpeedBrakes)
   {
      float value = (float)aValue;
      if (value < 0.0)
      {
         value = 0.0;
      }
      else if (value > 1.0)
      {
         value = 1.0;
      }
      mStdSpeedBrakesOut = value;
   }
}

void wsf::six_dof::PointMassPilotObject::SetLandingGearControlPosition(double /*aPosition*/)
{
   return; // No landing gear on MRM
}

void wsf::six_dof::PointMassPilotObject::SetFlapsControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   float value = (float)aPosition;
   if (value < 0.0)
   {
      value = 0.0;
   }
   else if (value > 1.0)
   {
      value = 1.0;
   }
   mStdFlapsDown = value;
}

void wsf::six_dof::PointMassPilotObject::SetSpoilersControlPosition(double aPosition)
{
   if (mIsDestroyed)
   {
      return;
   } // Do nothing if destroyed

   float value = (float)aPosition;
   if (value < 0.0)
   {
      value = 0.0;
   }
   else if (value > 1.0)
   {
      value = 1.0;
   }
   mStdSpoilersOut = value;
}

void wsf::six_dof::PointMassPilotObject::SetThrustReverserControlPosition(double aPosition)
{
   return; // No ground operations on PM6D
}

double wsf::six_dof::PointMassPilotObject::GetLandingGearControllerPosition() const
{
   // return mStdLandingGearDown; // No ground operations on PM6D

   return 0.0;
}

double wsf::six_dof::PointMassPilotObject::GetSpoilersControllerPosition() const
{
   return mStdSpoilersOut;
}

void wsf::six_dof::PointMassPilotObject::ZeroDisabledControlData()
{
   if (ControlModeDisabledActive())
   {
      mStdStickRight     = 0.0f;
      mStdStickBack      = 0.0f;
      mStdRudderRight    = 0.0f;
      mStdThrottleMil    = 0.0f;
      mStdThrottleAB     = 0.0f;
      mStdSpeedBrakesOut = 0.0f;
      mStdSpoilersOut    = 0.0f;
      mStdFlapsDown      = 0.0f;

      // mStdThrustVectoringYaw = 0.0f;
      // mStdThrustVectoringPitch = 0.0f;
      // mStdThrustVectoringRoll = 0.0f;
      // mStdLandingGearDown = 0.0f;
      // mStdNoseWheelSteering = 0.0f;
      // mStdNWS_Steering = 0.0f;
      // mStdNWS_Enabled = false;
      // mStdWheelBrakeLeft = 0.0f;
      // mStdWheelBrakeRight = 0.0f;
   }
}

void wsf::six_dof::PointMassPilotObject::InheritParentControlData()
{
   Mover* vehicle = GetParentVehicle();

   if (vehicle)
   {
      Mover* vehicleParent = vehicle->GetParentVehicle();

      if (vehicleParent)
      {
         mStdStickRight  = static_cast<float>(vehicleParent->GetStickRightInput());
         mStdStickBack   = static_cast<float>(vehicleParent->GetStickBackInput());
         mStdRudderRight = static_cast<float>(vehicleParent->GetRudderRightInput());

         double throttleInput    = vehicleParent->GetThrottleInput();
         double throttleInputMil = UtMath::Limit(throttleInput, 0.0, 1.0);
         double throttleInputAB  = UtMath::Limit(throttleInput - 1.0, 0.0, 1.0);

         mStdThrottleMil = static_cast<float>(throttleInputMil);
         mStdThrottleAB  = static_cast<float>(throttleInputAB);

         mStdSpeedBrakesOut = static_cast<float>(vehicleParent->GetSpeedBrakeControlPosition());
         mStdSpoilersOut    = static_cast<float>(vehicleParent->GetSpoilersLeverPosition());
         mStdFlapsDown      = static_cast<float>(vehicleParent->GetFlapsLeverPosition());
      }
   }
}
