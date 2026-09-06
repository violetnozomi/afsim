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

#ifndef WSFSPACEOBSERVER_HPP
#define WSFSPACEOBSERVER_HPP

#include "wsf_space_export.h"

#include "UtCallback.hpp"
class WsfOrbitalManeuvering;
class WsfOrbitalEvent;
class UtOrbitalPropagatorBase;
class WsfLocalTrack;
class WsfPlatform;
class WsfSimulation;
#include "WsfSimulationExtension.hpp"
class WsfSpaceMoverBase;

namespace WsfObserver
{
using EclipseEntryCallback      = UtCallbackListN<void(double, WsfSpaceMoverBase*)>;
using EclipseExitCallback       = UtCallbackListN<void(double, WsfSpaceMoverBase*)>;
using OrbitColorChangedCallback = UtCallbackListN<void(double, WsfSpaceMoverBase*)>;
using OrbitDeterminationInitiatedCallback =
   UtCallbackListN<void(double, WsfPlatform*, const WsfLocalTrack&, const UtOrbitalPropagatorBase&)>;
using OrbitDeterminationUpdatedCallback =
   UtCallbackListN<void(double, WsfPlatform*, const WsfLocalTrack&, const UtOrbitalPropagatorBase&)>;
using OrbitalManeuverInitiatedCallback  = UtCallbackListN<void(double, WsfSpaceMoverBase*, const WsfOrbitalEvent&)>;
using OrbitalManeuverUpdatedCallback    = UtCallbackListN<void(double, WsfSpaceMoverBase*, const WsfOrbitalEvent&)>;
using OrbitalManeuverCanceledCallback   = UtCallbackListN<void(double, WsfSpaceMoverBase*, const WsfOrbitalEvent&)>;
using OrbitalManeuverCompletedCallback  = UtCallbackListN<void(double, WsfSpaceMoverBase*, const WsfOrbitalEvent&)>;
using StagingOperationPerformedCallback = UtCallbackListN<void(double, const WsfOrbitalManeuvering&)>;

WSF_SPACE_EXPORT EclipseEntryCallback&                EclipseEntry(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT EclipseExitCallback&                 EclipseExit(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitColorChangedCallback&           OrbitColorChanged(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitDeterminationInitiatedCallback& OrbitDeterminationInitiated(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitDeterminationUpdatedCallback&   OrbitDeterminationUpdated(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitalManeuverInitiatedCallback&    OrbitalManeuverInitiated(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitalManeuverUpdatedCallback&      OrbitalManeuverUpdated(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitalManeuverCanceledCallback&     OrbitalManeuverCanceled(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT OrbitalManeuverCompletedCallback&    OrbitalManeuverCompleted(const WsfSimulation* aSimulationPtr);
WSF_SPACE_EXPORT StagingOperationPerformedCallback&   StagingOperationPerformed(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Space-related observer objects.
//! An instance of this class is maintained by the simulation.
class WSF_SPACE_EXPORT WsfSpaceObserver : public WsfSimulationExtension
{
public:
   WsfObserver::EclipseEntryCallback EclipseEntry;

   WsfObserver::EclipseExitCallback EclipseExit;

   WsfObserver::OrbitColorChangedCallback OrbitColorChanged;

   //! Orbit determination fusion has found an initial orbit for a previously unknown object.
   WsfObserver::OrbitDeterminationInitiatedCallback OrbitDeterminationInitiated;

   //! Orbit determination fusion has updated the orbit for an object previously identified with initial orbit determination.
   WsfObserver::OrbitDeterminationUpdatedCallback OrbitDeterminationUpdated;

   //! An orbital maneuver has been initiated by a space mover (WsfSpaceMoverBase).
   //! @param aSimTime The current simulation time.
   //! @param aSpaceMoverPtr The space mover that initiated the maneuver.
   //! @param aManeuverPtr The maneuver being executed.
   WsfObserver::OrbitalManeuverInitiatedCallback OrbitalManeuverInitiated;

   //! An orbital maneuver has been updated by a space mover (WsfSpaceMoverBase).
   //! @param aSimTime The current simulation time.
   //! @param aSpaceMoverPtr The space mover that initiated the maneuver.
   //! @param aManeuverPtr The maneuver being executed.
   //! @note This callback will only be triggered by non-impulsive (finite duration) maneuvers.
   WsfObserver::OrbitalManeuverUpdatedCallback OrbitalManeuverUpdated;

   //! An orbital maneuver has been canceled by a space mover (WsfSpaceMoverBase).
   //! @param aSimTime The current simulation time.
   //! @param aSpaceMoverPtr The space mover that initiated the maneuver.
   //! @param aManeuverPtr The maneuver being executed.
   //! @note This callback will only be triggered by non-impulsive (finite duration) maneuvers.
   WsfObserver::OrbitalManeuverCanceledCallback OrbitalManeuverCanceled;

   //! An orbital maneuver has been completed by a space mover (WsfSpaceMoverBase).
   //! @param aSimTime The current simulation time.
   //! @param aSpaceMoverPtr The space mover that initiated the maneuver.
   //! @param aManeuverPtr The maneuver being executed.
   WsfObserver::OrbitalManeuverCompletedCallback OrbitalManeuverCompleted;

   //! A staging operation has been performed for a rocket maneuvering object.
   //! This callback is intended to trigger the MoverStaged callback.
   //! @param aSimTime The current simulation time.
   //! @param aOrbitalManeuvering The maneuvering object that triggered the staging operation.
   WsfObserver::StagingOperationPerformedCallback StagingOperationPerformed;
};

#endif
