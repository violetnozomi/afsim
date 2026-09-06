// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtMemory.hpp"
#include "WsfAltLocComponent.hpp"
#include "WsfAltLocSimulationExtension.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
using Component = wsf::altloc::Component;

//! Component factory to process platform input.
class ComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aParent) override
   {
      std::string command;
      aInput.GetCommand(command);
      bool myCommand = false;

      // 1. Read in the alt-loc block type
      if (command == Component::cCOMMAND)
      {
         // 2. Check if another alt-loc block exists (if so, error)
         Component* component = Component::Find(aParent);
         if (component != nullptr)
         {
            throw UtInput::BadValue(aInput,
                                    std::string{"An "} + Component::cCOMMAND +
                                       " block has already been placed on this platform");
         }

         // 3. Add a new component for our alt-loc block
         component = Component::FindOrCreate(aParent);

         if (component != Component::Find(aParent))
         {
            throw UtInput::BadValue(aInput,
                                    std::string{"Could not add an "} + Component::cCOMMAND + " component to this platform");
         }

         myCommand = component->ProcessInput(aInput);
         if (!myCommand)
         {
            throw UtInput::BadValue(aInput);
         }
      }
      return myCommand;
   }
};

//! A type that only uses functionality of base class to register itself with the scenario.
class ComponentTypes : public WsfObjectTypeList<Component>
{
public:
   ComponentTypes(WsfScenario& aScenario)
      : WsfObjectTypeList<Component>(aScenario, cREDEFINITION_ALLOWED, Component::cCOMMAND)
   {
      SetSingularBaseType();
      // Allows for definition inside platform, platform_type blocks.
      aScenario.RegisterComponentFactory(ut::make_unique<ComponentFactory>());
   }
};

class ScenarioExtension : public WsfScenarioExtension
{
public:
   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

private:
   wsf::altloc::InputData mData;
};

void ScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();
   scenario.AddTypeList(ut::make_unique<ComponentTypes>(scenario));
}

bool ScenarioExtension::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (command == "alternate_locations_global_debug")
   {
      mData.mGlobalDebugEnabled = aInput.ReadBool();
   }
   else if (command == "alternate_locations_use_global_draw")
   {
      mData.mGlobalDrawEnabled = aInput.ReadBool();
   }
   else if (command == "alternate_locations_global_draw")
   {
      aInput.ReadValue(mData.mGlobalDraw);
      if (mData.mGlobalDraw != Component::cINVALID_DRAW)
      {
         aInput.ValueInClosedRange(mData.mGlobalDraw, 0.0, 1.0);
      }
   }
   else
   {
      return false;
   }
   return true;
}

void ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<wsf::altloc::SimulationExtension>(mData));
}
} // namespace

void UT_PLUGIN_EXPORT Register_wsf_alternate_locations(WsfApplication& aApplication)
{
   using namespace wsf::altloc;
   if (!aApplication.ExtensionIsRegistered(SimulationExtension::cEXTENSION))
   {
      aApplication.RegisterFeature("alternate_locations", SimulationExtension::cEXTENSION);
      aApplication.RegisterExtension(SimulationExtension::cEXTENSION,
                                     ut::make_unique<WsfDefaultApplicationExtension<ScenarioExtension>>());
      aApplication.ExtensionDepends(SimulationExtension::cEXTENSION, "event_output", false);
   }
}

extern "C"
{
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& version)
   {
      version =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
      Register_wsf_alternate_locations(aApplication);
   }
}
