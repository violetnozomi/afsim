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

#include "WsfP6DOF_ObjectManager.hpp"

#include "P6DofScenario.hpp"
#include "WsfP6DOF_TypeManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

WsfP6DOF_ObjectManager::WsfP6DOF_ObjectManager(WsfP6DOF_TypeManager* aTypeManager)
   : WsfSimulationExtension()
   , mWsfTypeManagerPtr(aTypeManager)
{
}

void WsfP6DOF_ObjectManager::AddedToSimulation()
{
   mCallbacks += WsfObserver::WeaponFired(&GetSimulation()).Connect(&WsfP6DOF_ObjectManager::WeaponFired, this);
   mCallbacks +=
      WsfObserver::WeaponReloadEnded(&GetSimulation()).Connect(&WsfP6DOF_ObjectManager::WeaponReloadEnded, this);
   mCallbacks += WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfP6DOF_ObjectManager::PlatformDeleted, this);
   mCallbacks += WsfObserver::PlatformAdded(&GetSimulation()).Connect(&WsfP6DOF_ObjectManager::PlatformAdded, this);
}

bool WsfP6DOF_ObjectManager::Initialize()
{
   return true;
}

void WsfP6DOF_ObjectManager::Start() {}

void WsfP6DOF_ObjectManager::Complete(double aSimTime) {}

bool WsfP6DOF_ObjectManager::PrepareExtension()
{
   return true;
}

void WsfP6DOF_ObjectManager::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) {}

void WsfP6DOF_ObjectManager::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr) {}

void WsfP6DOF_ObjectManager::WeaponFired(double                     aSimTime,
                                         const WsfWeaponEngagement* aEngagementPtr,
                                         const WsfTrack*            aTargetTrackPtr)
{
}

void WsfP6DOF_ObjectManager::WeaponReloadEnded(double aSimTime, WsfWeapon* aWeaponPtr) {}
