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

#include "P6DofGuidanceAutopilotSkidToTurn.hpp"

#include <cstring>

#include "P6DofCommonController.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofGuidanceAutopilotSkidToTurn::P6DofGuidanceAutopilotSkidToTurn(P6DofScenario* aScenario)
   : P6DofGuidanceAutopilot(aScenario)
{
}

P6DofGuidanceAutopilotSkidToTurn* P6DofGuidanceAutopilotSkidToTurn::Clone() const
{
   return new P6DofGuidanceAutopilotSkidToTurn(*this);
}

P6DofGuidanceAutopilotSkidToTurn::P6DofGuidanceAutopilotSkidToTurn(P6DofPilotObject* aPilotObject)
   : P6DofGuidanceAutopilot(aPilotObject)
{
}

bool P6DofGuidanceAutopilotSkidToTurn::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "guidance_autopilot_skid_to_turn")
   {
      UtInputBlock block(aInput, "end_guidance_autopilot_skid_to_turn");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "guidance_config_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);

            mCommonController = new P6DofCommonController(filename);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofGuidanceAutopilotSkidToTurn::ProcessInput().";
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
                          << "Unable to create mCommonController in P6DofGuidanceAutopilotSkidToTurn::ProcessInput().";
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofGuidanceAutopilotSkidToTurn::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofGuidanceAutopilotSkidToTurn::Initialize(int64_t aSimTime_nanosec)
{
   bool result = P6DofPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
