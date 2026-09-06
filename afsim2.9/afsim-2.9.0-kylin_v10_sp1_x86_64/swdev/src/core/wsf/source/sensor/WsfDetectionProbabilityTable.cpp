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

#include "WsfDetectionProbabilityTable.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"

namespace wsf
{

DetectionProbabilityTable::DetectionProbabilityTable()
   : wsf::SensorDetector()
   , mSignalTable()
   , mPdTable()
{
}

DetectionProbabilityTable* DetectionProbabilityTable::Clone() const
{
   return new DetectionProbabilityTable(*this);
}

bool DetectionProbabilityTable::Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex /* = 0*/)
{
   bool ok = SensorDetector::Initialize(aSimTime, aModePtr, aBeamIndex);

   // Compute the nominal detection threshold.

   double requiredPd = aModePtr->GetRequiredPd(); // Get the required Pd as entered.
   if ((requiredPd <= 0.0) || (requiredPd > 1.0))
   {
      requiredPd = 0.5;
   }
   requiredPd = std::min(std::max(requiredPd, 0.002), 0.998);

   // Find the signal to noise the generates a Pd equal to the required Pd.
   double loSignalToNoise    = 0.0;
   double hiSignalToNoise    = 1000.0;
   double detectionThreshold = 0.0;
   while (fabs(hiSignalToNoise - loSignalToNoise) > 0.001)
   {
      detectionThreshold = 0.5 * (loSignalToNoise + hiSignalToNoise);
      double pd          = ComputeProbabilityOfDetection(detectionThreshold);
      if (fabs(pd - requiredPd) < 0.001)
      {
         break;
      }
      else if (pd < requiredPd)
      {
         loSignalToNoise = detectionThreshold;
      }
      else
      {
         hiSignalToNoise = detectionThreshold;
      }
   }
   auto beamEntry = aModePtr->GetBeamEntry(aBeamIndex);
   if (beamEntry)
   {
      auto emReceiver = beamEntry->GetEM_Rcvr();
      if (emReceiver)
      {
         emReceiver->SetDetectionThreshold(detectionThreshold);
      }
      beamEntry->SetIntegrationGain(1);
   }

   return ok;
}

bool DetectionProbabilityTable::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "detection_probability")
   {
      ReadTable(aInput);
   }
   else
   {
      myCommand = SensorDetector::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Read a detection probability table.
//! @param aInput The current input stream. The current command should be the command that is
//! starting the input block.
// virtual
void DetectionProbabilityTable::ReadTable(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);

   std::string         command;
   std::vector<double> signalValues;
   std::vector<double> pdValues;
   while (inputBlock.ReadCommand(command))
   {
      // signal_to_noise <signal_to_noise> probability <probability>
      if (command == "signal_to_noise")
      {
         double signal;
         aInput.ReadValueOfType(signal, UtInput::cRATIO);
         aInput.ValueGreater(signal, 0.0);
         signal = UtMath::LinearToDB(signal);
         if ((!signalValues.empty()) && (signal <= signalValues.back()))
         {
            throw UtInput::BadValue(aInput, "signal-to-noise values must be monotonically ascending");
         }

         std::string pdString;
         aInput.ReadValue(pdString);
         UtStringUtil::ToLower(pdString);
         if ((pdString != "pd") && (pdString != "probability"))
         {
            throw UtInput::BadValue(aInput);
         }

         double pd;
         aInput.ReadValue(pd);
         aInput.ValueInClosedRange(pd, 0.0, 1.0);

         signalValues.push_back(signal);
         pdValues.push_back(pd);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (signalValues.size() < 2)
   {
      throw UtInput::BadValue(aInput, "detection_probability table must have at least two entries");
   }

   mSignalTable.SetValues(signalValues);
   mPdTable.SetValues(pdValues);
}

//! Compute the probability of detection using detection probability table.
//! @param aSignalToNoise      [input] Absolute signal-to-noise ratio
//! @param aDetectionThreshold [input] Absolute detection threshold (optional)
//! @return The probability of detection [0..1]
double DetectionProbabilityTable::ComputeProbabilityOfDetection(double aSignalToNoise,
                                                                double aDetectionThreshold /* = UtMath::DB_ToLinear(3.0)*/)
{
   TblLookupLU<double> luSignal;
   luSignal.Lookup(mSignalTable, UtMath::SafeLinearToDB(aSignalToNoise));
   return TblEvaluate(mPdTable, luSignal);
}

// ================================================================================================
//! The object factory for the Marcum-Swerling Detector type.
// static
wsf::SensorDetector* DetectionProbabilityTable::ObjectFactory(const std::string& aTypeName)
{
   wsf::SensorDetector* instancePtr = nullptr;
   if (aTypeName == "probability_table")
   {
      instancePtr = new DetectionProbabilityTable();
   }
   return instancePtr;
}

} // namespace wsf
