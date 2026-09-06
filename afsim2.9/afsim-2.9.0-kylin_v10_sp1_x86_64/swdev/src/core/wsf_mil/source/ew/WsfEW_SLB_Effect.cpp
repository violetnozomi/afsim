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

#include "WsfEW_SLB_Effect.hpp"

#include <cfloat>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtVec2.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAntennaPatternTypes.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_PulseEffect.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfEW_SLB_Effect::WsfEW_SLB_Effect()
   : WsfEW_PowerEffect()
   , mTargetBlankingEffect(cTB_NOTARGETBLANKING)
   , mTargetBlankingTable()
   , mAuxRcvrPtr(nullptr)
   , mAuxAntennaPtr(nullptr)
   , mAuxAntennaPatternPtr(nullptr)
   , mAuxBeamTilt(0.0)
   , mExplicitAuxBeamTilt(false)
   , mBlankingMap()
   , mBlankingThreshold(1.0)
   , mSaturationDutyCycle(1.0)
   , mSaturationEffect(cSE_NOSATURATIONEFFECT)
   , mBlankerState(cOFF)
   ,
   // mAuxChannelJammerPower(0.0),
   mTargetBlanked(false)
{
   mAuxThresholds[0]  = 0.0;
   mAuxThresholds[1]  = DBL_MAX;
   mMainThresholds[0] = 0.0;
   mMainThresholds[1] = DBL_MAX;

   mBehavior      = cEB_JAMMER_POWER_EFFECT;
   mCoherencyMask = cEC_NONE | cEC_NONCOHERENT | cEC_COHERENT | cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
}

WsfEW_SLB_Effect::WsfEW_SLB_Effect(const WsfEW_SLB_Effect& aSrc)
   : WsfEW_PowerEffect(aSrc)
   , mTargetBlankingEffect(aSrc.mTargetBlankingEffect)
   , mTargetBlankingTable(aSrc.mTargetBlankingTable)
   , mAuxRcvrPtr(nullptr)
   , mAuxAntennaPtr(nullptr)
   , mAuxAntennaPatternPtr(aSrc.mAuxAntennaPatternPtr)
   , // Cloned in UpdateEffects(...) if required
   mAuxBeamTilt(aSrc.mAuxBeamTilt)
   , mExplicitAuxBeamTilt(aSrc.mExplicitAuxBeamTilt)
   , mBlankingMap(aSrc.mBlankingMap)
   , mBlankingThreshold(aSrc.mBlankingThreshold)
   , mSaturationDutyCycle(aSrc.mSaturationDutyCycle)
   , mSaturationEffect(aSrc.mSaturationEffect)
   , mBlankerState(aSrc.mBlankerState)
   ,
   // mAuxChannelJammerPower(aSrc.mAuxChannelJammerPower),
   mTargetBlanked(aSrc.mTargetBlanked)
{
   UtVec2d::Set(mAuxThresholds, aSrc.mAuxThresholds);
   UtVec2d::Set(mMainThresholds, aSrc.mMainThresholds);

   if (aSrc.mAuxAntennaPtr != nullptr)
   {
      mAuxAntennaPtr = new WsfEM_Antenna(*aSrc.mAuxAntennaPtr);
   }

   if (aSrc.mAuxRcvrPtr != nullptr)
   {
      mAuxRcvrPtr = new WsfEM_Rcvr(*aSrc.mAuxRcvrPtr, mAuxAntennaPtr);
   }
}

// virtual
WsfEW_SLB_Effect::~WsfEW_SLB_Effect()
{
   delete mAuxRcvrPtr;
   delete mAuxAntennaPtr;
}

// virtual
WsfEW_Effect* WsfEW_SLB_Effect::Clone() const
{
   return new WsfEW_SLB_Effect(*this);
}

bool WsfEW_SLB_Effect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = true;

   ok &= WsfEW_PowerEffect::Initialize(aSimulation);

   if (mAuxAntennaPatternPtr != nullptr)
   {
      if (mAuxRcvrPtr != nullptr)
      {
         mAuxRcvrPtr->SetAntennaPattern(mAuxAntennaPatternPtr->Clone(), WsfEM_Types::cPOL_DEFAULT, 0.0);
      }
      mAuxAntennaPatternPtr->Initialize(GetSimulation());
   }
   else if (mAuxRcvrPtr->GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, 0.0) != nullptr)
   {
      //   mAuxAntennaPatternPtr = mAuxRcvrPtr->GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, 0.0);
   }
   else
   {
      auto out = ut::log::error() << "Antenna pattern not defined.";
      out.AddNote() << "Effect: " << GetName();
      ok = false;
   }

   if (mAuxAntennaPtr != nullptr)
   {
      mAuxAntennaPtr->Initialize(GetEA_EP_Ptr()->GetLinkedXmtrRcvr().GetArticulatedPart());
      if (mAuxRcvrPtr != nullptr)
      {
         mAuxRcvrPtr->SetAntenna(mAuxAntennaPtr);
         mAuxRcvrPtr->Initialize(*GetSimulation());
      }
   }

   return ok;
}

bool WsfEW_SLB_Effect::ProcessInput(UtInput& aInput)
{
   // Allocate temporary antenna and receiver objects for input processing if one hasn't been allocated.
   //
   // The pointer to the temporary object is held in a plain pointer and an unique_ptr. The later is used so the
   // temporary object will automatically be deleted if not used or if an exception is thrown. If the temporary
   // object is used (i.e.: the input would cause the object to be needed), its pointer is assigned to the
   // permanent object and the unique_ptr is 'released' from ownership (preventing deletion).

   std::unique_ptr<WsfEM_Antenna> tempAntennaPtr;
   WsfEM_Antenna*                 antennaPtr = mAuxAntennaPtr;
   if (mAuxAntennaPtr == nullptr)
   {
      antennaPtr = new WsfEM_Antenna();
      tempAntennaPtr.reset(antennaPtr);
   }

   std::unique_ptr<WsfEM_Rcvr> tempRcvrPtr;
   WsfEM_Rcvr*                 rcvrPtr = mAuxRcvrPtr;
   if (mAuxRcvrPtr == nullptr)
   {
      rcvrPtr = new WsfEM_Rcvr(WsfEM_Rcvr::cRF_SENSOR, mAuxAntennaPtr);
      tempRcvrPtr.reset(rcvrPtr);
   }

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "auxiliary_beam_tilt")
   {
      aInput.ReadValueOfType(mAuxBeamTilt, UtInput::cANGLE);
      aInput.ValueInClosedRange(mAuxBeamTilt, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      mExplicitAuxBeamTilt = true;
   }
   else if ((command == "auxiliary_antenna_pattern") || // Allow embedded antenna input if an antenna is attached.
            (command == "antenna_pattern"))             // Legacy support
   {
      std::string patternType;
      aInput.ReadValue(patternType);
      WsfAntennaPattern* auxAntennaPatternPtr =
         WsfAntennaPatternTypes::Get(WsfScenario::FromInput(aInput)).Find(patternType);
      if (auxAntennaPatternPtr == nullptr)
      {
         throw UtInput::BadValue(aInput);
      }
      mAuxAntennaPatternPtr = auxAntennaPatternPtr;
   }
   else if (antennaPtr->ProcessInput(aInput))
   {
      if (mAuxAntennaPtr == nullptr) // auxiliary antenna not yet allocated
      {
         mAuxAntennaPtr = tempAntennaPtr.release(); // Take ownership of the temporary object
      }
      rcvrPtr->SetAntenna(mAuxAntennaPtr);
   }
   else if (rcvrPtr->ProcessInputBlock(aInput)/* ||
            rcvrPtr->ProcessInput(aInput)*/)
   {
      if (mAuxRcvrPtr == nullptr) // auxiliary receiver not yet allocated
      {
         mAuxRcvrPtr = tempRcvrPtr.release(); // Take ownership of the temporary object
      }
   }
   else if (command == "blanking_threshold")
   {
      aInput.ReadValueOfType(mBlankingThreshold, UtInput::cRATIO);
   }
   else if ((command == "main_snr_thresholds") || (command == "main_jnr_thresholds"))
   {
      double detectionThreshold[2];
      aInput.ReadValueOfType(detectionThreshold[0], UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold[0], 0.0);
      aInput.ReadValueOfType(detectionThreshold[1], UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold[1], detectionThreshold[0]);
      UtVec2d::Set(mMainThresholds, detectionThreshold);
   }
   else if ((command == "auxiliary_snr_thresholds") || (command == "auxiliary_jnr_thresholds"))
   {
      double detectionThreshold[2];
      aInput.ReadValueOfType(detectionThreshold[0], UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold[0], 0.0);
      aInput.ReadValueOfType(detectionThreshold[1], UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold[1], detectionThreshold[0]);
      UtVec2d::Set(mAuxThresholds, detectionThreshold);
   }
   else if (command == "saturation_effect")
   {
      std::string  saturationCommand;
      UtInputBlock saturationBlock(aInput, "end_saturation_effect");
      while (saturationBlock.ReadCommand(saturationCommand))
      {
         if (saturationCommand == "no_saturation_effect")
         {
            mSaturationEffect = cSE_NOSATURATIONEFFECT;
         }
         else if (saturationCommand == "duty_cycle_limit_effect")
         {
            mSaturationEffect = cSE_DUTYCYCLELIMITEFFECT;
         }
         else if (saturationCommand == "duty_cycle_limit")
         {
            mSaturationEffect = cSE_DUTYCYCLELIMITEFFECT;
            aInput.ReadValue(mSaturationDutyCycle);
            aInput.ValueInClosedRange(mSaturationDutyCycle, 0.0, 1.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "target_blanking_effect")
   {
      UtInputBlock targetBlankingBlock(aInput, "end_target_blanking_effect");
      ProcessTargetBlankingBlock(targetBlankingBlock);
   }
   else
   {
      myCommand = WsfEW_PowerEffect::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
bool WsfEW_SLB_Effect::ProcessTargetBlankingBlock(UtInputBlock& aInputBlock)
{
   bool myCommand = true;

   while (aInputBlock.ReadCommand())
   {
      if (!ProcessTargetBlankingInput(aInputBlock.GetInput()))
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
   return myCommand;
}

bool WsfEW_SLB_Effect::ProcessTargetBlankingInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "no_target_blanking_effect")
   {
      mTargetBlankingEffect = cTB_NOTARGETBLANKING;
      mTargetBlankingTable.clear();
   }
   else if (command == "duty_cycle_probability_effect")
   {
      mTargetBlankingEffect = cTB_DUTYCYCLEPROBABILITY;
      mTargetBlankingTable.clear();
   }
   else if (command == "probabilities")
   {
      mTargetBlankingEffect = cTB_DUTYCYCLETABLE;
      mTargetBlankingTable.clear();

      std::string  probabilityCommand;
      UtInputBlock probabilityBlock(aInput, "end_probabilities");
      while (probabilityBlock.ReadCommand(probabilityCommand))
      {
         UtInput& probabilityInput = probabilityBlock.GetInput();
         if ((probabilityCommand == "duty_cycle") || (probabilityCommand == "pulse_density"))
         {
            double dutyCycle;
            probabilityInput.ReadValue(dutyCycle);
            probabilityInput.ValueGreaterOrEqual(dutyCycle, 0.0);

            double probabilty;
            probabilityInput.ReadValue(probabilty);
            probabilityInput.ValueGreaterOrEqual(probabilty, 0.0);

            if (!AddTargetBlankingProbability(dutyCycle, probabilty))
            {
               throw UtInput::BadValue(aInput,
                                       GetName() + " probability  previously defined for duty-cycle/pulse_density.");
            }
         }
      }
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }
   return myCommand;
}

// virtual
void WsfEW_SLB_Effect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PowerEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_SLB_Effect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   if (mBlankerState != cOFF)
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;
      WsfEW_PowerEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
   }

   double blanking(1.0);
   if (mBlankerState == cBLANKING)
   {
      auto bmi = mBlankingMap.find(mCurrentXmtrUniqueId);
      if (bmi != mBlankingMap.end())
      {
         blanking = 0.0;
         if (mDebug)
         {
            auto out = ut::log::debug() << "Sidelobe blanking factor.";
            out.AddNote() << "Blanking factor: " << blanking;
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Could not find blanking value for target. Blanking not applied.";
            out.AddNote() << "Target: " << mCurrentTargetId;
         }
      }
   }
   else if (mBlankerState == cPOWERLIMITED)
   {
      if (mDebug)
      {
         ut::log::debug() << "Jammer power outside blanker operating region. Blanking not applied.";
      }
   }
   else if (mBlankerState == cSTALLED)
   {
      if (mDebug)
      {
         ut::log::debug() << "Jammer duty cycle exceeded for blanker. Blanking not applied.";
      }
   }
   else if (mBlankerState == cMAINLOBE)
   {
      if (mDebug)
      {
         auto out = ut::log::debug() << "Current jammer signal resides in mainlobe for target. Blanking not applied.";
         out.AddNote() << "Target: " << mCurrentTargetId;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out =
            ut::log::debug() << "Current blanker state is 'Off' or not defined for target. Blanking not applied.";
         out.AddNote() << "Target: " << mCurrentTargetId;
      }
   }

   if (((aEW_EffectPtr->GetCoherencyMask() & cEC_COHERENT) != 0u) ||
       ((aEW_EffectPtr->GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
   {
      aEW_Effects.mEW_CoherentJammingEffect.mBlanking *= blanking;
   }

   if (((aEW_EffectPtr->GetCoherencyMask() & cEC_NONE) != 0u) ||
       ((aEW_EffectPtr->GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
   {
      aEW_Effects.mEW_NoiseJammingEffect.mBlanking *= blanking;
   }

   if ((aEW_EffectPtr->GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_PulseJammingEffect.mBlanking *= blanking;
   }

   if (mTargetBlanked)
   {
      aEW_Effects.mMask |= cEB_DROP_TRACK;
      if (mDebug && (mCurrentTargetId > nullptr))
      {
         auto out = ut::log::debug() << "Target blanked.";
         out.AddNote() << "Target: " << mCurrentTargetId;
      }
   }
}

// virtual
void WsfEW_SLB_Effect::UpdateEffects(double                                 aSimTime,
                                     const WsfEM_Interaction&               aTargetInteraction,
                                     const WsfEM_Interaction&               aJammerToTgtInteraction,
                                     const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                     WsfEW_Effects&                         aEW_Effects,
                                     WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEW_PowerEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Rcvr* radarRcvrPtr  = aTargetInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);
   WsfEM_Xmtr* radarXmtrPtr = aTargetInteraction.GetTransmitter();
   assert(radarXmtrPtr != nullptr);
   WsfPlatform* targetPtr = aTargetInteraction.GetTarget();

   // Set some variables as this is the first time they can be set or reset.
   mBlankerState  = cOFF;
   mTargetBlanked = false;

   double mainJammingPower = WsfRF_Jammer::ComputeJammerPower(aSimTime, jammerXmtrPtr, aTargetInteraction, aEW_EffectPtr);
   if (mainJammingPower <= 0.0)
   {
      return;
   }

   double mainSNR = radarRcvrPtr->ComputeSignalToNoise(mainJammingPower, aTargetInteraction.mClutterPower, 0.0);

   if (mMainThresholds[0] <= 1.0E-16)
   {
      mMainThresholds[0] = radarRcvrPtr->GetDetectionThreshold();
   }

   // Check to make sure the auxiliary receiver has been setup
   bool initializeAuxRcvr(false);
   if (mAuxRcvrPtr == nullptr)
   {
      // Make a new receiver for the auxiliary channel using the data from the main receiver
      mAuxRcvrPtr = new WsfEM_Rcvr(*radarRcvrPtr, mAuxAntennaPtr);
      assert(mAuxRcvrPtr != nullptr);

      // mAuxRcvrPtr->GetAntenna()->GetArticulatedPart()->SetYaw(0.0);
      // mAuxRcvrPtr->GetAntenna()->SetPitch(0.0);

      // Set the auxiliary antenna to force clearing of old patterns,
      // Assume it is default polarization
      mAuxRcvrPtr->SetAntennaPattern(mAuxAntennaPatternPtr->Clone(), WsfEM_Types::cPOL_DEFAULT, 0.0);

      initializeAuxRcvr = true;
   }

   if (mAuxRcvrPtr->GetFrequency() == 0.0)
   {
      mAuxRcvrPtr->SetFrequency(radarRcvrPtr->GetFrequency());
      initializeAuxRcvr = true;
   }

   if (mAuxRcvrPtr->GetBandwidth() == 0.0)
   {
      mAuxRcvrPtr->SetBandwidth(radarRcvrPtr->GetBandwidth());
      initializeAuxRcvr = true;
   }

   if (mAuxAntennaPtr == nullptr)
   {
      mAuxAntennaPtr = new WsfEM_Antenna(*(radarRcvrPtr->GetAntenna()));
      mAuxAntennaPtr->Initialize(radarRcvrPtr->GetArticulatedPart());
      mAuxRcvrPtr->SetAntenna(mAuxAntennaPtr);
      initializeAuxRcvr = true;
   }

   if (mExplicitAuxBeamTilt)
   {
      mAuxRcvrPtr->SetBeamTilt(mAuxBeamTilt);
      initializeAuxRcvr = true;
   }

   if (initializeAuxRcvr)
   {
      mAuxRcvrPtr->Initialize(*GetSimulation());
   }

   double            auxJammingPower = 0.0;
   WsfEM_Interaction auxInteraction;
   if (auxInteraction.BeginOneWayInteraction(jammerXmtrPtr, mAuxRcvrPtr, true, false) == 0)
   {
      // Copy the receiver beam position from the incoming interaction and set the position
      auxInteraction.SetReceiverBeamPosition(aTargetInteraction.mRcvrBeam);

      auxInteraction.SetTransmitterBeamPosition();

      // Compute the power from the jammer as seen by the receiver.
      auxJammingPower = WsfRF_Jammer::ComputeJammerPower(aSimTime, jammerXmtrPtr, auxInteraction, aEW_EffectPtr);
   }

   double auxSNR = mAuxRcvrPtr->ComputeSignalToNoise(auxJammingPower, auxInteraction.mClutterPower, 0.0);

   if (mAuxThresholds[0] < 1.0E-16)
   {
      mAuxThresholds[0] = mAuxRcvrPtr->GetDetectionThreshold();
   }

   // Check if the jammer signal is pulsed or not and get the duty-cycle if it is.
   double randomDraw = GetRandom().Uniform(0.0, 1.0);
   double dutyCycle  = aEW_EffectPtr->GetJammingPulseDensity(jammerXmtrPtr);

   // Need to calculate the target signal contribution if we are dealing with a
   // CW Jammer signal (i.e. duty-cycle is > 0.99) AND there is an interacting target
   // if ((dutyCycle >= randomDraw) && (targetPtr != 0) && (!targetPtr->IsFalseTarget())) //may be correct, need to check-AWK
   if ((dutyCycle >= 0.99) && (targetPtr != nullptr) && (!targetPtr->IsFalseTarget()))
   {
      double            mainTargetPower = 0.0;
      WsfEM_Interaction mainTargetInteraction;
      if (mainTargetInteraction.BeginTwoWayInteraction(radarXmtrPtr, targetPtr, radarRcvrPtr) == 0)
      {
         // Set the position of the antenna beam(s).
         mainTargetInteraction.SetTransmitterBeamPosition();
         mainTargetInteraction.SetReceiverBeamPosition(aTargetInteraction.mRcvrBeam);

         // Determine the radar cross section of the target.

         mainTargetInteraction.ComputeRadarSigAzEl();
         mainTargetInteraction.mRadarSig = WsfRadarSignature::GetValue(targetPtr,
                                                                       radarXmtrPtr,
                                                                       radarRcvrPtr,
                                                                       mainTargetInteraction.mRadarSigAz,
                                                                       mainTargetInteraction.mRadarSigEl,
                                                                       mainTargetInteraction.mRadarSigAz,
                                                                       mainTargetInteraction.mRadarSigEl);

         // Calculate the signal return.
         mainTargetPower = mainTargetInteraction.ComputeRF_TwoWayPower(mainTargetInteraction.mRadarSig);
      }

      double            auxTargetPower = 0.0;
      WsfEM_Interaction auxTargetInteraction;
      if (auxTargetInteraction.BeginTwoWayInteraction(radarXmtrPtr, targetPtr, mAuxRcvrPtr) == 0)
      {
         // Set the position of the antenna beam(s).
         auxTargetInteraction.SetTransmitterBeamPosition();
         auxTargetInteraction.SetReceiverBeamPosition(aTargetInteraction.mRcvrBeam);

         // Determine the radar cross section of the target.

         auxTargetInteraction.ComputeRadarSigAzEl();
         auxTargetInteraction.mRadarSig = WsfRadarSignature::GetValue(targetPtr,
                                                                      radarXmtrPtr,
                                                                      mAuxRcvrPtr,
                                                                      auxTargetInteraction.mRadarSigAz,
                                                                      auxTargetInteraction.mRadarSigEl,
                                                                      auxTargetInteraction.mRadarSigAz,
                                                                      auxTargetInteraction.mRadarSigEl);

         // Calculate the signal return.
         auxTargetPower = auxTargetInteraction.ComputeRF_TwoWayPower(auxTargetInteraction.mRadarSig);
      }


      // Get the powers to calculate the ratio
      // Note that the mainPower and auxPower are the jammer powers for each channel at this point
      if (mainTargetPower > mainJammingPower)
      {
         mainSNR =
            radarRcvrPtr->ComputeSignalToNoise(mainTargetPower, mainTargetInteraction.mClutterPower, mainJammingPower);
         mainJammingPower = mainTargetPower;

         if (mDebug)
         {
            auto out = ut::log::debug() << "Calculating main SNR.";
            out.AddNote() << "Main Target Power: " << mainTargetPower;
            out.AddNote() << "Main Jamming Power: " << mainJammingPower;
            out.AddNote() << "Main SNR: " << mainSNR;
         }
      }

      if (auxTargetPower > auxJammingPower)
      {
         auxSNR = radarRcvrPtr->ComputeSignalToNoise(auxTargetPower, auxTargetInteraction.mClutterPower, auxJammingPower);
         auxJammingPower = auxTargetPower;

         if (mDebug)
         {
            auto out = ut::log::debug() << "Calculating aux SNR.";
            out.AddNote() << "Aux Target Power: " << mainTargetPower;
            out.AddNote() << "Aux Jamming Power: " << mainJammingPower;
            out.AddNote() << "Aux SNR: " << mainSNR;
         }
      }
   }

   double auxToMainRatio = DBL_MAX;
   if (mainJammingPower > 0.0)
   {
      auxToMainRatio = auxJammingPower / mainJammingPower;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "WsfEW_SLB_Effect::UpdateEffects Report:";

      out.AddNote() << "AuxSNR: " << auxSNR;
      out.AddNote() << "Auxiliary Threshold: " << mAuxThresholds[0] << ", " << mAuxThresholds[1];
      out.AddNote() << "MainSNR: " << mainSNR;
      out.AddNote() << "Main Threshold: " << mMainThresholds[0] << ", " << mMainThresholds[1];
      out.AddNote() << "AuxToMainRatio: " << auxToMainRatio;
      out.AddNote() << "Blanking Threshold: " << mBlankingThreshold;
   }

   mBlankerState = cMAINLOBE;
   if ((mSaturationEffect == cSE_DUTYCYCLELIMITEFFECT) && (dutyCycle >= mSaturationDutyCycle))
   {
      mBlankerState = cSTALLED;
      mBlankingMap.clear();
      if (mDebug)
      {
         auto out = ut::log::debug() << "Blanking stalled due to a jamming signal Duty-Cycle with a Limitation.";
         out.AddNote() << "Duty-Cycle: " << dutyCycle;
         out.AddNote() << "Limitation: " << mSaturationDutyCycle;
      }
   }
   else if (auxToMainRatio >= mBlankingThreshold)
   {
      if (((mainSNR >= mMainThresholds[0]) && (mainSNR <= mMainThresholds[1])) &&
          ((auxSNR >= mAuxThresholds[0]) && (auxSNR <= mAuxThresholds[1])))
      {
         mBlankingMap[mCurrentXmtrUniqueId] = 0.0;
         mBlankerState                      = cBLANKING;
         if (mDebug)
         {
            auto out = ut::log::debug() << "Applied blanking using the AuxToMinRatio for comparison, blanking value.";
            out.AddNote() << "AuxToMinRatio: " << auxToMainRatio;
            out.AddNote() << "Blanking Value: " << mBlankingMap[mCurrentXmtrUniqueId];
         }
      }
      else
      {
         mBlankerState = cPOWERLIMITED;
      }
   }

   // Apply target blanking
   if ((mTargetBlankingEffect != cTB_NOTARGETBLANKING) && (mBlankerState == cBLANKING) && (targetPtr != nullptr) &&
       (!targetPtr->IsFalseTarget()))
   {
      double targetBlankingProbability = GetTargetBlankingProbability(dutyCycle);
      if (randomDraw <= targetBlankingProbability)
      {
         mTargetBlanked = true;
         if (mDebug)
         {
            auto out =
               ut::log::debug()
               << "Target has been blanked for a probability of being blanked for a duty-cycle and probability draw.";
            out.AddNote() << "Probability of Being Blanked: " << targetBlankingProbability;
            out.AddNote() << "Duty-Cycle: " << dutyCycle;
            out.AddNote() << "Probability Draw: " << randomDraw;
         }
      }
      else if (mDebug)
      {
         auto out =
            ut::log::debug()
            << "Target was NOT blanked for a probability of being blanked for a duty-cycle and probability draw.";
         out.AddNote() << "Probability of Being Blanked: " << targetBlankingProbability;
         out.AddNote() << "Duty-Cycle: " << dutyCycle;
         out.AddNote() << "Probability Draw: " << randomDraw;
      }
   }
}

//! Remove the specified jammer from the blanking map.
//!
//! @param aXmtrId The unique Id of the jammer transmitter.
// virtual
void WsfEW_SLB_Effect::RemoveJammerXmtr(unsigned int aXmtrId)
{
   auto bmi = mBlankingMap.find(aXmtrId);
   if (bmi != mBlankingMap.end())
   {
      mBlankingMap.erase(bmi);
      if (mDebug)
      {
         auto out = ut::log::debug() << "Effect removed xmtr.";
         out.AddNote() << "Effect: " << GetName();
         out.AddNote() << "Xmtr: " << aXmtrId;
      }
   }
}

//! Remove the specified jammer from the blanking map.
//!
//! @param aJammerXmtrPtr The pointer to the jammer transmitter.
// virtual
void WsfEW_SLB_Effect::RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr)
{
   unsigned int jammerId = aJammerXmtrPtr->GetUniqueId();
   auto         bmi      = mBlankingMap.find(jammerId);
   if (bmi != mBlankingMap.end())
   {
      mBlankingMap.erase(bmi);
      if (mDebug)
      {
         auto out = ut::log::debug() << "Effect removed xmtr.";
         out.AddNote() << "Effect: " << GetName();
         out.AddNote() << "Xmtr: " << aJammerXmtrPtr->GetUniqueId();
      }
   }
}

bool WsfEW_SLB_Effect::AddTargetBlankingProbability(double aDutyCycle, double aProbability)
{
   bool insertSuccess = true;
   auto tbti          = mTargetBlankingTable.begin();

   while (tbti != mTargetBlankingTable.end())
   {
      // sort inserts by increasing duty-cycle
      if (aDutyCycle < (*tbti).first)
      {
         break;
      }
      else if (aDutyCycle == (*tbti).first)
      {
         insertSuccess = false;
      }
      ++tbti;
   }

   if (insertSuccess == true)
   {
      mTargetBlankingTable.insert(tbti, std::make_pair(aDutyCycle, aProbability));
   }

   return insertSuccess;
}

double WsfEW_SLB_Effect::GetTargetBlankingProbability(double aDutyCycle) const
{
   double targetBlankingProbability = aDutyCycle;
   if (!mTargetBlankingTable.empty())
   {
      auto tbti  = mTargetBlankingTable.begin();
      auto tbtiP = mTargetBlankingTable.begin();

      while (tbti != mTargetBlankingTable.end())
      {
         if (aDutyCycle >= tbti->first)
         {
            tbtiP = tbti;
         }
         else
         {
            break;
         }
         ++tbti;
      }

      targetBlankingProbability = tbtiP->second;
      if ((tbti != tbtiP) && (tbti != mTargetBlankingTable.end()))
      {
         targetBlankingProbability +=
            (tbti->second - tbtiP->second) * (aDutyCycle - tbtiP->first) / (tbti->first - tbtiP->first);
      }
   }
   return targetBlankingProbability;
}
