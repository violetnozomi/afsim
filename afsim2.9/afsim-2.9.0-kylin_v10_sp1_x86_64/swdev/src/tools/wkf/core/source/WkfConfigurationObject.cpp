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

#include "WkfConfigurationObject.hpp"

#include <QAction>
#include <QActionGroup>
#include <QDialogButtonBox>
#include <QDir>
#include <QIcon>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfGeneralPrefObject.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp" // i would like to see this gone.

wkf::ConfigurationObject::ConfigurationObject(QObject* parent)
   : QObject(parent)
   , mLoadRecentMenu(nullptr)
{
   connect(wkfEnv.GetPreferenceObject<GeneralPrefObject>(),
           &GeneralPrefObject::NumRecentConfigsChanged,
           this,
           &ConfigurationObject::NumRecentConfigsChanged);
   connect(&wkfEnv, &Environment::Initialize, this, &ConfigurationObject::Initialize);
}

void wkf::ConfigurationObject::Initialize()
{
   LoadRecentsSettings();

   // Find the File Menu
   QMenu* fileMenu = wkfEnv.GetMainWindow()->GetMenuByName("File");
   if (fileMenu != nullptr)
   {
      Action* saveAction = new Action(QIcon::fromTheme("save_settings"), "Save Configuration...", wkfEnv.GetMainWindow());
      connect(saveAction, &QAction::triggered, this, &ConfigurationObject::SaveActionHandler);

      Action* loadAction = new Action(QIcon::fromTheme("load_settings"), "Load Configuration...", wkfEnv.GetMainWindow());
      connect(loadAction, &QAction::triggered, this, &ConfigurationObject::LoadActionHandler);

      Action* importAction = new Action("Import Configuration Options...", wkfEnv.GetMainWindow());
      connect(importAction, &QAction::triggered, this, &ConfigurationObject::ImportActionHandler);

      mLoadRecentMenu = new QMenu("Recent Configurations", fileMenu);
      PopulateMenu(mLoadRecentMenu);

      QList<QAction*> actions = fileMenu->actions();

      fileMenu->insertAction(actions.back(), saveAction);
      fileMenu->insertAction(actions.back(), loadAction);
      fileMenu->insertAction(actions.back(), importAction);
      fileMenu->insertMenu(actions.back(), mLoadRecentMenu);
      fileMenu->insertSeparator(actions.back());

      Action* clearPlatformSettings = new Action("Clear Platform Options", wkfEnv.GetMainWindow());
      connect(clearPlatformSettings, &QAction::triggered, this, &ConfigurationObject::ClearPlatformSettingsHandler);
      fileMenu->insertAction(actions.back(), clearPlatformSettings);
      fileMenu->insertSeparator(actions.back());
   }
}

void wkf::ConfigurationObject::LoadActionHandler()
{
   CheckForConfigDirectory();
   QString filePath =
      wkf::getOpenFileName(nullptr, "Load Configuration", Environment::GetInstance().GetConfigurationsDir(), "(*.ini)");

   if (filePath != "")
   {
      QFileInfo info(filePath);
      if (info.exists())
      {
         QString fileName = info.fileName();
         LoadConfig(fileName, filePath);
      }
      else
      {
         QMessageBox::warning(nullptr, "File Not Found", "The configuration file was not found.");
      }
   }
}

void wkf::ConfigurationObject::ImportActionHandler()
{
   CheckForConfigDirectory();
   QString filePath =
      wkf::getOpenFileName(nullptr, "Import From Configuration", Environment::GetInstance().GetConfigurationsDir(), "(*.ini)");

   if (!filePath.isEmpty())
   {
      QFileInfo info(filePath);
      if (info.exists())
      {
         const std::map<QString, PrefObject*>& prefList = wkfEnv.GetPrefObjectMap();

         QSettings settings(filePath, QSettings::IniFormat);

         QDialog selector;
         selector.setLayout(new QVBoxLayout(&selector));
         QPushButton* checkAll   = new QPushButton("Check All", &selector);
         QPushButton* uncheckAll = new QPushButton("Uncheck All", &selector);
         selector.layout()->addWidget(checkAll);
         selector.layout()->addWidget(uncheckAll);
         QListWidget* lw = new QListWidget(&selector);
         selector.layout()->addWidget(lw);
         connect(checkAll,
                 &QPushButton::clicked,
                 [lw]()
                 {
                    for (int i = 0; i < lw->count(); ++i)
                    {
                       lw->item(i)->setCheckState(Qt::Checked);
                    }
                 });
         connect(uncheckAll,
                 &QPushButton::clicked,
                 [lw]()
                 {
                    for (int i = 0; i < lw->count(); ++i)
                    {
                       lw->item(i)->setCheckState(Qt::Unchecked);
                    }
                 });
         QDialogButtonBox* bb =
            new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &selector);
         selector.layout()->addWidget(bb);
         connect(bb, &QDialogButtonBox::accepted, &selector, &QDialog::accept);
         connect(bb, &QDialogButtonBox::rejected, &selector, &QDialog::reject);
         for (auto&& it : prefList)
         {
            QListWidgetItem* item = new QListWidgetItem(it.first, lw);
            item->setCheckState(Qt::Unchecked);
         }
         if (selector.exec() == QDialog::Accepted)
         {
            QStringList filteredList;
            for (int i = 0; i < lw->count(); ++i)
            {
               if (lw->item(i)->checkState() == Qt::Checked)
               {
                  std::map<QString, PrefObject*>::const_iterator it = prefList.find(lw->item(i)->text());
                  it->second->LoadSettings(settings, settings);
               }
            }
         }
      }
      else
      {
         QMessageBox::warning(nullptr, "File not found", "The configuration file was not found.");
      }
   }
}

void wkf::ConfigurationObject::RecentActionHandler()
{
   QAction* action = qobject_cast<QAction*>(sender());
   if (action != nullptr)
   {
      QString filePath = action->data().toString();
      LoadConfig(action->text(), filePath);
   }
}

void wkf::ConfigurationObject::SaveActionHandler()
{
   CheckForConfigDirectory();
   QString filePath = wkf::getSaveFileName(nullptr,
                                           "Save Configuration",
                                           Environment::GetInstance().GetConfigurationsDir() + "//NewConfig.ini",
                                           "(*.ini)");
   if (filePath != "")
   {
      bool newFile = !QFile(filePath).exists();

      QSettings settings(filePath, QSettings::IniFormat);
      emit      SaveSettings(settings, settings);
      Observer::SaveSettings(settings);

      if (newFile)
      {
         // Add the new file to the map
         QString fileName = QFileInfo(filePath).fileName();
         if (fileName != "")
         {
            UpdateRecent(fileName, filePath);
         }
      }
   }
}

void wkf::ConfigurationObject::NumRecentConfigsChanged(int aSize)
{
   if (mLoadRecentMenu)
   {
      QList<QAction*> list = mLoadRecentMenu->actions();
      // if the size is lower than the current number of actions displayed
      while (list.size() > aSize)
      {
         mLoadRecentMenu->removeAction(list.last());
         delete list.takeLast();
      }

      // if the size requested is larger than the number of actions, see if we can expand the actions displayed
      int maxNumActionsToAdd = aSize - list.size();
      int numActionsHidden   = static_cast<int>(mRecentConfigurations.size() - list.size());
      int numActionsToAdd    = std::min(maxNumActionsToAdd, numActionsHidden);
      while (numActionsToAdd > 0)
      {
         // Add the actions stored in mRecentConfiguration but not shown in mLoadRecentMenu until the number of
         // actions are the equal to numActionsToAdd
         auto iter = mRecentConfigurations.begin();
         // Get the 1st element in mRecentConfigurations not shown in mLoadRecentMenu
         for (int i = 0; i < mLoadRecentMenu->actions().size(); ++i)
         {
            ++iter;
         }
         mLoadRecentMenu->addAction(CreateAction(iter->first, iter->second));
         --numActionsToAdd;
      }
   }
}

void wkf::ConfigurationObject::LoadRecentsSettings()
{
   QSettings& settings = wkfEnv.GetAppDataSettingsFile();
   settings.beginGroup("RecentConfigs");
   int size = settings.beginReadArray("array");
   for (int i = 0; i < size; i++)
   {
      settings.setArrayIndex(i);
      QString name = settings.value("name", "").toString();
      QString path = settings.value("path", true).toString();
      mRecentConfigurations.emplace_back(std::pair<QString, QString>(name, path));
   }
   settings.endArray();
   settings.endGroup();
}

void wkf::ConfigurationObject::SaveRecentsSettings()
{
   QSettings& settings = wkfEnv.GetAppDataSettingsFile();
   settings.beginGroup("RecentConfigs");
   int i = 0;
   settings.beginWriteArray("array");
   for (std::list<std::pair<QString, QString>>::const_iterator iter = mRecentConfigurations.begin();
        iter != mRecentConfigurations.end();
        ++iter)
   {
      settings.setArrayIndex(i);
      settings.setValue("name", iter->first);
      settings.setValue("path", iter->second);
      i++;
   }
   settings.endArray();
   settings.endGroup();
}

void wkf::ConfigurationObject::CheckForConfigDirectory()
{
   QDir dir(Environment::GetInstance().GetConfigurationsDir());
   if (!dir.exists())
   {
      dir.mkdir(Environment::GetInstance().GetConfigurationsDir());
   }
}

QAction* wkf::ConfigurationObject::CreateAction(const QString& aName, const QString& aPath)
{
   // Do not show the extension in the name of the action (user doesn't care that it is .ini)
   QString name  = aName;
   int     index = name.lastIndexOf(".");
   if (index != -1)
   {
      name.truncate(index);
   }

   // Create the new action
   auto* action = new QAction(name, mLoadRecentMenu);
   action->setData(aPath);
   connect(action, &QAction::triggered, this, &ConfigurationObject::RecentActionHandler);
   return action;
}

void wkf::ConfigurationObject::LoadConfig(const QString& aName, const QString& aPath)
{
   QSettings settings(aPath, QSettings::IniFormat);
   emit      LoadSettings(settings, settings);
   wkfEnv.LoadSettings(settings);
   UpdateRecent(aName, aPath);
}

void wkf::ConfigurationObject::UpdateRecent(const QString& aName, const QString& aPath)
{
   // check to make sure the file does not already exist
   for (const auto& i : mRecentConfigurations)
   {
      if (i.second == aPath)
      {
         mRecentConfigurations.remove(i);
         break;
      }
   }
   mRecentConfigurations.emplace_front(std::pair<QString, QString>(aName, aPath));

   // Check to make sure the action does not already exist for this file.
   QAction*        action     = nullptr;
   QList<QAction*> actionList = mLoadRecentMenu->actions();
   for (const auto& i : actionList)
   {
      if (i->data().toString() == aPath)
      {
         action = i;
         mLoadRecentMenu->removeAction(action);
      }
   }

   // if the action did not exist in the list already, create it
   if (action == nullptr)
   {
      action = CreateAction(aName, aPath);
   }

   actionList = mLoadRecentMenu->actions();
   // if adding an action to the menu would exceed the maximum recent configurations to display, remove one first
   if (mLoadRecentMenu->actions().size() == wkfEnv.GetPreferenceObject<GeneralPrefObject>()->GetNumberOfRecentConfigs())
   {
      mLoadRecentMenu->removeAction(actionList.last());
      delete actionList.takeLast();
   }

   // Add the new action to the beginning of the menu
   if (!actionList.empty())
   {
      mLoadRecentMenu->insertAction(*actionList.begin(), action);
   }
   else
   {
      mLoadRecentMenu->addAction(action);
   }
   // Save changes to the recent configurations file
   SaveRecentsSettings();
}

void wkf::ConfigurationObject::PopulateMenu(QMenu* aMenu)
{
   QAction* previousAction = nullptr;
   for (const auto& mRecentConfiguration : mRecentConfigurations)
   {
      QAction* action = CreateAction(mRecentConfiguration.first, mRecentConfiguration.second);
      if (previousAction != nullptr)
      {
         aMenu->insertAction(previousAction, action);
      }
      else
      {
         aMenu->addAction(action);
      }
      previousAction = action;
   }
   if (aMenu->isEmpty())
   {
      auto* action = aMenu->addAction("(no recent configurations)");
      action->setEnabled(false);
   }
}

void wkf::ConfigurationObject::RegisterPrefObject(PrefObject* aObjectPtr)
{
   connect(this, &ConfigurationObject::LoadSettings, aObjectPtr, &PrefObject::LoadSettings);
   connect(this, &ConfigurationObject::SaveSettings, aObjectPtr, &PrefObject::SaveSettings);
}

void wkf::ConfigurationObject::ClearPlatformSettingsHandler()
{
   emit wkfEnv.ClearPlatformSettings();
   emit wkfEnv.UpdatePlatformOptions();
}
