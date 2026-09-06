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

#ifndef WKF_FILTER_WIDGET_HPP
#define WKF_FILTER_WIDGET_HPP

#include "wkf_common_export.h"

#include <map>
#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QDialog>
#include <QString>
#include <QTableWidget>
#include <QThread>
#include <QTreeWidget>

#include "WkfFilterModel.hpp"
#include "WkfFilterPlot.hpp"
#include "WkfFilterView.hpp"
#include "ui_WkfFilterWidget.h"

class QSettings;

namespace WkfFilter
{
class FilterView;

//! FilterWidget is both the public API for the FilterWidget "library" and the user interface.
class WKF_COMMON_EXPORT FilterWidget : public QWidget
{
public:
   //! Note: The FilterWidget does NOT take ownership of the source model.
   //!       Do not use a model whose lifetime is less than the lifetime of the FilterWidget.
   FilterWidget(QWidget* aParent, QString aExportGroup, QAbstractItemModel* aSourceModel = nullptr);

   FilterWidget(const FilterWidget&) = delete;

   ~FilterWidget() override = default;

   void SetSourceModel(QAbstractItemModel* aSourceModel);

   //! Given row aRow in the filtered model, returns which row it is in the source model.
   //! Returns -1 on failure.
   int MapToSourceRow(int aRow) const;

   //! Given row aRow in the source model, returns which row it appears in in the filtered model.
   //! Returns -1 on failure, or if it does not appear in the filtered model.
   int MapFromSourceRow(int aRow) const;

   //! This is a stop-gap.
   //! When the FilterWidget was developed, the use case was for a non-changing source model.
   //! For now, call RunFilter() or RowsAppended() whenever the source model is modified.
   //! At some point in the future, this will be automated.
   //@{
   void RunFilter();
   void RowsAppended(int aCount);
   //@}

   template<typename T, typename... Args>
   void RegisterRule(Args&&... aArgs)
   {
      mUi.filterView->RegisterRule<T>(std::forward<Args>(aArgs)...);
   }

   FilterPlot& GetPlot() noexcept;

   QTableView* GetTableView() noexcept;

private slots:
   void OnFetchFinished();

private:
   void HeaderContextMenu(const QPoint& aPosition);
   void ResultsContextMenu(const QPoint& aPosition);

   //! Used by HeaderContextMenu() to add items to the menu.
   //@{
   void HeaderContextMenuHide(int aIndex, QHeaderView* aHeader, QMenu& aMenu);
   void HeaderContextMenuShow(int aIndex, QHeaderView* aHeader, QMenu& aMenu);
   //@}

   //! Used by ResultsContextMenu() to add items to the menu.
   //@{
   void ContextMenuNewFilter(const QModelIndex& aIndex, QMenu& aMenu);
   void ContextMenuExportCSV(const QModelIndex& aIndex, QMenu& aMenu);
   void ContextMenuPlot(const QModelIndex& aIndex, QMenu& aMenu);
   //@}

   Ui::WkfFilterWidget mUi;

   //! The model being displayed.
   FilterModel mFilterModel;

   //! A plotting window for visualizing results.
   FilterPlot mFilterPlot;

   //! Indicates whether the results view should automatically scroll to the bottom after rows are appended.
   //! If the filter is modified or RunFilter() is called, reset to false.
   bool mAtBottom = false;

   //! The pref object should be before the export helper.
   FilterPrefObject  mPrefObject;
   wkf::ExportHelper mExportHelper;

   //! Exports the current (filtered) model to a CSV file.
   //! Prompts user for file location.
   void ExportToCSV();
};
} // namespace WkfFilter

#endif
