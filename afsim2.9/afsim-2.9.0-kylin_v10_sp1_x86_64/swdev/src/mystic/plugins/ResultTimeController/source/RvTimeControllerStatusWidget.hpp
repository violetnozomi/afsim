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
#ifndef RVTIMECONTROLLERSTATUSWIDGET_HPP
#define RVTIMECONTROLLERSTATUSWIDGET_HPP

#include <QObject>
#include <QWidget>
class UtCalendar;

#include "WkfUnitsObject.hpp"
#include "ui_StatusBarWidget.h"

namespace RvTimeController
{
class Interface;
class PrefObject;

class StatusWidget : public QWidget
{
   //   Q_OBJECT

public:
   StatusWidget(Interface* aDataInterfacePtr, const PrefObject* aPrefObjectPtr, const size_t aPluginUniqueId);
   ~StatusWidget() override = default;

   void Update();
   //   public slots:
   //      void ShowClockRateHandler(bool aShow);
   //      void ShowSimTimeHandler(bool aShow);
   //      void ShowWallTimeHandler(bool aShow);

private:
   bool                       eventFilter(QObject* aObj, QEvent* aEvent) override;
   void                       DateFormatChanged(wkf::UnitsData::DateFormat aFormat);
   Interface*                 mDataInterfacePtr;
   const PrefObject*          mPrefObjectPtr;
   size_t                     mPluginUniqueId;
   wkf::UnitsData::DateFormat mDateFormat;
   Ui::StatusBarWidgetUi      mUi;
};
} // namespace RvTimeController
#endif // RVTIMECONTROLLERSTATUSWIDGET_HPP
