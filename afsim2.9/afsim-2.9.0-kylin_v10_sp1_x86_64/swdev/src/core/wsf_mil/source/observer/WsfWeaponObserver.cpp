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

#include "WsfWeaponObserver.hpp"

#include "WsfMil.hpp"

WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponFired)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponHit)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponBeginShot)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponUpdateShot)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponEndShot)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponAbortShot)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, ImplicitWeaponBeginEngagement)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, ImplicitWeaponEndEngagement)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, DirectedEnergyWeaponCooldownComplete)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, GuidanceComputerPhaseChanged)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, JammingAttempt)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, JammingRequestCanceled)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, JammingRequestInitiated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, JammingRequestUpdated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, TargetAllocated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, TargetEngaged)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, UplinkInitiated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, UplinkDropped)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponFinalOutgoingCheck)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponFireAborted)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponFired)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponFireRequested)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponPlatformPendingAdd)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponHit)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponMissed)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponModeActivated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponModeDeactivated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponQuantityChanged)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponReloadEnded)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponReloadStarted)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponSelected)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponTerminated)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponTurnedOff)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponTurnedOn)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponNonOperational)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponOperational)
WSF_MIL_OBSERVER_CALLBACK_DEFINE(Weapon, WeaponKilled)
