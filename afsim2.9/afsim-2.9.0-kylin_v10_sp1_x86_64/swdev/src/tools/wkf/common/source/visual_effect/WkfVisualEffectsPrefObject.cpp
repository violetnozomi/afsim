// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfVisualEffectsPrefObject.hpp"

wkf::VisualEffectsPrefObject::VisualEffectsPrefObject(QObject* parent)
   : PrefObjectT(parent, "VisualEffects")
   , mShowTrailingEffects(true)
   , mShowExplosions(true)
   , mMapExplosionScalar(1.0)
   , mMapExplosionLifetime(2.0)
{
}

void wkf::VisualEffectsPrefObject::Apply()
{
   if (mShowTrailingEffects != mCurrentPrefs.mShowTrailingEffects)
   {
      mShowTrailingEffects = mCurrentPrefs.mShowTrailingEffects;
      emit ShowTrailingEffectsChanged(mShowTrailingEffects);
   }

   if (mShowExplosions != mCurrentPrefs.mShowExplosions)
   {
      mShowExplosions = mCurrentPrefs.mShowExplosions;
      emit ShowExplosionsChanged(mShowExplosions);
   }

   if (mMapExplosionScalar != mCurrentPrefs.mMapExplosionScalar)
   {
      mMapExplosionScalar = mCurrentPrefs.mMapExplosionScalar;
      emit MapExplosionScaleChanged(mMapExplosionScalar);
   }

   if (mMapExplosionLifetime != mCurrentPrefs.mMapExplosionLifetime)
   {
      mMapExplosionLifetime = mCurrentPrefs.mMapExplosionLifetime;
      emit MapExplosionLifetimeChanged(mMapExplosionLifetime);
   }
}

wkf::VisualEffectsPrefData wkf::VisualEffectsPrefObject::ReadSettings(QSettings& aSettings) const
{
   VisualEffectsPrefData pData;
   pData.mShowTrailingEffects  = aSettings.value("showTrailingEffects", mDefaultPrefs.mShowTrailingEffects).toBool();
   pData.mShowExplosions       = aSettings.value("showExplosions", mDefaultPrefs.mShowExplosions).toBool();
   pData.mMapExplosionScalar   = aSettings.value("mapExplosionScalar", mDefaultPrefs.mMapExplosionScalar).toDouble();
   pData.mMapExplosionLifetime = aSettings.value("mapExplosionLifetime", mDefaultPrefs.mMapExplosionLifetime).toDouble();
   return pData;
}

void wkf::VisualEffectsPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("showTrailingEffects", mCurrentPrefs.mShowTrailingEffects);
   aSettings.setValue("showExplosions", mCurrentPrefs.mShowExplosions);
   aSettings.setValue("mapExplosionScalar", mCurrentPrefs.mMapExplosionScalar);
   aSettings.setValue("mapExplosionLifetime", mCurrentPrefs.mMapExplosionLifetime);
}
