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
#include "WkfPluginManagerDialog.hpp"

#include <QDesktopServices>
#include <QIcon>

#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPluginManager.hpp"

wkf::PluginManagerDialog::PluginManagerDialog(MainWindow* parent, Qt::WindowFlags f /* = Qt::WindowFlags() */)
   : Dialog(parent, new Action(QIcon::fromTheme("plugin_disconnected"), "Plugin Manager...", nullptr), "Options", f)
   , mPluginManagerPtr(wkfEnv.GetPluginManager())
{
   mUi.setupUi(this);

   QAction* openFolderActionPtr = new QAction("Open Containing Folder", this);
   mUi.pluginList->addAction(openFolderActionPtr);
   mUi.pluginList->setContextMenuPolicy(Qt::ActionsContextMenu);

   connect(mUi.pluginList, &QListWidget::currentItemChanged, this, &PluginManagerDialog::PluginListCurrentItemChanged);
   connect(mUi.autoStartCheckBox, &QCheckBox::clicked, this, &PluginManagerDialog::AutoStartClicked);
   connect(mUi.autoStartAllCheckBox, &QCheckBox::clicked, this, &PluginManagerDialog::AutoStartAllClicked);
   connect(mUi.openPluginsFolderButton,
           &QPushButton::clicked,
           this,
           [this]()
           {
              for (const auto& dir : mPluginManagerPtr->GetPluginDirectories())
              {
                 QDesktopServices::openUrl(QUrl::fromLocalFile(dir.c_str()));
              }
           });
   connect(openFolderActionPtr, &QAction::triggered, this, &PluginManagerDialog::PluginListOpenFolder);

   // Hide the unload button until unloading plugins is implemented
   mUi.unloadPushButton->hide();
}

void wkf::PluginManagerDialog::Initialize()
{
   Dialog::Initialize();

   // Counts for Auto Start
   int autoStartCount = 0;

   auto& plugins = mPluginManagerPtr->GetPluginFileMap();
   for (auto& p : plugins)
   {
      QString           filename(p.first.c_str());
      const PluginData& pluginData = mPluginManagerPtr->GetPluginData(p.second);

      QListWidgetItem* itemPtr = new QListWidgetItem;
      itemPtr->setData(PLUGIN_ID_ROLE, static_cast<unsigned int>(p.second));
      itemPtr->setData(FILEPATH_ROLE, QString::fromStdString(pluginData.mFilePath));

      // if plugin registered successfully display plugin name, else fall back to use file name
      QString pluginName = !pluginData.mRegistration.mName.empty() ? pluginData.mRegistration.mName.c_str() :
                                                                     QString("Unknown <%1>").arg(filename);
      itemPtr->setText(pluginName);

      QString iconStr;
      QString tooltip(filename);
      switch (pluginData.mLoadStatus)
      {
      case PluginManager::cLOAD_SUCCESS:
         iconStr = "plugin_connected";
         break;
      case PluginManager::cLOAD_FAIL:
         iconStr = "alert_red";
         tooltip.append(QString(" failed to load\n%1").arg(pluginData.mStatusReason.c_str()));
         break;
      case PluginManager::cLOAD_IGNORE:
         iconStr = "alert_orange";
         tooltip.append(QString(" incompatible\n%1").arg(pluginData.mStatusReason.c_str()));
         break;
      case PluginManager::cLOAD_DEFER:
         iconStr = "plugin_disconnected";
         tooltip.append(QString(" \n%1").arg(pluginData.mStatusReason.c_str()));
         break;
      }

      itemPtr->setIcon(QIcon::fromTheme(iconStr));
      itemPtr->setToolTip(tooltip);
      mUi.pluginList->addItem(itemPtr);

      if (mPluginManagerPtr->IsAutoStart(pluginName))
      {
         ++autoStartCount;
      }
   }

   if (autoStartCount == mUi.pluginList->count())
   {
      mUi.autoStartAllCheckBox->setCheckState(Qt::Checked);
   }
   else if (autoStartCount == 0)
   {
      mUi.autoStartAllCheckBox->setCheckState(Qt::Unchecked);
   }
   else
   {
      mUi.autoStartAllCheckBox->setCheckState(Qt::PartiallyChecked);
   }

   mUi.pluginList->sortItems();
}

void wkf::PluginManagerDialog::PluginListCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
   size_t      uid         = static_cast<size_t>(current->data(PLUGIN_ID_ROLE).toString().toUInt());
   QString     pluginName  = current->text();
   std::string description = mPluginManagerPtr->GetDescription(uid);
   mUi.textEdit->setText(description.c_str());

   bool autoStart;
   auto it = mAutoStartChanges.find(pluginName);
   if (it != mAutoStartChanges.end())
   {
      autoStart = it->second;
   }
   else
   {
      autoStart = mPluginManagerPtr->IsAutoStart(pluginName);
   }
   mUi.autoStartCheckBox->setChecked(autoStart);
}

void wkf::PluginManagerDialog::PluginListOpenFolder()
{
   auto* itemPtr = mUi.pluginList->currentItem();
   if (itemPtr)
   {
      QFileInfo fi(itemPtr->data(FILEPATH_ROLE).toString());
      QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
   }
}

void wkf::PluginManagerDialog::AutoStartClicked(bool checked)
{
   mAutoStartChanges[mUi.pluginList->currentItem()->text()] = checked;
   mUi.autoStartAllCheckBox->setCheckState(Qt::PartiallyChecked);
}

void wkf::PluginManagerDialog::AutoStartAllClicked(bool checked)
{
   // setCheckState(Qt::PartiallyChecked) implicitly makes Tristate == true, which allows the user to toggle
   // through all three states including PartiallyChecked. PartiallyChecked has no meaning if the user toggles
   // it to PartiallyChecked, thus prevent the user from entering this state by disabling tristate.
   mUi.autoStartAllCheckBox->setTristate(false);
   for (int i = 0; i < mUi.pluginList->count(); ++i)
   {
      mAutoStartChanges[mUi.pluginList->item(i)->text()] = checked;
   }
   mUi.autoStartCheckBox->setChecked(checked);
}

void wkf::PluginManagerDialog::closeEvent(QCloseEvent* event)
{
   reject();
}

void wkf::PluginManagerDialog::accept()
{
   for (auto& change : mAutoStartChanges)
   {
      mPluginManagerPtr->SetAutoStart(change.first, change.second);
   }

   mAutoStartChanges.clear();
   QDialog::accept();
}

void wkf::PluginManagerDialog::reject()
{
   QListWidgetItem* item = mUi.pluginList->currentItem();
   if (item)
   {
      // Reset state of auto-start check box to be consistent with state in plugin manager
      mUi.autoStartCheckBox->setChecked(mPluginManagerPtr->IsAutoStart(item->text()));
   }
   mAutoStartChanges.clear();
   QDialog::reject();
}
