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

#ifndef UTQTPLOTCHOOSER_HPP
#define UTQTPLOTCHOOSER_HPP

#include <QWidget>

class QAbstractItemModel;
class UtQtGL2DPlotWidget;
class UtQtPieChart;

class UtQtPlotChooser : public QWidget
{
   Q_OBJECT
public:
   UtQtPlotChooser(QWidget* aParent);
   ~UtQtPlotChooser() override = default;

   void SetMode(int aMode);

   void AddModelSeries(QAbstractItemModel* aModel, int aColumnX, int aColumnY);
   void ClearModelSeries();

   void SetShowCrosshairs(bool aState);
   void SetCrosshairsShowAllSeries(bool aState);
   void SetShowLegend(bool aState);
   void SetCrosshairsValueX(float aValue);
   void MakePlotPretty();
   void UpdatePlot();
   void EnableCustomPlotContextMenu();
signals:
   void CustomContextMenu(const QPoint& aPos, const QPointF& aDataPoint, bool aPlot2D);

private:
   UtQtGL2DPlotWidget* mPlot2DPtr;
   UtQtPieChart*       mPieChartPtr;

   bool  mShowCrosshairs;
   bool  mShowCrosshairsOnAllSeries;
   bool  mShowLegend;
   float mCrosshairX_Value;
   bool  mCustomContext;
};

#endif
