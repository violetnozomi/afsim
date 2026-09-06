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

#ifndef WKF_PLOT_WIDGET_HPP
#define WKF_PLOT_WIDGET_HPP

#include "wkf_export.h"

#include "UtQtGL2DPlotWidget.hpp"

namespace wkf
{

class WKF_EXPORT PlotWidget : public UtQtGL2DPlotWidget
{
   Q_OBJECT

public:
   PlotWidget(QWidget* aParent, UtQtGL2DPlot* aPlot = nullptr)
      : UtQtGL2DPlotWidget(aParent, aPlot)
   {
   }
   ~PlotWidget() override = default;

   //! builds the context menu
   //! @return a list of actions for the context menu
   //! @note the "Export Data" action will be disabled if wkfEnv.IsFileBrowsingLocked() is set true
   QList<QAction*> GetDefaultContextMenuActions() override;
};

} // namespace wkf

#endif // WKF_PLOT_WIDGET_HPP
