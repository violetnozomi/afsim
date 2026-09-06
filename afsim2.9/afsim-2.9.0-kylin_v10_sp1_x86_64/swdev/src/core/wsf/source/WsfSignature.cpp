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

#include "WsfSignature.hpp"

#include <algorithm>

#include "UtStringIdLiteral.hpp"

// =================================================================================================
WsfSignature::WsfSignature()
   : WsfObject()
   , mPlatformPtr(nullptr)
   , mBaseTime(0.0)
{
}

// =================================================================================================
WsfSignature::WsfSignature(const WsfSignature& aSrc)
   : WsfObject(aSrc)
   , mPlatformPtr(nullptr)
   , mBaseTime(aSrc.mBaseTime)
   , mRequiresPlatform(aSrc.mRequiresPlatform)
{
}

// =================================================================================================
//! Initialize an signature 'type' instance.
//! This is called by WsfObjectTypeList::LoadType after all the input for the instance has been read.
//! @returns true if successful.
// virtual
bool WsfSignature::InitializeType()
{
   return true;
}
// =================================================================================================
//! Initialize an instance to prepare for use in a simulation.
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr The platform to which the instance is attached.
//! @returns true if successful.

// virtual
bool WsfSignature::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   mPlatformPtr = aPlatformPtr;
   mBaseTime    = aSimTime;
   return mPlatformPtr != nullptr;
}

// =================================================================================================
//! Checks to make the entered state Id exists, returns true if it exists, false otherwise.
//! @param aId the name Id of the state.
//! @return Returns true if provided state exists.
bool WsfSignature::IsA_ValidState(WsfStringId aId) const
{
   if (aId == UtStringIdLiteral("default"))
   {
      return true;
   }
   auto stateNames = GetStateNames();
   return std::find(stateNames.begin(), stateNames.end(), aId) != stateNames.end();
}

// =================================================================================================
// virtual
bool WsfSignature::ProcessInput(UtInput& aInput)
{
   return WsfObject::ProcessInput(aInput);
}
