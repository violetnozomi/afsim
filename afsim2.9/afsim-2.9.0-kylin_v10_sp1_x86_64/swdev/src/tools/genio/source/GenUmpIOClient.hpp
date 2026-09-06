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


#ifndef GEN_UMP_IO_CLIENT_HPP
#define GEN_UMP_IO_CLIENT_HPP

// \author  Keith Stanley
// \date    Aug 28, 2008
// \version 1.0

/// \file  GenUmpIOClient.hpp
/// \brief GenUmpIOClient
/// \class GenUmpIOClient GenUmpIOClient.hpp "GenUmpIOClient.hpp"
/// \brief A Client for GenUmpIOServer
///
/// Each instantiation of this class represents a unique connection to a GenUmpIOServer.\n
///
/// This class uses the concept that messages or data is "Put" to a send buffer for bundling and then sent at once
/// with the Send() method.  If the send buffer is full, the buffered data will automatically be sent before accepting
/// additional data from PutMsg() or PutData().  Data is not guaranteed to be sent until the Send() method is called.
/// A call to Send() with an empty send buffer is harmless.\n
///
/// If using Reliable UDP (RUDP) a call to Send() should be made on a regular heartbeat, so the UMP has a chance at the
/// processor. See the UMP manual for more details on using RUDP.
///
/// An example of a client/server setup with some sample messages can be found in a typical
/// workspace at <tt>unclass/tools/genio/client_server_example</tt>.

#include <iostream>
#include <stdlib.h>
#include <string>

#include "GenError.hpp"
#include "GenIHelper.hpp"
#include "GenIOMsgConnect.hpp"
#include "GenIOMsgStatus.hpp"
#include "GenOHelper.hpp"
#include "GenThreadControl.hpp"
#include "GenTime.hpp"
#include "GenUmpIO.hpp"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

template<typename MsgType, typename MsgFactoryType>
class ClientToFeederPrv;
template<typename MsgType, typename MsgFactoryType>
class ClientToLoggerPrv;

template<typename MsgType, typename MsgFactoryType = GenIFactory<MsgType>>
class GenUmpIOClient
{
public:
   enum GenUmpIOClientType
   {
      TCP_IP       = 1,
      RELIABLE_UDP = 2,
      UDP          = 3
   };


   /// Constructor.
   ///
   /// @param aUmpMsgType            Locally defined enumerated values are TCP_IP, RELIABLE_UDP, or UDP
   /// @param aMsgFactory            A pointer to message factory.
   /// @param aGenThreadControl      A pointer to a GenThreadControl object.  This logic is no longer used.
   ///                               It will be removed in the future.  For now a straight GenThreadControl
   ///                               object will work fine.
   /// @param aName                  A user defined name that is used for error messages and can be used by GenUmpIOServer
   ///                               to indentify this object if requested by the user.  Although not required, it is best
   ///                               if this name is unique among all clients communicating with a given server.
   /// @param aLocalUpdatePeriodSec  Update period for Reliable UDP logic.  See the UMP manual for details on RUDP.
   GenUmpIOClient(GenUmpIOClientType aUmpMsgType,
                  MsgFactoryType*    aMsgFactroy,
                  GenThreadControl*  aGenThreadControl,
                  const char*        aName,
                  float              aLocalUpdatePeriodSec = 0.05F);

   /// Destructor
   ~GenUmpIOClient();

   /// Most users will find the LocalUpdatePeriodSec parameters will
   /// set the proper RUDP values, however, if needed this function can be used to
   /// override LocalUpdatePeriodSec.
   /// <b>This function must be called before calling Init(...)</b>
   void SetRudpStatOverrides(int aRetransmitTimeOutMicroSec, int aSendQueueLength, int aRecvQueueLength);


   /// Init function must be called and positve number returned before attempting to send or receive data
   /// This function may take a full second to establish a connection, but
   /// in practice a connection is generally established in 0.2 seconds.  The message group name and message
   /// name may consist of letters, numbers, periods, and underscores.  They are not case sensitive.
   ///
   /// @param aIOGroup        UMP Arbiter Message Group Name.
   /// @param aIOName         UMP Arbiter Message Name.
   /// @param aGroupEvn       An environment variable that can be used to override the aIOGroup value at run time.
   /// @param aNameEvn        An environment variable that can be used to override the aIOName value at run time.
   /// @param aUmpMsgTypeEnv  An environment variable that can be used to override the constructor's
   ///                        aUmpMsgType value at run time.
   /// @param aUmpOptions     A bitwise value passed to umpCreateNamedSocket().  See the UMP manual for more details.
   /// @return A positive number when successfully connected.
   /// \retval -99994 Error opening UDP or RUDP data socket
   /// \retval -99995 Server did not return a valid send to port.
   /// \retval -99996 No client name was provided.
   /// \retval -99997 Duplicant client name was provided.
   /// \retval -99999 Could not get a TCP connection to the server.
   int Init(const char* aIOGroup,
            const char* aIOName,
            const char* aGroupEnv      = 0,
            const char* aNameEnv       = 0,
            const char* aUmpMsgTypeEnv = 0,
            int         aUmpOptions    = GenUmpIO::QUEUED | GenUmpIO::UMP_NON_BLOCKING_TCP);


   /// Get a message.  Repeated calls should be made to obtain all the messages in the receive buffer.\n
   /// <b>User is responsible for the message pointer.</b>
   /// @return A pointer to a vaild message or zero if the receive buffer is empty.
   MsgType* GetMsg();

   /// Get method for raw data.  No byte swapping is performed.  No data is physically
   /// moved, but the "get" pointers are incremented if there is valid data.   This function
   /// is not used by most users.
   /// @param aData        A void pointer that is set by this function defining the start of the data.
   /// @param aSizeInBytes The number of bytes read or used by the user and no longer of interest.  The "get" pointers
   ///                     are incremented by this value.
   /// @return If there is an error, returns a negative number and bufferPtr is set to zero,
   /// else returns the number of bytes that are expected to be read from the stream and
   /// a valid bufferPtr.
   int GetData(const void*& aData, unsigned long aSizeInBytes);

   /// Put a message in the send buffer.
   /// @param aMsg A pointer to the message to be put.
   /// @return The number of bytes put on the buffer if successful, else a negative number if there is an error.
   int PutMsg(const MsgType* aMsg);

   /// Put raw data in the send buffer.  No byte swapping is performed.
   /// @param aData         A pointer to the raw data.
   /// @param aSizeInBytes  Number of bytes to be put on the send buffer.
   /// @return The number of bytes put on the buffer if successful if successful, else a negative number if there is an error.
   int PutData(const void* aData, unsigned long aSizeInBytes);

   /// Send any data in the send buffer.  It is OK to call this method if there is no data in the put buffer.
   /// For a Reliable UDP connection this method should be called on a heart beat.  See the UMP manual for more details
   /// @return The number of bytes sent or a negative number if there is an error.
   /// \retval     -1 If was connected, but now connection lost.
   /// \retval -99994 Error opening UDP or RUDP data socket
   /// \retval -99995 Server did not return a valid send to port.
   /// \retval -99996 No client name was provided
   /// \retval -99997 Duplicant client name was provided
   int Send();

   /// Receive only needs to be called if one is calling GetData(...).
   /// If using GetData(...), call this function first, call GetData(...) as needed, and then
   /// repeat by calling this function followed by GetData(...) until no more data is received by this function.
   /// Receive only needs to be called if one is calling GetData(...)
   /// \return The number of bytes received, else a zero for an empty data packet received, else a -1 for no data packet
   /// received, else a number less than -1 for an error.

   /// \retval >=0   Data received.  This value could optionally be the number of bytes received.
   /// \retval -1   No data has been received.
   /// \retval -99994 Error opening UDP or RUDP data socket
   /// \retval -99995 Server did not return a valid send to port.
   /// \retval -99996 No client name was provided
   /// \retval -99997 Duplicant client name was provided
   int Receive();

   /// Check connection status.
   /// \retval      1 If connected.
   /// \retval      0 If never connected.
   /// \retval     -1 If was connected, but now connection lost.
   /// \retval -99994 Error opening UDP or RUDP data socket
   /// \retval -99995 Server did not return a valid send to port.
   /// \retval -99996 No client name was provided
   /// \retval -99997 Duplicant client name was provided
   int GetConnectState() const;

   /// Get UMP connection type.
   /// \retval 1 If TCP.
   /// \retval 2 If Reliable UDP.
   /// \retval 3 If UDP.
   GenUmpIOClientType GetUmpMsgType() const;


private:
   /// \retval A valid pointer if client is successfully connected
   GenUmpIO* GetGenUmpIOData();

   // Must use public constructor above
   GenUmpIOClient();

   // Hide all the copy stuff
   GenUmpIOClient(const GenUmpIOClient& srcA);
   GenUmpIOClient& operator=(const GenUmpIOClient& srcA);

   int  ReceiveBytes();
   int  MakeConnection();
   int  MakeTcpConnection();
   int  ConfirmDataConnection();
   void KeepTcpConnectionAlive();

   GenUmpIO                             mGenUmpIOConnect;
   GenUmpIO*                            mGenUmpIOData;
   GenIHelper<MsgType, MsgFactoryType>* mGenIHelper;
   GenOHelper<MsgType>*                 mGenOHelper;
   MsgFactoryType*                      mMsgFactory;
   GenThreadControl*                    mGenThreadControl;
   std::string                          mIOGroup;
   std::string                          mIOName;
   std::string                          mName;
   std::string                          mConnectionType;
   GenUmpIOClientType                   mUmpMsgType;
   int                                  mLookupOk;
   int                                  mDataStatus;
   int                                  mTcpConnectOk;
   int                                  mDataConnectOk;
   int                                  mUmpOptions;
   int                                  mRudpRetransmitTimeOut;
   int                                  mRudpSendQueueLength;
   int                                  mRudpRecvQueueLength;
   unsigned int                         mMsgCount;
   float                                mLocalUpdatePeriodSec;

   friend class ClientToFeederPrv<MsgType, MsgFactoryType>;
   friend class ClientToLoggerPrv<MsgType, MsgFactoryType>;
};

template<typename MsgType, typename MsgFactoryType>
GenUmpIOClient<MsgType, MsgFactoryType>::GenUmpIOClient(GenUmpIOClientType aUmpMsgType,
                                                        MsgFactoryType*    aMsgFactory,
                                                        GenThreadControl*  aGenThreadControl,
                                                        const char*        aName,
                                                        float              aLocalUpdatePeriodSec)
   : mGenUmpIOData(0)
   , mGenIHelper(0)
   , mGenOHelper(0)
   , mMsgFactory(aMsgFactory)
   , mGenThreadControl(aGenThreadControl)
   , mName(aName != 0 ? aName : "")
   , mUmpMsgType(aUmpMsgType)
   , mLookupOk(-1)
   , mDataStatus(-1)
   , mTcpConnectOk(-1)
   , mDataConnectOk(-1)
   , mRudpRetransmitTimeOut(0)
   , mRudpSendQueueLength(0)
   , mRudpRecvQueueLength(0)
   , mMsgCount(5000)
   , mLocalUpdatePeriodSec(aLocalUpdatePeriodSec)
{
}


template<typename MsgType, typename MsgFactoryType>
GenUmpIOClient<MsgType, MsgFactoryType>::~GenUmpIOClient()
{
   std::cout << GenTime::GetWallClockTime() << " ";
   std::cout << "GenUmpIOClient::~GenUmpIOClient from client " << mName << std::endl;

   if (mGenUmpIOData != &mGenUmpIOConnect)
   {
      delete mGenUmpIOData;
   }
   delete mGenIHelper;
   delete mGenOHelper;
}


template<typename MsgType, typename MsgFactoryType>
void GenUmpIOClient<MsgType, MsgFactoryType>::SetRudpStatOverrides(int aRetransmitTimeOutMicroSec,
                                                                   int aSendQueueLength,
                                                                   int aRecvQueueLength)
{
   mRudpRetransmitTimeOut = aRetransmitTimeOutMicroSec;
   mRudpSendQueueLength   = aSendQueueLength;
   mRudpRecvQueueLength   = aRecvQueueLength;

   if (mGenUmpIOData != 0)
   {
      mGenUmpIOData->SetRudpStatOverrides(aRetransmitTimeOutMicroSec, aSendQueueLength, aRecvQueueLength);
   }
}

// Must be called first.  Returns a positve number if successful
// Environment variable "aGroupEnv"       overrides argument "aGroup".
// Environment variable "aNameEnv"        overrides argument "aName".
// Environment variable "aUmpMsgTypeEnv"  overrides argument "aUmpMsgType" set in the constructor.
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::Init(const char* aIOGroup,
                                                  const char* aIOName,
                                                  const char* aGroupEnv,
                                                  const char* aNameEnv,
                                                  const char* aUmpMsgTypeEnv,
                                                  int         aUmpOptions)
{
   mUmpOptions = aUmpOptions;

   if (aUmpMsgTypeEnv && *aUmpMsgTypeEnv != '\0')
   {
      char*       envValue = getenv(aUmpMsgTypeEnv);
      std::string umpMsgType(envValue != 0 ? envValue : "");
      if (umpMsgType == "TCP")
      {
         mUmpMsgType     = TCP_IP;
         mConnectionType = "TCP";
      }
      else if (umpMsgType == "RUDP")
      {
         mUmpMsgType     = RELIABLE_UDP;
         mConnectionType = "RUDP";
      }
      else if (umpMsgType == "UDP")
      {
         mUmpMsgType     = UDP;
         mConnectionType = "UDP";
      }
   }

   if (aGroupEnv && *aGroupEnv != '\0')
   {
      char* envValue = getenv(aGroupEnv);
      if (envValue != 0)
      {
         mIOGroup = envValue;
      }
      else
      {
         mIOGroup = aIOGroup != 0 ? aIOGroup : "";
      }
   }
   else
   {
      mIOGroup = aIOGroup != 0 ? aIOGroup : "";
   }

   if (aNameEnv && *aNameEnv != '\0')
   {
      char* envValue = getenv(aNameEnv);
      if (envValue != 0)
      {
         mIOName = envValue;
      }
      else
      {
         mIOName = aIOName != 0 ? aIOName : "";
      }
   }
   else
   {
      mIOName = aIOName != 0 ? aIOName : "";
   }

   // Terminate old connection TCP connection
   mGenUmpIOConnect.Terminate();

   // Delete old GenUmpIOData if this function gets called more than once
   if (mGenUmpIOData != &mGenUmpIOConnect)
   {
      delete mGenUmpIOData;
   }

   if (mUmpMsgType == RELIABLE_UDP || mUmpMsgType == UDP)
   {
      mGenUmpIOData = new GenUmpIO;
   }
   else
   {
      mGenUmpIOData = &mGenUmpIOConnect;
   }

   // Get Helpers ready
   delete mGenIHelper;
   mGenIHelper = new GenIHelper<MsgType, MsgFactoryType>(*mGenUmpIOData, mMsgFactory, mGenThreadControl);
   delete mGenOHelper;
   mGenOHelper = new GenOHelper<MsgType>(*mGenUmpIOData, mGenThreadControl);

   // OK Let's go try to connect to the server
   mLookupOk      = -1;
   mDataStatus    = -1;
   mTcpConnectOk  = -1;
   mDataConnectOk = -1;
   return MakeConnection();
}

template<typename MsgType, typename MsgFactoryType>
MsgType* GenUmpIOClient<MsgType, MsgFactoryType>::GetMsg()
{
   if (mDataConnectOk > 0)
   {
      KeepTcpConnectionAlive();
      return mGenIHelper->GetMsg();
   }
   else
   {
      if (MakeConnection() > 0)
      {
         return mGenIHelper->GetMsg();
      }
      else
      {
         return 0;
      }
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::GetData(const void*& aData, unsigned long aNumBytes)
{
   if (mDataConnectOk > 0)
   {
      return mGenUmpIOData->GetGetBufferIfValid(aData, aNumBytes);
   }
   else
   {
      return mDataConnectOk;
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::PutMsg(const MsgType* aMsg)
{
   if (mDataConnectOk > 0)
   {
      return mGenOHelper->PutMsg(aMsg);
   }
   else
   {
      return mDataConnectOk;
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::PutData(const void* aData, unsigned long aNumBytes)
{
   if (mDataConnectOk > 0)
   {
      mGenUmpIOData->PutRaw(aData, aNumBytes);
      return static_cast<int>(aNumBytes);
   }
   else
   {
      return mDataConnectOk;
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::Send()
{
   int status = GetConnectState();
   if (status > 0)
   {
      KeepTcpConnectionAlive();
      return mGenOHelper->FlushMsg();
   }
   else if (status == 0)
   {
      status = MakeConnection();
      if (status > 0)
      {
         return mGenOHelper->FlushMsg();
      }
   }
   return status;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::Receive()
{
   int status = GetConnectState();
   if (status > 0)
   {
      KeepTcpConnectionAlive();
      return mGenUmpIOData->Receive(0);
   }
   else if (status == 0)
   {
      status = MakeConnection();
      if (status > 0)
      {
         return mGenUmpIOData->Receive(0);
      }
   }
   return status;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::GetConnectState() const
{
   if (mGenUmpIOData == 0)
   {
      return -1;
   }
   else if (mDataStatus <= -99990)
   {
      return mDataStatus;
   }
   else if (mDataConnectOk <= -99990)
   {
      return mDataConnectOk;
   }
   else if (mDataConnectOk == -1)
   {
      return 0;
   }
   else
   {
      return mGenUmpIOConnect.GetTcpConnectState();
   }
}

template<typename MsgType, typename MsgFactoryType>
typename GenUmpIOClient<MsgType, MsgFactoryType>::GenUmpIOClientType GenUmpIOClient<MsgType, MsgFactoryType>::GetUmpMsgType() const
{
   return mUmpMsgType;
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIO* GenUmpIOClient<MsgType, MsgFactoryType>::GetGenUmpIOData()
{
   if (mGenUmpIOData == 0 || mDataStatus <= -99990 || mDataConnectOk <= -99990 || mGenUmpIOConnect.GetTcpConnectState() < 0)
   {
      return 0;
   }
   else
   {
      return mGenUmpIOData;
   }
}


template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::MakeConnection()
{
#ifndef _WIN32
   // For the record 100 milliseconds = 100,000 microseconds = 100,000,000 nanoseconds
   // Note: 10 microseconds is about the smallest time increment nanosleep
   // can sleep.
   struct timespec my1stTimeSpec;
   struct timespec my2ndTimeSpec;
   my1stTimeSpec.tv_sec  = 0;
   my1stTimeSpec.tv_nsec = 50000000;
#endif

   int status = 0;
   // If user has not call Init or
   // if user has used a duplicant name or didn't get a good port from the sender, reject the request right a away
   if (mGenUmpIOData == 0)
   {
      return -1;
   }
   else if (mDataStatus <= -99990)
   {
      status = mDataStatus;
   }
   else if (mDataConnectOk <= -99990)
   {
      status = mDataConnectOk;
   }
   else
   {
      // Loop for about a second or until we get a good connection, whatever comes first
      unsigned int j;
      for (j = 20; j != 0; --j)
      {
         if (mTcpConnectOk < 0)
         {
            status = MakeTcpConnection();
         }
         else if (mDataConnectOk < 0)
         {
            // See if data has been received to confirm a good name
            status = ConfirmDataConnection();
         }
         else
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "GenUmpIOClient::MakeConnection had internal logic error in " << mName << std::endl;
            break;
         }

         if (status > 0)
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "GenUmpIOClient::MakeConnection took " << 21 - j << " passes to connect " << mName << std::endl;
            break;
         }
         else if (status <= -99990)
         {
            // Could not make initial connection, or don't have a unique name
            // No point in continuing trying.
            break;
         }

#ifdef _WIN32
         // Sleep for 50 msc
         Sleep(50);
#else
         int sleepStatus = nanosleep(&my1stTimeSpec, &my2ndTimeSpec);
         if (sleepStatus == -1)
         {
            // Just in case my first sleep gets interrupted.
            nanosleep(&my2ndTimeSpec, 0);
         }
#endif
      }
   }
   return status;
}


template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::MakeTcpConnection()
{
   // If Using Reliable UDP get receive on port and create buffers by calling GenUmpIO::Init(...)
   if (mDataStatus <= 0)
   {
      if (mUmpMsgType == RELIABLE_UDP || mUmpMsgType == UDP)
      {
         GenUmpIO::UmpMsgType msgType;
         std::string          name(mName);
         if (mUmpMsgType == UDP)
         {
            name.append(" UDP Client");
            msgType = GenUmpIO::UDP_BIDIR;
         }
         else
         {
            name.append(" RUDP Client");
            msgType = GenUmpIO::UDP_RELIABLE_BIDIR;
         }

         mDataStatus = mGenUmpIOData->Init(0,                     // Socket File
                                           "127.0.0.1",           // Bosus send to address for now
                                           11111,                 // Bogus send to port for now
                                           0,                     // Let ump get recv port
                                           msgType,               // Msg type
                                           name.c_str(),          // Calling class name
                                           mUmpOptions,           // Ump Options
                                           mLocalUpdatePeriodSec, // Local frame time
                                           0.05F);                // Guess remote frame time

         if (mDataStatus > 0)
         {
            int recvOnPort = mGenUmpIOData->GetLocalRecvPort();
            if (recvOnPort <= 0)
            {
               mDataStatus = -1;
               std::cout << GenTime::GetWallClockTime() << " ";
               std::cout << "Error number " << recvOnPort << " in GenUmpIOClient when obtaining the ";
               std::cout << mConnectionType << " UMP socket for " << mName << std::endl;
            }
         }
         else
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error number " << mDataStatus << " in GenUmpIOClient when opening the " << mConnectionType;
            std::cout << " UMP socket for " << mName << " with a send to address of 127.0.0.1" << std::endl;

            // No point in going any further
            mDataStatus = -99994;
            return mDataStatus;
         }
      }
      else
      {
         mDataStatus = 1;
      }
   }

   // Start the connection process
   if (mLookupOk <= 0)
   {
      std::string name(mName);
      name.append(" TCP Client");
      mLookupOk =
         mGenUmpIOConnect.Init(mIOGroup.c_str(), mIOName.c_str(), GenUmpIO::TCP_CLIENT, name.c_str(), mUmpOptions);


      // Build an initial message to store space on the buffer, so the user
      // doesn't override if we don't connect right away.
      // This logic will break if we let the user change the name at some later time.
      if (mGenUmpIOConnect.GetPutOffset() == 0)
      {
         GenIOMsgConnect msg(0.0,
                             0,
                             mUmpMsgType == UDP ? 1 : 0,
                             mName,
                             mLocalUpdatePeriodSec,
                             mRudpRetransmitTimeOut,
                             mRudpSendQueueLength,
                             mRudpRecvQueueLength);

         // Store message on the buffer as a place holder for now.
         msg.Put(mGenUmpIOConnect);
      }

      if (mLookupOk <= 0)
      {
         // Did not connect
         if (!(mUmpOptions & GenUmpIO::TCP_CONNECT_NO_WARNING_MSG) && GenError::ShowError(GenError::cCONNECT_ERRORS))
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error number " << mLookupOk << " in GenUmpIOClient when creating first TCP socket for ";
            std::cout << mName << ", with group \"" << mIOGroup << "\" and name \"" << mIOName << "\"" << std::endl;

            // Show missed connect message only once
            mUmpOptions |= GenUmpIO::TCP_CONNECT_NO_WARNING_MSG;
         }
         return -99999;
      }
   }

   if (mLookupOk > 0 && mDataStatus > 0)
   {
      // Ok now it is time to send our first message to the server
      int recvOnPort;
      if (mUmpMsgType == RELIABLE_UDP || mUmpMsgType == UDP)
      {
         recvOnPort = mGenUmpIOData->GetLocalRecvPort();
         if (recvOnPort < 0)
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error number " << recvOnPort << " in GenUmpIOClient when opening " << mConnectionType;
            std::cout << " UMP socket on 2nd attempt for " << mName << std::endl;
         }
      }
      else
      {
         recvOnPort = 0;
      }

      // Build and send a good and proper initial message to establish the connection
      long endOfData = mGenUmpIOConnect.GetPutOffset();
      mGenUmpIOConnect.SetPutOffset(0);

      GenIOMsgConnect msg(0.0,
                          recvOnPort,
                          mUmpMsgType == UDP ? 1 : 0,
                          mName,
                          mLocalUpdatePeriodSec,
                          mRudpRetransmitTimeOut,
                          mRudpSendQueueLength,
                          mRudpRecvQueueLength);

      msg.Put(mGenUmpIOConnect);

      if (endOfData > 0)
      {
         mGenUmpIOConnect.SetPutOffset(endOfData);
      }

      mTcpConnectOk = mGenUmpIOConnect.Send();

      // Just in case the send fails
      if (mTcpConnectOk <= 0)
      {
         // This would be rare, but just in case protect ourselves
         // in case the user puts data on the connect buffer.
         msg.Put(mGenUmpIOConnect);
         return -99999;
      }
      else
      {
         // Should have a TCP connection
         return 0;
      }
   }
   else
   {
      return -99999;
   }
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOClient<MsgType, MsgFactoryType>::ConfirmDataConnection()
{
   // Look for returning connect message from server
   int bytesReceived = mGenUmpIOConnect.Receive(0);
   if (bytesReceived > 0)
   {
      GenIOMsgConnect connectMsg(mGenUmpIOConnect);
      if (connectMsg.GetFamilyType() == GenMsg::TypeGenIO && connectMsg.GetType() == GenIOMsg::TypeConnect)
      {
         // Received a good connect message.
         // The connect msg holds the severs receive port which
         // is what we want this client to send to.
         int sendToPort = connectMsg.GetRecvOnPort();

         if (sendToPort > 0)
         {
            // Keep the receive on port
            int recvOnPort = mGenUmpIOData->GetLocalRecvPort();

            // Get the send to address
            std::string sendToAddress(mGenUmpIOConnect.GetSendToAddrString());

            // Reset the socket to the new send to port
            GenUmpIO::UmpMsgType msgType;
            std::string          name(mName);
            if (mUmpMsgType == UDP)
            {
               name.append(" UDP Client");
               msgType = GenUmpIO::UDP_BIDIR;
            }
            else
            {
               name.append(" RUDP Client");
               msgType = GenUmpIO::UDP_RELIABLE_BIDIR;
            }

            mDataConnectOk = mGenUmpIOData->Init(0,                          // Socket File
                                                 sendToAddress.c_str(),      // Send to Address
                                                 sendToPort,                 // Got Good send to port now
                                                 recvOnPort,                 // Use old recv port
                                                 msgType,                    // Msg type
                                                 name.c_str(),               // Calling class name
                                                 mUmpOptions,                // Ump Options
                                                 mLocalUpdatePeriodSec,      // Local frame time
                                                 connectMsg.GetFrameTime()); // Remote frame time

            if (mDataConnectOk < 0)
            {
               std::cout << GenTime::GetWallClockTime() << " ";
               std::cout << "Error number " << mDataConnectOk << " in GenUmpIOClient when opening " << mConnectionType;
               std::cout << " UMP socket with good "
                         << "send to port for " << mName << std::endl;
            }
         }
         else if (connectMsg.GetName() == "dup")
         {
            // Got a duplicate name
            mDataConnectOk = -99997;

            // Tell the server this connection is dead
            mGenUmpIOConnect.Terminate();
         }
         else if (connectMsg.GetName() == "empty")
         {
            // Got an empty name
            mDataConnectOk = -99996;

            // Tell the server this connection is dead
            mGenUmpIOConnect.Terminate();
         }
         else if (mUmpMsgType != TCP_IP)
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error number " << sendToPort << " in GenUmpIOClient when sending to port for ";
            std::cout << mName << ".  Port number received was " << sendToPort << std::endl;

            // Got a weird error
            mDataConnectOk = -99995;

            // Tell the server this connection is dead
            mGenUmpIOConnect.Terminate();
         }
         else
         {
            // Got a good TCP connection with a unique name
            mDataConnectOk = mTcpConnectOk;
         }
      }
   }
   return mDataConnectOk;
}


// This function gets called from stategic locations inside this class so that a
// rough activity count.  With this count we can send a status message every so
// often just to keep the OS from closing an "inactive" TCP connection
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOClient<MsgType, MsgFactoryType>::KeepTcpConnectionAlive()
{
   if (mUmpMsgType == RELIABLE_UDP || mUmpMsgType == UDP)
   {
      if (--mMsgCount == 0)
      {
         mMsgCount = 5000;

         // Build and send an initial message to establish the connection
         GenIOMsgStatus msg(0.0);

         msg.Put(mGenUmpIOConnect);
         mGenUmpIOConnect.Send();
      }
   }
}

#endif
