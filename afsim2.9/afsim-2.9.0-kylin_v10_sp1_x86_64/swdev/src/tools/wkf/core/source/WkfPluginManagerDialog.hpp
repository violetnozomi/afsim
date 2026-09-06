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
#ifndef WKFPLUGINMANAGERDIALOG_HPP
#define WKFPLUGINMANAGERDIALOG_HPP

class QListWidgetItem;
#include <map>
#include <memory>

#include "WkfDialog.hpp"
#include "ui_WkfPluginManagerDialog.h"

namespace wkf
{
class PluginManager;

class PluginManagerDialog : public Dialog
{
public:
   PluginManagerDialog(MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags());

private:
   void Initialize() override;

   void PluginListCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
   void PluginListOpenFolder();
   void AutoStartClicked(bool checked);
   void AutoStartAllClicked(bool checked);

   void closeEvent(QCloseEvent* event) override;
   void accept() override;
   void reject() override;

   Ui::WkfPluginManagerDialogUi mUi;
   PluginManager* const         mPluginManagerPtr;
   std::map<QString, bool>      mAutoStartChanges; // Plugin name -> auto start
};
} // namespace wkf
#endif
