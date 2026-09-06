// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalTargetingCostTypes.hpp"

#include "UtInput.hpp"
#include "WsfOrbitalTargetingCost.hpp"
#include "WsfSpace.hpp"

namespace wsf
{
namespace space
{

OrbitalTargetingCostTypes::OrbitalTargetingCostTypes()
{
   // Add the wsf_space provided cost functions.
   AddObjectFactory(
      [](const std::string& aTypeName) -> std::unique_ptr<OrbitalTargetingCost>
      {
         if (aTypeName == "blended")
         {
            return ut::make_unique<BlendedCost>();
         }
         return nullptr;
      });
}

OrbitalTargetingCostTypes& OrbitalTargetingCostTypes::Get(WsfScenario& aScenario)
{
   return WsfSpaceExtension::Get(aScenario).GetOrbitalTargetingCostTypes();
}

const OrbitalTargetingCostTypes& OrbitalTargetingCostTypes::Get(const WsfScenario& aScenario)
{
   return WsfSpaceExtension::Get(aScenario).GetOrbitalTargetingCostTypes();
}

bool OrbitalTargetingCostTypes::LoadInstance(UtInput&                               aInput,
                                             std::unique_ptr<OrbitalTargetingCost>& aOrbitalTargetingCost) const
{
   bool        myCommand{false};
   std::string command{aInput.GetCommand()};
   if (command == "optimize_cost")
   {
      std::string costType;
      aInput.ReadCommand(costType);
      aOrbitalTargetingCost = CreateInstance(costType);
      if (aOrbitalTargetingCost == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown optimization cost type: " + costType);
      }

      myCommand = aOrbitalTargetingCost->ProcessInput(aInput);
   }
   return myCommand;
}

void OrbitalTargetingCostTypes::AddObjectFactory(FactoryFunction aFactoryFunction)
{
   mObjectFactoryList.push_back(aFactoryFunction);
}

std::unique_ptr<OrbitalTargetingCost> OrbitalTargetingCostTypes::CreateInstance(const std::string& aTypeName) const
{
   for (const auto& factory : mObjectFactoryList)
   {
      auto instancePtr = factory(aTypeName);
      if (instancePtr)
      {
         return instancePtr;
      }
   }
   return nullptr;
}

} // namespace space
} // namespace wsf
