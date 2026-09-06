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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfEW_CommComponent.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommResult.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Result.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfScenario.hpp"

namespace
{
class EWCommComponentFactory : public WsfComponentFactory<wsf::comm::Comm>
{
public:
   void PreInput(wsf::comm::Comm& aParent) override
   {
      // TODO_JAJ We should really only have to inject this component on RF comm devices.
      // TODO_JAJ Unlike WsfSensor, WsfComm does not include a way to determine if the
      // TODO_JAJ device operates in the RF spectrum.
      WsfEW_CommComponent::FindOrCreate(aParent);
   }

   // TODO_JAJ I don't think this is needed.
   bool PreInitialize(double aSimTime, wsf::comm::Comm& aParent) override
   {
      WsfEW_CommComponent::FindOrCreate(aParent);
      return true;
   }
};
} // namespace

// =================================================================================================
//! Register the component factory that handles input for this component.
void WsfEW_CommComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<EWCommComponentFactory>());
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
WsfEW_CommComponent* WsfEW_CommComponent::Find(const wsf::comm::Comm& aParent)
{
   WsfEW_CommComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfEW_CommComponent>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
WsfEW_CommComponent* WsfEW_CommComponent::FindOrCreate(wsf::comm::Comm& aParent)
{
   WsfEW_CommComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfEW_CommComponent;
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
WsfComponent* WsfEW_CommComponent::CloneComponent() const
{
   return new WsfEW_CommComponent(*this);
}

// =================================================================================================
WsfStringId WsfEW_CommComponent::GetComponentName() const
{
   return UtStringIdLiteral("ew_comm_component");
}

// =================================================================================================
const int* WsfEW_CommComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_EW_COMM, cWSF_COMPONENT_COMM, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* WsfEW_CommComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_EW_COMM)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return (wsf::comm::Component*)this;
   }
   return nullptr;
}

// =================================================================================================
// virtual
bool WsfEW_CommComponent::Initialize(double aSimTime)
{
   return true;
}

// =================================================================================================
// virtual
bool WsfEW_CommComponent::Initialize2(double aSimTime)
{
   return true;
}

// =================================================================================================
// virtual
bool WsfEW_CommComponent::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "jamming_perception_threshold")
   {
      double jnrPerceptionThreshold;
      aInput.ReadValueOfType(jnrPerceptionThreshold, UtInput::cRATIO);
      mContJNR_PerceptionThreshold     = jnrPerceptionThreshold;
      mPulsedJNR_PerceptionThreshold   = jnrPerceptionThreshold;
      mCoherentJNR_PerceptionThreshold = jnrPerceptionThreshold;
   }
   else if (command == "continuous_jamming_perception_threshold")
   {
      aInput.ReadValueOfType(mContJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else if (command == "pulsed_jamming_perception_threshold")
   {
      aInput.ReadValueOfType(mPulsedJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else if (command == "coherent_jamming_perception_threshold")
   {
      aInput.ReadValueOfType(mCoherentJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else if (command == "jamming_perception_timeout")
   {
      aInput.ReadValueOfType(mJammingPerceptionResetTime, UtInput::cTIME);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Checks to see if jamming can be perceived by the sensor beam.
//! @returns 'true' if jamming can be perceived by the sensor beam, 'false' otherwise.
bool WsfEW_CommComponent::CanPerceiveJamming() const
{
   return (((mPulsedJNR_PerceptionThreshold - 1.0e38) < 0.001) || ((mContJNR_PerceptionThreshold - 1.0e38) < 0.001) ||
           ((mCoherentJNR_PerceptionThreshold - 1.0e38) < 0.001));
}

// virtual
void WsfEW_CommComponent::AttemptToReceive(double aSimTime, wsf::comm::Result& aResult)
{
   // Ensure the CommResult object has an EW_Result component attached.
   WsfEW_Result& ewResult = WsfEW_Result::FindOrCreate(aResult);

   // Compute the effect of jamming on the receiver.
   WsfRF_Jammer::ComputeTotalJammerEffects(aSimTime, aResult, true);

   // Block the message if specified.
   if ((ewResult.mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_MESSAGE) != 0u)
   {
      aResult.mInterferenceFactor = 1.0;
   }

   // Update jamming perception status
   if (PerceivesJamming(aSimTime, aResult))
   {
      if (mJammingFirstPerceived < 0.0)
      {
         mJammingFirstPerceived = aSimTime;
      }
      mJammingLastPerceived = aSimTime;
   }
   else if ((mJammingLastPerceived >= 0.0) && ((aSimTime - mJammingLastPerceived) >= mJammingPerceptionResetTime))
   {
      mJammingFirstPerceived = -1.0;
      mJammingLastPerceived  = -1.0;
   }
}

// ============================================================================
//! Checks to see if jamming is being perceived by the comm.
//! @return Returns 'true' if jamming is perceived by the comm, 'false' otherwise.
// virtual
bool WsfEW_CommComponent::JammingPerceived()
{
   return (mJammingLastPerceived >= 0.0);
}

// ============================================================================
//! Checks to see if jamming is being perceived by the comm device.
//! @note  Currently assumes that the receiver clutter power was previously calculated.
//! @param aSimTime The current simulation time.
//! @param aResult  On input this contains the current information for the interaction.
//! @return Returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
// virtual
bool WsfEW_CommComponent::PerceivesJamming(double aSimTime, wsf::comm::Result& aResult)
{
   WsfEM_Rcvr* rcvrPtr = aResult.GetReceiver();
   if (rcvrPtr != nullptr)
   {
      WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);
      if (ewResultPtr != nullptr)
      {
         WsfEW_Result& ewResult = *ewResultPtr;

         // Compute the jammer-to-noise ratios for the perceives jamming flag.
         double noiseJammerPower =
            ewResult.mNoiseJammerPower / (ewResult.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                                          ewResult.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                                          ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                                          ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain *
                                          ewResult.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                                          ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain);
         double contJammerToNoise = rcvrPtr->ComputeSignalToNoise(noiseJammerPower, aResult.mClutterPower, 0.0);

         double pulseJammerPower =
            ewResult.mPulseJammerPower / (ewResult.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                                          ewResult.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                                          ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                                          ewResult.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                                          ewResult.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                                          ewResult.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain);
         double pulsedJammerToNoise = rcvrPtr->ComputeSignalToNoise(pulseJammerPower, aResult.mClutterPower, 0.0);
         double coherentJammerPower =
            ewResult.mCoherentJammerPower / (ewResult.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                                             ewResult.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                                             ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                                             ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                                             ewResult.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                                             ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain);
         double coherentJammerToNoise = rcvrPtr->ComputeSignalToNoise(coherentJammerPower, aResult.mClutterPower, 0.0);

         if ((contJammerToNoise >= mContJNR_PerceptionThreshold) ||
             (pulsedJammerToNoise >= mPulsedJNR_PerceptionThreshold) ||
             (coherentJammerToNoise >= mCoherentJNR_PerceptionThreshold))
         {
            return true;
         }
      }
   }
   return false;
}
