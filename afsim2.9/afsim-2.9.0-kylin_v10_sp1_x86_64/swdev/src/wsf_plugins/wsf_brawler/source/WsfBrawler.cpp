// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfBrawler.hpp"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfBrawlerFuel.hpp"
#include "WsfBrawlerMover.hpp"
#include "WsfBrawlerProcessor.hpp"
#include "WsfFuelTypes.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptBrawlerMoverClass.hpp"
#include "WsfScriptBrawlerProcessorClass.hpp"
#include "script/WsfScriptManager.hpp"

namespace
{
class ApplicationExtension : public WsfApplicationExtension
{
public:
   void AddedToApplication(WsfApplication& aApplication) override
   {
      // Register script classes associated with this extension
      UtScriptTypes* scriptTypesPtr = aApplication.GetScriptTypes();
      scriptTypesPtr->Register(ut::make_unique<WsfScriptBrawlerProcessorClass>("WsfBrawlerProcessor", scriptTypesPtr));
      scriptTypesPtr->Register(ut::make_unique<WsfScriptBrawlerMoverClass>("WsfBrawlerMover", scriptTypesPtr));
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      // NOTE: Everything that is done in the scenario extension constructor could have been done
      // inline right here. But in the interests of future extendability it was decided to leave it
      // this way. That way a future update could access scenario input, etc.
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfBrawler>());
   }
};
} // namespace

//! Registers the wsf_brawler extension with the application
//! so it is available for use.
void WSF_BRAWLER_EXPORT Register_wsf_brawler(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_brawler"))
   {
      aApplication.RegisterFeature("brawler", "wsf_brawler");
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_brawler", ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_brawler", "wsf_mil", true);
   }
}

void WsfBrawler::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_BRAWLER_PROCESSOR", ut::make_unique<WsfBrawlerProcessor>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_BRAWLER_MOVER", ut::make_unique<WsfBrawlerMover>(scenario));
   WsfFuelTypes::Get(scenario).AddCoreType("WSF_BRAWLER_FUEL", ut::make_unique<WsfBrawlerFuel>(scenario));
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
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_brawler(aApplication); }
}
