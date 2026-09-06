// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTABSTRACTTABLEMODEL_HPP
#define UTQTABSTRACTTABLEMODEL_HPP
#include "utilqt_export.h"

#include <QAbstractItemModel>

// Slightly Simplifies QAbstractItemModel's interface to work for tables
class UTILQT_EXPORT UtQtAbstractTableModel : public QAbstractItemModel
{
public:
   UtQtAbstractTableModel(QObject* parent = nullptr, int aInitialRows = 0, int aInitialColumns = 0);
   UtQtAbstractTableModel(QObject* parent, const UtQtAbstractTableModel& aRhs)
      : QAbstractItemModel(parent)
      , mRows(aRhs.mRows)
      , mColumns(aRhs.mColumns)
   {
   }
   QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

   QModelIndex parent(const QModelIndex& child) const override { return QModelIndex(); }

   // implement:
   // virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
   // virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
protected:
   // Table shape:
   //   These methods should be used when changing rows/columns


   void tableBeginInsertRows(int aRowIndex, int aCount)
   {
      beginInsertRows(QModelIndex(), aRowIndex, aRowIndex + aCount - 1);
      mRows += aCount;
   }
   void tableEndInsertRows() { endInsertRows(); }
   // helper method for a common operation
   void tableBeginAppendRow() { tableBeginInsertRows(mRows, 1); }

   void tableBeginRemoveRows(int aRowIndex, int aRowCount)
   {
      beginRemoveRows(QModelIndex(), aRowIndex, aRowIndex + aRowCount - 1);
      mRows -= aRowCount;
   }
   void tableEndRemoveRows() { endRemoveRows(); }

   void tableBeginInsertColumns(int aColumnIndex, int aCount)
   {
      beginInsertColumns(QModelIndex(), aColumnIndex, aColumnIndex + aCount - 1);
      mColumns += aCount;
   }
   void tableBeginAppendColumn() { tableBeginInsertColumns(mColumns, 1); }
   void tableEndInsertColumns() { endInsertColumns(); }

   void tableBeginRemoveColumns(int aColumnIndex, int aColumnCount)
   {
      beginRemoveColumns(QModelIndex(), aColumnIndex, aColumnIndex + aColumnCount - 1);
      mColumns -= aColumnCount;
   }
   void tableEndRemoveColumns() { endRemoveColumns(); }

   // if data has changed, call tableDataChanged
   void tableDataChanged(int aRowStart, int aRowEnd, int aColumnStart = -1, int aColumnEnd = -1)
   {
      if (aColumnStart == -1)
      {
         aColumnStart = 0;
         aColumnEnd   = mColumns;
      }
      dataChanged(createIndex(aRowStart, aColumnStart), createIndex(aRowEnd, aColumnEnd));
   }

   void tableClearRows()
   {
      tableBeginRemoveRows(0, mRows);
      mRows = 0;
      endRemoveRows();
   }
   void tableClearColumns()
   {
      if (mRows)
      {
         tableBeginRemoveColumns(0, mColumns);
         mColumns = 0;
         endRemoveColumns();
      }
      else
      {
         mColumns = 0;
      }
   }

protected:
   int rowCount(const QModelIndex& parent = QModelIndex()) const override;

   int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
   // hide these methods the derived class should not need
   using QAbstractItemModel::beginInsertColumns;
   using QAbstractItemModel::beginInsertRows;
   using QAbstractItemModel::endInsertColumns;
   using QAbstractItemModel::endInsertRows;


   int mRows;
   int mColumns;
};

#endif
