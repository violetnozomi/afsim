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

#ifndef VEHICLE_CONTROLS_HPP
#define VEHICLE_CONTROLS_HPP

// VehicleControls provides data encapsulation of pilots and controls.

namespace Designer
{
   class VehicleControls
   {
      public:
         VehicleControls() = default;
         ~VehicleControls() = default;

         // This returns true if the controls are valid. An unguided/uncontrolled vehicle
         // will return false. All controlled vehicles will return true.
         bool IsValid() { return mValid; }

         // This returns true when the vehicle is an aircraft
         bool IsAircraft() { return mIsAircraft; }

         enum VehicleControlPilotType
         {
            cNO_CONTROL,
            cSYNTHETIC_PILOT,
            cMANUAL_PILOT,
            cHARDWARE_AUTOPILOT,
            cGUIDANCE_SYSTEM
         };

         enum VehicleControlAugmentationType
         {
            cDIRECT_CONTROL,
            cSTABILITY_AUGMENTATION,
            cCONTROL_AUGMENTATION
         };

         // This should be called to get the pilot types that are present/active
         void GetPilotTypes(bool&                    aSyntheticPilotPresent,
                            bool&                    aManualPilotPresent,
                            bool&                    aHardwareAutopilotPresent,
                            bool&                    aGuidanceSystemPresent,
                            VehicleControlPilotType& aActivePilot);

         enum VehicleControlBankToTurnRollControl
         {
            cAILERONS_ONLY,
            cSTABILIZERS_ONLY,
            cAILERONS_AND_STABILIZERS
         };

         enum VehicleControlBankToTurnYawControlType
         {
            cNO_YAW_CONTROL,
            cYAW_DAMPER,
         };

         // This should be called to get the turn controls for vehicles using bank-to-turn
         void GetBankToTurnControlSettings(VehicleControlBankToTurnRollControl&    aBankControls,
                                           VehicleControlBankToTurnYawControlType& aYawControlType,
                                           bool&                                   aAlphaControlGain);

         enum VehicleControlSkidToTurnRollControlType
         {
            cNO_ROLL_CONTROL,
            cROLL_DAMPER,
            cFULL_ROLL_CONTROL
         };

         // This should be called to get the turn controls for vehicles using skid-to-turn
         void GetSkidToTurnControlSettings(VehicleControlSkidToTurnRollControlType& aRollControlType,
                                           bool&                                    aAlphaBetaControlsGain);


         // This should be called to set the manual pilot control settings
         void GetManualPilotControlSettings(bool&                           aUseExponentialYawControlMapping,
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
                                            double&                         aCAS_RollRate_dps);

         // This should be called to clear all control-related data. Calling this will
         // result in the vehicle controls becoming "invalid".
         void ClearAllControlsAndPilots();

         // This should be called when the vehicle is an aircraft
         void SetIsAircraft(bool aSetting) { mIsAircraft = aSetting; }

         // This should be called to set the pilot types that are present/active
         void SetPilotTypes(bool                    aSyntheticPilotPresent,
                            bool                    aManualPilotPresent,
                            bool                    aHardwareAutopilotPresent,
                            bool                    aGuidanceSystemPresent,
                            VehicleControlPilotType aActivePilot);

         // This should be called to set the turn controls for vehicles using bank-to-turn
         void SetBankToTurnControlSettings(VehicleControlBankToTurnRollControl    aBankControls,
                                           VehicleControlBankToTurnYawControlType aRollControlType,
                                           bool                                   aAlphaControlGain);

         // This should be called to set the turn controls for vehicles using skid-to-turn
         void SetSkidToTurnControlSettings(VehicleControlSkidToTurnRollControlType aRollControlType,
                                           bool                                    aAlphaBetaControlsGain);

         // This should be called to set the manual pilot direct control settings
         void SetManualPilotDirectControlSettings(bool                           aUseExponentialYawControlMapping,
                                                  bool                           aUseExponentialPitchControlMapping,
                                                  bool                           aUseExponentialRollControlMapping,
                                                  double                         aTrimFactorYaw,
                                                  double                         aTrimFactorPitch,
                                                  double                         aTrimFactorRoll,
                                                  VehicleControlAugmentationType aAugmentationSystem);

         // This should be called to set additional SAS settings on the manual pilot
         void SetManualPilotSAS_Settings(bool                           aSAS_UsePitchAxis,
                                         bool                           aSAS_UseYawAxis,
                                         bool                           aSAS_UseRollAxis);
         // This should be called to set additional CAS settings on the manual pilot
         void SetManualPilotCAS_Settings(double                         aCAS_PitchG,
                                         double                         aCAS_RollRate_dps);

         bool UseExponentialYawControlMapping() { return mUseExponentialYawControlMapping; }
         bool UseExponentialPitchControlMapping() { return mUseExponentialPitchControlMapping; }
         bool UseExponentialRollControlMapping() { return mUseExponentialRollControlMapping; }

      private:
         bool                                    mValid                             = false;
         bool                                    mIsAircraft                        = false;

         // Pilot Objects
         bool                                    mSyntheticPilotPresent             = false;
         bool                                    mManualPilotPresent                = false;
         bool                                    mHardwareAutopilotPresent          = false;
         bool                                    mGuidanceSystemPresent             = false;
         VehicleControlPilotType                 mActivePilot                       = cNO_CONTROL;

         // BTT Settings
         VehicleControlBankToTurnRollControl     mBankControls                      = cAILERONS_ONLY;
         VehicleControlBankToTurnYawControlType  mYawControlType                    = cNO_YAW_CONTROL;
         bool                                    mAlphaControlGain                  = false;

         // STT Settings
         VehicleControlSkidToTurnRollControlType mRollControlType                   = cNO_ROLL_CONTROL;
         bool                                    mAlphaBetaControlsGain             = false;

         // Manual Pilot Settings
         bool                                    mUseExponentialYawControlMapping   = false;
         bool                                    mUseExponentialPitchControlMapping = false;
         bool                                    mUseExponentialRollControlMapping  = false;
         double                                  mTrimFactorYaw                     = 0.0;
         double                                  mTrimFactorPitch                   = 0.0;
         double                                  mTrimFactorRoll                    = 0.0;
         VehicleControlAugmentationType          mAugmentationSystem                = cDIRECT_CONTROL;
         bool                                    mSAS_AugmentPitchAxis              = false;
         bool                                    mSAS_AugmentYawAxis                = false;
         bool                                    mSAS_AugmentRollAxis               = false;
         double                                  mCAS_PitchG                        = 0.0;
         double                                  mCAS_RollRate_dps                  = 0.0;
   };
}
#endif // !VEHICLE_CONTROLS_HPP
