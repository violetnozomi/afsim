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
#include "UtInput.hpp"
#include "WsfMultiresolutionMultirunTable.hpp"

namespace
{
WsfMultiresolutionMultirunTable CreateMultirunTable(const std::string& aInputStr, const int aRunNumber)
{
   UtInput input{};
   input.PushInputString(aInputStr);
   return WsfMultiresolutionMultirunTable(input, aRunNumber);
}

WsfMultiresolutionMultirunTable CreateMultirunTableAndCheckNextCommand(const std::string& aInputStr,
                                                                       const int          aRunNumber,
                                                                       const std::string& aAdditionalCommand)
{
   UtInput input{};
   input.PushInputString(aInputStr);
   WsfMultiresolutionMultirunTable multirunTable(input, aRunNumber);
   // Check next command
   std::string nextCommand;
   input.ReadCommand(nextCommand);
   EXPECT_EQ(nextCommand, aAdditionalCommand);
   return multirunTable;
}
} // namespace

TEST(FidelityTableParse, FidelityTableInput)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   default_fidelity 0.55 0.55 0.55 end_default_fidelity
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
   end_multiresolution_multirun_table
   )INPUT";
   {
      const auto multirunTable = CreateMultirunTable(inputStr, 1);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_1", "mover"}).value(), 0.0);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_2", "mover"}).value(), 0.1);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_3", "mover"}).value(), 0.2);
   }
   {
      const auto multirunTable = CreateMultirunTable(inputStr, 2);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_1", "mover"}).value(), 0.3);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_2", "mover"}).value(), 0.4);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_3", "mover"}).value(), 0.5);
   }
   {
      const auto multirunTable = CreateMultirunTable(inputStr, 3);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_1", "mover"}).value(), 0.55);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_2", "mover"}).value(), 0.55);
      EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_3", "mover"}).value(), 0.55);
   }
}
TEST(FidelityTableParse, FidelityTableLoop)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   const auto multirunTable = CreateMultirunTable(inputStr, 3);
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_1", "mover"}).value(), 0.0);
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_2", "mover"}).value(), 0.1);
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_3", "mover"}).value(), 0.2);
}
TEST(FidelityTableParse, FidelityTableWithAdditionalInputs)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   platform platform_1 WSF_PLATFORM end_platform
   )INPUT";
   auto multirunTable = CreateMultirunTableAndCheckNextCommand(inputStr, 3, "platform");
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_1", "mover"}).value(), 0.0);
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_2", "mover"}).value(), 0.1);
   EXPECT_EQ(multirunTable.GetFidelity({"PLATFORM_3", "mover"}).value(), 0.2);
}
TEST(FidelityTableParse, ErrorInvalidFidelityTable)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
         0.6
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtException);
}
TEST(FidelityTableParse, ErrorNoModelList)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtException);
}
TEST(FidelityTableParse, ErrorNoDefault)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtException);
}
TEST(FidelityTableParse, ErrorInvalidDefault)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   default_fidelity 0.55 0.55 end_default_fidelity
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtException);
}
TEST(FidelityTableParse, ErrorNoFidelityTable)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   default_fidelity 0.55 0.55 0.55 end_default_fidelity
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtException);
}
TEST(FidelityTableParse, ErrorFidelityTableBadValue)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   default_fidelity 0.55 0.55 0.55 end_default_fidelity
	   fidelity_table
		   pizza 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtInput::BadValue);
}
TEST(FidelityTableParse, ErrorDefaultFidelityBadValue)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   default_fidelity pizza 0.55 0.55 end_default_fidelity
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   UtInput input{};
   input.PushInputString(inputStr);

   WsfMultiresolutionMultirunTable multirunTable;
   EXPECT_THROW(multirunTable.ProcessInput(input), UtInput::BadValue);
}
TEST(FidelityTableParse, ErrorNoModelInTable)
{
   const std::string inputStr =
      R"INPUT(
   multiresolution_multirun_table
	   model_list 
		   PLATFORM_1 mover 
		   PLATFORM_2 mover
		   PLATFORM_3 mover
	   end_model_list
	   fidelity_table
		   0.0 0.1 0.2
		   0.3 0.4 0.5
	   end_fidelity_table
      loop_after_table_end
   end_multiresolution_multirun_table
   )INPUT";
   const auto multirunTable = CreateMultirunTable(inputStr, 1);
   EXPECT_FALSE(multirunTable.GetFidelity({"PLATFORM_4", "mover"}).has_value());
}
