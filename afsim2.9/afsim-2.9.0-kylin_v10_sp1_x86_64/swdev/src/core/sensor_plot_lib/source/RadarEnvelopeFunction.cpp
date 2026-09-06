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

#include "RadarEnvelopeFunction.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "UtAzElTable.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSignatureList.hpp"
#include "WsfStandardRadarSignature.hpp"
#include "WsfVersion.hpp"

RadarEnvelopeFunction::RadarEnvelopeFunction(const WsfScenario& aScenario)
   : Function(aScenario)
   , mSensor()
   , mTarget(aScenario.GetAtmosphere())
   , mAltValues()
   , mRCS_Values()
   , mAltUnitsStr("m")
   , mRangeUnitsStr("m")
   , mOutputName("")
   , mMaximumRange(0.0)
   , mLargeRangeStep(5000.0)
   , mSmallRangeStep(100.0)
{
}

// private
double RadarEnvelopeFunction::ComputeDetectionRange(double aTargetAlt)
{
   WsfSensorResult result;

   bool   done          = false;
   double range         = mMaximumRange;
   double rangeStep     = mLargeRangeStep;
   double detectedRange = 0.0;
   while (!done)
   {
      // Set the location, speed and attitude of the target.
      mTarget.SetLocationRBA(mSensor, range, 0.0, aTargetAlt);
      mTarget.SetSpeedAndAttitude(mSensor.GetSensor());

      // Attempt to cue the sensor to the target, just in case the sensor is a tracker.
      mSensor.CueToTarget(mTarget.GetPlatform());

      // Perform the detection attempt.
      bool detected = mSensor.AttemptToDetect(mTarget.GetPlatform(), result);
      if (!detected)
      {
         range -= rangeStep;
         if (range <= 0)
         {
            if (rangeStep == mSmallRangeStep)
            {
               done = true;
            }
            else
            {
               range     = 5 * mLargeRangeStep;
               rangeStep = mSmallRangeStep;
            }
         }
      }
      else
      {
         // We have a detection.  If doing the coarse step then switch to fine step.
         // If doing the fine step then we're done.

         detected      = true;
         detectedRange = range;

         if (rangeStep == mLargeRangeStep)
         {
            // We're on the big step, so back up a little and switch to the small step.
            range     = range + (5 * mLargeRangeStep);
            rangeStep = mSmallRangeStep;
         }
         else
         {
            // We're on the small step so we are done.
            done = true;
         }
      }
   }
   return detectedRange;
}

// virtual
bool RadarEnvelopeFunction::Execute(WsfSimulation& aSimulation)
{
   Function::Execute(aSimulation);
   if (mOutputName.empty())
   {
      ut::log::error() << "output_name not specified.";
      return false;
   }
   if (mAltValues.empty())
   {
      ut::log::error() << "No altitude values specified.";
      return false;
   }
   if (mRCS_Values.empty())
   {
      ut::log::error() << "No RCS values specified.";
      return false;
   }

   if ((!mSensor.CreateAndInitialize(aSimulation)) || (!mTarget.CreateAndInitialize(aSimulation)))
   {
      return false;
   }

   // Create a dummy signature table with a pointer to an az/el table that we will modify as required.
   // NOTE: This signature is owned by the platform and will be deleted when it is deleted.

   WsfStandardRadarSignature* sigPtr   = new WsfStandardRadarSignature();
   UtAzElTable*               tablePtr = new UtAzElTable();
   tablePtr->CreateConstantTable(1.0);
   sigPtr->AddTable(tablePtr, WsfStringId("default"), WsfEM_Types::cPOL_DEFAULT, 1.0E+30);
   if (!WsfRadarSignature::SetSignature(mTarget.GetPlatform(), sigPtr))
   {
      ut::log::error() << "Unable to define variable signature table.";
      delete sigPtr;
   }

   // Open the output file and write the header.

   std::ofstream ofs(mOutputName);
   if (!ofs)
   {
      auto out = ut::log::error() << "Unable to open output file.";
      out.AddNote() << "File: " << mOutputName;
      return false;
   }
   ofs << "# sensor_plot " << wsf::version::GetString() << std::endl;
   ofs << "# Radar Type: " << mSensor.GetSensor()->GetType() << std::endl;
   // clang-format off
   ofs << "# Column 1: RCS (dBsm)"
       << "; Column 2: altitude (" << mAltUnitsStr << ")"
       << "; Column 3: range (" << mRangeUnitsStr << ")" << std::endl;
   // clang-format on
   double altUnitScale   = UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH);
   double rangeUnitScale = UtInput::ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH);

   // Set the range limit to the user specified value, the sensor value or 2000 km.
   if (mMaximumRange == 0.0)
   {
      mMaximumRange = 2.0E+6;
      if (mSensor.GetMaximumRange() < 1.0E+9)
      {
         mMaximumRange = 1.5 * mSensor.GetMaximumRange();
      }
   }

   for (size_t altIndex = 0; altIndex < mAltValues.size(); ++altIndex)
   {
      double targetAlt = mAltValues[altIndex];
      for (size_t rcsIndex = 0; rcsIndex < mRCS_Values.size(); ++rcsIndex)
      {
         double targetRCS = mRCS_Values[rcsIndex];
         tablePtr->CreateConstantTable(static_cast<float>(targetRCS));
         double detectionRange = ComputeDetectionRange(targetAlt);
         double targetRCS_DB   = UtMath::LinearToDB(targetRCS);
         if (fabs(targetRCS_DB) < 1.0E-10)
         {
            targetRCS_DB = 0.0;
         }
         // clang-format off
         ofs << std::setw(5) << targetRCS_DB
             << ' ' << std::setw(10) << targetAlt * altUnitScale
             << ' ' << std::setw(10) << detectionRange * rangeUnitScale << std::endl;
         // clang-format on
      }
   }

   return true;
}

// virtual
bool RadarEnvelopeFunction::ProcessInput(UtInput& aInput)
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
      std::string word;
      aInput.ReadValue(word);
      if (word == "from")
      {
         double alt1;
         double alt2;
         double altStep;
         aInput.ReadValueOfType(alt1, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt1, 0.0);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "to");
         aInput.ReadValueOfType(alt2, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt2, alt1);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "by");
         aInput.ReadValueOfType(altStep, UtInput::cLENGTH);
         aInput.ValueGreater(altStep, 0.0);

         for (double alt = alt1; alt <= (alt2 + (0.1 * altStep)); alt += altStep)
         {
            mAltValues.push_back(alt);
         }
      }
      else
      {
         double alt;
         aInput.PushBack(word);
         aInput.ReadValueOfType(alt, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(alt, 0.0);
         mAltValues.push_back(alt);
      }
   }
   else if (command == "rcs")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "from")
      {
         // The RCS loop can go from small-to-big or big-to-small, based on user specification.
         double rcs1;
         double rcs2;
         double rcsStep;
         aInput.ReadValueOfType(rcs1, UtInput::cAREA_DB);
         aInput.ValueGreater(rcs1, 0.0);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "to");
         aInput.ReadValueOfType(rcs2, UtInput::cAREA_DB);
         aInput.ValueGreater(rcs2, 0.0);
         aInput.ReadValue(word);
         aInput.StringEqual(word, "by");
         aInput.ReadValueOfType(rcsStep, UtInput::cAREA_DB);
         aInput.ValueGreater(rcsStep, 0.0);

         // Input values are assumed to be in dB so they can be incremented in a linear fashion

         rcs1           = UtMath::LinearToDB(rcs1);
         rcs2           = UtMath::LinearToDB(rcs2);
         rcsStep        = UtMath::LinearToDB(rcsStep);
         double rcsFuzz = 0.01 * rcsStep;

         if (rcs2 >= rcs1)
         {
            for (double rcs = rcs1; rcs <= (rcs2 + rcsFuzz); rcs += rcsStep)
            {
               mRCS_Values.push_back(UtMath::DB_ToLinear(rcs));
            }
         }
         else
         {
            for (double rcs = rcs1; rcs >= (rcs2 - rcsFuzz); rcs -= rcsStep)
            {
               mRCS_Values.push_back(UtMath::DB_ToLinear(rcs));
            }
         }
      }
      else
      {
         double rcs;
         aInput.PushBack(word);
         aInput.ReadValueOfType(rcs, UtInput::cAREA_DB);
         aInput.ValueGreater(rcs, 0.0);
         mRCS_Values.push_back(rcs);
      }
   }
   else if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreater(mMaximumRange, 0.0);
   }
   else if (command == "large_range_step")
   {
      aInput.ReadValueOfType(mLargeRangeStep, UtInput::cLENGTH);
      aInput.ValueGreater(mLargeRangeStep, 0.0);
   }
   else if (command == "small_range_step")
   {
      aInput.ReadValueOfType(mSmallRangeStep, UtInput::cLENGTH);
      aInput.ValueGreater(mSmallRangeStep, 0.0);
   }
   else if (command == "altitude_units")
   {
      aInput.ReadValue(mAltUnitsStr);
      UtInput::ConvertValueTo(1.0, mAltUnitsStr, UtInput::cLENGTH); // Validate the specification
   }
   else if (command == "range_units")
   {
      aInput.ReadValue(mRangeUnitsStr);
      UtInput::ConvertValueTo(1.0, mRangeUnitsStr, UtInput::cLENGTH); // Validate the specification
   }
   else if ((command == "output_file") || (command == "output"))
   {
      aInput.ReadValueQuoted(mOutputName);
      mOutputName = aInput.SubstitutePathVariables(mOutputName);
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
