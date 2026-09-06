// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofFlightControlSystem.hpp"

#include "P6DofControlActuator.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofVehicle.hpp"
#include "P6DofVehicleData.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofFlightControlSystem::P6DofFlightControlSystem(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mP6DofPilotObjectPtr(nullptr)
   , mP6DofPilotManager(nullptr)
   , mLastUpdateSimTime_nanosec(0)
{
}

P6DofFlightControlSystem::~P6DofFlightControlSystem()
{
   for (auto controlInputIter : mControlInputList)
   {
      delete controlInputIter;
   }
   mControlInputList.clear();

   for (auto controlSignalModIter : mControlSignalModifiersList)
   {
      delete controlSignalModIter;
   }
   mControlSignalModifiersList.clear();

   for (auto controlSurfaceIter : mControlSurfaceList)
   {
      delete controlSurfaceIter;
   }
   mControlSurfaceList.clear();

   for (auto controlValueIter : mControlValueList)
   {
      delete controlValueIter;
   }
   mControlValueList.clear();

   for (auto controlBooleanModIter : mControlBooleanList)
   {
      delete controlBooleanModIter;
   }
   mControlBooleanList.clear();
}

P6DofFlightControlSystem::P6DofFlightControlSystem(const P6DofFlightControlSystem& aSrc)
   : P6DofObject(aSrc)
   , mP6DofPilotObjectPtr(nullptr)
   , mP6DofPilotManager(nullptr)
   , mLastUpdateSimTime_nanosec(aSrc.mLastUpdateSimTime_nanosec)
   , mState(aSrc.mState)
{
   // Copy control input list
   if (!aSrc.mControlInputList.empty())
   {
      for (auto controlInputIter : aSrc.mControlInputList)
      {
         auto temp             = new ControlInputValue();
         temp->name            = controlInputIter->name;
         temp->handle          = 0;
         temp->typeData        = controlInputIter->typeData;
         temp->normalizedValue = controlInputIter->normalizedValue;
         temp->boolData        = controlInputIter->boolData;

         mControlInputList.push_back(temp);
      }
   }

   // Copy control signal modifiers list
   if (!aSrc.mControlSignalModifiersList.empty())
   {
      for (auto controlSignalModifier : aSrc.mControlSignalModifiersList)
      {
         mControlSignalModifiersList.push_back(new ControlSignalModifier(*controlSignalModifier));
      }
   }

   // Copy control surface list
   if (!aSrc.mControlSurfaceList.empty())
   {
      for (auto controlSurfaceIter : aSrc.mControlSurfaceList)
      {
         auto temp                = new ControlSurfaceElement;
         temp->controlSurfaceName = controlSurfaceIter->controlSurfaceName;
         temp->currentAngle_deg   = controlSurfaceIter->currentAngle_deg;
         temp->minAngle_deg       = controlSurfaceIter->minAngle_deg;
         temp->maxAngle_deg       = controlSurfaceIter->maxAngle_deg;
         temp->actuator           = nullptr;
         temp->angleMappingAuto   = nullptr;
         temp->angleMappingManual = nullptr;

         if (controlSurfaceIter->actuator)
         {
            temp->actuator = controlSurfaceIter->actuator->Clone(this);
         }

         if (controlSurfaceIter->angleMappingAuto != nullptr)
         {
            temp->angleMappingAuto = controlSurfaceIter->angleMappingAuto->Clone();
         }

         if (controlSurfaceIter->angleMappingManual != nullptr)
         {
            temp->angleMappingManual = controlSurfaceIter->angleMappingManual->Clone();
         }

         for (auto controlInputIter : controlSurfaceIter->controlInputList)
         {
            auto tempInput = new ControlInputGainStream();

            tempInput->controlInputName = controlInputIter->controlInputName;
            tempInput->controlInput     = nullptr;

            for (const auto& nameIter : controlInputIter->modifierNameList)
            {
               tempInput->modifierNameList.push_back(nameIter);
            }
            temp->controlInputList.push_back(tempInput);
         }

         mControlSurfaceList.push_back(temp);
      }
   }

   // Copy control values list
   if (!aSrc.mControlValueList.empty())
   {
      for (auto controlValueIter : aSrc.mControlValueList)
      {
         auto temp              = new ControlValueElement();
         temp->controlValueName = controlValueIter->controlValueName;
         temp->currentValue     = controlValueIter->currentValue;
         temp->minValue         = controlValueIter->minValue;
         temp->maxValue         = controlValueIter->maxValue;

         for (auto controlInputIter : controlValueIter->controlInputList)
         {
            auto tempInput = new ControlInputGainStream();

            tempInput->controlInputName = controlInputIter->controlInputName;
            tempInput->controlInput     = nullptr;

            for (const auto& nameIter : controlInputIter->modifierNameList)
            {
               tempInput->modifierNameList.push_back(nameIter);
            }
            temp->controlInputList.push_back(tempInput);
         }

         mControlValueList.push_back(temp);
      }
   }

   // Copy control values list
   if (!aSrc.mControlBooleanList.empty())
   {
      for (auto controlBoolIter : aSrc.mControlBooleanList)
      {
         auto temp              = new ControlBooleanElement();
         temp->controlValueName = controlBoolIter->controlValueName;
         temp->currentValue     = controlBoolIter->currentValue;
         temp->lastValue        = controlBoolIter->lastValue;
         temp->thresholdValue   = controlBoolIter->thresholdValue;

         for (auto controlInputIter : controlBoolIter->controlInputList)
         {
            auto tempInput = new ControlInputGainStream();

            tempInput->controlInputName = controlInputIter->controlInputName;
            tempInput->controlInput     = nullptr;

            for (const auto& nameIter : controlInputIter->modifierNameList)
            {
               tempInput->modifierNameList.push_back(nameIter);
            }
            temp->controlInputList.push_back(tempInput);
         }

         mControlBooleanList.push_back(temp);
      }
   }
}

P6DofFlightControlSystem* P6DofFlightControlSystem::Clone() const
{
   return new P6DofFlightControlSystem(*this);
}

void P6DofFlightControlSystem::ConnectInputsWithOutputs()
{
   // We can't do anything if we lack some outputs
   if (mControlSurfaceList.empty() && mControlValueList.empty() && mControlBooleanList.empty())
   {
      return;
   }

   // Clear the current list
   if (!mControlInputList.empty())
   {
      for (auto controlInputIter : mControlInputList)
      {
         delete controlInputIter;
      }
      mControlInputList.clear();
   }

   // Now, setup the input list
   if (mP6DofPilotObjectPtr)
   {
      for (auto tempSurface : mControlSurfaceList)
      {
         // Get control inputs
         for (auto tempControlInput : tempSurface->controlInputList)
         {
            auto inputItem             = new ControlInputValue();
            inputItem->name            = tempControlInput->controlInputName;
            inputItem->typeData        = P6DOF::DataType::Float;
            inputItem->normalizedValue = 0;
            inputItem->boolData        = false;
            inputItem->handle          = mP6DofPilotObjectPtr->GetControlHandle(inputItem->name);
            if (inputItem->handle > 0)
            {
               inputItem->normalizedValue = mP6DofPilotObjectPtr->ControlInputValue(inputItem->handle);
            }

            mControlInputList.push_back(inputItem);
         }
      }

      for (auto temp : mControlValueList)
      {
         for (auto controlInputIter : temp->controlInputList)
         {
            auto inputItem             = new ControlInputValue();
            inputItem->name            = controlInputIter->controlInputName;
            inputItem->typeData        = P6DOF::DataType::Float;
            inputItem->normalizedValue = 0;
            inputItem->boolData        = false;
            inputItem->handle          = mP6DofPilotObjectPtr->GetControlHandle(inputItem->name);
            if (inputItem->handle > 0)
            {
               inputItem->normalizedValue = mP6DofPilotObjectPtr->ControlInputValue(inputItem->handle);
               inputItem->boolData        = mP6DofPilotObjectPtr->ControlInputValueBool(inputItem->handle);
            }

            mControlInputList.push_back(inputItem);
         }
      }

      for (auto controlBoolIter : mControlBooleanList)
      {
         for (auto tempControlInput : controlBoolIter->controlInputList)
         {
            auto inputItem             = new ControlInputValue();
            inputItem->name            = tempControlInput->controlInputName;
            inputItem->typeData        = P6DOF::DataType::Float;
            inputItem->normalizedValue = 0;
            inputItem->boolData        = false;
            inputItem->handle          = mP6DofPilotObjectPtr->GetControlHandle(inputItem->name);
            if (inputItem->handle > 0)
            {
               inputItem->normalizedValue = mP6DofPilotObjectPtr->ControlInputValue(inputItem->handle);
               inputItem->boolData        = mP6DofPilotObjectPtr->ControlInputValueBool(inputItem->handle);
            }

            mControlInputList.push_back(inputItem);
         }
      }
   }

   if (!mControlSurfaceList.empty())
   {
      for (auto tempSurface : mControlSurfaceList)
      {
         // TODO - Note: We used to set the actuator for the surface here, but that is no longer needed.
         // If it *is* needed later, this is the place it should occur.

         for (auto tempControlInput : tempSurface->controlInputList)
         {
            // Clear any control modifiers that may be in the list
            for (auto controlModifierIter : tempControlInput->modifierList)
            {
               delete controlModifierIter;
            }
            tempControlInput->modifierList.clear();

            // Get the control input
            ControlInputValue* input = GetControlInputByName(tempControlInput->controlInputName);

            // We need a valid input to proceed...
            if (input != nullptr)
            {
               tempControlInput->controlInput = input;

               // Get the control modifiers by name
               for (const auto& nameIter : tempControlInput->modifierNameList)
               {
                  ControlSignalModifier* tempModifier = GetControlSignalModifierByName(nameIter);

                  // If the control modifier is valid, add it to the list
                  if (tempModifier)
                  {
                     tempControlInput->modifierList.push_back(tempModifier);
                  }
               }
            }
         }
      }
   }

   if (!mControlValueList.empty())
   {
      for (auto tempValue : mControlValueList)
      {
         for (auto tempControlInputGainStream : tempValue->controlInputList)
         {
            // Clear any control modifiers that may be in the list
            for (auto controlModifierIter : tempControlInputGainStream->modifierList)
            {
               delete controlModifierIter;
            }
            tempControlInputGainStream->modifierList.clear();

            // Get the control input
            ControlInputValue* input = GetControlInputByName(tempControlInputGainStream->controlInputName);

            // We need a valid input to proceed...
            if (input != nullptr)
            {
               tempControlInputGainStream->controlInput = input;

               // Get the control modifiers by name
               for (const auto& nameIter : tempControlInputGainStream->modifierNameList)
               {
                  ControlSignalModifier* tempModifier = GetControlSignalModifierByName(nameIter);

                  // If the control modifier is valid, add it to the list
                  if (tempModifier)
                  {
                     tempControlInputGainStream->modifierList.push_back(tempModifier);
                  }
               }
            }
         }
      }
   }

   if (!mControlBooleanList.empty())
   {
      for (auto tempValue : mControlBooleanList)
      {
         for (auto tempControlInputGainStream : tempValue->controlInputList)
         {
            // Clear any control modifiers that may be in the list
            for (auto controlModifierIter : tempControlInputGainStream->modifierList)
            {
               delete controlModifierIter;
            }
            tempControlInputGainStream->modifierList.clear();

            // Get the control input
            ControlInputValue* input = GetControlInputByName(tempControlInputGainStream->controlInputName);

            // We need a valid input to proceed...
            if (input != nullptr)
            {
               tempControlInputGainStream->controlInput = input;

               // Get the control modifiers by name
               for (const auto& nameIter : tempControlInputGainStream->modifierNameList)
               {
                  ControlSignalModifier* tempModifier = GetControlSignalModifierByName(nameIter);

                  // If the control modifier is valid, add it to the list
                  if (tempModifier)
                  {
                     tempControlInputGainStream->modifierList.push_back(tempModifier);
                  }
               }
            }
         }
      }
   }
}

P6DofFlightControlSystem::ControlInputValue*
P6DofFlightControlSystem::GetControlInputByName(const std::string& aControlInputName) const
{
   for (auto controlInput : mControlInputList)
   {
      if (controlInput->name == aControlInputName)
      {
         return controlInput;
      }
   }

   return nullptr;
}

P6DofFlightControlSystem::ControlSignalModifier*
P6DofFlightControlSystem::GetControlSignalModifierByName(const std::string& aSignalModifierName) const
{
   for (auto controlSignalModifier : mControlSignalModifiersList)
   {
      if (aSignalModifierName == controlSignalModifier->name)
      {
         return controlSignalModifier;
      }
   }
   return nullptr;
}

bool P6DofFlightControlSystem::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "flight_controls")
   {
      UtInputBlock block(aInput, "end_flight_controls");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "mapping_table")
         {
            ProcessSignalModifierTable(aInput);
         }
         else if (localCommand == "gain_table")
         {
            ProcessSignalModifierTable(aInput);
         }
         else if (localCommand == "scalar_gain")
         {
            ProcessSignalModifierTable(aInput);
         }
         else if (localCommand == "clamp_gain")
         {
            ProcessSignalModifierTable(aInput);
         }
         else if (localCommand == "sas_channel")
         {
            ProcessSignalModifierTable(aInput);
         }
         else if (localCommand == "control_surface")
         {
            ProcessControlSurfaceInput(aInput);
         }
         else if (localCommand == "control_value")
         {
            ProcessControlValueInput(aInput);
         }
         else if (localCommand == "control_boolean")
         {
            ProcessControlBooleanInput(aInput);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofFlightControlSystem::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessSignalModifierTable(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   ControlSignalModifier* tempControlSignalModifier = nullptr;

   if (command == "mapping_table")
   {
      tempControlSignalModifier             = new ControlSignalModifier();
      tempControlSignalModifier->type       = Undefined;
      tempControlSignalModifier->table      = nullptr;
      tempControlSignalModifier->curve      = nullptr;
      tempControlSignalModifier->scalarGain = 0;
      tempControlSignalModifier->minClamp   = -1;
      tempControlSignalModifier->maxClamp   = 1;

      UtInputBlock block(aInput, "end_mapping_table");

      std::string name;
      aInput.ReadValue(name);
      tempControlSignalModifier->name = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "type")
         {
            std::string value;
            aInput.ReadValue(value);

            if (value == "mach_mapping")
            {
               tempControlSignalModifier->type = MachMappingTable;
            }
            else if (value == "ktas_mapping")
            {
               tempControlSignalModifier->type = KTASMappingTable;
            }
            else if (value == "alpha_mapping")
            {
               tempControlSignalModifier->type = AlphaMappingTable;
            }
            else if (value == "beta_mapping")
            {
               tempControlSignalModifier->type = BetaMappingTable;
            }
            else if (value == "g_x_load_mapping")
            {
               tempControlSignalModifier->type = GxLoadMappingTable;
            }
            else if (value == "g_y_load_mapping")
            {
               tempControlSignalModifier->type = GyLoadMappingTable;
            }
            else if (value == "g_z_load_mapping")
            {
               tempControlSignalModifier->type = GzLoadMappingTable;
            }
            else if (value == "alt_mapping")
            {
               tempControlSignalModifier->type = AltMappingTable;
            }
            else if (value == "q_mapping")
            {
               tempControlSignalModifier->type = QMappingTable;
            }
            else if (value == "signal_mapping")
            {
               tempControlSignalModifier->type = SignalMappingTable;
            }
            else
            {
               auto out = ut::log::warning()
                          << "Unknown value in P6DofFlightControlSystem::ProcessSignalModifierTable().";
               out.AddNote() << "Value: " << value;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
         }
         else if (localCommand == "table_data")
         {
            UtInputBlock subBlock(aInput, "end_table_data");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("control_value", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("input", UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), 1);

               tempControlSignalModifier->table =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "gain_table")
   {
      tempControlSignalModifier             = new ControlSignalModifier();
      tempControlSignalModifier->type       = Undefined;
      tempControlSignalModifier->table      = nullptr;
      tempControlSignalModifier->curve      = nullptr;
      tempControlSignalModifier->scalarGain = 0;
      tempControlSignalModifier->minClamp   = -1;
      tempControlSignalModifier->maxClamp   = 1;

      UtInputBlock block(aInput, "end_gain_table");

      std::string name;
      aInput.ReadValue(name);
      tempControlSignalModifier->name = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "type")
         {
            std::string value;
            aInput.ReadValue(value);

            if (value == "scalar_gain")
            {
               tempControlSignalModifier->type = ScalarGain;
            }
            else if (value == "clamp")
            {
               tempControlSignalModifier->type = ClampGain;
            }
            else if (value == "sas")
            {
               tempControlSignalModifier->type = SASChannel;
            }
            else if (value == "mach_gain")
            {
               tempControlSignalModifier->type = MachGainTable;
            }
            else if (value == "ktas_gain")
            {
               tempControlSignalModifier->type = KTASGainTable;
            }
            else if (value == "alpha_gain")
            {
               tempControlSignalModifier->type = AlphaGainTable;
            }
            else if (value == "beta_gain")
            {
               tempControlSignalModifier->type = BetaGainTable;
            }
            else if (value == "g_x_load_gain")
            {
               tempControlSignalModifier->type = GxLoadGainTable;
            }
            else if (value == "g_y_load_gain")
            {
               tempControlSignalModifier->type = GyLoadGainTable;
            }
            else if (value == "g_z_load_gain")
            {
               tempControlSignalModifier->type = GzLoadGainTable;
            }
            else if (value == "alt_gain")
            {
               tempControlSignalModifier->type = AltGainTable;
            }
            else if (value == "q_gain")
            {
               tempControlSignalModifier->type = QGainTable;
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "simple_table")
         {
            tempControlSignalModifier->curve = ut::make_unique<UtTable::Curve>();
            tempControlSignalModifier->curve->ProcessInput(aInput,
                                                           UtInput::cNON_DIMENSIONAL,
                                                           "input",
                                                           UtTable::NoCheck(),
                                                           UtInput::cNON_DIMENSIONAL,
                                                           "gain",
                                                           UtTable::NoCheck());
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "scalar_gain")
   {
      tempControlSignalModifier             = new ControlSignalModifier();
      tempControlSignalModifier->type       = ScalarGain;
      tempControlSignalModifier->table      = nullptr;
      tempControlSignalModifier->curve      = nullptr;
      tempControlSignalModifier->scalarGain = 0;
      tempControlSignalModifier->minClamp   = -1;
      tempControlSignalModifier->maxClamp   = 1;

      UtInputBlock block(aInput, "end_scalar_gain");

      std::string name;
      aInput.ReadValue(name);
      tempControlSignalModifier->name = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "gain")
         {
            double value;
            aInput.ReadValue(value);
            tempControlSignalModifier->scalarGain = value;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "clamp_gain")
   {
      tempControlSignalModifier             = new ControlSignalModifier();
      tempControlSignalModifier->type       = ClampGain;
      tempControlSignalModifier->table      = nullptr;
      tempControlSignalModifier->curve      = nullptr;
      tempControlSignalModifier->scalarGain = 0;
      tempControlSignalModifier->minClamp   = -1;
      tempControlSignalModifier->maxClamp   = 1;

      UtInputBlock block(aInput, "end_clamp_gain");

      std::string name;
      aInput.ReadValue(name);
      tempControlSignalModifier->name = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "min_clamp")
         {
            double value;
            aInput.ReadValue(value);
            tempControlSignalModifier->minClamp = value;
         }
         else if (localCommand == "max_clamp")
         {
            double value;
            aInput.ReadValue(value);
            tempControlSignalModifier->maxClamp = value;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "sas_channel")
   {
      tempControlSignalModifier             = new ControlSignalModifier;
      tempControlSignalModifier->type       = SASChannel;
      tempControlSignalModifier->table      = nullptr;
      tempControlSignalModifier->curve      = nullptr;
      tempControlSignalModifier->scalarGain = 0;
      tempControlSignalModifier->minClamp   = -1;
      tempControlSignalModifier->maxClamp   = 1;

      UtInputBlock block(aInput, "end_sas_channel");

      std::string name;
      aInput.ReadValue(name);
      tempControlSignalModifier->name = name;

      while (block.ReadCommand())
      {
         // std::string localCommand;
         // localCommand = block.GetCommand();
         // TODO -- Add SAS support
      }
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }

   if (tempControlSignalModifier != nullptr)
   {
      // Add to the list, if name is unique

      for (auto iter : mControlSignalModifiersList)
      {
         if (iter->name == tempControlSignalModifier->name)
         {
            // Name is already in list, so ignore it
            delete tempControlSignalModifier;
            throw UtInput::BadValue(aInput, "Duplicate control signal name (" + tempControlSignalModifier->name + ")");
         }
      }

      // If we are here, the name is not in the list, so add the item
      mControlSignalModifiersList.push_back(tempControlSignalModifier);
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlSurfaceInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_surface")
   {
      auto tempSurface                = new ControlSurfaceElement();
      tempSurface->currentAngle_deg   = 0;
      tempSurface->minAngle_deg       = 0;
      tempSurface->maxAngle_deg       = 0;
      tempSurface->angleMappingAuto   = nullptr;
      tempSurface->angleMappingManual = nullptr;
      tempSurface->actuator           = nullptr;

      UtInputBlock block(aInput, "end_control_surface");

      std::string name;
      aInput.ReadValue(name);
      tempSurface->controlSurfaceName = name;

      bool minAngleValid = false;
      bool maxAngleValid = false;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (command == "min_angle")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            tempSurface->minAngle_deg = value;
            minAngleValid             = true;
         }
         else if (command == "max_angle")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            tempSurface->maxAngle_deg = value;
            maxAngleValid             = true;
         }
         else if (command == "current_angle")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            tempSurface->currentAngle_deg = value;
         }
         else if (localCommand == "inputs")
         {
            ProcessInputSet(aInput, tempSurface);
         }
         else if (command == "angle_mapping_table")
         {
            // This allows the less specific angle_mapping_table to be
            // used instead of angle_mapping_auto_table
            if (tempSurface->angleMappingAuto != nullptr)
            {
               delete tempSurface->angleMappingAuto;
               tempSurface->angleMappingAuto = nullptr;
            }

            tempSurface->angleMappingAuto = new UtTable::Curve;
            tempSurface->angleMappingAuto->ProcessInput(aInput,
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "normalized_input",
                                                        UtTable::NoCheck(),
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "angle_output_deg",
                                                        UtTable::NoCheck());
         }
         else if (command == "angle_mapping_auto_table")
         {
            if (tempSurface->angleMappingAuto != nullptr)
            {
               delete tempSurface->angleMappingAuto;
               tempSurface->angleMappingAuto = nullptr;
            }

            tempSurface->angleMappingAuto = new UtTable::Curve;
            tempSurface->angleMappingAuto->ProcessInput(aInput,
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "normalized_input",
                                                        UtTable::NoCheck(),
                                                        UtInput::cNON_DIMENSIONAL,
                                                        "angle_output_deg",
                                                        UtTable::NoCheck());
         }
         else if (command == "angle_mapping_manual_table")
         {
            if (tempSurface->angleMappingManual != nullptr)
            {
               delete tempSurface->angleMappingManual;
               tempSurface->angleMappingManual = nullptr;
            }

            tempSurface->angleMappingManual = new UtTable::Curve;
            tempSurface->angleMappingManual->ProcessInput(aInput,
                                                          UtInput::cNON_DIMENSIONAL,
                                                          "normalized_input",
                                                          UtTable::NoCheck(),
                                                          UtInput::cNON_DIMENSIONAL,
                                                          "angle_output_deg",
                                                          UtTable::NoCheck());
         }
         else if (command == "actuator")
         {
            tempSurface->actuator = new P6DofControlActuator(this);
            tempSurface->actuator->ProcessInput(aInput);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (!minAngleValid)
      {
         { // RAII block
            auto out = ut::log::warning() << "No MinAngle specified for constrol surface.";
            out.AddNote() << "Surface: " << tempSurface->controlSurfaceName;
         }
         delete tempSurface;
         return false;
      }

      if (!maxAngleValid)
      {
         { // RAII block
            auto out = ut::log::warning() << "No MaxAngle specified for constrol surface.";
            out.AddNote() << "Surface: " << tempSurface->controlSurfaceName;
         }
         delete tempSurface;
         return false;
      }

      bool addedOk = AddControlSurfaceToList(tempSurface);
      if (!addedOk)
      {
         throw UtInput::BadValue(aInput, "Duplicate control surface name (" + tempSurface->controlSurfaceName + ")");
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlValueInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_value")
   {
      auto tempValueElement          = new ControlValueElement();
      tempValueElement->currentValue = 0;
      tempValueElement->minValue     = 0;
      tempValueElement->maxValue     = 0;

      UtInputBlock block(aInput, "end_control_value");

      std::string name;
      aInput.ReadValue(name);
      tempValueElement->controlValueName = name;

      bool minValueValid = false;
      bool maxValueValid = false;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (command == "min_value")
         {
            double value;
            aInput.ReadValue(value);
            tempValueElement->minValue = value;
            minValueValid              = true;
         }
         else if (command == "max_value")
         {
            double value;
            aInput.ReadValue(value);
            tempValueElement->maxValue = value;
            maxValueValid              = true;
         }
         else if (command == "current_value")
         {
            double value;
            aInput.ReadValue(value);
            tempValueElement->currentValue = value;
         }
         else if (localCommand == "inputs")
         {
            ProcessInputSet(aInput, tempValueElement);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (!minValueValid)
      {
         { // RAII block
            auto out = ut::log::warning() << "No minValue specified for control value.";
            out.AddNote() << "Value: " << tempValueElement->controlValueName;
         }
         delete tempValueElement;
         return false;
      }

      if (!maxValueValid)
      {
         { // RAII block
            auto out = ut::log::warning() << "No maxValue specified for control value.";
            out.AddNote() << "Value: " << tempValueElement->controlValueName;
         }
         delete tempValueElement;
         return false;
      }

      bool addedOk = AddControlValueToList(tempValueElement);
      if (!addedOk)
      {
         throw UtInput::BadValue(aInput, "Duplicate control value name (" + tempValueElement->controlValueName + ")");
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlBooleanInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_boolean")
   {
      auto tempBoolElement            = new ControlBooleanElement();
      tempBoolElement->currentValue   = false;
      tempBoolElement->lastValue      = false;
      tempBoolElement->thresholdValue = 0.5;

      UtInputBlock block(aInput, "end_control_boolean");

      std::string name;
      aInput.ReadValue(name);
      tempBoolElement->controlValueName = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (command == "current_value")
         {
            int value;
            aInput.ReadValue(value);

            if (value == 0)
            {
               tempBoolElement->currentValue = false;
            }
            else
            {
               tempBoolElement->currentValue = true;
            }
         }
         else if (command == "threshold_value")
         {
            double value;
            aInput.ReadValue(value);
            tempBoolElement->thresholdValue = value;
         }
         else if (localCommand == "inputs")
         {
            ProcessInputSet(aInput, tempBoolElement);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      bool addedOk = AddControlBooleanToList(tempBoolElement);
      if (!addedOk)
      {
         throw UtInput::BadValue(aInput, "Duplicate control boolean name (" + tempBoolElement->controlValueName + ")");
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessInputSet(UtInput& aInput, ControlSurfaceElement* aActiveControlSurfaceElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "inputs")
   {
      UtInputBlock block(aInput, "end_inputs");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_input")
         {
            ProcessControlInputs(aInput, aActiveControlSurfaceElement);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessInputSet(UtInput& aInput, ControlValueElement* aActiveControlValueElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "inputs")
   {
      UtInputBlock block(aInput, "end_inputs");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_input")
         {
            ProcessControlInputs(aInput, aActiveControlValueElement);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessInputSet(UtInput& aInput, ControlBooleanElement* aActiveControlBooleanElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "inputs")
   {
      UtInputBlock block(aInput, "end_inputs");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_input")
         {
            ProcessControlInputs(aInput, aActiveControlBooleanElement);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlInputs(UtInput& aInput, ControlSurfaceElement* aActiveControlSurfaceElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_input")
   {
      auto inputGainStream          = new ControlInputGainStream();
      inputGainStream->controlInput = nullptr;

      UtInputBlock block(aInput, "end_control_input");

      std::string name;
      aInput.ReadValue(name);
      inputGainStream->controlInputName = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "modifier")
         {
            std::string modifierName;
            aInput.ReadValue(modifierName);

            ControlSignalModifier* controlSignalModifier = GetControlSignalModifierByName(modifierName);

            if (controlSignalModifier)
            {
               inputGainStream->modifierNameList.push_back(modifierName);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown control signal modifier (" + modifierName + ")");
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      aActiveControlSurfaceElement->controlInputList.push_back(inputGainStream);
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlInputs(UtInput& aInput, ControlValueElement* aActiveControlValueElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_input")
   {
      auto inputGainStream          = new ControlInputGainStream();
      inputGainStream->controlInput = nullptr;

      UtInputBlock block(aInput, "end_control_input");

      std::string name;
      aInput.ReadValue(name);
      inputGainStream->controlInputName = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "modifier")
         {
            std::string modifierName;
            aInput.ReadValue(modifierName);

            ControlSignalModifier* controlSignalModifier = GetControlSignalModifierByName(modifierName);

            if (controlSignalModifier)
            {
               inputGainStream->modifierNameList.push_back(modifierName);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown control signal modifier (" + modifierName + ")");
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      aActiveControlValueElement->controlInputList.push_back(inputGainStream);
   }

   return myCommand;
}

bool P6DofFlightControlSystem::ProcessControlInputs(UtInput& aInput, ControlBooleanElement* aActiveControlBooleanElement)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "control_input")
   {
      auto inputGainStream          = new ControlInputGainStream();
      inputGainStream->controlInput = nullptr;

      UtInputBlock block(aInput, "end_control_input");

      std::string name;
      aInput.ReadValue(name);
      inputGainStream->controlInputName = name;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "modifier")
         {
            std::string modifierName;
            aInput.ReadValue(modifierName);

            ControlSignalModifier* controlSignalModifier = GetControlSignalModifierByName(modifierName);

            if (controlSignalModifier)
            {
               inputGainStream->modifierNameList.push_back(modifierName);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown control signal modifier (" + modifierName + ")");
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      aActiveControlBooleanElement->controlInputList.push_back(inputGainStream);
   }

   return myCommand;
}

bool P6DofFlightControlSystem::Initialize(int64_t aSimTime_nanosec)
{
   mLastUpdateSimTime_nanosec = aSimTime_nanosec;

   ConnectInputsWithOutputs();

   // Initialize actuators
   for (auto controlSurfaceIter : mControlSurfaceList)
   {
      if (controlSurfaceIter->actuator)
      {
         controlSurfaceIter->actuator->Initialize(aSimTime_nanosec);
      }
   }

   return true;
}

bool P6DofFlightControlSystem::GetControlSurfacesUsingInput(std::vector<ControlSurfaceElement*>& list,
                                                            unsigned int                         aInputHandle)
{
   list.clear();

   bool atLeastOneItem = false;

   // Now, set the control surfaces based on control inputs
   for (auto surface : mControlSurfaceList)
   {
      for (auto controlInput : surface->controlInputList)
      {
         if (controlInput->controlInput->handle == aInputHandle)
         {
            // This control surface is controlled by the specified input
            list.push_back(surface);
            atLeastOneItem = true;
         }
      }
   }

   return atLeastOneItem;
}

void P6DofFlightControlSystem::GetControlSurfaceNames(std::vector<std::string>& aList)
{
   aList.clear();

   for (auto controlSurfaceIter : mControlSurfaceList)
   {
      aList.push_back(controlSurfaceIter->controlSurfaceName);
   }
}

void P6DofFlightControlSystem::GetAllControlSurfaceAndActuatorsAngles(std::vector<double>& aControlSurfaceAngles_deg,
                                                                      std::vector<double>& aActuatorAngles_deg) const
{
   aControlSurfaceAngles_deg.clear();
   aActuatorAngles_deg.clear();

   aControlSurfaceAngles_deg.reserve(NumFlightControlSurfaces());
   aActuatorAngles_deg.reserve(NumFlightControlSurfaces());

   for (auto controlSurfaceIter : mControlSurfaceList)
   {
      aControlSurfaceAngles_deg.push_back(controlSurfaceIter->currentAngle_deg);
      aActuatorAngles_deg.push_back(controlSurfaceIter->actuator->GetCurrentAngle_deg());
   }
}

void P6DofFlightControlSystem::SetAllControlSurfaceAndActuatorsAngles(const std::vector<double>& aControlSurfaceAngles_deg,
                                                                      const std::vector<double>& aActuatorAngles_deg)
{
   size_t numSurfaceAngles = aControlSurfaceAngles_deg.size();
   size_t numActuators     = aActuatorAngles_deg.size();

   if (numSurfaceAngles == 0)
   {
      return;
   }

   if (numSurfaceAngles != numActuators)
   {
      auto out = ut::log::warning() << "Number of surfaces does not match number of actuators.";
      out.AddNote() << "Num Surfaces: " << numSurfaceAngles;
      out.AddNote() << "Num Actuators: " << numActuators;
      return;
   }

   if (numSurfaceAngles != mControlSurfaceList.size())
   {
      auto out = ut::log::warning() << "Number of surfaces does not match number of control surfaces in object.";
      out.AddNote() << "Num Surfaces: " << numSurfaceAngles;
      out.AddNote() << "Num Control Surfaces: " << mControlSurfaceList.size(); // Not sure what to label this value
      return;
   }

   auto objectControlSurfaceIter = mControlSurfaceList.begin();
   auto actuatorIter             = aActuatorAngles_deg.begin();

   for (const double controlSurfaceAngle : aControlSurfaceAngles_deg)
   {
      (*objectControlSurfaceIter)->currentAngle_deg = controlSurfaceAngle;
      (*objectControlSurfaceIter)->actuator->SetCurrentAngle_deg(*actuatorIter);

      ++objectControlSurfaceIter;
      ++actuatorIter;
   }
}

void P6DofFlightControlSystem::SetControlSurfacesBasedOnControlInputs(int64_t aSimTime_nanosec)
{
   double tempGain = 0.0;

   for (auto controlSurfaceIter : mControlSurfaceList)
   {
      // This will be modified to set the surface based on inputs and various gains, etc
      double controlOutput = 0.0;

      for (auto listIter : controlSurfaceIter->controlInputList)
      {
         double controlInput = listIter->controlInput->normalizedValue;

         for (auto modifierIter : listIter->modifierList)
         {
            switch (modifierIter->type)
            {
            case ScalarGain:
               controlInput *= modifierIter->scalarGain;
               break;
            case ClampGain:
            {
               if (controlInput > modifierIter->maxClamp)
               {
                  controlInput = modifierIter->maxClamp;
               }
               if (controlInput < modifierIter->minClamp)
               {
                  controlInput = modifierIter->minClamp;
               }
            }
            break;
            case SASChannel:
               break;
            case MachGainTable:
               break;
            case KTASGainTable:
               break;
            case AlphaGainTable:
               tempGain = modifierIter->curve->Lookup(mState.GetAlpha_deg());
               controlInput *= tempGain;
               break;
            case BetaGainTable:
               break;
            case GxLoadGainTable:
               break;
            case GyLoadGainTable:
               break;
            case GzLoadGainTable:
               break;
            case AltGainTable:
               break;
            case QGainTable:
               break;
            case MachMappingTable:
               break;
            case KTASMappingTable:
               break;
            case AlphaMappingTable:
               break;
            case BetaMappingTable:
               break;
            case GxLoadMappingTable:
               break;
            case GyLoadMappingTable:
               break;
            case GzLoadMappingTable:
               break;
            case AltMappingTable:
               break;
            case QMappingTable:
               break;
            case SignalMappingTable:
               break;

            default:
               break;
            }
         }

         // Sum in this modification
         controlOutput += controlInput;
      }

      // We need to get the angle through "angle mapping"
      double angle_deg = 0;

      // The mapping depends on the current control mode

      // Get the active pilot
      if (mP6DofPilotManager != nullptr)
      {
         mP6DofPilotObjectPtr = mP6DofPilotManager->GetActivePilot();
      }

      if (controlSurfaceIter->angleMappingAuto)
      {
         angle_deg = controlSurfaceIter->angleMappingAuto->Lookup(controlOutput);
      }
      else
      {
         angle_deg = controlOutput;
      }

      if (controlSurfaceIter->actuator)
      {
         angle_deg = controlSurfaceIter->actuator->UpdateAndGetCurrentAngle_deg(aSimTime_nanosec, angle_deg);
      }

      if (angle_deg < controlSurfaceIter->minAngle_deg)
      {
         angle_deg = controlSurfaceIter->minAngle_deg;
      }
      if (angle_deg > controlSurfaceIter->maxAngle_deg)
      {
         angle_deg = controlSurfaceIter->maxAngle_deg;
      }

      controlSurfaceIter->currentAngle_deg = angle_deg;
   }
}

void P6DofFlightControlSystem::SetControlValuesBasedOnControlInputs()
{
   double tempGain = 0.0;

   for (auto controlValueIter : mControlValueList)
   {
      // This will be modified to set the surface based on inputs and various gains, etc
      double controlOutput = 0.0;

      for (auto listIter : controlValueIter->controlInputList)
      {
         double controlInput = listIter->controlInput->normalizedValue;

         for (auto modifierIter : listIter->modifierList)
         {
            switch (modifierIter->type)
            {
            case ScalarGain:
               controlInput *= modifierIter->scalarGain;
               break;
            case ClampGain:
            {
               if (controlInput > modifierIter->maxClamp)
               {
                  controlInput = modifierIter->maxClamp;
               }
               if (controlInput < modifierIter->minClamp)
               {
                  controlInput = modifierIter->minClamp;
               }
            }
            break;
            case SASChannel:
               break;
            case MachGainTable:
               break;
            case KTASGainTable:
               break;
            case AlphaGainTable:
               tempGain = modifierIter->curve->Lookup(mState.GetAlpha_deg());
               controlInput *= tempGain;
               break;
            case BetaGainTable:
               break;
            case GxLoadGainTable:
               break;
            case GyLoadGainTable:
               break;
            case GzLoadGainTable:
               break;
            case AltGainTable:
               break;
            case QGainTable:
               break;
            case MachMappingTable:
               break;
            case KTASMappingTable:
               break;
            case AlphaMappingTable:
               break;
            case BetaMappingTable:
               break;
            case GxLoadMappingTable:
               break;
            case GyLoadMappingTable:
               break;
            case GzLoadMappingTable:
               break;
            case AltMappingTable:
               break;
            case QMappingTable:
               break;
            case SignalMappingTable:
               break;

            default:
               break;
            }
         }

         // Sum in this modification
         controlOutput += controlInput;
      }

      double value = controlOutput;

      if (value < controlValueIter->minValue)
      {
         value = controlValueIter->minValue;
      }
      if (value > controlValueIter->maxValue)
      {
         value = controlValueIter->maxValue;
      }

      controlValueIter->currentValue = value;
   }
}

void P6DofFlightControlSystem::SetControlBooleansBasedOnControlInputs()
{
   double tempGain = 0.0;

   for (auto controlBoolIter : mControlBooleanList)
   {
      // This will be modified to set the surface based on inputs and various gains, etc
      double controlOutput = 0.0;

      for (auto listIter : controlBoolIter->controlInputList)
      {
         double controlInput = listIter->controlInput->normalizedValue;

         for (auto modifierIter : listIter->modifierList)
         {
            switch (modifierIter->type)
            {
            case ScalarGain:
               controlInput *= modifierIter->scalarGain;
               break;
            case ClampGain:
            {
               if (controlInput > modifierIter->maxClamp)
               {
                  controlInput = modifierIter->maxClamp;
               }
               if (controlInput < modifierIter->minClamp)
               {
                  controlInput = modifierIter->minClamp;
               }
            }
            break;
            case SASChannel:
               break;
            case MachGainTable:
               break;
            case KTASGainTable:
               break;
            case AlphaGainTable:
               tempGain = modifierIter->curve->Lookup(mState.GetAlpha_deg());
               controlInput *= tempGain;
               break;
            case BetaGainTable:
               break;
            case GxLoadGainTable:
               break;
            case GyLoadGainTable:
               break;
            case GzLoadGainTable:
               break;
            case AltGainTable:
               break;
            case QGainTable:
               break;
            case MachMappingTable:
               break;
            case KTASMappingTable:
               break;
            case AlphaMappingTable:
               break;
            case BetaMappingTable:
               break;
            case GxLoadMappingTable:
               break;
            case GyLoadMappingTable:
               break;
            case GzLoadMappingTable:
               break;
            case AltMappingTable:
               break;
            case QMappingTable:
               break;
            case SignalMappingTable:
               break;

            default:
               break;
            }
         }

         // Sum in this modification
         controlOutput += controlInput;
      }

      double value = controlOutput;

      // Set the last value
      controlBoolIter->lastValue = controlBoolIter->currentValue;

      // Set the new (current) value
      if (value > controlBoolIter->thresholdValue)
      {
         controlBoolIter->currentValue = true;
      }
      else
      {
         controlBoolIter->currentValue = false;
      }
   }
}

void P6DofFlightControlSystem::Update(int64_t aSimTime_nanosec, P6DofKinematicState& aState)
{
   // Set the state data
   mState = aState;

   // Check for the "testing" condition (used for feed-forward calculations in the autopilot)
   bool testing = GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting();

   if (aSimTime_nanosec == mLastUpdateSimTime_nanosec)
   {
      if (!testing)
      {
         return;
      }
   }

   // Get the active pilot
   if (mP6DofPilotManager != nullptr)
   {
      mP6DofPilotObjectPtr = mP6DofPilotManager->GetActivePilot();
   }

   if (mP6DofPilotObjectPtr)
   {
      // First, update the controls provider
      mP6DofPilotObjectPtr->Update(aSimTime_nanosec, aState);

      // Next, update all of the control input values via the controls provider
      for (auto controlInputIter : mControlInputList)
      {
         controlInputIter->normalizedValue = mP6DofPilotObjectPtr->ControlInputValue(controlInputIter->handle);
         controlInputIter->boolData        = mP6DofPilotObjectPtr->ControlInputValueBool(controlInputIter->handle);
      }

      // Now, set the control surfaces based on control inputs
      SetControlSurfacesBasedOnControlInputs(aSimTime_nanosec);

      // Now, set the control values based on control inputs
      SetControlValuesBasedOnControlInputs();

      // Now, set the control booleans based on control inputs
      SetControlBooleansBasedOnControlInputs();
   }

   if (!testing)
   {
      // Set the last time
      mLastUpdateSimTime_nanosec = aSimTime_nanosec;
   }
}

void P6DofFlightControlSystem::SetPilotObject(P6DofPilotObject* aP6DofPilotObject)
{
   mP6DofPilotObjectPtr = aP6DofPilotObject;
}

void P6DofFlightControlSystem::SetPilotManager(P6DofPilotManager* aP6DofPilotManager)
{
   mP6DofPilotManager = aP6DofPilotManager;
}

// ..............................................................................................................

size_t P6DofFlightControlSystem::GetControlSurfaceHandle(const std::string& aControlSurfaceName) const
{
   for (size_t i = 0; i < mControlSurfaceList.size(); ++i)
   {
      if (mControlSurfaceList.at(i)->controlSurfaceName == aControlSurfaceName)
      {
         return i + 1;
      } // Add one to get the 'handle'
   }
   return 0;
}

size_t P6DofFlightControlSystem::GetControlSurfaceHandleContainingString(const std::string& aString) const
{
   for (size_t i = 0; i < mControlSurfaceList.size(); ++i)
   {
      std::size_t found = mControlSurfaceList.at(i)->controlSurfaceName.find(aString);

      if (found != std::string::npos)
      {
         return i + 1; // Add one to get the 'handle'
      }
   }
   return 0;
}

size_t P6DofFlightControlSystem::GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1,
                                                                             const std::string& aString2) const
{
   for (size_t i = 0; i < mControlSurfaceList.size(); ++i)
   {
      std::size_t found1 = mControlSurfaceList.at(i)->controlSurfaceName.find(aString1);
      std::size_t found2 = mControlSurfaceList.at(i)->controlSurfaceName.find(aString2);

      if ((found1 != std::string::npos) && (found2 != std::string::npos))
      {
         return i + 1; // Add one to get the 'handle'
      }
   }
   return 0;
}

std::string P6DofFlightControlSystem::GetControlSurfaceName(size_t aControlSurfaceHandle) const
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      return mControlSurfaceList.at(aControlSurfaceHandle - 1)->controlSurfaceName;
   }
   return std::string();
}

double P6DofFlightControlSystem::GetControlSurfaceAngle_deg(size_t aControlSurfaceHandle) const
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      return mControlSurfaceList.at(aControlSurfaceHandle - 1)->currentAngle_deg;
   }
   return 0;
}

double P6DofFlightControlSystem::GetControlSurfaceMinAngle_deg(size_t aControlSurfaceHandle) const
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      return mControlSurfaceList.at(aControlSurfaceHandle - 1)->minAngle_deg;
   }
   return 0;
}

double P6DofFlightControlSystem::GetControlSurfaceMaxAngle_deg(size_t aControlSurfaceHandle) const
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      return mControlSurfaceList.at(aControlSurfaceHandle - 1)->maxAngle_deg;
   }
   return 0;
}

double P6DofFlightControlSystem::GetControlSurfaceValueNormalized(size_t aControlSurfaceHandle) const
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      ControlSurfaceElement* surface = mControlSurfaceList.at(aControlSurfaceHandle - 1);
      if (surface == nullptr)
      {
         return 0.0;
      }

      double minAngle_deg = surface->minAngle_deg;
      double maxAngle_deg = surface->maxAngle_deg;
      // Check for location of min/max
      if ((minAngle_deg < 0) && (maxAngle_deg > 0))
      {
         // Split on each side of zero. This is the nominal case.
         if (surface->currentAngle_deg >= 0)
         {
            return surface->currentAngle_deg / maxAngle_deg;
         }
         else
         {
            return -1.0 * (surface->currentAngle_deg / minAngle_deg);
         }
      }
      else if (minAngle_deg < 0)
      {
         // Negative side. If the max is <= zero then we only use
         // the min value for determining normalized values.
         return -1.0 * (surface->currentAngle_deg / minAngle_deg);
      }
      else if (maxAngle_deg > 0)
      {
         // Positive side. If the min is >= zero then we only use
         // the max value for determining normalized values.
         return surface->currentAngle_deg / maxAngle_deg;
      }
      else
      {
         // Both zero
         return 0.0;
      }
   }

   return 0;
}

void P6DofFlightControlSystem::SetControlSurfaceAngle_deg(size_t aControlSurfaceHandle, double aAngle_deg)
{
   if ((aControlSurfaceHandle > 0) && (aControlSurfaceHandle <= mControlSurfaceList.size()))
   {
      ControlSurfaceElement* surface = mControlSurfaceList.at(aControlSurfaceHandle - 1);

      if ((aAngle_deg <= surface->maxAngle_deg) && (aAngle_deg >= surface->minAngle_deg))
      {
         surface->currentAngle_deg = aAngle_deg;

         // Set the actuator as well
         if (surface->actuator != nullptr)
         {
            surface->actuator->SetCurrentAngle_deg(aAngle_deg);
         }
      }
   }
}

size_t P6DofFlightControlSystem::GetControlValueHandle(const std::string& aControlValueName) const
{
   for (size_t i = 0; i < mControlValueList.size(); ++i)
   {
      if (mControlValueList.at(i)->controlValueName == aControlValueName)
      {
         return i + 1;
      } // Add one to get the 'handle'
   }
   return 0;
}

std::string P6DofFlightControlSystem::GetControlValueName(size_t aControlValueHandle) const
{
   if ((aControlValueHandle > 0) && (aControlValueHandle <= mControlValueList.size()))
   {
      return mControlValueList.at(aControlValueHandle - 1)->controlValueName;
   }
   return std::string();
}

double P6DofFlightControlSystem::GetControlValue(size_t aControlValueHandle) const
{
   if ((aControlValueHandle > 0) && (aControlValueHandle <= mControlValueList.size()))
   {
      return mControlValueList.at(aControlValueHandle - 1)->currentValue;
   }
   return 0;
}

size_t P6DofFlightControlSystem::GetBooleanControlHandle(const std::string& aControlBooleanName) const
{
   for (size_t i = 0; i < mControlBooleanList.size(); ++i)
   {
      if (mControlBooleanList.at(i)->controlValueName == aControlBooleanName)
      {
         return i + 1;
      } // Add one to get the 'handle'
   }
   return 0;
}

std::string P6DofFlightControlSystem::GetBooleanControlName(size_t aControlBooleanHandle) const
{
   if ((aControlBooleanHandle > 0) && (aControlBooleanHandle <= mControlBooleanList.size()))
   {
      return mControlBooleanList.at(aControlBooleanHandle - 1)->controlValueName;
   }
   return std::string();
}

bool P6DofFlightControlSystem::GetControlBoolean(size_t aControlBooleanHandle) const
{
   if ((aControlBooleanHandle > 0) && (aControlBooleanHandle <= mControlBooleanList.size()))
   {
      return mControlBooleanList.at(aControlBooleanHandle - 1)->currentValue;
   }
   return false;
}

bool P6DofFlightControlSystem::GetControlBooleanLastValue(size_t aControlBooleanHandle) const
{
   if ((aControlBooleanHandle > 0) && (aControlBooleanHandle <= mControlBooleanList.size()))
   {
      return mControlBooleanList.at(aControlBooleanHandle - 1)->lastValue;
   }
   return false;
}

void P6DofFlightControlSystem::ClearAllControlData()
{
   mControlSurfaceList.clear();
   mControlValueList.clear();
   mControlBooleanList.clear();
}

bool P6DofFlightControlSystem::AddControlSurfaceToList(ControlSurfaceElement* aControlSurface)
{
   // Check to make sure name does not exist prior to adding it
   for (auto iter : mControlSurfaceList)
   {
      if (iter->controlSurfaceName == aControlSurface->controlSurfaceName)
      {
         return false;
      }
   }

   // Name does not exist, so we can add it to the list
   mControlSurfaceList.push_back(aControlSurface);

   return true;
}

bool P6DofFlightControlSystem::AddControlValueToList(ControlValueElement* aControlValue)
{
   // Check to make sure name does not exist prior to adding it
   for (auto iter : mControlValueList)
   {
      if (iter->controlValueName == aControlValue->controlValueName)
      {
         return false;
      }
   }

   // Name does not exist, so we can add it to the list
   mControlValueList.push_back(aControlValue);

   return true;
}

bool P6DofFlightControlSystem::AddControlBooleanToList(ControlBooleanElement* aControlBoolean)
{
   // Check to make sure name does not exist prior to adding it
   for (auto controlBoolean : mControlBooleanList)
   {
      if (controlBoolean->controlValueName == aControlBoolean->controlValueName)
      {
         return false;
      }
   }

   // Name does not exist, so we can add it to the list
   mControlBooleanList.push_back(aControlBoolean);
   return true;
}

size_t P6DofFlightControlSystem::NumFlightControlInputs() const
{
   return mControlInputList.size();
}

void P6DofFlightControlSystem::LoadFlightControlInputData(size_t aIndex, P6DOF::FlightControlInputValue* aData) const
{
   // Verify index (which cannot be less than zero since it is a size_t)
   if (aIndex < mControlInputList.size())
   {
      aData->controlInputName = mControlInputList.at(aIndex)->name;
      aData->boolData         = mControlInputList.at(aIndex)->boolData;
      aData->typeData         = mControlInputList.at(aIndex)->typeData;
      aData->normalizedValue  = static_cast<float>(mControlInputList.at(aIndex)->normalizedValue);
   }
}

size_t P6DofFlightControlSystem::NumFlightControlSurfaces() const
{
   return mControlSurfaceList.size();
}

void P6DofFlightControlSystem::LoadFlightControlSurfaceData(size_t aIndex, P6DOF::ControlSurfaceValue* aData) const
{
   // Verify index (which cannot be less than zero since it is a size_t)
   if (aIndex < mControlSurfaceList.size())
   {
      aData->controlSurfaceName = mControlSurfaceList.at(aIndex)->controlSurfaceName;
      aData->typeData           = P6DOF::DataType::AngleDeg;
      aData->value              = static_cast<float>(mControlSurfaceList.at(aIndex)->currentAngle_deg);
   }
}
