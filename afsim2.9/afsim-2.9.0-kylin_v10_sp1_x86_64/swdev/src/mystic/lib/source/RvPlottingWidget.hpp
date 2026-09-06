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
#ifndef RVPLOTTINGWIDGET_HPP
#define RVPLOTTINGWIDGET_HPP

#include <memory>

#include "RvExport.hpp"
#include "RvPlotUpdater.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfDockWidget.hpp"
#include "ui_RvPlottingWidget.h"

class UtQtGL2DPlotWidget;

namespace wkf
{
class Platform;
}

namespace rv
{
class PlotUpdater;
class ResultData;

class RV_EXPORT PlottingWidget : public wkf::DockWidget
{
   Q_OBJECT

public:
   PlottingWidget(QMainWindow* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~PlottingWidget() override = default;

   void SetDataFromPlatforms(const int      aCategoryType,
                             const QString& aCategoryName,
                             const QString& aXDatumName,
                             const QString& aYDatumName);

   void        SetSeriesName(const std::string& aPlatformName, unsigned int aSeriesNum);
   void        AddDataPoint(double aXPt, double aYPt, unsigned int aSeriesNum);
   void        AddPlatformObject(wkf::Platform* aPlatformPtr);
   void        AddPlatform(std::string aPlatformName);
   void        ClearPlot();
   void        Clear();
   void        FinishPlotting();
   QStringList GetSelectedText() const;
   QString     GetXAxisText() const;
   QString     GetYAxisText() const;

   void SetXAxisText(QString aText);
   void SetYAxisText(QString aText);
   void SetSelectedPlatform(QString aPlatformName);

   void PopulateAxisComboBoxesX(QStringList aList);
   void PopulateAxisComboBoxesY(QStringList aList);

   void SetTime(const ResultData& aData);

   void SetUpdater(std::unique_ptr<rv::PlotUpdater> aPlotUpdater);

   bool BuildContextMenu(QMenu& aMenu, const QPoint& pos) override;
private slots:
   void PlatformSelectionChanged();
   void XAxisChanged(int aIndex);
   void YAxisChanged(int aIndex);
   void BuildPlotsFromUpdater();
   void UnitsChangedCB();
   void ApplyFilter(const QString& aFilter);
signals:
   void PlotChanged();

private:
   void PrepareForDockStateChange(bool aFloating) override;

   void SetSampleRateAction();

   QString GetXAxisLabel() const;
   QString GetYAxisLabel() const;

   vespa::VaCallbackHolder          mCallbacks;
   Ui::RvPlottingWidget             mUi;
   UtQtGL2DPlotWidget*              mPlotWidgetPtr;
   std::unique_ptr<rv::PlotUpdater> mPlotUpdaterPtr;
};
} // namespace rv
#endif
