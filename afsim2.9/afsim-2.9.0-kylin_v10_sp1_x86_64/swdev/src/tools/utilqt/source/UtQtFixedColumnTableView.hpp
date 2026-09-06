// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_QT_FIXED_COLUMN_TABLE_VIEW_HPP
#define UT_QT_FIXED_COLUMN_TABLE_VIEW_HPP

#include "utilqt_export.h"

#include <QTableView>

#include "UtQtTableView.hpp"

class QHBoxLayout;
class QItemSelectionModel;

namespace ut
{
namespace qt
{
//! Defines a table view with the first N columns remain fixed when the user scrolls horizontally.
class UTILQT_EXPORT FixedColumnTableView : public QWidget
{
   Q_OBJECT

public:
   using EditTriggers = QAbstractItemView::EditTriggers;
   using EditTrigger  = QAbstractItemView::EditTrigger;

   explicit FixedColumnTableView(QWidget* aParent);

   void SetModel(QAbstractTableModel* aModel);
   void SetFixedColumnCount(int aCount);
   void SetRowHeight(int aHeight);
   void SetVerticalHeaderVisible(bool aVisible);

   QAbstractTableModel* Model() const;
   QItemSelectionModel* SelectionModel() const;

   void SetFixedColumnsSortable(bool aSortable);

   void SetDataColumnsMovable(bool aMovable);
   void SetDataColumnsSortable(bool aSortable);
   void SetDataEditTriggers(EditTriggers aTriggers);

   void SetStretchLastColumn(bool aExpand);

   void SetColumnVisible(int aColumn, bool aVisible);

   void ShowColumn(int aColumn);
   void HideColumn(int aColumn);

   int RowAt(int aY) const;
   int ColumnAt(int aX) const;

signals:
   void Clicked(const QModelIndex& aIndex);
   void DoubleClicked(const QModelIndex& aIndex);

protected:
   void resizeEvent(QResizeEvent* aEvent) override;
   void showEvent(QShowEvent* aEvent) override;

private:
   class TableView : public QTableView
   {
   public:
      using QTableView::QTableView;

      using QTableView::updateGeometries;
   };

   QHBoxLayout* mLayout    = nullptr;
   TableView*   mFixedView = nullptr;
   TableView*   mDataView  = nullptr;

   int mFixedColumnCount = 0;

   void UpdateGeometry();

private slots:
   void ColumnsInserted(const QModelIndex& aParent, int aFirst, int aLast);

   void UpdateSectionWidth(int aIndex, int aOldSize, int aNewSize);
   void UpdateSectionHeight(int aIndex, int aOldSize, int aNewSize);
};
} // namespace qt
} // namespace ut

#endif
