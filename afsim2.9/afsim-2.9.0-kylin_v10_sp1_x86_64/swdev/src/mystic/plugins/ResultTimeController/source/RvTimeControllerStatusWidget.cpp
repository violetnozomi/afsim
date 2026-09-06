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
#include "RvTimeControllerStatusWidget.hpp"

#include <cmath>

#include <QDateTime>
#include <QDrag>
#include <QMimeData>

#include "RvTimeControllerInterface.hpp"
#include "RvTimeControllerPrefObject.hpp"

RvTimeController::StatusWidget::StatusWidget(RvTimeController::Interface*        aDataInterfacePtr,
                                             const RvTimeController::PrefObject* aPrefObjectPtr,
                                             const size_t                        aUniqueId)
   : QWidget()
   , mDataInterfacePtr(aDataInterfacePtr)
   , mPrefObjectPtr(aPrefObjectPtr)
   , mPluginUniqueId(aUniqueId)
{
   mUi.setupUi(this);
   connect(mPrefObjectPtr,
           &RvTimeController::PrefObject::ShowClockRateChanged,
           [this](bool aShow) { mUi.mClockRateWidget->setVisible(aShow); });
   connect(mPrefObjectPtr,
           &RvTimeController::PrefObject::ShowSimTimeChanged,
           [this](bool aShow) { mUi.mSimTimeWidget->setVisible(aShow); });
   connect(mPrefObjectPtr,
           &RvTimeController::PrefObject::ShowWallTimeChanged,
           [this](bool aShow) { mUi.mWallTimeWidget->setVisible(aShow); });
   mUi.mClockRateWidget->installEventFilter(this);
   mUi.mSimTimeWidget->installEventFilter(this);
   mUi.mWallTimeWidget->installEventFilter(this);
   connect(mPrefObjectPtr, &RvTimeController::PrefObject::WallTimeSpecChanged, this, &RvTimeController::StatusWidget::Update);
   wkf::UnitsObject* unitPrefs = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   mDateFormat                 = unitPrefs->GetDateFormatting();
   connect(unitPrefs, &wkf::UnitsObject::DateFormatChanged, this, &StatusWidget::DateFormatChanged);
}

void RvTimeController::StatusWidget::Update()
{
   double simTimeSec = mDataInterfacePtr->GetSimTime();
   // Update wall time
   UtCalendar calendar = mDataInterfacePtr->GetWallTime();
   calendar.AdvanceTimeBy(simTimeSec);
   QDate     date(calendar.GetYear(), calendar.GetMonth(), calendar.GetDay());
   QTime     time(calendar.GetHour(), calendar.GetMinute(), calendar.GetSecond());
   QDateTime wallTime(date, time, Qt::UTC);
   if (mPrefObjectPtr->GetWallTimeSpec() == Qt::LocalTime)
   {
      wallTime = wallTime.toLocalTime();
   }
   else if (mPrefObjectPtr->GetWallTimeSpec() == Qt::OffsetFromUTC)
   {
      wallTime = wallTime.toOffsetFromUtc((3600 * mPrefObjectPtr->GetUTC_OffsetHrs()) +
                                          (60 * mPrefObjectPtr->GetUTC_OffsetMins()));
   }
   if (mDateFormat == wkf::UnitsData::DATE_ISO)
   {
      mUi.mWallTimeLabel->setText(wallTime.toString(Qt::ISODate));
   }
   else if (mDateFormat == wkf::UnitsData::DATE_SHORT)
   {
      mUi.mWallTimeLabel->setText(wallTime.toString("MM/dd/yy hh:mm:ss"));
   }
   else
   {
      mUi.mWallTimeLabel->setText(wallTime.toString("MMM d yyyy hh:mm:ss"));
   }

   mUi.mSimTimeLabel->SetValue(simTimeSec);
   //   mUi.mSimTimeLabel->setText(simTime.addSecs(simTimeSec).addMSecs(simTimeMsec).toString("hh:mm:ss.zzz"));

   // Update simulation clock rate
   double clockRate = mDataInterfacePtr->GetClockRate();
   mUi.mClockRateLabel->setText(QString("x%1").arg(clockRate, 0, 'f', 2));
}

bool RvTimeController::StatusWidget::eventFilter(QObject* aObj, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::MouseButtonPress)
   {
      QMimeData*  mimeData = new QMimeData;
      QByteArray  bytes;
      QDataStream out(&bytes, QIODevice::WriteOnly);

      unsigned int qi = static_cast<unsigned int>(mPluginUniqueId);
      out << qi;
      out << QString();
      out << aObj->objectName();
      mimeData->setData(wkf::Updater::cMIME_IDENTIFIER, bytes); // need to give an address to the updater here
      QDrag* drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->exec(Qt::DropAction::LinkAction);
      // capture the updater and pass it to whoever receives the drop
      return true;
   }
   return false;
}

void RvTimeController::StatusWidget::DateFormatChanged(wkf::UnitsData::DateFormat aFormat)
{
   mDateFormat = aFormat;
   Update();
}
