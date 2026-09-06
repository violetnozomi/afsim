// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfPlatformContextDockWidget.hpp"

#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>

#include "WkfComboBoxTreeWidgetItem.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"

wkf::PlatformContextDockWidget::PlatformContextDockWidget(const QString&      title,
                                                          const ItemProvider& provider,
                                                          QWidget*            parent /*= nullptr*/,
                                                          Qt::WindowFlags     f /*= Qt::WindowFlags()*/)
   : QDockWidget(title, parent, f)
   , mPlatformName()
   , mItemProvider(provider)
{
   mUi.setupUi(this);
   connect(&wkfEnv, &Environment::PlatformOfInterestChanged, this, &PlatformContextDockWidget::PlatformOfInterestChanged);

   auto expFn = [this](QTreeWidgetItem* item, bool expanded)
   {
      mItemExpansionMap[item->text(0)] = expanded;
      SaveSettings();
   };
   connect(mUi.treeWidget, &QTreeWidget::itemExpanded, this, [expFn](QTreeWidgetItem* item) { expFn(item, true); });
   connect(mUi.treeWidget, &QTreeWidget::itemCollapsed, this, [expFn](QTreeWidgetItem* item) { expFn(item, false); });
   connect(mUi.treeWidget,
           &QTreeWidget::itemChanged,
           this,
           [this](QTreeWidgetItem* aItem, int aColumn) { this->ItemChanged(ChangeInfo(aItem, "", aColumn)); });
   connect(mUi.treeWidget, &QTreeWidget::itemSelectionChanged, this, &PlatformContextDockWidget::ItemSelectionChanged);
   connect(mUi.mGroupSelection,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           &PlatformContextDockWidget::GroupIndexChanged);

   mCallbacks.Add(Observer::LoadSettings.Connect(&PlatformContextDockWidget::LoadSettings, this));
}

wkf::PlatformContextDockWidget::~PlatformContextDockWidget()
{
   TakeChildren();
}

bool wkf::PlatformContextDockWidget::SetCurrentPlatform(const Platform& aPlatform)
{
   if (mPlatformName != aPlatform.GetName())
   {
      ClearCurrent();
      mPlatformName = aPlatform.GetName();

      QTreeWidgetItem* item = mUi.treeWidget->headerItem();
      if (item)
      {
         item->setText(0, QString::fromStdString(mPlatformName));
      }

      PopulateItems();

      mUi.treeWidget->resizeColumnToContents(0);
      return true;
   }
   return false;
}

void wkf::PlatformContextDockWidget::PopulateItems()
{
   auto plugins = wkfEnv.GetPluginManager()->GetLoadedPlugins();
   std::map<unsigned int, std::vector<std::pair<QString, QTreeWidgetItem*>>> itemMap;
   for (auto& p : plugins)
   {
      auto list = (p.second->*mItemProvider)(mPlatformName);
      for (auto& listIter : list)
      {
         unsigned int qi = static_cast<unsigned int>(p.first);
         // Store the plugin responsible this item and it children
         listIter->setData(0, wkf::PLUGIN_ID_ROLE, qi);

         // Add the top level item to the treeWidget
         QVariant v          = listIter->data(0, wkf::DISPLAY_PRECEDENCE);
         int      precedence = v.isValid() ? v.toInt() : 1000;

         itemMap[precedence].emplace_back(p.second->GetName(), listIter);
      }
   }
   for (auto&& itemPair : itemMap)
   {
      for (auto&& itemData : itemPair.second)
      {
         mUi.treeWidget->addTopLevelItem(itemData.second);
         itemData.second->setExpanded(GetDefaultItemExpanded(itemData.second->text(0), true));

         // Expand child items similar to how items on the top level are expanded.
         for (int i = 0; i < itemData.second->childCount(); ++i)
         {
            QTreeWidgetItem* item = itemData.second->child(i);
            if (item->childCount() > 0)
            {
               item->setExpanded(GetDefaultItemExpanded(item->text(0), false));
            }
         }
         HandleItemPreference(itemData.second, itemData.first);
      }
   }

   BuildComboBoxWidgetItems(mUi.treeWidget->invisibleRootItem());
}

void wkf::PlatformContextDockWidget::BuildComboBoxWidgetItems(QTreeWidgetItem* aItem)
{
   ComboBoxTreeWidgetItem* item = dynamic_cast<ComboBoxTreeWidgetItem*>(aItem);
   if (item != nullptr)
   {
      item->BuildWidget(mUi.treeWidget);

      // Covers any change through drop-down selection or hitting enter on new option
      connect(item->GetBox().data(),
              static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
              this,
              [this, item](int index)
              {
                 QStandardItemModel* modelPtr = dynamic_cast<QStandardItemModel*>(item->GetBox()->model());

                 // Guard for error that causes reclicking on tree child to select its category instead of it
                 // If selected first-level item at index is a category, and its text matches the current item, reset change
                 if (modelPtr && modelPtr->item(index) && !modelPtr->item(index)->isSelectable() &&
                     modelPtr->item(index)->text() == item->GetBox()->currentText())
                 {
                    item->setText(1, item->GetText());
                 }

                 // If change occurred, broadcast it
                 else if (item->GetText() != item->GetBox()->itemText(index))
                 {
                    item->setText(1, item->GetBox()->currentText());
                    this->ItemChanged(ChangeInfo(item, item->GetBox()->currentText(), 1));
                 }
              });

      // If editable, check for case where new string was added, but focus was lost instead of enter pressed
      connect(item->GetBox()->lineEdit(),
              &QLineEdit::editingFinished,
              [this, item]()
              {
                 if (item->GetBox()->currentText() == item->GetText())
                 {
                    return;
                 }
                 QStandardItemModel* modelPtr = dynamic_cast<QStandardItemModel*>(item->GetBox()->model());
                 QString             text     = item->GetBox()->currentText();
                 if (modelPtr && text != "")
                 {
                    if (item->IsCategorized())
                    {
                       int oldRoot = -1;
                       int oldInd  = -1;
                       for (auto row = 0; row < modelPtr->rowCount(); row++)
                       {
                          QStandardItem* cat = modelPtr->item(row);
                          for (auto i = 0; cat->child(i) != nullptr; i++)
                          {
                             if (cat->child(i)->text() == text)
                             {
                                item->GetBox()->setRootModelIndex(modelPtr->item(row)->index());
                                item->GetBox()->setCurrentIndex(i);
                                item->setText(1, cat->child(i)->text());
                                item->GetBox()->setRootModelIndex(modelPtr->invisibleRootItem()->index());
                                this->ItemChanged(ChangeInfo(item, text, 1));
                                return;
                             }
                             if (cat->child(i)->text() == item->GetText())
                             {
                                oldRoot = row;
                                oldInd  = i;
                             }
                          }
                       }
                       if (oldRoot != -1 && oldInd != -1)
                       {
                          item->GetBox()->setRootModelIndex(modelPtr->item(oldRoot)->index());
                          item->GetBox()->setCurrentIndex(oldInd);
                          item->GetBox()->setCurrentText(modelPtr->item(oldRoot)->child(oldInd)->text());
                          item->GetBox()->setRootModelIndex(modelPtr->invisibleRootItem()->index());
                       }
                    }
                    else
                    {
                       for (auto row = 0; row < modelPtr->rowCount(); row++)
                       {
                          // If item is already in list, change is covered by other function
                          if (modelPtr->item(row)->text() == text)
                          {
                             return;
                          }
                       }

                       item->GetBox()->addItem(text);
                       modelPtr->sort(0);
                       this->ItemChanged(ChangeInfo(item, text, 1));
                    }
                 }
              });
   }
   for (int i = 0; i < aItem->childCount(); ++i)
   {
      BuildComboBoxWidgetItems(aItem->child(i));
   }
}

void wkf::PlatformContextDockWidget::ClearCurrent(bool aExplicit)
{
   TakeChildren();

   QTreeWidgetItem* item = mUi.treeWidget->headerItem();
   if (item)
   {
      item->setText(0, "");
   }

   // Clear the name from the tree widget
   mPlatformName.clear();
}

void wkf::PlatformContextDockWidget::PlatformOfInterestChanged(Platform* aPlatform)
{
   if (aPlatform)
   {
      SetCurrentPlatform(*aPlatform);
   }
   else
   {
      ClearCurrent(true);
   }
}

bool wkf::PlatformContextDockWidget::GetDefaultItemExpanded(const QString& aItemName, bool aDefault) const
{
   auto entry = mItemExpansionMap.find(aItemName);
   if (entry != mItemExpansionMap.end())
   {
      return entry->second;
   }
   return aDefault;
}

void wkf::PlatformContextDockWidget::SaveSettings()
{
   QSettings& settings = wkfEnv.GetUserSettingsFile();
   settings.beginGroup("PlatformContextDock");
   settings.beginGroup(objectName());
   int i = 0;
   settings.beginWriteArray("Items");
   for (const auto& item : mItemExpansionMap)
   {
      settings.setArrayIndex(i);
      settings.setValue("name", item.first);
      settings.setValue("expanded", item.second);
      i++;
   }
   settings.endArray();
   settings.endGroup();
   settings.endGroup();
}

void wkf::PlatformContextDockWidget::LoadSettings(QSettings& aSettings)
{
   mItemExpansionMap.clear();

   aSettings.beginGroup("PlatformContextDock");
   aSettings.beginGroup(objectName());
   int count = aSettings.beginReadArray("Items");
   for (int i = 0; i < count; ++i)
   {
      aSettings.setArrayIndex(i);
      QString label            = aSettings.value("name").toString();
      bool    expanded         = aSettings.value("expanded").toBool();
      mItemExpansionMap[label] = expanded;
   }
   aSettings.endArray();
   aSettings.endGroup();
   aSettings.endGroup();
}

void wkf::PlatformContextDockWidget::TakeChildren()
{
   // Remove the children from the treeWidget without deleting the children
   // It is expected that the children of this widget are owned by someone else,
   //  and that they will clean up the children
   while (mUi.treeWidget->topLevelItemCount() > 0)
   {
      mUi.treeWidget->takeTopLevelItem(0);
   }
}

wkf::Plugin* wkf::PlatformContextDockWidget::GetPluginFromItem(const QTreeWidgetItem* aItem) const
{
   const QTreeWidgetItem* parentItem = aItem;
   while (parentItem->parent())
   {
      parentItem = parentItem->parent();
   }

   size_t pluginUID = static_cast<size_t>(parentItem->data(0, wkf::PLUGIN_ID_ROLE).toUInt());
   if (pluginUID > 0)
   {
      auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginUID);
      return plugin;
   }
   return nullptr;
}
