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

#include "WsfShadowMover.hpp"

#include "UtInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfShadowMover::WsfShadowMover(const WsfPlatform* aPlatformPtr)
   : WsfMover(aPlatformPtr->GetScenario())
   , mPlatformIndex(aPlatformPtr->GetIndex())
{
   mThreadSafe = false;
}

// virtual
WsfMover* WsfShadowMover::Clone() const
{
   return new WsfShadowMover(*this);
}

// virtual
bool WsfShadowMover::Initialize(double aSimTime)
{
   return WsfMover::Initialize(aSimTime);
}

// virtual
bool WsfShadowMover::ProcessInput(UtInput& aInput)
{
   return WsfMover::ProcessInput(aInput);
}

// virtual
void WsfShadowMover::Update(double aSimTime)
{
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      platformPtr->Update(aSimTime);
      UtEntity* thisEntityPtr  = static_cast<UtEntity*>(GetPlatform());
      UtEntity* otherEntityPtr = static_cast<UtEntity*>(platformPtr);
      *thisEntityPtr           = *otherEntityPtr;
   }
}
