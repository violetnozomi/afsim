// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: ScenarioAnalyzerPlugin.hpp
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

#include "wsf_scenario_analyzer_export.h"

#include "ScenarioAnalyzerPlugin.hpp"
#include "UtMemory.hpp"
#include "UtPlugin.hpp"
#include "UtScriptTypes.hpp"

#include "wsf/source/include/wsf_version_defines.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"


class WSF_SCENARIO_ANALYZER_EXPORT ScenarioAnalyzerExtension : public WsfApplicationExtension
{
   public:
      void AddedToApplication(WsfApplication& aApplication) override
      {
         ScenarioAnalyzerRegisterScriptTypes(aApplication);
      }
};

void WSF_SCENARIO_ANALYZER_EXPORT Register_wsf_scenario_analyzer(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_scenario_analyzer"))
   {
      aApplication.RegisterFeature("wsf_scenario_analyzer_plugin", "wsf_scenario_analyzer");
      aApplication.RegisterExtension("wsf_scenario_analyzer", ut::make_unique<ScenarioAnalyzerExtension>());
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
      Register_wsf_scenario_analyzer(aApplication);
   }
}
