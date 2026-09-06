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

#ifndef WKFCOMMVISDATACONTAINER_HPP
#define WKFCOMMVISDATACONTAINER_HPP

#include "wkf_comm_vis_common_export.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <QObject>

namespace wkf
{
enum class CommEventType
{
   AddNetwork,
   AddComm,
   AddRouter,
   RemoveComm,
   MessageTransmitted,
   MessageHop,
   CommTurnedOff,
   CommTurnedOn,
   CommAddedToLocal,
   CommRemovedFromLocal,
   LinkAddedToLocal,
   LinkRemovedFromLocal,
   PendingStartEvent
};

class WKF_COMM_VIS_COMMON_EXPORT CommEvent
{
public:
   CommEvent(double aSimTime, const CommEventType& aEventType)
      : mSimTime(aSimTime)
      , mEventType(aEventType)
   {
   }

   struct RouterInfo
   {
      std::string mRouterName;
      std::string mRouterAddress;
      std::string mRouterType;
      std::string mGatewayAddress;
      std::string mGatewayComm;
      std::string mPlatformName;
      std::string mCommName;
      double      mHopLimit;
      bool        mUsingMulticastInterface;
      bool        mUsingDefaultInterface;
   };

   struct CommInfo
   {
      std::string mCommName;
      std::string mCommAddress;
      std::string mPlatformName;
      std::string mCommType;
   };

   struct NetworkInfo
   {
      std::string mNetworkName;
      std::string mNetworkAddress;
      std::string mNetworkType;
   };

   struct GatewayInfo
   {
      std::string mPlatformName;
      std::string mGatewayName;
      std::string mGatewayAddress;
      std::string mGatewayRemotePlatform;
      std::string mGatewayRemoteComm;
      std::string mGatewayRemoteAddress;
   };

   struct MessageInfo
   {
      unsigned int mSerialNumber;
      std::string  mSourceAddress;
      std::string  mDestinationAddress;
      std::string  mHopAddress;
      std::string  mMessageStr;
   };

   struct LinkInfo
   {
      std::string mSourcePlatformName;
      std::string mSourceCommName;
      std::string mSourceAddress;
      std::string mDestinationPlatformName;
      std::string mDestinationCommName;
      std::string mDestinationAddress;
   };

   void SetMessage(const MessageInfo& aMessageInfo) { mMessageInfo = aMessageInfo; }

   void SetLinks(const std::vector<LinkInfo>& aLinks) { mLinks = aLinks; }

   void SetPlatform(const std::string& aPlatformName) { mPlatformName = aPlatformName; }

   void SetNetworkInfo(const NetworkInfo& aNetworkInfo) { mNetworkInfo = aNetworkInfo; }

   void SetCommInfo(const CommInfo& aCommInfo) { mCommInfo = aCommInfo; }

   void SetRouterInfo(const RouterInfo& aRouterInfo) { mRouterInfo = aRouterInfo; }

   void SetGatewayInfo(const GatewayInfo& aGatewayInfo) { mGatewayInfo = aGatewayInfo; }

   const std::string&           GetPlatformName() const { return mPlatformName; }
   const std::string&           GetMessage() const { return mMessageInfo.mMessageStr; }
   const std::string&           GetMessageDestination() const { return mMessageInfo.mDestinationAddress; }
   const std::vector<LinkInfo>& GetLinks() const { return mLinks; }

   const NetworkInfo GetNetworkInfo() const { return mNetworkInfo; }
   const CommInfo    GetCommInfo() const { return mCommInfo; }
   const RouterInfo  GetRouterInfo() const { return mRouterInfo; }
   const GatewayInfo GetGatewayInfo() const { return mGatewayInfo; }
   const MessageInfo GetMessageInfo() const { return mMessageInfo; }

   const double        GetSimTime() const { return mSimTime; }
   const CommEventType GetEventType() const { return mEventType; }

private:
   std::string           mPlatformName;
   std::vector<LinkInfo> mLinks;
   std::string           mNetworkType;

   MessageInfo   mMessageInfo;
   RouterInfo    mRouterInfo;
   CommInfo      mCommInfo;
   NetworkInfo   mNetworkInfo;
   GatewayInfo   mGatewayInfo;
   double        mSimTime = 0.0;
   CommEventType mEventType;
};

class WKF_COMM_VIS_COMMON_EXPORT DataContainer : public QObject
{
   Q_OBJECT
public:
   DataContainer() = default;

   const std::vector<CommEvent>& GetCommData() const { return mCommData; }

   void Reset()
   {
      mCommData.clear();
      emit ResetData();
   }

   void Clear() { mCommData.clear(); }

   void AddCommData(const CommEvent& aCommEvent) { mCommData.push_back(aCommEvent); }

signals:
   void ResetData();

private:
   std::vector<CommEvent> mCommData;
};
} // namespace wkf

#endif
