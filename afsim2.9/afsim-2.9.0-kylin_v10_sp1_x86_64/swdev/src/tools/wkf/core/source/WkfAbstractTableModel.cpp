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

#include "WkfAbstractTableModel.hpp"

#include "UtUnits.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

void wkf::AbstractTableModel::DefineColumn(const QString& aBaseTitle, int aUnitType, int aPlotType)
{
   mColumnInformation.emplace_back(aBaseTitle, aUnitType);
   mPlotType.push_back(aPlotType);
   if (!mUnitCallbackSet)
   {
      connect(wkfEnv.GetPreferenceObject<UnitsObject>(), &UnitsObject::UnitsChanged, this, &AbstractTableModel::UnitChanged);
      mUnitCallbackSet = true;
   }
}

QString wkf::AbstractTableModel::GetColumnTitle(int aColumn) const
{
   if ((int)mColumnInformation.size() > aColumn)
   {
      QString title = mColumnInformation[aColumn].first;
      if (mColumnInformation[aColumn].second >= 0)
      {
         int unit = wkfEnv.GetPreferenceObject<UnitsObject>()->GetUnit((ValueType)(mColumnInformation[aColumn].second));
         if (unit != 0)
         {
            title += QString(" (") +
                     QString::fromStdString(
                        UtUnits::GetUnitType((UtUnits::UnitType)mColumnInformation[aColumn].second)->FindUnitName(unit)) +
                     ")";
         }
      }
      return title;
   }
   else
   {
      return "???";
   }
}

double wkf::AbstractTableModel::Convert(int aColumnId, double aValue) const
{
   if ((int)mColumnInformation.size() > aColumnId)
   {
      if (mColumnInformation[aColumnId].second >= 0)
      {
         int unit = wkfEnv.GetPreferenceObject<UnitsObject>()->GetUnit((ValueType)(mColumnInformation[aColumnId].second));
         return UtUnits::GetUnitType((UtUnits::UnitType)mColumnInformation[aColumnId].second)->ConvertFromStandard(aValue, unit);
      }
   }
   return aValue;
}

void wkf::AbstractTableModel::UnitChanged()
{
   // note that we should probably be targeting these changes better than this
   beginResetModel();
   endResetModel();
   emit TableUnitsChanged();
}

QVariant wkf::AbstractTableModel::headerData(int aSection, Qt::Orientation aOrientation, int aRole) const
{
   if (aRole == Qt::DisplayRole)
   {
      if (aOrientation == Qt::Horizontal)
      {
         return GetColumnTitle(aSection);
      }
      else
      {
         if (aSection < rowCount())
         {
            return aSection + 1;
         }
      }
   }
   if (aRole == Qt::UserRole)
   {
      if (aOrientation == Qt::Horizontal)
      {
         return mPlotType[aSection];
      }
   }
   return QVariant();
}
