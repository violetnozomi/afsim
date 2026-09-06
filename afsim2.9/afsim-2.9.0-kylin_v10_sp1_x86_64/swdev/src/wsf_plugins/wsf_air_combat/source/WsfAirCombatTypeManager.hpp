// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFP6DOF_TYPEMANAGER_HPP
#define WSFP6DOF_TYPEMANAGER_HPP

#include "WsfAirCombatTypeManager.hpp"
#include "WsfScenarioExtension.hpp"

class UtInput;
class WsfSimulation;

class WsfAirCombatTypeManager : public WsfScenarioExtension
{
public:
   WsfAirCombatTypeManager();
   ~WsfAirCombatTypeManager() override;

   // The extension has been added to the scenario. GetScenario() is now callable.
   // This adds "WSF_SA_PROCESSOR".
   void AddedToScenario() override;

   // This calls WsfSimulation::RegisterExtension() every time a new simulation is
   // created, giving it a new air combat object manager.
   void SimulationCreated(WsfSimulation& aSimulation) override;
};

#endif
