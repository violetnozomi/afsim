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

#include "xio/WsfXIO_WeaponExtension.hpp"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_WeaponComponent.hpp"
#include "xio/WsfXIO_WeaponInterface.hpp"
#include "xio_sim/WsfXIO_Extension.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

static const std::string sXIO_ExtensionName("xio_interface");
static const std::string sExtensionName("xio_weapon_interface");

namespace
{
class ScenarioExtension : public WsfScenarioExtension
{
public:
   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      // If the XIO simulation is present then connect our component.
      WsfXIO_Simulation* xioSimulationPtr = WsfXIO_Extension::FindSimulation(aSimulation);
      if (xioSimulationPtr != nullptr)
      {
         xioSimulationPtr->AddComponent(new WsfXIO_WeaponComponent);
      }
   }
};

class XIOWeaponApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<ScenarioExtension>());
   }
};
} // namespace

// =================================================================================================
//! Registration method invoked by WSF_REGISTER_EXTENSION(app, xio_weapon_interface);
void Register_xio_weapon_interface(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered(sExtensionName))
   {
      // Register my extension and make it dependent on the XIO extension.
      aApplication.RegisterExtension(sExtensionName, ut::make_unique<XIOWeaponApplicationExtension>());
      aApplication.ExtensionDepends(sExtensionName, sXIO_ExtensionName, false);
   }
}
