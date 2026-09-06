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

#include "WsfEW_RadiusEffect.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfStringId.hpp"

WsfEW_RadiusEffect::WsfEW_RadiusEffect()
   : WsfEW_Effect()
   , mRadiusDataMap()
   , mCurrentJRL_Type(cJRL_NONE)

{
   mCoherencyMask = cEC_NONE;
   mBehavior      = cEB_JAMMER_POWER_EFFECT;
}

WsfEW_RadiusEffect::WsfEW_RadiusEffect(const WsfEW_RadiusEffect& aSrc)
   : WsfEW_Effect(aSrc)
   , mRadiusDataMap()
   , mCurrentJRL_Type(aSrc.mCurrentJRL_Type)
{
}

// virtual
WsfEW_Effect* WsfEW_RadiusEffect::Clone() const
{
   return new WsfEW_RadiusEffect(*this);
}

// virtual
bool WsfEW_RadiusEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_Effect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_SENSOR;
   }

   return ok;
}

// virtual
bool WsfEW_RadiusEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_Effect::ProcessInput(aInput);
}

// virtual
void WsfEW_RadiusEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   if (GetRadiusLimitMask(mEffectedSystemId) == 0)
   {
      return;
   }
   double radiusFactor = GetRadiusFactor(mEffectedSystemId, mCurrentJRL_Type);
   // if ((fabs(jammingRadiusFactor - 1.0) >= 0.00001)/* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   //{
   aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

   auto out = ut::log::debug();
   if (mDebug)
   {
      out << "Radius effect:";
   }

   if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
   {
      aEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Coherent radius factor: " << radiusFactor;
      }
   }

   if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
   {
      aEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Non-coherent radius factor: " << radiusFactor;
      }
   }

   if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Non-coherent pulse radius factor: " << radiusFactor;
      }
   }
   //}

   // WsfEW_Effect::ApplyUnmitigatedEffect(aEW_Effects);
}

// virtual
void WsfEW_RadiusEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   if (GetRadiusLimitMask(mEffectedSystemId) == 0)
   {
      return;
   }
   auto   out          = ut::log::debug();
   double radiusFactor = GetRadiusFactor(mEffectedSystemId, mCurrentJRL_Type);
   // if ((fabs(jammingRadiusFactor - 1.0) >= 0.00001)/* || (GetBehavior() == cEB_JAMMER_POWER_EFFECT)*/)
   //{
   aEW_Effects.mMask |= cEB_JAMMER_POWER_EFFECT;

   if (mDebug)
   {
      out << "Radius effect:";
   }

   if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
   {
      aEW_Effects.mEW_CoherentJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Coherent radius factor: " << radiusFactor;
      }
   }

   if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
   {
      aEW_Effects.mEW_NoiseJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Non-coherent radius factor: " << radiusFactor;
      }
   }

   if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
   {
      aEW_Effects.mEW_PulseJammingEffect.mRadiusFactor *= radiusFactor;
      if (mDebug)
      {
         out.AddNote() << "Non-coherent pulse radius factor: " << radiusFactor;
      }
   }
   //}

   // WsfEW_Effect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
void WsfEW_RadiusEffect::UpdateEffects(double                                 aSimTime,
                                       const WsfEM_Interaction&               aTargetInteraction,
                                       const WsfEM_Interaction&               aJammerToTgtInteraction,
                                       const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                       WsfEW_Effects&                         aEW_Effects,
                                       WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEW_Effect::UpdateEffects(aSimTime,
                               aTargetInteraction,
                               aJammerToTgtInteraction,
                               aTgtToJammerInteraction,
                               aEW_Effects,
                               aEW_EffectPtr);
   WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Rcvr* radarRcvrPtr  = aTargetInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);

   if (GetRadiusLimitMask(mEffectedSystemId) == 0)
   {
      return;
   }

   WsfEM_Interaction jammerInteraction;
   jammerInteraction.BeginOneWayInteraction(jammerXmtrPtr, radarRcvrPtr, true, false);
   if (jammerInteraction.mRcvrToTgt.mRange < 0.0)
   {
      jammerInteraction.ComputeUndefinedGeometry();
   }

   if (aTargetInteraction.mRcvrToTgt.mRange >= 0.0)
   {
      if (fabs(aTargetInteraction.mRcvrToTgt.mRange - jammerInteraction.mRcvrToXmtr.mRange) < 0.001)
      {
         mCurrentJRL_Type = cJRL_EQUAL;
      }
      else if (aTargetInteraction.mRcvrToTgt.mRange < jammerInteraction.mRcvrToXmtr.mRange)
      {
         mCurrentJRL_Type = cJRL_INSIDE;
      }
      else if (aTargetInteraction.mRcvrToTgt.mRange > jammerInteraction.mRcvrToXmtr.mRange)
      {
         mCurrentJRL_Type = cJRL_OUTSIDE;
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Radius condition not valid.";
         out.AddNote() << "receiver-to-target_range: " << aTargetInteraction.mRcvrToTgt.mRange << " m";
         out.AddNote() << "receiver-to-jammerXmtr_range: " << jammerInteraction.mRcvrToXmtr.mRange << " m";
      }
   }
   else if (mDebug)
   {
      ut::log::debug() << "Target interaction failed or is invalid.";
   }
}

// virtual
WsfEW_Effect::EffectData* WsfEW_RadiusEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mRadiusDataMap[aSystemTypeId] = dynamic_cast<RadiusData*>(WsfEW_Effect::PropagateEffectDataPtr(aSystemTypeId));
   return mRadiusDataMap[aSystemTypeId];
}

int WsfEW_RadiusEffect::GetRadiusLimitMask(WsfStringId aSystemTypeId) const
{
   auto edi = mRadiusDataMap.find(aSystemTypeId);
   if (edi == mRadiusDataMap.end())
   {
      edi = mRadiusDataMap.find(nullptr);
   }
   return edi->second->GetRadiusLimitMask();
}

double WsfEW_RadiusEffect::GetRadiusFactor(WsfStringId aSystemTypeId) const
{
   auto edi = mRadiusDataMap.find(aSystemTypeId);
   if (edi == mRadiusDataMap.end())
   {
      edi = mRadiusDataMap.find(nullptr);
   }
   return edi->second->GetRadiusFactor(mCurrentJRL_Type);
}

double WsfEW_RadiusEffect::GetRadiusFactor(WsfStringId aSystemTypeId, RadiusLimitType aJRL_Type) const
{
   auto edi = mRadiusDataMap.find(aSystemTypeId);
   if (edi == mRadiusDataMap.end())
   {
      edi = mRadiusDataMap.find(nullptr);
   }
   return edi->second->GetRadiusFactor(aJRL_Type);
}

// Embedded Effect data class
WsfEW_RadiusEffect::RadiusData::RadiusData()
   : EffectData()
   , mRadiusLimitMask(cJRL_NONE)
{
   mRadiusFactorMap[cJRL_NONE]    = 1.0;
   mRadiusFactorMap[cJRL_INSIDE]  = 1.0;
   mRadiusFactorMap[cJRL_OUTSIDE] = 1.0;
   mRadiusFactorMap[cJRL_BOTH]    = 1.0;
   mRadiusFactorMap[cJRL_EQUAL]   = 1.0;
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_RadiusEffect::RadiusData::Clone() const
{
   return new RadiusData(*this);
}

// virtual
bool WsfEW_RadiusEffect::RadiusData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "jamming_radius_factor") || (command == "radius_factor"))
   {
      unsigned int currentRadiusLimitMask(0);
      std::string  limit;
      aInput.ReadValue(limit);
      double radiusFactor;
      aInput.ReadValue(radiusFactor);
      aInput.ValueGreaterOrEqual(radiusFactor, 0.0);

      if (limit == "inside")
      {
         currentRadiusLimitMask = cJRL_INSIDE;
      }
      else if (limit == "inside_and_equal")
      {
         currentRadiusLimitMask = cJRL_INSIDE | cJRL_EQUAL;
      }
      else if (limit == "outside")
      {
         currentRadiusLimitMask = cJRL_OUTSIDE;
      }
      else if (limit == "outside_and_equal")
      {
         currentRadiusLimitMask = cJRL_OUTSIDE | cJRL_EQUAL;
      }
      else if (limit == "equal")
      {
         currentRadiusLimitMask = cJRL_EQUAL;
      }
      else if ((limit == "outside_and_inside") || (limit == "inside_and_outside"))
      {
         currentRadiusLimitMask = cJRL_BOTH | cJRL_EQUAL;
      }
      else if ((limit == "all") || (limit == "outside_and_inside_and_equal") || (limit == "inside_and_outside_and_equal"))
      {
         currentRadiusLimitMask = cJRL_BOTH | cJRL_EQUAL;
      }
      else
      {
         throw UtInput::BadValue(aInput,
                                 "WSF_RADIUS_EFFECT 'jamming_radius_factor' does not recognize the 'limit-type' " +
                                    limit + '.');
      }
      mRadiusLimitMask |= currentRadiusLimitMask;
      SetRadiusFactor(currentRadiusLimitMask, radiusFactor);
   }
   else if (!EffectData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_RadiusEffect::RadiusData::Initialize(WsfSimulation& aSimulation)
{
   return EffectData::Initialize(aSimulation);
}

// virtual
void WsfEW_RadiusEffect::RadiusData::Reset()
{
   mRadiusFactorMap[cJRL_NONE]    = 1.0;
   mRadiusFactorMap[cJRL_INSIDE]  = 1.0;
   mRadiusFactorMap[cJRL_OUTSIDE] = 1.0;
   mRadiusFactorMap[cJRL_BOTH]    = 1.0;
   mRadiusFactorMap[cJRL_EQUAL]   = 1.0;
   EffectData::Reset();
}

double WsfEW_RadiusEffect::RadiusData::GetRadiusFactor(RadiusLimitType aJRL_Type) const
{
   // Will return cNONE value or 1.0 if JRL type is not in the mask
   RadiusLimitType JRL_Type = static_cast<RadiusLimitType>(aJRL_Type & mRadiusLimitMask);
   return mRadiusFactorMap.find(JRL_Type)->second;
}

void WsfEW_RadiusEffect::RadiusData::SetRadiusFactor(unsigned int aJRL_Mask, double aRadiusFactor)
{
   if (aJRL_Mask == cJRL_NONE)
   {
      mRadiusFactorMap[cJRL_NONE] = aRadiusFactor;
   }

   if ((aJRL_Mask & cJRL_INSIDE) != 0u) // will take care of cBOTH also
   {
      mRadiusFactorMap[cJRL_INSIDE] = aRadiusFactor;
   }

   if ((aJRL_Mask & cJRL_OUTSIDE) != 0u) // will take care of cBOTH also
   {
      mRadiusFactorMap[cJRL_OUTSIDE] = aRadiusFactor;
   }

   if ((aJRL_Mask & cJRL_EQUAL) != 0u)
   {
      mRadiusFactorMap[cJRL_EQUAL] = aRadiusFactor;
   }
}
