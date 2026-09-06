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

#include "WsfRadarMTD_SignalProcessor.hpp"

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
#include "WsfPD_Filter.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
//! Constructor
WsfRadarMTD_SignalProcessor::WsfRadarMTD_SignalProcessor(MTD_Type aMTD_Type)
   : WsfSensorSignalProcessor()
   , mFilterOwnshipFromDoppler(true)
   , mMTD_Type(aMTD_Type)
   , mMTD_MinResponse(0.0)
   , mResponseTable()
   , mMTI_Filters()
   , mPD_Filter(nullptr)
   , mZVF_Filter(nullptr)
{
   if (mMTD_Type == cMTDT_ALARM)
   {
      mMTI_Filters.push_back(new WsfMTI_Filter());
      mPD_Filter  = new WsfPD_Filter();
      mZVF_Filter = new WsfPD_Filter(true); // set ZVF flag to true for PD filter
   }
}

// ================================================================================================
//! Copy constructor (used by Clone).
WsfRadarMTD_SignalProcessor::WsfRadarMTD_SignalProcessor(const WsfRadarMTD_SignalProcessor& aSrc)
   : WsfSensorSignalProcessor(aSrc)
   , mFilterOwnshipFromDoppler(aSrc.mFilterOwnshipFromDoppler)
   , mMTD_Type(aSrc.mMTD_Type)
   , mMTD_MinResponse(aSrc.mMTD_MinResponse)
   , mResponseTable(aSrc.mResponseTable)
{
   if (mMTD_Type == cMTDT_ALARM)
   {
      mMTI_Filters.clear();
      std::transform(aSrc.mMTI_Filters.begin(),
                     aSrc.mMTI_Filters.end(),
                     std::back_inserter(mMTI_Filters),
                     [](const WsfMTI_Filter* srcFilter) { return srcFilter->Clone(); });
      mPD_Filter  = aSrc.mPD_Filter->Clone();
      mZVF_Filter = aSrc.mZVF_Filter->Clone();
   }
}

// ================================================================================================
//! Destructor
// virtual
WsfRadarMTD_SignalProcessor::~WsfRadarMTD_SignalProcessor()
{
   delete mPD_Filter;
   delete mZVF_Filter;
   for (auto& mtiFilter : mMTI_Filters)
   {
      delete mtiFilter;
   }
}

// ================================================================================================
//! The object factory for the MTD signal processor type.
// static
WsfSensorSignalProcessor* WsfRadarMTD_SignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if ((aTypeName == "moving_target_detector") || (aTypeName == "moving_target_detection"))
   {
      instancePtr = new WsfRadarMTD_SignalProcessor(cMTDT_ALARM);
   }
   else if ((aTypeName == "mtd_table") || (aTypeName == "MTD_TABLE"))
   {
      instancePtr = new WsfRadarMTD_SignalProcessor(cMTDT_DATA_TABLE);
   }
   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* WsfRadarMTD_SignalProcessor::Clone() const
{
   return new WsfRadarMTD_SignalProcessor(*this);
}

// ================================================================================================
// virtual
bool WsfRadarMTD_SignalProcessor::Initialize(double         aSimTime,
                                             WsfSensor*     aSensorPtr,
                                             WsfSensorMode* aModePtr,
                                             size_t         aBeamIndex /* = 0*/)
{
   bool ok = true;

   // Verify input based on MTD response mode
   if (mMTD_Type == cMTDT_ALARM)
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

         size_t numPRFs = static_cast<size_t>(beam->GetEM_Xmtr()->GetPulseRepetitionFrequencyCount());
         if (mMTI_Filters.size() > numPRFs)
         {
            mMTI_Filters.resize(numPRFs);
         }
         for (size_t iprf = 0; iprf < numPRFs; ++iprf)
         {
            if (beam->GetEM_Xmtr()->GetPulseRepetitionFrequency(iprf + 1) <= 0.0)
            {
               auto logger =
                  ut::log::error()
                  << "'pulse_repetition_frequency/interval' must be specified with 'mtd' on the transmitter.";
               logger.AddNote() << "Number: " << iprf + 1;
               ok = false;
            }

            if ((mMTI_Filters.size() - 1) < iprf)
            {
               mMTI_Filters.push_back(mMTI_Filters[0]->Clone());
            }

            std::vector<double> priVec;
            priVec.assign(1, beam->GetEM_Xmtr()->GetPulseRepetitionFrequencies()[iprf]);
            ok &= mMTI_Filters[iprf]->Initialize(0.0, decayConst, beam->GetEM_Rcvr()->GetBandwidth(), 1, priVec, sigmac);

            // TODO - is getting the mti min response to override the mtd entered one correct?
            mMTD_MinResponse = std::max(mMTD_MinResponse, mMTI_Filters[iprf]->get_min_response());
         }


         ok &= mPD_Filter->Initialize(0.0,
                                      decayConst,
                                      beam->GetEM_Rcvr()->GetBandwidth(),
                                      beam->GetEM_Xmtr()->GetPulseRepetitionFrequencyCount(),
                                      beam->GetEM_Xmtr()->GetPulseRepetitionFrequencies(),
                                      beam->GetEM_Xmtr()->GetPulseWidth(),
                                      sigmac);

         ok &= mZVF_Filter->Initialize(0.0,
                                       decayConst,
                                       beam->GetEM_Rcvr()->GetBandwidth(),
                                       beam->GetEM_Xmtr()->GetPulseRepetitionFrequencyCount(),
                                       beam->GetEM_Xmtr()->GetPulseRepetitionFrequencies(),
                                       beam->GetEM_Xmtr()->GetPulseWidth(),
                                       sigmac);
      }
      else
      {
         ut::log::error() << "WsfRadarMTD_SignalProcessor::Initialize: WsfSensorBeam is nullptr";
         ok = false;
      }
   }
   else // MTD type is Data Table
   {
      // Verify that a data table is present
      if (!mResponseTable.IsDefined())
      {
         ut::log::error() << "'mtd_data_table' and valid data must be specified for MTD type of 'data_table'.";
         ok = false;
      }
   }

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Initializing MTD Response.";
      logger.AddNote() << "Type: " << mMTD_Type;
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfRadarMTD_SignalProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if ((mMTD_Type == cMTDT_ALARM) && mMTI_Filters[0]->ProcessInput(aInput))
   {
   }
   else if ((mMTD_Type == cMTDT_ALARM) && mPD_Filter->ProcessInput(aInput))
   {
   }
   else if ((mMTD_Type == cMTDT_ALARM) && mZVF_Filter->ProcessInput(aInput))
   {
   }
   else if ((mMTD_Type == cMTDT_DATA_TABLE) && (command == "mtd_data_table"))
   {
      mResponseTable.ProcessInput(aInput,
                                  UtInput::cFREQUENCY,
                                  "hz",
                                  UtTable::ValueGE(0.0),
                                  UtInput::cRATIO,
                                  "dB",
                                  UtTable::ValueGE(0.0));
   }
   else if ((mMTD_Type == cMTDT_DATA_TABLE) && (command == "mtd_min_response"))
   {
      aInput.ReadValueOfType(mMTD_MinResponse, UtInput::cRATIO);
      aInput.ValueLessOrEqual(mMTD_MinResponse, 1.0);
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
void WsfRadarMTD_SignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   // Default MTD level, means no change to the radar's signal to noise ratio
   double TgtMTI_PD_Level         = 1.0;
   double ClutterMTI_PD_Level     = 1.0;
   double NoiseJamMTI_PD_Level    = 1.0;
   double CoherentJamMTI_PD_Level = 1.0;

   // Default ZVF Levels, means no change to the radar's signal to noise ratio
   double TgtZVF_Level         = 1.0;
   double ClutterZVF_Level     = 1.0;
   double NoiseJamZVF_Level    = 1.0;
   double CoherentJamZVF_Level = 1.0;

   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Prior to applying MTD response:";
      logger.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      if (aResult.mClutterPower > 0.0)
      {
         logger.AddNote() << "Clutter Power: " << aResult.mClutterPower << " W";
      }
      if (aResult.mInterferencePower > 0.0)
      {
         logger.AddNote() << "Interference Power: " << aResult.mInterferencePower << " W";
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
         TgtMTI_PD_Level =
            ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz, aResult.mRcvrToTgt.mRange, fDop, prfs, WsfMTI_Filter::cST_TARGET);
         TgtZVF_Level = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                            aResult.mRcvrToTgt.mRange,
                                            fDop,
                                            prfs,
                                            WsfMTI_Filter::cST_TARGET,
                                            true);
      }

      if (aResult.mClutterPower > 0.0)
      {
         ClutterMTI_PD_Level =
            ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz, aResult.mRcvrToTgt.mRange, fDop, prfs, WsfPD_Filter::cST_CLUTTER);
         ClutterZVF_Level = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                                aResult.mRcvrToTgt.mRange,
                                                fDop,
                                                prfs,
                                                WsfPD_Filter::cST_CLUTTER,
                                                true);
      }

      if (ewResultPtr != nullptr)
      {
         if ((ewResultPtr->mNoiseJammerPower > 0.0) || (ewResultPtr->mPulseJammerPower > 0.0))
         {
            NoiseJamMTI_PD_Level = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                                       aResult.mRcvrToTgt.mRange,
                                                       fDop,
                                                       prfs,
                                                       WsfPD_Filter::cST_NOISE_JAM);
            NoiseJamZVF_Level    = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                                    aResult.mRcvrToTgt.mRange,
                                                    fDop,
                                                    prfs,
                                                    WsfPD_Filter::cST_NOISE_JAM,
                                                    true);
         }

         if (ewResultPtr->mCoherentJammerPower > 0.0)
         {
            CoherentJamMTI_PD_Level = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                                          aResult.mRcvrToTgt.mRange,
                                                          fDop,
                                                          prfs,
                                                          WsfPD_Filter::cST_DECEPT_JAM);
            CoherentJamZVF_Level    = ComputeMTD_Response(aResult.mRcvrToTgt.mTrueAz,
                                                       aResult.mRcvrToTgt.mRange,
                                                       fDop,
                                                       prfs,
                                                       WsfPD_Filter::cST_DECEPT_JAM,
                                                       true);
         }
      }
   }

   double MTI_PD_SNR = aResult.GetReceiver()->ComputeSignalToNoise(aResult.mRcvdPower * TgtMTI_PD_Level,
                                                                   aResult.mClutterPower * ClutterMTI_PD_Level,
                                                                   (aResult.mInterferencePower) * NoiseJamMTI_PD_Level);
   double ZVF_SNR    = aResult.GetReceiver()->ComputeSignalToNoise(aResult.mRcvdPower * TgtZVF_Level,
                                                                aResult.mClutterPower * ClutterZVF_Level,
                                                                (aResult.mInterferencePower) * NoiseJamZVF_Level);

   // Adjust the parameters used to calculate the signal to noise ratio
   // based on the MTD or ZVF response
   // Defend ourselves from bad input that results in an invalid calculated MTD level
   // if ((MTD_Level >= 0.0) &&
   //    (MTD_Level <= 1.0))
   //{
   auto logger     = ut::log::debug();
   auto resultNote = logger.AddNote();
   if (MTI_PD_SNR >= ZVF_SNR)
   {
      aResult.mRcvdPower *= TgtMTI_PD_Level;
      aResult.mClutterPower *= ClutterMTI_PD_Level;
      aResult.mInterferencePower *= NoiseJamMTI_PD_Level;
      if (ewResultPtr != nullptr)
      {
         ewResultPtr->mNoiseJammerPower *= NoiseJamMTI_PD_Level;
         ewResultPtr->mPulseJammerPower *= NoiseJamMTI_PD_Level;
         // TODO assume for now the best target returns gives us the coherent return,
         // TODO need to calculate SNR here also
         ewResultPtr->mCoherentJammerPower *= CoherentJamMTI_PD_Level;
      }

      if (mDebug)
      {
         logger << "After applying an MTD level:";
         // OK: levelNote should be applied before resultNote.
         auto levelNote = logger.AddNote() << "Applied Level:";
         levelNote.AddNote() << "Tgt: " << TgtMTI_PD_Level;
         levelNote.AddNote() << "Clut: " << ClutterMTI_PD_Level;
         levelNote.AddNote() << "NoiseJam: " << NoiseJamMTI_PD_Level;
         levelNote.AddNote() << "CohJam: " << CoherentJamMTI_PD_Level;

         resultNote << "Result:";
         resultNote.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      }
   }
   else
   {
      aResult.mRcvdPower *= TgtZVF_Level;
      aResult.mClutterPower *= ClutterZVF_Level;
      aResult.mInterferencePower *= NoiseJamZVF_Level;
      if (ewResultPtr != nullptr)
      {
         ewResultPtr->mNoiseJammerPower *= NoiseJamZVF_Level;
         ewResultPtr->mPulseJammerPower *= NoiseJamZVF_Level;
         // TODO assume for now the best target returns gives us the coherent return,
         // TODO need to calculate SNR here also
         ewResultPtr->mCoherentJammerPower *= CoherentJamZVF_Level;
      }

      if (mDebug)
      {
         logger << "After applying an MTD level:";
         // OK: levelNote should be applied before resultNote.
         auto levelNote = logger.AddNote() << "Applied Level:";
         levelNote.AddNote() << "Tgt: " << TgtZVF_Level;
         levelNote.AddNote() << "Clut: " << ClutterZVF_Level;
         levelNote.AddNote() << "NoiseJam: " << NoiseJamZVF_Level;
         levelNote.AddNote() << "CohJam: " << CoherentJamZVF_Level;

         resultNote << "Result:";
         resultNote.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      }
   }

   if (mDebug)
   {
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
// Computes the MTI and PD response based on Target Doppler, Bearing and Slant Range or provides
// value from a table
// This method is based on the ALARM method for calculating an MTD response. Differences
// from the ALARM implementation are noted.
// @param aTgtBearing                 The target azimuth wrt receiving sensor.
// @param aTgtSlantRange              The slant range from the receiving sensor to the target.
// @param aTgtDopplerFrequency        The target Doppler frequency of the receiving sensor to the target
// @param aPulseRepetitionFrequencies The PRF list in hertz of the transmitting sensor
// @param aTypeFlag                   The type of signal being passed to the calculation.
// @param aZVF_Flag                   The flag as to whether this is the ZVF channel return only (true),
//                                    otherwise the MTI-PD channel is returned (false) (default).
// @return Returns a double value of the MTD Response
// private
double WsfRadarMTD_SignalProcessor::ComputeMTD_Response(const double               aTgtBearing,
                                                        const double               aTgtSlantRange,
                                                        const double               aTgtDopplerFrequency,
                                                        const std::vector<double>& aPulseRepetitionFrequencies,
                                                        const int                  aTypeFlag,
                                                        bool                       aZVF_Only /* = false*/)
{
   double MTD_Response = 1.0; // Or 0.0 dB

   if (aPulseRepetitionFrequencies[0] <= 0.0)
   {
      // Bad input, just return default MTD response
      return MTD_Response;
   }


   if (mMTD_Type == cMTDT_ALARM)
   {
      if (aZVF_Only)
      {
         auto logger = ut::log::debug();
         if (mDebug)
         {
            logger << "Computed MTD (ZVF) result for type: " << aTypeFlag;
            logger.AddNote() << "Minimum MTD response: " << mMTD_MinResponse;
            logger.AddNote() << "Target Bearing: " << aTgtBearing * UtMath::cDEG_PER_RAD << " deg";
            logger.AddNote() << "Slant Range: " << aTgtSlantRange / 1000.0 << " km";
            logger.AddNote() << "Doppler Frequency: " << aTgtDopplerFrequency << " Hz";
         }

         double zvf_response = 0.0;
         for (int iprf = 0; iprf < static_cast<int>(aPulseRepetitionFrequencies.size()); ++iprf)
         {
            auto prfNote = logger.AddNote();
            if (mDebug)
            {
               prfNote << "PRF " << iprf + 1 << ":";
               prfNote.AddNote() << "Frequency: " << aPulseRepetitionFrequencies[iprf] << " Hz";
            }

            double response = mZVF_Filter->pd_response(aTgtDopplerFrequency, aTypeFlag, iprf);
            zvf_response    = std::max(zvf_response, response);

            if (mDebug)
            {
               prfNote.AddNote() << "ZVF Response: " << response;
            }
         }

         if (mDebug)
         {
            logger.AddNote() << "ZVF Response: " << zvf_response;
         }

         // Since there are two channels that go to the detector, one for the
         // MTI and PD fitlers and another for the ZVF, take the maximum of each
         // channel, the other channels will be 0.0 anyway.
         // TODO will need to do something different for PSD stuff.
         MTD_Response = zvf_response;
      }
      else
      {
         auto logger = ut::log::debug();
         if (mDebug)
         {
            logger << "Computed MTD (MTI-PD) result for type: " << aTypeFlag;
            logger.AddNote() << "Minimum MTD response: " << mMTD_MinResponse;
            logger.AddNote() << "Target Bearing: " << aTgtBearing * UtMath::cDEG_PER_RAD << " deg";
            logger.AddNote() << "Slant Range: " << aTgtSlantRange / 1000.0 << " km";
            logger.AddNote() << "Doppler Frequency: " << aTgtDopplerFrequency << " Hz";
         }

         double mti_response = 0.0;
         double pd_response  = 0.0;
         for (int iprf = 0; iprf < static_cast<int>(aPulseRepetitionFrequencies.size()); ++iprf)
         {
            auto prfNote = logger.AddNote();
            if (mDebug)
            {
               prfNote << "PRF " << iprf + 1 << ":";
               prfNote.AddNote() << "Frequency: " << aPulseRepetitionFrequencies[iprf] << " Hz";
            }

            double response =
               mMTI_Filters[iprf]->mti_response(aTgtBearing, aTgtDopplerFrequency, aTypeFlag, aTgtSlantRange);
            mti_response = std::max(mti_response, response);
            if (mDebug)
            {
               prfNote.AddNote() << "MTI Response: " << response;
            }

            response    = mPD_Filter->pd_response(aTgtDopplerFrequency, aTypeFlag, iprf);
            pd_response = std::max(pd_response, response);

            if (mDebug)
            {
               prfNote.AddNote() << "PD Response: " << response;
            }
         }

         if (mDebug)
         {
            logger.AddNote() << "MTI Response: " << mti_response;
            logger.AddNote() << "PD  Response: " << pd_response;
         }

         // TODO will need to do something different for PSD stuff.
         //  Limit the minimum MTD response
         MTD_Response = std::max(mMTD_MinResponse, mti_response * pd_response);
      }
   }
   else // if (mMTD_Type == cMTDT_DATA_TABLE)
   {
      auto logger = ut::log::debug();
      if (mDebug)
      {
         logger << "Computed MTD result for type: " << aTypeFlag;
         logger.AddNote() << "Minimum MTD Response: " << mMTD_MinResponse;
         logger.AddNote() << "Target Bearing: " << aTgtBearing * UtMath::cDEG_PER_RAD << " deg";
         logger.AddNote() << "Slant Range: " << aTgtSlantRange / 1000.0 << " km";
         logger.AddNote() << "Doppler Frequency: " << aTgtDopplerFrequency << " Hz";
      }

      // Get MTD response value as an absolute ratio from table for Doppler frequency
      // and limit the minimum MTD response
      MTD_Response = std::max(mMTD_MinResponse, GetTableResponse(aTgtDopplerFrequency));
   }

   if (mDebug)
   {
      ut::log::debug() << "Final MTD Response: " << MTD_Response;
   }

   return MTD_Response;
}

// ================================================================================================
// Looks up the MTD response based on Doppler frequency value from a table.
// @param aTargetDopplerFrequency  The target Doppler frequency response the target
// @return Returns a double value of the MTD Response. Returns 1.0 if the table is not valid.
// private
double WsfRadarMTD_SignalProcessor::GetTableResponse(const double aTargetDopplerFrequency)
{
   double response = 1.0;
   if (mResponseTable.IsDefined())
   {
      response = mResponseTable.Lookup(aTargetDopplerFrequency);
      if (mDebug)
      {
         auto logger = ut::log::debug() << "MTD response table lookup:";
         logger.AddNote() << "Response: " << response;
         logger.AddNote() << "Target Doppler Frequency: " << aTargetDopplerFrequency << " Hz.";
      }
   }
   return response;
}
