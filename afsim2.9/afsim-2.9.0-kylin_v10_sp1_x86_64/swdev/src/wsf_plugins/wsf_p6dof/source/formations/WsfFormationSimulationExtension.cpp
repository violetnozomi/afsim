// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfFormationSimulationExtension.hpp"

#include "UtMemory.hpp"
#include "WsfFormationManager.hpp"
#include "WsfFormationScenarioExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfFormationSimulationExtension::WsfFormationSimulationExtension()
   : WsfSimulationExtension()
   , mManagerPtr{nullptr}
{
}

void WsfFormationSimulationExtension::AddedToSimulation()
{
   WsfFormationScenarioExtension& scenExt =
      static_cast<WsfFormationScenarioExtension&>(GetScenario().GetExtension("wsf_formation"));
   mManagerPtr = ut::make_unique<WsfFormationManager>(scenExt.GetManager(), &GetSimulation());
}

void WsfFormationSimulationExtension::PendingStart()
{
   mManagerPtr->SetInitialMemberManeuvers();
}

WsfFormationSimulationExtension& WsfFormationSimulationExtension::Get(const WsfSimulation* aSimulation)
{
   return static_cast<WsfFormationSimulationExtension&>(aSimulation->GetExtension("wsf_formation"));
}
