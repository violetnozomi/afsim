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

#include "xio/WsfXIO_Destination.hpp"

#include <algorithm>

#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"

//! Adds a connection to the list.  A connection may be added more
//! than once.
void WsfXIO_Destination::Add(WsfXIO_Connection* aConnectionPtr)
{
   mConnectionList.push_back(aConnectionPtr);
   RebuildIO_List();
}

//! Removes a connection from the list.
void WsfXIO_Destination::Remove(WsfXIO_Connection* aConnectionPtr)
{
   std::vector<WsfXIO_Connection*>::iterator i = std::find(mConnectionList.begin(), mConnectionList.end(), aConnectionPtr);
   if (i != mConnectionList.end())
   {
      mConnectionList.erase(i);
      RebuildIO_List();
   }
}
void WsfXIO_Destination::RemoveAll(WsfXIO_Connection* aConnectionPtr)
{
   bool changed = false;
   for (size_t i = 0; i < mConnectionList.size();)
   {
      if (mConnectionList[i] == aConnectionPtr)
      {
         mConnectionList.erase(mConnectionList.begin() + i);
         changed = true;
      }
      else
      {
         ++i;
      }
   }
   if (changed)
   {
      RebuildIO_List();
   }
}
//! Sends a message to all connections in this destination
void WsfXIO_Destination::Send(WsfXIO_Packet& aPkt)
{
   if (!mConnectionList.empty())
   {
      mConnectionList[0]->GetInterface().Send(aPkt, *this);
   }
}

void WsfXIO_Destination::RebuildIO_List()
{
   mIO_List.clear();
   for (size_t i = 0; i < mConnectionList.size(); ++i)
   {
      mIO_List.push_back(&mConnectionList[i]->GetIO());
   }
   std::sort(mIO_List.begin(), mIO_List.end());
   mIO_List.erase(std::unique(mIO_List.begin(), mIO_List.end()), mIO_List.end());
}
