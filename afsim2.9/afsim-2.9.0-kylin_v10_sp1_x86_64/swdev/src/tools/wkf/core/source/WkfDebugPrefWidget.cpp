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

#include "WkfDebugPrefWidget.hpp"

wkf::DebugPrefWidget::DebugPrefWidget(QWidget* parent /*= nullptr*/)
   : PrefWidgetT<DebugPrefObject>(parent)
{
   mUi.setupUi(this);
}

void wkf::DebugPrefWidget::showEvent(QShowEvent* aEvent)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::DebugPrefWidget::ReadPreferenceData(const DebugPrefData& aPrefData)
{
   mUi.mDeveloperMenu->setChecked(aPrefData.mDeveloperMenu);
}

void wkf::DebugPrefWidget::WritePreferenceData(DebugPrefData& aPrefData)
{
   aPrefData.mDeveloperMenu = mUi.mDeveloperMenu->isChecked();
}
