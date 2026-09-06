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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "ApplicationExtension.hpp"

#include "InputConfig.hpp"
#include "ScriptExtensions.hpp"
#include "SimulationExtension.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace engage
{

// =================================================================================================
ApplicationExtension::ApplicationExtension()
   : WsfApplicationExtension()
{
}

// =================================================================================================
void ApplicationExtension::ScenarioCreated(WsfScenario& aScenario)
{
   // Add the scenario extension that reads and stores our input.
   aScenario.RegisterExtension("engage", ut::make_unique<InputConfig>());
}

// =================================================================================================
void ApplicationExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   // TODO aSimulation.RegisterExtension("engage", new SimulationExtension());
}

void ApplicationExtension::AddedToApplication(WsfApplication& aApplication)
{
   UtScriptTypes* scriptTypesPtr = aApplication.GetScriptTypes();

   // Add methods to existing script classes.
   ExtendScriptClasses(*scriptTypesPtr);
}

} // namespace engage
