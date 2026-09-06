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
#include "RvInterpolationPrefWidget.hpp"

#include "RvInterpolationPrefObject.hpp"
#include "ui_RvInterpolationPrefWidget.h"

rv::InterpolationPrefWidget::InterpolationPrefWidget(QWidget* parent)
   : PrefWidgetT<InterpolationPrefObject>(parent)
{
   mUi.setupUi(this);
}

void rv::InterpolationPrefWidget::WritePreferenceData(InterpolationPrefData& aPrefData)
{
   aPrefData.mInterpolateAngles   = mUi.interpolateOrientation->isChecked();
   aPrefData.mExtrapolatePosition = mUi.extrapolatePosition->isChecked();
}

void rv::InterpolationPrefWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void rv::InterpolationPrefWidget::ReadPreferenceData(const InterpolationPrefData& aPrefData)
{
   mUi.interpolateOrientation->setChecked(aPrefData.mInterpolateAngles);
   mUi.extrapolatePosition->setChecked(aPrefData.mExtrapolatePosition);
}
