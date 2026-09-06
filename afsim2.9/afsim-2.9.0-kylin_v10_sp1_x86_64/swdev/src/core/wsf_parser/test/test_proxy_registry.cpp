// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UnitTestFileHelper.h"
#include "UtMemory.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfParseDefinitions.hpp"

TEST(TestWsfPProxyRegistry, Setup)
{
   auto                grammarFile = UnitTestFilePathHelper::Instance().Path("core/wsf/grammar/wsf.ag");
   WsfParseDefinitions parseDefinitions;
   std::ifstream       grammarStream(grammarFile);
   parseDefinitions.AddGrammar("wsf.ag", grammarStream);
   parseDefinitions.Initialize();
   ASSERT_NE(nullptr, parseDefinitions.GetRootStruct());
   WsfPProxyRegistry testRegistry;
   testRegistry.Setup(parseDefinitions.GetRootStruct(), parseDefinitions.mBasicTypes);
   ASSERT_NE(nullptr, testRegistry.GetType("Int"));
   ASSERT_NE(nullptr, testRegistry.GetType("Real"));
   ASSERT_NE(nullptr, testRegistry.GetType("Latitude"));
   ASSERT_NE(nullptr, testRegistry.GetType("Longitude"));
   ASSERT_NE(nullptr, testRegistry.GetType("Position"));
   ASSERT_NE(nullptr, testRegistry.GetType("Length"));
   ASSERT_NE(nullptr, testRegistry.GetType("Time"));
   ASSERT_NE(nullptr, testRegistry.GetType("Speed"));
   ASSERT_NE(nullptr, testRegistry.GetType("DataSize"));
   ASSERT_NE(nullptr, testRegistry.GetType("Power"));
   ASSERT_NE(nullptr, testRegistry.GetType("PowerDB"));
   ASSERT_NE(nullptr, testRegistry.GetType("SolidAngle"));
   ASSERT_NE(nullptr, testRegistry.GetType("Mass"));
   ASSERT_NE(nullptr, testRegistry.GetType("Force"));
   ASSERT_NE(nullptr, testRegistry.GetType("Torque"));
   ASSERT_NE(nullptr, testRegistry.GetType("Area"));
   ASSERT_NE(nullptr, testRegistry.GetType("AreaDB"));
   ASSERT_NE(nullptr, testRegistry.GetType("Volume"));
   ASSERT_NE(nullptr, testRegistry.GetType("Time2"));
   ASSERT_NE(nullptr, testRegistry.GetType("Acceleration"));
   ASSERT_NE(nullptr, testRegistry.GetType("Frequency"));
   ASSERT_NE(nullptr, testRegistry.GetType("Angle"));
   ASSERT_NE(nullptr, testRegistry.GetType("AngleRate"));
   ASSERT_NE(nullptr, testRegistry.GetType("AngularAcceleration"));
   ASSERT_NE(nullptr, testRegistry.GetType("DataRate"));
   ASSERT_NE(nullptr, testRegistry.GetType("MassDensity"));
   ASSERT_NE(nullptr, testRegistry.GetType("MassTransfer"));
   ASSERT_NE(nullptr, testRegistry.GetType("Energy"));
   ASSERT_NE(nullptr, testRegistry.GetType("Fluence"));
   ASSERT_NE(nullptr, testRegistry.GetType("Irradiance"));
   ASSERT_NE(nullptr, testRegistry.GetType("Ratio"));
   ASSERT_NE(nullptr, testRegistry.GetType("NoisePressure"));
   ASSERT_NE(nullptr, testRegistry.GetType("Pressure"));
   ASSERT_NE(nullptr, testRegistry.GetType("Temperature"));
   ASSERT_NE(nullptr, testRegistry.GetType("SpecificRange"));
   ASSERT_NE(nullptr, testRegistry.GetType("AngularInertia"));
   ASSERT_NE(nullptr, testRegistry.GetType("String"));
   ASSERT_NE(nullptr, testRegistry.GetType("Length2"));
   ASSERT_NE(nullptr, testRegistry.GetType("Length3"));
   ASSERT_NE(nullptr, testRegistry.GetType("Bool"));
}
