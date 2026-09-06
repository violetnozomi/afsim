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

#include "WsfSixDOF_FormationSimulationExtension.hpp"

#include "UtMemory.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationScenarioExtension.hpp"

wsf::six_dof::FormationSimulationExtension::FormationSimulationExtension()
   : WsfSimulationExtension()
   , mManagerPtr{nullptr}
{
}

void wsf::six_dof::FormationSimulationExtension::AddedToSimulation()
{
   FormationScenarioExtension& scenExt =
      static_cast<FormationScenarioExtension&>(GetScenario().GetExtension("wsf_six_dof_formation"));
   mManagerPtr = ut::make_unique<FormationManager>(scenExt.GetManager(), &GetSimulation());
}

void wsf::six_dof::FormationSimulationExtension::PendingStart()
{
   mManagerPtr->SetInitialMemberManeuvers();
}

wsf::six_dof::FormationSimulationExtension& wsf::six_dof::FormationSimulationExtension::Get(const WsfSimulation* aSimulation)
{
   return static_cast<FormationSimulationExtension&>(aSimulation->GetExtension("wsf_six_dof_formation"));
}
