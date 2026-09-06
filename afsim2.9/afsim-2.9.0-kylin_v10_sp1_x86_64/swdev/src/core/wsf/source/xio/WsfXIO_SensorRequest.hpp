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

#ifndef WSFXIO_SENSORREQUEST_HPP
#define WSFXIO_SENSORREQUEST_HPP

#include "wsf_export.h"

#include "WsfStringId.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Request.hpp"

//! Requests copies of messages sent/received by a platform
//! Note: Override HandleResponse(WsfXIO_ResponsePkt& aPkt) to receive updates.
class WSF_EXPORT WsfXIO_SensorDataRequest : public WsfXIO_Request
{
public:
   WsfXIO_SensorDataRequest(WsfXIO_Connection* aConnectionPtr,
                            int                aPlatformIndex,
                            WsfStringId        aSensorName,
                            int                aRequestedData,
                            bool               aIsReliable = true);

private:
   void Initialized() override;

   WsfXIO_RequestSensorDataPkt mRequestPkt;
};
#endif
