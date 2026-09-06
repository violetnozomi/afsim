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

#include "RadarLookupTableFunction.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfUtil.hpp"

RadarLookupTableFunction::RadarLookupTableFunction(const WsfScenario& aScenario)
   : Function(aScenario)
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mAltitudes()
   , mRanges()
   , mAltUnitsStr("m")
   , mAltUnitsScale(1.0)
   , mRangeUnitsStr("m")
   , mRangeUnitsScale(1.0)
   , mOutputName()
   , mHeaderLine1("")
   , mHeaderLine2("")
   , mHeaderLine3("")
   , mOutputColumnLimit(501)
{
}

// virtual
bool RadarLookupTableFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   if ((!mSensor.CreateAndInitialize(aSimulation)) || (!mTarget.CreateAndInitialize(aSimulation)))
   {
      return false;
   }

   // Generate the base name for output files.  If one hasn't been defined then we'll use the
   // sensor type name.
   // std::string baseName(mOutputName);
   // if (baseName.empty())
   //{
   //   baseName = mSensor.GetSensor()->GetType();
   //}

   WsfSensorResult result;
   mSensor.GetSettings().mRequiredPd = 1.0E-5;

   size_t              rangeCount = mRanges.size();
   size_t              altCount   = mAltitudes.size();
   size_t              valueCount = rangeCount * rangeCount;
   std::vector<double> sigValues(valueCount);
   std::vector<double> f40Values(valueCount);
   std::vector<double> cltValues(valueCount);
   for (size_t altIndex = 0; altIndex < mAltitudes.size(); ++altIndex)
   {
      double altitude = mAltitudes[altIndex];
      for (size_t rangeIndex = 0; rangeIndex < mRanges.size(); ++rangeIndex)
      {
         double groundRange = mRanges[rangeIndex];

         // Set the location, speed and attitude of the target.

         mTarget.SetLocationRBA(mSensor, groundRange, 0.0, altitude);
         mTarget.SetSpeedAndAttitude(mSensor);

         // Cue the sensor to the target just in case this is a tracker.
         mSensor.CueToTarget(mTarget);

         // Perform the detection attempt.
         /* bool detected = */ mSensor.AttemptToDetect(mTarget, result);

         // Extract the detection values. Note that like SALRAM, values are calculated even
         // though the target may be masked by the horizon.

         double sigValue = -370.0;
         double f40Value = 1.0;
         double cltValue = -370.0;
         if (result.mRcvdPower > 0.0)
         {
            sigValue = result.mRcvdPower;
            // The received power includes the signature and atmospheric attenuation.
            // In order to be compatible with SALRAM these need to be removed.
            if (result.mRadarSig > 0.0)
            {
               sigValue = sigValue / result.mRadarSig;
            }
            sigValue = UtMath::SafeLinearToDB(sigValue);
            sigValue += 30.0; // SALRAM outputs dBm
         }
         if (result.mPropagationFactor > 0.0)
         {
            f40Value = result.mPropagationFactor;
            f40Value = UtMath::SafeLinearToDB(f40Value);
         }
         if (result.mClutterPower > 0.0)
         {
            cltValue = result.mClutterPower;
            cltValue = UtMath::SafeLinearToDB(cltValue);
            cltValue += 30.0; // SALRAM outputs dBm
         }
         size_t valueIndex     = (rangeIndex * altCount) + altIndex;
         sigValues[valueIndex] = sigValue;
         f40Values[valueIndex] = f40Value;
         cltValues[valueIndex] = cltValue;
      } // end of range loop
   }    // end of altitude loop

   ut::log::info() << "Writing output.";

   // Convert metric range and altitude units to user requested units

   size_t i;
   double conversionFactor;

   conversionFactor = UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   for (i = 0; i < mAltitudes.size(); ++i)
   {
      mAltitudes[i] *= conversionFactor;
   }

   conversionFactor = UtInput::ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);
   for (i = 0; i < mRanges.size(); ++i)
   {
      mRanges[i] *= conversionFactor;
   }

   // Write out the data files.
   if (mHeaderLine2.empty())
   {
      mHeaderLine2 = "Radar Type: " + mSensor.GetSensor()->GetType();
   }
   mHeaderLine3 = "Signal Level in dBm";
   WriteFile(mOutputName + ".sig", mRanges, mAltitudes, sigValues);

   mHeaderLine3 = "F40 Factor Level in dB";
   WriteFile(mOutputName + ".f40", mRanges, mAltitudes, f40Values);

   mHeaderLine3 = "Clutter Level in dBm";
   WriteFile(mOutputName + ".clt", mRanges, mAltitudes, cltValues);

   return true;
}


bool RadarLookupTableFunction::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (mSensor.ProcessInput(aInput))
   {
   }
   else if (mTarget.ProcessInput(aInput))
   {
   }
   else if (command == "altitude")
   {
      double alt;
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(alt, 0.0);
      mAltitudes.push_back(alt);
   }
   else if (command == "altitudes")
   {
      std::string word;
      double      altMin;
      double      altMax;
      double      altStep;
      aInput.ReadValue(word);
      aInput.StringEqual(word, "from");
      aInput.ReadValueOfType(altMin, UtInput::cLENGTH);
      if (mAltitudes.empty())
      {
         aInput.ValueGreaterOrEqual(altMin, 0.0);
      }
      else
      {
         aInput.ValueGreater(altMin, mAltitudes.back());
      }
      aInput.ReadValue(word);
      aInput.StringEqual(word, "to");
      aInput.ReadValueOfType(altMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(altMax, altMin);
      aInput.ReadValue(word);
      aInput.StringEqual(word, "by");
      aInput.ReadValueOfType(altStep, UtInput::cLENGTH);
      aInput.ValueGreater(altStep, 0.0);

      for (double alt = altMin; alt <= (altMax + 1.0E-4); alt += altStep)
      {
         mAltitudes.push_back(alt);
      }
   }
   else if (command == "range")
   {
      double range;
      aInput.ReadValueOfType(range, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(range, 0.0);
   }
   else if (command == "ranges")
   {
      std::string word;
      double      rangeMin;
      double      rangeMax;
      double      rangeStep;
      aInput.ReadValue(word);
      aInput.StringEqual(word, "from");
      aInput.ReadValueOfType(rangeMin, UtInput::cLENGTH);
      if (mRanges.empty())
      {
         aInput.ValueGreaterOrEqual(rangeMin, 0.0);
      }
      else
      {
         aInput.ValueGreater(rangeMin, mRanges.back());
      }
      aInput.ReadValue(word);
      aInput.StringEqual(word, "to");
      aInput.ReadValueOfType(rangeMax, UtInput::cLENGTH);
      aInput.ValueGreater(rangeMax, rangeMin);
      aInput.ReadValue(word);
      aInput.StringEqual(word, "by");
      aInput.ReadValueOfType(rangeStep, UtInput::cLENGTH);
      aInput.ValueGreater(rangeStep, 0.0);

      for (double range = rangeMin; range <= (rangeMax + 1.0E-4); range += rangeStep)
      {
         mRanges.push_back(range);
      }
   }
   else if (command == "altitude_units")
   {
      aInput.ReadValue(mAltUnitsStr);
      mAltUnitsScale = aInput.ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   }
   else if (command == "range_units")
   {
      aInput.ReadValue(mRangeUnitsStr);
      mRangeUnitsScale = aInput.ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);
   }
   else if (command == "output_name")
   {
      aInput.ReadValueQuoted(mOutputName);
      mOutputName = aInput.SubstitutePathVariables(mOutputName);
   }
   else if (command == "header_line_1")
   {
      aInput.ReadLine(mHeaderLine1, false);
      if (!mHeaderLine1.empty())
      {
         mHeaderLine1 = mHeaderLine1.substr(1);
      }
   }
   else if (command == "header_line_2")
   {
      aInput.ReadLine(mHeaderLine2, false);
      if (!mHeaderLine2.empty())
      {
         mHeaderLine2 = mHeaderLine2.substr(1);
      }
   }
   else if (command == "header_line_3")
   {
      aInput.ReadLine(mHeaderLine3, false);
      if (!mHeaderLine3.empty())
      {
         mHeaderLine3 = mHeaderLine3.substr(1);
      }
   }
   else if (command == "output_column_limit")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreater(value, 0);
      mOutputColumnLimit = static_cast<size_t>(value);
   }
   else if (Function::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Write an output file.
void RadarLookupTableFunction::WriteFile(const std::string&         aFileName,
                                         const std::vector<double>& aRowValues,
                                         const std::vector<double>& aColValues,
                                         const std::vector<double>& aDataValues)
{
   std::ofstream ofs(aFileName);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << aFileName;
      return;
   }

   size_t rowCount = aRowValues.size();
   size_t colCount = aColValues.size();

   ofs.setf(std::ios::showpoint);
   ofs.setf(std::ios::fixed, std::ios::floatfield);

   ofs << mHeaderLine1 << '\n';
   ofs << mHeaderLine2 << '\n';
   ofs << mHeaderLine3 << '\n';
   ofs << "   " << rowCount << "  " << colCount << '\n';

   size_t rowIndex;
   size_t colIndex;
   size_t itemsOnLine;

   ofs << "          ";
   itemsOnLine = 0;
   for (colIndex = 0; colIndex < colCount; ++colIndex)
   {
      if (itemsOnLine >= mOutputColumnLimit)
      {
         itemsOnLine = 0;
         ofs << "\n          ";
      }
      ofs.precision(2);
      ofs << ' ' << std::setw(14) << aColValues[colIndex];
      ++itemsOnLine;
   }
   ofs << '\n';

   size_t rowColIndex = 0;
   for (rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      ofs.precision(2);
      ofs << ' ' << std::setw(9) << aRowValues[rowIndex];
      ofs.precision(8);
      itemsOnLine = 0;
      for (colIndex = 0; colIndex < colCount; ++colIndex)
      {
         if (itemsOnLine >= mOutputColumnLimit)
         {
            itemsOnLine = 0;
            ofs << "\n          ";
         }
         ofs << ' ' << std::setw(14) << aDataValues[rowColIndex];
         ++itemsOnLine;
         ++rowColIndex;
      }
      ofs << '\n';
   }
}
