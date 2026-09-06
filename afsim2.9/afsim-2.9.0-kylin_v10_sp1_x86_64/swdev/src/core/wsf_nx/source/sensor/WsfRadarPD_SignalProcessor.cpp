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

#include "WsfRadarPD_SignalProcessor.hpp"

#include <algorithm>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_ALARM_Clutter.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Result.hpp"
#include "WsfPD_Filter.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
//! Constructor
WsfRadarPD_SignalProcessor::WsfRadarPD_SignalProcessor(PulseDopplerType aPulseDopplerType)
   : WsfSensorSignalProcessor()
   , mFilterOwnshipFromDoppler(true)
   , mPD_Type(aPulseDopplerType)
   , mPD_MinResponse(0.0)
   , mResponseTable()
{
   if (mPD_Type == cPDT_ALARM)
   {
      mPD_Filter = new WsfPD_Filter();
   }
}

// ================================================================================================
//! Copy constructor (used by Clone).
WsfRadarPD_SignalProcessor::WsfRadarPD_SignalProcessor(const WsfRadarPD_SignalProcessor& aSrc)
   : WsfSensorSignalProcessor(aSrc)
   , mFilterOwnshipFromDoppler(aSrc.mFilterOwnshipFromDoppler)
   , mPD_Type(aSrc.mPD_Type)
   , mPD_MinResponse(aSrc.mPD_MinResponse)
   , mResponseTable(aSrc.mResponseTable)
{
   if (mPD_Type == cPDT_ALARM)
   {
      mPD_Filter = aSrc.mPD_Filter->Clone();
   }
}

// ================================================================================================
//! Destructor
// virtual
WsfRadarPD_SignalProcessor::~WsfRadarPD_SignalProcessor()
{
   delete mPD_Filter;
}

// ================================================================================================
//! The object factory for the PD signal processor type.
// static
WsfSensorSignalProcessor* WsfRadarPD_SignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if ((aTypeName == "pulse_doppler") || (aTypeName == "PULSE_DOPPLER"))
   {
      instancePtr = new WsfRadarPD_SignalProcessor(cPDT_ALARM);
   }
   else if ((aTypeName == "pulse_doppler_table") || (aTypeName == "PULSE_DOPPLER_TABLE") || (aTypeName == "PD_TABLE"))
   {
      instancePtr = new WsfRadarPD_SignalProcessor(cPDT_DATA_TABLE);
   }
   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* WsfRadarPD_SignalProcessor::Clone() const
{
   return new WsfRadarPD_SignalProcessor(*this);
}


// ================================================================================================
// virtual
bool WsfRadarPD_SignalProcessor::Initialize(double         aSimTime,
                                            WsfSensor*     aSensorPtr,
                                            WsfSensorMode* aModePtr,
                                            size_t         aBeamIndex /* = 0*/)
{
   bool ok = true;

   // Verify input based on PD response mode
   if (mPD_Type == cPDT_ALARM)
   {
      WsfSensorBeam* beam = aModePtr->GetBeamEntry(aBeamIndex);
      if (beam)
      {
         double sigmac = 10.; // default value - 10 Hz for land, 50 Hz for sea
         if (aSensorPtr->GetSimulation()->GetEnvironment().GetLandCover() == WsfEnvironment::cWATER)
         {
            sigmac = 50.; // default value - 10 Hz for land, 50 Hz for sea
         }
         double         decayConst = 10.e-6; // suggested value 10e-6 Hz^2
         WsfEM_Clutter* clutterPtr = beam->GetClutter();
         if ((clutterPtr != nullptr) &&
             (clutterPtr->IsA_TypeOf("alarm") || clutterPtr->IsA_TypeOf("WSF_ALARM_CLUTTER")))
         {
            sigmac     = static_cast<WsfEM_ALARM_Clutter*>(clutterPtr)->GetSigmaC();
            decayConst = static_cast<WsfEM_ALARM_Clutter*>(clutterPtr)->GetDecayConstant();
         }

         // cout << "***** ERROR: 'pulse_repetition_frequency' for the transmitter must be specified with
         // 'sensitivity_time_control'" << endl; ok = false;
         ok &= mPD_Filter->Initialize(0.0,
                                      decayConst,
                                      beam->GetEM_Rcvr()->GetBandwidth(),
                                      beam->GetEM_Xmtr()->GetPulseRepetitionFrequencyCount(),
                                      beam->GetEM_Xmtr()->GetPulseRepetitionFrequencies(),
                                      beam->GetEM_Xmtr()->GetPulseWidth(),
                                      sigmac);
      }
      else
      {
         ut::log::error() << "WsfRadarPD_SignalProcessor::Initialize: WsfSensorBeam is nullptr";
         ok = false;
      }
   }
   else // PD type is Data Table
   {
      // Verify that a data table is present
      if (!mResponseTable.IsDefined())
      {
         ut::log::error() << "'pd_data_table' and valid data must be specified for PD type of 'data_table'.";
         ok = false;
      }
   }

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Initializing PD Response.";
      logger.AddNote() << "Type: " << mPD_Type;
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfRadarPD_SignalProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if ((mPD_Type == cPDT_ALARM) && mPD_Filter->ProcessInput(aInput))
   {
   }
   else if ((mPD_Type == cPDT_DATA_TABLE) && (command == "pd_data_table"))
   {
      mResponseTable.ProcessInput(aInput,
                                  UtInput::cFREQUENCY,
                                  "hz",
                                  UtTable::ValueGE(0.0),
                                  UtInput::cRATIO,
                                  "dB",
                                  UtTable::ValueGE(0.0));
   }
   else if (command == "pd_min_response")
   {
      aInput.ReadValueOfType(mPD_MinResponse, UtInput::cRATIO);
      aInput.ValueLessOrEqual(mPD_MinResponse, 1.0);
   }
   else if (command == "unfiltered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = false;
   }
   else if (command == "filtered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = true;
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

// ================================================================================================
// virtual
void WsfRadarPD_SignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   // Default PD level, means no change to the radar's signal to noise ratio
   double TgtPD_Level         = 1.0;
   double ClutterPD_Level     = 1.0;
   double NoiseJamPD_Level    = 1.0;
   double CoherentJamPD_Level = 1.0;

   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Prior to applying PD response:";
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
      double              fDop = aResult.ComputeTargetDopplerFrequency(mFilterOwnshipFromDoppler);
      std::vector<double> prfs = aResult.GetTransmitter()->GetPulseRepetitionFrequencies();
      if (aResult.mRcvdPower > 0.0)
      {
         TgtPD_Level = ComputePD_Response(fDop, prfs, WsfPD_Filter::cST_TARGET);
      }

      if (aResult.mClutterPower > 0.0)
      {
         ClutterPD_Level = ComputePD_Response(fDop, prfs, WsfPD_Filter::cST_CLUTTER);
      }

      if (ewResultPtr != nullptr)
      {
         if ((ewResultPtr->mNoiseJammerPower > 0.0) || (ewResultPtr->mPulseJammerPower > 0.0))
         {
            NoiseJamPD_Level = ComputePD_Response(fDop, prfs, WsfPD_Filter::cST_NOISE_JAM);
         }

         if (ewResultPtr->mCoherentJammerPower > 0.0)
         {
            CoherentJamPD_Level = ComputePD_Response(fDop, prfs, WsfPD_Filter::cST_DECEPT_JAM);
         }
      }
   }

   // Adjust the parameters used to calculate the signal to noise ratio
   // based on the PD response
   // Defend ourselves from bad input that results in an invalid calculated PD level
   if ((TgtPD_Level >= 0.0) && (TgtPD_Level <= 1.0))
   {
      aResult.mRcvdPower *= TgtPD_Level;
      aResult.mClutterPower *= ClutterPD_Level;
      aResult.mInterferencePower *= NoiseJamPD_Level;
      if (ewResultPtr != nullptr)
      {
         ewResultPtr->mNoiseJammerPower *= NoiseJamPD_Level;
         ewResultPtr->mPulseJammerPower *= NoiseJamPD_Level;
         ewResultPtr->mCoherentJammerPower *= CoherentJamPD_Level;
      }

      if (mDebug)
      {
         auto logger = ut::log::debug() << "After applying an PD level of (Tgt/Clut/NoiseJam/CohJam): ";
         { // RAII block
            auto levelNote = logger.AddNote() << "Applied Level:";
            levelNote.AddNote() << "Tgt: " << TgtPD_Level;
            levelNote.AddNote() << "Clut: " << ClutterPD_Level;
            levelNote.AddNote() << "NoiseJam: " << NoiseJamPD_Level;
            levelNote.AddNote() << "CohJam: " << CoherentJamPD_Level;
         }
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
// Computes the PD response based on Doppler return and PRF or provides a value from a table
// This method is based on the ALARM method for calculating an MTI response. Differences
// from the ALARM implementation are noted.
// @param aTgtDopplerFrequency        The target Doppler frequency of the receiving sensor to the target
// @param aPulseRepetitionFrequencies The PRF list in hertz of the transmitting sensor
// @param aTypeFlag                   The type of signal being passed to the calculation.
// @return Returns a double value of the PD Response
// private
double WsfRadarPD_SignalProcessor::ComputePD_Response(const double               aTgtDopplerFrequency,
                                                      const std::vector<double>& aPulseRepetitionFrequencies,
                                                      int                        aTypeFlag)
{
   double PD_Response = 1.0; // Or 0 dB

   if (aPulseRepetitionFrequencies[0] <= 0.0)
   {
      // Bad input, just return default PD response
      return PD_Response;
   }

   auto logger = ut::log::debug();
   if (mDebug)
   {
      logger << "Computed PD result.";
      logger.AddNote() << "Type: " << aTypeFlag;
      logger.AddNote() << "Minimum PD response: " << mPD_MinResponse;
      logger.AddNote() << "Doppler Frequency: " << aTgtDopplerFrequency << " Hz";
   }

   if (mPD_Type == cPDT_ALARM)
   {
      double pd_response = 0.0;
      for (int prfIndex = 0; prfIndex < static_cast<int>(aPulseRepetitionFrequencies.size()); ++prfIndex)
      {
         double response = mPD_Filter->pd_response(aTgtDopplerFrequency, aTypeFlag, prfIndex);
         if (mDebug)
         {
            auto prfNote = logger.AddNote() << "PRF " << prfIndex + 1 << ":";
            prfNote.AddNote() << "Frequency: " << aPulseRepetitionFrequencies[prfIndex] << " Hz";
            prfNote.AddNote() << "PD Response: " << response;
         }
         pd_response = std::max(pd_response, response);
      }
      PD_Response = pd_response;
   }
   else // if (mPD_Type == cPDT_DATA_TABLE)
   {
      // Get PD response value as an absolute ratio from table for Doppler frequency
      PD_Response = GetTableResponse(aTgtDopplerFrequency);
   }

   // Limit the PD response to be between PD_MinResponse and 1.0
   PD_Response = std::max(std::min(PD_Response, 1.0), mPD_MinResponse);

   if (mDebug)
   {
      logger.AddNote() << "Final PD Response: " << PD_Response;
   }

   return PD_Response;
}

// ================================================================================================
// Looks up the PD response based on Doppler frequency value from a table.
// @param aTargetDopplerFrequency  The target Doppler frequency response the target
// @return Returns a double value of the PD Response. Returns 1.0 if the table is not valid.
// private
double WsfRadarPD_SignalProcessor::GetTableResponse(const double aTargetDopplerFrequency)
{
   double response = 1.0;
   if (mResponseTable.IsDefined())
   {
      response = mResponseTable.Lookup(aTargetDopplerFrequency);
      if (mDebug)
      {
         auto logger = ut::log::debug() << "PD response table lookup:";
         logger.AddNote() << "Response: " << response;
         logger.AddNote() << "Target Doppler Frequency: " << aTargetDopplerFrequency << " Hz.";
      }
   }
   return response;
}
