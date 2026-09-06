// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PerformanceDialog.hpp"

#include <chrono>

Performance::Dialog::Dialog(QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mMemoryOverlayID(-1)
   , mMemoryMin(DBL_MAX)
   , mMemoryMax(-DBL_MAX)
   , mStartTime(std::chrono::system_clock::now())
{
   mUi.setupUi(this);

   connect(mUi.closePushButton, &QPushButton::clicked, this, &QDialog::close);

   Build();
   Update();
}

void Performance::Dialog::Build()
{
   mUi.mChartWidget->SetEnableDefaultContextMenu(true);
   mUi.mChartWidget->SetEnableCrosshairMouseControlled(true);

   UtQtGL2DPlot* plotPtr = mUi.mChartWidget->GetPlot();
   plotPtr->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   plotPtr->SetLabelYAxis("MBytes");
   plotPtr->SetLabelXAxis("seconds");
   plotPtr->SetSeriesColor(QColor(10, 240, 240), 1);

   plotPtr->SetTickIncrementX(10);
   plotPtr->SetTickJustifiedX(false);
}

void Performance::Dialog::Update()
{
   double memoryUsage = mMemoryUsage.GetMemoryUsageInMBytes();

   std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - mStartTime;

   // Plot
   UtQtGL2DPlot* plotPtr = mUi.mChartWidget->GetPlot();
   plotPtr->AddPoint(elapsed_seconds.count(), memoryUsage);

   double minX, minY, maxX, maxY;
   plotPtr->MakePlotPrettyY();
   plotPtr->GetDataMinMax(minX, minY, maxX, maxY);

   plotPtr->SetXAxisBounds(std::max(maxX - 60.0, 0.0), maxX);

   if ((maxX - minX) > 100.0)
   {
      for (unsigned int i = 0; i < 10; ++i)
      {
         plotPtr->RemovePoint(i);
      }
   }

   // Update min & max values
   mMemoryMin = std::min(mMemoryMin, memoryUsage);
   mMemoryMax = std::max(mMemoryMax, memoryUsage);
   mUi.memoryMinValueLabel->setText(QString::number(mMemoryMin) + " MBytes");
   mUi.memoryMaxValueLabel->setText(QString::number(mMemoryMax) + " MBytes");
}
