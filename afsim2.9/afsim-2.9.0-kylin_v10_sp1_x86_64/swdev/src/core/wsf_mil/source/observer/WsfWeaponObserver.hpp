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

#ifndef WSFWEAPONOBSERVER_HPP
#define WSFWEAPONOBSERVER_HPP

#include "UtCallback.hpp"
class WsfDirectedEnergyWeapon;
class WsfEM_Interaction;
class WsfGuidanceComputer;
class WsfImplicitWeapon;
#include "wsf_mil_export.h"
class WsfMessage;
class WsfSimulation;
#include "WsfWeapon.hpp"

namespace WsfObserver
{
using DirectedEnergyWeaponFiredCallback =
   UtCallbackListN<void(double, const WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponHitCallback =
   UtCallbackListN<void(double, const WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponBeginShotCallback =
   UtCallbackListN<void(double, WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponAbortShotCallback =
   UtCallbackListN<void(double, WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponEndShotCallback =
   UtCallbackListN<void(double, WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using ImplicitWeaponBeginEngagementCallback =
   UtCallbackListN<void(double, WsfImplicitWeapon*, const WsfWeaponEngagement*)>;
using ImplicitWeaponEndEngagementCallback = UtCallbackListN<void(double, WsfImplicitWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponUpdateShotCallback =
   UtCallbackListN<void(double, WsfDirectedEnergyWeapon*, const WsfWeaponEngagement*)>;
using DirectedEnergyWeaponCooldownCompleteCallback = UtCallbackListN<void(double, WsfWeapon*)>;
using FireButtonSelectedCallback                   = UtCallbackListN<void(WsfPlatform&, int, WsfTrack*)>;
using GuidanceComputerPhaseChangedCallback         = UtCallbackListN<void(double, WsfGuidanceComputer*)>;
using JammingAttemptCallback           = UtCallbackListN<void(double, WsfEM_Xmtr*, WsfEM_Rcvr*, WsfEM_Interaction&)>;
using JammingRequestCanceledCallback   = UtCallbackListN<void(double, WsfWeapon*, double, double, size_t)>;
using JammingRequestInitiatedCallback  = UtCallbackListN<void(double, WsfWeapon*, double, double, WsfStringId, size_t)>;
using JammingRequestUpdatedCallback    = UtCallbackListN<void(double, WsfWeapon*, double, double, WsfStringId, size_t)>;
using TargetAllocatedCallback          = UtCallbackListN<void(double, WsfPlatform*, const WsfTrack*)>;
using TargetEngagedCallback            = UtCallbackListN<void(double, WsfPlatform*, const WsfTrack*)>;
using WeaponFinalOutgoingCheckCallback = UtCallbackListN<void(double, WsfWeaponEngagement*)>;
using WeaponFireAbortedCallback        = UtCallbackListN<void(double, WsfWeapon*, const WsfTrack*, double)>;
using WeaponFiredCallback              = UtCallbackListN<void(double, const WsfWeaponEngagement*, const WsfTrack*)>;
using WeaponFireRequestedCallback      = UtCallbackListN<void(double, WsfWeapon*, const WsfTrack*, double)>;
using WeaponPlatformPendingAddCallback =
   UtCallbackListN<void(double, WsfPlatform*, const WsfWeaponEngagement*, const WsfTrack*)>;
using WeaponHitCallback             = UtCallbackListN<void(double, const WsfWeaponEngagement*, WsfPlatform*)>;
using WeaponKilledCallback          = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponMissedCallback          = UtCallbackListN<void(double, const WsfWeaponEngagement*, WsfPlatform*)>;
using WeaponModeActivatedCallback   = UtCallbackListN<void(double, WsfWeapon*, WsfWeapon::WsfWeaponMode*)>;
using WeaponModeDeactivatedCallback = UtCallbackListN<void(double, WsfWeapon*, WsfWeapon::WsfWeaponMode*)>;
using WeaponNonOperationalCallback  = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponOperationalCallback     = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponQuantityChangedCallback = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponReloadEndedCallback     = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponReloadStartedCallback   = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponSelectedCallback        = UtCallbackListN<void(double, WsfWeapon*, WsfTrack*, int)>;
using WeaponTerminatedCallback      = UtCallbackListN<void(double, const WsfWeaponEngagement*)>;
using WeaponTurnedOffCallback       = UtCallbackListN<void(double, WsfWeapon*)>;
using WeaponTurnedOnCallback        = UtCallbackListN<void(double, WsfWeapon*)>;
using UplinkDroppedCallback         = UtCallbackListN<void(double, WsfPlatform*, WsfPlatform*, WsfTrack*)>;
using UplinkInitiatedCallback       = UtCallbackListN<void(double, WsfPlatform*, WsfPlatform*, WsfTrack*)>;

WSF_MIL_EXPORT DirectedEnergyWeaponFiredCallback&      DirectedEnergyWeaponFired(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponHitCallback&        DirectedEnergyWeaponHit(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponBeginShotCallback&  DirectedEnergyWeaponBeginShot(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponUpdateShotCallback& DirectedEnergyWeaponUpdateShot(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponEndShotCallback&    DirectedEnergyWeaponEndShot(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponAbortShotCallback&  DirectedEnergyWeaponAbortShot(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT ImplicitWeaponBeginEngagementCallback&  ImplicitWeaponBeginEngagement(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT ImplicitWeaponEndEngagementCallback&    ImplicitWeaponEndEngagement(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT DirectedEnergyWeaponCooldownCompleteCallback&
DirectedEnergyWeaponCooldownComplete(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT GuidanceComputerPhaseChangedCallback& GuidanceComputerPhaseChanged(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT JammingAttemptCallback&               JammingAttempt(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT JammingRequestCanceledCallback&       JammingRequestCanceled(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT JammingRequestInitiatedCallback&      JammingRequestInitiated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT JammingRequestUpdatedCallback&        JammingRequestUpdated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT TargetAllocatedCallback&              TargetAllocated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT TargetEngagedCallback&                TargetEngaged(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponFinalOutgoingCheckCallback&     WeaponFinalOutgoingCheck(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponFireAbortedCallback&            WeaponFireAborted(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponFiredCallback&                  WeaponFired(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponFireRequestedCallback&          WeaponFireRequested(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponPlatformPendingAddCallback&     WeaponPlatformPendingAdd(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponHitCallback&                    WeaponHit(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponMissedCallback&                 WeaponMissed(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponModeActivatedCallback&          WeaponModeActivated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponModeDeactivatedCallback&        WeaponModeDeactivated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponQuantityChangedCallback&        WeaponQuantityChanged(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponReloadEndedCallback&            WeaponReloadEnded(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponReloadStartedCallback&          WeaponReloadStarted(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponSelectedCallback&               WeaponSelected(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponTerminatedCallback&             WeaponTerminated(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponTurnedOffCallback&              WeaponTurnedOff(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponTurnedOnCallback&               WeaponTurnedOn(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponNonOperationalCallback&         WeaponNonOperational(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponOperationalCallback&            WeaponOperational(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT WeaponKilledCallback&                 WeaponKilled(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT UplinkDroppedCallback&                UplinkDropped(const WsfSimulation* aSimulation);
WSF_MIL_EXPORT UplinkInitiatedCallback&              UplinkInitiated(const WsfSimulation* aSimulation);
} // namespace WsfObserver

//! The implementation of the Weapon observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_MIL_EXPORT WsfWeaponObserver
{
   //! A directed energy weapon has been fired.
   //! @param aSimTime                 The current simulation time.
   //! @param aDirectedEnergyWeaponPtr The directed energy weapon that has been fired.
   //! @param aEngagementPtr           The weapon engagement object.
   WsfObserver::DirectedEnergyWeaponFiredCallback DirectedEnergyWeaponFired;

   //! A directed energy weapon is continuing an engagement.
   //! @param aSimTime                 The current simulation time.
   //! @param aDirectedEnergyWeaponPtr The directed energy weapon that has been fired.
   //! @param aEngagementPtr           The weapon engagement object.
   WsfObserver::DirectedEnergyWeaponHitCallback DirectedEnergyWeaponHit;

   WsfObserver::DirectedEnergyWeaponBeginShotCallback DirectedEnergyWeaponBeginShot;

   WsfObserver::DirectedEnergyWeaponUpdateShotCallback DirectedEnergyWeaponUpdateShot;

   WsfObserver::DirectedEnergyWeaponEndShotCallback DirectedEnergyWeaponEndShot;

   WsfObserver::DirectedEnergyWeaponAbortShotCallback DirectedEnergyWeaponAbortShot;

   WsfObserver::ImplicitWeaponBeginEngagementCallback ImplicitWeaponBeginEngagement;

   WsfObserver::ImplicitWeaponEndEngagementCallback ImplicitWeaponEndEngagement;

   WsfObserver::DirectedEnergyWeaponCooldownCompleteCallback DirectedEnergyWeaponCooldownComplete;

   WsfObserver::GuidanceComputerPhaseChangedCallback GuidanceComputerPhaseChanged;

   //! A jamming interaction is occurring.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The jammer being used in the interaction.
   WsfObserver::JammingAttemptCallback JammingAttempt;

   //! A jamming request has been canceled.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The jammer being requested.
   WsfObserver::JammingRequestCanceledCallback JammingRequestCanceled;

   //! A jammer has been requested to start jamming.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The jammer being requested.
   //! @param aModePtr   The jammer mode being requested (can be zero).
   WsfObserver::JammingRequestInitiatedCallback JammingRequestInitiated;

   //! A jamming request has been updated (mode switch, etc).
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The jammer being requested.
   //! @param aModePtr   The jammer mode being requested (can be zero).
   WsfObserver::JammingRequestUpdatedCallback JammingRequestUpdated;

   WsfObserver::TargetAllocatedCallback TargetAllocated;

   WsfObserver::TargetEngagedCallback TargetEngaged;

   //! An uplink is being dropped from the sender to the receiver concerning the target
   //! @param aSimTime        The current simulation time
   //! @param aPlatformPtr    The platform sending the uplink track messages
   //! @param aWeaponPtr      The weapon platform receiving the uplink track messages
   //! @param aTrackPtr       The track being targeted
   WsfObserver::UplinkDroppedCallback UplinkDropped;

   //! An uplink is being initiated from the sender to the receiver concerning the target
   //! @param aSimTime        The current simulation time
   //! @param aPlatformPtr    The platform sending the uplink track messages
   //! @param aWeaponPtr      The weapon platform receiving the uplink track messages
   //! @param aTrackPtr       The track being targeted
   WsfObserver::UplinkInitiatedCallback UplinkInitiated;

   //! A request to make final weapon checks on an outgoing weapon transfer weapon.
   //! @param aSimTime        The current simulation time.
   //! @param aPlatformPtr    The platform firing the weapon.
   //! @param aWeaponPtr      The weapon being fired.
   //! @param aTrackPtr       The track being targeted.
   WsfObserver::WeaponFinalOutgoingCheckCallback WeaponFinalOutgoingCheck;

   //! A weapon fire request was aborted.
   //! This is invoked when a weapon firing request was unable to complete.
   //! @param aSimTime        The current simulation time.
   //! @param aWeaponPtr      The weapon for which firing was attempted.
   //! @param aTargetTrackPtr The target track (can be 0).
   //! @param aQuantity       The quantity of the resource for which fire was requested
   //!                        but could not be completed.
   WsfObserver::WeaponFireAbortedCallback WeaponFireAborted;

   //! A weapon has been fired.
   //! @param aSimTime        The current simulation time.
   //! @param aEngagementPtr  The weapon engagement object.
   //! @param aTargetTrackPtr The target track (can be 0).
   WsfObserver::WeaponFiredCallback WeaponFired;

   //! A request to fire a weapon has been made.
   //! @param aSimTime        The current simulation time.
   //! @param aWeaponPtr      The weapon being fired.
   //! @param aTargetTrackPtr The target track (can be 0).
   //! @param aQuantity       The quantity of the resource to fired.
   WsfObserver::WeaponFireRequestedCallback WeaponFireRequested;

   //! A weapon platform is pending being added to the simulation.
   //! @param aSimTime        The current simulation time.
   //! @param aPlatformPtr    The weapon platform to be added.
   //! @param aEngagementPtr  The weapon engagement object.
   //! @param aTrackPtr       The track being targeted.
   WsfObserver::WeaponPlatformPendingAddCallback WeaponPlatformPendingAdd;

   //! A weapon has damaged another platform.  In this context, a 'hit' does not necessarily
   //! mean that contact was made, but that at least some (significant) incremental
   //! damage occurred to the specified other platform.
   //! @param aSimTime       The current simulation time.
   //! @param aEngagementPtr The weapon engagement object.
   //! @param aTargetPtr     The damaged platform.
   WsfObserver::WeaponHitCallback WeaponHit;

   //! A weapon has missed its intended target, or did not damage a close proximate platform.
   //! In this context, a 'miss' means no significant damage was caused to the other platform.
   //! @param aSimTime       The current simulation time.
   //! @param aEngagementPtr The weapon engagement object.
   //! @param aTargetPtr     The proximate or intended target platform.
   WsfObserver::WeaponMissedCallback WeaponMissed;

   //! A weapon mode has been activated.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon.
   //! @param aModePtr   The weapon mode that has been activated.
   WsfObserver::WeaponModeActivatedCallback WeaponModeActivated;

   //! A weapon mode has been deactivated.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon.
   //! @param aModePtr   The weapon mode that has been deactivated.
   WsfObserver::WeaponModeDeactivatedCallback WeaponModeDeactivated;

   //! The weapon's quantity has been changed.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon.
   WsfObserver::WeaponQuantityChangedCallback WeaponQuantityChanged;

   //! A weapon is completing reload.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that has been reloaded.
   WsfObserver::WeaponReloadEndedCallback WeaponReloadEnded;

   //! A weapon is being reloaded.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that is being reloaded.
   WsfObserver::WeaponReloadStartedCallback WeaponReloadStarted;

   //! A request to select a weapon has been made.
   //! @param aSimTime        The current simulation time.
   //! @param aWeaponPtr      The weapon being fired.
   //! @param aTrackPtr       The track being targeted.
   WsfObserver::WeaponSelectedCallback WeaponSelected;

   //! A weapon has terminate (exploded, crashed, etc).
   //! @param aSimTime       The current simulation time.
   //! @param aEngagementPtr The weapon engagement object.
   WsfObserver::WeaponTerminatedCallback WeaponTerminated;

   //! A weapon is being turned off.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that is being turned off.
   //! @param aStoreId   The storeId initially mapped to the weapon.
   WsfObserver::WeaponTurnedOffCallback WeaponTurnedOff;

   //! A weapon has turned on.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon has been turned on.
   WsfObserver::WeaponTurnedOnCallback WeaponTurnedOn;

   //! A weapon is transitioning to non-Operational.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that is being made non-Operational.
   WsfObserver::WeaponNonOperationalCallback WeaponNonOperational;

   //! A weapon is transitioning to Operational.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that is being made Operational.
   WsfObserver::WeaponOperationalCallback WeaponOperational;

   //! A weapon is transitioning to terminal.
   //! @param aSimTime   The current simulation time.
   //! @param aWeaponPtr The weapon that is becoming Terminal.
   WsfObserver::WeaponKilledCallback WeaponKilled;
};

#endif
