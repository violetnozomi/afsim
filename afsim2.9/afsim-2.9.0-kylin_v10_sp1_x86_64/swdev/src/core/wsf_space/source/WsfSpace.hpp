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

#ifndef WSFSPACEEXTENSION_HPP
#define WSFSPACEEXTENSION_HPP

#include "wsf_space_export.h"

class UtInput;
#include "UtMemory.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
#include "WsfOrbitalTargetingCostTypes.hpp"
#include "WsfScenarioExtension.hpp"
class WsfSimulation;

class WSF_SPACE_EXPORT WsfSpaceExtension : public WsfScenarioExtension
{
public:
   static WsfSpaceExtension& Get(const WsfScenario& aScenario);

   void AddedToScenario() override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

   WsfOrbitalPropagatorTypes&             GetOrbitalPropagatorTypes() const { return *mOrbitalPropagatorTypes; }
   wsf::space::OrbitalTargetingCostTypes& GetOrbitalTargetingCostTypes() const
   {
      return *mOrbitalTargetingCostTypesPtr;
   }

private:
   std::unique_ptr<WsfOrbitalPropagatorTypes>             mOrbitalPropagatorTypes{nullptr};
   std::unique_ptr<wsf::space::OrbitalTargetingCostTypes> mOrbitalTargetingCostTypesPtr{nullptr};
};

#endif
