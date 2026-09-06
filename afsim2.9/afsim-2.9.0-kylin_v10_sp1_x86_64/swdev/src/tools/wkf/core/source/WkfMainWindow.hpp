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
#ifndef WKFMAINWINDOW_HPP
#define WKFMAINWINDOW_HPP

#include <queue>

#include <QMainWindow>
class QMenu;
class QSettings;
#include "wkf_export.h"

#include "VaCallbackHolder.hpp"
#include "ui_WkfMainWindow.h"

namespace wkf
{
class ConfigurationObject;
namespace permission
{
class PermissionDialog;
}

class WKF_EXPORT MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow() override = default;

   // Separated this function from the constructor so that dialog created by the MainWindow can use
   // wkfEnv.GetMainWindow() to get a valid pointer to the constructed MainWindow class for use (ie: Qt::connect())
   void InitializeDialogs();

   // Returns the menu from the menu bar that matches aNewMenuName, if it exists.
   // Otherwise it will create a menu with aNewMenuName as the name and return the new menu.
   // The newly created menu will be inserted on the menuBar prior to the menu named aInsertPriorTo if it exists,
   // otherwise it will be inserted prior to the "Help" menu.
   QMenu* FindOrCreateMenu(const QString& aNewMenuName, const QString& aInsertPriorTo = "");
   // Get a menu from the menu bar that match the string. Null if it doesn't exist.
   QMenu* GetMenuByName(const QString& aName) const;

   // Adds an action that will show the QDialog to the Tool menu.
   // This is a convenience method but does the same thing as accessing the Tool menu with GetMenuByName
   // then adding a toggleAction for the QDialog
   void AddDialogToToolMenu(QDialog* aDialogPtr, const QString& aActionName = "");
   // Adds an action that will show the QDockWidget to the View menu.
   // This is a convenience method but does the same thing as accessing the View menu with GetMenuByName
   // then adding the toggleAction for the QDockWidget
   // providing an optional aSubMenu will add a sub-menu (as necessary) and add the display action to that
   // Note: addDockWidget() & AddDockWidget() will call this function, thus it is rarely necessary to call this directly
   WKF_DEPRECATED void AddDialogToViewMenu(QDockWidget* aDockWidgetPtr, const QString& aSubMenu = "");
   void                AddDockWidgetToViewMenu(QDockWidget* aDockWidgetPtr, const QString& aSubMenu = "");
   // Adds an action to the View menu.
   void AddActionToViewMenu(QAction* aActionPtr, const QString& aSubMenu = "");
   // Retrieve a sub-menu from the View menu or create it if it does not exist.
   QMenu* FindOrCreateViewSubMenu(const QString& aMenuName);

   // Adds a QToolBar to the main window.  This also adds the toggle action for the QToolBar to the Toolbar sub-menu of the View Menu.
   void AddToolBar(QToolBar* aToolBarPtr, Qt::ToolBarArea aToolBarArea = Qt::TopToolBarArea);

   // QMainWindow::addDockWidget is not a virtual function, so we have to use function hiding
   void addDockWidget(Qt::DockWidgetArea aArea, QDockWidget* aDockwidgetPtr);
   // AddDockWidget() is a helper function that combines addDockWidget() and AddDialogToViewMenu() functionality
   void AddDockWidget(Qt::DockWidgetArea aArea, QDockWidget* aDockwidgetPtr, const QString& aViewMenuSubMenuName = "");

   void ShowStatusMessage(const QString& aMessage, int aTimeout = 0);

   QMainWindow* centralWidget() const { return mCentralDocker; }

   void ShowBannerText(const QString& aText,
                       const QColor&  aTextColor       = QColor(),
                       const QColor&  aBackgroundColor = QColor(0, 255, 0, 255),
                       double         aFontSize        = 12);
   void HideBannerText();

   ConfigurationObject* GetConfigurationObject() const { return mConfigurationObjectPtr; }

   // adds an application specific banner that is displayed below the generic wkf application banner that user's can
   // specify in Preferences.
   void AddCustomBanner(QWidget* aWidgetPtr);

   wkf::permission::PermissionDialog* GetPermissionDialog() const { return mPermissionDialog; }

signals:
   void MainWindowClose();

   void ShowPreferencePage(const QString& aPage);

protected:
   void closeEvent(QCloseEvent* aEventPtr) override;

   // Move setCentralWidget to protected instead of public, so that plugins can not change the wkf::CentralDisplayWidget
   void setCentralWidget(QWidget* aWidgetPtr) { QMainWindow::setCentralWidget(aWidgetPtr); }

private:
   void LoadSettings(QSettings& aSettings);
   void SaveSettings(QSettings& aSettings);

   void StatusMessageChanged(const QString& aMessage);

   vespa::VaCallbackHolder             mCallbacks;
   Ui::WkfMainWindowUi                 mUi;
   bool                                mStatusMessageIndefinite;
   std::queue<std::pair<QString, int>> mStatusMessageQueue;
   ConfigurationObject*                mConfigurationObjectPtr;
   permission::PermissionDialog*       mPermissionDialog = nullptr;
   QMainWindow*                        mCentralDocker;
};
} // namespace wkf
#endif
