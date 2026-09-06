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


#include "WkfCommVisNetworkGraphicsItem.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>

#include "UtQtMemory.hpp"

std::map<QString, QString> wkf::CommVisNetworkGraphicsItem::mNetworkTypeMap = {
   {"WsfNetwork", "WSF_COMM_NETWORK"},
   {"WsfNetworkAdHoc", "WSF_COMM_NETWORK_ADHOC"},
   {"WsfNetworkGeneric", "WSF_COMM_NETWORK_GENERIC"},
   {"WsfNetworkMesh", "WSF_COMM_NETWORK_MESH"},
   {"WsfNetworkMeshLegacy", "WSF_COMM_NETWORK_MESH_LEGACY"},
   {"WsfNetworkP2P", "WSF_COMM_NETWORK_P2P"},
   {"WsfNetworkRing", "WSF_COMM_NETWORK_RING"},
   {"WsfNetworkDirectedRing", "WSF_COMM_NETWORK_DIRECTED_RING"},
   {"WsfNetworkStar", "WSF_COMM_NETWORK_STAR"},
   {"WSF_COMM_NETWORK", "WSF_COMM_NETWORK"},
   {"WSF_COMM_NETWORK_ADHOC", "WSF_COMM_NETWORK_ADHOC"},
   {"WSF_COMM_NETWORK_GENERIC", "WSF_COMM_NETWORK_GENERIC"},
   {"WSF_COMM_NETWORK_MESH", "WSF_COMM_NETWORK_MESH"},
   {"WSF_COMM_NETWORK_MESH_LEGACY", "WSF_COMM_NETWORK_MESH_LEGACY"},
   {"WSF_COMM_NETWORK_P2P", "WSF_COMM_NETWORK_P2P"},
   {"WSF_COMM_NETWORK_RING", "WSF_COMM_NETWORK_RING"},
   {"WSF_COMM_NETWORK_DIRECTED_RING", "WSF_COMM_NETWORK_DIRECTED_RING"},
   {"WSF_COMM_NETWORK_STAR", "WSF_COMM_NETWORK_STAR"}};

wkf::CommVisNetworkGraphicsItem::CommVisNetworkGraphicsItem(const QString& aNetworkName,
                                                            const QString& aNetworkAddress,
                                                            const QString& aNetworkType)
   : mNetworkName(aNetworkName)
   , mNetworkAddress(aNetworkAddress)
   , mNetworkType(mNetworkTypeMap[aNetworkType])
{
   SetImage("network");

   QString networkStr = "default";
   if (!mNetworkName.isEmpty())
   {
      networkStr = mNetworkName;
   }
   if (!mNetworkAddress.isEmpty())
   {
      networkStr.append("\nAddress:  " + mNetworkAddress);
   }

   QString toolTipStr = "Network:   " + networkStr;
   SetToolTip(toolTipStr);

   SetName(mNetworkName);
   mDeviceType = "Network";
}

void wkf::CommVisNetworkGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent)
{
   ToggleSelected();

   QWidget* parentPtr = nullptr;
   if (scene() && !scene()->views().empty())
   {
      parentPtr = scene()->views()[0]->parentWidget();
   }

   if (!mDialogWindowPtr)
   {
      mDialogWindowPtr =
         QPointer<wkf::CommVisNetworkNodeDialog>(ut::qt::make_qt_ptr<wkf::CommVisNetworkNodeDialog>(this, parentPtr));
   }

   mDialogWindowPtr->show();
}

void wkf::CommVisNetworkGraphicsItem::SetName(const QString& aName)
{
   mNetworkName = aName;
   CommVisGraphicsItem::SetName(aName);
}

void wkf::CommVisNetworkGraphicsItem::DialogClosed()
{
   ToggleSelected();
}
