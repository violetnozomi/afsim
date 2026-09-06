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
#include "RvMemoryPrefWidget.hpp"

#include "RvMemoryPrefObject.hpp"

rv::MemoryPrefWidget::MemoryPrefWidget(QWidget* parent)
   : PrefWidgetT<MemoryPrefObject>(parent)
{
   mUi.setupUi(this);
}

void rv::MemoryPrefWidget::WritePreferenceData(MemoryPrefData& aPrefData)
{
   aPrefData.mPagesInCache = mUi.pagesInCache->value();
}

void rv::MemoryPrefWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void rv::MemoryPrefWidget::ReadPreferenceData(const MemoryPrefData& aPrefData)
{
   mUi.pagesInCache->setValue(aPrefData.mPagesInCache);
}
