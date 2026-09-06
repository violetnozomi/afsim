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

#include "WsfRadarSTC_SignalProcessor.hpp"

#include <algorithm>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Result.hpp"
#include "WsfRadarSensor.hpp"

// ================================================================================================
//! Constructor
WsfRadarSTC_SignalProcessor::WsfRadarSTC_SignalProcessor()
   : WsfSensorSignalProcessor()
   , mSTC_Type(cSTC_MIN_RANGE)
   , mSTC_MinResponse(1.0)
   , // Default of 0 dB, or no change to response
   mSTC_MinRange(0.0)
   , mSTC_MaxRange(0.0)
   , mSTC_Order(4)
{
}

// ================================================================================================
//! The object factory for the STC signal processor type.
// static
WsfSensorSignalProcessor* WsfRadarSTC_SignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if ((aTypeName == "sensitivity_time_control") || (aTypeName == "SENSITIVITY_TIME_CONTROL") || (aTypeName == "STC"))
   {
      instancePtr = new WsfRadarSTC_SignalProcessor();
   }

   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* WsfRadarSTC_SignalProcessor::Clone() const
{
   return new WsfRadarSTC_SignalProcessor(*this);
}


// ================================================================================================
// virtual
bool WsfRadarSTC_SignalProcessor::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   bool ok = true;

   // Ensure a PRF was specified.
   WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(aModePtr);
   for (auto& beamPtr : radarModePtr->mBeamList)
   {
      if (beamPtr->mXmtrPtr->GetPulseRepetitionFrequency() <= 0.0)
      {
         ut::log::error()
            << "'pulse_repetition_frequency' for the transmitter must be specified with 'sensitivity_time_control'.";
         ok = false;
      }
   }

   // Verify input based on STC response mode
   if (mSTC_Type != cSTC_DATA_TABLE)
   {
      // If the type is not a data table look-up, verify the parameters
      // that will be used in calculating the STC response type
      if (mSTC_Type != cSTC_MAX_RANGE && mSTC_MinRange < 0.0)
      {
         ut::log::error()
            << "'stc_min_range' must be provided if STC type of 'min_range' or 'min_max_range' is specified.";
         ok = false;
      }
      if ((mSTC_Type == cSTC_MAX_RANGE || mSTC_Type == cSTC_MIN_MAX_RANGE) && (mSTC_MaxRange <= 0.0))
      {
         ut::log::error()
            << "'stc_max_range' must be provided if STC type of 'max_range' or 'min_max_range' is specified.";
         ok = false;
      }
      if (mSTC_Type == cSTC_MIN_MAX_RANGE && (mSTC_MaxRange < mSTC_MinRange))
      {
         ut::log::error() << "'stc_max_range' must be greater than 'stc_min_range' for STC type of 'min_max_range'.";
         ok = false;
      }
   }
   else // STC type is Data Table
   {
      // Verify that a data table is present
      if (!mResponseTable.IsDefined())
      {
         ut::log::error() << "'stc_data_table' and valid data must be specified for STC type of 'data_table'.";
         ok = false;
      }
   }

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Initializing STC Response.";
      logger.AddNote() << "Type: " << mSTC_Type;
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
      logger.AddNote() << "Minimum STC Response: " << UtMath::SafeLinearToDB(mSTC_MinResponse) << " dB, "
                       << mSTC_MinResponse << " absolute";
      logger.AddNote() << "Minimum STC Range: " << mSTC_MinRange << " m";
      logger.AddNote() << "Maximum STC Range: " << mSTC_MaxRange << " m";
      logger.AddNote() << "STC Order: " << mSTC_Order;
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfRadarSTC_SignalProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if (command == "stc_type")
   {
      std::string tempSTCType;
      aInput.ReadValue(tempSTCType);
      if (tempSTCType == "min_range")
      {
         mSTC_Type = cSTC_MIN_RANGE;
      }
      else if (tempSTCType == "max_range")
      {
         mSTC_Type = cSTC_MAX_RANGE;
      }
      else if (tempSTCType == "min_max_range")
      {
         mSTC_Type = cSTC_MIN_MAX_RANGE;
      }
      else if (tempSTCType == "data_table")
      {
         mSTC_Type = cSTC_DATA_TABLE;
      }
      else
      {
         myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
      }
   }
   else if (command == "stc_min_response")
   {
      aInput.ReadValueOfType(mSTC_MinResponse, UtInput::cRATIO);
      aInput.ValueLessOrEqual(mSTC_MinResponse, 1.0);
   }
   else if (command == "stc_min_range")
   {
      aInput.ReadValueOfType(mSTC_MinRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSTC_MinRange, 0.0);
   }
   else if (command == "stc_max_range")
   {
      aInput.ReadValueOfType(mSTC_MaxRange, UtInput::cLENGTH);
      aInput.ValueGreater(mSTC_MaxRange, 0.0);
   }
   else if (command == "stc_order")
   {
      aInput.ReadValue(mSTC_Order);
      aInput.ValueGreaterOrEqual(mSTC_Order, 0.0);
   }
   else if (command == "stc_data_table")
   {
      mResponseTable
         .ProcessInput(aInput, UtInput::cLENGTH, "m", UtTable::ValueGE(0.0), UtInput::cRATIO, "dB", UtTable::ValueGE(0.0));
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

// ================================================================================================
// virtual
void WsfRadarSTC_SignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   // Default STC level, means no change to the radar's signal to noise ratio
   double STC_Level = 1.0;

   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Prior to applying STC response:";
      logger.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      if (aResult.mClutterPower > 0.0)
      {
         logger.AddNote() << "Clutter Power: " << aResult.mClutterPower << " W";
      }
      if (aResult.mInterferencePower > 0.0)
      {
         logger.AddNote() << "InterferencePower Power: " << aResult.mInterferencePower << " W";
      }
      if (ewResultPtr != nullptr)
      {
         if (ewResultPtr->mNoiseJammerPower > 0.0)
         {
            logger.AddNote() << "Noise Jamming Power: " << ewResultPtr->mNoiseJammerPower << " W";
         }
         if (ewResultPtr->mPulseJammerPower > 0.0)
         {
            logger.AddNote() << "Pulse Jamming Power: " << ewResultPtr->mPulseJammerPower << " W";
         }
         if (ewResultPtr->mCoherentJammerPower > 0.0)
         {
            logger.AddNote() << "Coherent Jamming Power: " << ewResultPtr->mCoherentJammerPower << " W";
         }
      }
   }

   // Make sure the interaction has a transmitter
   if (aResult.GetTransmitter() != nullptr)
   {
      STC_Level = ComputeSTC_Response(aResult.mRcvrToTgt.mRange, aResult.GetTransmitter()->GetPulseRepetitionFrequency());
   }

   // Adjust the parameters used to calculate the signal to noise ratio
   // based on the STC response
   // Defend ourselves from bad input that results in an invalid calculated STC level
   if ((STC_Level >= 0.0) && (STC_Level <= 1.0))
   {
      aResult.mRcvdPower *= STC_Level;
      aResult.mClutterPower *= STC_Level;
      aResult.mInterferencePower *= STC_Level;
      if (ewResultPtr != nullptr)
      {
         ewResultPtr->mNoiseJammerPower *= STC_Level;
         ewResultPtr->mPulseJammerPower *= STC_Level;
         ewResultPtr->mCoherentJammerPower *= STC_Level;
      }

      if (mDebug)
      {
         auto logger = ut::log::debug() << "After applying an STC level:";
         logger.AddNote() << "Applied Level: " << STC_Level;
         auto resultNote = logger.AddNote() << "Result:";
         resultNote.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
         if (aResult.mClutterPower > 0.0)
         {
            resultNote.AddNote() << "Clutter Power: " << aResult.mClutterPower << " W";
         }
         if (aResult.mInterferencePower > 0.0)
         {
            resultNote.AddNote() << "InterferencePower Power: " << aResult.mInterferencePower << " W";
         }
         if (ewResultPtr != nullptr)
         {
            if (ewResultPtr->mNoiseJammerPower > 0.0)
            {
               resultNote.AddNote() << "Noise Jamming Power: " << ewResultPtr->mNoiseJammerPower << " W";
            }
            if (ewResultPtr->mPulseJammerPower > 0.0)
            {
               resultNote.AddNote() << "Pulse Jamming Power: " << ewResultPtr->mPulseJammerPower << " W";
            }
            if (ewResultPtr->mCoherentJammerPower > 0.0)
            {
               resultNote.AddNote() << "Coherent Jamming Power: " << ewResultPtr->mCoherentJammerPower << " W";
            }
         }
      }
   }
}

// ================================================================================================
// Computes the STC response based on range and PRF or provides a value from a table
// This method is based on the SUPPRESSOR method for calculating an STC response and was taken
// from resstc.f. Differences from the SUPPRESSOR implementation are noted.
// @param aSlantRange               The range in meters of the receiving sensor to the target
// @param aPulseRepititionFrequency The PRF in hertz of the transmitting sensor
// @return Returns a double value of the STC Response
// private
double WsfRadarSTC_SignalProcessor::ComputeSTC_Response(const double aSlantRange, const double aPulseRepetitionFrequency)
{
   double STC_Response = 1.0; // Or 0 dB

   if ((aPulseRepetitionFrequency <= 0.0) || (aSlantRange <= 0.0))
   {
      // Bad input, just return default STC response
      return STC_Response;
   }

   // Convert PRF to maximum unambiguous range
   double unambiguousRangeMax = 0.5 * (UtMath::cLIGHT_SPEED / aPulseRepetitionFrequency);

   // Determine range to signal source within one unambiguous range
   double unambiguousRange = aSlantRange - floor(aSlantRange / unambiguousRangeMax) * unambiguousRangeMax;

   // Convert min / max STC ranges from slant range to unambiguous range
   // NOTE: this is slightly different than the SUPPRESSOR implementation, SUPPRESSOR
   // assumed that the min / max ranges were entered as the unambiguous range
   double unambiguousMinRange = mSTC_MinRange - floor(mSTC_MinRange / unambiguousRangeMax) * unambiguousRangeMax;
   double unambiguousMaxRange = mSTC_MaxRange - floor(mSTC_MaxRange / unambiguousRangeMax) * unambiguousRangeMax;

   if (mSTC_Type == cSTC_MIN_RANGE)
   {
      if (unambiguousRange >= unambiguousMinRange)
      {
         STC_Response = mSTC_MinResponse * pow((unambiguousRange / unambiguousMinRange), mSTC_Order);
      }
      else
      {
         STC_Response = mSTC_MinResponse;
      }
   }
   else if (mSTC_Type == cSTC_MAX_RANGE)
   {
      if (unambiguousRange <= unambiguousMaxRange)
      {
         STC_Response = pow((unambiguousRange / unambiguousMaxRange), mSTC_Order);
      }
      else
      {
         STC_Response = 1.0;
      }
   }
   else if (mSTC_Type == cSTC_MIN_MAX_RANGE)
   {
      if (unambiguousRange < unambiguousMinRange)
      {
         STC_Response = mSTC_MinResponse;
      }
      else if (unambiguousRange > mSTC_MaxRange)
      {
         STC_Response = 1.0;
      }
      else
      {
         STC_Response = pow((unambiguousRange / unambiguousMaxRange), mSTC_Order);
      }
   }
   else // if (mSTC_Type == cSTC_DATA_TABLE)
   {
      // Get STC response value as an absolute ratio from table for slant range
      STC_Response = GetTableResponse(aSlantRange);
      // NOTE: The SUPPRESSOR code used the unambiguous range as the look up
      // and had the data stored as dB values necessitating a conversion to absolute
   }

   // Limit the STC response to be between STC_MinResponse and 1.0
   STC_Response = std::max(std::min(STC_Response, 1.0), mSTC_MinResponse);

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Computed STC result.";
      logger.AddNote() << "Type: " << mSTC_Type;
      logger.AddNote() << "Minimum STC Response: " << UtMath::SafeLinearToDB(mSTC_MinResponse) << " dB, "
                       << mSTC_MinResponse << " absolute";
      logger.AddNote() << "Slant Range: " << aSlantRange << " m";
      logger.AddNote() << "PRF: " << aPulseRepetitionFrequency << " Hz";
      logger.AddNote() << "Unambiguous Range: " << unambiguousRange << " m";
      logger.AddNote() << "STC Response: " << STC_Response;
   }

   return STC_Response;
}

// ================================================================================================
// Looks up the STC response based on slant range value from a table.
// @param aSlantRange  The range in meters of the receiving sensor to the target
// @return Returns a double value of the STC Response. Returns 1.0 if the table is not valid.
// private
double WsfRadarSTC_SignalProcessor::GetTableResponse(const double aSlantRange)
{
   double response = 1.0;
   if (mResponseTable.IsDefined())
   {
      response = mResponseTable.Lookup(aSlantRange);
      if (mDebug)
      {
         auto logger = ut::log::debug() << "STC response table lookup:";
         logger.AddNote() << "Response: " << response;
         logger.AddNote() << "Slant Range: " << aSlantRange << " m";
      }
   }
   return response;
}
