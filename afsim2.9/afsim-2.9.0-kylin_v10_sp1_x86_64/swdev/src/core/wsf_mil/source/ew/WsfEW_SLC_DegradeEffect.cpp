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

#include "WsfEW_SLC_DegradeEffect.hpp"

#include <cassert>
#include <cfloat>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfStringId.hpp"

WsfEW_SLC_DegradeEffect::WsfEW_SLC_DegradeEffect()
   : WsfEW_PowerEffect()
   , mSLC_DegradeDataMap()
{
   mCoherencyMask = cEC_NONCOHERENT;
   mBehavior      = cEB_SLC_DEGRADE_EFFECT;
}

WsfEW_SLC_DegradeEffect::WsfEW_SLC_DegradeEffect(const WsfEW_SLC_DegradeEffect& aSrc)
   : WsfEW_PowerEffect(aSrc)
   , mSLC_DegradeDataMap()
{
}

// virtual
WsfEW_Effect* WsfEW_SLC_DegradeEffect::Clone() const
{
   return new WsfEW_SLC_DegradeEffect(*this);
}

bool WsfEW_SLC_DegradeEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_PowerEffect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      // For now, just affect sensors
      mSystemFunctionMask = cES_SENSOR;
   }

   return ok;
}

bool WsfEW_SLC_DegradeEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_PowerEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_SLC_DegradeEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_SLC_DEGRADE_EFFECT;
   WsfEW_PowerEffect::ApplyUnmitigatedEffect(aEW_Effects);

   if (mDebug)
   {
      ut::log::debug() << "SLC degrade effect applied.";
   }
}

// virtual
void WsfEW_SLC_DegradeEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   WsfEW_PowerEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

// virtual
WsfEW_Effect::EffectData* WsfEW_SLC_DegradeEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mSLC_DegradeDataMap[aSystemTypeId] =
      dynamic_cast<SLC_DegradeData*>(WsfEW_PowerEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mSLC_DegradeDataMap[aSystemTypeId];
}

double WsfEW_SLC_DegradeEffect::GetSLC_DegradationValue(WsfStringId aSystemTypeId) const
{
   auto edi = mSLC_DegradeDataMap.find(aSystemTypeId);
   if (edi == mSLC_DegradeDataMap.end())
   {
      edi = mSLC_DegradeDataMap.find(nullptr);
   }
   return (edi->second->mSLC_DegradationValue);
}

int WsfEW_SLC_DegradeEffect::GetSLC_ChannelsSaturated(WsfStringId aSystemTypeId) const
{
   auto edi = mSLC_DegradeDataMap.find(aSystemTypeId);
   if (edi == mSLC_DegradeDataMap.end())
   {
      edi = mSLC_DegradeDataMap.find(nullptr);
   }
   return (edi->second->mSLC_ChannelsSaturated);
}

WsfEW_SLC_DegradeEffect::EffectData* WsfEW_SLC_DegradeEffect::NewEffectData()
{
   return new SLC_DegradeData();
}


// Embedded Effect data class
WsfEW_SLC_DegradeEffect::SLC_DegradeData::SLC_DegradeData()
   : PowerData()
   , mModulation(WsfEW_Effect::cEM_NONE)
   , mPolModEffect(cPME_SATURATE_SLC)
   , mSLC_DegradationValue(1.0)
   , mSLC_ChannelsSaturated(1)
{
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_SLC_DegradeEffect::SLC_DegradeData::Clone() const
{
   return new SLC_DegradeData(*this);
}

// virtual
bool WsfEW_SLC_DegradeEffect::SLC_DegradeData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "signal_modulation")
   {
      std::string modName;
      aInput.ReadValue(modName);
      if (modName == "none")
      {
         mModulation = cEM_NONE;
      }
   }
   else if (command == "slc_degradation_value")
   {
      double degValue;
      aInput.ReadValueOfType(degValue, UtInput::cRATIO);
      aInput.ValueGreater(degValue, 0.0);
      mSLC_DegradationValue = degValue;
   }
   else if (command == "number_slc_channels_saturated")
   {
      int slcSataturated;
      aInput.ReadValue(slcSataturated);
      aInput.ValueGreater(slcSataturated, 0);
      mSLC_ChannelsSaturated = slcSataturated;
   }
   else if (!PowerData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_SLC_DegradeEffect::SLC_DegradeData::Initialize(WsfSimulation& aSimulation)
{
   return PowerData::Initialize(aSimulation);
}

// virtual
void WsfEW_SLC_DegradeEffect::SLC_DegradeData::Reset()
{
   mModulation            = WsfEW_Effect::cEM_NONE;
   mPolModEffect          = cPME_SATURATE_SLC;
   mSLC_DegradationValue  = 1.0;
   mSLC_ChannelsSaturated = 1;
   PowerData::Reset();
}
