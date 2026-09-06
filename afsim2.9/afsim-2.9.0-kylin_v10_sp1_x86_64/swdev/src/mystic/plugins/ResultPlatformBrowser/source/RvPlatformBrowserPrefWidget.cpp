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
#include "RvPlatformBrowserPrefWidget.hpp"

#include "RvPlatformBrowserPrefObject.hpp"
#include "WkfEnvironment.hpp"

RvPlatformBrowser::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<PrefObject>()
{
   mUi.setupUi(this);
}

void RvPlatformBrowser::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   aPrefData.mShowInactive = mUi.mShowInactive->isChecked();
}

void RvPlatformBrowser::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   mUi.mShowInactive->setChecked(aPrefData.mShowInactive);
}
