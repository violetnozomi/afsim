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

#include "xio/WsfXIO_PlatformInfoRequest.hpp"

#include "xio/WsfXIO_PacketRegistry.hpp"


WsfXIO_PlatformInfoRequest::WsfXIO_PlatformInfoRequest(int                aPlatformIndex,
                                                       unsigned int       aInformationTypes,
                                                       WsfXIO_Connection* aConnectionPtr,
                                                       bool               aIsReliable)
   : WsfXIO_Request(aConnectionPtr, aIsReliable)
   , mPlatformIndex(aPlatformIndex)
   , mInfoType(aInformationTypes)
   , mUseDeadReckonDefaults(true)
{
   SetDeadReckonParameters();
   mUseDeadReckonDefaults = true;
}

void WsfXIO_PlatformInfoRequest::SetDeadReckonParameters(double aDeadReckonInterval,
                                                         double aMaxDeadReckonError,
                                                         double aMaxDeadReckonOrientationError)
{
   mDeadReckonInterval            = aDeadReckonInterval;
   mMaxDeadReckonError            = aMaxDeadReckonError;
   mMaxDeadReckonOrientationError = aMaxDeadReckonOrientationError;
   mUseDeadReckonDefaults         = false;
}

void WsfXIO_PlatformInfoRequest::Initialized()
{
   WsfXIO_RequestPlatformInfoPkt pkt;
   pkt.mPlatformIndex                 = mPlatformIndex;
   pkt.mInfoTypes                     = mInfoType;
   pkt.mUseDeadReckonDefaults         = mUseDeadReckonDefaults;
   pkt.mDeadReckonInterval            = mDeadReckonInterval;
   pkt.mMaxDeadReckonError            = mMaxDeadReckonError;
   pkt.mMaxDeadReckonOrientationError = mMaxDeadReckonOrientationError;
   SendRequest(pkt);
}
