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

#include "WsfEW_RepeaterEffect.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_Result.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfStringId.hpp"

WsfEW_RepeaterEffect::WsfEW_RepeaterEffect()
   : WsfEW_RadiusEffect()
   , mRepeaterEffectDataMap()
   , mRepeaterFactor(1.0)
   , mMinGainFactor(1.0)
   , mMaxGainFactor(1.0)
{
   mCoherencyMask = cEC_NONE;
   mBehavior      = cEB_JAMMER_POWER_EFFECT;
}

WsfEW_RepeaterEffect::WsfEW_RepeaterEffect(const WsfEW_RepeaterEffect& aSrc)
   : WsfEW_RadiusEffect(aSrc)
   , mRepeaterEffectDataMap()
   , mRepeaterFactor(aSrc.mRepeaterFactor)
   , mMinGainFactor(aSrc.mMinGainFactor)
   , mMaxGainFactor(aSrc.mMaxGainFactor)
{
}

// virtual
WsfEW_Effect* WsfEW_RepeaterEffect::Clone() const
{
   return new WsfEW_RepeaterEffect(*this);
}

// virtual
bool WsfEW_RepeaterEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_RadiusEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_SENSOR | cES_COMM; // effect sensor and comms
   }

   return ok;
}

// virtual
bool WsfEW_RepeaterEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_RadiusEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_RepeaterEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   if (GetControlMethod(mEffectedSystemId) != cCM_NONE)
   {
      aEW_Effects.mMask |= cEB_REPEATER_EFFECT;

      auto out = ut::log::debug();
      if (mDebug)
      {
         out << "Repeater Effect:";
      }

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         aEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *= mRepeaterFactor;
         if (mDebug)
         {
            out.AddNote() << "Coherent repeater factor: " << mRepeaterFactor;
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         aEW_Effects.mEW_NoiseJammingEffect.mRepeaterFactor *= mRepeaterFactor;
         if (mDebug)
         {
            out.AddNote() << "Non-coherent repeater factor: " << mRepeaterFactor;
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         aEW_Effects.mEW_PulseJammingEffect.mRepeaterFactor *= mRepeaterFactor;
         if (mDebug)
         {
            out.AddNote() << "Non-coherent pulse repeater factor: " << mRepeaterFactor;
         }
      }
   }

   WsfEW_RadiusEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_RepeaterEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   WsfEW_RadiusEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_RepeaterEffect::UpdateEffects(double                                 aSimTime,
                                         const WsfEM_Interaction&               aTargetInteraction,
                                         const WsfEM_Interaction&               aJammerToTgtInteraction,
                                         const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                         WsfEW_Effects&                         aEW_Effects,
                                         WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEW_RadiusEffect::UpdateEffects(aSimTime,
                                     aTargetInteraction,
                                     aJammerToTgtInteraction,
                                     aTgtToJammerInteraction,
                                     aEW_Effects,
                                     aEW_EffectPtr);
   WsfEM_Xmtr*         jammerXmtrPtr    = aJammerToTgtInteraction.GetTransmitter();
   const WsfEW_Result* ewTgtInteraction = WsfEW_Result::Find(const_cast<WsfEM_Interaction&>((aTargetInteraction)));

   WsfEM_Xmtr* radarXmtrPtr = aTargetInteraction.GetTransmitter();
   assert(radarXmtrPtr != nullptr);

   mRepeaterFactor = 1.0;
   if (GetControlMethod(mEffectedSystemId) == cCM_NONE)
   {
      return;
   }

   double gains[3];
   UtVec3d::Set(gains, 1.0);
   auto out = ut::log::debug();
   if (((GetControlMethod(mEffectedSystemId) == cCM_USE_REPEATER) ||
        ((GetControlMethod(mEffectedSystemId) == cCM_USE_REPEATER_ACTUALS))) &&
       ((aEW_Effects.mMask & WsfEW_Effect::cEB_REPEATER_UPDATE) != 0))
   {
      gains[1] = aEW_Effects.mCurrentRepeaterData.mGain;
      gains[0] = aEW_Effects.mCurrentRepeaterData.mMinGain;
      gains[2] = aEW_Effects.mCurrentRepeaterData.mPeakGain;

      if (mDebug)
      {
         out << "Using Repeater Update Data:";
      }
   }
   else if ((GetControlMethod(mEffectedSystemId) == cCM_USE_ACTUALS) ||
            (((GetControlMethod(mEffectedSystemId) == cCM_USE_REPEATER_ACTUALS)) &&
             ((ewTgtInteraction->mEW_Effects.mMask & WsfEW_Effect::cEB_REPEATER_UPDATE) == 0)))
   {
      double jammerXmtrloc[3];
      double relLocWCS[3];
      double jammerAz(0.0);
      double jammerEl(0.0);
      jammerXmtrPtr->GetAntenna()->GetLocationWCS(jammerXmtrloc);
      radarXmtrPtr->GetAntenna()->GetRelativeLocationWCS(jammerXmtrloc, relLocWCS);
      radarXmtrPtr->GetAntenna()->ComputeAspect(relLocWCS, jammerAz, jammerEl);
      double azLookup = aTargetInteraction.mXmtrBeam.mAz + (jammerAz - aTargetInteraction.mXmtrToTgt.mTrueAz);
      double elLookup = aTargetInteraction.mXmtrBeam.mEl + (jammerEl - aTargetInteraction.mXmtrToTgt.mTrueEl);
      WsfAntennaPattern* antennaPatternPtr =
         radarXmtrPtr->GetAntennaPattern(radarXmtrPtr->GetPolarization(), radarXmtrPtr->GetFrequency());

      gains[1] = antennaPatternPtr->GetGain(radarXmtrPtr->GetFrequency(),
                                            azLookup,
                                            elLookup,
                                            aTargetInteraction.mXmtrBeam.mEBS_Az,
                                            aTargetInteraction.mXmtrBeam.mEBS_El);
      gains[0] = antennaPatternPtr->GetMinimumGain();
      gains[2] = antennaPatternPtr->GetPeakGain(radarXmtrPtr->GetFrequency());

      if (mDebug)
      {
         out << "Using Actual Data:";
      }
   }

   UpdateGainFactors(aTargetInteraction, aJammerToTgtInteraction, aTgtToJammerInteraction, gains);
   mRepeaterFactor = UtMath::Lerp(gains[1], gains[2], gains[0], mMinGainFactor, mMaxGainFactor);

   // Limit within the bounds
   mRepeaterFactor = std::max(GetMinimumGainFactor(mEffectedSystemId),
                              std::min(mRepeaterFactor, GetMaximumGainFactor(mEffectedSystemId)));

   if (mDebug)
   {
      out.AddNote() << "MinGain: " << gains[0];
      out.AddNote() << "Gain: " << gains[1];
      out.AddNote() << "PeakGain: " << gains[2];
      out.AddNote() << "MinGainFactor: " << mMinGainFactor;
      out.AddNote() << "MaxGainFactor: " << mMaxGainFactor;
      out.AddNote() << "RepeaterFactor: " << mRepeaterFactor;
      ;
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_RepeaterEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mRepeaterEffectDataMap[aSystemTypeId] =
      dynamic_cast<RepeaterEffectData*>(WsfEW_RadiusEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mRepeaterEffectDataMap[aSystemTypeId];
}

void WsfEW_RepeaterEffect::UpdateGainFactors(const WsfEM_Interaction&               aTargetInteraction,
                                             const WsfEM_Interaction&               aJammerToTgtInteraction,
                                             const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                             double                                 aCurrentGains[])

{
   int gainControlMask = GetGainControlTypeMask(mEffectedSystemId);
   if (gainControlMask == cMG_NONE)
   {
      return;
   }

   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Rcvr* rcvrPtr       = aTargetInteraction.GetReceiver();
   assert(rcvrPtr != nullptr);

   WsfEM_Interaction interaction;
   if (interaction.BeginOneWayInteraction(jammerXmtrPtr, rcvrPtr, true, false) == 0)
   {
      interaction.SetReceiverBeamPosition(aTargetInteraction.mRcvrBeam);
      interaction.SetTransmitterBeamPosition();

      // Compute the power from the transmitter as seen by the receiver.
      interaction.ComputeRF_OneWayPower();
      // interaction.mRcvdPower *= GetJammingPowerGain(mEffectedSystemId);
      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         interaction.mRcvdPower *= WsfRF_Jammer::ComputeReceiverGains(rcvrPtr);
      }

      double reqPowerJtoN(0.0);
      if ((gainControlMask & cMG_MIN_J_TO_N) != 0)
      {
         reqPowerJtoN = rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier();
         if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
         {
            reqPowerJtoN += aTargetInteraction.mClutterPower;
         }
         reqPowerJtoN *= GetDesiredJtoN(mEffectedSystemId);
      }
      double requiredPower(reqPowerJtoN);

      double reqPowerJtoS(0.0);
      if ((gainControlMask & cMG_MIN_J_TO_S) != 0)
      {
         reqPowerJtoS = aTargetInteraction.mRcvdPower * GetDesiredJtoS(mEffectedSystemId);
      }
      requiredPower = std::max(requiredPower, reqPowerJtoS);

      double reqdPowerMinDetect(0.0);
      if ((gainControlMask & cMG_MIN_DETECT) != 0)
      {
         if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
         {
            reqdPowerMinDetect =
               aTargetInteraction.mDetectionThreshold *
               (rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier() + aTargetInteraction.mClutterPower);
         }
         else
         {
            // Non-coherent, get minimum power to detect jamming
            reqdPowerMinDetect = rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier();
         }
         reqdPowerMinDetect *= GetMinDetectFactor(mEffectedSystemId);
      }
      requiredPower = std::max(requiredPower, reqdPowerMinDetect);

      double reqPowerMasking(0.0);
      if ((gainControlMask & cMG_MASKING) != 0)
      {
         if ((aTargetInteraction.GetTarget() != nullptr) &&
             (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u)))
         {
            reqPowerMasking = aTargetInteraction.mRcvdPower;
         }
         else
         {
            // Non-coherent and/or a non-target interaction, get minimum power to mask signal out
            reqPowerMasking =
               ((aTargetInteraction.mRcvdPower / aTargetInteraction.mDetectionThreshold) -
                rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier() - aTargetInteraction.mClutterPower);
         }
         reqPowerMasking *= GetMaskingFactor(mEffectedSystemId);
      }
      requiredPower = std::max(requiredPower, reqPowerMasking);

      double reqPowerRCS(0.0);
      if ((gainControlMask & cMG_DESIRED_RCS) != 0)
      {
         if ((aTargetInteraction.GetTarget() != nullptr) &&
             (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u)))
         {
            reqPowerRCS = aTargetInteraction.mRcvdPower;
            if (aTargetInteraction.GetTarget()->IsFalseTarget())
            {
               reqPowerRCS *= GetDesiredFT_RCS(mEffectedSystemId) / aTargetInteraction.mRadarSig;
            }
         }
      }
      requiredPower = std::max(requiredPower, reqPowerRCS);

      /*
            double requiredPower = interaction.mRcvdPower;
            if ((gainControlMask & cMG_MASKING))
            {
               if (aTargetInteraction.GetTarget() != 0)
               {
                  if ((GetCoherencyMask() & cEC_COHERENT) ||
                     (GetCoherencyMask() & cEC_COHERENT_PULSE))
                  {
                     if (aTargetInteraction.GetTarget()->IsFalseTarget())
                     {
                        requiredPower =  aTargetInteraction.mRcvdPower *
                           GetDesiredFT_MaskingRCS(mEffectedSystemId) / aTargetInteraction.mRadarSig;
                     }
                     else
                     {
                        requiredPower = aTargetInteraction.mRcvdPower;
                     }
                  }
                  else
                  {
                     // Non-coherent, get minimum power to mask signal out
                     requiredPower = (aTargetInteraction.mRcvdPower / aTargetInteraction.mDetectionThreshold) -
                        rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier() -
                        aTargetInteraction.mClutterPower;
                  }
               }
               else
               {

                  // Non-coherent, get minimum power to mask signal out
                  requiredPower = (aTargetInteraction.mRcvdPower / aTargetInteraction.mDetectionThreshold) -
                     rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier() -
                     aTargetInteraction.mClutterPower;
               }
            }
            else if ((gainControlMask & cMG_MIN_DETECT))
            {
               if ((GetCoherencyMask() & cEC_COHERENT) ||
                  (GetCoherencyMask() & cEC_COHERENT_PULSE))
               {
                  requiredPower = aTargetInteraction.mDetectionThreshold *
                     (rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier() + aTargetInteraction.mClutterPower);
               }
               else
               {
                  // Non-coherent, get minimum power to detect jamming
                  requiredPower = rcvrPtr->GetNoisePower() * rcvrPtr->GetNoiseMultiplier();
               }
            }
      */

      mMinGainFactor = (requiredPower / interaction.mRcvdPower) * (aCurrentGains[2] / interaction.mRcvrBeam.mGain);
      mMaxGainFactor = (requiredPower / interaction.mRcvdPower) * (aCurrentGains[0] / interaction.mRcvrBeam.mGain);

      if (mDebug)
      {
         auto out = ut::log::debug() << "Gain Factors Updated.";
         out.AddNote() << "JammerPowerRcvd: " << interaction.mRcvdPower;
         out.AddNote() << "RequiredJammerPower: " << requiredPower;
         out.AddNote() << "RepeaterFactor: " << requiredPower / interaction.mRcvdPower;
         out.AddNote() << "MinGainFactor: " << mMinGainFactor;
         out.AddNote() << "MaxGainFactor: " << mMaxGainFactor;
      }
   }
}


WsfEW_RepeaterEffect::ControlMethod WsfEW_RepeaterEffect::GetControlMethod(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mControlMethod;
}

void WsfEW_RepeaterEffect::SetControlMethod(WsfStringId aSystemTypeId, ControlMethod aControlMethod)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mControlMethod = aControlMethod;
}

double WsfEW_RepeaterEffect::GetMinimumGainFactor(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mMinimumGainFactor;
}

double WsfEW_RepeaterEffect::GetMaximumGainFactor(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mMaximumGainFactor;
}

void WsfEW_RepeaterEffect::SetGainFactors(WsfStringId aSystemTypeId, double aMinimumGainFactor, double aMaximumGainFactor)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mMinimumGainFactor = aMinimumGainFactor;
   mRepeaterEffectDataMap[aSystemTypeId]->mMaximumGainFactor = aMaximumGainFactor;
}

int WsfEW_RepeaterEffect::GetGainControlTypeMask(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mGainControlTypeMask;
}

double WsfEW_RepeaterEffect::GetDesiredJtoN(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mDesiredJtoN;
}

void WsfEW_RepeaterEffect::SetDesiredJtoN(WsfStringId aSystemTypeId, double aDesiredJtoN)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mDesiredJtoN = aDesiredJtoN;
}

double WsfEW_RepeaterEffect::GetDesiredJtoS(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mDesiredJtoS;
}

void WsfEW_RepeaterEffect::SetDesiredJtoS(WsfStringId aSystemTypeId, double aDesiredJtoS)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mDesiredJtoS = aDesiredJtoS;
}

double WsfEW_RepeaterEffect::GetDesiredFT_RCS(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mDesiredFT_RCS;
}

void WsfEW_RepeaterEffect::SetDesiredFT_RCS(WsfStringId aSystemTypeId, double aDesiredFT_MaskingRCS)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mDesiredFT_RCS = aDesiredFT_MaskingRCS;
}

double WsfEW_RepeaterEffect::GetMinDetectFactor(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mMinDetectFactor;
}

void WsfEW_RepeaterEffect::SetMinDetectFactor(WsfStringId aSystemTypeId, double aMinDetectFactor)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mMinDetectFactor = aMinDetectFactor;
}

double WsfEW_RepeaterEffect::GetMaskingFactor(WsfStringId aSystemTypeId) const
{
   auto edi = mRepeaterEffectDataMap.find(aSystemTypeId);
   if (edi == mRepeaterEffectDataMap.end())
   {
      edi = mRepeaterEffectDataMap.find(nullptr);
   }
   return edi->second->mMaskingFactor;
}

void WsfEW_RepeaterEffect::SetMaskingFactor(WsfStringId aSystemTypeId, double aMaskingFactor)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mRepeaterEffectDataMap[aSystemTypeId]->mMaskingFactor = aMaskingFactor;
}

// Embedded Effect data class
WsfEW_RepeaterEffect::RepeaterEffectData::RepeaterEffectData()
   : RadiusData()
   , mControlMethod(cCM_NONE)
   , mMinimumGainFactor(0.0)
   , mMaximumGainFactor(DBL_MAX)
   , mGainControlTypeMask(cMG_NONE)
   , mDesiredJtoN(1.0)
   , mDesiredJtoS(1.0)
   , mDesiredFT_RCS(1000.0)
   , mExplicitDesiredFT_RCS(false)
   , mMaskingFactor(1.0)
   , mMinDetectFactor(1.0)

{
}

// virtual
bool WsfEW_RepeaterEffect::RepeaterEffectData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "repeater_effect_control_method")
   {
      std::string controlMethodTypeStr;
      aInput.ReadValue(controlMethodTypeStr);
      if (controlMethodTypeStr == "none")
      {
         mControlMethod = cCM_NONE;
      }
      else if (controlMethodTypeStr == "actuals")
      {
         mControlMethod = cCM_USE_ACTUALS;
      }
      else if (controlMethodTypeStr == "repeater")
      {
         mControlMethod = cCM_USE_REPEATER;
      }
      else if (controlMethodTypeStr == "repeater_actuals")
      {
         mControlMethod = cCM_USE_REPEATER_ACTUALS;
      }
      else
      {
         throw UtInput::BadValue(aInput, " not recognized as a 'repeater_effect_control_method'.");
      }
   }
   else if (command == "gain_control_method")
   {
      std::string minGainMethodTypeStr;
      aInput.ReadValue(minGainMethodTypeStr);
      if (minGainMethodTypeStr == "none")
      {
         SetGainControlType(cMG_NONE);
      }
      else if (minGainMethodTypeStr == "jammer_to_noise")
      {
         AddGainControlType(cMG_MIN_J_TO_N);
      }
      else if (minGainMethodTypeStr == "jammer_to_signal")
      {
         AddGainControlType(cMG_MIN_J_TO_S);
      }
      else if (minGainMethodTypeStr == "minimum_detect")
      {
         AddGainControlType(cMG_MIN_DETECT);
      }
      else if (minGainMethodTypeStr == "masking")
      {
         AddGainControlType(cMG_MASKING);
      }
      else if (minGainMethodTypeStr == "desired_rcs")
      {
         AddGainControlType(cMG_DESIRED_RCS);
      }
      else
      {
         throw UtInput::BadValue(aInput, " not recognized as a 'gain_control_method'.");
      }
   }
   else if (command == "repeater_factor_limits")
   {
      double minGain;
      double maxGain;
      aInput.ReadValueOfType(minGain, UtInput::cRATIO);
      aInput.ReadValueOfType(maxGain, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(minGain, 0.0);
      aInput.ValueGreaterOrEqual(maxGain, minGain);
      mMinimumGainFactor = minGain;
      mMaximumGainFactor = maxGain;
   }
   else if (command == "desired_false_target_rcs")
   {
      double desiredRCS;
      aInput.ReadValueOfType(desiredRCS, UtInput::cAREA_DB);
      mDesiredFT_RCS         = desiredRCS;
      mExplicitDesiredFT_RCS = true;
      AddGainControlType(cMG_DESIRED_RCS);
   }
   else if ((command == "desired_jammer_to_noise") || (command == "required_jammer_to_noise"))
   {
      double desiredJtoN;
      aInput.ReadValueOfType(desiredJtoN, UtInput::cRATIO);
      mDesiredJtoN = desiredJtoN;
      AddGainControlType(cMG_MIN_J_TO_N);
   }
   else if ((command == "desired_jammer_to_signal") || (command == "required_jammer_to_signal"))
   {
      double desiredJtoS;
      aInput.ReadValueOfType(desiredJtoS, UtInput::cRATIO);
      mDesiredJtoS = desiredJtoS;
      AddGainControlType(cMG_MIN_J_TO_S);
   }
   else if (command == "minimum_detect_factor")
   {
      double minDetectFactor;
      aInput.ReadValueOfType(minDetectFactor, UtInput::cRATIO);
      mMinDetectFactor = minDetectFactor;
   }
   else if (command == "masking_factor")
   {
      double maskingFactor;
      aInput.ReadValueOfType(maskingFactor, UtInput::cRATIO);
      mMaskingFactor = maskingFactor;
   }
   else if (!RadiusData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_RepeaterEffect::RepeaterEffectData::Initialize(WsfSimulation& aSimulation)
{
   return EffectData::Initialize(aSimulation);
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_RepeaterEffect::RepeaterEffectData::Clone() const
{
   return new RepeaterEffectData(*this);
}

// virtual
void WsfEW_RepeaterEffect::RepeaterEffectData::Reset()
{
   mControlMethod         = cCM_NONE;
   mMinimumGainFactor     = 0.1;
   mMaximumGainFactor     = 1.0;
   mGainControlTypeMask   = cMG_NONE;
   mDesiredJtoN           = 1.0;
   mDesiredJtoS           = 1.0;
   mDesiredFT_RCS         = 1000.0;
   mExplicitDesiredFT_RCS = false;
   mMaskingFactor         = 1.0;
   mMinDetectFactor       = 1.0;
   RadiusData::Reset();
}

void WsfEW_RepeaterEffect::RepeaterEffectData::AddGainControlType(GainControlType aGainControlType)
{
   mGainControlTypeMask ^= cMG_NONE;
   mGainControlTypeMask |= aGainControlType;
}

void WsfEW_RepeaterEffect::RepeaterEffectData::SetGainControlType(GainControlType aGainControlType)
{
   mGainControlTypeMask = aGainControlType;
}
