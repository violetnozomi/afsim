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

#include "WkfPermissionDialog.hpp"

#include <QPushButton>

#include "UtQtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfConfigurationObject.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPermissionGuiFactory.hpp"
#include "WkfPermissionManager.hpp"

wkf::permission::PermissionDialog::PermissionDialog(MainWindow* aParent, Qt::WindowFlags aFlags /* = Qt::WindowFlags()*/)
   : Dialog(aParent, ut::qt::make_qt_ptr<Action>(QIcon::fromTheme("gears"), "Permissions...", nullptr), "Options", aFlags)
{
   mUi.setupUi(this);

   // Connect button box signals to dialog slots
   connect(mUi.buttonBox, &QDialogButtonBox::accepted, this, &PermissionDialog::accept);
   connect(mUi.buttonBox, &QDialogButtonBox::rejected, this, &PermissionDialog::reject);
   connect(this, &QDialog::accepted, this, &PermissionDialog::SavePermissions);

   connect(mUi.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PermissionDialog::SavePermissions);
   connect(mUi.restoreButton, &QPushButton::clicked, this, &PermissionDialog::HandleRestoreDefaultsAction);
   connect(mUi.saveButton, &QPushButton::clicked, this, &PermissionDialog::HandleSaveAction);

   connect(mUi.treeWidget, &QTreeWidget::currentItemChanged, this, &PermissionDialog::TreeWidgetCurrentItemChanged);
}

wkf::permission::GuiFactory& wkf::permission::PermissionDialog::GetGuiFactory() noexcept
{
   return mFactory;
}

void wkf::permission::PermissionDialog::Refresh()
{
   ClearDialog();

   auto& manager = wkfEnv.GetPermissions();
   manager.ReadFile(wkfEnv.GetPermissionFile());

   auto names = manager.GetCategories();
   for (const auto& name : names)
   {
      auto& category = *manager.GetCategory(name);
      auto* widget   = mFactory.MakeCategoryPage(category);
      int   index    = mUi.stackedWidget->addWidget(widget);

      auto item = ut::make_unique<QTreeWidgetItem>();
      item->setText(0, name);
      item->setData(0, Qt::UserRole, index);
      mUi.treeWidget->addTopLevelItem(item.release());
   }

   mUi.treeWidget->setSortingEnabled(true);
   mUi.treeWidget->sortByColumn(0);
   mUi.treeWidget->resizeColumnToContents(0);

   mUi.treeWidget->setItemSelected(mUi.treeWidget->topLevelItem(0), true);
}

void wkf::permission::PermissionDialog::LockPermissions()
{
   mUi.restoreButton->setEnabled(false);
   mUi.stackedWidget->setEnabled(false);
   mUi.saveButton->setEnabled(false); // Need to set this to per Note in HandleSaveAction() about lock file loading
}

void wkf::permission::PermissionDialog::SetActionVisible(bool aVisible)
{
   mTriggerActionPtr->setVisible(aVisible);
}

void wkf::permission::PermissionDialog::HandleRestoreDefaultsAction()
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
      auto page = dynamic_cast<CategoryPage*>(mUi.stackedWidget->currentWidget());
      if (page)
      {
         emit page->RestoreDefaults();
      }

      wkfEnv.GetPermissions().WriteFile(wkfEnv.GetPermissionFile());

      break;
   }
      // Restore Defaults to all of the pages
   case QMessageBox::Reset:
   {
      for (auto page : mUi.stackedWidget->findChildren<CategoryPage*>())
      {
         emit page->RestoreDefaults();
      }

      wkfEnv.GetPermissions().WriteFile(wkfEnv.GetPermissionFile());

      break;
   }
   case QMessageBox::Cancel:
   default:
      break;
   }
}

void wkf::permission::PermissionDialog::HandleSaveAction()
{
   SavePermissions();

   // Note: This can not use the wkf::getSaveFileName because you may be trying to save the Permission that controls the
   // "Lock File Loading".
   //       If so, you would be unable to save the file.
   //       Instead we make sure this button is disabled entirely if Permissions are set.
   QString filename = QFileDialog::getSaveFileName(nullptr,
                                                   "Save Permissions",
                                                   wkfEnv.GetConfigurationsDir() + "//NewPermissions.ini",
                                                   "(*.ini)");
   if (!filename.isEmpty())
   {
      QSettings settings{filename, QSettings::IniFormat};

      wkfEnv.GetPermissions().WriteFile(settings);
   }
}

void wkf::permission::PermissionDialog::closeEvent(QCloseEvent* aEvent)
{
   reject();
}

void wkf::permission::PermissionDialog::showEvent(QShowEvent* aEvent)
{
   // Build up the pages for the Preference Dialog, that the Dialog is populated when shown
   Refresh();
}

void wkf::permission::PermissionDialog::TreeWidgetCurrentItemChanged(QTreeWidgetItem* aCurrent, QTreeWidgetItem* aPrevious)
{
   if (aCurrent != nullptr)
   {
      int index = aCurrent->data(0, Qt::UserRole).toInt();
      mUi.stackedWidget->setCurrentIndex(index);
   }
}

void wkf::permission::PermissionDialog::SavePermissions()
{
   for (auto* page : mUi.stackedWidget->findChildren<CategoryPage*>())
   {
      emit page->ApplyChanges();
   }

   wkfEnv.GetPermissions().WriteFile(wkfEnv.GetPermissionFile());
}

void wkf::permission::PermissionDialog::ClearDialog()
{
   mUi.treeWidget->clear();

   while (mUi.stackedWidget->count() > 0)
   {
      QWidget* widget = mUi.stackedWidget->widget(0);
      mUi.stackedWidget->removeWidget(widget);
      widget->deleteLater();
   }
}
