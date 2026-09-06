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

#include "WsfPointMassSixDOF_ManualPilotSimpleControls.hpp"

#include <cstring>
#include <math.h>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_CommonController.hpp"

wsf::six_dof::PointMassManualPilotSimpleControls* wsf::six_dof::PointMassManualPilotSimpleControls::Clone() const
{
   return new PointMassManualPilotSimpleControls(*this);
}

wsf::six_dof::PointMassManualPilotSimpleControls::PointMassManualPilotSimpleControls(PointMassPilotObject* aPilotObject)
   : PointMassManualPilot(aPilotObject)
{
}

bool wsf::six_dof::PointMassManualPilotSimpleControls::ProcessInput(UtInput& aInput)
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

         if (PointMassPilotObject::ProcessInput(aInput))
         {
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
                          << "Negative pitch_trim_factor in PointMassManualPilotSimpleControls::ProcessInput().";
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
                          << "Negative roll_trim_factor in PointMassManualPilotSimpleControls::ProcessInput().";
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
                          << "Negative yaw_trim_factor in PointMassManualPilotSimpleControls::ProcessInput().";
               out.AddNote() << "Using positive value instead.";
               out.AddNote() << "New Value: " << value;
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
               ut::log::error() << "Unrecognized command within PointMassManualPilotSimpleControls::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::PointMassManualPilotSimpleControls::Initialize(int64_t aSimTime_nanosec)
{
   bool result = PointMassPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
