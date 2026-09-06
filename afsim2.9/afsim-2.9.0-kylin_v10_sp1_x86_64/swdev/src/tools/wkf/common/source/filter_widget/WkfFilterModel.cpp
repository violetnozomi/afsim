// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfFilterModel.hpp"

#include <algorithm>
#include <array>
#include <fstream>

#include <QCoreApplication>
#include <QMessageBox>
#include <QProgressDialog>

namespace WkfFilter
{
constexpr size_t             cCSV_PAGE_SIZE = 100;
constexpr Qt::ConnectionType cQUEUED_UNIQUE = static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection);

FilterModel::FilterModel(QAbstractItemModel* aSourceModel)
{
   setSourceModel(aSourceModel);

   connect(aSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &FilterModel::OnAboutToReset);
   connect(aSourceModel, &QAbstractItemModel::modelReset, this, &FilterModel::OnReset);

   connect(&mThread, &FilterThread::finished, this, &FilterModel::OnThreadFinished, Qt::QueuedConnection);

   connect(&mThread, &FilterThread::NewDataReady, this, &FilterModel::LoadCache, cQUEUED_UNIQUE);
}

void FilterModel::RunFilter(FilterPtr aFilter)
{
   mPassedRows.clear();
   mThread.RunFilter(std::move(aFilter), sourceModel());
}

void FilterModel::ContinueFilter()
{
   mThread.ContinueFilter();
}

void FilterModel::SetModel(QAbstractItemModel* aSourceModel, FilterPtr aFilter)
{
   mThread.Stop();
   setSourceModel(aSourceModel);
   RunFilter(std::move(aFilter));
}

void FilterModel::WaitForFinish()
{
   mThread.WaitForFinish();
}

bool FilterModel::ExportToCSV(const QString& aFilePath)
{
   std::ofstream file(aFilePath.toStdString());

   if (!file.is_open())
   {
      return false;
   }

   WaitForFinish();

   QProgressDialog progress("Exporting to CSV", "Cancel", 0, rowCount());
   progress.setModal(true);

   // Write out headings
   file << headerData(0, Qt::Horizontal).toString().toStdString();
   for (int col = 1; col < columnCount(); col++)
   {
      file << ',' << headerData(col, Qt::Horizontal).toString().toStdString();
   }

   for (int row = 0; row < rowCount(); row++)
   {
      if (row % cCSV_PAGE_SIZE == 0)
      {
         progress.setValue(row);
      }
      if (progress.wasCanceled())
      {
         return true;
      }

      file << '\n' << index(row, 0).data().toString().toStdString();
      for (int col = 1; col < columnCount(); col++)
      {
         file << ',' << index(row, col).data().toString().toStdString();
      }
   }

   return true;
}

QModelIndex FilterModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
   if (aParent.isValid() || sourceModel() == nullptr || aRow < 0 || aRow >= rowCount() || aColumn < 0 ||
       aColumn >= columnCount())
   {
      return QModelIndex();
   }

   return createIndex(aRow, aColumn);
}

int FilterModel::MapToSourceRow(int aRow) const
{
   if (sourceModel() == nullptr || aRow < 0 || aRow >= rowCount())
   {
      return -1;
   }
   return mPassedRows[aRow];
}

int FilterModel::MapFromSourceRow(int aRow) const
{
   if (sourceModel() == nullptr || aRow < 0 || aRow >= sourceModel()->rowCount())
   {
      return -1;
   }

   auto it = std::lower_bound(mPassedRows.begin(), mPassedRows.end(), aRow);

   if (it == mPassedRows.end() || *it != aRow)
   {
      return -1;
   }
   return static_cast<int>(it - mPassedRows.begin());
}

QModelIndex FilterModel::parent(const QModelIndex& aParent) const
{
   return QModelIndex();
}

int FilterModel::rowCount(const QModelIndex& aParent) const
{
   if (aParent.isValid())
   {
      return 0;
   }

   return static_cast<int>(mPassedRows.size());
}

int FilterModel::columnCount(const QModelIndex& aParent) const
{
   if (aParent.isValid() || sourceModel() == nullptr)
   {
      return 0;
   }

   return sourceModel()->columnCount();
}

QVariant FilterModel::headerData(int aSection, Qt::Orientation aOrientation, int aRole) const
{
   if (sourceModel() == nullptr)
   {
      return QVariant();
   }

   if (aOrientation == Qt::Horizontal)
   {
      if (aRole == Qt::ToolTipRole)
      {
         return aSection + 1;
      }
      else if (aRole == Qt::DisplayRole)
      {
         QVariant sourceHeader = sourceModel()->headerData(aSection, Qt::Horizontal, Qt::DisplayRole);

         if (!sourceHeader.isNull())
         {
            return sourceHeader;
         }
         return GetColumnFromContents(aSection);
      }
      return sourceModel()->headerData(aSection, Qt::Horizontal, aRole);
   }
   else
   {
      if (aRole == Qt::ToolTipRole || aRole == Qt::DisplayRole)
      {
         return aSection + 1;
      }
      else if (aSection >= 0 && aSection < rowCount())
      {
         return sourceModel()->headerData(mPassedRows[aSection], Qt::Vertical, aRole);
      }
      return QVariant();
   }
}

QModelIndex FilterModel::mapFromSource(const QModelIndex& aSourceIndex) const
{
   if (!aSourceIndex.isValid() || sourceModel() == nullptr)
   {
      return QModelIndex();
   }
   return index(MapFromSourceRow(aSourceIndex.row()), aSourceIndex.column());
}

QModelIndex FilterModel::mapToSource(const QModelIndex& aProxyIndex) const
{
   if (!aProxyIndex.isValid() || sourceModel() == nullptr || aProxyIndex.row() >= rowCount() ||
       aProxyIndex.column() >= columnCount())
   {
      return QModelIndex();
   }
   return sourceModel()->index(MapToSourceRow(aProxyIndex.row()), aProxyIndex.column());
}

void FilterModel::LoadCache() noexcept
{
   auto newLines = mThread.TakeCache();

   if (!newLines.empty())
   {
      beginInsertRows(QModelIndex(), rowCount(), rowCount() + static_cast<int>(newLines.size()) - 1);
      mPassedRows.insert(mPassedRows.end(), newLines.begin(), newLines.end());
      endInsertRows();
      emit Updated();
   }
}

void FilterModel::OnThreadFinished()
{
   mColumnTitles.clear();
   emit beginResetModel();
   emit endResetModel();
   emit Finished();
}

void FilterModel::OnAboutToReset()
{
   emit beginResetModel();
}

void FilterModel::OnReset()
{
   mPassedRows.clear();
   mColumnTitles.clear();
   mThread.RestartFilter();
   emit endResetModel();
}

QString FilterModel::GetColumnFromContents(int aColumn) const
{
   auto it = mColumnTitles.find(aColumn);
   if (it != mColumnTitles.end())
   {
      return it->second;
   }

   QString title;
   bool    first = true;

   for (int i = 0; i < rowCount(); i++)
   {
      const QVariant toolTip = index(i, aColumn).data(Qt::ToolTipRole);

      if (toolTip.isNull())
      {
         mColumnTitles[aColumn] = QString::number(aColumn + 1);
         return QString::number(aColumn + 1);
      }
      else if (first)
      {
         title = toolTip.toString();
         first = false;
      }
      else if (toolTip.toString() != title)
      {
         mColumnTitles[aColumn] = QString::number(aColumn + 1);
         return QString::number(aColumn + 1);
      }
   }

   mColumnTitles[aColumn] = title;
   return title;
}
} // namespace WkfFilter
