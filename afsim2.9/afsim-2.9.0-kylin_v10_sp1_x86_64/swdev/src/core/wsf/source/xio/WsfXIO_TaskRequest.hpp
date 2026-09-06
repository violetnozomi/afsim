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

#ifndef WSFXIO_TASKREQUEST_HPP
#define WSFXIO_TASKREQUEST_HPP

#include "wsf_export.h"

#include "WsfStringId.hpp"
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_ResponsePkt;
class WsfXIO_TaskInitPkt;
class WsfXIO_TaskUpdatePkt;

class WSF_EXPORT WsfXIO_TaskRequest : public WsfXIO_Request
{
public:
   WsfXIO_TaskRequest(WsfXIO_Connection* aConnectionPtr,
                      unsigned int       aPlatformIndex,
                      WsfStringId        aProcessorName,
                      bool               aTakeControl = false,
                      bool               aIsReliable  = true);

   virtual void HandleInitialize(WsfXIO_TaskInitPkt& aPkt) = 0;
   virtual void HandleUpdate(WsfXIO_TaskUpdatePkt& aPkt)   = 0;

protected:
   void HandleResponse(WsfXIO_ResponsePkt& aPkt) override;
   void Initialized() override;

   bool         mTakeControl;
   unsigned int mPlatformIndex;
   WsfStringId  mProcessorName;
};
#endif
