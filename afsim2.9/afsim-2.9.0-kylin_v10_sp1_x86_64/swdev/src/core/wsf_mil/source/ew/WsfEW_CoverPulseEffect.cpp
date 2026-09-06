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

#include "WsfEW_CoverPulseEffect.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfEW_CoverPulseEffect::WsfEW_CoverPulseEffect()
   : WsfEW_PulseEffect()
   , mCoverPulseDataMap()
   , mCoverProbability(1.0)
   , mJammingPowerFactor(1.0)
{
   mCoherencyMask = cEC_NONCOHERENT_PULSE;
   mBehavior      = cEB_JAMMER_POWER_EFFECT;
}

WsfEW_CoverPulseEffect::WsfEW_CoverPulseEffect(const WsfEW_CoverPulseEffect& aSrc)
   : WsfEW_PulseEffect(aSrc)
   , mCoverPulseDataMap()
   , mCoverProbability(aSrc.mCoverProbability)
   , mJammingPowerFactor(aSrc.mJammingPowerFactor)
{
}

// virtual
WsfEW_Effect* WsfEW_CoverPulseEffect::Clone() const
{
   return new WsfEW_CoverPulseEffect(*this);
}

// virtual
bool WsfEW_CoverPulseEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_PulseEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_SENSOR | cES_COMM;
   }

   return ok;
}

// virtual
bool WsfEW_CoverPulseEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_PulseEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_CoverPulseEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   if ((mCoverProbability != 1.0) || (fabs(mJammingPowerFactor - 1.0) >= 0.001))
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

      auto out = ut::log::debug();
      if (mDebug)
      {
         out << "Cover pulse effect:";
      }

      if (mCoverProbability != 1.0)
      {
         if (mCoverProbability > GetCoverProbability(mEffectedSystemId))
         {
            aEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *= 0.0;

            if (mDebug)
            {
               out.AddNote() << "Application probability draw: " << mCoverProbability;
               out.AddNote() << "Probability setting: " << GetCoverProbability(mEffectedSystemId);
               out.AddNote() << "Jamming power has been set to 0.0 watts.";
            }
         }
         else if (mDebug)
         {
            out.AddNote() << "Application probability draw: " << mCoverProbability;
            out.AddNote() << "Probability setting: " << GetCoverProbability(mEffectedSystemId);
         }
      }

      if (fabs(mJammingPowerFactor - 1.0) >= 0.001)
      {
         aEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *= mJammingPowerFactor;

         if (mDebug)
         {
            out.AddNote() << "Jamming Power Factor: " << mJammingPowerFactor;
         }
      }
   }

   WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_CoverPulseEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const {}

// virtual
void WsfEW_CoverPulseEffect::UpdateEffects(double                                 aSimTime,
                                           const WsfEM_Interaction&               aTargetInteraction,
                                           const WsfEM_Interaction&               aJammerToTgtInteraction,
                                           const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                           WsfEW_Effects&                         aEW_Effects,
                                           WsfEW_Effect*                          aEW_EffectPtr /* = 0 */)
{
   WsfEW_PulseEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   // These updates only apply to EA
   if (GetEP_Ptr() != nullptr)
   {
      return;
   }

   mCoverProbability   = 1.0;
   mJammingPowerFactor = 1.0;

   WsfEM_Interaction jammerInteraction;
   double            jammerPower = WsfRF_Jammer::ComputeJammerPower(aSimTime,
                                                         aJammerToTgtInteraction.GetTransmitter(),
                                                         aTargetInteraction,
                                                         jammerInteraction,
                                                         this);

   WsfEW_Result* ewInteraction = WsfEW_Result::Find(jammerInteraction);

   jammerPower *= ewInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                  // ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *  // already applied
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                  // jammerInteraction.mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *    // already applied
                  ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain * GetRepeaterFactor() *
                  GetJammingPowerGain(mEffectedSystemId) * GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT_PULSE) *
                  GetModulationGain(mEffectedSystemId) * GetSignalPowerGain(mEffectedSystemId) *
                  GetRadiusFactor(mEffectedSystemId);
   if ((aTargetInteraction.mRcvdPower <= 0.0) ||
       (jammerPower / aTargetInteraction.mRcvdPower <= GetRequiredJtoS(mEffectedSystemId)))
   {
      mJammingPowerFactor = 0.0;
      if (mDebug)
      {
         if (aTargetInteraction.mRcvdPower <= 0.0)
         {
            ut::log::debug() << "No signal being detected.";
         }
         else
         {
            ut::log::debug() << "Insufficient J/S.";
         }
      }
   }
   else if (GetCoverProbability(mEffectedSystemId) != 1.0)
   {
      mCoverProbability = GetRandom().Uniform<double>();
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_CoverPulseEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mCoverPulseDataMap[aSystemTypeId] =
      dynamic_cast<CoverPulseData*>(WsfEW_PulseEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mCoverPulseDataMap[aSystemTypeId];
}

// virtual
void WsfEW_CoverPulseEffect::UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr)
{
   assert(mEffectedSystemId != 0);

   // Set the jamming pulse density for possible use by EP techniques.
   if (!ExplicitJPD(mEffectedSystemId))
   {
      double jammerPulseDensity(GetJammerPulseDensity(mEffectedSystemId));
      if ((aJammerXmtrPtr->GetPulseWidth() > 0.0) && (aJammerXmtrPtr->GetPulseRepetitionInterval() > 0.0))
      {
         jammerPulseDensity = aJammerXmtrPtr->GetPulseWidth() / aJammerXmtrPtr->GetPulseRepetitionInterval();
      }
      else if ((aRadarXmtrPtr != nullptr) && (aRadarXmtrPtr->GetPulseWidth() > 0.0) &&
               (aRadarXmtrPtr->GetPulseRepetitionInterval() > 0.0))
      {
         jammerPulseDensity = aRadarXmtrPtr->GetPulseWidth() / aRadarXmtrPtr->GetPulseRepetitionInterval();
      }

      // Need to clone the effect data from the default data if data does not exist when updating the JammerPulseDensity.
      if (!EffectDataExists(mEffectedSystemId))
      {
         InsertNewEffectData(mEffectedSystemId, mEffectDataMap[nullptr]->Clone());
      }
      SetJammerPulseDensity(mEffectedSystemId, jammerPulseDensity, false);
   }
}

double WsfEW_CoverPulseEffect::GetCoverProbability(WsfStringId aSystemTypeId) const
{
   auto it = mCoverPulseDataMap.find(aSystemTypeId);
   if (it == mCoverPulseDataMap.end())
   {
      it = mCoverPulseDataMap.find(nullptr);
   }
   return it->second->mCoverProbability;
}

double WsfEW_CoverPulseEffect::GetRequiredJtoS(WsfStringId aSystemTypeId) const
{
   auto it = mCoverPulseDataMap.find(aSystemTypeId);
   if (it == mCoverPulseDataMap.end())
   {
      it = mCoverPulseDataMap.find(nullptr);
   }
   return it->second->mRequiredJtoS;
}

// Embedded Effect data class
WsfEW_CoverPulseEffect::CoverPulseData::CoverPulseData()
   : PulseData()
   , mCoverProbability()
   , mRequiredJtoS(UtMath::DB_ToLinear(3.0))
{
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_CoverPulseEffect::CoverPulseData::Clone() const
{
   return new CoverPulseData(*this);
}

// virtual
bool WsfEW_CoverPulseEffect::CoverPulseData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "probability_of_cover")
   {
      double probCover;
      aInput.ReadValue(probCover);
      aInput.ValueInClosedRange(probCover, 0.0, 1.0);
      mCoverProbability = probCover;
   }
   else if (command == "required_j_to_s")
   {
      aInput.ReadValueOfType(mRequiredJtoS, UtInput::cRATIO);
   }
   else if (!PulseData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_CoverPulseEffect::CoverPulseData::Initialize(WsfSimulation& aSimulation)
{
   return PulseData::Initialize(aSimulation);
}

// virtual
void WsfEW_CoverPulseEffect::CoverPulseData::Reset()
{
   mCoverProbability = 1.0;
   mRequiredJtoS     = UtMath::DB_ToLinear(3.0);
   PulseData::Reset();
}

WsfEW_CoverPulseEffect::EffectData* WsfEW_CoverPulseEffect::NewEffectData()
{
   return new CoverPulseData();
}
