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

#ifndef WSFXIO_PLATFORMINFOREQUEST_HPP
#define WSFXIO_PLATFORMINFOREQUEST_HPP

#include "wsf_export.h"

#include "UtMath.hpp"
#include "xio/WsfXIO_Request.hpp"

//! Request for platform information
class WSF_EXPORT WsfXIO_PlatformInfoRequest : public WsfXIO_Request
{
public:
   WsfXIO_PlatformInfoRequest(int                aPlatformIndex,
                              unsigned int       aInformationTypes,
                              WsfXIO_Connection* aConnectionPtr,
                              bool               aIsReliable = true);
   void SetDeadReckonParameters(double aDeadReckonInterval            = 1.0,
                                double aMaxDeadReckonError            = 5.0,
                                double aMaxDeadReckonOrientationError = 3.0 * UtMath::cRAD_PER_DEG);

   void Initialized() override;

   int          GetPlatformIndex() const { return mPlatformIndex; }
   unsigned int GetInfoType() const { return mInfoType; }
   double       GetDeadReckonInterval() const { return mDeadReckonInterval; }
   double       GetMaxDeadReckonError() const { return mMaxDeadReckonError; }
   double       GetMaxDeadReckonOrientationError() const { return mMaxDeadReckonOrientationError; }

protected:
   int          mPlatformIndex;
   unsigned int mInfoType;
   bool         mUseDeadReckonDefaults;
   double       mDeadReckonInterval;
   double       mMaxDeadReckonError;
   double       mMaxDeadReckonOrientationError;
};
#endif
