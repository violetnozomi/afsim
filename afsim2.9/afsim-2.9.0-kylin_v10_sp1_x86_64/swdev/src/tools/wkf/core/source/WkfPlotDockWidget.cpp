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

#include "WkfPlotDockWidget.hpp"

#include <QContextMenuEvent>
#include <QDoubleValidator>
#include <QList>
#include <QListIterator>
#include <QMenu>

#include "UtQt.hpp"
#include "UtQtGL2DPlot.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

wkf::PlotDockWidget::PlotDockWidget(QMainWindow* aParent /*= 0*/, Qt::WindowFlags aFlags /*= 0*/)
   : DockWidget("plotwidget", aParent, aFlags)
{
   mUi.setupUi(this);

   mUi.xAxisBounds->SetXAxisControls();
   mUi.yAxisBounds->SetYAxisControls();

   mUi.plotWidget->setContextMenuPolicy(Qt::NoContextMenu);
   mUi.plotWidget->EnableMouseZoom(true);

   mPlot = mUi.plotWidget->GetPlot();
   mPlot->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   mPlot->SetSeriesColor(QColor(255, 0, 0));

   connect(mUi.xAxisBounds, &ut::qt::AxisBoundsWidget::BoundsChanged, this, &PlotDockWidget::PlotBoundsChanged);

   connect(mUi.xAxisBounds, &ut::qt::AxisBoundsWidget::BoundsChanged, this, &PlotDockWidget::PlotBoundsChanged);

   connect(mUi.clearDataPushButton, &QPushButton::clicked, this, &PlotDockWidget::ClearPlotDataHandler);

   PrepareForDockStateChange(isFloating());
}

bool wkf::PlotDockWidget::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   if (mUi.plotWidget->contentsRect().contains(aPos))
   {
      aMenu.addActions(mUi.plotWidget->GetDefaultContextMenuActions());
   }
   return true;
}

void wkf::PlotDockWidget::ClearPlotDataHandler()
{
   // Clear the data from the plot
   mPlot->ClearData();
   // Immediately redraw the plot to remove the data from the rendered scene
   mUi.plotWidget->Update();
}

void wkf::PlotDockWidget::PlotBoundsChanged()
{
   UpdatePlotBoundaries();
}

void wkf::PlotDockWidget::UpdatePlotBoundaries(bool aEndZoom)
{
   if (aEndZoom)
   {
      mUi.plotWidget->EndZoom();
   }

   // If the user is currently zoomed in, do not update the boundaries
   if (!mUi.plotWidget->IsZoomed())
   {
      mUi.xAxisBounds->SetXAxisBounds(*mPlot);
      mUi.yAxisBounds->SetYAxisBounds(*mPlot);
   }
}

void wkf::PlotDockWidget::PrepareForDockStateChange(bool aFloating)
{
   if (aFloating)
   {
      mUi.clearDataPushButton->show();
      mUi.yAxisBounds->show();
      mUi.xAxisBounds->show();
   }
   else
   {
      mUi.clearDataPushButton->hide();
      mUi.yAxisBounds->hide();
      mUi.xAxisBounds->hide();
   }
}
