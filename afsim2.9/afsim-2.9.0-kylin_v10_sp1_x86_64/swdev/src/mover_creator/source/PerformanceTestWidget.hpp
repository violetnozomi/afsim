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

#ifndef PERFORMANCE_TEST_WIDGET_HPP
#define PERFORMANCE_TEST_WIDGET_HPP

#include <QWidget>

class QTimer;
namespace Ui
{
   class PerformanceTestWidget;
}

namespace Designer
{
   class PerformanceTestWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit PerformanceTestWidget(QWidget*        aParent,
                                        Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~PerformanceTestWidget();

         double RunMaxSpeedTest();
         double RunMinSpeedTest();
         double RunMaxGLoadTest();
         double RunMaxSustainedGLoadTest();
         double RunMaxLevelFlightTest();
         double RunMaxLoftedRangeTest();
         double RunMaxSustainedPitchAngleTest();
         double RunMaxRollRateTest();
         double RunTimeToDampRollTest();
         double RunMaxPitchRateTest();
         double RunTimeToDampPitchTest();
         double RunMaxYawRateTest();
         double RunTimeToDampYawTest();

      private slots:
         void RunTestClicked(bool aClicked);
         void UpdateProgressBar();
         void HandleTestChanged();

      private:
         Ui::PerformanceTestWidget* mUIPtr;
         QTimer*                    mProgressTimer;
   };
}
#endif // !PERFORMANCE_TEST_WIDGET_HPP

