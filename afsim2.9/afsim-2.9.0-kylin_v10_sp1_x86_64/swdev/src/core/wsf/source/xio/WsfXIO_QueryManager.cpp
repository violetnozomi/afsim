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

#include "xio/WsfXIO_QueryManager.hpp"

#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Query.hpp"

WsfXIO_QueryManager::WsfXIO_QueryManager(WsfXIO_Interface* aInterfacePtr)
{
   mCallbacks.Add(aInterfacePtr->Connect(&WsfXIO_QueryManager::HandleQueryResolution, this));
}

WsfXIO_QueryManager::~WsfXIO_QueryManager() {}

void WsfXIO_QueryManager::AddQuery(WsfXIO_Query* aQueryPtr)
{
   mQueries.insert(QueryMap::value_type(aQueryPtr->GetQueryId(), aQueryPtr));
}

void WsfXIO_QueryManager::RemoveQuery(WsfXIO_Query* aQueryPtr)
{
   mQueries.erase(aQueryPtr->GetQueryId());
}

void WsfXIO_QueryManager::HandleQueryResolution(WsfXIO_QueryResolutionPkt& aPkt)
{
   QueryMap::iterator i = mQueries.find(aPkt.mQueryId);
   if (i != mQueries.end())
   {
      i->second->HandleResolution(aPkt);
   }
}
