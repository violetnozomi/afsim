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

#ifndef WSF_SIXDOF_FORMATIONSCENARIOEXTENSION_HPP
#define WSF_SIXDOF_FORMATIONSCENARIOEXTENSION_HPP

#include "wsf_six_dof_export.h"

#include <memory>

#include "WsfScenarioExtension.hpp"
#include "WsfSixDOF_FormationManager.hpp"

class UtInput;
class WsfSimulation;

namespace wsf
{
namespace six_dof
{
//! Add formation support to a scenario.
class WSF_SIX_DOF_EXPORT FormationScenarioExtension : public WsfScenarioExtension
{
public:
   FormationScenarioExtension();
   ~FormationScenarioExtension() override;

   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   bool Complete2() override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

   //! Return the formation manager owned by this extension.
   FormationManager& GetManager() const { return *mManagerPtr.get(); }

private:
   std::unique_ptr<FormationManager> mManagerPtr;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONSCENARIOEXTENSION_HPP
