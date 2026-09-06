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

#include "xio/WsfXIO_Interface.hpp"

#include <limits>
#include <memory>

#include "GenIP.hpp"
#include "GenInternetSocketAddress.hpp"
#include "GenNetInfo.hpp"
#include "GenSocket.hpp"
#include "GenSocketManager.hpp"
#include "GenTCP_IO.hpp"
#include "GenUDP_IO.hpp"
#include "PakProcessor.hpp"
#include "PakTCP_Connector.hpp"
#include "PakTCP_IO.hpp"
#include "PakUDP_IO.hpp"
#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "script/WsfScriptContext.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Publisher.hpp"
#include "xio/WsfXIO_QueryManager.hpp"
#include "xio/WsfXIO_Request.hpp"
#include "xio/WsfXIO_Subscription.hpp"
#include "xio/WsfXIO_UDP_Header.hpp"

namespace
{
class DefaultClockSource : public WsfXIO_Interface::ClockSource
{
public:
   DefaultClockSource(const WsfXIO_Interface& aInterface)
      : mInterface(aInterface)
   {
   }
   double                  GetSimTime() const override { return mInterface.GetLastAdvanceTime(); }
   const WsfXIO_Interface& mInterface;
};
} // namespace

// =================================================================================================
//! This form is used when the caller is not a WSF simulation.
WsfXIO_Interface::WsfXIO_Interface()
   : WsfXIO_Interface(WsfXIO_InputData())
{
}

// =================================================================================================
//! This form is used when the caller is a WSF simulation.
WsfXIO_Interface::WsfXIO_Interface(const WsfXIO_InputData& aInputData)
   : WsfXIO_InputData(aInputData)
   , mApplicationType(wsf::xio::cAPP_SIMULATION)
   , mClockSourcePtr(new DefaultClockSource(*this))
   , mUDP_HeaderPtr(new WsfXIO_UDP_Header(WsfXIO_PacketRegistry::GetPacketVersion()))
   , mConnectorPtr(nullptr)
   , mLastAdvanceTime(0.0)
   , mCurrentTime(0.0)
   , mPreviousHeartbeatTime(-1.0E6)
   , mPreviousConnectionUpdateTime(-1.0E6)
   , mConnectionUpdateInterval(0.5)
   , mTotalBytesSent(0)
   , mTotalBytesReceived(0)
   , mPreviousBytesSent(0)
   , mPreviousBytesReceived(0)
   , mIsInitialized(false)
{
   WsfXIO_PacketRegistry::RegisterPackets(*this);
   mRequestManagerPtr = new WsfXIO_RequestManager(this);
   mQueryManagerPtr   = new WsfXIO_QueryManager(this);
   mPublisherPtr      = new WsfXIO_Publisher(this);

   mPublisherPtr->SetDebugLevel(mPublisherDebugLevel);

   mCallbacks += Connect(&WsfXIO_Interface::HandleServiceAvailableQuery, this);
   mCallbacks += Connect(&WsfXIO_Interface::HandleHeartbeat, this);
   mCallbacks += Connect(&WsfXIO_Interface::HandleInitialize, this);
   mCallbacks += Connect<WsfXIO_TimeBehindPkt>([this](WsfXIO_TimeBehindPkt& aPkt)
                                               { OnApplicationBehind(aPkt.mIsBehind, aPkt.mApplicationId.ToString()); });
   // interpret a disconnected remote sim as no longer being behind
   mCallbacks += OnDisconnected.Connect([this](WsfXIO_Connection* aConnection)
                                        { OnApplicationBehind(false, aConnection->GetApplicationId().ToString()); });
   mCallbacks += mThreadedIO.Disconnected.Connect(&WsfXIO_Interface::HandleDisconnect, this);
}

// =================================================================================================
WsfXIO_Interface::~WsfXIO_Interface()
{
   if (mIsInitialized && IsDebugEnabled())
   {
      size_t sentBytes, recvBytes;
      GetBytesCommunicated(sentBytes, recvBytes);

      double elapsedTime = mCurrentTime;
      if (elapsedTime <= 1.0)
      {
         elapsedTime = 1.0;
      }
      double sendRate = sentBytes / elapsedTime;
      double recvRate = recvBytes / elapsedTime;

      auto out = ut::log::debug() << "xio_interface: Terminated.";
      out.AddNote() << "Sent " << sentBytes << " bytes at " << sendRate << " bytes/second(simulation)";
      out.AddNote() << "Received " << recvBytes << " bytes at " << recvRate << " bytes/second(simulation)";
   }

   delete mPublisherPtr;
   delete mQueryManagerPtr;
   delete mUDP_HeaderPtr;
   delete mRequestManagerPtr;
   delete mConnectorPtr;
   delete mClockSourcePtr;

   if (mIsInitialized)
   {
      mThreadedIO.Stop();
      mThreadedIO.Join();
   }
   for (auto& connection : mConnections)
   {
      delete connection;
   }
}

// =================================================================================================
void WsfXIO_Interface::SetClockSource(ClockSource* aClockSourcePtr)
{
   delete mClockSourcePtr;
   if (aClockSourcePtr != nullptr)
   {
      mClockSourcePtr = aClockSourcePtr;
   }
   else
   {
      mClockSourcePtr = new DefaultClockSource(*this);
      ;
   }
}

// =================================================================================================
bool WsfXIO_Interface::Initialize()
{
   mInterfaceRequested |= (!mUDP_Targets.empty()); // The interface is instantiated if connections are requested
   mShowConnections |= mDebugEnabled;

   if (mInterfaceRequested)
   {
      mConnectorPtr = new PakTCP_Connector(this);
      if (!mConnectorPtr->Listen(0))
      {
         ut::log::info() << "xio_interface: Could not bind to a port.";
      }
      else
      {
         mTCP_Port = mConnectorPtr->GetBoundPort();
         if (mShowConnections)
         {
            auto out = ut::log::info() << "xio_interface: Accepting connections.";
            out.AddNote() << "Port: " << mTCP_Port;
         }
      }
   }

   for (UDP_Target& target : mUDP_Targets)
   {
      ConnectToTarget(target);
   }
   // mUDP_Targets.clear(); //Why do we clear this here?

   if (mInterfaceRequested)
   {
      mThreadedIO.Start();
      mIsInitialized = true;
   }

   return true;
}

bool WsfXIO_Interface::ConnectToTarget(UDP_Target& aTarget)
{
   bool connected = false;

   if (aTarget.mType == cBROADCAST || aTarget.mType == cUNICAST)
   {
      std::unique_ptr<GenUDP_IO> udpIO(new GenUDP_IO());
      if (aTarget.mType == cUNICAST)
      {
         udpIO->GetRecvSocket()->RemoveSocketOptions(GenSockets::GenSocket::cDISABLE_UNIQUE_BINDING_CHECK);
      }
      if (aTarget.mRecvPort == 0)
      {
         connected = udpIO->Init(aTarget.mAddress, aTarget.mSendPort);
      }
      else if (aTarget.mSendPort == 0)
      {
         int recvPort = aTarget.mRecvPort;
         if (recvPort == -1) // -1 indicates we should let the OS choose a port
         {
            recvPort = 0;
         }
         connected = udpIO->InitRecv(aTarget.mAddress, recvPort);
         if (connected && udpIO->GetRecvSocket())
         {
            aTarget.mRecvPort = udpIO->GetRecvSocket()->GetBoundPort();
         }
      }
      else
      {
         connected = udpIO->Init(aTarget.mAddress, aTarget.mSendPort, aTarget.mRecvPort);
      }

      if (!connected)
      {
         auto out = ut::log::info() << "xio_interface: Could not bind to port.";
         out.AddNote() << "Port: " << aTarget.mRecvPort;
      }
      else
      {
         udpIO->RememberSenderAddress(true);
         WsfXIO_Connection* connectionPtr =
            new WsfXIO_Connection(this, new PakUDP_IO(udpIO.release(), this, mUDP_HeaderPtr->Clone()));
         mThreadedIO.AddIO(&connectionPtr->GetIO(), connectionPtr);
         AddConnection(connectionPtr);
         connectionPtr->SetInitialized();
         aTarget.mConnectionId = connectionPtr->GetConnectionId();
      }
   }
   else if (aTarget.mType == cMULTICAST)
   {
      std::unique_ptr<GenUDP_IO> udpIO(new GenUDP_IO());
      if (!udpIO->Init(aTarget.mAddress, aTarget.mInterface, aTarget.mSendPort, aTarget.mRecvPort))
      {
         auto out = ut::log::info() << "xio_interface: Could not bind to port.";
         out.AddNote() << "Port: " << aTarget.mRecvPort;
         return false;
      }
      else
      {
         if (mMulticastTimeToLive != -1)
         {
            udpIO->GetSendSocket()->SetMulticastTTL(mMulticastTimeToLive);
         }
         if (!mMulticastLoopback)
         {
            udpIO->GetSendSocket()->RemoveSocketOptions(GenSockets::GenSocket::cENABLE_MULTICAST_LOOPBACK);
         }

         udpIO->RememberSenderAddress(true);
         udpIO->AddMulticastMembership(aTarget.mInterface, aTarget.mAddress);
         WsfXIO_Connection* connectionPtr =
            new WsfXIO_Connection(this, new PakUDP_IO(udpIO.release(), this, mUDP_HeaderPtr->Clone()));
         mThreadedIO.AddIO(&connectionPtr->GetIO(), connectionPtr);
         AddConnection(connectionPtr);
         connectionPtr->SetInitialized();
         aTarget.mConnectionId = connectionPtr->GetConnectionId();
         connected             = true;
      }
   }

   if (mDebugEnabled && connected)
   {
      auto out = ut::log::debug() << "xio_interface: Connected.";
      switch (aTarget.mType)
      {
      case cBROADCAST:
         out.AddNote() << "Broadcast Address: " << aTarget.mAddress;
         break;
      case cUNICAST:
         out.AddNote() << "Unicast Address: " << aTarget.mAddress;
         break;
      case cMULTICAST:
         out.AddNote() << "Multicast Address: " << aTarget.mAddress << " " << aTarget.mInterface;
         break;
      }

      out.AddNote() << "Sending Port: " << aTarget.mSendPort;
      out.AddNote() << "Receiving Port: " << aTarget.mRecvPort;
   }

   return connected;
}

void WsfXIO_Interface::AcceptConnections()
{
   if (mConnectorPtr != nullptr)
   {
      PakTCP_IO* ioPtr;
      while ((ioPtr = mConnectorPtr->Accept(0)) != nullptr)
      {
         WsfXIO_Connection* connectionPtr = new WsfXIO_Connection(this, ioPtr);
         mThreadedIO.AddIO(ioPtr, connectionPtr);
         AddConnection(connectionPtr);
      }

      GenSockets::GenInternetSocketAddress inetSockAddr;
      while (mConnectorPtr->CompleteConnect(inetSockAddr, ioPtr))
      {
         WsfXIO_Connection* connectionPtr = new WsfXIO_Connection(this, ioPtr);
         mThreadedIO.AddIO(ioPtr, connectionPtr);
         AddConnection(connectionPtr);

         //! Begin the initialization handshake
         WsfXIO_InitializePkt pkt;
         pkt.mApplicationName = mApplicationName;
         pkt.mApplicationType = mApplicationType;
         pkt.mStage           = 0;
         connectionPtr->Send(pkt);
      }
   }
}

std::unique_ptr<UtCallbackN<void()>> WsfXIO_Interface::TimeoutConnect(double                       aWaitTime,
                                                                      const std::function<void()>& aFunction)
{
   Timeout t;
   t.mTime            = mClock.GetRawClock() + aWaitTime;
   t.mCallbackListPtr = new UtCallbackListN<void()>;
   auto callbackPtr   = t.mCallbackListPtr->Connect(aFunction);
   mTimeouts.push(t);
   return callbackPtr;
}

void WsfXIO_Interface::TriggerTimeouts()
{
   if (!mTimeouts.empty())
   {
      double timeNow = mClock.GetRawClock();
      while (!mTimeouts.empty() && timeNow >= mTimeouts.top().mTime)
      {
         const Timeout& t = mTimeouts.top();
         (*t.mCallbackListPtr)();
         delete t.mCallbackListPtr;
         mTimeouts.pop();
      }
   }
}

void WsfXIO_Interface::Send(WsfXIO_Packet& aPkt, WsfXIO_Connection* aConnectionPtr)
{
   aPkt.mApplicationId = mApplicationId;
   aPkt.SetTimeStamp(static_cast<float>(GetSimTime()));
   mThreadedIO.Send(&aConnectionPtr->GetIO(), aPkt);
}

void WsfXIO_Interface::Send(WsfXIO_Packet& aPkt, WsfXIO_Destination& aDestination)
{
   Send(aPkt, aDestination.GetIO_List());
}

void WsfXIO_Interface::Send(WsfXIO_Packet& aPkt, const std::vector<PakSocketIO*>& aConnections)
{
   aPkt.mApplicationId = mApplicationId;
   aPkt.SetTimeStamp(static_cast<float>(GetSimTime()));
   mThreadedIO.Send(aConnections, aPkt);
}

WsfXIO_Connection* WsfXIO_Interface::GetSender(PakPacket& aPkt)
{
   return static_cast<WsfXIO_Connection*>(aPkt.GetSender());
}

void WsfXIO_Interface::SendToAll(WsfXIO_Packet& aPkt)
{
   aPkt.mApplicationId = mApplicationId;
   aPkt.SetTimeStamp(static_cast<float>(GetSimTime()));
   mThreadedIO.SendToAll(aPkt);
}

void WsfXIO_Interface::SendToAllUDP(WsfXIO_Packet& aPkt)
{
   aPkt.mApplicationId = mApplicationId;
   aPkt.SetTimeStamp(static_cast<float>(GetSimTime()));
   std::vector<PakSocketIO*> sendList;
   for (auto& connection : mConnections)
   {
      PakUDP_IO* ioPtr = connection->GetUDP_IO();
      if (ioPtr != nullptr && ioPtr->GetConnection().GetSendToPort() > 0)
      {
         sendList.push_back(ioPtr);
      }
   }
   mThreadedIO.Send(sendList, aPkt);
}

void WsfXIO_Interface::SendToAllTCP(WsfXIO_Packet& aPkt)
{
   aPkt.mApplicationId = mApplicationId;
   std::vector<PakSocketIO*> sendList;
   for (auto& connection : mConnections)
   {
      if (connection->GetTCP_IO() != nullptr)
      {
         sendList.push_back(connection->GetTCP_IO());
      }
   }
   mThreadedIO.Send(sendList, aPkt);
}

void WsfXIO_Interface::AddConnection(WsfXIO_Connection* aConnectionPtr)
{
   mConnections.push_back(aConnectionPtr);
   mConnectionsById[aConnectionPtr->GetConnectionId()] = aConnectionPtr;
   // TCP Connections are not considered 'connected' until handshaking with WsfXIO_InitializePkt
   if (aConnectionPtr->GetTCP_IO() == nullptr)
   {
      OnConnected(aConnectionPtr);
   }
}

std::unique_ptr<WsfXIO_Interface::ConnectionCallback>
WsfXIO_Interface::DisconnectConnect(WsfXIO_Connection* aConnectionPtr, const ConnectionCallback::FunctionType& aFunction)
{
   ConnectionCallbackList*         cbListPtr = nullptr;
   ConnectionCallbackMap::iterator i         = mDisconnectCallbacks.find(aConnectionPtr);
   if (i == mDisconnectCallbacks.end())
   {
      mDisconnectCallbacks[aConnectionPtr] = cbListPtr = new ConnectionCallbackList;
   }
   else
   {
      cbListPtr = i->second;
   }
   auto callbackPtr = cbListPtr->Connect(aFunction);
   return callbackPtr;
}

void WsfXIO_Interface::HandleDisconnect(PakSocketIO* aIOPtr, PakConnection* aConnectionPtr)
{
   WsfXIO_Connection* connectionPtr = static_cast<WsfXIO_Connection*>(aConnectionPtr);
   connectionPtr->SetDisconnecting();
   if (connectionPtr->IsInitialized())
   {
      if (connectionPtr->GetTCP_IO() != nullptr)
      {
         GenSockets::GenSocket* socketPtr = connectionPtr->GetTCP_IO()->GetSendSocket();
         mTotalBytesReceived += socketPtr->GetTotalBytesReceived();
         mTotalBytesSent += socketPtr->GetTotalBytesSent();
      }
      OnDisconnected(connectionPtr);
      ConnectionCallbackMap::iterator cbIter = mDisconnectCallbacks.find(connectionPtr);
      if (cbIter != mDisconnectCallbacks.end())
      {
         (*cbIter->second)(connectionPtr);
      }
   }

   if (mDebugEnabled && connectionPtr->IsInitialized() && connectionPtr->IsReliable())
   {
      auto out = ut::log::debug() << "xio_interface: Disconnected from application.";
      out.AddNote() << "Application: " << connectionPtr->GetApplicationName();
   }

   ConnectionList::iterator i = std::find(mConnections.begin(), mConnections.end(), aConnectionPtr);
   if (i != mConnections.end())
   {
      mConnections.erase(i);
      mConnectionsById.erase(connectionPtr->GetConnectionId());
      ConnectionList::iterator j = std::find(mConnectedConnections.begin(), mConnectedConnections.end(), aConnectionPtr);
      if (j != mConnectedConnections.end())
      {
         mConnectedConnections.erase(j);
      }
      for (SenderConnectionMap::iterator k = mSenderConnections.begin(); k != mSenderConnections.end(); ++k)
      {
         if (k->second == aConnectionPtr)
         {
            mSenderConnections.erase(k);
            break;
         }
      }
   }
   delete aConnectionPtr;
}

void WsfXIO_Interface::Disconnect(WsfXIO_Connection* aConnectionPtr)
{
   // This will trigger HandleDisconnect()
   mThreadedIO.RemoveIO(&aConnectionPtr->GetIO());
}

void WsfXIO_Interface::AdvanceTime(double aSimTime)
{
   mLastAdvanceTime = aSimTime; // Save last advance time for non-WSF/XIO simulations
   if (mPreviousConnectionUpdateTime < mCurrentTime - mConnectionUpdateInterval)
   {
      mPreviousConnectionUpdateTime = mCurrentTime;
      AcceptConnections();
   }

   // mThreadedIO.Process();
   ProcessMessages(aSimTime);

   for (auto& connection : mConnections)
   {
      PakTCP_IO* ioPtr = connection->GetTCP_IO();
      if (ioPtr != nullptr)
      {
         ioPtr->Flush();
      }
   }

   mCurrentTime = mClock.GetRawClock();
   if (mPreviousHeartbeatTime < mCurrentTime - mHeartbeatInterval)
   {
      mPreviousHeartbeatTime = mCurrentTime;
      SendHeartbeat();
      if (mShowTransferRate || !BandwidthDataEvent.IsEmpty())
      {
         size_t sentBytes, recvBytes;
         GetBytesCommunicated(sentBytes, recvBytes);
         size_t newSentBytes    = sentBytes - mPreviousBytesSent;
         size_t newRecvBytes    = recvBytes - mPreviousBytesReceived;
         mPreviousBytesSent     = sentBytes;
         mPreviousBytesReceived = recvBytes;
         if (mShowTransferRate)
         {
            auto out = ut::log::debug() << "xio_interface: Advanced time.";
            out.AddNote() << "Sent " << sentBytes << " bytes at " << newSentBytes / mHeartbeatInterval << " bytes/second";
            out.AddNote() << "Received " << recvBytes << " bytes at " << newRecvBytes / mHeartbeatInterval
                          << " bytes/second";
         }
         BandwidthDataEvent((int)sentBytes, (int)newSentBytes, (int)recvBytes, (int)newRecvBytes, mHeartbeatInterval);
      }
   }
   TriggerTimeouts();
}

//! Handle packets which need processed
void WsfXIO_Interface::ProcessMessages(double aSimTime)
{
   // Handle newly received packets
   {
      PakThreadedIO::PacketList packets;
      mThreadedIO.Extract(packets);
      for (auto& packet : packets)
      {
         // This is dangerous, should be dynamic cast and check for validity
         WsfXIO_Packet& pkt        = (WsfXIO_Packet&)*packet;
         bool           processNow = true;
         if (pkt.GetFlags() & WsfXIO_Packet::cSYNCHRONIZED)
         {
            processNow = !ProcessSynchronizedPacket(aSimTime, pkt);
         }
         if (processNow)
         {
            WsfXIO_Connection* senderPtr = (WsfXIO_Connection*)pkt.GetSender();
            if (senderPtr && senderPtr->IsReliable())
            {
               senderPtr->SetLastTimeStamp((double)pkt.GetTimeStamp());
            }
            PakProcessor::ProcessPacket(&pkt, true);
         }
      }
   }

   // Handle buffered packets
   {
      PacketBuffer::iterator i = mReceivePacketBuffer.begin();
      for (; i != mReceivePacketBuffer.end() && ((*i)->GetTimeStamp() < aSimTime); ++i)
      {
         PakProcessor::ProcessPacket(*i, true);
      }
      mReceivePacketBuffer.erase(mReceivePacketBuffer.begin(), i);
   }
}

//! Attempt to buffer a synchronized packet.
//! @return 'true' if packet was buffered, 'false' if the packet needs processing now
bool WsfXIO_Interface::ProcessSynchronizedPacket(double aSimTime, WsfXIO_Packet& aPkt)
{
   bool               wasBuffered = false;
   WsfXIO_Connection* senderPtr   = (WsfXIO_Connection*)aPkt.GetSender();
   if (senderPtr->IsReliable())
   {
      if (senderPtr->HasClockTranslation())
      {
         double recvOffset = senderPtr->GetClockOffset();
         aPkt.SetTimeStamp((float)(aPkt.GetTimeStamp() + recvOffset));
         if (aPkt.GetTimeStamp() > aSimTime)
         {
            BufferPacket(&aPkt);
            wasBuffered = true;
         }
         else if (mShowConnections)
         {
            auto out = ut::log::debug() << "xio_interface: 'Synchronized packet' failed.";
            out.AddNote() << aSimTime - aPkt.GetTimeStamp() << " seconds behind.";
         }
      }
   }
   else
   {
      SenderConnectionMap::iterator iter =
         mSenderConnections.find(SenderAddress(aPkt.GetOriginatorAddress(), aPkt.GetOriginatorPort()));
      if (iter != mSenderConnections.end())
      {
         WsfXIO_Connection* connectionPtr = iter->second;
         if (connectionPtr->HasClockTranslation())
         {
            double recvOffset = iter->second->GetClockOffset();
            aPkt.SetTimeStamp((float)(aPkt.GetTimeStamp() + recvOffset));
            if (aPkt.GetTimeStamp() > aSimTime)
            {
               BufferPacket(&aPkt);
               wasBuffered = true;
            }
            else if (mShowConnections)
            {
               auto out = ut::log::debug() << "xio_interface: 'Synchronized packet' failed.";
               out.AddNote() << aSimTime - aPkt.GetTimeStamp() << " seconds behind.";
            }
         }
      }
   }
   return wasBuffered;
}

//! Insert a packet into the receive buffer for delayed processing
void WsfXIO_Interface::BufferPacket(WsfXIO_Packet* aPacketPtr)
{
   // Assume timestamps received are generally increasing, so search from the end.
   double                         timeStamp = aPacketPtr->GetTimeStamp();
   PacketBuffer::reverse_iterator i         = mReceivePacketBuffer.rbegin();
   while (i != mReceivePacketBuffer.rend() && (*i)->GetTimeStamp() >= timeStamp)
   {
      ++i;
   }
   mReceivePacketBuffer.insert(i.base(), aPacketPtr);
}

void WsfXIO_Interface::SendHeartbeat()
{
   OnHeartbeatUpdate(GetSimTime(), mApplicationId, false);
   WsfXIO_HeartbeatPkt pkt;
   pkt.mTCP_Port        = ut::safe_cast<uint16_t, int>(mTCP_Port);
   pkt.mApplicationName = mApplicationName;
   pkt.mApplicationType = mApplicationType;
   SendToAllUDP(pkt);
}

void WsfXIO_Interface::HandleHeartbeat(WsfXIO_HeartbeatPkt& aPkt)
{
   OnHeartbeatUpdate(aPkt.GetTimeStamp(), aPkt.mApplicationId, true);
   if (aPkt.mTCP_Port != 0)
   {
      if (mProcessedHeartbeats.find(aPkt.mApplicationId) == mProcessedHeartbeats.end())
      {
         if (mApplicationTypeFilter.find(aPkt.mApplicationType) != mApplicationTypeFilter.end())
         {
            if (FindConnection(aPkt.mApplicationId) == nullptr)
            {
               unsigned int                         addr = aPkt.GetOriginatorAddress();
               GenSockets::GenIP                    ip(addr);
               GenSockets::GenInternetSocketAddress address(ip, aPkt.mTCP_Port);

               if (mShowConnections)
               {
                  auto out = ut::log::debug() << "xio_interface: Received heartbeat.";
                  out.AddNote() << "Attempting to connect to " << static_cast<std::string>(address);
               }
               mConnectorPtr->BeginConnect(address, 10.0);
               SendHeartbeat();
            }
         }
         HeartbeatInfo newHeartbeat(aPkt.mApplicationId);
         newHeartbeat.mConnectionPtr = (WsfXIO_Connection*)aPkt.GetSender();
         mProcessedHeartbeats.insert(newHeartbeat);

         for (WsfXIO_Connection* connectionPtr : mConnections)
         {
            if (connectionPtr->GetApplicationId() == aPkt.mApplicationId && connectionPtr->GetTCP_IO() != nullptr)
            {
               connectionPtr->SetLinkedConnection((WsfXIO_Connection*)aPkt.GetSender());
            }
         }
      }
   }
}

//! Determines if aConnectionPtr is a connection to an application that is already connected
//! Returns 'true' if aConnectionPtr is a duplicate, and aConnectionPtr is destroyed
//! Returns 'false' if aConnectionPtr is unique
bool WsfXIO_Interface::CheckForDuplicateConnection(WsfXIO_Connection* aConnectionPtr)
{
   // Don't connect if there already exists an initialized connection
   WsfXIO_Connection* duplicatePtr = nullptr;
   for (const auto& connection : mConnections)
   {
      if (aConnectionPtr != connection && aConnectionPtr->GetApplicationId() == connection->GetApplicationId())
      {
         duplicatePtr = connection;
      }
   }
   bool isDuplicate = false;
   if (duplicatePtr != nullptr)
   {
      if (duplicatePtr->IsInitialized())
      {
         isDuplicate = true;
         Disconnect(aConnectionPtr);
      }
      else
      {
         // The application ID is used to make sure each application disconnects the same connection
         if (mApplicationId < aConnectionPtr->GetApplicationId())
         {
            isDuplicate = true;
            Disconnect(aConnectionPtr);
         }
         else
         {
            Disconnect(duplicatePtr);
         }
      }
   }

   return isDuplicate;
}

//! Returns a connection which should be used to send to an address.
//! May create a unicast UDP connection if necessary.
//! @param aConnectionPtr The connection the message was received by.
//! @param aSendAddress   Address of the sender
WsfXIO_Connection* WsfXIO_Interface::GetSendConnection(WsfXIO_Connection* aConnectionPtr, SenderAddress aSendAddress)
{
   WsfXIO_Connection* sendConnectionPtr = nullptr;
   PakUDP_IO*         ioPtr             = nullptr;
   if (aConnectionPtr != nullptr)
   {
      ioPtr = aConnectionPtr->GetUDP_IO();
   }
   if (ioPtr == nullptr)
   {
      sendConnectionPtr = aConnectionPtr;
   }
   else if (ioPtr->GetConnection().GetSendToPort() > 0)
   {
      sendConnectionPtr = aConnectionPtr;
   }
   return sendConnectionPtr;
}

void WsfXIO_Interface::HandleInitialize(WsfXIO_InitializePkt& aPkt)
{
   const int cCONNECT_STAGE = 15;
   // WsfXIO_InitializePkt is sent 3 times as a 3-way handshake
   // This prevents duplicate connections from being used.
   bool               ok            = true;
   WsfXIO_Connection* connectionPtr = GetSender(aPkt);

   // Note: At this point it is possible the connection is already gone (connectionPtr has been deleted).
   //       This should be very rare, but protect from crashing if this is the case.
   if (std::find(mConnections.begin(), mConnections.end(), connectionPtr) == mConnections.end())
   {
      return;
   }

   if (aPkt.mStage <= cCONNECT_STAGE)
   {
      connectionPtr->SetApplicationId(aPkt.mApplicationId);
      connectionPtr->SetApplicationType(aPkt.mApplicationType);
      connectionPtr->SetApplicationName(aPkt.mApplicationName.GetString());

      WsfXIO_Connection*                relatedConnectionPtr = nullptr;
      std::set<HeartbeatInfo>::iterator iter                 = mProcessedHeartbeats.find(aPkt.mApplicationId);
      if (iter != mProcessedHeartbeats.end())
      {
         relatedConnectionPtr = iter->mConnectionPtr;
      }
      SenderAddress      senderAddr(aPkt.GetOriginatorAddress(), aPkt.GetOriginatorPort());
      WsfXIO_Connection* sendConnectionPtr = GetSendConnection(relatedConnectionPtr, senderAddr);
      if (sendConnectionPtr != nullptr)
      {
         connectionPtr->SetLinkedConnection(sendConnectionPtr);
      }
      else
      {
         connectionPtr->SetLinkedConnection(connectionPtr);
      }
   }

   if (ok && aPkt.mStage <= cCONNECT_STAGE)
   {
      ok = !CheckForDuplicateConnection(connectionPtr);
   }

   if (ok)
   {
      if (aPkt.mStage <= cCONNECT_STAGE)
      {
         WsfXIO_InitializePkt response;
         response.mStage           = aPkt.mStage + 1;
         response.mApplicationName = mApplicationName;
         response.mApplicationType = mApplicationType;
         connectionPtr->Send(response);
      }

      if (aPkt.mStage >= cCONNECT_STAGE)
      {
         connectionPtr->SetInitialized();
         mConnectedConnections.push_back(connectionPtr);
         SenderAddress senderAddr(aPkt.GetOriginatorAddress(), aPkt.GetOriginatorPort());
         mSenderConnections[senderAddr] = connectionPtr;

         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "xio_interface: Connected to application.";
            out.AddNote() << "Application: " << connectionPtr->GetApplicationName();
         }
         OnConnected(connectionPtr);
      }
   }
}

WsfXIO_Connection* WsfXIO_Interface::FindConnection(const GenUniqueId& aApplicationId)
{
   WsfXIO_Connection* connectionPtr = nullptr;
   for (auto& connection : mConnections)
   {
      if (aApplicationId == connection->GetApplicationId())
      {
         connectionPtr = connection;
      }
   }
   return connectionPtr;
}

//! Returns a pointer to the connection with the given ID
WsfXIO_Connection* WsfXIO_Interface::FindConnection(int aConnectionIndex)
{
   ConnectionIdMap::iterator i = mConnectionsById.find(aConnectionIndex);
   if (i != mConnectionsById.end())
   {
      return i->second;
   }
   return nullptr;
}

WsfXIO_Connection* WsfXIO_Interface::FindConnection(const std::string& aApplicationName)
{
   WsfXIO_Connection* connectionPtr = nullptr;
   for (auto& connection : mConnections)
   {
      if (aApplicationName == connection->GetApplicationName())
      {
         connectionPtr = connection;
      }
   }
   return connectionPtr;
}

void WsfXIO_Interface::GetBytesCommunicated(size_t& aBytesSent, size_t& aBytesReceived)
{
   aBytesSent     = mTotalBytesSent;
   aBytesReceived = mTotalBytesReceived;

   for (WsfXIO_Connection* connectionPtr : mConnections)
   {
      PakTCP_IO* tcpPtr = connectionPtr->GetTCP_IO();
      if (tcpPtr != nullptr)
      {
         aBytesReceived += tcpPtr->GetRecvSocket()->GetTotalBytesReceived();
         aBytesSent += tcpPtr->GetSendSocket()->GetTotalBytesSent();
      }
      else
      {
         PakUDP_IO* udpPtr = connectionPtr->GetUDP_IO();
         aBytesReceived += udpPtr->GetRecvSocket()->GetTotalBytesReceived();
         aBytesSent += udpPtr->GetSendSocket()->GetTotalBytesSent();
      }
   }
}

//! Specifies that a service is available by this application.
void WsfXIO_Interface::RegisterServiceType(std::string aServiceType, std::string aServiceVersion)
{
   mAvailableServices[aServiceType] = aServiceVersion;
}

//! Responds to a 'Service Available Query'
void WsfXIO_Interface::HandleServiceAvailableQuery(WsfXIO_ServiceAvailableQuery& aQuery)
{
   if (aQuery.mPacketType == WsfXIO_ServiceAvailableQuery::cREQUEST)
   {
      bool                         avail = mAvailableServices.find(aQuery.mServiceType) != mAvailableServices.end();
      WsfXIO_ServiceAvailableQuery response;
      response.mServiceType = aQuery.mServiceType;
      if (avail)
      {
         response.mPacketType  = WsfXIO_ServiceAvailableQuery::cRESPOND_AVAILABLE;
         response.mVersionInfo = mAvailableServices[aQuery.mServiceType];
      }
      else
      {
         response.mPacketType = WsfXIO_ServiceAvailableQuery::cRESPOND_NOT_AVAILABLE;
      }
      ((WsfXIO_Connection*)aQuery.GetSender())->Send(response);
   }
}

//! Adds a connection target.  Can be called before or after initialization.
void WsfXIO_Interface::AddUDP_Target(UDP_Target aTarget)
{
   if (mIsInitialized)
   {
      if (ConnectToTarget(aTarget))
      {
         mUDP_Targets.push_back(aTarget);
      }
   }
   else
   {
      mUDP_Targets.push_back(aTarget);
      mInterfaceRequested = true;
   }
}

bool WsfXIO_Interface::RemoveUDP_Target(int aTargetIndex)
{
   bool removed = false;
   if (mIsInitialized)
   {
      const UDP_Target&  target        = mUDP_Targets[aTargetIndex];
      WsfXIO_Connection* connectionPtr = FindConnection(target.mConnectionId);
      if (connectionPtr != nullptr)
      {
         // Don't remove if it is linked to another connection.
         for (WsfXIO_Connection* cPtr : mConnections)
         {
            if (cPtr->GetLinkedConnection() == connectionPtr)
            {
               return false;
            }
         }
      }
      Disconnect(connectionPtr);
      mUDP_Targets.erase(mUDP_Targets.begin() + aTargetIndex);
      removed = true;
   }
   else
   {
      mUDP_Targets.erase(mUDP_Targets.begin() + aTargetIndex);
      removed = true;
   }
   return removed;
}
