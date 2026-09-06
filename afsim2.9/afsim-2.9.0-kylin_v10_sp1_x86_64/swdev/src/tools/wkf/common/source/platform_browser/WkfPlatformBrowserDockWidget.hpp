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
#ifndef WKFPLATFORMBROWSERDOCKWIDGET_HPP
#define WKFPLATFORMBROWSERDOCKWIDGET_HPP

#include "wkf_common_export.h"

#include <QDockWidget>
#include <QMutex>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "VaCallbackHolder.hpp"
#include "VaEntity.hpp"
#include "ui_WkfPlatformBrowserDockWidget.h"

namespace wkf
{
class Entity;
class Platform;

class WKF_COMMON_EXPORT PlatformBrowserDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   PlatformBrowserDockWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

   void ClearFilter();
   void Reset();

   void SetPlatformSelected(const QString& aName, int aSelect);

   QStringList GetSelectedPlatforms();

   void UpdateInfo();

   void ShowInactive(bool aState);
   bool GetShowInactive() const { return mShowInactive; }

signals:
   void ContextMenuRequested(const QPoint& aPoint, const QString& aText);
   void SelectedPlatformChanged(const QList<QString>& aNames);

private:
   void FilterChanged();
   void TableViewContextMenu(const QPoint& aPoint);
   void TableViewItemSelectionChanged(const QItemSelection& aSelected, const QItemSelection& aDeselected);

   void PlatformVisibilityChanged();
   void TeamColorsChanged();

   void PlatformDecoratedCB(wkf::Entity* aEntityPtr, const QIcon& aIcon, const std::string& aDescription);
   void PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive);

   void EntityAddedCB(vespa::VaEntity* aEntityPtr);
   void EntityDeletedCB(vespa::VaEntity* aEntityPtr);

   Ui::WkfPlatformBrowserDockWidget mUI;
   vespa::VaCallbackHolder          mCallbacks;

   // Indicates that the windows needs to be Updated.
   bool mDirtyFlag;

   bool mShowInactive;

   class FilterModel : public QSortFilterProxyModel
   {
   public:
      bool filterAcceptsRow(int aRow, const QModelIndex& aSourceParent) const override;
      void SetBrowser(PlatformBrowserDockWidget* aBrowser) { mBrowser = aBrowser; }

   private:
      const PlatformBrowserDockWidget* mBrowser = nullptr;
   };
   class ItemModel : public QAbstractItemModel
   {
   public:
      QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole /* = Qt::DisplayRole */) const override;
      int      columnCount(const QModelIndex& aParent) const override { return 4; }
      QVariant data(const QModelIndex& aIndex, int aRole) const override;
      QModelIndex parent(const QModelIndex& aIndex) const override { return QModelIndex(); }
      QModelIndex index(int aRow, int aColumn, const QModelIndex& aParent = QModelIndex()) const override;
      int         rowCount(const QModelIndex& aParent = QModelIndex()) const override;
      void        EntityAdded(vespa::VaEntity* aEntityPtr);
      void        EntityRemoved(vespa::VaEntity* aEntityPtr);
      int         GetEntityId(int aRow) { return mIndexList.at(aRow); }
      // returns the index of the row that matches this platform-unique-id
      QModelIndex IndexOf(int aUniqueId);
      void        NotifyOfActiveChange(int aUniqueId, bool aActive);
      void        NotifyOfDecoratorChange(int aUniqueId);
      bool        IsActive(int aUniqueId) const;

   private:
      std::vector<int>    mIndexList;
      std::map<int, bool> mActiveMap;
   };

   ItemModel   mItemModel;
   FilterModel mFilterModel;

   enum ColumnType
   {
      NAME       = 0,
      TYPE       = 1,
      DECORATION = 2,
      TEAM       = 3,
      ALL        = -1
   };
};
} // namespace wkf
#endif
