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

#include "WsfMilScriptObserver.hpp"

#include "UtScriptDataPack.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptObserver.hpp"

namespace
{
void JammingAttemptPacker(UtScriptDataPacker& aScriptArgs, WsfEM_Xmtr* aXmtrPtr, WsfEM_Rcvr* aRcvrPtr, WsfEM_Interaction& aResult)
{
   aScriptArgs << aXmtrPtr->GetPlatform() << aRcvrPtr->GetPlatform() << aResult;
}

void JammingRequestCanceledPacker(UtScriptDataPacker& aScriptArgs,
                                  WsfWeapon*          aWeaponPtr,
                                  double              aFrequency,
                                  double              aBandwidth,
                                  size_t              aTargetIndex)
{
   aScriptArgs << aWeaponPtr->GetPlatform() << aWeaponPtr << aFrequency << aBandwidth << aTargetIndex;
}

void JammingRequestPacker(UtScriptDataPacker& aScriptArgs,
                          WsfWeapon*          aWeaponPtr,
                          double              aFrequency,
                          double              aBandwidth,
                          WsfStringId         aTechniqueId,
                          size_t              aTargetIndex)
{
   aScriptArgs << aWeaponPtr->GetPlatform() << aWeaponPtr << aFrequency << aBandwidth << aTechniqueId << aTargetIndex;
}

void WeaponModePacker(UtScriptDataPacker& aScriptArgs, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr)
{
   aScriptArgs << aWeaponPtr->GetPlatform() << aWeaponPtr;
}

void WeaponPacker(UtScriptDataPacker& aScriptArgs, WsfWeapon* aWeaponPtr)
{
   aScriptArgs << aWeaponPtr->GetPlatform() << aWeaponPtr;
}
} // namespace

void WsfMilScriptObserver::Register_script_observer(WsfScriptObserver& aScriptObserver)
{
   auto& sim = aScriptObserver.GetSimulation();
   aScriptObserver.AddEvent("DIRECTED_ENERGY_WEAPON_BEGIN_SHOT",
                            WsfObserver::DirectedEnergyWeaponBeginShot(&sim),
                            "DirectedEnergyWeaponBeginShot",
                            "WsfDirectedEnergyWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent("DIRECTED_ENERGY_WEAPON_UPDATE_SHOT",
                            WsfObserver::DirectedEnergyWeaponUpdateShot(&sim),
                            "DirectedEnergyWeaponUpdateShot",
                            "WsfDirectedEnergyWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent("DIRECTED_ENERGY_WEAPON_ABORT_SHOT",
                            WsfObserver::DirectedEnergyWeaponAbortShot(&sim),
                            "DirectedEnergyWeaponAbortShot",
                            "WsfDirectedEnergyWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent("DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE",
                            WsfObserver::DirectedEnergyWeaponCooldownComplete(&sim),
                            "DirectedEnergyWeaponCooldownComplete",
                            "WsfWeapon");
   aScriptObserver.AddEvent("DIRECTED_ENERGY_WEAPON_END_SHOT",
                            WsfObserver::DirectedEnergyWeaponEndShot(&sim),
                            "DirectedEnergyWeaponEndShot",
                            "WsfDirectedEnergyWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent("IMPLICIT_WEAPON_BEGIN_ENGAGEMENT",
                            WsfObserver::ImplicitWeaponBeginEngagement(&sim),
                            "ImplicitWeaponBeginEngagement",
                            "WsfImplicitWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent("IMPLICIT_WEAPON_END_ENGAGEMENT",
                            WsfObserver::ImplicitWeaponEndEngagement(&sim),
                            "ImplicitWeaponEndEngagement",
                            "WsfImplicitWeapon, WsfWeaponEngagement");
   aScriptObserver.AddEvent<true>("JAMMING_ATTEMPT",
                                  WsfObserver::JammingAttempt(&sim),
                                  "JammingAttempt",
                                  "WsfPlatform, WsfPlatform, WsfEM_Interaction",
                                  UtStd::Bind(&JammingAttemptPacker));
   aScriptObserver.AddEvent("JAMMING_REQUEST_CANCELED",
                            WsfObserver::JammingRequestCanceled(&sim),
                            "JammingRequestCanceled",
                            "WsfPlatform, WsfWeapon, double, double, int",
                            UtStd::Bind(&JammingRequestCanceledPacker));
   aScriptObserver.AddEvent("JAMMING_REQUEST_INITIATED",
                            WsfObserver::JammingRequestInitiated(&sim),
                            "JammingRequestInitiated",
                            "WsfPlatform, WsfWeapon, double, double, string, int",
                            UtStd::Bind(&JammingRequestPacker));
   aScriptObserver.AddEvent("JAMMING_REQUEST_UPDATED",
                            WsfObserver::JammingRequestUpdated(&sim),
                            "JammingRequestUpdated",
                            "WsfPlatform, WsfWeapon, double, double, string, int",
                            UtStd::Bind(&JammingRequestPacker));
   // DEPRECATED as of 2.9
   aScriptObserver.AddEvent("PLATFORM_KILLED", WsfObserver::PlatformBroken(&sim), "PlatformKilled", "WsfPlatform");
   aScriptObserver.AddEvent("UPLINK_DROPPED",
                            WsfObserver::UplinkDropped(&sim),
                            "UplinkDropped",
                            "WsfPlatform, WsfPlatform, WsfTrack");
   aScriptObserver.AddEvent("UPLINK_INITIATED",
                            WsfObserver::UplinkInitiated(&sim),
                            "UplinkInitiated",
                            "WsfPlatform, WsfPlatform, WsfTrack");
   aScriptObserver.AddEvent("WEAPON_FIRED", WsfObserver::WeaponFired(&sim), "WeaponFired", "WsfWeaponEngagement, WsfTrack");
   aScriptObserver.AddEvent("WEAPON_FIRE_ABORTED",
                            WsfObserver::WeaponFireAborted(&sim),
                            "WeaponFireAborted",
                            "WsfWeapon, WsfTrack, double");
   aScriptObserver.AddEvent("WEAPON_FIRE_REQUESTED",
                            WsfObserver::WeaponFireRequested(&sim),
                            "WeaponFireRequested",
                            "WsfWeapon, WsfTrack, double");
   aScriptObserver.AddEvent("WEAPON_HIT", WsfObserver::WeaponHit(&sim), "WeaponHit", "WsfWeaponEngagement, WsfPlatform");
   aScriptObserver.AddEvent("WEAPON_KILLED", WsfObserver::WeaponKilled(&sim), "WeaponKilled", "WsfWeapon");
   aScriptObserver.AddEvent("WEAPON_MISSED", WsfObserver::WeaponMissed(&sim), "WeaponMissed", "WsfWeaponEngagement, WsfPlatform");
   aScriptObserver.AddEvent("WEAPON_MODE_ACTIVATED",
                            WsfObserver::WeaponModeActivated(&sim),
                            "WeaponModeActivated",
                            "WsfPlatform, WsfWeapon",
                            UtStd::Bind(&WeaponModePacker));
   aScriptObserver.AddEvent("WEAPON_MODE_DEACTIVATED",
                            WsfObserver::WeaponModeDeactivated(&sim),
                            "WeaponModeDeactivated",
                            "WsfPlatform, WsfWeapon",
                            UtStd::Bind(&WeaponModePacker));
   aScriptObserver.AddEvent("WEAPON_RELOAD_STARTED", WsfObserver::WeaponReloadStarted(&sim), "WeaponReloadStarted", "WsfWeapon");
   aScriptObserver.AddEvent("WEAPON_RELOAD_ENDED", WsfObserver::WeaponReloadEnded(&sim), "WeaponReloadEnded", "WsfWeapon");
   aScriptObserver.AddEvent("WEAPON_TERMINATED", WsfObserver::WeaponTerminated(&sim), "WeaponTerminated", "WsfWeaponEngagement");
   aScriptObserver.AddEvent("WEAPON_TURNED_OFF",
                            WsfObserver::WeaponTurnedOff(&sim),
                            "WeaponTurnedOff",
                            "WsfPlatform, WsfWeapon",
                            UtStd::Bind(&WeaponPacker));
   aScriptObserver.AddEvent("WEAPON_TURNED_ON",
                            WsfObserver::WeaponTurnedOn(&sim),
                            "WeaponTurnedOn",
                            "WsfPlatform, WsfWeapon",
                            UtStd::Bind(&WeaponPacker));
}
