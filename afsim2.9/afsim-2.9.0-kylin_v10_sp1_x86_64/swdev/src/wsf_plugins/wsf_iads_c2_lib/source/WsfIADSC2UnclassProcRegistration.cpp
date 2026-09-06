// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfIADSC2UnclassProcRegistration.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include "WsfIADSC2UnclassProcRegistration.hpp"

#include <iostream>

#include "UtMemory.hpp"

#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

#include "script/WsfScriptManager.hpp"

#include "wsf_iads_c2_export.h"

#include "WsfAssetManager.hpp"
#include "WsfBattleManager.hpp"
#include "WsfBMDisseminateC2.hpp"
#include "WsfBMWeaponsManager.hpp"
#include "WsfBMMOELogger.hpp"
#include "WsfBMPluginUtilities.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"
#include "WsfSensorsManagerFov.hpp"
#include "WsfSimpleSensorsManager.hpp"
#include "WsfUnclassAssetManager.hpp"
#include "WsfUnclassBattleManager.hpp"
#include "WsfUnclassDisseminateC2.hpp"
#include "WsfWeaponsManagerAI.hpp"
#include "WsfWeaponsManagerSAM.hpp"

class WSF_IADS_C2_EXPORT IADSC2Extension : public WsfApplicationExtension
{
   public:
      void AddedToApplication(WsfApplication& aApplication) override
      {
         WsfUnclassifiedAssetManager::RegisterScriptTypes(&aApplication);
         WsfUnclassifiedBattleManager::RegisterScriptTypes(&aApplication);
         WsfUnclassifiedDisseminateC2::RegisterScriptTypes(&aApplication);
         WsfSimpleSensorsManager::RegisterScriptTypes(&aApplication);
         WsfWeaponsManagerSAM::RegisterScriptTypes(&aApplication);
         WsfWeaponsManagerAI::RegisterScriptTypes(&aApplication);
         WsfSensorsManagerFov::RegisterScriptTypes(&aApplication);
      }

      void ScenarioCreated(WsfScenario& aScenario) override
      {
         aScenario.RegisterExtension("wsf_iads_c2", ut::make_unique<IADSC2ScenarioExtension>());
      }
};

WSF_IADS_C2_EXPORT void Register_wsf_iads_c2(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_iads_c2"))
   {
      aApplication.RegisterFeature("wsf_iads_c2_plugin", "wsf_iads_c2");
      aApplication.RegisterExtension("wsf_iads_c2", ut::make_unique<IADSC2Extension>());
      aApplication.ExtensionDepends("wsf_iads_c2", "wsf_mil", true);
      aApplication.ExtensionDepends("wsf_iads_c2", "event_output", false);
      aApplication.ExtensionDepends("wsf_iads_c2", "csv_event_output", false);
   }
}

extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were built with
   //! the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion = UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION,
                                 WSF_PLUGIN_API_MINOR_VERSION,
                                 WSF_PLUGIN_API_COMPILER_STRING);
   }
   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to succeed.
   //! As we only have an application reference at load time, one usually registers an application extension, within which the
   //! ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs
   //! access to the simulation, one should also instantiate and register a simulation extension by overriding
   //! the SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
      Register_wsf_iads_c2(aApplication);
   }
}
