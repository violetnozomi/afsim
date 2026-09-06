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

#include "WsfArticulatedPartEvent.hpp"

#include <cassert>

#include "WsfArticulatedPart.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfArticulatedPartEvent::WsfArticulatedPartEvent(double aSimTime, EventType aEventType, WsfArticulatedPart* aArticulatedPartPtr)
   : WsfEvent(aSimTime)
   , mEventType(aEventType)
   , mArticulatedPartPtr(aArticulatedPartPtr)
{
   assert(mArticulatedPartPtr != nullptr);
   WsfPlatform* platformPtr = mArticulatedPartPtr->GetPlatform();
   assert(platformPtr != nullptr);
   mPlatformIndex                = platformPtr->GetIndex();
   mArticulationUpdateEventEpoch = mArticulatedPartPtr->GetArticulationUpdateEventEpoch();
}

// virtual
WsfEvent::EventDisposition WsfArticulatedPartEvent::Execute()
{
   EventDisposition eventDisposition = cDELETE;

   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      double simTime = GetTime();
      if (mEventType == cUPDATE_ARTICULATION) // update the articulation, but not the sensor
      {
         if (mArticulationUpdateEventEpoch == mArticulatedPartPtr->GetArticulationUpdateEventEpoch())
         {
            mArticulatedPartPtr->SetArticulationUpdateEventActive(true);
            mArticulatedPartPtr->UpdatePosition(simTime);
            mArticulatedPartPtr->SetArticulationUpdateEventActive(false);
            double updateInterval = mArticulatedPartPtr->GetArticulationUpdateInterval();
            if (updateInterval > 0.0)
            {
               SetTime(simTime + updateInterval);
               eventDisposition = cRESCHEDULE;
            }
         }
      }
   }
   return eventDisposition;
}
