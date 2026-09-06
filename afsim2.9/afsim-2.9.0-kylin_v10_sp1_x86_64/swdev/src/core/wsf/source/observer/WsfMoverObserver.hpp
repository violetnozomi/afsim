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

#ifndef WSFMOVEROBSERVER_HPP
#define WSFMOVEROBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfMover.hpp"
class WsfPlatform;
class WsfSimulation;

namespace WsfObserver
{
using MoverChangedCallback        = UtCallbackListN<void(WsfPlatform*)>;
using MoverBrokenCallback         = UtCallbackListN<void(double, WsfMover*)>;
using MoverNonOperationalCallback = UtCallbackListN<void(double, WsfMover*)>;
using MoverOperationalCallback    = UtCallbackListN<void(double, WsfMover*)>;
using MoverTurnedOffCallback      = UtCallbackListN<void(double, WsfMover*)>;
using MoverTurnedOnCallback       = UtCallbackListN<void(double, WsfMover*)>;
using MoverBurnedOutCallback      = UtCallbackListN<void(double, WsfMover*)>;
using MoverStagedCallback         = UtCallbackListN<void(double, WsfMover*)>;
using MoverUpdatedCallback        = UtCallbackListN<void(double, WsfMover*)>;
using RouteChangedCallback        = UtCallbackListN<void(WsfMover*)>;
using MoverPathEndedCallback      = UtCallbackListN<void(double, WsfMover*)>;

WSF_EXPORT MoverChangedCallback&        MoverChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverTurnedOffCallback&      MoverTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverTurnedOnCallback&       MoverTurnedOn(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverNonOperationalCallback& MoverNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverOperationalCallback&    MoverOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverBrokenCallback&         MoverBroken(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverBurnedOutCallback&      MoverBurnedOut(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverStagedCallback&         MoverStaged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverUpdatedCallback&        MoverUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouteChangedCallback&        RouteChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MoverPathEndedCallback&      MoverPathEnded(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Mover observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfMoverObserver
{
   //! The mover of a platform has changed.
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform whose mover has changed.
   WsfObserver::MoverChangedCallback MoverChanged;

   //! The mover of a platform has been turned off.
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover that has been turned off.
   WsfObserver::MoverTurnedOffCallback MoverTurnedOff;

   //! The mover of a platform has been turned on.
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover that has been turned on.
   WsfObserver::MoverTurnedOnCallback MoverTurnedOn;

   //! The mover of a platform has transitioned to non-operational.
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover that has gone non-operational.
   WsfObserver::MoverNonOperationalCallback MoverNonOperational;

   //! The mover of a platform has transitioned to operational.
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover that has gone operational.
   WsfObserver::MoverOperationalCallback MoverOperational;

   //! The mover of a platform has transitioned to broken (unrepairable).
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover that has gone terminal.
   WsfObserver::MoverBrokenCallback MoverBroken;

   //! A mover has determined that its means of propulsion has waned to insignificance.
   //! @param aSimTime  The current simulation time.
   //! @param aMoverPtr The mover.
   WsfObserver::MoverBurnedOutCallback MoverBurnedOut;

   //! A mover has determined that a stage has been discarded.
   //! @param aSimTime  The current simulation time.
   //! @param aMoverPtr The mover.
   WsfObserver::MoverStagedCallback MoverStaged;

   //! A mover has updated the position, orientation, velocity or acceleration.
   //! @param aSimTime  The current simulation time.
   //! @param aMoverPtr The mover.
   WsfObserver::MoverUpdatedCallback MoverUpdated;

   //! A mover's route has been modified.
   //! @param aSimTime The current simulation time.
   //! @param aMoverPtr The mover with the changed route.
   WsfObserver::RouteChangedCallback RouteChanged;

   //! The mover's path has ended (route has completed)
   //! @param aSimTime The current simulation time
   //! @param aMoverPtr The mover with the completed path
   WsfObserver::MoverPathEndedCallback MoverPathEnded;
};

#endif
