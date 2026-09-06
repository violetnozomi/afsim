// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfMoonOrbitPrefWidget.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>

#include "UtQtColorButton.hpp"
#include "UtQtMemory.hpp"

wkf::MoonOrbitPrefWidget::MoonOrbitPrefWidget()
   : wkf::PrefWidgetT<wkf::MoonOrbitPrefObject>()
{
   mUi.setupUi(this);
}


void wkf::MoonOrbitPrefWidget::WritePreferenceData(MoonOrbitPrefData& aPrefData)
{
   aPrefData.mVisible   = mUi.mVisibilityBox->isChecked();
   aPrefData.mColor     = mUi.mColorButton->GetColor();
   aPrefData.mLineWidth = mUi.mLineWidthBox->value();
}

void wkf::MoonOrbitPrefWidget::ReadPreferenceData(const MoonOrbitPrefData& aPrefData)
{
   mUi.mVisibilityBox->setChecked(aPrefData.mVisible);
   mUi.mColorButton->SetColor(aPrefData.mColor);
   mUi.mLineWidthBox->setValue(aPrefData.mLineWidth);
}
