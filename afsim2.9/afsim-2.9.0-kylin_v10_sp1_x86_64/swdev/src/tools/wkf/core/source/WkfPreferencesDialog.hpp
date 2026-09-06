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

#ifndef WKFPREFERENCESDIALOG_HPP
#define WKFPREFERENCESDIALOG_HPP

class QCloseEvent;
class QShowEvent;
class QTreeWidgetItem;

#include "wkf_export.h"

#include "WkfDialog.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfPreferencesDialog.h"

namespace wkf
{
class WKF_EXPORT PreferencesDialog : public Dialog
{
public:
   PreferencesDialog(MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags());

   // Will set the current page of the Preferences Dialog to the page that matches aPage.
   //  If a page with the name aPage is found, the page will displayed.
   void ShowPreferencePage(const QString& aPage);

protected:
   void HandleRestoreDefaultsAction();

   void TreeWidgetCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
   void SavePreferences();

   void closeEvent(QCloseEvent* aEvent) override;
   void showEvent(QShowEvent* aEvent) override;

   void ConstructPreferencesDialog();
   void AddPrefWidgets(const QList<PrefWidget*>& aPrefWidgetList);
   void ConnectWidget(PrefWidget* aPrefWidgetPtr);

   Ui::WkfPreferencesDialog mUI;
   bool                     mInitialized{false};
   QList<PrefWidget*>       mStandardWidgets;
};
} // namespace wkf
#endif
