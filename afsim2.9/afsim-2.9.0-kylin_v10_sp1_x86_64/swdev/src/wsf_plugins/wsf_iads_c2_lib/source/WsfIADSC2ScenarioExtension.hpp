// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_IADS_C2_SCENARIO_EXTENSION_HPP
#define WSF_IADS_C2_SCENARIO_EXTENSION_HPP

#include <memory>

#include "wsf_iads_c2_lib_export.h"

#include "logger.hpp"

#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"

class WSF_IADS_C2_LIB_EXPORT IADSC2ScenarioExtension : public WsfScenarioExtension
{
   public:
      void AddedToScenario() override;
      void SimulationCreated(WsfSimulation& aSimulation) override;

      static const std::string cNAME;

      std::weak_ptr<il::logger::GlobalLogger> GetGlobalLogger() const { return std::weak_ptr<il::logger::GlobalLogger>(mGlobalLogger); }

      static std::weak_ptr<il::logger::GlobalLogger> GetGlobalLogger(const WsfScenario& aScenario)
      {
         return static_cast<IADSC2ScenarioExtension&>(aScenario.GetExtension(cNAME)).GetGlobalLogger();
      }

   private:
      std::shared_ptr<il::logger::GlobalLogger> mGlobalLogger;
};

#endif
