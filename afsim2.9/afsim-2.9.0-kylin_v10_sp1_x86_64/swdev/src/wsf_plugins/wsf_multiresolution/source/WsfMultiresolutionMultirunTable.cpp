// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMultiresolutionMultirunTable.hpp"

#include "ComponentNameHelper.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"

namespace
{
std::string KeyFromPlatformComponentName(const wsf::multiresolution::PlatformComponentName& aPlatformComponentName)
{
   return aPlatformComponentName.mPlatform + ' ' + aPlatformComponentName.mComponent + ' ' +
          aPlatformComponentName.mComponentName;
}
} // namespace

WsfMultiresolutionMultirunTable::WsfMultiresolutionMultirunTable(UtInput& aInput, const int aRunNumber)
   : mRunNumber(aRunNumber)
{
   ProcessInput(aInput);
   CreateFidelityMap(aRunNumber);
}

void WsfMultiresolutionMultirunTable::CreateFidelityMap(const int aRunNumber)
{
   const size_t modelCount      = mModelVec.size();
   size_t       tableStartIndex = (static_cast<size_t>(aRunNumber) - 1) * modelCount; // RunNumber is 1 indexed.
   if (tableStartIndex < mFidelityTable.size() || mLoop)
   {
      tableStartIndex %= mFidelityTable.size();
      for (size_t i = 0; i < modelCount; i++)
      {
         mFidelityMap[mModelVec[i]] = mFidelityTable[tableStartIndex + i];
      }
   }
   else
   {
      for (size_t i = 0; i < modelCount; i++)
      {
         mFidelityMap[mModelVec[i]] = mDefaultValues[i];
      }
   }
}

void WsfMultiresolutionMultirunTable::SimulationCreated(WsfSimulation& aSimulation)
{
   mRunNumber = aSimulation.GetRunNumber();
   CreateFidelityMap(mRunNumber);
}

std::vector<double> WsfMultiresolutionMultirunTable::ParseFidelityList(UtInputBlock& aInputBlock)
{
   std::vector<double> fidelityVec;
   std::string         command;
   while (aInputBlock.ReadCommand(command))
   {
      double fidelityVal;
      try
      {
         fidelityVal = ut::lexical_cast<double>(command);
      }
      catch (ut::bad_lexical_cast&)
      {
         throw UtInput::BadValue(aInputBlock.GetInput());
      }
      aInputBlock.GetInput().ValueInClosedRange(fidelityVal, 0.0, 1.0);
      fidelityVec.push_back(fidelityVal);
   }
   return fidelityVec;
}

bool WsfMultiresolutionMultirunTable::ProcessInput(UtInput& aInput)
{
   bool       myCommand          = false;
   bool       hasModelList       = false;
   bool       hasDefaultFidelity = false;
   bool       hasFidelityTable   = false;
   const auto componentNameMap   = wsf::multiresolution::ComponentTypeRequiresNameMap();

   std::string command = aInput.GetCommand();
   if (command == "multiresolution_multirun_table")
   {
      UtInputBlock tableBlock(aInput, "end_multiresolution_multirun_table");
      while (tableBlock.ReadCommand(command))
      {
         if (command == "model_list")
         {
            UtInputBlock inputBlock(aInput, "end_model_list");
            std::string  platformName;
            while (inputBlock.ReadCommand(platformName))
            {
               std::string componentType;
               inputBlock.ReadCommand(componentType);
               const auto inputTypeInMap = componentNameMap.find(componentType);
               if (inputTypeInMap == componentNameMap.cend())
               {
                  throw UtInput::BadValue(aInput, "Invalid model type: " + componentType);
               }
               std::string componentName;
               if (inputTypeInMap->second)
               {
                  // Named component
                  inputBlock.ReadCommand(componentName);
               }
               mModelVec.push_back(KeyFromPlatformComponentName({platformName, componentType, componentName}));
            }
            hasModelList = true;
         }
         else if (command == "default_fidelity")
         {
            UtInputBlock inputBlock(aInput, "end_default_fidelity");
            mDefaultValues     = ParseFidelityList(inputBlock);
            hasDefaultFidelity = true;
         }
         else if (command == "fidelity_table")
         {
            UtInputBlock inputBlock(aInput, "end_fidelity_table");
            mFidelityTable   = ParseFidelityList(inputBlock);
            hasFidelityTable = true;
         }
         else if (command == "loop_after_table_end")
         {
            mLoop = true;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Invalid command: " + command);
         }
      }
      if (!hasModelList) // Check that a model list exists
      {
         throw UtException("model_list not included in multiresolution_multirun_table block");
      }
      else if (!hasFidelityTable) // Check that a fidelity table exists
      {
         throw UtException("fidelity_table not included in multiresolution_multirun_table block");
      }
      else if (mFidelityTable.size() % mModelVec.size() != 0)
      {
         // Check that each row in the fidelity table has the right number of entries to match the number of models
         throw UtException("fidelity_table has invalid entry count");
      }
      else if (!hasDefaultFidelity && !mLoop)
      {
         // Check that either a default fidelity list exists, or that looping is enabled (which makes the default list redundant)
         throw UtException("Either default_fidelity is missing or loop_after_table_end is disabled");
      }
      else if (!mLoop && mModelVec.size() != mDefaultValues.size())
      {
         // If the default fidelity list is necessary, check that it's the same size as the model list
         throw UtException("default_fidelity count does not match model_list count");
      }
      else // Command block is only valid if all these checks pass
      {
         myCommand = true;
      }
   }
   return myCommand;
}

ut::optional<double> WsfMultiresolutionMultirunTable::GetFidelity(
   const wsf::multiresolution::PlatformComponentName& aPlatformComponentName) const
{
   const std::string key           = KeyFromPlatformComponentName(aPlatformComponentName);
   const auto        foundPlatform = mFidelityMap.find(key);
   if (foundPlatform != mFidelityMap.end())
   {
      return foundPlatform->second;
   }
   else
   {
      return ut::nullopt;
   }
}

WsfMultiresolutionMultirunTable* WsfMultiresolutionMultirunTable::Find(const WsfScenario& aScenario)
{
   return static_cast<WsfMultiresolutionMultirunTable*>(aScenario.FindExtension("multiresolution_multirun_table"));
}
