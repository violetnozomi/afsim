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

#ifndef GEN_UMP_IO_SERVER_CC_IS_DEFINED
#define GEN_UMP_IO_SERVER_CC_IS_DEFINED

#include "GenExport.hpp"

/// \file  GenUmpIOServerCC.hpp
/// \brief GenUmpIOServerCC
/// \class GenUmpIOServerCC GenUmpIOServerCC.hpp "GenUmpIOServerCC.hpp"
/// \brief Used by GenUmpIOServer to manage a unique connection to a GenUmpIOClient.
///
///
/// This class is used by GenUmpIOServer.  Each instantiation of this class represents a unique connection to a
/// client.  An object of this class is instantiated by GenUmpIOServer when a new client makes an initial connection.
/// Conversely, the user may instantiate this class for a broadcast or multicast connection and add the object
/// to GenUmpIOServer.\n
///
/// Users may inherit from this class to overide the virutal functions as needed.  If a user does
/// derive from this class then a user derived class of GenUmpIOServer must be written.  The user derived function
/// of GenUmpIOServer::CreateCC() needs to create the user derived version of GenUmpIOServerCC.\n
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

#include <iostream>
#include <string>
#include <vector>

#include "GenIHelper.hpp"
#include "GenIOMsgConnect.hpp"
#include "GenIOMsgStatus.hpp"
#include "GenOHelper.hpp"
#include "GenServerCCStats.hpp"
#include "GenTime.hpp"
#include "GenUmpIO.hpp"

template<typename MsgType, typename MsgFactoryType>
class GenUmpIOServer;

template<typename MsgType, typename MsgFactoryType = GenIFactory<MsgType>>
class GenUmpIOServerCC
{
public:
   /// This constructor is generally called by the user.  It does not have
   /// an option for a Factory pointer, so no received messages can be accepted.
   /// \param aServerName Server name used for error messages
   /// \param aClientName Client name used for error messages and can be used by GenUmpIOServer
   ///                    to find this GenUmpIOServerCC
   /// \param aUmpMsgType Can be any type supported by UMP.  See GenUmpIO for details.
   GenUmpIOServerCC(const std::string& aServerName, const std::string& aClientName, GenUmpIO::UmpMsgType aUmpMsgType);

   /// This constructor is typically called by GenUmpIOServer, but for special
   /// cases can be called by the user.
   /// \param aNewSocet   Socket number for new connection on server.
   /// \param aMsgFactory Pointer to a message factory.
   /// \param aServerName Server name used in error messages.
   GenUmpIOServerCC(int aNewSocket, MsgFactoryType* aMsgFactory, const std::string& aServerName);

   /// Destructor
   virtual ~GenUmpIOServerCC();

   /// This function is generally called by the user.  It does not have
   /// an option for a Factory pointer, so no recieved messages can be built.
   /// \param aIPaddress  The IP address
   /// \param aSendToPort Port number on the client side that we will send data to.
   /// \param aRecvOnPort Port number on the server side that we will listen to.
   /// \retval For TCP client and TCP active connections return values are:
   /// \retval    2 :  Connection was established in this call.
   /// \retval    1 :  Was previously connected (and stil is).
   /// \retval    0 :  Connection could not be established.
   /// \retval   -1 :  Connection was previously established, but is now closed.
   /// \retval For any other type of connection, a positive number is returned if successful.
   int Init(const std::string& aIPaddress,
            int                aSendToPort,
            int                aRecvOnPort,
            int aUmpOptions = GenUmpIO::QUEUED | GenUmpIO::UMP_NON_BLOCKING_TCP | GenUmpIO::UMP_ALLOW_MULTIPLE_RECEIVERS);

   /// This function is generally called by the user.  It does not have
   /// an option for a Factory pointer, so no recieved messages can be built.
   /// \param aGroup      UMP group name.
   /// \param aName       UMP message name.
   /// \retval For TCP client and TCP active connections return values are:
   /// \retval    2 :  Connection was established in this call.
   /// \retval    1 :  Was previously connected (and stil is).
   /// \retval    0 :  Connection could not be established.
   /// \retval   -1 :  Connection was previously established, but is now closed.
   /// \retval For any other type of connection, a positive number is returned if successful.
   int Init(const std::string& aGroup,
            const std::string& aName,
            int aUmpOptions = GenUmpIO::QUEUED | GenUmpIO::UMP_NON_BLOCKING_TCP | GenUmpIO::UMP_ALLOW_MULTIPLE_RECEIVERS);

   /// Put a message on the send buffer.
   /// \param aMsg Pointer to a message
   /// \return A positive number if successful or a negative number if there is an error.
   int PutMsg(const MsgType* aMsg);

   /// Put raw data to the client.
   /// \param aDataPtr Pointer to the data.
   /// \param aDataSize Number of bytes to send.
   /// \return A number of bytes sent if successful or a negative number if there is an error.
   long PutRawData(const void* aDataPtr, unsigned long aDataSize);

   /// Send any  messages that have been "Put" in the send buffer.
   /// \return The number of bytes sent or a negative number if there is an error.
   int Send();

   /// Set a client name to be used in debugging statements.
   /// \param aClientName : Name of client
   void SetClientName(const std::string& aClientName);

   /// Set expected update rate for RUDP
   /// \param aUpdatePeriodSec  Time between calls to Send in secs.
   void SetUpdatePeriodSec(float aUpdatePeriodSec);

   /// Get name of client.
   /// \return Name client reported in with
   const std::string& GetClientName() const;

   /// Get name validity
   /// \return name validiity
   int GetNameValid() const;

   /// Get connetion type.
   /// \return Connection type client reported in with.  It will be either TCP, UDP, or RUDP.
   const std::string& GetConnectionType() const;

   /// Change buffer currently being used.  <em>This function is not typically called and will destroy the original Put
   /// buffer.</em> Calling any form of Put after calling this function is not advised. \param aUserBuffer     Pointer
   /// to user buffer. \param aSizeInBytes    Total size of user buffer. \param aNumValidBytes  Number of valid bytes in
   /// user buffer.
   void ChangePutBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

   // These two functions are not currently used.
   // unsigned int         GetInSendErrorCond() const;
   // unsigned int         GetInRecvErrorCond() const;

protected:
   /// Get pointer to the GenUmpIO object that is handling the data.
   /// \return A GenUmpIO pointer.
   GenUmpIO* GetGenUmpIO();

   /// Let derived class to have it's own receive error messages.
   virtual void ProcessRecvError(int aErrorCode);

   /// Let derived class to have it's own receive error messages.
   virtual void ProcessSendError(int aErrorCode);

   /// Allow derived classes to change the client name.  Client name is normally
   /// send to this class from GenUmpIOClient.
   /// \param aClientName  Name of client as received from client
   virtual void ProcessSetClientName(const std::string& aClientName);

   /// Get name of server.
   /// \return Name of server
   const std::string& GetServerName() const;

   /// Get name of client/server combination typically used in error messages
   /// \return Name of client/server combination.
   const std::string& GetFullConnectionName() const;

private:
   void HandleSendError(int aStatus);
   void SendConnectMsg(int aNameValid);
   void SetRecvStats(float aProcessTime, unsigned int aNumBytes);
   void GetGenServerCCRecvStats(GenServerCCStats& aServerCCStats);


   /// Allow derived classes to do their own thing with their messages
   /// Returns a zero if the message should be deleted
   /// \param aMsg Pointer to the message.
   /// \return The message will be deleted if a zero is returned.
   virtual int ProcessInComingMsg(MsgType* aMsg);

   /// Allow derived classes to process rawdata.
   /// See code in this class for an example.
   /// \param aDataSize Data length.
   /// \return A positive non zero means success.
   virtual int ProcessInComingRawData(unsigned long aDataSize);

   /// Allows derived class to determine if a message should be sent.
   /// \param aMsg Pointer to a message.
   /// \return A non zero if message should be sent.
   virtual int ProcessOutGoingMsg(const MsgType* aMsg);

   /// Allows derived class to determine if raw data should be sent.
   /// \param aDataPtr  Pointer to the data.
   /// \param aDataSize Length of data.
   /// \return A non zero if the raw data should be sent.
   virtual int ProcessOutGoingRawData(const void* aDataPtr, unsigned long aDataSize);

   /// Allow a derived class to do something with a new fully defined client connection
   virtual void ProcessNewConnection();

   // The following methods are called by the GenUmpIOServer class
   void                 HandleReceivedMsgs(int aRecvSocket, int& aNewDataSocket, int& aNewConnection);
   void                 HandleRecvError(int aErrorCode);
   int                  GetRecvDataSocket() const;
   int                  GetBytesReceived(int aSocketNumber) const;
   GenUmpIO::UmpMsgType GetUmpMsgType() const;


   GenUmpIO*                            mGenUmpIOData;
   GenIHelper<MsgType, MsgFactoryType>* mGenIHelper;
   GenOHelper<MsgType>*                 mGenOHelper;
   GenUmpIO*                            mGenUmpIOConnect;
   MsgFactoryType*                      mMsgFactory;
   unsigned int                         mInSendErrorCond;
   unsigned int                         mInRecvErrorCond;
   unsigned int                         mInInitProcess;
   unsigned int                         mNumBytesRecv;
   unsigned int                         mNumRecvs;
   int                                  mDataConnectOk;
   int                                  mRecvUdpDataPort;
   int                                  mNameValid;
   GenUmpIO::UmpMsgType                 mUmpMsgType;
   float                                mUpdatePeriodSec;
   float                                mRecvProcessTime;
   std::string                          mClientName;
   std::string                          mServerName;
   std::string                          mFullConnectionName;
   std::string                          mConnectionType;

   friend class GenUmpIOServer<MsgType, MsgFactoryType>;
};

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>::GenUmpIOServerCC(const std::string&   aServerName,
                                                            const std::string&   aClientName,
                                                            GenUmpIO::UmpMsgType aUmpMsgType)
   : mGenIHelper(0)
   , mGenUmpIOConnect(0)
   , mMsgFactory(0)
   , mInSendErrorCond(0)
   , mInRecvErrorCond(0)
   , mInInitProcess(1)
   , mNumBytesRecv(0)
   , mNumRecvs(0)
   , mDataConnectOk(-1)
   , mRecvUdpDataPort(0)
   , mNameValid(0)
   , mUmpMsgType(aUmpMsgType)
   , mUpdatePeriodSec(0.05F)
   , mRecvProcessTime(0.0F)
   , mServerName(aServerName)
{
   SetClientName(aClientName);

   // Create a GenUmpIO connection to the client
   mGenUmpIOData = new GenUmpIO();

   mGenUmpIOConnect = mGenUmpIOData;
   mGenOHelper      = new GenOHelper<MsgType>(*mGenUmpIOData);
}

template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>::GenUmpIOServerCC(int                aNewSocket,
                                                            MsgFactoryType*    aMsgFactory,
                                                            const std::string& aServerName)
   : mGenUmpIOData(0)
   , mGenIHelper(0)
   , mGenOHelper(0)
   , mMsgFactory(aMsgFactory)
   , mInSendErrorCond(0)
   , mInRecvErrorCond(0)
   , mInInitProcess(1)
   , mNumBytesRecv(0)
   , mNumRecvs(0)
   , mDataConnectOk(-1)
   , mRecvUdpDataPort(0)
   , mNameValid(0)
   , mUmpMsgType(GenUmpIO::INTERNAL_USE_ONLY)
   , // Variable is never used when constructed by server.
   mUpdatePeriodSec(0.05F)
   , mRecvProcessTime(0.0F)
   , mServerName(aServerName)
{
   SetClientName(std::string("unknown"));

   // Create a GenUmpIO connection to the client
   mGenUmpIOConnect = new GenUmpIO(aNewSocket, mFullConnectionName.c_str());
}


template<typename MsgType, typename MsgFactoryType>
GenUmpIOServerCC<MsgType, MsgFactoryType>::~GenUmpIOServerCC()
{
   std::cout << GenTime::GetWallClockTime() << " ";
   std::cout << mFullConnectionName << " is lost" << std::endl;

   if (mGenUmpIOData != mGenUmpIOConnect)
   {
      delete mGenUmpIOData;
      delete mGenUmpIOConnect;
   }
   else
   {
      delete mGenUmpIOData;
   }

   delete mGenIHelper;
   delete mGenOHelper;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::Init(const std::string& aIPaddress, int aSendToPort, int aRecvOnPort, int aUmpOptions)
{
   return mGenUmpIOData->Init(0,                           // socket table
                              aIPaddress.c_str(),          // agentName
                              aSendToPort,                 // send to port
                              aRecvOnPort,                 // recv on port
                              mUmpMsgType,                 // msg type
                              mFullConnectionName.c_str(), // calling class name
                              aUmpOptions);                // optional Ump paramenters
}


template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::Init(const std::string& aGroup, const std::string& aName, int aUmpOptions)
{
   return mGenUmpIOData->Init(aGroup.c_str(),              // UMP Group name
                              aName.c_str(),               // UMP Message name
                              mUmpMsgType,                 // msg type
                              mFullConnectionName.c_str(), // calling class name
                              aUmpOptions);                // optional Ump paramenters
}


// This function is private since it is only called by GenUmpIOServer
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::HandleReceivedMsgs(int aRecvSocket, int& aNewDataSocket, int& aNewConnection)
{
   // Clear any errors
   HandleRecvError(1);

   if (mMsgFactory != 0)
   {
      int connectMsgRecv = 0;
      if (mInInitProcess)
      {
         GenIOMsgConnect connectMsg(*mGenUmpIOConnect);
         if (connectMsg.GetFamilyType() == GenMsg::TypeGenIO && connectMsg.GetType() == GenIOMsg::TypeConnect)
         {
            // Received a good connect message.
            connectMsgRecv = 1;

            SetClientName(connectMsg.GetName());

            // Look for UDP or Reliable UDP connection
            int sendToPort = connectMsg.GetRecvOnPort();
            if (sendToPort > 0)
            {
               // OK we are going UDP or reliable UDP
               // Get the send to address
               std::string sendToAddress(mGenUmpIOConnect->GetSendToAddrString());

               if (mGenUmpIOData == 0)
               {
                  mGenUmpIOData = new GenUmpIO;
               }

               // Reset the socket to the new send to port
               if (mDataConnectOk < 0)
               {
                  GenUmpIO::UmpMsgType msgType;
                  if (connectMsg.IsUdpRequested())
                  {
                     msgType         = GenUmpIO::UDP_BIDIR;
                     mConnectionType = "UDP";
                  }
                  else
                  {
                     msgType         = GenUmpIO::UDP_RELIABLE_BIDIR;
                     mConnectionType = "RUDP";
                  }

                  // Adjust queue lengths to match client.  Most of the time these
                  // overides will be set to zero.  The recv and send values are reversed here
                  // to match the queue lengths of the client.
                  mGenUmpIOData->SetRudpStatOverrides(0,
                                                      connectMsg.GetRudpRecvQueueLength(), // Send queue length override
                                                      connectMsg.GetRudpSendQueueLength()); // Recv queue length override


                  mDataConnectOk = mGenUmpIOData->Init(0,                           // Socket File
                                                       sendToAddress.c_str(),       // Send to Address
                                                       sendToPort,                  // Send to port
                                                       0,                           // System will find recv port
                                                       msgType,                     // Msg type
                                                       mFullConnectionName.c_str(), // Calling class name
                                                       GenUmpIO::QUEUED,            // UMP option for UDP
                                                       mUpdatePeriodSec,            // Local update time
                                                       connectMsg.GetFrameTime());  // Remote update time
               }

               if (mDataConnectOk > 0)
               {
                  mRecvUdpDataPort = mGenUmpIOData->GetLocalRecvPort();
                  if (mRecvUdpDataPort > 0)
                  {
                     // Tell server we have a new data socket
                     aNewDataSocket = mGenUmpIOData->GetSocketRecv();

                     // Things look good for RUDP
                     std::cout << GenTime::GetWallClockTime() << " ";
                     std::cout << mFullConnectionName << " received a " << mConnectionType
                               << " connect msg with update time of ";
                     std::cout << connectMsg.GetFrameTime() << std::endl;

                     // Clear init flag
                     mInInitProcess = 0;
                  }
                  else
                  {
                     std::cout << GenTime::GetWallClockTime() << " ";
                     std::cout << "Error obtaining the " << mConnectionType << " UMP socket in ";
                     std::cout << mFullConnectionName << " client connection for server " << mServerName << std::endl;
                  }
               }
               else
               {
                  std::cout << GenTime::GetWallClockTime() << " ";
                  std::cout << "Error opening the " << mConnectionType << " UMP socket in ";
                  std::cout << mFullConnectionName << " with a send to address of " << sendToAddress << std::endl;
               }
            }
            else if (mGenUmpIOData == 0)
            {
               // OK we are going TCP
               mConnectionType = "TCP";
               std::cout << GenTime::GetWallClockTime() << " ";
               std::cout << mFullConnectionName << " received a TCP connect msg." << std::endl;

               // Set the full name
               mGenUmpIOConnect->SetCallingClassName(mFullConnectionName);

               // Set the data pipe the same as the connection pipe
               mGenUmpIOData = mGenUmpIOConnect;

               // Clear init flag
               mInInitProcess = 0;
            }

            // Define Helper clases
            if (mInInitProcess == 0)
            {
               mGenIHelper = new GenIHelper<MsgType, MsgFactoryType>(*mGenUmpIOData, mMsgFactory);
               mGenOHelper = new GenOHelper<MsgType>(*mGenUmpIOData);

               // Let the derived class have a chance at the new connection
               ProcessNewConnection();

               // Set a flag so the server class knows we are good
               aNewConnection = 1;
            }
         }
         else
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error " << mFullConnectionName << " did not receive a valid connect message" << std::endl;
         }
      }

      if (mGenUmpIOData != mGenUmpIOConnect && mGenUmpIOConnect->GetSocketRecv() == aRecvSocket && connectMsgRecv == 0)
      {
         // Not much to do here, just got a status message
         // Clear Buffer by generating a Status message
         if (mGenUmpIOConnect->GetGetOffset(GenBuf::FromDataEnd) > 24)
         {
            std::cout << GenTime::GetWallClockTime() << " ";
            std::cout << "Error in GenUmpIOServerCC::HandleReceivedMsgs "
                      << mGenUmpIOConnect->GetGetOffset(GenBuf::FromDataEnd);
            std::cout << " extra bytes received in " << mFullConnectionName;
            std::cout << " when only expecting a status message in " << mConnectionType << "  mode";
            unsigned int addr1 = mGenUmpIOConnect->GetRecvFromAddr() >> 24;
            unsigned int addr2 = mGenUmpIOConnect->GetRecvFromAddr() >> 16 & 0xFF;
            unsigned int addr3 = mGenUmpIOConnect->GetRecvFromAddr() >> 8 & 0xFF;
            unsigned int addr4 = mGenUmpIOConnect->GetRecvFromAddr() & 0xFF;
            std::cout << "  Sent from " << addr1 << "." << addr2 << "." << addr3 << "." << addr4 << " with from port of ";
            std::cout << mGenUmpIOConnect->GetRecvFromPort() << std::endl;

            unsigned int counter = 0;
            unsigned int trip    = 0;
            while (mGenUmpIOConnect->GetGetOffset(GenBuf::FromDataEnd) > 0)
            {
               if (trip == 0)
               {
                  std::cout << std::endl;
                  std::cout << std::dec << "(" << counter << ")";
                  trip = 10;
                  counter += 10;
                  // Just in case we get a big one.
                  if (counter > 1000)
                  {
                     break;
                  }
               }
               --trip;
               unsigned char tempy = 0;
               *mGenUmpIOConnect >> tempy;
               if (mGenUmpIOConnect->GetInputStatus() == GenBuf::NoError)
               {
                  std::cout << "  " << std::hex << static_cast<unsigned int>(tempy);
               }
               else
               {
                  std::cerr << "[" << __FILE__ << ": " << __LINE__ << "]: GenUmpIOServerCC<MsgType, MsgFactoryType>"
                            << "::HandleReceivedMsgs(...)\n"
                            << "\tBad Read of `unsigned char` from "
                            << "mGenUmpIOConnect !" << std::endl;
               }
            }
            std::cout << std::dec << std::endl;
         }
         else
         {
            // Pull the normal message off of the buffer, so above error logic will work if we
            // get a goofy data on this port
            GenIOMsgStatus msg(*mGenUmpIOConnect);
         }
      }
      else
      {
         // Check to be sure data has really come in.  It is posible with a later
         // server start up that data came in with the connect message.
         if (mGenUmpIOData->GetGetOffset(GenBuf::FromDataEnd) != 0)
         {
            // Dealing with User type messages
            MsgType* ptr = mGenIHelper->GetMsg();
            while (ptr)
            {
               // Let derived class handle message
               int takeOwnerShip = ProcessInComingMsg(ptr);

               if (!takeOwnerShip)
               {
                  delete ptr;
               }
               ptr = mGenIHelper->GetMsg();
            }
         }
      }
   }
   else
   {
      do
      {
         // Get the data size
         unsigned long dataSize = mGenUmpIOData->GetGetOffset(GenBuf::FromDataEnd);

         // Do something with the data blob, user will use GenUmpIO::GetRaw
         // to physically get the data.
         ProcessInComingRawData(dataSize);
      } while (mGenUmpIOData->Receive(0) > 0);
   }
}


template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::PutMsg(const MsgType* aMsg)
{
   int status = 0;
   // Test to see if this is a message we want to send
   if (mGenOHelper != 0 && ProcessOutGoingMsg(aMsg))
   {
      status = mGenOHelper->PutMsg(aMsg);
      HandleSendError(status);
   }
   return status;
}

template<typename MsgType, typename MsgFactoryType>
long GenUmpIOServerCC<MsgType, MsgFactoryType>::PutRawData(const void* aDataPtr, unsigned long aDataSize)
{
   int  sendStatus = 0;
   long putStatus  = 0;
   // Test to see if this is message we want to send
   if (mGenOHelper != 0 && ProcessOutGoingRawData(aDataPtr, aDataSize))
   {
      if (static_cast<long>(aDataSize) > mGenUmpIOData->GetPutOffset(GenBuf::FromPhyEnd))
      {
         sendStatus = Send();
      }
      if (sendStatus >= 0)
      {
         putStatus = mGenUmpIOData->PutRaw(aDataPtr, aDataSize);
      }
      HandleSendError(sendStatus);
   }
   return sendStatus < 0 ? sendStatus : putStatus;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::Send()
{
   if (mGenOHelper != 0)
   {
      int status = mGenOHelper->FlushMsg();
      HandleSendError(status);
      return status;
   }
   else
   {
      return 0;
   }
}

// This function is private since it is only called by GenUmpIOServer
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::HandleRecvError(int aErrorCode)
{
   if (aErrorCode < 0 && !mInRecvErrorCond)
   {
      mInRecvErrorCond = 1;
      ProcessRecvError(aErrorCode);
   }
   else if (aErrorCode > 0 && mInRecvErrorCond)
   {
      mInRecvErrorCond = 0;
      ProcessRecvError(aErrorCode);
   }
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::SetClientName(const std::string& aClientName)
{
   mClientName         = aClientName;
   mFullConnectionName = "Client Connection for ";
   mFullConnectionName.append(aClientName);
   mFullConnectionName.append(" on Server ");
   mFullConnectionName.append(mServerName);
   ProcessSetClientName(aClientName);
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::SetUpdatePeriodSec(float aUpdatePeriodSec)
{
   mUpdatePeriodSec = aUpdatePeriodSec;
}

template<typename MsgType, typename MsgFactoryType>
inline GenUmpIO* GenUmpIOServerCC<MsgType, MsgFactoryType>::GetGenUmpIO()
{
   return mGenUmpIOData;
}

template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::GetBytesReceived(int aSocketNumber) const
{
   if (mGenUmpIOData != mGenUmpIOConnect && mGenUmpIOConnect->GetSocketRecv() != aSocketNumber)
   {
      // In Reliable UDP mode
      int numBytes = mGenUmpIOData->Receive(0);
      if (numBytes < -1)
      {
         std::cout << GenTime::GetWallClockTime() << " ";
         std::cout << "Error number " << numBytes << " in " << mFullConnectionName;
         std::cout << " when receiving " << mConnectionType << std::endl;
         return -1;
      }
      else
      {
         return numBytes;
      }
   }
   else
   {
      // In TCP mode or got a status message in on the TCP side or client was
      // instantiated from outside.
      return mGenUmpIOConnect->Receive(0);
   }
}

// template<typename MsgType, typename MsgFactoryType>
// inline unsigned int GenUmpIOServerCC<MsgType, MsgFactoryType>::GetInSendErrorCond() const
//{
//    return  mInSendErrorCond;
// }

// template<typename MsgType, typename MsgFactoryType>
// inline unsigned int GenUmpIOServerCC<MsgType, MsgFactoryType>::GetInRecvErrorCond() const
//{
//    return mInRecvErrorCond;
// }

template<typename MsgType, typename MsgFactoryType>
inline GenUmpIO::UmpMsgType GenUmpIOServerCC<MsgType, MsgFactoryType>::GetUmpMsgType() const
{
   return mUmpMsgType;
}

template<typename MsgType, typename MsgFactoryType>
inline int GenUmpIOServerCC<MsgType, MsgFactoryType>::GetNameValid() const
{
   return mNameValid;
}

template<typename MsgType, typename MsgFactoryType>
const std::string& GenUmpIOServerCC<MsgType, MsgFactoryType>::GetClientName() const
{
   return mClientName;
}

template<typename MsgType, typename MsgFactoryType>
inline const std::string& GenUmpIOServerCC<MsgType, MsgFactoryType>::GetConnectionType() const
{
   return mConnectionType;
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::ChangePutBuffer(unsigned char* aUserBuffer,
                                                                unsigned long  aSizeInBytes,
                                                                unsigned long  aNumValidBytes)
{
   if (mGenUmpIOData != 0)
   {
      mGenUmpIOData->ChangePutBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
   }
}


//
// Protected
//

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessRecvError(int aErrorCode)
{
   std::cout << GenTime::GetWallClockTime() << " ";
   std::cout << mFullConnectionName;
   if (aErrorCode < 0)
   {
      std::cout << " is in recv error condition " << aErrorCode << std::endl;
   }
   else
   {
      std::cout << " is no longer in recv error condition" << std::endl;
   }
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessSendError(int aErrorCode)
{
   std::cout << GenTime::GetWallClockTime() << " ";
   std::cout << mFullConnectionName;
   if (aErrorCode < 0)
   {
      std::cout << " is in send error condition " << aErrorCode << std::endl;
   }
   else
   {
      std::cout << " is no longer in send error condition" << std::endl;
   }
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessSetClientName(const std::string& /* aClientName */)
{
}


template<typename MsgType, typename MsgFactoryType>
const std::string& GenUmpIOServerCC<MsgType, MsgFactoryType>::GetServerName() const
{
   return mServerName;
}

template<typename MsgType, typename MsgFactoryType>
const std::string& GenUmpIOServerCC<MsgType, MsgFactoryType>::GetFullConnectionName() const
{
   return mFullConnectionName;
}


//
// Private
//

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::HandleSendError(int aErrorCode)
{
   if (aErrorCode < 0 && !mInSendErrorCond)
   {
      mInSendErrorCond = 1;
      ProcessSendError(aErrorCode);
   }
   else if (aErrorCode > 0 && mInSendErrorCond)
   {
      mInSendErrorCond = 0;
      ProcessSendError(aErrorCode);
   }
}

// This function is private since it is only called by GenUmpIOServer
template<typename MsgType, typename MsgFactoryType>
inline int GenUmpIOServerCC<MsgType, MsgFactoryType>::GetRecvDataSocket() const
{
   return mGenUmpIOData != 0 ? mGenUmpIOData->GetSocketRecv() : 0;
}

// This function is private since it is only called by GenUmpIOServer
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::SendConnectMsg(int aNameValid)
{
   mNameValid = aNameValid;
   std::string name;
   int         port = 0;
   if (aNameValid > 0)
   {
      name = mFullConnectionName;
      port = mRecvUdpDataPort;
   }
   else if (aNameValid == -1)
   {
      name = "dup";
   }
   else if (aNameValid == -2)
   {
      name = "empty";
   }
   else
   {
   }

   // Build message to tell client about recv port and update time
   GenIOMsgConnect msg(0.0,                              // time
                       port,                             // Receiving port
                       mConnectionType == "UDP" ? 1 : 0, // Straight UDP flag
                       name,                             // Name
                       mUpdatePeriodSec,                 // Time between updates
                       0,                                // Retransmit time out
                       0,                                // Send Queue Length
                       0);                               // Recv Queue Length

   msg.Put(*mGenUmpIOConnect);
   mGenUmpIOConnect->Send();
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::SetRecvStats(float aProcessTime, unsigned int aNumBytes)
{
   mRecvProcessTime += aProcessTime;
   mNumBytesRecv += aNumBytes;
   ++mNumRecvs;
}

template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::GetGenServerCCRecvStats(GenServerCCStats& aServerCCStats)
{
   aServerCCStats.SetClientName(mClientName);
   aServerCCStats.SetProcessTime(mRecvProcessTime);
   aServerCCStats.SetNumRecvsSends(mNumRecvs);
   aServerCCStats.SetNumBytes(mNumBytesRecv);

   // Reset the values back to zero for the next time period
   mRecvProcessTime = 0.0F;
   mNumBytesRecv    = 0;
   mNumRecvs        = 0;
}


// Virtual
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessInComingMsg(MsgType* aMsg)
{
   // This method is called by HandleRecvMsgs and if a zero is returned
   // "aGenMsg" will be deleted.

   // By defalult, a zero is returned by DoIt so this message will be deleted.
   return aMsg->DoIt();
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessInComingRawData(unsigned long aDataSize)
{
   // In most cases, the derived class will have full implementation of
   // how the the incomming data should be processed.

   // The following lines show an example of how the user will get the
   // incoming buffer and use them.

   // Allocate memories for the user data
   unsigned char* userDataPtr = new unsigned char[aDataSize];

   // User must use the GenUmpIO::GetRaw method to get the data,
   // otherwise internal indexes in GenUmpIO get messed up.
   GetGenUmpIO()->GetRaw(userDataPtr, aDataSize);

   // Do something with the user data that were just retreived
   // ...

   // Done manipulating the data.  Delete its memories
   delete userDataPtr;

   // A one means success
   return 1;
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessOutGoingMsg(const MsgType* /* aMsg */)
{
   // By default, Always send message
   return 1;
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
int GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessOutGoingRawData(const void* /*aDataPtr*/, unsigned long /*aDataSize*/)
{
   // By default, Always send message.  The derived class will have full
   // implementation of this method.
   return 1;
}

// Virtual
template<typename MsgType, typename MsgFactoryType>
void GenUmpIOServerCC<MsgType, MsgFactoryType>::ProcessNewConnection()
{
   // Totally optional stuff here
}

#endif
