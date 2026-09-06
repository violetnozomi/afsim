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
#include "WkfMainWindow.hpp"

#include <functional>

#include <QTimer>
#include <QToolBar>

#include "WkfAboutDialog.hpp"
#include "WkfConfigurationObject.hpp"
#include "WkfDebugPrefObject.hpp"
#include "WkfDocumentationDialog.hpp"
#include "WkfEnvironment.hpp"
#include "WkfFactory.hpp"
#include "WkfObserver.hpp"
#include "WkfPermissionDialog.hpp"
#include "WkfPlatformOptionsDockWidget.hpp"
#include "WkfPluginManagerDialog.hpp"
#include "WkfPreferencesDialog.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::MainWindow::MainWindow()
   : QMainWindow()
   , mStatusMessageIndefinite(false)
   , mConfigurationObjectPtr(nullptr)
{
   mUi.setupUi(this);
   mUi.mActionExit->setShortcut(QKeySequence("Alt+F4"));

   mCallbacks.Add(Observer::LoadSettings.Connect(&MainWindow::LoadSettings, this));
   mCallbacks.Add(Observer::SaveSettings.Connect(&MainWindow::SaveSettings, this));

   connect(statusBar(), &QStatusBar::messageChanged, this, &MainWindow::StatusMessageChanged);

   // initially hide the Tools and Developer menus
   QMenu* toolsMenu = GetMenuByName("Tools");
   if (toolsMenu != nullptr)
   {
      toolsMenu->menuAction()->setVisible(false);
   }
   QMenu* devMenu = GetMenuByName("Developer");
   if (devMenu != nullptr)
   {
      devMenu->menuAction()->setVisible(false);
   }

   setDockNestingEnabled(true);
   setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
   setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
   setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
   setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

   mCentralDocker = new QMainWindow(this);
   mUi.centralwidget->layout()->addWidget(mCentralDocker);
   mCentralDocker->setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
}

void wkf::MainWindow::InitializeDialogs()
{
   // Create child dialogs and objects
   vaEnv.GetFactory()->CreateDialog("About", this);
   vaEnv.GetFactory()->CreateDialog("Documentation", this);

   PreferencesDialog* prefDialog = dynamic_cast<PreferencesDialog*>(vaEnv.GetFactory()->CreateDialog("Preferences", this));
   connect(this, &MainWindow::ShowPreferencePage, prefDialog, &PreferencesDialog::ShowPreferencePage);

   mPermissionDialog =
      dynamic_cast<wkf::permission::PermissionDialog*>(vaEnv.GetFactory()->CreateDialog("Permission", this));

   QMenu* devmenu = GetMenuByName("Developer");
   if (devmenu)
   {
      devmenu->setTearOffEnabled(true);
      QAction* devAction = devmenu->menuAction();
      // configure the developer menu to only show when requested by the pref object
      connect(wkfEnv.GetPreferenceObject<wkf::DebugPrefObject>(),
              &wkf::DebugPrefObject::DeveloperMenuVisibilityChanged,
              [devAction](bool aState) { devAction->setVisible(aState); });
   }

   new PluginManagerDialog(this);
   mConfigurationObjectPtr = new ConfigurationObject(this);

   // Create the context data dock widget
   auto* platformDataDock = vaEnv.GetFactory()->CreateDockWidget("PlatformData", this);
   if (platformDataDock)
   {
      addDockWidget(Qt::LeftDockWidgetArea, platformDataDock);

      auto* optDock = new PlatformOptionsDockWidget(this);
      addDockWidget(Qt::LeftDockWidgetArea, optDock);

      tabifyDockWidget(platformDataDock, optDock);
      platformDataDock->raise();
   }
   auto* mapLayerDock = vaEnv.GetFactory()->CreateDockWidget("MapLayer", this);
   if (mapLayerDock)
   {
      addDockWidget(Qt::RightDockWidgetArea, mapLayerDock);
   }
}

QMenu* wkf::MainWindow::FindOrCreateMenu(const QString& aNewMenuName, const QString& aInsertPriorTo)
{
   QMenu* menu = wkfEnv.GetMainWindow()->GetMenuByName(aNewMenuName);
   if (!menu)
   {
      menu = new QMenu(aNewMenuName, menuBar());

      // If the user specified a menu to insert this one before, do so (as long as it exists)
      QMenu* insertPriorToMenu = GetMenuByName(aInsertPriorTo);
      if (insertPriorToMenu)
      {
         menuBar()->insertMenu(insertPriorToMenu->menuAction(), menu);
      }
      else
      {
         // Insert new menus prior to the Help menu (we want the Help menu to always be last)
         QMenu* helpMenu = GetMenuByName("Help");
         if (helpMenu != nullptr)
         {
            menuBar()->insertMenu(helpMenu->menuAction(), menu);
         }
         else
         {
            menuBar()->addMenu(menu);
         }
      }
   }
   return menu;
}

QMenu* wkf::MainWindow::GetMenuByName(const QString& aName) const
{
   // If aName is empty, it will still match to one of the menuBar's children
   //  We do not want to return an unnamed menu, menus should have names if you want to access them.
   if (!aName.isEmpty())
   {
      QList<QMenu*> menus = menuBar()->findChildren<QMenu*>();
      foreach (QMenu* m, menus)
      {
         if (m->title() == aName)
         {
            // if this is the Tools menu, set it to visible
            if (aName == "Tools")
            {
               m->menuAction()->setVisible(true);
            }
            return m;
         }
      }
   }
   return nullptr;
}

void wkf::MainWindow::AddDialogToToolMenu(QDialog* aDialogPtr, const QString& aActionName)
{
   QString name = aActionName;
   if (name == "")
   {
      name = aDialogPtr->objectName();
   }

   auto* action = new QAction(name, mUi.mMenuTools);
   connect(action, &QAction::triggered, this, [=]() { aDialogPtr->show(); });
   mUi.mMenuTools->addAction(action);
}

WKF_DEPRECATED void wkf::MainWindow::AddDialogToViewMenu(QDockWidget* aDockWidgetPtr, const QString& aSubMenu)
{
   AddDockWidgetToViewMenu(aDockWidgetPtr, aSubMenu);
}

void wkf::MainWindow::AddDockWidgetToViewMenu(QDockWidget* aDockWidgetPtr, const QString& aSubMenu)
{
   AddActionToViewMenu(aDockWidgetPtr->toggleViewAction(), aSubMenu);
}

void wkf::MainWindow::AddActionToViewMenu(QAction* aActionPtr, const QString& aSubMenu)
{
   // Add the dock widget to the View Menu
   QMenu* menu = FindOrCreateMenu("View");
   if (menu)
   {
      if (!aSubMenu.isEmpty())
      {
         menu = wkf::MainWindow::FindOrCreateViewSubMenu(aSubMenu);
      }

      auto actionList = menu->actions();
      bool inserted   = false;
      if (!actionList.empty())
      {
         // Insert the action in the View Menu after all Sub-Menus, and alphabetically with the rest of the actions
         for (auto&& it : actionList)
         {
            if (!it->menu() &&                   // If the action is not a sub-menu
                aActionPtr->text() < it->text()) // Check alphabetical order
            {
               menu->insertAction(it, aActionPtr);
               inserted = true;
               break;
            }
         }
      }

      if (!inserted)
      {
         menu->addAction(aActionPtr);
      }
   }
}

// The View Menu has ownership of the QMenu pointer returned by this function
QMenu* wkf::MainWindow::FindOrCreateViewSubMenu(const QString& aMenuName)
{
   QMenu* viewMenu = GetMenuByName("View");
   if (viewMenu)
   {
      // If the requested menu already exists, return it.
      auto* subMenu = findChild<QMenu*>(aMenuName);
      if (subMenu)
      {
         return subMenu;
      }

      // Find where to insert the requested menu
      for (auto* it : viewMenu->actions())
      {
         // If we reached the end of the sub-menus, insert this sub-menu
         // Otherwise, check alphabetical order to determine if we should insert it
         if (!it->menu() || aMenuName < it->text())
         {
            auto* menu = new QMenu(aMenuName, viewMenu);
            menu->setObjectName(aMenuName);
            viewMenu->insertMenu(it, menu);
            return menu;
         }
      }

      // If we have reached the end of the list, go ahead and Add the requested menu
      auto* menu = viewMenu->addMenu(aMenuName);
      menu->setObjectName(aMenuName);
      return menu;
   }
   return nullptr;
}

void wkf::MainWindow::AddToolBar(QToolBar* aToolBarPtr, Qt::ToolBarArea aToolBarArea)
{
   addToolBar(aToolBarArea, aToolBarPtr);
   mUi.mMenuToolbars->addAction(aToolBarPtr->toggleViewAction());
}

void wkf::MainWindow::addDockWidget(Qt::DockWidgetArea aArea, QDockWidget* aDockwidget)
{
   AddDockWidget(aArea, aDockwidget, "");
}

void wkf::MainWindow::AddDockWidget(Qt::DockWidgetArea aArea, QDockWidget* aDockWidgetPtr, const QString& aViewMenuSubMenuName)
{
   QMainWindow::addDockWidget(aArea, aDockWidgetPtr);
   AddDockWidgetToViewMenu(aDockWidgetPtr, aViewMenuSubMenuName);
}

void wkf::MainWindow::ShowStatusMessage(const QString& aMessage, int aTimeout)
{
   if (statusBar()->currentMessage().isEmpty() || mStatusMessageIndefinite)
   {
      statusBar()->showMessage(aMessage, aTimeout);
      mStatusMessageIndefinite = (aTimeout == 0);
   }
   else
   {
      mStatusMessageQueue.push(std::pair<QString, int>(aMessage, aTimeout));
   }
}

void wkf::MainWindow::ShowBannerText(const QString& aText,
                                     const QColor&  aTextColor,
                                     const QColor&  aBackgroundColor,
                                     double         aFontSize)
{
   if (!aText.isEmpty())
   {
      // The style sheet must be set prior to the font size being set so that the correct font size is used when the
      // application is initialized launched.
      mUi.bannerLabel->setStyleSheet("QLabel { background-color : " + aBackgroundColor.name() +
                                     "; color : " + aTextColor.name() + "; }");

      QFont font = mUi.bannerLabel->font();
      font.setPointSize(aFontSize);
      mUi.bannerLabel->setFont(font);

      mUi.bannerLabel->setAutoFillBackground(true);
      mUi.bannerLabel->setText(aText);
      mUi.bannerLabel->show();
   }
   else
   {
      HideBannerText();
   }
}

void wkf::MainWindow::HideBannerText()
{
   mUi.bannerLabel->hide();
}

void wkf::MainWindow::AddCustomBanner(QWidget* aWidgetPtr)
{
   mUi.verticalLayout->insertWidget(1, aWidgetPtr);
}

void wkf::MainWindow::closeEvent(QCloseEvent* aEventPtr)
{
   emit MainWindowClose();
   emit GetConfigurationObject()->SaveSettings(wkfEnv.GetUserSettingsFile(), wkfEnv.GetSharedSettingsFile());
   Observer::SaveSettings(wkfEnv.GetUserSettingsFile());
   QMainWindow::closeEvent(aEventPtr);
}

void wkf::MainWindow::LoadSettings(QSettings& aSettings)
{
   aSettings.beginGroup("MainWindow");
   restoreGeometry(aSettings.value("geometry").toByteArray());
   restoreState(aSettings.value("state").toByteArray());
   aSettings.beginGroup("CentralWidget");
   mCentralDocker->restoreGeometry(aSettings.value("geometry").toByteArray());
   mCentralDocker->restoreState(aSettings.value("state").toByteArray());
   aSettings.endGroup();
   aSettings.endGroup();
}

void wkf::MainWindow::SaveSettings(QSettings& aSettings)
{
   aSettings.beginGroup("MainWindow");
   aSettings.setValue("geometry", saveGeometry());
   aSettings.setValue("state", saveState());
   aSettings.beginGroup("CentralWidget");
   aSettings.setValue("geometry", mCentralDocker->saveGeometry());
   aSettings.setValue("state", mCentralDocker->saveState());
   aSettings.endGroup();
   aSettings.endGroup();
}

void wkf::MainWindow::StatusMessageChanged(const QString& aMessage)
{
   if ((aMessage.isEmpty()) && (!mStatusMessageQueue.empty()))
   {
      std::pair<QString, int> newMessage = mStatusMessageQueue.front();
      mStatusMessageQueue.pop();
      if ((newMessage.second == 0) && (!mStatusMessageQueue.empty()))
      {
         newMessage.second = 1500;
      }
      statusBar()->showMessage(newMessage.first, newMessage.second);
   }
}
