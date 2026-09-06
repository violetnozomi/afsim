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

#include "UtQtPlotChooser.hpp"

#include <QVBoxLayout>

#include "UtQtGL2DPlotWidget.hpp"
#include "UtQtPieChart.hpp"

UtQtPlotChooser::UtQtPlotChooser(QWidget* aParent)
   : QWidget(aParent)
   , mPlot2DPtr(nullptr)
   , mPieChartPtr(nullptr)
   , mShowCrosshairs(false)
   , mShowCrosshairsOnAllSeries(false)
   , mShowLegend(false)
   , mCustomContext(false)
{
   setObjectName("plot_chooser");
   setLayout(new QVBoxLayout(this));
   layout()->setContentsMargins(0, 0, 0, 0);

   SetMode(1);
}

void UtQtPlotChooser::SetMode(int aMode)
{
   if (aMode == 1)
   {
      if (mPieChartPtr != nullptr)
      {
         delete mPieChartPtr;
         mPieChartPtr = nullptr;
      }
      if (mPlot2DPtr == nullptr)
      {
         mPlot2DPtr = new UtQtGL2DPlotWidget(this);
         mPlot2DPtr->setObjectName("2d_plot");
         layout()->addWidget(mPlot2DPtr);
         if (mCustomContext)
         {
            mPlot2DPtr->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(mPlot2DPtr,
                    &UtQtGL2DPlotWidget::customContextMenuRequested,
                    [this](const QPoint& aPos) {
                       emit CustomContextMenu(mPlot2DPtr->mapToGlobal(aPos), mPlot2DPtr->ConvertPositionToData(aPos), true);
                    });
         }
      }
   }
   else if (aMode == 2)
   {
      if (mPlot2DPtr != nullptr)
      {
         delete mPlot2DPtr;
         mPlot2DPtr = nullptr;
      }
      if (mPieChartPtr == nullptr)
      {
         mPieChartPtr = new UtQtPieChart(this);
         mPieChartPtr->setObjectName("pie_chart");
         layout()->addWidget(mPieChartPtr);
         if (mCustomContext)
         {
            mPieChartPtr->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(mPieChartPtr,
                    &UtQtGL2DPlotWidget::customContextMenuRequested,
                    [this](const QPoint& aPos)
                    { emit CustomContextMenu(mPieChartPtr->mapToGlobal(aPos), QPoint(), false); });
         }
      }
   }
   SetShowCrosshairs(mShowCrosshairs);
   SetCrosshairsShowAllSeries(mShowCrosshairsOnAllSeries);
   SetShowLegend(mShowLegend);
   SetCrosshairsValueX(mCrosshairX_Value);
   MakePlotPretty();
}

void UtQtPlotChooser::SetShowCrosshairs(bool aState)
{
   mShowCrosshairs = aState;
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->SetShowCrosshairs(aState);
   }
}

void UtQtPlotChooser::SetCrosshairsShowAllSeries(bool aState)
{
   mShowCrosshairsOnAllSeries = aState;
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->SetCrosshairsShowAllSeries(aState);
   }
}

void UtQtPlotChooser::SetShowLegend(bool aState)
{
   mShowLegend = aState;
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->SetShowLegend(aState);
   }
   if (mPieChartPtr != nullptr)
   {
      mPieChartPtr->ShowKey(aState);
   }
}

void UtQtPlotChooser::SetCrosshairsValueX(float aValue)
{
   mCrosshairX_Value = aValue;
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->SetCrosshairsValueX(aValue);
   }
}

void UtQtPlotChooser::MakePlotPretty()
{
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->MakePlotPretty();
   }
}

void UtQtPlotChooser::AddModelSeries(QAbstractItemModel* aModel, int aColumnX, int aColumnY)
{
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->AddModelSeries(aModel,
                                            aColumnX,
                                            aColumnY,
                                            2 + (unsigned int)mPlot2DPtr->GetPlot()->GetNumberOfSeries());
   }
   else if (mPieChartPtr != nullptr)
   {
      mPieChartPtr->AddModelSeries(aModel, aColumnY);
   }
}

void UtQtPlotChooser::ClearModelSeries()
{
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->GetPlot()->ClearModelSeries();
   }
   else if (mPieChartPtr != nullptr)
   {
      mPieChartPtr->Clear();
   }
}

void UtQtPlotChooser::UpdatePlot()
{
   if (mPlot2DPtr != nullptr)
   {
      mPlot2DPtr->update();
   }
   else if (mPieChartPtr != nullptr)
   {
      mPieChartPtr->update();
   }
}

void UtQtPlotChooser::EnableCustomPlotContextMenu()
{
   if (!mCustomContext)
   {
      mCustomContext = true;
      if (mPlot2DPtr)
      {
         mPlot2DPtr->setContextMenuPolicy(Qt::CustomContextMenu);
         connect(mPlot2DPtr,
                 &UtQtGL2DPlotWidget::customContextMenuRequested,
                 [this](const QPoint& aPos) {
                    emit CustomContextMenu(mPlot2DPtr->mapToGlobal(aPos), mPlot2DPtr->ConvertPositionToData(aPos), true);
                 });
      }
      else if (mPieChartPtr)
      {
         mPieChartPtr->setContextMenuPolicy(Qt::CustomContextMenu);
         connect(mPieChartPtr,
                 &UtQtGL2DPlotWidget::customContextMenuRequested,
                 [this](const QPoint& aPos) { emit CustomContextMenu(mPieChartPtr->mapToGlobal(aPos), QPoint(), false); });
      }
   }
}