// ****************************************************************************
// CUI
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

#include "UtAzElAzElTable.hpp"

#include <fstream>
#include <iostream>

#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"

// =================================================================================================
UtAzElAzElTable::~UtAzElAzElTable()
{
   // Delete the az/el tables at each point in the slices...
   for (Slice& slice : mSlices)
   {
      for (auto& tablePtr : slice.mTablePtrs)
      {
         delete tablePtr;
      }
   }
}

// =================================================================================================
float UtAzElAzElTable::Lookup(float aTgtToXmtrAz, float aTgtToXmtrEl, float aTgtToRcvrAz, float aTgtToRcvrEl) const
{
   // Assume slices of constant elevation....
   float conValue(aTgtToXmtrEl); // Used to select the slice.
   float varValue(aTgtToXmtrAz); // Used to select the 'point' on the slice.

   if (mConstantAzSlices) // Slices are of constant azimuth...
   {
      conValue = aTgtToXmtrAz;
      varValue = aTgtToXmtrEl;
   }

   // Locate the one or two slices involved in the interpolation.
   size_t conIndex = 0;
   float  conRatio = 0.0F;
   mConValues.Lookup(conValue, conIndex, conRatio);

   // Interpolate on the lower slice
   float f = mSlices[conIndex].Lookup(varValue, aTgtToRcvrAz, aTgtToRcvrEl);

   // Interpolate on the upper slice if the lower slice is not the only or last slice.
   if ((conIndex + 1) < mSlices.size())
   {
      float f0 = f;
      float f1 = mSlices[conIndex + 1].Lookup(varValue, aTgtToRcvrAz, aTgtToRcvrEl);
      f        = f0 + (f1 - f0) * conRatio;
   }
   return f;
}

// =================================================================================================
// static
bool UtAzElAzElTable::ProcessInput(UtInput& aInput, UtAzElAzElTable*& aTablePtr, std::string& aTableUnits)
{
   aTablePtr             = nullptr;
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "inline_table")
   {
      std::string terminator("end_" + command);
      LoadFromInput(aInput, aTablePtr, aTableUnits);
      std::string command2;
      aInput.ReadCommand(command2);
      if (command2 != terminator)
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
#ifdef TODO_LATER
   else if (command == "file")
   {
      LoadFromFile(aInput, aTablePtr, aTableUnits);
   }
#endif
   else if ((command == "azimuth") || (command == "elevation"))
   {
      // Start of implicit inline_table.
      aInput.PushBack(command);
      LoadFromInput(aInput, aTablePtr, aTableUnits);
   }

   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// private static
void UtAzElAzElTable::LoadFromFile(UtInput& /*aInput*/, UtAzElAzElTable*& /*aTablePtr*/, std::string& /*aTableUnits*/)
{
#ifdef TODO_LATER
   aTablePtr = 0;

   std::string fileName;
   aInput.ReadValueQuoted(fileName);
   fileName = aInput.LocateFile(fileName);
   aInput.DoFileOpenCallback(fileName); // Used for file logging

   std::ifstream ifs(fileName.c_str());
   if (!ifs)
   {
      throw UtInput::BadValue(aInput, "Unable to open file " + fileName);
   }

   // Skip the first three lines...
   std::string header;
   if ((!getline(ifs, header)) || (!getline(ifs, header)) || (!getline(ifs, header)))
   {
      throw UtInput::BadValue(aInput, "Error skipping first three lines in file " + fileName);
   }

   InputTable inputTable;

   float azValue;
   float elValue;
   while (ifs >> azValue >> elValue)
   {
      aInput.ValueInClosedRange(azValue, -180.0, 180.0);
      aInput.ValueInClosedRange(elValue, -90.0, 90.0);
      azValue *= UtMath::cRAD_PER_DEG;
      elValue *= UtMath::cRAD_PER_DEG;
      UtAzElTable* azElTablePtr(0);
      std::string  tableUnits(aTableUnits);
      if (!UtAzElTableLoader::ProcessTable(aInput, azElTablePtr, tableUnits))
      {
         throw UtInput::BadValue(aInput);
      }
      inputTable.mPoints.push_back(InputPoint(azValue, elValue, azElTablePtr));
   }
   MakeTable(aInput, inputTable, aTablePtr);
#endif
}

// =================================================================================================
// NOTE: This routine pushes the first unrecognized command back on the input stack. The caller is
//       responsible for ensuring the command is valid.
// private static
void UtAzElAzElTable::LoadFromInput(UtInput& aInput, UtAzElAzElTable*& aTablePtr, std::string& aTableUnits)
{
   static const double cUNDEFINED = -999.0;

   // The first azimuth or elevation command determines if the table is azimuth-first or elevation-first.
   // It must remain consistent after that.

   enum
   {
      cNOT_ESTABLISHED,
      cAZIMUTH_IS_FIRST,
      cELEVATION_IS_FIRST
   } angleOrder = cNOT_ESTABLISHED;

   InputTable inputTable;

   double      azimuth   = cUNDEFINED;
   double      elevation = cUNDEFINED;
   std::string command;
   while (aInput.ReadCommand(command))
   {
      if (command == "azimuth")
      {
         if (azimuth != cUNDEFINED)
         {
            throw UtInput::OutOfContext(aInput); // Can't have two 'azimuth' commands in a row
         }

         if (angleOrder == cNOT_ESTABLISHED)
         {
            angleOrder = cAZIMUTH_IS_FIRST;
         }
         else if ((angleOrder == cELEVATION_IS_FIRST) && (elevation == cUNDEFINED))
         {
            throw UtInput::OutOfContext(aInput); // Can't have 'azimuth' before 'elevation' if elevation-first table
         }
         aInput.ReadValueOfType(azimuth, UtInput::cANGLE);
         aInput.ValueInClosedRange(azimuth, -UtMath::cPI, UtMath::cPI);
      }
      else if (command == "elevation")
      {
         if (elevation != cUNDEFINED)
         {
            throw UtInput::OutOfContext(aInput); // Can't have two 'elevation' commands in a row.
         }

         if (angleOrder == cNOT_ESTABLISHED)
         {
            angleOrder = cELEVATION_IS_FIRST;
         }
         else if ((angleOrder == cAZIMUTH_IS_FIRST) && (azimuth == cUNDEFINED))
         {
            throw UtInput::OutOfContext(aInput); // Can't have 'elevation' before 'azimuth' if azimuth-first table
         }
         aInput.ReadValueOfType(elevation, UtInput::cANGLE);
         aInput.ValueInClosedRange(elevation, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if ((azimuth != cUNDEFINED) && (elevation != cUNDEFINED))
      {
         // A table should be present once an azimuth and elevation have been defined.
         UtAzElTable* azElTablePtr(nullptr);
         std::string  tableUnits(aTableUnits);
         if (!UtAzElTableLoader::ProcessTable(aInput, azElTablePtr, tableUnits))
         {
            throw UtInput::BadValue(aInput);
         }
         inputTable.mPoints.emplace_back(static_cast<float>(azimuth), static_cast<float>(elevation), azElTablePtr);
         azimuth   = cUNDEFINED;
         elevation = cUNDEFINED;
      }
      else
      {
         aInput.PushBack(command);
         break;
      }
   }

   // The last command shouldn't be 'azimuth' or 'elevation' - those commands should be followed by an UtAzElTable
   if ((azimuth != cUNDEFINED) || (elevation != cUNDEFINED))
   {
      throw UtInput::OutOfContext(aInput);
   }

   // All of the input data has been created. Convert the input to run-time format.
   MakeTable(aInput, inputTable, aTablePtr);
}

// =================================================================================================
//! Converts the raw input data into the run-time table.
// private static
void UtAzElAzElTable::MakeTable(UtInput& aInput, InputTable& aInputTable, UtAzElAzElTable*& aTablePtr)
{
   std::vector<size_t> sliceStart;  // The index where each slice starts
   std::vector<float>  sliceValues; // The constant coordinate value of the slice...

   std::vector<InputPoint>& points(aInputTable.mPoints);

   // Look at the input angles to decide if the user specified constant azimuth slices or constant elevation slices.
   // Note that if all the slices are degenerate (only of a single point) then the table will be considered as having
   // constant elevation slices. Elevation slices are preferred because they offer the ability to use less data.
   // (Slices at higher elevations would require fewer points to get the same resolution. In particular, one would
   // require only one point at el=+/-90 deg.)

   size_t numPoints = points.size();
   if (numPoints == 0)
   {
      throw UtInput::BadValue(aInput, "No table data provided");
   }

   bool   constantElSlices = true;
   size_t begCut           = 0; // Starting index of the cut
   for (size_t i = 1; i < numPoints; ++i)
   {
      if (points[i].mEl != points[begCut].mEl)
      {
         // Change in elevation. To be valid it must be greater than elevation that started the previous slice.
         if (points[i].mEl <= points[begCut].mEl)
         {
            constantElSlices = false;
            break;
         }
      }
      else
      {
         // Point has the same elevation as the previous point, so should be part of the same slice.
         // Azimuths should be monotonically ascending.
         if (points[i].mAz <= points[i - 1].mAz)
         {
            constantElSlices = false;
            break;
         }
      }
   }

   bool constantAzSlices = (!constantElSlices);

   // If the table cannot be interpreted as having constant elevation slices, then it MUST fulfill the requirement
   // of having constant azimuth slices.
   if (!constantElSlices)
   {
      begCut = 0;
      for (size_t i = 1; i < numPoints; ++i)
      {
         if (points[i].mAz != points[begCut].mAz)
         {
            // Change in azimuth. To be valid it must be greater than the azimuth that started previous slice.
            if (points[i].mAz <= points[begCut].mAz)
            {
               constantAzSlices = false;
               break;
            }
         }
         else
         {
            // Point has the same azimuth as the previous point, so it should be part of the same slice.
            // Elevations should be monotonically ascending within the slice.
            if (points[i].mEl <= points[i - 1].mEl)
            {
               constantAzSlices = false;
               break;
            }
         }
      }
      if (!constantAzSlices)
      {
         throw UtInput::BadValue(aInput, "Cannot determine azimuth/elevation order of table.");
      }
   }

   if (constantAzSlices)
   {
      float lastAz = -999;
      float lastEl = -999;
      for (size_t i = 0; i < points.size(); ++i)
      {
         if (points[i].mAz != lastAz)
         {
            // Starting a new slice.
            if (points[i].mAz < lastAz)
            {
               throw UtException("Azimuth slices must be monotonically ascending");
            }
            lastAz = points[i].mAz;
            lastEl = -999;
            sliceStart.push_back(i);
            sliceValues.push_back(lastAz);
         }
         // Make sure the elevation values are monotonically increasing in the slice.
         if (points[i].mEl <= lastEl)
         {
            throw UtException("Points in azimuth slice are not monotonically increasing");
         }
         lastEl = points[i].mEl;
      }
   }
   else
   {
      float lastAz = -999;
      float lastEl = -999;
      for (size_t i = 0; i < points.size(); ++i)
      {
         if (points[i].mEl != lastEl)
         {
            // Starting a new slice.
            if (points[i].mEl < lastEl)
            {
               throw UtException("Elevation slices must be monotonically ascending");
            }
            lastEl = points[i].mEl;
            lastAz = -999;
            sliceStart.push_back(i);
            sliceValues.push_back(lastEl);
         }
         // Make sure the elevation values are monotonically increasing in the slice.
         if (points[i].mAz <= lastAz)
         {
            throw UtException("Points in elevation slice are not monotonically increasing");
         }
         lastAz = points[i].mAz;
      }
   }

   // We now know where each slice starts and we know that all of the input is valid.
   // At this point we can now create the run-time table and not have to worry about memory
   // getting leaked because an exception gets thrown...

   aTablePtr                    = new UtAzElAzElTable();
   aTablePtr->mConstantAzSlices = constantAzSlices;

   // Create each of the slices in the run-time table.

   size_t sliceCount = sliceStart.size();
   aTablePtr->mSlices.resize(sliceCount);
   for (size_t sliceIndex = 0; sliceIndex < sliceCount; ++sliceIndex)
   {
      size_t begIndex = sliceStart[sliceIndex];
      size_t endIndex = points.size();
      if ((sliceIndex + 1) < sliceCount)
      {
         endIndex = sliceStart[sliceIndex + 1];
      }
      size_t pointCount = endIndex - begIndex;
      Slice& slice      = aTablePtr->mSlices[sliceIndex];
      slice.mTablePtrs.resize(pointCount);
      std::vector<float> varValues(pointCount);
      for (size_t i = 0; i < pointCount; ++i)
      {
         slice.mTablePtrs[i] = points[begIndex + i].mTablePtr;
         if (constantAzSlices)
         {
            varValues[i] = points[begIndex + i].mEl;
         }
         else
         {
            varValues[i] = points[begIndex + i].mAz;
         }
      }

      // Save the locations of the 'points' on the slice
      slice.mVarValues.SetValues(varValues);
   }

   // Save the locations of the slices...
   aTablePtr->mConValues.SetValues(sliceValues);

   // The run-time table has been successfully completed. The vector of input points is now erased. This
   // signifies to the destructor that ownership of the points has been transferred to the run-time table.
   aInputTable.mPoints.clear();
}

// =================================================================================================
// Nested class IndVar.
// =================================================================================================

// =================================================================================================
void UtAzElAzElTable::IndVar::SetValues(const std::vector<float>& aValues)
{
   size_t valueCount = aValues.size();
   if (valueCount <= 1)
   {
      mLookupMethod = cLM_NONE;
   }
   else
   {
      // Determine if the values are equally spaced (more efficient lookup)
      bool equallySpaced = true;
      if (valueCount > 2)
      {
         float  firstDelta     = aValues[1] - aValues[0];
         float  deltaTolerance = 1.0E-4F * firstDelta;
         size_t i              = 2;
         while ((i < valueCount) && equallySpaced)
         {
            float delta   = aValues[i] - aValues[i - 1];
            equallySpaced = (fabs(delta - firstDelta) < deltaTolerance);
            ++i;
         }
      }
      if (equallySpaced)
      {
         mValuesE.SetValues(aValues[0], aValues.back(), valueCount);
         mLookupMethod = cLM_LE;
      }
      else
      {
         mValuesU.SetValues(aValues);
         mLookupMethod = (valueCount < 64) ? cLM_LU : cLM_LUB;
      }
   }
}

// =================================================================================================
void UtAzElAzElTable::IndVar::Lookup(float aValue, size_t& aIndex, float& aRatio) const
{
   switch (mLookupMethod)
   {
   case cLM_NONE:
      aIndex = 0;
      aRatio = 0.0F;
      break;
   case cLM_E:
   {
      TblLookupE<float> lookup;
      lookup.Lookup(mValuesE, aValue);
      aIndex = (int)lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   break;
   case cLM_U:
   {
      TblLookupU<float> lookup;
      lookup.Lookup(mValuesU, aValue);
      aIndex = lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   break;
   case cLM_UB:
   {
      TblLookupUB<float> lookup;
      lookup.Lookup(mValuesU, aValue);
      aIndex = lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   case cLM_LE:
   {
      TblLookupLE<float> lookup;
      lookup.Lookup(mValuesE, aValue);
      aIndex = (int)lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   break;
   case cLM_LU:
   {
      TblLookupLU<float> lookup;
      lookup.Lookup(mValuesU, aValue);
      aIndex = lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   break;
   case cLM_LUB:
   {
      TblLookupLUB<float> lookup;
      lookup.Lookup(mValuesU, aValue);
      aIndex = lookup.GetIndex();
      aRatio = lookup.GetRatio();
   }
   break;
   }
}

// =================================================================================================
// Nested class InputPoint.
// =================================================================================================

UtAzElAzElTable::InputPoint::InputPoint(float aAz, float aEl, UtAzElTable* aTablePtr)
   : mAz(aAz)
   , mEl(aEl)
   , mTablePtr(aTablePtr)
{
}

// =================================================================================================
// Nested class InputTable.
// This class exists to prevent memory leaks when an exception is thrown during input processing.
// =================================================================================================

// =================================================================================================
UtAzElAzElTable::InputTable::~InputTable()
{
   // 'mPoints' is cleared in MakeTable once the run-time table has been created.
   for (auto& point : mPoints)
   {
      delete point.mTablePtr;
   }
}

// =================================================================================================
// Nested class Slice.
// =================================================================================================

float UtAzElAzElTable::Slice::Lookup(float aVarValue, float aTgtToRcvrAz, float aTgtToRcvrEl) const
{
   // Locate the one or two points on the cut involved in the interpolation.
   size_t varIndex = 0;
   float  varRatio = 0.0F;
   mVarValues.Lookup(aVarValue, varIndex, varRatio);

   // Interpolate on the first 'point'.
   UtAzElLookup c0(mTablePtrs[varIndex]->GetContext());
   float        f = c0.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);

   // Interpolate on the second 'point' if it is not the last point on the cut.
   if ((varIndex + 1) < mTablePtrs.size())
   {
      float        f0 = f;
      UtAzElLookup c1(mTablePtrs[varIndex + 1]->GetContext());
      float        f1 = c1.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);
      f               = f0 + (f1 - f0) * varRatio;
   }
   return f;
}
