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

#include "WsfIntegratingSpaceMover.hpp"

#include "UtMemory.hpp"
#include "WsfIntegratingPropagator.hpp"
#include "WsfSimulation.hpp"

WsfIntegratingSpaceMover::WsfIntegratingSpaceMover(const WsfScenario& aScenario)
   : WsfSpaceMoverBase(aScenario, ut::make_unique<WsfIntegratingPropagator>(aScenario), "WsfSpaceMover")
{
}

WsfMover* WsfIntegratingSpaceMover::Clone() const
{
   return new WsfIntegratingSpaceMover{*this};
}

bool WsfIntegratingSpaceMover::Initialize2(double aSimTime)
{
   bool ok = WsfSpaceMoverBase::Initialize2(aSimTime);

   UtCalendar startEpoch = GetSimulation()->GetDateTime().GetStartDateAndTime();
   startEpoch.AdvanceTimeBy(aSimTime);
   ok = ok && GetIntegratingPropagator().InitializeDynamics(GetSimulation(), startEpoch);

   UpdatePropagation(startEpoch);

   return ok;
}

const char* WsfIntegratingSpaceMover::GetScriptClassName() const
{
   return "WsfIntegratingSpaceMover";
}

WsfIntegratingPropagator& WsfIntegratingSpaceMover::GetIntegratingPropagator()
{
   return dynamic_cast<WsfIntegratingPropagator&>(GetPropagator());
}

std::unique_ptr<WsfOrbitalDynamics> WsfIntegratingSpaceMover::GetOrbitalDynamicsClone() const
{
   return dynamic_cast<const WsfIntegratingPropagator&>(GetPropagator()).GetOrbitalDynamicsClone();
}

bool WsfIntegratingSpaceMover::SetOrbitalDynamics(std::unique_ptr<WsfOrbitalDynamics> aDynamicsPtr)
{
   return GetIntegratingPropagator().SetOrbitalDynamics(std::move(aDynamicsPtr));
}

std::unique_ptr<WsfOrbitalIntegrator> WsfIntegratingSpaceMover::GetOrbitalIntegratorClone() const
{
   return dynamic_cast<const WsfIntegratingPropagator&>(GetPropagator()).GetOrbitalIntegratorClone();
}

bool WsfIntegratingSpaceMover::SetOrbitalIntegrator(std::unique_ptr<WsfOrbitalIntegrator> aIntegratorPtr)
{
   return GetIntegratingPropagator().SetOrbitalIntegrator(std::move(aIntegratorPtr));
}
