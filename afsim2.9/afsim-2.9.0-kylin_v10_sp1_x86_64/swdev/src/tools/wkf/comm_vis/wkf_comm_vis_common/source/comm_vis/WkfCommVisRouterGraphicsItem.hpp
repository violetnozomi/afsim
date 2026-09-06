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

#ifndef WKFCOMMVISROUTERGRAPHICSITEM_HPP
#define WKFCOMMVISROUTERGRAPHICSITEM_HPP

#include "wkf_comm_vis_common_export.h"

#include <memory>

#include <QPointer>

#include "WkfCommVisDataContainer.hpp"
#include "WkfCommVisGraphicsItem.hpp"
#include "WkfCommVisRouterNodeDialog.hpp"
namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisRouterGraphicsItem : public CommVisGraphicsItem
{
public:
   explicit CommVisRouterGraphicsItem(const CommEvent::RouterInfo& aRouterInfo);
   ~CommVisRouterGraphicsItem() override = default;

   const QString& GetName() const override { return mRouterName; }
   const QString& GetAddress() const override { return mGatewayAddress; }
   const QString& GetPlatform() const { return mPlatformName; }
   const QString& GetCommName() const { return mCommName; }
   const QString& GetRouterType() const { return mRouterType; }
   const QString& GetGateway() const { return mGateway; }
   double         GetHopLimit() const { return mHopLimit; }
   bool           UseDefaultProtocol() const { return mUseDefaultProtocol; }
   bool           UseMulticastProtocol() const { return mUseMulticastProtocol; }
   bool           UseAutomatedInterfaceLinking() const { return mHasAutomatedInterfaceLinking; }

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent) override;
   void DialogClosed() override;

private:
   QString mRouterName;
   QString mPlatformName;
   QString mCommName;
   QString mGatewayAddress;
   QString mGateway;
   QString mRouterType;

   int  mHopLimit{64};
   bool mHasAutomatedInterfaceLinking{false};
   bool mUseDefaultProtocol{true};
   bool mUseMulticastProtocol{true};

   QPointer<wkf::CommVisRouterNodeDialog> mDialogWindowPtr;
};
} // namespace wkf

#endif
