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

#ifndef WKFABSTRACTTABLEMODEL_HPP
#define WKFABSTRACTTABLEMODEL_HPP

#include "wkf_export.h"

#include <set>

#include <QAbstractTableModel>

// this adds wkf unit converters to the abstract table

namespace wkf
{
class WKF_EXPORT AbstractTableModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   AbstractTableModel(QObject* aParent)
      : QAbstractTableModel(aParent)
      , mUnitCallbackSet(false)
   {
   }

   // -1 in the unit type indicates un-unit-ed data, otherwise a UtUnit::UnitType
   // 0 in plot type indicates not able to be plotted
   // 1 in plot type indicates line plot
   // 2 in plot type indicates pie chart
   void     DefineColumn(const QString& aBaseTitle, int aUnitType = -1, int aPlotType = 1);
   QString  GetColumnTitle(int aColumnId) const;
   double   Convert(int aColumnId, double aValue) const;
   void     UnitChanged();
   QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole) const override;
   int      columnCount(const QModelIndex& aParent) const override { return (int)mColumnInformation.size(); }

signals:
   void TableUnitsChanged();

private:
   std::vector<std::pair<QString, int>> mColumnInformation;
   std::vector<int>                     mPlotType;
   bool                                 mUnitCallbackSet;
};
} // namespace wkf
#endif
