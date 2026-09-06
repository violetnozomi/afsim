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

#ifndef WSFATMOSPHERESIMULATIONEXTENSION_HPP
#define WSFATMOSPHERESIMULATIONEXTENSION_HPP

#include <map>
#include <string>

#include "UtCloneablePtr.hpp"
#include "WsfAtmosphere.hpp"
#include "WsfSimulationExtension.hpp"

class UtInput;

namespace wsf
{
namespace space
{

class AtmosphereScenarioExtension;

class AtmosphereSimulationExtension : public WsfCloneableSimulationExtension
{
public:
   explicit AtmosphereSimulationExtension(const AtmosphereScenarioExtension& aScenarioExtension)
      : WsfCloneableSimulationExtension()
      , mScenarioExtension(aScenarioExtension)
   {
   }
   AtmosphereSimulationExtension(const AtmosphereSimulationExtension& aOther) = default;
   ~AtmosphereSimulationExtension() override                                  = default;

   AtmosphereSimulationExtension* Clone() const override { return new AtmosphereSimulationExtension{*this}; }

   bool ProcessInput(UtInput& aInput) override;

   Atmosphere* GetAtmosphere(const std::string& aModelName) const;

   static const AtmosphereSimulationExtension& Get(const WsfSimulation& aSimulation);

private:
   const AtmosphereScenarioExtension&                  mScenarioExtension;
   std::map<std::string, ut::CloneablePtr<Atmosphere>> mModels{};
};

} // namespace space
} // namespace wsf

#endif // WSFATMOSPHERESIMULATIONEXTENSION_HPP
