// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_EVENTRESULTS_HPP
#define WSFXIO_EVENTRESULTS_HPP

#include "wsf_export.h"

#include <ostream>

#include "WsfEventResult.hpp"
class WsfXIO_Connection;

namespace wsf
{
namespace event
{

// ===================================================================================================
class WSF_EXPORT HandleConnect : public Result
{
public:
   HandleConnect(double aSimTime, WsfXIO_Connection* aConnectionPtr, const Settings& aSettings)
      : Result(aSimTime, aSettings)
      , mConnectionPtr(aConnectionPtr)
   {
   }
   ~HandleConnect() override = default;

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

private:
   WsfXIO_Connection* mConnectionPtr;
};

// ===================================================================================================
class WSF_EXPORT HandleDisconnect : public Result
{
public:
   HandleDisconnect(double aSimTime, WsfXIO_Connection* aConnectionPtr, const Settings& aSettings)
      : Result(aSimTime, aSettings)
      , mConnectionPtr(aConnectionPtr)
   {
   }
   ~HandleDisconnect() override = default;

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

private:
   WsfXIO_Connection* mConnectionPtr;
};

// ===================================================================================================
class WSF_EXPORT HandleBandwidthData : public Result
{
public:
   HandleBandwidthData(double          aSimTime,
                       int             aTotalSentBytes,
                       int             aDeltaSentBytes,
                       int             aTotalRecvBytes,
                       int             aDeltaRecvBytes,
                       double          aDeltaTime,
                       const Settings& aSettings)
      : Result(aSimTime, aSettings)
      , mTotalSentBytes(aTotalSentBytes)
      , mDeltaSentBytes(aDeltaSentBytes)
      , mTotalRecvBytes(aTotalRecvBytes)
      , mDeltaRecvBytes(aDeltaRecvBytes)
      , mDeltaTime(aDeltaTime)
   {
   }
   ~HandleBandwidthData() override = default;

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

private:
   int    mTotalSentBytes;
   int    mDeltaSentBytes;
   int    mTotalRecvBytes;
   int    mDeltaRecvBytes;
   double mDeltaTime;
};

} // namespace event
} // namespace wsf

#endif
