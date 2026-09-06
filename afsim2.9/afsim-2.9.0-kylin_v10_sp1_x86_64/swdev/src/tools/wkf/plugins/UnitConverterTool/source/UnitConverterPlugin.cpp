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

#include "UnitConverterPlugin.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"


WKF_PLUGIN_DEFINE_SYMBOLS(UnitConverter::Plugin,
                          "Unit Converter Tool",
                          "The Unit Converter provides a dialog that can be accessed through the Tools menu to convert "
                          "between common units of measurement.",
                          "all")

UnitConverter::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mDialog                        = new Dialog(mainWindowPtr);

   QMenu* toolsMenuPtr = mainWindowPtr->GetMenuByName("Tools");
   if (toolsMenuPtr)
   {
      wkf::Action* unitActionPtr = new wkf::Action(QIcon::fromTheme("switch"), "Unit Converter Tool...", mainWindowPtr);
      connect(unitActionPtr, &QAction::triggered, mDialog.data(), &QDialog::show);
      toolsMenuPtr->addAction(unitActionPtr);
   }
}
