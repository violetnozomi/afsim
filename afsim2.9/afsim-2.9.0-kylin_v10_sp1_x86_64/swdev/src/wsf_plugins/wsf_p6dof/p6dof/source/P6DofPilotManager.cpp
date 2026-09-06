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

#include "P6DofPilotManager.hpp"

#include <cstring>

#include "P6DofCommonController.hpp"
#include "P6DofGuidanceAutopilotBankToTurn.hpp"
#include "P6DofGuidanceAutopilotSkidToTurn.hpp"
#include "P6DofHardwareAutopilotBankToTurn.hpp"
#include "P6DofHardwareAutopilotSkidToTurn.hpp"
#include "P6DofManualPilotAugmentedControls.hpp"
#include "P6DofManualPilotSimpleControls.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofSyntheticPilot.hpp"
#include "P6DofVehicle.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"

P6DofPilotManager::P6DofPilotManager(P6DofScenario* aScenario)
   : mScenario(aScenario)
   , mManualPilotSimpleControls(nullptr)
   , mManualPilotAugmentedControls(nullptr)
   , mSyntheticPilot(nullptr)
   , mHardwareAutopilotBankToTurn(nullptr)
   , mHardwareAutopilotSkidToTurn(nullptr)
   , mGuidanceAutopilotBankToTurn(nullptr)
   , mGuidanceAutopilotSkidToTurn(nullptr)
   , mActivePilot(nullptr)
   , mIsDestroyed(false)
   , mSpecialCommonController(nullptr)
{
}

P6DofPilotManager::~P6DofPilotManager()
{
   if (mManualPilotSimpleControls != nullptr)
   {
      delete mManualPilotSimpleControls;
   }
   if (mManualPilotAugmentedControls != nullptr)
   {
      delete mManualPilotAugmentedControls;
   }
   if (mSyntheticPilot != nullptr)
   {
      delete mSyntheticPilot;
   }
   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      delete mHardwareAutopilotBankToTurn;
   }
   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      delete mHardwareAutopilotSkidToTurn;
   }
   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      delete mGuidanceAutopilotBankToTurn;
   }
   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      delete mGuidanceAutopilotSkidToTurn;
   }
   if (mSpecialCommonController != nullptr)
   {
      delete mSpecialCommonController;
   }
}

P6DofPilotManager::P6DofPilotManager(const P6DofPilotManager& aSrc)
   : mScenario(aSrc.mScenario)
   , mManualPilotSimpleControls(nullptr)
   , mManualPilotAugmentedControls(nullptr)
   , mSyntheticPilot(nullptr)
   , mHardwareAutopilotBankToTurn(nullptr)
   , mHardwareAutopilotSkidToTurn(nullptr)
   , mGuidanceAutopilotBankToTurn(nullptr)
   , mGuidanceAutopilotSkidToTurn(nullptr)
   , mActivePilot(nullptr)
   , mIsDestroyed(aSrc.mIsDestroyed)
   , mSpecialCommonController(nullptr)
{
   if (aSrc.mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls = aSrc.mManualPilotSimpleControls->Clone();
   }
   if (aSrc.mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls = aSrc.mManualPilotAugmentedControls->Clone();
   }
   if (aSrc.mSyntheticPilot != nullptr)
   {
      mSyntheticPilot = aSrc.mSyntheticPilot->Clone();
   }
   if (aSrc.mHardwareAutopilotBankToTurn != nullptr)
   {
      mHardwareAutopilotBankToTurn = aSrc.mHardwareAutopilotBankToTurn->Clone();
   }
   if (aSrc.mHardwareAutopilotSkidToTurn != nullptr)
   {
      mHardwareAutopilotSkidToTurn = aSrc.mHardwareAutopilotSkidToTurn->Clone();
   }
   if (aSrc.mGuidanceAutopilotBankToTurn != nullptr)
   {
      mGuidanceAutopilotBankToTurn = aSrc.mGuidanceAutopilotBankToTurn->Clone();
   }
   if (aSrc.mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mGuidanceAutopilotSkidToTurn = aSrc.mGuidanceAutopilotSkidToTurn->Clone();
   }

   if (aSrc.mActivePilot != nullptr)
   {
      if (aSrc.mActivePilot == aSrc.mManualPilotSimpleControls)
      {
         mActivePilot = mManualPilotSimpleControls;
      }
      else if (aSrc.mActivePilot == aSrc.mManualPilotAugmentedControls)
      {
         mActivePilot = mManualPilotAugmentedControls;
      }
      else if (aSrc.mActivePilot == aSrc.mSyntheticPilot)
      {
         mActivePilot = mSyntheticPilot;
      }
      else if (aSrc.mActivePilot == aSrc.mHardwareAutopilotBankToTurn)
      {
         mActivePilot = mHardwareAutopilotBankToTurn;
      }
      else if (aSrc.mActivePilot == aSrc.mHardwareAutopilotSkidToTurn)
      {
         mActivePilot = mHardwareAutopilotSkidToTurn;
      }
      else if (aSrc.mActivePilot == aSrc.mGuidanceAutopilotBankToTurn)
      {
         mActivePilot = mGuidanceAutopilotBankToTurn;
      }
      else if (aSrc.mActivePilot == aSrc.mGuidanceAutopilotSkidToTurn)
      {
         mActivePilot = mGuidanceAutopilotSkidToTurn;
      }
   }

   if (aSrc.mSpecialCommonController != nullptr)
   {
      mSpecialCommonController = aSrc.mSpecialCommonController->Clone();
   }
}

P6DofPilotManager* P6DofPilotManager::Clone() const
{
   return new P6DofPilotManager(*this);
}

bool P6DofPilotManager::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "pilot_manager")
   {
      UtInputBlock block(aInput, "end_pilot_manager");

      // We will use a temporary pilot object to read the control_inputs block
      P6DofPilotObject* tempPilotObjectPtr = nullptr;

      // This is used to store the filename for the common autopilot support file
      std::string tempCommonAutopilotSupportFile;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_inputs")
         {
            if (tempPilotObjectPtr == nullptr)
            {
               tempPilotObjectPtr = new P6DofPilotObject(mScenario);
               tempPilotObjectPtr->ProcessInput(aInput);
            }
            else
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of 'control_inputs' in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "manual_pilot_simple_controls")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a manual pilot, but no 'control_inputs' block "
                                                 "has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mManualPilotSimpleControls != nullptr) || (mManualPilotAugmentedControls != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of manual pilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mManualPilotSimpleControls = new P6DofManualPilotSimpleControls(tempPilotObjectPtr);
               mManualPilotSimpleControls->ProcessInput(aInput);
            }
         }
         else if (localCommand == "manual_pilot_augmented_controls")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a manual pilot, but no 'control_inputs' block "
                                                 "has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mManualPilotSimpleControls != nullptr) || (mManualPilotAugmentedControls != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of manual pilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mManualPilotAugmentedControls = new P6DofManualPilotAugmentedControls(tempPilotObjectPtr);
               mManualPilotAugmentedControls->ProcessInput(aInput);
            }
         }
         else if (localCommand == "manual_pilot_augmented_stability")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a manual pilot, but no 'control_inputs' block "
                                                 "has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mManualPilotSimpleControls != nullptr) || (mManualPilotAugmentedControls != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of manual pilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mManualPilotAugmentedControls = new P6DofManualPilotAugmentedControls(tempPilotObjectPtr);
               mManualPilotAugmentedControls->ProcessInput(aInput);
            }
         }
         else if (localCommand == "synthetic_pilot")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a synthetic pilot, but no 'control_inputs' "
                                                 "block has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if (mSyntheticPilot != nullptr)
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of synthetic pilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mSyntheticPilot = new P6DofSyntheticPilot(tempPilotObjectPtr);
               mSyntheticPilot->ProcessInput(aInput);
            }
         }
         else if (localCommand == "hardware_autopilot_bank_to_turn")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a hardware autopilot, but no 'control_inputs' "
                                                 "block has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mHardwareAutopilotBankToTurn != nullptr) || (mHardwareAutopilotSkidToTurn != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of hardware autopilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mHardwareAutopilotBankToTurn = new P6DofHardwareAutopilotBankToTurn(tempPilotObjectPtr);
               mHardwareAutopilotBankToTurn->ProcessInput(aInput);
            }
         }
         else if (localCommand == "hardware_autopilot_skid_to_turn")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a hardware autopilot, but no 'control_inputs' "
                                                 "block has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mHardwareAutopilotBankToTurn != nullptr) || (mHardwareAutopilotSkidToTurn != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of hardware autopilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mHardwareAutopilotSkidToTurn = new P6DofHardwareAutopilotSkidToTurn(tempPilotObjectPtr);
               mHardwareAutopilotSkidToTurn->ProcessInput(aInput);
            }
         }
         else if (localCommand == "guidance_autopilot_bank_to_turn")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a guidance autopilot, but no 'control_inputs' "
                                                 "block has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mGuidanceAutopilotBankToTurn != nullptr) || (mGuidanceAutopilotSkidToTurn != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of guidance autopilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mGuidanceAutopilotBankToTurn = new P6DofGuidanceAutopilotBankToTurn(tempPilotObjectPtr);
               mGuidanceAutopilotBankToTurn->ProcessInput(aInput);
            }
         }
         else if (localCommand == "guidance_autopilot_skid_to_turn")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (tempPilotObjectPtr == nullptr)
            {
               { // RAII block
                  auto out = ut::log::error() << "Attempting to create a guidance autopilot, but no 'control_inputs' "
                                                 "block has been specified yet in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }

            if ((mGuidanceAutopilotBankToTurn != nullptr) || (mGuidanceAutopilotSkidToTurn != nullptr))
            {
               { // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of guidance autopilots in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mGuidanceAutopilotSkidToTurn = new P6DofGuidanceAutopilotSkidToTurn(tempPilotObjectPtr);
               mGuidanceAutopilotSkidToTurn->ProcessInput(aInput);
            }
         }
         else if (localCommand == "active_pilot")
         {
            std::string name;
            aInput.ReadValue(name);

            if (name == "manual_pilot_simple_controls")
            {
               if (mManualPilotSimpleControls != nullptr)
               {
                  mActivePilot = mManualPilotSimpleControls;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote() << "No manual pilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "manual_pilot_augmented_controls")
            {
               if (mManualPilotAugmentedControls != nullptr)
               {
                  mActivePilot = mManualPilotAugmentedControls;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote() << "No manual pilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "synthetic_pilot")
            {
               if (mSyntheticPilot != nullptr)
               {
                  mActivePilot = mSyntheticPilot;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote() << "No synthetic pilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "hardware_autopilot_bank_to_turn")
            {
               if (mHardwareAutopilotBankToTurn != nullptr)
               {
                  mActivePilot = mHardwareAutopilotBankToTurn;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote()
                        << "No hardware autopilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "hardware_autopilot_skid_to_turn")
            {
               if (mHardwareAutopilotSkidToTurn != nullptr)
               {
                  mActivePilot = mHardwareAutopilotSkidToTurn;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote()
                        << "No hardware autopilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "guidance_autopilot_bank_to_turn")
            {
               if (mGuidanceAutopilotBankToTurn != nullptr)
               {
                  mActivePilot = mGuidanceAutopilotBankToTurn;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote()
                        << "No guidance autopilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "guidance_autopilot_skid_to_turn")
            {
               if (mGuidanceAutopilotSkidToTurn != nullptr)
               {
                  mActivePilot = mGuidanceAutopilotSkidToTurn;
               }
               else
               {
                  { // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote()
                        << "No guidance autopilot has been defined yet in P6DofPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete tempPilotObjectPtr;
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else
            {
               // Unknown active pilot specified
               { // RAII block
                  auto out = ut::log::error() << "Unknown 'active_pilot' in P6DofPilotManager::ProcessInput().";
                  out.AddNote() << "Requested: " << name;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete tempPilotObjectPtr;
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "common_autopilot_support_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename                       = aInput.LocateFile(filename);
            tempCommonAutopilotSupportFile = filename;
         }
         else
         {
            { // RAII block
               auto out = ut::log::error() << "Unrecognized command within P6DofPilotManager::ProcessInput().";
               out.AddNote() << "Command: " << localCommand;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            delete tempPilotObjectPtr;
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Free-up the temp pilot -- we no longer need it
      if (tempPilotObjectPtr != nullptr)
      {
         delete tempPilotObjectPtr;
         tempPilotObjectPtr = nullptr;
      }

      // Read the common autopilot support file into the pilots, if present
      if (!tempCommonAutopilotSupportFile.empty())
      {
         if (mManualPilotSimpleControls != nullptr)
         {
            P6DofCommonController* autopilot = mManualPilotSimpleControls->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mManualPilotAugmentedControls != nullptr)
         {
            P6DofCommonController* autopilot = mManualPilotAugmentedControls->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mSyntheticPilot != nullptr)
         {
            P6DofCommonController* autopilot = mSyntheticPilot->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mHardwareAutopilotBankToTurn != nullptr)
         {
            P6DofCommonController* autopilot = mHardwareAutopilotBankToTurn->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mHardwareAutopilotSkidToTurn != nullptr)
         {
            P6DofCommonController* autopilot = mHardwareAutopilotSkidToTurn->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mGuidanceAutopilotBankToTurn != nullptr)
         {
            P6DofCommonController* autopilot = mGuidanceAutopilotBankToTurn->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mGuidanceAutopilotSkidToTurn != nullptr)
         {
            P6DofCommonController* autopilot = mGuidanceAutopilotSkidToTurn->GetCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         // We use an special common controller (autopilot) that is always present, regardless of any
         // other "pilots" above. This provides the ability to always access the common autopilot
         // support file functions.
         if (mSpecialCommonController == nullptr)
         {
            mSpecialCommonController = new P6DofCommonController("");
         }

         mSpecialCommonController->ReadSupportFile(tempCommonAutopilotSupportFile);
      }

      // Check for an active pilot
      if (mActivePilot == nullptr)
      {
         // We lack an active pilot, so if we have any pilot objects, be sure one of them is "active"

         // Try mSyntheticPilot, mHardwareAutopilot, and mManualPilot.
         if (mSyntheticPilot != nullptr)
         {
            mActivePilot = mSyntheticPilot;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the synthetic pilot as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mHardwareAutopilotBankToTurn != nullptr)
         {
            mActivePilot = mHardwareAutopilotBankToTurn;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the hardware autopilot (bank-to-turn) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mHardwareAutopilotSkidToTurn != nullptr)
         {
            mActivePilot = mHardwareAutopilotSkidToTurn;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the hardware autopilot (skid-to-turn) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mGuidanceAutopilotBankToTurn != nullptr)
         {
            mActivePilot = mGuidanceAutopilotBankToTurn;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the guidance autopilot (bank-to-turn) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mGuidanceAutopilotSkidToTurn != nullptr)
         {
            mActivePilot = mGuidanceAutopilotSkidToTurn;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the guidance autopilot (skid-to-turn) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mManualPilotSimpleControls != nullptr)
         {
            mActivePilot = mManualPilotSimpleControls;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the manual pilot (simple controls) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mManualPilotAugmentedControls != nullptr)
         {
            mActivePilot = mManualPilotAugmentedControls;
            auto out     = ut::log::warning() << "Active pilot not defined in P6DofPilotManager::ProcessInput().";
            out.AddNote() << "Using the manual pilot (augmented controls) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else
         {
            // No pilots available. This is acceptable if the vehicle is unguided/un-piloted.
         }
      }
   }

   return myCommand;
}

bool P6DofPilotManager::Initialize(int64_t aSimTime_nanosec)
{
   bool result = true;

   if (mManualPilotSimpleControls != nullptr)
   {
      result = mManualPilotSimpleControls->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for manual pilot simple.");
      }
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      result = mManualPilotAugmentedControls->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for manual pilot augmented.");
      }
   }

   if (mSyntheticPilot != nullptr)
   {
      result = mSyntheticPilot->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for synthetic pilot.");
      }
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      result = mHardwareAutopilotBankToTurn->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for hardware autopilot BTT.");
      }
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      result = mHardwareAutopilotSkidToTurn->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for hardware autopilot STT.");
      }
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      result = mGuidanceAutopilotBankToTurn->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for guidance autopilot BTT.");
      }
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      result = mGuidanceAutopilotSkidToTurn->Initialize(aSimTime_nanosec);
      if (!result)
      {
         throw UtException("P6DofPilotManager::Initialize(): Initialize failed for guidance autopilot STT.");
      }
   }

   return true;
}

void P6DofPilotManager::SetLastSimTime(int64_t aLastSimTime_nanosec)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      mHardwareAutopilotBankToTurn->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      mHardwareAutopilotSkidToTurn->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      mGuidanceAutopilotBankToTurn->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mGuidanceAutopilotSkidToTurn->SetLastSimTime(aLastSimTime_nanosec);
   }
}

void P6DofPilotManager::SetParentVehicle(P6DofVehicle* aParentVehicle)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->SetParentVehicle(aParentVehicle);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->SetParentVehicle(aParentVehicle);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->SetParentVehicle(aParentVehicle);
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      mHardwareAutopilotBankToTurn->SetParentVehicle(aParentVehicle);
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      mHardwareAutopilotSkidToTurn->SetParentVehicle(aParentVehicle);
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      mGuidanceAutopilotBankToTurn->SetParentVehicle(aParentVehicle);
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mGuidanceAutopilotSkidToTurn->SetParentVehicle(aParentVehicle);
   }
}

P6DofPilotObject* P6DofPilotManager::GetActivePilot()
{
   return mActivePilot;
}

P6DofPilotObject* P6DofPilotManager::GetActiveManualPilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mManualPilotAugmentedControls)
      {
         return mActivePilot;
      }
      else if (mActivePilot == mManualPilotSimpleControls)
      {
         return mActivePilot;
      }
   }

   return (P6DofPilotObject*)nullptr;
}

P6DofPilotObject* P6DofPilotManager::GetActiveSyntheticPilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mSyntheticPilot)
      {
         return mActivePilot;
      }
   }

   return (P6DofPilotObject*)nullptr;
}

P6DofPilotObject* P6DofPilotManager::GetActiveHardwareAutopilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mHardwareAutopilotBankToTurn)
      {
         return mActivePilot;
      }
      else if (mActivePilot == mHardwareAutopilotSkidToTurn)
      {
         return mActivePilot;
      }
   }

   return (P6DofPilotObject*)nullptr;
}

P6DofPilotObject* P6DofPilotManager::GetActiveGuidanceAutopilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mGuidanceAutopilotBankToTurn)
      {
         return mActivePilot;
      }
      else if (mActivePilot == mGuidanceAutopilotSkidToTurn)
      {
         return mActivePilot;
      }
   }

   return (P6DofPilotObject*)nullptr;
}

void P6DofPilotManager::InputAngleDeltasToPilotObjects(const double& aYaw_rad, const double& aPitch_rad, const double& aRoll_rad)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      mHardwareAutopilotBankToTurn->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      mHardwareAutopilotSkidToTurn->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      mGuidanceAutopilotBankToTurn->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mGuidanceAutopilotSkidToTurn->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }
}

void P6DofPilotManager::EnableControls(bool aEnabled)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->EnableControlInputs(aEnabled);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->EnableControlInputs(aEnabled);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->EnableControlInputs(aEnabled);
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      mHardwareAutopilotBankToTurn->EnableControlInputs(aEnabled);
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      mHardwareAutopilotSkidToTurn->EnableControlInputs(aEnabled);
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      mGuidanceAutopilotBankToTurn->EnableControlInputs(aEnabled);
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mGuidanceAutopilotSkidToTurn->EnableControlInputs(aEnabled);
   }
}

bool P6DofPilotManager::ControlsEnabled() const
{
   if (mActivePilot == nullptr)
   {
      return false;
   }

   return mActivePilot->ControlsAreEnabled();
}

bool P6DofPilotManager::MakeManualPilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mManualPilotSimpleControls != nullptr)
   {
      mActivePilot = mManualPilotSimpleControls;
      return true;
   }
   else if (mManualPilotAugmentedControls != nullptr)
   {
      mActivePilot = mManualPilotAugmentedControls;
      return true;
   }

   return false;
}

bool P6DofPilotManager::MakeSyntheticPilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mSyntheticPilot != nullptr)
   {
      mActivePilot = mSyntheticPilot;
      return true;
   }

   return false;
}

bool P6DofPilotManager::MakeHardwareAutopilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mHardwareAutopilotBankToTurn != nullptr)
   {
      mActivePilot = mHardwareAutopilotBankToTurn;
      return true;
   }

   if (mHardwareAutopilotSkidToTurn != nullptr)
   {
      mActivePilot = mHardwareAutopilotSkidToTurn;
      return true;
   }

   return false;
}

bool P6DofPilotManager::MakeGuidanceAutopilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mGuidanceAutopilotBankToTurn != nullptr)
   {
      mActivePilot = mGuidanceAutopilotBankToTurn;
      return true;
   }

   if (mGuidanceAutopilotSkidToTurn != nullptr)
   {
      mActivePilot = mGuidanceAutopilotSkidToTurn;
      return true;
   }

   return false;
}

void P6DofPilotManager::SetDestroyed(double aPitchInputModifier, double aRollInputModifier)
{
   if (mActivePilot == nullptr)
   {
      mIsDestroyed = true;
      return;
   }

   // Take control of the vehicle
   mActivePilot->TakeExternalDirectControl();

   // Disable the autopilot, if it is enabled
   if (mActivePilot->AutopilotIsEnabled())
   {
      mActivePilot->EnableAutopilot(false);
   }

   // Note: The pitch and roll input modifiers are randomly
   // assigned (outside of P6DofPilotManager) and vary between +/-1.0

   // Excessive pitch control can result in "looping", which is
   // unexpected when a vehicle is destroyed. As a result, we limit
   // the pitch control.

   // Use a minimal pitch change 80% of the time. Since aPitchInputModifier
   // varies between +/-1, we can use its value as the 80% check.
   if (fabs(aPitchInputModifier) < 0.8)
   {
      // Limit to a maximum pitch control of only 2% of maximum
      aPitchInputModifier *= 0.02;
   }
   else
   {
      // Limit to a maximum pitch control of 5% of maximum
      aPitchInputModifier *= 0.05;
   }

   // For roll, we want to see some roll to help notice that the vehicle
   // is shot down. So, we'll use 30%-60% roll control for 80% of the
   // cases. So, begin by testing for 80%:
   if (fabs(aRollInputModifier) < 0.8)
   {
      // For these cases, we want a control of 0.3 to 0.8:
      if (aRollInputModifier >= 0.0)
      {
         aRollInputModifier = 0.3 + 0.3 * (aRollInputModifier / 0.8);
      }
      else
      {
         aRollInputModifier = -0.3 + 0.3 * (aRollInputModifier / 0.8);
      }
   }
   else
   {
      // For these cases, which have an unmodified setting of 0.8 to 1.0,
      // we will reduce to a maximum of 10%:
      aRollInputModifier *= 0.1;
   }

   // Get the current stick position
   double stickRightPosition = mActivePilot->GetStickRightControllerPosition();

   // Adjust the stick position
   stickRightPosition += aRollInputModifier;

   // Ensure value is between -1 and +1
   if (stickRightPosition > 1.0)
   {
      stickRightPosition = 1.0;
   }
   if (stickRightPosition < -1.0)
   {
      stickRightPosition = -1.0;
   }

   // Get the current stick position
   double stickAftPosition = mActivePilot->GetStickBackControllerPosition();

   // Adjust the stick position
   stickAftPosition += aPitchInputModifier;

   // Ensure value is between -1 and +1
   if (stickAftPosition > 1.0)
   {
      stickAftPosition = 1.0;
   }
   if (stickAftPosition < -1.0)
   {
      stickAftPosition = -1.0;
   }

   // Kill the engines completely
   mActivePilot->GetParentVehicle()->ShutdownObject();

   // Special: If we have a minimal roll command (less than 10%), we will use a
   // minimal stick forward command (10%) to slowly pitch over...
   if (fabs(stickRightPosition) < 0.1)
   {
      stickAftPosition = -0.1;
   }

   // Set the controls, pulling thew power to idle and extending the speed brakes
   mActivePilot->SetExternalDirectControlData(stickRightPosition, stickAftPosition, 0.0, 1.0, 0.0, 0.0);

   // Now, set various destroyed flags so no further commands can be issued

   mIsDestroyed = true;

   // Inform the active pilot of the destroyed condition
   mActivePilot->SetDestroyed();

   // Inform all other pilot objects of the destroyed condition
   if (mManualPilotSimpleControls)
   {
      mManualPilotSimpleControls->SetDestroyed();
   }
   if (mManualPilotAugmentedControls)
   {
      mManualPilotAugmentedControls->SetDestroyed();
   }
   if (mSyntheticPilot)
   {
      mSyntheticPilot->SetDestroyed();
   }
   if (mHardwareAutopilotBankToTurn)
   {
      mHardwareAutopilotBankToTurn->SetDestroyed();
   }
   if (mHardwareAutopilotSkidToTurn)
   {
      mHardwareAutopilotSkidToTurn->SetDestroyed();
   }
   if (mGuidanceAutopilotBankToTurn)
   {
      mGuidanceAutopilotBankToTurn->SetDestroyed();
   }
   if (mGuidanceAutopilotSkidToTurn)
   {
      mGuidanceAutopilotSkidToTurn->SetDestroyed();
   }
}
