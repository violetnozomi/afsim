// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FLIGHT_TEST_WIDGET_HPP
#define FLIGHT_TEST_WIDGET_HPP

#include <QWidget>
#include <QSettings>

class QProcess;

namespace Ui
{
   class FlightTestWidget;
}

namespace Designer
{
   class FlightTestWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit FlightTestWidget(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~FlightTestWidget();

         void showEvent(QShowEvent* aEvent);
         void PauseGLAnimation();

         void Reset() { }

      private slots:
         void LaunchTestClicked();
         void PreviousButtonClicked();
         void HandleProcessFinished();

      private:
         Ui::FlightTestWidget*   mUIPtr;
         QProcess*               mWarlockProcess     = nullptr;
         QString                 mFlightTestFileName = "";
         QString                 mCustomIniFile      = "";
         QSettings*              mWarlockSettings    = nullptr;
   };
}
#endif // !FLIGHT_TEST_WIDGET_HPP
