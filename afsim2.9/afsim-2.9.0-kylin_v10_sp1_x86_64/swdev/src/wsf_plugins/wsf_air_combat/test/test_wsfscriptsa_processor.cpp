//*****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include <string>
#include <vector>

#include "WsfSA_Processor.hpp"
#include "WsfScriptSA_Processor.hpp"
#include "gtest/gtest.h"

TEST(WsfScriptSA_Processor, GetUpdateTypeEnum)
{
   // Valid update type strings (same as scenario command input for corresponding update intervals)
   std::vector<std::string> updateTypeStrings = {
      "report_interval",                              // cSTATUS
      "engagement_data_update_interval",              // cENGAGEMENT_DATA
      "flight_data_update_interval",                  // cFLIGHT_DATA
      "fuel_data_update_interval",                    // cFUEL_DATA
      "nav_data_update_interval",                     // cNAV_DATA
      "flight_controls_data_update_interval",         // cCONTROLS_DATA
      "weapons_data_update_interval",                 // cWEAPONS_DATA
      "track_data_update_interval",                   // cTRACK_DATA
      "asset_data_update_interval",                   // cASSETS_DATA
      "perceived_item_data_update_interval",          // cPERCEIVED_ITEM_DATA
      "prioritized_item_data_update_interval",        // cPRIORITIZED_ITEM_DATA
      "perceived_item_calculation_update_interval",   // cPERCEIVED_ITEM_CALCULATION
      "prioritized_item_calculation_update_interval", // cPRIORITIZED_ITEM_CALCULATION
      "behavior_calculation_update_interval"          // cBEHAVIOR_CALCULATION
   };
   // Enumeration values that correspond to each valid update type string
   std::vector<WsfSA_Processor::UpdateType> updateTypeEnums = {WsfSA_Processor::UpdateType::cSTATUS,
                                                               WsfSA_Processor::UpdateType::cENGAGEMENT_DATA,
                                                               WsfSA_Processor::UpdateType::cFLIGHT_DATA,
                                                               WsfSA_Processor::UpdateType::cFUEL_DATA,
                                                               WsfSA_Processor::UpdateType::cNAV_DATA,
                                                               WsfSA_Processor::UpdateType::cCONTROLS_DATA,
                                                               WsfSA_Processor::UpdateType::cWEAPONS_DATA,
                                                               WsfSA_Processor::UpdateType::cTRACK_DATA,
                                                               WsfSA_Processor::UpdateType::cASSETS_DATA,
                                                               WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA,
                                                               WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA,
                                                               WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION,
                                                               WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION,
                                                               WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION};
   WsfSA_Processor::UpdateType              updateTypeEnum;

   // Test all valid update type strings
   for (size_t i = 0; i < WsfSA_Processor::cUPDATE_TYPE_COUNT; ++i)
   {
      EXPECT_TRUE(WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(updateTypeStrings[i], updateTypeEnum));
      EXPECT_EQ(updateTypeEnums[i], updateTypeEnum);
   }

   // Test an invalid update type string
   EXPECT_FALSE(WsfScriptSA_ProcessorClass::GetUpdateTypeEnum("invalid_update_interval", updateTypeEnum));

   // Test that the UpdateType enumeration uses an unsigned data type
   updateTypeEnum = static_cast<WsfSA_Processor::UpdateType>(-1);
   EXPECT_GT(updateTypeEnum, 0);
}

TEST(WsfScriptSA_Processor, GetGroupedTypes)
{
   // expected return vectors
   std::map<std::string, std::vector<WsfSA_Processor::UpdateType>> groupedUpdateTypesMap = {
      {"cognitive_update_interval",
       {WsfSA_Processor::UpdateType::cASSETS_DATA,
        WsfSA_Processor::UpdateType::cENGAGEMENT_DATA,
        WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA,
        WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA}},
      {"platform_update_interval",
       {WsfSA_Processor::UpdateType::cSTATUS,
        WsfSA_Processor::UpdateType::cFLIGHT_DATA,
        WsfSA_Processor::UpdateType::cFUEL_DATA,
        WsfSA_Processor::UpdateType::cNAV_DATA,
        WsfSA_Processor::UpdateType::cCONTROLS_DATA,
        WsfSA_Processor::UpdateType::cWEAPONS_DATA,
        WsfSA_Processor::UpdateType::cTRACK_DATA}},
      {"universal_update_interval",
       {WsfSA_Processor::UpdateType::cSTATUS,
        WsfSA_Processor::UpdateType::cENGAGEMENT_DATA,
        WsfSA_Processor::UpdateType::cFLIGHT_DATA,
        WsfSA_Processor::UpdateType::cFUEL_DATA,
        WsfSA_Processor::UpdateType::cNAV_DATA,
        WsfSA_Processor::UpdateType::cCONTROLS_DATA,
        WsfSA_Processor::UpdateType::cWEAPONS_DATA,
        WsfSA_Processor::UpdateType::cTRACK_DATA,
        WsfSA_Processor::UpdateType::cASSETS_DATA,
        WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA,
        WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA}}};

   std::vector<WsfSA_Processor::UpdateType> returnedGroupedTypesVec;

   for (std::map<std::string, std::vector<WsfSA_Processor::UpdateType>>::iterator i = groupedUpdateTypesMap.begin();
        i != groupedUpdateTypesMap.end();
        ++i)
   {
      EXPECT_TRUE(WsfScriptSA_ProcessorClass::GetGroupedTypes(i->first, returnedGroupedTypesVec));
      EXPECT_EQ(returnedGroupedTypesVec, i->second);
   }

   EXPECT_FALSE(WsfScriptSA_ProcessorClass::GetGroupedTypes("non_existent_type", returnedGroupedTypesVec));
}
