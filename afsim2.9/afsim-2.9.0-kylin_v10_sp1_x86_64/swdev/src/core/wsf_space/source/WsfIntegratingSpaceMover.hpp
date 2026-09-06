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

#ifndef WSFINTEGRATINGSPACEMOVER_HPP
#define WSFINTEGRATINGSPACEMOVER_HPP

#include "wsf_space_export.h"

#include <memory>

#include "WsfSpaceMoverBase.hpp"

class WsfIntegratingPropagator;
class WsfOrbitalDynamics;
class WsfOrbitalIntegrator;

class WSF_SPACE_EXPORT WsfIntegratingSpaceMover : public WsfSpaceMoverBase
{
public:
   explicit WsfIntegratingSpaceMover(const WsfScenario& aScenario);
   WsfIntegratingSpaceMover(const WsfIntegratingSpaceMover& aOther) = default;
   WsfIntegratingSpaceMover(WsfIntegratingSpaceMover&& aOther)      = delete;
   ~WsfIntegratingSpaceMover() override                             = default;

   WsfIntegratingSpaceMover& operator=(const WsfIntegratingSpaceMover& aOther) = delete;
   WsfIntegratingSpaceMover& operator=(const WsfIntegratingSpaceMover&& aOther) = delete;

   WsfMover* Clone() const override;
   bool      Initialize2(double aSimTime) override;

   const char* GetScriptClassName() const override;

   std::unique_ptr<WsfOrbitalDynamics> GetOrbitalDynamicsClone() const;
   bool                                SetOrbitalDynamics(std::unique_ptr<WsfOrbitalDynamics> aDynamicsPtr);

   std::unique_ptr<WsfOrbitalIntegrator> GetOrbitalIntegratorClone() const;
   bool                                  SetOrbitalIntegrator(std::unique_ptr<WsfOrbitalIntegrator> aIntegratorPtr);

private:
   WsfIntegratingPropagator& GetIntegratingPropagator();
};

#endif // WSFINTEGRATINGSPACEMOVER_HPP
