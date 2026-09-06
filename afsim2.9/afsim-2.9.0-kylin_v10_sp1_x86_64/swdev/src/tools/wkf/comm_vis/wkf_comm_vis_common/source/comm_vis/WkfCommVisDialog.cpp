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

#include "WkfCommVisDialog.hpp"

#include <QGraphicsItemAnimation>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QTextStream>
#include <QTimeLine>

#include "UtCast.hpp"
#include "UtQtMemory.hpp"
#include "VaTimeManager.hpp"
#include "WkfCommVisCommGraphicsItem.hpp"
#include "WkfCommVisGatewayGraphicsItem.hpp"
#include "WkfCommVisLink.hpp"
#include "WkfCommVisNetworkGraphicsItem.hpp"
#include "WkfCommVisPacketGraphicsItem.hpp"
#include "WkfCommVisRouterGraphicsItem.hpp"
#include "WkfEnvironment.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{

constexpr double       cDELAY_TOLERANCE{1.5};
constexpr unsigned int cX_OFFSET{600};
constexpr unsigned int cY_PADDING{10};
constexpr unsigned int cROUTER_GATEWAY_X_OFFSET{500};
constexpr unsigned int cROUTER_COMM_X_OFFSET{700};

bool CommSort(wkf::CommVisCommGraphicsItem* aRhs, wkf::CommVisCommGraphicsItem* aLhs)
{
   return (aRhs->GetName().compare(aLhs->GetName(), Qt::CaseInsensitive) < 0);
}
} // namespace

wkf::CommVisDialog::CommVisDialog(DataContainer& aCommData, QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mCommData(aCommData)
{
   mUi.setupUi(this);

   mDarkTheme = (QIcon::themeName() == "Dark") ? true : false;
   mRandom.SetSeed(156);
   connect(mUi.mViewComboBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &CommVisDialog::Build);
   connect(mUi.mHideLinksCB, &QCheckBox::clicked, this, &CommVisDialog::Build);
   connect(mUi.mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::close);
   connect(this, &QDialog::finished, this, &CommVisDialog::CloseChildDialogs);

   connect(&mCommData, &DataContainer::ResetData, this, &CommVisDialog::ClearData);

   ConfigureSidePanel();
}

void wkf::CommVisDialog::CloseChildDialogs()
{
   auto            childrenVec  = children();
   QList<QDialog*> childDialogs = findChildren<QDialog*>();
   for (auto& curChild : childDialogs)
   {
      curChild->close();
   }
}

void wkf::CommVisDialog::ClearData()
{
   mInitialized = false;
   // Clear the scene
   for (auto& curSceneItem : mUi.mGraphicsView->scene()->items())
   {
      mUi.mGraphicsView->scene()->removeItem(curSceneItem);
      if (curSceneItem->type() == QGraphicsPathItem::Type)
      {
         delete curSceneItem;
      }
   }
   mMaxComms = 10000;
   mPacketList.clear();
   mLinksList.clear();

   mNetworkList.clear();
   mCommList.clear();
   mRouterList.clear();
   mGatewayList.clear();
   mNetworkCommMap.clear();
   mDynamicLinksList.clear();
}

void wkf::CommVisDialog::Update()
{
   size_t commDataSize = mCommData.GetCommData().size();
   for (size_t i = 0; i < commDataSize; ++i)
   {
      auto commData  = mCommData.GetCommData()[i];
      auto eventType = commData.GetEventType();

      switch (eventType)
      {
      case CommEventType::MessageTransmitted:
         TransmitMessage(commData);
         break;

      case CommEventType::MessageHop:
         MessageHop(commData);
         break;

      case CommEventType::AddNetwork:
         AddNetworkToScene(commData);
         break;

      case CommEventType::AddComm:
         AddCommToScene(commData);
         break;

      case CommEventType::RemoveComm:
         RemoveComm(commData);
         break;

      case CommEventType::CommTurnedOff:
         TurnOffComm(commData);
         break;

      case CommEventType::CommTurnedOn:
         TurnOnComm(commData);
         break;

      case CommEventType::LinkAddedToLocal:
         LinkAddedToLocal(commData);
         break;

      case CommEventType::LinkRemovedFromLocal:
         LinkRemovedFromLocal(commData);
         break;

      case CommEventType::PendingStartEvent:
         mInitialized = true;
         Build();
         break;

      default:
         break;
      }
   }

   mCommData.Clear();
}

void wkf::CommVisDialog::AddNetworkToScene(const QString& aNetworkName,
                                           const QString& aNetworkAddress,
                                           const QString& aNetworkType)
{
   auto newNetwork = ut::qt::make_qt_ptr<wkf::CommVisNetworkGraphicsItem>(aNetworkName, aNetworkAddress, aNetworkType);
   mNetworkList.emplace_back(newNetwork);
   if (mInitialized)
   {
      Build();
   }
}

void wkf::CommVisDialog::AddNetworkToScene(const CommEvent& aCommEvent)
{
   auto networkInfo = aCommEvent.GetNetworkInfo();
   AddNetworkToScene(QString::fromStdString(networkInfo.mNetworkName),
                     QString::fromStdString(networkInfo.mNetworkAddress),
                     QString::fromStdString(networkInfo.mNetworkType));
}

void wkf::CommVisDialog::AddCommToScene(const CommEvent& aCommEvent)
{
   auto    commInfo    = aCommEvent.GetCommInfo();
   QString commName    = QString::fromStdString(commInfo.mCommName);
   QString commAddress = QString::fromStdString(commInfo.mCommAddress);
   QString commType    = QString::fromStdString(commInfo.mCommType);

   auto    networkInfo    = aCommEvent.GetNetworkInfo();
   QString networkName    = QString::fromStdString(networkInfo.mNetworkName);
   QString networkAddress = QString::fromStdString(networkInfo.mNetworkAddress);
   QString networkType    = QString::fromStdString(networkInfo.mNetworkType);

   auto    routerInfo    = aCommEvent.GetRouterInfo();
   QString routerName    = QString::fromStdString(routerInfo.mRouterName);
   QString routerAddress = QString::fromStdString(routerInfo.mRouterAddress);

   auto    gatewayInfo    = aCommEvent.GetGatewayInfo();
   QString gatewayName    = QString::fromStdString(gatewayInfo.mGatewayName);
   QString gatewayAddress = QString::fromStdString(gatewayInfo.mGatewayAddress);
   QString platformName   = QString::fromStdString(commInfo.mPlatformName);

   auto newComm = ut::qt::make_qt_ptr<wkf::CommVisCommGraphicsItem>(commName, commAddress, commType);

   newComm->SetNetworkInfo(networkName, networkAddress);
   newComm->SetNetworkType(networkType);
   newComm->SetGatewayInfo(gatewayInfo);
   newComm->SetRouterInfo(routerName, routerAddress);
   newComm->SetLinks(aCommEvent.GetLinks());
   newComm->SetPlatform(platformName);
   newComm->SetName(platformName + "'s " + commName);
   mCommList.emplace_back(std::move(newComm));
   AddCommToNetwork(networkName, networkAddress, networkType, newComm);

   auto newRouter = ut::qt::make_qt_ptr<wkf::CommVisRouterGraphicsItem>(routerInfo);
   mRouterList.emplace_back(newRouter);

   if (!gatewayName.isEmpty())
   {
      auto newGateway = ut::qt::make_qt_ptr<wkf::CommVisGatewayGraphicsItem>(gatewayInfo);
      newGateway->SetRouterGraphicsItem(newRouter);
      mGatewayList.emplace_back(newGateway);
   }

   if (mInitialized)
   {
      Build();
   }
}

void wkf::CommVisDialog::AddCommToNetwork(const QString&           aNetworkName,
                                          const QString&           aNetworkAddress,
                                          const QString&           aNetworkType,
                                          CommVisCommGraphicsItem* aCommPtr)
{
   // Find the network graphics item
   bool networkFound = false;
   for (auto& network : mNetworkList)
   {
      if (network->GetName() == aNetworkName)
      {
         mNetworkCommMap[network.get()].emplace_back(aCommPtr);
         networkFound = true;
         break;
      }
      else if (network->GetAddress() == aNetworkAddress && network->GetName() == "default")
      {
         network->SetName(aNetworkName);
         mNetworkCommMap[network.get()].emplace_back(aCommPtr);
         networkFound = true;
         break;
      }
   }
   // If the network wasn't found, add it
   if (!networkFound)
   {
      AddNetworkToScene(aNetworkName, aNetworkAddress, aNetworkType);
      auto newNetworkPtr = mNetworkList[mNetworkList.size() - 1].get();
      mNetworkCommMap[newNetworkPtr].emplace_back(aCommPtr);
   }
}

void wkf::CommVisDialog::TransmitMessage(const CommEvent& aCommEvent)
{
   // If the dialog isn't open, don't bother creating the visual message packets
   if (!isVisible())
   {
      return;
   }

   // Don't bother creating messages that are well behind the current sim time
   if (vaEnv.GetTime()->GetCurrentTime() - aCommEvent.GetSimTime() > cDELAY_TOLERANCE)
   {
      return;
   }

   auto networkInfo = aCommEvent.GetNetworkInfo();

   auto networkName    = QString::fromStdString(networkInfo.mNetworkName);
   auto networkAddress = QString::fromStdString(networkInfo.mNetworkAddress);

   auto commInfo           = aCommEvent.GetCommInfo();
   auto commName           = QString::fromStdString(commInfo.mCommName);
   auto commAddress        = QString::fromStdString(commInfo.mCommAddress);
   auto messageDestination = QString::fromStdString(aCommEvent.GetMessageDestination());

   QColor packetColor = Qt::green;
   // First check to see if this is a message that is part of a message hop,
   auto message = aCommEvent.GetMessageInfo();

   if (mHopMessages.find(message.mSerialNumber) != mHopMessages.end())
   {
      if (message.mHopAddress == message.mDestinationAddress)
      {
         mHopMessages[message.mSerialNumber].mHopAddresses.emplace_back(message.mHopAddress);
         TransmitMessageHop(message.mSerialNumber);
      }
   }
   else if (message.mDestinationAddress == message.mHopAddress)
   {
      auto foundLink = FindLink(commAddress, messageDestination);
      if (foundLink && foundLink->GetPathItem())
      {
         auto packet = ut::make_unique<CommVisPacketGraphicsItem>(packetColor,
                                                                  commAddress,
                                                                  messageDestination,
                                                                  foundLink,
                                                                  nullptr,
                                                                  mUi.mGraphicsView->scene());
         packet->SendPacket();
         CommVisPacketGraphicsItem* packetPtr = packet.get();
         connect(packetPtr, &CommVisPacketGraphicsItem::PacketComplete, [this, packetPtr]() { RemovePacket(packetPtr); });
         mPacketList.emplace_back(std::move(packet));
      }
      else
      {
         // No direct link found, route through the network
         auto fromComm        = FindCommByAddress(commAddress);
         auto destComm        = FindCommByAddress(messageDestination);
         auto networkFromLink = FindLink(commAddress, fromComm->GetNetworkAddress());
         auto networkDestLink = FindLink(messageDestination, destComm->GetNetworkAddress());

         if (networkFromLink && networkDestLink)
         {
            if (networkFromLink->GetPathItem() && networkDestLink->GetPathItem())
            {
               auto packet = ut::make_unique<CommVisPacketGraphicsItem>(packetColor,
                                                                        commAddress,
                                                                        messageDestination,
                                                                        networkFromLink,
                                                                        networkDestLink,
                                                                        mUi.mGraphicsView->scene());
               packet->SendPacket();
               CommVisPacketGraphicsItem* packetPtr = packet.get();
               connect(packetPtr,
                       &CommVisPacketGraphicsItem::PacketComplete,
                       [this, packetPtr]() { RemovePacket(packetPtr); });
               mPacketList.emplace_back(std::move(packet));
            }
         }
      }
   }
}

void wkf::CommVisDialog::MessageHop(const CommEvent& aCommEvent)
{
   auto message = aCommEvent.GetMessageInfo();

   // If the dialog isn't open, don't bother creating the visual message packets
   if (!isVisible())
   {
      return;
   }

   // Don't bother creating messages that are well behind the current sim time
   if (vaEnv.GetTime()->GetCurrentTime() - aCommEvent.GetSimTime() > cDELAY_TOLERANCE)
   {
      return;
   }

   // If a message with this serial number exists, add to its hop list
   if (mHopMessages.find(message.mSerialNumber) != mHopMessages.end())
   {
      // Found
      mHopMessages[message.mSerialNumber].mHopAddresses.emplace_back(message.mHopAddress);
   }
   // Else this is a new hop message, add it to the hop message list
   else
   {
      // Not found
      HopMessage newHop;
      newHop.mOrigin      = message.mSourceAddress;
      newHop.mDestination = message.mDestinationAddress;
      newHop.mHopAddresses.emplace_back(message.mSourceAddress);
      newHop.mHopAddresses.emplace_back(message.mHopAddress);
      mHopMessages[message.mSerialNumber] = newHop;
   }
}

void wkf::CommVisDialog::AddLink(const CommVisGraphicsItem* aLinkFromPtr,
                                 const CommVisGraphicsItem* aLinkToPtr,
                                 const QColor&              aColor,
                                 bool                       aIsQuad,
                                 bool                       aIsDynamic)
{
   auto newLink = ut::make_unique<CommVisLink>(aLinkFromPtr, aLinkToPtr);
   if ((!mUi.mHideLinksCB->isChecked() || !newLink->IsDirectLink()) &&
       !(aIsDynamic && mUi.mViewComboBox->currentText() == "Routers"))
   {
      if (!aIsQuad)
      {
         newLink->SetLine(aColor);
      }
      else
      {
         newLink->SetQuad(aColor);
      }
      newLink->SetPathItem(mUi.mGraphicsView->scene()->addPath(newLink->GetPath(), newLink->GetPen()));
   }

   if (aIsDynamic)
   {
      mDynamicLinksList.emplace_back(std::move(newLink));
   }
   else
   {
      mLinksList.emplace_back(std::move(newLink));
   }
}

void wkf::CommVisDialog::ResetScene()
{
   // Clear the scene
   for (auto& curSceneItem : mUi.mGraphicsView->scene()->items())
   {
      mUi.mGraphicsView->scene()->removeItem(curSceneItem);
      if (curSceneItem->type() == QGraphicsPathItem::Type)
      {
         delete curSceneItem;
      }
   }

   mPacketList.clear();
   mLinksList.clear();

   for (auto& curDynamicLink : mDynamicLinksList)
   {
      curDynamicLink->SetPathItem(nullptr);
   }

   // Change the text color for all graphics items
   ChangeTextColor(mDarkTheme ? Qt::white : Qt::black);

   if (GetCurrentView() == NETWORKS_VIEW)
   {
      mUi.mHideLinksCB->show();
   }
   else
   {
      mUi.mHideLinksCB->hide();
   }
}

QColor wkf::CommVisDialog::GetThemeLinkColor() const
{
   return (mDarkTheme) ? QColor(Qt::darkYellow) : QColor(Qt::darkBlue);
}

QColor wkf::CommVisDialog::GetRandomLinkColor()
{
   return QColor(mRandom.Uniform(128, 255), mRandom.Uniform(128, 255), mRandom.Uniform(128, 255));
}

void wkf::CommVisDialog::AddGraphicsItem(wkf::CommVisGraphicsItem* aItem)
{
   mUi.mGraphicsView->AddItem(aItem);
}

void wkf::CommVisDialog::Build()
{
   ResetScene();

   if (GetCurrentView() == NETWORKS_VIEW)
   {
      AddNetworksAndCommsToScene();

      if (!LinksHidden())
      {
         AddDirectLinksToScene();
         AddNetworkTypeLinksToScene();
      }
   }

   else if (GetCurrentView() == ROUTERS_VIEW)
   {
      AddRoutersToScene();
      AddGatewaysToScene();
   }
}

void wkf::CommVisDialog::AddNetworksAndCommsToScene()
{
   int curMaxY = 0;
   for (auto& networkCommMap : mNetworkCommMap)
   {
      int numComms = ut::cast_to_int(networkCommMap.second.size());
      if (numComms == 0)
      {
         AddGraphicsItem(networkCommMap.first);
         networkCommMap.first->SetPos(QPoint(0, curMaxY));
      }
      else if (numComms == 1)
      {
         // Add the network
         AddGraphicsItem(networkCommMap.first);
         networkCommMap.first->SetPos(QPoint(0, curMaxY));

         // Add the comm straight across from it
         auto curComm = networkCommMap.second[0];
         if (curComm)
         {
            AddGraphicsItem(curComm);
            curComm->SetPos(QPoint(cX_OFFSET, curMaxY));

            AddLink(networkCommMap.first, curComm, GetThemeLinkColor());
         }
      }
      else
      {
         int commImgHeight = networkCommMap.second[0]->pixmap().height();

         // Calculate the starting y position of this network
         int networkYPos = curMaxY + (commImgHeight * numComms);

         // Add the network
         AddGraphicsItem(networkCommMap.first);
         networkCommMap.first->SetPos(QPoint(0, networkYPos));

         // sort the comms in alphabetical order
         std::sort(networkCommMap.second.begin(), networkCommMap.second.end(), CommSort);

         // Figure out starting y position based on the number of members
         int halfMembers = numComms / 2;
         int yPosIdx     = 0;
         for (int i = halfMembers - 1; i >= 0; --i)
         {
            auto curComm = networkCommMap.second[i];
            if (curComm)
            {
               int yPos = networkYPos + (-1 * ((yPosIdx + 1) * commImgHeight * 2)) - cY_PADDING;
               AddGraphicsItem(curComm);
               curComm->SetPos(QPoint(cX_OFFSET, yPos));
               AddLink(networkCommMap.first, curComm, GetThemeLinkColor(), true);
               ++yPosIdx;
            }
         }

         // Bottom half of the graph
         for (int i = halfMembers; i < numComms; ++i)
         {
            auto curComm = networkCommMap.second[i];
            if (curComm)
            {
               int yPos = networkYPos + (((i - halfMembers) + 1) * commImgHeight * 2) + cY_PADDING;
               AddGraphicsItem(curComm);
               curComm->SetPos(QPoint(cX_OFFSET, yPos));
               AddLink(networkCommMap.first, curComm, GetThemeLinkColor(), true);
               curMaxY = yPos;
            }
         }
      }
      curMaxY += ((networkCommMap.first->pixmap().height() * 2) + cY_PADDING);
   }

   // There may be some networks that have no comms attached, find them and add them to the scene
   for (auto& network : mNetworkList)
   {
      bool networkFound = false;
      // check to see that this network doesn't already exist in the scene
      QList<QGraphicsItem*> sceneItems = mUi.mGraphicsView->scene()->items();
      for (auto& curItem : sceneItems)
      {
         wkf::CommVisNetworkGraphicsItem* curNet = dynamic_cast<wkf::CommVisNetworkGraphicsItem*>(curItem);
         if (curNet)
         {
            if (curNet->GetName() == network->GetName())
            {
               networkFound = true;
               break;
            }
         }
      }
      if (!networkFound)
      {
         AddGraphicsItem(network.get());
         network.get()->SetPos(QPoint(0, curMaxY));
         curMaxY += ((network->pixmap().height() * 2) + cY_PADDING);
      }
   }
}

void wkf::CommVisDialog::AddRoutersToScene()
{
   int curMaxY = 0;
   for (auto& curRouter : mRouterList)
   {
      AddGraphicsItem(curRouter.get());
      curRouter->SetPos(QPoint(0, curMaxY));
      curMaxY += ((curRouter->pixmap().height() * 2) + cY_PADDING);
   }
}

void wkf::CommVisDialog::AddGatewaysToScene()
{
   int curMaxY = 0;
   for (auto& curGateway : mGatewayList)
   {
      AddGraphicsItem(curGateway.get());
      curGateway->SetPos(QPoint(cROUTER_GATEWAY_X_OFFSET, curGateway->GetRouterGraphicsItem()->pos().y()));
      AddLink(curGateway->GetRouterGraphicsItem(), curGateway.get(), GetThemeLinkColor());

      // Check to see if there is a remote comm address and if so, look for it
      if (!curGateway->GetGatewayRemoteAddress().isEmpty())
      {
         for (auto& curComm : mCommList)
         {
            if (curComm->GetAddress() == curGateway->GetGatewayRemoteAddress())
            {
               AddGraphicsItem(curComm.get());
               curComm->SetPos(QPoint(cROUTER_COMM_X_OFFSET, curGateway->GetRouterGraphicsItem()->pos().y()));
               AddLink(curGateway.get(), curComm.get(), GetThemeLinkColor());
               break;
            }
         }
      }
      // If no address is given, check the remote comm name
      if (!curGateway->GetGatewayRemoteCommName().isEmpty())
      {
         for (auto& curComm : mCommList)
         {
            if (curComm->GetName() == curGateway->GetGatewayRemoteCommName() &&
                (curComm->GetPlatformName() != curGateway->GetPlatformName()))
            {
               if (!curComm->scene())
               {
                  AddGraphicsItem(curComm.get());
                  curComm->SetPos(QPoint(cROUTER_COMM_X_OFFSET, curGateway->GetRouterGraphicsItem()->pos().y()));
               }
               AddLink(curGateway.get(), curComm.get(), GetThemeLinkColor());
               break;
            }
         }
      }
      curMaxY += ((curGateway->pixmap().height() * 2) + cY_PADDING);
   }
}

void wkf::CommVisDialog::AddDirectLinksToScene()
{
   // Go through the list of comms and add any direct links
   for (auto& curComm : mCommList)
   {
      for (auto& curLink : curComm->GetLinks())
      {
         QString linkToCommName = QString::fromStdString(curLink.mDestinationCommName);
         for (auto& curLinkComm : mCommList)
         {
            if ((curLinkComm->GetName() == linkToCommName) && (curLinkComm->GetAddress() != curComm->GetAddress()))
            {
               if (!FindLink(curLinkComm->GetAddress(), curComm->GetAddress()))
               {
                  AddLink(curLinkComm.get(), curComm.get(), GetThemeLinkColor(), true);
               }
            }
         }
      }
   }

   // Check for dynamically added links
   for (auto& curDynamicLink : mDynamicLinksList)
   {
      if (curDynamicLink->GetPath().length() == 0)
      {
         CommVisCommGraphicsItem* fromComm = FindCommByAddress(curDynamicLink->GetFromDeviceAddress());
         CommVisCommGraphicsItem* toComm   = FindCommByAddress(curDynamicLink->GetToDeviceAddress());

         RemoveLink(curDynamicLink->GetFromDeviceAddress(), curDynamicLink->GetToDeviceAddress(), true);
         AddLink(fromComm, toComm, GetThemeLinkColor(), true, true);
      }

      else
      {
         curDynamicLink->SetPathItem(
            mUi.mGraphicsView->scene()->addPath(curDynamicLink->GetPath(), curDynamicLink->GetPen()));
      }
   }
}

void wkf::CommVisDialog::AddNetworkTypeLinksToScene()
{
   // Link any comms together as defined by the network type they reside in
   for (const auto& networkCommMap : mNetworkCommMap)
   {
      int numComms = ut::cast_to_int(networkCommMap.second.size());

      if (numComms > 1 && GetMaxComms() > 0)
      {
         auto curNetwork = networkCommMap.first;
         if (curNetwork->GetNetworkType() == "WSF_COMM_NETWORK_MESH")
         {
            for (int i = 0; i < numComms; ++i)
            {
               auto curComm = networkCommMap.second[i];
               // In a mesh network all comms are connected to each other
               for (int j = i + 1; j < numComms; ++j)
               {
                  auto nextComm = networkCommMap.second[j];
                  AddLink(curComm, nextComm, GetRandomLinkColor(), true);
                  SetMaxComms(GetMaxComms() - 1);
                  if (GetMaxComms() <= 0)
                  {
                     break;
                  }
               }
            }

            if (GetMaxComms() <= 0)
            {
               QString msg = "The network has more than the max displayable comm-to-comm connection lines.  The number "
                             "of connection lines has been capped to display 10,000 of these lines.  Therefore, while "
                             "the comm to comm connections do exist in the simulation, this is not an entirely "
                             "accurate visual representation of the network.";
               QMessageBox::warning(this, tr("Comms Visualization Plugin"), msg, QMessageBox::Ok, QMessageBox::Ok);
               break;
            }
         }
      }
   }
}

void wkf::CommVisDialog::changeEvent(QEvent* aEvent)
{
   // If there has been a theme change, force a redraw
   QWidget::changeEvent(aEvent);
   if (aEvent->type() == QEvent::StyleChange)
   {
      mDarkTheme = (QIcon::themeName() == "Dark") ? false : true;
      Build();
   }
}

void wkf::CommVisDialog::ChangeTextColor(const QColor& aTextColor)
{
   for (auto& curNetwork : mNetworkList)
   {
      curNetwork->GetNameGraphicsItem()->setDefaultTextColor(aTextColor);
   }
   for (auto& curComm : mCommList)
   {
      curComm->GetNameGraphicsItem()->setDefaultTextColor(aTextColor);
   }
   for (auto& curRouter : mRouterList)
   {
      curRouter->GetNameGraphicsItem()->setDefaultTextColor(aTextColor);
   }
   for (auto& curGateway : mGatewayList)
   {
      curGateway->GetNameGraphicsItem()->setDefaultTextColor(aTextColor);
   }
}


void wkf::CommVisDialog::RemovePacket(CommVisPacketGraphicsItem* aPacket)
{
   for (auto packetIter = mPacketList.begin(); packetIter != mPacketList.end(); ++packetIter)
   {
      if (packetIter->get() == aPacket)
      {
         mUi.mGraphicsView->scene()->removeItem(packetIter->get());
         mPacketList.erase(packetIter);
         return;
      }
   }
}

void wkf::CommVisDialog::TurnOffComm(const CommEvent& aCommEvent)
{
   auto foundComm = FindCommByAddress(QString::fromStdString(aCommEvent.GetCommInfo().mCommAddress));
   if (foundComm)
   {
      foundComm->TurnOff();
   }
}

void wkf::CommVisDialog::RemoveComm(const CommEvent& aCommEvent)
{
   for (auto commIter = mCommList.begin(); commIter != mCommList.end(); ++commIter)
   {
      if (commIter->get()->GetAddress().toStdString() == aCommEvent.GetCommInfo().mCommAddress)
      {
         RemoveLinksFromDevice(commIter->get()->GetAddress());
         mCommList.erase(commIter);
         break;
      }
   }

   // Clear and rebuild the networkComm map
   mNetworkCommMap.clear();
   for (auto commIter = mCommList.begin(); commIter != mCommList.end(); ++commIter)
   {
      AddCommToNetwork(commIter->get()->GetNetworkName(),
                       commIter->get()->GetNetworkAddress(),
                       commIter->get()->GetNetworkType(),
                       commIter->get());
   }
   Build();
}

void wkf::CommVisDialog::TurnOnComm(const CommEvent& aCommEvent)
{
   auto foundComm = FindCommByAddress(QString::fromStdString(aCommEvent.GetCommInfo().mCommAddress));
   if (foundComm)
   {
      foundComm->TurnOn();
   }
}

void wkf::CommVisDialog::LinkAddedToLocal(const CommEvent& aCommEvent)
{
   for (auto& curLink : aCommEvent.GetLinks())
   {
      auto foundComm1 = FindCommByAddress(QString::fromStdString(curLink.mSourceAddress));
      auto foundComm2 = FindCommByAddress(QString::fromStdString(curLink.mDestinationAddress));
      if (foundComm1 && foundComm2)
      {
         AddLink(foundComm1, foundComm2, Qt::darkYellow, true, true);
      }
   }
}

void wkf::CommVisDialog::LinkRemovedFromLocal(const CommEvent& aCommEvent)
{
   for (auto& curLink : aCommEvent.GetLinks())
   {
      RemoveLink(QString::fromStdString(curLink.mSourceAddress), QString::fromStdString(curLink.mDestinationAddress), true);
   }
}

wkf::CommVisCommGraphicsItem* wkf::CommVisDialog::FindCommByAddress(const QString& aCommAddr)
{
   for (auto& curComm : mCommList)
   {
      if (curComm->GetAddress() == aCommAddr)
      {
         return curComm.get();
      }
   }
   return nullptr;
}

void wkf::CommVisDialog::RemoveLinksFromDevice(const QString& aDeviceAddress)
{
   for (auto&& linksIter = mLinksList.begin(); linksIter != mLinksList.end(); ++linksIter)
   {
      auto curLink = linksIter;
      if (curLink->get()->GetFromDeviceAddress() == aDeviceAddress || curLink->get()->GetToDeviceAddress() == aDeviceAddress)
      {
         mLinksList.erase(linksIter);
         linksIter = mLinksList.begin();
      }
   }
}

void wkf::CommVisDialog::RemoveLink(const QString& aSourceAddress, const QString& aDestinationAddress, bool aIsDynamic)
{
   if (!aIsDynamic)
   {
      for (auto&& linksIter = mLinksList.begin(); linksIter != mLinksList.end(); ++linksIter)
      {
         auto curLink = linksIter;
         if ((curLink->get()->GetFromDeviceAddress() == aSourceAddress) &&
             (curLink->get()->GetToDeviceAddress() == aDestinationAddress))
         {
            RemovePacketsOnLink(curLink->get());
            mUi.mGraphicsView->scene()->removeItem(curLink->get()->GetPathItem());
            mLinksList.erase(linksIter);
            break;
         }
         else if ((curLink->get()->GetFromDeviceAddress() == aDestinationAddress) &&
                  (curLink->get()->GetToDeviceAddress() == aSourceAddress))
         {
            RemovePacketsOnLink(curLink->get());
            mUi.mGraphicsView->scene()->removeItem(curLink->get()->GetPathItem());
            mLinksList.erase(linksIter);
            break;
         }
      }
   }
   else
   {
      for (auto&& linksIter = mDynamicLinksList.begin(); linksIter != mDynamicLinksList.end(); ++linksIter)
      {
         auto curLink = linksIter;
         if ((curLink->get()->GetFromDeviceAddress() == aSourceAddress) &&
             (curLink->get()->GetToDeviceAddress() == aDestinationAddress))
         {
            RemovePacketsOnLink(curLink->get());
            mUi.mGraphicsView->scene()->removeItem(curLink->get()->GetPathItem());
            mDynamicLinksList.erase(linksIter);
            break;
         }
         else if ((curLink->get()->GetFromDeviceAddress() == aDestinationAddress) &&
                  (curLink->get()->GetToDeviceAddress() == aSourceAddress))
         {
            RemovePacketsOnLink(curLink->get());
            mUi.mGraphicsView->scene()->removeItem(curLink->get()->GetPathItem());
            mDynamicLinksList.erase(linksIter);
            break;
         }
      }
   }
}

wkf::CommVisLink* wkf::CommVisDialog::FindLink(const QString& aSourceAddress, const QString& aDestinationAddress) const
{
   // Check any dynamically added links first
   for (const auto& curLink : mDynamicLinksList)
   {
      if ((curLink->GetFromDeviceAddress() == aSourceAddress) && (curLink->GetToDeviceAddress() == aDestinationAddress))
      {
         return curLink.get();
      }
      else if ((curLink->GetFromDeviceAddress() == aDestinationAddress) && (curLink->GetToDeviceAddress() == aSourceAddress))
      {
         return curLink.get();
      }
   }

   // If a dynamic link wasn't found, check the other links
   for (const auto& curLink : mLinksList)
   {
      if ((curLink->GetFromDeviceAddress() == aSourceAddress) && (curLink->GetToDeviceAddress() == aDestinationAddress))
      {
         return curLink.get();
      }
      else if ((curLink->GetFromDeviceAddress() == aDestinationAddress) && (curLink->GetToDeviceAddress() == aSourceAddress))
      {
         return curLink.get();
      }
   }
   return nullptr;
}

void wkf::CommVisDialog::TransmitMessageHop(const unsigned int aMessageSerialNumber)
{
   auto hopMessage = mHopMessages[aMessageSerialNumber];
   if (hopMessage.mHopAddresses.size() <= 1)
   {
      // If there are no more hops associated with this message, remove it from the hop list
      mHopMessages.erase(aMessageSerialNumber);
      return;
   }

   QColor                                     packetColor = Qt::green;
   std::unique_ptr<CommVisPacketGraphicsItem> packet      = nullptr;
   auto                                       frontIter   = hopMessage.mHopAddresses.begin();
   auto                                       fromAddress = QString::fromStdString(*frontIter);
   ++frontIter;
   auto destAddress = QString::fromStdString(*frontIter);
   auto foundLink   = FindLink(fromAddress, destAddress);
   if (!foundLink)
   {
      // No direct link found, route through the network
      auto fromComm        = FindCommByAddress(fromAddress);
      auto destComm        = FindCommByAddress(destAddress);
      auto networkFromLink = FindLink(fromAddress, fromComm->GetNetworkAddress());
      auto networkDestLink = FindLink(destAddress, destComm->GetNetworkAddress());

      if (networkFromLink && networkDestLink)
      {
         if (networkFromLink->GetPathItem() && networkDestLink->GetPathItem())
            packet = ut::make_unique<CommVisPacketGraphicsItem>(packetColor,
                                                                fromAddress,
                                                                destAddress,
                                                                networkFromLink,
                                                                networkDestLink,
                                                                mUi.mGraphicsView->scene());
      }
   }
   else
   {
      if (foundLink->GetPathItem())
      {
         packet = ut::make_unique<CommVisPacketGraphicsItem>(packetColor,
                                                             fromAddress,
                                                             destAddress,
                                                             foundLink,
                                                             nullptr,
                                                             mUi.mGraphicsView->scene());
      }
   }

   if (packet)
   {
      packet->SendPacket();
      if (++frontIter != hopMessage.mHopAddresses.end())
      {
         packet->SetFade(false);
      }
      CommVisPacketGraphicsItem* packetPtr = packet.get();
      mHopMessages[aMessageSerialNumber].mHopAddresses.pop_front();
      connect(packetPtr,
              &CommVisPacketGraphicsItem::PacketComplete,
              [this, packetPtr, aMessageSerialNumber]()
              {
                 RemovePacket(packetPtr);
                 TransmitMessageHop(aMessageSerialNumber);
              });
      mPacketList.emplace_back(std::move(packet));
   }
}
void wkf::CommVisDialog::RemovePacketsOnLink(const CommVisLink* aLink)
{
   // Gather any packets active on this link so they can be removed
   std::vector<CommVisPacketGraphicsItem*> removePacketList;
   for (auto& curPacket : mPacketList)
   {
      if (curPacket->GetStageOneLink() == aLink || curPacket->GetStageTwoLink() == aLink)
      {
         removePacketList.emplace_back(curPacket.get());
      }
   }
   for (auto& curPacket : removePacketList)
   {
      RemovePacket(curPacket);
   }
}

void wkf::CommVisDialog::ConfigureSidePanel()
{
   AddSidePanelButton("Network");
   AddSidePanelButton("Comm");
   AddSidePanelButton("Router");
   AddSidePanelButton("Gateway");
}


void wkf::CommVisDialog::AddSidePanelButton(const QString& aButtonName)
{
   QString buttonIconName = ":/icons/" + aButtonName.toLower() + "_shadow.png";

   QPointer<QLabel> newButton = new QLabel(this);
   newButton->setObjectName(aButtonName);
   newButton->setPixmap(QPixmap(buttonIconName));
   newButton->setAlignment(Qt::AlignCenter);

   mUi.mSidePanelLayout->addWidget(newButton);

   QPointer<QLabel> newButtonLabel = new QLabel(this);
   newButtonLabel->setObjectName(aButtonName + "Label");
   newButtonLabel->setAlignment(Qt::AlignCenter);
   newButtonLabel->setText(aButtonName);
   mUi.mSidePanelLayout->addWidget(newButtonLabel);

   QSpacerItem* newButtonSpacer =
      new QSpacerItem(20, 15, QSizePolicy::Minimum, QSizePolicy::Expanding); // QSpacerItem does not inherit from QObject,
                                                                             // therefore QPointer cannot be used here

   mUi.mSidePanelLayout->addItem(newButtonSpacer);
}
