// ****************************************************************************
// CUI
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

#ifndef WSFDISEXTENSION_HPP
#define WSFDISEXTENSION_HPP

class UtInput;

class WsfApplication;
#include "WsfApplicationExtension.hpp"
class WsfScenario;
#include "WsfScenarioExtension.hpp"
class WsfSimulation;
#include "WsfDisInput.hpp"

namespace wsf
{
namespace dis
{

//! The WSF application extension for cyber.
class WSF_EXPORT ApplicationExtension : public WsfApplicationExtension
{
public:
   ~ApplicationExtension() override = default;
   void ScenarioCreated(WsfScenario& aScenario) override;
   void AddedToApplication(WsfApplication& aApplication) override;
};

class WSF_EXPORT ScenarioExtension : public WsfScenarioExtension, public WsfDisInput
{
public:
   static ScenarioExtension* Find(const WsfScenario& aScenario);
   void                      SimulationCreated(WsfSimulation& aSimulation) override;
   bool                      ProcessInput(UtInput& aInput) override;
};

} // namespace dis
} // namespace wsf
#endif
