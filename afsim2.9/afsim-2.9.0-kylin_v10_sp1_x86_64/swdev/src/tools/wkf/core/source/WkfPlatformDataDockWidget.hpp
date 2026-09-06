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
#ifndef WKFPLATFORMDATADOCKWIDGET_HPP
#define WKFPLATFORMDATADOCKWIDGET_HPP

#include "wkf_export.h"

#include <QListWidget>
#include <QTreeWidgetItem>

#include "WkfPlatformContextDockWidget.hpp"
#include "WkfPlatformDataTreeItem.hpp"

namespace wkf
{
class WKF_EXPORT PlatformDataDockWidget : public PlatformContextDockWidget
{
public:
   PlatformDataDockWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   virtual ~PlatformDataDockWidget() override;

protected:
   void DragEvent(QTreeWidgetItem* aItem);

   void ClearCurrent(bool aExplicit) override;

   void ItemSelectionChanged() override;

   bool eventFilter(QObject* aObject, QEvent* aEvent) override;

   void    HandleItemPreference(QTreeWidgetItem* aItemPtr, const QString& aPluginName) override;
   void    HandleItemPreference(PlatformDataTreeItem* aItemPtr, const QString& aPluginName);
   void    ShowAllItems();
   void    HideItem(PlatformDataTreeItem* aItemPtr);
   QString BuildName(QTreeWidgetItem* aItemPtr, const QString& aPluginName);

   virtual void BuildMenuForItem(QMenu& aMenu, QTreeWidgetItem& aItemPtr) {}

   void TreeWidgetContextMenu(const QPoint& aPos);
   // Return the QTreeWidgetItem located at the global QPoint location
   QTreeWidgetItem* ItemAt(QPoint aPosition);

   // The list widget that appears in the popup display
   QListWidget* mPopupWidget;
   // The popup display shown when hovering over WkUnitTreeWidgetItems
   QFrame* mPopupFrame;

   // Stores the last plugin to have a QTreeWidgetItem selected in the treeWidget.
   // This allows us to notify a plugin when an Item is no longer selected.
   Plugin* mPreviousPluginItemSelected;
};
} // namespace wkf
#endif
