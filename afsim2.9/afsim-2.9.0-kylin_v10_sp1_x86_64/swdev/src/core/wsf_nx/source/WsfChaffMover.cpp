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
#include "WsfChaffMover.hpp"

#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfChaffMover::WsfChaffMover(const WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mParcelPtr(nullptr)
{
}

// Chaff mover does not support Clone
WsfMover* WsfChaffMover::Clone() const
{
   return nullptr;
}

bool WsfChaffMover::Initialize(double aSimTime)
{
   if (!WsfChaffParcel::Find(mParcelPtr, *GetPlatform()))
   {
      auto logger = ut::log::error() << "Chaff mover cannot find chaff parcel extension on platform.";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();

      return false;
   }

   return WsfMover::Initialize(aSimTime);
}

void WsfChaffMover::Update(double aSimTime)
{
   mParcelPtr->Update(aSimTime);

   // Check for possible expiration of parcel and delete platform
   if (mParcelPtr->IsExpired(aSimTime))
   {
      GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
   }

   WsfMover::Update(aSimTime);
}
