// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfBrawlerConsciousnessEvent.hpp"

#include "WsfBrawlerProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfBrawlerConsicousnessEvent::WsfBrawlerConsicousnessEvent(double aSimTime, size_t aPlatformIndex, ConsicousnessEventType aType)
   : WsfEvent(aSimTime)
   , mPlatformIndex(aPlatformIndex)
   , mType(aType)
{
}

WsfEvent::EventDisposition WsfBrawlerConsicousnessEvent::Execute()
{
   // Only BRAWLER platforms should have BRAWLER consciousness events scheduled
   // so cast WsfPlatform returned from simulation
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr && !platformPtr->IsBroken()) // TODO check for this event being "obsolete"
   {
      WsfBrawlerProcessor* brawlerProcPtr = WsfBrawlerProcessor::Find(*platformPtr);
      double               eventTime      = GetTime();
      platformPtr->Update(eventTime);
      // Platform is valid so update consciousness state, pick new actions
      // and schedule next consciousness event
      double nextEventTime =
         brawlerProcPtr->MindUpdate(eventTime); // Called to update BRAWLER specific status used in decision making
      // Schedule next event
      if (mType == SELF_CALLED)
      {
         // Based on the current event execution time and the interval to
         // the next event, schedule the new self called event time
         SetTime(nextEventTime);
         return cRESCHEDULE;
      }
      // TODO all other event types are just deleted for now
   }
   return cDELETE;
}
