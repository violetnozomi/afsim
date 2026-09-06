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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfP6DOF_TypeManager.hpp"

#include "P6DofAtmosphere.hpp"
#include "P6DofGravity.hpp"
#include "P6DofJetEngineObject.hpp"
#include "P6DofRamjetEngineObject.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofVehicleType.hpp"
#include "P6DofWind.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEventPipe.hpp"
#include "WsfFuelTypes.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfP6DOF_EventPipe.hpp"
#include "WsfP6DOF_ExplicitWeapon.hpp"
#include "WsfP6DOF_Fuel.hpp"
#include "WsfP6DOF_GuidanceComputer.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfP6DOF_ObjectManager.hpp"
#include "WsfP6DOF_Observer.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponTypes.hpp"

WsfP6DOF_TypeManager::WsfP6DOF_TypeManager()
   : WsfScenarioExtension()
   , P6DofTypeManager()
   , mObjectPlatformMatchList()
{
}

WsfP6DOF_TypeManager::~WsfP6DOF_TypeManager() {}

void WsfP6DOF_TypeManager::AddedToScenario()
{
   WsfScenario& scenario(GetScenario());
   WsfMoverTypes::Get(scenario).Add("WSF_P6DOF_MOVER", ut::make_unique<WsfP6DOF_Mover>(scenario));
   WsfProcessorTypes::Get(scenario).Add("WSF_P6DOF_GUIDANCE_COMPUTER",
                                        ut::make_unique<WsfP6DOF_GuidanceComputer>(scenario));
   WsfFuelTypes::Get(scenario).Add("WSF_P6DOF_FUEL", ut::make_unique<WsfP6DOF_Fuel>(scenario));
   WsfWeaponTypes::Get(scenario).Add("WSF_P6DOF_EXPLICIT_WEAPON", ut::make_unique<WsfP6DOF_ExplicitWeapon>(scenario));

   // If the "event_pipe" extension has been defined then hook things up so our events will also be logged.
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr)
   {
      wsf::p6dof::EventPipe::RegisterEvents(*eventPipePtr);
   }
}

bool WsfP6DOF_TypeManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "p6dof_object_types")
   {
      myCommand = true;
      UtInputBlock block(aInput);

      std::string subCommand;
      while (block.ReadCommand(subCommand))
      {
         if (subCommand == "map_p6dof_object_to_platform")
         {
            std::string nameP6DofObjType;
            std::string namePlatformType;
            aInput.ReadValue(nameP6DofObjType);
            aInput.ReadValue(namePlatformType);

            AddP6DofVehiclePlatformTypeMatch(nameP6DofObjType, namePlatformType);
         }
         else if (P6DofTypeManager::ProcessInput(aInput))
         {
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within WsfP6DOF_TypeManager::ProcessInput().";
            out.AddNote() << "Command: " << subCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

void WsfP6DOF_TypeManager::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("p6dof_observer", ut::make_unique<WsfP6DOF_Observer>());
   aSimulation.RegisterExtension("wsf_p6dof_eventpipe", ut::make_unique<wsf::p6dof::EventPipeInterface>());
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfP6DOF_ObjectManager>(this));
}

void WsfP6DOF_TypeManager::AddP6DofVehiclePlatformTypeMatch(const std::string& aNameP6DofVehicleType,
                                                            const std::string& aNamePlatformType)
{
   bool addItem = true;
   // First, confirm there isn't already a match for this object type
   for (auto& object : mObjectPlatformMatchList)
   {
      if (object.nameP6DofVehicleType == aNameP6DofVehicleType)
      {
         // Already exists -- over-write the previous data
         object.namePlatformType = aNamePlatformType;
         addItem                 = false;
         break;
      }
   }

   if (addItem)
   {
      // Not in the list, so add it
      ObjectPlatformMatch item;
      item.nameP6DofVehicleType = aNameP6DofVehicleType;
      item.namePlatformType     = aNamePlatformType;
      mObjectPlatformMatchList.push_back(item);
   }
}

std::string WsfP6DOF_TypeManager::GetP6DofVehiclePlatformTypeMatch(const std::string& aNameP6DofVehicleType) const
{
   std::string pltfrmType = "";
   for (auto& match : mObjectPlatformMatchList)
   {
      if (match.nameP6DofVehicleType == aNameP6DofVehicleType)
      {
         // Match found
         pltfrmType = match.namePlatformType;
         break;
      }
   }
   return pltfrmType;
}
