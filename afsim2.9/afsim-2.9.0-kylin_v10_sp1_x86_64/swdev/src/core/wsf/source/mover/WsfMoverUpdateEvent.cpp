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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfMoverUpdateEvent.hpp"

#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfMoverUpdateEvent::WsfMoverUpdateEvent(double aSimTime, WsfPlatform* aPlatformPtr)
   : WsfEvent(aSimTime)
   , mPlatformIndex(aPlatformPtr->GetIndex())
{
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition WsfMoverUpdateEvent::Execute()
{
   EventDisposition eventDisposition = cDELETE;
   WsfPlatform*     platformPtr      = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfMover* moverPtr = platformPtr->GetMover();
      if (moverPtr != nullptr)
      {
         platformPtr->Update(GetTime());
         double updateInterval = moverPtr->GetUpdateInterval();
         if (updateInterval > 0.0)
         {
            SetTime(GetTime() + updateInterval);
            eventDisposition = cRESCHEDULE;
         }
      }
   }
   return eventDisposition;
}
