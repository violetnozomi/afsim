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

#include "FiresTableLoader.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "FiresTable.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"

namespace Fires
{

//// Test routine declaration.
// void UnitTestTable(FiresTable*& aTablePtr);

//// virtual
// FiresTableLoader::~FiresTableLoader()
//{
// }

//! Process a single table (either a "fires" or "firesim" table).
FiresTable* FiresTableLoader::ProcessTable(UtInput& aInput)
{
   FiresTable* tablePtr = nullptr;
   std::string command  = aInput.GetCommand();
   if (command == "fires_table")
   {
      LoadFiresTable(aInput, tablePtr);
   }
   else if (command == "fires_elevation_angle_table")
   {
      LoadFiresElevationAngleTable(aInput, tablePtr);
   }
   else if (command == "fires_maximum_ordinate_table")
   {
      LoadFiresMaximumOrdinateTable(aInput, tablePtr);
   }
   return tablePtr;
}

// private
void FiresTableLoader::GetUnits(UtInput& aInput, std::string& aUnits)
{
   std::string command;
   aInput.ReadCommand(command);
   if (isalpha(command[0]))
   {
      aUnits = command;
   }
   else
   {
      aInput.PushBack(command);
   }
}

// private
void FiresTableLoader::LoadFiresTable(UtInput& aInput, FiresTable*& aTablePtr)
{
   std::unique_ptr<FiresTable> tablePtr(new FiresTable());
   std::vector<double>         values;
   std::string                 systemMunition;

   std::string command;
   aInput.ReadCommand(systemMunition);
   UtInputBlock block(aInput, "end_fires_table");
   while (block.ReadCommand(command))
   {
      if (command == "range_values")
      {
         UtInputBlock rangeBlock(block.GetInput(), "end_range_values");
         std::string  units = "meters";
         GetUnits(rangeBlock.GetInput(), units);
         while (rangeBlock.ReadCommand(command))
         {
            rangeBlock.GetInput().PushBack(command);
            double range;
            rangeBlock.GetInput().ReadValue(range);
            UtInput::ConvertValueFrom(range, units, UtInput::cLENGTH);
            values.push_back(range);
         }
         tablePtr->mRangeValues.SetValues(values);
         values.clear();
      }
      else if (command == "maximum_ordinate_values")
      {
         UtInputBlock maxOrdBlock(block.GetInput(), "end_maximum_ordinate_values");
         std::string  units = "meters";
         GetUnits(maxOrdBlock.GetInput(), units);
         while (maxOrdBlock.ReadCommand(command))
         {
            maxOrdBlock.GetInput().PushBack(command);
            double maxOrd;
            maxOrdBlock.GetInput().ReadValue(maxOrd);
            UtInput::ConvertValueFrom(maxOrd, units, UtInput::cLENGTH);
            values.push_back(maxOrd);
         }
         tablePtr->mMaxOrdValues.SetValues(values);
         values.clear();
         tablePtr->mUseMaxOrd = true;
      }
      else if (command == "elevation_angle_values")
      {
         UtInputBlock elevationAngleBlock(block.GetInput(), "end_elevation_angle_values");
         std::string  units = "mils";
         GetUnits(elevationAngleBlock.GetInput(), units);
         while (elevationAngleBlock.ReadCommand(command))
         {
            elevationAngleBlock.GetInput().PushBack(command);
            double elevationAngle;
            elevationAngleBlock.GetInput().ReadValue(elevationAngle);

            // elevation angle in mils; convert to radians
            elevationAngle = UtInput::ConvertValueFrom(elevationAngle, units, UtInput::cANGLE);
            // elevationAngle *= UtMath::cTWO_PI / 6400.0;
            values.push_back(elevationAngle);
         }
         tablePtr->mElevationValues.SetValues(values);
         values.clear();
         tablePtr->mUseMaxOrd = false;
      }
      else if (command == "time_of_flight_values")
      {
         UtInputBlock timeOfFlightBlock(block.GetInput(), "end_time_of_flight_values");
         std::string  units = "seconds";
         GetUnits(timeOfFlightBlock.GetInput(), units);
         while (timeOfFlightBlock.ReadCommand(command))
         {
            timeOfFlightBlock.GetInput().PushBack(command);
            double timeOfFlight;
            timeOfFlightBlock.GetInput().ReadValue(timeOfFlight);
            UtInput::ConvertValueFrom(timeOfFlight, units, UtInput::cTIME);
            values.push_back(timeOfFlight);
         }
         tablePtr->mTimeOfFlight.SetValues(values);
         values.clear();
      }
   }

   tablePtr->SetName(systemMunition);
   aTablePtr = tablePtr.get();
   tablePtr.release();
}

void FiresTableLoader::LoadFiresElevationAngleTable(UtInput& aInput, FiresTable*& aTablePtr)
{
   std::unique_ptr<FiresTable> tablePtr(new FiresTable());
   std::vector<double>         rangeValues;
   std::vector<double>         elevationValues;
   std::vector<double>         TOF_Values;
   std::string                 systemMunition;

   std::string command;
   aInput.ReadCommand(systemMunition);
   UtInputBlock block(aInput, "end_fires_elevation_angle_table");
   while (block.ReadCommand(command))
   {
      if (command == "range_values_in")
      {
      }
      else if (command == "elevation_angle_values_in")
      {
      }
      else
      {
         block.GetInput().PushBack(command);
         double range, elevationAngle, timeOfFlight;
         block.GetInput().ReadValue(range);
         rangeValues.push_back(range);
         block.GetInput().ReadValue(elevationAngle);
         // todo use UtInput
         elevationAngle *= UtMath::cRAD_PER_DEG;
         elevationValues.push_back(elevationAngle);
         block.GetInput().ReadValue(timeOfFlight);
         TOF_Values.push_back(timeOfFlight);
      }
   }
   tablePtr->mRangeValues.SetValues(rangeValues);
   tablePtr->mElevationValues.SetValues(elevationValues);
   tablePtr->mTimeOfFlight.SetValues(TOF_Values);

   tablePtr->SetName(systemMunition);
   tablePtr->mUseMaxOrd = false;
   aTablePtr            = tablePtr.get();
   tablePtr.release();
}

void FiresTableLoader::LoadFiresMaximumOrdinateTable(UtInput& aInput, FiresTable*& aTablePtr)
{
   std::unique_ptr<FiresTable> tablePtr(new FiresTable());
   std::vector<double>         rangeValues;
   std::vector<double>         maxOrdValues;
   std::vector<double>         TOF_Values;
   std::string                 systemMunition;

   std::string command;
   aInput.ReadCommand(systemMunition);
   UtInputBlock block(aInput, "end_fires_maximum_ordinate_table");
   while (block.ReadCommand(command))
   {
      if (command == "maximum_ordinate_values_in")
      {
      }
      else if (command == "elevation_angle_values_in")
      {
      }
      else
      {
         block.GetInput().PushBack(command);
         double range, maxOrd, timeOfFlight;
         block.GetInput().ReadValue(range);
         rangeValues.push_back(range);
         block.GetInput().ReadValue(maxOrd);
         maxOrdValues.push_back(maxOrd);
         block.GetInput().ReadValue(timeOfFlight);
         TOF_Values.push_back(timeOfFlight);
      }
   }
   tablePtr->mRangeValues.SetValues(rangeValues);
   tablePtr->mMaxOrdValues.SetValues(maxOrdValues);
   tablePtr->mTimeOfFlight.SetValues(TOF_Values);

   tablePtr->SetName(systemMunition);
   aTablePtr = tablePtr.get();
   tablePtr.release();
}

// uncomment the following to perform unit tests with the table.
/*
#include "UtUnitTest.hpp"
void UnitTestTable(FiresTable*& aTablePtr)
{
   StartTests();
   PdLookup lookup;
   aTablePtr->GetContext(lookup);
   double pd = lookup.Lookup(1000);
   Test(pd, 0.9, 1.0e-6);
   pd = lookup.Lookup(1500);
   Test(pd, 0.85, 0.001);
   pd = lookup.Lookup(12000);
   Test(pd, 0.0, 0.001);
   EndTests();
}
*/

} // namespace Fires
