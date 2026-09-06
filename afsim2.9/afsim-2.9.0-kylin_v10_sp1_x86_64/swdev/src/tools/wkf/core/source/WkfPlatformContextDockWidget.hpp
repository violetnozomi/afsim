// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFPLATFORMCONTEXTDOCKWIDGET_HPP
#define WKFPLATFORMCONTEXTDOCKWIDGET_HPP

#include "wkf_export.h"

#include <memory>

#include <QDockWidget>
#include <QSettings>

#include "VaCallbackHolder.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlugin.hpp"
#include "ui_WkfPlatformContextDockWidget.h"

namespace wkf
{
// Represents a change within a QTreeWidgetItem's contents. If pointer is null, use Qstring instead
struct ChangeInfo
{
   QTreeWidgetItem* changedPtr; // Address of changed item
   QString          text;       // New value of QComboBox, empty if no ComboBox is present
   int              column;
   ChangeInfo(QTreeWidgetItem* aPtr, QString aText, int aColumn)
      : changedPtr(aPtr)
      , text(aText)
      , column(aColumn){};
};

//! An abstract base class that provides a tree widget for displaying context-sensitive information about a Platform
class WKF_EXPORT PlatformContextDockWidget : public QDockWidget
{
protected:
   using ItemProvider = QList<QTreeWidgetItem*> (Plugin::*)(const std::string&);

   PlatformContextDockWidget(const QString&      title,
                             const ItemProvider& provider,
                             QWidget*            parent = nullptr,
                             Qt::WindowFlags     f      = Qt::WindowFlags());

   virtual ~PlatformContextDockWidget() override = 0;

   virtual bool SetCurrentPlatform(const Platform& aPlatform);
   virtual void PopulateItems();
   //! @note an explicit clear does not anticipate a follow-up SetCurrentPlatform
   virtual void ClearCurrent(bool aExplicit = false);
   virtual void ItemChanged(const ChangeInfo& aInfo) {}
   virtual void GroupIndexChanged(int aIndex) {}
   virtual void ItemSelectionChanged() {}

   //! @returns the plugin responsible for the QTreeWidgetItem
   Plugin* GetPluginFromItem(const QTreeWidgetItem* aItem) const;

   Ui::WkfPlatformContextDockWidget mUi;
   std::string                      mPlatformName;

private:
   void PlatformOfInterestChanged(Platform* aPlatform);

   bool GetDefaultItemExpanded(const QString& aItemName, bool aDefault) const;

   void SaveSettings();
   void LoadSettings(QSettings& aSettings);

   virtual void HandleItemPreference(QTreeWidgetItem* aItemPtr, const QString& aPluginName) {}

   // Initializes each combobox widget item with a combobox full of its information
   void BuildComboBoxWidgetItems(QTreeWidgetItem* aItem);

   // Note: When deleting mUi, the destructor will delete mUi.treewidget's children. These children should not be deleted
   // from this class,
   //  as the plugin's responsible for updating the data are responsible for deleting the children.
   // Basically: QTreeWidgets usually take ownership of the items, but we do not want that, and QPointers do not work on
   // QTreeWidgetItems
   //  since they are not QObject, and Qt does not support c++11 shared_ptrs so we can not use managed pointers on the delete.
   // So be careful. This can be reworked if a later version of Qt supports smart pointers.
   void TakeChildren();

   ItemProvider            mItemProvider;
   std::map<QString, bool> mItemExpansionMap;
   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf
#endif
