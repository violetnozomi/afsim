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

#include "wsf_coverage_export.h"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfCoverageScenarioExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"

namespace // {anonymous}
{

class ApplicationExtension : public WsfApplicationExtension
{
public:
   void AddedToApplication(WsfApplication& aApplication) override
   {
      // Register script types.
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<wsf::coverage::ScenarioExtension>());
   }
};

} // namespace

void WSF_COVERAGE_EXPORT Register_wsf_coverage(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_coverage"))
   {
      aApplication.RegisterFeature("wsf_coverage");
      aApplication.RegisterFeature("coverage", "wsf_coverage");
      aApplication.RegisterExtension("wsf_coverage", ut::make_unique<ApplicationExtension>());
   }
}

extern "C"
{
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_coverage(aApplication); }
}
