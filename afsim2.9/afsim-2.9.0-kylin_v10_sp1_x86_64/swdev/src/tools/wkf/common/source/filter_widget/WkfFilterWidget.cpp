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

#include "WkfFilterWidget.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QToolButton>

#include "UtMemory.hpp"
#include "UtQtGL2DPlotWidget.hpp"
#include "WkfEnvironment.hpp"
#include "WkfFilterItem.hpp"
#include "WkfFilterPlot.hpp"
#include "WkfFilterRules.hpp"
#include "WkfFilterView.hpp"
#include "WkfPlatformGrouping.hpp"

namespace WkfFilter
{
static constexpr int cMIN_ROWS_FOR_PROGRESS_BAR = 20;

FilterWidget::FilterWidget(QWidget* aParent, QString aExportGroup, QAbstractItemModel* aSourceModel)
   : QWidget(aParent)
   , mFilterModel(aSourceModel)
   , mFilterPlot(this)
   , mPrefObject(nullptr, aExportGroup)
   , mExportHelper(mPrefObject, "Filter")
{
   mUi.setupUi(this);
   mUi.filterView->SetModel(&mFilterModel);
   mUi.resultsView->setModel(&mFilterModel);

   mUi.filterView->SetupExports(&mExportHelper, &mPrefObject);

   RegisterRule<WkfFilter::RuleEquals>();
   RegisterRule<WkfFilter::RuleNotEquals>();
   RegisterRule<WkfFilter::RuleLessThan>();
   RegisterRule<WkfFilter::RuleLessOrEqual>();
   RegisterRule<WkfFilter::RuleGreaterThan>();
   RegisterRule<WkfFilter::RuleGreaterOrEqual>();
   RegisterRule<WkfFilter::RuleContains>();

   // Connect signals
   connect(mUi.filterView, &FilterView::FilterChanged, this, &FilterWidget::RunFilter);

   connect(&mFilterModel,
           &FilterModel::Finished,
           [this]()
           {
              mUi.filterView->UpdateAllItems();
              mUi.progressBar->hide();

              if (mAtBottom)
              {
                 mUi.resultsView->scrollToBottom();
              }
           });

   connect(&mFilterModel, &FilterModel::Updated, [this]() { mUi.resultsView->update(QModelIndex()); });

   mUi.resultsView->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mUi.resultsView, &QTableView::customContextMenuRequested, this, &FilterWidget::ResultsContextMenu);

   auto* header = mUi.resultsView->horizontalHeader();
   header->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(header, &QWidget::customContextMenuRequested, this, &FilterWidget::HeaderContextMenu);

   mFilterModel.RunFilter(nullptr);
}

void FilterWidget::SetSourceModel(QAbstractItemModel* aSourceModel)
{
   mFilterModel.SetModel(aSourceModel, mUi.filterView->GetFilter());
}

int FilterWidget::MapToSourceRow(int aRow) const
{
   return mFilterModel.MapToSourceRow(aRow);
}

int FilterWidget::MapFromSourceRow(int aRow) const
{
   return mFilterModel.MapFromSourceRow(aRow);
}

void FilterWidget::RunFilter()
{
   mUi.progressBar->show();
   mUi.resultsView->scrollToTop();
   mAtBottom = false;
   mFilterModel.RunFilter(mUi.filterView->GetFilter());
}

void FilterWidget::RowsAppended(int aCount)
{
   if (aCount >= cMIN_ROWS_FOR_PROGRESS_BAR)
   {
      mUi.progressBar->show();
   }
   auto* scrollBar = mUi.resultsView->verticalScrollBar();
   mAtBottom       = (scrollBar->value() == scrollBar->maximum());

   mFilterModel.ContinueFilter();
}

FilterPlot& FilterWidget::GetPlot() noexcept
{
   return mFilterPlot;
}

QTableView* FilterWidget::GetTableView() noexcept
{
   return mUi.resultsView;
}

void FilterWidget::ExportToCSV()
{
   QString path = QFileDialog::getSaveFileName(nullptr, "Filter CSV", QString(), "Comma Separated Values (*.csv)");
   if (!path.isEmpty())
   {
      if (!mFilterModel.ExportToCSV(path))
      {
         QMessageBox::warning(nullptr, "Error opening file", "The file you selected could not be opened.");
      }
   }
}

void FilterWidget::OnFetchFinished()
{
   mUi.resultsView->reset();
   mFilterPlot.ReloadModel();
}

void FilterWidget::HeaderContextMenu(const QPoint& aPosition)
{
   auto*     header = mUi.resultsView->horizontalHeader();
   const int index  = header->logicalIndexAt(aPosition);
   if (index >= 0 && index < header->count())
   {
      QMenu menu;

      HeaderContextMenuHide(index, header, menu);
      HeaderContextMenuShow(index, header, menu);

      menu.exec(header->mapToGlobal(aPosition));
   }
}

void FilterWidget::HeaderContextMenuHide(int aIndex, QHeaderView* aHeader, QMenu& aMenu)
{
   if (aHeader->count() > aHeader->hiddenSectionCount() + 1)
   {
      auto* hideAction = aMenu.addAction("Hide " + mFilterModel.headerData(aIndex, Qt::Horizontal).toString());
      connect(hideAction, &QAction::triggered, [aHeader, aIndex]() { aHeader->hideSection(aIndex); });
   }
}

void FilterWidget::HeaderContextMenuShow(int aIndex, QHeaderView* aHeader, QMenu& aMenu)
{
   auto* showAction = aMenu.addAction("Show All Hidden Columns");
   connect(showAction,
           &QAction::triggered,
           [aHeader]()
           {
              const int count = aHeader->count();
              for (int i = 0; i < count; i++)
              {
                 aHeader->showSection(i);
              }
           });
}

void FilterWidget::ResultsContextMenu(const QPoint& aPosition)
{
   QMenu menu;

   QModelIndex index = mUi.resultsView->indexAt(aPosition);

   ContextMenuNewFilter(index, menu);
   ContextMenuExportCSV(index, menu);
   ContextMenuPlot(index, menu);

   menu.exec(mUi.resultsView->mapToGlobal(aPosition));
}

void FilterWidget::ContextMenuNewFilter(const QModelIndex& aIndex, QMenu& aMenu)
{
   QString actionName = "Make filter for [%1] == %2";
   actionName         = actionName.arg(aIndex.data(Qt::ToolTipRole).toString());
   actionName         = actionName.arg(aIndex.data(Qt::DisplayRole).toString());

   QAction* action = aMenu.addAction(actionName);

   connect(action,
           &QAction::triggered,
           [this, aIndex]() { mUi.filterView->AddConditionToRoot(aIndex.column(), aIndex.data().toString()); });
}

void FilterWidget::ContextMenuExportCSV(const QModelIndex& aIndex, QMenu& aMenu)
{
   QAction* action = aMenu.addAction("Export to CSV");

   connect(action,
           &QAction::triggered,
           [this]()
           {
              QString filePath = wkf::getSaveFileName(nullptr, "Export to CSV", QString(), "CSV (*.csv)");

              if (!filePath.isEmpty())
              {
                 mFilterModel.ExportToCSV(filePath);
              }
           });
}

void FilterWidget::ContextMenuPlot(const QModelIndex& aIndex, QMenu& aMenu)
{
   auto actionName = QString("Plot %1").arg(aIndex.data(Qt::ToolTipRole).toString());

   QAction* action = aMenu.addAction(actionName);

   connect(action,
           &QAction::triggered,
           [this, aIndex]()
           {
              GetPlot().SetSourceModel(&mFilterModel);
              GetPlot().SetYAxisColumn(aIndex.column());
              GetPlot().show();
           });
}
} // namespace WkfFilter
