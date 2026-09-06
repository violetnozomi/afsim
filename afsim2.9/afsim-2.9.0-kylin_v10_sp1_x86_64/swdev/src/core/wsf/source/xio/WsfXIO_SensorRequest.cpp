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

#include "xio/WsfXIO_SensorRequest.hpp"

WsfXIO_SensorDataRequest::WsfXIO_SensorDataRequest(WsfXIO_Connection* aConnectionPtr,
                                                   int                aPlatformIndex,
                                                   WsfStringId        aSensorName,
                                                   int                aRequestedData,
                                                   bool               aIsReliable /*=true*/)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
{
   mRequestPkt.mPlatformIndex = aPlatformIndex;
   mRequestPkt.mSensorNameId  = aSensorName;
   mRequestPkt.mRequiredData  = aRequestedData;
}

void WsfXIO_SensorDataRequest::Initialized()
{
   SendRequest(mRequestPkt);
}
