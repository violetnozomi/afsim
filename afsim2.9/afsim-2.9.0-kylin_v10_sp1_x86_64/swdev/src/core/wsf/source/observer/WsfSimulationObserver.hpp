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

#ifndef WSFSIMULATIONOBSERVER_HPP
#define WSFSIMULATIONOBSERVER_HPP

#include "wsf_export.h"

#include <string>

#include "UtCallback.hpp"
class WsfMessage;
class WsfPlatform;
class WsfSimulation;

// Note: The following only exists in this file to prevent redundancy
//    in the compiled DOxygen code documentation.
//! A collection of accessors to observer objects for publish/subscribe.
//!
//! This namespace defines a set of callback objects to which simulation participants can
//! 'subscribe' to receive notification of significant events within the simulation. The
//! observer concept allows simulation developers to learn of certain events without having
//! to modify the core software that is issuing the event. Observers are typically used for
//! data collection and logging.
//!
//! Each variable represents a 'publisher' to which others can 'subscribe' using the
//! following idiom:
//!
//! \code
//! Class MyObserver
//! {
//!    public:
//!       void SensorTurnedOn(double     aSimTime,
//!                           WsfSensor* aSensorPtr);
//!    private:
//!       UtCallbackHolder   mCallbacks;
//! };
//!
//! // And then in the constructor.
//! mCallbacks.Add(WsfSensorObserver::Get(GetSimulation()).Connect(&MyObserver::SensorTurnedOn, this));
//! \endcode
//!
//! Observers can be single instances (e.g.: WsfDisInterface or WsfEventOutput) or they may be
//! contained within a platform part (processor, etc.). In the former case the callbacks
//! are typically registered in the constructor.  In the case of observers on platform parts,
//! they should almost always be registered within the Initialize method in order to prevent
//! creating observers in 'type' objects (instances of objects that are 'templates' that are
//! used to create 'real' objects).
//!
//! @see WsfEventOutput and WsfDisInterface for examples on how this is used.
namespace WsfObserver
{
//! A special callback list used in WsfObserver::Initialize.
class InitializeCallback : public UtCallbackListN<bool()>
{
public:
   bool Callback()
   {
      bool ok = true;
      for (auto it = mCallbackList.begin(); it != mCallbackList.end();)
      {
         if (!static_cast<CallbackType*>(*(it++))->operator()())
         {
            ok = false;
         }
      }
      return ok;
   }
   void Clear() { mCallbackList.clear(); }
};

using AdvanceTimeCallback               = UtCallbackListN<void(double)>;
using ApplicationExitCallback           = UtCallbackListN<void()>;
using ApplicationInitializeCallback     = UtCallbackListN<void()>;
using FrameCompleteCallback             = UtCallbackListN<void(double)>;
using FramePlatformsUpdatedCallback     = UtCallbackListN<void(double)>;
using FrameStartingCallback             = UtCallbackListN<void(double)>;
using RequestTimeAdvanceCallback        = UtCallbackListN<void(double, double, double)>;
using SimulationClockRateChangeCallback = UtCallbackListN<void(double)>;
using SimulationInitializingCallback    = UtCallbackListN<void()>;
using SimulationPendingStartCallback    = UtCallbackListN<void()>;
using SimulationStartingCallback        = UtCallbackListN<void()>;
using SimulationPausingCallback         = UtCallbackListN<void()>;
using SimulationResumingCallback        = UtCallbackListN<void()>;
using SimulationCompleteCallback        = UtCallbackListN<void(double)>;
using SimulationTimeBehindCallback      = UtCallbackListN<void(double)>;

WSF_EXPORT AdvanceTimeCallback&               AdvanceTime(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ApplicationExitCallback&           ApplicationExit(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ApplicationInitializeCallback&     ApplicationInitialize(const WsfSimulation* aSimulationPtr);
WSF_EXPORT FrameCompleteCallback&             FrameComplete(const WsfSimulation* aSimulationPtr);
WSF_EXPORT FramePlatformsUpdatedCallback&     FramePlatformsUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT FrameStartingCallback&             FrameStarting(const WsfSimulation* aSimulationPtr);
WSF_EXPORT InitializeCallback&                Initialize(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RequestTimeAdvanceCallback&        RequestTimeAdvance(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationClockRateChangeCallback& SimulationClockRateChange(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationInitializingCallback&    SimulationInitializing(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationPendingStartCallback&    SimulationPendingStart(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationStartingCallback&        SimulationStarting(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationPausingCallback&         SimulationPausing(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationResumingCallback&        SimulationResuming(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationCompleteCallback&        SimulationComplete(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SimulationTimeBehindCallback&      SimulationTimeBehind(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Simulation observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfSimulationObserver
{
   //! The simulation is advancing to the specified time.
   //! @param aSimTime The current simulation time.
   WsfObserver::AdvanceTimeCallback AdvanceTime;

   //! The application is exiting normally.  Subscribers will be called in FILO order.
   WsfObserver::ApplicationExitCallback ApplicationExit;

   //! The application is initializing.
   WsfObserver::ApplicationInitializeCallback ApplicationInitialize;

   //! The frame has completed.
   //! @param aSimTime The current simulation time.
   WsfObserver::FrameCompleteCallback FrameComplete;

   //! The platforms updates have completed in the frame.
   //! @param aSimTime The current simulation time.
   WsfObserver::FramePlatformsUpdatedCallback FramePlatformsUpdated;

   //! The frame is starting execution.
   //! @param aSimTime The current simulation time.
   WsfObserver::FrameStartingCallback FrameStarting;

   //! Initialize the simulation observer.
   //!
   //! This is invoked by WsfSimulation::Initialize(). Simulation observers
   //! are initialized prior to initializing any platforms.  If a simulation
   //! has multiple observers then the order in which they are initialized is
   //! undefined.
   //!
   //! @return 'true' if the observer was able to initialize successfully or
   //! 'false' if not.
   WsfObserver::InitializeCallback Initialize;

   //! Request time advance in a scaled-and-stepped frame-step dis simulation
   //! @param aSimTime The current simulation time.
   //! @param aMaxRate The max rate we believe we can proceed at.
   //! @param aTimeStep The amount of time we'd like to simulate in the next step.
   WsfObserver::RequestTimeAdvanceCallback RequestTimeAdvance;

   //! The simulation is starting execution.
   WsfObserver::SimulationClockRateChangeCallback SimulationClockRateChange;

   //! The simulation is initializing.
   WsfObserver::SimulationInitializingCallback SimulationInitializing;

   //! The simulation is pending start (i.e. simulation initialization is complete).
   WsfObserver::SimulationPendingStartCallback SimulationPendingStart;

   //! The simulation is starting execution.
   WsfObserver::SimulationStartingCallback SimulationStarting;

   //! The simulation is starting execution.
   WsfObserver::SimulationPausingCallback SimulationPausing;

   //! The simulation is starting execution.
   WsfObserver::SimulationResumingCallback SimulationResuming;

   //! The simulation has completed.
   //!
   //! This is called just before all the platforms that existed at the time of
   //! completion are removed. The simulation event queue has been purge and
   //! no further events will be dispatched.
   //!
   //! @param aSimTime The ending simulation time.
   WsfObserver::SimulationCompleteCallback SimulationComplete;

   //! For real-time simulations, notifies subscribers if the simulation clock is behind real-time.
   //! @param aTimeBehind The amount of time the simulation clock is behind the real-time clock.
   WsfObserver::SimulationTimeBehindCallback SimulationTimeBehind;
};

#endif
