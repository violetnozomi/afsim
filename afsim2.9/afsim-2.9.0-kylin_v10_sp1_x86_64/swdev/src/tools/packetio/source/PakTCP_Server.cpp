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

#include "PakTCP_Server.hpp"

#include <algorithm>
#include <iostream>

#include "GenSocket.hpp"
#include "GenTCP_IO.hpp"
#include "UtLog.hpp"

UtCallbackListN<void(GenTCP_IO*)> PakTCP_Server::OnDisconnect;

PakTCP_Server::PakTCP_Server()
   : mServerSocketPtr(nullptr)
   , mOwnsConnections(true)
{
   mCallbacks.Add(OnDisconnect.Connect(&PakTCP_Server::CloseConnection, this));
}

PakTCP_Server::~PakTCP_Server()
{
   if (mOwnsConnections)
   {
      CloseAllConnections();
   }
   delete mServerSocketPtr;
}

//! Closes any connections made using Accept()
//! @note Only valid if the OwnsConnections is true
void PakTCP_Server::CloseAllConnections()
{
   if (mOwnsConnections)
   {
      for (size_t i = 0; i < mConnectionList.size(); ++i)
      {
         if (mConnectionList[i] != nullptr)
         {
            CloseConnection(mConnectionList[i]);
         }
      }
      mConnectionList.clear();
      mConnectionAddressList.clear();
   }
}

//! Closes a connection belonging to this server.
//! @param aConnectionPtr Pointer to a connection that was created
//!                       using Accept()
//! @note Only valid if the OwnsConnections is true
void PakTCP_Server::CloseConnection(GenTCP_IO* aConnectionPtr)
{
   if (mOwnsConnections)
   {
      if (mConnectionAddressList.size() > 0)
      {
         ClearConnectionAddressList(aConnectionPtr);
      }
      IoList::iterator iter = std::find(mConnectionList.begin(), mConnectionList.end(), aConnectionPtr);
      if (iter != mConnectionList.end())
      {
         mConnectionList.erase(iter);
      }
   }
}

//! Clear the corresponding connected address to the tcp connection
//! that just dropped off.  Must be called before CloseConnection if used.
//! @ param aConnectionPtr Pointer to a connection that was created
//!                        using Accept()
//! @note Only valid if the OwnsConnections is true
void PakTCP_Server::ClearConnectionAddressList(GenTCP_IO* aConnectionPtr)
{
   if (mOwnsConnections)
   {
      std::map<GenTCP_IO*, std::string>::iterator m = mConnectionAddressList.find(aConnectionPtr);
      if (m != mConnectionAddressList.end())
      {
         mConnectionAddressList.erase(m);
      }
   }
}

//! Get the sender ip address as string
//! @ param aConnectionPtr Pointer to a connection that was created
//!                        using Accept()
//! @note Only valid if the OwnsConnection is true
void PakTCP_Server::GetSenderIPAddress(GenTCP_IO* aConnectionPtr, std::string& aStr)
{
   aStr = "NotValid";
   if (mOwnsConnections)
   {
      std::map<GenTCP_IO*, std::string>::iterator m = mConnectionAddressList.find(aConnectionPtr);
      if (m != mConnectionAddressList.end())
      {
         aStr = (*m).second;
      }
   }
}

//! Initializes the server and begins listening for
//! connection requests
bool PakTCP_Server::Init(int aPortNumber, bool aNoDelay)
{
   bool lReturn     = false;
   mServerSocketPtr = new GenSockets::GenSocket(GenSockets::GenSocket::cTCP_SOCKET, aNoDelay);
   if (mServerSocketPtr->Bind(aPortNumber))
   {
      mServerSocketPtr->Listen();
      lReturn = true;
   }
   else
   {
      auto out = ut::log::error() << "Socket cannot bind to port.";
      out.AddNote() << "Port Number: " << aPortNumber;
   }
   return lReturn;
}

//! Returns 'true' the this server class is configured
//! to manage the connections.  Returns 'false' if the
//! connections are managed externally.
bool PakTCP_Server::OwnsConnections()
{
   return mOwnsConnections;
}

//! Sets the OwnsConnections() value
void PakTCP_Server::SetOwnsConnections(bool aTF)
{
   mOwnsConnections = aTF;
}

//! If a connection requests is waiting or is received in the
//! specified wait time, a connection is made to that computer.
//! @param aWaitTimeInMicroSeconds If no connection request is available
//!        PakTCP_Server will wait this number of milliseconds.
//! @return A pointer to the new connection.
GenTCP_IO* PakTCP_Server::Accept(int aWaitTimeInMicroSeconds)
{
   GenTCP_IO* lReturn = nullptr;
   if (mServerSocketPtr)
   {
      std::unique_ptr<GenSockets::GenSocket> sockPtr{mServerSocketPtr->Accept(aWaitTimeInMicroSeconds * 1.0E-6F)};
      if (sockPtr)
      {
         GenSockets::GenIP ip        = sockPtr->GetBoundAddr().GetAddress();
         std::string       ipAddress = ip;
         lReturn                     = new GenTCP_IO();
         lReturn->Init(std::move(sockPtr));
         mConnectionList.push_back(lReturn);
         mConnectionAddressList.insert(make_pair(lReturn, ipAddress));
      }
   }
   return lReturn;
}
