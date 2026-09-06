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

#ifndef WKFCOMMVISGATEWAYGRAPHICSITEM_HPP
#define WKFCOMMVISGATEWAYGRAPHICSITEM_HPP

#include "wkf_comm_vis_common_export.h"

#include <QPointer>

#include "WkfCommVisDataContainer.hpp"
#include "WkfCommVisGatewayNodeDialog.hpp"
#include "WkfCommVisGraphicsItem.hpp"
#include "WkfCommVisRouterGraphicsItem.hpp"

namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisGatewayGraphicsItem : public CommVisGraphicsItem
{
public:
   CommVisGatewayGraphicsItem(const CommEvent::GatewayInfo& aGatewayInfo);
   ~CommVisGatewayGraphicsItem() override = default;
   const QString& GetName() const override { return mGatewayName; }
   const QString& GetAddress() const override { return mGatewayAddress; }
   const QString& GetPlatformName() const { return mPlatformName; }
   const QString& GetRouterName() const { return mRouterName; }
   const QString& GetRemotePlatform() const { return mGatewayRemotePlatform; }
   const QString& GetRemoteComm() const { return mGatewayRemoteComm; }
   const QString& GetRemoteAddress() const { return mGatewayRemoteAddress; }


   void SetRouterGraphicsItem(const CommVisRouterGraphicsItem* aRouterPtr) { mRouterPtr = aRouterPtr; }
   const CommVisRouterGraphicsItem* GetRouterGraphicsItem() const { return mRouterPtr; }
   void                             SetRouterName(const QString& aRouterName) { mRouterName = aRouterName; }
   void SetGatewayRemoteAddress(const QString& aGatewayRemoteAddress) { mGatewayRemoteAddress = aGatewayRemoteAddress; }
   QString GetGatewayRemoteAddress() const { return mGatewayRemoteAddress; }
   QString GetGatewayRemoteCommName() const { return mGatewayRemoteComm; }

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent) override;
   void DialogClosed() override;

   void SetRemoteInterface(const QString& aRemotePlatformName, const QString& aRemoteCommName);

private:
   QString mPlatformName;
   QString mGatewayName;
   QString mGatewayAddress;
   QString mRouterName;
   QString mGatewayRemotePlatform;
   QString mGatewayRemoteAddress;
   QString mGatewayRemoteComm;

   const CommVisRouterGraphicsItem*        mRouterPtr;
   QPointer<wkf::CommVisGatewayNodeDialog> mDialogWindowPtr;
};
} // namespace wkf

#endif
