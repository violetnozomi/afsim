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

#include "dis/WsfDisExtension.hpp"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisScriptClass.hpp"
#include "ext/WsfExtInterface.hpp"

namespace wsf
{
namespace dis
{

// =================================================================================================
void ApplicationExtension::ScenarioCreated(WsfScenario& aScenario)
{
   aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<ScenarioExtension>());
}

// =================================================================================================
void ApplicationExtension::AddedToApplication(WsfApplication& aApplication)
{
   // Register new script classes and add new methods to existing script classes.
   UtScriptTypes* scriptTypesPtr = aApplication.GetScriptTypes();
   scriptTypesPtr->Register(ut::make_unique<WsfDisScriptClass>("WsfDIS", scriptTypesPtr));
}


// =================================================================================================
ScenarioExtension* ScenarioExtension::Find(const WsfScenario& aScenario)
{
   return static_cast<ScenarioExtension*>(aScenario.FindExtension("dis_interface"));
}

// =================================================================================================
void ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(),
                                 ut::make_unique<WsfDisInterface>(*this, aSimulation, WsfExtInterface::Find(aSimulation)));
}

// =================================================================================================
bool ScenarioExtension::ProcessInput(UtInput& aInput)
{
   return WsfDisInput::ProcessInput(GetScenario(), aInput);
}

} // namespace dis
} // namespace wsf

// ============================================================================
// DIS Extension Registration
WSF_EXPORT void Register_dis_interface(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("dis_interface"))
   {
      // This extension REQUIRES by the "ext_interface" extension.
      WSF_REGISTER_EXTENSION(aApplication, ext_interface);
      aApplication.RegisterExtension("dis_interface", ut::make_unique<wsf::dis::ApplicationExtension>());
      // Make sure ext_interface is defined before this
      aApplication.ExtensionDepends("dis_interface", "ext_interface", true);
   }
}
