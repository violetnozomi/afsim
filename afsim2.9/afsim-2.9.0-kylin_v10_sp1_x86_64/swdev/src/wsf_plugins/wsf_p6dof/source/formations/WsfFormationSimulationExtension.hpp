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

#ifndef WSFFORMATIONSIMULATIONEXTENSION_HPP
#define WSFFORMATIONSIMULATIONEXTENSION_HPP

#include "wsf_p6dof_export.h"

#include <memory>

class WsfFormationManager;
#include "WsfSimulationExtension.hpp"

class WSF_P6DOF_EXPORT WsfFormationSimulationExtension : public WsfSimulationExtension
{
public:
   WsfFormationSimulationExtension();
   ~WsfFormationSimulationExtension() = default;

   void AddedToSimulation() override;

   void PendingStart() override;

   //! Get the formation manager owned by this extension.
   WsfFormationManager& GetManager() const { return *mManagerPtr.get(); }

   static WsfFormationSimulationExtension& Get(const WsfSimulation* aSimulation);

private:
   std::unique_ptr<WsfFormationManager> mManagerPtr;
};

#endif // WSFFORMATIONSIMULATIONEXTENSION_HPP
