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

#ifndef WKFCOMMVISDIALOG_HPP
#define WKFCOMMVISDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <map>
#include <memory>
#include <set>

#include <QDialog>
#include <QMenu>
#include <QString>

#include "UtRandom.hpp"
#include "WkfCommVisCommGraphicsItem.hpp"
#include "WkfCommVisDataContainer.hpp"
#include "WkfCommVisGatewayGraphicsItem.hpp"
#include "WkfCommVisGraphicsItem.hpp"
#include "WkfCommVisLink.hpp"
#include "WkfCommVisNetworkGraphicsItem.hpp"
#include "WkfCommVisPacketGraphicsItem.hpp"
#include "WkfCommVisRouterGraphicsItem.hpp"
#include "ui_WkfCommVisPluginDialog.h"

namespace wkf
{

class WKF_COMM_VIS_COMMON_EXPORT CommVisDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisDialog(DataContainer& aCommData, QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   enum CommVisViewType
   {
      NETWORKS_VIEW,
      ROUTERS_VIEW
   };

   void         Update();
   virtual void Build();
   void         AddGraphicsItem(wkf::CommVisGraphicsItem* aItem);
   virtual void AddCommToScene(const CommEvent& aCommEvent);
   void         AddNetworkToScene(const CommEvent& aCommEvent);

   virtual void AddNetworkToScene(const QString& aNetworkName, const QString& aNetworkAddress, const QString& aNetworkType);

   virtual void AddCommToNetwork(const QString&           aNetworkName,
                                 const QString&           aNetworkAddress,
                                 const QString&           aNetworkType,
                                 CommVisCommGraphicsItem* aCommPtr);
   void         RemoveComm(const CommEvent& aCommEvent);

   void TransmitMessage(const CommEvent& aCommEvent);
   void TransmitMessageHop(const unsigned int aMessageSerialNumber);
   void MessageHop(const CommEvent& aCommEvent);
   void TurnOffComm(const CommEvent& aCommEvent);
   void TurnOnComm(const CommEvent& aCommEvent);
   void LinkAddedToLocal(const CommEvent& aCommEvent);
   void LinkRemovedFromLocal(const CommEvent& aCommEvent);

   void AddLink(const CommVisGraphicsItem* aLinkFromPtr,
                const CommVisGraphicsItem* aLinkToPtr,
                const QColor&              aColor     = Qt::yellow,
                bool                       aIsQuad    = false,
                bool                       aIsDynamic = false);

   void RemoveLinksFromDevice(const QString& aDeviceAddress);
   void RemoveLink(const QString& aSourceAddress, const QString& aDestinationAddress, bool aIsDynamic = false);

   CommVisLink* FindLink(const QString& aSourceAddress, const QString& aDestinationAddress) const;

   void changeEvent(QEvent* aEvent) override;

   CommVisCommGraphicsItem* FindCommByAddress(const QString& aCommAddr);
   void                     ResetScene();
   QColor                   GetThemeLinkColor() const;
   QColor                   GetRandomLinkColor();
   unsigned int             GetCurrentView() const { return mUi.mViewComboBox->currentIndex(); }
   bool                     LinksHidden() const { return mUi.mHideLinksCB->isChecked(); }
   bool                     Initialized() const { return mInitialized; }

   unsigned int GetMaxComms() const { return mMaxComms; }
   void         SetMaxComms(unsigned int aMaxComms) { mMaxComms = aMaxComms; }

protected:
   void         CloseChildDialogs();
   void         ClearData();
   void         AddNetworksAndCommsToScene();
   void         AddRoutersToScene();
   void         AddGatewaysToScene();
   virtual void AddDirectLinksToScene();
   void         AddNetworkTypeLinksToScene();
   virtual void ConfigureSidePanel();
   virtual void AddSidePanelButton(const QString& aButtonName);

   std::vector<std::unique_ptr<CommVisNetworkGraphicsItem>> mNetworkList;
   std::vector<std::unique_ptr<CommVisCommGraphicsItem>>    mCommList;
   std::vector<std::unique_ptr<CommVisRouterGraphicsItem>>  mRouterList;
   std::vector<std::unique_ptr<CommVisGatewayGraphicsItem>> mGatewayList;
   std::vector<std::unique_ptr<CommVisLink>>                mLinksList; // Links that are added based on network
   std::vector<std::unique_ptr<CommVisLink>> mDynamicLinksList; // Links that are added dynamically during the scenario
   std::vector<std::unique_ptr<CommVisPacketGraphicsItem>> mPacketList;

   struct NetworkSort
   {
      bool operator()(CommVisNetworkGraphicsItem* aRhs, CommVisNetworkGraphicsItem* aLhs) const
      {
         return (aRhs->GetName().compare(aLhs->GetName(), Qt::CaseInsensitive) < 0);
      }
   };

   std::map<CommVisNetworkGraphicsItem*, std::vector<CommVisCommGraphicsItem*>, NetworkSort> mNetworkCommMap;
   ut::Random                                                                                mRandom;
   Ui::CommVisPluginDialog                                                                   mUi;

private:
   void ChangeTextColor(const QColor& aTextColor);
   void RemovePacket(CommVisPacketGraphicsItem* aPacket);
   void RemovePacketsOnLink(const CommVisLink* aLink);


   struct HopMessage
   {
      std::string            mOrigin;
      std::string            mDestination;
      std::list<std::string> mHopAddresses;
   };

   std::map<unsigned int, HopMessage> mHopMessages; // Message serial number, list of addresses the message hops to

   bool           mDarkTheme{true};
   unsigned int   mMaxComms{10000};
   bool           mInitialized{false};
   DataContainer& mCommData;
};
} // namespace wkf

#endif
