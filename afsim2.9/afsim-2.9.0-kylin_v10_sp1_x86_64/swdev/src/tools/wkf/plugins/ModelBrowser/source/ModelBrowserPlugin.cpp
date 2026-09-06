// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "ModelBrowserPlugin.hpp"

#include <QMenu>
#include <QString>
#include <osg/Node>

#include "ModelBrowserWidget.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   ModelBrowser::Plugin,
   "Model Browser",
   "The Model Browser plugin adds an option to the Tools menu to open a dockable window to view and modify models.",
   "all")

ModelBrowser::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
   , mDockWidget(nullptr)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();

   QMenu* toolsMenuPtr = mainWindowPtr->GetMenuByName("Tools");
   if (toolsMenuPtr)
   {
      wkf::Action* dlgActionPtr = new wkf::Action(QIcon::fromTheme("airplane"), "Model Viewer...", mainWindowPtr);
      connect(dlgActionPtr, &QAction::triggered, this, &Plugin::Launch);
      toolsMenuPtr->addAction(dlgActionPtr);
   }
   QMenu* devMenuPtr = mainWindowPtr->GetMenuByName("Developer");
   if (devMenuPtr)
   {
      wkf::Action* reload = new wkf::Action(QIcon::fromTheme(""), "Reload Model Database", mainWindowPtr);
      connect(reload, &QAction::triggered, this, &Plugin::Reload);
      devMenuPtr->addAction(reload);
   }
}

QList<wkf::Tip> ModelBrowser::Plugin::GetTips() const
{
   QList<wkf::Tip> tips;
   tips.append(wkf::Tip("Model Viewer",
                        "You can view existing platform models and load in additional platform models using the Model "
                        "Browser located in the Tools menu.",
                        "wkf_plugin/" + wkfEnv.GetApplicationPrefix() + "_model_viewer.html",
                        "Model Viewer"));
   return tips;
}

void ModelBrowser::Plugin::Launch()
{
   if (!mDockWidget)
   {
      mDockWidget = new wkf::DockWidget("model viewer", wkfEnv.GetMainWindow()->centralWidget(), Qt::WindowFlags());
      mDockWidget->setWidget(new Widget(mDockWidget));
      mDockWidget->setWindowTitle("ModelViewer");
   }
   mDockWidget->show();
}

void ModelBrowser::Plugin::Reload()
{
   vaEnv.GetModelDatabase()->Reset();
   wkfEnv.LoadModelDatabase();
}
