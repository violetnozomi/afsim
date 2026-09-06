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

#ifndef WKFCOMMVISNETWORKGRAPHICSITEM_HPP
#define WKFCOMMVISNETWORKGRAPHICSITEM_HPP

#include "wkf_comm_vis_common_export.h"

#include <QPointer>

#include "WkfCommVisGraphicsItem.hpp"
#include "WkfCommVisNetworkNodeDialog.hpp"

namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisNetworkGraphicsItem : public CommVisGraphicsItem
{
public:
   CommVisNetworkGraphicsItem(const QString& aNetworkName, const QString& aNetworkAddress, const QString& aNetworkType);

   ~CommVisNetworkGraphicsItem() override = default;

   void           mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent) override;
   void           DialogClosed() override;
   void           SetName(const QString& aName) override;
   const QString& GetName() const override { return mNetworkName; }
   const QString& GetAddress() const override { return mNetworkAddress; }
   const QString& GetNetworkType() const { return mNetworkType; }

protected:
   QString                           mNetworkName;
   QString                           mNetworkAddress;
   QString                           mNetworkType;
   static std::map<QString, QString> mNetworkTypeMap;

private:
   QPointer<wkf::CommVisNetworkNodeDialog> mDialogWindowPtr;
};
} // namespace wkf

#endif
