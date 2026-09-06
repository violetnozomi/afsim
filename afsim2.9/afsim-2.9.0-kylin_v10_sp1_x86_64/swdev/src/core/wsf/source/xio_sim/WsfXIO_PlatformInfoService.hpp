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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFXIO_PLATFORMINFOSERVICE_HPP
#define WSFXIO_PLATFORMINFOSERVICE_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfEvent.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"
class WsfArticulatedPart;
class WsfMover;
class WsfObject;
class WsfPlatformPart;
class WsfProcessor;
class WsfSensor;
class WsfSensorMode;
class WsfSimulation;
#include "WsfStringId.hpp"
#include "xio/WsfXIO.hpp"
#include "xio/WsfXIO_Destination.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_PartStateChangePkt;
class WsfXIO_PlatformInfoRequest;
class WsfXIO_ProcessorInfo;
class WsfXIO_ProcessorInfoPkt;
class WsfXIO_RequestPlatformInfoPkt;
#include "xio_sim/WsfXIO_SimService.hpp"
class WsfXIO_Simulation;
class WsfXIO_TypeQueryPkt;
class WsfXIO_TypeListQueryPkt;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

//! Provides information about a specific platform and the platform parts attached to that platform
//! Processes:                                  Responds with:
//! WsfXIO_TypeQueryPkt                         WsfXIO_TypeQueryResolutionPkt
//! WsfXIO_RequestPlatformInfoPkt               WsfXIO_PlatformPartInfoPkt
//!                                             WsfXIO_PlatformCommandChainPkt
//!                                             WsfXIO_EntityStatePkt
//!                                             WsfXIO_PartStateChangePkt
//! WsfXIO_TypeListQueryPkt                     WsfXIO_TypeListQueryResolutionPkt
class WSF_EXPORT WsfXIO_PlatformInfoService : public WsfXIO_ServiceT<WsfXIO_RequestPlatformInfoPkt, WsfXIO_SimService>
{
protected:
   class EvaluateDeadReckoningEvent;

public:
   friend class EvaluateDeadReckoningEvent;

   using BaseClassType = WsfXIO_ServiceT<WsfXIO_RequestPlatformInfoPkt, WsfXIO_SimService>;

   WsfXIO_PlatformInfoService(WsfXIO_Simulation& aXIO_Simulation);
   ~WsfXIO_PlatformInfoService() override = default;

   void PartDebugChange(WsfPlatformPart* aPartPtr);

   void SendPartState(const WsfPlatformPart& aPart, int aInfoMask);

protected:
   void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, WsfXIO_RequestPlatformInfoPkt& aRequestPkt) override;

   void UpdateCallbacks();

   void HandleRequest(WsfXIO_RequestPlatformInfoPkt& aPkt);

   void ProcessorStateChange(double aSimTime, WsfProcessor* aProcessorPtr);

   void SensorStateChange(double aSimTime, WsfSensor* aSensorPtr);
   void SensorModeChange(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void CommStateChange(double aSimTime, wsf::comm::Comm* aCommPtr);

   void MoverChanged(WsfPlatform* aPlatformPtr);
   void RouteChanged(WsfMover* aMoverPtr);

   void TrackManagerStateChange(double aSimTime, WsfPlatform* aPlatformPtr);

   void FillPartState(const WsfPlatformPart& aPart, WsfXIO_PartStateChangePkt& aPkt);

   void SendCommandChain(WsfXIO_Subscription& aSubscription, WsfPlatform& aPlatform);

   void SendTrackManagerState(WsfPlatform& aPlatform);

   WsfObject* FindObjectType(WsfXIO_TypeQueryPkt& aPkt);
   void       HandleTypeQuery(WsfXIO_TypeQueryPkt& aPkt);
   void       HandleTypeListQuery(WsfXIO_TypeListQueryPkt& aPkt);

   void DR_Change(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDRData);

   // Represents a request for platform information
   struct PlatformInfo
   {
      int                  mPlatformIndex;
      int                  mInfoTypes;
      WsfXIO_Subscription* mSubscriptionPtr;
   };

   //! Destination for DR updates for a single platform.
   struct DR_Destination
   {
      WsfXIO_Destination mDestination;
   };

   // Event to evaluate dead reckoning at an interval given by the requester.
   // Note that this is NOT used when using the default DR thresholds / update rates.
   class EvaluateDeadReckoningEvent : public WsfEvent
   {
   public:
      EvaluateDeadReckoningEvent() = default;
      EvaluateDeadReckoningEvent(double                      aSimTime,
                                 WsfXIO_PlatformInfoService* aServicePtr,
                                 WsfXIO_SubscriptionId       aSubscription,
                                 WsfPlatform*                aPlatformPtr,
                                 double                      aInterval,
                                 double                      aMaxError,
                                 double                      aMaxOrientationError);
      EventDisposition Execute() override;

      void SendState(WsfXIO_Subscription* aSubscriptionPtr);

      void CopyState(WsfPlatform* aPlatformPtr);

      WsfXIO_PlatformInfoService* mServicePtr;
      WsfXIO_SubscriptionId       mSubscriptionId;
      size_t                      mPlatformIndex;
      double                      mInterval;
      double                      mMaxError;
      double                      mMaxOrientationError;
      UtVec3d                     mLocation;
      UtVec3d                     mVelocity;
      UtVec3d                     mAcceleration;
      double                      mPsi;
      double                      mTheta;
      double                      mPhi;
      double                      mRootTime;
   };

   PlatformInfo* FindPlatformInfo(size_t aPlatformIndex);

   using PlatformInfoMap = std::multimap<size_t, PlatformInfo>;

   void GetInfoRange(size_t aIndex, PlatformInfoMap::iterator& aBegin, PlatformInfoMap::iterator& aEnd);

   using iterator  = PlatformInfoMap::iterator;
   using ValuePair = PlatformInfoMap::value_type;

   WsfXIO_Simulation& mXIO_Simulation;
   WsfSimulation&     mSimulation;
   const WsfScenario& mScenario;
   WsfExtInterface*   mExtInterfacePtr{nullptr};
   // Map of platform index to requested data in PlatformInfo
   PlatformInfoMap mPlatformInfo;
   //! Array of destinations receiving DR data for each platform index.
   std::vector<DR_Destination*> mDR_Destinations;
   //! Callback for WsfExtEntityDeadReckon::DeadReckonChanged
   std::unique_ptr<UtCallback> mDR_MoverUpdatedCallbackPtr;
   //! List of callbacks only required while we have subscriptions
   UtCallbackHolder mActiveCallbacks;
};

//! Request for platform part information
class WSF_EXPORT WsfXIO_PlatformPartInfoRequest : public WsfXIO_Request
{
public:
   WsfXIO_PlatformPartInfoRequest(int aPlatformIndex, int aPartType, WsfStringId aPartName, WsfXIO_Connection* aConnectionPtr);
   void SetUpdateInterval(double aUpdateInterval, bool aRealTimeInterval = false)
   {
      mUpdateInterval   = aUpdateInterval;
      mRealtimeInterval = aRealTimeInterval;
   }

   void Initialized() override;

   int GetPlatformIndex() const { return mPlatformIndex; }

   WsfStringId GetPartName() const { return mPartName; }

protected:
   int         mPlatformIndex;
   int         mPartType;
   WsfStringId mPartName;
   double      mUpdateInterval;
   bool        mRealtimeInterval;
};

class WSF_EXPORT WsfXIO_PlatformPartInfoService :
   public WsfXIO_ServiceT<WsfXIO_RequestPlatformPartInfoPkt, WsfXIO_SimService>
{
public:
   using BaseClassType = WsfXIO_ServiceT<WsfXIO_RequestPlatformPartInfoPkt, WsfXIO_SimService>;
   WsfXIO_PlatformPartInfoService(WsfXIO_Simulation& aXIO_Simulation);
   ~WsfXIO_PlatformPartInfoService() override;

protected:
   void SubscriptionCancelledT(WsfXIO_Subscription*               aSubscriptionPtr,
                               WsfXIO_RequestPlatformPartInfoPkt& aRequestPkt) override;

   void HandleRequest(WsfXIO_RequestPlatformPartInfoPkt& aPkt);

   friend class UpdatePartEvent;

   class UpdatePartEvent : public WsfEvent
   {
   public:
      UpdatePartEvent(double                          aSimTime,
                      WsfXIO_PlatformPartInfoService* aServicePtr,
                      WsfArticulatedPart*             aPartPtr,
                      double                          aInterval);
      EventDisposition Execute() override;

      void SendState();
      void CopyState();

      WsfXIO_PlatformPartInfoService* mServicePtr;
      WsfArticulatedPart*             mPartPtr;
      WsfXIO_Destination              mDestination;
      double                          mInterval;
      WsfXIO_PlatformPartStatePkt     mStateData;
      bool                            mDisabled;
   };

   WsfXIO_Simulation& mXIO_Simulation;
   WsfSimulation&     mSimulation;

   using EventMap = std::map<WsfPlatformPart*, UpdatePartEvent*>;
   EventMap mEvents;
};

#endif
