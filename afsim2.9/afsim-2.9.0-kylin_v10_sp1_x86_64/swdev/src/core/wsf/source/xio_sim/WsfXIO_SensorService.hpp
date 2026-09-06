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

#ifndef WSFXIO_SENSORSERVICE_HPP
#define WSFXIO_SENSORSERVICE_HPP

#include "wsf_export.h"

class WsfSensor;
class WsfSensorMode;
class WsfSimulation;
class WsfTrack;
#include "WsfStringId.hpp"
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_RequestSensorDataPkt;
#include "xio_sim/WsfXIO_SimService.hpp"
class WsfXIO_Simulation;

//! Provides extended information about WSF sensors:
//!   * Tracking Requests
class WSF_EXPORT WsfXIO_SensorService : public WsfXIO_ServiceT<WsfXIO_RequestSensorDataPkt, WsfXIO_SimService>
{
public:
   typedef WsfXIO_ServiceT<WsfXIO_RequestSensorDataPkt, WsfXIO_SimService> BaseClassType;

   WsfXIO_SensorService(WsfXIO_Simulation& aXIO_Simulation);

protected:
   void RequireCallbacks(bool aRequired);
   void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, WsfXIO_RequestSensorDataPkt& aRequestPkt) override;

   void HandleRequest(WsfXIO_RequestSensorDataPkt& aPkt);

   void RequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);
   void RequestUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void SensorTurnedOff(double aSimTime, WsfSensor* aSenosrPtr);

   WsfSimulation&   mSimulation;
   UtCallbackHolder mRequestCallbacks;
   //! The number of requests for sensor request data.
   int mRequestCount;

   typedef std::multimap<WsfSensor*, WsfXIO_Subscription*> SensorToRequestMap;
   SensorToRequestMap                                      mSensorToRequests;
};
#endif
