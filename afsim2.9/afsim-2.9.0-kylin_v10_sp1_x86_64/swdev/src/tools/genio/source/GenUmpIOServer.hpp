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

#ifndef GEN_UMPIO_SERVER_HPP
#define GEN_UMPIO_SERVER_HPP

/// \file  GenUmpIOServer.hpp
/// \brief GenUmpIOServer
/// \class GenUmpIOServer GenUmpIOServer.hpp "GenUmpIOServer.hpp"
/// \brief A Server for multiple GenUmpIOClient objects
///
/// This class works with the GenUmpIOServerCC class to form a server that
/// \link GenUmpIOClient GenUmpIOClients \endlink can connect to.  A GenUmpIOServerCC class is typically instantiated
/// (on the sever side) for each client that establishes a connection.
/// Currently this server supports TCP, UDP, and Reliable UDP connections.
/// Since the connection type is detemined by the client, the
/// "GenUmpIOServerType" variable currently has no effect.  This
/// varible/argument is in the constructors and Init methods for only future growth.
/// Currently only a maximum of 32 objects of this class should be
/// instantiated per process.  Most sims will instantiate only one, so this limit
/// should not be a big deal.\n
///
/// This class uses the concpt that messages or data is "Put" to a send buffer for bundling and then sent at once
/// with the Send() method.  If the send buffer is full, the buffered data will automatically be sent before accepting
/// additional data from PutMsg() or PutData().  Data is not guaranteed to be sent until the Send() method is called.
/// A call to Send() with an empty send buffer is harmless.\n
///
/// If using Reliable UDP (RUDP) a call to Send() should be made on a regular heartbeat, so the UMP has a chance at the
/// processor. See the UMP manual for more details on using RUDP.
///
/// An example of a client/server setup with some sample messages can be found in a typical
/// workspace at <tt>unclass/tools/genio/client_server_example</tt>.

#include <algorithm>  // For mem_fun, bind2nd
#include <functional> // For for_each
#include <iostream>
#include <unordered_map>

#include "GenServerCCStats.hpp"
#include "GenTime.hpp"
#include "GenUmpIO.hpp"
#include "GenUmpIOServerCC.hpp"
#include "GenUmpIOServerPrv.hpp"
#include "UtDeleteObj.hpp"
#include "UtStringUtil.hpp"
#include "UtWallClock.hpp"
#include "ump.h"

template<typename MsgType, typename MsgFactoryType = GenIFactory<MsgType>>
class GenUmpIOServer : public GenUmpIOServerPrv
{
public:
   enum GenUmpIOServerType
   {
      TCP_IP = 1
   };

   /// Constructor
   /// Generic constructor that supports named addressing and manual addressing
   /// \note  This class does not take ownership of the MsgFactory pointer.
   /// \param aMsgFactory       A pointer to a message factory.
   /// \param aCallingClassNmae A name to desribe this object in error messages.
   /// \param aUpdatePeriodSec  Time between calls to Send or Receive, only needed for RUDP.
   GenUmpIOServer(MsgFactoryType* aMsgFactory, const char* aCallingClassName, float aUpdatePeriodSec = 0.05F);

   /// Constructor
   /// Manual connection constructor where the user specifies the receive TCP port that
   /// clients will use for the initial connection.
   /// \note This class does not take ownership of the MsgFactory pointer.
   /// \param aRecvOnPort       Port number that clients should use to make their initial connection.
   /// \param aMsgFactory       A pointer to a message factory.
   /// \param aType             For future growth, for now just use the locally defined enumerated value TCP_IP"
   /// \param aCallingClassNmae A name to desribe this object in error messages.
   /// \param aUpdatePeriodSec  Time between calls to Send or Receive, only needed for RUDP.
   GenUmpIOServer(unsigned int       aRecvOnPort,
                  MsgFactoryType*    aMsgFactory,
                  GenUmpIOServerType aType,
                  const char*        aCallingClassName,
                  float              aUpdatePeriodSec = 0.05F);

   /// Constructor
   /// Named addressing constructor that uses the UMP Arbiter to negiotiate ports
   /// and addresses with the clients.  The message group name and message
   /// name may consist of letters, numbers, periods, and underscores.  They are not case sensitive.
   /// \note This class does not take ownership of the MsgFactory pointer.
   /// \param aGroup             Connection group name for this object.
   /// \param aName              Connection name for this object
   /// \param aMsgFactory        A pointer to a message factory.
   /// \param aType              For future growth, for now just use the locally defined enumerated value TCP_IP"
   /// \param aCallingClassNmae  A name to desribe this object in error messages.
   /// \param aGroupEvn          An environment variable that can be used to override the aGroup value at run time.
   /// \param aNameEvn           An environment variable that can be used to override the aName value at run time.
   /// \param aUpdatePeriodSec   Time between calls to Send or Receive, only needed for RUDP.
   GenUmpIOServer(const char*        aGroup,
                  const char*        aName,
                  MsgFactoryType*    aMsgFactory,
                  GenUmpIOServerType aType,
                  const char*        aCallingClassName,
                  const char*        aGroupEnv        = 0,
                  const char*        aNameEnv         = 0,
                  float              aUpdatePeriodSec = 0.05F);

   /// Destructor
   virtual ~GenUmpIOServer();

   /// Initiallize the class to a user defined receive TCP port
   /// The class should be instantiated with the generic constructor to use this function.
   /// \param aRecvOnPort        Port number that clients should use to make their initial connection.
   /// \param aType              For future growth, for now just use the locally defined enumerated value TCP_IP".
   /// \return A positive number if successful.
   int Init(unsigned int aRecvOnPort, GenUmpIOServerType aType);

   /// Initiallize the class to a named address.  The UMP Arbiter will be used to find
   /// the correcet address and port. The message group name and message
   /// name may consist of letters, numbers, periods, and underscores.  They are not case sensitive.
   /// The class should be instantiated with the generic constructor to use this function.
   /// \param aGroup     Connection group name for this object.
   /// \param aName      Connection name for this object
   /// \param aType      For future growth, for now just use the locally defined enumerated value "TCP_IP"
   /// \param aGroupEvn  An environment variable that can be used to override the aGroup value at run time.
   /// \param aNameEvn   An environment variable that can be used to override the aName value at run time.
   /// \return Ay positvie number if successful.
   int Init(const char*        aGroup,
            const char*        aName,
            GenUmpIOServerType aType,
            const char*        aGroupEnv = 0,
            const char*        aNameEnv  = 0);

   /// Start call back process to receive data from clients
   /// \param aTimeOutMicroSecs : The timeout value is in micro seconds. Use the UMP enumerated type
   ///                            of "UMP_WAIT_FOREVER" for an infinite wait.
   /// \retval -1  No messages were received.
   /// \retval  0  At least one message was received and no errors occurred.
   /// \retval <-1  An error occurred.
   int Receive(int aTimeOutMicroSecs = 0);

   /// Get the receive port
   /// \return Port number clients ust to make their initial connection.
   int GetReceiveOnPort();

   /// Allows you to manually add a client connection.  Handy for setting up a broadcast or
   /// multicast connection.
   /// \param aClientConnection  A pointer to a user created GenUmpIOServerCC class. <br>This class will take
   ///                            ownership of the "aClientConnection" pointer if it is successfully added.</br>.
   /// \retval  1  GenUmpIOServerCC was successfully added.
   /// \retval -1  GenUmpIOServerCC does not have a unique name and was not added.
   /// \retval -2  GenUmpIOServerCC has an empty name and was not added.
   int AddClientConnection(GenUmpIOServerCC<MsgType, MsgFactoryType>* aClientConnection);

   /// Deletes the client connecition.
   /// \param aClientConnectioin Pointer to the GenUmpIOServerCC that needs to be deleted.
   void RemoveCC(GenUmpIOServerCC<MsgType, MsgFactoryType>* aClientConnection, int aClientSocket);

   /// Put message to one client based on the client name.  Caution -- unique client names are not enforced.  Therefore,
   /// if multiple clients connect using the same client name, the message will only go to the client that connected first.
   /// \param aMsg          Pointer to the message.
   /// \param aClientName   Name of client message should go to.
   /// \return A pointer to the client if it exists under the name aClientName.
   GenUmpIOServerCC<MsgType, MsgFactoryType>* PutMsg(const MsgType* aMsg, const std::string& aClientName);

   // Put message to all clients.
   /// \param aMsg  Pointer to the message.
   void PutMsg(const MsgType* aMsg);

   /// Send raw data to all clients
   /// \param aDataBlob  Pointer to the data
   /// \param aDataSize  Number of bytes to send
   void PutRawData(const void* aDataBlob, unsigned long aDataSize);

   /// Send all data put in the send buffer to all the clients.
   void Send();

   /// Set flag to compute server receive statistics.  Statistics are not computed by default.
   /// \param aCollectRecvStats  A nonzero value turns on calculation of receive statistics.
   void SetCollectRecvStats(unsigned int aCollectRecvStats);

   /// Get receive statistics.  SetCollectRecvStats needs to be called first to start computing
   /// receive statistics.
   /// \param out aServerCCStatsVec    A vector of GenServerCCStats.
   /// \param out aActualRecvWaitTime  Acutual time spent in umpMakeCallbacks.
   void GetGenServerCCRecvStats(std::vector<GenServerCCStats>& aServerCCStatsVec, float& aActualRecvWaitTime);

protected:
   const std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>& GetClientConnectionVec() const;
   const std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil>&
                   GetClientConnectionHash() const;
   MsgFactoryType* GetMsgFactory() const;

   /// Redefine MsgFactory.  Most users will use the factory they set in the constructor.
   /// \note This class does not take ownership of the MsgFactory pointer.
   /// \param A pointer to a Message Factory
   void SetMsgFactory(MsgFactoryType* aMsgFactory);

private:
   // Disallow copy and assignment
   GenUmpIOServer(const GenUmpIOServer& aSrc);
   GenUmpIOServer& operator=(const GenUmpIOServer& aRhs);

   // The function UMP calls when a client first makes connection.
   static int ConnectCallback(int aServerSocket, int aHandle, void* aUserPtr);

   // The function UMP calls when a client message arrives and is ready to be received.
   // aClientSocket : a socket that has a message ready to be received.
   static int ReceiveCallback(int aClientSocket, int aHandle, void* aUserPtr);

   // An internal method to start the creation of a client connection.
   GenUmpIOServerCC<MsgType, MsgFactoryType>* StartCreateCC(int aNewSocket);

   /// Let a derived class instantiate their version of a GenUmpIOServerCC class.
   /// If the user dervies from GenUmpIOServerCC, this function
   /// needs to be redefined in a user derived class so the
   /// correct client connetion class is instantiated.
   /// \param aNewSocket The number of the UMP socket to be passed on to the new GenUmpIOServerCC
   virtual GenUmpIOServerCC<MsgType, MsgFactoryType>* CreateCC(int aNewSocket);

   /// Let a derived class have a chance at a new connection.
   /// \param Pointer to a CenUmpIOServerCC object.
   /// \param aStatus Set to greater than zero  if the new client connection has a unique name and was successfully
   /// added to the hash_map
   ///                Set to -1 if the name is not unique
   ///                Set to -2 if the name is empty
   virtual void ProcessNewConnection(GenUmpIOServerCC<MsgType, MsgFactoryType>* aClientConnection, int aStatus);

   /// Let a derived class have a chance at the client connection
   /// just prior to its deletion.
   /// \param Pointer to a CenUmpIOServerCC object.
   virtual void StartRemoveCC(GenUmpIOServerCC<MsgType, MsgFactoryType>* aClientConnection);

   // An internal method to register callbacks for UMP.
   void RegisterConnectCB();

   std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>                                   mClientConnectionVec;
   std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil> mClientConnectionHash;
   std::unordered_map<unsigned int, int>                                                     mConnectionCountHash;

   UtWallClock     mProcessTimer;
   MsgFactoryType* mMsgFactory;
   float           mActualRecvWaitTime;
   unsigned int    mCollectRecvStats;
};


template<typename MsgType, typename MsgFactoryType>
GenUmpIOServer<MsgType, MsgFactoryType>::GenUmpIOServer(MsgFactoryType* aMsgFactory,
                                                        const char*     aCallingClassName,
                                                        float           aUpdatePeriodSec)
   : GenUmpIOServerPrv(aCallingClassName, aUpdatePeriodSec)
   , mMsgFactory(aMsgFactory)
   , mActualRecvWaitTime(0.0F)
   , mCollectRecvStats(0)
{
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServer<MsgType, MsgFactoryType>::GenUmpIOServer(unsigned int       aRecvOnPort,
                                                        MsgFactoryType*    aMsgFactory,
                                                        GenUmpIOServerType aType,
                                                        const char*        aCallingClassName,
                                                        float              aUpdatePeriodSec)
   :

   GenUmpIOServerPrv(aCallingClassName, aUpdatePeriodSec)
   , mMsgFactory(aMsgFactory)
   , mActualRecvWaitTime(0.0F)
   , mCollectRecvStats(0)
{
   Init(aRecvOnPort, aType);
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServer<MsgType, MsgFactoryType>::GenUmpIOServer(const char*        aGroup,
                                                        const char*        aName,
                                                        MsgFactoryType*    aMsgFactory,
                                                        GenUmpIOServerType aType,
                                                        const char*        aCallingClassName,
                                                        const char*        aGroupEnv,
                                                        const char*        aNameEnv,
                                                        float              aUpdatePeriodSec)
   :

   GenUmpIOServerPrv(aCallingClassName, aUpdatePeriodSec)
   , mMsgFactory(aMsgFactory)
   , mActualRecvWaitTime(0.0F)
   , mCollectRecvStats(0)
{
   Init(aGroup, aName, aType, aGroupEnv, aNameEnv);
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServer<MsgType, MsgFactoryType>::~GenUmpIOServer()
{
   // Remove all the remaining interfaces
   std::for_each(mClientConnectionVec.begin(), mClientConnectionVec.end(), UtDeleteObj());
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::Init(unsigned int aRecvOnPort, GenUmpIOServerType /* aType */)
{
   if (mSocketRecv < 0)
   {
      mSocketRecv = umpCreateSocketC(0, 0, aRecvOnPort, TCP_SERVER, NO_CONVERT, UMP_NON_BLOCKING_TCP);

      if (mSocketRecv >= 0)
      {
         RegisterConnectCB();
      }
      else
      {
         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "Error in GenUmpIOServer::Init(unsigned int) opening socket on port ";
         std::cout << aRecvOnPort << std::endl;
      }
   }
   return mSocketRecv;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::Init(const char* aGroup,
                                                  const char* aName,
                                                  GenUmpIOServerType /* aType */,
                                                  const char* aGroupEnv,
                                                  const char* aNameEnv)
{
   if (mSocketRecv < 0)
   {
      // Could maybe check for an Ump arbiter running??
      const char* group;
      if (aGroupEnv && *aGroupEnv != '\0')
      {
         group = getenv(aGroupEnv);
         if (group == 0)
         {
            group = aGroup;
         }
      }
      else
      {
         group = aGroup;
      }

      const char* name;
      if (aNameEnv && *aNameEnv != '\0')
      {
         name = getenv(aNameEnv);
         if (name == 0)
         {
            name = aName;
         }
      }
      else
      {
         name = aName;
      }

      mSocketRecv = umpCreateNamedSocketC(name, group, TCP_SERVER, NO_CONVERT, UMP_NON_BLOCKING_TCP);

      if (mSocketRecv >= 0)
      {
         RegisterConnectCB();
      }
      else
      {
         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "Error in GenUmpIOServer::Init(const char*, const char*) ";
         std::cout << "opening named socket for " << group << "  " << name << std::endl;
      }
   }
   return mSocketRecv;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::Receive(int aTimeOutMicroSecs)
{
   int status;

   if (mCollectRecvStats)
   {
      UtWallClock waitTimer;

      // Look for incoming client connections and messages since last call/
      // int status = umpMakeCallbacksC(mHandleIndex, 0, 0);
      status = umpMakeCallbacksC(mHandleIndex, aTimeOutMicroSecs, 0);

      mActualRecvWaitTime = static_cast<float>(waitTimer.GetClock());
   }
   else
   {
      // Look for incoming client connections and messages since last call/
      // int status = umpMakeCallbacksC(mHandleIndex, 0, 0);
      status = umpMakeCallbacksC(mHandleIndex, aTimeOutMicroSecs, 0);
   }

   if (status < -1)
   {
      std::cout << GenTime::GetWallClockTime() << " ";
      std::cout << "GenUmpIOServer::Receive() had error: " << status << std::endl;
   }

   return status;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::GetReceiveOnPort()
{
   if (mSocketRecv != -99999)
   {
      UMPSockInfoType socketRecvInfo;
      umpGetSocketInfoC(mSocketRecv, &socketRecvInfo, 0);
      return static_cast<int>(socketRecvInfo.localPort);
   }
   else
   {
      return -1;
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::AddClientConnection(GenUmpIOServerCC<MsgType, MsgFactoryType>* aClientConnection)
{
   if (!aClientConnection->GetClientName().empty())
   {
      // Ensure this new ClientConnection has a unique name
      std::pair<typename std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil>::iterator, bool>
         answer(mClientConnectionHash.insert(
            std::pair<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*>(aClientConnection->GetClientName(),
                                                                               aClientConnection)));
      if (answer.second)
      {
         if (aClientConnection->GetUmpMsgType() != GenUmpIO::UDP_SEND_ONLY)
         {
            umpRegisterCallbackC(mHandleIndex,
                                 aClientConnection->GetRecvDataSocket(),
                                 aClientConnection, // Info about the client
                                 ReceiveCallback);
         }

         // Set update time
         aClientConnection->SetUpdatePeriodSec(GetUpdatePeriodSec());

         // Take ownership of the Client Connection
         mClientConnectionVec.push_back(aClientConnection);

         return 1;
      }
      else
      {
         // ClientConnection does not have a unique name
         return -1;
      }
   }
   else
   {
      // ClientConnection does not have a valid name
      return -2;
   }
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::RemoveCC(GenUmpIOServerCC<MsgType, MsgFactoryType>* aServerCCPtr,
                                                       int                                        aClientSocket)
{
   // A client disconnected...
   // std::cout << "Got a receive delete on socket: " << aUmpSocket << std::endl;

   UMPSockInfoType sockInfoType;
   int             status = umpGetSocketInfoC(aClientSocket, &sockInfoType, 0);
   if (status > 0)
   {
      std::unordered_map<unsigned int, int>::iterator answer(mConnectionCountHash.find(sockInfoType.srcInetAddr));
      if (answer != mConnectionCountHash.end())
      {
         mConnectionCountHash[sockInfoType.srcInetAddr] = --answer->second;
         if (answer->second >= 0)
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "GenUmpIOServer::RemoveCC removed client socket: " << aClientSocket;
            std::cout << " from ";
            std::cout << (sockInfoType.srcInetAddr >> 24 & 0xFF) << ".";
            std::cout << (sockInfoType.srcInetAddr >> 16 & 0xFF) << ".";
            std::cout << (sockInfoType.srcInetAddr >> 8 & 0xFF) << ".";
            std::cout << (sockInfoType.srcInetAddr & 0xFF);
            std::cout << " with count " << answer->second << std::endl;
         }
         else
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "GenUmpIOServer::RemoveCC connection count just went negative for: " << sockInfoType.srcInetAddr;
         }
      }
      else
      {
         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "GenUmpIOServer::RemoveCC connection all messed up for: " << sockInfoType.srcInetAddr;
      }
   }
   else
   {
      std::cout << GenTime::GetWallClockTime() << " ";
      std::cout << "GenUmpIOServer::ConnectCallback could not get good info on socket: " << aClientSocket << std::endl;
   }

   if (aServerCCPtr != 0)
   {
      // Let any derived classes know what is happening
      StartRemoveCC(aServerCCPtr);

      // Remove from our vector and hash maps
      mClientConnectionVec.erase(std::remove(mClientConnectionVec.begin(), mClientConnectionVec.end(), aServerCCPtr),
                                 mClientConnectionVec.end());

      if (aServerCCPtr->GetNameValid() > 0)
      {
         mClientConnectionHash.erase(aServerCCPtr->GetClientName());
      }

      delete aServerCCPtr;
   }
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>* GenUmpIOServer<MsgType, MsgFactoryType>::PutMsg(const MsgType*     aMsg,
                                                                                           const std::string& aClientName)
{
   typename std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil>::iterator iter(
      mClientConnectionHash.find(aClientName));
   if (iter != mClientConnectionHash.end())
   {
      (*iter).second->PutMsg(aMsg);

      // Success
      return (*iter).second;
   }
   else
   {
      // Bad name
      return 0;
   }
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::PutMsg(const MsgType* aMsg)
{
#if defined(__host_mips) && _COMPILER_VERSION < 740
   typename std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>::iterator iter(mClientConnectionVec.begin());
   while (iter != mClientConnectionVec.end())
   {
      (*iter)->PutMsg(aMsg);
      ++iter;
   }
#else
   std::for_each(mClientConnectionVec.begin(),
                 mClientConnectionVec.end(),
                 std::bind2nd(std::mem_fun(&GenUmpIOServerCC<MsgType, MsgFactoryType>::PutMsg), aMsg));
#endif
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::PutRawData(const void* aDataBlob, unsigned long aDataSize)
{
   typename std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>::iterator iter(mClientConnectionVec.begin());
   while (iter != mClientConnectionVec.end())
   {
      (*iter)->PutRawData(aDataBlob, aDataSize);
      ++iter;
   }
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::Send()
{
   std::for_each(mClientConnectionVec.begin(),
                 mClientConnectionVec.end(),
                 std::mem_fun(&GenUmpIOServerCC<MsgType, MsgFactoryType>::Send));
}


template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::SetCollectRecvStats(unsigned int aCollectRecvStats)
{
   mCollectRecvStats = aCollectRecvStats;
}


template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::GetGenServerCCRecvStats(std::vector<GenServerCCStats>& aServerCCStatsVec,
                                                                      float& aActualRecvWaitTime)
{
   // Set size
   aServerCCStatsVec.resize(mClientConnectionVec.size() + 1);

   std::string serverName("GenUmpIOServer called by ");
   serverName.append(mCallingClassName);

   aServerCCStatsVec[0].SetClientName(serverName);
   aServerCCStatsVec[0].SetProcessTime(0.0F);
   aServerCCStatsVec[0].SetNumRecvsSends(0);
   aServerCCStatsVec[0].SetNumBytes(0);

   unsigned int                                                               j = 0;
   typename std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>::iterator iter(mClientConnectionVec.begin());
   while (iter != mClientConnectionVec.end())
   {
      (*iter)->GetGenServerCCRecvStats(aServerCCStatsVec[++j]);
      aServerCCStatsVec[0] += aServerCCStatsVec[j];
      ++iter;
   }

   aActualRecvWaitTime = mActualRecvWaitTime;
}


//
// Protected
//

template<typename MsgType, typename MsgFactoryType>
inline const std::vector<GenUmpIOServerCC<MsgType, MsgFactoryType>*>&
GenUmpIOServer<MsgType, MsgFactoryType>::GetClientConnectionVec() const
{
   return mClientConnectionVec;
}

template<typename MsgType, typename MsgFactoryType>
inline const std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil>&
GenUmpIOServer<MsgType, MsgFactoryType>::GetClientConnectionHash() const
{
   return mClientConnectionHash;
}


template<typename MsgType, typename MsgFactoryType>
inline MsgFactoryType* GenUmpIOServer<MsgType, MsgFactoryType>::GetMsgFactory() const
{
   return mMsgFactory;
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::SetMsgFactory(MsgFactoryType* aMsgFactory)
{
   mMsgFactory = aMsgFactory;
}


//
// Private
//

// Static
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::ConnectCallback(int aServerSocket, int aHandleIndex, void* /* aUserPtr */)
{
   // Create a new TCP_PASSIVE socket connected to the client
   int clientSocket = umpAcceptClientConnectionC(aServerSocket, 0);

   if (clientSocket >= 0)
   {
      // Since the pointer to this server class was held by our trusted base class
      // I feel OK about doing this reinterprest_cast.  Besides it is the only way
      // to make this work short of a dynamice cast
      GenUmpIOServer<MsgType, MsgFactoryType>* genUmpIOServerPtr =
         reinterpret_cast<GenUmpIOServer<MsgType, MsgFactoryType>*>(sHandleList[aHandleIndex]);

      // Get a GenUmpIOServerCC object by calling virtual function Create
      GenUmpIOServerCC<MsgType, MsgFactoryType>* serverCCPtr = genUmpIOServerPtr->StartCreateCC(clientSocket);

      if (serverCCPtr != 0)
      {
         // Add to new connection to the ump callback list
         umpRegisterCallbackC(aHandleIndex,
                              clientSocket,
                              serverCCPtr, // Pointer to the GenUmpIOServerCC
                              ReceiveCallback);
      }
   }
   else
   {
      std::cout << "GenUmpIOServer::ConnectCallback error number: " << clientSocket << std::endl;
   }
   return 0;
}


// Static
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServer<MsgType, MsgFactoryType>::ReceiveCallback(int aClientSocket, int aHandleIndex, void* aUserPtr)
{
   int status;

   // std::cout << "Got a receive on socket: " << aUmpSocket << std::endl;
   if (aUserPtr != 0)
   {
      GenUmpIOServer<MsgType, MsgFactoryType>* genUmpIOServer =
         reinterpret_cast<GenUmpIOServer<MsgType, MsgFactoryType>*>(sHandleList[aHandleIndex]);

      // Start timer if we are letting UMP wait for data
      if (genUmpIOServer->mCollectRecvStats)
      {
         genUmpIOServer->mProcessTimer.ResetClock();
      }

      GenUmpIOServerCC<MsgType, MsgFactoryType>* serverCCPtr =
         reinterpret_cast<GenUmpIOServerCC<MsgType, MsgFactoryType>*>(aUserPtr);

      int msgSize = serverCCPtr->GetBytesReceived(aClientSocket);

      if (msgSize == UMP_CONNECTION_CLOSED)
      {
         // Take off the ump list.  Needs to be called before
         // closing the socket
         umpUnregisterCallbackC(aHandleIndex, aClientSocket);

         // Look for a seperated data connection that must be taken off the list as well
         if (serverCCPtr->GetRecvDataSocket() != 0 && serverCCPtr->GetRecvDataSocket() != aClientSocket)
         {
            umpUnregisterCallbackC(aHandleIndex, serverCCPtr->GetRecvDataSocket());
         }

         // Delete the client connection and tell user this connection is going away
         genUmpIOServer->RemoveCC(serverCCPtr, aClientSocket);

         status = 0;
      }
      else if (msgSize < 0)
      {
         // Give the user a chance to do something about the error
         if (msgSize < -1)
         {
            serverCCPtr->HandleRecvError(msgSize);
         }

         // An error occurred.  Force 'umpMakeCallbacks()' to return with
         // with the error.
         status = msgSize;
      }
      else
      {
         // Data has been received.  Tell the user.
         // These "data" may be raw data or GenMsg type.  The derived client
         // connection class has its own implementation of
         // HandleReceivedMsgs().
         int newDataSocket = 0;
         int newConnection = 0;
         serverCCPtr->HandleReceivedMsgs(aClientSocket, newDataSocket, newConnection);

         // Add new connection to hash map
         if (newConnection != 0)
         {
            // First check if name has been defined
            int goodName = 1;
            if (serverCCPtr->GetClientName().empty())
            {
               goodName = -2;
            }

            // Next look for a unique name and if so add to hash_map
            if (goodName > 0)
            {
               std::pair<typename std::unordered_map<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*, UtStringUtil>::iterator, bool>
                  answer(genUmpIOServer->mClientConnectionHash.insert(
                     std::pair<std::string, GenUmpIOServerCC<MsgType, MsgFactoryType>*>(serverCCPtr->GetClientName(),
                                                                                        serverCCPtr)));
               if (!answer.second)
               {
                  goodName = -1;
               }
            }

            if (goodName > 0)
            {
               // Got a unique connection.
               // Add a new data socket if one just got created
               if (newDataSocket != 0)
               {
                  umpRegisterCallbackC(aHandleIndex,
                                       newDataSocket,
                                       serverCCPtr, // Info about the client
                                       ReceiveCallback);
               }
            }

            // Give a derived class a chance to deal with the new connection
            genUmpIOServer->ProcessNewConnection(serverCCPtr, goodName);

            // Tell client connection status.  Cleint will eventually kill the connection if it is a duplicate.
            serverCCPtr->SendConnectMsg(goodName);
         }

         // Stop timer if we are letting UMP wait for data
         if (genUmpIOServer->mCollectRecvStats)
         {
            serverCCPtr->SetRecvStats(static_cast<float>(genUmpIOServer->mProcessTimer.GetClock()),
                                      static_cast<unsigned int>(msgSize));
         }

         // Tell ump all is well
         status = 0;
      }
   }
   else
   {
      std::cout << GenTime::GetWallClockTime() << " ";
      std::cout << "GenUmpIOServer::ReceiveCallback had no user data." << std::endl;
      status = -1;
   }

   return status;
}

// Private
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::RegisterConnectCB()
{
   sHandleList.push_back(this);
   if (sHandleList.size() > UMP_MAX_LIST_HANDLE)
   {
      std::cout << GenTime::GetWallClockTime() << " ";
      std::cout << "GenUmpIOServer::GenUmpIOServer list handle overflow.  " << std::endl;
      std::cout << "Errors will start occuring" << std::endl;
      mHandleIndex = UMP_MAX_LIST_HANDLE;
   }
   else
   {
      mHandleIndex = static_cast<int>(sHandleList.size()) - 1;
      // Register a message callback for the tcpServerSock socket on the
      // next available list e
      // tcpServerSock is a TCP_SERVER socket, you can't receive messages
      // through it.  It can only be used to accept connections.
      // Therefore, clientConnectCallback() is really a
      // connection-accepting callback.  When it "receives" a connection
      // request, it accepts the connection and registers the new client
      // socket on the same list.  That way, the same umpMakeCallbacks()
      // below that is in an infinite timeout handles both accepting
      // connections and receiving data messages from all clients. */
      umpRegisterCallbackC(mHandleIndex, mSocketRecv, 0, ConnectCallback);
   }
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>* GenUmpIOServer<MsgType, MsgFactoryType>::StartCreateCC(int aClientSocket)
{
   GenUmpIOServerCC<MsgType, MsgFactoryType>* serverCCPtr = 0;

   UMPSockInfoType sockInfoType;
   int             status = umpGetSocketInfoC(aClientSocket, &sockInfoType, 0);
   if (status > 0)
   {
      // Determine current number of connections for the client IP address
      std::unordered_map<unsigned int, int>::iterator answer(mConnectionCountHash.find(sockInfoType.srcInetAddr));
      int numConnects = answer != mConnectionCountHash.end() ? answer->second : 0;

      // Limit connections from a given IP address to 15, so a client cannot overrun any UMP buffers
      if (numConnects < 15)
      {
         // Increment connection numConnects
         mConnectionCountHash[sockInfoType.srcInetAddr] = ++numConnects;

         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "GenUmpIOServer::StartCreateCC added client socket: " << aClientSocket << " from ";
         std::cout << (sockInfoType.srcInetAddr >> 24 & 0xFF) << ".";
         std::cout << (sockInfoType.srcInetAddr >> 16 & 0xFF) << ".";
         std::cout << (sockInfoType.srcInetAddr >> 8 & 0xFF) << ".";
         std::cout << (sockInfoType.srcInetAddr & 0xFF);
         std::cout << " with count " << numConnects << std::endl;

         // Get a GenUmpIOServerCC object by calling virtual function Create
         serverCCPtr = CreateCC(aClientSocket);

         // Set update time
         serverCCPtr->SetUpdatePeriodSec(GetUpdatePeriodSec());

         // Add to vector
         mClientConnectionVec.push_back(serverCCPtr);
      }
      else
      {
         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "Error - GenUmpIOServer::StartCreateCC exceeded max number of connections for socket: "
                   << sockInfoType.srcInetAddr << std::endl;
      }
   }
   else
   {
      std::cout << GenTime::GetWallClockTime() << " ";
      std::cout << "GenUmpIOServer::StartCreateCC could not get good info on socket: " << aClientSocket << std::endl;
   }

   return serverCCPtr;
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::ProcessNewConnection(
   GenUmpIOServerCC<MsgType, MsgFactoryType>* /* aClientConnection */,
   int /* aStatus */)
{
   // aStatus will be greater than zero if the new connection was added to the hash map successfully
}


// Virtual
template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>* GenUmpIOServer<MsgType, MsgFactoryType>::CreateCC(int aNewSocket)
{
   return new GenUmpIOServerCC<MsgType, MsgFactoryType>(aNewSocket, mMsgFactory, mCallingClassName);
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServer<MsgType, MsgFactoryType>::StartRemoveCC(GenUmpIOServerCC<MsgType, MsgFactoryType>* /* aClientConnection */)
{
   // Do nothing here, but a derived class could want to do something
}


#endif
