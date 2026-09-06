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

#include "GenUmpIO.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "GenBuf.hpp"
#include "GenBufIUser.hpp"
#include "GenBufOManaged.hpp"
#include "GenError.hpp"
#include "GenTime.hpp"
#include "GenUmpIORudpStats.hpp"
#include "ump.h"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_IT(A) A
#else
#define DEBUG_IT(A) /* A */
#endif

//
// This class inherits from GenIO
//

unsigned int GenUmpIO::sRudpFailedSends   = 4;
unsigned int GenUmpIO::sRudpRetrans       = 4;
unsigned int GenUmpIO::sRudpUnnecRetrans  = 4;
unsigned int GenUmpIO::sRudpRejectedRecvs = 4;


GenUmpIO::GenUmpIO()
   : GenIO("GenUmpIO", "GenUmpIO")
   , mSocketRecv(-99999)
   , mSocketSend(-99999)
   , mSendToPort(0)
   , mSendFromPort(0)
   , mSendError(0)
   , mRecvError(0)
   , mRudpRetransmitTimeOut(0)
   , mRudpSendQueueLength(0)
   , mRudpRecvQueueLength(0)
   , mRecvBuf(0)
   , mSendBuf(0)
   , mRudpStats(0)
   , mRudpCheckCount(100)
{
   mSocketRecvInfoPtr = new UMPSockInfoType;
}

GenUmpIO::GenUmpIO(int aClientSocket, const char* aCallingClassName)
   : GenIO("GenUmpIO", "GenUmpIO")
   , mSocketRecv(aClientSocket)
   , mSocketSend(aClientSocket)
   , mSendToPort(0)
   , mSendFromPort(0)
   , mSendError(0)
   , mRecvError(0)
   , mRudpRetransmitTimeOut(0)
   , mRudpSendQueueLength(0)
   , mRudpRecvQueueLength(0)
   , mRecvBuf(0)
   , mSendBuf(0)
   , mRudpStats(0)
   , mRudpCheckCount(100)
   , mCallingClassName(aCallingClassName != 0 ? aCallingClassName : "")
{
   mSocketRecvInfoPtr = new UMPSockInfoType;

   // Hard code this to large buffer for now
   CreateBuffers(0);

   SetUmpDefaultOptions(aClientSocket, TCP_EXISTING);
   umpGetSocketInfoC(mSocketRecv, mSocketRecvInfoPtr, UMP_AUTO_UPDATE);
}

// Named addressing constructor
GenUmpIO::GenUmpIO(const char* aGroup,
                   const char* aName,
                   UmpMsgType  aMsgType,
                   const char* aCallingClassName,
                   int         aUmpOptions,
                   float       aLocalUpdatePeriodSec,
                   float       aRemoteUpdatePeriodSec)
   : GenIO("GenUmpIO", "GenUmpIO")
   , mSocketRecv(-99999)
   , mSocketSend(-99999)
   , mSendToPort(0)
   , mSendFromPort(0)
   , mSendError(0)
   , mRecvError(0)
   , mRudpRetransmitTimeOut(0)
   , mRudpSendQueueLength(0)
   , mRudpRecvQueueLength(0)
   , mRecvBuf(0)
   , mSendBuf(0)
   , mRudpStats(0)
   , mRudpCheckCount(100)
{
   mSocketRecvInfoPtr = new UMPSockInfoType;

   // Open the sockets
   Init(aGroup, aName, aMsgType, aCallingClassName, aUmpOptions, aLocalUpdatePeriodSec, aRemoteUpdatePeriodSec);
}

GenUmpIO::GenUmpIO(const char* const aSocketFile[],
                   const char*       aSendRecvAgentName,
                   unsigned int      aSendToPort,
                   unsigned int      aRecvOnPort,
                   UmpMsgType        aMsgType,
                   const char*       aCallingClassName,
                   int               aUmpOptions,
                   float             aLocalUpdatePeriodSec,
                   float             aRemoteUpdatePeriodSec)
   : GenIO("GenUmpIO", "GenUmpIO")
   , mSocketRecv(-99999)
   , mSocketSend(-99999)
   , mSendToPort(0)
   , mSendFromPort(0)
   , mSendError(0)
   , mRecvError(0)
   , mRecvBuf(0)
   , mSendBuf(0)
   , mRudpStats(0)
   , mRudpCheckCount(100)
{
   mSocketRecvInfoPtr = new UMPSockInfoType;

   // Open the sockets
   Init(aSocketFile,
        aSendRecvAgentName,
        aSendToPort,
        aRecvOnPort,
        aMsgType,
        aCallingClassName,
        aUmpOptions,
        aLocalUpdatePeriodSec,
        aRemoteUpdatePeriodSec);
}

GenUmpIO::~GenUmpIO()
{
   // Release resources allocated by the UMP.
   Terminate();

   delete mSocketRecvInfoPtr;
}

void GenUmpIO::SetRudpStatOverrides(int aRetransmitTimeOutMicroSec, int aSendQueueLength, int aRecvQueueLength)
{
   mRudpRetransmitTimeOut = aRetransmitTimeOutMicroSec;
   mRudpSendQueueLength   = aSendQueueLength;
   mRudpRecvQueueLength   = aRecvQueueLength;
}

// Name addressing Init method
int GenUmpIO::Init(const char* aGroup,
                   const char* aName,
                   UmpMsgType  aMsgType,
                   const char* aCallingClassName,
                   int         aUmpOptions,
                   float       aLocalUpdatePeriodSec,
                   float       aRemoteUpdatePeriodSec)
{
   if (aCallingClassName != 0)
   {
      mCallingClassName = aCallingClassName;
   }

   // Set to max send size
   CreateBuffers(0);

   int connectStatus;
   if (mSocketRecv > 0 && (aMsgType == TCP_CLIENT || aMsgType == TCP_ACTIVE))
   {
      connectStatus = umpRetryConnect(mSocketRecv, 0); //  "0" means no options set

      if (connectStatus > 0)
      {
         mSocketSend = mSocketRecv;
         SetUmpDefaultOptions(mSocketRecv, aMsgType);
      }
   }
   else
   {
      // Close old sockets if they exist
      DeleteSockets();

      // Multiple receiver flag has the potential to induce an error in UMP 7.07.
      // UMP turns on the multiple receiver flag for multicast by default, so
      // will turn it off here for all requests.
      aUmpOptions = aUmpOptions & ~UMP_ALLOW_MULTIPLE_RECEIVERS;

      // Look to see if two sockets should be open which is primarily
      // needed for multicast on a bi-directional socket.  Set "twoSocket" flag accordingly.
      if (aMsgType == UDP_BIDIR && aUmpOptions & UMP_NAMED_MULTICAST)
      {
         mSocketRecv = umpCreateNamedSocketC(aName, aGroup, UDP_RCV_ONLY, NO_CONVERT, aUmpOptions);

         mSocketSend = umpCreateNamedSocketC(aName, aGroup, UDP_SEND_ONLY, NO_CONVERT, aUmpOptions);

         connectStatus = mSocketRecv;
         if (connectStatus > mSocketSend)
         {
            connectStatus = mSocketSend;
         }

         if (connectStatus > 0)
         {
            // Get socket info to find source port so I can ignore
            // myself when sending and receiving multicast
            UMPSockInfoType socketSendInfo;
            umpGetSocketInfoC(mSocketSend, &socketSendInfo, 0);
            mSendFromPort = socketSendInfo.localPort;

            SetUmpDefaultOptions(mSocketRecv, UDP_RCV_ONLY);
            SetUmpDefaultOptions(mSocketSend, UDP_SEND_ONLY);
         }
      }
      else
      {
         mSocketRecv =
            umpCreateNamedSocketC(aName, aGroup, aMsgType, NO_CONVERT, aUmpOptions & ~TCP_CONNECT_NO_WARNING_MSG);

         if (aMsgType == TCP_CLIENT || aMsgType == TCP_ACTIVE)
         {
            connectStatus = umpGetConnectedState(mSocketRecv, 1); // "1" means do actively querry connection.
         }
         else
         {
            connectStatus = mSocketRecv;
         }

         if (connectStatus > 0)
         {
            mSocketSend = mSocketRecv;
            SetUmpDefaultOptions(mSocketRecv, aMsgType);
         }
      }
   }

   if (connectStatus <= 0)
   {
      if (!(aUmpOptions & TCP_CONNECT_NO_WARNING_MSG) && GenError::ShowError(GenError::cCONNECT_ERRORS))
      {
         cerr << GenTime::GetWallClockTime() << " ";
         cerr << "WARNING -- ";
         cerr << "Could not open name socket in GenUmpIO::Init() when called from " << mCallingClassName << " for name "
              << aName << " and group " << aGroup << ".  Received UMP error code/status: " << connectStatus << endl;
      }
      return connectStatus;
   }
   else
   {
      umpGetSocketInfoC(mSocketRecv, mSocketRecvInfoPtr, UMP_AUTO_UPDATE);

      if (aMsgType == UDP_RELIABLE_BIDIR)
      {
         return SetRudpStats(mSocketSend, aLocalUpdatePeriodSec, aRemoteUpdatePeriodSec);
      }
      else
      {
         return 1;
      }
   }
}


int GenUmpIO::Init(const char* const aSocketFile[],
                   const char*       aSendRecvAgentName,
                   unsigned int      aSendToPort,
                   unsigned int      aRecvOnPort,
                   UmpMsgType        aMsgType,
                   const char*       aCallingClassName,
                   int               aUmpOptions,
                   float             aLocalUpdatePeriodSec,
                   float             aRemoteUpdatePeriodSec)
{
   unsigned int index    = 0;
   const char*  fileName = 0;

   CreateBuffers(aSendRecvAgentName);

   if (aCallingClassName != 0)
   {
      mCallingClassName = aCallingClassName;
   }

   if (aSocketFile)
   {
      // Look through array until a valid socket file name is found
      while (aSocketFile[index] && *aSocketFile[index] != '\0')
      {
         ifstream socketFile(aSocketFile[index]);
         if (socketFile.good())
         {
            socketFile.close();
            fileName = aSocketFile[index];

            // Initialize UMP with socket file
            int status = umpInitC(fileName);
            if (status < 0)
            {
               cerr << GenTime::GetWallClockTime() << " ";
               cerr << "WARNING -- ";
               cerr << mCallingClassName;
               cerr << " called GenUmpIO::Init which had an error number " << status;
               if (fileName)
               {
                  cerr << "when trying to read file ";
                  cerr << fileName << endl;
               }
               else
               {
                  cerr << endl;
               }
               return status;
            }
            break;
         }
         else
         {
            ++index;
         }
      }

      // Write out error message if a socket file was given, but not found
      if (index && !fileName && GenError::ShowError(GenError::cCONNECT_ERRORS))
      {
         cerr << GenTime::GetWallClockTime() << " ";
         cerr << "WARNING -- ";
         cerr << mCallingClassName;
         cerr << " called GenUmpIO::Init which could not find a socket file on the list.  ";
         cerr << "Last file on list was: " << aSocketFile[index - 1];
         cerr << "\nWill initialize UMP with out a socket file" << endl;
      }
   }

   // Look to see if two sockets should be open which is primarily
   // needed for broadcast.  Set "twoSocket" flag accordingly.
   int twoSocket;
   if (aMsgType >= 9 && aMsgType <= 11)
   {
      // Look for a broad cast address and if found open two ump sockets,
      // otherwise just open one.

      string            address(aSendRecvAgentName != 0 ? aSendRecvAgentName : "");
      string::size_type pos = address.find_first_of('.');

      if (pos == string::npos)
      {
         // Using socket table agent name so no need to go
         // any futher
         twoSocket = 0;
      }
      else
      {
         std::stringstream strm(address.substr(0, pos));

         int byteOneValue;
         strm >> byteOneValue;

         if (byteOneValue >= 224 && byteOneValue <= 239)
         {
            // Got multicast, no need to look any more
            twoSocket = 1;
         }
         else
         {
            // Look for "255" in the last byte of the address
            // If found, set two socket flag high
            string::size_type whitespacePos = address.find_first_of(" \t\n");
            if (whitespacePos != string::npos)
            {
               // Extract IP address (without NetID)
               address = address.substr(0, whitespacePos);
            }

            pos       = address.find_last_of('.');
            twoSocket = address.compare(pos + 1, 3, string("255")) ? 0 : 1;
         }
      }
   }
   else
   {
      // Using TCP or RUDP so one socket will work
      twoSocket = 0;
   }

   // Multiple receiver flag has the potential to induce an error in UMP 7.07.
   // Only allow option for multicast and broadcast.
   if (twoSocket == 0)
   {
      aUmpOptions = aUmpOptions & ~UMP_ALLOW_MULTIPLE_RECEIVERS;
   }

   // Create the first socket
   if (aMsgType != UDP_SEND_ONLY)
   {
      int connectStatus;
      if (mSocketRecv > 0 && (aMsgType == TCP_CLIENT || aMsgType == TCP_ACTIVE))
      {
         connectStatus = umpRetryConnect(mSocketRecv, 0); //  "0" means no options set
      }
      else
      {
         // Close old sockets if they exist
         DeleteSockets();

         mSocketRecv = umpCreateSocketC(aSendRecvAgentName,
                                        aSendToPort,
                                        aRecvOnPort,
                                        aMsgType,
                                        NO_CONVERT,
                                        aUmpOptions & ~TCP_CONNECT_NO_WARNING_MSG);

         if (aMsgType == TCP_CLIENT || aMsgType == TCP_ACTIVE)
         {
            connectStatus = umpGetConnectedState(mSocketRecv, 1); // "1" means do actively querry connection.
         }
         else
         {
            connectStatus = mSocketRecv;
         }
      }

      if (connectStatus <= 0)
      {
         if (!(aUmpOptions & TCP_CONNECT_NO_WARNING_MSG) && GenError::ShowError(GenError::cCONNECT_ERRORS))
         {
            cerr << GenTime::GetWallClockTime() << " ";
            cerr << "WARNING -- ";
            cerr << "Could not open First UMP socket in GenUmpIO::Init() when called from " << mCallingClassName;
            cerr << " for agent " << aSendRecvAgentName;
            cerr << ", for send port " << aSendToPort;
            cerr << ", for recv port " << aRecvOnPort;
            cerr << ", for msg Type " << aMsgType;
            cerr << ".  Received UMP error code/status: " << connectStatus << endl;
         }
         return connectStatus; // Need to return a better error code
      }
      else
      {
         SetUmpDefaultOptions(mSocketRecv, aMsgType);
         umpGetSocketInfoC(mSocketRecv, mSocketRecvInfoPtr, UMP_AUTO_UPDATE);
      }
   }

   // Create/define a send socket if requested
   if (aSendRecvAgentName != 0 && *aSendRecvAgentName != '\0')
   {
      if (twoSocket || aMsgType == UDP_SEND_ONLY)
      {
         // Delete old socket if it was opened
         if (mSocketSend > 0)
         {
            umpDeleteSocketC(mSocketSend);
         }

         // Create a seperate socket for sending
         mSocketSend =
            umpCreateSocketC(aSendRecvAgentName, aSendToPort, 0, aMsgType, NO_CONVERT, aUmpOptions & ~TCP_CONNECT_NO_WARNING_MSG);
         if (mSocketSend < 0)
         {
            if (!(aUmpOptions & TCP_CONNECT_NO_WARNING_MSG) && GenError::ShowError(GenError::cCONNECT_ERRORS))
            {
               cerr << GenTime::GetWallClockTime() << " ";
               cerr << "WARNING -- ";
               cerr << "Could not open Transmit UMP socket in GenUmpIO::Init() when called from " << mCallingClassName;
               cerr << "\n.Received UMP error code/status: " << mSocketSend;
               cerr << " from umpCreateSocket" << endl;
            }
            return mSocketSend;
         }

         SetUmpDefaultOptions(mSocketSend, UDP_SEND_ONLY);

         // Get socket info to find source port so I can ignore
         // myself when sending and receiving broadcast
         UMPSockInfoType socketSendInfo;
         umpGetSocketInfoC(mSocketSend, &socketSendInfo, 0);
         mSendFromPort = socketSendInfo.localPort;
      }
      else
      {
         // Use same socket for sending and receiving
         mSocketSend = mSocketRecv;

         SetUmpDefaultOptions(mSocketRecv, aMsgType);

         if (aMsgType == UDP_RELIABLE_BIDIR)
         {
            return SetRudpStats(mSocketSend, aLocalUpdatePeriodSec, aRemoteUpdatePeriodSec);
         }
      }
   }
   return 1;
}

void GenUmpIO::Terminate()
{
   DeleteSockets();

   delete mSendBuf;
   delete mRecvBuf;
   delete mRudpStats;

   mSendBuf   = 0;
   mRecvBuf   = 0;
   mRudpStats = 0;

   // The code originally had an 'umpEndC()' at this point.  It had to be
   // removed because there may be multiple GenUmpIO objects and performing
   // an 'umpEndC()' closes them all!
}

int GenUmpIO::GetLocalRecvPort() const
{
   if (mSocketRecv >= 0)
   {
      return umpGetLocalPort(mSocketRecv);
   }
   else
   {
      return -1;
   }
}

unsigned int GenUmpIO::GetSendToAddr() const
{
   if (mSocketSend >= 0)
   {
      UMPSockInfoType socketInfo;
      umpGetSocketInfoC(mSocketSend, &socketInfo, 0);
      return socketInfo.destInetAddr;
   }
   else
   {
      return 0;
   }
}

string GenUmpIO::GetSendToAddrString() const
{
   if (mSocketSend >= 0)
   {
      UMPSockInfoType socketInfo;
      umpGetSocketInfoC(mSocketSend, &socketInfo, 0);
      char buffer[UMP_IP_STRING_LENGTH];
      umpIPtoString(&socketInfo.destIPAddr, buffer, UMP_IP_STRING_LENGTH);
      return string(buffer);
   }
   else
   {
      return string("");
   }
}

unsigned short GenUmpIO::GetRecvFromPort() const
{
   if (mSocketRecv >= 0)
   {
      return mSocketRecvInfoPtr->srcPort;
   }
   else
   {
      return 0;
   }
}


unsigned int GenUmpIO::GetRecvFromAddr() const
{
   if (mSocketRecv >= 0)
   {
      return mSocketRecvInfoPtr->srcInetAddr;
   }
   else
   {
      return 0;
   }
}

string GenUmpIO::GetRecvFromAddrString() const
{
   if (mSocketRecv >= 0)
   {
      char buffer[UMP_IP_STRING_LENGTH];
      umpIPtoString(&mSocketRecvInfoPtr->srcIPAddr, buffer, UMP_IP_STRING_LENGTH);
      return string(buffer);
   }
   else
   {
      return string("");
   }
}

int GenUmpIO::GetSendToPort() const
{
   if (mSendToPort != 0)
   {
      return mSendToPort;
   }
   else if (mSocketSend >= 0)
   {
      UMPSockInfoType socketInfo;
      umpGetSocketInfoC(mSocketRecv, &socketInfo, 0);
      return static_cast<int>(socketInfo.destPort);
   }
   else
   {
      return -1;
   }
}

int GenUmpIO::Receive(int aWaitTimeInMicroSec)
{
   // Return here if socket is not open
   if (mSocketRecv < 0)
   {
      cerr << GenTime::GetWallClockTime() << " ";
      cerr << "WARNING -- ";
      cerr << mCallingClassName;
      cerr << " called GenUmpIO::Receive without UMP being correctly initialized." << endl;
      return -1;
   }

   ClearInputErrorStatus();

   unsigned long validBytes = mRecvBuf->GetGetOffset(GenBuf::FromDataEnd);
   if (validBytes && GenError::ShowError(GenError::cIO_ERRORS))
   {
      cerr << GenTime::GetWallClockTime() << " ";
      cerr << "WARNING -- ";
      cerr << mCallingClassName;
      cerr << " called GenUmpIO::Receive which has " << validBytes << " bytes left in buffer." << endl;
   }

   int numBytes;

   // Loop until we find data from the outside or no data is present
   for (;;)
   {
      unsigned char* bufferPtr;
      numBytes = umpRcvMsgPtrC(mSocketRecv, reinterpret_cast<void**>(&bufferPtr), aWaitTimeInMicroSec, 0);

      // Return immediately if an error occured
      if (numBytes < -1)
      {
         if (mRecvError == 0)
         {
            mRecvError = 1;
            if (GenError::ShowError(GenError::cIO_ERRORS))
            {
               cerr << GenTime::GetWallClockTime() << " ";
               cerr << "WARNING -- ";
               cerr << "  GenUmpIO::Receive() is in an error condition, because ";
               switch (numBytes)
               {
               case UMP_CONNECTION_CLOSED:
                  cerr << "the TCP sender has closed the connection";
                  break;
               case UMP_FOREIGN_AGENT_NOT_READY:
                  cerr << "the UDP sender/receiver had an error on the previous receive";
                  break;
               default:
                  cerr << "beats me, but the status error number is " << numBytes;
                  break;
               }
               cerr << " when called by " << mCallingClassName << endl;
            }
         }
         break;
      }
      else if (mRecvError)
      {
         mRecvError = 0;
         if (GenError::ShowError(GenError::cIO_ERRORS))
         {
            cerr << GenTime::GetWallClockTime() << " ";
            cerr << "WARNING -- ";
            cerr << "GenUmpIO::Receive() no longer has an error when called by ";
            cerr << mCallingClassName << endl;
         }
      }

      if (numBytes == -1)
      {
         // Got no data and there are no errors to deal with
         break;
      }

      // If this message came from me then ignore it
      if (mSendFromPort != 0 && mSendFromPort == mSocketRecvInfoPtr->srcPort)
      {
         unsigned int fromMe = 0;
         // Look for a loop back address
         if (((mSocketRecvInfoPtr->srcInetAddr >> 24) & 0xFF) == 127)
         {
            fromMe = 1;
         }
         else
         {
            const unsigned int* localIPaddresses = umpGetLocalIPaddresses();
            while (*localIPaddresses != 0)
            {
               if (*localIPaddresses == mSocketRecvInfoPtr->srcInetAddr)
               {
                  fromMe = 1;
                  break;
               }
               ++localIPaddresses;
            }
         }
         if (!fromMe)
         {
            mRecvBuf->SetBuffer(bufferPtr, numBytes, numBytes);
            break;
         }
      }
      else
      {
         mRecvBuf->SetBuffer(bufferPtr, numBytes, numBytes);
         break;
      }
   }

   if (numBytes > 0)
   {
      CheckForRudpStats();
   }

   return numBytes;
}

int GenUmpIO::Send()
{
   // Return here if socket is not open
   if (mSocketSend < 0)
   {
      cerr << GenTime::GetWallClockTime() << " ";
      cerr << "WARNING -- ";
      cerr << mCallingClassName;
      cerr << " called GenUmpIO::Send called without UMP being correctly initialized." << endl;
      return -1;
   }

   int length = static_cast<int>(mSendBuf->GetPutOffset(GenBuf::FromBeg));
   if (length)
   {
      int status = umpSendMsgC(mSocketSend, mSendBuf->GetBuffer(), length, static_cast<int>(mSendToPort));

      if (status < 0)
      {
         if (mSendError == 0)
         {
            mSendError = 1;
            if (GenError::ShowError(GenError::cIO_ERRORS))
            {
               cerr << GenTime::GetWallClockTime() << " ";
               cerr << "WARNING -- ";
               cerr << "  GenUmpIO::Send() is in an error condition, because ";
               switch (status)
               {
               case UMP_WARNING:
                  cerr << "the receiver has not registered with the arbiter or a TCP connection has not been made";
                  break;
               case UMP_CONNECTION_CLOSED:
                  cerr << "the TCP receiver has closed the connection";
                  break;
               case UMP_FOREIGN_AGENT_NOT_READY:
                  cerr << "the UDP sender/receiver had an error on the previous send";
                  break;
               case UMP_RUDP_QUEUE_FULL:
                  cerr << "the RUDP send buffer is full and cannot accept anymore data";
                  break;
               case UMP_SEND_BUF_FULL:
                  cerr << "the low level send buffer is full and cannot accept anymore data";
                  break;
               default:
                  cerr << "beats me, but the status error number is " << status;
                  break;
               }
               cerr << " while sending " << length << " bytes ";
               cerr << " when called by " << mCallingClassName << endl;
            }
         }
      }
      else if (mSendError)
      {
         mSendError = 0;
         if (GenError::ShowError(GenError::cIO_ERRORS))
         {
            cerr << GenTime::GetWallClockTime() << " ";
            cerr << "WARNING -- ";
            cerr << "GenUmpIO::Send() no longer has an error when called by ";
            cerr << mCallingClassName << endl;
         }
      }

      mSendBuf->Reset();
      ClearOutputErrorStatus();
      CheckForRudpStats();

      return status < 0 ? status : length;
   }
   else
   {
      if (mRudpStats != 0)
      {
         umpCheckRUDPC(mSocketSend, 0); // Last argument is for no options
      }

      return 0;
   }
}

unsigned int GenUmpIO::GetMaxSendSize()
{
   return mSendBuf != 0 ? static_cast<unsigned int>(mSendBuf->GetTotalBytes()) : 0;
}

int GenUmpIO::GetTcpConnectState() const
{
   // Return  1 if connected
   //         0 if never connected
   //        -1 if was connected, but now connection lost

   if (mSocketSend > 0)
   {
      return umpGetConnectedState(mSocketSend, 1);
   }
   else
   {
      return 0;
   }
}

void GenUmpIO::ChangePutBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   if (mSendBuf != 0)
   {
      mSendBuf->ChangeBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
   }
}

//
//  Private functions
//


void GenUmpIO::CreateBuffers(const char* aSendRecvAgentName)
{
   if (mRecvBuf == 0)
   {
      // We won't have a buffer until we call UmpRecv for the first time.
      mRecvBuf = new GenBufIUser(GenBuf::BigEndian, 0, 0, 0);
      SetGenBufXferI(mRecvBuf);
   }

   // Take the default and not let the buffer grow
   // There needs to be a better way to look for broadcast
   if (mSendBuf == 0)
   {
#ifdef __host_mips
      if (aSendRecvAgentName != 0 && strstr(aSendRecvAgentName, ".255") != 0)
      {
         mSendBuf = new GenBufOManaged(GenBuf::BigEndian, 1472);
      }
      else
#endif
      {
         mSendBuf = new GenBufOManaged(GenBuf::BigEndian, 65200);
      }
      SetGenBufXferO(mSendBuf);
   }
}

void GenUmpIO::DeleteSockets()
{
   if (mSocketRecv == mSocketSend)
   {
      if (mSocketRecv >= 0)
      {
         umpDeleteSocketC(mSocketRecv);
         mSocketRecv = -99999;
         mSocketSend = -99999;
      }
   }
   else
   {
      if (mSocketRecv >= 0)
      {
         umpDeleteSocketC(mSocketRecv);
         mSocketRecv = -99999;
      }
      if (mSocketSend >= 0)
      {
         umpDeleteSocketC(mSocketSend);
         mSocketSend = -99999;
      }
   }
}

// Private
void GenUmpIO::SetUmpDefaultOptions(int aSocketNumber, UmpMsgType aMsgType)
{
   // Determine TCP or UDP socket type.  We need a larger buffer size for
   // UDP connections since delivery is not guaranteed
   int bufferSize;
   if ((aMsgType <= UDP_CLIENT) || (aMsgType == UDP_RELIABLE_BIDIR))
   {
      bufferSize = 1048576;
   }
   else
   {
      bufferSize = 262144;
   }

   if (aMsgType != UDP_RCV_ONLY)
   {
      umpSetSockOpt(aSocketNumber, UMP_SO_SNDBUF, bufferSize);
      // cout << "GenUmpIO when called by " << mCallingClassName << " set send buffer size to ";
      // cout << umpGetSockOpt(aSocketNumber, UMP_SO_SNDBUF) << endl;
   }

   if (aMsgType != UDP_SEND_ONLY)
   {
      umpSetSockOpt(aSocketNumber, UMP_SO_RCVBUF, bufferSize);
      // cout << "GenUmpIO when called by " << mCallingClassName << " set recv buffer size to ";
      // cout << umpGetSockOpt(aSocketNumber, UMP_SO_RCVBUF) << endl;
   }
}


// Private
int GenUmpIO::SetRudpStats(int aSocketNum, float aLocalUpdatePeriodSec, float aRemoteUpdatePeriodSec)
{
   int rudpRetransmitTimeOut;
   if (mRudpRetransmitTimeOut != 0)
   {
      // Take overrides
      rudpRetransmitTimeOut = mRudpRetransmitTimeOut;
   }
   else
   {
      // Compute RUDP variables
      float retransmitTimeOut = (aLocalUpdatePeriodSec + aRemoteUpdatePeriodSec) * 1.1F; // Add 10% fudge
      if (retransmitTimeOut > aRemoteUpdatePeriodSec * 3.0F)
      {
         // Remote is runnig much quicker than local computer, so if we don't get an acknowledgement
         // message within 3 times of the remote time step we are never go to get an  acknowledgement
         // message.
         retransmitTimeOut = aRemoteUpdatePeriodSec * 3.0F;
      }

      rudpRetransmitTimeOut = static_cast<int>(retransmitTimeOut * 1.0e+6F); // Convert to micro secs
   }

   int rudpSendQueueLength;
   if (mRudpSendQueueLength != 0)
   {
      // Take overrides
      rudpSendQueueLength = mRudpSendQueueLength;

      // Set trip levels for error messages.
      mRudpMaxSendQ = static_cast<unsigned int>(static_cast<float>(rudpSendQueueLength * 0.8F));
   }
   else
   {
      // Compute RUDP variables
      rudpSendQueueLength = static_cast<int>(aRemoteUpdatePeriodSec / aLocalUpdatePeriodSec + 0.5) + 9;

      // Set trip levels for error messages.  If we have the same update rate on both
      // sides of the "pipe" then the queue length will be 10 and we would like to allow the Queue
      // get bigger that 5 before we diplay any errors, so 10 - 5 = 5.
      // of queues we will have it both sides of the "pipe" are updating at the same rate.
      mRudpMaxSendQ = rudpSendQueueLength - 3;
   }

   int rudpRecvQueueLength;
   if (mRudpRecvQueueLength != 0)
   {
      // Take overrides
      rudpRecvQueueLength = mRudpRecvQueueLength;

      // Set trip levels for error messages.
      mRudpMaxRecvQ = static_cast<unsigned int>(static_cast<float>(rudpRecvQueueLength * 0.8F));
   }
   else
   {
      // Compute RUDP variables
      rudpRecvQueueLength = static_cast<int>(aLocalUpdatePeriodSec / aRemoteUpdatePeriodSec + 0.5) + 9;

      // Set trip levels for error messages.  If we have the same update rate on both
      // sides of the "pipe" then the queue length will be 10 and we would like to allow the Queue
      // get bigger that 5 before we diplay any errors, so 10 - 5 = 5.
      // of queues we will have it both sides of the "pipe" are updating at the same rate.
      mRudpMaxRecvQ = rudpRecvQueueLength - 3;
   }

   if (mRudpStats == 0)
   {
      mRudpStats = new GenUmpIORudpStats;
   }

   int stat = umpSetupRUDPC(aSocketNum,            // socket
                            rudpRetransmitTimeOut, // retransmitTimeout (micorsec)
                            rudpSendQueueLength,   // #msgs UMP can hold in send que before it overflows
                            rudpRecvQueueLength,   // #msgs UMP can hold in receive que before it overflows
                            mRudpStats,            // statStructPtr
                            0);                    // option

   // zero if successful, a negative if an error occured
   if (stat < 0)
   {
      cerr << GenTime::GetWallClockTime() << " ";
      cerr << "WARNING -- ";
      cerr << "Could not configure UDP_RELIABLE in GenUmpIO::SetRudpStats "
           << "when called by " << mCallingClassName << endl;
      return stat;
   }
   else
   {
      // cout << "GenUmpIO when called by " << mCallingClassName;
      // cout << " set RUDP timeout to " << rudpRetransmitTimeOut;
      // cout << ", set RUDP send Que to " << rudpSendQueueLength;
      // cout << ", set RUDP recv Que to " << rudpRecvQueueLength << endl;
      return 1;
   }
}

void GenUmpIO::CheckForRudpStats()
{
   if (mRudpStats != 0)
   {
      if (--mRudpCheckCount == 0)
      {
         // Reset counter
         mRudpCheckCount = 100;

         if (mRudpStats->rusMaxSendQ > mRudpMaxSendQ)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName << " RUDP max msgs in sendQ is now ";
            cout << mRudpStats->rusMaxSendQ << endl;
         }

         if (mRudpStats->rusMaxRcvQ > mRudpMaxRecvQ)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName << " RUDP max msgs in recvQ is now ";
            cout << mRudpStats->rusMaxRcvQ << endl;
         }

         if (mRudpStats->rusFailedSends > sRudpFailedSends)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName << " RUDP total failed sends is now ";
            cout << mRudpStats->rusFailedSends << endl;
         }

         if (mRudpStats->rusRetrans > sRudpRetrans)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName << " RUDP total retransmitted sends is now ";
            cout << mRudpStats->rusRetrans << endl;
         }

         if (mRudpStats->rusUnnecRetrans > sRudpUnnecRetrans)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName
                 << " RUDP total unnecessarily retransmitted sends is now ";
            cout << mRudpStats->rusUnnecRetrans << endl;
         }

         if (mRudpStats->rusRejectedRcvs > sRudpRejectedRecvs)
         {
            cout << GenTime::GetWallClockTime() << " ";
            cout << "In GenUmpIO, called by " << mCallingClassName << " RUDP total rejected receives is now ";
            cout << mRudpStats->rusRejectedRcvs << endl;
         }

         // Zero counters for next sample period.
         // rusMsgsInSendQ and rusMsgsInRcvQ are not cumulative so they should not be reset.
         mRudpStats->rusMaxSendQ     = 0;
         mRudpStats->rusMaxRcvQ      = 0;
         mRudpStats->rusFailedSends  = 0;
         mRudpStats->rusRetrans      = 0;
         mRudpStats->rusUnnecRetrans = 0;
         mRudpStats->rusRejectedRcvs = 0;
      }
   }
}
