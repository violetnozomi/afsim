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

#include "UtQtFixedColumnTableView.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QTimer>

ut::qt::FixedColumnTableView::FixedColumnTableView(QWidget* aParent)
   : QWidget(aParent)
{
   mLayout    = new QHBoxLayout(this);
   mFixedView = new TableView(this);
   mDataView  = new TableView(this);

   mLayout->setSpacing(0);
   mLayout->setMargin(0);

   mFixedView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   mFixedView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   mFixedView->horizontalHeader()->setFixedHeight(24);
   mFixedView->setEditTriggers(EditTrigger::NoEditTriggers);

   mDataView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   mDataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   mDataView->horizontalHeader()->setFixedHeight(24);
   mDataView->verticalHeader()->setVisible(false);

   mLayout->addWidget(mFixedView);
   mLayout->addWidget(mDataView);

   setLayout(mLayout);

   connect(mFixedView, &QTableView::clicked, this, &FixedColumnTableView::Clicked);
   connect(mDataView, &QTableView::clicked, this, &FixedColumnTableView::Clicked);
   connect(mFixedView, &QTableView::doubleClicked, this, &FixedColumnTableView::DoubleClicked);
   connect(mDataView, &QTableView::doubleClicked, this, &FixedColumnTableView::DoubleClicked);

   connect(mFixedView->verticalScrollBar(),
           &QAbstractSlider::valueChanged,
           mDataView->verticalScrollBar(),
           &QAbstractSlider::setValue);
   connect(mDataView->verticalScrollBar(),
           &QAbstractSlider::valueChanged,
           mFixedView->verticalScrollBar(),
           &QAbstractSlider::setValue);

   connect(mFixedView->horizontalHeader(),
           &QHeaderView::sortIndicatorChanged,
           [this](int aColumn, Qt::SortOrder aOrder)
           {
              QSignalBlocker b{mDataView->horizontalHeader()};
              mDataView->horizontalHeader()->setSortIndicator(aColumn, aOrder);
           });
   connect(mDataView->horizontalHeader(),
           &QHeaderView::sortIndicatorChanged,
           [this](int aColumn, Qt::SortOrder aOrder)
           {
              QSignalBlocker b{mFixedView->horizontalHeader()};
              mFixedView->horizontalHeader()->setSortIndicator(aColumn, aOrder);
           });

   connect(mFixedView->horizontalHeader(), &QHeaderView::sectionResized, this, &FixedColumnTableView::UpdateSectionWidth);
   connect(mFixedView->verticalHeader(), &QHeaderView::sectionResized, this, &FixedColumnTableView::UpdateSectionHeight);
   connect(mDataView->horizontalHeader(), &QHeaderView::sectionResized, this, &FixedColumnTableView::UpdateSectionWidth);
}

void ut::qt::FixedColumnTableView::SetModel(QAbstractTableModel* aModel)
{
   disconnect(Model());

   mFixedView->setModel(aModel);
   mDataView->setModel(aModel);

   if (aModel)
   {
      // mUi.dataView->setSelectionModel(new QItemSelectionModel);
      mFixedView->setSelectionModel(mDataView->selectionModel());

      // This is queued because it should be called after both models add columns.
      // With direct connect (the default), it's called too soon.
      connect(aModel, &QAbstractItemModel::columnsInserted, this, &FixedColumnTableView::ColumnsInserted, Qt::QueuedConnection);
      connect(aModel, &QAbstractItemModel::columnsRemoved, this, &FixedColumnTableView::UpdateGeometry, Qt::QueuedConnection);
   }

   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::SetFixedColumnCount(int aCount)
{
   mFixedColumnCount = aCount;

   if (Model())
   {
      for (int i = 0; i < Model()->columnCount(); i++)
      {
         mFixedView->setColumnHidden(i, i >= mFixedColumnCount);
         mDataView->setColumnHidden(i, i < mFixedColumnCount);
      }
   }
   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::SetRowHeight(int aHeight)
{
   auto* fvh = mFixedView->verticalHeader();
   auto* dvh = mDataView->verticalHeader();
   if (fvh->maximumSectionSize() < aHeight)
   {
      fvh->setMaximumSectionSize(aHeight);
      fvh->setMinimumSectionSize(aHeight);

      dvh->setMaximumSectionSize(aHeight);
      dvh->setMinimumSectionSize(aHeight);
   }
   else
   {
      fvh->setMinimumSectionSize(aHeight);
      fvh->setMaximumSectionSize(aHeight);

      dvh->setMinimumSectionSize(aHeight);
      dvh->setMaximumSectionSize(aHeight);
   }

   fvh->setDefaultSectionSize(aHeight);
   dvh->setDefaultSectionSize(aHeight);

   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::SetVerticalHeaderVisible(bool aVisible)
{
   mFixedView->verticalHeader()->setVisible(aVisible);
   UpdateGeometry();
}

QAbstractTableModel* ut::qt::FixedColumnTableView::Model() const
{
   return static_cast<QAbstractTableModel*>(mFixedView->model());
}

QItemSelectionModel* ut::qt::FixedColumnTableView::SelectionModel() const
{
   return mFixedView->selectionModel();
}

void ut::qt::FixedColumnTableView::SetFixedColumnsSortable(bool aSortable)
{
   mFixedView->setSortingEnabled(aSortable);
}

void ut::qt::FixedColumnTableView::SetDataColumnsMovable(bool aMovable)
{
   mDataView->horizontalHeader()->setSectionsMovable(aMovable);
}

void ut::qt::FixedColumnTableView::SetDataColumnsSortable(bool aSortable)
{
   mDataView->setSortingEnabled(aSortable);
}

void ut::qt::FixedColumnTableView::SetDataEditTriggers(EditTriggers aTriggers)
{
   mDataView->setEditTriggers(aTriggers);
}

void ut::qt::FixedColumnTableView::SetStretchLastColumn(bool aExpand)
{
   mDataView->horizontalHeader()->setStretchLastSection(aExpand);
}

void ut::qt::FixedColumnTableView::SetColumnVisible(int aColumn, bool aVisible)
{
   if (aColumn < 0)
   {
      return;
   }
   else if (aColumn < mFixedColumnCount)
   {
      mFixedView->setColumnHidden(aColumn, !aVisible);
   }
   else
   {
      mDataView->setColumnHidden(aColumn, !aVisible);
   }
   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::ShowColumn(int aColumn)
{
   SetColumnVisible(aColumn, true);
}

void ut::qt::FixedColumnTableView::HideColumn(int aColumn)
{
   SetColumnVisible(aColumn, false);
}

int ut::qt::FixedColumnTableView::RowAt(int aY) const
{
   return mFixedView->rowAt(aY);
}

int ut::qt::FixedColumnTableView::ColumnAt(int aX) const
{
   const int fixedWidth = mFixedView->width();
   if (aX < fixedWidth)
   {
      return mFixedView->columnAt(aX);
   }
   return mDataView->columnAt(aX - fixedWidth);
}

void ut::qt::FixedColumnTableView::resizeEvent(QResizeEvent* aEvent)
{
   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::showEvent(QShowEvent* aEvent)
{
   QWidget::showEvent(aEvent);
   QTimer::singleShot(0, this, &FixedColumnTableView::UpdateGeometry);
}

void ut::qt::FixedColumnTableView::UpdateGeometry()
{
   const int fixedWidth = mFixedView->columnViewportPosition(mFixedColumnCount);
   const int frameWidth = mFixedView->frameWidth();

   const int contentsWidth  = mDataView->horizontalHeader()->length();
   const int contentsHeight = mDataView->verticalHeader()->length();

   const int  dataWidth     = width() - fixedWidth - frameWidth;
   const bool showScrollBar = contentsWidth > dataWidth;

   mDataView->setVerticalScrollBarPolicy((contentsHeight > height()) ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
   mDataView->setHorizontalScrollBarPolicy(showScrollBar ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
   mDataView->setGeometry(fixedWidth + 2 * frameWidth, 0, dataWidth, height());

   const auto* scrollBar      = mDataView->horizontalScrollBar();
   const int   scrollBarWidth = scrollBar->height() * static_cast<int>(showScrollBar);

   mFixedView->setGeometry(-frameWidth, 0, fixedWidth + 4 * frameWidth, height() - scrollBarWidth);
}

void ut::qt::FixedColumnTableView::ColumnsInserted(const QModelIndex& aParent, int aFirst, int aLast)
{
   for (int i = aFirst; i <= aLast; i++)
   {
      mFixedView->setColumnHidden(i, i >= mFixedColumnCount);
      mDataView->setColumnHidden(i, i < mFixedColumnCount);
   }
   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::UpdateSectionWidth(int aIndex, int aOldSize, int aNewSize)
{
   UpdateGeometry();
}

void ut::qt::FixedColumnTableView::UpdateSectionHeight(int aIndex, int aOldSize, int aNewSize)
{
   mFixedView->setRowHeight(aIndex, aNewSize);
   mDataView->setRowHeight(aIndex, aNewSize);
}
