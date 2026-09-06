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

#include "WsfEW_PulseEffect.hpp"

#include <algorithm>
#include <cassert>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfStringId.hpp"

WsfEW_PulseEffect::WsfEW_PulseEffect()
   : WsfEW_PowerEffect()
   , mPulseDataMap()
   , mPulseDensityFactor(1.0)
   , mJammingPulseDensity(0.0)
{
   mCoherencyMask = cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
}

WsfEW_PulseEffect::WsfEW_PulseEffect(const WsfEW_PulseEffect& aSrc)
   : WsfEW_PowerEffect(aSrc)
   , mPulseDataMap()
   , mPulseDensityFactor(aSrc.mPulseDensityFactor)
   , mJammingPulseDensity(aSrc.mJammingPulseDensity)
{
}

// virtual
WsfEW_Effect* WsfEW_PulseEffect::Clone() const
{
   return new WsfEW_PulseEffect(*this);
}

// virtual
bool WsfEW_PulseEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_PowerEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_SENSOR | cES_COMM;
   }

   return ok;
}

// virtual
bool WsfEW_PulseEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_PowerEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_PulseEffect::UpdateEffects(double                                 aSimTime,
                                      const WsfEM_Interaction&               aTargetInteraction,
                                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                      WsfEW_Effects&                         aEW_Effects,
                                      WsfEW_Effect*                          aEW_EffectPtr /* = 0 */)
{
   // Set the jamming pulse density if an EA effect.
   if (GetEA_Ptr() != nullptr)
   {
      WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
      WsfEM_Rcvr* radarRcvrPtr  = aTargetInteraction.GetReceiver();
      assert(radarRcvrPtr != nullptr);
      WsfEM_Xmtr* radarXmtrPtr = aTargetInteraction.GetTransmitter();
      assert(radarXmtrPtr != nullptr);

      UpdateCurrentIds(radarRcvrPtr, jammerXmtrPtr, aTargetInteraction.GetTarget());

      UpdateJammerPulseDensity(jammerXmtrPtr, radarXmtrPtr);
      SetJammingPulseDensity(GetJammerPulseDensity(mEffectedSystemId) * mPulseDensityFactor);
      mPulseDensityFactor = 1.0;
   }
   WsfEW_PowerEffect::UpdateEffects(aSimTime,
                                    aTargetInteraction,
                                    aJammerToTgtInteraction,
                                    aTgtToJammerInteraction,
                                    aEW_Effects,
                                    aEW_EffectPtr);

   if (mDebug && (GetEA_Ptr() != nullptr))
   {
      auto out = ut::log::debug() << "Effects update on Jamming Pulse Density.";
      out.AddNote() << "Jamming Pulse Density: " << GetJammingPulseDensity();
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_PulseEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mPulseDataMap[aSystemTypeId] = dynamic_cast<PulseData*>(WsfEW_PowerEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mPulseDataMap[aSystemTypeId];
}

// virtual
void WsfEW_PulseEffect::UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr)
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

      // Need to clone the effect data from the default data if data does not exist when updating the JammerPulseDensity.
      if (!EffectDataExists(mEffectedSystemId))
      {
         InsertNewEffectData(mEffectedSystemId, mEffectDataMap[nullptr]->Clone());
      }
      SetJammerPulseDensity(mEffectedSystemId, jammerPulseDensity, false);
   }
}


// virtual
double WsfEW_PulseEffect::GetJammingPulseDensity(WsfEM_Xmtr* aXmtrPtr) const
{
   double jammingPulseDensity = GetJammingPulseDensity();
   if ((jammingPulseDensity <= 0.0) && (aXmtrPtr->GetPulseWidth() > 0.0) && (aXmtrPtr->GetPulseRepetitionInterval() > 0.0))
   {
      jammingPulseDensity = aXmtrPtr->GetPulseWidth() / aXmtrPtr->GetPulseRepetitionInterval();
   }
   return jammingPulseDensity;
}

// virtual
double WsfEW_PulseEffect::GetJammerPulseDensity(WsfStringId aSystemTypeId) const
{
   auto edi = mPulseDataMap.find(aSystemTypeId);
   if (edi == mPulseDataMap.end())
   {
      edi = mPulseDataMap.find(nullptr);
   }
   return edi->second->GetJammerPulseDensity();
}

void WsfEW_PulseEffect::SetJammerPulseDensity(WsfStringId aSystemTypeId, double aJammerPulseDensity, bool aIsExplicit /*= false*/)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      InsertNewEffectData(aSystemTypeId, NewEffectData());
   }
   mPulseDataMap[aSystemTypeId]->SetJammerPulseDensity(aJammerPulseDensity, aIsExplicit);
}

bool WsfEW_PulseEffect::ExplicitJPD(WsfStringId aSystemTypeId)
{
   auto edi = mPulseDataMap.find(aSystemTypeId);
   if (edi == mPulseDataMap.end())
   {
      edi = mPulseDataMap.find(nullptr);
   }
   return edi->second->ExplicitJPD();
}


// Embedded Effect data class
WsfEW_PulseEffect::PulseData::PulseData()
   : PowerData()
   , mJammerPulseDensity(1.0)
   , mExplicitJPD(false)
{
}

// virtual
bool WsfEW_PulseEffect::PulseData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "jamming_pulse_density") || (command == "jamming_duty_cycle"))
   {
      double jpd;
      aInput.ReadValue(jpd);
      aInput.ValueInClosedRange(jpd, 0.0, 1.0);
      SetJammerPulseDensity(jpd, true);
   }
   else if (!PowerData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_PulseEffect::PulseData::Initialize(WsfSimulation& aSimulation)
{
   return PowerData::Initialize(aSimulation);
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_PulseEffect::PulseData::Clone() const
{
   return new PulseData(*this);
}

// virtual
void WsfEW_PulseEffect::PulseData::Reset()
{
   mJammerPulseDensity = 1.0;
   mExplicitJPD        = false;
   PowerData::Reset();
}

void WsfEW_PulseEffect::PulseData::SetJammerPulseDensity(double aJammerPulseDensity, bool aIsExplicit /*= false*/)
{
   mJammerPulseDensity = aJammerPulseDensity;
   mExplicitJPD        = aIsExplicit;
}
