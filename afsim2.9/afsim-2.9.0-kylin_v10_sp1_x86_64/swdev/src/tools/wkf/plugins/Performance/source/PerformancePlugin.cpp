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
#include "PerformancePlugin.hpp"

#include <QMenu>

#include "PerformanceDialog.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(Performance::Plugin,
                          "Performance",
                          "The Performance plugin provides a dialog, accessible from the Developer menu, that records "
                          "memory usage information for the application.",
                          "all")

Performance::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mDialog                        = new Dialog(mainWindowPtr);

   QMenu* devMenuPtr = mainWindowPtr->GetMenuByName("Developer");
   if (devMenuPtr)
   {
      wkf::Action* dlgActionPtr =
         new wkf::Action(QIcon::fromTheme("performance"), "Performance Analyzer...", mainWindowPtr);
      connect(dlgActionPtr, &QAction::triggered, mDialog.data(), &QDialog::show);
      devMenuPtr->addAction(dlgActionPtr);
   }
}

void Performance::Plugin::GuiUpdate()
{
   mDialog->Update();
}
