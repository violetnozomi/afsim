// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLOTTINGWIDGET_HPP
#define RVPLOTTINGWIDGET_HPP

#include "PlotUpdater.hpp"
#include "WkfDockWidget.hpp"
#include "ui_RvTrackPlottingWidget.h"

class UtQtGL2DPlotWidget;

namespace rv
{
class ResultData;
}

namespace RvTracks
{
class PlottingWidget : public wkf::DockWidget
{
   Q_OBJECT

public:
   PlottingWidget(QMainWindow* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~PlottingWidget() override = default;

   void SetUpdater(std::unique_ptr<RvTracks::PlotUpdater> aPlotUpdater);
signals:
   void PlotChanged();

private:
   void    SetTime(const rv::ResultData& aData);
   void    UnitsChangedCB();
   void    Clear();
   void    PrepareForDockStateChange(bool aFloating) override;
   QString GetYAxisText() const;
   QString GetXAxisText() const;
   void    SetYAxisText(const QString& aText);
   void    SetXAxisText(const QString& aText);
   void    BuildPlotsFromUpdater();
   void    AddDataPoint(double aXPt, double aYPt, unsigned int aSeriesNum);
   void    PopulateAxisComboBoxesX(QStringList aList);
   void    PopulateAxisComboBoxesY(QStringList aList);
   bool    BuildContextMenu(QMenu& aMenu, const QPoint& pos) override;

   QString                                mPlatformName;
   Ui::RvTrackPlottingWidget              mUi;
   std::unique_ptr<RvTracks::PlotUpdater> mPlotUpdaterPtr;
};
} // namespace RvTracks

#endif
