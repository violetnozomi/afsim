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

#include "WsfEW_Effects.hpp"

#include <iostream>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EA_EP.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_Types.hpp"
#include "WsfSimulation.hpp"


void WsfEW_Effects::Process(double                                 aSimTime,
                            const WsfEM_Interaction&               aTargetInteraction,
                            const WsfEM_Interaction&               aJammerToTgtInteraction,
                            const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                            bool                                   aProcessOnlyEA_Effects)
{
   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   // Check for valid EW attack and protect pointers
   WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(*jammerXmtrPtr);
   if (eaPtr == nullptr)
   {
      return;
   }

   WsfEM_Rcvr*  rcvrPtr = aTargetInteraction.GetReceiver();
   WsfEW_EA_EP* epPtr   = WsfEW_EP::GetElectronicProtect(*rcvrPtr);

   // Determine the system type that we are applying the EW effects to.
   WsfEW_Effect::SystemFunction affectedSystemFunction = WsfEW_Effect::ConvertToEW_SystemFunction(rcvrPtr->GetFunction());

   // Clear the EA effects coherency mask
   mEA_CoherencyMask ^= mEA_CoherencyMask;

   // Clear all of the EA Effect Bits, except the repeater effect so it can be
   // applied as necessary within the EW effects.
   mMask ^= (mMask ^ WsfEW_Effect::cEB_REPEATER_UPDATE);

   // Iterate through all the EA techniques, select and mitigate
   // the techniques that apply based on system type
   const WsfEW_Types::TechniqueIdVec& eaTechniquesToCounter = eaPtr->GetTechniquesInUse();
   for (WsfEW_Types::TechniqueId EA_TechId : eaTechniquesToCounter)
   {
      WsfEW_Technique* techPtr = eaPtr->GetTechnique(EA_TechId);
      if (techPtr == nullptr)
      {
         continue;
      }
      if (!techPtr->CanApplyTechnique(affectedSystemFunction))
      {
         continue;
      }

      // Iterate through all the effects on this technique, select and mitigate
      // the effects that apply based on system type
      const WsfEW_Technique::EffectPtrVec& effectsToMitigate = techPtr->GetEffects();
      for (unsigned int effectIndex = 0; effectIndex < effectsToMitigate.size(); ++effectIndex)
      {
         WsfEW_Effect* unMitigatedEffectPtr = techPtr->GetEffect(effectIndex);
         if (unMitigatedEffectPtr == nullptr)
         {
            continue;
         }
         mEA_CoherencyMask |= unMitigatedEffectPtr->GetCoherencyMask();
         unMitigatedEffectPtr->ProcessUnmitigatedEffect(aSimTime,
                                                        aTargetInteraction,
                                                        aJammerToTgtInteraction,
                                                        aTgtToJammerInteraction,
                                                        *this);

         if (aProcessOnlyEA_Effects || (epPtr == nullptr))
         {
            continue;
         }

         // Determine the system type that we are applying the EW effects to.
         WsfEW_Effect::SystemFunction affectingSystemFunction =
            WsfEW_Effect::ConvertToEW_SystemFunction(jammerXmtrPtr->GetFunction());

         // Get the effect
         WsfEW_Technique::EffectPtrVec mitigatingEffectList =
            epPtr->GetMitigatingEffects(unMitigatedEffectPtr, affectingSystemFunction, affectedSystemFunction);
         for (WsfEW_Effect* mitigatingEffectPtr : mitigatingEffectList)
         {
            if (mitigatingEffectPtr == nullptr)
            {
               continue;
            }
            mitigatingEffectPtr->ProcessMitigatedEffect(aSimTime,
                                                        aTargetInteraction,
                                                        aJammerToTgtInteraction,
                                                        aTgtToJammerInteraction,
                                                        *this,
                                                        unMitigatedEffectPtr);
         }
      }
   }
}

// =================================================================================================
void WsfEW_Effects::Print(ut::log::MessageStream& aStream) const
{
   aStream.AddNote() << "EW Interaction: " << mMask;
   aStream.AddNote() << "Azimuth Error: " << mEW_Error.mAzimuth * UtMath::cDEG_PER_RAD;
   aStream.AddNote() << "Elevation Error: " << mEW_Error.mElevation * UtMath::cDEG_PER_RAD;
   aStream.AddNote() << "Range: " << mEW_Error.mRange;
   aStream.AddNote() << "Velocity: " << mEW_Error.mVelocity;
   aStream.AddNote() << "Noise Power Blanking: " << mEW_NoiseJammingEffect.mBlanking;
   aStream.AddNote() << "Noise Power Cancelation: " << mEW_NoiseJammingEffect.mCancelation;
   aStream.AddNote() << "Noise Modulation Gain: " << mEW_NoiseJammingEffect.mModulationGain;
   aStream.AddNote() << "Noise Jamming Power Gain: " << mEW_NoiseJammingEffect.mJammingPowerGain;
   aStream.AddNote() << "Noise J to X Gain: " << mEW_NoiseJammingEffect.mJtoXGain;
   aStream.AddNote() << "Noise Power Radius Factor: " << mEW_NoiseJammingEffect.mRadiusFactor;
   aStream.AddNote() << "Noise Power Repeater Factor: " << mEW_NoiseJammingEffect.mRepeaterFactor;
   aStream.AddNote() << "Pulse Power Blanking: " << mEW_PulseJammingEffect.mBlanking;
   aStream.AddNote() << "Pulse Power Cancellation: " << mEW_PulseJammingEffect.mCancelation;
   aStream.AddNote() << "Pulse Modulation Gain: " << mEW_PulseJammingEffect.mModulationGain;
   aStream.AddNote() << "Pulse Jamming Power Gain: " << mEW_PulseJammingEffect.mJammingPowerGain;
   aStream.AddNote() << "Pulse J to X Gain: " << mEW_PulseJammingEffect.mJtoXGain;
   aStream.AddNote() << "Pulse Power RadiusFactor: " << mEW_PulseJammingEffect.mRadiusFactor;
   aStream.AddNote() << "Pulse Power RPJ: " << mEW_PulseJammingEffect.mRPJ_Gain;
   aStream.AddNote() << "Pulse Power Suppression: " << mEW_PulseJammingEffect.mPulseSuppression;
   aStream.AddNote() << "Pulse Power RepeaterFactor: " << mEW_PulseJammingEffect.mRepeaterFactor;
   aStream.AddNote() << "Coherent Power Blanking: " << mEW_CoherentJammingEffect.mBlanking;
   aStream.AddNote() << "Coherent Power Cancellation: " << mEW_CoherentJammingEffect.mCancelation;
   aStream.AddNote() << "Coherent Modulation Gain: " << mEW_CoherentJammingEffect.mModulationGain;
   aStream.AddNote() << "Coherent Jamming Power Gain: " << mEW_CoherentJammingEffect.mJammingPowerGain;
   aStream.AddNote() << "Coherent J to X Gain: " << mEW_CoherentJammingEffect.mJtoXGain;
   aStream.AddNote() << "Coherent Power Radius Factor: " << mEW_CoherentJammingEffect.mRadiusFactor;
   aStream.AddNote() << "Coherent Power RPJ: " << mEW_CoherentJammingEffect.mRPJ_Gain;
   aStream.AddNote() << "Coherent Pulse Suppression: " << mEW_CoherentJammingEffect.mPulseSuppression;
   aStream.AddNote() << "Coherent Power RepeaterFactor: " << mEW_CoherentJammingEffect.mRepeaterFactor;
   aStream.AddNote() << "Signal Power Gain: " << mEW_SignalEffect.mSignalPowerGain;
   aStream.AddNote() << "Rcvr Noise Power Gain: " << mEW_SignalEffect.mRcvrNoiseGain;
}

// =================================================================================================
void WsfEW_Effects::Reset()
{
   mMask             = 0;
   mEA_CoherencyMask = 0;
   mEW_Error.Reset();
   mEW_NoiseJammingEffect.Reset();
   mEW_PulseJammingEffect.Reset();
   mEW_CoherentJammingEffect.Reset();
   mEW_SignalEffect.Reset();
   mCurrentRepeaterData.Reset();
}

// =================================================================================================
// Embedded EW_Error class
// =================================================================================================

// =================================================================================================
void WsfEW_Effects::EW_Error::Reset()
{
   mAzimuth      = 0.0;
   mElevation    = 0.0;
   mRange        = 0.0;
   mVelocity     = 0.0;
   mBitErrorRate = 0.0;
}

// =================================================================================================
// Embedded EW_JammingEffect class
// =================================================================================================

// =================================================================================================
void WsfEW_Effects::EW_JammingEffect::Reset()
{
   mBlanking         = 1.0;
   mCancelation      = 1.0;
   mModulationGain   = 1.0;
   mJammingPowerGain = 1.0;
   mJtoXGain         = 1.0;
   mPulseSuppression = 1.0;
   mProtectTarget    = -1;
   mRadiusFactor     = 1.0;
   mRepeaterFactor   = 1.0;
   mRPJ_Gain         = 1.0;
}

// =================================================================================================
void WsfEW_Effects::EW_JammingEffect::Zeroize()
{
   mBlanking         = 0.0;
   mCancelation      = 0.0;
   mModulationGain   = 0.0;
   mJammingPowerGain = 0.0;
   mJtoXGain         = 0.0;
   mPulseSuppression = 0.0;
   mProtectTarget    = -1;
   mRadiusFactor     = 0.0;
   mRepeaterFactor   = 0.0;
   mRPJ_Gain         = 0.0;
}

// =================================================================================================
// Embedded EW_SignalEffect class
// =================================================================================================

// =================================================================================================
void WsfEW_Effects::EW_SignalEffect::Reset()
{
   mSignalPowerGain = 1.0;
   mRcvrNoiseGain   = 1.0;
}

// =================================================================================================
void WsfEW_Effects::EW_SignalEffect::Zeroize()
{
   mSignalPowerGain = 0.0;
   mRcvrNoiseGain   = 0.0;
}

// =================================================================================================
// Embedded EW_RepeaterData class
// =================================================================================================

// =================================================================================================
void WsfEW_Effects::RepeaterData::Reset()
{
   mMinGain  = -1.0;
   mGain     = -1.0;
   mPeakGain = -1.0;
}

// =================================================================================================
// Embedded WsfEW_Effects scripting class
// =================================================================================================

// =================================================================================================
//! Define script methods for WsfEW_Effects.,
class WSF_MIL_EXPORT WsfScriptEW_EffectsClass : public UtScriptClass
{
public:
   WsfScriptEW_EffectsClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptEW_EffectsClass() override = default;

   // EW Effects methods
   UT_DECLARE_SCRIPT_METHOD(Mask);
   UT_DECLARE_SCRIPT_METHOD(EA_CoherencyCheck);
   UT_DECLARE_SCRIPT_METHOD(EffectValue_1);
   UT_DECLARE_SCRIPT_METHOD(EffectValue_2);
};

std::unique_ptr<UtScriptClass> WsfEW_Effects::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptEW_EffectsClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
WsfScriptEW_EffectsClass::WsfScriptEW_EffectsClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfEW_Effects");

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<Mask>());
   AddMethod(ut::make_unique<EA_CoherencyCheck>());
   AddMethod(ut::make_unique<EffectValue_1>("EffectValue"));
   AddMethod(ut::make_unique<EffectValue_2>("EffectValue"));
}

namespace
{
double GetJammingEffectValue(WsfEW_Effects::EW_JammingEffect* aJammingEffectPtr, const std::string& aValueTypeString)
{
   double value = 1.0;
   if (aJammingEffectPtr != nullptr)
   {
      if (aValueTypeString == "blanking")
      {
         value = aJammingEffectPtr->mBlanking;
      }
      else if ((aValueTypeString == "cancellation") || (aValueTypeString == "cancelation"))
      {
         value = aJammingEffectPtr->mCancelation;
      }
      else if (aValueTypeString == "modulation_gain")
      {
         value = aJammingEffectPtr->mModulationGain;
      }
      else if (aValueTypeString == "jamming_power_gain")
      {
         value = aJammingEffectPtr->mJammingPowerGain;
      }
      else if (aValueTypeString == "JtoX_Gain")
      {
         value = aJammingEffectPtr->mJtoXGain;
      }
      else if (aValueTypeString == "protect_target")
      {
         value = (double)aJammingEffectPtr->mProtectTarget;
      }
      else if (aValueTypeString == "pulse_suppression")
      {
         value = aJammingEffectPtr->mPulseSuppression;
      }
      else if (aValueTypeString == "radius_factor")
      {
         value = aJammingEffectPtr->mRadiusFactor;
      }
      else if (aValueTypeString == "repeater_factor")
      {
         value = aJammingEffectPtr->mRepeaterFactor;
      }
      else if (aValueTypeString == "RPJ_Gain")
      {
         value = aJammingEffectPtr->mRPJ_Gain;
      }
      else
      {
         auto out = ut::log::warning() << "EffectValue: Undefined value type string.";
         out.AddNote() << "Type String: " << aValueTypeString;
         value = -1.0;
      }
   }
   return value;
}

double GetEffectValue(WsfEW_Effects*     aEW_EffectsPtr,
                      const std::string& aEW_CategoryString,
                      const std::string& aValueTypeString,
                      int                aEA_CoherencyMask)
{
   double value = -1.0;
   if (aEW_CategoryString == "error")
   {
      if (aValueTypeString == "azimuth")
      {
         value = aEW_EffectsPtr->mEW_Error.mAzimuth;
      }
      else if (aValueTypeString == "elevation")
      {
         value = aEW_EffectsPtr->mEW_Error.mElevation;
      }
      else if (aValueTypeString == "range")
      {
         value = aEW_EffectsPtr->mEW_Error.mRange;
      }
      else if (aValueTypeString == "velocity")
      {
         value = aEW_EffectsPtr->mEW_Error.mVelocity;
      }
      else if ((aValueTypeString == "BER") || (aValueTypeString == "bit_error_rate"))
      {
         value = aEW_EffectsPtr->mEW_Error.mBitErrorRate;
      }
   }
   else if ((aEW_CategoryString == "jamming") || (aEW_CategoryString == "power"))
   {
      WsfEW_Effects::EW_JammingEffect* jammingEffectPtr1(nullptr);
      WsfEW_Effects::EW_JammingEffect* jammingEffectPtr2(nullptr);
      if ((((aEA_CoherencyMask & (WsfEW_Effect::cEC_NONE | WsfEW_Effect::cEC_NONCOHERENT)) != 0) ||
           (aEA_CoherencyMask == 0)) &&
          ((aEA_CoherencyMask & WsfEW_Effect::cEC_NONCOHERENT_PULSE) != 0))
      {
         jammingEffectPtr1 = &(aEW_EffectsPtr->mEW_NoiseJammingEffect);
         jammingEffectPtr2 = &(aEW_EffectsPtr->mEW_PulseJammingEffect);
      }
      if (((aEA_CoherencyMask & (WsfEW_Effect::cEC_NONE | WsfEW_Effect::cEC_NONCOHERENT)) != 0) || (aEA_CoherencyMask == 0))
      {
         jammingEffectPtr1 = &(aEW_EffectsPtr->mEW_NoiseJammingEffect);
      }
      else if ((aEA_CoherencyMask & WsfEW_Effect::cEC_NONCOHERENT_PULSE) != 0)
      {
         jammingEffectPtr1 = &(aEW_EffectsPtr->mEW_PulseJammingEffect);
      }
      else if ((aEA_CoherencyMask & (WsfEW_Effect::cEC_COHERENT | WsfEW_Effect::cEC_COHERENT_PULSE)) != 0)
      {
         jammingEffectPtr2 = &(aEW_EffectsPtr->mEW_CoherentJammingEffect);
      }
      else
      {
         auto out = ut::log::warning() << "EffectValue: Undefined coherency check type string.";
         out.AddNote() << "Type String: " << aEA_CoherencyMask;
      }

      value = GetJammingEffectValue(jammingEffectPtr1, aValueTypeString) *
              GetJammingEffectValue(jammingEffectPtr2, aValueTypeString);
   }
   else if (aEW_CategoryString == "signal")
   {
      if (aValueTypeString == "signal_gain")
      {
         value = aEW_EffectsPtr->mEW_SignalEffect.mSignalPowerGain;
      }
      else if (aValueTypeString == "noise_power_gain")
      {
         value = aEW_EffectsPtr->mEW_SignalEffect.mRcvrNoiseGain;
      }
   }
   else if (aEW_CategoryString == "repeater_data")
   {
      if (aValueTypeString == "minimum_gain")
      {
         value = aEW_EffectsPtr->mCurrentRepeaterData.mMinGain;
      }
      else if (aValueTypeString == "gain")
      {
         value = aEW_EffectsPtr->mCurrentRepeaterData.mGain;
      }
      else if (aValueTypeString == "maximum_gain")
      {
         value = aEW_EffectsPtr->mCurrentRepeaterData.mGain;
      }
   }
   return value;
}
} // namespace

// bool mask = <x>.Mask();
UT_DEFINE_SCRIPT_METHOD(WsfScriptEW_EffectsClass, WsfEW_Effects, Mask, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->mMask);
}

// bool EA_CoherencyCheck = <x>.EA_CoherencyCheck(string aEA_CoherencyType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptEW_EffectsClass, WsfEW_Effects, EA_CoherencyCheck, 1, "bool", "string")
{
   bool cohMatch = false;

   std::string coherencyTypeString = aVarArgs[0].GetString();
   if (coherencyTypeString == "noise")
   {
      if ((aObjectPtr->mEA_CoherencyMask & (WsfEW_Effect::cEC_NONE | WsfEW_Effect::cEC_NONCOHERENT)) ||
          (aObjectPtr->mEA_CoherencyMask == 0))
      {
         cohMatch = true;
      }
   }
   else if (coherencyTypeString == "pulse")
   {
      if (aObjectPtr->mEA_CoherencyMask & WsfEW_Effect::cEC_NONCOHERENT_PULSE)
      {
         cohMatch = true;
      }
   }
   else if (coherencyTypeString == "noise_and_pulse")
   {
      if (((aObjectPtr->mEA_CoherencyMask & (WsfEW_Effect::cEC_NONE | WsfEW_Effect::cEC_NONCOHERENT)) ||
           (aObjectPtr->mEA_CoherencyMask == 0)) &&
          (aObjectPtr->mEA_CoherencyMask & WsfEW_Effect::cEC_NONCOHERENT_PULSE))
      {
         cohMatch = true;
      }
   }
   else if (coherencyTypeString == "coherent")
   {
      if (aObjectPtr->mEA_CoherencyMask & (WsfEW_Effect::cEC_COHERENT | WsfEW_Effect::cEC_COHERENT_PULSE))
      {
         cohMatch = true;
      }
   }
   else
   {
      auto out = ut::log::warning() << "EA_CoherencyCheck: Undefined coherency check type string.";
      out.AddNote() << "Type String: " << coherencyTypeString;
   }
   aReturnVal.SetBool(cohMatch);
}

// double EffectValue = <x>.EffectValue(string aEW_Category, string aValueType, string aEA_CoherencyType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptEW_EffectsClass, WsfEW_Effects, EffectValue_1, 3, "double", "string, string, string")
{
   std::string EW_CategoryString   = aVarArgs[0].GetString();
   std::string valueTypeString     = aVarArgs[1].GetString();
   std::string coherencyTypeString = aVarArgs[2].GetString();

   int    coherencyMask = WsfEW_Effect::ConvertCoherencyIdToMask(coherencyTypeString);
   double value         = GetEffectValue(aObjectPtr, EW_CategoryString, valueTypeString, coherencyMask);
   aReturnVal.SetDouble(value);
}


// double EffectValue = <x>.EffectValue(string aEW_Category, string aValueType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptEW_EffectsClass, WsfEW_Effects, EffectValue_2, 2, "double", "string, string")
{
   std::string EW_CategoryString = aVarArgs[0].GetString();
   std::string valueTypeString   = aVarArgs[1].GetString();

   double value = GetEffectValue(aObjectPtr, EW_CategoryString, valueTypeString, aObjectPtr->mEA_CoherencyMask);
   aReturnVal.SetDouble(value);
}
