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


#include "WkfCommVisGatewayGraphicsItem.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>

wkf::CommVisGatewayGraphicsItem::CommVisGatewayGraphicsItem(const CommEvent::GatewayInfo& aGatewayInfo)
   : mPlatformName(QString::fromStdString(aGatewayInfo.mPlatformName))
   , mGatewayName(QString::fromStdString(aGatewayInfo.mGatewayName))
   , mGatewayAddress(QString::fromStdString(aGatewayInfo.mGatewayAddress))
   , mGatewayRemotePlatform(QString::fromStdString(aGatewayInfo.mGatewayRemotePlatform))
   , mGatewayRemoteAddress(QString::fromStdString(aGatewayInfo.mGatewayRemoteAddress))
   , mGatewayRemoteComm(QString::fromStdString(aGatewayInfo.mGatewayRemoteComm))
{
   SetImage("gateway");
   QString toolTipStr = "Gateway:   " + mGatewayName + "\nPlatform:   " + mPlatformName;
   if (!mGatewayAddress.isEmpty())
   {
      toolTipStr.append("\nAddress:  " + mGatewayAddress);
   }
   SetToolTip(toolTipStr);

   SetName(mGatewayName);
   mDeviceType = "Gateway";
}


void wkf::CommVisGatewayGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent)
{
   ToggleSelected();
   QWidget* parentPtr = nullptr;
   if (scene() && !scene()->views().empty())
   {
      parentPtr = scene()->views()[0]->parentWidget();
   }

   if (!mDialogWindowPtr)
   {
      mDialogWindowPtr = QPointer<wkf::CommVisGatewayNodeDialog>(new wkf::CommVisGatewayNodeDialog(this, parentPtr));
   }

   mDialogWindowPtr->show();
}


void wkf::CommVisGatewayGraphicsItem::DialogClosed()
{
   ToggleSelected();
}

void wkf::CommVisGatewayGraphicsItem::SetRemoteInterface(const QString& aRemotePlatformName, const QString& aRemoteCommName)
{
   mGatewayRemotePlatform = aRemotePlatformName;
   mGatewayRemoteComm     = aRemoteCommName;
}
