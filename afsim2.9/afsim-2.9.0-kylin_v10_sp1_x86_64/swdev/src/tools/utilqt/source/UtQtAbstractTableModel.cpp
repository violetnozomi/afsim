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

#include "UtQtAbstractTableModel.hpp"

UtQtAbstractTableModel::UtQtAbstractTableModel(QObject* parent /*= 0*/, int aInitialRows /*= 0*/, int aInitialColumns /*= 0*/)
   : QAbstractItemModel(parent)
   , mRows(aInitialRows)
   , mColumns(aInitialColumns)
{
}

QModelIndex UtQtAbstractTableModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
   if (!parent.isValid() && row < mRows && column < mColumns)
   {
      return createIndex(row, column);
   }
   return QModelIndex();
}

int UtQtAbstractTableModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
   if (!parent.isValid())
   {
      return mRows;
   }
   return 0;
}

int UtQtAbstractTableModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
   if (!parent.isValid())
   {
      return mColumns;
   }
   return 0;
}
