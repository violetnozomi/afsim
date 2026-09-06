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

#include "WkfPreferencesDialog.hpp"

#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>

#include "WkfAction.hpp"
#include "WkfConfigurationObject.hpp"
#include "WkfDebugPrefWidget.hpp"
#include "WkfEnvironment.hpp"
#include "WkfGeneralPrefWidget.hpp"
#include "WkfKeyBindingsWidget.hpp"
#include "WkfMainWindow.hpp"
#include "WkfMilStdIconPrefWidget.hpp"
#include "WkfObserver.hpp"
#include "WkfOverlayUpdaterPrefWidget.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfSharedMapPreferencesWidget.hpp"
#include "WkfTeamVisibilityPrefWidget.hpp"
#include "WkfUnitsWidget.hpp"

wkf::PreferencesDialog::PreferencesDialog(MainWindow* parent, Qt::WindowFlags f /* = Qt::WindowFlags() */)
   : Dialog(parent, new Action(QIcon::fromTheme("wrench"), "Preferences...", nullptr), "Options", f)
{
   mUI.setupUi(this);
   mTriggerActionPtr->setShortcut(QKeySequence(QKeySequence::Preferences));

   mStandardWidgets << new UnitsWidget(this);
   mStandardWidgets << new GeneralPrefWidget(this);
   mStandardWidgets << new SharedMapPreferencesWidget(this);
   mStandardWidgets << new DebugPrefWidget(this);
   mStandardWidgets << new MilStdIconPrefWidget(this);
   mStandardWidgets << new KeyBindingsWidget(this);
   mStandardWidgets << new TeamVisibilityPrefWidget(this);
   mStandardWidgets << new OverlayUpdaterPrefWidget(this);

   // Connect button box signals to dialog slots
   connect(mUI.buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::accept);
   connect(mUI.buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);
   connect(mUI.buttonBox->button(QDialogButtonBox::RestoreDefaults),
           &QPushButton::clicked,
           this,
           &PreferencesDialog::HandleRestoreDefaultsAction);

   connect(mUI.treeWidget, &QTreeWidget::currentItemChanged, this, &PreferencesDialog::TreeWidgetCurrentItemChanged);
}

void wkf::PreferencesDialog::closeEvent(QCloseEvent* aEvent)
{
   reject();
}

void wkf::PreferencesDialog::showEvent(QShowEvent* aEvent)
{
   // Build up the pages for the Preference Dialog, that the Dialog is populated when shown
   ConstructPreferencesDialog();
}

void wkf::PreferencesDialog::ShowPreferencePage(const QString& aPage)
{
   // Build up the pages for the Preference Dialog, so mUI.treeWidget->findItems() can find the requested page
   ConstructPreferencesDialog();

   QList<QTreeWidgetItem*> items = mUI.treeWidget->findItems(aPage, Qt::MatchExactly | Qt::MatchRecursive, 0);
   if (!items.isEmpty())
   {
      show();
      mUI.treeWidget->setCurrentItem(items.front());
   }
}

void wkf::PreferencesDialog::HandleRestoreDefaultsAction()
{
   QMessageBox prompt(this);

   prompt.setWindowTitle("Restore Defaults");
   prompt.setText("Would you like to restore the defaults to all of the Preferences pages or only on the currently "
                  "displayed page?");
   prompt.setIcon(QMessageBox::Question);
   prompt.setStandardButtons(QMessageBox::Cancel | QMessageBox::RestoreDefaults | QMessageBox::Reset);
   prompt.setDefaultButton(QMessageBox::Cancel);

   prompt.button(QMessageBox::Reset)->setText("All");
   prompt.button(QMessageBox::RestoreDefaults)->setText("Current");

   const int userChoice = prompt.exec();
   switch (userChoice)
   {
   // Restore Defaults only to the current page.
   case QMessageBox::RestoreDefaults:
   {
      auto wid = mUI.stackedWidget->currentWidget();
      if (wid)
      {
         auto* pw = mUI.stackedWidget->findChild<PrefWidget*>(wid->objectName());
         if (pw)
         {
            pw->RestoreDefaults();
         }
      }
      break;
   }
   // Restore Defaults to all of the pages
   case QMessageBox::Reset:
   {
      for (auto pw : mUI.stackedWidget->findChildren<PrefWidget*>())
      {
         pw->RestoreDefaults();
      }

      break;
   }
   case QMessageBox::Cancel:
   default:
      break;
   }
}

void wkf::PreferencesDialog::TreeWidgetCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
   if (current != nullptr)
   {
      int index = current->data(0, Qt::UserRole).toInt();
      mUI.stackedWidget->setCurrentIndex(index);
   }
}

void wkf::PreferencesDialog::ConstructPreferencesDialog()
{
   if (!mInitialized)
   {
      mInitialized = true;

      QList<PrefWidget*> prefWidgets = mStandardWidgets;

      auto plugins = wkfEnv.GetPluginManager()->GetLoadedPlugins();
      for (auto& p : plugins)
      {
         prefWidgets.append(p.second->GetPreferencesWidgets());
      }

      AddPrefWidgets(prefWidgets);

      // Sort the plugins in tree widget alphabetically
      mUI.treeWidget->sortItems(0, Qt::AscendingOrder);

      // Set "General" item as the current item.
      for (int i = 0; i < mUI.treeWidget->topLevelItemCount(); ++i)
      {
         auto* categoryPtr = mUI.treeWidget->topLevelItem(i);
         if (categoryPtr->text(0) == "Application")
         {
            for (int index = 0; index < categoryPtr->childCount(); ++index)
            {
               auto* itemPtr = categoryPtr->child(index);
               if (itemPtr->text(0) == "General")
               {
                  mUI.treeWidget->setCurrentItem(itemPtr);
                  break;
               }
            }
            categoryPtr->setExpanded(true);
            break;
         }
      }

      mUI.treeWidget->resizeColumnToContents(0);

      // Connect signals to SavePreferences AFTER dialog is constructed to ensure correct order of slot execution
      connect(this, &QDialog::accepted, this, &PreferencesDialog::SavePreferences);
      connect(mUI.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferencesDialog::SavePreferences);
      connect(mUI.buttonBox->button(QDialogButtonBox::RestoreDefaults),
              &QPushButton::clicked,
              this,
              &PreferencesDialog::SavePreferences);
   }
}

void wkf::PreferencesDialog::SavePreferences()
{
   emit wkfEnv.GetMainWindow()->GetConfigurationObject()->SaveSettings(wkfEnv.GetUserSettingsFile(),
                                                                       wkfEnv.GetSharedSettingsFile());
   Observer::SaveSettings(wkfEnv.GetUserSettingsFile());
}

void wkf::PreferencesDialog::AddPrefWidgets(const QList<PrefWidget*>& aPrefWidgetList)
{
   auto SetScrollAreaInfo = [&](PrefWidget* aPrefWidget, QScrollArea* aScrollArea, QTreeWidgetItem* aTreeWidgetItem)
   {
      aScrollArea->setObjectName(aPrefWidget->objectName());
      aScrollArea->setWidget(aPrefWidget);
      aScrollArea->setWidgetResizable(true);
      aScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      aScrollArea->horizontalScrollBar()->setEnabled(false);
      aScrollArea->setMinimumWidth(aPrefWidget->minimumSizeHint().width());
      aTreeWidgetItem->setData(0, Qt::UserRole, mUI.stackedWidget->addWidget(aScrollArea));
      ConnectWidget(aPrefWidget);
   };

   for (const auto prefWidget : aPrefWidgetList)
   {
      QString name       = prefWidget->windowTitle();
      QString category   = prefWidget->GetCategoryHint();
      bool    isPriority = prefWidget->GetCategoryHintPriority();

      // If no category is provide, create a top-level item with the name
      if (category.isEmpty())
      {
         QTreeWidgetItem* topLevelItemPtr = new QTreeWidgetItem(QStringList(name));
         mUI.treeWidget->insertTopLevelItem(0, topLevelItemPtr);
         SetScrollAreaInfo(prefWidget, new QScrollArea(this), topLevelItemPtr);
      }
      else // Category is provided
      {
         bool newCategory = false;

         QTreeWidgetItem* categoryItemPtr = nullptr;
         // Check to see if an item with the specified category already exists
         for (int i = 0; i < mUI.treeWidget->topLevelItemCount(); ++i)
         {
            auto* topLevelItemPtr = mUI.treeWidget->topLevelItem(i);
            if (topLevelItemPtr->text(0) == category)
            {
               categoryItemPtr = topLevelItemPtr;
               break;
            }
         }
         // An item with the specified category was not found, so create it
         if (categoryItemPtr == nullptr)
         {
            newCategory     = true;
            categoryItemPtr = new QTreeWidgetItem(QStringList(category));
            mUI.treeWidget->insertTopLevelItem(0, categoryItemPtr);
         }

         // Create the entry for this pref widget with the category item as the parent
         QTreeWidgetItem* childItem = new QTreeWidgetItem(categoryItemPtr, QStringList(name));
         categoryItemPtr->addChild(childItem);
         SetScrollAreaInfo(prefWidget, new QScrollArea(this), childItem);

         // Sort the elements within the category, then take the first child and
         // make sure that widget is shown when the category item is selected.
         categoryItemPtr->sortChildren(0, Qt::AscendingOrder);

         // If the childItem/Pref Widget is marked as the priority Pref Widget, make the category Item
         // show the Pref Widget when category is selected
         if (isPriority)
         {
            categoryItemPtr->setData(0, Qt::UserRole, childItem->data(0, Qt::UserRole));
            // Mark that a priority Pref Widget was set.
            categoryItemPtr->setData(0, Qt::UserRole + 1, true);
         }
         // If this is a new Category and the childItem was not priority
         // OR
         // If a priority childItem has not been set
         else if (newCategory || !categoryItemPtr->data(0, Qt::UserRole + 1).toBool())
         {
            // we want the Category to show the first item in the list
            categoryItemPtr->setData(0, Qt::UserRole, categoryItemPtr->child(0)->data(0, Qt::UserRole));
         }
      }
   }
}

void wkf::PreferencesDialog::ConnectWidget(PrefWidget* aPrefWidgetPtr)
{
   connect(this, &QDialog::accepted, aPrefWidgetPtr, &PrefWidget::ApplyChanges);
   connect(mUI.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, aPrefWidgetPtr, &PrefWidget::ApplyChanges);
   connect(this, &QDialog::rejected, aPrefWidgetPtr, &PrefWidget::RejectChanges);
}
