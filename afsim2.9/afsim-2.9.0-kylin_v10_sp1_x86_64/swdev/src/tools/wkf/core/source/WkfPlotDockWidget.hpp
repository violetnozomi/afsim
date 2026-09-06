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
#ifndef WkfPlotDockWidget_HPP
#define WkfPlotDockWidget_HPP

#include "wkf_export.h"

#include <QSettings>

#include "VaCallbackHolder.hpp"
#include "WkfDockWidget.hpp"
#include "ui_WkfPlotDockWidget.h"

class QMenu;
class UtQtGL2DPlot;

namespace vespa
{
class VaEntity;
class VaViewer;
} // namespace vespa

namespace wkf
{
class WKF_EXPORT PlotDockWidget : public DockWidget
{
   Q_OBJECT

public:
   PlotDockWidget(QMainWindow* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());
   ~PlotDockWidget() override = default;

   UtQtGL2DPlot* GetPlot() { return mPlot; }

   //! builds the context menu
   //! @param aMenu is the context menu to build
   //! @param aPos is the point where to create the context menu
   //! @note disables file dialog interaction if the -lock_fileload command line argument is provided to the application
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) override;

protected:
   void ClearPlotDataHandler();

   void PlotBoundsChanged();
   void UpdatePlotBoundaries(bool aEndZoom = true);

   void PrepareForDockStateChange(bool aFloating) override;

   Ui::WkfPlotDockWidget mUi;
   UtQtGL2DPlot*         mPlot;
};
} // namespace wkf
#endif
