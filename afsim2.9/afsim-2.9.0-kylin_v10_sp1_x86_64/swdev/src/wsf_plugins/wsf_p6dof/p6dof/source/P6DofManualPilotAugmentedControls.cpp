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

#include "P6DofManualPilotAugmentedControls.hpp"

#include <cmath>
#include <cstring>
#include <memory>

#include "P6DofCommonController.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

P6DofManualPilotAugmentedControls::P6DofManualPilotAugmentedControls(P6DofScenario* aScenario)
   : P6DofManualPilot(aScenario)
{
}

P6DofManualPilotAugmentedControls* P6DofManualPilotAugmentedControls::Clone() const
{
   return new P6DofManualPilotAugmentedControls(*this);
}

P6DofManualPilotAugmentedControls::P6DofManualPilotAugmentedControls(P6DofPilotObject* aPilotObject)
   : P6DofManualPilot(aPilotObject)
{
}

bool P6DofManualPilotAugmentedControls::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   // Process CAS-style augmentation
   if (command == "manual_pilot_augmented_controls")
   {
      // Set CAS mode
      mControlAugmentationModeActive = true;

      UtInputBlock block(aInput, "end_manual_pilot_augmented_controls");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_augmentation_system_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);

            mCommonController = new P6DofCommonController(filename);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "autopilot_config")
         {
            mCommonController = new P6DofCommonController(aInput, localCommand);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "pitch_control_mapping_table")
         {
            mPitchControlMapping = ut::make_unique<UtTable::Curve>();
            mPitchControlMapping->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "normalized_input",
                                               UtTable::NoCheck(),
                                               UtInput::cNON_DIMENSIONAL,
                                               "adjusted_normalized_output_deg",
                                               UtTable::NoCheck());
         }
         else if (localCommand == "roll_control_mapping_table")
         {
            mRollControlMapping = ut::make_unique<UtTable::Curve>();
            mRollControlMapping->ProcessInput(aInput,
                                              UtInput::cNON_DIMENSIONAL,
                                              "normalized_input",
                                              UtTable::NoCheck(),
                                              UtInput::cNON_DIMENSIONAL,
                                              "adjusted_normalized_output_deg",
                                              UtTable::NoCheck());
         }
         else if (localCommand == "yaw_control_mapping_table")
         {
            mYawControlMapping = ut::make_unique<UtTable::Curve>();
            mYawControlMapping->ProcessInput(aInput,
                                             UtInput::cNON_DIMENSIONAL,
                                             "normalized_input",
                                             UtTable::NoCheck(),
                                             UtInput::cNON_DIMENSIONAL,
                                             "adjusted_normalized_output_deg",
                                             UtTable::NoCheck());
         }
         else if (localCommand == "pitch_control_augmentation_factor_g")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning() << "Negative pitch_control_augmentation_factor_g in "
                                                "P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mPitchControlAugmentationFactor_g = value;
         }
         else if (localCommand == "roll_control_augmentation_factor_dps")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning() << "Negative roll_control_augmentation_factor_dps in "
                                                "P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mRollControlAugmentationFactor_dps = value;
         }
         else if (localCommand == "pitch_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative pitch_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mPitchTrimFactor = value;
         }
         else if (localCommand == "roll_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative roll_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mRollTrimFactor = value;
         }
         else if (localCommand == "yaw_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative yaw_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mYawTrimFactor = value;
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofManualPilotAugmentedControls::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   // Process SAS-style augmentation
   if (command == "manual_pilot_augmented_stability")
   {
      // Set CAS mode
      mControlAugmentationModeActive = true;

      UtInputBlock block(aInput, "end_manual_pilot_augmented_stability");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_augmentation_system_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);

            mCommonController = new P6DofCommonController(filename);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "autopilot_config")
         {
            mCommonController = new P6DofCommonController(aInput, localCommand);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "pitch_control_mapping_table")
         {
            mPitchControlMapping = ut::make_unique<UtTable::Curve>();
            mPitchControlMapping->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "normalized_input",
                                               UtTable::NoCheck(),
                                               UtInput::cNON_DIMENSIONAL,
                                               "adjusted_normalized_output_deg",
                                               UtTable::NoCheck());
         }
         else if (localCommand == "roll_control_mapping_table")
         {
            mRollControlMapping = ut::make_unique<UtTable::Curve>();
            mRollControlMapping->ProcessInput(aInput,
                                              UtInput::cNON_DIMENSIONAL,
                                              "normalized_input",
                                              UtTable::NoCheck(),
                                              UtInput::cNON_DIMENSIONAL,
                                              "adjusted_normalized_output_deg",
                                              UtTable::NoCheck());
         }
         else if (localCommand == "yaw_control_mapping_table")
         {
            mYawControlMapping = ut::make_unique<UtTable::Curve>();
            mYawControlMapping->ProcessInput(aInput,
                                             UtInput::cNON_DIMENSIONAL,
                                             "normalized_input",
                                             UtTable::NoCheck(),
                                             UtInput::cNON_DIMENSIONAL,
                                             "adjusted_normalized_output_deg",
                                             UtTable::NoCheck());
         }
         else if (localCommand == "pitch_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative pitch_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mPitchTrimFactor = value;
         }
         else if (localCommand == "roll_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative roll_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mRollTrimFactor = value;
         }
         else if (localCommand == "yaw_trim_factor")
         {
            double value;
            aInput.ReadValue(value);
            if (value < 0)
            {
               value    = fabs(value);
               auto out = ut::log::warning()
                          << "Negative yaw_trim_factor in P6DofManualPilotAugmentedControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mYawTrimFactor = value;
         }
         else if (localCommand == "pitch_stability_augmentation")
         {
            bool value;
            aInput.ReadValue(value);
            mPitchStabilityAugmentationModeActive = value;
         }
         else if (localCommand == "yaw_stability_augmentation")
         {
            bool value;
            aInput.ReadValue(value);
            mYawStabilityAugmentationModeActive = value;
         }
         else if (localCommand == "roll_stability_augmentation")
         {
            bool value;
            aInput.ReadValue(value);
            mRollStabilityAugmentationModeActive = value;
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofManualPilotAugmentedControls::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofManualPilotAugmentedControls::Initialize(int64_t aSimTime_nanosec)
{
   bool result = P6DofPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
