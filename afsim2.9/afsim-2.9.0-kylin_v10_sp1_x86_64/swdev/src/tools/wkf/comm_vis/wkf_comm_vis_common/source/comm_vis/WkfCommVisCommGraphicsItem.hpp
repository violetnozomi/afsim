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

#ifndef WKFCOMMVISCOMMGRAPHICSITEM_HPP
#define WKFCOMMVISCOMMGRAPHICSITEM_HPP

#include "wkf_comm_vis_common_export.h"

#include <QPointer>

#include "WkfCommVisCommNodeDialog.hpp"
#include "WkfCommVisDataContainer.hpp"
#include "WkfCommVisGraphicsItem.hpp"
namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisCommGraphicsItem : public CommVisGraphicsItem
{
public:
   CommVisCommGraphicsItem(const QString& aCommName, const QString& aCommAddress, const QString& aCommType);

   ~CommVisCommGraphicsItem() override = default;

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent) override;

   void           DialogClosed() override;
   const QString& GetName() const override { return mCommName; }
   const QString& GetAddress() const override { return mCommAddress; }
   const QString& GetType() const { return mCommType; }
   const QString& GetNetworkName() const { return mNetworkInfo.first; }
   const QString& GetNetworkAddress() const { return mNetworkInfo.second; }
   const QString& GetNetworkType() const { return mNetworkType; }
   const QString& GetRouterName() const { return mRouterInfo.first; }
   const QString& GetRouterAddress() const { return mRouterInfo.second; }
   const QString  GetGatewayName() const { return QString::fromStdString(mGatewayInfo.mGatewayName); }
   const QString  GetGatewayAddress() const { return QString::fromStdString(mGatewayInfo.mGatewayAddress); }
   const QString  GetGatewayPlatform() const { return QString::fromStdString(mGatewayInfo.mGatewayRemotePlatform); }
   const QString& GetPlatformName() const { return mPlatformName; }
   const std::vector<CommEvent::LinkInfo>& GetLinks() const { return mLinks; }

   void SetLinks(const std::vector<CommEvent::LinkInfo>& aLinks) { mLinks = aLinks; }

   void SetPlatform(const QString& aPlatformName) { mPlatformName = aPlatformName; }

   void SetNetworkInfo(const QString& aNetworkName, const QString& aNetworkAddress)
   {
      mNetworkInfo = std::make_pair(aNetworkName, aNetworkAddress);
   }

   void SetRouterInfo(const QString& aRouterName, const QString& aRouterAddress)
   {
      mRouterInfo = std::make_pair(aRouterName, aRouterAddress);
   }

   void SetGatewayInfo(const wkf::CommEvent::GatewayInfo& aGatewayInfo) { mGatewayInfo = aGatewayInfo; }

   void SetNetworkType(const QString& aNetworkType) { mNetworkType = aNetworkType; }

   void SetOnOffStatus(bool aOn);

   void RemoveComm();

private:
   QString mCommName;
   QString mCommAddress;
   QString mCommType;
   QString mPlatformName;
   QString mNetworkType;
   bool    mIsTurnedOn = true;

   std::pair<QString, QString> mNetworkInfo;
   std::pair<QString, QString> mCommInfo;
   std::pair<QString, QString> mMessageInfo;
   std::pair<QString, QString> mRouterInfo;
   wkf::CommEvent::GatewayInfo mGatewayInfo;

   std::vector<CommEvent::LinkInfo>     mLinks;
   QPointer<wkf::CommVisCommNodeDialog> mDialogWindowPtr;
};
} // namespace wkf

#endif
