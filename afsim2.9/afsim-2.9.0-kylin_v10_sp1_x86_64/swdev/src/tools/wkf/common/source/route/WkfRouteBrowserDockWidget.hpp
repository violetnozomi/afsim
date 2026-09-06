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

#ifndef WKFROUTEBROWSERDOCKWIDGET_HPP
#define WKFROUTEBROWSERDOCKWIDGET_HPP

#include "wkf_common_export.h"

#include <QDockWidget>
#include <QTreeWidgetItem>

#include "WkfRouteBrowserCreateDialog.hpp"
#include "WkfRouteBrowserEditDialog.hpp"
#include "WkfRouteBrowserInterface.hpp"
#include "WkfUnitTypes.hpp"
#include "ui_WkfRouteBrowserDockWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT RouteBrowserDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   RouteBrowserDockWidget(QWidget* aParent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

   virtual ~RouteBrowserDockWidget() override = default;

   void Clear();

   void EditRoute(const RouteBrowserInterface::RouteInfo& aRouteInfo);
   void PopulateLocal(const RouteBrowserInterface::RouteInfo& aLocalRoute);
   void PopulateGlobal(std::vector<RouteBrowserInterface::RouteInfo>& aGlobalRoutes);
   void AddGlobal(const RouteBrowserInterface::RouteInfo& aGlobalRoute);
   void RemoveLocal(const QString& aPlatformName);
   void RemoveGlobal(const QString& aRouteName);
   bool IsPlatformDisplayed(const QString& aPlatformName);

   void ExpandLocalRoute(const QString& aPlatformName, int aIndex = -1);
   int  ExpandGlobalRoute(const QString& aRouteName, int aIndex = -1);
   void CollapseLocalRoute(const QString& aPlatformName);
   void CollapseGlobalRoute(const QString& aRouteName);
   void CollapseAllRoutes();
   void CollapseAllLocalRoutes();
   void CollapseAllGlobalRoutes();

   RouteBrowserEditDialog* GetEditDialog() const { return mEditDialog; }
   void                    SetEditDialog(RouteBrowserEditDialog* aDialogPtr);

   RouteBrowserCreateDialog* GetCreateDialog() const { return mCreateDialog; }
   void                      SetCreateDialog(RouteBrowserCreateDialog* aDialogPtr);

   void EditClickedHandler(); // emits EditDialogRequested
   void DeleteRouteHandler();
   void NewRouteClickedHandler();
   void SetNewRouteButtonEnabled(bool aEnable);
   void SetEditRouteButtonEnabled(bool aEnable);

   QString GetSelectedRouteName() const
   {
      return (mSelectedRouteItem != nullptr ? mSelectedRouteItem->text(0) : QString(""));
   }
   bool GlobalRouteSelected();
   void ClearSelection();

signals:
   void LocalRouteEditDialogRequested(const QString& aPlatformName);
   void ContextMenuRequested(QWidget*       aParent,
                             const QPoint&  aPoint,
                             const QString& aRouteName,
                             const QString& aPlatformName,
                             bool           aEditable,
                             bool           aDeleteable);

   void DockGlobalRouteSelectedChanged(const QString& aPrevRouteName,
                                       int            aPrevWaypoint,
                                       const QString& aCurrRouteName,
                                       int            aCurrWaypoint);
   void DockRouteSelectionChanged(const QString& aPrevPlatformName,
                                  const QString& aPrevRouteName,
                                  int            aPrevWaypoint,
                                  const QString& aCurrPlatformName,
                                  const QString& aCurrRouteName,
                                  int            aCurrWaypoint);

protected:
   RouteBrowserEditDialog*   mEditDialog;
   RouteBrowserCreateDialog* mCreateDialog;

private:
   enum RouteRole
   {
      EDITABLE  = Qt::UserRole,
      DELETABLE = Qt::UserRole + 1
   };

   void Connect();

   void GlobalRouteEditedHandler(const RouteBrowserInterface::RouteInfo& aRouteInfo);

   void WaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                             int                                        aInsertIndex,
                             const QString&                             aPlatformName,
                             const QString&                             aRouteName);
   void GlobalWaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                                   int                                        aInsertIndex,
                                   const QString&                             aRouteName);
   void LocalWaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                                  int                                        aInsertIndex,
                                  const QString&                             aPlatformName);

   void WaypointRemovedHandler(int aInsertIndex, const QString& aPlatformName, const QString& aRouteName);
   void GlobalWaypointRemovedHandler(int aInsertIndex, const QString& aRouteName);
   void LocalWaypointRemovedHandler(int aInsertIndex, const QString& aPlatformName);

   void ItemCollapsedHandler(QTreeWidgetItem* aItem);
   void CustomContextMenuHandler(const QPoint& aPoint); // emits ContextMenuRequested
   void CurrentItemChangedHandler(QTreeWidgetItem* aCurrent, QTreeWidgetItem* aPrevious);

   Ui::WkfRouteBrowserDockWidget mUi;
   QTreeWidgetItem*              mSelectedRouteItem;
   unsigned int                  mSelectedWaypoint;
   unsigned int                  mSelectedWaypointCount;

   void             FindRouteItemFromSelection();
   void             FindRouteItemFromTreeItem(QTreeWidgetItem* aItem);
   QTreeWidgetItem* FindRouteItem(QTreeWidgetItem* aItem, unsigned int& aWaypointCount, int& aSelectedWaypoint);


   enum ItemIndex
   {
      LABEL,
      LATITUDE,
      LONGITUDE,
      XOFFSET,
      YOFFSET,
      ALTITUDE,
      AGL,
      GOTO,
      SPEED,
      CLIMB_RATE,
      LINEAR_ACCEL,
      RADIAL_ACCEL,
      TURN_DIRECTION,
      SWITCH_TYPE
   };

   void WaypointLabelEditedHandler(const QString& aPlatformName, const QString& aRouteName, const QString& aLabel, int aIndex);
   void WaypointLatEditedHandler(const QString& aPlatformName, const QString& aRouteName, const UtLatPos& aLat, int aIndex);
   void WaypointLonEditedHandler(const QString& aPlatformName, const QString& aRouteName, const UtLonPos& aLon, int aIndex);
   void WaypointAltEditedHandler(const QString& aPlatformName, const QString& aRouteName, float aAlt, int aIndex);
   void WaypointAglEditedHandler(const QString& aPlatformName, const QString& aRouteName, int aAgl, int aIndex);
   void WaypointGoToEditedHandler(const QString& aPlatformName, const QString& aRouteName, const QString& aGoTo, int aIndex);
   void WaypointSpeedEditedHandler(const QString& aPlatformName, const QString& aRouteName, float aSpeed, int aIndex);
   void WaypointClimbRateEditedHandler(const QString& aPlatformName, const QString& aRouteName, float aClimbRate, int aIndex);
   void WaypointLinAccelEditedHandler(const QString& aPlatformName, const QString& aRouteName, float aLinAccel, int aIndex);
   void WaypointRadAccelEditedHandler(const QString& aPlatformName, const QString& aRouteName, float aRadAccel, int aIndex);
   void WaypointTurnDirEditedHandler(const QString& aPlatformName, const QString& aRouteName, int aTurnDir, int aIndex);
   void WaypointSwitchTypeEditedHandler(const QString& aPlatformName, const QString& aRouteName, int aSwitchType, int aIndex);
   void WaypointEndPathEditedHandler(const QString& aPlatformName, const QString& aRouteName, int aEndOfPath);

   void LocalWaypointLabelEditedHandler(const QString& aPlatformName, const QString& aLabel, int aIndex);
   void GlobalWaypointLabelEditedHandler(const QString& aRouteName, const QString& aLabel, int aIndex);

   void LocalWaypointLatEditedHandler(const QString& aPlatformName, const UtLatPos& aLat, int aIndex);
   void GlobalWaypointLatEditedHandler(const QString& aRouteName, const UtLatPos& aLat, int aIndex);

   void LocalWaypointLonEditedHandler(const QString& aPlatformName, const UtLonPos& aLon, int aIndex);
   void GlobalWaypointLonEditedHandler(const QString& aRouteName, const UtLonPos& aLon, int aIndex);

   void LocalWaypointAltEditedHandler(const QString& aPlatformName, float aAlt, int aIndex);
   void GlobalWaypointAltEditedHandler(const QString& aRouteName, float aAlt, int aIndex);

   void LocalWaypointAglEditedHandler(const QString& aPlatformName, int aAgl, int aIndex);
   void GlobalWaypointAglEditedHandler(const QString& aRouteName, int aAgl, int aIndex);

   void LocalWaypointGoToEditedHandler(const QString& aPlatformName, const QString& aGoTo, int aIndex);
   void GlobalWaypointGoToEditedHandler(const QString& aRouteName, const QString& aGoTo, int aIndex);

   void LocalWaypointSpeedEditedHandler(const QString& aPlatformName, float aSpeed, int aIndex);
   void GlobalWaypointSpeedEditedHandler(const QString& aRouteName, float aSpeed, int aIndex);

   void LocalWaypointClimbRateEditedHandler(const QString& aPlatformName, float aClimbRate, int aIndex);
   void GlobalWaypointClimbRateEditedHandler(const QString& aRouteName, float aClimbRate, int aIndex);

   void LocalWaypointLinAccelEditedHandler(const QString& aPlatformName, float aLinAccel, int aIndex);
   void GlobalWaypointLinAccelEditedHandler(const QString& aRouteName, float aLinAccel, int aIndex);

   void LocalWaypointRadAccelEditedHandler(const QString& aPlatformName, float aRadAccel, int aIndex);
   void GlobalWaypointRadAccelEditedHandler(const QString& aRouteName, float aRadAccel, int aIndex);

   void LocalWaypointTurnDirEditedHandler(const QString& aPlatformName, int aTurnDir, int aIndex);
   void GlobalWaypointTurnDirEditedHandler(const QString& aRouteName, int aTurnDir, int aIndex);

   void LocalWaypointSwitchTypeEditedHandler(const QString& aPlatformName, int aSwitchType, int aIndex);
   void GlobalWaypointSwitchTypeEditedHandler(const QString& aRouteName, int aSwitchType, int aIndex);

   void LocalWaypointEndPathEditedHandler(const QString& aPlatformName, int aEndOfPath);
   void GlobalWaypointEndPathEditedHandler(const QString& aRouteName, int aEndOfPath);

   void PopulateItems(QTreeWidgetItem* aRouteItem, const RouteBrowserInterface::RouteInfo& aRouteInfo);

   void PopulateLabel(QTreeWidgetItem* aWaypointItem, const QString& aLabel);
   void PopulateLatLon(QTreeWidgetItem* aWaypointItem, UtLatPos aLat, UtLonPos aLon, bool aVisible);
   void PopulateOffset(QTreeWidgetItem* aWaypointItem, double aXOffset, double aYOffset, bool aVisible);
   void PopulateAlt(QTreeWidgetItem* aWaypointItem, float aAlt);
   void PopulateAgl(QTreeWidgetItem* aWaypointItem, int aAgl);
   void PopulateGoTo(QTreeWidgetItem* aWaypointItem, const QString& aGoTo);
   void PopulateSpeed(QTreeWidgetItem* aWaypointItem, float aSpeed);
   void PopulateClimbRate(QTreeWidgetItem* aWaypointItem, float aClimbRate);
   void PopulateLinAccel(QTreeWidgetItem* aWaypointItem, float aLinAccel);
   void PopulateRadAccel(QTreeWidgetItem* aWaypointItem, float aRadAccel);
   void PopulateTurnDir(QTreeWidgetItem* aWaypointItem, int aTurnDir);
   void PopulateSwitchType(QTreeWidgetItem* aWaypointItem, int aSwitchType);
   void PopulateEndOfPath(QTreeWidgetItem* aRouteItem, int aEndOfPath);

   void EditLabel(QTreeWidgetItem* aLabelItem, const QString& aLabel);
   void EditLat(QTreeWidgetItem* aLatItem, UtLatPos aLat);
   void EditLon(QTreeWidgetItem* aLatItem, UtLonPos aLon);
   void EditLength(QTreeWidgetItem* aLengthItem, double aLength);
   void EditAlt(QTreeWidgetItem* aAltItem, float aAlt);
   void EditAgl(QTreeWidgetItem* aAglItem, int aAgl);
   void EditGoTo(QTreeWidgetItem* aGoToItem, const QString& aGoTo);
   void EditSpeed(QTreeWidgetItem* aSpeedItem, float aSpeed);
   void EditClimbRate(QTreeWidgetItem* aClimbRateItem, float aClimbRate);
   void EditLinAccel(QTreeWidgetItem* aLinAccelItem, float aLinAccel);
   void EditRadAccel(QTreeWidgetItem* aRadAccelItem, float aRadAccel);
   void EditTurnDir(QTreeWidgetItem* aTurnDirItem, int aTurnDir);
   void EditSwitchType(QTreeWidgetItem* aSwitchTypeItem, int aSwitchType);
   void EditEndPath(QTreeWidgetItem* aEndOfPath, int aEndPath);
};
} // namespace wkf
#endif // !WKFROUTEBROWSERDOCKWIDGET_HPP
