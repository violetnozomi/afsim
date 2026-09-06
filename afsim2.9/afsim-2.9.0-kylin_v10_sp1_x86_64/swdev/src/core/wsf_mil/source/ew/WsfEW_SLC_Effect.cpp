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

#include "WsfEW_SLC_Effect.hpp"

#include <cfloat>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
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
#include "WsfEW_PolModEffect.hpp"
#include "WsfEW_PulseEffect.hpp"
#include "WsfEW_SLC_DegradeEffect.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfScenario.hpp"

WsfEW_SLC_Effect::WsfEW_SLC_Effect()
   : WsfEW_PowerEffect()
   , mAuxRcvrPtr(nullptr)
   , mAuxAntennaPtr(nullptr)
   , mAuxAntennaPatternPtr(nullptr)
   , mAuxBeamTilt(0.0)
   , mExplicitAuxBeamTilt(false)
   , mCancelationLockRatio(100)
   , mMinimumPulseWidth(0.0)
   , mLoopSettlingTime(0)
   , mNumberCancelers(0)
   ,
   // mCalculateSaturatedCR(false),
   mCancelationMethod(cNONE)
   , mCancelationTable()
   , mExplicitCancelationTable(false)
   , mCancelerState(cOFF)
   ,
   // mAuxChannelJammerPower(0.0),
   mCanceledJammers(0)
   , mCancelationMap()
{
   mAuxThresholds[0]  = 0.0;
   mAuxThresholds[1]  = DBL_MAX;
   mMainThresholds[0] = 0.0;
   mMainThresholds[1] = DBL_MAX;

   mBehavior      = cEB_JAMMER_POWER_EFFECT;
   mCoherencyMask = cEC_NONE | cEC_NONCOHERENT | cEC_COHERENT | cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
}

WsfEW_SLC_Effect::WsfEW_SLC_Effect(const WsfEW_SLC_Effect& aSrc)
   : WsfEW_PowerEffect(aSrc)
   , mAuxRcvrPtr(nullptr)
   , mAuxAntennaPtr(nullptr)
   , mAuxAntennaPatternPtr(aSrc.mAuxAntennaPatternPtr)
   , // Cloned in UpdateEffects(...) if required
   mAuxBeamTilt(aSrc.mAuxBeamTilt)
   , mExplicitAuxBeamTilt(aSrc.mExplicitAuxBeamTilt)
   , mCancelationLockRatio(aSrc.mCancelationLockRatio)
   , mMinimumPulseWidth(aSrc.mMinimumPulseWidth)
   , mLoopSettlingTime(aSrc.mLoopSettlingTime)
   , mNumberCancelers(aSrc.mNumberCancelers)
   ,
   // mCalculateSaturatedCR(aSrc.mCalculateSaturatedCR),
   mCancelationMethod(aSrc.mCancelationMethod)
   , mCancelationTable(aSrc.mCancelationTable)
   , mExplicitCancelationTable(aSrc.mExplicitCancelationTable)
   , mCancelerState(aSrc.mCancelerState)
   ,
   // mAuxChannelJammerPower(aSrc.mAuxChannelJammerPower),
   mCanceledJammers(aSrc.mCanceledJammers)
   , mCancelationMap(aSrc.mCancelationMap)
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
WsfEW_SLC_Effect::~WsfEW_SLC_Effect()
{
   delete mAuxRcvrPtr;
   delete mAuxAntennaPtr;
}

// virtual
WsfEW_Effect* WsfEW_SLC_Effect::Clone() const
{
   return new WsfEW_SLC_Effect(*this);
}

bool WsfEW_SLC_Effect::Initialize(WsfSimulation& aSimulation)
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

   if (mCancelationTable.empty())
   {
      auto out = ut::log::error() << "'cancelation_ratios' were not entered.";
      out.AddNote() << "Effect: " << GetName();

      ok = false;
   }
   else
   {
      for (int i = 1; i <= mNumberCancelers; ++i)
      {
         auto cti = mCancelationTable.find(i);
         if (cti == mCancelationTable.end())
         {
            if (mExplicitCancelationTable)
            {
               auto out = ut::log::error()
                          << "'cancelation_ratios' not entered for 'number_canceled_jammers' or 'jammer_canceled'";
               out.AddNote() << "Effect: " << GetName();
               out.AddNote() << "Number Not Entered: " << i;
               ok &= false;
               break;
            }
            else if ((mCancelationMethod == cNONE) && (i != 1))
            {
               mCancelationTable[i] = mCancelationTable[1];
            }
            else
            {
               auto out = ut::log::error() << "'cancelation_ratios' were not entered.";
               out.AddNote() << "Effect: " << GetName();
               ok &= false;
               break;
            }
         }
      }
   }

   return ok;
}

bool WsfEW_SLC_Effect::ProcessInput(UtInput& aInput)
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
   else if ((command == "number_of_canceller_channels") || (command == "number_canceller_channels") ||
            (command == "number_of_canceler_channels") || (command == "number_canceler_channels"))
   {
      aInput.ReadValue(mNumberCancelers);
   }
   else if ((command == "cancellation_lock_ratio") || (command == "cancelation_lock_ratio"))
   {
      aInput.ReadValueOfType(mCancelationLockRatio, UtInput::cRATIO);
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
   else if ((command == "cancellation_ratio") || (command == "cancelation_ratio"))
   {
      double ratio;
      aInput.ReadValueOfType(ratio, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(ratio, 0.0);

      if (mCancelationTable.find(1) != mCancelationTable.end())
      {
         mCancelationTable.erase(mCancelationTable.find(1), mCancelationTable.end());
      }

      if (!AddCancelationRatio(1, 0, ratio))
      {
         throw UtInput::BadValue(aInput, GetName() + " cancelation ratio previously defined for JNR.");
      }
   }
   else if (command == "saturation_ratio")
   {
      double ratio;
      aInput.ReadValueOfType(ratio, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(ratio, 0.0);

      if (mCancelationTable.find(0) != mCancelationTable.end())
      {
         mCancelationTable[0].clear();
      }

      if (!AddCancelationRatio(0, 0, ratio))
      {
         throw UtInput::BadValue(aInput, GetName() + " cancelation ratio previously defined for JNR.");
      }
   }
   else if ((command == "cancellation_ratios") || (command == "cancelation_ratios"))
   {
      mExplicitCancelationTable = false;
      mCancelationTable.clear();
      int          numCanceledJammers = 1;
      std::string  cancelationCommand;
      UtInputBlock cancelationBlock(aInput);
      while (cancelationBlock.ReadCommand(cancelationCommand))
      {
         UtInput& cancelationInput = cancelationBlock.GetInput();
         if ((cancelationCommand == "number_cancelled_jammers") || (cancelationCommand == "number_canceled_jammers"))
         {
            if (mCancelationMethod == cINCREMENTAL)
            {
               throw UtInput::BadValue(aInput, GetName() + " 'jammer_canceled' previously input, cannot mix cancelation types.");
            }
            mExplicitCancelationTable = true;
            mCancelationMethod        = cSUM;

            cancelationInput.ReadValue(numCanceledJammers);
            cancelationInput.ValueGreater(numCanceledJammers, 0);

            if (mCancelationTable.find(numCanceledJammers) != mCancelationTable.end())
            {
               mCancelationTable[numCanceledJammers].clear();
            }
         }
         else if ((cancelationCommand == "jammer_cancelled") || (cancelationCommand == "jammer_canceled"))
         {
            if (mCancelationMethod == cSUM)
            {
               throw UtInput::BadValue(aInput,
                                       GetName() +
                                          " 'number_canceled_jammers' previously input, cannot mix cancelation types.");
            }
            mExplicitCancelationTable = true;
            mCancelationMethod        = cINCREMENTAL;

            cancelationInput.ReadValue(numCanceledJammers);
            cancelationInput.ValueGreater(numCanceledJammers, 0);

            if (mCancelationTable.find(numCanceledJammers) != mCancelationTable.end())
            {
               mCancelationTable[numCanceledJammers].clear();
            }
         }
         else if (cancelationCommand == "saturation")
         {
            numCanceledJammers = 0;

            if (mCancelationTable.find(numCanceledJammers) != mCancelationTable.end())
            {
               mCancelationTable[numCanceledJammers].clear();
            }
         }
         else if (cancelationCommand == "saturation_ratio")
         {
            double ratio;
            cancelationInput.ReadValueOfType(ratio, UtInput::cRATIO);
            cancelationInput.ValueGreaterOrEqual(ratio, 0.0);

            if (mCancelationTable.find(0) != mCancelationTable.end())
            {
               mCancelationTable[0].clear();
            }

            if (!AddCancelationRatio(0, 0, ratio))
            {
               throw UtInput::BadValue(aInput, GetName() + " cancelation ratio previously defined for JNR.");
            }
         }
         else if ((cancelationCommand == "cancelation_ratio") || (cancelationCommand == "cancellation_ratio"))
         {
            double ratio;
            cancelationInput.ReadValueOfType(ratio, UtInput::cRATIO);
            cancelationInput.ValueGreaterOrEqual(ratio, 0.0);

            if (mCancelationTable.find(numCanceledJammers) != mCancelationTable.end())
            {
               mCancelationTable[numCanceledJammers].clear();
            }

            if (!AddCancelationRatio(numCanceledJammers, 0, ratio))
            {
               throw UtInput::BadValue(aInput, GetName() + " cancelation ratio previously defined for JNR.");
            }
         }
         else if (cancelationCommand == "jammer_to_noise")
         {
            double jnr;
            cancelationInput.ReadValueOfType(jnr, UtInput::cRATIO);
            cancelationInput.ValueGreaterOrEqual(jnr, 0.0);

            double ratio;
            cancelationInput.ReadValueOfType(ratio, UtInput::cRATIO);
            cancelationInput.ValueGreaterOrEqual(ratio, 0.0);

            if (!AddCancelationRatio(numCanceledJammers, jnr, ratio))
            {
               throw UtInput::BadValue(aInput, GetName() + " cancelation ratio previously defined for JNR.");
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (mCancelationTable.empty())
      {
         throw UtInput::BadValue(aInput, GetName() + " need cancelation ratio(s).");
      }
   }
   else if ((command == "minimum_cancelled_pulse_width") || (command == "minimum_pulse_width"))
   {
      aInput.ReadValueOfType(mMinimumPulseWidth, UtInput::cTIME);
      aInput.ValueGreater(mMinimumPulseWidth, 0.0);
   }
   else if ((command == "loop_settling_time") || (command == "canceller_settling_time"))
   {
      aInput.ReadValueOfType(mLoopSettlingTime, UtInput::cTIME);
      aInput.ValueGreater(mLoopSettlingTime, 0.0);
   }
   else
   {
      myCommand = WsfEW_PowerEffect::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfEW_SLC_Effect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PowerEffect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_SLC_Effect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   if (mCancelerState != cOFF)
   {
      aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;
   }

   double cancelation(1.0);
   if ((mCancelerState == cCANCELING) || (mCancelerState == cSTALLED))
   {
      WsfEW_PowerEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);

      auto cmi = mCancelationMap.find(mCurrentXmtrUniqueId);
      if (cmi != mCancelationMap.end())
      {
         cancelation = cmi->second.mCancelationValue * cmi->second.mSLC_DegradeFactor;
         if (mDebug && (mCancelerState == cCANCELING))
         {
            auto out = ut::log::debug() << "Applied sidelobe cancelation factor.";
            out.AddNote() << "Cancellation: " << cancelation;
         }
         else if (mDebug && (mCancelerState == cSTALLED))
         {
            auto out =
               ut::log::debug() << "Number of jammers has exceeded sidelobe canceler capacity, canceler stalled.";
            out.AddNote() << "Sidelobe Cancellation Factor: " << cancelation;
         }
      }
      else
      {
         if (mDebug)
         {
            ut::log::debug() << "Could not find cancelation value for transmitter, sidelobe cancelation not applied.";
         }
      }
   }
   // else if (mCancelerState == cSTALLED)
   //{
   //    if (mDebug)
   //    {
   //       cout << "  *Number of jammers has exceeded sidelobe canceler capacity,"
   //          << " canceler stalled, sidelobe cancelation not applied." << endl;
   //    }
   // }
   else if (mCancelerState == cMAINLOBE)
   {
      if (mDebug)
      {
         ut::log::debug() << "Current jammer signal resides in mainlobe, sidelobe cancelation not applied.";
      }
   }
   else
   {
      if (mDebug)
      {
         ut::log::debug()
            << "Current canceler state is 'Off' or not defined for target, sidelobe cancelation not applied.";
      }
   }

   cancelation = std::min(cancelation, aEW_Effects.mEW_CoherentJammingEffect.mCancelation);
   if (((aEW_EffectPtr->GetCoherencyMask() & cEC_COHERENT) != 0u) ||
       ((aEW_EffectPtr->GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
   {
      aEW_Effects.mEW_CoherentJammingEffect.mCancelation =
         std::min(aEW_Effects.mEW_CoherentJammingEffect.mCancelation, cancelation);
   }

   if (((aEW_EffectPtr->GetCoherencyMask() & cEC_NONE) != 0u) ||
       ((aEW_EffectPtr->GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
   {
      aEW_Effects.mEW_NoiseJammingEffect.mCancelation =
         std::min(aEW_Effects.mEW_NoiseJammingEffect.mCancelation, cancelation);
   }

   if ((aEW_EffectPtr->GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_PulseJammingEffect.mCancelation =
         std::min(aEW_Effects.mEW_PulseJammingEffect.mCancelation, cancelation);
   }
}

// virtual
void WsfEW_SLC_Effect::UpdateEffects(double                                 aSimTime,
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

   // Set some variables as this is the first time they can be set or reset.
   if (mCancelerState != cSTALLED)
   {
      mCancelerState = cOFF;
   }

   double mainJammingPower = WsfRF_Jammer::ComputeJammerPower(aSimTime, jammerXmtrPtr, aTargetInteraction, aEW_EffectPtr);
   if (mainJammingPower <= radarRcvrPtr->GetNoisePower())
   {
      RemoveJammerXmtr(mCurrentXmtrUniqueId);
      return;
   }

   double mainJNR = radarRcvrPtr->ComputeSignalToNoise(mainJammingPower, aTargetInteraction.mClutterPower, 0.0);

   if (mMainThresholds[0] <= 1.0E-16)
   {
      mMainThresholds[0] = 1.0;
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

   if (auxJammingPower <= radarRcvrPtr->GetNoisePower())
   {
      RemoveJammerXmtr(mCurrentXmtrUniqueId);
      return;
   }

   double auxJNR = mAuxRcvrPtr->ComputeSignalToNoise(auxJammingPower, auxInteraction.mClutterPower, 0.0);

   if (mAuxThresholds[0] < 1.0E-16)
   {
      mAuxThresholds[0] = 1.0;
   }

   double auxToMainRatio = DBL_MAX;
   if (mainJammingPower > 0.0)
   {
      auxToMainRatio = auxJammingPower / mainJammingPower;
   }

   if (mDebug)
   {
      if (auxInteraction.mFailedStatus == 0)
      {
         auto out = ut::log::debug() << "WsfEW_PowerEffect::UpdateEffects Report:";
         { // RAII block
            auto t1 = out.AddNote() << "AuxToMainRatio: " << UtMath::SafeLinearToDB(auxToMainRatio) << " dB";
            t1.AddNote() << "Min Threshold: " << UtMath::SafeLinearToDB(mCancelationLockRatio) << " dB";
         }
         { // RAII block
            auto t2 = out.AddNote() << "MainJNR: " << UtMath::SafeLinearToDB(mainJNR) << " dB";
            t2.AddNote() << "Min Threshold: " << UtMath::SafeLinearToDB(mMainThresholds[0]) << " dB";
            t2.AddNote() << "Max Threshold: " << UtMath::SafeLinearToDB(mMainThresholds[1]) << " dB";
         }
         { // RAII block
            auto t3 = out.AddNote() << "AuxJNR: " << UtMath::SafeLinearToDB(auxJNR) << " dB";
            t3.AddNote() << "Min Threshold: " << UtMath::SafeLinearToDB(mAuxThresholds[0]) << " dB";
            t3.AddNote() << "Max Threshold: " << UtMath::SafeLinearToDB(mAuxThresholds[1]) << " dB";
         }
      }
      else
      {
         auto out =
            ut::log::debug() << "Auxiliary SLC interaction failed! START Printing auxiliary interaction output.";
         auxInteraction.Print(out);
      }
   }

   // Check if the jammer signal is pulsed or not and get the pulse width if it is.
   double pulseWidth = jammerXmtrPtr->GetPulseWidth();
   if ((pulseWidth <= 0.0) && (aEW_EffectPtr != nullptr))
   {
      pulseWidth = aEW_EffectPtr->GetJammingPulseDensity() * radarXmtrPtr->GetPulseRepetitionInterval();
   }

   mCancelerState = cMAINLOBE;
   if ((auxToMainRatio >= mCancelationLockRatio) && ((mainJNR >= mMainThresholds[0]) && (mainJNR <= mMainThresholds[1])) &&
       ((auxJNR >= mAuxThresholds[0]) && (auxJNR <= mAuxThresholds[1])) && (pulseWidth >= mMinimumPulseWidth))
   {
      // Set default values and check for the Polarization Modulation effect.
      double                   degradationValue     = 1.0;
      int                      num_SLC_ChannelsUsed = 1;
      double                   polModSwitchRate     = 0.0;
      WsfEW_SLC_DegradeEffect* slcDegradEffectPtr   = dynamic_cast<WsfEW_SLC_DegradeEffect*>(aEW_EffectPtr);
      if (slcDegradEffectPtr != nullptr)
      {
         degradationValue     = slcDegradEffectPtr->GetSLC_DegradationValue(mEffectingSystemId);
         num_SLC_ChannelsUsed = slcDegradEffectPtr->GetSLC_ChannelsSaturated(mEffectingSystemId);

         WsfEW_PolModEffect* polModEffectPtr = dynamic_cast<WsfEW_PolModEffect*>(aEW_EffectPtr);
         if (polModEffectPtr != nullptr)
         {
            polModSwitchRate = polModEffectPtr->GetPolarizationSwitchingRate(mEffectingSystemId);
            if ((num_SLC_ChannelsUsed > 1) && (polModSwitchRate > 0.0) && ((1.0 / polModSwitchRate) < mLoopSettlingTime))
            {
               // Negate any effects by the technique be it degradation or channels used.
               num_SLC_ChannelsUsed = 1;
               degradationValue     = 1.0;
            }
         }
      }

      if (mCancelationMap.find(mCurrentXmtrUniqueId) == mCancelationMap.end())
      {
         mCanceledJammers += num_SLC_ChannelsUsed;
         mCancelationMap[mCurrentXmtrUniqueId].Reset();
      }
      else if (num_SLC_ChannelsUsed > mCancelationMap[mCurrentXmtrUniqueId].mChannelsCanceled)
      {
         // Need to adjust since the last calculation was using less canceler channels
         mCanceledJammers -= mCancelationMap[mCurrentXmtrUniqueId].mChannelsCanceled;
         mCanceledJammers += num_SLC_ChannelsUsed;
      }
      mCancelationMap[mCurrentXmtrUniqueId].mChannelsCanceled =
         std::max(mCancelationMap[mCurrentXmtrUniqueId].mChannelsCanceled, num_SLC_ChannelsUsed);
      mCancelationMap[mCurrentXmtrUniqueId].mJammerToNoise = mainJNR;
      mCancelationMap[mCurrentXmtrUniqueId].mSLC_DegradeFactor =
         degradationValue * mCancelationMap[mCurrentXmtrUniqueId].mSLC_DegradeFactor;
      mCancelationMap[mCurrentXmtrUniqueId].mPolModSwitchRate =
         std::max(mCancelationMap[mCurrentXmtrUniqueId].mPolModSwitchRate, polModSwitchRate);

      mCancelerState = cCANCELING;
      if (mCanceledJammers > mNumberCancelers)
      {
         mCancelerState = cSTALLED;
      }

      int canceledJammers = mCanceledJammers;
      if (mCancelationMethod == cINCREMENTAL)
      {
         canceledJammers = 0;
      }
      auto cmi = mCancelationMap.begin();
      for (; cmi != mCancelationMap.end(); ++cmi)
      {
         if (mCancelationMethod == cINCREMENTAL)
         {
            canceledJammers += cmi->second.mChannelsCanceled;
         }
         cmi->second.mCancelationValue =
            GetCancelationFactor(canceledJammers, cmi->second.mJammerToNoise) * cmi->second.mSLC_DegradeFactor;
      }

      if (mDebug)
      {
         auto out = ut::log::debug() << "Reporting Jammer, interaction and TechResult data.";
         { // RAII block
            auto note = out.AddNote() << "JammerData:";
            note.AddNote() << "DegradationValue: " << degradationValue;
            note.AddNote() << "SLC_ChannelsUsed: " << num_SLC_ChannelsUsed;
         }
         { // RAII block
            auto note = out.AddNote() << "InteractionData:";
            note.AddNote() << "CanceledJammers: " << mCanceledJammers;
            note.AddNote() << "MainJNR: " << mainJNR;
            note.AddNote() << "CancelationFactor: " << GetCancelationFactor(mCanceledJammers, mainJNR);
         }
         { // RAII block
            auto note = out.AddNote() << "TechResultData:";
            note.AddNote() << "DegradationFactor: " << mCancelationMap[mCurrentXmtrUniqueId].mSLC_DegradeFactor;
            note.AddNote() << "MainJNR: " << mCancelationMap[mCurrentXmtrUniqueId].mJammerToNoise;
            note.AddNote() << "ChannelsCanceled: " << mCancelationMap[mCurrentXmtrUniqueId].mChannelsCanceled;
         }
      }
   }
}

//! Remove the specified jammer from the cancelation map.
//!
//! @param aXmtrId The unique Id of the jammer transmitter.
// virtual
void WsfEW_SLC_Effect::RemoveJammerXmtr(unsigned int aXmtrId)
{
   auto cmi = mCancelationMap.find(aXmtrId);
   if (cmi != mCancelationMap.end())
   {
      mCanceledJammers -= cmi->second.mChannelsCanceled;
      mCancelationMap.erase(cmi);
      if (mDebug)
      {
         auto out = ut::log::debug() << "Removed xmtr.";
         out.AddNote() << "Effect: " << GetName();
         out.AddNote() << "Xmtr: " << aXmtrId;
      }
   }
}

//! Remove the specified jammer from the cancelation  map.
//!
//! @param aJammerXmtrPtr The pointer to the jammer transmitter.
// virtual
void WsfEW_SLC_Effect::RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr)
{
   unsigned int jammerId = aJammerXmtrPtr->GetUniqueId();
   auto         cmi      = mCancelationMap.find(jammerId);
   if (cmi != mCancelationMap.end())
   {
      mCanceledJammers -= cmi->second.mChannelsCanceled;
      mCancelationMap.erase(cmi);
      if (mDebug)
      {
         auto out = ut::log::debug() << "Removed xmtr.";
         out.AddNote() << "Effect: " << GetName();
         out.AddNote() << "Xmtr: " << aJammerXmtrPtr->GetUniqueId();
      }
   }
}


//! Add the cancelation ratio to the list with the associated lower jammer-to-noise bound.//!
//! @param aNumCanceledJammers .
//! @param aJammerToNoiseRatio The lower frequency bound of the antenna pattern.
//! @param aCancelationRatio The polarization of the antenna pattern. Defaults to DEFAULT
//! @returns True if the insert was successful, False if a pattern associated with the frequency already exists.
bool WsfEW_SLC_Effect::AddCancelationRatio(int aNumCanceledJammers, double aJammerToNoiseRatio, double aCancelationRatio)
{
   bool              insertSuccess = true;
   JNR_To_CR_PairVec crVec         = mCancelationTable[aNumCanceledJammers];
   auto              crvi          = crVec.begin();

   while (crvi != crVec.end())
   {
      // sort inserts by increasing jnr
      if (aJammerToNoiseRatio < (*crvi).first)
      {
         break;
      }
      else if (aJammerToNoiseRatio == (*crvi).first)
      {
         insertSuccess = false;
      }
      ++crvi;
   }

   if (insertSuccess == true)
   {
      crVec.insert(crvi, std::make_pair(aJammerToNoiseRatio, aCancelationRatio));
      mCancelationTable[aNumCanceledJammers] = crVec;
   }

   return insertSuccess;
}

//! Return the cancelation ratio for the supplied inputs.
//!
//! @param aNumCanceledJammers The number of canceled jammers.
//! @param aJammerPower_NoiseRatio The jammer power or jammer-to-noise ratio.
//! @return Cancelation ratio at the associated input data or 1.0 if there is no associated value found.
double WsfEW_SLC_Effect::GetCancelationRatio(int aNumCanceledJammers, double aJammerToNoiseRatio) const
{
   double cancelationRatio = 1.0;
   if (!mCancelationTable.empty())
   {
      auto crti = mCancelationTable.find(aNumCanceledJammers);
      if (crti == mCancelationTable.end())
      {
         if (mCanceledJammers > mNumberCancelers)
         {
            // get the saturation ratio table
            crti = mCancelationTable.find(0);
         }

         if (crti == mCancelationTable.end())
         {
            // return the default ratio table
            crti = mCancelationTable.find(1);
         }
      }

      if (crti != mCancelationTable.end())
      {
         JNR_To_CR_PairVec                 crVec  = crti->second;
         JNR_To_CR_PairVec::const_iterator crvi   = crVec.begin();
         JNR_To_CR_PairVec::const_iterator crviCR = crVec.begin();

         while (crvi != crVec.end())
         {
            if (aJammerToNoiseRatio >= crvi->first)
            {
               crviCR = crvi;
            }
            else
            {
               break;
            }
            ++crvi;
         }

         cancelationRatio = crviCR->second;
         if ((crvi != crviCR) && crvi != crVec.end())
         {
            cancelationRatio +=
               (crvi->second - crviCR->second) * (aJammerToNoiseRatio - crviCR->first) / (crvi->first - crviCR->first);
         }
      }
   }
   return cancelationRatio;
}

//! Return the cancelation factor that can be multiplied by the jammer power for the supplied inputs.
//!
//! @param aNumCanceledJammers The number of canceled jammers.
//! @param aJammerToNoiseRatio the jammer-to-noise ratio.
//! @return the cancelation factor or 1.0 if there is no associated threshold.
double WsfEW_SLC_Effect::GetCancelationFactor(int aNumCanceledJammers, double aJammerToNoiseRatio) const
{
   double cancelationFactor = 1.0 / GetCancelationRatio(aNumCanceledJammers, aJammerToNoiseRatio);

   return cancelationFactor;
}
