// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfLaunchHandoffData.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
WsfLaunchHandoffData* WsfLaunchHandoffData::Find(const WsfPlatform& aPlatform)
{
   WsfLaunchHandoffData* extensionPtr(nullptr);
   aPlatform.FindByRole(extensionPtr);
   return extensionPtr;
}

// =================================================================================================
WsfLaunchHandoffData& WsfLaunchHandoffData::FindOrCreate(WsfPlatform& aPlatform)
{
   WsfLaunchHandoffData* extensionPtr = Find(aPlatform);
   if (extensionPtr == nullptr)
   {
      extensionPtr = new WsfLaunchHandoffData;
      aPlatform.AddComponent(extensionPtr);
   }
   return *extensionPtr;
}

// =================================================================================================
WsfLaunchHandoffData::WsfLaunchHandoffData()
   : WsfSimplePlatformComponent(cWSF_COMPONENT_LAUNCH_HANDOFF_DATA)
   , mValidityFlags(0)
   , mLaunchPlatformIndex(0)
   , mLaunchPlatformLat(0.0)
   , mLaunchPlatformLon(0.0)
   , mLaunchPlatformAlt(0.0)
   , mLaunchPlatformHeading(0.0)
   , mLaunchPlatformPitch(0.0)
   , mLaunchPlatformRoll(0.0)
   , mLaunchSystemYaw(0.0)
   , mLaunchSystemPitch(0.0)
   , mLaunchSystemRoll(0.0)
   , mLaunchSystemTilt(0.0)
   , mLoftAngle(0.0)
   , mBurnTime(0.0)
   , mTimeOfFlight(0.0)
   , mInterceptTime(0.0)
   , mInterceptPoint()
   , mEmptyMass(0.0)
   , mFuelMass(0.0)
   , mPayloadMass(0.0)
{
   UtVec3d::Set(mLaunchPlatformVelNED, 0.0);
   UtVec3d::Set(mLaunchSystemLocation, 0.0);
   UtVec3d::Set(mLaunchDeltaV, 0.0);
}

// =================================================================================================
WsfLaunchHandoffData::WsfLaunchHandoffData(const WsfLaunchHandoffData& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , mValidityFlags(aSrc.mValidityFlags)
   , mLaunchPlatformIndex(aSrc.mLaunchPlatformIndex)
   , mLaunchPlatformLat(aSrc.mLaunchPlatformLat)
   , mLaunchPlatformLon(aSrc.mLaunchPlatformLon)
   , mLaunchPlatformAlt(aSrc.mLaunchPlatformAlt)
   , mLaunchPlatformHeading(aSrc.mLaunchPlatformHeading)
   , mLaunchPlatformPitch(aSrc.mLaunchPlatformPitch)
   , mLaunchPlatformRoll(aSrc.mLaunchPlatformRoll)
   , mLaunchSystemYaw(aSrc.mLaunchSystemYaw)
   , mLaunchSystemPitch(aSrc.mLaunchSystemPitch)
   , mLaunchSystemRoll(aSrc.mLaunchSystemRoll)
   , mLaunchSystemTilt(aSrc.mLaunchSystemTilt)
   , mLoftAngle(aSrc.mLoftAngle)
   , mBurnTime(aSrc.mBurnTime)
   , mTimeOfFlight(aSrc.mTimeOfFlight)
   , mInterceptTime(aSrc.mInterceptTime)
   , mInterceptPoint(aSrc.mInterceptPoint)
   , mEmptyMass(aSrc.mEmptyMass)
   , mFuelMass(aSrc.mFuelMass)
   , mPayloadMass(aSrc.mPayloadMass)
{
   UtVec3d::Set(mLaunchPlatformVelNED, aSrc.mLaunchPlatformVelNED);
   UtVec3d::Set(mLaunchSystemLocation, aSrc.mLaunchSystemLocation);
   UtVec3d::Set(mLaunchDeltaV, aSrc.mLaunchDeltaV);
}

// =================================================================================================
void* WsfLaunchHandoffData::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_LAUNCH_HANDOFF_DATA)
      return this;
   return nullptr;
}

// =================================================================================================
//! Called at the time of launch to capture the current state of the launcher platform and system.
//! @param aPlatformPtr Pointer to the launching platform. If this is zero then the platform
//!                     associated with the launching system (aPartPtr) will be used.
//! @param aPartPtr     Pointer to the launching system. This can be zero if there is no associated
//!                     launch system.
void WsfLaunchHandoffData::SetLaunchPlatformData(WsfPlatform* aPlatformPtr, WsfArticulatedPart* aPartPtr)
{
   WsfPlatform* platformPtr = aPlatformPtr;
   if ((aPlatformPtr == nullptr) && (aPartPtr != nullptr))
   {
      platformPtr = aPartPtr->GetPlatform();
   }
   if (platformPtr == nullptr)
   {
      ut::log::error() << "WsfLaunchHandoff cannot determine launch platform.";
      return;
   }

   // Capture the launch platform data.
   mLaunchPlatformIndex = platformPtr->GetIndex();
   platformPtr->GetLocationLLA(mLaunchPlatformLat, mLaunchPlatformLon, mLaunchPlatformAlt);
   platformPtr->GetVelocityNED(mLaunchPlatformVelNED);
   platformPtr->GetOrientationNED(mLaunchPlatformHeading, mLaunchPlatformPitch, mLaunchPlatformRoll);
   mEmptyMass   = platformPtr->GetEmptyMass();
   mFuelMass    = platformPtr->GetFuelMass();
   mPayloadMass = platformPtr->GetPayloadMass();

   // Capture the state of the launching system/articulated part.
   if (aPartPtr != nullptr)
   {
      mLaunchSystemYaw   = aPartPtr->GetYaw();
      mLaunchSystemPitch = aPartPtr->GetPitch();
      mLaunchSystemRoll  = aPartPtr->GetRoll();
      mLaunchSystemTilt  = aPartPtr->GetTilt();
      aPartPtr->GetLocation(mLaunchSystemLocation);
   }
}

// =================================================================================================
//! Called at launch time to propagate the launch computer data to the launched object.
//! @param aComputerPtr Pointer to the launch computer.
void WsfLaunchHandoffData::SetLaunchComputerData(WsfLaunchComputer* aComputerPtr)
{
   if (aComputerPtr == nullptr)
   {
      return;
   }

   // Propagate results from the launch computer to the handoff data.

   if (aComputerPtr->LoftAngleIsValid())
   {
      SetLoftAngle(aComputerPtr->GetLoftAngle());
   }
   if (aComputerPtr->BurnTimeIsValid())
   {
      SetBurnTime(aComputerPtr->GetBurnTime());
   }
   if (aComputerPtr->TimeOfFlightIsValid())
   {
      SetTimeOfFlight(aComputerPtr->GetTimeOfFlight());
   }
   if (aComputerPtr->InterceptTimeIsValid())
   {
      SetInterceptTime(aComputerPtr->GetInterceptTime());
   }
   if (aComputerPtr->InterceptPointIsValid())
   {
      SetInterceptPoint(aComputerPtr->GetInterceptPoint());
   }
}
