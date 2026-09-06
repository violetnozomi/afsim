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

#include "xio/WsfXIO_Query.hpp"

#include <cassert>

#include "UtBinder.hpp"
#include "UtCallback.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_QueryManager.hpp"

//! Initializes a query to an application
//! @param aConnectionPtr A reliable connection to the application which is the destination
WsfXIO_Query::WsfXIO_Query(WsfXIO_Connection* aConnectionPtr)
   : mResult(wsf::xio::cQUERY_NOT_COMPLETE)
   , mConnectionPtr(aConnectionPtr)
{
   // Queries only allowed with reliable connections
   assert(aConnectionPtr->IsReliable());

   mCallbacks.Add(mConnectionPtr->GetInterface().DisconnectConnect(aConnectionPtr,
                                                                   [this](WsfXIO_Connection* aConnectionPtr)
                                                                   { HandleDisconnect(aConnectionPtr); }));

   mConnectionPtr->GetInterface().GetQueryManager().AddQuery(this);
}

// virtual
WsfXIO_Query::~WsfXIO_Query()
{
   if (mResult == wsf::xio::cQUERY_NOT_COMPLETE)
   {
      mConnectionPtr->GetInterface().GetQueryManager().RemoveQuery(this);
   }
}

// virtual
void WsfXIO_Query::HandleDisconnect(WsfXIO_Connection* aConnectionPtr)
{
   if (mResult == wsf::xio::cQUERY_NOT_COMPLETE)
   {
      mResult = wsf::xio::cQUERY_DISCONNECTED;
   }
   Complete();
   mConnectionPtr = nullptr;
}

//! Sets the query timeout.  If they query has not been resolved prior to this time,
//! OnComplete will be invoked
void WsfXIO_Query::SetTimeout(double aTimeoutSeconds)
{
   mCallbacks.Add(mConnectionPtr->GetInterface().TimeoutConnect(aTimeoutSeconds, [this] { HandleTimeout(); }));
}

// virtual
void WsfXIO_Query::HandleTimeout()
{
   mResult = wsf::xio::cQUERY_TIMED_OUT;
   Complete();
}

// virtual
void WsfXIO_Query::HandleResolution(WsfXIO_QueryResolutionPkt& aPkt)
{
   mResult = aPkt.mResult;
   Complete();
}

// virtual
void WsfXIO_Query::Complete()
{
   mConnectionPtr->GetInterface().GetQueryManager().RemoveQuery(this);
   OnComplete(this);
}

//! Adds a query to the query set.  This object takes ownership of aQuery
void WsfXIO_QuerySet::AddQuery(WsfXIO_Query* aQuery)
{
   if (aQuery->GetResult() == wsf::xio::cQUERY_NOT_COMPLETE)
   {
      mCallbacks.Add(aQuery->OnComplete.Connect(UtStd::Bind(&WsfXIO_QuerySet::HandleQueryComplete, this)));
      mIncompleteQueries.push_back(aQuery);
   }
   else
   {
      mCompleteQueries.push_back(aQuery);
   }
}

void WsfXIO_QuerySet::HandleQueryComplete(WsfXIO_Query* aQuery)
{
   QueryList::iterator i = std::find(mIncompleteQueries.begin(), mIncompleteQueries.end(), aQuery);
   if (i != mIncompleteQueries.end())
   {
      mIncompleteQueries.erase(i);
      mCompleteQueries.push_back(aQuery);

      if (mIncompleteQueries.empty())
      {
         OnComplete(this);
      }
   }
}

//! Copies the list of successful queries to aQueryList
void WsfXIO_QuerySet::GetSuccessfulQueries(QueryList& aQueryList) const
{
   for (size_t i = 0; i < mCompleteQueries.size(); ++i)
   {
      WsfXIO_Query* queryPtr = mCompleteQueries[i];
      if ((queryPtr->GetResult() & wsf::xio::cQUERY_SUCCESS_MASK) != 0)
      {
         aQueryList.push_back(queryPtr);
      }
   }
}

//! Sets a timeout for the query set.  OnComplete will be invoked
//! if the timeout is reached prior to all queries completing.  Using WsfXIO_QuerySet::SetTimeout()
//! removes the need for calling WsfXIO_Query::SetTimeout
void WsfXIO_QuerySet::SetTimeout(double aWaitTimeSeconds)
{
   mCallbacks.Add(mInterfacePtr->TimeoutConnect(aWaitTimeSeconds, [this] { HandleTimeout(); }));
}

void WsfXIO_QuerySet::HandleTimeout()
{
   mCallbacks.Clear();
   OnComplete(this);
}

//! Removes all queries from the query set
void WsfXIO_QuerySet::Clear()
{
   mCallbacks.Clear();
   for (size_t i = 0; i < mCompleteQueries.size(); ++i)
   {
      delete mCompleteQueries[i];
   }
   for (size_t i = 0; i < mIncompleteQueries.size(); ++i)
   {
      delete mIncompleteQueries[i];
   }
}
