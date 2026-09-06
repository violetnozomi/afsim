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

#include "P6DofHardwareAutopilotBankToTurn.hpp"

#include <cstring>

#include "P6DofCommonController.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofHardwareAutopilotBankToTurn::P6DofHardwareAutopilotBankToTurn(P6DofScenario* aScenario)
   : P6DofHardwareAutopilot(aScenario)
{
}

P6DofHardwareAutopilotBankToTurn* P6DofHardwareAutopilotBankToTurn::Clone() const
{
   return new P6DofHardwareAutopilotBankToTurn(*this);
}

P6DofHardwareAutopilotBankToTurn::P6DofHardwareAutopilotBankToTurn(P6DofPilotObject* aPilotObject)
   : P6DofHardwareAutopilot(aPilotObject)
{
}

bool P6DofHardwareAutopilotBankToTurn::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "hardware_autopilot_bank_to_turn")
   {
      UtInputBlock block(aInput, "end_hardware_autopilot_bank_to_turn");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "autopilot_config_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename = aInput.LocateFile(filename);

            mCommonController = new P6DofCommonController(filename);

            if (mCommonController == nullptr)
            {
               auto out = ut::log::error()
                          << "Unable to create mCommonController in P6DofHardwareAutopilotBankToTurn::ProcessInput().";
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
                          << "Unable to create mCommonController in P6DofHardwareAutopilotBankToTurn::ProcessInput().";
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofHardwareAutopilotBankToTurn::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofHardwareAutopilotBankToTurn::Initialize(int64_t aSimTime_nanosec)
{
   bool result = P6DofPilotObject::Initialize(aSimTime_nanosec);

   return result;
}
