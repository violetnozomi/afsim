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

#include "WsfMoverMode.hpp"

#include <iostream>
#include <limits>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfModeList.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
// virtual
WsfMoverMode::WsfMoverMode()
   : WsfMode()
   , mMoverPtr(nullptr)
   , mPlatformPtr(nullptr)
{
}

// =================================================================================================
WsfMoverMode::WsfMoverMode(const WsfMoverMode& aSrc)
   : WsfMode(aSrc)
   , mMoverPtr(nullptr)
   , mPlatformPtr(nullptr)
{
}

// =================================================================================================
WsfMoverMode& WsfMoverMode::operator=(const WsfMoverMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfMode::operator=(aRhs);
      mMoverPtr        = nullptr;
      mPlatformPtr     = nullptr;
   }
   return *this;
}

// =================================================================================================
// virtual
//! Initialize the Mover mode.
//! @param aSimTime   The current simulation time.
//! @param aMoverPtr The pointer to the Mover with which this mode is associated.
//! @returns 'true' if the mode was successfully initialize or 'false' if not.
bool WsfMoverMode::Initialize(double aSimTime, WsfMover* aMoverPtr)
{
   bool ok = true;

   mMoverPtr    = aMoverPtr;
   mPlatformPtr = aMoverPtr->GetPlatform();

   return ok;
}

// =================================================================================================
// virtual
bool WsfMoverMode::ProcessInput(UtInput& aInput)
{
   /* Null in the base class. */
   return false;
}
