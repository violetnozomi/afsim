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

#ifndef WKF_FILTER_MODEL_HPP
#define WKF_FILTER_MODEL_HPP

#include "wkf_common_export.h"

#include <QSortFilterProxyModel>

#include "WkfFilterObject.hpp"
#include "WkfFilterThread.hpp"

namespace WkfFilter
{
//! A model that represents the filtered data.
//! Does NOT own the data it represents.
class WKF_COMMON_EXPORT FilterModel : public QAbstractProxyModel
{
   Q_OBJECT

public:
   friend class FilterThread;

   explicit FilterModel(QAbstractItemModel* aSourceModel);

   FilterModel(const FilterModel&) = delete;
   FilterModel& operator=(const FilterModel&) = delete;

   ~FilterModel() = default;

   //! Runs filter on source model.
   //! If aFilter is nullptr, all rows pass.
   void RunFilter(FilterPtr aFilter);

   void ContinueFilter();

   void SetModel(QAbstractItemModel* aSourceModel, FilterPtr aFilter = nullptr);

   //! Blocks until the thread responsible for filtering finishes.
   void WaitForFinish();

   //! Exports the current data to a CSV file at the given file path.
   bool ExportToCSV(const QString& aFilePath);

   //! Given row aRow in the FilterModel, returns which row it is in the source model.
   //! Returns -1 on failure.
   int MapToSourceRow(int aRow) const;

   //! Given row aRow in the source model, returns which row it appears in the FilterModel.
   //! Returns -1 on failure, or if it does not appear in the FilterModel.
   int MapFromSourceRow(int aRow) const;

   //! Required overloads of the QAbstractProxyModel class
   //! @{
   QModelIndex index(int aRow, int aColumn, const QModelIndex& aParent = QModelIndex()) const override;

   QModelIndex parent(const QModelIndex& aParent) const override;

   int rowCount(const QModelIndex& aParent = QModelIndex()) const override;

   int columnCount(const QModelIndex& aParent = QModelIndex()) const override;

   QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole = Qt::DisplayRole) const override;

   QModelIndex mapFromSource(const QModelIndex& aSourceIndex) const override;

   QModelIndex mapToSource(const QModelIndex& aProxyIndex) const override;
   //! @}

signals:
   //! Emitted when the filtering thread finishes.
   void Finished();

   //! Emitted when new data has been added to the model.
   void Updated();

private:
   //! Called by the main thread to load the cache into the database.
   void LoadCache() noexcept;

   void OnThreadFinished();

   //! Called when the source model emits `modelAboutToBeReset()`.
   void OnAboutToReset();
   //! Called when the source model emits `modelReset()`.
   void OnReset();

   //! Gets the title of a column based on the tool tips in the model.
   //! Uses mColumnTitles as a cache to increase performance.
   QString GetColumnFromContents(int aColumn) const;

   //! The instance of the thread responsible for performing the filtering.
   FilterThread mThread;

   //! A list of all rows that have been cleared by the filtering thread.
   std::vector<int> mPassedRows;

   //! \see GetColumnFromContents
   mutable std::map<int, QString> mColumnTitles;
};
} // namespace WkfFilter

#endif
