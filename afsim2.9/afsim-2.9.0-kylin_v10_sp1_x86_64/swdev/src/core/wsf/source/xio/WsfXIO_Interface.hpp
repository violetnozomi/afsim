// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_INTERFACE_HPP
#define WSFXIO_INTERFACE_HPP

#include "wsf_export.h"

#include <deque>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "GenInternetSocketAddress.hpp"
#include "GenUniqueId.hpp"
#include "PakConnection.hpp"
#include "PakProcessor.hpp"
class GenTCP_Server;
class PakTCP_Connector;
class PakTCP_IO;
#include "PakThreadedIO.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtWallClock.hpp"
#include "xio/WsfXIO.hpp"
class WsfXIO_Connection;
class WsfXIO_Destination;
class WsfXIO_HeartbeatPkt;
class WsfXIO_InitializePkt;
#include "xio/WsfXIO_InputData.hpp"
class WsfXIO_Packet;
class WsfXIO_Publisher;
class WsfXIO_QueryManager;
class WsfXIO_RequestManager;
class WsfXIO_ServiceAvailableQuery;
class WsfXIO_UDP_Header;

//! This is communications interface for XIO.
//! It is responsible for:
//! - Maintaining all XIO connections to other applications
//! - Sending out heartbeat messages via UDP connections
//! - Connecting to applications upon receipt of heartbeats
//! - Sending and receiving of data
class WSF_EXPORT WsfXIO_Interface : public WsfXIO_InputData, public PakProcessor
{
public:
   class ClockSource
   {
   public:
      virtual ~ClockSource() {} // Warning RHEL5 if this isn't here.
      virtual double GetSimTime() const = 0;
   };

   typedef std::pair<int, int>                       SenderAddress;
   typedef std::vector<WsfXIO_Connection*>           ConnectionList;
   typedef UtCallbackN<void(WsfXIO_Connection*)>     ConnectionCallback;
   typedef UtCallbackListN<void(WsfXIO_Connection*)> ConnectionCallbackList;

   WsfXIO_Interface();
   WsfXIO_Interface(const WsfXIO_InputData& aInputData);
   ~WsfXIO_Interface();

   //! @name Application identification methods
   //@{
   const GenUniqueId& GetApplicationId() const { return mApplicationId; }
   void               SetApplicationName(const std::string& aApplicationName) { mApplicationName = aApplicationName; }
   std::string        GetApplicationName() const { return mApplicationName.GetString(); }
   void SetApplicationType(wsf::xio::ApplicationType aApplicationType) { mApplicationType = aApplicationType; }
   //@}

   bool Initialize();
   void AdvanceTime(double aSimTime);

   //! @name Simulation clock methods.
   //@{
   //! Returns the current simulation time.
   double GetSimTime() const { return mClockSourcePtr->GetSimTime(); }
   double GetLastAdvanceTime() const { return mLastAdvanceTime; }
   void   SetClockSource(ClockSource* aClockSourcePtr);
   //@}

   //! @name Packet send methods.
   //@{
   void Send(WsfXIO_Packet& aPktPtr, WsfXIO_Connection* aConnectionPtr);

   void Send(WsfXIO_Packet& aPkt, WsfXIO_Destination& aDestination);

   void Send(WsfXIO_Packet& aPktPtr, const std::vector<PakSocketIO*>& aConnections);

   void SendToAll(WsfXIO_Packet& aPkt);

   void SendToAllUDP(WsfXIO_Packet& aPkt);

   void SendToAllTCP(WsfXIO_Packet& aPkt);
   //@}

   void SetValidApplicationTypes(const std::vector<int>& aApplicationTypes);

   WsfXIO_Connection* GetSender(PakPacket& aPkt);

   //! @name Various callbacks to which others can subscribe.
   //@{
   //! Invoked when a connection is initialized
   UtCallbackListN<void(WsfXIO_Connection*)> OnConnected;

   //! Invoked when a connection is no longer connected, prior to deletion
   UtCallbackListN<void(WsfXIO_Connection*)> OnDisconnected;

   //! Invoked when a heartbeat is sent or received.
   //! double      = Time Stamp
   //! GenUniqueId = ApplicationId
   //! bool        = Received (!Sent)
   UtCallbackListN<void(double, GenUniqueId, bool)> OnHeartbeatUpdate;

   //! Invoked when a connected application has fallen behind the real-time clock by some threshold, or is no longer behind
   UtCallbackListN<void(bool, const std::string&)> OnApplicationBehind;

   //! Executes on the heartbeat to provide bandwidth information to event output
   UtCallbackListN<void(int, int, int, int, double)> BandwidthDataEvent;

   std::unique_ptr<UtCallbackN<void()>> TimeoutConnect(double aWaitTime, const std::function<void()>& aFunction);

   std::unique_ptr<ConnectionCallback> DisconnectConnect(WsfXIO_Connection*                      aConnectionPtr,
                                                         const ConnectionCallback::FunctionType& aFunction);
   //@}

   //! @name Accessors to objects maintained by this class.
   //@

   WsfXIO_RequestManager& GetRequestManager() { return *mRequestManagerPtr; }

   WsfXIO_QueryManager& GetQueryManager() const { return *mQueryManagerPtr; }

   WsfXIO_Publisher& GetPublisher() const { return *mPublisherPtr; }
   //@}

   //! @name Service management
   //@{
   void RegisterServiceType(std::string aServiceType, std::string aServiceVersion = std::string());
   //@}

   //! @name Connection management
   //@{
   void                           AddUDP_Target(UDP_Target aTarget);
   bool                           RemoveUDP_Target(int aTargetIndex);
   const std::vector<UDP_Target>& GetUDP_Targets() { return mUDP_Targets; }

   const ConnectionList& GetConnections() const { return mConnections; }
   const ConnectionList& GetReliableConnections() const { return mConnectedConnections; }
   WsfXIO_Connection*    FindConnection(const GenUniqueId& aApplicationId);
   WsfXIO_Connection*    FindConnection(int aConnectionIndex);
   WsfXIO_Connection*    FindConnection(const std::string& aApplicationName);
   void                  Disconnect(WsfXIO_Connection* aConnectionPtr);
   //@}

   //! @name Miscellaneous methods.
   //@{

   //! Was a network connection requested?
   bool IsXIO_Requested() const { return mInterfaceRequested; }
   void SetXIO_Requested() { mInterfaceRequested = true; }

   //! Should connection status messages be shown?
   bool ShowConnections() const { return mShowConnections; }
   void SetShowConnections(bool aShowConnections) { mShowConnections = aShowConnections; }

   //! Was detailed debugging information requested?
   bool IsDebugEnabled() const { return mDebugEnabled; }
   void SetDebugEnabled(bool aIsEnabled) { mDebugEnabled = aIsEnabled; }

   void GetBytesCommunicated(size_t& aBytesSent, size_t& aBytesReceived);
   //@}

private:
   void SendHeartbeat();

   void BufferPacket(WsfXIO_Packet* aPacketPtr);

   bool CheckForDuplicateConnection(WsfXIO_Connection* aConnectionPtr);

   WsfXIO_Connection* GetSendConnection(WsfXIO_Connection* aConnectionPtr, SenderAddress aSendAddress);

   void HandleServiceAvailableQuery(WsfXIO_ServiceAvailableQuery& aQuery);

   void ProcessMessages(double aSimTime);

   bool ProcessSynchronizedPacket(double aSimTime, WsfXIO_Packet& aPkt);

   void TriggerTimeouts();

   void HandleHeartbeat(WsfXIO_HeartbeatPkt& aPkt);

   void HandleInitialize(WsfXIO_InitializePkt& aPkt);

   void AddConnection(WsfXIO_Connection* aConnectionPtr);

   void AcceptConnections();

   void HandleDisconnect(PakSocketIO* aIOPtr, PakConnection* aConnectionPtr);

   bool ConnectToTarget(UDP_Target& aTarget);

   struct Timeout
   {
      UtCallbackListN<void()>* mCallbackListPtr;
      double                   mTime;
      bool                     operator<(const Timeout& aRHS) const { return mTime < aRHS.mTime; }
   };

   struct HeartbeatInfo
   {
      HeartbeatInfo(GenUniqueId aId)
         : mUniqueId(aId)
      {
      }
      bool               operator<(const HeartbeatInfo& aInfo) const { return mUniqueId < aInfo.mUniqueId; }
      GenUniqueId        mUniqueId;
      WsfXIO_Connection* mConnectionPtr;
   };

   using TimeoutQueue = std::priority_queue<Timeout>;
   // using UDP_Target            = std::pair<UDP_Type, std::string>;
   using AddrPort              = std::pair<unsigned int, short>;
   using ConnectionCallbackMap = std::map<WsfXIO_Connection*, UtCallbackListN<void(WsfXIO_Connection*)>*>;
   using PendingConnectionInfo = std::pair<GenSockets::GenInternetSocketAddress, WsfXIO_Connection*>;
   using SenderConnectionMap   = std::map<SenderAddress, WsfXIO_Connection*>;
   using ConnectionIdMap       = std::map<int, WsfXIO_Connection*>;
   using PacketBuffer          = std::deque<WsfXIO_Packet*>;

   UtCallbackHolder mCallbacks;

   GenUniqueId               mApplicationId;   //!< The application's unique ID
   wsf::xio::ApplicationType mApplicationType; //!< The current application's type

   ClockSource*           mClockSourcePtr;
   WsfXIO_UDP_Header*     mUDP_HeaderPtr; //!< Header used for UDP messages
   WsfXIO_RequestManager* mRequestManagerPtr;
   WsfXIO_QueryManager*   mQueryManagerPtr;
   WsfXIO_Publisher*      mPublisherPtr;
   PakTCP_Connector*      mConnectorPtr;

   TimeoutQueue mTimeouts;

   //! This is used user-created instances that are not WSF applications.
   //! (They don't have WsfApplication/WsfScenario/WsfSimulation).
   double mLastAdvanceTime;

   // The following variables are used for sending out the heartbeat pdu's
   UtWallClock mClock;
   double      mCurrentTime;
   double      mPreviousHeartbeatTime;
   double      mPreviousConnectionUpdateTime;
   double      mConnectionUpdateInterval;

   //! Set of application ID's that have already had a connection-chance
   std::set<HeartbeatInfo> mProcessedHeartbeats;

   //! Maintains a mapping between UDP sender address and the related reliable connection
   SenderConnectionMap mSenderConnections;

   //! List of all current connections
   ConnectionList mConnections;

   //! List of current reliable connections
   ConnectionList mConnectedConnections;

   //! Map from connection id to connection
   ConnectionIdMap mConnectionsById;

   //! Handles sending and receiving using a secondary thread
   PakThreadedIO mThreadedIO;

   ConnectionCallbackMap mDisconnectCallbacks;

   PacketBuffer                       mReceivePacketBuffer;
   std::map<std::string, std::string> mAvailableServices;

   size_t mTotalBytesSent;
   size_t mTotalBytesReceived;

   size_t mPreviousBytesSent;
   size_t mPreviousBytesReceived;

   //! True if the interface has been initialized
   bool mIsInitialized;
};

#endif
