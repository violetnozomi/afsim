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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISEXCHANGE_HPP
#define WSFDISEXCHANGE_HPP

#include "wsf_export.h"

class DisEntityId;
#include "WsfExchange.hpp"
class WsfDisInterface;
class WsfDisPlatform;
class DisPdu;
#include "UtCallbackHolder.hpp"

//! A helper class for platform-to-platform suppling or receiving of a commodity or service
//! across a DIS connection.  This was previously placed in WsfDisInterface, but now has
//! been broken out into a separate object to keep from further cluttering WsfDisInterface.

class WSF_EXPORT WsfDisExchange
{
public:
   static bool DebugEnabled() { return sDebug; }

   WsfDisExchange();
   ~WsfDisExchange();

   bool Initialize(WsfDisInterface* aDisInterfacePtr);

   static bool ProcessInput(UtInput& aInput);

   // Callback calls.
   void ExchangeQueried(double aSimTime, const WsfExchange::Query& aQuery);

private:
   static bool sDebug;

   void FillInPduHeader(double aSimTime, DisPdu* aPduPtr);

   WsfDisExchange(const WsfDisExchange& aSrc); // declare but do not define.

   WsfDisInterface* mDisInterfacePtr;
   UtCallbackHolder mCallbacks;
};

#endif
