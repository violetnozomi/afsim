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

#include "WkfRouteBrowserDockWidget.hpp"

#include <cmath>
#include <sstream>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "UtCast.hpp"
#include "WkfRouteBrowserCreateDialog.hpp"
#include "WkfRouteBrowserEditDialog.hpp"
#include "WkfUnitTypes.hpp"

wkf::RouteBrowserDockWidget::RouteBrowserDockWidget(QWidget* aParent /*=0*/, Qt::WindowFlags aFlags)
   : QDockWidget(aParent, aFlags)
   , mSelectedRouteItem(nullptr)
   , mSelectedWaypoint(0)
   , mSelectedWaypointCount(0)

{
   mEditDialog   = new RouteBrowserEditDialog(aParent);
   mCreateDialog = new RouteBrowserCreateDialog(aParent);

   mUi.setupUi(this);

   mUi.treeWidget->setColumnCount(2);
   mUi.treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   mUi.mEditBtn->setEnabled(false);

   Connect();
}

void wkf::RouteBrowserDockWidget::Connect()
{
   connect(mUi.mEditBtn, &QPushButton::clicked, this, &RouteBrowserDockWidget::EditClickedHandler);
   connect(mUi.mNewRouteBtn, &QPushButton::clicked, this, &RouteBrowserDockWidget::NewRouteClickedHandler);
   connect(mUi.treeWidget, &QTreeWidget::itemCollapsed, this, &RouteBrowserDockWidget::ItemCollapsedHandler);
   connect(mUi.treeWidget, &QTreeWidget::customContextMenuRequested, this, &RouteBrowserDockWidget::CustomContextMenuHandler);
   connect(mUi.treeWidget, &QTreeWidget::currentItemChanged, this, &RouteBrowserDockWidget::CurrentItemChangedHandler);

   connect(mCreateDialog, &RouteBrowserCreateDialog::GlobalRouteCreated, this, &RouteBrowserDockWidget::AddGlobal);

   connect(mEditDialog, &wkf::RouteBrowserEditDialog::RouteEdited, this, &RouteBrowserDockWidget::EditRoute);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::GlobalRouteEdited, this, &RouteBrowserDockWidget::GlobalRouteEditedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointAdded, this, &RouteBrowserDockWidget::WaypointAddedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointRemoved, this, &RouteBrowserDockWidget::WaypointRemovedHandler);

   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointLabelEdited,
           this,
           &RouteBrowserDockWidget::WaypointLabelEditedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointLatEdited, this, &RouteBrowserDockWidget::WaypointLatEditedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointLonEdited, this, &RouteBrowserDockWidget::WaypointLonEditedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointAltEdited, this, &RouteBrowserDockWidget::WaypointAltEditedHandler);
   connect(mEditDialog, &wkf::RouteBrowserEditDialog::WaypointAglEdited, this, &RouteBrowserDockWidget::WaypointAglEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointGoToEdited,
           this,
           &RouteBrowserDockWidget::WaypointGoToEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointSpeedEdited,
           this,
           &RouteBrowserDockWidget::WaypointSpeedEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointClimbRateEdited,
           this,
           &RouteBrowserDockWidget::WaypointClimbRateEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointLinAccelEdited,
           this,
           &RouteBrowserDockWidget::WaypointLinAccelEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointRadAccelEdited,
           this,
           &RouteBrowserDockWidget::WaypointRadAccelEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointTurnDirEdited,
           this,
           &RouteBrowserDockWidget::WaypointTurnDirEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointSwitchTypeEdited,
           this,
           &RouteBrowserDockWidget::WaypointSwitchTypeEditedHandler);
   connect(mEditDialog,
           &wkf::RouteBrowserEditDialog::WaypointEndPathEdited,
           this,
           &RouteBrowserDockWidget::WaypointEndPathEditedHandler);
}

void wkf::RouteBrowserDockWidget::GlobalRouteEditedHandler(const RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   EditRoute(aRouteInfo);
   mCreateDialog->EditGlobalRoute(aRouteInfo); // updates the saved routes
}

void wkf::RouteBrowserDockWidget::WaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                                                       int                                        aInsertIndex,
                                                       const QString&                             aPlatformName,
                                                       const QString&                             aRouteName)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointAddedHandler(aWaypoint, aInsertIndex, aRouteName);
   }
   else
   {
      LocalWaypointAddedHandler(aWaypoint, aInsertIndex, aPlatformName);
   }
}

void wkf::RouteBrowserDockWidget::WaypointRemovedHandler(int aIndex, const QString& aPlatformName, const QString& aRouteName)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointRemovedHandler(aIndex, aRouteName);
   }
   else
   {
      LocalWaypointRemovedHandler(aIndex, aPlatformName);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                                                             int                                        aInsertIndex,
                                                             const QString&                             aRouteName)
{
   QList<QTreeWidgetItem*> matchedItems = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly | Qt::MatchRecursive, 0);
   if (!matchedItems.empty()) // only add a new item if it's not already in tree
   {
      QTreeWidgetItem* routeItem = matchedItems[0];

      // Add Waypoint to tree -------------------------------------------------------
      QTreeWidgetItem* waypointItem = new QTreeWidgetItem();
      waypointItem->setText(0, QString::fromStdString("Waypoint " + std::to_string(aInsertIndex)));
      routeItem->insertChild(aInsertIndex, waypointItem);

      // Add Waypoint Property Widgets ----------------------------------------------
      PopulateLabel(waypointItem, aWaypoint.mLabel);
      PopulateLatLon(waypointItem,
                     aWaypoint.mLat,
                     aWaypoint.mLon,
                     aWaypoint.mPointType == RouteBrowserInterface::WaypointInfo::cABSOLUTE);
      PopulateOffset(waypointItem,
                     aWaypoint.mXOffset,
                     aWaypoint.mYOffset,
                     aWaypoint.mPointType == RouteBrowserInterface::WaypointInfo::cRELATIVE);
      PopulateAlt(waypointItem, aWaypoint.mAlt);
      PopulateAgl(waypointItem, aWaypoint.mAltRef);
      PopulateGoTo(waypointItem, aWaypoint.mGoTo);
      PopulateSpeed(waypointItem, aWaypoint.mSpeed);
      PopulateClimbRate(waypointItem, aWaypoint.mClimbRate);
      PopulateLinAccel(waypointItem, aWaypoint.mLinAccel);
      PopulateRadAccel(waypointItem, aWaypoint.mRadAccel);
      PopulateTurnDir(waypointItem, aWaypoint.mTurnDir);
      PopulateSwitchType(waypointItem, aWaypoint.mSwitchType);

      for (int i = aInsertIndex + 1; i < routeItem->childCount() - 1; ++i)
      {
         routeItem->child(i)->setText(0, QString::fromStdString("Waypoint " + std::to_string(i)));
      }
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointAddedHandler(const RouteBrowserInterface::WaypointInfo& aWaypoint,
                                                            int                                        aInsertIndex,
                                                            const QString&                             aPlatformName)
{
   QList<QTreeWidgetItem*> matchedItems = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly, 0);
   if (!matchedItems.empty()) // only add a new item if it's not already in tree
   {
      QTreeWidgetItem* routeItem = matchedItems[0]->child(0);

      // Add Waypoint to tree -------------------------------------------------------
      QTreeWidgetItem* waypointItem = new QTreeWidgetItem();
      waypointItem->setText(0, QString::fromStdString("Waypoint " + std::to_string(aInsertIndex)));
      routeItem->insertChild(aInsertIndex, waypointItem);

      // Add Waypoint Property Widgets ----------------------------------------------
      PopulateLabel(waypointItem, aWaypoint.mLabel);
      PopulateLatLon(waypointItem,
                     aWaypoint.mLat,
                     aWaypoint.mLon,
                     aWaypoint.mPointType == RouteBrowserInterface::WaypointInfo::cABSOLUTE);
      PopulateOffset(waypointItem,
                     aWaypoint.mXOffset,
                     aWaypoint.mYOffset,
                     aWaypoint.mPointType == RouteBrowserInterface::WaypointInfo::cRELATIVE);
      PopulateAlt(waypointItem, aWaypoint.mAlt);
      PopulateAgl(waypointItem, aWaypoint.mAltRef);
      PopulateGoTo(waypointItem, aWaypoint.mGoTo);
      PopulateSpeed(waypointItem, aWaypoint.mSpeed);
      PopulateClimbRate(waypointItem, aWaypoint.mClimbRate);
      PopulateLinAccel(waypointItem, aWaypoint.mLinAccel);
      PopulateRadAccel(waypointItem, aWaypoint.mRadAccel);
      PopulateTurnDir(waypointItem, aWaypoint.mTurnDir);
      PopulateSwitchType(waypointItem, aWaypoint.mSwitchType);

      for (int i = aInsertIndex + 1; i < routeItem->childCount() - 1; ++i)
      {
         routeItem->child(i)->setText(0, QString::fromStdString("Waypoint " + std::to_string(i)));
      }
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointRemovedHandler(int aIndex, const QString& aRouteName)
{
   QList<QTreeWidgetItem*> matchedItems = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly | Qt::MatchRecursive, 0);
   if (!matchedItems.empty()) // only add a new item if it's not already in tree
   {
      QTreeWidgetItem* routeItem = matchedItems[0];

      if (routeItem != nullptr)
      {
         delete routeItem->takeChild(aIndex);

         for (int i = aIndex; i < routeItem->childCount() - 1; ++i)
         {
            routeItem->child(i)->setText(0, QString::fromStdString("Waypoint " + std::to_string(i)));
         }

         mUi.treeWidget->setCurrentItem(nullptr);
         mUi.treeWidget->setCurrentItem(routeItem->child(aIndex));
      }
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointRemovedHandler(int aIndex, const QString& aPlatformName)
{
   QList<QTreeWidgetItem*> matchedItems = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly, 0);
   if (!matchedItems.empty()) // only add a new item if it's not already in tree
   {
      QTreeWidgetItem* routeItem = matchedItems[0]->child(0);

      if (routeItem != nullptr)
      {
         delete routeItem->takeChild(aIndex);

         for (int i = aIndex; i < routeItem->childCount() - 1; ++i)
         {
            routeItem->child(i)->setText(0, QString::fromStdString("Waypoint " + std::to_string(i)));
         }

         mUi.treeWidget->setCurrentItem(nullptr);
         mUi.treeWidget->setCurrentItem(routeItem->child(aIndex));
      }
   }
}

void wkf::RouteBrowserDockWidget::Clear()
{
   mUi.treeWidget->clear();
}

// Finds the route described by aRouteInfo and edits the route in the dock tree widget, if it exists
void wkf::RouteBrowserDockWidget::EditRoute(const RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   QTreeWidgetItem* routeItem = nullptr;

   // look for local route
   if (!aRouteInfo.mPlatformName.isEmpty())
   {
      QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aRouteInfo.mPlatformName, Qt::MatchExactly);
      if (localMatches.size() == 1)
      {
         QTreeWidgetItem* platformItem = localMatches[0];
         if (platformItem != nullptr)
         {
            routeItem = platformItem->child(0);
            if (routeItem->childCount() - 1 != aRouteInfo.mWaypoints.size())
            {
               delete platformItem;
               routeItem = nullptr;
               PopulateLocal(aRouteInfo);
               ExpandLocalRoute(aRouteInfo.mPlatformName, mEditDialog->GetCurrentWaypoint());
            }
         }
      }
   }
   // look for global route
   else if (!aRouteInfo.mName.isEmpty())
   {
      QList<QTreeWidgetItem*> globalMatches =
         mUi.treeWidget->findItems(aRouteInfo.mName, Qt::MatchExactly | Qt::MatchRecursive);
      if (globalMatches.size() == 1)
      {
         routeItem = globalMatches[0];
         if (routeItem != nullptr)
         {
            if (routeItem->childCount() - 1 !=
                aRouteInfo.mWaypoints.size()) // if we added a waypoint, just erase route from dock, and repopulate
            {
               delete routeItem;
               routeItem = nullptr;
               AddGlobal(aRouteInfo);
               ExpandGlobalRoute(aRouteInfo.mName, mEditDialog->GetCurrentWaypoint());
            }
         }
      }
   }

   // once we have the routeItem, update all the fields
   if (routeItem != nullptr)
   {
      int index = 0;
      for (auto& w : aRouteInfo.mWaypoints)
      {
         QTreeWidgetItem* waypointItem = routeItem->child(index);

         QTreeWidgetItem* labelItem      = waypointItem->child(LABEL);
         QTreeWidgetItem* latItem        = waypointItem->child(LATITUDE);
         QTreeWidgetItem* lonItem        = waypointItem->child(LONGITUDE);
         QTreeWidgetItem* xoffsetItem    = waypointItem->child(XOFFSET);
         QTreeWidgetItem* yoffsetItem    = waypointItem->child(YOFFSET);
         QTreeWidgetItem* altItem        = waypointItem->child(ALTITUDE);
         QTreeWidgetItem* aglItem        = waypointItem->child(AGL);
         QTreeWidgetItem* goToItem       = waypointItem->child(GOTO);
         QTreeWidgetItem* speedItem      = waypointItem->child(SPEED);
         QTreeWidgetItem* climbRateItem  = waypointItem->child(CLIMB_RATE);
         QTreeWidgetItem* linAccelItem   = waypointItem->child(LINEAR_ACCEL);
         QTreeWidgetItem* radAccelItem   = waypointItem->child(RADIAL_ACCEL);
         QTreeWidgetItem* turnDirItem    = waypointItem->child(TURN_DIRECTION);
         QTreeWidgetItem* switchTypeItem = waypointItem->child(SWITCH_TYPE);

         // Altitude
         AltitudeTreeWidgetItem* wkAltItem = dynamic_cast<AltitudeTreeWidgetItem*>(altItem);
         if (w.mAlt > WaypointValueType::cUSE_DEFAULT)
         {
            if (wkAltItem == nullptr)
            {
               delete waypointItem->takeChild(ALTITUDE);
               wkAltItem =
                  new AltitudeTreeWidgetItem(nullptr, "Altitude: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(ALTITUDE, wkAltItem);
               EditAlt(wkAltItem, w.mAlt);
            }
            else
            {
               EditAlt(altItem, w.mAlt);
            }
         }
         else
         {
            if (wkAltItem != nullptr) // if we need to assign a -default-, -unset-, or -previous-, but the item is a
                                      // WkfAltitudeTreeWidgetItem
            {
               delete waypointItem->takeChild(ALTITUDE);
               altItem = new QTreeWidgetItem(QStringList()
                                             << "Altitude: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(ALTITUDE, altItem);
            }
            EditAlt(altItem, w.mAlt);
         }

         // Speed
         SpeedTreeWidgetItem* wkSpeedItem = dynamic_cast<SpeedTreeWidgetItem*>(speedItem);
         if (w.mSpeed > WaypointValueType::cUSE_DEFAULT)
         {
            if (wkSpeedItem == nullptr)
            {
               delete waypointItem->takeChild(SPEED);
               wkSpeedItem =
                  new SpeedTreeWidgetItem(nullptr, "Speed: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(SPEED, wkSpeedItem);
               EditSpeed(wkSpeedItem, w.mSpeed);
            }
            else
            {
               EditSpeed(speedItem, w.mSpeed);
            }
         }
         else
         {
            if (wkSpeedItem != nullptr) // if we need to assign a -default-, -unset-, or -previous-, but the item is a
                                        // WkfSpeedTreeWidgetItem
            {
               delete waypointItem->takeChild(SPEED);
               speedItem =
                  new QTreeWidgetItem(QStringList() << "Speed: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(SPEED, speedItem);
            }
            EditSpeed(speedItem, w.mSpeed);
         }

         // Climb Rate
         SpeedTreeWidgetItem* wkClimbRateItem = dynamic_cast<SpeedTreeWidgetItem*>(climbRateItem);
         if (w.mClimbRate > WaypointValueType::cUSE_DEFAULT)
         {
            if (wkClimbRateItem == nullptr)
            {
               delete waypointItem->takeChild(CLIMB_RATE);
               wkClimbRateItem =
                  new SpeedTreeWidgetItem(nullptr, "Climb Rate: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(CLIMB_RATE, wkClimbRateItem);
               EditClimbRate(wkClimbRateItem, w.mClimbRate);
            }
            else
            {
               EditClimbRate(climbRateItem, w.mClimbRate);
            }
         }
         else
         {
            if (wkClimbRateItem != nullptr) // if we need to assign a -default-, -unset-, or -previous-, but the item is
                                            // a WkfSpeedTreeWidgetItem
            {
               delete waypointItem->takeChild(CLIMB_RATE);
               climbRateItem = new QTreeWidgetItem(
                  QStringList() << "Climb Rate: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(CLIMB_RATE, climbRateItem);
            }
            EditClimbRate(climbRateItem, w.mClimbRate);
         }

         // Linear Acceleration
         AccelerationTreeWidgetItem* wkLinAccelItem = dynamic_cast<AccelerationTreeWidgetItem*>(linAccelItem);
         if (w.mLinAccel > WaypointValueType::cUSE_DEFAULT)
         {
            if (wkLinAccelItem == nullptr)
            {
               delete waypointItem->takeChild(LINEAR_ACCEL);
               wkLinAccelItem =
                  new AccelerationTreeWidgetItem(nullptr, "Linear Acceleration: "); // parent = nullptr for insertChild
                                                                                    // to work properly
               waypointItem->insertChild(LINEAR_ACCEL, wkLinAccelItem);
               EditLinAccel(wkLinAccelItem, w.mLinAccel);
            }
            else
            {
               EditLinAccel(linAccelItem, w.mLinAccel);
            }
         }
         else
         {
            if (wkLinAccelItem != nullptr) // if we need to assign a -default-, -unset-, or -previous-, but the item is
                                           // a WkfAccelerationTreeWidgetItem
            {
               delete waypointItem->takeChild(LINEAR_ACCEL);
               linAccelItem = new QTreeWidgetItem(
                  QStringList() << "Linear Acceleration: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(LINEAR_ACCEL, linAccelItem);
            }
            EditLinAccel(linAccelItem, w.mLinAccel);
         }

         // Radial Acceleration
         AccelerationTreeWidgetItem* wkRadAccelItem = dynamic_cast<AccelerationTreeWidgetItem*>(radAccelItem);
         if (w.mRadAccel > WaypointValueType::cUSE_DEFAULT)
         {
            if (wkRadAccelItem ==
                nullptr) // if we need to assign a value, but the item is not an WkfAccelerationTreeWidgetItem
            {
               delete waypointItem->takeChild(RADIAL_ACCEL);
               wkRadAccelItem =
                  new AccelerationTreeWidgetItem(nullptr, "Radial Acceleration: "); // parent = nullptr for insertChild
                                                                                    // to work properly
               waypointItem->insertChild(RADIAL_ACCEL, wkRadAccelItem);
               EditRadAccel(wkRadAccelItem, w.mRadAccel);
            }
            else
            {
               EditRadAccel(radAccelItem, w.mRadAccel);
            }
         }
         else // -default-, -unset-, or -previous-
         {
            if (wkRadAccelItem != nullptr) // if we need to assign a -default-, -unset-, or -previous-, but the item is
                                           // a WkfAccelerationTreeWidgetItem
            {
               delete waypointItem->takeChild(RADIAL_ACCEL);
               radAccelItem = new QTreeWidgetItem(
                  QStringList() << "Radial Acceleration: "); // parent = nullptr for insertChild to work properly
               waypointItem->insertChild(RADIAL_ACCEL, radAccelItem);
            }
            EditRadAccel(radAccelItem, w.mRadAccel);
         }

         EditLabel(labelItem, w.mLabel);
         EditLat(latItem, w.mLat);
         EditLon(lonItem, w.mLon);
         EditLength(xoffsetItem, w.mXOffset);
         EditLength(yoffsetItem, w.mYOffset);
         EditAgl(aglItem, w.mAltRef);
         EditGoTo(goToItem, w.mGoTo);
         EditTurnDir(turnDirItem, w.mTurnDir);
         EditSwitchType(switchTypeItem, w.mSwitchType);

         index++;
      }
      QTreeWidgetItem* endPathItem = routeItem->child(routeItem->childCount() - 1);
      EditEndPath(endPathItem, aRouteInfo.mEndPathType);
   }
}

// Adds the local route given by aLocalRoute to the dock widget if it's not already there
void wkf::RouteBrowserDockWidget::PopulateLocal(const RouteBrowserInterface::RouteInfo& aLocalRoute)
{
   QList<QTreeWidgetItem*> matchedItems = mUi.treeWidget->findItems(aLocalRoute.mPlatformName, Qt::MatchExactly, 0);
   if (matchedItems.empty()) // only add a new item if it's not already in tree
   {
      QTreeWidgetItem* platformItem = new QTreeWidgetItem();
      platformItem->setText(0, aLocalRoute.mPlatformName);
      mUi.treeWidget->addTopLevelItem(platformItem);

      // Add Route to Tree
      QTreeWidgetItem* routeItem = new QTreeWidgetItem();
      routeItem->setText(0, "Current Route");
      routeItem->setData(0, RouteRole::EDITABLE, true);   // editable
      routeItem->setData(0, RouteRole::DELETABLE, false); // not deleteable
      platformItem->addChild(routeItem);
      PopulateItems(routeItem, aLocalRoute);
      mUi.treeWidget->setCurrentItem(platformItem);
   }
}

// Populates the global routes in the route browser given by aGlobalRoutes
void wkf::RouteBrowserDockWidget::PopulateGlobal(std::vector<RouteBrowserInterface::RouteInfo>& aGlobalRoutes)
{
   // Top level "Global Routes" item
   QTreeWidgetItem* globalItem = new QTreeWidgetItem();
   globalItem->setText(0, "Global Routes");

   mUi.treeWidget->addTopLevelItem(globalItem);
   std::sort(aGlobalRoutes.begin(), aGlobalRoutes.end());
   for (auto& route : aGlobalRoutes)
   {
      // for each global route, inform the Create dialog of the route name, so it can prevent duplicates
      mCreateDialog->AddRouteName(route.mName);

      // Add Route to Tree
      QTreeWidgetItem* routeItem = new QTreeWidgetItem();
      routeItem->setText(0, route.mName);
      routeItem->setData(0, RouteRole::EDITABLE, route.mUserDefined);
      routeItem->setData(0, RouteRole::DELETABLE, route.mUserDefined);
      globalItem->addChild(routeItem);

      PopulateItems(routeItem, route);
   }
   globalItem->setExpanded(true);
}

// Add a (editable) route to the dock
void wkf::RouteBrowserDockWidget::AddGlobal(const RouteBrowserInterface::RouteInfo& aGlobalRoute)
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems("Global Routes", Qt::MatchExactly);
   if (items.empty())
   {
      // Top level "Global Routes" item
      QTreeWidgetItem* globalItem = new QTreeWidgetItem();
      globalItem->setText(0, "Global Routes");

      mUi.treeWidget->addTopLevelItem(globalItem);

      // Add Route to Tree
      QTreeWidgetItem* routeItem = new QTreeWidgetItem();
      routeItem->setText(0, aGlobalRoute.mName);
      routeItem->setData(0, RouteRole::EDITABLE, true);  // editable
      routeItem->setData(0, RouteRole::DELETABLE, true); // deleteable

      PopulateItems(routeItem, aGlobalRoute);
      globalItem->addChild(routeItem);
      mUi.treeWidget->setCurrentItem(routeItem);
   }

   else if (items.size() == 1)
   {
      QTreeWidgetItem* globalItem = items[0];

      // Add Route to Tree
      QList<QTreeWidgetItem*> routes =
         mUi.treeWidget->findItems(aGlobalRoute.mName, Qt::MatchExactly | Qt::MatchRecursive);
      if (routes.isEmpty())
      {
         QTreeWidgetItem* routeItem = new QTreeWidgetItem();
         routeItem->setText(0, aGlobalRoute.mName);
         routeItem->setData(0, RouteRole::EDITABLE, true);  // editable
         routeItem->setData(0, RouteRole::DELETABLE, true); // deleteable

         PopulateItems(routeItem, aGlobalRoute);
         globalItem->addChild(routeItem);
         mUi.treeWidget->setCurrentItem(routeItem);
      }
   }
}

// Removes the local route from the browser belonging to platform aPlatformName
void wkf::RouteBrowserDockWidget::RemoveLocal(const QString& aPlatformName)
{
   FindRouteItemFromSelection(); // get previous route (before deletion)
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   for (auto& item : items)
   {
      int index = mUi.treeWidget->indexOfTopLevelItem(item);
      if (index >= 0 && index < mUi.treeWidget->topLevelItemCount())
      {
         QTreeWidgetItem* i = mUi.treeWidget->takeTopLevelItem(index);
         if (i)
         {
            delete i;
         }
      }
   }
   if (mSelectedRouteItem != nullptr)
   {
      // get selected route after deletion.
      // If mSelectedRoute was non null before deletion and null after,
      // this means that the we deleted a selected item and need to emit
      // RouteSelectionChanged
      FindRouteItemFromSelection();
      if (!mSelectedRouteItem)
      {
         emit DockRouteSelectionChanged(aPlatformName, "Current Route", mSelectedWaypoint, "", "", -1);
      }
   }
}

void wkf::RouteBrowserDockWidget::RemoveGlobal(const QString& aRouteName)
{
   QList<QTreeWidgetItem*> routeItems  = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly | Qt::MatchRecursive);
   QList<QTreeWidgetItem*> globalItems = mUi.treeWidget->findItems("Global Routes", Qt::MatchExactly);
   if (!routeItems.empty())
   {
      QTreeWidgetItem* routeItem = routeItems[0];

      if (routeItem)
      {
         if (!globalItems.empty())
         {
            QTreeWidgetItem* globalItem = globalItems[0];
            if (globalItem)
            {
               int              index = globalItem->indexOfChild(routeItem);
               QTreeWidgetItem* item  = globalItem->takeChild(index);
               if (item)
               {
                  delete item;
                  if (globalItem->childCount() == 0)
                  {
                     // the last route was deleted, delete "Global Routes" as well
                     delete globalItem;
                     mSelectedRouteItem = nullptr;
                     mUi.mEditBtn->setEnabled(false);
                  }
               }
            }
         }
      }
   }
}

// Returns true if a platform with a given name is being displayed in the browser.
// aPlatformName: name of the platform in question
bool wkf::RouteBrowserDockWidget::IsPlatformDisplayed(const QString& aPlatformName)
{
   return !mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly).isEmpty();
}

// Expands the local route in the tree
// aPlatformName: name of the platform the route belongs to
// aWaypoint (optional): waypoint to expand
void wkf::RouteBrowserDockWidget::ExpandLocalRoute(const QString& aPlatformName, int aWaypointIndex /* = -1 */)
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (!items.isEmpty())
   {
      QTreeWidgetItem* platformItem = items[0];
      mUi.treeWidget->expandItem(platformItem);
      QTreeWidgetItem* routeItem = platformItem->child(0);
      if (routeItem != nullptr)
      {
         mUi.treeWidget->expandItem(routeItem);
         if ((aWaypointIndex >= 0) && (aWaypointIndex < routeItem->childCount() - 1))
         {
            mUi.treeWidget->expandItem(routeItem->child(aWaypointIndex));
            mUi.treeWidget->setCurrentItem(routeItem->child(
               aWaypointIndex)); // this will cause the routeItem to be deselected (setSelected will not do this)
         }
         else
         {
            mUi.treeWidget->setCurrentItem(platformItem);
         }
      }
   }
}

// Expands the global route in the tree
// aRouteName: name of the route
// aWaypoint (optional): waypoint to expand
int wkf::RouteBrowserDockWidget::ExpandGlobalRoute(const QString& aRouteName, int aWaypointIndex /* = -1 */)
{
   int                     waypointCount = 0;
   QList<QTreeWidgetItem*> items         = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly | Qt::MatchRecursive);
   if (!items.isEmpty())
   {
      QTreeWidgetItem* routeItem = items[0];
      if (routeItem != nullptr)
      {
         waypointCount = routeItem->childCount() - 1;
         if ((aWaypointIndex >= 0) && (aWaypointIndex < waypointCount))
         {
            mUi.treeWidget->setCurrentItem(routeItem->child(
               aWaypointIndex)); // this will cause the routeItem to be deselected (setSelected will not do this)
            mUi.treeWidget->expandItem(routeItem->child(aWaypointIndex));
         }
         else if (!routeItem->isExpanded())
         {
            mUi.treeWidget->setCurrentItem(routeItem);
            mUi.treeWidget->expandItem(routeItem);
         }
      }
   }
   return waypointCount;
}

// Collapse the local route in the tree
// aPlatformName: name of the platform the route belongs to
void wkf::RouteBrowserDockWidget::CollapseLocalRoute(const QString& aPlatformName)
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (!items.isEmpty())
   {
      QTreeWidgetItem* platformItem = items[0];
      QTreeWidgetItem* routeItem    = platformItem->child(0);
      if (routeItem != nullptr)
      {
         for (int i = 0; i < routeItem->childCount(); ++i)
         {
            mUi.treeWidget->collapseItem(routeItem->child(i));
         }
         mUi.treeWidget->setCurrentItem(platformItem);
      }
   }
}

void wkf::RouteBrowserDockWidget::CollapseGlobalRoute(const QString& aRouteName)
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly | Qt::MatchRecursive);
   if (!items.isEmpty())
   {
      QTreeWidgetItem* routeItem = items[0];
      if (routeItem != nullptr)
      {
         for (int i = 0; i < routeItem->childCount(); ++i)
         {
            mUi.treeWidget->collapseItem(routeItem->child(i));
         }
         mUi.treeWidget->setCurrentItem(routeItem);
      }
   }
}

// Collapses everything in the dock widget
void wkf::RouteBrowserDockWidget::CollapseAllRoutes()
{
   mUi.treeWidget->collapseAll();
}

// collapse only the local routes in the dock widget
void wkf::RouteBrowserDockWidget::CollapseAllLocalRoutes()
{
   CollapseAllRoutes();
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems("Global Routes", Qt::MatchExactly);
   if (items.size() == 1)
   {
      items[0]->setExpanded(true);
   }
}

// collapse only the global routes in the dock widget
void wkf::RouteBrowserDockWidget::CollapseAllGlobalRoutes()
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems("Global Routes", Qt::MatchExactly);
   if (items.size() == 1)
   {
      QTreeWidgetItem* globalItem = items[0];
      for (int i = 0; i < globalItem->childCount(); ++i)
      {
         globalItem->child(i)->setExpanded(false);
      }
      globalItem->setExpanded(false);
   }
}


// Called when "Edit Route" button is clicked
void wkf::RouteBrowserDockWidget::EditClickedHandler()
{
   // these dialogs are mutually exclusive and should not be open simultaneously
   if (mCreateDialog && mCreateDialog->isVisible())
   {
      mCreateDialog->close();
   }

   mEditDialog->SetCurrentWaypoint(mSelectedWaypoint);
   if (mSelectedRouteItem != nullptr)
   {
      QTreeWidgetItem* platformItem = mSelectedRouteItem->parent();
      if (platformItem)
      {
         QString routeName    = mSelectedRouteItem->text(0);
         QString platformName = platformItem->text(0);

         if (routeName.compare("Current Route") == 0)
         {
            emit LocalRouteEditDialogRequested(platformName);
         }
         else if (platformName.compare("Global Routes") == 0)
         {
            platformName                                         = "";
            std::vector<RouteBrowserInterface::RouteInfo> routes = mCreateDialog->GetSavedRoutes();
            for (auto& r : routes)
            {
               if (QString::compare(r.mName, routeName) == 0)
               {
                  mEditDialog->PopulateDialog(r);
                  mEditDialog->show();
                  break;
               }
            }
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::DeleteRouteHandler()
{
   if (mSelectedRouteItem)
   {
      mCreateDialog->RemoveRouteName(mSelectedRouteItem->text(0));
      RemoveGlobal(mSelectedRouteItem->text(0));
   }
}

// Called when "New Route" is clicked
void wkf::RouteBrowserDockWidget::NewRouteClickedHandler()
{
   // these dialogs are mutually exclusive and should not be open simultaneously
   if (mEditDialog && mEditDialog->isVisible())
   {
      mEditDialog->close();
   }

   if (mCreateDialog && mCreateDialog->isVisible())
   {
      mCreateDialog->CreateRouteTerminated(mCreateDialog->GetRouteInfo().mName);
   }
   mCreateDialog->PopulateDialog();
}

// Private ========================================================================================

// Deselects the platform from the browser if the corresponding route selection
// becomes hidden due to tree item collapse. This prevents the "Edit Route" button
// from being enabled when the route is collapsed and not selected.
void wkf::RouteBrowserDockWidget::ItemCollapsedHandler(QTreeWidgetItem* aItem)
{
   QList<QTreeWidgetItem*> selectedItems = mUi.treeWidget->selectedItems();
   if (!selectedItems.empty())
   {
      QTreeWidgetItem* item = selectedItems[0];
      // if we collapse a selected item, do nothing
      if (item != aItem)
      {
         // if one of the selected item's ancestors was collapsed (matches aItem), deselect the item
         while (item->parent() != nullptr)
         {
            if (item->parent() == aItem)
            {
               selectedItems[0]->setSelected(false);
               mUi.treeWidget->setCurrentItem(nullptr);
               break;
            }
            item = item->parent();
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::CustomContextMenuHandler(const QPoint& aPoint)
{
   QTreeWidgetItem* item = mUi.treeWidget->itemAt(aPoint);
   if (item != nullptr)
   {
      if (QString::compare(item->text(0), "Global Routes") != 0)
      {
         if (mSelectedRouteItem != nullptr)
         {
            QTreeWidgetItem* platformItem = mSelectedRouteItem->parent();
            if (platformItem != nullptr)
            {
               QString routeName    = mSelectedRouteItem->text(0);
               QString platformName = platformItem->text(0);
               if (routeName.compare("Current Route") == 0) // Local Route
               {
                  routeName = "";
               }
               if (platformName.compare("Global Routes") == 0)
               {
                  platformName = "";
               }
               bool editable   = mSelectedRouteItem->data(0, RouteRole::EDITABLE).value<bool>();
               bool deleteable = mSelectedRouteItem->data(0, RouteRole::DELETABLE).value<bool>();
               emit ContextMenuRequested(this, mUi.treeWidget->mapToGlobal(aPoint), routeName, platformName, editable, deleteable);
            }
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::CurrentItemChangedHandler(QTreeWidgetItem* aCurrent, QTreeWidgetItem* aPrevious)
{
   // ------------------- Update Edit Button enabled state ------------------------------
   bool enable = false;

   FindRouteItemFromTreeItem(aCurrent);
   if (mSelectedRouteItem != nullptr)
   {
      if (mSelectedRouteItem->data(0, RouteRole::EDITABLE).value<bool>())
      {
         enable = true;
      }
   }

   mUi.mEditBtn->setEnabled(enable);

   // ------------------- Find Global Route selection -----------------------------------

   QString      prevRoute    = "";
   QString      currRoute    = "";
   QString      prevPlatform = "";
   QString      currPlatform = "";
   int          prevWaypoint = -1;
   int          currWaypoint = -1;
   unsigned int count        = 0;

   QTreeWidgetItem* prevRouteItem = nullptr;
   if (aPrevious != nullptr)
   {
      prevRouteItem = FindRouteItem(aPrevious, count, prevWaypoint);
      if (prevRouteItem != nullptr)
      {
         prevRoute                         = prevRouteItem->text(0);
         QTreeWidgetItem* prevPlatformItem = prevRouteItem->parent();
         if (prevPlatformItem != nullptr)
         {
            if (prevPlatformItem->text(0).compare("Global Routes") != 0)
            {
               prevPlatform = prevPlatformItem->text(0);
            }
         }
      }
   }
   QTreeWidgetItem* currRouteItem = nullptr;
   if (aCurrent != nullptr)
   {
      currRouteItem = FindRouteItem(aCurrent, count, currWaypoint);
      if (currRouteItem != nullptr)
      {
         currRoute                         = currRouteItem->text(0);
         QTreeWidgetItem* currPlatformItem = currRouteItem->parent();
         if (currPlatformItem != nullptr)
         {
            if (currPlatformItem->text(0).compare("Global Routes") != 0)
            {
               currPlatform = currPlatformItem->text(0);
            }
            if (currWaypoint != -1)
            {
               if (mEditDialog->isVisible())
               {
                  if (currPlatform == mEditDialog->GetRouteInfo().mPlatformName)
                  {
                     mEditDialog->SelectWaypoint(currWaypoint);
                  }
               }
            }
         }
      }
   }

   bool prevRouteIsGlobal = (prevRouteItem != nullptr) && (prevPlatform.isEmpty());
   bool currRouteIsGlobal = (currRouteItem != nullptr) && (currPlatform.isEmpty());

   if (prevRouteIsGlobal || currRouteIsGlobal)
   {
      emit DockGlobalRouteSelectedChanged(prevRoute, prevWaypoint, currRoute, currWaypoint);
   }

   // ----------------------- Toggle Waypoint Labels -------------------------- //
   // need to let RouteBrowserInterface know that a route selection changed
   // so it can show/hide relevant labels.
   emit DockRouteSelectionChanged(prevPlatform, prevRoute, prevWaypoint, currPlatform, currRoute, currWaypoint);
}

// Returns the routeItem that is an ancestor of the currently selected item
// If nothing is selected or selection has no ancestor that is a route, returns nullptr.
void wkf::RouteBrowserDockWidget::FindRouteItemFromSelection()
{
   mSelectedRouteItem            = nullptr;
   unsigned int waypointCount    = 0;
   int          selectedWaypoint = -1;

   QList<QTreeWidgetItem*> selectedItems = mUi.treeWidget->selectedItems();
   if (!selectedItems.empty())
   {
      QTreeWidgetItem* item = selectedItems[0];
      mSelectedRouteItem    = FindRouteItem(item, waypointCount, selectedWaypoint);
      if (selectedWaypoint == -1)
      {
         selectedWaypoint = 0;
      }
      mSelectedWaypoint      = ut::safe_cast<unsigned int, int>(selectedWaypoint);
      mSelectedWaypointCount = waypointCount;
   }
}

// Returns the routeItem that is an ancestor of the aItem
// If nothing is selected or selection has no ancestor that is a route, returns nullptr.
void wkf::RouteBrowserDockWidget::FindRouteItemFromTreeItem(QTreeWidgetItem* aItem)
{
   mSelectedRouteItem            = nullptr;
   unsigned int waypointCount    = 0;
   int          selectedWaypoint = -1;

   if (aItem != nullptr)
   {
      mSelectedRouteItem = FindRouteItem(aItem, waypointCount, selectedWaypoint);
      if (selectedWaypoint == -1)
      {
         selectedWaypoint = 0;
      }
      mSelectedWaypoint      = ut::safe_cast<unsigned int, int>(selectedWaypoint);
      mSelectedWaypointCount = waypointCount;
   }
}

// Returns the routeItem that is an ancestor of aItem.
// If aItem has no ancestor that is a route, returns nullptr.
QTreeWidgetItem* wkf::RouteBrowserDockWidget::FindRouteItem(QTreeWidgetItem* aItem,
                                                            unsigned int&    aWaypointCount,
                                                            int&             aSelectedWaypoint)
{
   aSelectedWaypoint = -1;

   QTreeWidgetItem* routeItem = nullptr;

   QString selectedPlatform = "";
   QString selectedRoute    = "";

   QTreeWidgetItem* item = aItem;
   QList<QString>   itemAncestors;

   itemAncestors.push_front(item->text(0));
   int depth = 0;
   while (item->parent() != nullptr)
   {
      itemAncestors.push_front(item->parent()->text(0));
      item = item->parent();
      ++depth;
   }

   // item should now be the top most parent item.
   // It will either be the PlatformName or "Global Routes"
   if (itemAncestors[0].compare("Global Routes") != 0) // if it's not a global route
   {
      // QString selectedPlatform = itemAncestors[0];
      selectedPlatform = itemAncestors[0];
      QList<QTreeWidgetItem*> platformMatches =
         mUi.treeWidget->findItems(selectedPlatform, Qt::MatchExactly | Qt::MatchRecursive);

      if (!platformMatches.isEmpty())
      {
         QTreeWidgetItem* platformItem = platformMatches[0];
         if (platformItem != nullptr)
         {
            routeItem = platformItem->child(0);
            if (routeItem != nullptr)
            {
               aWaypointCount = routeItem->childCount() - 1;
            }
         }
      }
      if (depth > 1) // if the user selected a specific waypoint,
      {
         QString waypointText = itemAncestors[2];
         if (waypointText.startsWith("Waypoint"))
         {
            aSelectedWaypoint =
               std::stoi(waypointText.split(' ').back().toStdString()); // get just the number and convert to int.
         }
      }
   }
   else
   {
      if (depth > 0) // if depth == 0, the user selected "Global Routes" we can't populate a routeInfo from this
      {
         // QString selectedRoute = itemAncestors[1];
         selectedRoute = itemAncestors[1];
         QList<QTreeWidgetItem*> routeMatches =
            mUi.treeWidget->findItems(selectedRoute, Qt::MatchExactly | Qt::MatchRecursive);

         if (!routeMatches.isEmpty())
         {
            routeItem = routeMatches[0];
         }
         if (routeItem != nullptr)
         {
            aWaypointCount = routeItem->childCount() - 1;
         }
      }
      if (depth > 1) // if the user selected a specific waypoint,
      {
         QString waypointText = itemAncestors[2];
         if (waypointText.startsWith("Waypoint"))
         {
            aSelectedWaypoint =
               std::stoi(waypointText.split(' ').back().toStdString()); // get just the number and convert to int.
         }
      }
   }

   return routeItem;
}

void wkf::RouteBrowserDockWidget::WaypointLabelEditedHandler(const QString& aPlatformName,
                                                             const QString& aRouteName,
                                                             const QString& aLabel,
                                                             int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointLabelEditedHandler(aRouteName, aLabel, aIndex);
   }
   else
   {
      LocalWaypointLabelEditedHandler(aPlatformName, aLabel, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointLatEditedHandler(const QString&  aPlatformName,
                                                           const QString&  aRouteName,
                                                           const UtLatPos& aLat,
                                                           int             aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointLatEditedHandler(aRouteName, aLat, aIndex);
   }
   else
   {
      LocalWaypointLatEditedHandler(aPlatformName, aLat, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointLonEditedHandler(const QString&  aPlatformName,
                                                           const QString&  aRouteName,
                                                           const UtLonPos& aLon,
                                                           int             aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointLonEditedHandler(aRouteName, aLon, aIndex);
   }
   else
   {
      LocalWaypointLonEditedHandler(aPlatformName, aLon, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointAltEditedHandler(const QString& aPlatformName,
                                                           const QString& aRouteName,
                                                           float          aAlt,
                                                           int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointAltEditedHandler(aRouteName, aAlt, aIndex);
   }
   else
   {
      LocalWaypointAltEditedHandler(aPlatformName, aAlt, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointAglEditedHandler(const QString& aPlatformName,
                                                           const QString& aRouteName,
                                                           int            aAgl,
                                                           int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointAglEditedHandler(aRouteName, aAgl, aIndex);
   }
   else
   {
      LocalWaypointAglEditedHandler(aPlatformName, aAgl, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointGoToEditedHandler(const QString& aPlatformName,
                                                            const QString& aRouteName,
                                                            const QString& aGoTo,
                                                            int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointGoToEditedHandler(aRouteName, aGoTo, aIndex);
   }
   else
   {
      LocalWaypointGoToEditedHandler(aPlatformName, aGoTo, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointSpeedEditedHandler(const QString& aPlatformName,
                                                             const QString& aRouteName,
                                                             float          aSpeed,
                                                             int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointSpeedEditedHandler(aRouteName, aSpeed, aIndex);
   }
   else
   {
      LocalWaypointSpeedEditedHandler(aPlatformName, aSpeed, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointClimbRateEditedHandler(const QString& aPlatformName,
                                                                 const QString& aRouteName,
                                                                 float          aClimbRate,
                                                                 int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointClimbRateEditedHandler(aRouteName, aClimbRate, aIndex);
   }
   else
   {
      LocalWaypointClimbRateEditedHandler(aPlatformName, aClimbRate, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointLinAccelEditedHandler(const QString& aPlatformName,
                                                                const QString& aRouteName,
                                                                float          aLinAccel,
                                                                int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointLinAccelEditedHandler(aRouteName, aLinAccel, aIndex);
   }
   else
   {
      LocalWaypointLinAccelEditedHandler(aPlatformName, aLinAccel, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointRadAccelEditedHandler(const QString& aPlatformName,
                                                                const QString& aRouteName,
                                                                float          aRadAccel,
                                                                int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointRadAccelEditedHandler(aRouteName, aRadAccel, aIndex);
   }
   else
   {
      LocalWaypointRadAccelEditedHandler(aPlatformName, aRadAccel, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointTurnDirEditedHandler(const QString& aPlatformName,
                                                               const QString& aRouteName,
                                                               int            aTurnDir,
                                                               int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointTurnDirEditedHandler(aRouteName, aTurnDir, aIndex);
   }
   else
   {
      LocalWaypointTurnDirEditedHandler(aPlatformName, aTurnDir, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointSwitchTypeEditedHandler(const QString& aPlatformName,
                                                                  const QString& aRouteName,
                                                                  int            aSwitchType,
                                                                  int            aIndex)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointSwitchTypeEditedHandler(aRouteName, aSwitchType, aIndex);
   }
   else
   {
      LocalWaypointSwitchTypeEditedHandler(aPlatformName, aSwitchType, aIndex);
   }
}

void wkf::RouteBrowserDockWidget::WaypointEndPathEditedHandler(const QString& aPlatformName,
                                                               const QString& aRouteName,
                                                               int            aEndOfPath)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointEndPathEditedHandler(aRouteName, aEndOfPath);
   }
   else
   {
      LocalWaypointEndPathEditedHandler(aPlatformName, aEndOfPath);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointLabelEditedHandler(const QString& aPlatformName,
                                                                  const QString& aLabel,
                                                                  int            aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditLabel(routeItem->child(aIndex)->child(LABEL), aLabel);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointLabelEditedHandler(const QString& aRouteName, const QString& aLabel, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditLabel(routeItem->child(aIndex)->child(LABEL), aLabel);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointLatEditedHandler(const QString& aPlatformName, const UtLatPos& aLat, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditLat(routeItem->child(aIndex)->child(LATITUDE), aLat);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointLatEditedHandler(const QString& aRouteName, const UtLatPos& aLat, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditLat(routeItem->child(aIndex)->child(LATITUDE), aLat);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointLonEditedHandler(const QString& aPlatformName, const UtLonPos& aLon, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditLon(routeItem->child(aIndex)->child(LONGITUDE), aLon);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointLonEditedHandler(const QString& aRouteName, const UtLonPos& aLon, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditLon(routeItem->child(aIndex)->child(LONGITUDE), aLon);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointAltEditedHandler(const QString& aPlatformName, float aAlt, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditAlt(routeItem->child(aIndex)->child(ALTITUDE), aAlt);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointAltEditedHandler(const QString& aRouteName, float aAlt, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditAlt(routeItem->child(aIndex)->child(ALTITUDE), aAlt);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointAglEditedHandler(const QString& aPlatformName, int aAgl, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditAgl(routeItem->child(aIndex)->child(AGL), aAgl);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointAglEditedHandler(const QString& aRouteName, int aAgl, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditAgl(routeItem->child(aIndex)->child(AGL), aAgl);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointGoToEditedHandler(const QString& aPlatformName, const QString& aGoTo, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditGoTo(routeItem->child(aIndex)->child(GOTO), aGoTo);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointGoToEditedHandler(const QString& aRouteName, const QString& aGoTo, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditGoTo(routeItem->child(aIndex)->child(GOTO), aGoTo);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointSpeedEditedHandler(const QString& aPlatformName, float aSpeed, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditSpeed(routeItem->child(aIndex)->child(SPEED), aSpeed);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointSpeedEditedHandler(const QString& aRouteName, float aSpeed, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditSpeed(routeItem->child(aIndex)->child(SPEED), aSpeed);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointClimbRateEditedHandler(const QString& aPlatformName, float aClimbRate, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditClimbRate(routeItem->child(aIndex)->child(CLIMB_RATE), aClimbRate);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointClimbRateEditedHandler(const QString& aRouteName, float aClimbRate, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditClimbRate(routeItem->child(aIndex)->child(CLIMB_RATE), aClimbRate);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointLinAccelEditedHandler(const QString& aPlatformName, float aLinAccel, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditLinAccel(routeItem->child(aIndex)->child(LINEAR_ACCEL), aLinAccel);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointLinAccelEditedHandler(const QString& aRouteName, float aLinAccel, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditLinAccel(routeItem->child(aIndex)->child(LINEAR_ACCEL), aLinAccel);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointRadAccelEditedHandler(const QString& aPlatformName, float aRadAccel, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditRadAccel(routeItem->child(aIndex)->child(RADIAL_ACCEL), aRadAccel);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointRadAccelEditedHandler(const QString& aRouteName, float aRadAccel, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditRadAccel(routeItem->child(aIndex)->child(RADIAL_ACCEL), aRadAccel);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointTurnDirEditedHandler(const QString& aPlatformName, int aTurnDir, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditTurnDir(routeItem->child(aIndex)->child(TURN_DIRECTION), aTurnDir);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointTurnDirEditedHandler(const QString& aRouteName, int aTurnDir, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditTurnDir(routeItem->child(aIndex)->child(TURN_DIRECTION), aTurnDir);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointSwitchTypeEditedHandler(const QString& aPlatformName, int aSwitchType, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditSwitchType(routeItem->child(aIndex)->child(SWITCH_TYPE), aSwitchType);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointSwitchTypeEditedHandler(const QString& aRouteName, int aSwitchType, int aIndex)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditSwitchType(routeItem->child(aIndex)->child(SWITCH_TYPE), aSwitchType);
   }
}

void wkf::RouteBrowserDockWidget::LocalWaypointEndPathEditedHandler(const QString& aPlatformName, int aEndOfPath)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> localMatches = mUi.treeWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (localMatches.size() == 1)
   {
      QTreeWidgetItem* platformItem = localMatches[0];
      if (platformItem != nullptr)
      {
         routeItem = platformItem->child(0);
      }
   }
   if (routeItem != nullptr)
   {
      EditEndPath(routeItem->child(routeItem->childCount() - 1), aEndOfPath);
   }
}

void wkf::RouteBrowserDockWidget::GlobalWaypointEndPathEditedHandler(const QString& aRouteName, int aEndOfPath)
{
   QTreeWidgetItem* routeItem = nullptr;

   QList<QTreeWidgetItem*> globalMatches = mUi.treeWidget->findItems(aRouteName, Qt::MatchExactly);
   if (globalMatches.size() == 1)
   {
      routeItem = globalMatches[0];
   }
   if (routeItem != nullptr)
   {
      EditEndPath(routeItem->child(routeItem->childCount() - 1), aEndOfPath);
   }
}

void wkf::RouteBrowserDockWidget::PopulateItems(QTreeWidgetItem*                        aRouteItem,
                                                const RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   // get list of labels from all waypoints (to populate GoTo comboBox with)
   QStringList goToLabels;
   for (auto& waypoint : aRouteInfo.mWaypoints)
   {
      if (!waypoint.mLabel.isEmpty())
      {
         goToLabels.push_back(waypoint.mLabel);
      }
   }

   // add Waypoint Items to Route Item
   int index = 0;
   for (auto& waypoint : aRouteInfo.mWaypoints)
   {
      // Add Waypoint to tree -------------------------------------------------------
      QTreeWidgetItem* waypointItem = new QTreeWidgetItem();
      waypointItem->setText(0, QString::fromStdString("Waypoint " + std::to_string(index)));
      aRouteItem->addChild(waypointItem);

      // Add Waypoint Property Widgets ----------------------------------------------
      PopulateLabel(waypointItem, waypoint.mLabel);
      PopulateLatLon(waypointItem,
                     waypoint.mLat,
                     waypoint.mLon,
                     waypoint.mPointType == RouteBrowserInterface::WaypointInfo::cABSOLUTE);
      PopulateOffset(waypointItem,
                     waypoint.mXOffset,
                     waypoint.mYOffset,
                     waypoint.mPointType == RouteBrowserInterface::WaypointInfo::cRELATIVE);
      PopulateAlt(waypointItem, waypoint.mAlt);
      PopulateAgl(waypointItem, waypoint.mAltRef);
      PopulateGoTo(waypointItem, waypoint.mGoTo);
      PopulateSpeed(waypointItem, waypoint.mSpeed);
      PopulateClimbRate(waypointItem, waypoint.mClimbRate);
      PopulateLinAccel(waypointItem, waypoint.mLinAccel);
      PopulateRadAccel(waypointItem, waypoint.mRadAccel);
      PopulateTurnDir(waypointItem, waypoint.mTurnDir);
      PopulateSwitchType(waypointItem, waypoint.mSwitchType);

      index++;
   }
   PopulateEndOfPath(aRouteItem, aRouteInfo.mEndPathType);
}

void wkf::RouteBrowserDockWidget::PopulateLabel(QTreeWidgetItem* aWaypointItem, const QString& aLabel)
{
   QTreeWidgetItem* labelItem = new QTreeWidgetItem(aWaypointItem);
   labelItem->setText(0, "Label: ");

   EditLabel(labelItem, aLabel);
}

void wkf::RouteBrowserDockWidget::PopulateLatLon(QTreeWidgetItem* aWaypointItem, UtLatPos aLat, UtLonPos aLon, bool aVisible)
{
   LatitudeTreeWidgetItem* latItem = new LatitudeTreeWidgetItem(aWaypointItem, "Latitude: ");
   aWaypointItem->addChild(latItem);
   EditLat(latItem, aLat);

   LongitudeTreeWidgetItem* lonItem = new LongitudeTreeWidgetItem(aWaypointItem, "Longitude: ");
   aWaypointItem->addChild(lonItem);
   EditLon(lonItem, aLon);

   latItem->setHidden(!aVisible);
   lonItem->setHidden(!aVisible);
}

void wkf::RouteBrowserDockWidget::PopulateOffset(QTreeWidgetItem* aWaypointItem, double aXOffset, double aYOffset, bool aVisible)
{
   LengthTreeWidgetItem* xOffsetItem = new LengthTreeWidgetItem(aWaypointItem, "X Offset: ");
   aWaypointItem->addChild(xOffsetItem);
   EditLength(xOffsetItem, aXOffset);

   LengthTreeWidgetItem* yOffsetItem = new LengthTreeWidgetItem(aWaypointItem, "Y Offset: ");
   aWaypointItem->addChild(yOffsetItem);
   EditLength(yOffsetItem, aYOffset);

   xOffsetItem->setHidden(!aVisible);
   yOffsetItem->setHidden(!aVisible);
}

void wkf::RouteBrowserDockWidget::PopulateAlt(QTreeWidgetItem* aWaypointItem, float aAlt)
{
   QTreeWidgetItem* altItem = nullptr;
   if (std::abs(aAlt) <= WaypointValueType::cUSE_DEFAULT) // Why is this <= to cUSE_DEFAULT, that is incorrect.
   {
      altItem = new QTreeWidgetItem(aWaypointItem);
      altItem->setText(0, "Altitude: ");
   }
   else
   {
      altItem = new AltitudeTreeWidgetItem(aWaypointItem, "Altitude: ");
   }
   aWaypointItem->addChild(altItem);
   EditAlt(altItem, aAlt);
}

void wkf::RouteBrowserDockWidget::PopulateAgl(QTreeWidgetItem* aWaypointItem, int aAgl)
{
   QTreeWidgetItem* aglItem = new QTreeWidgetItem(aWaypointItem);
   aglItem->setText(0, "Altitude Reference: ");
   aWaypointItem->addChild(aglItem);
   EditAgl(aglItem, aAgl);
}

void wkf::RouteBrowserDockWidget::PopulateGoTo(QTreeWidgetItem* aWaypointItem, const QString& aGoTo)
{
   QTreeWidgetItem* goToItem = new QTreeWidgetItem(aWaypointItem);
   goToItem->setText(0, "Go To: ");
   aWaypointItem->addChild(goToItem);
   EditGoTo(goToItem, aGoTo);
}

void wkf::RouteBrowserDockWidget::PopulateSpeed(QTreeWidgetItem* aWaypointItem, float aSpeed)
{
   QTreeWidgetItem* speedItem;
   if (std::abs(aSpeed) <= WaypointValueType::cUSE_DEFAULT) // Why is this <= to cUSE_DEFAULT, that is incorrect.
   {
      speedItem = new QTreeWidgetItem(aWaypointItem);
      speedItem->setText(0, "Speed: ");
   }
   else
   {
      speedItem = new SpeedTreeWidgetItem(aWaypointItem, "Speed: ");
   }
   aWaypointItem->addChild(speedItem);
   EditSpeed(speedItem, aSpeed);
}

void wkf::RouteBrowserDockWidget::PopulateClimbRate(QTreeWidgetItem* aWaypointItem, float aClimbRate)
{
   QTreeWidgetItem* climbRateItem;
   if (std::abs(aClimbRate) <= WaypointValueType::cUSE_DEFAULT) // Why is this <= to cUSE_DEFAULT, that is incorrect.
   {
      climbRateItem = new QTreeWidgetItem(aWaypointItem);
      climbRateItem->setText(0, "Climb Rate: ");
   }
   else
   {
      climbRateItem = new SpeedTreeWidgetItem(aWaypointItem, "Climb Rate: ");
   }
   aWaypointItem->addChild(climbRateItem);
   EditClimbRate(climbRateItem, aClimbRate);
}

void wkf::RouteBrowserDockWidget::PopulateLinAccel(QTreeWidgetItem* aWaypointItem, float aLinAccel)
{
   QTreeWidgetItem* linAccelItem;
   if (std::abs(aLinAccel) <= WaypointValueType::cUSE_DEFAULT) // Why is this <= to cUSE_DEFAULT, that is incorrect.
   {
      linAccelItem = new QTreeWidgetItem(aWaypointItem);
      linAccelItem->setText(0, "Linear Acceleration: ");
   }
   else
   {
      linAccelItem = new AccelerationTreeWidgetItem(aWaypointItem, "Linear Acceleration: ");
   }
   aWaypointItem->addChild(linAccelItem);
   EditLinAccel(linAccelItem, aLinAccel);
}

void wkf::RouteBrowserDockWidget::PopulateRadAccel(QTreeWidgetItem* aWaypointItem, float aRadAccel)
{
   QTreeWidgetItem* radAccelItem;
   if (std::abs(aRadAccel) <= WaypointValueType::cUSE_DEFAULT) // Why is this <= to cUSE_DEFAULT, that is incorrect.
   {
      radAccelItem = new QTreeWidgetItem(aWaypointItem);
      radAccelItem->setText(0, "Radial Acceleration: ");
   }
   else
   {
      radAccelItem = new AccelerationTreeWidgetItem(aWaypointItem, "Radial Acceleration: ");
   }
   aWaypointItem->addChild(radAccelItem);
   EditRadAccel(radAccelItem, aRadAccel);
}

void wkf::RouteBrowserDockWidget::PopulateTurnDir(QTreeWidgetItem* aWaypointItem, int aTurnDir)
{
   QTreeWidgetItem* turnDirItem = new QTreeWidgetItem(aWaypointItem);
   turnDirItem->setText(0, "Turn Direction: ");
   aWaypointItem->addChild(turnDirItem);
   EditTurnDir(turnDirItem, aTurnDir);
}

void wkf::RouteBrowserDockWidget::PopulateSwitchType(QTreeWidgetItem* aWaypointItem, int aSwitchType)
{
   QTreeWidgetItem* switchItem = new QTreeWidgetItem(aWaypointItem);
   switchItem->setText(0, "Switch Type: ");
   aWaypointItem->addChild(switchItem);
   EditSwitchType(switchItem, aSwitchType);
}

void wkf::RouteBrowserDockWidget::PopulateEndOfPath(QTreeWidgetItem* aRouteItem, int aEndOfPath)
{
   QTreeWidgetItem* endPathItem = new QTreeWidgetItem(aRouteItem);
   endPathItem->setText(0, "End Path Option: ");
   aRouteItem->addChild(endPathItem);
   EditEndPath(endPathItem, aEndOfPath);
}

void wkf::RouteBrowserDockWidget::EditLabel(QTreeWidgetItem* aLabelItem, const QString& aLabel)
{
   if (aLabelItem != nullptr)
   {
      if (aLabel.isEmpty())
      {
         aLabelItem->setText(1, "-unset-");
      }
      else
      {
         aLabelItem->setText(1, aLabel);
      }
   }
}

void wkf::RouteBrowserDockWidget::EditLat(QTreeWidgetItem* aLatItem, UtLatPos aLat)
{
   LatitudeTreeWidgetItem* latItem = dynamic_cast<LatitudeTreeWidgetItem*>(aLatItem);
   if (latItem != nullptr)
   {
      latItem->SetValue(aLat);
   }
}

void wkf::RouteBrowserDockWidget::EditLon(QTreeWidgetItem* aLonItem, UtLonPos aLon)
{
   LongitudeTreeWidgetItem* lonItem = dynamic_cast<LongitudeTreeWidgetItem*>(aLonItem);
   if (lonItem)
   {
      lonItem->SetValue(aLon);
   }
}

void wkf::RouteBrowserDockWidget::EditLength(QTreeWidgetItem* aLengthItem, double aLength)
{
   LengthTreeWidgetItem* lengthItem = dynamic_cast<LengthTreeWidgetItem*>(aLengthItem);
   if (lengthItem)
   {
      lengthItem->SetValue(aLength);
   }
}

void wkf::RouteBrowserDockWidget::EditAlt(QTreeWidgetItem* aAltItem, float aAlt)
{
   if (aAltItem != nullptr)
   {
      AltitudeTreeWidgetItem* altItem = dynamic_cast<AltitudeTreeWidgetItem*>(aAltItem);
      if (altItem != nullptr)
      {
         altItem->SetValue(aAlt);
      }
      else
      {
         if (aAlt == WaypointValueType::cUSE_PREVIOUS)
         {
            aAltItem->setText(1, "-previous-");
         }
         else // cNOT_SET
         {
            aAltItem->setText(1, "-unset-");
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::EditAgl(QTreeWidgetItem* aAglItem, int aAgl)
{
   if (aAgl == 0)
   {
      aAglItem->setText(1, "-mover default-");
   }
   else if (aAgl == 1)
   {
      aAglItem->setText(1, "MSL");
   }
   else
   {
      aAglItem->setText(1, "AGL");
   }
}

void wkf::RouteBrowserDockWidget::EditGoTo(QTreeWidgetItem* aGoToItem, const QString& aGoTo)
{
   if (aGoTo.isEmpty())
   {
      aGoToItem->setText(1, "-unset-");
   }
   else
   {
      aGoToItem->setText(1, aGoTo);
   }
}

void wkf::RouteBrowserDockWidget::EditSpeed(QTreeWidgetItem* aSpeedItem, float aSpeed)
{
   if (aSpeedItem != nullptr)
   {
      SpeedTreeWidgetItem* speedItem = dynamic_cast<SpeedTreeWidgetItem*>(aSpeedItem);
      if (speedItem != nullptr)
      {
         speedItem->SetValue(aSpeed);
      }
      else
      {
         if (aSpeed == WaypointValueType::cUSE_PREVIOUS)
         {
            aSpeedItem->setText(1, "-previous-");
         }
         else // cNOT_SET
         {
            aSpeedItem->setText(1, "-unset-");
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::EditClimbRate(QTreeWidgetItem* aClimbRateItem, float aClimbRate)
{
   if (aClimbRateItem != nullptr)
   {
      SpeedTreeWidgetItem* climbRateItem = dynamic_cast<SpeedTreeWidgetItem*>(aClimbRateItem);
      if (climbRateItem != nullptr)
      {
         climbRateItem->SetValue(aClimbRate);
      }
      else
      {
         if (aClimbRate == WaypointValueType::cUSE_DEFAULT)
         {
            aClimbRateItem->setText(1, "-mover default-");
         }
         else if (aClimbRate == WaypointValueType::cUSE_PREVIOUS)
         {
            aClimbRateItem->setText(1, "-previous-");
         }
         else // cNOT_SET
         {
            aClimbRateItem->setText(1, "-unset-");
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::EditLinAccel(QTreeWidgetItem* aLinAccelItem, float aLinAccel)
{
   if (aLinAccelItem != nullptr)
   {
      AccelerationTreeWidgetItem* linAccelItem = dynamic_cast<AccelerationTreeWidgetItem*>(aLinAccelItem);
      if (linAccelItem != nullptr)
      {
         linAccelItem->SetValue(aLinAccel);
      }
      else
      {
         if (aLinAccel == WaypointValueType::cUSE_DEFAULT)
         {
            aLinAccelItem->setText(1, "-mover default-");
         }
         else if (aLinAccel == WaypointValueType::cUSE_PREVIOUS)
         {
            aLinAccelItem->setText(1, "-previous-");
         }
         else // cNOT_SET
         {
            aLinAccelItem->setText(1, "-unset-");
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::EditRadAccel(QTreeWidgetItem* aRadAccelItem, float aRadAccel)
{
   if (aRadAccelItem != nullptr)
   {
      AccelerationTreeWidgetItem* radAccelItem = dynamic_cast<AccelerationTreeWidgetItem*>(aRadAccelItem);
      if (radAccelItem != nullptr)
      {
         radAccelItem->SetValue(aRadAccel);
      }
      else
      {
         if (aRadAccel == WaypointValueType::cUSE_DEFAULT)
         {
            aRadAccelItem->setText(1, "-mover default-");
         }
         else if (aRadAccel == WaypointValueType::cUSE_PREVIOUS)
         {
            aRadAccelItem->setText(1, "-previous-");
         }
         else // cNOT_SET
         {
            aRadAccelItem->setText(1, "-unset-");
         }
      }
   }
}

void wkf::RouteBrowserDockWidget::EditTurnDir(QTreeWidgetItem* aTurnDirItem, int aTurnDir)
{
   QStringList turnDirList = QStringList() << "Left"
                                           << "Shortest"
                                           << "Right";
   if (aTurnDir >= -1 && aTurnDir < turnDirList.size() - 1)
   {
      aTurnDirItem->setText(1, turnDirList[aTurnDir + 1]); // aTurnDir : [-1, 0, 1]
   }
}

void wkf::RouteBrowserDockWidget::EditSwitchType(QTreeWidgetItem* aSwitchTypeItem, int aSwitchType)
{
   QStringList switchList = QStringList() << "-mover default-"
                                          << "On Passing"
                                          << "On Approach";
   if (aSwitchType >= 0 && aSwitchType < switchList.size()) // aSwitchType : [0, 1, 2]
   {
      aSwitchTypeItem->setText(1, switchList[aSwitchType]);
   }
}

void wkf::RouteBrowserDockWidget::EditEndPath(QTreeWidgetItem* aEndOfPathItem, int aEndPath)
{
   QStringList endPathList = QStringList() << "-mover default-"
                                           << "Extrapolate"
                                           << "Stop"
                                           << "Remove";
   if (aEndPath >= 0 && aEndPath < endPathList.size())
   {
      aEndOfPathItem->setText(1, endPathList[aEndPath]);
   }
}

void wkf::RouteBrowserDockWidget::SetNewRouteButtonEnabled(bool aEnable)
{
   mUi.mNewRouteBtn->setEnabled(aEnable);
}

void wkf::RouteBrowserDockWidget::SetEditRouteButtonEnabled(bool aEnable)
{
   mUi.mEditBtn->setEnabled(aEnable);
}


bool wkf::RouteBrowserDockWidget::GlobalRouteSelected()
{
   QList<QTreeWidgetItem*> items = mUi.treeWidget->findItems("Global Routes", Qt::MatchExactly);
   if (items.size() == 1)
   {
      QTreeWidgetItem*        globalItem = items[0];
      QList<QTreeWidgetItem*> routes =
         mUi.treeWidget->findItems(GetSelectedRouteName(), Qt::MatchExactly | Qt::MatchRecursive);
      for (const auto& curRoute : routes)
      {
         int foundItem = globalItem->indexOfChild(curRoute);
         return (foundItem >= 0);
      }
   }
   return false;
}

void wkf::RouteBrowserDockWidget::ClearSelection()
{
   mUi.treeWidget->setCurrentItem(nullptr);
}


void wkf::RouteBrowserDockWidget::SetCreateDialog(RouteBrowserCreateDialog* aDialogPtr)
{
   if (mCreateDialog != nullptr)
   {
      delete mCreateDialog;
   }
   mCreateDialog = aDialogPtr;
}


void wkf::RouteBrowserDockWidget::SetEditDialog(RouteBrowserEditDialog* aDialogPtr)
{
   if (mEditDialog != nullptr)
   {
      delete mEditDialog;
   }
   mEditDialog = aDialogPtr;
}
