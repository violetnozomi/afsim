// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_ObjectManager.hpp"

#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

wsf::six_dof::ObjectManager::ObjectManager(TypeManager* aTypeManager)
   : WsfSimulationExtension()
   , mWsfTypeManagerPtr(aTypeManager)
{
}

void wsf::six_dof::ObjectManager::AddedToSimulation()
{
   mCallbacks += WsfObserver::WeaponFired(&GetSimulation()).Connect(&ObjectManager::WeaponFired, this);
   mCallbacks += WsfObserver::WeaponReloadEnded(&GetSimulation()).Connect(&ObjectManager::WeaponReloadEnded, this);
   mCallbacks += WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&ObjectManager::PlatformDeleted, this);
   mCallbacks += WsfObserver::PlatformAdded(&GetSimulation()).Connect(&ObjectManager::PlatformAdded, this);
}

bool wsf::six_dof::ObjectManager::Initialize()
{
   return true;
}

void wsf::six_dof::ObjectManager::Start() {}

void wsf::six_dof::ObjectManager::Complete(double aSimTime) {}

bool wsf::six_dof::ObjectManager::PrepareExtension()
{
   return true;
}

void wsf::six_dof::ObjectManager::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) {}

void wsf::six_dof::ObjectManager::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr) {}

void wsf::six_dof::ObjectManager::WeaponFired(double                     aSimTime,
                                              const WsfWeaponEngagement* aEngagementPtr,
                                              const WsfTrack*            aTargetTrackPtr)
{
}

void wsf::six_dof::ObjectManager::WeaponReloadEnded(double aSimTime, WsfWeapon* aWeaponPtr) {}
