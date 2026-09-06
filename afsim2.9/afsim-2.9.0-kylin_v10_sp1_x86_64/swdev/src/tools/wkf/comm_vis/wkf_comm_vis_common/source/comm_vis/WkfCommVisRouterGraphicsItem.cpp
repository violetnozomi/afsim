// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#include "WkfCommVisRouterGraphicsItem.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>

#include "WkfCommVisRouterNodeDialog.hpp"

wkf::CommVisRouterGraphicsItem::CommVisRouterGraphicsItem(const CommEvent::RouterInfo& aRouterInfo)
   : mRouterName(QString::fromStdString(aRouterInfo.mRouterName))
   , mPlatformName(QString::fromStdString(aRouterInfo.mPlatformName))
   , mCommName(QString::fromStdString(aRouterInfo.mCommName))
   , mGatewayAddress(QString::fromStdString(aRouterInfo.mGatewayAddress))
   , mGateway(QString::fromStdString(aRouterInfo.mGatewayComm))
   , mRouterType(QString::fromStdString(aRouterInfo.mRouterType))
   , mHopLimit(aRouterInfo.mHopLimit)
   , mUseDefaultProtocol(aRouterInfo.mUsingDefaultInterface)
   , mUseMulticastProtocol(aRouterInfo.mUsingMulticastInterface)
{
   SetImage("router");

   QString toolTipStr = "Router:      " + mRouterName + "\nPlatform:   " + mPlatformName + "\nComm:      " + mCommName;
   SetToolTip(toolTipStr);
   SetName(toolTipStr);
   mDeviceType = "Router";
}

void wkf::CommVisRouterGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent)
{
   ToggleSelected();

   QWidget* parentPtr = nullptr;
   if (scene() && !scene()->views().empty())
   {
      parentPtr = scene()->views()[0]->parentWidget();
   }

   if (!mDialogWindowPtr)
   {
      mDialogWindowPtr = QPointer<wkf::CommVisRouterNodeDialog>(new wkf::CommVisRouterNodeDialog(this, parentPtr));
   }

   mDialogWindowPtr->show();
}

void wkf::CommVisRouterGraphicsItem::DialogClosed()
{
   ToggleSelected();
}
