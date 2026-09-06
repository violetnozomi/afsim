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

#ifndef WSFPROCESSOROBSERVER_HPP
#define WSFPROCESSOROBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfProcessor.hpp"
class WsfSimulation;
class WsfTrack;

namespace WsfObserver
{
using OperatingLevelChangedCallback   = UtCallbackListN<void(double, WsfProcessor*, WsfStringId, int)>;
using ProcessorBrokenCallback         = UtCallbackListN<void(double, WsfProcessor*)>;
using ProcessorNonOperationalCallback = UtCallbackListN<void(double, WsfProcessor*)>;
using ProcessorOperationalCallback    = UtCallbackListN<void(double, WsfProcessor*)>;
using ProcessorTurnedOffCallback      = UtCallbackListN<void(double, WsfProcessor*)>;
using ProcessorTurnedOnCallback       = UtCallbackListN<void(double, WsfProcessor*)>;
using StateEntryCallback              = UtCallbackListN<void(double, WsfProcessor*, const WsfTrack*, WsfStringId)>;
using StateExitCallback               = UtCallbackListN<void(double, WsfProcessor*, const WsfTrack*, WsfStringId)>;

WSF_EXPORT OperatingLevelChangedCallback&   OperatingLevelChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ProcessorTurnedOffCallback&      ProcessorTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ProcessorTurnedOnCallback&       ProcessorTurnedOn(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ProcessorNonOperationalCallback& ProcessorNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ProcessorOperationalCallback&    ProcessorOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ProcessorBrokenCallback&         ProcessorBroken(const WsfSimulation* aSimulationPtr);
WSF_EXPORT StateEntryCallback&              StateEntry(const WsfSimulation* aSimulationPtr);
WSF_EXPORT StateExitCallback&               StateExit(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Processor observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfProcessorObserver
{
   //! The operating level was changed.
   //! @param aSimTime The current simulation time.
   //! @param aProcessorPtr The processor whose operating level was changed.
   //! @param aName         The name (string id) of the condition or status that was changed.
   //! @param aLevel        The new level.
   WsfObserver::OperatingLevelChangedCallback OperatingLevelChanged;

   //! A processor is being turned off.
   //! @param aSimTime   The current simulation time.
   //! @param aProcessorPtr The processor that is being turned off.
   WsfObserver::ProcessorTurnedOffCallback ProcessorTurnedOff;

   //! A processor has been turned on.
   //! @param aSimTime   The current simulation time.
   //! @param aProcessorPtr The processor that has been turned on.
   WsfObserver::ProcessorTurnedOnCallback ProcessorTurnedOn;

   //! A processor is being made non-0perational.
   //! @param aSimTime   The current simulation time.
   //! @param aProcessorPtr The processor that is being made non-Operational.
   WsfObserver::ProcessorNonOperationalCallback ProcessorNonOperational;

   //! A processor is being made 0perational.
   //! @param aSimTime   The current simulation time.
   //! @param aProcessorPtr The processor that is being made Operational.
   WsfObserver::ProcessorOperationalCallback ProcessorOperational;

   //! A processor has gone terminal.
   //! @param aSimTime   The current simulation time.
   //! @param aProcessorPtr The processor that is terminated.
   WsfObserver::ProcessorBrokenCallback ProcessorBroken;

   //! A state machine transition into a state has occurred.
   //! @param aSimTime         The current simulation time.
   //! @param aProcessorPtr    The processor executing the state machine.
   //! @param aTrackPtr        A pointer to the track to which the transition applies. This may be zero
   //!                         if the transition is not associated with a specific track.
   //! @param aStateId         The string ID of the state entered.
   WsfObserver::StateEntryCallback StateEntry;

   //! A state machine transition into a state has occurred.
   //! @param aSimTime         The current simulation time.
   //! @param aProcessorPtr    The processor executing the state machine.
   //! @param aTrackPtr        A pointer to the track to which the transition applies. This may be zero
   //!                         if the transition is not associated with a specific track.
   //! @param aStateId         The string ID of the state exited.
   WsfObserver::StateExitCallback StateExit;
};

#endif
