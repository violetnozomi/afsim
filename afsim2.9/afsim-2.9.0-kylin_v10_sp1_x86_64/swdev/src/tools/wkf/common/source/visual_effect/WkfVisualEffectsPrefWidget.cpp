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

#include "WkfVisualEffectsPrefWidget.hpp"

wkf::VisualEffectsPrefWidget::VisualEffectsPrefWidget()
   : PrefWidgetT<VisualEffectsPrefObject>()
{
   mUi.setupUi(this);
}

void wkf::VisualEffectsPrefWidget::ReadPreferenceData(const VisualEffectsPrefData& aPrefData)
{
   mUi.mShowTrailingEffects->setChecked(aPrefData.mShowTrailingEffects);
   mUi.mShowExplosions->setChecked(aPrefData.mShowExplosions);
   mUi.mMapExplosionScalar->setValue(aPrefData.mMapExplosionScalar);
   mUi.mMapExplosionLifetime->setValue(aPrefData.mMapExplosionLifetime);
}

void wkf::VisualEffectsPrefWidget::WritePreferenceData(VisualEffectsPrefData& aPrefData)
{
   aPrefData.mShowTrailingEffects  = mUi.mShowTrailingEffects->isChecked();
   aPrefData.mShowExplosions       = mUi.mShowExplosions->isChecked();
   aPrefData.mMapExplosionScalar   = mUi.mMapExplosionScalar->value();
   aPrefData.mMapExplosionLifetime = mUi.mMapExplosionLifetime->value();
}
