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

#include "WsfRadarMTI_SignalProcessor.hpp"

#include <algorithm>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_ALARM_Clutter.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Result.hpp"
#include "WsfMTI_Filter.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
//! Constructor
WsfRadarMTI_SignalProcessor::WsfRadarMTI_SignalProcessor(MTI_Type aMTI_Type)
   : WsfSensorSignalProcessor()
   , mFilterOwnshipFromDoppler(true)
   , mMTI_Type(aMTI_Type)
   , mMTI_MinResponse(0.0)
   , mResponseTable()
{
   if (mMTI_Type == cMTIT_ALARM)
   {
      mMTI_Filter = new WsfMTI_Filter();
   }
}

// ================================================================================================
//! Copy constructor (used by Clone).
WsfRadarMTI_SignalProcessor::WsfRadarMTI_SignalProcessor(const WsfRadarMTI_SignalProcessor& aSrc)
   : WsfSensorSignalProcessor(aSrc)
   , mFilterOwnshipFromDoppler(aSrc.mFilterOwnshipFromDoppler)
   , mMTI_Type(aSrc.mMTI_Type)
   , mMTI_MinResponse(aSrc.mMTI_MinResponse)
   , mResponseTable(aSrc.mResponseTable)
{
   if (mMTI_Type == cMTIT_ALARM)
   {
      mMTI_Filter = aSrc.mMTI_Filter->Clone();
   }
}

// ================================================================================================
//! Destructor
// virtual
WsfRadarMTI_SignalProcessor::~WsfRadarMTI_SignalProcessor()
{
   delete mMTI_Filter;
}

// ================================================================================================
//! The object factory for the MTI signal processor type.
// static
WsfSensorSignalProcessor* WsfRadarMTI_SignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if ((aTypeName == "moving_target_indicator") || (aTypeName == "moving_target_indication"))
   {
      instancePtr = new WsfRadarMTI_SignalProcessor(cMTIT_ALARM);
   }
   // else if ((aTypeName == "mti_table") ||
   //          (aTypeName == "MTI_TABLE"))
   //{
   //    instancePtr = new WsfRadarMTI_SignalProcessor(cMTIT_DATA_TABLE);
   // }
   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* WsfRadarMTI_SignalProcessor::Clone() const
{
   return new WsfRadarMTI_SignalProcessor(*this);
}

// ================================================================================================
// virtual
bool WsfRadarMTI_SignalProcessor::Initialize(double         aSimTime,
                                             WsfSensor*     aSensorPtr,
                                             WsfSensorMode* aModePtr,
                                             size_t         aBeamIndex /* = 0*/)
{
   bool ok = true;

   // Verify input based on MTI response mode
   if (mMTI_Type == cMTIT_ALARM)
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
         ok &= mMTI_Filter->Initialize(0.0,
                                       decayConst,
                                       beam->GetEM_Rcvr()->GetBandwidth(),
                                       beam->GetEM_Xmtr()->GetPulseRepetitionFrequencyCount(),
                                       beam->GetEM_Xmtr()->GetPulseRepetitionFrequencies(),
                                       sigmac);
         mMTI_MinResponse = mMTI_Filter->get_min_response();
      }
      else
      {
         ut::log::error() << "WsfRadarMTI_SignalProcessor::Initialize: WsfSensorBeam is nullptr";
         ok = false;
      }
   }
   // else if (mMTI_Type == cMTIT_DATA_TABLE) // MTI type is Data Table
   //{
   //    // Verify that a data table is present
   //    if (! mResponseTable.IsDefined())
   //    {
   //       cout << "***** ERROR: 'mti_data_table' and valid data must be specified for MTI type of 'data_table'" <<
   //       endl; ok = false;
   //    }
   // }

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Initializing MTI Response.";
      logger.AddNote() << "Type: " << mMTI_Type;
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfRadarMTI_SignalProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if ((mMTI_Type == cMTIT_ALARM) && mMTI_Filter->ProcessInput(aInput))
   {
   }
   // else if ((mMTI_Type == cMTIT_DATA_TABLE) &&
   //          ProcessDataTableInput(aInput))
   //{
   //
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

// bool WsfRadarMTI_SignalProcessor::ProcessDataTableInput(UtInput& aInput)
//{
//    bool myCommand = true;
//
//    std::string command = aInput.GetCommand();
//
//    if (command == "mti_data_table")
//    {
//       mResponseTable.ProcessInput(aInput, UtInput::cFREQUENCY, "hz", UtTable::ValueGE(0.0), UtInput::cRATIO, "dB",
//       UtTable::ValueGE(0.0));
//    }
//    else if (command == "mti_min_response")
//    {
//       aInput.ReadValueOfType(mMTI_MinResponse, UtInput::cRATIO);
//       aInput.ValueLessOrEqual(mMTI_MinResponse, 1.0);
//    }
//    else
//    {
//       myCommand = false;
//    }
//
//    return myCommand;
// }

// ================================================================================================
// virtual
void WsfRadarMTI_SignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   // Default MTI level, means no change to the radar's signal to noise ratio
   double TgtMTI_Level           = 1.0;
   double ClutterMTI_Level       = 1.0;
   double NoiseJamMTI_Level      = 1.0;
   double CoherentJamMTI_Level   = 1.0;
   double ReceiverNoiseMTI_Level = 1.0;

   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Prior to applying MTI response:";
      logger.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      logger.AddNote() << "Receiver Noise Power: " << aResult.mRcvrNoisePower << " W";

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
         TgtMTI_Level =
            ComputeMTI_Response(aResult.mRcvrToTgt.mTrueAz, aResult.mRcvrToTgt.mRange, fDop, prfs, WsfMTI_Filter::cST_TARGET);
      }

      if (aResult.mClutterPower > 0.0)
      {
         ClutterMTI_Level =
            ComputeMTI_Response(aResult.mRcvrToTgt.mTrueAz, aResult.mRcvrToTgt.mRange, fDop, prfs, WsfMTI_Filter::cST_CLUTTER);
      }

      if (ewResultPtr != nullptr)
      {
         if ((ewResultPtr->mNoiseJammerPower > 0.0) || (ewResultPtr->mPulseJammerPower > 0.0))
         {
            NoiseJamMTI_Level = ComputeMTI_Response(aResult.mRcvrToTgt.mTrueAz,
                                                    aResult.mRcvrToTgt.mRange,
                                                    fDop,
                                                    prfs,
                                                    WsfMTI_Filter::cST_NOISE_JAM);
         }

         if (ewResultPtr->mCoherentJammerPower > 0.0)
         {
            CoherentJamMTI_Level = ComputeMTI_Response(aResult.mRcvrToTgt.mTrueAz,
                                                       aResult.mRcvrToTgt.mRange,
                                                       fDop,
                                                       prfs,
                                                       WsfMTI_Filter::cST_DECEPT_JAM);
         }
      }

      // if (aResult.mRcvrNoisePower > 0)
      //{
      //    ReceiverNoiseMTI_Level = ComputeMTI_Response(aResult.mRcvrToTgt.mTrueAz,
      //                                                 aResult.mRcvrToTgt.mRange,
      //                                                 fDop,
      //                                                 prfs,
      //                                                 xx);
      // }
   }

   // Adjust the parameters used to calculate the signal to noise ratio
   // based on the MTI response
   // Defend ourselves from bad input that results in an invalid calculated MTI level
   // if ((MTI_Level >= 0.0)/* &&
   //    (MTI_Level <= 1.0)*/)
   //{
   aResult.mRcvdPower *= TgtMTI_Level;
   aResult.mClutterPower *= ClutterMTI_Level;
   aResult.mInterferencePower *= NoiseJamMTI_Level;
   if (ewResultPtr != nullptr)
   {
      ewResultPtr->mNoiseJammerPower *= NoiseJamMTI_Level;
      ewResultPtr->mPulseJammerPower *= NoiseJamMTI_Level;
      ewResultPtr->mCoherentJammerPower *= CoherentJamMTI_Level;
   }
   aResult.mRcvrNoisePower *= ReceiverNoiseMTI_Level;

   if (mDebug)
   {
      auto logger = ut::log::debug() << "After applying an MTI level: ";
      { // RAII block
         auto levelNote = logger.AddNote() << "Applied Level:";
         levelNote.AddNote() << "Tgt: " << TgtMTI_Level;
         levelNote.AddNote() << "RcvrNoise: " << ReceiverNoiseMTI_Level;
         levelNote.AddNote() << "Clut: " << ClutterMTI_Level;
         levelNote.AddNote() << "NoiseJam: " << NoiseJamMTI_Level;
         levelNote.AddNote() << "CohJam: " << CoherentJamMTI_Level;
      }
      auto resultNote = logger.AddNote() << "Result:";
      resultNote.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      resultNote.AddNote() << "Receiver Noise Power: " << aResult.mRcvrNoisePower << " W";
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
   //}
}

// ================================================================================================
// Computes the MTI response based on range and PRF or provides a value from a table
// This method is based on the ALARM method for calculating an MTI response. Differences
// from the ALARM implementation are noted.
// @param aTgtBearing                 The target azimuth wrt receiving sensor.
// @param aTgtSlantRange              The slant range from the receiving sensor to the target.
// @param aTgtDopplerFrequency        The target Doppler frequency of the receiving sensor to the target
// @param aPulseRepetitionFrequencies The PRF list in hertz of the transmitting sensor
// @param aTypeFlag                   The type of signal being passed to the calculation.
// @return Returns a double value of the MTI Response
// private
double WsfRadarMTI_SignalProcessor::ComputeMTI_Response(const double               aTgtBearing,
                                                        const double               aTgtSlantRange,
                                                        const double               aTgtDopplerFrequency,
                                                        const std::vector<double>& aPulseRepetitionFrequencies,
                                                        const int                  aTypeFlag)
{
   double MTI_Response = 1.0; // Or 0 dB

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Computed MTI result.";
      logger.AddNote() << "Type: " << aTypeFlag;
      logger.AddNote() << "Minimum MTI response: " << mMTI_MinResponse;
      logger.AddNote() << "Target Bearing: " << aTgtBearing * UtMath::cDEG_PER_RAD << " deg";
      logger.AddNote() << "Slant Range: " << aTgtSlantRange / 1000.0 << " km";
      logger.AddNote() << "Doppler Frequency: " << aTgtDopplerFrequency << " Hz";
   }

   if (mMTI_Type == cMTIT_ALARM)
   {
      if (aPulseRepetitionFrequencies[0] <= 0.0)
      {
         // Bad input, just return default MTI response
         return MTI_Response;
      }

      // TODO - MTD probably may need a filter per PRF, but will require a PRF loop
      // double response = 0.0;
      // for (int prfIndex = 0; prfIndex < (int)aPulseRepetitionFrequencies.size(); ++prfIndex)
      //{
      //   response = max(response, mMTI_Filter->mti_response(aTgtBearing,
      //                                                      aTgtDopplerFrequency,
      //                                                      aTypeFlag,
      //                                                      aTgtSlantRange));
      //   if (mDebug)
      //   {
      //      cout << "\n  MTI response (PRF " << prfIndex + 1 << " = "
      //           << aPulseRepetitionFrequencies[prfIndex] << " Hz) = "
      //           << response;
      //   }
      //}
      // MTI_Response = response;
      MTI_Response = mMTI_Filter->mti_response(aTgtBearing, aTgtDopplerFrequency, aTypeFlag, aTgtSlantRange);
   }
   else // if (mMTI_Type == cMTIT_DATA_TABLE)
   {
      // Get MTI response value as an absolute ratio from table for Doppler frequency
      MTI_Response = GetTableResponse(aTgtDopplerFrequency);
   }

   // Limit the minimum MTI response
   MTI_Response = std::max(MTI_Response, mMTI_MinResponse);

   if (mDebug)
   {
      ut::log::debug() << "Final MTI response:" << MTI_Response;
   }

   return MTI_Response;
}

// ================================================================================================
// Looks up the MTI response based on Doppler frequency value from a table.
// @param aTargetDopplerFrequency  The target Doppler frequency response the target
// @return Returns a double value of the MTI Response. Returns 1.0 if the table is not valid.
// private
double WsfRadarMTI_SignalProcessor::GetTableResponse(const double aTargetDopplerFrequency)
{
   double response = 1.0;
   if (mResponseTable.IsDefined())
   {
      response = mResponseTable.Lookup(aTargetDopplerFrequency);
      if (mDebug)
      {
         auto logger = ut::log::debug() << "MTI response table lookup:";
         logger.AddNote() << "Response: " << response;
         logger.AddNote() << "Target Doppler Frequency: " << aTargetDopplerFrequency << " Hz.";
      }
   }
   return response;
}
