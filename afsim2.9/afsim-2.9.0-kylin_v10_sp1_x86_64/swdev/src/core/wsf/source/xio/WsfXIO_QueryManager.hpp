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

#ifndef WSFXIO_QUERYMANAGER_HPP
#define WSFXIO_QUERYMANAGER_HPP

#include "wsf_export.h"

#include <map>

#include "GenUniqueId.hpp"
#include "UtCallbackHolder.hpp"
class WsfXIO_Interface;
class WsfXIO_Query;
class WsfXIO_QueryResolutionPkt;

class WSF_EXPORT WsfXIO_QueryManager
{
public:
   WsfXIO_QueryManager(WsfXIO_Interface* aInterfacePtr);
   ~WsfXIO_QueryManager();

   void AddQuery(WsfXIO_Query* aQueryPtr);
   void RemoveQuery(WsfXIO_Query* aQueryPtr);

protected:
   void HandleQueryResolution(WsfXIO_QueryResolutionPkt& aPkt);

   typedef std::map<GenUniqueId, WsfXIO_Query*> QueryMap;
   UtCallbackHolder                             mCallbacks;
   QueryMap                                     mQueries;
};

#endif
