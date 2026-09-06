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

#include "WsfMarcumSwerling.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"

namespace wsf
{

MarcumSwerling::MarcumSwerling()
   : wsf::SensorDetector()
   , mCase(0)
   , mNumberOfPulsesIntegrated(1)
   , mProbabilityOfFalseAlarm(1.0E-6)
   , mDetectorLaw(cDL_LINEAR)
   , mComputeConstants(true)
   , mBase(0.0)
   , mExp(0.0)
{
}

MarcumSwerling* MarcumSwerling::Clone() const
{
   return new MarcumSwerling(*this);
}

bool MarcumSwerling::Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex /* = 0*/)
{
   bool ok = SensorDetector::Initialize(aSimTime, aModePtr, aBeamIndex);

   // Compute the nominal detection threshold and the integration gain.

   double requiredPd = aModePtr->GetRequiredPd(); // Get the required Pd as entered.
   if ((requiredPd <= 0.0) || (requiredPd > 1.0))
   {
      requiredPd = 0.5;
   }
   requiredPd = std::min(std::max(requiredPd, 0.002), 0.998);

   // Make two passes through the following loop. The first pass uses the 'number_of_pulses_integrated'
   // as defined by the user. This produces the pre-integration signal-to-noise for the desired Pd. The
   // second pass uses the 'number_of_pulses_integrated' equal to one. This produces the post-integration
   // signal-to-noise for the desired Pd. From these two numbers we compute the effective integration gain.
   //
   // Note that the detector is left with the number_of_pulses_integrated set to one. The computed integration
   // gain is stored in the beam and used to adjust the non-integrated signal-to-noise to explicitly produce an
   // integrated signal-to-noise which is then fed to the detector (which does no integration).
   //
   // The reason for all of this is that the error model should use post-integration signal levels rather than
   // pre-integration signal levels. If this isn't done, the smaller pre-integration signal will be used by
   // the error model and will result in larger errors in the track, and very erratic tracks from the filters.

   double singlePulseThreshold     = 0.0;
   double multiPulseThreshold      = 0.0;
   auto   beamEntry                = aModePtr->GetBeamEntry(aBeamIndex);
   int    numberOfPulsesIntegrated = 0;
   if (beamEntry)
   {
      numberOfPulsesIntegrated = beamEntry->GetNumberOfPulsesIntegrated();
   }

   SetNumberOfPulsesIntegrated(numberOfPulsesIntegrated);
   for (int pass = 1; pass <= 2; ++pass)
   {
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
      // Assume this is the final pass - capture the single pulse threshold.
      singlePulseThreshold = detectionThreshold;
      if (pass == 1)
      {
         // On first pass capture the multiple-pulse threshold (which is the same as
         // the single-pulse threshold if multiple pulses are not being integrated).
         multiPulseThreshold = detectionThreshold;
         SetNumberOfPulsesIntegrated(1);
         if (numberOfPulsesIntegrated == 1)
         {
            break; // Bypass second loop if not integrating multiple pulses
         }
      }
   }

   if (beamEntry)
   {
      auto emReceiver = beamEntry->GetEM_Rcvr();
      if (emReceiver)
      {
         emReceiver->SetDetectionThreshold(singlePulseThreshold);
      }
      beamEntry->SetIntegrationGain(singlePulseThreshold / multiPulseThreshold);
   }

   return ok;
}

bool MarcumSwerling::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "detector_law")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "linear")
      {
         SetDetectorLaw(wsf::MarcumSwerling::cDL_LINEAR);
      }
      else if (word == "square")
      {
         SetDetectorLaw(wsf::MarcumSwerling::cDL_SQUARE);
      }
      else if (word == "log")
      {
         SetDetectorLaw(wsf::MarcumSwerling::cDL_LOG);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "probability_of_false_alarm")
   {
      double probabilityOfFalseAlarm;
      aInput.ReadValue(probabilityOfFalseAlarm);
      aInput.ValueInClosedRange(probabilityOfFalseAlarm, 0.0, 1.0);
      SetProbabilityOfFalseAlarm(probabilityOfFalseAlarm);
   }
   else if (command == "swerling_case")
   {
      int swerlingCase;
      aInput.ReadValue(swerlingCase);
      aInput.ValueInClosedRange(swerlingCase, 0, 4);
      SetCase(swerlingCase);
   }
   else if (command == "number_of_pulses_integrated")
   {
      aInput.ReadValue(mNumberOfPulsesIntegrated);
      aInput.ValueGreater(mNumberOfPulsesIntegrated, 0);
   }
   else
   {
      myCommand = SensorDetector::ProcessInput(aInput);
   }
   return myCommand;
}

//! Compute the probability of detection using Marcum-Swerling curves
//! @param aSignalToNoise      [input] Absolute signal-to-noise ratio
//! @param aDetectionThreshold [input] Absolute detection threshold (optional)
//! @return The probability of detection [0..1]
double MarcumSwerling::ComputeProbabilityOfDetection(double aSignalToNoise,
                                                     double aDetectionThreshold /* = UtMath::DB_ToLinear(3.0)*/)
{
   if (mComputeConstants)
   {
      mComputeConstants = false;
      ComputeConstants();
   }

   // Process trivial cases where the signal is very weak or very strong.

   if (aSignalToNoise < 1.0E-50)
   {
      return 0.0;
   }
   else if (aSignalToNoise > 1.0E50)
   {
      return 1.0;
   }

   double u  = pow(mBase / aSignalToNoise, mExp);
   double pd = 0.0;
   if (u <= 50.0)
   {
      pd = pow(10.0, -u);
   }
   return pd;
}

void MarcumSwerling::SetCase(int aCase)
{
   if ((aCase < 0) || (aCase > 4))
   {
      auto out = ut::log::error() << "MarcumSwerling: Invalid case.";
      out.AddNote() << "Actual: " << aCase;
      out.AddNote() << "Expected: 0, 1, 2, 3 or 4";
      return;
   }
   mCase             = aCase;
   mComputeConstants = true;
}

void MarcumSwerling::SetDetectorLaw(DetectorLaw aDetectorLaw)
{
   mDetectorLaw      = aDetectorLaw;
   mComputeConstants = true;
}

void MarcumSwerling::SetNumberOfPulsesIntegrated(int aNumberOfPulsesIntegrated)
{
   if (aNumberOfPulsesIntegrated <= 0)
   {
      auto out = ut::log::error() << "MarcumSwerling:: Invalid number of pulses integrated.";
      out.AddNote() << "Actual: " << aNumberOfPulsesIntegrated;
      out.AddNote() << "Expected: N > 0.";
      return;
   }
   mNumberOfPulsesIntegrated = aNumberOfPulsesIntegrated;
   mComputeConstants         = true;
}

void MarcumSwerling::SetProbabilityOfFalseAlarm(double aProbabilityOfFalseAlarm)
{
   if ((aProbabilityOfFalseAlarm <= 0.0) || (aProbabilityOfFalseAlarm >= 1.0))
   {
      auto out = ut::log::error() << "MarcumSwerling: Invalid probability of false alarm.";
      out.AddNote() << "Actual: " << aProbabilityOfFalseAlarm;
      out.AddNote() << "Expected: 0.0 < P < 1.0";
      return;
   }
   mProbabilityOfFalseAlarm = aProbabilityOfFalseAlarm;
   mComputeConstants        = true;
}

// private
void MarcumSwerling::ComputeConstants()
{
   // Set Swerling cases 2 and 4 to 1 and 3 if n > 1
   // Set Swerling cases 2 and 4 to 1 and 3 if n = 1

   double pfa = mProbabilityOfFalseAlarm;
   int    sc  = mCase;
   int    n   = mNumberOfPulsesIntegrated;
   if (n == 1)
   {
      if (mCase == 2)
      {
         sc = 1;
      }
      else if (mCase == 4)
      {
         sc = 3;
      }
   }

   // Set parameters for each Swerling case

   double alp = 0.0;
   double bet = 0.0;
   double g1  = 0.0;
   double g2  = 0.0;
   double g3  = 0.0;
   if (sc == 0)
   {
      alp = 1.8;
      bet = 0.20;
      g1  = 1.2;
      g3  = 1.0;
   }
   else if (sc == 1)
   {
      alp = (2.0 / 3.0) * (1.0 + (2.0 / 3.0) * exp(-n / 3.0));
      bet = 1.0;
      g1  = 1.0;
      g3  = 1.0;
   }
   else if (sc == 2)
   {
      alp = 1.5 - log10(pfa) / 60.0;
      bet = (1.0 / 9.0) + exp(-n / 5.0);
      g1  = 0.5;
      g3  = 2.95;
   }
   else if (sc == 3)
   {
      alp = (3.0 / 4.0) * (1.0 + (2.0 / 3.0) * exp(-n / 3.0));
      bet = 2.0 / 3.0;
      g1  = 0.93;
      g3  = 1.0;
   }
   else if (sc == 4)
   {
      alp = 1.3 - log10(pfa) / 70.0;
      bet = (1.0 / 6.0) + (2.0 / 3.0) * exp(-n / 4.0);
      g1  = 0.83;
      g3  = 1.55;
   }
   else
   {
      assert((sc < 0) || (sc > 4));
   }

   double ne = n;
   if (n > 1)
   {
      ne = g1 * n;
   }

   if (mDetectorLaw == cDL_LINEAR)
   {
      g2 = 0.915;
   }
   else if (mDetectorLaw == cDL_SQUARE)
   {
      g2 = 1.0;
   }
   else if (mDetectorLaw == cDL_LOG)
   {
      g2 = 0.608;
   }

   double nfa   = log(0.5) / log(1.0 - pfa);
   double num   = alp * log10(nfa);
   double gama  = (2.0 / 3.0) * g2 * g1;
   double dem   = g3 /* * aSignalToNoise */ * pow(ne, gama);
   double frac1 = num / dem;
   double frac2 = 1.0 / bet;
   mBase        = frac1;
   mExp         = frac2;
}


// ================================================================================================
//! The object factory for the Marcum-Swerling Detector type.
// static
wsf::SensorDetector* MarcumSwerling::ObjectFactory(const std::string& aTypeName)
{
   wsf::SensorDetector* instancePtr = nullptr;
   if ((aTypeName == "swerling") || (aTypeName == "marcum_swerling"))
   {
      instancePtr = new MarcumSwerling();
   }
   return instancePtr;
}

} // namespace wsf
