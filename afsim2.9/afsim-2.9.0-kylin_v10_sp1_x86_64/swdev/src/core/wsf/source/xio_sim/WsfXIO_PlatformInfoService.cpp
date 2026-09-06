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

#include "xio_sim/WsfXIO_PlatformInfoService.hpp"

#include "WsfCommTypes.hpp"
#include "WsfCommandChain.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfFuelTypes.hpp"
#include "WsfFusionStrategy.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRouteMover.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "ext/WsfExtInterface.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_ObjectInfoLoad.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_PlatformInfoService::WsfXIO_PlatformInfoService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseClassType(aXIO_Simulation.GetInterface())
   , mXIO_Simulation(aXIO_Simulation)
   , mSimulation(aXIO_Simulation.GetSimulation())
   , mScenario(aXIO_Simulation.GetSimulation().GetScenario())
   , mExtInterfacePtr(nullptr)
   , mDR_MoverUpdatedCallbackPtr(nullptr)
{
   mExtInterfacePtr = WsfExtInterface::Find(mSimulation);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_PlatformInfoService::HandleRequest, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_PlatformInfoService::HandleTypeListQuery, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_PlatformInfoService::HandleTypeQuery, this);
}

// Updated the callbacks required to service subscribers
void WsfXIO_PlatformInfoService::UpdateCallbacks()
{
   if (mPlatformInfo.empty())
   {
      mActiveCallbacks.Clear();
   }
   else if (mActiveCallbacks.Empty())
   {
      mActiveCallbacks.Add(
         WsfObserver::ProcessorTurnedOn(&mSimulation).Connect(&WsfXIO_PlatformInfoService::ProcessorStateChange, this));
      mActiveCallbacks.Add(
         WsfObserver::ProcessorTurnedOff(&mSimulation).Connect(&WsfXIO_PlatformInfoService::ProcessorStateChange, this));

      mActiveCallbacks.Add(
         WsfObserver::SensorTurnedOn(&mSimulation).Connect(&WsfXIO_PlatformInfoService::SensorStateChange, this));
      mActiveCallbacks.Add(
         WsfObserver::SensorTurnedOff(&mSimulation).Connect(&WsfXIO_PlatformInfoService::SensorStateChange, this));
      mActiveCallbacks.Add(
         WsfObserver::SensorModeActivated(&mSimulation).Connect(&WsfXIO_PlatformInfoService::SensorModeChange, this));
      mActiveCallbacks.Add(
         WsfObserver::SensorModeDeactivated(&mSimulation).Connect(&WsfXIO_PlatformInfoService::SensorModeChange, this));

      mActiveCallbacks.Add(
         WsfObserver::CommTurnedOn(&mSimulation).Connect(&WsfXIO_PlatformInfoService::CommStateChange, this));
      mActiveCallbacks.Add(
         WsfObserver::CommTurnedOff(&mSimulation).Connect(&WsfXIO_PlatformInfoService::CommStateChange, this));
      mActiveCallbacks.Add(WsfObserver::CorrelationStrategyChanged(&mSimulation)
                              .Connect(&WsfXIO_PlatformInfoService::TrackManagerStateChange, this));
      mActiveCallbacks.Add(
         WsfObserver::FusionStrategyChanged(&mSimulation).Connect(&WsfXIO_PlatformInfoService::TrackManagerStateChange, this));

      mActiveCallbacks.Add(WsfObserver::RouteChanged(&mSimulation).Connect(&WsfXIO_PlatformInfoService::RouteChanged, this));
      mActiveCallbacks.Add(WsfObserver::MoverChanged(&mSimulation).Connect(&WsfXIO_PlatformInfoService::MoverChanged, this));

      // Tell any components to register their simulation observers.
      for (auto componentPtr : mXIO_Simulation)
      {
         componentPtr->RegisterSimulationObservers(mSimulation, mActiveCallbacks);
      }
   }

   if (!mDR_Destinations.empty())
   {
      bool empty = true;
      for (auto destPtr : mDR_Destinations)
      {
         if (destPtr != nullptr)
         {
            empty = false;
            break;
         }
      }
      if (empty)
      {
         mDR_Destinations.clear();
      }
   }
   if (mDR_Destinations.empty())
   {
      mDR_MoverUpdatedCallbackPtr.reset();
   }
   else if (mDR_MoverUpdatedCallbackPtr == nullptr)
   {
      mExtInterfacePtr->GetEntityDeadReckon()->RequireExternalEntities(true);
      // WsfExtInterface::GetInstance().GetEntityDeadReckon()->RequestService(true);
      mExtInterfacePtr->GetEntityDeadReckon()->UseInitialDistributionInterval();
      mExtInterfacePtr->GetEntityDeadReckon()->RequestService(false);
      mDR_MoverUpdatedCallbackPtr =
         mExtInterfacePtr->GetEntityDeadReckon()->DeadReckonChanged.Connect(&WsfXIO_PlatformInfoService::DR_Change, this);
   }
}

void WsfXIO_PlatformInfoService::HandleRequest(WsfXIO_RequestPlatformInfoPkt& aPkt)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      AddRequiredPlatform(*platformPtr, subscriptionPtr);
      PlatformInfo info;
      info.mPlatformIndex   = aPkt.mPlatformIndex;
      info.mInfoTypes       = aPkt.mInfoTypes;
      info.mSubscriptionPtr = subscriptionPtr;

      if (aPkt.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cPLATFORM_INFO)
      {
         bool                       sendRoute = ((aPkt.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cROUTE_INFO) != 0);
         WsfXIO_PlatformPartInfoPkt parts;
         parts.mPlatformInfoPtr = new WsfXIO_PlatformInfo;
         WsfXIO_ObjectInfoLoad::Load_Platform(mXIO_Simulation, *parts.mPlatformInfoPtr, *platformPtr);
         if (sendRoute)
         {
            WsfMover* moverPtr = platformPtr->GetMover();
            if (moverPtr != nullptr)
            {
               parts.mPlatformInfoPtr->mRoutePtr            = const_cast<WsfRoute*>(moverPtr->GetRoute());
               parts.mPlatformInfoPtr->mTargetWaypointIndex = 0;
               auto routeMoverPtr                           = dynamic_cast<WsfRouteMover*>(moverPtr);
               if (routeMoverPtr != nullptr)
               {
                  parts.mPlatformInfoPtr->mTargetWaypointIndex = routeMoverPtr->GetTargetIndex();
               }
            }
         }
         parts.mRequestId = subscriptionPtr->GetRequestId();
         subscriptionPtr->SendResponse(parts);
      }

      if ((aPkt.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cCOMMAND_CHAIN_INFO) != 0)
      {
         SendCommandChain(*subscriptionPtr, *platformPtr);
      }
      bool forceUpdate = false;
      if ((aPkt.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cENTITY_STATE_INFO) != 0)
      {
         // There are currently 2 ways of sending out the dead-reckon data.
         // If we want the default behavior, we send the updates out at the rate configured
         // in WsfExtEntityDeadReckon (same as DIS).
         // If some other rates are specified, we have to make a special event to provide the update.
         if (aPkt.mUseDeadReckonDefaults)
         {
            if ((int)mDR_Destinations.size() <= aPkt.mPlatformIndex)
            {
               mDR_Destinations.resize(aPkt.mPlatformIndex + 1, (DR_Destination*)nullptr);
            }
            DR_Destination*& destPtr = mDR_Destinations[aPkt.mPlatformIndex];
            if (destPtr == nullptr)
            {
               destPtr = new DR_Destination;
            }
            destPtr->mDestination.Add(subscriptionPtr->GetDataConnection());
            forceUpdate = true;
         }
         else
         {
            auto drEventPtr = ut::make_unique<EvaluateDeadReckoningEvent>(mSimulation.GetSimTime(),
                                                                          this,
                                                                          subscriptionPtr->GetId(),
                                                                          platformPtr,
                                                                          aPkt.mDeadReckonInterval,
                                                                          aPkt.mMaxDeadReckonError,
                                                                          aPkt.mMaxDeadReckonOrientationError);
            drEventPtr->SendState(subscriptionPtr);
            mSimulation.AddEvent(std::move(drEventPtr));
         }
      }

      mPlatformInfo.insert(ValuePair(aPkt.mPlatformIndex, info));
      UpdateCallbacks();

      // For an update to send out the initial state.  Send reliably.
      if (forceUpdate)
      {
         EvaluateDeadReckoningEvent evt;
         evt.mPlatformIndex = static_cast<int>(platformPtr->GetIndex());
         evt.CopyState(platformPtr);
         evt.SetTime(mSimulation.GetSimTime());
         evt.SendState(subscriptionPtr);
      }
   }
   else
   {
      DenyRequest(aPkt);
   }
}

void WsfXIO_PlatformInfoService::SubscriptionCancelledT(WsfXIO_Subscription*           aSubscriptionPtr,
                                                        WsfXIO_RequestPlatformInfoPkt& aRequestPkt)
{
   auto end = mPlatformInfo.upper_bound(aRequestPkt.mPlatformIndex);
   for (auto i = mPlatformInfo.lower_bound(aRequestPkt.mPlatformIndex); i != end; ++i)
   {
      if (i->second.mSubscriptionPtr == aSubscriptionPtr)
      {
         if (i->second.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cENTITY_STATE_INFO)
         {
            if ((int)mDR_Destinations.size() > i->second.mPlatformIndex)
            {
               DR_Destination*& dest = mDR_Destinations[i->second.mPlatformIndex];
               if (dest != nullptr)
               {
                  dest->mDestination.Remove(aSubscriptionPtr->GetDataConnection());
                  if (dest->mDestination.IsEmpty())
                  {
                     delete dest;
                     dest = nullptr;
                  }
               }
            }
         }
         mPlatformInfo.erase(i);
         break;
      }
   }
   BaseClassType::SubscriptionCancelledT(aSubscriptionPtr, aRequestPkt);
   UpdateCallbacks();
}

WsfXIO_PlatformInfoService::PlatformInfo* WsfXIO_PlatformInfoService::FindPlatformInfo(size_t aPlatformIndex)
{
   PlatformInfo* infoPtr = nullptr;
   auto          i       = mPlatformInfo.find(aPlatformIndex);
   if (i != mPlatformInfo.end())
   {
      infoPtr = &i->second;
   }
   return infoPtr;
}

void WsfXIO_PlatformInfoService::ProcessorStateChange(double aSimTime, WsfProcessor* aProcessorPtr)
{
   SendPartState(*aProcessorPtr, WsfXIO_RequestPlatformInfoPkt::cPROCESSOR_INFO);
}

void WsfXIO_PlatformInfoService::SensorStateChange(double aSimTime, WsfSensor* aSensorPtr)
{
   SendPartState(*aSensorPtr, WsfXIO_RequestPlatformInfoPkt::cSENSOR_INFO);
}

void WsfXIO_PlatformInfoService::SensorModeChange(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aSensorModePtr)
{
   SendPartState(*aSensorPtr, WsfXIO_RequestPlatformInfoPkt::cSENSOR_INFO);
}

void WsfXIO_PlatformInfoService::CommStateChange(double aSimTime, wsf::comm::Comm* aCommPtr)
{
   SendPartState(*aCommPtr, WsfXIO_RequestPlatformInfoPkt::cCOMM_INFO);
}


void WsfXIO_PlatformInfoService::PartDebugChange(WsfPlatformPart* aPartPtr)
{
   SendPartState(*aPartPtr, WsfXIO_RequestPlatformInfoPkt::cPART_INFO);
}

void WsfXIO_PlatformInfoService::TrackManagerStateChange(double aSimTime, WsfPlatform* aPlaformPtr)
{
   SendTrackManagerState(*aPlaformPtr);
}


void WsfXIO_PlatformInfoService::SendPartState(const WsfPlatformPart& aPart, int aInfoMask)
{
   int  index = static_cast<int>(aPart.GetPlatform()->GetIndex());
   auto end   = mPlatformInfo.upper_bound(index);
   auto i     = mPlatformInfo.lower_bound(index);
   if (i != end)
   {
      WsfXIO_PartStateChangePkt pkt;
      pkt.mPlatformIndex = index;
      FillPartState(aPart, pkt);
      PlatformInfo& info = i->second;
      for (; i != end; ++i)
      {
         if (i->second.mInfoTypes & aInfoMask)
         {
            pkt.mRequestId = info.mSubscriptionPtr->GetRequestId();
            i->second.mSubscriptionPtr->SendResponse(pkt);
         }
      }
   }
}

void WsfXIO_PlatformInfoService::FillPartState(const WsfPlatformPart& aPart, WsfXIO_PartStateChangePkt& aPkt)
{
   aPkt.mIsTurnedOn        = aPart.IsTurnedOn();
   aPkt.mPartName          = aPart.GetNameId();
   aPkt.mPartType          = aPart.GetPartType();
   aPkt.mDebugOn           = aPart.DebugEnabled();
   aPkt.mQuantityRemaining = 0.0;
   if (aPkt.mPartType == cCOMPONENT_ROLE<WsfSensor>())
   {
      WsfSensor& sensor = ((WsfSensor&)aPart);
      for (size_t i = 0; i < sensor.GetModeCount(); ++i)
      {
         if (sensor.GetModeEntry(i)->IsSelected())
         {
            aPkt.mSelectedModes.push_back(sensor.GetModeEntry(i)->GetNameId());
         }
      }
      aPkt.mCurrentModeName = sensor.GetCurrentModeName();
   }
   else
   {
      // Give components a chance.
      const WsfXIO_ComponentList& components(mXIO_Simulation);
      for (WsfXIO_ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
      {
         if ((*i)->FillPartState(aPart, aPkt))
         {
            break;
         }
      }
   }
}

void WsfXIO_PlatformInfoService::SendCommandChain(WsfXIO_Subscription& aSubscription, WsfPlatform& aPlatform)
{
   WsfXIO_PlatformCommandChainPkt pkt;

   for (WsfComponentList::RoleIterator<WsfCommandChain> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      WsfXIO_CommandChain chain;
      WsfCommandChain*    chainPtr = *iter;
      if (!chainPtr)
      {
         continue;
      }

      // a platform can possibly not have a commander.
      if (chainPtr->GetCommander() != nullptr)
      {
         chain.mCommander = chainPtr->GetCommander()->GetNameId();
      }
      else
      {
         chain.mCommander = WsfStringId(nullptr); // this is how WsfCommandChain initializes commander.
      }
      chain.mChainName                            = chainPtr->GetNameId();
      WsfCommandChain::PlatformList& subordinates = chainPtr->GetSubordinates();
      for (WsfPlatform* subPtr : subordinates)
      {
         chain.mSubordinates.push_back(subPtr->GetNameId());
      }
      pkt.mCommandChains.push_back(chain);
   }
   aSubscription.SendResponse(pkt);
}

WsfObject* WsfXIO_PlatformInfoService::FindObjectType(WsfXIO_TypeQueryPkt& aPkt)
{
   WsfObject* objectPtr = nullptr;
   if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM)
   {
      objectPtr = mSimulation.GetPlatformByName(aPkt.mRequestedTypeName);
   }
   else if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_TYPE)
   {
      objectPtr = mScenario.FindType("platform_type", aPkt.mRequestedTypeName.GetString());
   }
   else if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPART_TYPE)
   {
      if (aPkt.mPartType == cCOMPONENT_ROLE<WsfProcessor>())
      {
         objectPtr = WsfProcessorTypes::Get(mScenario).Find(aPkt.mRequestedTypeName);
      }
      else if (aPkt.mPartType == cCOMPONENT_ROLE<wsf::comm::Comm>())
      {
         objectPtr = wsf::comm::Types::Get(mScenario).Find(aPkt.mRequestedTypeName);
      }
      else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfSensor>())
      {
         objectPtr = WsfSensorTypes::Get(mScenario).Find(aPkt.mRequestedTypeName);
      }
      else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfFuel>())
      {
         objectPtr = WsfFuelTypes::Get(mScenario).Find(aPkt.mRequestedTypeName);
      }
      else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfMover>())
      {
         objectPtr = WsfMoverTypes::Get(mScenario).Find(aPkt.mRequestedTypeName);
      }
      else
      {
         // Try components
         const WsfXIO_ComponentList& components(mXIO_Simulation);
         for (WsfXIO_ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
         {
            objectPtr = (*i)->FindPartType(aPkt.mPartType, aPkt.mRequestedTypeName);
            if (objectPtr != nullptr)
            {
               break;
            }
         }
      }
   }
   else if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_MEMBER ||
            aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_TYPE_MEMBER)
   {
      WsfPlatform* platformPtr = nullptr;
      if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_MEMBER)
      {
         platformPtr = mSimulation.GetPlatformByName(aPkt.mPlatformName);
      }
      else
      {
         platformPtr = (WsfPlatform*)mScenario.FindType("platform", aPkt.mPlatformName.GetString());
      }
      if (platformPtr != nullptr)
      {
         if (aPkt.mPartType == cCOMPONENT_ROLE<WsfProcessor>())
         {
            objectPtr = platformPtr->GetComponent<WsfProcessor>(aPkt.mRequestedTypeName);
         }
         else if (aPkt.mPartType == cCOMPONENT_ROLE<wsf::comm::Comm>())
         {
            objectPtr = platformPtr->GetComponent<wsf::comm::Comm>(aPkt.mRequestedTypeName);
         }
         else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfSensor>())
         {
            objectPtr = platformPtr->GetComponent<WsfSensor>(aPkt.mRequestedTypeName);
         }
         else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfFuel>())
         {
            objectPtr = platformPtr->GetComponent<WsfFuel>();
         }
         else if (aPkt.mPartType == cCOMPONENT_ROLE<WsfMover>())
         {
            objectPtr = platformPtr->GetMover();
         }
         else
         {
            // Try components
            const WsfXIO_ComponentList& components(mXIO_Simulation);
            for (WsfXIO_ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
            {
               objectPtr = (*i)->FindPart(*platformPtr, aPkt.mPartType, aPkt.mRequestedTypeName);
               if (objectPtr != nullptr)
               {
                  break;
               }
            }
         }
      }
   }
   return objectPtr;
}

void WsfXIO_PlatformInfoService::HandleTypeQuery(WsfXIO_TypeQueryPkt& aPkt)
{
   auto senderPtr = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
   if (senderPtr == nullptr)
   {
      return;
   }

   WsfXIO_TypeQueryResolutionPkt pkt;
   pkt.mResult          = wsf::xio::cQUERY_FALSE;
   pkt.mQueryId         = aPkt.mQueryId;
   pkt.mRequestedType   = aPkt.mRequestedTypeName;
   pkt.mObjectType      = WsfXIO_ObjectInfo::cNO_OBJECT;
   pkt.mObjectInfo      = nullptr;
   WsfObject* objectPtr = FindObjectType(aPkt);
   if (objectPtr != nullptr)
   {
      if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPART_TYPE ||
          aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_MEMBER ||
          aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_TYPE_MEMBER)
      {
         pkt.mObjectInfo = WsfXIO_ObjectInfoLoad::CreatePlatformPart(mXIO_Simulation, (WsfPlatformPart&)*objectPtr);
      }
      else if (aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM ||
               aPkt.mTypeLocation == WsfXIO_TypeQueryPkt::cPLATFORM_TYPE)
      {
         auto infoPtr = new WsfXIO_PlatformInfo;
         WsfXIO_ObjectInfoLoad::Load_Platform(mXIO_Simulation, *infoPtr, (WsfPlatform&)*objectPtr);
         pkt.mObjectInfo = infoPtr;
      }
      if (pkt.mObjectInfo != nullptr)
      {
         pkt.mObjectType = pkt.mObjectInfo->GetObjectType();
         pkt.mResult     = wsf::xio::cQUERY_TRUE;
      }
   }
   senderPtr->Send(pkt);
}

void WsfXIO_PlatformInfoService::HandleTypeListQuery(WsfXIO_TypeListQueryPkt& aPkt)
{
   auto senderPtr = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
   if (senderPtr == nullptr)
   {
      return;
   }
   if (aPkt.mRequestTypes == WsfXIO_TypeListQueryPkt::cSENSOR_TYPES)
   {
      WsfXIO_TypeListQueryResolutionPkt pkt;
      WsfObjectTypeListBase::TypeIdList types;
      WsfSensorTypes::Get(mScenario).GetTypeIds(types);
      pkt.mTypes.resize(types.size());
      for (size_t i = 0; i < types.size(); ++i)
      {
         pkt.mTypes[i] = types[i];
      }
      pkt.mQueryId  = aPkt.mQueryId;
      pkt.mResult   = wsf::xio::cQUERY_TRUE;
      pkt.mListType = WsfXIO_TypeListQueryPkt::cSENSOR_TYPES;
      senderPtr->Send(pkt);
   }
}

WsfXIO_PlatformInfoService::EvaluateDeadReckoningEvent::EvaluateDeadReckoningEvent(double                      aSimTime,
                                                                                   WsfXIO_PlatformInfoService* aServicePtr,
                                                                                   WsfXIO_SubscriptionId aSubscription,
                                                                                   WsfPlatform*          aPlatformPtr,
                                                                                   double                aInterval,
                                                                                   double                aMaxError,
                                                                                   double aMaxOrientationError)
   : mServicePtr(aServicePtr)
   , mSubscriptionId(aSubscription)
   , mPlatformIndex(aPlatformPtr->GetIndex())
   , mInterval(aInterval)
   , mMaxError(aMaxError)
   , mMaxOrientationError(aMaxOrientationError)
{
   SetTime(aSimTime);
   CopyState(aPlatformPtr);
}

WsfEvent::EventDisposition WsfXIO_PlatformInfoService::EvaluateDeadReckoningEvent::Execute()
{
   WsfXIO_Subscription* subscriptionPtr = mServicePtr->FindSubscription(mSubscriptionId);
   WsfPlatform*         platformPtr     = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (subscriptionPtr == nullptr || platformPtr == nullptr)
   {
      return cDELETE;
   }

   double  dt  = GetTime() - mRootTime;
   UtVec3d vel = mVelocity + mAcceleration * dt;
   UtVec3d pos = mLocation + mVelocity * dt + mAcceleration * (dt * dt / 2.0);
   platformPtr->Update(GetTime());
   UtVec3d actualPos;
   platformPtr->GetLocationWCS(actualPos.GetData());
   double positionError2 = (actualPos - pos).MagnitudeSquared();
   bool   doSend         = false;
   if (positionError2 > mMaxError * mMaxError)
   {
      doSend = true;
   }
   else
   {
      UtVec3d trueVelocity;
      platformPtr->GetVelocityWCS(trueVelocity.GetData());
      double cosDiff     = UtVec3d::DotProduct(trueVelocity, vel) / (vel.Magnitude() * trueVelocity.Magnitude());
      double cosMaxError = cos(mMaxOrientationError);
      if (cosDiff < cosMaxError)
      {
         doSend = true;
      }
   }

   if (doSend)
   {
      CopyState(platformPtr);
      SendState(subscriptionPtr);
   }
   SetTime(GetTime() + mInterval);
   return cRESCHEDULE;
}

void WsfXIO_PlatformInfoService::EvaluateDeadReckoningEvent::CopyState(WsfPlatform* aPlatformPtr)
{
   aPlatformPtr->GetLocationWCS(mLocation.GetData());
   aPlatformPtr->GetVelocityWCS(mVelocity.GetData());
   aPlatformPtr->GetAccelerationWCS(mAcceleration.GetData());
   aPlatformPtr->GetOrientationWCS(mPsi, mTheta, mPhi);
   mRootTime = GetTime();
}

void WsfXIO_PlatformInfoService::EvaluateDeadReckoningEvent::SendState(WsfXIO_Subscription* aSubscriptionPtr)
{
   WsfXIO_EntityStatePkt pkt;
   pkt.mPlatformIndex = static_cast<int>(mPlatformIndex);
   pkt.mSimTime       = GetTime();
   UtVec3d::Set(pkt.mAcceleration, mAcceleration.GetData());
   UtVec3d::Set(pkt.mVelocity, mVelocity.GetData());
   UtVec3d::Set(pkt.mLocation, mLocation.GetData());
   pkt.mOrientation[0] = mPsi;
   pkt.mOrientation[1] = mTheta;
   pkt.mOrientation[2] = mPhi;
   pkt.SetFlags(WsfXIO_Packet::cSYNCHRONIZED);
   aSubscriptionPtr->SendResponse(pkt);
}

void WsfXIO_PlatformInfoService::SendTrackManagerState(WsfPlatform& aPlatform)
{
   PlatformInfo* infoPtr = FindPlatformInfo(aPlatform.GetIndex());
   if (infoPtr != nullptr)
   {
      WsfXIO_TrackManagerStateChangePkt pkt;
      pkt.mPlatformIndex = static_cast<int>(aPlatform.GetIndex());

      WsfTrackManager& tmRef   = aPlatform.GetTrackManager();
      pkt.mFusionStrategy      = tmRef.GetFusionStrategy().GetType();
      pkt.mCorrelationStrategy = tmRef.GetCorrelationStrategy().GetType();

      pkt.mRequestId = infoPtr->mSubscriptionPtr->GetRequestId();
      infoPtr->mSubscriptionPtr->GetConnection().Send(pkt);
   }
}

void WsfXIO_PlatformInfoService::DR_Change(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDRData)
{
   size_t index = aDRData.mPlatformPtr->GetIndex();
   if (index < mDR_Destinations.size())
   {
      DR_Destination* destPtr = mDR_Destinations[index];
      if (destPtr != nullptr)
      {
         WsfXIO_EntityStateRawPkt pkt;
         for (int i = 0; i < 3; ++i)
         {
            pkt.mLocation[i]     = aDRData.mSentState.mLocationWCS[i];
            pkt.mAcceleration[i] = aDRData.mSentState.mAccelerationWCS[i];
            pkt.mVelocity[i]     = aDRData.mSentState.mVelocityWCS[i];
            pkt.mOrientation[i]  = aDRData.mSentState.mOrientationWCS[i];
         }
         pkt.mPlatformIndex = static_cast<int>(aDRData.mPlatformPtr->GetIndex());
         pkt.mSimTime       = aSimTime;
         destPtr->mDestination.Send(pkt);
      }
   }
}
void WsfXIO_PlatformInfoService::MoverChanged(WsfPlatform* aPlatformPtr)
{
   auto iter = mPlatformInfo.lower_bound(aPlatformPtr->GetIndex());
   auto last = mPlatformInfo.upper_bound(aPlatformPtr->GetIndex());
   for (; iter != last; ++iter)
   {
      if (iter->second.mInfoTypes & WsfXIO_RequestPlatformInfoPkt::cROUTE_INFO)
      {
         WsfXIO_RouteUpdatePkt pkt;
         pkt.mPlatformIndex       = static_cast<int>(aPlatformPtr->GetIndex());
         pkt.mRoutePtr            = nullptr;
         pkt.mTargetWaypointIndex = 0;
         WsfMover* moverPtr       = aPlatformPtr->GetMover();
         if (moverPtr != nullptr)
         {
            pkt.mRoutePtr      = const_cast<WsfRoute*>(moverPtr->GetRoute());
            auto routeMoverPtr = dynamic_cast<WsfRouteMover*>(moverPtr);
            if (routeMoverPtr != nullptr)
            {
               pkt.mTargetWaypointIndex = routeMoverPtr->GetTargetIndex();
            }
         }
         iter->second.mSubscriptionPtr->SendResponse(pkt);
      }
   }
}
void WsfXIO_PlatformInfoService::RouteChanged(WsfMover* aMoverPtr)
{
   MoverChanged(aMoverPtr->GetPlatform());
}

WsfXIO_PlatformPartInfoRequest::WsfXIO_PlatformPartInfoRequest(int                aPlatformIndex,
                                                               int                aPartType,
                                                               WsfStringId        aPartName,
                                                               WsfXIO_Connection* aConnectionPtr)
   : WsfXIO_Request(aConnectionPtr)
   , mPlatformIndex(aPlatformIndex)
   , mPartType(aPartType)
   , mPartName(aPartName)
   , mUpdateInterval(5.0)
   , mRealtimeInterval(false)
{
}

void WsfXIO_PlatformPartInfoRequest::Initialized()
{
   WsfXIO_RequestPlatformPartInfoPkt pkt;
   pkt.mPlatformIndex    = mPlatformIndex;
   pkt.mPartType         = mPartType;
   pkt.mPartName         = mPartName;
   pkt.mUpdateInterval   = mUpdateInterval;
   pkt.mRealtimeInterval = mRealtimeInterval;
   SendRequest(pkt);
}

WsfXIO_PlatformPartInfoService::WsfXIO_PlatformPartInfoService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseClassType(aXIO_Simulation.GetInterface())
   , mXIO_Simulation(aXIO_Simulation)
   , mSimulation(aXIO_Simulation.GetSimulation())
{
   mCallbacks.Add(GetInterface()->Connect(&WsfXIO_PlatformPartInfoService::HandleRequest, this));
   WsfXIO_SimService::Initialize(aXIO_Simulation.GetSimulation());
}

WsfXIO_PlatformPartInfoService::~WsfXIO_PlatformPartInfoService() {}

void WsfXIO_PlatformPartInfoService::SubscriptionCancelledT(WsfXIO_Subscription*               aSubscriptionPtr,
                                                            WsfXIO_RequestPlatformPartInfoPkt& aRequestPkt)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aRequestPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfComponent*    componentPtr = platformPtr->FindComponent(aRequestPkt.mPartName, aRequestPkt.mPartType);
      WsfPlatformPart* partPtr;
      if (componentPtr->QueryInterfaceT(partPtr))
      {
         auto i = mEvents.find(partPtr);
         if (i != mEvents.end())
         {
            UpdatePartEvent* eventPtr = i->second;
            eventPtr->mDestination.Remove(aSubscriptionPtr->GetDataConnection());
            if (eventPtr->mDestination.IsEmpty())
            {
               eventPtr->mDisabled = true;
               mEvents.erase(i);
            }
         }
      }
   }
}

void WsfXIO_PlatformPartInfoService::HandleRequest(WsfXIO_RequestPlatformPartInfoPkt& aPkt)
{
   bool         ok          = false;
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfComponent*    componentPtr = platformPtr->FindComponent(aPkt.mPartName, aPkt.mPartType);
      WsfPlatformPart* partPtr;
      componentPtr->QueryInterfaceT(partPtr);
      auto articulatedPartPtr = dynamic_cast<WsfArticulatedPart*>(partPtr);
      if (articulatedPartPtr != nullptr)
      {
         WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
         AddRequiredPlatform(*platformPtr, subscriptionPtr);

         UpdatePartEvent* eventPtr = nullptr;
         auto             iter     = mEvents.find(partPtr);
         if (iter == mEvents.end())
         {
            if (aPkt.mRealtimeInterval)
            {
               eventPtr = mSimulation.AddWallEventT<UpdatePartEvent>(mSimulation.GetWallTime(),
                                                                     this,
                                                                     articulatedPartPtr,
                                                                     aPkt.mUpdateInterval);
            }
            else
            {
               eventPtr = mSimulation.AddEventT<UpdatePartEvent>(mSimulation.GetSimTime(),
                                                                 this,
                                                                 articulatedPartPtr,
                                                                 aPkt.mUpdateInterval);
            }
            mEvents[partPtr] = eventPtr;
         }
         else
         {
            eventPtr            = iter->second;
            eventPtr->mInterval = std::min(eventPtr->mInterval, aPkt.mUpdateInterval);
         }
         eventPtr->mDestination.Add(subscriptionPtr->GetDataConnection());
         ok = true;
      }
   }
   if (!ok)
   {
      DenyRequest(aPkt);
   }
}

WsfXIO_PlatformPartInfoService::UpdatePartEvent::UpdatePartEvent(double                          aSimTime,
                                                                 WsfXIO_PlatformPartInfoService* aServicePtr,
                                                                 WsfArticulatedPart*             aPartPtr,
                                                                 double                          aInterval)
   : WsfEvent(aSimTime)
   , mServicePtr(aServicePtr)
   , mPartPtr(aPartPtr)
   , mInterval(aInterval)
   , mDisabled(false)
{
   mStateData.mPlatformIndex = static_cast<int32_t>(mPartPtr->GetPlatform()->GetIndex());
   mStateData.mPartType      = mPartPtr->GetPartType();
   mStateData.mPartName      = mPartPtr->GetName();
}

WsfEvent::EventDisposition WsfXIO_PlatformPartInfoService::UpdatePartEvent::Execute()
{
   if (mDisabled)
   {
      return cDELETE;
   }
   CopyState();
   SendState();
   SetTime(GetTime() + mInterval);
   return cRESCHEDULE;
}

void WsfXIO_PlatformPartInfoService::UpdatePartEvent::SendState()
{
   mDestination.Send(mStateData);
}

void WsfXIO_PlatformPartInfoService::UpdatePartEvent::CopyState()
{
   // Articulated part's GetYaw/Pitch/Roll functions are not always updated
   // Compute the yaw/pitch here.
   // Note: mSimulation.GetSimTime() is required as this may be a real-time event
   mPartPtr->UpdatePosition(GetSimulation()->GetSimTime());
   double ecsForward[] = {1.0, 0.0, 0.0};
   double pcsForward[3];
   mPartPtr->ConvertPCSVectorToECS(pcsForward, ecsForward);
   mStateData.mYaw                    = atan2(pcsForward[1], pcsForward[0]);
   mStateData.mPitch                  = -asin(pcsForward[2]);
   mStateData.mRoll                   = 0;
   mStateData.mIsExternallyControlled = mPartPtr->IsExternallyControlled();
   if (mPartPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>())
   {
      auto sensor = (WsfSensor*)mPartPtr;
      while (mStateData.mReceivers.size() > sensor->GetEM_RcvrCount())
      {
         delete mStateData.mReceivers.back();
         mStateData.mReceivers.pop_back();
      }
      while (mStateData.mReceivers.size() < sensor->GetEM_RcvrCount())
      {
         mStateData.mReceivers.push_back(new WsfXIO_EM_RcvrInfo);
      }
      for (unsigned int i = 0; i < sensor->GetEM_RcvrCount(); ++i)
      {
         WsfEM_Rcvr& rcvr = sensor->GetEM_Rcvr(i);
         WsfXIO_ObjectInfoLoad::Load_EM_Rcvr(mServicePtr->mXIO_Simulation, *(mStateData.mReceivers[i]), rcvr);
      }

      while (mStateData.mTransmitters.size() > sensor->GetEM_XmtrCount())
      {
         delete mStateData.mTransmitters.back();
         mStateData.mTransmitters.pop_back();
      }
      while (mStateData.mTransmitters.size() < sensor->GetEM_XmtrCount())
      {
         mStateData.mTransmitters.push_back(new WsfXIO_EM_XmtrInfo);
      }
      for (unsigned int i = 0; i < sensor->GetEM_XmtrCount(); ++i)
      {
         WsfEM_Xmtr& xmtr = sensor->GetEM_Xmtr(i);
         WsfXIO_ObjectInfoLoad::Load_EM_Xmtr(mServicePtr->mXIO_Simulation, *(mStateData.mTransmitters[i]), xmtr);
      }
   }
}
