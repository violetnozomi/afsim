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

#include <gtest/gtest.h>

#include "ComponentNameHelper.hpp"
#include "WsfMover.hpp"

namespace
{
TEST(ComponentNames, Mover)
{
   namespace wm = wsf::multiresolution;
   EXPECT_EQ(wm::CommandName<WsfMover>(), std::string("multiresolution_mover"));
   EXPECT_EQ(wm::BaseTypeName<WsfMover>(), std::string("WSF_MULTIRESOLUTION_MOVER"));
   EXPECT_FALSE(wm::RequiresUserInputName<WsfMover>());

   EXPECT_EQ(std::string(WsfMover::cTYPE_KIND), std::string(wm::ComponentName<WsfMover>::cLOWER_CASE_NAME));
}

TEST(ComponentNames, Sensor)
{
   namespace wm = wsf::multiresolution;
   EXPECT_EQ(wm::CommandName<WsfSensor>(), std::string("multiresolution_sensor"));
   EXPECT_EQ(wm::BaseTypeName<WsfSensor>(), std::string("WSF_MULTIRESOLUTION_SENSOR"));
   EXPECT_TRUE(wm::RequiresUserInputName<WsfSensor>());
}

TEST(ComponentNames, Fuel)
{
   namespace wm = wsf::multiresolution;
   EXPECT_EQ(wm::CommandName<WsfFuel>(), std::string("multiresolution_fuel"));
   EXPECT_EQ(wm::BaseTypeName<WsfFuel>(), std::string("WSF_MULTIRESOLUTION_FUEL"));
   EXPECT_FALSE(wm::RequiresUserInputName<WsfFuel>());
}

TEST(ComponentNames, Comm)
{
   namespace wm = wsf::multiresolution;
   EXPECT_EQ(wm::CommandName<wsf::comm::Comm>(), std::string("multiresolution_comm"));
   EXPECT_EQ(wm::BaseTypeName<wsf::comm::Comm>(), std::string("WSF_MULTIRESOLUTION_COMM"));
   EXPECT_TRUE(wm::RequiresUserInputName<wsf::comm::Comm>());
}

TEST(ComponentNames, Processor)
{
   namespace wm = wsf::multiresolution;
   EXPECT_EQ(wm::CommandName<WsfProcessor>(), std::string("multiresolution_processor"));
   EXPECT_EQ(wm::BaseTypeName<WsfProcessor>(), std::string("WSF_MULTIRESOLUTION_PROCESSOR"));
   EXPECT_TRUE(wm::RequiresUserInputName<WsfProcessor>());
}
TEST(ComponentNames, TypeInputMap)
{
   namespace wm                = wsf::multiresolution;
   const auto componentTypeMap = wm::ComponentTypeRequiresNameMap();
   // Named components
   EXPECT_TRUE(componentTypeMap.at("comm"));
   EXPECT_TRUE(componentTypeMap.at("processor"));
   EXPECT_TRUE(componentTypeMap.at("sensor"));
   // Unnamed components
   EXPECT_FALSE(componentTypeMap.at("fuel"));
   EXPECT_FALSE(componentTypeMap.at("mover"));
   EXPECT_FALSE(componentTypeMap.at("acoustic_signature"));
   EXPECT_FALSE(componentTypeMap.at("infrared_signature"));
   EXPECT_FALSE(componentTypeMap.at("optical_signature"));
   EXPECT_FALSE(componentTypeMap.at("radar_signature"));
}

TEST(ComponentNames, AcousticSignature)
{
   EXPECT_EQ(wsf::multiresolution::CommandName<WsfAcousticSignature>(), std::string("multiresolution_acoustic_signature"));
   EXPECT_EQ(wsf::multiresolution::BaseTypeName<WsfAcousticSignature>(),
             std::string("WSF_MULTIRESOLUTION_ACOUSTIC_SIGNATURE"));
   EXPECT_FALSE(wsf::multiresolution::RequiresUserInputName<WsfAcousticSignature>());
}

TEST(ComponentNames, InfraredSignature)
{
   EXPECT_EQ(wsf::multiresolution::CommandName<WsfInfraredSignature>(), std::string("multiresolution_infrared_signature"));
   EXPECT_EQ(wsf::multiresolution::BaseTypeName<WsfInfraredSignature>(),
             std::string("WSF_MULTIRESOLUTION_INFRARED_SIGNATURE"));
   EXPECT_FALSE(wsf::multiresolution::RequiresUserInputName<WsfInfraredSignature>());
}

TEST(ComponentNames, OpticalSignature)
{
   EXPECT_EQ(wsf::multiresolution::CommandName<WsfOpticalSignature>(), std::string("multiresolution_optical_signature"));
   EXPECT_EQ(wsf::multiresolution::BaseTypeName<WsfOpticalSignature>(),
             std::string("WSF_MULTIRESOLUTION_OPTICAL_SIGNATURE"));
   EXPECT_FALSE(wsf::multiresolution::RequiresUserInputName<WsfOpticalSignature>());
}

TEST(ComponentNames, RadarSignature)
{
   EXPECT_EQ(wsf::multiresolution::CommandName<WsfRadarSignature>(), std::string("multiresolution_radar_signature"));
   EXPECT_EQ(wsf::multiresolution::BaseTypeName<WsfRadarSignature>(), std::string("WSF_MULTIRESOLUTION_RADAR_SIGNATURE"));
   EXPECT_FALSE(wsf::multiresolution::RequiresUserInputName<WsfRadarSignature>());
}

} // namespace
