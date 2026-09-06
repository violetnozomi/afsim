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

#include "WsfEW_PolModEffect.hpp"

#include <cassert>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfStringId.hpp"

WsfEW_PolModEffect::WsfEW_PolModEffect()
   : WsfEW_SLC_DegradeEffect()
   , mPolModDataMap()
{
   mCoherencyMask      = cEC_NONCOHERENT;
   mBehavior           = cEB_SLC_DEGRADE_EFFECT;
   mSystemFunctionMask = cES_SENSOR;
}


WsfEW_PolModEffect::WsfEW_PolModEffect(const WsfEW_PolModEffect& aSrc)
   : WsfEW_SLC_DegradeEffect(aSrc)
   , mPolModDataMap()
{
}

// virtual
WsfEW_Effect* WsfEW_PolModEffect::Clone() const
{
   return new WsfEW_PolModEffect(*this);
}

bool WsfEW_PolModEffect::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_SLC_DegradeEffect::Initialize(aSimulation);
}

bool WsfEW_PolModEffect::ProcessInput(UtInput& aInput)
{
   return WsfEW_SLC_DegradeEffect::ProcessInput(aInput);
}

// virtual
void WsfEW_PolModEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   aEW_Effects.mMask |= cEB_SLC_DEGRADE_EFFECT;
   WsfEW_SLC_DegradeEffect::ApplyUnmitigatedEffect(aEW_Effects);

   if (mDebug)
   {
      ut::log::debug() << "Pol-Mod effect applied.";
   }
}

// virtual
void WsfEW_PolModEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const
{
   WsfEW_SLC_DegradeEffect::ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
}

double WsfEW_PolModEffect::GetPolarizationSwitchingRate(WsfStringId aSystemTypeId)
{
   PolModDataMap::const_iterator edi = mPolModDataMap.find(aSystemTypeId);
   if (edi == mPolModDataMap.end())
   {
      edi = mPolModDataMap.find(nullptr);
   }
   return edi->second->mPolSwitchRate;
}

// virtual
WsfEW_Effect::EffectData* WsfEW_PolModEffect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   mPolModDataMap[aSystemTypeId] =
      dynamic_cast<PolModData*>(WsfEW_SLC_DegradeEffect::PropagateEffectDataPtr(aSystemTypeId));
   return mPolModDataMap[aSystemTypeId];
}

WsfEW_PolModEffect::EffectData* WsfEW_PolModEffect::NewEffectData()
{
   return new PolModData();
}


// Embedded Effect data class
WsfEW_PolModEffect::PolModData::PolModData()
   : SLC_DegradeData()
   , mPolSwitchRate(-1.0)
{
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_PolModEffect::PolModData::Clone() const
{
   return new PolModData(*this);
}

// virtual
bool WsfEW_PolModEffect::PolModData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "polarization_switching_rate")
   {
      double polSwitchValue;
      aInput.ReadValue(polSwitchValue);
      aInput.ValueGreater(polSwitchValue, 0.0);
      mPolSwitchRate = polSwitchValue;
   }
   else if (!SLC_DegradeData::ProcessInput(aInput))
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
bool WsfEW_PolModEffect::PolModData::Initialize(WsfSimulation& aSimulation)
{
   return SLC_DegradeData::Initialize(aSimulation);
}

// virtual
void WsfEW_PolModEffect::PolModData::Reset()
{
   mPolSwitchRate = -1.0;
   SLC_DegradeData::Reset();
}
