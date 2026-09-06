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

#ifndef WkfStartupWidgetWrapper_HPP
#define WkfStartupWidgetWrapper_HPP

#include "wkf_export.h"

#include <QLayout>
#include <QWidget>

namespace wkf
{

//! StartupWidget is an interface for a widget that allows plugin makers to add custom widgets and tips to the startup dialog
class WKF_EXPORT StartupWidgetWrapper : public QObject
{
   Q_OBJECT

public:
   StartupWidgetWrapper() {}
   StartupWidgetWrapper(const QString& aLayoutString, QWidget* aWidgetPtr)
      : mTargetGroupBox(aLayoutString)
      , mWidgetPtr(aWidgetPtr)
   {
   }
   StartupWidgetWrapper(const QString& aLayoutString, QBoxLayout* aLayoutPtr)
      : mTargetGroupBox(aLayoutString)
      , mLayoutPtr(aLayoutPtr)
   {
   }

   ~StartupWidgetWrapper() = default;

   // This returns the group box within which the widget or layout will be placed
   virtual const QString GetTargetGroupBox() const { return mTargetGroupBox; }

   QWidget*    GetWidget() { return mWidgetPtr; }
   QBoxLayout* GetLayout() { return mLayoutPtr; }

signals:
   void CloseStartDialog();

private:
   QString     mTargetGroupBox = "";
   QWidget*    mWidgetPtr      = nullptr;
   QBoxLayout* mLayoutPtr      = nullptr;
};
} // namespace wkf
#endif
