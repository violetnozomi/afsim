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

#include "WsfPassiveDetector.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfSensorResult.hpp"

// =================================================================================================
bool WsfPassiveDetector::Initialize()
{
   bool ok = true;

   // If neither threshold or sensitivity is defined, default to a 3db detection threshold.
   // This is for compatibility with old input files which may have relied on the 3db default in WsfEM_Rcvr.
   if (mDetectionThresholdTable.empty() && mDetectionSensitivityTable.empty())
   {
      SetDetectionThreshold(cST_CONTINUOUS, 0.0, UtMath::DB_ToLinear(3.0));
      SetDetectionThreshold(cST_PULSED, 0.0, UtMath::DB_ToLinear(3.0));
   }
   else if ((!mDetectionThresholdTable.empty()) && mDetectionSensitivityTable.empty())
   {
      SignalTypeToSensThreshTable::iterator dti = mDetectionThresholdTable.find(cST_CONTINUOUS);
      if (dti != mDetectionThresholdTable.end())
      {
         if (dti->second.empty())
         {
            ut::log::error() << "Continuous detection threshold not defined.";
            ok &= false;
         }
      }
      else
      {
         ut::log::error() << "Continuous detection threshold not defined.";
         ok &= false;
      }

      dti = mDetectionThresholdTable.find(cST_PULSED);
      if (dti != mDetectionThresholdTable.end())
      {
         if (dti->second.empty())
         {
            ut::log::error() << "Pulsed detection threshold not defined.";
            ok &= false;
         }
      }
      else
      {
         ut::log::error() << "Pulsed detection threshold not defined.";
         ok &= false;
      }
   }
   else if ((!mDetectionSensitivityTable.empty()) && mDetectionThresholdTable.empty())
   {
      SignalTypeToSensThreshTable::iterator dsi = mDetectionSensitivityTable.find(cST_CONTINUOUS);
      if (dsi != mDetectionSensitivityTable.end())
      {
         if (dsi->second.empty())
         {
            ut::log::error() << "Continuous sensitivity not defined.";
            ok &= false;
         }
      }
      else
      {
         ut::log::error() << "Continuous sensitivity not defined.";
         ok &= false;
      }

      dsi = mDetectionSensitivityTable.find(cST_PULSED);
      if (dsi != mDetectionSensitivityTable.end())
      {
         if (dsi->second.empty())
         {
            ut::log::error() << "Pulse sensitivity not defined.";
            ok &= false;
         }
      }
      else
      {
         ut::log::error() << "Pulsed sensitivity not defined.";
         ok &= false;
      }
   }

   // Ensure one last time that the detection or sensitivity threshold tables are defined (i.e. not empty).
   if (mDetectionThresholdTable.empty() && mDetectionSensitivityTable.empty())
   {
      ut::log::error() << "Must define 'detection_threshold' or 'detection_sensitivity'.";
      ok &= false;
   }
   return ok;
}

// =================================================================================================
bool WsfPassiveDetector::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "detection_threshold")
   {
      double threshold;
      aInput.ReadValueOfType(threshold, UtInput::cRATIO);
      aInput.ValueGreater(threshold, 0.0);
      SetDetectionThreshold(cST_CONTINUOUS, 0.0, threshold);
      SetDetectionThreshold(cST_PULSED, 0.0, threshold);
   }
   else if (command == "continuous_detection_threshold")
   {
      double contThreshold;
      aInput.ReadValueOfType(contThreshold, UtInput::cRATIO);
      aInput.ValueGreater(contThreshold, 0.0);
      SetDetectionThreshold(cST_CONTINUOUS, 0.0, contThreshold);
   }
   else if (command == "pulsed_detection_threshold")
   {
      double pulsedThreshold;
      aInput.ReadValueOfType(pulsedThreshold, UtInput::cRATIO);
      aInput.ValueGreater(pulsedThreshold, 0.0);
      SetDetectionThreshold(cST_PULSED, 0.0, pulsedThreshold);
   }
   else if (command == "detection_thresholds")
   {
      UtInputBlock inputBlock(aInput, "end_detection_thresholds");
      ClearDetectionTables();
      std::string signalType = "both";
      while (inputBlock.ReadCommand(command))
      {
         if (command == "frequency")
         {
            double frequency;
            inputBlock.GetInput().ReadValueOfType(frequency, UtInput::cFREQUENCY);
            inputBlock.GetInput().ValueGreater(frequency, 0.0);

            double threshold;
            inputBlock.GetInput().ReadValueOfType(threshold, UtInput::cRATIO);
            inputBlock.GetInput().ValueGreater(threshold, 0.0);

            if (signalType == "continuous")
            {
               if (!AddDetectionThreshold(cST_CONTINUOUS, frequency, threshold))
               {
                  throw UtInput::BadValue(aInput, "Detection threshold previously defined for frequency.");
               }
            }
            else if (signalType == "pulsed")
            {
               if (!AddDetectionThreshold(cST_PULSED, frequency, threshold))
               {
                  throw UtInput::BadValue(aInput, "Detection threshold previously defined for frequency.");
               }
            }
            else if (signalType == "both")
            {
               bool added = true;
               added &= AddDetectionThreshold(cST_CONTINUOUS, frequency, threshold);
               added &= AddDetectionThreshold(cST_PULSED, frequency, threshold);
               if (!added)
               {
                  throw UtInput::BadValue(aInput, "Detection threshold previously defined for frequency.");
               }
            }
            else
            {
               throw UtInput::BadValue(inputBlock.GetInput());
            }
         }
         else if (command == "signal_type")
         {
            inputBlock.GetInput().ReadValue(signalType);
            if (!((signalType == "continuous") || (signalType == "pulsed") || (signalType == "both")))
            {
               throw UtInput::BadValue(aInput, "Invalid signal_type: " + signalType);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
   }
   else if (command == "detection_sensitivity")
   {
      double sensitivity;
      aInput.ReadValueOfType(sensitivity, UtInput::cPOWER_DB);
      aInput.ValueGreater(sensitivity, 0.0);
      ClearDetectionTables();
      AddDetectionSensitivity(cST_CONTINUOUS, 0.0, sensitivity);
      AddDetectionSensitivity(cST_PULSED, 0.0, sensitivity);
   }
   else if (command == "continuous_detection_sensitivity")
   {
      double contSensitivity;
      aInput.ReadValueOfType(contSensitivity, UtInput::cPOWER_DB);
      aInput.ValueGreater(contSensitivity, 0.0);
      SetDetectionSensitivity(cST_CONTINUOUS, 0.0, contSensitivity);
   }
   else if (command == "pulsed_detection_sensitivity")
   {
      double pulsedSensitivity;
      aInput.ReadValueOfType(pulsedSensitivity, UtInput::cPOWER_DB);
      aInput.ValueGreater(pulsedSensitivity, 0.0);
      SetDetectionSensitivity(cST_PULSED, 0.0, pulsedSensitivity);
   }
   else if (command == "detection_sensitivities")
   {
      UtInputBlock inputBlock(aInput, "end_detection_sensitivities");
      ClearDetectionTables();
      std::string signalType = "both";
      while (inputBlock.ReadCommand(command))
      {
         if (command == "frequency")
         {
            double frequency;
            inputBlock.GetInput().ReadValueOfType(frequency, UtInput::cFREQUENCY);
            inputBlock.GetInput().ValueGreater(frequency, 0.0);

            double sensitivity;
            inputBlock.GetInput().ReadValueOfType(sensitivity, UtInput::cPOWER_DB);
            inputBlock.GetInput().ValueGreater(sensitivity, 0.0);

            if (signalType == "continuous")
            {
               if (!AddDetectionSensitivity(cST_CONTINUOUS, frequency, sensitivity))
               {
                  throw UtInput::BadValue(aInput, "Detection sensitivity previously defined for frequency.");
               }
            }
            else if (signalType == "pulsed")
            {
               if (!AddDetectionSensitivity(cST_PULSED, frequency, sensitivity))
               {
                  throw UtInput::BadValue(aInput, "Detection sensitivity previously defined for frequency.");
               }
            }
            else if (signalType == "both")
            {
               bool added = true;
               added &= AddDetectionSensitivity(cST_CONTINUOUS, frequency, sensitivity);
               added &= AddDetectionSensitivity(cST_PULSED, frequency, sensitivity);
               if (!added)
               {
                  throw UtInput::BadValue(aInput, "Detection sensitivity previously defined for frequency.");
               }
            }
            else
            {
               throw UtInput::BadValue(aInput);
            }
         }
         else if (command == "signal_type")
         {
            inputBlock.GetInput().ReadValue(signalType);

            if (!((signalType == "continuous") || (signalType == "pulsed") || (signalType == "both")))
            {
               throw UtInput::BadValue(aInput, "Invalid signal_type: " + signalType);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
   }
   else if (command == "detection_probability")
   {
      std::vector<double> signalValues;
      std::vector<double> pdValues;
      UtInputBlock        inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "signal")
         {
            std::string pdString;
            double      signal;
            double      pd;
            aInput.ReadValueOfType(signal, UtInput::cRATIO);
            aInput.ValueGreaterOrEqual(signal, 0.0);
            aInput.ReadValue(pdString);
            UtStringUtil::ToLower(pdString);
            aInput.StringEqual(pdString, "pd");
            aInput.ReadValue(pd);
            aInput.ValueInClosedRange(pd, 0.0, 1.0);
            if ((!signalValues.empty()) && (signal <= signalValues.back()))
            {
               throw UtInput::BadValue(aInput, "signal levels must be monotonically increasing");
            }
            signalValues.push_back(signal);
            pdValues.push_back(pd);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mSignalTable.SetValues(signalValues);
      mPdTable.SetValues(pdValues);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
//! Determines if the signal level from the current detection chance can be detected.
void WsfPassiveDetector::ComputeDetectability(WsfSensorResult& aResult)
{
   WsfEM_Xmtr* xmtrPtr = aResult.GetTransmitter();
   WsfEM_Rcvr* rcvrPtr = aResult.GetReceiver();

   // Initialize() should have ensured the following are true, but we check just in case...
   assert((!mDetectionThresholdTable.empty()) || (!mDetectionSensitivityTable.empty()));
   assert(rcvrPtr->GetNoisePower() > 0.0);

   // Determine the detection threshold (signal level or signal/noise level) that is appropriate
   // for the type of signal being received (continuous or pulsed.)

   SignalType signalType = cST_CONTINUOUS; // Assume a continuous signal
   if (xmtrPtr->GetPulseWidth() > 0.0)
   {
      signalType = cST_PULSED;
   }
   if (!mDetectionSensitivityTable.empty())
   {
      // Check signal vs. detection_sensitivity.
      double sensitivityThreshold = GetDetectionSensitivity(signalType, xmtrPtr->GetFrequency());

      // Compute the signal/noise ratio of the detection sensitivity.
      aResult.mDetectionThreshold = sensitivityThreshold / rcvrPtr->GetNoisePower();
   }
   else if (!mDetectionThresholdTable.empty())
   {
      // Check signal/noise vs. 'detection_threshold'.
      aResult.mDetectionThreshold = GetDetectionThreshold(signalType, xmtrPtr->GetFrequency());
   }

   if (mSignalTable.GetSize() > 0)
   {
      // Pd vs. signal table specified.
      TblLookupLU<double> luSignalLevel;
      luSignalLevel.Lookup(mSignalTable, aResult.mSignalToNoise / aResult.mDetectionThreshold);
      aResult.mPd = TblEvaluate(mPdTable, luSignalLevel);
   }
   else
   {
      // Simple binary detector. Pd is 1 if the signal or signal/noise is sufficient.
      aResult.mPd = (aResult.mSignalToNoise >= aResult.mDetectionThreshold) ? 1.0 : 0.0;
   }
}

// =================================================================================================
//! Clear current list of detection threshold and add the detection sensitivity to
//! the list with the associated lower frequency bound.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The lower frequency bound of the sensitivity value.
//! @param aSensitivity The detection sensitivity to set.
//! @returns True if the insert was successful, False if the input frequency was previously defined.
bool WsfPassiveDetector::SetDetectionSensitivity(SignalType aSignalType, double aFrequency, double aSensitivity)
{
   ClearDetectionThresholdTable();
   mDetectionSensitivityTable[aSignalType].clear();
   return AddDetectionThresholdSensitivity(mDetectionSensitivityTable[aSignalType], aFrequency, aSensitivity);
}

// =================================================================================================
//! Clear current list of detection sensitivities and add the detection threshold to
//! the list with the associated lower frequency bound.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The lower frequency bound of the threshold value.
//! @param aThreshold The detection threshold to set.
//! @returns True if the insert was successful, False if the input frequency was previously defined.
bool WsfPassiveDetector::SetDetectionThreshold(SignalType aSignalType, double aFrequency, double aThreshold)
{
   ClearDetectionSensitivityTable();
   mDetectionThresholdTable[aSignalType].clear();
   return AddDetectionThresholdSensitivity(mDetectionThresholdTable[aSignalType], aFrequency, aThreshold);
}

// =================================================================================================
//! Gets the count of frequency-threshold entries.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @returns the count of frequency-threshold entries.
size_t WsfPassiveDetector::GetDetectionThresholdCount(SignalType aSignalType) const
{
   size_t count = 0;
   if (!mDetectionThresholdTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionThresholdTable.find(aSignalType);
      if (dti != mDetectionThresholdTable.end())
      {
         count = (*dti).second.size();
      }
   }
   return count;
}

// =================================================================================================
//! Gets the data in a frequency-threshold entry.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aIndex The index of the entry.
//! @param aFrequency The frequency in the entry.
//! @param aThreshold The threshold value in the entry.
void WsfPassiveDetector::GetDetectionThresholdEntry(SignalType aSignalType,
                                                    size_t     aIndex,
                                                    double&    aFrequency,
                                                    double&    aThreshold) const
{
   if (!mDetectionThresholdTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionThresholdTable.find(aSignalType);
      if (dti != mDetectionThresholdTable.end())
      {
         aFrequency = (*dti).second[aIndex].first;
         aThreshold = (*dti).second[aIndex].second;
      }
   }
}

// =================================================================================================
//! Gets the count of frequency-sensitivity entries.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @returns the count of frequency-threshold entries.
size_t WsfPassiveDetector::GetDetectionSensitivityCount(SignalType aSignalType) const
{
   size_t count = 0;
   if (!mDetectionSensitivityTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionSensitivityTable.find(aSignalType);
      if (dti != mDetectionSensitivityTable.end())
      {
         count = (*dti).second.size();
      }
   }
   return count;
}

// =================================================================================================
//! Gets the data in a frequency-sensitivity entry.
//!
//! @param aSignalType  Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aIndex       The index of the entry.
//! @param aFrequency   The frequency in the entry.
//! @param aSensitivity The sensitivity value in the entry.
void WsfPassiveDetector::GetDetectionSensitivityEntry(SignalType aSignalType,
                                                      size_t     aIndex,
                                                      double&    aFrequency,
                                                      double&    aSensitivity) const
{
   if (!mDetectionSensitivityTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionSensitivityTable.find(aSignalType);
      if (dti != mDetectionSensitivityTable.end())
      {
         aFrequency   = (*dti).second[aIndex].first;
         aSensitivity = (*dti).second[aIndex].second;
      }
   }
}

// =================================================================================================
//! Add the detection threshold to the list with the associated lower frequency bound.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The lower frequency bound of the threshold value.
//! @param aThreshold The detection threshold to set.
//! @returns True if the insert was successful, False if the input frequency was previously defined.
// private
bool WsfPassiveDetector::AddDetectionThreshold(SignalType aSignalType, double aFrequency, double aThreshold)
{
   return AddDetectionThresholdSensitivity(mDetectionThresholdTable[aSignalType], aFrequency, aThreshold);
}

// =================================================================================================
//! Add the detection sensitivity to the list with the associated lower frequency bound.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The lower frequency bound of the sensitivity value.
//! @param aSensitivity The detection sensitivity to set.
//! @returns True if the insert was successful, False if the input frequency was previously defined.
// private
bool WsfPassiveDetector::AddDetectionSensitivity(SignalType aSignalType, double aFrequency, double aSensitivity)
{
   return AddDetectionThresholdSensitivity(mDetectionSensitivityTable[aSignalType], aFrequency, aSensitivity);
}

// =================================================================================================
//! Return the detection sensitivity for the supplied signal type and frequency.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The frequency of the signal.
//! @return Detection threshold or 0 if there is no associated threshold.
// private
double WsfPassiveDetector::GetDetectionSensitivity(SignalType aSignalType, double aFrequency) const
{
   double detectSensitivity = 0.0;
   if (!mDetectionSensitivityTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionSensitivityTable.find(aSignalType);
      if (dti != mDetectionSensitivityTable.end())
      {
         detectSensitivity = GetDetectionThresholdSensitivity((*dti).second, aFrequency);
      }
   }
   return detectSensitivity;
}

// =================================================================================================
//! Return the detection threshold for the supplied signal type and frequency.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aFrequency The frequency of the signal.
//! @return Detection threshold or 0 if there is no associated threshold.
// private
double WsfPassiveDetector::GetDetectionThreshold(SignalType aSignalType, double aFrequency) const
{
   double detectThreshold = 0.0;
   if (!mDetectionThresholdTable.empty())
   {
      SignalTypeToSensThreshTable::const_iterator dti = mDetectionThresholdTable.find(aSignalType);
      if (dti != mDetectionThresholdTable.end())
      {
         detectThreshold = GetDetectionThresholdSensitivity((*dti).second, aFrequency);
      }
   }
   return detectThreshold;
}

// =================================================================================================
//! Clear the array of tables for the detection threshold and sensitivity
void WsfPassiveDetector::ClearDetectionTables()
{
   ClearDetectionSensitivityTable();
   ClearDetectionThresholdTable();
}

// =================================================================================================
//! Clear the array of tables for the detection sensitivity
void WsfPassiveDetector::ClearDetectionSensitivityTable()
{
   mDetectionSensitivityTable.clear();
}

// =================================================================================================
//! Clear the array of tables for the detection threshold
void WsfPassiveDetector::ClearDetectionThresholdTable()
{
   mDetectionThresholdTable.clear();
}

// =================================================================================================
//! Add the detection threshold/sensitivity to the list with the associated lower frequency bound.
//! @param aDetectionThreshSensList Pointer to the detection/sensitivity list.
//! @param aFrequency The lower frequency bound of the antenna pattern.
//! @param aThreshSens The threshold/sensitivity value.
//! @returns True if the insert was successful, False if a value associated with the frequency already exists.
bool WsfPassiveDetector::AddDetectionThresholdSensitivity(SensitivityThresholdList& aDetectionThreshSensList,
                                                          double                    aFrequency,
                                                          double                    aThreshSens)
{
   bool                               insertSuccess = true;
   SensitivityThresholdList::iterator dtsi          = aDetectionThreshSensList.begin();

   while (dtsi != aDetectionThreshSensList.end())
   {
      // sort inserts by increasing frequency
      if (aFrequency < (*dtsi).first)
      {
         break;
      }
      else if (aFrequency == (*dtsi).first)
      {
         insertSuccess = false;
      }
      ++dtsi;
   }

   if (insertSuccess == true)
   {
      aDetectionThreshSensList.insert(dtsi, std::make_pair(aFrequency, aThreshSens));
   }

   return insertSuccess;
}

// =================================================================================================
//! Get the threshold/sensitivity at a frequency.
//! @param aDetectionThreshSensList Pointer to the detection/sensitivity list.
//! @param aFrequency The frequency at which to get the threshold/sensitivity.
//! @returns The threshold/sensitivity at the input frequency.
double WsfPassiveDetector::GetDetectionThresholdSensitivity(const SensitivityThresholdList& aDetectionThreshSensList,
                                                            double                          aFrequency) const
{
   double detectionThreshold = 0.0;

   if (!aDetectionThreshSensList.empty())
   {
      SensitivityThresholdList::const_iterator cdi = aDetectionThreshSensList.begin();

      while (cdi != aDetectionThreshSensList.end())
      {
         // sorted by increasing frequency
         if (aFrequency < (*cdi).first)
         {
            break;
         }
         ++cdi;
      }

      // Frequency in vector is lower bound of antenna pattern
      // frequency validity range.
      if (cdi != aDetectionThreshSensList.begin())
      {
         --cdi;
      }
      detectionThreshold = (*cdi).second;
   }

   return detectionThreshold;
}
