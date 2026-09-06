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

#include "WsfPlatformPartEvent.hpp"

#include <cassert>

#include "UtMemory.hpp"
#include "WsfMultiThreadManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfPlatformPartEvent::WsfPlatformPartEvent(double aSimTime, EventType aEventType, WsfPlatformPart* aPlatformPartPtr)
   : WsfEvent(aSimTime)
   , mEventType(aEventType)
   , mPartPtr(aPlatformPartPtr)
{
   WsfPlatform* platformPtr = mPartPtr->GetPlatform();
   assert(platformPtr != nullptr);
   mPlatformIndex    = platformPtr->GetIndex();
   mUpdateEventEpoch = mPartPtr->GetUpdateEventEpoch();
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition WsfPlatformPartEvent::Execute()
{
   EventDisposition eventDisposition = cDELETE;

   if (mPartPtr.IsValid())
   {
      WsfPlatformPart* partPtr = mPartPtr.Get();
      double           simTime = GetTime();
      if ((mEventType == cUPDATE) && (!partPtr->IsExternallyControlled()))
      {
         if (mUpdateEventEpoch == partPtr->GetUpdateEventEpoch())
         {
            partPtr->SetUpdateEventActive(true);
            partPtr->Update(simTime);
            partPtr->SetUpdateEventActive(false);
            double updateInterval = partPtr->GetUpdateInterval();
            if (updateInterval > 0.0)
            {
               SetTime(simTime + updateInterval);
               eventDisposition = cRESCHEDULE;
            }
         }
      }
      else if (mEventType == cTURN_OFF)
      {
         partPtr->TurnOff(simTime);
         if ((partPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>()) && GetSimulation()->MultiThreaded())
         {
            GetSimulation()->GetMultiThreadManager().TurnSensorOff(simTime, static_cast<WsfSensor*>(partPtr));
         }
         partPtr->IncrementUpdateEventEpoch(); // Invalidate outstanding 'update' events
      }
      else if (mEventType == cTURN_ON)
      {
         partPtr->TurnOn(simTime);

         partPtr->IncrementUpdateEventEpoch(); // Invalidate outstanding 'update' events
         if (partPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>())
         {
            WsfSensor* sensorPtr = static_cast<WsfSensor*>(partPtr);
            if (GetSimulation()->MultiThreaded())
            {
               GetSimulation()->GetMultiThreadManager().TurnSensorOn(simTime, sensorPtr);
            }
            else if (!sensorPtr->IsSlave())
            {
               // Schedule the event to perform periodic calls to the PlatformParts Update() method.
               GetSimulation()->AddEvent(ut::make_unique<WsfPlatformPartEvent>(simTime, cUPDATE, partPtr));
            }
         }
         else if (partPtr->GetPartType() !=
                  cCOMPONENT_ROLE<WsfMover>()) // Movers have their own update events scheduled (see WsfMoverUpdateEvent)
         {
            if (partPtr->GetUpdateInterval() > 0.0)
            {
               // Schedule the event to periodically invoke the processors Update() method.
               GetSimulation()->AddEvent(ut::make_unique<WsfPlatformPartEvent>(simTime, cUPDATE, partPtr));
            }
         }
      }
   }
   return eventDisposition;
}
