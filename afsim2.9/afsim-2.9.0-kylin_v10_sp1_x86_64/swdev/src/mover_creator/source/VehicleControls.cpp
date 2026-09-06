// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VehicleControls.hpp"

namespace Designer
{

void VehicleControls::GetPilotTypes(bool&                    aSyntheticPilotPresent,
                                    bool&                    aManualPilotPresent,
                                    bool&                    aHardwareAutopilotPresent,
                                    bool&                    aGuidanceSystemPresent,
                                    VehicleControlPilotType& aActivePilot)
{
   aSyntheticPilotPresent    = mSyntheticPilotPresent;
   aManualPilotPresent       = mManualPilotPresent;
   aHardwareAutopilotPresent = mHardwareAutopilotPresent;
   aGuidanceSystemPresent    = mGuidanceSystemPresent;
   aActivePilot              = mActivePilot;
}

void VehicleControls::GetBankToTurnControlSettings(VehicleControlBankToTurnRollControl&    aBankControls,
                                                   VehicleControlBankToTurnYawControlType& aYawControlType,
                                                   bool&                                   aAlphaControlGain)
{
   aBankControls     = mBankControls;
   aYawControlType   = mYawControlType;
   aAlphaControlGain = mAlphaControlGain;
}

void VehicleControls::GetSkidToTurnControlSettings(VehicleControlSkidToTurnRollControlType& aRollControlType,
                                                   bool&                                    aAlphaBetaControlsGain)
{
   aRollControlType       = mRollControlType;
   aAlphaBetaControlsGain = mAlphaBetaControlsGain;
}

void VehicleControls::GetManualPilotControlSettings(bool&                           aUseExponentialYawControlMapping,
                                                    bool&                           aUseExponentialPitchControlMapping,
                                                    bool&                           aUseExponentialRollControlMapping,
                                                    double&                         aTrimFactorYaw,
                                                    double&                         aTrimFactorPitch,
                                                    double&                         aTrimFactorRoll,
                                                    VehicleControlAugmentationType& aAugmentationSystem,
                                                    bool&                           aSAS_AugmentPitchAxis,
                                                    bool&                           aSAS_AugmentYawAxis,
                                                    bool&                           aSAS_AugmentRollAxis,
                                                    double&                         aCAS_PitchG,
                                                    double&                         aCAS_RollRate_dps)
{
   aUseExponentialYawControlMapping   = mUseExponentialYawControlMapping;
   aUseExponentialPitchControlMapping = mUseExponentialPitchControlMapping;
   aUseExponentialRollControlMapping  = mUseExponentialRollControlMapping;
   aTrimFactorYaw                     = mTrimFactorYaw;
   aTrimFactorPitch                   = mTrimFactorPitch;
   aTrimFactorRoll                    = mTrimFactorRoll;
   aAugmentationSystem                = mAugmentationSystem;
   aSAS_AugmentPitchAxis              = mSAS_AugmentPitchAxis;
   aSAS_AugmentYawAxis                = mSAS_AugmentYawAxis;
   aSAS_AugmentRollAxis               = mSAS_AugmentRollAxis;
   aCAS_PitchG                        = mCAS_PitchG;
   aCAS_RollRate_dps                  = mCAS_RollRate_dps;
}

void VehicleControls::ClearAllControlsAndPilots()
{
   mValid = false;
   mIsAircraft = false;
   mSyntheticPilotPresent = false;
   mManualPilotPresent = false;
   mHardwareAutopilotPresent = false;
   mGuidanceSystemPresent = false;
   mActivePilot = cNO_CONTROL;
   mBankControls = cAILERONS_ONLY;
   mAlphaControlGain = false;
   mRollControlType = cNO_ROLL_CONTROL;
   mAlphaBetaControlsGain = false;
   mUseExponentialYawControlMapping = false;
   mUseExponentialPitchControlMapping = false;
   mUseExponentialRollControlMapping = false;
   mTrimFactorYaw = 0.0;
   mTrimFactorPitch = 0.0;
   mTrimFactorRoll = 0.0;
   mAugmentationSystem = cDIRECT_CONTROL;
   mCAS_PitchG = 0.0;
   mCAS_RollRate_dps = 0.0;
}

void VehicleControls::SetPilotTypes(bool                    aSyntheticPilotPresent,
                                    bool                    aManualPilotPresent,
                                    bool                    aHardwareAutopilotPresent,
                                    bool                    aGuidanceSystemPresent,
                                    VehicleControlPilotType aActivePilot)
{
   mValid                    = true;
   mSyntheticPilotPresent    = aSyntheticPilotPresent;
   mManualPilotPresent       = aManualPilotPresent;
   mHardwareAutopilotPresent = aHardwareAutopilotPresent;
   mGuidanceSystemPresent    = aGuidanceSystemPresent;
   mActivePilot              = aActivePilot;
}

void VehicleControls::SetBankToTurnControlSettings(VehicleControlBankToTurnRollControl    aBankControls,
                                                   VehicleControlBankToTurnYawControlType aYawControlType,
                                                   bool                                   aAlphaControlGain)
{
   mValid                = true;
   mYawControlType       = aYawControlType;
   mBankControls         = aBankControls;
   mAlphaControlGain     = aAlphaControlGain;
}

void VehicleControls::SetSkidToTurnControlSettings(VehicleControlSkidToTurnRollControlType aRollControlType,
                                                   bool                                    aAlphaBetaControlsGain)
{
   mValid                 = true;
   mRollControlType       = aRollControlType;
   mAlphaBetaControlsGain = aAlphaBetaControlsGain;
}

void VehicleControls::SetManualPilotDirectControlSettings(bool                           aUseExponentialYawControlMapping,
                                                          bool                           aUseExponentialPitchControlMapping,
                                                          bool                           aUseExponentialRollControlMapping,
                                                          double                         aTrimFactorYaw,
                                                          double                         aTrimFactorPitch,
                                                          double                         aTrimFactorRoll,
                                                          VehicleControlAugmentationType aAugmentationSystem)
{
   mValid                             = true;
   mUseExponentialYawControlMapping   = aUseExponentialYawControlMapping;
   mUseExponentialPitchControlMapping = aUseExponentialPitchControlMapping;
   mUseExponentialRollControlMapping  = aUseExponentialRollControlMapping;
   mTrimFactorYaw                     = aTrimFactorYaw;
   mTrimFactorPitch                   = aTrimFactorPitch;
   mTrimFactorRoll                    = aTrimFactorRoll;
   mAugmentationSystem                = aAugmentationSystem;
}

void VehicleControls::SetManualPilotSAS_Settings(bool                           aSAS_AugmentPitchAxis,
                                                 bool                           aSAS_AugmentYawAxis,
                                                 bool                           aSAS_AugmentRollAxis)
{
   mSAS_AugmentPitchAxis              = aSAS_AugmentPitchAxis;
   mSAS_AugmentYawAxis                = aSAS_AugmentYawAxis;
   mSAS_AugmentRollAxis               = aSAS_AugmentRollAxis;
}

void VehicleControls::SetManualPilotCAS_Settings(double                         aCAS_PitchG,
                                                 double                         aCAS_RollRate_dps)
{
   mCAS_PitchG                        = aCAS_PitchG;
   mCAS_RollRate_dps                  = aCAS_RollRate_dps;
}

} // namespace Designer
