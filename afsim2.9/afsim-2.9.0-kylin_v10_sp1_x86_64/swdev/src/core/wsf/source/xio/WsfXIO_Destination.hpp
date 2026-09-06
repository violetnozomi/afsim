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

#ifndef WSFXIO_DESTINATION_HPP
#define WSFXIO_DESTINATION_HPP

#include "wsf_export.h"

#include <vector>

class PakSocketIO;
class WsfXIO_Connection;
class WsfXIO_Packet;

//! Maintains a list of connections and provides a method to send messages
//! to all connections at once.  A connection is allowed to be added multiple times.
class WSF_EXPORT WsfXIO_Destination
{
public:
   void Add(WsfXIO_Connection* aConnectionPtr);

   void Remove(WsfXIO_Connection* aConnectionPtr);

   void RemoveAll(WsfXIO_Connection* aConnectionPtr);

   //! Returns true if the destination is null
   bool IsEmpty() const { return mIO_List.empty(); }

   //! Returns a list of PakSocketIO objects used to send messages
   //! Each member in the list is guaranteed to be unique.
   const std::vector<PakSocketIO*>& GetIO_List() const { return mIO_List; }

   void Send(WsfXIO_Packet& aPkt);

   const std::vector<WsfXIO_Connection*>& GetConnections() { return mConnectionList; }

private:
   void                            RebuildIO_List();
   std::vector<WsfXIO_Connection*> mConnectionList;
   std::vector<PakSocketIO*>       mIO_List;
};

#endif
