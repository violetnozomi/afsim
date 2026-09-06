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

#ifndef WKF_PERMISSION_DIALOG_HPP
#define WKF_PERMISSION_DIALOG_HPP

class QCloseEvent;
class QShowEvent;
class QTreeWidgetItem;

#include "wkf_export.h"

#include "WkfDialog.hpp"
#include "WkfPermissionGuiFactory.hpp"
#include "ui_WkfPermissionDialog.h"

namespace wkf
{
namespace permission
{
class Manager;

class WKF_EXPORT PermissionDialog : public Dialog
{
   Q_OBJECT

public:
   PermissionDialog(MainWindow* aParent, Qt::WindowFlags aFlags = Qt::WindowFlags());

   //! Returns the GUI factory.
   //! This can be used for adding GUI specializations of derived types.
   GuiFactory& GetGuiFactory() noexcept;

   //! Regenerates the GUI from the GUI factory.
   void Refresh();

   //! Locks the generated GUIs and Restore Defaults button.
   void LockPermissions();

   void SetActionVisible(bool aVisible);

protected:
   void HandleRestoreDefaultsAction();
   void HandleSaveAction();

   void TreeWidgetCurrentItemChanged(QTreeWidgetItem* aCurrent, QTreeWidgetItem* aPrevious);
   void SavePermissions();

   void closeEvent(QCloseEvent* aEvent) override;
   void showEvent(QShowEvent* aEvent) override;
   void ClearDialog();

   Ui::WkfPermissionDialog mUi;
   GuiFactory              mFactory;
   bool                    mInitialized = false;
};
} // namespace permission
} // namespace wkf

#endif
