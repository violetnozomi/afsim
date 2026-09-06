// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPlotWidget.hpp"

#include "WkfEnvironment.hpp"

namespace wkf
{

// =================================================================================================
// override
QList<QAction*> PlotWidget::GetDefaultContextMenuActions()
{
   QList<QAction*> actions;

   QAction* dataViewAction = new QAction(this);
   QAction* exportAction   = new QAction(this);
   actions.push_back(dataViewAction);
   actions.push_back(exportAction);

   if (mDataView)
   {
      // Create option to move to Plot View page
      dataViewAction->setText("Plot View");
      connect(dataViewAction, &QAction::triggered, this, &wkf::PlotWidget::DataViewAction);

      // Create option to export data
      exportAction->setText("Export Data");
      connect(exportAction, &QAction::triggered, this, &wkf::PlotWidget::ExportDataAction);
      // disable the "Export Data" option if the file dialog interaction is disabled
      exportAction->setDisabled(wkfEnv.IsFileBrowsingLocked());
   }
   else
   {
      // Create option to move to Data View page
      dataViewAction->setText("Data View");
      connect(dataViewAction, &QAction::triggered, this, &wkf::PlotWidget::DataViewAction);

      // Create option to export data
      exportAction->setText("Export Data");
      connect(exportAction, &QAction::triggered, this, &wkf::PlotWidget::ExportDataAction);
      // disable the "Export Data" option if the file dialog interaction is disabled
      exportAction->setDisabled(wkfEnv.IsFileBrowsingLocked());

      // Create option to toggle the legend
      QAction* legendAction = new QAction(this);
      if (mPlot->GetShowLegend())
      {
         legendAction->setText("Hide Legend");
      }
      else
      {
         legendAction->setText("Show Legend");
      }

      connect(legendAction, &QAction::triggered, this, &wkf::PlotWidget::LegendAction);

      actions.push_back(legendAction);
   }
   return actions;
}

} // namespace wkf
