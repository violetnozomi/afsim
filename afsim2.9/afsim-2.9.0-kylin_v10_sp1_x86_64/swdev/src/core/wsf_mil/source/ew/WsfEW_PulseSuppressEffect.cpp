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

#include "WsfEW_PulseSuppressEffect.hpp"

#include <cfloat>
#include <map>
#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtVec2.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_RPJ_Effect.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"

WsfEW_PulseSuppressEffect::WsfEW_PulseSuppressEffect()
   : WsfEW_PulseEffect()
   , mAllowedMap()
   , mRejectMap()
   , mSuppressTypeMask(0)
   , mSuppressGain(1.0)
   , mNoiseSuppressGain(1.0)
   , mCohSuppressGain(1.0)
{
}

// virtual
WsfEW_Effect* WsfEW_PulseSuppressEffect::Clone() const
{
   return new WsfEW_PulseSuppressEffect(*this);
}

bool WsfEW_PulseSuppressEffect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_PulseEffect::Initialize(aSimulation);
}

bool WsfEW_PulseSuppressEffect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "reject") || (command == "allow"))
   {
      std::string  suppressionString;
      unsigned int suppressionType;
      Limits       limits;
      limits.Reset();
      aInput.ReadValue(suppressionString);
      if (suppressionString == "none")
      {
         suppressionType = cST_NONE;
      }
      else if (suppressionString == "frequency")
      {
         suppressionType = cST_FREQUENCY;
         aInput.ReadValueOfType(limits.mLower, UtInput::cFREQUENCY);
         aInput.ValueGreaterOrEqual(limits.mLower, 0.0);
         aInput.ReadValueOfType(limits.mUpper, UtInput::cFREQUENCY);
         aInput.ValueGreaterOrEqual(limits.mUpper, limits.mUpper);
      }
      else if (suppressionString == "pulse_width")
      {
         suppressionType = cST_PW;
         aInput.ReadValueOfType(limits.mLower, UtInput::cTIME);
         aInput.ValueGreaterOrEqual(limits.mLower, 0.0);
         aInput.ReadValueOfType(limits.mUpper, UtInput::cTIME);
         aInput.ValueGreaterOrEqual(limits.mUpper, limits.mUpper);
      }
      else if (suppressionString == "pulse_repitition_frequency")
      {
         suppressionType = cST_PRF;
         aInput.ReadValueOfType(limits.mLower, UtInput::cFREQUENCY);
         aInput.ValueGreaterOrEqual(limits.mLower, 0.0);
         aInput.ReadValueOfType(limits.mUpper, UtInput::cFREQUENCY);
         aInput.ValueGreaterOrEqual(limits.mUpper, limits.mUpper);
      }
      else if (suppressionString == "modulation")
      {
         suppressionType = cST_MODULATION;
         std::string modulationString;
         aInput.ReadValue(modulationString);
         if (modulationString == "none")
         {
            limits.mModulationType = cEM_NONE;
         }
         else if (modulationString == "continuous_wave")
         {
            limits.mModulationType = cEM_CW;
         }
         else if (modulationString == "cw")
         {
            limits.mModulationType = cEM_CW;
         }
         else if (modulationString == "coherent_pulse")
         {
            limits.mModulationType = cEM_COHPULSE;
         }
         else if (modulationString == "non_coherent_pulse")
         {
            limits.mModulationType = cEM_NONCOHPULSE;
         }
         else if (modulationString == "linear_fm")
         {
            limits.mModulationType = cEM_LINEARFM;
         }
         else if (modulationString == "non_linear_fm")
         {
            limits.mModulationType = cEM_NONLINEARFM;
         }
         else if (modulationString == "phase_key")
         {
            limits.mModulationType = cEM_PHASEKEY;
         }
         else if (modulationString == "phase_modulation")
         {
            limits.mModulationType = cEM_PHASEMOD;
         }
         else if (modulationString == "phase_coding")
         {
            limits.mModulationType = cEM_PHASECODE;
         }
         else if (modulationString == "ask")
         {
            limits.mModulationType = cEM_ASK;
         }
         else if (modulationString == "fsk")
         {
            limits.mModulationType = cEM_FSK;
         }
         else if (modulationString == "psk")
         {
            limits.mModulationType = cEM_PSK;
         }
         else if (modulationString == "bpsk")
         {
            limits.mModulationType = cEM_BPSK;
         }
         else if (modulationString == "dpsk")
         {
            limits.mModulationType = cEM_DPSK;
         }
         else if (modulationString == "qpsk")
         {
            limits.mModulationType = cEM_QPSK;
         }
         else if (modulationString == "qam")
         {
            limits.mModulationType = cEM_QAM;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Pulse suppression modulation type " + modulationString + " not recognized.");
         }
         aInput.ReadValue(limits.mLower);
         aInput.ValueInClosedRange(limits.mLower, 0.0, 1.0);
      }
      else if (suppressionString == "coherent")
      {
         suppressionType = cST_COHERENT;
         aInput.ReadValue(limits.mLower);
         aInput.ValueInClosedRange(limits.mLower, 0.0, 1.0);
      }
      else if (suppressionString == "noncoherent")
      {
         suppressionType = cST_NONCOHERENT;
         aInput.ReadValue(limits.mLower);
         aInput.ValueInClosedRange(limits.mLower, 0.0, 1.0);
      }
      else if ((suppressionString == "percent_of_pulses") || (suppressionString == "pulses"))
      {
         suppressionType = cST_PULSE_RATIO;
         aInput.ReadValue(limits.mLower);
         aInput.ValueInClosedRange(limits.mLower, 0.0, 1.0);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Pulse suppression type " + suppressionString + " not recognized");
      }

      mSuppressTypeMask |= suppressionType;

      if (command == "allow")
      {
         mAllowedMap[suppressionType] = limits;
      }
      else if (command == "reject")
      {
         mRejectMap[suppressionType] = limits;
      }
   }
   else
   {
      myCommand = WsfEW_PulseEffect::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfEW_PulseSuppressEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_PulseSuppressEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   auto out = ut::log::debug();
   if (mDebug)
   {
      out << "Platform being jammed when attempting to detect target.";
      out.AddNote() << "Platform: " << mCurrentRcvrPlatformId;
      out.AddNote() << "Jammed by: " << mCurrentXmtrPlatformId;
      out.AddNote() << "Target: " << mCurrentTargetId;
   }

   aEW_Effects.mMask |= cEB_PULSE_EFFECT;
   WsfEW_PulseEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);

   if ((aEW_EffectPtr->GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_PulseJammingEffect.mPulseSuppression = mSuppressGain * mNoiseSuppressGain;

      if (mDebug)
      {
         out.AddNote() << "Noise pulse suppression factor: " << mSuppressGain * mNoiseSuppressGain;
      }
   }

   if ((aEW_EffectPtr->GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression = mSuppressGain * mCohSuppressGain;
      if (mDebug)
      {
         out.AddNote() << "Coherent pulse suppression factor: " << mSuppressGain * mCohSuppressGain;
      }
   }
}

// virtual
void WsfEW_PulseSuppressEffect::UpdateEffects(double                                 aSimTime,
                                              const WsfEM_Interaction&               aTargetInteraction,
                                              const WsfEM_Interaction&               aJammerToTgtInteraction,
                                              const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                              WsfEW_Effects&                         aEW_Effects,
                                              WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Xmtr* radarXmtrPtr  = aTargetInteraction.GetTransmitter();
   assert(radarXmtrPtr != nullptr);

   WsfEW_PulseEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   unsigned int checkMask = 0;

   mSuppressGain      = 1.0;
   mNoiseSuppressGain = 1.0;
   mCohSuppressGain   = 1.0;

   if (aTargetInteraction.mFailedStatus != 0)
   {
      return;
   }

   if (aEW_EffectPtr != nullptr)
   {
      // Effect type specific applied effects (i.e. dependent upon the effect type to mitigate)
      if (aEW_EffectPtr->GetType() == "WSF_RPJ_EFFECT")
      {
         WsfEW_RPJ_Effect* effectPtr = dynamic_cast<WsfEW_RPJ_Effect*>(aEW_EffectPtr);

         if ((mSuppressTypeMask & cST_PRF) == cST_PRF)
         {
            checkMask |= cST_PRF;

            double pw = radarXmtrPtr->GetPulseWidth();
            if (pw > 0.0)
            {
               double averagePRI =
                  pw / dynamic_cast<WsfEW_RPJ_Effect*>(aEW_EffectPtr)->GetJammerPulseDensity(mEffectingSystemId);
               mSuppressGain *= GetSuppressionValue(cST_PRF, averagePRI);
            }
         }

         if (((mSuppressTypeMask & cST_PULSE_RATIO) == cST_PULSE_RATIO) &&
             ((checkMask & cST_PULSE_RATIO) != cST_PULSE_RATIO))
         {
            checkMask |= cST_PULSE_RATIO;
            double suppressValue = 1.0;
            suppressValue        = std::min(suppressValue, GetSuppressionValue(cST_PULSE_RATIO, 0.0));
            double oldGain       = effectPtr->GetRPJ_Gain();
            if ((oldGain > 0.0) && ((suppressValue < 1.0) && (suppressValue >= 0.0)) &&
                (effectPtr->GetJammerToNoise() > 0.0))
            {
               effectPtr->SetPulseDensityFactor(suppressValue);
               effectPtr->UpdateEffects(aSimTime,
                                        aTargetInteraction,
                                        aJammerToTgtInteraction,
                                        aTgtToJammerInteraction,
                                        aEW_Effects,
                                        aEW_EffectPtr);
               mSuppressGain *= effectPtr->GetRPJ_Gain() / oldGain;
            }
         }
      }
      else if (aEW_EffectPtr->GetType() == "WSF_FALSE_TARGET_EFFECT")
      {
         // For false-targets each blip calls the UpdateEffects routine here and can be treated
         // individually. The best way to handle the suppression of false-targets is to use the
         // pulse suppression to mean false-target suppression instead. So lets do a uniform
         // random number draw and see if the draw is less than the set value and suppress the
         // blip if it is.
         WsfEW_PulseEffect* effectPtr = dynamic_cast<WsfEW_PulseEffect*>(aEW_EffectPtr);

         if (((mSuppressTypeMask & cST_PULSE_RATIO) == cST_PULSE_RATIO) &&
             ((checkMask & cST_PULSE_RATIO) != cST_PULSE_RATIO))
         {
            checkMask |= cST_PULSE_RATIO;
            double suppressValue = 1.0;
            suppressValue        = std::min(suppressValue, GetSuppressionValue(cST_PULSE_RATIO, 0.0));
            if ((suppressValue < 1.0) && (suppressValue >= 0.0))
            {
               effectPtr->SetPulseDensityFactor(suppressValue);
               effectPtr->UpdateEffects(aSimTime,
                                        aTargetInteraction,
                                        aJammerToTgtInteraction,
                                        aTgtToJammerInteraction,
                                        aEW_Effects,
                                        aEW_EffectPtr);
               if (GetRandom().Bernoulli(1.0 - suppressValue))
               {
                  mSuppressGain *= 0.0;
               }
            }
         }
      }
   }

   // Applied on effects if they were not applied above and are valid
   /// The checkMask is evaluated so the applied effect is not applied repeatedly.
   if (((mSuppressTypeMask & cST_NONE) == cST_NONE) && ((checkMask & cST_NONE) != cST_NONE))
   {
      checkMask |= cST_NONE;

      mSuppressGain *= 1.0;
   }

   if (((mSuppressTypeMask & cST_FREQUENCY) == cST_FREQUENCY) && ((checkMask & cST_FREQUENCY) != cST_FREQUENCY))
   {
      checkMask |= cST_FREQUENCY;

      mSuppressGain *= GetSuppressionValue(cST_FREQUENCY, jammerXmtrPtr->GetFrequency());
   }

   if (((mSuppressTypeMask & cST_PW) == cST_PW) && ((checkMask & cST_PW) != cST_PW))
   {
      checkMask |= cST_PW;

      if (jammerXmtrPtr->GetPulseWidth() > 0.0)
      {
         mSuppressGain *= GetSuppressionValue(cST_PW, jammerXmtrPtr->GetPulseWidth());
      }
   }

   if (((mSuppressTypeMask & cST_PRF) == cST_PRF) && ((checkMask & cST_PRF) != cST_PRF))
   {
      checkMask |= cST_PRF;

      if (jammerXmtrPtr->GetPulseRepetitionFrequency() > 0.0)
      {
         mSuppressGain *= GetSuppressionValue(cST_PW, jammerXmtrPtr->GetPulseRepetitionFrequency());
      }
   }

   if (((mSuppressTypeMask & cST_MODULATION) == cST_MODULATION) && ((checkMask & cST_MODULATION) != cST_MODULATION))
   {
      checkMask |= cST_MODULATION;

      mSuppressGain *= GetSuppressionValue(cST_MODULATION, 0.0);
   }

   if (((mSuppressTypeMask & cST_COHERENT) == cST_COHERENT) && ((checkMask & cST_COHERENT) != cST_COHERENT))
   {
      checkMask |= cST_COHERENT;

      mCohSuppressGain *= GetSuppressionValue(cST_COHERENT, 0.0);
   }

   if (((mSuppressTypeMask & cST_NONCOHERENT) == cST_NONCOHERENT) && ((checkMask & cST_NONCOHERENT) != cST_NONCOHERENT))
   {
      checkMask |= cST_NONCOHERENT;

      mNoiseSuppressGain *= GetSuppressionValue(cST_NONCOHERENT, 0.0);
   }

   // The below needs applied on a per effect basis.
   // if (((mSuppressTypeMask & cST_PULSE_RATIO) == cST_PULSE_RATIO) &&
   //    ((checkMask & cST_PULSE_RATIO) != cST_PULSE_RATIO))
   //{
   //   checkMask |= cST_PULSE_RATIO;

   //   mSuppressValue *= GetSuppressionValue(cST_PULSE_RATIO, 0.0);
   //}
}

double WsfEW_PulseSuppressEffect::GetSuppressionValue(SuppressionType aSuppressType, double aCheckValue)
{
   double suppressValue = 1.0;

   auto ami = mAllowedMap.find(aSuppressType);
   if (ami != mAllowedMap.end())
   {
      if ((*ami).second.mUpper > 0.0)
      {
         if ((aCheckValue < (*ami).second.mLower) || (aCheckValue > (*ami).second.mUpper))
         {
            suppressValue *= 0.0;
         }
      }
      else
      {
         suppressValue *= (*ami).second.mLower;
      }
   }

   auto rmi = mRejectMap.find(aSuppressType);
   if (rmi != mRejectMap.end())
   {
      if ((*rmi).second.mUpper > 0.0)
      {
         if ((aCheckValue >= (*rmi).second.mLower) && (aCheckValue <= (*rmi).second.mUpper))
         {
            suppressValue *= 0.0;
         }
      }
      else
      {
         suppressValue *= 1.0 - (*rmi).second.mLower;
      }
   }
   return suppressValue;
}

double WsfEW_PulseSuppressEffect::GetSuppressionValue(SuppressionType aSuppressType, ModulationType aModType)
{
   double suppressValue = 1.0;

   auto ami = mAllowedMap.find(cST_MODULATION);
   if (ami != mAllowedMap.end())
   {
      if (aModType == (*ami).second.mModulationType)
      {
         suppressValue *= (*ami).second.mLower;
      }
   }

   auto rmi = mRejectMap.find(cST_MODULATION);
   if (rmi != mRejectMap.end())
   {
      if (aModType == (*rmi).second.mModulationType)
      {
         suppressValue *= 1.0 - (*rmi).second.mLower;
      }
   }
   return suppressValue;
}
