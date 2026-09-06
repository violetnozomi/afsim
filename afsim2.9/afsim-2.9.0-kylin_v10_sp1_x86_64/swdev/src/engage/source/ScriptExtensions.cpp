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

#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"

namespace engage
{

namespace SimulationScriptMethods
{
// Get the Simulation Run Number
UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, RunNumber, 0, "int", "")
{
   Simulation& simulation = dynamic_cast<Simulation&>(*SIMULATION);
   aReturnVal.SetInt(simulation.GetTask().GetRunConfig().GetRunIndex() + 1);
}

// Get the Simulation Target Number
UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, TargetNumber, 0, "int", "")
{
   Simulation& simulation = dynamic_cast<Simulation&>(*SIMULATION);
   aReturnVal.SetInt(simulation.GetTask().GetTargetIndex() + 1);
}

// Get the Simulation Site Number
UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, SiteNumber, 0, "int", "")
{
   Simulation& simulation = dynamic_cast<Simulation&>(*SIMULATION);
   aReturnVal.SetInt(simulation.GetTask().GetSiteIndex() + 1);
}

// Get the Simulation Repetition Number
UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, Repetition, 0, "int", "")
{
   Simulation& simulation = dynamic_cast<Simulation&>(*SIMULATION);
   aReturnVal.SetInt(simulation.GetTask().GetRepetition() + 1);
}
} // namespace SimulationScriptMethods


void ExtendScriptClasses(UtScriptTypes& aTypes)
{
   // SimulationScriptMethods
   {
      using namespace SimulationScriptMethods;
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<RunNumber>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<TargetNumber>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<SiteNumber>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<Repetition>());
   }
}

} // namespace engage
