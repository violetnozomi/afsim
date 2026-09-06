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

#include "WkfCommentPrefWidget.hpp"

#include <QFormLayout>
#include <QGroupBox>

wkf::CommentPrefWidget::CommentPrefWidget()
   : wkf::PrefWidgetT<wkf::CommentPrefObject>()
   , mUI()
{
   mUI.setupUi(this);
}

void wkf::CommentPrefWidget::ReadPreferenceData(const CommentPrefData& aPrefData)
{
   mUI.enableGroupBox->setChecked(aPrefData.mEnabled);
   mUI.timeoutSpinBox->setValue(aPrefData.mTimeoutSec);
   mUI.timestampCheckBox->setChecked(aPrefData.mShowTimestamp);
}

void wkf::CommentPrefWidget::WritePreferenceData(CommentPrefData& aPrefData)
{
   aPrefData.mTimeoutSec    = mUI.timeoutSpinBox->value();
   aPrefData.mEnabled       = mUI.enableGroupBox->isChecked();
   aPrefData.mShowTimestamp = mUI.timestampCheckBox->isChecked();
}
