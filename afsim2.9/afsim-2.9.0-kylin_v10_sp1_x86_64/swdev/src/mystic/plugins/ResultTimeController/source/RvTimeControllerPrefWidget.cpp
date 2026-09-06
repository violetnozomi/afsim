// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvTimeControllerPrefWidget.hpp"

#include <QTimeZone>

#include "RvTimeControllerPrefObject.hpp"

RvTimeController::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<RvTimeController::PrefObject>()
{
   mUi.setupUi(this);

   // There doesn't seem to be a way to add userData in Qt designer.
   mUi.mSpeedLimitComboBox->addItem("1,000 times real time", 3);
   mUi.mSpeedLimitComboBox->addItem("10,000 times real time", 4);
}

void RvTimeController::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   aPrefData.mShowClockRate    = mUi.mShowClockRateCheckBox->isChecked();
   aPrefData.mShowSimTime      = mUi.mShowSimTimeCheckBox->isChecked();
   aPrefData.mShowWallTime     = mUi.mShowWallTimeCheckBox->isChecked();
   aPrefData.mSpeedLimit       = mUi.mSpeedLimitComboBox->currentData().toInt();
   aPrefData.mUTC_OffsetHrs    = mUi.mUTC_OffsetHrsBox->value();
   aPrefData.mUTC_OffsetMins   = mUi.mUTC_OffsetMinsBox->value();
   aPrefData.mLoopedPlayback   = mUi.mLoop->isChecked();
   aPrefData.mExtrapolateAtEnd = mUi.mExtrapolate->isChecked();
   // mUi.mPause is the implicit third state

   if (mUi.mLocalTimeSpecButton->isChecked())
   {
      aPrefData.mWallTimeSpec = Qt::LocalTime;
   }
   else if (mUi.mUTC_TimeSpecButton->isChecked())
   {
      aPrefData.mWallTimeSpec = Qt::UTC;
   }
   else if (mUi.mUTC_OffsetTimeSpecButton->isChecked())
   {
      aPrefData.mWallTimeSpec = Qt::OffsetFromUTC;
   }
}

void RvTimeController::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   mUi.mShowClockRateCheckBox->setChecked(aPrefData.mShowClockRate);
   mUi.mShowSimTimeCheckBox->setChecked(aPrefData.mShowSimTime);
   mUi.mShowWallTimeCheckBox->setChecked(aPrefData.mShowWallTime);
   mUi.mSpeedLimitComboBox->setCurrentIndex(mUi.mSpeedLimitComboBox->findData(aPrefData.mSpeedLimit));

   mUi.mUTC_OffsetHrsBox->setValue(aPrefData.mUTC_OffsetHrs);
   mUi.mUTC_OffsetMinsBox->setValue(aPrefData.mUTC_OffsetMins);

   mUi.mPause->setChecked(true);
   mUi.mLoop->setChecked(aPrefData.mLoopedPlayback);
   mUi.mExtrapolate->setChecked(aPrefData.mExtrapolateAtEnd);

   switch (aPrefData.mWallTimeSpec)
   {
   case Qt::LocalTime:
      mUi.mLocalTimeSpecButton->setChecked(true);
      break;
   case Qt::UTC:
      mUi.mUTC_TimeSpecButton->setChecked(true);
      break;
   case Qt::OffsetFromUTC:
      mUi.mUTC_OffsetTimeSpecButton->setChecked(true);
      break;
   default:
      break;
   }
}
