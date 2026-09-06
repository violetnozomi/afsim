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

#ifndef WSFCOVERAGESIMULATIONEXTENSION_HPP
#define WSFCOVERAGESIMULATIONEXTENSION_HPP

#include <map>
#include <string>

#include "UtCloneablePtr.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfSimulationExtension.hpp"

class UtInput;

namespace wsf
{
namespace coverage
{

class ScenarioExtension;

class SimulationExtension : public WsfCloneableSimulationExtension
{
public:
   explicit SimulationExtension(const ScenarioExtension& aScenarioExtension)
      : WsfCloneableSimulationExtension()
      , mScenarioExtension(aScenarioExtension)
   {
   }
   SimulationExtension(const SimulationExtension& aOther) = default;
   ~SimulationExtension() override                        = default;

   SimulationExtension* Clone() const override { return new SimulationExtension{*this}; }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize() override;
   void PendingStart() override;
   void Complete(double aSimTime) override;

   Grid*     GetGrid(const std::string& aGridName) const;
   Coverage* GetCoverage(const std::string& aCoverageName) const;

   static const SimulationExtension& Get(const WsfSimulation& aSimulation);

private:
   void CategorizeAllNonAssets(const WsfSimulation& aSimulation) const;
   void RemoveUnusedGrids();

   const ScenarioExtension&                          mScenarioExtension;
   std::map<std::string, ut::CloneablePtr<Grid>>     mGrids{};
   std::map<std::string, ut::CloneablePtr<Coverage>> mCoverages{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGESIMULATIONEXTENSION_HPP
