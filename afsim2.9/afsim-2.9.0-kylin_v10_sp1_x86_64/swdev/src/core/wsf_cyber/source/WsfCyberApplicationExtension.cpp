// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberApplicationExtension.hpp"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfCyberScriptExtensions.hpp"
#include "WsfScenario.hpp"

namespace wsf
{

namespace cyber
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
   ScriptExtensions::ExtendScriptClasses(*scriptTypesPtr);
}

} // namespace cyber

} // namespace wsf

// =================================================================================================
//! Register the extension with the application.
//!
//! CMake will dynamically generate 'wsf_extensions.hpp' which will call this method.
void WSF_CYBER_EXPORT Register_wsf_cyber(WsfApplication& aApplication)
{
   // Register the extension only if it hasn't already been registered.
   if (!aApplication.ExtensionIsRegistered("wsf_cyber"))
   {
      aApplication.RegisterFeature("cyber", "wsf_cyber");
      aApplication.RegisterExtension("wsf_cyber", ut::make_unique<wsf::cyber::ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_cyber", "event_pipe", false);
   }
}
