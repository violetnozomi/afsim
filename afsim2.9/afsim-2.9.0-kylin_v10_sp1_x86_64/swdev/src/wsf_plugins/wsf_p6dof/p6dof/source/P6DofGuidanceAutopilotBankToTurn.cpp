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

#include "P6DofGuidanceAutopilotBankToTurn.hpp"

#include <cstring>

#include "P6DofCommonController.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofGuidanceAutopilotBankToTurn::P6DofGuidanceAutopilotBankToTurn(P6DofScenario* aScenario)
   : P6DofGuidanceAutopilot(aScenario)
{
}

P6DofGuidanceAutopilotBankToTurn* P6DofGuidanceAutopilotBankToTurn::Clone() const
{
   return new P6DofGuidanceAutopilotBankToTurn(*this);
}

P6DofGuidanceAutopilotBankToTurn::P6DofGuidanceAutopilotBankToTurn(P6DofPilotObject* aPilotObject)
   : P6DofGuidanceAutopilot(aPilotObject)
{
}

bool P6DofGuidanceAutopilotBankToTurn::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "guidance_autopilot_bank_to_turn")
   {
      UtInputBlock block(aInput, "end_guidance_autopilot_bank_to_turn");

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
                          << "Unable to create mCommonController in P6DofGuidanceAutopilotBankToTurn::ProcessInput().";
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
                          << "Unable to create mCommonController in P6DofGuidanceAutopilotBankToTurn::ProcessInput().";
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofGuidanceAutopilotBankToTurn::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofGuidanceAutopilotBankToTurn::Initialize(int64_t aSimTime_nanosec)
{
   bool result = P6DofPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
