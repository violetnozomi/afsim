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

#ifndef WSFXIO_QUERY_HPP
#define WSFXIO_QUERY_HPP

#include "wsf_export.h"

#include <vector>

#include "GenUniqueId.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "xio/WsfXIO.hpp"
class WsfXIO_Connection;
class WsfXIO_Interface;
class WsfXIO_QueryResolutionPkt;

//! Represents a request for information from another application.
//! The actual message containing the request parameters must be sent separately.
class WSF_EXPORT WsfXIO_Query
{
public:
   WsfXIO_Query(WsfXIO_Connection* aConnectionPtr);
   virtual ~WsfXIO_Query();

   void SetTimeout(double aTimeoutSeconds);

   const GenUniqueId& GetQueryId() const { return mId; }

   virtual void HandleResolution(WsfXIO_QueryResolutionPkt& aPkt);

   virtual void Complete();

   //! Returns the connection to the requested application
   WsfXIO_Connection* GetConnection() const { return mConnectionPtr; }

   //! Returns the result of the query
   wsf::xio::QueryResult GetResult() const { return mResult; }

   //! Callback list invoked when the query is completed
   UtCallbackListN<void(WsfXIO_Query*)> OnComplete;

protected:
   virtual void HandleDisconnect(WsfXIO_Connection* aConnectionPtr);
   virtual void HandleTimeout();

   wsf::xio::QueryResult mResult;
   GenUniqueId           mId;
   WsfXIO_Connection*    mConnectionPtr;
   UtCallbackHolder      mCallbacks;
};

//! Commonly, a query must be sent to multiple applications.  In this case, a new WsfXIO_Query
//! must be made for each connection.  The WsfXIO_QuerySet provides a way to gather the
//! results of all of these related queries.
class WsfXIO_QuerySet
{
public:
   typedef std::vector<WsfXIO_Query*> QueryList;

   WsfXIO_QuerySet(WsfXIO_Interface* aInterfacePtr)
      : mInterfacePtr(aInterfacePtr)
   {
   }

   ~WsfXIO_QuerySet() { Clear(); }

   void AddQuery(WsfXIO_Query* aQueryPtr);

   void SetTimeout(double aWaitTimeSeconds);

   void Clear();

   //! Callback list invoked when all of the queries are complete or the
   //! timeout has been reached
   UtCallbackListN<void(WsfXIO_QuerySet*)> OnComplete;

   //! Returns a list of completed queries
   const QueryList& GetCompleteQueries() const { return mCompleteQueries; }
   //! Gets a list of completed queries
   void GetCompleteQueries(QueryList& aQueryList) const { aQueryList = mCompleteQueries; }
   //! Copies the list of incomplete queries to aQueryList
   void GetIncompleteQueries(QueryList& aQueryList) const { aQueryList = mIncompleteQueries; }

   void GetSuccessfulQueries(QueryList& aQueryList) const;

protected:
   void HandleTimeout();
   void HandleQueryComplete(WsfXIO_Query* aQueryPtr);

   WsfXIO_Interface* mInterfacePtr;
   UtCallbackHolder  mCallbacks;
   QueryList         mCompleteQueries;
   QueryList         mIncompleteQueries;
};

#endif
