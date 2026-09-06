// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "wsf_multiresolution_export.h"

#include "ComponentNameHelper.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfMultiresolutionMultirunTable.hpp"
#include "WsfMultiresolutionTypes.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"

namespace
{
using RegisterMultiresolutionComponent = WsfDefaultApplicationExtension<WsfMultiresolutionTypesRegistration>;
using RegisterMultirunTableExtension   = WsfDefaultApplicationExtension<WsfMultiresolutionMultirunTable>;
} // namespace

extern "C"
{
   //! This method is called to check the plugin version and compiler type.
   //! If values do not match the plugin will not load.
   WSF_MULTIRESOLUTION_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   //! This method is called to register the plugin with the application.
   WSF_MULTIRESOLUTION_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
      aApplication.RegisterFeature(wsf::multiresolution::cFEATURE_NAME, wsf::multiresolution::cEXTENSION_NAME);
      aApplication.RegisterExtension(wsf::multiresolution::cEXTENSION_NAME,
                                     ut::make_unique<RegisterMultiresolutionComponent>());
      aApplication.RegisterExtension("multiresolution_multirun_table", ut::make_unique<RegisterMultirunTableExtension>());
   }
}
