// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfAirCombatTypeManager.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptSA_Processor.hpp"

namespace
{
class ApplicationExtension : public WsfApplicationExtension
{
   void AddedToApplication(WsfApplication& aApplication) override
   {
      UtScriptTypes* typesPtr = aApplication.GetScriptTypes();

      typesPtr->Register(ut::make_unique<WsfScriptSA_ProcessorClass>("WsfSA_Processor", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSA_PerceivedItemClass>("WsfSA_PerceivedItem", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSA_EntityPerceptionClass>("WsfSA_EntityPerception", typesPtr));
      typesPtr->Register(ut::make_unique<WsfScriptSA_GroupClass>("WsfSA_Group", typesPtr));
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfAirCombatTypeManager>());
   }
};
} // namespace

void WSF_AIR_COMBAT_EXPORT Register_wsf_air_combat(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_air_combat"))
   {
      aApplication.RegisterFeature("air_combat", "wsf_air_combat");

      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_air_combat", ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_air_combat", "wsf_mil", true);

      WSF_REGISTER_EXTENSION(aApplication, wsf_brawler); // This extension REQUIRES the "wsf_brawler" extension
      aApplication.ExtensionDepends("wsf_air_combat", "wsf_brawler", true);

      WSF_REGISTER_EXTENSION(aApplication, wsf_p6dof); // This extension REQUIRES the "wsf_p6dof" extension
      aApplication.ExtensionDepends("wsf_air_combat", "wsf_p6dof", true);

      WSF_REGISTER_EXTENSION(aApplication, wsf_six_dof); // This extension REQUIRES the "wsf_six_dof" extension
      aApplication.ExtensionDepends("wsf_air_combat", "wsf_six_dof", true);

      aApplication.ExtensionDepends("wsf_air_combat", "event_pipe", false);
   }
}

extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were
   //! built with the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to
   //! succeed. As we only have an application reference at load time, one usually registers an application extension,
   //! within which the ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs access
   //! to the simulation, one should also instantiate and register a simulation extension by overriding the
   //! SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_air_combat(aApplication); }
}
