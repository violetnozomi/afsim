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

#include "P6DofManualPilotSimpleControls.hpp"

#include <cstring>
#include <math.h>

#include "P6DofCommonController.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofManualPilotSimpleControls::P6DofManualPilotSimpleControls(P6DofScenario* aScenario)
   : P6DofManualPilot(aScenario)
{
}

P6DofManualPilotSimpleControls* P6DofManualPilotSimpleControls::Clone() const
{
   return new P6DofManualPilotSimpleControls(*this);
}

P6DofManualPilotSimpleControls::P6DofManualPilotSimpleControls(P6DofPilotObject* aPilotObject)
   : P6DofManualPilot(aPilotObject)
{
}

bool P6DofManualPilotSimpleControls::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "manual_pilot_simple_controls")
   {
      UtInputBlock block(aInput, "end_manual_pilot_simple_controls");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "pitch_control_mapping_table")
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
                          << "Negative pitch_trim_factor in P6DofManualPilotSimpleControls::ProcessInput(). Using "
                             "positive value instead.";
               out.AddNote() << "Using Value: " << value;
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
                          << "Negative roll_trim_factor in P6DofManualPilotSimpleControls::ProcessInput(). Using "
                             "positive value instead.";
               out.AddNote() << "Using Value: " << value;
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
                          << "Negative yaw_trim_factor in P6DofManualPilotSimpleControls::ProcessInput(). Using "
                             "positive value instead.";
               out.AddNote() << "Using Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            mYawTrimFactor = value;
         }
         else if (localCommand == "simple_yaw_damper")
         {
            bool value;
            aInput.ReadValue(value);
            mSimpleYawDamper = value;
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofManualPilotSimpleControls::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofManualPilotSimpleControls::Initialize(int64_t aSimTime_nanosec)
{
   bool result = P6DofPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
