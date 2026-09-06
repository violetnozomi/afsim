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

#include "WsfSimulation.hpp"

#if defined(_WIN32)
#include <windows.h>
#if defined(SendMessage) // winuser.h defines SendMessage as SendMessageA.
#undef SendMessage
#endif
#endif

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtScriptData.hpp"
#include "UtScriptExecutor.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfApplication.hpp"
#include "WsfBehaviorObserver.hpp"
#include "WsfClockSource.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommUtil.hpp"
#include "WsfDateTime.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfDisObserver.hpp"
#include "WsfEM_Manager.hpp"
#include "WsfEnvironment.hpp"
#include "WsfEvent.hpp"
#include "WsfEventManager.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfExtensionList.hpp"
#include "WsfFuel.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfGroupManager.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfLOS_Manager.hpp"
#include "WsfMessage.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfMultiThreadManager.hpp"
#include "WsfPathFinder.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPartObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfRealTimeClockSource.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfStringId.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfZone.hpp"
#include "WsfZoneAttenuation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

namespace
{
//! String representation of the WsfSimulation::State enumeration.
const std::array<std::string, 7> sStateString =
   {"PENDING_INITIALIZE", "INITIALIZING", "PENDING_START", "STARTING", "ACTIVE", "PENDING_COMPLETE", "COMPLETE"};
} // namespace

// =================================================================================================
WsfSimulation::WsfSimulation(const WsfScenario& aScenario, unsigned int aRunNumber)
   : mEventManager(*this)
   , mWallEventManager(*this)
   , mRunNumber(aRunNumber)
   , mSimulationInput(aScenario.GetSimulationInput())
   , mDateTime(mSimulationInput.GetDateTime())
   , mMultiThreaded(mSimulationInput.mMultiThreaded)
   , mMinimumMoverTimestep(mSimulationInput.mMinimumMoverTimestep)
   , mEndTime(mSimulationInput.mEndTime)
   , mIsRealTime(mSimulationInput.mIsRealTime)
   , mScenario(aScenario)
   , mClockRate(mSimulationInput.mClockRate)
   , mGroupManager(this)
   , mMultiThreadManager(mSimulationInput.mNumberOfThreads,
                         mSimulationInput.mBreakUpdateTime,
                         mSimulationInput.mDebugMultiThreading,
                         this)
   , mPathFinderList(aScenario.GetPathFinderList())
   , mZoneAttenuation(this)
   , mScriptExecutor(&aScenario.GetScriptEnvironment())
   , mGlobalContext(*mScenario.GetScriptContext())
{
   if (!aScenario.LoadIsComplete())
   {
      throw CreateError();
   }

   mGlobalContext.GetContext().Var("__SIMULATION").GetPointer()->SetAppObject(this);

   mRandom.SetSeed(aScenario.GetRandomSeed(aRunNumber));
   mScriptRandom.SetSeed(aScenario.GetRandomSeed(aRunNumber));

   if (GetTerrainInterface())
   {
      GetTerrainInterface()->PerformQueries(*this);
   }

   ResetPlatformList();
}

// =================================================================================================
// virtual
WsfSimulation::~WsfSimulation()
{
   // Delete any remaining 'active' platforms (there normally shouldn't be any)
   for (unsigned int i = 0; i < GetPlatformCount(); ++i)
   {
      WsfPlatform* platformPtr = GetPlatformEntry(i);
      platformPtr->SetDeleted();
      delete platformPtr;
   }
}

// =================================================================================================
// protected virtual
//! This is a method used to create the type of clock the simulation expects.
//!@note See WsfEventStepSimulation for a non-trivial implementation.
void WsfSimulation::CreateClock()
{
   SetClockSource(ut::make_unique<WsfClockSource>());
}

// =================================================================================================
//! Add an event to the simulation event queue.
//!
//! @param aEventPtr A pointer to the event to be queued. This classes assumes
//! ownership of the object.
// virtual
void WsfSimulation::AddEvent(std::unique_ptr<WsfEvent> aEventPtr)
{
   mEventManager.AddEvent(std::move(aEventPtr));
}

// =================================================================================================
//! Add an event to the real-world event queue. Event will be triggered based
//! on WallClock time, rather than simulation time, and will not pause if the
//! simulation pauses.  This should be used for Simulation Management events,
//! not simulated events.
//!
//! @param aEventPtr A pointer to the event to be queued. This class assumes
//! ownership of the object.
// virtual
void WsfSimulation::AddWallEvent(std::unique_ptr<WsfEvent> aEventPtr)
{
   mWallEventManager.AddEvent(std::move(aEventPtr));
}

// =================================================================================================
//! Dispatch the next event.
//!
//! @return The simulation time of the dispatched event.
// virtual
double WsfSimulation::AdvanceTime()
{
   // Track time spent running for synchronized NRT simulation.
   double timeStart = 0.0;
   if (mIsRealTime && (mRealTimeClockSourcePtr != nullptr))
   {
      timeStart = mRealTimeClockSourcePtr->GetElapsedWallTime();
   }

   WsfEvent* eventPtr = mEventManager.PeekEvent();
   if (eventPtr != nullptr)
   {
      mSimTime = eventPtr->GetTime();
   }
   else
   {
      mSimTime = GetEndTime() + 0.1;
   }

   // Let the clock controller override the time if necessary
   mSimTime = mClockSourcePtr->GetClock(mSimTime);
   WsfObserver::AdvanceTime(this)(mSimTime);

   // If it is still past the end time, change the simulation's state.
   if (mSimTime > GetEndTime())
   {
      mState = cPENDING_COMPLETE;
   }

   // Dispatch pending events up to and including the current time.
   DispatchEvents(mSimTime);

   // Track time spent running for synchronized NRT simulation
   if (mIsRealTime && (mRealTimeClockSourcePtr != nullptr) && !mClockSourcePtr->IsStopped())
   {
      double timeEnd = mRealTimeClockSourcePtr->GetElapsedWallTime();
      if (timeEnd > timeStart)
      {
         mSyncAccumulatedTime += (timeEnd - timeStart);
      }
   }
   return mSimTime;
}

// =================================================================================================
//! Dispatch the next event if its time is less than or equal to the specified time.
//!
//! @param  aSimTime Desired simulation time.
//!
//! @return If an event was dispatched then the time of the event, otherwise
//!         'aSimTime'.
// virtual
double WsfSimulation::AdvanceTime(double aSimTime)
{
   mSimTime           = aSimTime;
   WsfEvent* eventPtr = mEventManager.PeekEvent();
   if (eventPtr != nullptr)
   {
      mSimTime = std::min(aSimTime, eventPtr->GetTime());
   }

   // Let the clock controller override the time if necessary

   mSimTime = mClockSourcePtr->GetClock(mSimTime);
   WsfObserver::AdvanceTime(this)(mSimTime);

   // If it is still past the end time, change the simulation's state.
   if (mSimTime > GetEndTime())
   {
      mState = cPENDING_COMPLETE;
   }

   // Dispatch pending events up to and including the requested time.

   DispatchEvents(mSimTime);
   return mSimTime;
}

// =================================================================================================
//! Add a platform to an active simulation.
//!
//! This method should be used to add platforms to a simulation that is running.
//!
//! It is an error to invoke this before WsfSimulation::Initialize has been called.
//!
//! @param aPlatformPtr Pointer to the platform to be added.  This class assumes ownership
//!                     of the platform.
//! @return 'true' if the platform was added or 'false' if an not.
// virtual
bool WsfSimulation::AddPlatform(WsfPlatform* aPlatformPtr)
{
   return AddPlatform(GetSimTime(), aPlatformPtr);
}

// =================================================================================================
//! Add a platform to an active simulation.
//!
//! This method should be used to add platforms to a simulation that is running.
//!
//! It is an error to invoke this before WsfSimulation::Initialize has been called.
//!
//! @param aSimTime     The time at which to add the platform to the simulation.
//! @param aPlatformPtr Pointer to the platform to be added.  This class assumes ownership
//!                     of the platform.
//! @return 'true' if the platform was added or 'false' if an not.
//! @note If aSimTime is less than the current simulation time, it will be clamped to the
//!       current simulation time. If aSimTime is greater than the current simulation time,
//!       it will be scheduled to be added to the simulation at that future time.
// virtual
bool WsfSimulation::AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok                      = false;
   aPlatformPtr->mSimulationPtr = this;

   // If 'aSimTime' is less than the current simulation time, clamp it to the current simulation time.
   // If 'aSimTime' is greater than the current simulation time by 1/100th of a second, schedule the
   // the platform to be added at that future time.
   double simTime = GetSimTime();
   if (((aSimTime - GetSimTime()) > 0.01) && (!aPlatformPtr->IsExternallyControlled()))
   {
      simTime = aSimTime;

      // Create the platform at some future time.
      AddEvent(ut::make_unique<AddPlatformEvent>(simTime, aPlatformPtr));
      ok = true;
   }
   else
   {
      if (mState >= cINITIALIZING)
      {
         AssignDefaultName(aPlatformPtr); // Assign a default name if none provided
         if (aPlatformPtr == nullptr)
         {
            ut::log::error() << "WsfSimulation: Attempting to add a null platform pointer.";
         }
         else if (PlatformPointerExists(aPlatformPtr))
         {
            auto out = ut::log::error() << "WsfSimulation: Attempting to add the same platform twice.";
            out.AddNote() << "Name: " << aPlatformPtr->GetName();
            out.AddNote() << "Index: " << aPlatformPtr->GetIndex();
         }
         else if (aPlatformPtr->GetNameId() == nullptr)
         {
            ut::log::error() << "WsfSimulation: Trying to add a platform that does not have a name.";
         }
         else if (GetPlatformByName(aPlatformPtr->GetNameId()) != nullptr)
         {
            auto out = ut::log::error()
                       << "WsfSimulation: Attempting to add platform with the same name as an existing platform.";
            out.AddNote() << "Name: " << aPlatformPtr->GetNameId();
         }
         else
         {
            AddToPlatformList(aPlatformPtr);
            WsfObserver::PlatformAdded(this)(simTime, aPlatformPtr);

            // This needs to be looked at in terms of what happens if one of the steps fails.
            // This works but if a step fails we are notifying observers of creation and deletion
            // in one swoop.  ... kinda strange...

            if (aPlatformPtr->Initialize(this, simTime) && aPlatformPtr->Initialize2(this, simTime) &&
                PlatformInitialized(simTime, aPlatformPtr) && IntroducePlatform(simTime, aPlatformPtr))
            {
               WsfObserver::PlatformInitialized(this)(simTime, aPlatformPtr);
               TurnOnSystems(simTime, aPlatformPtr);
               ok = true;
            }
            else
            {
               // If any of the above fail then we back out what has been done.  This gives
               // control of the new platform back to the caller so it can delete it.
               if (DeleteFromPlatformList(aPlatformPtr))
               {
                  // Let others platforms know this platform has been deleted.
                  for (unsigned int i = 0; i < GetPlatformCount(); ++i)
                  {
                     GetPlatformEntry(i)->PlatformDeleted(aPlatformPtr);
                  }

                  // Tell any observers that the platform is being deleted.
                  WsfObserver::PlatformDeleted(this)(simTime, aPlatformPtr);
                  aPlatformPtr->SetDeleted();
               }
            }
         }
      }
      else
      {
         ut::log::error()
            << "WsfSimulation: Attempting to add a platform with a sim-time before Initialize() was called.";
      }
   }
   return ok;
}

// =================================================================================================
//! Assign a unique platform name if one was not explicitly specified.
//!
//! @param aPlatformPtr Pointer to the platform being added.
// private
void WsfSimulation::AssignDefaultName(WsfPlatform* aPlatformPtr)
{
   if ((aPlatformPtr != nullptr) &&
       ((aPlatformPtr->GetNameId() == nullptr) || (aPlatformPtr->GetNameId() == UtStringIdLiteral("<default>"))))
   {
      WsfPlatform* existingPlatformPtr = nullptr;
      int&         nameIndex           = mNextDefaultNameNumber[aPlatformPtr->GetTypeId()];
      do
      {
         std::ostringstream oss;
         oss << aPlatformPtr->GetType() << ':' << (++nameIndex);
         aPlatformPtr->SetName(oss.str());
         existingPlatformPtr = GetPlatformByName(aPlatformPtr->GetNameId());
      } while (existingPlatformPtr != nullptr);
   }
}

// =================================================================================================
//! Delete a platform from the simulation
//! @param aSimTime      Current simulation time
//! @param aPlatformPtr  Pointer to platform to be deleted
//! @param aDeleteMemory If true (default), platform's memory will be cleaned up. Otherwise, platform
//!                      is simply removed from the simulation and ownership of platform is released.
// virtual
void WsfSimulation::DeletePlatform(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeleteMemory)
{
   if (aPlatformPtr == nullptr)
   {
      ut::log::warning() << "WsfSimulation: Attempting to delete a null platform pointer.";
   }
   else if (PlatformExists(aPlatformPtr->GetIndex()))
   {
      // Notify the 'Single Platform Observers' registered with the platform
      aPlatformPtr->NotifyDeleting(aSimTime);
      if (!aPlatformPtr->IsDeleted())
      {
         aPlatformPtr->SetDeleted();
         //! A platform cannot be deleted directly because the caller may be something on the platform itself (such as a
         //! mover). In such a case the platform pointer must remain valid until the active event completes. To get
         //! around this, we schedule this event for the current simulation time. This allows the current event to
         //! complete and causes the platform to actually get deleted when this event is dispatched.
         AddEvent(ut::make_unique<WsfOneShotEvent>(aSimTime,
                                                   [=]() {
                                                      ProcessRemovePlatformEvent(aSimTime, aPlatformPtr, aDeleteMemory);
                                                   }));
      }
   }
   else
   {
      // In the case that a platform hasn't been added to the simulation yet, as is the case for hand-offs
      // go ahead and directly delete.
      // It MAY be on the  platform list... remove it if it is...
      mPlatforms.erase(std::remove(mPlatforms.begin(), mPlatforms.end(), aPlatformPtr), mPlatforms.end());
      if (aDeleteMemory)
      {
         delete aPlatformPtr;
      }
   }
}

// =================================================================================================
//! Dispatch any pending events up to and including the indicated time.
//!
//! @param aSimTime  All events whose time is less than or equal to this
//!                  value will be dispatched.
// protected virtual
void WsfSimulation::DispatchEvents(double aSimTime)
{
   // Advance SimTime event queue.
   DispatchSimEvents(aSimTime);

   // Advance WallTime event queue.
   DispatchWallEvents();
}

namespace
{
void DispatchEventsHelper(WsfEventManager& aEventManager, double aSimTime)
{
   WsfEvent* peekEventPtr = aEventManager.PeekEvent();
   while (peekEventPtr && (peekEventPtr->GetTime() <= aSimTime))
   {
      auto eventPtr = aEventManager.PopEvent();
      if (eventPtr->ShouldExecute())
      {
         WsfEvent::EventDisposition disposition = eventPtr->Execute();
         if (disposition == WsfEvent::cRESCHEDULE)
         {
            aEventManager.AddEvent(std::move(eventPtr));
         }
      }
      peekEventPtr = aEventManager.PeekEvent();
   }
}
} // namespace

// =================================================================================================
//! Dispatch any pending events up to and including the indicated time.
//!
//! @param aSimTime  All events whose time is less than or equal to this
//!                  value will be dispatched.
// protected virtual
void WsfSimulation::DispatchSimEvents(double aSimTime)
{
   // Dispatch pending simulation events up to and including the current time.
   DispatchEventsHelper(mEventManager, aSimTime);
}

// =================================================================================================
//! Dispatch any pending wall clock events up to and including the indicated time.
// protected virtual
void WsfSimulation::DispatchWallEvents()
{
   // Dispatch pending real-world (simulation management)
   // events up to and including the current wall time.
   DispatchEventsHelper(mWallEventManager, mWallClock.GetClock());
}

// =================================================================================================
void WsfSimulation::PrepareSimulation()
{
   // Prepare Extensions
   bool ok = true;
   for (const auto& i : mExtensionList.GetExtensionOrder())
   {
      ok &= GetExtension(i).PrepareExtension();
   }
   if (!ok)
   {
      throw InitializeError();
   }
}

// =================================================================================================
//! Initialize the simulation.
//!
//! This method initializes the simulation by performing the following actions:
//!
//! \li Invokes the Initialize method for each registered simulation extension.
//! \li Invokes Initialize each registered simulation observer.
//! \li Adds the 'initial state' platforms to the simulation.
//! \li Invokes the derived class SimulationInitialized() method.
//! \li Marks the simulation as 'pending_start'
//!
//! Upon completion of this call, the application should call WsfSimulation::Start,
//! enter a loop to execute events and then call WsfSimulation::Complete.
//!
//! @throw WsfSimulation::InitializeError if an error is encountered.
//!
//! @note It is an error to call this method again without calling WsfSimulation::Complete.
// virtual
void WsfSimulation::Initialize()
{
   mScenario.SimulationCreated(*this);
   mCommNetworkManagerPtr = static_cast<wsf::comm::NetworkManager*>(FindExtension("comm_network_manager"));
   mLOS_ManagerPtr        = static_cast<WsfLOS_Manager*>(FindExtension("los_manager"));

#if defined(_WIN32)
   if (mIsRealTime)
   {
      if (mSimulationInput.mProcessPriority == WsfSimulationInput::cPP_BELOW_NORMAL)
      {
         SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
      }
      else if (mSimulationInput.mProcessPriority == WsfSimulationInput::cPP_NORMAL)
      {
         SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
      }
      else if (mSimulationInput.mProcessPriority == WsfSimulationInput::cPP_ABOVE_NORMAL)
      {
         SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
      }
      else if (mSimulationInput.mProcessPriority == WsfSimulationInput::cPP_HIGH)
      {
         SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
      }
      else if (mSimulationInput.mProcessPriority == WsfSimulationInput::cPP_REALTIME)
      {
         SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
      }
   }
#endif

   if (GetScenario().GetScriptContext()->GetContext().GetScope() != GetScriptContext().GetContext().GetScope())
   {
      // This check is done to make sure we don't change the script definitions in the simulation.
      // The simulation should be able to use the definitions as defined by WsfScenario.
      // If this message is displayed, it means we are adding a registered script variable AFTER simulation input was
      // processed. Generally, this should not be required, and effectively results in additional copies of all script
      // 'scope' definitions
      ut::log::warning() << "The simulation has changed script definitions which may degrade performance.";
   }

   // Guard against re-initialization.
   if (mState != cPENDING_INITIALIZE)
   {
      ut::log::error() << "WsfSimulation: Simulation is already initialized.";
      throw InitializeError();
   }

   mState = cINITIALIZING;
   WsfObserver::SimulationInitializing(this)();

   // Print the seed when it's not obvious what the seed is
   if ((mScenario.GetFinalRunNumber() != mScenario.GetInitalRunNumber()) ||
       (mScenario.GetInitialSeed() != mRandom.GetSeed()))
   {
      auto out = ut::log::info() << "Using generated random seed.";
      out.AddNote() << "Seed: " << mRandom.GetSeed();
   }

   CreateClock();
   mSimTime    = 0.0;
   mRealTime   = 0.0;
   mTimeBehind = 0.0;
   mWallClock.SetTimingMethod((UtWallClock::TimingMethod)mSimulationInput.mWallClockTimingMethod);

   bool ok = true;

   ok &= mPathFinderList.Initialize(this);

   // Initialize the global script context
   ok &= GetScriptContext().InitializeGlobalContext(*this);

   // Initialize any zone set attenuation mapping if necessary
   ok &= mZoneAttenuation.Initialize(mScenario);

   // Initialize Extensions
   for (const auto& i : mExtensionList.GetExtensionOrder())
   {
      ok &= GetExtension(i).Initialize();
      ok &= GetExtension(i).PrepareExtension();
   }

   // Let all simulation observers initialize themselves.
   ok &= WsfObserver::Initialize(this).Callback(); // Use specialized Callback() instead of operator()

   // Add the available input platforms to the simulation.
   ok &= AddInputPlatforms();

   // Let the derived class do its initialization.
   if (ok)
   {
      ok = SimulationInitialized();
   }

   if (!ok)
   {
      throw InitializeError();
   }

   // Advance to the PENDING_START state.
   mState = cPENDING_START;
   // Notify Extensions of pending start (i.e. initialization complete)
   for (const auto& i : mExtensionList.GetExtensionOrder())
   {
      GetExtension(i).PendingStart();
   }
   WsfObserver::SimulationPendingStart(this)();

   // Schedule a dummy event for slightly past the simulation end_time
   AddEvent(ut::make_unique<WsfOneShotEvent>(GetEndTime() + 0.001, []() {}));
}

// =================================================================================================
//! This routine 'introduces' a new platform to the existing platforms.
//!
//! @param aSimTime        The current simulation time.
//! @param aNewPlatformPtr The new platform to be introduced.
//!
//! @return 'true' if successful and 'false' otherwise.
// private
bool WsfSimulation::IntroducePlatform(double aSimTime, WsfPlatform* aNewPlatformPtr)
{
   // Introduce the new and old platforms to each other
   for (unsigned int i = 0; i < GetPlatformCount(); ++i)
   {
      WsfPlatform* oldPlatformPtr = GetPlatformEntry(i);
      if (aNewPlatformPtr != oldPlatformPtr)
      {
         // Tell the new platform about the existence of an existing platform
         aNewPlatformPtr->PlatformAdded(aSimTime, oldPlatformPtr);
         // Tell the existing platform about the existence of the new platform
         oldPlatformPtr->PlatformAdded(aSimTime, aNewPlatformPtr);
      }
   }
   return PlatformIntroduced(aSimTime, aNewPlatformPtr);
}

// =================================================================================================
//! Set the clock source object.
//!
//! The clock source object is the source of time for the simulation.  Not all
//! simulations require a simulation clock.  For instance, in an event-stepped
//! simulation the flow of time can be derived from the events.  However one may
//! want to run the event-stepped simulation in a real-time environment.  Attaching
//! a real-time clock object will (if used properly) prevent events from being
//! dispatched until the correct simulation is encountered.
//!
//! There can be only once clock source.  Calling this method deletes any previously
//! defined clock source and replaces it with the new source.  The clock source is
//! owned by the simulation and will be deleted when the simulation is deleted.
//!
//! @param aClockSourcePtr Pointer to the clock source object.  This class assumes
//!                        ownership of the object.
void WsfSimulation::SetClockSource(std::unique_ptr<WsfClockSource> aClockSourcePtr)
{
   mClockSourcePtr = std::move(aClockSourcePtr);

   // If a clock was not specified then use the default clock source.
   if (mClockSourcePtr == nullptr)
   {
      mClockSourcePtr = ut::make_unique<WsfClockSource>();
   }

   // Initialize the clock rate
   mClockSourcePtr->SetClockRate(mClockRate);

   // set the timing method if this is a real time clock source
   mRealTimeClockSourcePtr = dynamic_cast<WsfRealTimeClockSource*>(mClockSourcePtr.get());
   if (mRealTimeClockSourcePtr != nullptr)
   {
      mRealTimeClockSourcePtr->SetTimingMethod((UtWallClock::TimingMethod)mSimulationInput.mWallClockTimingMethod);
   }

   WsfObserver::SimulationClockRateChange(this)(mClockRate);
}

// =================================================================================================
//! Return the pointer to the platform given its unique platform index.
//! The return value is 0 if the platform does not exist.
WsfPlatform* WsfSimulation::GetPlatformByIndex(size_t aIndex) const
{
   return (mPlatformsByIndex.size() > aIndex) ? mPlatformsByIndex.at(aIndex) : nullptr;
}

// =================================================================================================
//! Return a pointer to a platform given the string ID of the platform name.
//! @return A pointer to the platform or 0 if the platform does not exist.
WsfPlatform* WsfSimulation::GetPlatformByName(WsfStringId aNameId) const
{
   WsfPlatform* platformPtr = nullptr;
   auto         iter        = mPlatformsByName.find(aNameId);
   if (iter != mPlatformsByName.end())
   {
      platformPtr = iter->second;
   }
   return platformPtr;
}

// =================================================================================================
//! Returns 'true' if the platform with the specified index still exists.
bool WsfSimulation::PlatformExists(size_t aIndex) const
{
   return (mPlatformsByIndex.size() > aIndex) ? (mPlatformsByIndex.at(aIndex) != nullptr) : false;
}

// =================================================================================================
//! Return the name (ID) of a platform, even if the platform has been deleted from the simulation.

WsfStringId WsfSimulation::GetPlatformNameId(size_t aIndex) const
{
   return (aIndex < mPlatformNameIds.size()) ? mPlatformNameIds.at(aIndex) : nullptr;
}

// =================================================================================================
//! Return the type (ID) of a platform, even if the platform has been deleted from the simulation.
WsfStringId WsfSimulation::GetPlatformTypeId(size_t aIndex) const
{
   return (aIndex < mPlatformTypeIds.size()) ? mPlatformTypeIds.at(aIndex) : nullptr;
}

// =================================================================================================
//! Return the number of platforms in the platform list.
//! @return the number of platform in the platform list.
size_t WsfSimulation::GetPlatformCount() const
{
   return mPlatforms.size();
}

// =================================================================================================
//! Return a specific entry from the list of platforms.
//! @param aEntry The slot of the desired entry.  This is not the same
//! as the platform index. It must be valid, i.e.: 0 <= aEntry < GetPlatformCount().
//! @return Pointer to the requested platform.
WsfPlatform* WsfSimulation::GetPlatformEntry(size_t aEntry) const
{
   return mPlatforms[aEntry];
}

// =================================================================================================
//! Get the current wall clock elapsed time since simulation creation.
double WsfSimulation::GetWallTime() const
{
   return mWallClock.GetClock();
}

// =================================================================================================
//! Allow an external source (esp., an observer such as the DIS Interface) to
//! start the simulation in lieu of the simulation's starting itself.
void WsfSimulation::SetIsExternallyStarted(bool aIsExternallyStarted)
{
   mIsExternallyStarted = aIsExternallyStarted;
}

// =================================================================================================
//! Set the defined end time of the simulation.
void WsfSimulation::SetEndTime(double aEndTime)
{
   mEndTime = std::max(0.0, aEndTime);

   // Schedule a dummy event if there is ANY possibility of an event dispatch.
   if ((mState > cINITIALIZING) && (mState < cCOMPLETE))
   {
      AddEvent(ut::make_unique<WsfOneShotEvent>(mEndTime + 0.001, []() {}));
   }
}

// =================================================================================================
//! Return the time of the next event in the event queue.  If there is no event, the current
//! simulation time is returned.
double WsfSimulation::GetNextEventTime() const
{
   WsfEvent* nextEvent = mEventManager.PeekEvent();
   if (nextEvent != nullptr)
   {
      return nextEvent->GetTime();
   }
   return mSimTime;
}

// =================================================================================================
//! Indicate that the application has completed its simulation loop.
//!
//! This performs the following action (in the order specified):
//!
//! \li Marks the simulation as complete.
//! \li Stops the clock.
//! \li Invokes SimulationComplete for each registered simulation observer.
//! \li Deletes all remaining platforms.
//! \li Deletes all remaining events in the event queue
//! \li Invokes the Complete method for each registered simulation extension.
//! \li Resets the clock back to 0.0
//!
//! @param aSimTime The current simulation time.
// virtual
void WsfSimulation::Complete(double aSimTime)
{
   if ((mState != cACTIVE) && (mState != cPENDING_COMPLETE))
   {
      auto out = ut::log::warning() << "WsfSimulation: Simulation cannot be completed. It is in an expected state.";
      out.AddNote() << "Current State: " << sStateString[mState];
      out.AddNote() << "Expected: ACTIVE or PENDING_COMPLETE";
   }
   mState = cCOMPLETE;

   // A completion reason should have been set.
   if ((mCompletionReason == cNONE) && (mSimTime >= mEndTime))
   {
      mCompletionReason = cEND_TIME_REACHED;
   }

   mClockSourcePtr->StopClock();

   // In accordance with the documentation, this must be done before deleting the platforms.
   WsfObserver::SimulationComplete(this)(aSimTime);

   // Delete any remaining platforms.  This is a little inefficient but it ensures that
   // a platform deleted at this time under goes the exact same processing (callbacks,
   // etc.) as one deleted during the simulation.
   while (GetPlatformCount() != 0)
   {
      ProcessRemovePlatformEvent(aSimTime, GetPlatformEntry(0), true);
   }
   ResetPlatformList();
   mEventManager.Reset(); // Clean out the event queue

   // Inform observers that the simulation is complete.
   //
   // This must be done AFTER deleting the remaining platforms because Complete is defined to be
   // used for closing files. Platform deletion may cause other activity such as observer callbacks
   // that would normally write to the file.
   for (const auto& i : mExtensionList.GetExtensionOrder())
   {
      GetExtension(i).Complete(aSimTime);
   }

   mClockSourcePtr->ResetClock();
}

// =================================================================================================
//! Indicate the application is going to start its simulation loop.
//!
//! This performs the following actions:
//!
//! \li Resets the clock back to 0.0
//! \li Invokes the Start method for each registered simulation extension.
//! \li Invokes SimulationStarting for each registered observer.
//! \li Mark the simulation as active.
//!
//! @note This should be invoked IMMEDIATELY before starting the simulation loop as some
//!       observers may be time-sensitive.
// virtual
void WsfSimulation::Start()
{
   bool canStart = ((mState == cPENDING_START) || (mState == cCOMPLETE));
   if (!canStart)
   {
      auto out = ut::log::error() << "WsfSimulation: Simulation cannot be started.";
      out.AddNote() << "Current State: " << sStateString[mState];
      if (mState < cPENDING_START)
      {
         out.AddNote() << "Did you call WsfSimulation::Initialize?";
      }
      else
      {
         out.AddNote() << "Did you call WsfSimulation::Complete?";
      }
      throw StartError();
   }

   mState            = cSTARTING;
   mCompletionReason = cNONE;

   mClockSourcePtr->ResetClock();
   if (!mIsExternallyStarted)
   {
      mClockSourcePtr->StartClock();
   }
   else
   {
      ut::log::info() << "Waiting for simulation clock to start...";
   }
   // Inform extensions and observers that the simulation is starting
   for (const auto& i : mExtensionList.GetExtensionOrder())
   {
      GetExtension(i).Start();
   }
   WsfObserver::SimulationStarting(this)();

   mState = cACTIVE;
}

// =================================================================================================
//! Pause execution of the simulation
void WsfSimulation::Pause()
{
   if (mState == cACTIVE && !mClockSourcePtr->IsStopped())
   {
      mClockSourcePtr->StopClock();
      WsfObserver::SimulationPausing(this)();
   }
}

// =================================================================================================
//! Resume execution of the paused simulation
void WsfSimulation::Resume()
{
   if (mState == cACTIVE && mClockSourcePtr->IsStopped())
   {
      mClockSourcePtr->StartClock();
      WsfObserver::SimulationResuming(this)();
   }
}

// =================================================================================================
//! Pause the execution of the simulation and request a time advance in scaled-and-stepped mode.
void WsfSimulation::PauseAndRequestAdvance(double aSimTime)
{
   Pause();
   double currentRate = GetClockRate();
   double actualRate  = currentRate;

   // Calculate average simulation rate of the last step
   if ((mSyncTimestep > 0.0) && (mSyncAccumulatedTime > 0.0))
   {
      actualRate = mSyncTimestep / mSyncAccumulatedTime;
   }

   // adjust simulation rate based on being over/under the last requested rate
   double margin  = actualRate / currentRate;
   double maxRate = currentRate * (margin * 0.98);

   WsfObserver::RequestTimeAdvance(this)(aSimTime, maxRate, 5.0);
}

// =================================================================================================
//! Returns the clock rate of the simulation
// virtual
double WsfSimulation::GetClockRate() const
{
   return mClockRate;
}

// =================================================================================================
//! Changes the clock rate of the simulation
// virtual
void WsfSimulation::SetClockRate(double aClockRate)
{
   if (mClockRate != aClockRate)
   {
      mClockRate = aClockRate;
      if (mClockSourcePtr != nullptr)
      {
         mClockSourcePtr->SetClockRate(aClockRate);
         WsfObserver::SimulationClockRateChange(this)(mClockRate);
      }
   }
}

// =================================================================================================
//! Change the time parameters of the simulation
// virtual
void WsfSimulation::SetTimeParameters(int aTimeScheme, double aSimTime, double aClockRate, double aTimeStep, bool aTimeAdvance)
{
   SetClockRate(aClockRate);

   mSyncTimestep  = aTimeStep;
   double newTime = aSimTime - aTimeStep;
   if (newTime > mClockSourcePtr->GetClock(1.0e37))
   {
      mClockSourcePtr->SetClock(aSimTime - aTimeStep);
   }
   mRealTime = mClockSourcePtr->GetClock(1.0e37);

   AddEvent(ut::make_unique<WsfOneShotEvent>(aSimTime, [=]() { PauseAndRequestAdvance(aSimTime); }));

   if (aTimeAdvance && GetClockSource()->IsStopped())
   {
      mSyncAccumulatedTime = 0.0;
      Resume();
   }
}

// =================================================================================================
//! Virtual method invoked by the base class just after it has called the a platforms Initialize() method.
//!
//! It is provided so derived classes can perform their own unique platform initialization.
//!
//! @param aSimTime   The current simulation time.
//! @param aNewPlatformPtr Pointer to the platform just initialized.
//! @return 'true' if successful and 'false' otherwise.
// protected virtual
bool WsfSimulation::PlatformInitialized(double /* aSimTime */, WsfPlatform* /* aNewPlatformPtr */)
{
   return true;
}

// =================================================================================================
//! Virtual method invoked by the base class after it has introduced a new platform to the simulation.
//!
//! @param aSimTime   The current simulation time.
//! @param aNewPlatformPtr Pointer to the platform just introduced.
//! @return 'true' if successful and 'false' otherwise.
// protected virtual
bool WsfSimulation::PlatformIntroduced(double /* aSimTime */, WsfPlatform* /* aNewPlatformPtr */)
{
   return true;
}

// =================================================================================================
//! Virtual method invoked by the base class used to notify other platforms and observers of the impending
//! deletion of a platform.  This is called immediately prior to deleting the platform object.
//!
//! @param aSimTime   The current simulation time.
//! @param aOldPlatformPtr Pointer to the platform to be deleted.
//! @return 'true' if successful and 'false' otherwise.
// virtual
bool WsfSimulation::PlatformDeleted(double aSimTime, WsfPlatform* aOldPlatformPtr)
{
   // Let others platforms know this platform has been deleted.
   for (unsigned int i = 0; i < GetPlatformCount(); ++i)
   {
      WsfPlatform* platformPtr = GetPlatformEntry(i);
      platformPtr->PlatformDeleted(aOldPlatformPtr);
   }

   // Notify the 'Single Platform Observers' registered with the platform
   aOldPlatformPtr->NotifyDeleted(aSimTime);

   // If the platform is critically damaged AND it hasn't yet been noted as dead, inform the observer.
   if ((aOldPlatformPtr->GetDamageFactor() >= 1.0) && (!aOldPlatformPtr->IsBroken()))
   {
      WsfObserver::PlatformBroken(this)(aSimTime, aOldPlatformPtr);
   }

   // Tell any observers that the platform is being deleted.
   WsfObserver::PlatformDeleted(this)(aSimTime, aOldPlatformPtr);

   // Remove it from the platform list
   return DeleteFromPlatformList(aOldPlatformPtr);
}

// =================================================================================================
//! Called from RemovePlatformEvent to complete the processing of deleting a platform.
void WsfSimulation::ProcessRemovePlatformEvent(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeletePlatform)
{
   PlatformDeleted(aSimTime, aPlatformPtr);
   aPlatformPtr->SetDeleted();
   if (aDeletePlatform)
   {
      delete aPlatformPtr;
   } // Now we can delete the platform!!!
}

// =================================================================================================
//! Call from PlatformBrokenEvent to complete the processing of the broken of a platform.
void WsfSimulation::ProcessPlatformBrokenEvent(double aSimTime, WsfPlatform* aPlatformPtr)
{
   aPlatformPtr->OnBrokenEvent(aSimTime);
}

// =================================================================================================
//! Virtual method invoked by the base class Initialize() method
//! after it has performed the basic simulation initialization.
//!
//! Derived classes can use it to add any additional functionality they require.
//!
//! @return 'true' if successful and 'false' otherwise.
// protected virtual
bool WsfSimulation::SimulationInitialized()
{
   return true;
}

// =================================================================================================
//! Reset the simulation.
//!
//! This method is called when some external source wants to reset the simulation.
//! A derived class can override this method to perform its own special processing.
//!
//! The default behavior is to set the simulation state to PENDING_COMPLETE with a completion
//! reason of RESET.  It is the responsibility of the caller to detect this condition and take action.
// virtual
void WsfSimulation::RequestReset()
{
   if (mState != cACTIVE)
   {
      auto out = ut::log::warning() << "WsfSimulation: Simulation cannot be reset. It is in an unexpected state.";
      out.AddNote() << "Current State: " << sStateString[mState];
      out.AddNote() << "Expected: ACTIVE";
   }
   mState            = cPENDING_COMPLETE;
   mCompletionReason = cRESET;
}

// =================================================================================================
//! Terminate the simulation.
//!
//! This method is called when some external source wants to terminate the simulation.
//! A derived class can override this method to perform its own special processing.
//!
//! The default behavior is to set the simulation state to PENDING_COMPLETE with a completion
//! reason of TERMINATE.  It is the responsibility of the caller to detect this condition and take action.
// virtual
void WsfSimulation::RequestTermination()
{
   if (mState != cACTIVE)
   {
      auto out = ut::log::warning() << "WsfSimulation: Simulation cannot be terminated. It is in an unexpected state.";
      out.AddNote() << "Current State: " << sStateString[mState];
      out.AddNote() << "Expected: ACTIVE";
   }
   mState            = cPENDING_COMPLETE;
   mCompletionReason = cTERMINATE;
}

// =================================================================================================
//! Adds platforms to the simulation in a phase-stepped manner. This is only used by the simulation
//! during initialization of platforms. Any addition of platforms outside of this process is done
//! on a platform-by-platform basis using the standard AddPlatform call.
bool WsfSimulation::AddInputPlatforms()
{
   bool ok = true;

   WsfScriptContext& globalContext    = GetScriptContext();
   UtScriptClass*    platformClassPtr = globalContext.GetClass("WsfPlatform");
   UtScript*         scriptPtr        = globalContext.FindScript("on_platform_injection");
   if (scriptPtr != nullptr)
   {
      if (!globalContext.ValidateScript(scriptPtr, "bool", "WsfPlatform"))
      {
         ut::log::warning() << "Definition of on_platform_injection has the wrong signature.";
         scriptPtr = nullptr;
      }
   }

   std::vector<WsfPlatform*> platformList;
   for (unsigned int i = 0; i < mScenario.GetInputPlatformCount(); ++i)
   {
      WsfPlatform* inputPlatformPtr = mScenario.GetInputPlatformEntry(i);
      if (PlatformIsAvailable(inputPlatformPtr))
      {
         WsfPlatform* platformPtr    = inputPlatformPtr->Clone();
         platformPtr->mSimulationPtr = this;
         // Perform the one and only potential random draw for the platform creation time.
         bool okToAdd = platformPtr->InitializeCreationTime();

         // If the user has defined a on_platform_injection script, invoke it at this time.
         // The return value must be true to continue adding the platform.
         //
         // Note that we have first cloned the platform - so the script can modify it if desired.
         if (okToAdd && (scriptPtr != nullptr))
         {
            UtScriptData     ret;
            UtScriptDataList args;

            args.emplace_back(new UtScriptRef(platformPtr, platformClassPtr));
            globalContext.ExecuteScript(0.0, scriptPtr, ret, args);
            okToAdd = ret.GetBool();
         }

         if (!okToAdd)
         {
            // The script indicated the platform should not be added.
            WsfObserver::PlatformOmitted(this)(0.0, platformPtr);
            delete platformPtr;
         }
         else if (platformPtr->GetCreationTime() > 0.0)
         {
            // Create the platform at some future time.
            AddEvent(ut::make_unique<AddPlatformEvent>(platformPtr->GetCreationTime(), platformPtr));
         }
         else
         {
            bool result = false;
            AssignDefaultName(platformPtr); // Assign a default name if none provided
            if (platformPtr == nullptr)
            {
               ut::log::error() << "WsfSimulation: Attempting to add a null platform pointer.";
            }
            else if (PlatformPointerExists(platformPtr))
            {
               auto out = ut::log::error() << "WsfSimulation: Attempting to add the same platform twice.";
               out.AddNote() << "Name: " << platformPtr->GetName();
               out.AddNote() << "Index: " << platformPtr->GetIndex();
            }
            else if (platformPtr->GetNameId() == nullptr)
            {
               ut::log::error() << "WsfSimulation: Trying to add a platform that does not have a name.";
            }
            else if (GetPlatformByName(platformPtr->GetNameId()) != nullptr)
            {
               auto out = ut::log::error()
                          << "WsfSimulation: Attempting to add platform with the same name as an existing platform.";
               out.AddNote() << "Name: " << platformPtr->GetNameId();
            }
            else
            {
               platformList.emplace_back(platformPtr);
               result = true;
            }

            if (!result)
            {
               ok = false;
               WsfObserver::PlatformOmitted(this)(0.0, platformPtr);
               delete platformPtr;
            }
         }
      }
      else
      {
         WsfObserver::PlatformOmitted(this)(0.0, inputPlatformPtr);
      }
   }

   for (auto platformPtr : platformList)
   {
      AddToPlatformList(platformPtr);
      WsfObserver::PlatformAdded(this)(0.0, platformPtr);
      if (!platformPtr->Initialize(this, 0.0))
      {
         HandlePlatformInitializationFailure(platformPtr);
         ok = false;
      }
   }

   // Allow extensions to perform processing after platforms have gone through initialize calls.
   // Allows for processing not conducted via Platforms, but predicated on their existence and
   // initial initialization state for subsequent usage/provision during Initialize2 calls.
   if (ok)
   {
      for (const auto& i : mExtensionList.GetExtensionOrder())
      {
         ok &= GetExtension(i).PlatformsInitialized();
      }
   }

   if (ok)
   {
      for (auto platformPtr : platformList)
      {
         if (!platformPtr->Initialize2(this, 0.0))
         {
            HandlePlatformInitializationFailure(platformPtr);
            ok = false;
         }
      }
   }

   if (ok)
   {
      for (auto platformPtr : platformList)
      {
         if (!PlatformInitialized(0.0, platformPtr))
         {
            HandlePlatformInitializationFailure(platformPtr);
            ok = false;
         }
      }
   }

   if (ok)
   {
      for (auto platformPtr : platformList)
      {
         if (!IntroducePlatform(0.0, platformPtr))
         {
            HandlePlatformInitializationFailure(platformPtr);
            ok = false;
         }
      }
   }

   if (ok)
   {
      for (auto platformPtr : platformList)
      {
         WsfObserver::PlatformInitialized(this)(0.0, platformPtr);
         TurnOnSystems(0.0, platformPtr);
      }
   }

   return ok;
}

// =================================================================================================
//! Turn on all of the systems on a platform that are initially marked on.
// private
void WsfSimulation::TurnOnSystems(double aSimTime, WsfPlatform* aPlatformPtr)
{
   for (WsfComponentList::RoleIterator<WsfPlatformPart> partIter(*aPlatformPtr); !partIter.AtEnd(); ++partIter)
   {
      if (partIter->IsOperational() && (partIter->GetDamageFactor() < 1.0))
      {
         partIter->SetOperational(aSimTime);
         if (partIter->InitiallyTurnedOn())
         {
            TurnPartOn(aSimTime, *partIter);
         }
      }
   }
}

// =================================================================================================
//! Turn off a platform part.
//! @param aSimTime  The current simulation time.
//! @param aPartPtr  A pointer to the system to be turned off.
//! @return 'true' if successful or 'false' if already turned off.
// virtual
bool WsfSimulation::TurnPartOffP(double aSimTime, WsfPlatformPart* aPartPtr)
{
   if (aPartPtr->IsTurnedOn())
   {
      aPartPtr->TurnOff(aSimTime);
      return true;
   }
   return false;
}

// =================================================================================================
//! Turn on a platform part.
//! @param aSimTime  The current simulation time.
//! @param aPartPtr  A pointer to the system to be turned on.
//! @return 'true' if successful or 'false' if already turned on.
// virtual
bool WsfSimulation::TurnPartOnP(double aSimTime, WsfPlatformPart* aPartPtr)
{
   if (aPartPtr->CanBeTurnedOn())
   {
      if (aPartPtr->IsOperational() && (!aPartPtr->IsTurnedOn()))
      {
         aPartPtr->TurnOn(aSimTime);
         return true;
      }
   }
   return false;
}

// =================================================================================================
//! Set the platform part operation state
//! @param aSimTime  The current simulation time.
//! @param aPartPtr  A pointer to the system whose operational state is to be changed
//! @param aOperational  Desired operational state of the system
//! @return 'true' if successful or 'false' if operational state was not changed
// virtual
bool WsfSimulation::SetPartOperationalP(double aSimTime, WsfPlatformPart* aPartPtr, bool aOperational)
{
   if (aOperational != aPartPtr->IsOperational())
   {
      aPartPtr->SetOperational(aSimTime, aOperational);
      return true;
   }
   return false;
}

// =================================================================================================
//! Set the update interval for a platform part.
//! This method defines how often the simulation executive will call the Update() method.
//! @param aSimTime        The current simulation time.
//! @param aPartPtr        A pointer to the sensor whose update interval is to be changed.
//! @param aUpdateInterval The new update interval. If this value is less than or equal to zero, the periodic
//! calls to Update() will be deactivated until another call is made to reactive the periodic calls.
// virtual
void WsfSimulation::SetPartUpdateInterval(double aSimTime, WsfPlatformPart* aPartPtr, double aUpdateInterval)
{
   aPartPtr->SetUpdateInterval(aUpdateInterval);
}

// =================================================================================================
//! Add a platform to the list of platforms.
//!
//! It is assumed the caller has already verified the platform does not
//! exist in the list.
//!
//! @param aPlatformPtr Pointer to the platform to be added.
// protected
void WsfSimulation::AddToPlatformList(WsfPlatform* aPlatformPtr)
{
   assert(aPlatformPtr->GetIndex() == 0);

   mPlatforms.push_back(aPlatformPtr);
   mPlatformsByName[aPlatformPtr->GetNameId()] = aPlatformPtr;

   aPlatformPtr->SetIndex(static_cast<unsigned int>(mPlatformsByIndex.size()));
   mPlatformsByIndex.push_back(aPlatformPtr);
   mPlatformNameIds.push_back(aPlatformPtr->GetNameId());
   mPlatformTypeIds.push_back(aPlatformPtr->GetTypeId());
}

// =================================================================================================
//! Delete a platform pointer from the list of platforms.
//!
//! This only deletes the pointer from the list.  It does not delete the platform.
//! @return 'true' if the platform was successfully removed from the list
//! or 'false' if it did not exist in the list.
// protected
bool WsfSimulation::DeleteFromPlatformList(WsfPlatform* aPlatformPtr)
{
   bool wasDeleted = false;

   // For safety we always try to remove the platform from each container individually.

   auto iter = find(mPlatforms.begin(), mPlatforms.end(), aPlatformPtr);
   if (iter != mPlatforms.end())
   {
      mPlatforms.erase(iter);
      wasDeleted = true;
   }

   size_t platformIndex = aPlatformPtr->GetIndex();
   assert(platformIndex < mPlatformsByIndex.size());
   if ((platformIndex != 0) && (platformIndex < mPlatformsByIndex.size()))
   {
      assert(mPlatformsByIndex.at(platformIndex) == aPlatformPtr); // Make sure we're consistent!
      mPlatformsByIndex.at(platformIndex) = nullptr;
      wasDeleted                          = true;
   }

   auto mapIter = mPlatformsByName.find(aPlatformPtr->GetNameId());
   if (mapIter != mPlatformsByName.end())
   {
      assert(mapIter->second == aPlatformPtr); // Make sure we're consistent!
      mPlatformsByName.erase(mapIter);
      wasDeleted = true;
   }

   // We used to have a 'aPlatformPtr->SetIndex(0)' at this point to indicate the platform was no
   // longer in the list. Unfortunately this caused a problem that is way too involved to describe
   // here. The real issue is, that once set, the platform index is part of the platform's identity
   // and should not be modified. We don't alter that identity in any way until we actually delete
   // the platform object (see DeletePlatformEvent).
   // aPlatformPtr->SetIndex(0);     // Indicate that the platform is no longer in the list

   return wasDeleted;
}

// =================================================================================================
//! Does the platform pointer already exist in the list?
//!
//! @param aPlatformPtr The pointer to be tested.
//! @return 'true' if already in the list or 'false' if not.
// protected
bool WsfSimulation::PlatformPointerExists(WsfPlatform* aPlatformPtr)
{
   bool exists = false;
   if (find(mPlatforms.begin(), mPlatforms.end(), aPlatformPtr) != mPlatforms.end())
   {
      exists = true;
   }
   return exists;
}

// =================================================================================================
//! Reset the list to its initial (empty) state.
// protected
void WsfSimulation::ResetPlatformList()
{
   mPlatforms.clear();
   mPlatformsByIndex.clear();
   mPlatformsByName.clear();
   mPlatformNameIds.clear();
   mPlatformTypeIds.clear();

   // There is always one entry on the platform-by-index list as the first
   // entry is reserved so index 0 corresponds to 'no-platform'.
   mPlatformsByIndex.push_back(nullptr);
   mPlatformNameIds.emplace_back(nullptr);
   mPlatformTypeIds.emplace_back(nullptr);
   mNextDefaultNameNumber.clear();
}

// =================================================================================================
//! Determine whether the simulation should be executed.
//! To test for a reset condition or the end of a set of Monte-Carlo repetitions,
//! this method can be called as the exit condition of a while loop (see example, below).
//! @note This execution proceeds with calls to WsfSimulation::Initialize, WsfSimulation::Start and
//! WsfSimulation::AdvanceTime, as shown below.
//! \code
//! // Example simulation loop, using the ShouldExecute() method.
//! while (sim.ShouldExecute())
//! {
//!    sim.Initialize();
//!    sim.Start();
//!    while (sim.IsActive())
//!    {
//!       sim.AdvanceTime();
//!    }
//!    sim.Complete(sim.GetEndTime());
//! };
//! \endcode
// virtual
bool WsfSimulation::ShouldExecute() const
{
   bool shouldRestart = ((mState == cCOMPLETE) && (mCompletionReason == cRESET));
   return ((mState == cPENDING_INITIALIZE) || shouldRestart);
}

// =================================================================================================
WsfApplication& WsfSimulation::GetApplication() const
{
   return GetScenario().GetApplication();
}

// =================================================================================================
WsfSimulationExtension* WsfSimulation::FindExtension(const std::string& aName) const
{
   return static_cast<WsfSimulationExtension*>(mExtensionList.FindExtension(aName));
}

// =================================================================================================
WsfSimulationExtension& WsfSimulation::GetExtension(const std::string& aName) const
{
   return static_cast<WsfSimulationExtension&>(mExtensionList.GetExtension(aName));
}

// =================================================================================================
void WsfSimulation::RegisterExtension(const std::string& aName, std::unique_ptr<WsfSimulationExtension> aExtensionPtr)
{
   auto* extensionPtr = aExtensionPtr.get();
   mExtensionList.AddExtension(aName, std::move(aExtensionPtr));
   extensionPtr->AddedToSimulationP(aName, *this);
}

// =================================================================================================
void WsfSimulation::WaitForAdvanceTime() {}

// =================================================================================================
//! Assign the next unique identifier for WsfUniqueId.
unsigned int WsfSimulation::AssignUniqueId()
{
   std::lock_guard<std::recursive_mutex> lock(mUniqueIdMutex);
   return (++mUniqueId);
}

// =================================================================================================
ut::Random& WsfSimulation::GetRandom()
{
   // If this assert is hit then use LockRandom() and UnlockRandom()
   // or use the local random number generator for the object.
   assert(!mMultiThreadingActive);
   return mRandom;
}

// =================================================================================================
ut::Random& WsfSimulation::LockRandom()
{
   mRandomMutex.lock();
   return mRandom;
}

// =================================================================================================
void WsfSimulation::UnlockRandom()
{
   mRandomMutex.unlock();
}

// =================================================================================================
ut::Random& WsfSimulation::GetScriptRandom()
{
   // If this assert is hit then use LockScriptRandom() and UnlockScriptRandom()
   // or use the local random number generator for the object.
   assert(!mMultiThreadingActive);
   return mScriptRandom;
}

// =================================================================================================
ut::Random& WsfSimulation::LockScriptRandom()
{
   mScriptRandomMutex.lock();
   return mScriptRandom;
}

// =================================================================================================
void WsfSimulation::UnlockScriptRandom()
{
   mScriptRandomMutex.unlock();
}

// =================================================================================================
wsf::TerrainInterface* WsfSimulation::GetTerrainInterface() const
{
   return wsf::TerrainInterface::Get(mScenario);
}

// =================================================================================================
double WsfSimulation::CreateMessageDataTag()
{
   return (++mNextMessageDataTag);
}

// =================================================================================================
bool WsfSimulation::PlatformIsAvailable(WsfPlatform* aPlatformPtr)
{
   return mScenario.PlatformIsAvailable(aPlatformPtr, *this);
}

// =================================================================================================
unsigned int WsfSimulation::GetFinalRunNumber() const
{
   return GetScenario().GetFinalRunNumber();
}

// =================================================================================================
bool WsfSimulation::RandomizeFrequency() const
{
   return mSimulationInput.mRandomizeFrequency;
}

// =================================================================================================
bool WsfSimulation::UseDefaultFrequency() const
{
   return mSimulationInput.mUseDefaultFrequency;
}

const std::string& WsfSimulation::GetStateString() const
{
   return sStateString[mState];
}

// static
const std::string& WsfSimulation::GetStateString(State aState)
{
   return sStateString[aState];
}

// =================================================================================================
//! Return the string representation of the simulation's reason for completion.
//! If the simulation has not actually completed, the returned value will be "NONE".
std::string WsfSimulation::GetCompletionReasonString() const
{
   switch (mCompletionReason)
   {
   case cNONE:
      return "NONE";
   case cEND_TIME_REACHED:
      return "END_TIME_REACHED";
   case cRESET:
      return "RESET";
   case cTERMINATE:
      return "TERMINATED";
   default:
      return "OTHER";
   }
}

// =================================================================================================
//! Return the UtAtmosphere object used by the simulation
UtAtmosphere& WsfSimulation::GetAtmosphere() const
{
   return mScenario.GetAtmosphere();
}

// =================================================================================================
WsfEnvironment& WsfSimulation::GetEnvironment() const
{
   return mScenario.GetEnvironment();
}

// =================================================================================================
WsfSystemLog& WsfSimulation::GetSystemLog() const
{
   return mScenario.GetSystemLog();
}

// =================================================================================================
WsfIFF_Manager* WsfSimulation::GetIFF_Manager() const
{
   return mScenario.GetIFF_Manager();
}

// =================================================================================================
const std::vector<std::string>& WsfSimulation::GetExtensionNames() const
{
   return mExtensionList.GetExtensionOrder();
}

// =================================================================================================
//! Performs substitution for naming output files
//!    %d  - Run Number
//!    %D  - Date   M-D-Y
//!    %T  - Time   HHMMSS
std::string WsfSimulation::SubstituteOutputFileVariables(const std::string& aOutputFile)
{
   std::string newString = aOutputFile;
   UtCalendar  cal;
   cal.SetCurrentDateAndTime();
   std::ostringstream time;
   time << std::setfill('0') << std::setw(2) << cal.GetHour() << std::setw(2) << cal.GetMinute() << std::setw(2)
        << static_cast<int>(cal.GetSecond());

   UtStringUtil::ReplaceAll(newString, "%d", std::to_string(GetRunNumber()));
   UtStringUtil::ReplaceAll(newString,
                            "%D",
                            std::to_string(cal.GetMonth()) + "-" + std::to_string(cal.GetDay()) + "-" +
                               std::to_string(cal.GetYear()));
   UtStringUtil::ReplaceAll(newString, "%T", time.str());

   return newString;
}

// =================================================================================================
WsfEvent::EventDisposition WsfSimulation::AddPlatformEvent::Execute()
{
   // Check also to see if the event has been canceled
   if (GetSimulation()->AddPlatform(mPlatformPtr))
   {
      mPlatformPtr = nullptr; // Addition successful, the simulation now owns the platform
   }
   return cDELETE;
}

// =================================================================================================
WsfSimulation::AddPlatformEvent::AddPlatformEvent(double aSimTime, WsfPlatform* aPlatformPtr)
   : WsfEvent(aSimTime)
   , mPlatformPtr(aPlatformPtr)
{
}

// =================================================================================================
WsfSimulation::AddPlatformEvent::~AddPlatformEvent()
{
   delete mPlatformPtr;
}

// =================================================================================================
WsfSimulation::InitializeError::InitializeError()
   : WsfException("InitializeError")
{
}

// =================================================================================================
WsfSimulation::StartError::StartError()
   : WsfException("StartError")
{
}

// =================================================================================================
WsfSimulation::CreateError::CreateError()
   : WsfException("CreateError")
{
}

// =================================================================================================
void WsfSimulation::HandlePlatformInitializationFailure(WsfPlatform* aPlatformPtr)
{
   if (DeleteFromPlatformList(aPlatformPtr))
   {
      // Let others platforms know this platform has been deleted.
      for (size_t i = 0; i < GetPlatformCount(); ++i)
      {
         GetPlatformEntry(i)->PlatformDeleted(aPlatformPtr);
      }

      // Tell any observers that the platform is being deleted.
      WsfObserver::PlatformDeleted(this)(0.0, aPlatformPtr);
      aPlatformPtr->SetDeleted();
   }

   WsfObserver::PlatformOmitted(this)(0.0, aPlatformPtr);
   delete aPlatformPtr;
   aPlatformPtr = nullptr;
}
