// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef LOADER_HPP
#define LOADER_HPP

#include "wsf_l16_export.h"

#include <memory>

#include "Interface.hpp"
#include "WsfScenarioExtension.hpp"

namespace WsfL16
{
class ComputerPartTypes;
}

class WSF_L16_EXPORT WsfL16_Extension : public WsfScenarioExtension
{
public:
   WsfL16_Extension(WsfScenario& aScenario);
   ~WsfL16_Extension() override;

   static WsfL16_Extension* Find(const WsfScenario& aScenario);
   bool                     ProcessInput(UtInput& aInput) override;

   //! Returns the name of the module this setup object creates
   void SimulationCreated(WsfSimulation& aSimulation) override;

   WsfL16::ComputerPartTypes& GetComputerPartTypes() const { return *mComputerPartTypes; }

   std::unique_ptr<WsfL16::ComputerPartTypes> mComputerPartTypes;
   WsfL16::InterfaceSetup                     mInterfaceSetup;
};
#endif
