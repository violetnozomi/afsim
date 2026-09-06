// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtMemory.hpp"

#include "wsf_argo8_export.h"
#include "WsfARGO8_Mover.hpp"

#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfSimulation.hpp"

class WsfARGO8_Extension : public WsfScenarioExtension
{
   public:
      virtual void AddedToScenario()
      {
          GetScenario().GetMoverTypes().AddCoreType("WSF_ARGO8_MOVER", ut::make_unique<WsfARGO8_Mover>(GetScenario()));
      }
      ~WsfARGO8_Extension() {}
};

void WSF_ARGO8_EXPORT Register_wsf_argo8(WsfApplication& aApplication)
{
   if (! aApplication.ExtensionIsRegistered("wsf_argo8"))
   {
      aApplication.RegisterFeature("argo8", "wsf_argo8");
         aApplication.RegisterExtension("wsf_argo8", ut::make_unique<WsfDefaultApplicationExtension<WsfARGO8_Extension>>());
   }
}


extern "C"
{
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
      Register_wsf_argo8(aApplication);
   }
}



