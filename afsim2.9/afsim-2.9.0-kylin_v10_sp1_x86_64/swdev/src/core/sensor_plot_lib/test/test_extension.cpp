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

#include <memory>

#include <gtest/gtest.h>

#include "MapPlotVariable.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"
#include "UtTestDataService.hpp"
#include "WsfApplication.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorPlot.hpp"
#include "WsfSensorResult.hpp"


namespace
{
class TestVar : public MapPlotVariable
{
public:
   TestVar()
      : MapPlotVariable(0.0)
   {
   }
   ~TestVar() override = default;

   double Evaluate(double           aSimTime,
                   WsfSimulation*   aSimulation,
                   double           aValue,
                   WsfSensor*       aSensor,
                   WsfSensorResult& aResult) const override
   {
      if (aResult.Detected())
      {
         return 1.0;
      }
      else
      {
         return 0.0;
      }
   };
};
} // namespace

TEST(WsfSensorPlotExtension, ProcessInput)
{
   const auto& dataService = ut::Locator<ut::TestDataService>::get();
   auto    file = ut::make_unique<UtInputFile>(dataService.getResource("extension_variable.txt").GetNormalizedPath());
   UtInput input;
   input.PushInput(std::move(file));
   WsfApplication app1("sp_tester", 0, {}, {"no_plugins"});
   WsfScenario    scn(app1);
   auto           spe = ut::make_unique<WsfSensorPlotExtension>(nullptr);
   spe->RegisterMapPlotVariable("test_ext", std::make_shared<TestVar>());
   scn.RegisterExtension("sp_ext", std::move(spe));

   EXPECT_TRUE(scn.ProcessInput(input));
}

TEST(WsfSensorPlotExtension, DuplicateMapPlotVariable)
{
   WsfSensorPlotExtension spe{nullptr};
   spe.RegisterMapPlotVariable("test_ext", std::make_shared<TestVar>());
   // Expect exception thrown when registering duplicate variable name
   EXPECT_THROW(spe.RegisterMapPlotVariable("test_ext", std::make_shared<TestVar>()), UtException);
}
