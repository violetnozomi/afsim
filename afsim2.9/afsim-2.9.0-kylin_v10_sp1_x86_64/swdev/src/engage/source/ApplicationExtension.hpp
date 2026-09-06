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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef APPLICATIONEXTENSION_HPP
#define APPLICATIONEXTENSION_HPP

#include "WsfApplicationExtension.hpp"

namespace engage
{

class ApplicationExtension : public WsfApplicationExtension
{
public:
   ApplicationExtension();

   void ScenarioCreated(WsfScenario& aScenario) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
   void AddedToApplication(WsfApplication& aApplication) override;
};

} // namespace engage

#endif
