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

#ifndef UTQTTABLEPLOTWIDGET_HPP
#define UTQTTABLEPLOTWIDGET_HPP

// this class presents a table a 2D plot to the user
// data in these are populated by a provided model
// user can choose plots from the table
// user can customize the displayed columns of the table
// at the moment there is an assumption that this is time-based data

#include "utilqt_export.h"

#include <set>

#include <QWidget>

#include "ui_UtQtTablePlotWidget.h"

class QAbstractItemModel;
class QMenu;

class UTILQT_EXPORT UtQtTablePlotWidget : public QWidget
{
   Q_OBJECT
public:
   UtQtTablePlotWidget(QWidget* aParent);
   ~UtQtTablePlotWidget() override = default;

   virtual void SetModel(QAbstractItemModel* aModelPtr);
   void         ShowColumn(int aColumn, bool aState = true);
   void         SetTime(float aTime);
   void         AddPlot(int aColumn);
   void         SuppressNormalContextMenus(bool aState = true) { mContextSuppressed = aState; }
   void         AddActions(int aIndex, QMenu* aMenu);
   void         EnableCustomPlotContextMenu();
signals:
   void itemClicked(const QModelIndex& aIdx);
   void tableContext(const QModelIndex& aIndex, const QPoint& aPoint);
   void CustomPlotContext(const QPoint& aPos, const QPointF& aData, bool aPlot2D);
public slots:
   void itemClickedSlot(const QModelIndex& aIdx);
   void MakePretty();
   void ChooseColumnsSlot();
   void PlotSlot();
   void AddPlotSlot();
   void HideColumnSlot();
   void ExportVisibleSlot();
   void ExportAllSlot();

private:
   void HeaderContextMenu(const QPoint& aPoint);
   void TableContextMenu(const QPoint& aPoint);
   void ColumnContextMenu(int aIdx, const QPoint& aPoint);
   void ExportTable(bool aAll);

   Ui::UtQtTablePlotWidgetUi mUi;
   std::set<int>             mColumnDisplaySet;
   QAbstractItemModel*       mModelPtr;
   float                     mTime;
   bool                      mContextSuppressed;
   int                       mActivePlotType;
};

#endif
