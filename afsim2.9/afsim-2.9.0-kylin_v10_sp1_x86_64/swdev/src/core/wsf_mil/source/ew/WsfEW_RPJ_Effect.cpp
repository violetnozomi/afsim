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

#include "WsfEW_RPJ_Effect.hpp"

#include <cassert>
#include <cfloat>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Result.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfEW_RPJ_Effect::WsfEW_RPJ_Effect()
   : WsfEW_PulseEffect()
   , mCoherentRatio(0.0)
   , mRPJ_DataMap()
   , mRPJ_Gain(0.0)
   , mPulsesInSamples(0)
   , mPulseDensity(0.0)
   , mJammerToNoise(0.0)
   , mPulseRangesVec()
{
   mCoherencyMask = cEC_NONCOHERENT_PULSE;
}


WsfEW_RPJ_Effect::WsfEW_RPJ_Effect(const WsfEW_RPJ_Effect& aSrc)
   : WsfEW_PulseEffect(aSrc)
   , mCoherentRatio(aSrc.mCoherentRatio)
   , mRPJ_DataMap()
   , mRPJ_Gain(aSrc.mRPJ_Gain)
   , mPulsesInSamples(aSrc.mPulsesInSamples)
   , mPulseDensity(aSrc.mPulseDensity)
   , mJammerToNoise(aSrc.mJammerToNoise)
   , mPulseRangesVec(aSrc.mPulseRangesVec)
{
}

// virtual
WsfEW_Effect* WsfEW_RPJ_Effect::Clone() const
{
   return new WsfEW_RPJ_Effect(*this);
}

bool WsfEW_RPJ_Effect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_PulseEffect::Initialize(aSimulation);
}

bool WsfEW_RPJ_Effect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "coherency_ratio")
   {
      aInput.ReadValue(mCoherentRatio);
      aInput.ValueInClosedRange(mCoherentRatio, 0.0, 1.0);
      if (mCoherentRatio > 0.0)
      {
         mCoherencyMask = cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
      }
      else
      {
         mCoherencyMask = cEC_NONCOHERENT_PULSE;
      }
   }
   else
   {
      myCommand = WsfEW_PulseEffect::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfEW_RPJ_Effect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_PULSE_EFFECT;
   WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);

   aEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain    = mRPJ_Gain * (1 - mCoherentRatio);
   aEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain = mRPJ_Gain * mCoherentRatio;

   if (mDebug)
   {
      auto out = ut::log::debug() << "RPJ gain and pulse density with number of pulses report.";
      out.AddNote() << "RPJ Gain Factor: " << mRPJ_Gain;
      out.AddNote() << "RPJ Pulse Density: " << GetJammingPulseDensity();
      out.AddNote() << "Number Pulses in Sample: " << mPulsesInSamples;
   }
}

// virtual
void WsfEW_RPJ_Effect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PulseEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_RPJ_Effect::UpdateEffects(double                                 aSimTime,
                                     const WsfEM_Interaction&               aTargetInteraction,
                                     const WsfEM_Interaction&               aJammerToTgtInteraction,
                                     const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                     WsfEW_Effects&                         aEW_Effects,
                                     WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEW_PulseEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);
   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   mJammerToNoise            = 0.0;
   mRPJ_Gain                 = 0.0;
   mPulsesInSamples          = 0;
   mPulseRangesVec.clear();

   if ((aTargetInteraction.mFailedStatus != 0) || (aTargetInteraction.GetTarget() == nullptr))
   {
      return;
   }

   WsfEM_Rcvr* radarRcvrPtr = aTargetInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);
   WsfEM_Xmtr* radarXmtrPtr = aTargetInteraction.GetTransmitter();
   assert(radarXmtrPtr != nullptr);

   // Get the jammer power and EW_Effects between the receiver and jammer transmitter
   WsfEM_Interaction jammerInteraction;
   double            jammingPower =
      WsfRF_Jammer::ComputeJammerPower(aSimTime, jammerXmtrPtr, aTargetInteraction, jammerInteraction, this);

   WsfEW_Result* ewJammerInteraction = WsfEW_Result::Find(jammerInteraction);
   jammingPower *= // ewJammerInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *       // only for coherent
                   // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mBlanking *         // applied later
                   // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mCancellation *     // applied later
                   // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain * // already applied
      ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
      ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
      ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
      // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *     // applied below
      // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *   // already applied
      // ewJammerInteraction->mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain *         // N/A
      GetRepeaterFactor() * GetJammingPowerGain(mEffectedSystemId) *
      GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT_PULSE) * GetModulationGain(mEffectedSystemId) *
      GetSignalPowerGain(mEffectedSystemId);
   // * GetRadiusFactor(mEffectedSystemId);                                  // applied later
   // double jammingPower = WsfRF_Jammer::ComputeJammerPower(aSimTime, aXmtrPtr, aTargetInteraction, this)
   if (jammingPower <= 0.0)
   {
      return;
   }

   mJammerToNoise = radarRcvrPtr->ComputeSignalToNoise(jammingPower, aTargetInteraction.mClutterPower, 0.0);

   if (mJammerToNoise < radarRcvrPtr->GetDetectionThreshold())
   {
      return;
   }

   RPJ_Data& radarData = dynamic_cast<RPJ_Data&>(GetEffectData(mEffectedSystemId));

   if ((mJammerToNoise >= radarData.mMinJNR_Threshold) && (GetJammingPulseDensity() > 0.0))
   {
      double pulseWidth = radarXmtrPtr->GetPulseWidth();
      double pri        = radarXmtrPtr->GetPulseRepetitionInterval();

      if (mDebug)
      {
         if ((pulseWidth <= 0.0) || (pri <= 0.0))
         {
            auto out = ut::log::warning() << "WSF_RPJ_EFFECT- pulsewidth and/or PRI not specified on radar.";
            out.AddNote() << "Radar: " << radarRcvrPtr->GetArticulatedPart()->GetName();
         }
      }

      double binRange     = UtMath::cLIGHT_SPEED * pulseWidth / 2;
      double maxPRI_Range = UtMath::cLIGHT_SPEED * pri / 2;
      // int numBins = static_cast<int>(maxPRI_Range / binRange);
      // int numBins = static_cast<int>(pri / pulseWidth);
      int targetBin = static_cast<int>(aTargetInteraction.mRcvrToTgt.mRange / binRange);

      if (jammerXmtrPtr->GetPulseWidth() > 0.0)
      {
         pulseWidth = jammerXmtrPtr->GetPulseWidth();
      }
      double tauAverage = pulseWidth / GetJammingPulseDensity();

      int samplesPerDetection = radarData.mSamplesPerDetection;
      if (samplesPerDetection == 0)
      {
         samplesPerDetection = 1; // In case any of the below fails, guarantee at least a single pulse sample.
         WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(radarRcvrPtr->GetMode());
         const WsfSensorResult*     sensorResult = dynamic_cast<const WsfSensorResult*>(&aTargetInteraction);
         if ((radarModePtr != nullptr) && (sensorResult != nullptr))
         {
            WsfRadarSensor::RadarBeam* radarBeamPtr = radarModePtr->mBeamList[sensorResult->mBeamIndex];
            if ((radarBeamPtr != nullptr) && radarBeamPtr->UsingDetector())
            {
               samplesPerDetection = radarBeamPtr->GetNumberOfPulsesIntegrated();
               ;
            }
            else if (radarXmtrPtr->GetPulseRepetitionFrequency() != 0.0)
            {
               // Pulsed radar...
               double timeOnTarget = 0.0;
               if (radarModePtr->GetDwellTime() == 0.0)
               {
                  if (radarXmtrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
                  {
                     double minEl;
                     double maxEl;
                     radarXmtrPtr->GetAntenna()->GetElevationScanLimits(minEl, maxEl);
                     timeOnTarget =
                        radarModePtr->GetFrameTime() * radarXmtrPtr->GetElevationBeamwidth(0.0, 0.0) / (maxEl - minEl);
                  }
                  else
                  {
                     double minAz;
                     double maxAz;
                     radarXmtrPtr->GetAntenna()->GetAzimuthScanLimits(minAz, maxAz);
                     timeOnTarget =
                        radarModePtr->GetFrameTime() * radarXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0) / (maxAz - minAz);
                  }
               }
               else
               {
                  // Tracker
                  timeOnTarget = radarModePtr->GetDwellTime();
               }
               samplesPerDetection = (int)(timeOnTarget * radarXmtrPtr->GetPulseRepetitionFrequency());
            }
         }
      }

      int    samples = 0;
      double Tpulse  = std::max(pulseWidth, tauAverage * GetRandom().Uniform(0.0, 2.0));
      while (samples < samplesPerDetection)
      {
         if (Tpulse >= (2 * maxPRI_Range / UtMath::cLIGHT_SPEED))
         {
            Tpulse -= (2 * maxPRI_Range / UtMath::cLIGHT_SPEED);
            samples += 1;
         }
         double pulseRange = UtMath::cLIGHT_SPEED * Tpulse / 2;
         int    pulseBin   = static_cast<int>(pulseRange / binRange);
         if (pulseBin == targetBin)
         {
            mPulsesInSamples += 1;
         }

         // Store out for display purposes
         if (samples == 0)
         {
            mPulseRangesVec.push_back(pulseRange);
         }

         Tpulse += std::max(pulseWidth, tauAverage * GetRandom().Uniform(0.75, 1.25));
      }

      mPulseDensity = (static_cast<double>(mPulsesInSamples) / samplesPerDetection);
      mRPJ_Gain     = GetRPJ_Gain(radarData.mJNR_PD_GainMapMap, mJammerToNoise, mPulseDensity);
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_RPJ_Effect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mRPJ_DataMap[aSystemTypeId] = dynamic_cast<RPJ_Data*>(WsfEW_PulseEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mRPJ_DataMap[aSystemTypeId];
}

double WsfEW_RPJ_Effect::GetMinJNR_Threshold(WsfStringId aSystemTypeId) const
{
   auto edi = mRPJ_DataMap.find(aSystemTypeId);
   if (edi == mRPJ_DataMap.end())
   {
      edi = mRPJ_DataMap.find(nullptr);
   }
   return edi->second->mMinJNR_Threshold;
}

WsfEW_RPJ_Effect::JNR_PD_GainMapMap WsfEW_RPJ_Effect::GetJNR_PD_GainMapMap(WsfStringId aSystemTypeId) const
{
   auto edi = mRPJ_DataMap.find(aSystemTypeId);
   if (edi == mRPJ_DataMap.end())
   {
      edi = mRPJ_DataMap.find(nullptr);
   }
   return edi->second->mJNR_PD_GainMapMap;
}

int WsfEW_RPJ_Effect::GetSamplesPerDetection(WsfStringId aSystemTypeId) const
{
   auto edi = mRPJ_DataMap.find(aSystemTypeId);
   if (edi == mRPJ_DataMap.end())
   {
      edi = mRPJ_DataMap.find(nullptr);
   }
   return edi->second->mSamplesPerDetection;
}

double WsfEW_RPJ_Effect::GetRPJ_Gain(JNR_PD_GainMapMap& aJNR_PD_GainMapMap, double aJammerToNoise, double aPulseDensity)
{
   double jammerGain = 0.0;

   auto       jpdgmi     = aJNR_PD_GainMapMap.begin();
   PD_GainMap pd_GainMap = jpdgmi->second;
   for (; jpdgmi != aJNR_PD_GainMapMap.end(); ++jpdgmi)
   {
      if (aJammerToNoise >= jpdgmi->first)
      {
         pd_GainMap = jpdgmi->second;
      }
      else
      {
         break;
      }
   }

   auto pdgmi     = pd_GainMap.begin();
   auto pdgmiGain = pd_GainMap.begin();
   for (; pdgmi != pd_GainMap.end(); ++pdgmi)
   {
      if (aPulseDensity >= pdgmi->first)
      {
         pdgmiGain = pdgmi;
      }
      else
      {
         break;
      }
   }

   jammerGain = pdgmiGain->second;
   if ((pdgmi != pdgmiGain) && (pdgmi != pd_GainMap.end()))
   {
      // jammerGain = UtMath::Lerp(aPulseDensity, pdgmiGain->first, pdgmi->first, pdgmiGain->second, pdgmi->second);
      jammerGain +=
         (pdgmi->second - pdgmiGain->second) * (aPulseDensity - pdgmiGain->first) / (pdgmi->first - pdgmiGain->first);
   }

   return jammerGain;
}

double WsfEW_RPJ_Effect::GetRPJ_Gain(WsfStringId aRadarTypeId, double aJammerToNoise, double aPulseDensity)
{
   auto rdi = mRPJ_DataMap.find(aRadarTypeId);
   if (rdi == mRPJ_DataMap.end())
   {
      rdi = mRPJ_DataMap.find(nullptr);
   }
   return rdi->second->GetRPJ_Gain(aJammerToNoise, aPulseDensity);
}


// Embedded Effect data class
WsfEW_RPJ_Effect::RPJ_Data::RPJ_Data()
   : PulseData()
   , mSamplesPerDetection(0)
   , mMinJNR_Threshold(1.0)
   , mJNR_PD_GainMapMap()
{
   PD_GainMap pd_GainMap;
   pd_GainMap[0.0]                       = 0.0;
   mJNR_PD_GainMapMap[mMinJNR_Threshold] = pd_GainMap;

   SetJammerPulseDensity(0.1, false);
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_RPJ_Effect::RPJ_Data::Clone() const
{
   return new RPJ_Data(*this);
}

// virtual
bool WsfEW_RPJ_Effect::RPJ_Data::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "samples_per_detection")
   {
      int value;
      aInput.ReadValue(value);
      aInput.ValueGreater(value, 0);
      mSamplesPerDetection = value;
   }
   else if (command == "minimum_jammer_to_noise_threshold")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cRATIO);
      aInput.ValueGreater(value, 0.0);
      mMinJNR_Threshold = value;
   }
   else if (command == "jammer_gain_table")
   {
      mJNR_PD_GainMapMap.clear();
      double       jammerToNoiseRatio;
      double       pulseDensity;
      double       jammerGain;
      PD_GainMap   pd_gainMap;
      UtInputBlock inputBlock2(aInput, "end_jammer_gain_table");
      while (inputBlock2.ReadCommand(command))
      {
         if ((command == "jammer_to_noise_ratio") || (command == "jammer_to_noise"))
         {
            if (!pd_gainMap.empty())
            {
               mJNR_PD_GainMapMap[jammerToNoiseRatio] = pd_gainMap;
               pd_gainMap.clear();
            }
            pd_gainMap[0.0] = 0.0; // set lower threshold in case it is not input
            aInput.ReadValueOfType(jammerToNoiseRatio, UtInput::cRATIO);
            aInput.ValueGreater(jammerToNoiseRatio, 0.0);
         }
         else if ((command == "pulse_density_to_jamming_gain") || (command == "pulse_density"))
         {
            aInput.ReadValue(pulseDensity);
            aInput.ValueGreaterOrEqual(pulseDensity, 0.0);
            aInput.ReadValueOfType(jammerGain, UtInput::cRATIO);
            aInput.ValueGreaterOrEqual(jammerGain, 0.0);
            pd_gainMap[pulseDensity] = jammerGain;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Incorrect jammer gain table input of " + command);
         }
      }
      mJNR_PD_GainMapMap[jammerToNoiseRatio] = pd_gainMap;
   }
   else if (!PulseData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_RPJ_Effect::RPJ_Data::Initialize(WsfSimulation& aSimulation)
{
   return PulseData::Initialize(aSimulation);
}

// virtual
void WsfEW_RPJ_Effect::RPJ_Data::Reset()
{
   mSamplesPerDetection = 0;
   mMinJNR_Threshold    = 1.0;

   mJNR_PD_GainMapMap.clear();
   PD_GainMap pd_GainMap;
   pd_GainMap[0.0]                       = 0.0;
   mJNR_PD_GainMapMap[mMinJNR_Threshold] = pd_GainMap;

   PulseData::Reset();
   SetJammerPulseDensity(0.1, false);
}

double WsfEW_RPJ_Effect::RPJ_Data::GetRPJ_Gain(double aJammerToNoise, double aPulseDensity) const
{
   double jammerGain = 0.0;

   auto       jpdgmi     = mJNR_PD_GainMapMap.begin();
   PD_GainMap pd_GainMap = jpdgmi->second;
   for (; jpdgmi != mJNR_PD_GainMapMap.end(); ++jpdgmi)
   {
      if (aJammerToNoise >= jpdgmi->first)
      {
         pd_GainMap = jpdgmi->second;
      }
      else
      {
         break;
      }
   }

   auto pdgmi     = pd_GainMap.begin();
   auto pdgmiGain = pd_GainMap.begin();
   for (; pdgmi != pd_GainMap.end(); ++pdgmi)
   {
      if (aPulseDensity >= pdgmi->first)
      {
         pdgmiGain = pdgmi;
      }
      else
      {
         break;
      }
   }

   jammerGain = pdgmiGain->second;
   if ((pdgmi != pdgmiGain) && (pdgmi != pd_GainMap.end()))
   {
      jammerGain +=
         (pdgmi->second - pdgmiGain->second) * (aPulseDensity - pdgmiGain->first) / (pdgmi->first - pdgmiGain->first);
   }

   return jammerGain;
}
