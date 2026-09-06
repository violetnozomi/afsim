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

#include "WsfAtmosphereSimulationExtension.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfAtmosphereScenarioExtension.hpp"
#include "WsfAtmosphereTypes.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace space
{

bool AtmosphereSimulationExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command;
   aInput.GetCommand(command);
   if (command == "atmosphere_model")
   {
      UtInputBlock block{aInput};
      std::string  modelName;
      if (!block.ReadCommand(modelName))
      {
         throw UtInput::BadValue{aInput, "Expected atmosphere model name."};
      }
      if (mModels.find(modelName) != mModels.end())
      {
         std::stringstream msg{};
         msg << "Atmosphere model with name '" << modelName << "' already defined.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
      std::string modelType;
      if (!block.ReadCommand(modelType))
      {
         throw UtInput::BadValue{aInput, "Expected atmosphere model type."};
      }
      auto modelPrototypePtr = AtmosphereTypes::Get(mScenarioExtension.GetScenario()).Find(modelType);
      if (!modelPrototypePtr)
      {
         std::stringstream msg{};
         msg << "Unrecognized atmosphere model type '" << modelType << "'.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
      auto modelPtr = ut::clone(modelPrototypePtr);
      modelPtr->SetName(modelName);
      block.ProcessInput(modelPtr.get());
      mModels[modelName] = std::move(modelPtr);
      myCommand          = true;
   }
   return myCommand;
}

Atmosphere* AtmosphereSimulationExtension::GetAtmosphere(const std::string& aModelName) const
{
   Atmosphere* retvalPtr{nullptr};
   auto        iter = mModels.find(aModelName);
   if (iter != mModels.end())
   {
      retvalPtr = iter->second.get();
   }
   return retvalPtr;
}

const AtmosphereSimulationExtension& AtmosphereSimulationExtension::Get(const WsfSimulation& aSimulation)
{
   return static_cast<AtmosphereSimulationExtension&>(
      aSimulation.GetExtension(AtmosphereScenarioExtension::cEXTENSION_NAME));
}

} // namespace space
} // namespace wsf
