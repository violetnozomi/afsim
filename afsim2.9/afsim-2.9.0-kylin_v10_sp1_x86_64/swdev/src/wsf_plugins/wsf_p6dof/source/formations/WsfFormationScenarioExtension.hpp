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

#ifndef WSFFORMATIONSCENARIOEXTENSION_HPP
#define WSFFORMATIONSCENARIOEXTENSION_HPP

#include "wsf_p6dof_export.h"

#include <memory>

class UtInput;

#include "WsfFormationManager.hpp"
#include "WsfScenarioExtension.hpp"
class WsfSimulation;

//! Add formation support to a scenario.
class WSF_P6DOF_EXPORT WsfFormationScenarioExtension : public WsfScenarioExtension
{
public:
   WsfFormationScenarioExtension();
   ~WsfFormationScenarioExtension() override;

   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   bool Complete2() override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

   //! Return the formation manager owned by this extension.
   WsfFormationManager& GetManager() const { return *mManagerPtr.get(); }

private:
   std::unique_ptr<WsfFormationManager> mManagerPtr;
};

#endif // WSFFORMATIONSCENARIOEXTENSION_HPP
