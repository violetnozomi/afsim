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

#ifndef WSF_SIXDOF_FORMATIONSIMULATIONEXTENSION_HPP
#define WSF_SIXDOF_FORMATIONSIMULATIONEXTENSION_HPP

#include "wsf_six_dof_export.h"

#include <memory>

#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace six_dof
{
class FormationManager;

class WSF_SIX_DOF_EXPORT FormationSimulationExtension : public WsfSimulationExtension
{
public:
   FormationSimulationExtension();
   ~FormationSimulationExtension() = default;

   void AddedToSimulation() override;

   void PendingStart() override;

   //! Get the formation manager owned by this extension.
   FormationManager& GetManager() const { return *mManagerPtr.get(); }

   static FormationSimulationExtension& Get(const WsfSimulation* aSimulation);

private:
   std::unique_ptr<FormationManager> mManagerPtr;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONSIMULATIONEXTENSION_HPP
