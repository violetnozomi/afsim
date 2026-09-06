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

#include "WsfEW_TrackEffect.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfEW_EA_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_Result.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackObserver.hpp"

WsfEW_TrackEffect::WsfEW_TrackEffect()
   : WsfEW_PulseEffect()
   , mTrackEffectDataMap()
   , mApplyCurrentEffect(false)
   , mCallbacks()
{
   mCoherencyMask = cEC_COHERENT; // Set to coherent as most of the time we don't want this affecting the SNR
}

WsfEW_TrackEffect::WsfEW_TrackEffect(const WsfEW_TrackEffect& aSrc)
   : WsfEW_PulseEffect(aSrc)
   , mTrackEffectDataMap(aSrc.mTrackEffectDataMap)
   , mApplyCurrentEffect(aSrc.mApplyCurrentEffect)
   , mCallbacks()
{
}

// virtual
WsfEW_Effect* WsfEW_TrackEffect::Clone() const
{
   return new WsfEW_TrackEffect(*this);
}

// virtual
bool WsfEW_TrackEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_PulseEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      // For now, just affect sensors
      mSystemFunctionMask = cES_SENSOR;
   }

   mCallbacks.Add(WsfObserver::SensorModeDeactivated(&aSimulation).Connect(&WsfEW_TrackEffect::SensorModeDeactivated, this));
   mCallbacks.Add(WsfObserver::SensorTrackDropped(&aSimulation).Connect(&WsfEW_TrackEffect::SensorTrackDropped, this));

   return ok;
}

// virtual
bool WsfEW_TrackEffect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "track_behavior")
   {
      std::string trackEffect;
      aInput.ReadValue(trackEffect);
      if (trackEffect == "maintain")
      {
         mBehavior = cEB_MAINTAIN_TRACK;
      }
      else if (trackEffect == "drop")
      {
         mBehavior = cEB_DROP_TRACK;
      }
      else if (trackEffect == "distort")
      {
         mBehavior = cEB_TRACK_ERROR;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Specified track effect type unknown.");
      }
   }
   else
   {
      myCommand = WsfEW_PulseEffect::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
void WsfEW_TrackEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   if (mApplyCurrentEffect)
   {
      WsfEW_PulseEffect::ApplyUnmitigatedEffect(aEW_Effects);
      if (GetBehavior() == cEB_MAINTAIN_TRACK)
      {
         aEW_Effects.mMask &= ~cEB_DROP_TRACK;
         aEW_Effects.mMask |= cEB_MAINTAIN_TRACK;
         if (mDebug)
         {
            ut::log::debug() << "Unmitigated Track: Maintaining Track.";
         }
      }
      else if (GetBehavior() == cEB_DROP_TRACK)
      {
         aEW_Effects.mMask &= ~cEB_MAINTAIN_TRACK;
         aEW_Effects.mMask |= cEB_DROP_TRACK;
         if (mDebug)
         {
            ut::log::debug() << "Unmitigated Track: Dropping Track.";
         }
      }
      else if (GetBehavior() == cEB_TRACK_ERROR)
      {
         aEW_Effects.mMask |= cEB_TRACK_ERROR;
         aEW_Effects.mEW_Error.mAzimuth = std::max(aEW_Effects.mEW_Error.mAzimuth, GetAzimuthError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mElevation =
            std::max(aEW_Effects.mEW_Error.mElevation, GetElevationError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mRange    = std::max(aEW_Effects.mEW_Error.mRange, GetRangeError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mVelocity = std::max(aEW_Effects.mEW_Error.mVelocity, GetVelocityError(mEffectedSystemId));

         if (mDebug)
         {
            auto out = ut::log::debug() << "Unmitigated Track:";
            out.AddNote() << "Azimuth Error: " << UtMath::cDEG_PER_RAD * aEW_Effects.mEW_Error.mAzimuth << " deg";
            out.AddNote() << "Elevation Error: " << UtMath::cDEG_PER_RAD * aEW_Effects.mEW_Error.mElevation << " deg";
            out.AddNote() << "Range Error: " << aEW_Effects.mEW_Error.mRange << " m";
            out.AddNote() << "Velocity Error: " << aEW_Effects.mEW_Error.mVelocity << " m/s";
         }
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Undefined or improper Track Effect behavior.";
         out.AddNote() << "Behavior: " << GetBehavior();
      }
   }
   else if (mDebug)
   {
      auto out = ut::log::debug() << "Did not apply any effects for behavior.";
      out.AddNote() << "Behavior: " << GetBehavior();
   }
}

// virtual
void WsfEW_TrackEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   if (mApplyCurrentEffect)
   {
      // See what the EP Mitigation Effect is and set the behavior
      if (GetBehavior() == cEB_MAINTAIN_TRACK)
      {
         aEW_Effects.mMask &= ~cEB_DROP_TRACK;
         aEW_Effects.mMask |= cEB_MAINTAIN_TRACK;
         if (mDebug)
         {
            ut::log::debug() << "Mitigated Track: Maintaining Track.";
         }
      }
      else if (GetBehavior() == cEB_DROP_TRACK)
      {
         aEW_Effects.mMask &= ~cEB_MAINTAIN_TRACK;
         aEW_Effects.mMask |= cEB_DROP_TRACK;
         if (mDebug)
         {
            ut::log::debug() << "Mitigated Track: Dropping Track.";
         }
      }
      else if (GetBehavior() == cEB_TRACK_ERROR)
      {
         aEW_Effects.mMask |= cEB_TRACK_ERROR;
         aEW_Effects.mEW_Error.mAzimuth = std::min(aEW_Effects.mEW_Error.mAzimuth, GetAzimuthError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mElevation =
            std::min(aEW_Effects.mEW_Error.mElevation, GetElevationError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mRange    = std::min(aEW_Effects.mEW_Error.mRange, GetRangeError(mEffectedSystemId));
         aEW_Effects.mEW_Error.mVelocity = std::min(aEW_Effects.mEW_Error.mVelocity, GetVelocityError(mEffectedSystemId));

         if (mDebug)
         {
            auto out = ut::log::debug() << "Mitigated Track:";
            out.AddNote() << "Azimuth Error: " << UtMath::cDEG_PER_RAD * aEW_Effects.mEW_Error.mAzimuth << " deg";
            out.AddNote() << "Elevation Error: " << UtMath::cDEG_PER_RAD * aEW_Effects.mEW_Error.mElevation << " deg";
            out.AddNote() << "Range Error: " << aEW_Effects.mEW_Error.mRange << " m";
            out.AddNote() << "Velocity Error: " << aEW_Effects.mEW_Error.mVelocity << " m/s";
         }
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Undefined or improper Track Effect behavior.";
         out.AddNote() << "Behavior: " << GetBehavior();
      }
   }
   else if (mDebug)
   {
      auto out = ut::log::debug() << "Did not apply any effects for behavior.";
      out.AddNote() << "Behavior: " << GetBehavior();
   }
}

// virtual
void WsfEW_TrackEffect::UpdateEffects(double                                 aSimTime,
                                      const WsfEM_Interaction&               aTargetInteraction,
                                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                      WsfEW_Effects&                         aEW_Effects,
                                      WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   mApplyCurrentEffect = true;

   WsfEW_PulseEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   if (!EffectDataExists(mEffectedSystemId))
   {
      InsertNewEffectData(mEffectedSystemId, mEffectDataMap[nullptr]->Clone());
   }

   WsfEM_Rcvr* radarRcvrPtr = aTargetInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);

   if (GetEP_Ptr() != nullptr)
   {
      SetRequiredStoN(mEffectedSystemId, radarRcvrPtr->GetDetectionThreshold());
   }

   if (GetLastUpdateTime(mEffectedSystemId) <= 0.0)
   {
      SetLastUpdateTime(mEffectedSystemId, aSimTime);
   }


   WsfEM_Interaction jammerInteraction;
   double            jammerPower = WsfRF_Jammer::ComputeJammerPower(aSimTime,
                                                         aJammerToTgtInteraction.GetTransmitter(),
                                                         aTargetInteraction,
                                                         jammerInteraction,
                                                         this);

   WsfEW_Result* ewJammerResult = WsfEW_Result::Find(jammerInteraction);
   jammerPower *= ewJammerResult->mEW_Effects.mEW_SignalEffect.mSignalPowerGain *
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mModulationGain *
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                  // ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *  // already applied
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *
                  // ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mRepeaterFactor *    // already applied
                  ewJammerResult->mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain * GetRepeaterFactor() *
                  GetJammingPowerGain(mEffectedSystemId) * GetJtoSGain(mEffectedSystemId, cEC_COHERENT) *
                  GetModulationGain(mEffectedSystemId) * GetSignalPowerGain(mEffectedSystemId) *
                  GetRadiusFactor(mEffectedSystemId);

   double ston = radarRcvrPtr->ComputeSignalToNoise(aTargetInteraction.mRcvdPower, aTargetInteraction.mClutterPower, 0.0);
   bool   draw = false;
   double jtos = std::numeric_limits<double>::lowest();
   if (aTargetInteraction.mRcvdPower > 0.0)
   {
      jtos = jammerPower / aTargetInteraction.mRcvdPower;
   }
   else if (mDebug)
   {
      ut::log::debug()
         << "WsfEW_TrackEffect::UpdateEffects: Target Interaction failed due to no signal being detected.";
   }

   if ((jtos >= GetRequiredJtoS(mEffectedSystemId)) && (ston >= GetRequiredStoN(mEffectedSystemId)))
   {
      if ((GetPreviousJtoS(mEffectedSystemId) < GetRequiredJtoS(mEffectedSystemId) ||
           (GetPreviousStoN(mEffectedSystemId) < GetRequiredStoN(mEffectedSystemId))))
      {
         draw = true;
      }
      if (mDebug)
      {
         auto out = ut::log::debug() << "WsfEW_TrackEffect::UpdateEffects J/S and S/N(if available) report.";
         out.AddNote() << "J/S: " << UtMath::SafeLinearToDB(jtos) << " dB";
         if (GetEP_Ptr() != nullptr)
         {
            out.AddNote() << "S/N: " << UtMath::SafeLinearToDB(ston) << " dB";
         }
      }
   }
   else
   {
      mApplyCurrentEffect = false;
      if (mDebug)
      {
         auto out = ut::log::debug() << "WsfEW_TrackEffect::UpdateEffects: J/S and S/N report.";
         if (jtos < GetRequiredJtoS(mEffectedSystemId))
         {
            out.AddNote() << "Insufficient J/S of " << UtMath::SafeLinearToDB(jtos) << " dB";
         }
         if ((GetEP_Ptr() != nullptr) && (ston < GetRequiredStoN(mEffectedSystemId)))
         {
            out.AddNote() << "Insufficient S/N of " << UtMath::SafeLinearToDB(ston) << " dB";
         }
      }
   }
   SetPreviousJtoS(mEffectedSystemId, jtos);
   SetPreviousStoN(mEffectedSystemId, ston);

   // Always need to keep data current as the jammer is shifting the walkoff no matter if the receiver sees it or not.
   double elapsedTime = aSimTime - GetLastUpdateTime(mEffectedSystemId);
   for (unsigned int i = 0; i < TrackEffectData::cLAST_ERROR_TYPE; ++i)
   {
      TrackEffectData::WalkoffData& walkoffData = GetWalkoffData(mEffectedSystemId, i);
      if (draw)
      {
         walkoffData.mDelayTime   = walkoffData.GetDelayTime(this, jtos);
         walkoffData.mHoldoutTime = walkoffData.GetHoldoutTime(this, jtos);
         walkoffData.mRecycleTime = walkoffData.GetRecycleTime(this, jtos);
         if (walkoffData.mRecycle)
         {
            walkoffData.Recycle(aSimTime);
         }
         if (mDebug)
         {
            auto out = ut::log::debug() << "Draw Times: ";
            out.AddNote() << "Delay Time: " << walkoffData.mDelayTime;
            out.AddNote() << "Holdout Time: " << walkoffData.mHoldoutTime;
         }
      }

      if (walkoffData.mLastCycleTime <= 0.0)
      {
         walkoffData.mLastCycleTime = aSimTime;
      }

      double elapsedCycleTime = (aSimTime - walkoffData.mLastCycleTime);

      if (elapsedCycleTime < walkoffData.mDelayTime)
      {
         // Check for delay conditions
         mApplyCurrentEffect = false;
         if (mDebug)
         {
            auto out = ut::log::debug() << "Delay time note met for cycle.";
            out.AddNote() << "Delay Time: " << elapsedCycleTime + walkoffData.mLastCycleTime;
            out.AddNote() << "Start: " << walkoffData.mLastCycleTime;
            out.AddNote() << "Delay: " << walkoffData.mDelayTime + walkoffData.mLastCycleTime;
            out.AddNote() << "End: " << walkoffData.mHoldoutTime + walkoffData.mLastCycleTime;
         }
      }
      else if ((elapsedCycleTime >= walkoffData.mHoldoutTime) || (walkoffData.mTrackError >= walkoffData.mHoldoutTarget))
      {
         // Check for holdout conditions
         if (walkoffData.mRecycle &&
             ((elapsedCycleTime >= walkoffData.mRecycleTime) || (walkoffData.mTrackError >= walkoffData.mHoldoutTarget)))
         {
            walkoffData.mRecycleTime = walkoffData.GetRecycleTime(this, jtos);
            walkoffData.Recycle(aSimTime);
         }

         if ((GetBehavior() == cEB_MAINTAIN_TRACK) || (GetBehavior() == cEB_DROP_TRACK))
         {
            // Stop applying the effect at the holdout/stop time
            mApplyCurrentEffect = false;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Stop/Holdout time met for cycle.";
               out.AddNote() << "Stop/Holdout time: " << elapsedCycleTime + walkoffData.mLastCycleTime;
               out.AddNote() << "Start: " << walkoffData.mLastCycleTime;
               out.AddNote() << "Delay: " << walkoffData.mDelayTime + walkoffData.mLastCycleTime;
               out.AddNote() << "End: " << walkoffData.mHoldoutTime + walkoffData.mLastCycleTime;
            }
         }
      }
      else if ((elapsedCycleTime >= walkoffData.mHoldoutTime) || (walkoffData.mTrackError >= walkoffData.mHoldoutTarget) ||
               ((walkoffData.mRecycleTime > 0.0) && (elapsedCycleTime >= walkoffData.mRecycleTime)))
      {
         // Check for holdout conditions
         if (walkoffData.mRecycle && (elapsedCycleTime >= walkoffData.mRecycleTime))
         {
            walkoffData.mRecycleTime = walkoffData.GetRecycleTime(this, jtos);
            walkoffData.Recycle(aSimTime);
         }
         else if ((GetBehavior() == cEB_MAINTAIN_TRACK) || (GetBehavior() == cEB_DROP_TRACK))
         {
            // Stop applying the effect at the holdout/stop time
            mApplyCurrentEffect = false;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Stop/Holdout time met for cycle.";
               out.AddNote() << "Stop/Holdout time: " << elapsedCycleTime + walkoffData.mLastCycleTime;
               out.AddNote() << "Start: " << walkoffData.mLastCycleTime;
               out.AddNote() << "Delay: " << walkoffData.mDelayTime + walkoffData.mLastCycleTime;
               out.AddNote() << "End: " << walkoffData.mHoldoutTime + walkoffData.mLastCycleTime;
            }
         }
      }
      else if ((i != TrackEffectData::cMAINTAIN_DROP) && (GetEA_EP_Ptr() != nullptr))
      {
         walkoffData.mTrackError += walkoffData.mWalkoffRate * elapsedTime;

         // Cap error at holdout
         walkoffData.mTrackError = std::min(walkoffData.mTrackError, walkoffData.mHoldoutTarget);
      }
   }
   SetLastUpdateTime(mEffectedSystemId, aSimTime);
}

// virtual
WsfEW_Effect::EffectData* WsfEW_TrackEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mTrackEffectDataMap[aSystemTypeId] =
      dynamic_cast<TrackEffectData*>(WsfEW_PulseEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mTrackEffectDataMap[aSystemTypeId];
}

void WsfEW_TrackEffect::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   if (GetEP_Ptr() == nullptr)
   {
      return;
   }
   WsfEM_XmtrRcvr& xmtrRcvr = GetEA_EP_Ptr()->GetLinkedXmtrRcvr();
   if ((aModePtr->GetNameId() == xmtrRcvr.GetMode()->GetNameId()) &&
       (xmtrRcvr.GetArticulatedPart()->GetNameId() == aSensorPtr->GetNameId()))
   {
      for (auto effectData : mTrackEffectDataMap)
      {
         SetPreviousJtoS(effectData.first, -DBL_MAX);
         SetPreviousStoN(effectData.first, -DBL_MAX);
         SetLastUpdateTime(effectData.first, 0.0);
      }
   }
}

void WsfEW_TrackEffect::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (GetEP_Ptr() == nullptr)
   {
      return;
   }
   WsfEM_XmtrRcvr& xmtrRcvr = GetEA_EP_Ptr()->GetLinkedXmtrRcvr();
   if ((aTrackPtr->GetSensorModeId() == xmtrRcvr.GetMode()->GetNameId()) &&
       (xmtrRcvr.GetArticulatedPart()->GetNameId() == aSensorPtr->GetNameId()))
   {
      for (auto effectData : mTrackEffectDataMap)
      {
         SetPreviousJtoS(effectData.first, -DBL_MAX);
         SetPreviousStoN(effectData.first, -DBL_MAX);
         SetLastUpdateTime(effectData.first, 0.0);
      }
   }
}

// virtual
void WsfEW_TrackEffect::UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr)
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

double WsfEW_TrackEffect::GetLastUpdateTime(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it != mTrackEffectDataMap.end())
   {
      return it->second->mLastUpdateTime;
   }
   return mTrackEffectDataMap.find(nullptr)->second->mLastUpdateTime;
}

void WsfEW_TrackEffect::SetLastUpdateTime(WsfStringId aSystemTypeId, double aLastUpdateTime)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, mEffectDataMap[nullptr]->Clone()); // Want to clone default data here.
   }
   mTrackEffectDataMap[aSystemTypeId]->mLastUpdateTime = aLastUpdateTime;
}

double WsfEW_TrackEffect::GetRequiredJtoS(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mRequiredJtoS;
}

double WsfEW_TrackEffect::GetPreviousJtoS(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mPreviousJtoS;
}

void WsfEW_TrackEffect::SetPreviousJtoS(WsfStringId aSystemTypeId, double aPreviousJtoS)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, mEffectDataMap[nullptr]->Clone()); // Want to clone default data here.
   }
   mTrackEffectDataMap[aSystemTypeId]->mPreviousJtoS = aPreviousJtoS;
}

double WsfEW_TrackEffect::GetRequiredStoN(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mRequiredStoN;
}

void WsfEW_TrackEffect::SetRequiredStoN(WsfStringId aSystemTypeId, double aRequiredStoN)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, mEffectDataMap[nullptr]->Clone()); // Want to clone default data here.
   }
   mTrackEffectDataMap[aSystemTypeId]->mRequiredStoN = aRequiredStoN;
}

double WsfEW_TrackEffect::GetPreviousStoN(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mPreviousStoN;
}

void WsfEW_TrackEffect::SetPreviousStoN(WsfStringId aSystemTypeId, double aPreviousStoN)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, mEffectDataMap[nullptr]->Clone()); // Want to clone default data here.
   }
   mTrackEffectDataMap[aSystemTypeId]->mPreviousStoN = aPreviousStoN;
}

double WsfEW_TrackEffect::GetRangeError(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[TrackEffectData::cRANGE].mTrackError;
}

void WsfEW_TrackEffect::SetRangeError(WsfStringId aSystemTypeId, double aRange)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mTrackEffectDataMap[aSystemTypeId]->mWalkoffData[TrackEffectData::cRANGE].mTrackError = aRange;
}

double WsfEW_TrackEffect::GetRangeWalkoffRate(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[TrackEffectData::cRANGE].mWalkoffRate;
}

void WsfEW_TrackEffect::SetRangeWalkoffRate(WsfStringId aSystemTypeId, double aWalkoffRate)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mTrackEffectDataMap[aSystemTypeId]->mWalkoffData[TrackEffectData::cRANGE].mWalkoffRate = aWalkoffRate;
}

double WsfEW_TrackEffect::GetAzimuthError(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[TrackEffectData::cAZIMUTH].mTrackError;
}

void WsfEW_TrackEffect::SetAzimuthError(WsfStringId aSystemTypeId, double aAzimuth)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mTrackEffectDataMap[aSystemTypeId]->mWalkoffData[TrackEffectData::cAZIMUTH].mTrackError = aAzimuth;
}

double WsfEW_TrackEffect::GetElevationError(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[TrackEffectData::cELEVATION].mTrackError;
}

void WsfEW_TrackEffect::SetElevationError(WsfStringId aSystemTypeId, double aElevation)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mTrackEffectDataMap[aSystemTypeId]->mWalkoffData[TrackEffectData::cELEVATION].mTrackError = aElevation;
}

double WsfEW_TrackEffect::GetVelocityError(WsfStringId aSystemTypeId) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[TrackEffectData::cVELOCITY].mTrackError;
}

void WsfEW_TrackEffect::SetVelocityError(WsfStringId aSystemTypeId, double aVelocity)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mTrackEffectDataMap[aSystemTypeId]->mWalkoffData[TrackEffectData::cVELOCITY].mTrackError = aVelocity;
}

WsfEW_TrackEffect::TrackEffectData::WalkoffData& WsfEW_TrackEffect::GetWalkoffData(WsfStringId aSystemTypeId, int aType) const
{
   auto it = mTrackEffectDataMap.find(aSystemTypeId);
   if (it == mTrackEffectDataMap.end())
   {
      it = mTrackEffectDataMap.find(nullptr);
   }
   return it->second->mWalkoffData[aType];
}

WsfEW_TrackEffect::EffectData* WsfEW_TrackEffect::NewEffectData()
{
   return new TrackEffectData();
}


// Embedded Effect data class
WsfEW_TrackEffect::TrackEffectData::TrackEffectData()
   : PulseData()
   , mLastUpdateTime(0.0)
   , mRequiredJtoS(UtMath::DB_ToLinear(3.0))
   , mPreviousJtoS(-DBL_MAX)
   , mRequiredStoN(-DBL_MAX)
   , mPreviousStoN(-DBL_MAX)
{
   for (unsigned int i = 0; i < cLAST_ERROR_TYPE; ++i)
   {
      mWalkoffData[i].mTrackError = 0.0;
   }
}

WsfEW_TrackEffect::TrackEffectData::TrackEffectData(const TrackEffectData& aSrc)
   : PulseData(aSrc)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mRequiredJtoS(aSrc.mRequiredJtoS)
   , mPreviousJtoS(-DBL_MAX)
   , mRequiredStoN(aSrc.mRequiredStoN)
   , mPreviousStoN(-DBL_MAX)
{
   for (unsigned int i = 0; i < cLAST_ERROR_TYPE; ++i)
   {
      mWalkoffData[i] = aSrc.mWalkoffData[i];
   }
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_TrackEffect::TrackEffectData::Clone() const
{
   return new TrackEffectData(*this);
}

// virtual
bool WsfEW_TrackEffect::TrackEffectData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "required_j_to_s")
   {
      aInput.ReadValueOfType(mRequiredJtoS, UtInput::cRATIO);
   }
   else if (command == "range_error")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mTrackError, UtInput::cLENGTH);
   }
   else if (command == "azimuth_error")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mTrackError, UtInput::cANGLE);
   }
   else if (command == "elevation_error")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mTrackError, UtInput::cANGLE);
   }
   else if (command == "velocity_error")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mTrackError, UtInput::cSPEED);
   }
   else if (command == "range_walkoff_rate")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mWalkoffRate, UtInput::cSPEED);
   }
   else if (command == "azimuth_walkoff_rate")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mWalkoffRate, UtInput::cANGULAR_RATE);
   }
   else if (command == "elevation_walkoff_rate")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mWalkoffRate, UtInput::cANGULAR_RATE);
   }
   else if (command == "velocity_walkoff_rate")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mWalkoffRate, UtInput::cACCELERATION);
   }
   else if (command == "range_holdout")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mHoldoutTarget, UtInput::cLENGTH);
      data.mHoldoutTime = DBL_MAX; // overwrite any holdout time previously encountered
      data.mRecycleTime = 0.0;     // overwrite any holdout time previously encountered
   }
   else if (command == "azimuth_holdout")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mHoldoutTarget, UtInput::cANGLE);
      data.mHoldoutTime = DBL_MAX; // overwrite any holdout time previously encountered
      data.mRecycleTime = 0.0;     // overwrite any holdout time previously encountered
   }
   else if (command == "elevation_holdout")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mHoldoutTarget, UtInput::cANGLE);
      data.mHoldoutTime = DBL_MAX; // overwrite any holdout time previously encountered
      data.mRecycleTime = 0.0;     // overwrite any holdout time previously encountered
   }
   else if (command == "velocity_holdout")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mHoldoutTarget, UtInput::cSPEED);
      data.mHoldoutTime = DBL_MAX; // overwrite any holdout time previously encountered
      data.mRecycleTime = 0.0;     // overwrite any holdout time previously encountered
   }
   else if (command == "range_holdout_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mHoldoutTime, UtInput::cTIME);
      data.mHoldoutTarget = DBL_MAX; // overwrite any range holdout previously encountered
   }
   else if (command == "azimuth_holdout_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mHoldoutTime, UtInput::cTIME);
      data.mHoldoutTarget = DBL_MAX; // overwrite any azimuth holdout previously encountered
   }
   else if (command == "elevation_holdout_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mHoldoutTime, UtInput::cTIME);
      data.mHoldoutTarget = DBL_MAX; // overwrite any elevation holdout previously encountered
   }
   else if (command == "velocity_holdout_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mHoldoutTime, UtInput::cTIME);
      data.mHoldoutTarget = DBL_MAX; // overwrite any velocity holdout previously encountered
   }
   else if (command == "maintain_drop_holdout_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      aInput.ReadValueOfType(data.mHoldoutTime, UtInput::cTIME);
   }
   else if (command == "range_delay_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mDelayTime, UtInput::cTIME);
   }
   else if (command == "azimuth_delay_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mDelayTime, UtInput::cTIME);
   }
   else if (command == "elevation_delay_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mDelayTime, UtInput::cTIME);
   }
   else if (command == "velocity_delay_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mDelayTime, UtInput::cTIME);
   }
   else if (command == "maintain_drop_delay_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      aInput.ReadValueOfType(data.mDelayTime, UtInput::cTIME);
   }
   else if (command == "range_recycle_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cRANGE];
      aInput.ReadValueOfType(data.mRecycleTime, UtInput::cTIME);
   }
   else if (command == "azimuth_recycle_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cAZIMUTH];
      aInput.ReadValueOfType(data.mRecycleTime, UtInput::cTIME);
   }
   else if (command == "elevation_recycle_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cELEVATION];
      aInput.ReadValueOfType(data.mRecycleTime, UtInput::cTIME);
   }
   else if (command == "velocity_recycle_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cVELOCITY];
      aInput.ReadValueOfType(data.mRecycleTime, UtInput::cTIME);
   }
   else if (command == "maintain_drop_recycle_time")
   {
      TrackEffectData::WalkoffData& data = mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      aInput.ReadValueOfType(data.mRecycleTime, UtInput::cTIME);
   }
   else if (command == "range_recycle")
   {
      aInput.ReadValue(mWalkoffData[TrackEffectData::cRANGE].mRecycle);
   }
   else if (command == "azimuth_recycle")
   {
      aInput.ReadValue(mWalkoffData[TrackEffectData::cAZIMUTH].mRecycle);
   }
   else if (command == "elevation_recycle")
   {
      aInput.ReadValue(mWalkoffData[TrackEffectData::cELEVATION].mRecycle);
   }
   else if (command == "velocity_recycle")
   {
      aInput.ReadValue(mWalkoffData[TrackEffectData::cVELOCITY].mRecycle);
   }
   else if (command == "maintain_drop_recycle")
   {
      aInput.ReadValue(mWalkoffData[TrackEffectData::cMAINTAIN_DROP].mRecycle);
   }
   else if (command == "delay_table")
   {
      // The table is time delay values vs. j-to-s ratio
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);

      WalkoffData* wod = nullptr;
      if (type == "range")
      {
         wod = &mWalkoffData[TrackEffectData::cRANGE];
      }
      else if (type == "azimuth")
      {
         wod = &mWalkoffData[TrackEffectData::cAZIMUTH];
      }
      else if (type == "elevation")
      {
         wod = &mWalkoffData[TrackEffectData::cELEVATION];
      }
      else if (type == "velocity")
      {
         wod = &mWalkoffData[TrackEffectData::cVELOCITY];
      }
      else if (type == "maintain_drop")
      {
         wod = &mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      }
      else
      {
         throw UtInput::BadValue(aInput, "unrecognized type");
      }

      std::vector<double> jToS_Values;
      std::vector<double> minTimeValues;
      std::vector<double> maxTimeValues;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "jamming_to_signal")
         {
            double jToS;
            double minTimeValue;
            double maxTimeValue;
            aInput.ReadValueOfType(jToS, UtInput::cRATIO);
            aInput.ReadValueOfType(minTimeValue, UtInput::cTIME);
            aInput.ReadValueOfType(maxTimeValue, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(jToS, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueLessOrEqual(minTimeValue, maxTimeValue);
            if (!jToS_Values.empty() && (jToS <= jToS_Values.back()))
            {
               throw UtInput::BadValue(aInput, "entries must be in order of increasing j-to-s");
            }
            jToS_Values.push_back(jToS);
            minTimeValues.push_back(minTimeValue);
            maxTimeValues.push_back(maxTimeValue);
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

      wod->mDelayTable.mJtoS.SetValues(jToS_Values);
      wod->mDelayTable.mMinTime.SetValues(minTimeValues);
      wod->mDelayTable.mMaxTime.SetValues(maxTimeValues);
   }
   else if (command == "holdout_table")
   {
      // The table is holdout/stop time values vs. j-to-s ratio
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);

      WalkoffData* wod = nullptr;
      if (type == "range")
      {
         wod = &mWalkoffData[TrackEffectData::cRANGE];
      }
      else if (type == "azimuth")
      {
         wod = &mWalkoffData[TrackEffectData::cAZIMUTH];
      }
      else if (type == "elevation")
      {
         wod = &mWalkoffData[TrackEffectData::cELEVATION];
      }
      else if (type == "velocity")
      {
         wod = &mWalkoffData[TrackEffectData::cVELOCITY];
      }
      else if (type == "maintain_drop")
      {
         wod = &mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      }
      else
      {
         throw UtInput::BadValue(aInput, "unrecognized type");
      }

      std::vector<double> jToS_Values;
      std::vector<double> minTimeValues;
      std::vector<double> maxTimeValues;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "jamming_to_signal")
         {
            double jToS;
            double minTimeValue;
            double maxTimeValue;
            aInput.ReadValueOfType(jToS, UtInput::cRATIO);
            aInput.ReadValueOfType(minTimeValue, UtInput::cTIME);
            aInput.ReadValueOfType(maxTimeValue, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(jToS, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueLessOrEqual(minTimeValue, maxTimeValue);
            if (!jToS_Values.empty() && (jToS <= jToS_Values.back()))
            {
               throw UtInput::BadValue(aInput, "entries must be in order of increasing j-to-s");
            }
            jToS_Values.push_back(jToS);
            minTimeValues.push_back(minTimeValue);
            maxTimeValues.push_back(maxTimeValue);
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

      wod->mHoldOutTable.mJtoS.SetValues(jToS_Values);
      wod->mHoldOutTable.mMinTime.SetValues(minTimeValues);
      wod->mHoldOutTable.mMaxTime.SetValues(maxTimeValues);
   }
   else if (command == "recycle_table")
   {
      // The table is time recycle values vs. j-to-s ratio
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);

      WalkoffData* wod = nullptr;
      if (type == "range")
      {
         wod = &mWalkoffData[TrackEffectData::cRANGE];
      }
      else if (type == "azimuth")
      {
         wod = &mWalkoffData[TrackEffectData::cAZIMUTH];
      }
      else if (type == "elevation")
      {
         wod = &mWalkoffData[TrackEffectData::cELEVATION];
      }
      else if (type == "velocity")
      {
         wod = &mWalkoffData[TrackEffectData::cVELOCITY];
      }
      else if (type == "maintain_drop")
      {
         wod = &mWalkoffData[TrackEffectData::cMAINTAIN_DROP];
      }
      else
      {
         throw UtInput::BadValue(aInput, "unrecognized type");
      }

      std::vector<double> jToS_Values;
      std::vector<double> minTimeValues;
      std::vector<double> maxTimeValues;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "jamming_to_signal")
         {
            double jToS;
            double minTimeValue;
            double maxTimeValue;
            aInput.ReadValueOfType(jToS, UtInput::cRATIO);
            aInput.ReadValueOfType(minTimeValue, UtInput::cTIME);
            aInput.ReadValueOfType(maxTimeValue, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(jToS, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueGreaterOrEqual(minTimeValue, 0.0);
            aInput.ValueLessOrEqual(minTimeValue, maxTimeValue);
            if (!jToS_Values.empty() && (jToS <= jToS_Values.back()))
            {
               throw UtInput::BadValue(aInput, "entries must be in order of increasing j-to-s");
            }
            jToS_Values.push_back(jToS);
            minTimeValues.push_back(minTimeValue);
            maxTimeValues.push_back(maxTimeValue);
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

      wod->mRecycleTable.mJtoS.SetValues(jToS_Values);
      wod->mRecycleTable.mMinTime.SetValues(minTimeValues);
      wod->mRecycleTable.mMaxTime.SetValues(maxTimeValues);
   }
   else if (!PulseData::ProcessInput(aInput))
   {
      myCommand = false;
   }

   return myCommand;
}

// virtual
bool WsfEW_TrackEffect::TrackEffectData::Initialize(WsfSimulation& aSimulation)
{
   bool ok = PulseData::Initialize(aSimulation);

   for (unsigned int i = 0; i < cLAST_ERROR_TYPE; ++i)
   {
      WalkoffData& wod = mWalkoffData[i];
      if ((wod.mRecycleTime < DBL_MAX) && (wod.mHoldoutTime == DBL_MAX))
      {
         wod.mHoldoutTime = wod.mRecycleTime;
      }
      else if ((wod.mHoldoutTime < DBL_MAX) && (wod.mRecycleTime == DBL_MAX))
      {
         wod.mRecycleTime = wod.mHoldoutTime;
      }

      if (wod.mRecycleTime < wod.mHoldoutTime)
      {
         wod.mHoldoutTime = wod.mRecycleTime;
      }

      if (wod.mRecycle)
      {
         wod.Recycle(aSimulation.GetSimTime());
      }
   }

   mLastUpdateTime = aSimulation.GetSimTime();

   return ok;
}

// virtual
void WsfEW_TrackEffect::TrackEffectData::Reset()
{
   mLastUpdateTime = 0.0;
   mRequiredJtoS   = UtMath::DB_ToLinear(3.0);
   mPreviousJtoS   = -DBL_MAX;
   for (unsigned int i = 0; i < cLAST_ERROR_TYPE; ++i)
   {
      mWalkoffData[i].mTrackError = 0.0;
   }
   PulseData::Reset();
}

double WsfEW_TrackEffect::TrackEffectData::WalkoffData::GetDelayTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal)
{
   double delayTime = mDelayTime;
   if (mDelayTable.mJtoS.GetSize() > 0)
   {
      TblLookupLU<double> jToS;
      jToS.Lookup(mDelayTable.mJtoS, aJammerToSignal);
      double minTime = TblEvaluate(mDelayTable.mMinTime, jToS);
      double maxTime = TblEvaluate(mDelayTable.mMaxTime, jToS);
      delayTime      = aEffectPtr->GetRandom().Uniform(minTime, maxTime);
   }
   return delayTime;
}

double WsfEW_TrackEffect::TrackEffectData::WalkoffData::GetHoldoutTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal)
{
   double holdoutTime = mHoldoutTime;
   if (mHoldOutTable.mJtoS.GetSize() > 0)
   {
      TblLookupLU<double> jToS;
      jToS.Lookup(mHoldOutTable.mJtoS, aJammerToSignal);
      double minTime = TblEvaluate(mHoldOutTable.mMinTime, jToS);
      double maxTime = TblEvaluate(mHoldOutTable.mMaxTime, jToS);
      holdoutTime    = aEffectPtr->GetRandom().Uniform(minTime, maxTime);
   }
   return holdoutTime;
}

double WsfEW_TrackEffect::TrackEffectData::WalkoffData::GetRecycleTime(WsfEW_Effect* aEffectPtr, double aJammerToSignal)
{
   double recycleTime = mRecycleTime;
   if (mRecycleTable.mJtoS.GetSize() > 0)
   {
      TblLookupLU<double> jToS;
      jToS.Lookup(mRecycleTable.mJtoS, aJammerToSignal);
      double minTime = TblEvaluate(mRecycleTable.mMinTime, jToS);
      double maxTime = TblEvaluate(mRecycleTable.mMaxTime, jToS);
      recycleTime    = aEffectPtr->GetRandom().Uniform(minTime, maxTime);
   }
   return recycleTime;
}
