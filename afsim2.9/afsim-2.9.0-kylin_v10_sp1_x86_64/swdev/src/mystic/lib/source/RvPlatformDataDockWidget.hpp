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
#ifndef RVPLATFORMDATADOCKWIDGET_HPP
#define RVPLATFORMDATADOCKWIDGET_HPP

#include "RvPlugin.hpp"
#include "WkfPlatformDataDockWidget.hpp"
#include "WkfPlatformDataTreeItem.hpp"

namespace rv
{
class MsgEntityState;
class PlottingWidget;

class PlatformDataDockWidget : public wkf::PlatformDataDockWidget
{
public:
   PlatformDataDockWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
   void PlotActionHandler();
   void AddRingActionHandler();
   void RemoveRingActionHandler();

   void BuildMenuForItem(QMenu& aMenu, QTreeWidgetItem& aItem) override;
   void AddRingItem(QMenu& aMenu, QTreeWidgetItem& aItem);

   Plugin* mContextMenuItemPlugin;
   QString mContextMenuItemId;
};
} // namespace rv
#endif
