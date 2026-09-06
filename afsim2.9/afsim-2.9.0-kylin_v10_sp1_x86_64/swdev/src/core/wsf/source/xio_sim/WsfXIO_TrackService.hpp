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

#ifndef WSFXIO_TRACKSERVICE_HPP
#define WSFXIO_TRACKSERVICE_HPP

#include "wsf_export.h"

#include <map>
#include <set>
#include <vector>

#include "GenUniqueId.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
class WsfPlatform;
class WsfSensor;
class WsfSimulation;
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfTrackManager;
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
class WsfXIO_ExtendedTrackInfoPkt;
class WsfXIO_ExtendedTrackInfoRequestPkt;
class WsfXIO_RequestTracksPkt;
#include "xio_sim/WsfXIO_SimService.hpp"
class WsfXIO_Simulation;

//! Sends track information to fulfill the WsfXIO_TrackRequests
class WSF_EXPORT WsfXIO_TrackService : public WsfXIO_ServiceT<WsfXIO_RequestTracksPkt, WsfXIO_SimService>
{
public:
   using BaseClassType = WsfXIO_ServiceT<WsfXIO_RequestTracksPkt, WsfXIO_SimService>;

   WsfXIO_TrackService(WsfXIO_Simulation& aXIO_Simulation);

   void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, WsfXIO_RequestTracksPkt& aRequest) override;

private:
   void SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);
   void SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);
   // void SensorTrackInitiated(double          aSimTime,
   //                           WsfSensor*      aSensorPtr,
   //                           const WsfTrack* aTrackPtr);
   void SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void UpdateSensorCallbacks();

   // Services requests for track data which do have an update interval.
   // Sends track data on a regular interval, possibly reducing bandwidth.
   class UpdateSendEvent : public WsfEvent
   {
   public:
      UpdateSendEvent(WsfXIO_Subscription* aSubscriptionPtr, WsfPlatform* aPlatformPtr);
      ~UpdateSendEvent() override = default;
      EventDisposition Execute() override;

      void Disable();

      using TrackIdSet = std::set<WsfTrackId>;

      double               mUpdateInterval;
      bool                 mEnabled{true};
      int                  mRequiredData{0};
      WsfXIO_Subscription* mSubscriptionPtr;
      WsfPlatform*         mPlatformPtr;
      WsfTrackManager*     mTrackManagerPtr;

      TrackIdSet mLocalUpdateSet;
      TrackIdSet mLocalDropSet;
      TrackIdSet mRawUpdateSet;
      TrackIdSet mRawDropSet;

      UtCallbackHolder mCallbacks;
   };

   // Services requests for track updates which do not have an update interval.
   // Sends track data immediately upon update.
   class PlatformObserver : public WsfXIO_Destination
   {
   public:
      PlatformObserver(WsfPlatform* aPlatformPtr);

      int              mRequiredData;
      WsfPlatform*     mPlatformPtr;
      WsfTrackManager* mTrackManagerPtr;
      UtCallbackHolder mCallbacks;
   };

   class SensorObserver : public WsfXIO_Destination
   {
   public:
      int        mRequiredData;
      WsfSensor* mSensorPtr;
   };

   PlatformObserver* FindPlatformObserver(int aPlatformIndex);

   void HandleRequest(WsfXIO_RequestTracksPkt& aPkt);
   void HandleExtendedTrackInfoRequest(WsfXIO_ExtendedTrackInfoRequestPkt& aPkt);

   void SelectSensorTracks(WsfXIO_ExtendedTrackInfoPkt& aPkt,
                           const WsfTrackId&            aOtherTrackId,
                           std::set<WsfTrackId>&        aSelectedTrackIds,
                           std::set<WsfTrackId>&        aSearchedTrackIds);

   using PlatformObserverMap = std::map<int, PlatformObserver*>;
   using SensorObserverMap   = std::map<WsfSensor*, SensorObserver*>;

   WsfSimulation&      mSimulation;
   UtCallbackHolder    mCallbacks;
   UtCallbackHolder    mSensorCallbacks;
   PlatformObserverMap mPlatformObservers;
   SensorObserverMap   mSensorObservers;
   //! List of all active UpdateSendEvents.
   std::vector<UpdateSendEvent*> mUpdateSendEvents;
};

#endif
