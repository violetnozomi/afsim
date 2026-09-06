// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEclipseEventManager.hpp"

#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceObserver.hpp"

WsfEclipseEventManager::WsfEclipseEventManager()
   : WsfSimulationExtension()
   , mIsEnabled(false)
{
}

bool WsfEclipseEventManager::Initialize()
{
   if (WsfObserver::EclipseEntry(&GetSimulation()).IsEmpty() && WsfObserver::EclipseExit(&GetSimulation()).IsEmpty())
   {
      // Do not incur overhead if data are not needed.
   }
   else
   {
      Enable(0.0);
   }
   return true;
}

//! Enable eclipse event reporting.  This method is called automatically in Initialize() if there are subscibers to
//! eclipse events at the start of the simulation; otherwise Enable() must be called before subscribing to eclipse
//! events.
void WsfEclipseEventManager::Enable(double aSimTime)
{
   if (!mIsEnabled)
   {
      mIsEnabled = true;

      // Register for callbacks that allow us to update detections and write to the file at the end of the simulation.
      mCallbacks.Add(
         WsfObserver::PlatformInitialized(&GetSimulation()).Connect(&WsfEclipseEventManager::InitiateMonitoring, this));
      mCallbacks.Add(WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfEclipseEventManager::CeaseMonitoring, this));
      mCallbacks.Add(
         WsfObserver::OrbitalManeuverUpdated(&GetSimulation()).Connect(&WsfEclipseEventManager::UpdateMonitoring, this));
      mCallbacks.Add(
         WsfObserver::OrbitalManeuverCompleted(&GetSimulation()).Connect(&WsfEclipseEventManager::UpdateMonitoring, this));

      if (GetSimulation().IsActive())
      {
         // Add events for all space platforms currently in the simulation
         for (size_t index = 0; index < GetSimulation().GetPlatformCount(); ++index)
         {
            WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(index);
            if (platformPtr != nullptr)
            {
               InitiateMonitoring(aSimTime, platformPtr);
            }
         }
      }
   }
}

//! Disable eclipse event reporting.  No further eclipse events will be triggered after this method is called.
void WsfEclipseEventManager::Disable()
{
   mIsEnabled = false;
   mPlatformToCurrentEventIdMap.clear();
   mCallbacks.Clear();
}

// private
//! Initiate eclipse events reported for the given platform.
void WsfEclipseEventManager::InitiateMonitoring(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr->GetSpatialDomain() == WsfSpatialDomain::WSF_SPATIAL_DOMAIN_SPACE)
   {
      WsfSpaceMoverBase* spaceMoverPtr = dynamic_cast<WsfSpaceMoverBase*>(aPlatformPtr->GetMover());
      if (spaceMoverPtr != nullptr)
      {
         InitiateEclipseEvent(aSimTime, aPlatformPtr->GetUniqueId(), *spaceMoverPtr);
      }
   }
}

// private
//! Cease checking for eclipses from a particular platform.  This method is called when the platform is deleted from the simulation.
void WsfEclipseEventManager::CeaseMonitoring(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (mPlatformToCurrentEventIdMap.find(aPlatformPtr->GetNameId()) != mPlatformToCurrentEventIdMap.end())
   {
      mPlatformToCurrentEventIdMap.erase(aPlatformPtr->GetNameId());
   }
}

//! Set up the eclipse event (called when the platform is initialized or the satellite maneuvers).
//! @param aSimTime The current simulation time.
//! @param aId The id to be associated with this event.  The most recently initiated event for a given platform is
//! considered the current one.
//! @param aSpaceMover The space mover object associated with this eclipse.
void WsfEclipseEventManager::InitiateEclipseEvent(double aSimTime, size_t aId, WsfSpaceMoverBase& aSpaceMover)
{
   std::unique_ptr<EclipseEvent> eventPtr{nullptr};
   mPlatformToCurrentEventIdMap[aSpaceMover.GetPlatform()->GetNameId()] = aId;

   if (aSpaceMover.GetSimulation()->IsActive())
   {
      double timeToEntry, timeToExit;
      bool   solutionExists = (aSpaceMover.GetPropagator().GetEclipseTimes(timeToEntry, timeToExit));
      if (solutionExists)
      {
         if (timeToEntry > timeToExit)
         {
            eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cEXIT, aId, *this, &aSpaceMover);
            eventPtr->SetTime(aSimTime + timeToExit);
         }
         else
         {
            eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cENTRY, aId, *this, &aSpaceMover);
            eventPtr->SetTime(aSimTime + timeToEntry);
         }
      }
      else
      {
         // re-evaluate after one quarter orbit.
         double timeToEval =
            0.25 / (aSpaceMover.GetPropagator().GetOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cTWO_PI);
         eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cEVALUATE, aId, *this, &aSpaceMover);
         eventPtr->SetTime(aSimTime + timeToEval);
      }
   }
   else
   {
      // Re-evaluate immediately after the start of the simulation.
      eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cEVALUATE, aId, *this, &aSpaceMover);
      eventPtr->SetTime(aSimTime + 1.0e-6);
   }

   GetSimulation().AddEvent(std::move(eventPtr));
}

// private
//! Update the eclipse event due to changes in the orbit from maneuvering.
void WsfEclipseEventManager::UpdateMonitoring(double                 aSimTime,
                                              WsfSpaceMoverBase*     aSpaceMoverPtr,
                                              const WsfOrbitalEvent& aManeuver)
{
   InitiateEclipseEvent(aSimTime, aManeuver.GetUniqueId(), *aSpaceMoverPtr);
}

void WsfConfigureEclipseEventManger::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfEclipseEventManager>());
}

WsfEclipseEventManager::EclipseEvent::EclipseEvent(Type                    aType,
                                                   size_t                  aId,
                                                   WsfEclipseEventManager& aEclipseManager,
                                                   WsfSpaceMoverBase*      aSpaceMoverPtr)
   : WsfEvent()
   , mType(aType)
   , mId(aId)
   , mEclipseManager(aEclipseManager)
   , mSpaceMoverPtr(aSpaceMoverPtr)
   , mPlatformIndex(aSpaceMoverPtr->GetPlatform()->GetIndex())
{
}

//! Update eclipse data (entry and exit times) for a given platform.  If the orbit is not in eclipse at the current
//! time, re-evaluate after 1/4 orbit, as the satellite could move into shadow due to earth's rotation.
WsfEvent::EventDisposition WsfEclipseEventManager::EclipseEvent::Execute()
{
   // If we have maneuvered, there can be an old event scheduling the eclipses from the previous orbit.
   if (mEclipseManager.IsEnabled() && GetSimulation()->PlatformExists(mPlatformIndex) &&
       (mId == mEclipseManager.mPlatformToCurrentEventIdMap[mSpaceMoverPtr->GetPlatform()->GetNameId()]))
   {
      mSpaceMoverPtr->Update(GetTime()); // In case the mover was not up-to-date.
      double timeToEntry    = 0.0;
      double timeToExit     = 0.0;
      bool   solutionExists = mSpaceMoverPtr->GetPropagator().GetEclipseTimes(timeToEntry, timeToExit);
      if (solutionExists)
      {
         if (mType == cENTRY)
         {
            WsfObserver::EclipseEntry(GetSimulation())(GetTime(), mSpaceMoverPtr);

            // Switch to evaluate exit type after timeToExit.
            mType = cEXIT;
            SetTime(GetTime() + timeToExit);
         }
         else if (mType == cEXIT)
         {
            WsfObserver::EclipseExit(GetSimulation())(GetTime(), mSpaceMoverPtr);

            // Switch to evaluate entry type after timeToEntry.
            mType = cENTRY;
            SetTime(GetTime() + timeToEntry);
         }
         else // cEVALUATE
         {
            if (timeToEntry > timeToExit)
            {
               mType = cEXIT;
               SetTime(GetTime() + timeToExit);
            }
            else
            {
               mType = cENTRY;
               SetTime(GetTime() + timeToEntry);
            }
         }
      }
      else
      {
         mType = cEVALUATE;

         // Evaluate one quarter orbit later
         double timeToEval =
            0.25 /
            (mSpaceMoverPtr->GetPropagator().GetOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cTWO_PI);
         SetTime(GetTime() + timeToEval);
      }

      return WsfEvent::cRESCHEDULE;
   }
   else
   {
      return WsfEvent::cDELETE;
   }
}
