// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "TetherViewPrefWidget.hpp"

#include "TetherViewPrefObject.hpp"

TetherView::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<PrefObject>()
{
   mUi.setupUi(this);
}

void TetherView::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   mUi.lockCameraCheckBox->setChecked(aPrefData.mLockCameraToHeadingEnabled);
   mUi.enableLighting->setChecked(aPrefData.mEnableLighting);
}

void TetherView::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   aPrefData.mLockCameraToHeadingEnabled = mUi.lockCameraCheckBox->isChecked();
   aPrefData.mEnableLighting             = mUi.enableLighting->isChecked();
}
