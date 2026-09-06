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

#ifndef WSFXIO_SIMTIMESERVICE_HPP
#define WSFXIO_SIMTIMESERVICE_HPP

#include "wsf_export.h"

class UtWallClock;
class WsfClockSource;
class WsfSimulation;
class WsfXIO_Connection;
class WsfXIO_Interface;
#include "WsfEvent.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;

//! Provides other applications with information regarding this application's simulation clock.
class WSF_EXPORT WsfXIO_SimTimeService : public WsfXIO_Service
{
   typedef WsfXIO_Service BaseType;

public:
   WsfXIO_SimTimeService(WsfXIO_Simulation& aXIO_Simulation);
   ~WsfXIO_SimTimeService() override;

   bool UpdateRequestInterval(double aSimTime, WsfXIO_SubscriptionId aSubscriptionId);

protected:
   void HandlePause();
   void HandleResume();
   void HandleClockRateChange(double aClockRate);

   void HandleRequest(WsfXIO_RequestSimTimePkt& aPkt);
   void SetTimeData(WsfXIO_SimTimePkt& aPkt);
   void SendTimeStateToAll(WsfXIO_SimTimePkt::SimulationState aState);

   void SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr) override;

   class UpdateEvent : public WsfEvent
   {
   public:
      UpdateEvent(WsfXIO_SimTimeService* aServicePtr,
                  WsfXIO_SubscriptionId  aSubscriptionId,
                  double                 aInterval,
                  bool                   aIsAdjusted,
                  bool                   aIsWallClockEvent);

      UpdateEvent(const UpdateEvent& aSrc);

      void             Disable() { mIsActive = false; }
      EventDisposition Execute() override;

      bool                   mIsActive;
      bool                   mIsAdjusted;
      bool                   mIsWallClockEvent;
      double                 mInterval;
      WsfXIO_SimTimeService* mServicePtr;
      WsfXIO_SubscriptionId  mSubscriptionId;
      double                 mRate;
   };

   struct SubscriptionData
   {
      WsfXIO_SubscriptionId mSubscriptionId;
      UpdateEvent*          mEventPtr;
   };

   std::vector<SubscriptionData> mSubscriptionData;
   WsfSimulation&                mSimulation;
};
#endif
