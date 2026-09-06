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

#ifndef PAKTCP_SERVER_HPP
#define PAKTCP_SERVER_HPP

#include "packetio_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"

class GenTCP_IO;
namespace GenSockets
{
class GenSocket;
}


class PACKETIO_EXPORT PakTCP_Server
{
public:
   PakTCP_Server();

   ~PakTCP_Server();

   bool Init(int aPortNumber, bool aNoDelay = false);

   GenSockets::GenSocket* GetSocket() const { return mServerSocketPtr; }

   bool OwnsConnections();

   void SetOwnsConnections(bool aTF);

   GenTCP_IO* Accept(int aWaitTimeInMicroSeconds);

   void CloseAllConnections();

   void CloseConnection(GenTCP_IO* aConnectionPtr);

   void ClearConnectionAddressList(GenTCP_IO* aConnectionPtr);

   size_t ConnectionCount() const { return mConnectionList.size(); }

   GenTCP_IO* GetConnection(size_t aIndex) const { return mConnectionList[aIndex]; }

   void GetSenderIPAddress(GenTCP_IO* aConnectionPtr, std::string& aStr);

   //! Occurs as often as SimulationUpdate, even if the simulation is paused.
   static UtCallbackListN<void(GenTCP_IO*)> OnDisconnect;

protected:
   typedef std::vector<GenTCP_IO*>           IoList;
   typedef std::map<GenTCP_IO*, std::string> AddressList;
   GenSockets::GenSocket*                    mServerSocketPtr;
   IoList                                    mConnectionList;
   AddressList                               mConnectionAddressList;
   bool                                      mOwnsConnections;
   UtCallbackHolder                          mCallbacks;
};
#endif
