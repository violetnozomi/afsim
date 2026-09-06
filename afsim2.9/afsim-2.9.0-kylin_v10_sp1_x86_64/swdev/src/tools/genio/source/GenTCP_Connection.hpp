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

#ifndef GENTCP_CONNECTION_HPP
#define GENTCP_CONNECTION_HPP

#include "genio_export.h"

#include <string>

#include "GenBuffer.hpp"
#include "GenInternetSocketAddress.hpp"
#include "GenSocketConnection.hpp"

//! A TCP Connection
class GENIO_EXPORT GenTCP_Connection : public GenSocketConnection
{
public:
   GenTCP_Connection();

   bool InitListen(int aRecvPort = 0);

   bool Init(const std::string& aSendAddress, int aSendToPort, float aWaitTimeSeconds = 1.0f);

   bool Init(std::unique_ptr<GenSockets::GenSocket> aConnectedSocket);

   bool Accept(float aWaitTimeSeconds = 0.0f);

   int SendBuffer(int aWaitTimeInMicroSec, const char* aDataPtr, int aBytes);

   int ReceiveBuffer(int aWaitTimeInMicroSec, char* aBuffer, int aMaxBytes);

   int ReceiveBuffer(int aWaitTimeInMicroSec, GenBuffer& aBuffer, int aMaxBytes = 0);

   GenSockets::GenSocket* GetSocket() { return mSocket.get(); }

   GenSockets::GenInternetSocketAddress GetSenderAddress() const;

   bool GetSenderId(unsigned int& aSrcInetAddr, unsigned short int& aSrcPort) const;

   void SetUseMessageHeaders(bool aTF);

   bool GetUseMessageHeaders() const;

   enum ErrorTypes
   {
      cSOCKET_ERROR    = -1, // use GenSocketManager to get error type
      cNOT_INITIALIZED = -2
   };

protected:
   void ActivateSocketOptions();

   void ResetSocket(std::unique_ptr<GenSockets::GenSocket> aNewSocket = nullptr);

   void UnInitialize();

private:
   std::unique_ptr<GenSockets::GenInternetSocketAddress> mSendAddress;
   bool                                                  mUseMessageHeaders;
   bool                                                  mIsInitialized;
};
#endif
