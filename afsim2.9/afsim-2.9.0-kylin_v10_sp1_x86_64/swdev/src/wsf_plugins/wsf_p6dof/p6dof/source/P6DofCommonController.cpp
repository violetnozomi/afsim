// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofCommonController.hpp"

#include <math.h>

#include "P6DofAtmosphere.hpp"
#include "P6DofAutopilotAction.hpp"
#include "P6DofGroundReactionPoint.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofPID.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofCommonController::P6DofCommonController(const std::string& aConfigFile)
   : mCurrentActivityPtr(nullptr)
   , mCombinedWheelBraking(0)
   , mWheelBrakingLeft(0)
   , mWheelBrakingRight(0)
   , mAchievedWaypoint(false)
   , mStartTurnHdg_rad(0)
   , mTurnRadius_m(0)
   , mTurnAngle_rad(0)
   , mTurnCW(true)
   , mTurning(false)
   , mTurnRefPoint()
   , mAimHeading_rad(0)
   , mTurnCenterLat(0)
   , mTurnCenterLon(0)
   , mExternalGearControlValue(0)
   , mExternalFlapsControlValue(0)
   , mExternalSpoilersControlValue(0)
   , mExternalSpeedBrakesControlValue(0)
   , mExternalAfterburnerControlValue(0)
   , mExternalLftWheelBrakeControlValue(0)
   , mExternalRgtWheelBrakeControlValue(0)
   , mVerticalMiddleLoopFactor(10)
   , mLateralMiddleLoopFactor(10)
   , mSpeedMiddleLoopFactor(10)
   , mVerticalOuterLoopFactor(5)
   , mLateralOuterLoopFactor(5)
   , mSpeedOuterLoopFactor(5)
   , mVerticalMiddleLoopCount(0)
   , mLateralMiddleLoopCount(0)
   , mSpeedMiddleLoopCount(0)
   , mVerticalOuterLoopCount(0)
   , mLateralOuterLoopCount(0)
   , mSpeedOuterLoopCount(0)
   , mExecuteVerticalOuterLoop(true)
   , mExecuteLateralOuterLoop(true)
   , mExecuteSpeedOuterLoop(true)
   , mExecuteVerticalMiddleLoop(true)
   , mExecuteLateralMiddleLoop(true)
   , mExecuteSpeedMiddleLoop(true)
   , mControlMethod(P6DOF::Control::Undefined)
   , mParentObjectPtr(nullptr)
   , mLastCommandedVertRate_fpm(0)
   , mLastCommandedAlpha_deg(0)
   , mLastCommandedYawRate_dps(0)
   , mLastCommandedBeta_deg(0)
   , mLastComandedBankAngle_deg(0)
   , mLastCommandedRollRate_dps(0)
   , mLastCommandedSpeed_fps(0)
   , mLastCommandedForwardAccel_g(0)
   , mTaxiModeActive(false)
   , mLimitedBeta_deg(0)
   , mLimitedMinAlpha_deg(0)
   , mLimitedMaxAlpha_deg(0)
   , mIntegratedDeltaYaw_deg(0)
   , mIntegratedDeltaPitch_deg(0)
   , mIntegratedDeltaRoll_deg(0)
   , mControlAugmentationModeActive(false)
   , mMinimumTaxiTurnRadius_ft(50.0)
   , mDesiredTaxiTurnRadius_ft(50.0)
   , mCurrentGBias_g(1)
   , mCurrentGBiasAlpha_deg(0)
   , mCLMaxMachTablePtr(nullptr)
   , mCLMinMachTablePtr(nullptr)
   , mAlphaMaxMachTablePtr(nullptr)
   , mAlphaMinMachTablePtr(nullptr)
   , mStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mEffectiveCLVsMachAlphaTablePtr(nullptr)
   , mAlphaVsMachCLTablePtr(nullptr)
   , mUseSimpleYawDamper(false)
   , mAtmosphere(nullptr)
{
   // Note: mCurrentActivityPtr should be null until set from external source

   mControlOutputsP6Dof.stickBack           = 0;
   mControlOutputsP6Dof.stickRight          = 0;
   mControlOutputsP6Dof.rudderRight         = 0;
   mControlOutputsP6Dof.throttleMilitary    = 0;
   mControlOutputsP6Dof.throttleAfterburner = 0;
   mControlOutputsP6Dof.thrustVectorYaw     = 0;
   mControlOutputsP6Dof.thrustVectorPitch   = 0;
   mControlOutputsP6Dof.thrustVectorRoll    = 0;
   mControlOutputsP6Dof.speedBrake          = 0;
   mControlOutputsP6Dof.noseWheelSteering   = 0;
   mControlOutputsP6Dof.nwsSteering         = 0;
   mControlOutputsP6Dof.wheelBrakeLeft      = 0;
   mControlOutputsP6Dof.wheelBrakeRight     = 0;

   mTurnRefPoint.SetLat(0.0);
   mTurnRefPoint.SetLon(0.0);
   mTurnRefPoint.SetAlt(0.0);

   if (aConfigFile != "")
   {
      LoadConfigFile(aConfigFile);
   }
}

P6DofCommonController::P6DofCommonController(UtInput& aInput, const std::string& aCommand)
   : mCurrentActivityPtr(nullptr)
   , mCombinedWheelBraking(0)
   , mWheelBrakingLeft(0)
   , mWheelBrakingRight(0)
   , mAchievedWaypoint(false)
   , mStartTurnHdg_rad(0)
   , mTurnRadius_m(0)
   , mTurnAngle_rad(0)
   , mTurnCW(true)
   , mTurning(false)
   , mTurnRefPoint()
   , mAimHeading_rad(0)
   , mTurnCenterLat(0)
   , mTurnCenterLon(0)
   , mExternalGearControlValue(0)
   , mExternalFlapsControlValue(0)
   , mExternalSpoilersControlValue(0)
   , mExternalSpeedBrakesControlValue(0)
   , mExternalAfterburnerControlValue(0)
   , mExternalLftWheelBrakeControlValue(0)
   , mExternalRgtWheelBrakeControlValue(0)
   , mVerticalMiddleLoopFactor(10)
   , mLateralMiddleLoopFactor(10)
   , mSpeedMiddleLoopFactor(10)
   , mVerticalOuterLoopFactor(5)
   , mLateralOuterLoopFactor(5)
   , mSpeedOuterLoopFactor(5)
   , mVerticalMiddleLoopCount(0)
   , mLateralMiddleLoopCount(0)
   , mSpeedMiddleLoopCount(0)
   , mVerticalOuterLoopCount(0)
   , mLateralOuterLoopCount(0)
   , mSpeedOuterLoopCount(0)
   , mExecuteVerticalOuterLoop(true)
   , mExecuteLateralOuterLoop(true)
   , mExecuteSpeedOuterLoop(true)
   , mExecuteVerticalMiddleLoop(true)
   , mExecuteLateralMiddleLoop(true)
   , mExecuteSpeedMiddleLoop(true)
   , mControlMethod(P6DOF::Control::Undefined)
   , mParentObjectPtr(nullptr)
   , mLastCommandedVertRate_fpm(0)
   , mLastCommandedAlpha_deg(0)
   , mLastCommandedYawRate_dps(0)
   , mLastCommandedBeta_deg(0)
   , mLastComandedBankAngle_deg(0)
   , mLastCommandedRollRate_dps(0)
   , mLastCommandedSpeed_fps(0)
   , mLastCommandedForwardAccel_g(0)
   , mTaxiModeActive(false)
   , mLimitedBeta_deg(0)
   , mLimitedMinAlpha_deg(0)
   , mLimitedMaxAlpha_deg(0)
   , mIntegratedDeltaYaw_deg(0)
   , mIntegratedDeltaPitch_deg(0)
   , mIntegratedDeltaRoll_deg(0)
   , mControlAugmentationModeActive(false)
   , mMinimumTaxiTurnRadius_ft(50.0)
   , mDesiredTaxiTurnRadius_ft(50.0)
   , mCurrentGBias_g(1)
   , mCurrentGBiasAlpha_deg(0)
   , mCLMaxMachTablePtr(nullptr)
   , mCLMinMachTablePtr(nullptr)
   , mAlphaMaxMachTablePtr(nullptr)
   , mAlphaMinMachTablePtr(nullptr)
   , mStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr(nullptr)
   , mEffectiveCLVsMachAlphaTablePtr(nullptr)
   , mAlphaVsMachCLTablePtr(nullptr)
   , mUseSimpleYawDamper(false)
   , mAtmosphere(nullptr)
   , mUseLegacyBeta(true)
{
   // Note: mCurrentActivityPtr should be null until set from external source

   mControlOutputsP6Dof.stickBack           = 0;
   mControlOutputsP6Dof.stickRight          = 0;
   mControlOutputsP6Dof.rudderRight         = 0;
   mControlOutputsP6Dof.throttleMilitary    = 0;
   mControlOutputsP6Dof.throttleAfterburner = 0;
   mControlOutputsP6Dof.thrustVectorYaw     = 0;
   mControlOutputsP6Dof.thrustVectorPitch   = 0;
   mControlOutputsP6Dof.thrustVectorRoll    = 0;
   mControlOutputsP6Dof.speedBrake          = 0;
   mControlOutputsP6Dof.noseWheelSteering   = 0;
   mControlOutputsP6Dof.nwsSteering         = 0;
   mControlOutputsP6Dof.wheelBrakeLeft      = 0;
   mControlOutputsP6Dof.wheelBrakeRight     = 0;

   mTurnRefPoint.SetLat(0.0);
   mTurnRefPoint.SetLon(0.0);
   mTurnRefPoint.SetAlt(0.0);

   ProcessInputCommand(aInput, aCommand);
}

P6DofCommonController::P6DofCommonController(const P6DofCommonController& aSrc)
   : mCurrentActivityPtr(nullptr)
   , mState(aSrc.mState)
   , mControlOutputsP6Dof(aSrc.mControlOutputsP6Dof)
   , mCombinedWheelBraking(aSrc.mCombinedWheelBraking)
   , mWheelBrakingLeft(aSrc.mWheelBrakingLeft)
   , mWheelBrakingRight(aSrc.mWheelBrakingRight)
   , mAchievedWaypoint(aSrc.mAchievedWaypoint)
   , mStartTurnHdg_rad(aSrc.mStartTurnHdg_rad)
   , mTurnRadius_m(aSrc.mTurnRadius_m)
   , mTurnAngle_rad(aSrc.mTurnAngle_rad)
   , mTurnCW(aSrc.mTurnCW)
   , mTurning(aSrc.mTurning)
   , mTurnRefPoint(aSrc.mTurnRefPoint)
   , mAimHeading_rad(aSrc.mAimHeading_rad)
   , mTurnCenterLat(aSrc.mTurnCenterLat)
   , mTurnCenterLon(aSrc.mTurnCenterLon)
   , mExternalGearControlValue(aSrc.mExternalGearControlValue)
   , mExternalFlapsControlValue(aSrc.mExternalFlapsControlValue)
   , mExternalSpoilersControlValue(aSrc.mExternalSpoilersControlValue)
   , mExternalSpeedBrakesControlValue(aSrc.mExternalSpeedBrakesControlValue)
   , mExternalAfterburnerControlValue(aSrc.mExternalAfterburnerControlValue)
   , mExternalLftWheelBrakeControlValue(aSrc.mExternalLftWheelBrakeControlValue)
   , mExternalRgtWheelBrakeControlValue(aSrc.mExternalRgtWheelBrakeControlValue)
   , mVerticalMiddleLoopFactor(aSrc.mVerticalMiddleLoopFactor)
   , mLateralMiddleLoopFactor(aSrc.mLateralMiddleLoopFactor)
   , mSpeedMiddleLoopFactor(aSrc.mSpeedMiddleLoopFactor)
   , mVerticalOuterLoopFactor(aSrc.mVerticalOuterLoopFactor)
   , mLateralOuterLoopFactor(aSrc.mLateralOuterLoopFactor)
   , mSpeedOuterLoopFactor(aSrc.mSpeedOuterLoopFactor)
   , mVerticalMiddleLoopCount(aSrc.mVerticalMiddleLoopCount)
   , mLateralMiddleLoopCount(aSrc.mLateralMiddleLoopCount)
   , mSpeedMiddleLoopCount(aSrc.mSpeedMiddleLoopCount)
   , mVerticalOuterLoopCount(aSrc.mVerticalOuterLoopCount)
   , mLateralOuterLoopCount(aSrc.mLateralOuterLoopCount)
   , mSpeedOuterLoopCount(aSrc.mSpeedOuterLoopCount)
   , mExecuteVerticalOuterLoop(aSrc.mExecuteVerticalOuterLoop)
   , mExecuteLateralOuterLoop(aSrc.mExecuteLateralOuterLoop)
   , mExecuteSpeedOuterLoop(aSrc.mExecuteSpeedOuterLoop)
   , mExecuteVerticalMiddleLoop(aSrc.mExecuteVerticalMiddleLoop)
   , mExecuteLateralMiddleLoop(aSrc.mExecuteLateralMiddleLoop)
   , mExecuteSpeedMiddleLoop(aSrc.mExecuteSpeedMiddleLoop)
   , mControlMethod(aSrc.mControlMethod)
   , mParentObjectPtr(nullptr)
   , mLastCommandedVertRate_fpm(aSrc.mLastCommandedVertRate_fpm)
   , mLastCommandedAlpha_deg(aSrc.mLastCommandedAlpha_deg)
   , mLastCommandedYawRate_dps(aSrc.mLastCommandedYawRate_dps)
   , mLastCommandedBeta_deg(aSrc.mLastCommandedBeta_deg)
   , mLastComandedBankAngle_deg(aSrc.mLastComandedBankAngle_deg)
   , mLastCommandedRollRate_dps(aSrc.mLastCommandedRollRate_dps)
   , mLastCommandedSpeed_fps(aSrc.mLastCommandedSpeed_fps)
   , mLastCommandedForwardAccel_g(aSrc.mLastCommandedForwardAccel_g)
   , mAlphaPID(aSrc.mAlphaPID)
   , mVertSpeedPID(aSrc.mVertSpeedPID)
   , mPitchAnglePID(aSrc.mPitchAnglePID)
   , mPitchRatePID(aSrc.mPitchRatePID)
   , mFltpathAnglePID(aSrc.mFltpathAnglePID)
   , mDeltaPitchPID(aSrc.mDeltaPitchPID)
   , mAltitudePID(aSrc.mAltitudePID)
   , mBetaPID(aSrc.mBetaPID)
   , mYawRatePID(aSrc.mYawRatePID)
   , mYawHeadingPID(aSrc.mYawHeadingPID)
   , mTaxiHeadingPID(aSrc.mTaxiHeadingPID)
   , mRollRatePID(aSrc.mRollRatePID)
   , mDeltaRollPID(aSrc.mDeltaRollPID)
   , mBankAnglePID(aSrc.mBankAnglePID)
   , mRollHeadingPID(aSrc.mRollHeadingPID)
   , mForwardAccelPID(aSrc.mForwardAccelPID)
   , mSpeedPID(aSrc.mSpeedPID)
   , mTaxiForwardAccelPID(aSrc.mTaxiForwardAccelPID)
   , mTaxiSpeedPID(aSrc.mTaxiSpeedPID)
   , mTaxiYawRatePID(aSrc.mTaxiYawRatePID)
   , mDefaultLimitsAndSettings(aSrc.mDefaultLimitsAndSettings)
   , mCurrentLimitsAndSettings(aSrc.mCurrentLimitsAndSettings)
   , mTaxiModeActive(aSrc.mTaxiModeActive)
   , mLimitedBeta_deg(aSrc.mLimitedBeta_deg)
   , mLimitedMinAlpha_deg(aSrc.mLimitedMinAlpha_deg)
   , mLimitedMaxAlpha_deg(aSrc.mLimitedMaxAlpha_deg)
   , mIntegratedDeltaYaw_deg(aSrc.mIntegratedDeltaYaw_deg)
   , mIntegratedDeltaPitch_deg(aSrc.mIntegratedDeltaPitch_deg)
   , mIntegratedDeltaRoll_deg(aSrc.mIntegratedDeltaRoll_deg)
   , mControlAugmentationModeActive(aSrc.mControlAugmentationModeActive)
   , mMinimumTaxiTurnRadius_ft(aSrc.mMinimumTaxiTurnRadius_ft)
   , mDesiredTaxiTurnRadius_ft(aSrc.mDesiredTaxiTurnRadius_ft)
   , mCurrentGBias_g(aSrc.mCurrentGBias_g)
   , mCurrentGBiasAlpha_deg(aSrc.mCurrentGBiasAlpha_deg)
   , mCLMaxMachTablePtr(aSrc.mCLMaxMachTablePtr)
   , mCLMinMachTablePtr(aSrc.mCLMinMachTablePtr)
   , mAlphaMaxMachTablePtr(aSrc.mAlphaMaxMachTablePtr)
   , mAlphaMinMachTablePtr(aSrc.mAlphaMinMachTablePtr)
   , mStickForZeroMomentVsMachAlphaTablePtr(aSrc.mStickForZeroMomentVsMachAlphaTablePtr)
   , mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr(aSrc.mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
   , mEffectiveCLVsMachAlphaTablePtr(aSrc.mEffectiveCLVsMachAlphaTablePtr)
   , mAlphaVsMachCLTablePtr(aSrc.mAlphaVsMachCLTablePtr)
   , mUseSimpleYawDamper(aSrc.mUseSimpleYawDamper)
   , mAtmosphere(aSrc.mAtmosphere)
   , mUseLegacyBeta(aSrc.mUseLegacyBeta)
{
   // Note: mCurrentActivityPtr should be null until set from external source

   // Point to the same action as the source
   mCurrentActivityPtr = aSrc.mCurrentActivityPtr;
}

P6DofCommonController* P6DofCommonController::Clone() const
{
   return new P6DofCommonController(*this);
}

void P6DofCommonController::Update(P6DofVehicle*        aParentObject,
                                   P6DofKinematicState& aState,
                                   sAutopilotControls&  aControls,
                                   double               aDT)
{
   mParentObjectPtr = aParentObject;

   // Set the atm pointer, if not yet initialized
   if (mAtmosphere == nullptr)
   {
      mAtmosphere = mParentObjectPtr->GetScenario()->GetAtmosphere();
   }

   // Set the internal vehicle state
   mState = aState;

   CalcAlphaBetaGLimits();

   // We need to determine if the outer/middle loops should execute this frame

   // Check for middle loop execution ...
   mExecuteVerticalMiddleLoop = false;
   mVerticalMiddleLoopCount++;
   if (mVerticalMiddleLoopFactor <= mVerticalMiddleLoopCount)
   {
      mVerticalMiddleLoopCount   = 0;
      mExecuteVerticalMiddleLoop = true;
   }

   mExecuteLateralMiddleLoop = false;
   mLateralMiddleLoopCount++;
   if (mLateralMiddleLoopFactor <= mLateralMiddleLoopCount)
   {
      mLateralMiddleLoopCount   = 0;
      mExecuteLateralMiddleLoop = true;
   }

   mExecuteSpeedMiddleLoop = false;
   mSpeedMiddleLoopCount++;
   if (mSpeedMiddleLoopFactor <= mSpeedMiddleLoopCount)
   {
      mSpeedMiddleLoopCount   = 0;
      mExecuteSpeedMiddleLoop = true;
   }

   // Check for outer loop execution ...
   mExecuteVerticalOuterLoop = false;
   if (mVerticalMiddleLoopCount == 0)
   {
      mVerticalOuterLoopCount++;
      if (mVerticalOuterLoopFactor <= mVerticalOuterLoopCount)
      {
         mVerticalOuterLoopCount   = 0;
         mExecuteVerticalOuterLoop = true;
      }
   }

   mExecuteLateralOuterLoop = false;
   if (mLateralMiddleLoopCount == 0)
   {
      mLateralOuterLoopCount++;
      if (mLateralOuterLoopFactor <= mLateralOuterLoopCount)
      {
         mLateralOuterLoopCount   = 0;
         mExecuteLateralOuterLoop = true;
      }
   }

   mExecuteSpeedOuterLoop = false;
   if (mSpeedMiddleLoopCount == 0)
   {
      mSpeedOuterLoopCount++;
      if (mSpeedOuterLoopFactor <= mSpeedOuterLoopCount)
      {
         mSpeedOuterLoopCount   = 0;
         mExecuteSpeedOuterLoop = true;
      }
   }

   // Set the controlling value for the pids
   double dynPressure_psf = mParentObjectPtr->GetDynamicPressure_psf();
   SetControllingValueForAllPIDs(dynPressure_psf);

   // Reset the temp braking values
   mCombinedWheelBraking = 0.0f;
   mWheelBrakingLeft     = 0.0f;
   mWheelBrakingRight    = 0.0f;

   // To determine our primary control action, we determine what to do based on
   // mControlMethod (namely, bank-to-turn or yaw-to-turn) along with the
   // lateral and vertical channel commands. Previous autopilot versions processed
   // the lateral and vertical channels separately -- that approach had many shortcomings.
   // By considering the possible relationship between lateral and vertical commands,
   // a proper control response is possible. In particular, a split-s maneuver is only
   // possible with consideration of both lateral and vertical commands.

   if ((mControlMethod == P6DOF::Control::BankToTurnNoYaw) || (mControlMethod == P6DOF::Control::BankToTurnWithYaw))
   {
      UpdateBankToTurn(aControls, aDT);
   }
   else if ((mControlMethod == P6DOF::Control::YawToTurnNoRoll) || (mControlMethod == P6DOF::Control::YawToTurnRollRate) ||
            (mControlMethod == P6DOF::Control::YawToTurnZeroBank))
   {
      UpdateYawToTurn(aControls, aDT);
   }
   else
   {
      // Control::Undefined (no control)
   }

   // Set wheel brakes based on the temp braking values

   // Check for any significant differential braking
   if ((mWheelBrakingLeft > 0.01) || (mWheelBrakingRight > 0.01))
   {
      // Differential braking is present
      mControlOutputsP6Dof.wheelBrakeLeft  = mWheelBrakingLeft + 0.8f * mCombinedWheelBraking;
      mControlOutputsP6Dof.wheelBrakeRight = mWheelBrakingRight + 0.8f * mCombinedWheelBraking;
   }
   else
   {
      // No differential braking is present
      mControlOutputsP6Dof.wheelBrakeLeft  = mCombinedWheelBraking;
      mControlOutputsP6Dof.wheelBrakeRight = mCombinedWheelBraking;
   }

   // Future Improvement -- SAS control input could be summed into control stream at this point

   // Enforce control limits
   EnforceControlLimits();

   // Return the current controls
   aControls = mControlOutputsP6Dof;
}

void P6DofCommonController::UpdateBankToTurn(sAutopilotControls& aControls, double aDT)
{
   // The bank-to-turn control method is typically more complex than the yaw-to-turn method.
   // In some cases, it must consider both the lateral and vertical channels together in
   // order to produce the best control response.

   // Process the horizontal/heading channel,
   // plus any requested yaw stabilization
   ProcessLaternalNavChannelsBankToTurn(aDT);

   // Process the vertical/altitude channel
   ProcessVerticalNavChannelBankToTurn(aDT);

   // Process the speed channel
   ProcessSpeedChannelBankToTurn(aDT);

   // Return the current controls
   aControls = mControlOutputsP6Dof;
}

void P6DofCommonController::UpdateYawToTurn(sAutopilotControls& aControls, double aDT)
{
   // The yaw-to-turn control method is simpler in many respects than the bank-to-turn.
   // Specifically, it can generally handle the lateral and vertical channels separately.

   // Process the horizontal/heading channel,
   // plus any requested roll stabilization
   ProcessLaternalNavChannelsYawToTurn(aDT);

   // Process the vertical/altitude channel
   ProcessVerticalNavChannelYawToTurn(aDT);

   // Process the speed channel
   ProcessSpeedChannelYawToTurn(aDT);

   // Return the current controls
   aControls = mControlOutputsP6Dof;
}

void P6DofCommonController::EnforceControlLimits()
{
   EnforceSingleControlLimit(mControlOutputsP6Dof.stickBack, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.stickRight, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.rudderRight, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.throttleMilitary, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.throttleAfterburner, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.speedBrake, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.noseWheelSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.nwsSteering, -1.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.wheelBrakeLeft, 0.0, 1.0);
   EnforceSingleControlLimit(mControlOutputsP6Dof.wheelBrakeRight, 0.0, 1.0);
}

void P6DofCommonController::EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue)
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

void P6DofCommonController::SetCurrentActivity(P6DofAutopilotAction* aAPActivity)
{
   mCurrentActivityPtr = aAPActivity;

   // If we achieved the waypoint, we do not clear the turning flag
   if (mAchievedWaypoint)
   {
      // Clear the flag
      mAchievedWaypoint = false;
   }
   else
   {
      // Since we did not achieve the waypoint
      mTurning = false;
   }
}

void P6DofCommonController::SetTaxiMode(bool aTaxiModeActive)
{
   mTaxiModeActive = aTaxiModeActive;
}

bool P6DofCommonController::GetTaxiMode() const
{
   return mTaxiModeActive;
}

void P6DofCommonController::ProcessLaternalNavChannelsBankToTurn(double aDT)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case P6DOF::Lateral::Waypoint:
         ProcessLateralNavMode_Taxi_Waypoint(aDT);
         break;

         // case P6DOF::Lateral::Taxi_Waypoint:
         //    ProcessLateralNavMode_Taxi_Waypoint(aDT);
         //    break;

      case P6DOF::Lateral::Heading:
         ProcessLateralNavMode_Taxi_Heading(aDT);
         break;

         // case P6DOF::Lateral::Taxi_Heading:
         //    ProcessLateralNavMode_Taxi_Heading(aDT);
         //    break;

      case P6DOF::Lateral::YawRate:
         ProcessLateralNavMode_Taxi_YawRate(aDT);
         break;

         // case P6DOF::Lateral::Taxi_YawRate:
         //    ProcessLateralNavMode_Taxi_YawRate(aDT);
         //    break;

      default:
         ProcessLateralNavMode_NoControl();
         break;
      }
   }
   else
   {
      bool stabilizingChannelIsControlled = false;

      // Process the navigational channel
      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case P6DOF::Lateral::Waypoint:
         ProcessLateralNavMode_RollWaypoint(aDT);
         break;
      case P6DOF::Lateral::Heading:
         ProcessLateralNavMode_RollHeading(aDT);
         break;
      case P6DOF::Lateral::Point:
         ProcessLateralNavMode_RollPoint(aDT);
         break;
      case P6DOF::Lateral::RollRate:
         ProcessLateralNavMode_RollRate(aDT);
         break;
      case P6DOF::Lateral::Bank:
         ProcessLateralNavMode_Bank(aDT);
         break;
      case P6DOF::Lateral::DeltaRoll:
         ProcessLateralNavMode_DeltaRoll(aDT);
         break;
      // Yaw cases here are mostly vestigial, but are
      // still needed to tune stabilizing channels
      case P6DOF::Lateral::YawGLoad:
         ProcessLateralNavMode_YawGLoad(aDT);
         stabilizingChannelIsControlled = true;
         break;
      case P6DOF::Lateral::YawRate:
         ProcessLateralNavMode_YawRate(aDT);
         stabilizingChannelIsControlled = true;
         break;
      case P6DOF::Lateral::Beta:
         ProcessLateralNavMode_Beta(aDT);
         stabilizingChannelIsControlled = true;
         break;
      default:
         ProcessLateralNavMode_NoRollControl();
         break;
      }

      // Process the stabilizing channel, if requested
      if (!stabilizingChannelIsControlled)
      {
         switch (mCurrentActivityPtr->GetStabilizingChannelMode())
         {
         case P6DOF::Lateral::YawGLoad:
            ProcessLateralNavMode_YawGLoad(aDT);
            break;
         case P6DOF::Lateral::YawRate:
            ProcessLateralNavMode_YawRate(aDT);
            break;
         case P6DOF::Lateral::Beta:
            ProcessLateralNavMode_Beta(aDT);
            break;
         case P6DOF::Lateral::Undefined:
            // In lieu of an explicit stabilizing control, use the control type
            switch (mControlMethod)
            {
            case P6DOF::Control::BankToTurnWithYaw:
               ProcessStandardLateralNavMode_Beta(0.0, aDT);
               break;
            case P6DOF::Control::BankToTurnNoYaw:
            default:
               ProcessLateralNavMode_NoYawControl();
               break;
            }
            break;
         default:
            ProcessLateralNavMode_NoYawControl();
            break;
         }
      }
   }
}

void P6DofCommonController::ProcessVerticalNavChannelBankToTurn(double aDT)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
      switch (mCurrentActivityPtr->GetVerticalChannelMode())
      {
      case P6DOF::Vertical::Waypoint:
         ProcessVerticalNavMode_Waypoint(aDT);
         break;
      case P6DOF::Vertical::Altitude:
         ProcessVerticalNavMode_Altitude(aDT);
         break;
      case P6DOF::Vertical::VertSpeed:
         ProcessVerticalNavMode_VertSpeed(aDT);
         break;
      case P6DOF::Vertical::Point:
         ProcessVerticalNavMode_Point(aDT);
         break;
      case P6DOF::Vertical::PitchGLoad:
         ProcessVerticalNavMode_PitchGLoad(aDT);
         break;
      case P6DOF::Vertical::PitchAng:
         ProcessVerticalNavMode_PitchAng(aDT);
         break;
      case P6DOF::Vertical::PitchRate:
         ProcessVerticalNavMode_PitchRate(aDT);
         break;
      case P6DOF::Vertical::FltPathAng:
         ProcessVerticalNavMode_FltPathAng(aDT);
         break;
      case P6DOF::Vertical::DeltaPitch:
         ProcessVerticalNavMode_DeltaPitch(aDT);
         break;
      case P6DOF::Vertical::Alpha:
         ProcessVerticalNavMode_Alpha(aDT);
         break;
      default:
         ProcessVerticalNavMode_NoControl(aDT);
         break;
      }
   }
   else
   {
      switch (mCurrentActivityPtr->GetVerticalChannelMode())
      {
      case P6DOF::Vertical::Waypoint:
         ProcessVerticalNavMode_Waypoint(aDT);
         break;
      case P6DOF::Vertical::Altitude:
         ProcessVerticalNavMode_Altitude(aDT);
         break;
      case P6DOF::Vertical::VertSpeed:
         ProcessVerticalNavMode_VertSpeed(aDT);
         break;
      case P6DOF::Vertical::Point:
         ProcessVerticalNavMode_Point(aDT);
         break;
      case P6DOF::Vertical::PitchGLoad:
         ProcessVerticalNavMode_PitchGLoad(aDT);
         break;
      case P6DOF::Vertical::PitchAng:
         ProcessVerticalNavMode_PitchAng(aDT);
         break;
      case P6DOF::Vertical::PitchRate:
         ProcessVerticalNavMode_PitchRate(aDT);
         break;
      case P6DOF::Vertical::FltPathAng:
         ProcessVerticalNavMode_FltPathAng(aDT);
         break;
      case P6DOF::Vertical::DeltaPitch:
         ProcessVerticalNavMode_DeltaPitch(aDT);
         break;
      case P6DOF::Vertical::Alpha:
         ProcessVerticalNavMode_Alpha(aDT);
         break;
      default:
         ProcessVerticalNavMode_NoControl(aDT);
         break;
      }
   }
}

void P6DofCommonController::ProcessSpeedChannelBankToTurn(double aDT)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

   if (mTaxiModeActive)
   {
      // TODO-Taxi

      switch (mCurrentActivityPtr->GetSpeedChannelMode())
      {
         // case P6DOF::Speed::Taxi_Waypoint:
         //    speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aDT);
         //    break;
         // case P6DOF::Speed::Taxi_ForwardAccel:
         //    speedCommand_g = ProcessSpeedMode_Taxi_ForwardAccel(aDT);
         //    break;
         // case P6DOF::Speed::Taxi_KIAS:
         //    speedCommand_g = ProcessSpeedMode_Taxi_KIAS(aDT);
         //    break;
         // case P6DOF::Speed::Taxi_KTAS:
         //    speedCommand_g = ProcessSpeedMode_Taxi_KTAS(aDT);
         //    break;

      case P6DOF::Speed::Waypoint:
         // case P6DOF::Speed::Taxi_Waypoint:
         speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aDT);
         break;

      case P6DOF::Speed::ForwardAccel:
         // case P6DOF::Speed::Taxi_ForwardAccel:
         speedCommand_g = ProcessSpeedMode_ForwardAccel(aDT);
         break;

      case P6DOF::Speed::KIAS:
         // case P6DOF::Speed::Taxi_KIAS:
         speedCommand_g = ProcessSpeedMode_KIAS(aDT);
         break;

      case P6DOF::Speed::KTAS:
         // case P6DOF::Speed::Taxi_KTAS:
         speedCommand_g = ProcessSpeedMode_KTAS(aDT);
         break;

      case P6DOF::Speed::Mach:
         speedCommand_g = ProcessSpeedMode_Mach(aDT);
         break;

      case P6DOF::Speed::FPS:
         speedCommand_g = ProcessSpeedMode_FPS(aDT);
         break;

      default:
         speedCommand_g = ProcessSpeedMode_NoControl();
         controlled     = false;
         break;
      }

      // TODO-Taxi -- need to add the threshold for wheel brake use

      // Set wheel brakes
      if (speedCommand_g < 0.0)
      {
         mCombinedWheelBraking = (double)mCurrentLimitsAndSettings.speedBrakeThreshold - speedCommand_g;
      }
   }
   else
   {
      switch (mCurrentActivityPtr->GetSpeedChannelMode())
      {
      case P6DOF::Speed::Waypoint:
         speedCommand_g = ProcessSpeedMode_Waypoint(aDT);
         break;
      case P6DOF::Speed::ForwardAccel:
         speedCommand_g = ProcessSpeedMode_ForwardAccel(aDT);
         break;
      case P6DOF::Speed::KIAS:
         speedCommand_g = ProcessSpeedMode_KIAS(aDT);
         break;
      case P6DOF::Speed::KTAS:
         speedCommand_g = ProcessSpeedMode_KTAS(aDT);
         break;
      case P6DOF::Speed::Mach:
         speedCommand_g = ProcessSpeedMode_Mach(aDT);
         break;
      case P6DOF::Speed::FPS:
         speedCommand_g = ProcessSpeedMode_FPS(aDT);
         break;
      default:
         speedCommand_g = ProcessSpeedMode_NoControl();
         controlled     = false;
         break;
      }
   }

   if (controlled)
   {
      // Set throttles and speed brakes
      if (mCurrentLimitsAndSettings.enableAfterburnerAutoControl &&
          (speedCommand_g > mCurrentLimitsAndSettings.afterburnerThreshold))
      {
         // Use afterburner
         mControlOutputsP6Dof.throttleAfterburner = speedCommand_g - (double)mCurrentLimitsAndSettings.afterburnerThreshold;
         mControlOutputsP6Dof.throttleMilitary = 1;
         mControlOutputsP6Dof.speedBrake       = 0;
      }
      else if (mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl &&
               (speedCommand_g < mCurrentLimitsAndSettings.speedBrakeThreshold))
      {
         // Use speed brake
         mControlOutputsP6Dof.throttleAfterburner = 0;
         mControlOutputsP6Dof.throttleMilitary    = 0;
         mControlOutputsP6Dof.speedBrake = (double)mCurrentLimitsAndSettings.speedBrakeThreshold - speedCommand_g;
      }
      else
      {
         // normal range
         mControlOutputsP6Dof.throttleAfterburner = 0;
         mControlOutputsP6Dof.throttleMilitary    = speedCommand_g;
         mControlOutputsP6Dof.speedBrake          = 0;
      }
   }
}

void P6DofCommonController::ProcessLaternalNavChannelsYawToTurn(double aDT)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }
   else
   {
      bool stabilizingChannelIsControlled = false;

      switch (mCurrentActivityPtr->GetLateralChannelMode())
      {
      case P6DOF::Lateral::Waypoint:
         ProcessLateralNavMode_YawWaypoint(aDT);
         break;
      case P6DOF::Lateral::Heading:
         ProcessLateralNavMode_YawHeading(aDT);
         break;
      case P6DOF::Lateral::Point:
         ProcessLateralNavMode_YawPoint(aDT);
         break;
      case P6DOF::Lateral::YawGLoad:
         ProcessLateralNavMode_YawGLoad(aDT);
         break;
      case P6DOF::Lateral::YawRate:
         ProcessLateralNavMode_YawRate(aDT);
         break;
      case P6DOF::Lateral::Beta:
         ProcessLateralNavMode_Beta(aDT);
         break;
      // Roll channels here are mostly vestigial, but are
      // still needed to tune stabilizing channels
      case P6DOF::Lateral::RollRate:
         ProcessLateralNavMode_RollRate(aDT);
         stabilizingChannelIsControlled = true;
         break;
      case P6DOF::Lateral::Bank:
         ProcessLateralNavMode_Bank(aDT);
         stabilizingChannelIsControlled = true;
         break;
      case P6DOF::Lateral::DeltaRoll:
         ProcessLateralNavMode_DeltaRoll(aDT);
         stabilizingChannelIsControlled = true;
         break;
      default:
         ProcessLateralNavMode_NoYawControl();
         break;
      }

      // Process the stabilizing channel, if requested
      if (!stabilizingChannelIsControlled)
      {
         switch (mCurrentActivityPtr->GetStabilizingChannelMode())
         {
         case P6DOF::Lateral::RollRate:
            ProcessLateralNavMode_RollRate(aDT);
            break;
         case P6DOF::Lateral::Bank:
            ProcessLateralNavMode_Bank(aDT);
            break;
         case P6DOF::Lateral::DeltaRoll:
            ProcessLateralNavMode_DeltaRoll(aDT);
            break;
         case P6DOF::Lateral::Undefined:
            // In lieu of an explicit stabilizing control, use the control type
            switch (mControlMethod)
            {
            case P6DOF::Control::YawToTurnRollRate:
               ProcessStandardLateralNavMode_RollRate(0.0, aDT);
               break;
            case P6DOF::Control::YawToTurnZeroBank:
               ProcessStandardLateralNavMode_Bank(0.0, aDT);
               break;
            case P6DOF::Control::YawToTurnNoRoll:
            default:
               ProcessLateralNavMode_NoRollControl();
               break;
            }
            break;
         default:
            ProcessLateralNavMode_NoRollControl();
            break;
         }
      }
   }
}

void P6DofCommonController::ProcessVerticalNavChannelYawToTurn(double aDT)
{
   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }

   switch (mCurrentActivityPtr->GetVerticalChannelMode())
   {
   case P6DOF::Vertical::Waypoint:
      ProcessVerticalNavMode_Waypoint(aDT);
      break;
   case P6DOF::Vertical::Altitude:
      ProcessVerticalNavMode_Altitude(aDT);
      break;
   case P6DOF::Vertical::VertSpeed:
      ProcessVerticalNavMode_VertSpeed(aDT);
      break;
   case P6DOF::Vertical::Point:
      ProcessVerticalNavMode_Point(aDT);
      break;
   case P6DOF::Vertical::PitchGLoad:
      ProcessVerticalNavMode_PitchGLoad(aDT);
      break;
   case P6DOF::Vertical::PitchAng:
      ProcessVerticalNavMode_PitchAng(aDT);
      break;
   case P6DOF::Vertical::PitchRate:
      ProcessVerticalNavMode_PitchRate(aDT);
      break;
   case P6DOF::Vertical::FltPathAng:
      ProcessVerticalNavMode_FltPathAng(aDT);
      break;
   case P6DOF::Vertical::DeltaPitch:
      ProcessVerticalNavMode_DeltaPitch(aDT);
      break;
   case P6DOF::Vertical::Alpha:
      ProcessVerticalNavMode_Alpha(aDT);
      break;
   default:
      ProcessVerticalNavMode_NoControl(aDT);
      break;
   }
}

void P6DofCommonController::ProcessSpeedChannelYawToTurn(double aDT)
{
   double speedCommand_g = 0.0;
   bool   controlled     = true;

   if (mTaxiModeActive)
   {
      // TODO-Taxi
   }

   switch (mCurrentActivityPtr->GetSpeedChannelMode())
   {
   case P6DOF::Speed::Waypoint:
      speedCommand_g = ProcessSpeedMode_Waypoint(aDT);
      break;
   case P6DOF::Speed::ForwardAccel:
      speedCommand_g = ProcessSpeedMode_ForwardAccel(aDT);
      break;
   case P6DOF::Speed::KIAS:
      speedCommand_g = ProcessSpeedMode_KIAS(aDT);
      break;
   case P6DOF::Speed::KTAS:
      speedCommand_g = ProcessSpeedMode_KTAS(aDT);
      break;
   case P6DOF::Speed::Mach:
      speedCommand_g = ProcessSpeedMode_Mach(aDT);
      break;
   case P6DOF::Speed::FPS:
      speedCommand_g = ProcessSpeedMode_FPS(aDT);
      break;

      // case P6DOF::Speed::Taxi_Waypoint:
      //    speedCommand_g = ProcessSpeedMode_Taxi_Waypoint(aDT);
      //    break;
      // case P6DOF::Speed::Taxi_ForwardAccel:
      //    speedCommand_g = ProcessSpeedMode_Taxi_ForwardAccel(aDT);
      //    break;
      // case P6DOF::Speed::Taxi_KIAS:
      //    speedCommand_g = ProcessSpeedMode_Taxi_KIAS(aDT);
      //    break;
      // case P6DOF::Speed::Taxi_KTAS:
      //    speedCommand_g = ProcessSpeedMode_Taxi_KTAS(aDT);
      //    break;

   default:
      speedCommand_g = ProcessSpeedMode_NoControl();
      controlled     = false;
      break;
   }

   if (controlled)
   {
      // Set throttles and speed brakes
      if (mCurrentLimitsAndSettings.enableAfterburnerAutoControl &&
          (speedCommand_g > (double)mCurrentLimitsAndSettings.afterburnerThreshold))
      {
         // Use afterburner
         mControlOutputsP6Dof.throttleAfterburner = speedCommand_g - (double)mCurrentLimitsAndSettings.afterburnerThreshold;
         mControlOutputsP6Dof.throttleMilitary = 1.0f;
         mControlOutputsP6Dof.speedBrake       = 0.0f;
      }
      else if (mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl &&
               (speedCommand_g < (double)mCurrentLimitsAndSettings.speedBrakeThreshold))
      {
         // Use speed brake
         mControlOutputsP6Dof.throttleAfterburner = 0;
         mControlOutputsP6Dof.throttleMilitary    = 0;
         mControlOutputsP6Dof.speedBrake = (double)mCurrentLimitsAndSettings.speedBrakeThreshold - speedCommand_g;
      }
      else
      {
         // normal range
         mControlOutputsP6Dof.throttleAfterburner = 0;
         mControlOutputsP6Dof.throttleMilitary    = speedCommand_g;
         mControlOutputsP6Dof.speedBrake          = 0;
      }
   }
}

void P6DofCommonController::LoadConfigFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in P6DofCommonController::LoadConfigFile()");
   }
   UtInput input;
   input.PushInput(std::move(inputPtr));

   ProcessInput(input);
}

void P6DofCommonController::ReadSupportFile(const std::string& aFilename)
{
   std::unique_ptr<UtInputFile> inputPtr = nullptr;
   try
   {
      inputPtr = ut::make_unique<UtInputFile>(aFilename);
   }
   catch (std::exception&)
   {
      throw UtException("Unable to open input file " + aFilename + " in P6DofCommonController::ReadSupportFile()");
   }

   UtInput input;
   input.PushInput(std::move(inputPtr));

   const std::string& command = input.GetCommand();
   if (command == "autopilot_support_tables")
   {
      UtInputBlock block(input, "end_autopilot_support_tables");
      while (block.ReadCommand())
      {
         std::string blockCommand = block.GetCommand();

         if (blockCommand == "cl_max_mach_table")
         {
            mCLMaxMachTablePtr = ut::make_unique<UtTable::Curve>();
            mCLMaxMachTablePtr->ProcessInput(input,
                                             UtInput::cNON_DIMENSIONAL,
                                             "mach",
                                             UtTable::ValueGE(0.0),
                                             UtInput::cNON_DIMENSIONAL,
                                             "clmax",
                                             UtTable::NoCheck());
         }
         else if (blockCommand == "cl_min_mach_table")
         {
            mCLMinMachTablePtr = ut::make_unique<UtTable::Curve>();
            mCLMinMachTablePtr->ProcessInput(input,
                                             UtInput::cNON_DIMENSIONAL,
                                             "mach",
                                             UtTable::ValueGE(0.0),
                                             UtInput::cNON_DIMENSIONAL,
                                             "clmin",
                                             UtTable::NoCheck());
         }
         else if (blockCommand == "alpha_max_mach_table")
         {
            mAlphaMaxMachTablePtr = ut::make_unique<UtTable::Curve>();
            mAlphaMaxMachTablePtr->ProcessInput(input,
                                                UtInput::cNON_DIMENSIONAL,
                                                "mach",
                                                UtTable::ValueGE(0.0),
                                                UtInput::cNON_DIMENSIONAL,
                                                "alpha_deg",
                                                UtTable::NoCheck());
         }
         else if (blockCommand == "alpha_min_mach_table")
         {
            mAlphaMinMachTablePtr = ut::make_unique<UtTable::Curve>();
            mAlphaMinMachTablePtr->ProcessInput(input,
                                                UtInput::cNON_DIMENSIONAL,
                                                "mach",
                                                UtTable::ValueGE(0.0),
                                                UtInput::cNON_DIMENSIONAL,
                                                "alpha_deg",
                                                UtTable::NoCheck());
         }
         else if (blockCommand == "stick_zero_moment_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_thrust_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_thrust_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_xcg_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_xcg_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "stick_zero_moment_delta_zcg_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_stick_zero_moment_delta_zcg_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "effective_CL_versus_mach_alpha_table")
         {
            UtInputBlock subBlock(input, "end_effective_CL_versus_mach_alpha_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alpha", UtInput::cANGLE, UtTable::ValueGE_LE(-UtMath::cPI, UtMath::cPI), 1);

               mEffectiveCLVsMachAlphaTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (blockCommand == "alpha_versus_mach_cl_table")
         {
            UtInputBlock subBlock(input, "end_alpha_versus_mach_cl_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("cl", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 1);

               mAlphaVsMachCLTablePtr =
                  UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
      }
   }
}

void P6DofCommonController::ProcessInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();
   if (command == "autopilot_config")
   {
      bool         pidGroupDefined = false;
      UtInputBlock block(aInput, "end_autopilot_config");
      while (block.ReadCommand())
      {
         std::string blockCommand = block.GetCommand();

         if (blockCommand == "vertical_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue     = (unsigned char)value;
               mVerticalMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "lateral_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue    = (unsigned char)value;
               mLateralMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "speed_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue  = (unsigned char)value;
               mSpeedMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "vertical_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue    = (unsigned char)value;
               mVerticalOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "lateral_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue   = (unsigned char)value;
               mLateralOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "speed_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue = (unsigned char)value;
               mSpeedOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "control_method")
         {
            std::string value;
            aInput.ReadValue(value);

            if (value == "bank_to_turn_no_yaw")
            {
               mControlMethod = P6DOF::Control::BankToTurnNoYaw;
            }
            else if (value == "bank_to_turn_with_yaw")
            {
               mControlMethod = P6DOF::Control::BankToTurnWithYaw;
            }
            else if (value == "yaw_to_turn_no_roll")
            {
               mControlMethod = P6DOF::Control::YawToTurnNoRoll;
            }
            else if (value == "yaw_to_turn_roll_rate")
            {
               mControlMethod = P6DOF::Control::YawToTurnRollRate;
            }
            else if (value == "yaw_to_turn_zero_bank")
            {
               mControlMethod = P6DOF::Control::YawToTurnZeroBank;
            }
            else if (value == "BANK_TO_TURN_NO_YAW")
            {
               mControlMethod = P6DOF::Control::BankToTurnNoYaw;
            }
            else if (value == "BANK_TO_TURN_WITH_YAW")
            {
               mControlMethod = P6DOF::Control::BankToTurnWithYaw;
            }
            else if (value == "YAW_TO_TURN_NO_ROLL")
            {
               mControlMethod = P6DOF::Control::YawToTurnNoRoll;
            }
            else if (value == "YAW_TO_TURN_ROLL_RATE")
            {
               mControlMethod = P6DOF::Control::YawToTurnRollRate;
            }
            else if (value == "YAW_TO_TURN_ZERO_BANK")
            {
               mControlMethod = P6DOF::Control::YawToTurnZeroBank;
            }
         }
         else if (blockCommand == "use_legacy_beta")
         {
            aInput.ReadValue(mUseLegacyBeta);
         }
         else if (blockCommand == "pid_group")
         {
            if (pidGroupDefined)
            {
               // Error
               ut::log::warning() << "A pid_group has already been defined.";
            }
            else
            {
               pidGroupDefined = true;

               UtInputBlock subBlock(aInput, "end_pid_group");
               ProcessPidGroupInputBlock(subBlock, aInput);
            }
         }
         else if (blockCommand == "min_taxi_turn_radius")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            mMinimumTaxiTurnRadius_ft = value * UtMath::cFT_PER_M;
            mDesiredTaxiTurnRadius_ft = mMinimumTaxiTurnRadius_ft;
         }
         else if (blockCommand == "use_simple_yaw_damper")
         {
            bool value;
            aInput.ReadValue(value);

            mUseSimpleYawDamper = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofCommonController::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
}

void P6DofCommonController::ProcessInputCommand(UtInput& aInput, const std::string& aCommand)
{
   if (aCommand == "autopilot_config")
   {
      bool         pidGroupDefined = false;
      UtInputBlock block(aInput, "end_autopilot_config");
      while (block.ReadCommand())
      {
         std::string blockCommand = block.GetCommand();

         if (blockCommand == "vertical_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue     = (unsigned char)value;
               mVerticalMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "lateral_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue    = (unsigned char)value;
               mLateralMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "speed_middle_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue  = (unsigned char)value;
               mSpeedMiddleLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "vertical_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue    = (unsigned char)value;
               mVerticalOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "lateral_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue   = (unsigned char)value;
               mLateralOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "speed_outer_loop_rate_factor")
         {
            int value;
            aInput.ReadValue(value);
            if ((value > 0) && (value < 255))
            {
               unsigned char ucValue = (unsigned char)value;
               mSpeedOuterLoopFactor = ucValue;
            }
         }
         else if (blockCommand == "control_method")
         {
            std::string value;
            aInput.ReadValue(value);

            if (value == "bank_to_turn_no_yaw")
            {
               mControlMethod = P6DOF::Control::BankToTurnNoYaw;
            }
            else if (value == "bank_to_turn_with_yaw")
            {
               mControlMethod = P6DOF::Control::BankToTurnWithYaw;
            }
            else if (value == "yaw_to_turn_no_roll")
            {
               mControlMethod = P6DOF::Control::YawToTurnNoRoll;
            }
            else if (value == "yaw_to_turn_roll_rate")
            {
               mControlMethod = P6DOF::Control::YawToTurnRollRate;
            }
            else if (value == "yaw_to_turn_zero_bank")
            {
               mControlMethod = P6DOF::Control::YawToTurnZeroBank;
            }
            else if (value == "BANK_TO_TURN_NO_YAW")
            {
               mControlMethod = P6DOF::Control::BankToTurnNoYaw;
            }
            else if (value == "BANK_TO_TURN_WITH_YAW")
            {
               mControlMethod = P6DOF::Control::BankToTurnWithYaw;
            }
            else if (value == "YAW_TO_TURN_NO_ROLL")
            {
               mControlMethod = P6DOF::Control::YawToTurnNoRoll;
            }
            else if (value == "YAW_TO_TURN_ROLL_RATE")
            {
               mControlMethod = P6DOF::Control::YawToTurnRollRate;
            }
            else if (value == "YAW_TO_TURN_ZERO_BANK")
            {
               mControlMethod = P6DOF::Control::YawToTurnZeroBank;
            }
         }
         else if (blockCommand == "use_legacy_beta")
         {
            bool value;
            aInput.ReadValue(value);
            mUseLegacyBeta = value;
         }
         else if (blockCommand == "pid_group")
         {
            if (pidGroupDefined)
            {
               // Error
               ut::log::error() << "A pid_group has already been defined.";
            }
            else
            {
               pidGroupDefined = true;

               UtInputBlock subBlock(aInput, "end_pid_group");
               ProcessPidGroupInputBlock(subBlock, aInput);
            }
         }
         else if (blockCommand == "min_taxi_turn_radius")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            mMinimumTaxiTurnRadius_ft = value * UtMath::cFT_PER_M;
            mDesiredTaxiTurnRadius_ft = mMinimumTaxiTurnRadius_ft;
         }
         else if (blockCommand == "use_simple_yaw_damper")
         {
            bool value;
            aInput.ReadValue(value);

            mUseSimpleYawDamper = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofCommonController::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
}

void P6DofCommonController::ProcessPidGroupInputBlock(UtInputBlock& aInputBlock, UtInput& aInput)
{
   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "pid_alpha")
      {
         UtInputBlock subBlock(aInput, "end_pid_alpha");
         ProcessPidInputBlock(subBlock, aInput, mAlphaPID);
      }
      else if (command == "pid_vert_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_vert_speed");
         ProcessPidInputBlock(subBlock, aInput, mVertSpeedPID);
      }
      else if (command == "pid_pitch_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_pitch_angle");
         ProcessPidInputBlock(subBlock, aInput, mPitchAnglePID);
      }
      else if (command == "pid_pitch_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_pitch_rate");
         ProcessPidInputBlock(subBlock, aInput, mPitchRatePID);
      }
      else if (command == "pid_flightpath_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_flightpath_angle");
         ProcessPidInputBlock(subBlock, aInput, mFltpathAnglePID);
      }
      else if (command == "pid_delta_pitch")
      {
         UtInputBlock subBlock(aInput, "end_pid_delta_pitch");
         ProcessPidInputBlock(subBlock, aInput, mDeltaPitchPID);
      }
      else if (command == "pid_altitude")
      {
         UtInputBlock subBlock(aInput, "end_pid_altitude");
         ProcessPidInputBlock(subBlock, aInput, mAltitudePID);
      }
      else if (command == "pid_beta")
      {
         UtInputBlock subBlock(aInput, "end_pid_beta");
         ProcessPidInputBlock(subBlock, aInput, mBetaPID);
      }
      else if (command == "pid_yaw_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_yaw_rate");
         ProcessPidInputBlock(subBlock, aInput, mYawRatePID);
      }
      else if (command == "pid_yaw_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_yaw_heading");
         ProcessPidInputBlock(subBlock, aInput, mYawHeadingPID);
      }
      else if (command == "pid_taxi_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_heading");
         ProcessPidInputBlock(subBlock, aInput, mTaxiHeadingPID);
      }
      else if (command == "pid_roll_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_roll_rate");
         ProcessPidInputBlock(subBlock, aInput, mRollRatePID);
      }
      else if (command == "pid_delta_roll")
      {
         UtInputBlock subBlock(aInput, "end_pid_delta_roll");
         ProcessPidInputBlock(subBlock, aInput, mDeltaRollPID);
      }
      else if (command == "pid_bank_angle")
      {
         UtInputBlock subBlock(aInput, "end_pid_bank_angle");
         ProcessPidInputBlock(subBlock, aInput, mBankAnglePID);
      }
      else if (command == "pid_roll_heading")
      {
         UtInputBlock subBlock(aInput, "end_pid_roll_heading");
         ProcessPidInputBlock(subBlock, aInput, mRollHeadingPID);
      }
      else if (command == "pid_forward_accel")
      {
         UtInputBlock subBlock(aInput, "end_pid_forward_accel");
         ProcessPidInputBlock(subBlock, aInput, mForwardAccelPID);
      }
      else if (command == "pid_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_speed");
         ProcessPidInputBlock(subBlock, aInput, mSpeedPID);
      }
      else if (command == "pid_taxi_forward_accel")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_forward_accel");
         ProcessPidInputBlock(subBlock, aInput, mTaxiForwardAccelPID);
      }
      else if (command == "pid_taxi_speed")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_speed");
         ProcessPidInputBlock(subBlock, aInput, mTaxiSpeedPID);
      }
      else if (command == "pid_taxi_yaw_rate")
      {
         UtInputBlock subBlock(aInput, "end_pid_taxi_yaw_rate");
         ProcessPidInputBlock(subBlock, aInput, mTaxiYawRatePID);
      }
      else if (command == "limits_and_settings")
      {
         UtInputBlock subBlock(aInput, "end_limits_and_settings");
         ProcessLimitsAndSettingsInputBlock(subBlock);
      }
      else
      {
         auto out =
            ut::log::warning() << "Unrecognized data command in P6DofCommonController::ProcessPidGroupInputBlock().";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
}

void P6DofCommonController::ProcessPidInputBlock(UtInputBlock& aInputBlock, UtInput& aInput, P6DofPID& aPID)
{
   bool usingScalarGainData  = false;
   bool usingTabularGainData = false;
   bool validKpGain          = false;
   bool validKiGain          = false;
   bool validKdGain          = false;
   bool validMaxAccum        = false;
   bool validMaxErrorZero    = false;
   bool validMinErrorZero    = false;
   bool validLowpassAlpha    = false;
   bool validKtAntiWindup    = false;

   std::vector<P6DOF::PidGainData> gainTable;
   unsigned char                   flags = 0;

   P6DOF::PidGainData singleGainTableElement;
   singleGainTableElement.ControllingValue = 0;

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();
      if (command == "kp")
      {
         float Kp = 1.0;
         controllerInput.ReadValue(Kp);
         singleGainTableElement.KpGain = Kp;
         usingScalarGainData           = true;
      }
      else if (command == "ki")
      {
         float Ki = 0.0;
         controllerInput.ReadValue(Ki);
         singleGainTableElement.KiGain = Ki;
         usingScalarGainData           = true;
      }
      else if (command == "kd")
      {
         float Kd = 0.0;
         controllerInput.ReadValue(Kd);
         singleGainTableElement.KdGain = Kd;
         usingScalarGainData           = true;
      }
      else if (command == "max_error_accum")
      {
         float accum = 0.0;
         controllerInput.ReadValue(accum);
         singleGainTableElement.MaxAccum = accum;
         flags |= P6DOF::Pid::LimitMax;
         usingScalarGainData = true;
      }
      else if (command == "low_pass_alpha")
      {
         float lowpassAlpha = 0.0;
         controllerInput.ReadValue(lowpassAlpha);
         singleGainTableElement.LowpassAlpha = lowpassAlpha;
         flags |= P6DOF::Pid::UseAlpha;
         usingScalarGainData = true;
      }
      else if (command == "ignore_large_error_accum")
      {
         float value = 0.0;
         controllerInput.ReadValue(value);
         singleGainTableElement.MaxErrorZero = value;
         flags |= P6DOF::Pid::ZeroGtMax;
         usingScalarGainData = true;
      }
      else if (command == "ignore_small_error_accum")
      {
         float value = 0.0;
         controllerInput.ReadValue(value);
         singleGainTableElement.MinErrorZero = value;
         flags |= P6DOF::Pid::ZeroLtMin;
         usingScalarGainData = true;
      }
      else if (command == "kt_anti_windup_gain")
      {
         float value = 0.0;
         controllerInput.ReadValue(value);
         singleGainTableElement.KtAntiWindup = value;
         flags |= P6DOF::Pid::UseKt;
         usingScalarGainData = true;
      }
      else if (command == "gain_table")
      {
         P6DOF::PidGainData gainTableElement;

         UtInputBlock subBlock(aInput, "end_gain_table");

         bool tempValidControllingValue = false;
         bool tempValidKpGain           = false;
         bool tempValidKiGain           = false;
         bool tempValidKdGain           = false;
         bool tempValidMaxAccum         = false;
         bool tempValidMaxErrorZero     = false;
         bool tempValidMinErrorZero     = false;
         bool tempValidLowpassAlpha     = false;
         bool tempValidKtAntiWindup     = false;

         ProcessPidGainTableBlock(subBlock,
                                  gainTableElement,
                                  tempValidControllingValue,
                                  tempValidKpGain,
                                  tempValidKiGain,
                                  tempValidKdGain,
                                  tempValidMaxAccum,
                                  tempValidMaxErrorZero,
                                  tempValidMinErrorZero,
                                  tempValidLowpassAlpha,
                                  tempValidKtAntiWindup,
                                  flags);

         bool errorPresent = false;

         if (!tempValidControllingValue)
         {
            ut::log::error() << "Missing control_value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
         }

         if (validKpGain)
         {
            if (!tempValidKpGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing kp value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validKiGain)
         {
            if (!tempValidKiGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing ki value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validKdGain)
         {
            if (!tempValidKdGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing kd value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validMaxAccum)
         {
            if (!tempValidMaxAccum)
            {
               errorPresent = true;
               ut::log::error()
                  << "Missing max_error_accum value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validMaxErrorZero)
         {
            if (!tempValidMaxErrorZero)
            {
               errorPresent = true;
               ut::log::error() << "Missing ignore_large_error_accum value in gain_table in "
                                   "P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validMinErrorZero)
         {
            if (!tempValidMinErrorZero)
            {
               errorPresent = true;
               ut::log::error() << "Missing ignore_small_error_accum value in gain_table in "
                                   "P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validLowpassAlpha)
         {
            if (!tempValidLowpassAlpha)
            {
               errorPresent = true;
               ut::log::error()
                  << "Missing low_pass_alpha value in gain_table in P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         if (validKtAntiWindup)
         {
            if (!tempValidKtAntiWindup)
            {
               errorPresent = true;
               ut::log::error() << "Missing kt_anti_windup_gain value in gain_table in "
                                   "P6DofCommonController::ProcessPidInputBlock().";
            }
         }

         // Set flags
         if (tempValidKpGain)
         {
            validKpGain = true;
         }
         if (tempValidKiGain)
         {
            validKiGain = true;
         }
         if (tempValidKdGain)
         {
            validKdGain = true;
         }
         if (tempValidMaxAccum)
         {
            validMaxAccum = true;
         }
         if (tempValidMaxErrorZero)
         {
            validMaxErrorZero = true;
         }
         if (tempValidMinErrorZero)
         {
            validMinErrorZero = true;
         }
         if (tempValidLowpassAlpha)
         {
            validLowpassAlpha = true;
         }
         if (tempValidKtAntiWindup)
         {
            validKtAntiWindup = true;
         }

         if (tempValidControllingValue && !errorPresent)
         {
            gainTable.push_back(gainTableElement);
         }
         usingTabularGainData = true;
      }
      else
      {
         auto out =
            ut::log::warning() << "Unrecognized data command within P6DofCommonController::ProcessPidInputBlock().";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }

   if (usingTabularGainData && usingScalarGainData)
   {
      ut::log::error() << "Both tabular and scalar data are used in P6DofCommonController::ProcessPidInputBlock().";
   }

   if (usingScalarGainData)
   {
      gainTable.push_back(singleGainTableElement);
   }
   aPID.SetPidGainTableData(gainTable, flags);
}

void P6DofCommonController::ProcessPidGainTableBlock(UtInputBlock&       aInputBlock,
                                                     P6DOF::PidGainData& aTableElement,
                                                     bool&               aValidControllingValue,
                                                     bool&               aValidKpGain,
                                                     bool&               aValidKiGain,
                                                     bool&               aValidKdGain,
                                                     bool&               aValidMaxAccum,
                                                     bool&               aValidMaxErrorZero,
                                                     bool&               aValidMinErrorZero,
                                                     bool&               aValidLowpassAlpha,
                                                     bool&               aValidKtAntiWindup,
                                                     unsigned char&      aPackedFlags)
{
   // Clear data
   aTableElement.clearData();

   // Clear flags
   aValidControllingValue = false;
   aValidKpGain           = false;
   aValidKiGain           = false;
   aValidKdGain           = false;
   aValidMaxAccum         = false;
   aValidMaxErrorZero     = false;
   aValidMinErrorZero     = false;
   aValidLowpassAlpha     = false;
   aValidKtAntiWindup     = false;

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "control_value")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.ControllingValue = (float)value;
         aValidControllingValue         = true;
      }
      else if (command == "kp")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.KpGain = (float)value;
         aValidKpGain         = true;
      }
      else if (command == "ki")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.KiGain = (float)value;
         aValidKiGain         = true;
      }
      else if (command == "kd")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.KdGain = (float)value;
         aValidKdGain         = true;
      }
      else if (command == "max_error_accum")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.MaxAccum = (float)value;
         aValidMaxAccum         = true;
         aPackedFlags |= P6DOF::Pid::LimitMax;
      }
      else if (command == "low_pass_alpha")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.LowpassAlpha = (float)value;
         aValidLowpassAlpha         = true;
         aPackedFlags |= P6DOF::Pid::UseAlpha;
      }
      else if (command == "ignore_large_error_accum")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.MaxErrorZero = (float)value;
         aValidMaxErrorZero         = true;
         aPackedFlags |= P6DOF::Pid::ZeroGtMax;
      }
      else if (command == "ignore_small_error_accum")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.MinErrorZero = (float)value;
         aValidMinErrorZero         = true;
         aPackedFlags |= P6DOF::Pid::ZeroLtMin;
      }
      else if (command == "kt_anti_windup_gain")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         aTableElement.KtAntiWindup = (float)value;
         aValidKtAntiWindup         = true;
         aPackedFlags |= P6DOF::Pid::UseKt;
      }
      else
      {
         auto out =
            ut::log::warning() << "Unrecognized data command within P6DofCommonController::ProcessPidGainTableBlock().";
         out.AddNote() << "Command: " << command;
      }
   }
}

void P6DofCommonController::ProcessLimitsAndSettingsInputBlock(UtInputBlock& aInputBlock)
{
   mDefaultLimitsAndSettings = P6DOF::AutopilotLimitsAndSettings();

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "afterburner_threshold")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.afterburnerThreshold         = (float)value;
         mDefaultLimitsAndSettings.enableAfterburnerAutoControl = true;
      }
      else if (command == "speedbrake_threshold")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.speedBrakeThreshold         = (float)value;
         mDefaultLimitsAndSettings.enableSpeedBrakeAutoControl = true;
      }
      else if (command == "turn_roll_in_multiplier")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.turnRollInMultiplier = (float)value;
      }
      else if (command == "route_allowable_angle_error")
      {
         double value = 0.0;
         controllerInput.ReadValueOfType(value, UtInput::cANGLE);
         mDefaultLimitsAndSettings.routeAllowableAngleError_rad = (float)value;
      }
      else if (command == "pitch_gload_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchGLoad_Min = (float)value;
      }
      else if (command == "pitch_gload_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchGLoad_Max = (float)value;
      }
      else if (command == "alpha_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.alpha_Min = (float)value;
      }
      else if (command == "alpha_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.alpha_Max = (float)value;
      }
      else if (command == "pitch_rate_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchRate_Min = (float)value;
      }
      else if (command == "pitch_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.pitchRate_Max = (float)value;
      }
      else if (command == "vert_speed_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.vertSpd_Min = (float)value;
      }
      else if (command == "vert_speed_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.vertSpd_Max = (float)value;
      }
      else if (command == "yaw_gload_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.yawGLoad_Max = (float)value;
      }
      else if (command == "taxi_speed_max_fps")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.taxiSpeed_Max = (float)value;
      }
      else if (command == "taxi_yaw_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.taxiYawRate_Max = (float)value;
      }
      else if (command == "beta_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.beta_Max = (float)value;
      }
      else if (command == "yaw_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.yawRate_Max = (float)value;
      }

      else if (command == "roll_rate_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.rollRate_Max = (float)value;
      }
      else if (command == "bank_angle_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.bankAngle_Max = (float)value;
      }
      else if (command == "forward_accel_min")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.forwardAccel_Min = (float)value;
      }
      else if (command == "forward_accel_max")
      {
         double value = 0.0;
         controllerInput.ReadValue(value);
         mDefaultLimitsAndSettings.forwardAccel_Max = (float)value;
      }
   }

   // Once the block has finished parsing, mDefaultLimitsAndSettings should be complete.
   // Set the current limits to be equal to the default limits.
   mCurrentLimitsAndSettings = mDefaultLimitsAndSettings;
}

double P6DofCommonController::GetAimHeadingForWaypointNav_deg(double aDT)
{
   // No waypoint to travel to, so maintain last computed heading
   if (mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      mNavData.executeTurn = false;
      return mNavData.aimHeading_rad * UtMath::cDEG_PER_RAD;
   }

   double aimHdg_rad = 0.0;

   UtLLAPos        currentPosition = mState.GetCurrentPositionLLA();
   const UtLLAPos& currPos         = currentPosition;

   double currHdg_rad   = mState.GetLocalHeading_rad();
   double currSpeed_mps = mState.GetSpeed_mps();

   // Get speed at waypoint in m/sec
   double waypointSpeed_mps = currSpeed_mps;
   double currentAlt_ft     = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt() * UtMath::cFT_PER_M;

   P6DofWaypoint::sSpeed speed = mCurrentActivityPtr->GetCurrWaypoint()->GetSpeed();
   switch (speed.type)
   {
   case P6DofWaypoint::SPD_TYPE_MACH:
      waypointSpeed_mps = mAtmosphere->CalcFpsFromMach(currentAlt_ft, speed.val) * UtMath::cM_PER_FT;
      break;
   case P6DofWaypoint::SPD_TYPE_TAS_KNOTS:
      waypointSpeed_mps = mAtmosphere->CalcFpsFromKtas(speed.val) * UtMath::cM_PER_FT;
      break;
   case P6DofWaypoint::SPD_TYPE_CAS_KNOTS:
      waypointSpeed_mps = mAtmosphere->CalcFpsFromKcas(speed.val, currentAlt_ft) * UtMath::cM_PER_FT;
      break;
   case P6DofWaypoint::SPD_TYPE_FPS:
      waypointSpeed_mps = speed.val * UtMath::cM_PER_FT;
      break;
   default:
      break;
   }

   UtVec2d currVel(mState.GetSpeed_mps() * cos(currHdg_rad), mState.GetSpeed_mps() * sin(currHdg_rad));

   // If roll is the control method, then call CalcAimHeadingAndBankAngle
   if ((mControlMethod == P6DOF::Control::BankToTurnNoYaw) || (mControlMethod == P6DOF::Control::BankToTurnWithYaw))
   {
      P6DofRoute::CalcAimHeadingAndBankAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                             mCurrentActivityPtr->GetCurrWaypoint(),
                                             mCurrentActivityPtr->GetNextWaypoint(),
                                             mCurrentActivityPtr->GetCurrSegment(),
                                             mCurrentActivityPtr->GetNextSegment(),
                                             currPos,
                                             currVel,
                                             mNavData,
                                             (double)mCurrentLimitsAndSettings.turnRollInMultiplier,
                                             (double)mDefaultLimitsAndSettings.routeAllowableAngleError_rad,
                                             currHdg_rad,
                                             waypointSpeed_mps,
                                             (double)mCurrentLimitsAndSettings.bankAngle_Max * UtMath::cRAD_PER_DEG,
                                             (double)mCurrentLimitsAndSettings.rollRate_Max * UtMath::cRAD_PER_DEG,
                                             (double)mCurrentLimitsAndSettings.pitchGLoad_Max,
                                             aDT,
                                             mAchievedWaypoint);
   }
   // If yaw is the control method, then call CalcYawAimHeadingAngle
   else if ((mControlMethod == P6DOF::Control::YawToTurnNoRoll) || (mControlMethod == P6DOF::Control::YawToTurnRollRate) ||
            (mControlMethod == P6DOF::Control::YawToTurnZeroBank))
   {
      P6DofRoute::CalcYawAimHeadingAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                         mCurrentActivityPtr->GetCurrWaypoint(),
                                         mCurrentActivityPtr->GetNextWaypoint(),
                                         mCurrentActivityPtr->GetCurrSegment(),
                                         mCurrentActivityPtr->GetNextSegment(),
                                         currPos,
                                         currVel,
                                         mNavData,
                                         (double)mDefaultLimitsAndSettings.routeAllowableAngleError_rad,
                                         currHdg_rad,
                                         currSpeed_mps,
                                         (double)mCurrentLimitsAndSettings.pitchGLoad_Max,
                                         aDT,
                                         mAchievedWaypoint);
   }

   if (mCurrentActivityPtr->GetCurrWaypoint()->FollowHorizontalTrack())
   {
      aimHdg_rad = mNavData.aimHeading_rad;
   }
   else
   {
      aimHdg_rad = P6DofRoute::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());
   }

   // Return the aim heading
   return aimHdg_rad * UtMath::cDEG_PER_RAD;
}

double P6DofCommonController::CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(double aTurnRadius_ft)
{
   // Get current speed
   double speed_fps = mState.GetSpeed_fps(); // TODO - We should use ground_speed_fps

   if (std::abs(aTurnRadius_ft) < 0.01) // We test to ensure we have a reasonable radius
   {
      aTurnRadius_ft = 0.01; // Set a limit (we do not want zero, for example)
   }

   double turnRate_rps = speed_fps / aTurnRadius_ft;

   return (turnRate_rps * UtMath::cDEG_PER_RAD);
}

double P6DofCommonController::CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(double aTurnRate_dps)
{
   // Get current speed
   double speed_fps = mState.GetSpeed_fps(); // TODO - We should use ground_speed_fps

   if (std::abs(aTurnRate_dps) < 1.0E-8) // Avoid a near zero rate
   {
      return 1.0E20; // Return a very large turn radius
   }

   double aTurnRate_rps = aTurnRate_dps * UtMath::cRAD_PER_DEG;

   double turnRadius_ft = speed_fps / aTurnRate_rps;

   return turnRadius_ft;
}

void P6DofCommonController::GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(double  aDT,
                                                                           double& aAimHeading_deg,
                                                                           double& aTurnRate_dps)
{
   // No waypoint or ATM, maintain current heading and return
   if ((mCurrentActivityPtr->GetCurrWaypoint() == nullptr) || (mAtmosphere == nullptr))
   {
      mNavData.executeTurn = false;

      aAimHeading_deg = mNavData.aimHeading_rad * UtMath::cDEG_PER_RAD;
      aTurnRate_dps   = 0;

      return;
   }

   double aimHdg_rad = 0.0;

   UtLLAPos        currentPosition = mState.GetCurrentPositionLLA();
   const UtLLAPos& currPos         = currentPosition;

   double currentHdg_rad = mState.GetLocalHeading_rad();

   UtVec2d currVel(mState.GetSpeed_mps() * cos(currentHdg_rad), mState.GetSpeed_mps() * sin(currentHdg_rad));

   double desiredTaxiRadius_ft = mDesiredTaxiTurnRadius_ft;

   // We have the desired radius, but we need to be sure that it is within the taxi yaw rate limit
   double rateYawLimitedTaxiRadius_ft =
      CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(mCurrentLimitsAndSettings.taxiYawRate_Max);
   if (desiredTaxiRadius_ft < rateYawLimitedTaxiRadius_ft)
   {
      desiredTaxiRadius_ft = rateYawLimitedTaxiRadius_ft;
   }

   P6DofRoute::CalcTaxiAimHeadingAngle(mCurrentActivityPtr->GetPrevWaypoint(),
                                       mCurrentActivityPtr->GetCurrWaypoint(),
                                       mCurrentActivityPtr->GetNextWaypoint(),
                                       mCurrentActivityPtr->GetCurrSegment(),
                                       mCurrentActivityPtr->GetNextSegment(),
                                       currPos,
                                       currVel,
                                       mNavData,
                                       (float)currentHdg_rad,
                                       desiredTaxiRadius_ft,
                                       aDT,
                                       mAchievedWaypoint);

   if (mCurrentActivityPtr->GetCurrWaypoint()->FollowHorizontalTrack())
   {
      aimHdg_rad = mNavData.aimHeading_rad;
   }
   else
   {
      aimHdg_rad = P6DofRoute::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());
   }

   // Return the aim heading in deg
   aAimHeading_deg = aimHdg_rad * UtMath::cDEG_PER_RAD;

   aTurnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(desiredTaxiRadius_ft);
}

double P6DofCommonController::GetAimHeadingForPoint_deg()
{
   UtLLAPos currentPosition = mState.GetCurrentPositionLLA();

   const UtLLAPos& currPos = currentPosition;

   double aimHdg_rad = P6DofRoute::GetInitialHeading_rad(currPos, mCurrentActivityPtr->GetCurrWaypoint()->GetLLA());

   // Return the aim heading (in deg)
   return aimHdg_rad * UtMath::cDEG_PER_RAD;
}

void P6DofCommonController::GetAutoPilotTurnData(double& aAimHeading_rad,
                                                 double& aStartTurnHdg_rad,
                                                 double& aTurnRadius_m,
                                                 double& aTurnAngle_rad,
                                                 double& aTurnRefPtLat,
                                                 double& aTurnRefPtLon,
                                                 bool&   aTurnCW,
                                                 bool&   aTurning,
                                                 double& aTurnCenterLat,
                                                 double& aTurnCenterLon)
{
   aAimHeading_rad   = mAimHeading_rad;
   aStartTurnHdg_rad = mStartTurnHdg_rad;
   aTurnRadius_m     = mTurnRadius_m;
   aTurnAngle_rad    = mTurnAngle_rad;
   aTurnRefPtLat     = mTurnRefPoint.GetLat();
   aTurnRefPtLon     = mTurnRefPoint.GetLon();
   aTurnCW           = mTurnCW;
   aTurning          = mTurning;
   aTurnCenterLat    = mTurnCenterLat;
   aTurnCenterLon    = mTurnCenterLon;
}

bool P6DofCommonController::GetAfterburnerEnabled()
{
   return mCurrentLimitsAndSettings.enableAfterburnerAutoControl;
}

void P6DofCommonController::SetAfterburnerEnabled(bool aEnabled)
{
   mCurrentLimitsAndSettings.enableAfterburnerAutoControl = aEnabled;
}

double P6DofCommonController::GetAfterburnerThreshold()
{
   return (double)mCurrentLimitsAndSettings.afterburnerThreshold;
}

void P6DofCommonController::SetAfterburnerThreshold(double aValue)
{
   mCurrentLimitsAndSettings.afterburnerThreshold = (float)aValue;
}

bool P6DofCommonController::GetSpeedBrakeEnabled()
{
   return mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl;
}

void P6DofCommonController::SetSpeedBrakeEnabled(bool aEnabled)
{
   mCurrentLimitsAndSettings.enableSpeedBrakeAutoControl = aEnabled;
}

double P6DofCommonController::GetSpeedBrakeThreshold()
{
   return (double)mCurrentLimitsAndSettings.speedBrakeThreshold;
}

void P6DofCommonController::SetSpeedBrakeThreshold(double aValue)
{
   mCurrentLimitsAndSettings.speedBrakeThreshold = (float)aValue;
}

double P6DofCommonController::GetCurrentTurnRollInMultiplier()
{
   return (double)mCurrentLimitsAndSettings.turnRollInMultiplier;
}

double P6DofCommonController::GetDefaultTurnRollInMultiplier()
{
   return (double)mDefaultLimitsAndSettings.turnRollInMultiplier;
}

void P6DofCommonController::SetCurrentTurnRollInMultiplier(double aValue)
{
   mCurrentLimitsAndSettings.turnRollInMultiplier = (float)aValue;
}

double P6DofCommonController::GetCurrentRouteAllowableAngleError_rad()
{
   return (double)mCurrentLimitsAndSettings.routeAllowableAngleError_rad;
}

double P6DofCommonController::GetDefaultRouteAllowableAngleError_rad()
{
   return (double)mDefaultLimitsAndSettings.routeAllowableAngleError_rad;
}

void P6DofCommonController::SetRouteAllowableAngleError_rad(double aValue_rad)
{
   mCurrentLimitsAndSettings.routeAllowableAngleError_rad = (float)aValue_rad;
}

bool P6DofCommonController::GetAutopilotPidGainData(P6DOF::Pid::Type   aTableType,
                                                    size_t&            aNumElements,
                                                    P6DOF::PidGainData aPidGainData[],
                                                    unsigned char&     aPidFlags)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      std::vector<P6DOF::PidGainData>* dataTables = pid->GetPidGainTableData(aPidFlags);
      aPidFlags                                   = 0;

      size_t numPidElements = dataTables->size();
      if (aNumElements >= numPidElements)
      {
         for (size_t i = 0; i < numPidElements; ++i)
         {
            aPidGainData[i] = dataTables->at(i);
         }
         aNumElements = numPidElements;
      }
      return true;
   }
   return false;
}

std::vector<P6DOF::PidGainData>* P6DofCommonController::GetAutopilotPidGainData(P6DOF::Pid::Type aTableType,
                                                                                unsigned char&   aPidFlags)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      return pid->GetPidGainTableData(aPidFlags);
   }
   return nullptr;
}

void P6DofCommonController::GetAutopilotPidValues(P6DOF::AutopilotPidGroupValueData* aData)
{
   mAlphaPID.GetPidValueData(aData->alphaPID);
   mVertSpeedPID.GetPidValueData(aData->vertSpeedPID);
   mPitchAnglePID.GetPidValueData(aData->pitchAnglePID);
   mPitchRatePID.GetPidValueData(aData->pitchRatePID);
   mFltpathAnglePID.GetPidValueData(aData->fltpathAnglePID);
   mDeltaPitchPID.GetPidValueData(aData->deltaPitchPID);
   mAltitudePID.GetPidValueData(aData->altitudePID);
   mBetaPID.GetPidValueData(aData->betaPID);
   mYawRatePID.GetPidValueData(aData->yawRatePID);
   mYawHeadingPID.GetPidValueData(aData->yawheadingPID);
   mTaxiHeadingPID.GetPidValueData(aData->taxiHeadingPID);
   mRollRatePID.GetPidValueData(aData->rollRatePID);
   mDeltaRollPID.GetPidValueData(aData->deltaRollPID);
   mBankAnglePID.GetPidValueData(aData->bankAnglePID);
   mRollHeadingPID.GetPidValueData(aData->rollHeadingPID);
   mForwardAccelPID.GetPidValueData(aData->forwardAccelPID);
   mSpeedPID.GetPidValueData(aData->speedPID);
   mTaxiForwardAccelPID.GetPidValueData(aData->taxiForwardAccelPID);
   mTaxiSpeedPID.GetPidValueData(aData->taxiSpeedPID);
   mTaxiYawRatePID.GetPidValueData(aData->taxiYawRatePID);
}

P6DOF::Control::Method P6DofCommonController::GetControlMethod()
{
   return mControlMethod;
}

void P6DofCommonController::SetPidGainData(P6DOF::Pid::Type   aPidType,
                                           int                aNumElements,
                                           P6DOF::PidGainData aPidGainData[],
                                           unsigned char      aPidFlags)
{
   std::vector<P6DOF::PidGainData> tempDataTables;
   for (int i = 0; i < aNumElements; ++i)
   {
      P6DOF::PidGainData data = aPidGainData[i];
      tempDataTables.push_back(data);
   }

   P6DofPID* pid = GetPID_ByType(aPidType);
   if (pid != nullptr)
   {
      pid->SetPidGainTableData(tempDataTables, aPidFlags);
   }
}

bool P6DofCommonController::AddNewPidGainElement(P6DOF::Pid::Type aTableType, P6DOF::PidGainData& aPidGainData)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      unsigned char                    dummy;
      std::vector<P6DOF::PidGainData>* data = pid->GetPidGainTableData(dummy);

      if (data != nullptr)
      {
         for (std::vector<P6DOF::PidGainData>::iterator iter = data->begin(); iter != data->end(); ++iter)
         {
            if (aPidGainData.ControllingValue < iter->ControllingValue)
            {
               data->insert(iter, aPidGainData);
               // Iterator is now invalid so just return
               return true;
            }
            if (aPidGainData.ControllingValue == iter->ControllingValue)
            {
               // Element with the same controlling value already exist. Vector must be monotonically increasing.
               return false;
            }
         }

         // If we reach here, then the controlling value but be greater than last element in the vector,
         //  thus push it onto the end of the vector;
         data->push_back(aPidGainData);
         return true;
      }
   }
   return false;
}

bool P6DofCommonController::SetPidGainKP(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      unsigned char                    dummy;
      std::vector<P6DOF::PidGainData>* data = pid->GetPidGainTableData(dummy);

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KpGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool P6DofCommonController::SetPidGainKI(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      unsigned char                    dummy;
      std::vector<P6DOF::PidGainData>* data = pid->GetPidGainTableData(dummy);

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KiGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool P6DofCommonController::SetPidGainKD(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      unsigned char                    dummy;
      std::vector<P6DOF::PidGainData>* data = pid->GetPidGainTableData(dummy);

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.KdGain = aGainValue;
               return true;
            }
         }
      }
   }
   return false;
}

bool P6DofCommonController::SetPidMaxAccum(P6DOF::Pid::Type aTableType, float aControllingValue, float aMaxAccumValue)
{
   P6DofPID* pid = GetPID_ByType(aTableType);
   if (pid != nullptr)
   {
      unsigned char                    dummy;
      std::vector<P6DOF::PidGainData>* data = pid->GetPidGainTableData(dummy);

      if (data != nullptr)
      {
         for (auto& iter : *data)
         {
            if (iter.ControllingValue == aControllingValue)
            {
               iter.MaxAccum = aMaxAccumValue;
               return true;
            }
         }
      }
   }
   return false;
}

void P6DofCommonController::RevertLimitsAndSettingsToDefaults()
{
   mCurrentLimitsAndSettings = mDefaultLimitsAndSettings;
}

void P6DofCommonController::SetCurrentLimitsAndSettings(P6DOF::AutopilotLimitsAndSettings& aData)
{
   mCurrentLimitsAndSettings = aData;
}

void P6DofCommonController::SetCurrentMinPitchGLimit(double aMinPitchGLimit)
{
   mCurrentLimitsAndSettings.pitchGLoad_Min = (float)aMinPitchGLimit;
}

void P6DofCommonController::SetCurrentMaxPitchGLimit(double aMaxPitchGLimit)
{
   mCurrentLimitsAndSettings.pitchGLoad_Max = (float)aMaxPitchGLimit;
}

void P6DofCommonController::SetCurrentMinAlphaLimit(double aMinAlphaLimit)
{
   mCurrentLimitsAndSettings.alpha_Min = (float)aMinAlphaLimit;
}

void P6DofCommonController::SetCurrentMaxAlphaLimit(double aMaxAlphaLimit)
{
   mCurrentLimitsAndSettings.alpha_Max = (float)aMaxAlphaLimit;
}

void P6DofCommonController::SetCurrentMinPitchRateLimit(double aMinPitchRateLimit)
{
   mCurrentLimitsAndSettings.pitchRate_Min = (float)aMinPitchRateLimit;
}

void P6DofCommonController::SetCurrentMaxPitchRateLimit(double aMaxPitchRateLimit)
{
   mCurrentLimitsAndSettings.pitchRate_Max = (float)aMaxPitchRateLimit;
}

void P6DofCommonController::SetCurrentMinVertSpeedLimit(double aMinVertSpeedLimit)
{
   mCurrentLimitsAndSettings.vertSpd_Min = (float)aMinVertSpeedLimit;
}

void P6DofCommonController::SetCurrentMaxVertSpeedLimit(double aMaxVertSpeedLimit)
{
   mCurrentLimitsAndSettings.vertSpd_Max = (float)aMaxVertSpeedLimit;
}

void P6DofCommonController::SetCurrentMaxYawGLimit(double aMaxYawGLimit)
{
   mCurrentLimitsAndSettings.yawGLoad_Max = (float)aMaxYawGLimit;
}

void P6DofCommonController::SetCurrentMaxBetaLimit(double aMaxBetaLimit)
{
   mCurrentLimitsAndSettings.beta_Max = (float)aMaxBetaLimit;
}

void P6DofCommonController::SetCurrentMaxYawRateLimit(double aMaxYawRateLimit)
{
   mCurrentLimitsAndSettings.yawRate_Max = (float)aMaxYawRateLimit;
}

void P6DofCommonController::SetCurrentMaxRollRateLimit(double aMaxRollRateLimit)
{
   mCurrentLimitsAndSettings.rollRate_Max = (float)aMaxRollRateLimit;
}

void P6DofCommonController::SetCurrentMaxBankAngleLimit(double aMaxBankAngleLimit)
{
   mCurrentLimitsAndSettings.bankAngle_Max = (float)aMaxBankAngleLimit;
}

void P6DofCommonController::SetCurrentMinForwardAccelLimit(double aMinForwardAccelLimit)
{
   mCurrentLimitsAndSettings.forwardAccel_Min = (float)aMinForwardAccelLimit;
}

void P6DofCommonController::SetCurrentMaxForwardAccelLimit(double aMaxForwardAccelLimit)
{
   mCurrentLimitsAndSettings.forwardAccel_Max = (float)aMaxForwardAccelLimit;
}

void P6DofCommonController::SetCurrentMaxTaxiSpeedLimit(double aMaxTaxiSpeedLimit)
{
   mCurrentLimitsAndSettings.taxiSpeed_Max = (float)aMaxTaxiSpeedLimit;
}

void P6DofCommonController::SetCurrentMaxTaxiYawRateLimit(double aMaxTaxiYawRateLimit)
{
   mCurrentLimitsAndSettings.taxiYawRate_Max = (float)aMaxTaxiYawRateLimit;
}

void P6DofCommonController::AngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad)
{
   mIntegratedDeltaYaw_deg += (aYaw_rad * UtMath::cDEG_PER_RAD);
   mIntegratedDeltaPitch_deg += (aPitch_rad * UtMath::cDEG_PER_RAD);
   mIntegratedDeltaRoll_deg += (aRoll_rad * UtMath::cDEG_PER_RAD);
}

void P6DofCommonController::ResetLateralDeltaAngles()
{
   mIntegratedDeltaYaw_deg  = 0;
   mIntegratedDeltaRoll_deg = 0;
}

void P6DofCommonController::ResetAllDeltaAngles()
{
   mIntegratedDeltaYaw_deg   = 0;
   mIntegratedDeltaPitch_deg = 0;
   mIntegratedDeltaRoll_deg  = 0;
}

void P6DofCommonController::ResetAccumulatedPidData()
{
   for (P6DOF::Pid::Type pidType = (P6DOF::Pid::Type)(1); pidType != P6DOF::Pid::LastPidType;
        pidType                  = (P6DOF::Pid::Type)(pidType + 1))
   {
      P6DofPID* pid = GetPID_ByType(pidType);

      pid->ResetPidState();
   }
}

void P6DofCommonController::ProcessLateralNavMode_RollWaypoint(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForWaypointNav_deg(aDT);

   // Process PIDs
   ProcessStandardLateralNavMode_WaypointRollHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_RollPoint(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForPoint_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_RollHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_RollHeading(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = (double)mCurrentActivityPtr->GetHeading_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_RollHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::CalcLateralNavMode_RollHeadingCore(double aHeading_deg, double aMaxBankAngle_rad, double aDT)
{
   // Get current states
   double currentHeading_deg = mState.GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer and middle loop values (in case they are not executed this frame)
   double commandedBankAngle_deg = mLastComandedBankAngle_deg;

   if (mExecuteLateralOuterLoop)
   {
      // Set epsilon (used below)
      const double epsilonVal = std::numeric_limits<double>::epsilon();

      // Check max bank angle
      if (aMaxBankAngle_rad < epsilonVal)
      {
         // Bank angle is so small we cannot maneuver
         mLastComandedBankAngle_deg = 0.0;
         ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aDT);
         return;
      }

      // Calc the heading error
      double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

      // We previously explored a feed-forward for the mRollHeadingPID, but testing showed
      // that it was not needed and actually performed better without any feed-forward

      // Check max g
      double maxG = mCurrentLimitsAndSettings.pitchGLoad_Max;
      if (maxG < epsilonVal)
      {
         // Max g is so small we cannot maneuver
         mLastComandedBankAngle_deg = 0.0;
         ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aDT);
         return;
      }

      // Set the lateral g-load
      double lateral_g = 0.0;

      // If max bank angle is past 90 deg, limit max bank angle to PI/2
      if (aMaxBankAngle_rad > UtMath::cPI_OVER_2)
      {
         // We have a 90 deg bank, so lateral g-load is the maximum
         aMaxBankAngle_rad = UtMath::cPI_OVER_2;
         lateral_g         = maxG;
      }
      else
      {
         // Set lateral g based on a one g vertical component
         lateral_g = tan(aMaxBankAngle_rad);
      }

      // At this point, we will have a non-zero lateral g

      // Get the current pitch angle
      double pitchAngle_rad = mState.GetLocalPitch_deg() * UtMath::cRAD_PER_DEG;

      // Use 89 degrees as the maximum effect - since it will have an effect of ~57x which is more than enough
      const double maxPitchFactorEffectAngle_rad = 89.0 * UtMath::cRAD_PER_DEG;

      // Set a pitch factor for pitch effects
      double pitchFactor = 1.0; // Default is 1 (no effect)

      // Limit the pitch angle
      if (pitchAngle_rad < -maxPitchFactorEffectAngle_rad)
      {
         pitchAngle_rad = -maxPitchFactorEffectAngle_rad;
      }
      else if (pitchAngle_rad > maxPitchFactorEffectAngle_rad)
      {
         pitchAngle_rad = maxPitchFactorEffectAngle_rad;
      }
      pitchFactor = 1.0 / cos(pitchAngle_rad);

      // Adjust lateral_g for pitch effects
      lateral_g *= pitchFactor;

      // Limit the lateral g to the maximum g
      if (lateral_g > maxG)
      {
         lateral_g = maxG;
      }

      // Get the current speed
      double currentSpeed_fps = mState.GetSpeed_fps();

      // Set a minimum speed to consider - we'll use 0.001 ft/sec
      const double minSpeedToConsider_fps = 0.001; // This may need to be a vehicle-based value

      // Check for a low speed condition
      if (currentSpeed_fps < minSpeedToConsider_fps)
      {
         // The speed is so slow, we cannot maneuver
         mLastComandedBankAngle_deg = 0.0;
         ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aDT);
         return;
      }

      double radius_ft        = (currentSpeed_fps * currentSpeed_fps) / (32.174 * lateral_g);
      double circumference_ft = UtMath::cTWO_PI * radius_ft;
      double timeToCircle_sec = circumference_ft / currentSpeed_fps;

      // Check for a zero time to circle condition
      if (timeToCircle_sec < epsilonVal)
      {
         // We will simply turn at maximum bank angle

         if (hdgError_deg < 0)
         {
            commandedBankAngle_deg = -aMaxBankAngle_rad;
         }
         else
         {
            commandedBankAngle_deg = aMaxBankAngle_rad;
         }

         ProcessStandardLateralNavMode_Bank(commandedBankAngle_deg, aDT);
         return;
      }

      // Now, we need to determine the turn rate at the max bank angle (in a level turn)
      double maxTurnRate_dps = 360.0 / timeToCircle_sec;

      // Get the commanded turn rate from the roll heading PID
      double comandedTurnRate_dps =
         mRollHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg, aDT, -maxTurnRate_dps, maxTurnRate_dps);

      // Check for a zero turn rate condition
      if (fabs(comandedTurnRate_dps) < epsilonVal)
      {
         // The turn rate is so slow, we assume level flight
         mLastComandedBankAngle_deg = 0.0;
         ProcessStandardLateralNavMode_Bank(mLastComandedBankAngle_deg, aDT);
         return;
      }

      // Now, we need to convert from turn rate into bank angle

      // First, get the time to circle, circumference, and radius
      timeToCircle_sec = 360.0 / fabs(comandedTurnRate_dps);
      circumference_ft = timeToCircle_sec * currentSpeed_fps;
      radius_ft        = circumference_ft / UtMath::cTWO_PI;

      // Next, calculate the the lateral g
      lateral_g = (currentSpeed_fps * currentSpeed_fps) / (radius_ft * 32.174);

      // Bank angle is a function of lateral g-load
      double bank_rad = atan2(lateral_g, pitchFactor);

      commandedBankAngle_deg = bank_rad * UtMath::cDEG_PER_RAD;

      // Adjust sign
      if (comandedTurnRate_dps < 0)
      {
         commandedBankAngle_deg *= -1.0;
      }

      if (commandedBankAngle_deg > mCurrentLimitsAndSettings.bankAngle_Max)
      {
         commandedBankAngle_deg = mCurrentLimitsAndSettings.bankAngle_Max;
      }
      else if (commandedBankAngle_deg < -mCurrentLimitsAndSettings.bankAngle_Max)
      {
         commandedBankAngle_deg = -mCurrentLimitsAndSettings.bankAngle_Max;
      }
   }

   ProcessStandardLateralNavMode_Bank(commandedBankAngle_deg, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_RollHeading(double aHeading_deg, double aDT)
{
   double maxBankAngle_rad = mCurrentLimitsAndSettings.bankAngle_Max * UtMath::cRAD_PER_DEG;

   CalcLateralNavMode_RollHeadingCore(aHeading_deg, maxBankAngle_rad, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_WaypointRollHeading(double aHeading_deg, double aDT)
{
   double maxBankAngle_rad =
      std::min(mNavData.commandedBank_rad, (double)mCurrentLimitsAndSettings.bankAngle_Max * UtMath::cRAD_PER_DEG);

   CalcLateralNavMode_RollHeadingCore(aHeading_deg, maxBankAngle_rad, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_Bank(double aBankAngle_deg, double aDT)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedRollRate_dps = mLastCommandedRollRate_dps;

   // Get the desired bank angle
   double commandedBankAngle_deg = aBankAngle_deg;

   if (mExecuteLateralMiddleLoop)
   {
      // Get current states
      double currentBank_deg = mState.GetLocalRoll_deg();

      // Limit the commanded vert speed
      if (commandedBankAngle_deg < -mCurrentLimitsAndSettings.bankAngle_Max)
      {
         commandedBankAngle_deg = -mCurrentLimitsAndSettings.bankAngle_Max;
      }
      else if (commandedBankAngle_deg > mCurrentLimitsAndSettings.bankAngle_Max)
      {
         commandedBankAngle_deg = mCurrentLimitsAndSettings.bankAngle_Max;
      }

      double bankError_deg = UtMath::NormalizeAngleMinus180_180(commandedBankAngle_deg - currentBank_deg);

      // Get the desired g-load with g-limits
      commandedRollRate_dps = mBankAnglePID.CalcOutputFromErrorWithLimits(bankError_deg,
                                                                          aDT,
                                                                          -mCurrentLimitsAndSettings.rollRate_Max,
                                                                          mCurrentLimitsAndSettings.rollRate_Max);
   }

   // Execute Inner Loop
   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aDT);

   // Store last command for future
   mLastComandedBankAngle_deg = aBankAngle_deg;
}

void P6DofCommonController::ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps, double aDT)
{
   double currentRollRate_dps = mState.GetRollRate_dps();

   // Get the desired roll rate
   double commandedRollRate_dps = aCommandedRollRate_dps;

   // Store last command for future
   mLastCommandedRollRate_dps = aCommandedRollRate_dps;

   // Limit the command
   if (commandedRollRate_dps < -mCurrentLimitsAndSettings.rollRate_Max)
   {
      commandedRollRate_dps = -mCurrentLimitsAndSettings.rollRate_Max;
   }
   else if (commandedRollRate_dps > mCurrentLimitsAndSettings.rollRate_Max)
   {
      commandedRollRate_dps = mCurrentLimitsAndSettings.rollRate_Max;
   }

   // Execute Inner Loop
   mControlOutputsP6Dof.stickRight =
      mRollRatePID.CalcOutputFromTargetAndCurrent(commandedRollRate_dps, currentRollRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_Taxi_Waypoint(double aDT)
{
   double aimHeading_deg = 0;
   double turnRate_dps   = 0;

   GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(aDT, aimHeading_deg, turnRate_dps);

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiHeading(aimHeading_deg, turnRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_Taxi_Heading(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = (double)mCurrentActivityPtr->GetHeading_deg();

   double turnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);
   ;

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiHeading(commandedHeading_deg, turnRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_Taxi_YawRate(double aDT)
{
   // Get commanded rate
   double commandedYawRate_dps = mCurrentActivityPtr->GetYawRate_dps();

   // Process PIDs
   ProcessStandardLateralNavMode_TaxiYawRate(commandedYawRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_Beta(double aDT)
{
   double commandedBeta_deg = mCurrentActivityPtr->GetBeta_deg();
   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_YawGLoad(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return;
   }

   double commandedGLoad    = mCurrentActivityPtr->GetYawGLoad_g();
   double commandedBeta_deg = 0;

   mParentObjectPtr->CalculateBetaAtSpecifiedGLoad_deg(commandedGLoad, commandedBeta_deg);

   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_TaxiHeading(double aHeading_deg, double aTurnRate_dps, double aDT)
{
   // Get current states
   double currentHeading_deg = mState.GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer loop value (in case it is not executed this frame)
   double commandedYawRate_dps = mLastCommandedYawRate_dps;

   if (mExecuteLateralMiddleLoop)
   {
      // Calc the heading error
      double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

      // Get autopilot-based max turn rate based on max turn radius
      // Note that this uses mDesiredTaxiTurnRadius_ft which is >= mMinimumTaxiTurnRadius_ft
      double maxTurnRateBasedOnAutopilot_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);

      // Also limit the turn rate to AP yaw-rate limit
      double autopilotLimitedYawRate_dps = mCurrentLimitsAndSettings.taxiYawRate_Max;
      if (maxTurnRateBasedOnAutopilot_dps > autopilotLimitedYawRate_dps)
      {
         maxTurnRateBasedOnAutopilot_dps = autopilotLimitedYawRate_dps;
      }

      // Set min/max values
      double maxTurnRate_dps = std::abs(aTurnRate_dps);
      if (maxTurnRate_dps > maxTurnRateBasedOnAutopilot_dps)
      {
         maxTurnRate_dps = maxTurnRateBasedOnAutopilot_dps;
      }

      double minValue = -maxTurnRate_dps;
      double maxValue = maxTurnRate_dps;

      // Get the commanded yaw rate
      commandedYawRate_dps = mTaxiHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg, aDT, minValue, maxValue);
   }

   ProcessStandardLateralNavMode_TaxiYawRate(commandedYawRate_dps, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_TaxiYawRate(double aCommandedYawRate_dps, double aDT)
{
   P6DofLandingGear* gear = mParentObjectPtr->GetLandingGear();
   if (gear == nullptr)
   {
      return;
   }

   // Get the desired yaw rate
   double commandedYawRate_dps = aCommandedYawRate_dps;

   // Calc rate limit - note that this uses mDesiredTaxiTurnRadius_ft which is >= mMinimumTaxiTurnRadius_ft
   double maxTurnRate_dps = CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(mDesiredTaxiTurnRadius_ft);

   // Also limit the turn rate to AP yaw-rate limit
   double autopilotLimitedYawRate_dps = mCurrentLimitsAndSettings.taxiYawRate_Max;
   if (maxTurnRate_dps > autopilotLimitedYawRate_dps)
   {
      maxTurnRate_dps = autopilotLimitedYawRate_dps;
   }

   // Limit the command
   if (commandedYawRate_dps > maxTurnRate_dps)
   {
      commandedYawRate_dps = maxTurnRate_dps;
   }
   else if (commandedYawRate_dps < -maxTurnRate_dps)
   {
      commandedYawRate_dps = -maxTurnRate_dps;
   }

   double currentYawRate_dps = mState.GetYawRate_dps();

   // Execute Inner Loop

   double speed_fps = mState.GetSpeed_fps();

   // Note -- a negative turn radius implies a turn to the left
   double turnRadius_ft = 0.0;

   if (std::abs(commandedYawRate_dps) < 0.0000001)
   {
      // Essentially a zero rate -- this results in an infinite radius
      if (commandedYawRate_dps > 0)
      {
         turnRadius_ft = 1.0E20;
      }
      else
      {
         turnRadius_ft = -1.0E20;
      }
   }
   else
   {
      turnRadius_ft = speed_fps / (commandedYawRate_dps * UtMath::cRAD_PER_DEG);
   }

   // We need to limit how much the angle can be at very low speeds
   double spd_fps = mState.GetSpeed_fps(); // should be ground_speed_fps

   double speedGain = spd_fps / 1.0;
   if (speedGain < 1)
   {
      speedGain = 0;
   }
   if (speedGain > 1)
   {
      speedGain = 1;
   }

   // Get the nose gear
   P6DofGroundReactionPoint* noseGear = gear->GetNoseGear();
   if (noseGear == nullptr)
   {
      // We lack a nose gear, so return "zero"
      mControlOutputsP6Dof.rudderRight       = 0.0;
      mControlOutputsP6Dof.nwsSteering       = 0.0;
      mControlOutputsP6Dof.noseWheelSteering = 0.0;

      // Store last command for future
      mLastCommandedYawRate_dps = aCommandedYawRate_dps;

      return;
   }

   double currentSteeringAngle_deg = 0;
   double maxSteeringAngle_deg     = 0;
   noseGear->GetSteeringData(currentSteeringAngle_deg, maxSteeringAngle_deg);

   // Limit the turn radius (avoid infinite radius)
   if (std::abs(turnRadius_ft) > 1E10)
   {
      turnRadius_ft = (std::abs(turnRadius_ft) / turnRadius_ft) * 1E10;
   }

   // Get the steering angle
   double calculatedSteeringAngle_deg = 0;
   gear->SteeringAngleForDesiredRadiusFt_deg(turnRadius_ft, calculatedSteeringAngle_deg, maxSteeringAngle_deg);

   // Set the feed-forward (bias)
   double bias = calculatedSteeringAngle_deg / maxSteeringAngle_deg;
   mTaxiYawRatePID.SetBias(bias);

   double command =
      mTaxiYawRatePID.CalcOutputFromTargetAndCurrentWithLimits(commandedYawRate_dps, currentYawRate_dps, aDT, -1.0, 1.0);

   // At some point, we may want to add differential braking such as this:
   // if (command >  0.75) { mWheelBrakingRight = (command - 0.75) / 0.25; }
   // if (command < -0.75) { mWheelBrakingLeft  = (std::abs(command) - 0.75) / 0.25; }

   // Apply the speed gain to steering commands
   command *= speedGain;

   mControlOutputsP6Dof.rudderRight       = command;
   mControlOutputsP6Dof.nwsSteering       = command;
   mControlOutputsP6Dof.noseWheelSteering = command;

   // Store last command for future
   mLastCommandedYawRate_dps = aCommandedYawRate_dps;
}

void P6DofCommonController::ProcessStandardLateralNavMode_YawHeading(double aHeading_deg, double aDT)
{
   // Get current states
   double currentHeading_deg = mState.GetLocalHeading_deg();

   // Set the aim heading
   mAimHeading_rad = aHeading_deg * UtMath::cRAD_PER_DEG;

   // Set outer loop value (in case it is not executed this frame)
   double commandedYawRate_dps = mLastCommandedYawRate_dps;

   if (mExecuteLateralOuterLoop)
   {
      // Calc the heading error
      double hdgError_deg = UtMath::NormalizeAngleMinus180_180(aHeading_deg - currentHeading_deg);

      // Get the commanded roll/bank angle
      commandedYawRate_dps = mYawHeadingPID.CalcOutputFromErrorWithLimits(hdgError_deg,
                                                                          aDT,
                                                                          -mCurrentLimitsAndSettings.yawRate_Max,
                                                                          mCurrentLimitsAndSettings.yawRate_Max);
   }

   ProcessStandardLateralNavMode_YawRate(commandedYawRate_dps, aDT);
}

void P6DofCommonController::ProcessStandardLateralNavMode_YawRate(double aCommandedYawRate_dps, double aDT)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedBeta_deg = mLastCommandedBeta_deg;

   // Get the desired yaw rate
   double commandedYawRate_dps = aCommandedYawRate_dps;

   // Get rate limits
   double minRate = -mCurrentLimitsAndSettings.yawRate_Max;
   double maxRate = mCurrentLimitsAndSettings.yawRate_Max;

   // Limit the command
   if (commandedYawRate_dps < minRate)
   {
      commandedYawRate_dps = minRate;
   }
   else if (commandedYawRate_dps > maxRate)
   {
      commandedYawRate_dps = maxRate;
   }

   if (mExecuteLateralMiddleLoop)
   {
      // Get current states
      double currentYawRate_dps = mState.GetYawRate_dps();

      bool usingYawToTurn = (mControlMethod == P6DOF::Control::YawToTurnNoRoll) ||
                            (mControlMethod == P6DOF::Control::YawToTurnRollRate) ||
                            (mControlMethod == P6DOF::Control::YawToTurnZeroBank);

      if (usingYawToTurn)
      {
         double spd_fps                 = mState.GetSpeed_fps();
         double commandedYawRate_rps    = UtMath::cRAD_PER_DEG * commandedYawRate_dps;
         double targetAcceleration_fps2 = spd_fps * commandedYawRate_rps;
         double targetAcceleration_g    = targetAcceleration_fps2 / 32.174;

         // Convert g to beta
         double feedForwardBeta_deg = 0;
         mParentObjectPtr->CalculateBetaAtSpecifiedGLoad_deg(targetAcceleration_g, feedForwardBeta_deg);

         // Set bias
         mYawRatePID.SetBias(feedForwardBeta_deg);
      }

      commandedBeta_deg = mYawRatePID.CalcOutputFromTargetAndCurrentWithLimits(commandedYawRate_dps,
                                                                               currentYawRate_dps,
                                                                               aDT,
                                                                               -mLimitedBeta_deg,
                                                                               mLimitedBeta_deg);

      // Beta is opposite in sign compared to yaw rate
      if (!mUseLegacyBeta)
      {
         commandedBeta_deg *= -1.0;
      }
   }

   ProcessStandardLateralNavMode_Beta(commandedBeta_deg, aDT);

   // Store last command for future
   mLastCommandedYawRate_dps = aCommandedYawRate_dps;
}

void P6DofCommonController::ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aDT)
{
   // Get current states
   double currentBeta_deg = mState.GetBeta_deg();

   // Get the desired beta
   double commandedBeta_deg = aCommandedBeta_deg;

   // Limit the command
   if (commandedBeta_deg < -mLimitedBeta_deg)
   {
      commandedBeta_deg = -mLimitedBeta_deg;
   }
   else if (commandedBeta_deg > mLimitedBeta_deg)
   {
      commandedBeta_deg = mLimitedBeta_deg;
   }

   bool usingYawToTurn = (mControlMethod == P6DOF::Control::YawToTurnNoRoll) ||
                         (mControlMethod == P6DOF::Control::YawToTurnRollRate) ||
                         (mControlMethod == P6DOF::Control::YawToTurnZeroBank);

   if (mUseLegacyBeta)
   {
      double rudderRight = 0;

      if (usingYawToTurn)
      {
         mParentObjectPtr->CalculateStickBackForZeroMoment(commandedBeta_deg, rudderRight);
         rudderRight *= -1.0; // Beta has opposite sense to alpha
         mBetaPID.SetFeedForward(rudderRight);
      }

      // Execute Inner Loop
      mControlOutputsP6Dof.rudderRight = mBetaPID.CalcOutputFromTargetAndCurrent(commandedBeta_deg, currentBeta_deg, aDT);

      // Store last command for future
      mLastCommandedBeta_deg = commandedBeta_deg;
   }
   else
   {
      // Calculate the feed-forward for beta
      double rudderRight = 0;

      if (usingYawToTurn)
      {
         mParentObjectPtr->CalculateStickBackForZeroMoment(commandedBeta_deg, rudderRight);
         mBetaPID.SetFeedForward(rudderRight);
      }

      // Execute Inner Loop -- Notice the negation of the PID output (rudder command is opposite to beta)
      mControlOutputsP6Dof.rudderRight = -mBetaPID.CalcOutputFromTargetAndCurrent(commandedBeta_deg, currentBeta_deg, aDT);

      // Store last command for future
      mLastCommandedBeta_deg = commandedBeta_deg;
   }
}

void P6DofCommonController::ProcessLateralNavMode_YawWaypoint(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForWaypointNav_deg(aDT);

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_YawPoint(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = GetAimHeadingForPoint_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_YawHeading(double aDT)
{
   // Get commanded heading
   double commandedHeading_deg = (double)mCurrentActivityPtr->GetHeading_deg();

   // Process PIDs
   ProcessStandardLateralNavMode_YawHeading(commandedHeading_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_YawRate(double aDT)
{
   double commandedYawRate_dps = mCurrentActivityPtr->GetYawRate_dps();

   ProcessStandardLateralNavMode_YawRate(commandedYawRate_dps, aDT);
}

P6DofPID* P6DofCommonController::GetPID_ByType(P6DOF::Pid::Type aPidType)
{
   switch (aPidType)
   {
   case P6DOF::Pid::Alpha:
      return &mAlphaPID;
   case P6DOF::Pid::VertSpeed:
      return &mVertSpeedPID;
   case P6DOF::Pid::PitchAngle:
      return &mPitchAnglePID;
   case P6DOF::Pid::PitchRate:
      return &mPitchRatePID;
   case P6DOF::Pid::FltPathAngle:
      return &mFltpathAnglePID;
   case P6DOF::Pid::DeltaPitch:
      return &mDeltaPitchPID;
   case P6DOF::Pid::Altitude:
      return &mAltitudePID;
   case P6DOF::Pid::Beta:
      return &mBetaPID;
   case P6DOF::Pid::YawRate:
      return &mYawRatePID;
   case P6DOF::Pid::YawHeading:
      return &mYawHeadingPID;
   case P6DOF::Pid::TaxiHeading:
      return &mTaxiHeadingPID;
   case P6DOF::Pid::RollRate:
      return &mRollRatePID;
   case P6DOF::Pid::DeltaRoll:
      return &mDeltaRollPID;
   case P6DOF::Pid::BankAngle:
      return &mBankAnglePID;
   case P6DOF::Pid::RollHeading:
      return &mRollHeadingPID;
   case P6DOF::Pid::ForwardAccel:
      return &mForwardAccelPID;
   case P6DOF::Pid::Speed:
      return &mSpeedPID;
   case P6DOF::Pid::TaxiForwardAccel:
      return &mTaxiForwardAccelPID;
   case P6DOF::Pid::TaxiSpeed:
      return &mTaxiSpeedPID;
   case P6DOF::Pid::TaxiYawRate:
      return &mTaxiYawRatePID;
   default:
      return nullptr;
   }
}

void P6DofCommonController::ProcessLateralNavMode_RollRate(double aDT)
{
   double commandedRollRate_dps = (double)mCurrentActivityPtr->GetRollRate_dps();

   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_Bank(double aDT)
{
   double comandedBankAngle_deg = mCurrentActivityPtr->GetBank_deg();

   ProcessStandardLateralNavMode_Bank(comandedBankAngle_deg, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_DeltaRoll(double aDT)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedRollRate_dps = mLastCommandedRollRate_dps;

   // Get the desired angle
   double commandedDeltaRollAngle_deg = mCurrentActivityPtr->GetRollDelta_deg();

   if (mExecuteLateralMiddleLoop)
   {
      // Get current states
      double currentRollDelta_deg = mIntegratedDeltaRoll_deg;

      // Get the desired g-load with g-limits
      commandedRollRate_dps =
         mDeltaRollPID.CalcOutputFromTargetAndCurrentWithLimits(commandedDeltaRollAngle_deg,
                                                                currentRollDelta_deg,
                                                                aDT,
                                                                -mCurrentLimitsAndSettings.rollRate_Max,
                                                                mCurrentLimitsAndSettings.rollRate_Max);
   }

   // Execute Inner Loop
   ProcessStandardLateralNavMode_RollRate(commandedRollRate_dps, aDT);
}

void P6DofCommonController::ProcessLateralNavMode_NoControl()
{
   ProcessLateralNavMode_NoRollControl();
   ProcessLateralNavMode_NoYawControl();
}

void P6DofCommonController::ProcessLateralNavMode_NoYawControl()
{
   mControlOutputsP6Dof.rudderRight = 0.0f;
}

void P6DofCommonController::ProcessLateralNavMode_NoRollControl()
{
   mControlOutputsP6Dof.stickRight = 0.0f;
}

void P6DofCommonController::ProcessVerticalNavMode_Waypoint(double aDT)
{
   double commandedAltMSL_ft = 0.0;

   // If previous turn is cleared and there is a current waypoint
   if (!mNavData.executeTurn && mCurrentActivityPtr->GetCurrWaypoint() != nullptr)
   {
      commandedAltMSL_ft = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt() * UtMath::cFT_PER_M;
   }
   else if (mCurrentActivityPtr->GetPrevWaypoint() != nullptr)
   {
      commandedAltMSL_ft = mCurrentActivityPtr->GetPrevWaypoint()->GetLLA().GetAlt() * UtMath::cFT_PER_M;
   }
   else
   {
      commandedAltMSL_ft = 0.0;
   }

   if (mCurrentActivityPtr->GetCurrWaypoint() != nullptr)
   {
      // Commanded to follow a vertical track
      if (mCurrentActivityPtr->GetCurrWaypoint()->FollowVerticalTrack())
      {
         P6DofRoute::CalcVerticalSpeed(mNavData);

         double vertRate_fpm = (mNavData.vertSpeed_mps * UtMath::cFT_PER_M * 60.0);

         // If the current waypoint is at a different altitude than the previous waypoint, command the
         // vertical rate needed to follow the track
         if (mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt() !=
             mCurrentActivityPtr->GetPrevWaypoint()->GetLLA().GetAlt())
         {
            ProcessStandardVerticalNavMode_VertSpeed(vertRate_fpm, aDT);
         }
         else
         {
            // No change in altitude, so just maintain altitude
            ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aDT);
         }
      }
      else // Default to attaining target altitude as quickly as possible
      {
         // Follow normal approach to waypoint altitude
         //  commandedAltMSL_ft = mCurrentActivityPtr->GetCurrWaypoint()->GetLLA().GetAlt()*UtMath::cFT_PER_M;
         ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aDT);
      }
   }
   else
   {
      // No waypoint is set, so maintain current altitude
      ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aDT);
   }
}

void P6DofCommonController::ProcessVerticalNavMode_Altitude(double aDT)
{
   double commandedAltMSL_ft = (double)mCurrentActivityPtr->GetAltitudeMSL_ft();
   ProcessStandardVerticalNavMode_Altitude(commandedAltMSL_ft, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_VertSpeed(double aDT)
{
   double commandedVertRate_fpm = mCurrentActivityPtr->GetVerticalRate_fpm();
   ProcessStandardVerticalNavMode_VertSpeed(commandedVertRate_fpm, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_PitchGLoad(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return;
   }

   double commandedGLoad     = mCurrentActivityPtr->GetPitchGLoad_g();
   double commandedAlpha_deg = 0;

   mParentObjectPtr->CalculateAlphaAtSpecifiedGLoad_deg(commandedGLoad, commandedAlpha_deg);

   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessStandardVerticalNavMode_Altitude(double aAltitude_ft, double aDT)
{
   // Set outer and middle loop values (in case they are not executed this frame)
   double commandedVertRate_fpm = mLastCommandedVertRate_fpm;

   if (mExecuteVerticalOuterLoop)
   {
      // Get current states
      double currentAlt_ft = mState.GetAlt_ft();

      // Get the desired altitude
      double commandedAltMSL_ft = aAltitude_ft;

      // Get the commanded vertical speed with vert speed limits
      commandedVertRate_fpm =
         mAltitudePID.CalcOutputFromTargetAndCurrentWithLimits(commandedAltMSL_ft,
                                                               currentAlt_ft,
                                                               aDT,
                                                               mCurrentLimitsAndSettings.vertSpd_Min,
                                                               mCurrentLimitsAndSettings.vertSpd_Max);
   }

   // Execute Middle and Inner Loops
   ProcessStandardVerticalNavMode_VertSpeed(commandedVertRate_fpm, aDT);
}

void P6DofCommonController::CalcGBiasData(double& aGBias_g, double& aGBiasAlpha_deg)
{
   // We start by setting the g-bias (in terms of g-load) to unity
   aGBias_g = 1.0;

   // Get current states
   double pitchAngle_rad = mState.GetLocalPitch_rad();
   double rollAngle_rad  = mState.GetLocalRoll_rad();

   // Include the G-bias due to pitch angle
   double cosRoll = cos(rollAngle_rad);

   if (cosRoll == 0.0)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Max;
   }
   else
   {
      aGBias_g = 1.0 / cosRoll;
   }

   // Include the G-bias due to pitch angle
   aGBias_g *= cos(pitchAngle_rad);

   // Limit g
   if (aGBias_g > mCurrentLimitsAndSettings.pitchGLoad_Max)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Max;
   }

   if (aGBias_g < mCurrentLimitsAndSettings.pitchGLoad_Min)
   {
      aGBias_g = mCurrentLimitsAndSettings.pitchGLoad_Min;
   }

   // Get the alpha for the given g-load
   mParentObjectPtr->CalculateAlphaAtSpecifiedGLoad_deg(aGBias_g, aGBiasAlpha_deg);
}

void P6DofCommonController::ProcessStandardVerticalNavMode_VertSpeed(double aCommandedVertSpeed_fpm, double aDT)
{
   // Set middle loop value (in case it is not executed this frame)
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   // Get the desired vertical speed
   double commandedVertRate_fpm = aCommandedVertSpeed_fpm;

   // Limit the commanded vert speed
   if (commandedVertRate_fpm < mCurrentLimitsAndSettings.vertSpd_Min)
   {
      commandedVertRate_fpm = mCurrentLimitsAndSettings.vertSpd_Min;
   }
   else if (commandedVertRate_fpm > mCurrentLimitsAndSettings.vertSpd_Max)
   {
      commandedVertRate_fpm = mCurrentLimitsAndSettings.vertSpd_Max;
   }

   if (mExecuteVerticalMiddleLoop)
   {
      // Get current states
      double currentVertSpeed_fpm = mState.GetVerticalSpeed_fpm();

      // Set bias
      mVertSpeedPID.SetBias(mCurrentGBiasAlpha_deg);

      // Get the desired g-load with g-limits
      commandedAlpha_deg = mVertSpeedPID.CalcOutputFromTargetAndCurrentWithLimits(commandedVertRate_fpm,
                                                                                  currentVertSpeed_fpm,
                                                                                  aDT,
                                                                                  mLimitedMinAlpha_deg,
                                                                                  mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);

   // Store last command for future
   mLastCommandedVertRate_fpm = aCommandedVertSpeed_fpm;
}

void P6DofCommonController::ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aDT)
{
   // Get current states
   double currentAlpha_deg = mState.GetAlpha_deg();

   // Get the desired alpha
   double commandedAlpha_deg = aCommandedAlpha_deg;

   // Limit the command using the g-limits calculated in the Update function
   if (commandedAlpha_deg < mLimitedMinAlpha_deg)
   {
      commandedAlpha_deg = mLimitedMinAlpha_deg;
   }
   if (commandedAlpha_deg > mLimitedMaxAlpha_deg)
   {
      commandedAlpha_deg = mLimitedMaxAlpha_deg;
   }

   double stickBack = 0;
   mParentObjectPtr->CalculateStickBackForZeroMoment(commandedAlpha_deg, stickBack);
   mAlphaPID.SetFeedForward(stickBack);

   // Execute Inner Loop
   mControlOutputsP6Dof.stickBack = mAlphaPID.CalcOutputFromTargetAndCurrent(commandedAlpha_deg, currentAlpha_deg, aDT);
   // Store last command for future
   mLastCommandedAlpha_deg = aCommandedAlpha_deg;
}

void P6DofCommonController::ProcessVerticalNavMode_Taxi(double) // aDT)
{
}

void P6DofCommonController::ProcessVerticalNavMode_Point(double) // aDT)
{
}

void P6DofCommonController::ProcessVerticalNavMode_PitchAng(double aDT)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   if (mExecuteVerticalMiddleLoop)
   {
      // Set outer and middle loop values (in case they are not executed this frame)
      double currentPitchAngle_deg = mState.GetLocalPitch_deg();

      // Get the desired angle
      double commandedPitchAngle_deg = mCurrentActivityPtr->GetPitchAngle_deg();

      // Limit the command using +/-90 deg
      if (commandedPitchAngle_deg < -90.0)
      {
         commandedPitchAngle_deg = -90.0;
      }
      if (commandedPitchAngle_deg > 90.0)
      {
         commandedPitchAngle_deg = 90.0;
      }

      // Set bias
      mPitchAnglePID.SetBias(mCurrentGBiasAlpha_deg);

      // Get the desired pitch rate within limits
      commandedAlpha_deg = mPitchAnglePID.CalcOutputFromTargetAndCurrentWithLimits(commandedPitchAngle_deg,
                                                                                   currentPitchAngle_deg,
                                                                                   aDT,
                                                                                   mLimitedMinAlpha_deg,
                                                                                   mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_PitchRate(double aDT)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   if (mExecuteVerticalMiddleLoop)
   {
      // Set outer and middle loop values (in case they are not executed this frame)
      double currentPitchRate_dps = mState.GetPitchRate_dps();

      // Get the desired rate
      double commandedPitchRate_dps = mCurrentActivityPtr->GetPitchRate_dps();

      // Limit the command

      if (commandedPitchRate_dps < mCurrentLimitsAndSettings.pitchRate_Min)
      {
         commandedPitchRate_dps = mCurrentLimitsAndSettings.pitchRate_Min;
      }

      if (commandedPitchRate_dps > mCurrentLimitsAndSettings.pitchRate_Max)
      {
         commandedPitchRate_dps = mCurrentLimitsAndSettings.pitchRate_Max;
      }

      double spd_fps                 = mState.GetSpeed_fps();
      double commandedPitchRate_rps  = UtMath::cRAD_PER_DEG * commandedPitchRate_dps;
      double targetAcceleration_fps2 = spd_fps * commandedPitchRate_rps;
      double targetAcceleration_g    = targetAcceleration_fps2 / 32.174;

      if (mControlMethod == P6DOF::Control::BankToTurnNoYaw || mControlMethod == P6DOF::Control::BankToTurnWithYaw)
      {
         targetAcceleration_g += 1.0;
      }

      // Convert g to alpha
      double feedForwardAlpha_deg = 0;
      mParentObjectPtr->CalculateAlphaAtSpecifiedGLoad_deg(targetAcceleration_g, feedForwardAlpha_deg);

      // Set bias
      mPitchRatePID.SetBias(feedForwardAlpha_deg);

      // Get the desired pitch rate within limits
      commandedAlpha_deg = mPitchRatePID.CalcOutputFromTargetAndCurrentWithLimits(commandedPitchRate_dps,
                                                                                  currentPitchRate_dps,
                                                                                  aDT,
                                                                                  mLimitedMinAlpha_deg,
                                                                                  mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_FltPathAng(double aDT)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   if (mExecuteVerticalMiddleLoop)
   {
      // Set outer and middle loop values (in case they are not executed this frame)
      double currentFltPathAngle_deg = mState.GetFlightPathAngle_deg();

      // Get the desired angle
      double commandedFltPathAngle_deg = mCurrentActivityPtr->GetFlightPathAngle_deg();

      // Limit the command using +/-90 deg
      if (commandedFltPathAngle_deg < -90.0)
      {
         commandedFltPathAngle_deg = -90.0;
      }
      if (commandedFltPathAngle_deg > 90.0)
      {
         commandedFltPathAngle_deg = 90.0;
      }

      // Set bias
      mFltpathAnglePID.SetBias(mCurrentGBiasAlpha_deg);

      // Get the desired pitch rate within limits
      commandedAlpha_deg = mFltpathAnglePID.CalcOutputFromTargetAndCurrentWithLimits(commandedFltPathAngle_deg,
                                                                                     currentFltPathAngle_deg,
                                                                                     aDT,
                                                                                     mLimitedMinAlpha_deg,
                                                                                     mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_DeltaPitch(double aDT)
{
   // Set inner loop value
   double commandedAlpha_deg = mLastCommandedAlpha_deg;

   if (mExecuteVerticalMiddleLoop)
   {
      // Get current states
      double currentPitchDelta_deg = mIntegratedDeltaPitch_deg;

      // Get the desired angle
      double commandedDeltaPitchAngle_deg = mCurrentActivityPtr->GetDeltaPitch_deg();

      // Set bias
      mDeltaPitchPID.SetBias(mCurrentGBias_g);

      // Get the desired pitch rate within limits
      commandedAlpha_deg = mDeltaPitchPID.CalcOutputFromTargetAndCurrentWithLimits(commandedDeltaPitchAngle_deg,
                                                                                   currentPitchDelta_deg,
                                                                                   aDT,
                                                                                   mLimitedMinAlpha_deg,
                                                                                   mLimitedMaxAlpha_deg);
   }

   // Execute Inner Loop
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_Alpha(double aDT)
{
   double commandedAlpha_deg = mCurrentActivityPtr->GetAlpha_deg();
   ProcessStandardVerticalNavMode_Alpha(commandedAlpha_deg, aDT);
}

void P6DofCommonController::ProcessVerticalNavMode_NoControl(double) // aDT)
{
   mControlOutputsP6Dof.stickBack = 0.0f;
}

double P6DofCommonController::ProcessSpeedMode_Waypoint(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   } // Return if we lack a parent

   if (mAtmosphere == nullptr)
   {
      return 0.0;
   } // Return if we lack an atm

   // Get current states
   double tgtSpeed_fps = mState.GetSpeed_fps();

   P6DofWaypoint::sSpeed speed;

   // Either still executing turn or done with route, so use last waypoint speed
   if (mNavData.executeTurn || mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      if (mCurrentActivityPtr->GetPrevWaypoint() != nullptr)
      {
         speed = mCurrentActivityPtr->GetPrevWaypoint()->GetSpeed();
      }
      else
      {
         speed.type = P6DofWaypoint::SPD_TYPE_FPS;
         speed.val  = 0.0;
      }
   }
   else
   {
      // Turn complete, so align speed with next waypoint speed
      speed = mCurrentActivityPtr->GetCurrWaypoint()->GetSpeed();
   }

   double currentAlt_ft = mState.GetAlt_ft();

   switch (speed.type)
   {
   case P6DofWaypoint::SPD_TYPE_MACH:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromMach(currentAlt_ft, speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_TAS_KNOTS:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromKtas(speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_CAS_KNOTS:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromKcas(currentAlt_ft, speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_FPS:
      tgtSpeed_fps = speed.val;
      break;
   default:
      tgtSpeed_fps = 0.0;
      break;
   }

   return ProcessStandardSpeedMode_FPS(tgtSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_ForwardAccel(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double commandedAccel_g = mCurrentActivityPtr->GetForwardAcceleration_g();

   return ProcessStandardForwardAccel(commandedAccel_g, aDT);
}

double P6DofCommonController::ProcessStandardSpeedMode_FPS(double aSpeed_fps, double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double currentSpeed_fps = mState.GetSpeed_fps();

   double commandedSpeed_fps = aSpeed_fps;

   double biasThrottle = 0.0;
   double drag         = mParentObjectPtr->GetDrag_lbs();
   double alpha_rad    = mParentObjectPtr->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle     = cos(alpha_rad);
   double maxThrust    = mParentObjectPtr->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust    = mParentObjectPtr->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust  = maxThrust - minThrust;

   if (drag > maxThrust)
   {
      biasThrottle = 1.0;
   }
   else if (drag < minThrust)
   {
      biasThrottle = -1.0;
   }
   else
   {
      if (deltaThrust == 0.0)
      {
         biasThrottle = 0.0;
      }
      else
      {
         biasThrottle = (drag - minThrust) / deltaThrust;
      }
   }

   mSpeedPID.SetBias(biasThrottle);

   double minOutput = -1.0;
   double maxOutput = 2.0;

   double commandedThrottle =
      mSpeedPID.CalcOutputFromTargetAndCurrentWithLimits(commandedSpeed_fps, currentSpeed_fps, aDT, minOutput, maxOutput);
   return commandedThrottle;
}

double P6DofCommonController::ProcessStandardForwardAccel(double aForwardAccel_g, double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   // TODO --- For now use Nx, but that is not accurate
   double currentAccel = mState.GetNx_g();
   double biasThrottle = 0.0;
   double drag         = mParentObjectPtr->GetDrag_lbs();
   double alpha_rad    = mParentObjectPtr->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle     = cos(alpha_rad);
   double maxThrust    = mParentObjectPtr->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust    = mParentObjectPtr->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust  = maxThrust - minThrust;

   if (drag > maxThrust)
   {
      biasThrottle = 1.0;
   }
   else if (drag < minThrust)
   {
      biasThrottle = -1.0;
   }
   else
   {
      if (deltaThrust == 0.0)
      {
         biasThrottle = 0.0;
      }
      else
      {
         biasThrottle = (drag - minThrust) / deltaThrust;
      }
   }

   mForwardAccelPID.SetBias(biasThrottle);

   // Calc limits
   double accelMax_g = (double)mCurrentLimitsAndSettings.forwardAccel_Max;
   double accelMin_g = (double)mCurrentLimitsAndSettings.forwardAccel_Min;

   double commandedAccel_g = aForwardAccel_g;
   if (aForwardAccel_g > accelMax_g)
   {
      commandedAccel_g = accelMax_g;
   }
   if (aForwardAccel_g < accelMin_g)
   {
      commandedAccel_g = accelMin_g;
   }

   // Execute Inner Loop
   double speedCommand_g = mForwardAccelPID.CalcOutputFromTargetAndCurrent(commandedAccel_g, currentAccel, aDT);
   return speedCommand_g;
}

double P6DofCommonController::ProcessStandardSpeedModeTaxi_FPS(double aSpeed_fps, double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double currentSpeed_fps = mState.GetSpeed_fps();

   double commandedSpeed_fps = aSpeed_fps;
   double biasThrottle       = 0.0;
   double drag               = mParentObjectPtr->GetDrag_lbs();
   double alpha_rad          = mParentObjectPtr->GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double cosAngle           = cos(alpha_rad);
   double maxThrust          = mParentObjectPtr->GetMaximumPotentialThrust_lbs() * cosAngle;
   double minThrust          = mParentObjectPtr->GetMinimumPotentialThrust_lbs() * cosAngle;
   double deltaThrust        = maxThrust - minThrust;

   if (drag > maxThrust)
   {
      biasThrottle = 1.0;
   }
   else if (drag < minThrust)
   {
      biasThrottle = -1.0;
   }
   else
   {
      if (deltaThrust == 0.0)
      {
         biasThrottle = 0.0;
      }
      else
      {
         biasThrottle = (drag - minThrust) / deltaThrust;
      }
   }

   mSpeedPID.SetBias(biasThrottle);

   double minOutput = -1.0;
   double maxOutput = 2.0;

   double commandedThrottle =
      mTaxiSpeedPID.CalcOutputFromTargetAndCurrentWithLimits(commandedSpeed_fps, currentSpeed_fps, aDT, minOutput, maxOutput);
   return commandedThrottle;
}

double P6DofCommonController::ProcessSpeedMode_KIAS(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double alt_ft             = mState.GetAlt_ft();
   double commmanded_kias    = mCurrentActivityPtr->GetCalibratedAirSpeed_KCAS();
   double commandedSpeed_fps = mAtmosphere->CalcFpsFromKcas(alt_ft, commmanded_kias);

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_KTAS(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double commmanded_ktas    = mCurrentActivityPtr->GetTrueAirSpeed_KTAS();
   double commandedSpeed_fps = mAtmosphere->CalcFpsFromKtas(commmanded_ktas);

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_Mach(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   }

   double alt_ft             = mState.GetAlt_ft();
   double commmanded_mach    = mCurrentActivityPtr->GetMach();
   double commandedSpeed_fps = mAtmosphere->CalcFpsFromMach(alt_ft, commmanded_mach);

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_FPS(double aDT)
{
   double commandedSpeed_fps = (double)mCurrentActivityPtr->GetSpeed_fps();

   return ProcessStandardSpeedMode_FPS(commandedSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_Taxi_Waypoint(double aDT)
{
   if (mParentObjectPtr == nullptr)
   {
      return 0.0;
   } // Return if we lack a parent

   if (mAtmosphere == nullptr)
   {
      return 0.0;
   } // Return if we lack an atm

   // Get current states
   double tgtSpeed_fps = mState.GetSpeed_fps();

   P6DofWaypoint::sSpeed speed;

   // Either still executing turn or done with route, so use last waypoint speed
   if (mNavData.executeTurn || mCurrentActivityPtr->GetCurrWaypoint() == nullptr)
   {
      if (mCurrentActivityPtr->GetPrevWaypoint() != nullptr)
      {
         speed = mCurrentActivityPtr->GetPrevWaypoint()->GetSpeed();
      }
      else
      {
         speed.type = P6DofWaypoint::SPD_TYPE_FPS;
         speed.val  = 0.0;
      }
   }
   else
   {
      // Turn complete, so align speed with next waypoint speed
      speed = mCurrentActivityPtr->GetCurrWaypoint()->GetSpeed();
   }

   double currentAlt_ft = mState.GetAlt_ft();

   switch (speed.type)
   {
   case P6DofWaypoint::SPD_TYPE_MACH:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromMach(currentAlt_ft, speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_TAS_KNOTS:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromKtas(speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_CAS_KNOTS:
      tgtSpeed_fps = mAtmosphere->CalcFpsFromKcas(currentAlt_ft, speed.val);
      break;
   case P6DofWaypoint::SPD_TYPE_FPS:
      tgtSpeed_fps = speed.val;
      break;
   default:
      tgtSpeed_fps = 0.0;
      break;
   }

   return ProcessStandardSpeedModeTaxi_FPS(tgtSpeed_fps, aDT);
}

double P6DofCommonController::ProcessSpeedMode_Taxi_ForwardAccel(double) // aDT)
{
   return 0.0;
}

double P6DofCommonController::ProcessSpeedMode_Taxi_KIAS(double) // aDT)
{
   return 0.0;
}

double P6DofCommonController::ProcessSpeedMode_Taxi_KTAS(double) // aDT)
{
   return 0.0;
}

double P6DofCommonController::ProcessSpeedMode_NoControl()
{
   // Previously, we kept the throttle set to full with no control
   // mControlOutputsP6Dof.throttleAfterburner = 1.0f;
   // mControlOutputsP6Dof.throttleMilitary    = 1.0f;
   // Now, we do not move the throttle at all -- we do nothing

   return 0.0;
}

void P6DofCommonController::CalcAlphaBetaGLimits()
{
   // Calculate the g-bias (in g-load and alpha) to hold 1-g, given
   // the current pitch and roll
   CalcGBiasData(mCurrentGBias_g, mCurrentGBiasAlpha_deg);

   // Direct alpha limits
   double alphaMin_deg = (double)mCurrentLimitsAndSettings.alpha_Min;
   double alphaMax_deg = (double)mCurrentLimitsAndSettings.alpha_Max;

   // G-load-induced alpha limits
   double pitchGLoadMax   = (double)mCurrentLimitsAndSettings.pitchGLoad_Max;
   double alphaAtMaxG_deg = 0;
   mParentObjectPtr->CalculateAlphaAtSpecifiedGLoad_deg(pitchGLoadMax, alphaAtMaxG_deg);
   double pitchGLoadMin   = (double)mCurrentLimitsAndSettings.pitchGLoad_Min;
   double alphaAtMinG_deg = 0;
   mParentObjectPtr->CalculateAlphaAtSpecifiedGLoad_deg(pitchGLoadMin, alphaAtMinG_deg);

   // We now need to determine the alpha limits, considering both direct
   // alpha limits and g-load induced alpha limits

   if (alphaAtMaxG_deg < alphaMax_deg)
   {
      // Use the restrained g-load alpha instead
      mLimitedMaxAlpha_deg = alphaAtMaxG_deg;

      if (alphaAtMinG_deg > alphaMin_deg)
      {
         // Use the restrained g-load alpha instead
         mLimitedMinAlpha_deg = alphaAtMinG_deg;
      }
      else
      {
         mLimitedMinAlpha_deg = alphaMin_deg;
      }
   }
   else
   {
      mLimitedMaxAlpha_deg = alphaMax_deg;

      if (alphaAtMinG_deg > alphaMin_deg)
      {
         // Use the restrained g-load alpha instead
         mLimitedMinAlpha_deg = alphaAtMinG_deg;
      }
      else
      {
         mLimitedMinAlpha_deg = alphaMin_deg;
      }
   }

   // Check for strange limits
   if (mLimitedMaxAlpha_deg < mLimitedMinAlpha_deg)
   {
      mLimitedMinAlpha_deg = mLimitedMaxAlpha_deg;
   }

   // Direct beta limits
   double betaMax_deg = (double)mCurrentLimitsAndSettings.beta_Max;

   if (mControlMethod == P6DOF::Control::YawToTurnNoRoll || mControlMethod == P6DOF::Control::YawToTurnRollRate ||
       mControlMethod == P6DOF::Control::YawToTurnZeroBank)
   {
      // G-load-induced beta limits
      double yawGLoadMax    = (double)mCurrentLimitsAndSettings.yawGLoad_Max;
      double betaAtMaxG_deg = 0;
      mParentObjectPtr->CalculateBetaAtSpecifiedGLoad_deg(yawGLoadMax, betaAtMaxG_deg);
      betaAtMaxG_deg = std::abs(betaAtMaxG_deg); // Enforce positive

      // We now need to determine the beta limits, considering both
      // direct beta limits and g-load induced beta limits
      if (betaAtMaxG_deg < betaMax_deg)
      {
         mLimitedBeta_deg = betaAtMaxG_deg;
      }
      else
      {
         mLimitedBeta_deg = betaMax_deg;
      }
   }
   else
   {
      mLimitedBeta_deg = betaMax_deg;
   }
}

void P6DofCommonController::SetDesiredTaxiRadius(double aTaxiRadius_ft)
{
   if (aTaxiRadius_ft <= mMinimumTaxiTurnRadius_ft)
   {
      // Limit to the minimum
      mDesiredTaxiTurnRadius_ft = mMinimumTaxiTurnRadius_ft;
      auto out                  = ut::log::warning() << "Requested taxi radius too low. Using minimum radius instead.";
      out.AddNote() << "Requested: " << aTaxiRadius_ft << " ft";
      out.AddNote() << "Minimum: " << mMinimumTaxiTurnRadius_ft << " ft";
   }
   else
   {
      mDesiredTaxiTurnRadius_ft = aTaxiRadius_ft;
   }
}

void P6DofCommonController::SetControllingValueForAllPIDs(double aControllingValue)
{
   mAlphaPID.SetControllingValue(aControllingValue);
   mVertSpeedPID.SetControllingValue(aControllingValue);
   mPitchAnglePID.SetControllingValue(aControllingValue);
   mPitchRatePID.SetControllingValue(aControllingValue);
   mFltpathAnglePID.SetControllingValue(aControllingValue);
   mDeltaPitchPID.SetControllingValue(aControllingValue);
   mAltitudePID.SetControllingValue(aControllingValue);
   mBetaPID.SetControllingValue(aControllingValue);
   mYawRatePID.SetControllingValue(aControllingValue);
   mYawHeadingPID.SetControllingValue(aControllingValue);
   mTaxiHeadingPID.SetControllingValue(aControllingValue);
   mRollRatePID.SetControllingValue(aControllingValue);
   mDeltaRollPID.SetControllingValue(aControllingValue);
   mBankAnglePID.SetControllingValue(aControllingValue);
   mRollHeadingPID.SetControllingValue(aControllingValue);
   mForwardAccelPID.SetControllingValue(aControllingValue);
   mSpeedPID.SetControllingValue(aControllingValue);
   mTaxiForwardAccelPID.SetControllingValue(aControllingValue);
   mTaxiSpeedPID.SetControllingValue(aControllingValue);
   mTaxiYawRatePID.SetControllingValue(aControllingValue);
}

bool P6DofCommonController::GetCLMaxMach(double aMach, double& aCLMax) const
{
   if (mCLMaxMachTablePtr == nullptr)
   {
      return false;
   }

   aCLMax = mCLMaxMachTablePtr->Lookup(aMach);
   return true;
}

bool P6DofCommonController::GetCLMinMach(double aMach, double& aCLMin) const
{
   if (mCLMinMachTablePtr == nullptr)
   {
      return false;
   }

   aCLMin = mCLMinMachTablePtr->Lookup(aMach);
   return true;
}

bool P6DofCommonController::GetAlphaMaxMach(double aMach, double& aAlphaMax_deg) const
{
   if (mAlphaMaxMachTablePtr == nullptr)
   {
      return false;
   }

   aAlphaMax_deg = mAlphaMaxMachTablePtr->Lookup(aMach);
   return true;
}

bool P6DofCommonController::GetAlphaMinMach(double aMach, double& aAlphaMin_deg) const
{
   if (mAlphaMinMachTablePtr == nullptr)
   {
      return false;
   }

   aAlphaMin_deg = mAlphaMinMachTablePtr->Lookup(aMach);
   return true;
}

bool P6DofCommonController::GetStickForZeroMomentVsMachAlpha(double  aMach,
                                                             double  aAlpha_rad,
                                                             double  aThrust_lbs,
                                                             double& aStickBack) const
{
   if (mStickForZeroMomentVsMachAlphaTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;

   aStickBack = mStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);

   if (aStickBack == 1.0 || aStickBack == -1.0)
   {
      // Stick command was limited to +/-1.0, so leave it alone
      return true;
   }

   if (mParentObjectPtr)
   {
      const P6DofMassProperties& massProperties = mParentObjectPtr->GetMassProperties();

      if (mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         // Account for the change in moment caused by thrust and thrust vectoring
         double referenceMass_lbs = massProperties.GetBaseMass_lbs();

         double deltaStickBack = mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * aThrust_lbs / referenceMass_lbs;
      }

      UtVec3dX changeInCenterOfGravity_ft =
         massProperties.GetCmPosRelToRef_ft() - massProperties.GetBaseCmPosRelToRef_ft();

      if (mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         double deltaStickBack = mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * changeInCenterOfGravity_ft.X();
      }

      if (mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr)
      {
         double deltaStickBack = mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr->Lookup(lookupArgs);
         aStickBack += deltaStickBack * changeInCenterOfGravity_ft.Z();
      }
   }

   return true;
}

bool P6DofCommonController::GetEffectiveCLVsMachAlpha(double aMach, double aAlpha_rad, double& aCL) const
{
   if (mEffectiveCLVsMachAlphaTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aAlpha_rad;
   aCL           = mEffectiveCLVsMachAlphaTablePtr->Lookup(lookupArgs);
   return true;
}

bool P6DofCommonController::GetAlphaVsMachCL(double aMach, double aCL, double& aAlpha_deg) const
{
   if (mAlphaVsMachCLTablePtr == nullptr)
   {
      return false;
   }

   double lookupArgs[2];
   lookupArgs[0] = aMach;
   lookupArgs[1] = aCL;
   aAlpha_deg    = mAlphaVsMachCLTablePtr->Lookup(lookupArgs);
   return true;
}
