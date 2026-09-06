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
#include "WkfPlatformDataDockWidget.hpp"

#include <QDrag>
#include <QHoverEvent>
#include <QMenu>
#include <QMimeData>

#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformDataPrefObject.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfUnitTypes.hpp"

wkf::PlatformDataDockWidget::PlatformDataDockWidget(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
   : PlatformContextDockWidget("Platform Details", &Plugin::GetPlatformData, parent, f)
   , mPopupWidget(new QListWidget(nullptr))
   , mPopupFrame(new QFrame(nullptr, Qt::ToolTip))
   , mPreviousPluginItemSelected(nullptr)
{
   wkfEnv.RegisterPrefObject(new PlatformDataPrefObject(this));

   setObjectName("PlatformDataDockWidget");

   mUi.mGroupSelection->setVisible(false); // hide group selection combo box
   mUi.treeWidget->setColumnCount(2);
   mUi.treeWidget->headerItem()->setText(1, ""); // clear 2nd column header text

   connect(mUi.treeWidget, &DraggableTreeWidget::DragEvent, this, &PlatformDataDockWidget::DragEvent);
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &QTreeWidget::customContextMenuRequested, this, &PlatformDataDockWidget::TreeWidgetContextMenu);

   auto* layout = new QHBoxLayout(mPopupFrame);
   layout->setContentsMargins(0, 0, 0, 0);
   mPopupFrame->setLayout(layout);
   layout->addWidget(mPopupWidget); // mPopupWidget will be deleted when the layout is deleted when mPopupFrame is deleted.

   mUi.treeWidget->setAttribute(Qt::WA_Hover, true);
   mUi.treeWidget->installEventFilter(this);

   mPopupFrame->setAttribute(Qt::WA_Hover, true);
   mPopupFrame->installEventFilter(this);
}

wkf::PlatformDataDockWidget::~PlatformDataDockWidget()
{
   delete mPopupFrame;
}

void wkf::PlatformDataDockWidget::DragEvent(QTreeWidgetItem* aItem)
{
   if (dynamic_cast<QTreeWidgetItem*>(aItem))
   {
      QMimeData*             mimeData = new QMimeData;
      QByteArray             bytes;
      QDataStream            out(&bytes, QIODevice::WriteOnly);
      const QTreeWidgetItem* parentItem = aItem;
      while (parentItem->parent())
      {
         parentItem = parentItem->parent();
      }

      unsigned int pluginId = parentItem->data(0, wkf::PLUGIN_ID_ROLE).toUInt();
      out << pluginId;
      out << QString(mPlatformName.c_str());
      out << aItem->text(0);
      out << aItem->data(0, wkf::UPDATER_CONTEXT).toUInt();
      mimeData->setData(Updater::cMIME_IDENTIFIER, bytes); // need to give an address to the updater here
      QDrag* drag = new QDrag(this);
      drag->setMimeData(mimeData);
      drag->exec(Qt::DropAction::LinkAction);
      // capture the updater and pass it to whoever receives the drop
   }
}

void wkf::PlatformDataDockWidget::ClearCurrent(bool aExplicit)
{
   PlatformContextDockWidget::ClearCurrent();

   mPopupFrame->hide();
}

void wkf::PlatformDataDockWidget::ItemSelectionChanged()
{
   // handle de-select on the previous item
   if (mPreviousPluginItemSelected)
   {
      mPreviousPluginItemSelected->PlatformDataItemUnSelected();
      mPreviousPluginItemSelected = nullptr;
   }

   // Notify the appropriate plugin of a new selection
   QList<QTreeWidgetItem*> items = mUi.treeWidget->selectedItems();
   for (auto& childItem : items)
   {
      // Get the plugin that is responsible for the updating the selected item
      Plugin* plugin = GetPluginFromItem(childItem);
      if (plugin)
      {
         mPreviousPluginItemSelected = plugin;
         plugin->PlatformDataItemSelected(childItem);
      }
   }
}

bool wkf::PlatformDataDockWidget::eventFilter(QObject* aObject, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::HoverMove || aEvent->type() == QEvent::HoverEnter)
   {
      QHoverEvent* hover = dynamic_cast<QHoverEvent*>(aEvent);
      if (hover != nullptr)
      {
         QPoint newGlobalPos;
         QPoint oldGlobalPos;
         // The position in aEvent is relative to aObject, convert to global point
         if (aObject == mUi.treeWidget)
         {
            newGlobalPos = mUi.treeWidget->mapToGlobal(hover->pos());
            oldGlobalPos = mUi.treeWidget->mapToGlobal(hover->oldPos());
         }
         else
         {
            newGlobalPos = mPopupFrame->mapToGlobal(hover->pos());
            oldGlobalPos = mPopupFrame->mapToGlobal(hover->oldPos());
         }

         // itemAt() takes a point relative to the treeWidget->viewport()
         QPoint newViewportPos = mUi.treeWidget->viewport()->mapFromGlobal(newGlobalPos);
         QPoint oldViewportPos = mUi.treeWidget->viewport()->mapFromGlobal(oldGlobalPos);

         QTreeWidgetItem* oldItem = mUi.treeWidget->itemAt(oldViewportPos);
         QTreeWidgetItem* newItem = mUi.treeWidget->itemAt(newViewportPos);

         // Offset the popup away from the cursor because it is more aesthetically pleasing
         QPoint displayPoint(newGlobalPos.x() + 5, newGlobalPos.y() + 15);

         if (oldItem != newItem)
         {
            UnitTreeWidgetItem* oldUnitItem = dynamic_cast<UnitTreeWidgetItem*>(oldItem);
            if (oldUnitItem != nullptr)
            {
               oldUnitItem->RemoveListWidget();
            }

            UnitTreeWidgetItem* newUnitItem = dynamic_cast<UnitTreeWidgetItem*>(newItem);
            // If the newItem is a WkUnitTreeWidgetItem that supports the Popup display
            if (newUnitItem != nullptr && newUnitItem->PopupSupported())
            {
               mPopupFrame->move(displayPoint);
               // inform the WkUnitTreeWidgetItem which QListWidget to update for the popup
               newUnitItem->SetListWidget(mPopupWidget);
               // update the contents of the popup so that it can be resized.
               newUnitItem->UpdateDisplay();
               // resize the contents of the popup
               mPopupFrame->resize(mPopupWidget->sizeHintForColumn(0) + 2,
                                   mPopupWidget->sizeHintForRow(0) * mPopupWidget->count() + 2);
               mPopupFrame->show();
            }
            else
            {
               // Hide popup if not current hovering over a WkUnitTreeWidgetItem
               mPopupFrame->hide();
            }
         }
         // Update the position of the popup, if it is visible
         else if (mPopupFrame->isVisible())
         {
            mPopupFrame->move(displayPoint);
         }
      }
   }
   else if (aEvent->type() == QEvent::HoverLeave)
   {
      // On HoverEvent, if the Old WkUnitTreeWidgetItem still has a pointer to the QListWidget,
      // inform the WkUnitTreeWidgetItem to stop updating the QListWidget
      if (aObject == mUi.treeWidget)
      {
         QHoverEvent* hover = dynamic_cast<QHoverEvent*>(aEvent);
         if (hover != nullptr)
         {
            QPoint oldPos = mUi.treeWidget->viewport()->mapFromGlobal(mUi.treeWidget->mapToGlobal(hover->oldPos()));
            QTreeWidgetItem* oldItem = mUi.treeWidget->itemAt(oldPos);
            if (oldItem != nullptr)
            {
               UnitTreeWidgetItem* oldUnitItem = dynamic_cast<UnitTreeWidgetItem*>(oldItem);
               if (oldUnitItem != nullptr)
               {
                  oldUnitItem->RemoveListWidget();
               }
            }
         }

         mPopupFrame->hide();
      }
   }
   // Allow the event to be processed by aObject
   return false;
}

void wkf::PlatformDataDockWidget::HandleItemPreference(QTreeWidgetItem* aItemPtr, const QString& aPluginName)
{
   PlatformDataTreeItem* item = dynamic_cast<PlatformDataTreeItem*>(aItemPtr);
   if (item)
   {
      HandleItemPreference(item, aPluginName);

      for (int i = 0; i < aItemPtr->childCount(); ++i)
      {
         HandleItemPreference(aItemPtr->child(i), aPluginName);
      }
   }
}

void wkf::PlatformDataDockWidget::HandleItemPreference(PlatformDataTreeItem* aItemPtr, const QString& aPluginName)
{
   QString name = BuildName(aItemPtr, aPluginName);
   if (!name.isEmpty())
   {
      PlatformDataPrefObject* dataPrefs = wkfEnv.GetPreferenceObject<PlatformDataPrefObject>();

      if (aItemPtr->PreferenceHideable())
      {
         if (dataPrefs->HasPreferences(name))
         {
            aItemPtr->SetHidden(!dataPrefs->IsVisible(name), PlatformDataTreeItem::PREFERENCE);
         }
         else
         {
            bool defaultHidden = aItemPtr->DefaultHidden();
            if (defaultHidden)
            {
               dataPrefs->SetHidden(name);
            }
            aItemPtr->SetHidden(defaultHidden, PlatformDataTreeItem::PREFERENCE);
         }
      }
   }
}

void wkf::PlatformDataDockWidget::ShowAllItems()
{
   PlatformDataPrefObject* dataPrefs = wkfEnv.GetPreferenceObject<PlatformDataPrefObject>();
   dataPrefs->ClearHidden();
   mUi.treeWidget->topLevelItemCount();
   for (int i = 0; i < mUi.treeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* item = mUi.treeWidget->topLevelItem(i);
      wkf::Plugin*     p    = GetPluginFromItem(item);
      if (p != nullptr)
      {
         HandleItemPreference(item, p->GetName());
      }
   }
}

void wkf::PlatformDataDockWidget::HideItem(PlatformDataTreeItem* aItemPtr)
{
   if (aItemPtr->PreferenceHideable())
   {
      wkf::Plugin* p = GetPluginFromItem(aItemPtr);
      if (p != nullptr)
      {
         QString name = BuildName(aItemPtr, p->GetName());
         if (!name.isEmpty())
         {
            PlatformDataPrefObject* dataPrefs = wkfEnv.GetPreferenceObject<PlatformDataPrefObject>();
            dataPrefs->SetHidden(name);
            aItemPtr->SetHidden(true, PlatformDataTreeItem::PREFERENCE);
         }
      }
   }
}

QString wkf::PlatformDataDockWidget::BuildName(QTreeWidgetItem* aItemPtr, const QString& aPluginName)
{
   QStringList      id;
   QTreeWidgetItem* item    = aItemPtr;
   bool             success = true;
   while (item != nullptr)
   {
      QString t = item->text(0);
      if (t.isEmpty())
      {
         success = false;
         break;
      }
      id.prepend(t);
      item = item->parent();
   }
   if (success)
   {
      id.prepend(aPluginName);
      return id.join('|');
   }
   return QString();
}

void wkf::PlatformDataDockWidget::TreeWidgetContextMenu(const QPoint& aPos)
{
   // get the item that the context menu was created on
   QMenu            ctxMenu;
   QPoint           globalPos = mapToGlobal(aPos);
   QTreeWidgetItem* ctxItem   = ItemAt(globalPos);

   if (ctxItem)
   {
      BuildMenuForItem(ctxMenu, *ctxItem);
   }

   if (!ctxMenu.actions().empty())
   {
      ctxMenu.addSeparator();
   }
   if (ctxItem)
   {
      wkf::PlatformDataTreeItem* item = dynamic_cast<wkf::PlatformDataTreeItem*>(ctxItem);
      if (item != nullptr)
      {
         if (item->PreferenceHideable())
         {
            QAction* hideItem = new QAction("Hide " + ctxItem->text(0), &ctxMenu);
            ctxMenu.addAction(hideItem);
            connect(hideItem, &QAction::triggered, [this, item]() { HideItem(item); });
         }
      }
   }
   QAction* showAll = new QAction("Show all items", &ctxMenu);
   ctxMenu.addAction(showAll);
   connect(showAll, &QAction::triggered, this, &PlatformDataDockWidget::ShowAllItems);

   if (mPopupFrame)
   {
      mPopupFrame->hide();
   }
   ctxMenu.exec(globalPos);
}

QTreeWidgetItem* wkf::PlatformDataDockWidget::ItemAt(QPoint aPosition)
{
   return mUi.treeWidget->itemAt(mUi.treeWidget->viewport()->mapFromGlobal(aPosition));
}
