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

#ifndef WSFTRACKOBSERVER_HPP
#define WSFTRACKOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
class WsfMessage;
class WsfLocalTrack;
class WsfPlatform;
class WsfSensor;
class WsfSimulation;
class WsfTrack;
class WsfTrackId;

namespace WsfObserver
{
using CorrelationStrategyChangedCallback = UtCallbackListN<void(double, WsfPlatform*)>;
using FusionStrategyChangedCallback      = UtCallbackListN<void(double, WsfPlatform*)>;
using LocalTrackCorrelationCallback = UtCallbackListN<void(double, WsfPlatform*, const WsfTrackId&, const WsfTrackId&)>;
using LocalTrackDecorrelationCallback = UtCallbackListN<void(double, WsfPlatform*, const WsfTrackId&, const WsfTrackId&)>;
using LocalTrackDroppedCallback       = UtCallbackListN<void(double, WsfPlatform*, const WsfLocalTrack*)>;
using LocalTrackInitiatedCallback  = UtCallbackListN<void(double, WsfPlatform*, const WsfLocalTrack*, const WsfTrack*)>;
using LocalTrackUpdatedCallback    = UtCallbackListN<void(double, WsfPlatform*, const WsfLocalTrack*, const WsfTrack*)>;
using SensorTrackCoastedCallback   = UtCallbackListN<void(double, WsfSensor*, const WsfTrack*)>;
using SensorTrackDroppedCallback   = UtCallbackListN<void(double, WsfSensor*, const WsfTrack*)>;
using SensorTrackInitiatedCallback = UtCallbackListN<void(double, WsfSensor*, const WsfTrack*)>;
using SensorTrackUpdatedCallback   = UtCallbackListN<void(double, WsfSensor*, const WsfTrack*)>;

WSF_EXPORT CorrelationStrategyChangedCallback& CorrelationStrategyChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT FusionStrategyChangedCallback&      FusionStrategyChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LocalTrackCorrelationCallback&      LocalTrackCorrelation(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LocalTrackDecorrelationCallback&    LocalTrackDecorrelation(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LocalTrackDroppedCallback&          LocalTrackDropped(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LocalTrackInitiatedCallback&        LocalTrackInitiated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LocalTrackUpdatedCallback&          LocalTrackUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTrackCoastedCallback&         SensorTrackCoasted(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTrackDroppedCallback&         SensorTrackDropped(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTrackInitiatedCallback&       SensorTrackInitiated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTrackUpdatedCallback&         SensorTrackUpdated(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Track observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfTrackObserver
{
   //! A platform's track manager's correlation strategy has changed.
   //! @param aSimTime   The current simulation time.
   //! @param aPlatformPtr The platform with the modified strategy.
   WsfObserver::CorrelationStrategyChangedCallback CorrelationStrategyChanged;

   //! A platform's track manager's fusion strategy has changed.
   //! @param aSimTime   The current simulation time.
   //! @param aPlatformPtr The platform with the modified strategy.
   WsfObserver::FusionStrategyChangedCallback FusionStrategyChanged;

   //! Indicate a non-local track is to be 'associated' with a specified local track.
   //! This is typically called by a track manager to indicate that a non-local track is associated (or 'correlated')
   //! with a specified local track.
   //! @param aSimTime         The current simulation time.
   //! @param aPlatformPtr     The platform that owns the local track.
   //! @param aLocalTrackId    The ID of the local track with which the non-local track is being associated.
   //! @param aNonLocalTrackId The ID of the non-local track to be associated with the local track.
   WsfObserver::LocalTrackCorrelationCallback LocalTrackCorrelation;

   //! Indicate that non-local track is no longer 'associated' with the specified local track.
   //! This is typically called by a track manager to indicate that a non-local track is no longer associated
   //! with a specified local track.
   //! @param aSimTime         The current simulation time.
   //! @param aPlatformPtr     The platform that owns the local track.
   //! @param aLocalTrackId    The ID of the local track from which the association is being removed.
   //! @param aNonLocalTrackId The ID of the non-local track that is no longer associated with the specified local track.
   WsfObserver::LocalTrackDecorrelationCallback LocalTrackDecorrelation;

   //! A platform has dropped a local track.
   //! @param aSimTime     The current simulation time.
   //! @param aPlatformPtr The platform that is dropping the track.
   //! @param aTrackPtr    The track that was dropped.
   WsfObserver::LocalTrackDroppedCallback LocalTrackDropped;

   //! A platform has initiated a local track.
   //! @param aSimTime     The current simulation time.
   //! @param aPlatformPtr The platform that is initiating the track.
   //! @param aTrackPtr    The track that was initiated.
   //! @param aSourcePtr   The source track that was used to create the track (may be 0).
   WsfObserver::LocalTrackInitiatedCallback LocalTrackInitiated;

   //! A platform has updated a local track.
   //! @param aSimTime     The current simulation time.
   //! @param aPlatformPtr The platform that is updating the track.
   //! @param aTrackPtr    The track that was updated.
   //! @param aSourcePtr   The source track that was used to update the track (may be 0).
   WsfObserver::LocalTrackUpdatedCallback LocalTrackUpdated;

   //! A sensor has coasted a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is coasting the track.
   //! @param aTrackPtr  The track being coasted.
   WsfObserver::SensorTrackCoastedCallback SensorTrackCoasted;

   //! A sensor has dropped a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is dropping a track.
   //! @param aTrackPtr  The track being dropped.
   WsfObserver::SensorTrackDroppedCallback SensorTrackDropped;

   //! A sensor has initiated a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is initiating a track.
   //! @param aTrackPtr  The track being initiated.
   WsfObserver::SensorTrackInitiatedCallback SensorTrackInitiated;

   //! A sensor has initiated a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is updating a track.
   //! @param aTrackPtr  The track being updated.
   WsfObserver::SensorTrackUpdatedCallback SensorTrackUpdated;
};

#endif
