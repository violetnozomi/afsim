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

#include "WkfRouteBrowserEditDialog.hpp"

#include <QFormLayout>

#include "UtEarth.hpp"
#include "WkfDialog.hpp"
#include "WkfEnvironment.hpp"
#include "ui_WkfRouteBrowserDialog.h"

wkf::RouteBrowserEditDialog::RouteBrowserEditDialog(QWidget* aParent /*= 0*/, Qt::WindowFlags aFlags /*= 0*/)
   : RouteBrowserDialog(aParent, aFlags)
{
   setWindowTitle("Edit Route");

   mRouteDescription = new QLabel(this);
   mRouteDescription->setObjectName(QStringLiteral("mRouteDescription"));

   mUi.waypointPropertiesSheet->setWidget(0, QFormLayout::SpanningRole, mRouteDescription);
   mRouteDescription->setAlignment(Qt::AlignCenter);

   mUi.buttonBox->button(QDialogButtonBox::Apply)->setVisible(true);

   mUi.treeWidget->setColumnCount(1);
   mUi.treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

   connect(&wkfEnv,
           QOverload<const wkf::PlatformList&, const wkf::EntityList&, const vespa::VaAttachmentList&>::of(
              &wkf::Environment::MoveSelectionAndAttachmentComplete),
           this,
           &wkf::RouteBrowserEditDialog::MoveSelectionAndAttachmentComplete);
}

wkf::RouteBrowserEditDialog::~RouteBrowserEditDialog()
{
   delete mRouteDescription;
}

// Set the current route to aRouteInfo, and populate the edit dialog with the route's info
void wkf::RouteBrowserEditDialog::PopulateDialog(const RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   RouteBrowserInterface::RouteInfo previousCopy = mPreviousRoute;
   mCurrentRoute                                 = aRouteInfo;
   mPreviousRoute                                = aRouteInfo;
   PopulateDescription();                         // populate route description in dialog
   PopulateTree(mCurrentRoute, mCurrentWaypoint); // populate left side of dialog and select appropriate waypoint
   emit RoutePopulated(aRouteInfo, previousCopy);
}

void wkf::RouteBrowserEditDialog::EnableButtons()
{
   mUi.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
   mUi.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
   mUi.mChangesLabel->setVisible(false);
}

void wkf::RouteBrowserEditDialog::DisableButtons()
{
   mUi.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
   mUi.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
   mUi.mChangesLabel->setVisible(true);
}


bool wkf::RouteBrowserEditDialog::ApplyChanges()
{
   if (CheckValidWaypointProperties())
   {
      mPreviousRoute = mCurrentRoute;
      emit Apply(mCurrentRoute);
      return true;
   }
   return false;
}

void wkf::RouteBrowserEditDialog::CancelHandler()
{
   IsPreviousRouteGlobal() ? emit GlobalRouteEdited(mPreviousRoute) : emit RouteEdited(mPreviousRoute);
   emit EditDialogCancelled(mPreviousRoute);
   close();
}

void wkf::RouteBrowserEditDialog::Refresh()
{
   emit RequestRouteInfo(mCurrentRoute.mPlatformName);
}

// private ================================================================================

void wkf::RouteBrowserEditDialog::PopulateDescription()
{
   QString routeName    = mCurrentRoute.mName;
   QString platformName = mCurrentRoute.mPlatformName;

   QString description = "";

   if (!platformName.isEmpty()) // local route
   {
      description = "Route for Platform " + platformName;
   }
   else // global route
   {
      description = "Global Route " + routeName;
   }
   mRouteDescription->setText(description);

   // if the currently selected platform has no route do not allow users to add waypoints
   mUi.mLocationSelector->setEnabled(!mCurrentRoute.mWaypoints.isEmpty());
}


void wkf::RouteBrowserEditDialog::MoveSelectionAndAttachmentComplete(const wkf::PlatformList&       aPlatformList,
                                                                     const wkf::EntityList&         aNonPlatformList,
                                                                     const vespa::VaAttachmentList& aAttachmentList)
{
   if (isVisible())
   {
      for (auto&& curPlatform : aPlatformList)
      {
         if ((curPlatform->GetName() == mCurrentRoute.mPlatformName.toStdString()) && !mCurrentRoute.mWaypoints.isEmpty())
         {
            UtLLAPos newlla;
            curPlatform->GetPosition().GetLLA(newlla);

            SelectWaypoint(0);
            SetCurrentWaypoint(0);
            mCurrentRoute.mWaypoints[0].mLat = newlla.GetLat();
            mCurrentRoute.mWaypoints[0].mLon = newlla.GetLon();
            mCurrentRoute.mWaypoints[0].mAlt = newlla.GetAlt();
            mUi.mLatEdit->SetValue(newlla.GetLat());
            mUi.mLonEdit->SetValue(newlla.GetLon());

            WaypointLatEdited(mCurrentRoute.mPlatformName, mCurrentRoute.mName, newlla.GetLat(), 0);
            WaypointLonEdited(mCurrentRoute.mPlatformName, mCurrentRoute.mName, newlla.GetLon(), 0);
            WaypointAltEdited(mCurrentRoute.mPlatformName, mCurrentRoute.mName, newlla.GetAlt(), 0);
            emit RouteEdited(mCurrentRoute);
         }
      }
   }
}
