// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_MULTIRESOLUTION_MULTIRUN_TABLE_HPP
#define WSF_MULTIRESOLUTION_MULTIRUN_TABLE_HPP

#include "wsf_multiresolution_export.h"

#include <map>
#include <vector>

#include "UtInputBlock.hpp"
#include "UtOptional.hpp"
#include "WsfScenarioExtension.hpp"

class WsfScenario;

namespace wsf
{
namespace multiresolution
{
struct PlatformComponentName;
}
} // namespace wsf

//! Contains a table of fidelity values which can be mapped onto multiresolution models based on the run number
//! used with Monte Carlo iterations. Usage is that when SimulationCreated is called, any subsequent calls to
//! GetFidelity will return values for the run number (Monte Carlo iteration) associated with the WsfSimulation
//! passed on the call to SimulationCreated.
class WSF_MULTIRESOLUTION_EXPORT WsfMultiresolutionMultirunTable : public WsfScenarioExtension
{
public:
   WsfMultiresolutionMultirunTable() = default;
   //! @note This ctor is for testing. Typical use of this class would be to default construct and use ProcessInput.
   //! Then, for each new simulation, SimulationCreated will be called to set up the map of fidelity values
   //! corresponding to the current run number.
   WsfMultiresolutionMultirunTable(UtInput& aInput, int aRunNumber);

   //! @name Framework methods.
   //@{
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
   //@}

   //! @return The assigned fidelity value for this particular simulation (with a given run number set on a call
   //!  to SimulationCreated) for the specified platform and component given by @a aPlatform and @a aComponent.
   ut::optional<double> GetFidelity(const wsf::multiresolution::PlatformComponentName& aPlatformComponentName) const;

   static WsfMultiresolutionMultirunTable* Find(const WsfScenario& aScenario);

private:
   //! Creates the map of fidelity values for each model based on @a aRunNumber.
   void CreateFidelityMap(int aRunNumber);
   //! Processes the input block for lists of fidelity values
   std::vector<double> ParseFidelityList(UtInputBlock& aInputBlock);

   std::vector<std::string>      mModelVec;
   std::vector<double>           mDefaultValues;
   std::vector<double>           mFidelityTable;
   int                           mRunNumber = 1;
   bool                          mLoop      = false;
   std::map<std::string, double> mFidelityMap;
};

#endif
