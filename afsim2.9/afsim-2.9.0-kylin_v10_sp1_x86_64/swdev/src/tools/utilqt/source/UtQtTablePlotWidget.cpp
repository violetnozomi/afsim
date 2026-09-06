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

#include "UtQtTablePlotWidget.hpp"

#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QListWidget>
#include <QMenu>
#include <QTextStream>

UtQtTablePlotWidget::UtQtTablePlotWidget(QWidget* aParent)
   : QWidget(aParent)
   , mTime(0.0f)
   , mContextSuppressed(false)
   , mActivePlotType(1)
{
   mUi.setupUi(this);

   mUi.mTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mUi.mTable->horizontalHeader(), &QWidget::customContextMenuRequested, this, &UtQtTablePlotWidget::HeaderContextMenu);
   mUi.mTable->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mUi.mTable, &QWidget::customContextMenuRequested, this, &UtQtTablePlotWidget::TableContextMenu);
   connect(mUi.mTable, &QTableView::clicked, this, &UtQtTablePlotWidget::itemClickedSlot);

   mUi.mPlotter->SetShowCrosshairs(true);
   mUi.mPlotter->SetCrosshairsShowAllSeries(true);
}

void UtQtTablePlotWidget::SetModel(QAbstractItemModel* aModelPtr)
{
   mModelPtr = aModelPtr;
   mUi.mTable->setModel(aModelPtr);
   mUi.mPlotter->SetShowLegend(true);

   for (int i = 0; i < aModelPtr->rowCount(); ++i)
   {
      if (mColumnDisplaySet.find(i) != mColumnDisplaySet.end())
      {
         mUi.mTable->setColumnHidden(i, false);
      }
      else
      {
         mUi.mTable->setColumnHidden(i, true);
      }
   }
}

void UtQtTablePlotWidget::SetTime(float aTime)
{
   if (aTime != mTime)
   {
      mTime = aTime;
      mUi.mPlotter->SetCrosshairsValueX(aTime);
      mUi.mPlotter->UpdatePlot(); // I am not sure why update isn't working here
   }
}

void UtQtTablePlotWidget::HeaderContextMenu(const QPoint& aPoint)
{
   int idx = mUi.mTable->horizontalHeader()->logicalIndexAt(aPoint);
   ColumnContextMenu(idx, mUi.mTable->horizontalHeader()->mapToGlobal(aPoint));
}

void UtQtTablePlotWidget::TableContextMenu(const QPoint& aPoint)
{
   if (mContextSuppressed)
   {
      emit tableContext(mUi.mTable->indexAt(aPoint), mUi.mTable->mapToGlobal(aPoint));
   }
   else
   {
      int idx = mUi.mTable->indexAt(aPoint).column();
      ColumnContextMenu(idx, mUi.mTable->mapToGlobal(aPoint));
   }
}

void UtQtTablePlotWidget::ColumnContextMenu(int aIdx, const QPoint& aPoint)
{
   QMenu* menu = new QMenu(this);
   AddActions(aIdx, menu);
   menu->exec(aPoint);
   delete menu;
}

void UtQtTablePlotWidget::ExportTable(bool aAll)
{
   QString filename =
      QDir::home().absolutePath() + "/" + "export_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".csv";
   filename.replace(" ", "_");
   QString fname =
      QFileDialog::getSaveFileName(this, "Export to CSV", filename, QString("CSV (*.csv);; Text (*.txt);; All Files(*.*)"));
   if (!fname.isEmpty())
   {
      QFile file(fname);
      if (file.open(QIODevice::WriteOnly))
      {
         QTextStream stream(&file);
         stream << "time";
         for (int c = 0; c < mModelPtr->columnCount(); ++c)
         {
            if (!mUi.mTable->isColumnHidden(c) || aAll)
            {
               stream << ", ";
               stream << mModelPtr->headerData(c, Qt::Horizontal).toString();
            }
         }
         stream << "\n";
         for (int r = 0; r < mModelPtr->rowCount(); ++r)
         {
            stream << mModelPtr->headerData(r, Qt::Vertical).toString();
            for (int c = 0; c < mModelPtr->columnCount(); ++c)
            {
               if (!mUi.mTable->isColumnHidden(c) || aAll)
               {
                  stream << ", ";
                  stream << mModelPtr->data(mModelPtr->index(r, c), Qt::DisplayRole).toString();
               }
            }
            stream << "\n";
         }
      }
      file.close();
   }
}

void UtQtTablePlotWidget::ShowColumn(int aColumn, bool aState)
{
   if (aState)
   {
      mColumnDisplaySet.insert(aColumn);
   }
   else
   {
      mColumnDisplaySet.erase(aColumn);
   }
   mUi.mTable->setColumnHidden(aColumn, !aState);
}

void UtQtTablePlotWidget::AddPlot(int aColumn)
{
   int plotType = mModelPtr->headerData(aColumn, Qt::Horizontal, Qt::UserRole).toInt();
   if (plotType != mActivePlotType)
   {
      mUi.mPlotter->SetMode(plotType);
      mActivePlotType = plotType;
      mUi.mPlotter->ClearModelSeries();
   }
   mUi.mPlotter->AddModelSeries(mModelPtr, -1, aColumn);
}

void UtQtTablePlotWidget::MakePretty()
{
   mUi.mPlotter->MakePlotPretty();
}

void UtQtTablePlotWidget::itemClickedSlot(const QModelIndex& aIdx)
{
   emit itemClicked(aIdx);
}

void UtQtTablePlotWidget::AddActions(int aIndex, QMenu* aMenu)
{
   QAction* chooseAct = new QAction("Choose columns", aMenu);
   connect(chooseAct, &QAction::triggered, this, &UtQtTablePlotWidget::ChooseColumnsSlot);
   aMenu->addAction(chooseAct);
   if (mColumnDisplaySet.find(aIndex) != mColumnDisplaySet.end())
   {
      int     plotType = mModelPtr->headerData(aIndex, Qt::Horizontal, Qt::UserRole).toInt();
      QString name     = mModelPtr->headerData(aIndex, Qt::Horizontal, Qt::DisplayRole).toString();
      if (plotType > 0)
      {
         QString  actionStr = "Plot: " + name;
         QAction* plotAct   = new QAction(actionStr, aMenu);
         plotAct->setData(aIndex);
         connect(plotAct, &QAction::triggered, this, &UtQtTablePlotWidget::PlotSlot);
         aMenu->addAction(plotAct);
         // only if a line plot, and the associated data is good for a line plot
         if ((mActivePlotType == 1) && (plotType == 1))
         {
            QString  actionStr2 = "Add to plot: " + name;
            QAction* addPlotAct = new QAction(actionStr2, aMenu);
            addPlotAct->setData(aIndex);
            connect(addPlotAct, &QAction::triggered, this, &UtQtTablePlotWidget::AddPlotSlot);
            aMenu->addAction(addPlotAct);
         }
      }
      QAction* hideAct = new QAction("Hide column", aMenu);
      hideAct->setData(aIndex);
      connect(hideAct, &QAction::triggered, this, &UtQtTablePlotWidget::HideColumnSlot);
      aMenu->addAction(hideAct);
      QAction* exportAct = new QAction("Export visible columns", aMenu);
      connect(exportAct, &QAction::triggered, this, &UtQtTablePlotWidget::ExportVisibleSlot);
      aMenu->addAction(exportAct);
      QAction* exportAllAct = new QAction("Export all columns", aMenu);
      connect(exportAllAct, &QAction::triggered, this, &UtQtTablePlotWidget::ExportAllSlot);
      aMenu->addAction(exportAllAct);
   }
}

void UtQtTablePlotWidget::ChooseColumnsSlot()
{
   QDialog chooser(this);
   chooser.setLayout(new QVBoxLayout(&chooser));
   QListWidget* lw = new QListWidget(&chooser);
   chooser.layout()->addWidget(lw);
   QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &chooser);
   connect(bb, &QDialogButtonBox::accepted, &chooser, &QDialog::accept);
   connect(bb, &QDialogButtonBox::rejected, &chooser, &QDialog::reject);
   for (int i = 0; i < mModelPtr->columnCount(); ++i)
   {
      QString          itemName = mModelPtr->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
      QListWidgetItem* item     = new QListWidgetItem(itemName, lw);
      if (mColumnDisplaySet.find(i) != mColumnDisplaySet.end())
      {
         item->setCheckState(Qt::Checked);
      }
      else
      {
         item->setCheckState(Qt::Unchecked);
      }
   }
   chooser.layout()->addWidget(bb);
   if (chooser.exec())
   {
      mColumnDisplaySet.clear();
      for (int i = 0; i < lw->count(); ++i)
      {
         QListWidgetItem* item = lw->item(i);
         if (item->checkState() == Qt::Checked)
         {
            mColumnDisplaySet.insert(i);
            mUi.mTable->setColumnHidden(i, false);
         }
         else
         {
            mUi.mTable->setColumnHidden(i, true);
         }
      }
   }
}

void UtQtTablePlotWidget::PlotSlot()
{
   int aIdx     = ((QAction*)sender())->data().toInt();
   int plotType = mModelPtr->headerData(aIdx, Qt::Horizontal, Qt::UserRole).toInt();
   if (plotType != mActivePlotType)
   {
      mUi.mPlotter->SetMode(plotType);
      mActivePlotType = plotType;
   }
   mUi.mPlotter->ClearModelSeries();
   mUi.mPlotter->AddModelSeries(mModelPtr, -1, aIdx);
   mUi.mPlotter->MakePlotPretty();
}

void UtQtTablePlotWidget::AddPlotSlot()
{
   int aIdx = ((QAction*)sender())->data().toInt();
   mUi.mPlotter->AddModelSeries(mModelPtr, -1, aIdx);
   mUi.mPlotter->MakePlotPretty();
}

void UtQtTablePlotWidget::HideColumnSlot()
{
   int aIdx = ((QAction*)sender())->data().toInt();
   mColumnDisplaySet.erase(aIdx);
   mUi.mTable->setColumnHidden(aIdx, true);
}

void UtQtTablePlotWidget::ExportVisibleSlot()
{
   ExportTable(false);
}

void UtQtTablePlotWidget::ExportAllSlot()
{
   ExportTable(true);
}

void UtQtTablePlotWidget::EnableCustomPlotContextMenu()
{
   mUi.mPlotter->EnableCustomPlotContextMenu();
   connect(mUi.mPlotter,
           &UtQtPlotChooser::CustomContextMenu,
           [this](const QPoint& aPos, const QPointF& aData, bool aPlot2D)
           { emit CustomPlotContext(aPos, aData, aPlot2D); });
}
