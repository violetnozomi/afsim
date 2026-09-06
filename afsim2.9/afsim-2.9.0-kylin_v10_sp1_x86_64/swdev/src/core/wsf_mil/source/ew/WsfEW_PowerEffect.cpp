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

#include "WsfEW_PowerEffect.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfStringId.hpp"

WsfEW_PowerEffect::WsfEW_PowerEffect()
   : WsfEW_RepeaterEffect()
   , mPowerDataMap()
{
   mCoherencyMask = cEC_NONE;
   mBehavior      = cEB_JAMMER_POWER_EFFECT;
   UtVec3d::Set(mJammerToSignal, 0.0);
}

WsfEW_PowerEffect::WsfEW_PowerEffect(const WsfEW_PowerEffect& aSrc)
   : WsfEW_RepeaterEffect(aSrc)
   , mPowerDataMap()
{
   UtVec3d::Set(mJammerToSignal, aSrc.mJammerToSignal);
}

// virtual
WsfEW_Effect* WsfEW_PowerEffect::Clone() const
{
   return new WsfEW_PowerEffect(*this);
}

// virtual
bool WsfEW_PowerEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_RepeaterEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_SENSOR | cES_COMM;
   }

   return ok;
}

// virtual
bool WsfEW_PowerEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_RepeaterEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_PowerEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   double jammingPowerGain = GetJammingPowerGain(mEffectedSystemId);

   // If any notes are added to out, it will be labeled with "Power Effect".
   // Otherwise, no label will be added and out will not be sent.
   auto                       out = ut::log::debug();
   ut::log::DataMessageHelper nl(out, "Power Effect:");

   if ((fabs(jammingPowerGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;
      auto logNotePower = out.AddNote();
      if (mDebug)
      {
         logNotePower << "Power Gain:";
      }

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         aEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            logNotePower.AddNote() << "Coherent delta gain factor: " << jammingPowerGain;
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         aEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            logNotePower.AddNote() << "Non-coherent delta gain factor: " << jammingPowerGain;
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         aEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            logNotePower.AddNote() << "Non-coherent pulse gain factor: " << jammingPowerGain;
         }
      }
   }
   double modulationGain = GetModulationGain(mEffectedSystemId);
   if ((fabs(modulationGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

      auto gainNote = out.AddNote();
      if (mDebug)
      {
         gainNote << "Processing Gain:";
      }

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         aEW_Effects.mEW_CoherentJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Coherent delta gain factor: " << modulationGain;
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         aEW_Effects.mEW_NoiseJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Non-coherent delta gain factor: " << modulationGain;
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         aEW_Effects.mEW_PulseJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Non-coherent pulse gain factor: " << modulationGain;
         }
      }
   }

   double signalPowerGain = GetSignalPowerGain(mEffectedSystemId);
   if ((fabs(signalPowerGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_SIGNAL_POWER_EFFECT;

      if (mDebug)
      {
         out.AddNote() << "Signal delta gain factor: " << signalPowerGain;
      }
      aEW_Effects.mEW_SignalEffect.mSignalPowerGain *= signalPowerGain;
   }

   double rcvrNoiseGain = GetRcvrNoiseGain(mEffectedSystemId);
   if ((fabs(rcvrNoiseGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_SIGNAL_POWER_EFFECT;

      if (mDebug)
      {
         out.AddNote() << "Rcvr noise delta gain factor: " << rcvrNoiseGain;
      }
      aEW_Effects.mEW_SignalEffect.mRcvrNoiseGain *= rcvrNoiseGain;
   }

   if (GetJtoSTableSize(mEffectedSystemId) >= 2)
   {
      double nonCohJtoSGain      = GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT);
      double nonCohPulseJtoSGain = GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT_PULSE);
      double cohJtoSGain         = GetJtoSGain(mEffectedSystemId, cEC_COHERENT);
      if ((fabs(nonCohJtoSGain  - 1.0) >= 0.001) ||
          (fabs(nonCohPulseJtoSGain - 1.0) >= 0.001) ||
          (fabs(cohJtoSGain - 1.0) >= 0.001)/* ||
          (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
      {
         aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

         auto gainNote = out.AddNote();
         if (mDebug)
         {
            gainNote << "Jammer to Signal Gain:";
         }

         if ((fabs(cohJtoSGain - 1.0) > 0.001) &&
             (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u)))
         {
            aEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *= cohJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Coherent J/S gain factor: " << cohJtoSGain;
            }
         }

         if ((fabs(nonCohJtoSGain - 1.0) > 0.001) &&
             (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u)))
         {
            aEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *= nonCohJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Non-coherent J/S gain factor: " << nonCohJtoSGain;
            }
         }

         if ((fabs(nonCohPulseJtoSGain - 1.0) > 0.001) && ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u))
         {
            aEW_Effects.mEW_PulseJammingEffect.mJtoXGain *= nonCohPulseJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Non-coherent J/S gain factor: " << nonCohPulseJtoSGain;
            }
         }
      }
   }

   WsfEW_RepeaterEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_PowerEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   auto out = ut::log::debug();
   if (mDebug)
   {
      out << "WsfEW_PowerEffect::ApplyMitigatedEffect():";
   }
   double jammingPowerGain = GetJammingPowerGain(mEffectedSystemId);
   if ((fabs(jammingPowerGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      auto powerNote = out.AddNote();
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;
      if (mDebug)
      {
         powerNote << "Power Gain:";
      }

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         aEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            powerNote.AddNote() << "Coherent delta gain factor: " << jammingPowerGain;
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         aEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            powerNote.AddNote() << "Non-coherent delta gain factor: " << jammingPowerGain;
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         aEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *= jammingPowerGain;
         if (mDebug)
         {
            powerNote.AddNote() << "Non-coherent pulse gain factor: " << jammingPowerGain;
         }
      }
   }
   double modulationGain = GetModulationGain(mEffectedSystemId);
   if ((fabs(modulationGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

      auto gainNote = out.AddNote();
      if (mDebug)
      {
         gainNote << "Processing gain:";
      }

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         aEW_Effects.mEW_CoherentJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Coherent delta gain factor: " << modulationGain;
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         aEW_Effects.mEW_NoiseJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Non-coherent delta gain factor: " << modulationGain;
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         aEW_Effects.mEW_PulseJammingEffect.mModulationGain *= modulationGain;
         if (mDebug)
         {
            gainNote.AddNote() << "Non-coherent pulse gain factor: " << modulationGain;
         }
      }
   }

   double signalPowerGain = GetSignalPowerGain(mEffectedSystemId);
   if ((fabs(signalPowerGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_SIGNAL_POWER_EFFECT;

      if (mDebug)
      {
         out.AddNote() << "Signal delta gain factor: " << signalPowerGain;
      }
      aEW_Effects.mEW_SignalEffect.mSignalPowerGain *= signalPowerGain;
   }

   double rcvrNoiseGain = GetRcvrNoiseGain(mEffectedSystemId);
   if ((fabs(rcvrNoiseGain - 1.0) >= 0.00001) /* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   {
      aEW_Effects.mMask |= cEB_SIGNAL_POWER_EFFECT;

      if (mDebug)
      {
         out.AddNote() << "Rcvr noise delta gain factor: " << rcvrNoiseGain;
      }
      aEW_Effects.mEW_SignalEffect.mRcvrNoiseGain *= rcvrNoiseGain;
   }

   if (GetJtoSTableSize(mEffectedSystemId) >= 2)
   {
      double nonCohJtoSGain      = GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT);
      double nonCohPulseJtoSGain = GetJtoSGain(mEffectedSystemId, cEC_NONCOHERENT_PULSE);
      double cohJtoSGain         = GetJtoSGain(mEffectedSystemId, cEC_COHERENT);
      if ((fabs(nonCohJtoSGain  - 1.0) >= 0.001) ||
          (fabs(nonCohPulseJtoSGain - 1.0) >= 0.001) ||
          (fabs(cohJtoSGain - 1.0) >= 0.001)/* ||
          (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
      {
         aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

         auto gainNote = out.AddNote();
         if (mDebug)
         {
            gainNote << "Jammer to Signal Gain:";
         }

         if ((fabs(cohJtoSGain - 1.0) > 0.001) &&
             (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u)))
         {
            aEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *= cohJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Coherent J/S gain factor: " << cohJtoSGain;
            }
         }

         if ((fabs(nonCohJtoSGain - 1.0) > 0.001) &&
             (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u)))
         {
            aEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *= nonCohJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Non-coherent J/S gain factor: " << nonCohJtoSGain;
            }
         }

         if ((fabs(nonCohPulseJtoSGain - 1.0) > 0.001) && ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u))
         {
            aEW_Effects.mEW_PulseJammingEffect.mJtoXGain *= nonCohPulseJtoSGain;
            if (mDebug)
            {
               gainNote.AddNote() << "Non-coherent J/S gain factor: " << nonCohPulseJtoSGain;
            }
         }
      }
   }

   WsfEW_RepeaterEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_PowerEffect::UpdateEffects(double                                 aSimTime,
                                      const WsfEM_Interaction&               aTargetInteraction,
                                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                      WsfEW_Effects&                         aEW_Effects,
                                      WsfEW_Effect*                          aEW_EffectPtr /* = 0 */)
{
   WsfEW_RepeaterEffect::UpdateEffects(aSimTime,
                                       aTargetInteraction,
                                       aJammerToTgtInteraction,
                                       aTgtToJammerInteraction,
                                       aEW_Effects,
                                       aEW_EffectPtr);

   UtVec3d::Set(mJammerToSignal, 0.0);

   if (aTargetInteraction.mRcvdPower <= 0.0)
   {
      return;
   }

   if (GetJtoSTableSize(mEffectedSystemId) < 2)
   {
      return;
   }

   WsfEM_Interaction jammerInteraction;
   double            jammerPowers[3];
   WsfRF_Jammer::ComputeJammerPowers(aSimTime,
                                     aJammerToTgtInteraction.GetTransmitter(),
                                     aTargetInteraction,
                                     jammerInteraction,
                                     jammerPowers,
                                     this);

   WsfEW_Result* ewInteraction = WsfEW_Result::Find(jammerInteraction);
   if (((mCoherencyMask & cEC_NONE) != 0u) || ((mCoherencyMask & cEC_NONCOHERENT) != 0u))
   {
      jammerPowers[0] *= ewInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mModulationGain *
                         // ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *  // already applied
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *
                         // jammerInteraction->mEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor *    // already applied
                         ewInteraction->mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain * GetRepeaterFactor() *
                         GetJammingPowerGain(mEffectedSystemId) * GetModulationGain(mEffectedSystemId) *
                         GetSignalPowerGain(mEffectedSystemId);
      // GetRadiusFactor(mEffectedSystemId);                 // applied seperately...maybe
   }

   if ((mCoherencyMask & cEC_NONCOHERENT_PULSE) != 0u)
   {
      jammerPowers[1] *= ewInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mModulationGain *
                         // ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *  // already applied
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *
                         // ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *    // already applied
                         ewInteraction->mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain * GetRepeaterFactor() *
                         GetJammingPowerGain(mEffectedSystemId) * GetModulationGain(mEffectedSystemId) *
                         GetSignalPowerGain(mEffectedSystemId);
      // GetRadiusFactor(mEffectedSystemId);                 // applied seperately...maybe
   }

   if (((mCoherencyMask & cEC_COHERENT) != 0u) || ((mCoherencyMask & cEC_COHERENT_PULSE) != 0u))
   {
      jammerPowers[2] *= ewInteraction->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                         // ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *  // already applied
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
                         // ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *    // already applied
                         ewInteraction->mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain * GetRepeaterFactor() *
                         GetJammingPowerGain(mEffectedSystemId) * GetModulationGain(mEffectedSystemId) *
                         GetSignalPowerGain(mEffectedSystemId);
      // GetRadiusFactor(mEffectedSystemId);                 // applied seperately...maybe
   }

   mJammerToSignal[0] = jammerPowers[0] / aTargetInteraction.mRcvdPower; // Already checked that mRcvdPower > 0.0 above.
   mJammerToSignal[1] = jammerPowers[1] / aTargetInteraction.mRcvdPower; // Already checked that mRcvdPower > 0.0 above.
   mJammerToSignal[2] = jammerPowers[2] / aTargetInteraction.mRcvdPower; // Already checked that mRcvdPower > 0.0 above.
}

// virtual
WsfEW_Effect::EffectData* WsfEW_PowerEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mPowerDataMap[aSystemTypeId] = dynamic_cast<PowerData*>(WsfEW_RepeaterEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mPowerDataMap[aSystemTypeId];
}

double WsfEW_PowerEffect::GetJammingPowerGain(WsfStringId aSystemTypeId) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }
   return edi->second->mJammingPowerGain;
}

void WsfEW_PowerEffect::SetJammingPowerGain(WsfStringId aSystemTypeId, double aJammingPowerGain)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mPowerDataMap[aSystemTypeId]->mJammingPowerGain = aJammingPowerGain;
}

double WsfEW_PowerEffect::GetModulationGain(WsfStringId aSystemTypeId) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }
   return edi->second->mModulationGain;
}

void WsfEW_PowerEffect::SetModulationGain(WsfStringId aSystemTypeId, double aModulationGain)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mPowerDataMap[aSystemTypeId]->mModulationGain = aModulationGain;
}

double WsfEW_PowerEffect::GetSignalPowerGain(WsfStringId aSystemTypeId) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }
   return edi->second->mSignalPowerGain;
}

void WsfEW_PowerEffect::SetSignalPowerGain(WsfStringId aSystemTypeId, double aSignalPowerGain)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mPowerDataMap[aSystemTypeId]->mSignalPowerGain = aSignalPowerGain;
}

double WsfEW_PowerEffect::GetRcvrNoiseGain(WsfStringId aSystemTypeId) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }
   return edi->second->mRcvrNoiseGain;
}

void WsfEW_PowerEffect::SetRcvrNoiseGain(WsfStringId aSystemTypeId, double aRcvrNoiseGain)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mPowerDataMap[aSystemTypeId]->mRcvrNoiseGain = aRcvrNoiseGain;
}

double WsfEW_PowerEffect::GetJtoSGain(WsfStringId aSystemTypeId, Coherency aCoherencyType /* = cEC_NONE*/) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }

   double jammerToSignal = mJammerToSignal[0];
   if (aCoherencyType == cEC_NONCOHERENT_PULSE)
   {
      jammerToSignal = mJammerToSignal[1];
   }
   else if ((aCoherencyType == cEC_COHERENT) || (aCoherencyType == cEC_COHERENT_PULSE))
   {
      jammerToSignal = mJammerToSignal[2];
   }

   if (jammerToSignal > 0.0)
   {
      TblLookupLU<double> jToS;
      jToS.Lookup(edi->second->mJtoSGainTable.mJtoX, jammerToSignal);
      return TblEvaluate(edi->second->mJtoSGainTable.mGain, jToS);
   }
   return 1.0;
}

size_t WsfEW_PowerEffect::GetJtoSTableSize(WsfStringId aSystemTypeId) const
{
   auto edi = mPowerDataMap.find(aSystemTypeId);
   if (edi == mPowerDataMap.end())
   {
      edi = mPowerDataMap.find(nullptr);
   }
   return edi->second->mJtoSGainTable.mJtoX.GetSize();
}

// Embedded Effect data class
WsfEW_PowerEffect::PowerData::PowerData()
   : RepeaterEffectData()
   , mJammingPowerGain(1.0)
   , mModulationGain(1.0)
   , mSignalPowerGain(1.0)
   , mRcvrNoiseGain(1.0)
{
}

// virtual
bool WsfEW_PowerEffect::PowerData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   double gainValue;
   if ((command == "power_gain") || (command == "jamming_power_gain"))
   {
      aInput.ReadValueOfType(gainValue, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(gainValue, 0.0);
      mJammingPowerGain = gainValue;
   }
   else if ((command == "jamming_delta_gain") || // legacy compatibility
            (command == "jamming_modulation_gain") || (command == "modulation_gain"))
   {
      aInput.ReadValueOfType(gainValue, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(gainValue, 0.0);
      mModulationGain = gainValue;
   }
   else if (command == "jamming_to_signal_gain_table")
   {
      // The table is gain vs. j-to-s ratio
      std::vector<double> jToS_Values;
      std::vector<double> gainValues;
      UtInputBlock        inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "jamming_to_signal")
         {
            double jToS;
            aInput.ReadValueOfType(jToS, UtInput::cRATIO);
            aInput.ReadValueOfType(gainValue, UtInput::cRATIO);
            aInput.ValueGreater(jToS, 0.0);
            aInput.ValueGreater(gainValue, 0.0);
            if (!jToS_Values.empty() && (jToS <= jToS_Values.back()))
            {
               throw UtInput::BadValue(aInput, "entries must be in order of increasing j-to-s");
            }
            jToS_Values.push_back(jToS);
            gainValues.push_back(gainValue);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (jToS_Values.size() < 2)
      {
         throw UtInput::BadValue(aInput, "At least two entries must be given");
      }

      mJtoSGainTable.mJtoX.SetValues(jToS_Values);
      mJtoSGainTable.mGain.SetValues(gainValues);
   }
   else if ((command == "signal_delta_gain") || // legacy compatibility
            (command == "signal_power_gain"))
   {
      aInput.ReadValueOfType(gainValue, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(gainValue, 0.0);
      mSignalPowerGain = gainValue;
   }
   else if ((command == "receiver_noise_delta_gain") || // legacy compatibility
            (command == "receiver_noise_power_gain"))
   {
      aInput.ReadValueOfType(gainValue, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(gainValue, 0.0);
      mRcvrNoiseGain = gainValue;
   }
   else if (!RepeaterEffectData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

/// virtual
bool WsfEW_PowerEffect::PowerData::Initialize(WsfSimulation& aSimulation)
{
   return RepeaterEffectData::Initialize(aSimulation);
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_PowerEffect::PowerData::Clone() const
{
   return new PowerData(*this);
}

// virtual
void WsfEW_PowerEffect::PowerData::Reset()
{
   mModulationGain   = 1.0;
   mJammingPowerGain = 1.0;
   mSignalPowerGain  = 1.0;
   mRcvrNoiseGain    = 1.0;
   RepeaterEffectData::Reset();
}
