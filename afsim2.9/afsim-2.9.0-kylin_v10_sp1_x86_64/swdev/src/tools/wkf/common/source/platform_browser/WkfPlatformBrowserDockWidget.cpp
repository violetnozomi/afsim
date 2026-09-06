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
#include "WkfPlatformBrowserDockWidget.hpp"

#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QMessageBox>

#include "UtQtTransparentBackgroundDelegate.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::PlatformBrowserDockWidget::PlatformBrowserDockWidget(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
   : QDockWidget(parent, f)
   , mDirtyFlag(false)
   , mShowInactive(false)
{
   mFilterModel.SetBrowser(this);
   mUI.setupUi(this);

   mUI.tableView->setContextMenuPolicy(Qt::CustomContextMenu);
   mUI.tableView->setItemDelegate(new UtQtTransparentBackgroundDelegate(this));
   mUI.tableView->setSortingEnabled(false);
   mUI.tableView->setModel(&mFilterModel);
   mFilterModel.setSourceModel(&mItemModel);
   mFilterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);

   connect(mUI.filterLineEdit, &QLineEdit::textChanged, this, &PlatformBrowserDockWidget::FilterChanged);
   connect(mUI.filterComboBox, &QComboBox::currentTextChanged, this, &PlatformBrowserDockWidget::FilterChanged);
   connect(mUI.tableView->selectionModel(),
           &QItemSelectionModel::selectionChanged,
           this,
           &PlatformBrowserDockWidget::TableViewItemSelectionChanged);
   connect(mUI.tableView, &QTableView::customContextMenuRequested, this, &PlatformBrowserDockWidget::TableViewContextMenu);

   mCallbacks.Add(wkf::Observer::EntityDecorated.Connect(&PlatformBrowserDockWidget::PlatformDecoratedCB, this));
   mCallbacks.Add(wkf::Observer::PlatformActiveChanged.Connect(&PlatformBrowserDockWidget::PlatformActiveChangedCB, this));
   connect(&wkfEnv, &Environment::PlatformVisibilityChanged, this, &PlatformBrowserDockWidget::PlatformVisibilityChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &TeamVisibilityPrefObject::TeamColorChanged,
           this,
           &PlatformBrowserDockWidget::TeamColorsChanged);

   mCallbacks.Add(wkf::Observer::EntityAdded.Connect(&PlatformBrowserDockWidget::EntityAddedCB, this));
   mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&PlatformBrowserDockWidget::EntityDeletedCB, this));

   mUI.filterComboBox->addItem("All", (int)ALL);
   mUI.filterComboBox->addItem("Name", (int)NAME);
   mUI.filterComboBox->addItem("Type", (int)TYPE);
   mUI.filterComboBox->addItem("Side", (int)TEAM);
   mUI.tableView->hideColumn(TEAM);
}

void wkf::PlatformBrowserDockWidget::ClearFilter()
{
   mUI.filterLineEdit->clear();
   mUI.filterComboBox->setCurrentIndex(0);
}

void wkf::PlatformBrowserDockWidget::Reset()
{
   ClearFilter();
}

void wkf::PlatformBrowserDockWidget::TeamColorsChanged()
{
   mFilterModel.invalidate();
}

//! Method: SetPlatformSelected
//! Description: Select (highlight) item named aName in the dock widget
//! @param aName - the platform that will change selection state
//! @param aSelect - selection state of aTeamName
void wkf::PlatformBrowserDockWidget::SetPlatformSelected(const QString& aName, int aSelect)
{
   const auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.GetCurrentScenario()->FindEntity(aName.toStdString()));
   if (platform)
   {
      QModelIndex idx = mFilterModel.mapFromSource(mItemModel.IndexOf(platform->GetUniqueId()));
      mUI.tableView->selectionModel()->select(idx,
                                              QItemSelectionModel::Rows |
                                                 (aSelect ? QItemSelectionModel::Select : QItemSelectionModel::Deselect));
      if (aSelect)
      {
         mUI.tableView->scrollTo(idx);
      }
   }
}

QStringList wkf::PlatformBrowserDockWidget::GetSelectedPlatforms()
{
   QStringList retVal;
   for (auto it : mUI.tableView->selectionModel()->selectedIndexes())
   {
      if (it.isValid())
      {
         if (it.column() == NAME)
         {
            int         row      = mFilterModel.mapToSource(it).row();
            int         pid      = mItemModel.GetEntityId(row);
            const auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.GetCurrentScenario()->FindEntity(pid));
            if (platform)
            {
               retVal.push_back(QString::fromStdString(platform->GetName()));
            }
         }
      }
   }
   return retVal;
}

//! Method: FilterChanged
//! Description: Called when user edits dock widget line edit or combo box
void wkf::PlatformBrowserDockWidget::FilterChanged()
{
   mDirtyFlag = true;
   int column = mUI.filterComboBox->currentData().toInt();
   mFilterModel.setFilterKeyColumn(column);
   mFilterModel.setFilterWildcard(mUI.filterLineEdit->text());
   UpdateInfo();
}

//! Method: TableViewContextMenu
//! Description: Notifies interface that we need a context menu
//! @param aPoint - the point at which to show the context menu
void wkf::PlatformBrowserDockWidget::TableViewContextMenu(const QPoint& aPoint)
{
   QModelIndex idx = mUI.tableView->indexAt(aPoint);

   if (idx.isValid())
   {
      int         row      = mFilterModel.mapToSource(idx).row();
      int         pid      = mItemModel.GetEntityId(row);
      const auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.GetCurrentScenario()->FindEntity(pid));
      if (platform)
      {
         emit ContextMenuRequested(mUI.tableView->mapToGlobal(aPoint), QString::fromStdString(platform->GetName()));
      }
   }
}

//! Method: TableViewItemSelectionChanged
//! Description: Notifies interface that a platform should be selected on the map
void wkf::PlatformBrowserDockWidget::TableViewItemSelectionChanged(const QItemSelection& aSelected,
                                                                   const QItemSelection& aDeselected)
{
   QStringList selected = GetSelectedPlatforms();
   emit        SelectedPlatformChanged(selected);
}

void wkf::PlatformBrowserDockWidget::UpdateInfo()
{
   if (mDirtyFlag)
   {
      mDirtyFlag = false;
      mUI.tableView->resizeColumnsToContents();
      mUI.tableView->resizeRowsToContents();
      mUI.tableView->setSortingEnabled(true);
      setWindowTitle(QString("Platform Browser (%1)").arg(mFilterModel.rowCount()));
   }
}

void wkf::PlatformBrowserDockWidget::ShowInactive(bool aState)
{
   mShowInactive = aState;
   mFilterModel.invalidate();
}

void wkf::PlatformBrowserDockWidget::PlatformVisibilityChanged()
{
   mFilterModel.invalidate();
   mDirtyFlag = true;
   UpdateInfo();
}

void wkf::PlatformBrowserDockWidget::PlatformDecoratedCB(wkf::Entity*       aEntityPtr,
                                                         const QIcon&       aIcon,
                                                         const std::string& aDescription)
{
   mItemModel.NotifyOfDecoratorChange(aEntityPtr->GetUniqueId());
}

void wkf::PlatformBrowserDockWidget::PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive)
{
   mItemModel.NotifyOfActiveChange(aPlatformPtr->GetUniqueId(), aActive);
   setWindowTitle(QString("Platform Browser (%1)").arg(mFilterModel.rowCount()));
}

void wkf::PlatformBrowserDockWidget::EntityAddedCB(vespa::VaEntity* aEntityPtr)
{
   mDirtyFlag = true;
   mItemModel.EntityAdded(aEntityPtr);
}

void wkf::PlatformBrowserDockWidget::EntityDeletedCB(vespa::VaEntity* aEntityPtr)
{
   mDirtyFlag = true;
   mItemModel.EntityRemoved(aEntityPtr);
}

bool wkf::PlatformBrowserDockWidget::FilterModel::filterAcceptsRow(int aRow, const QModelIndex& aSourceParent) const
{
   bool hidden   = false;
   auto srcModel = static_cast<ItemModel*>(sourceModel());

   int idx = srcModel->GetEntityId(aRow);

   const auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.GetCurrentScenario()->FindEntity(idx));
   if (!platform || !wkfEnv.IsPlatformVisible(platform))
   {
      hidden = true;
   }
   else if (!mBrowser->GetShowInactive() && !(srcModel->IsActive(platform->GetUniqueId())))
   {
      hidden = true;
   }

   return !hidden && QSortFilterProxyModel::filterAcceptsRow(aRow, aSourceParent);
}

QVariant wkf::PlatformBrowserDockWidget::ItemModel::headerData(int             aSection,
                                                               Qt::Orientation aOrientation,
                                                               int             aRole /* = Qt::DisplayRole */) const
{
   if ((aRole == Qt::DisplayRole) && (aOrientation == Qt::Horizontal))
   {
      if (aSection == NAME)
      {
         return "Name";
      }
      else if (aSection == TYPE)
      {
         return "Type";
      }
      else if (aSection == TEAM)

      {
         return "Side";
      }
   }
   return QVariant();
}

QVariant wkf::PlatformBrowserDockWidget::ItemModel::data(const QModelIndex& aIndex, int aRole) const
{
   if (static_cast<unsigned int>(aIndex.row()) < mIndexList.size())
   {
      int         platid   = mIndexList.at(aIndex.row());
      const auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.GetCurrentScenario()->FindEntity(platid));
      if (aRole == Qt::DisplayRole)
      {
         switch (aIndex.column())
         {
         case (NAME):
            return QString::fromStdString(platform->GetName());
         case (TYPE):
            return QString::fromStdString(platform->GetTypeAFSIM());
         case (DECORATION):
            if (!platform->GetDecoration().isNull())
            {
               return QString::number(platform->GetDecoration().cacheKey());
            }
            return QVariant();
         case (TEAM):
            return QString::fromStdString(platform->GetSide());
         }
      }
      else if (aRole == Qt::BackgroundRole)
      {
         const auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
         if (prefs)
         {
            QColor color = prefs->GetTeamColor(QString::fromStdString(platform->GetSide()));
            color.setAlpha(85);
            if (!IsActive(platform->GetUniqueId()))
            {
               qreal hsva[4];
               color.getHsvF(&hsva[0], &hsva[1], &hsva[2], &hsva[3]);
               hsva[1] *= 0.5;
               hsva[2] *= 0.8;
               color.setHsvF(hsva[0], hsva[1], hsva[2], hsva[3]);
            }
            // modify by active state
            return color;
         }
      }
      else if (aRole == Qt::DecorationRole)
      {
         if (aIndex.column() == DECORATION)
         {
            return platform->GetDecoration();
         }
      }
      else if (aRole == Qt::UserRole)
      {
         if (aIndex.column() == DECORATION)
         {
            return UtQtTransparentBackgroundDelegate::eHIDE_TEXT;
         }
      }
   }
   return QVariant();
}

QModelIndex wkf::PlatformBrowserDockWidget::ItemModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
   return createIndex(aRow, aColumn);
}

void wkf::PlatformBrowserDockWidget::ItemModel::NotifyOfActiveChange(int aUniqueId, bool aActive)
{
   mActiveMap[aUniqueId] = aActive;
   QModelIndex idx       = IndexOf(aUniqueId);
   QModelIndex idx2      = IndexOf(aUniqueId).siblingAtColumn(2);
   dataChanged(idx, idx2, QVector<int>{Qt::BackgroundRole});
}

void wkf::PlatformBrowserDockWidget::ItemModel::NotifyOfDecoratorChange(int aUniqueId)
{
   QModelIndex idx = IndexOf(aUniqueId).siblingAtColumn(2);
   dataChanged(idx, idx, QVector<int>{Qt::DisplayRole, Qt::DecorationRole});
}

bool wkf::PlatformBrowserDockWidget::ItemModel::IsActive(int aUniqueId) const
{
   auto it = mActiveMap.find(aUniqueId);
   if (it != mActiveMap.end())
   {
      return it->second;
   }
   return true;
}

int wkf::PlatformBrowserDockWidget::ItemModel::rowCount(const QModelIndex& aParent) const
{
   return mIndexList.size();
}


void wkf::PlatformBrowserDockWidget::ItemModel::EntityAdded(vespa::VaEntity* aEntityPtr)
{
   if (dynamic_cast<wkf::Platform*>(aEntityPtr) != nullptr)
   {
      int lsize = mIndexList.size();
      beginInsertRows(QModelIndex(), lsize, lsize);
      mIndexList.push_back(aEntityPtr->GetUniqueId());
      endInsertRows();
   }
}

void wkf::PlatformBrowserDockWidget::ItemModel::EntityRemoved(vespa::VaEntity* aEntityPtr)
{
   if (dynamic_cast<wkf::Platform*>(aEntityPtr) != nullptr)
   {
      auto it  = std::find(mIndexList.begin(), mIndexList.end(), aEntityPtr->GetUniqueId());
      int  row = it - mIndexList.begin();
      beginRemoveRows(QModelIndex(), row, row);
      mIndexList.erase(it);
      endRemoveRows();
   }
}

QModelIndex wkf::PlatformBrowserDockWidget::ItemModel::IndexOf(int aUniqueId)
{
   auto it  = std::find(mIndexList.begin(), mIndexList.end(), aUniqueId);
   int  row = it - mIndexList.begin();
   return index(row, 0, QModelIndex());
}
