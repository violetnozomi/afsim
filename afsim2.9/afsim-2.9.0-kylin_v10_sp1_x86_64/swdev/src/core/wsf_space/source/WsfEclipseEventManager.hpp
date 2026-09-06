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

#ifndef WSFECLIPSEEVENTMANAGER_HPP
#define WSFECLIPSEEVENTMANAGER_HPP

#include "wsf_space_export.h"

#include <map>

#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
class WsfSpaceMoverBase;
#include "WsfStringId.hpp"

class WsfOrbitalEvent;
class WsfPlatform;
class WsfSimuluation;

//! A class that optimizes eclipse event generation (ECLIPSE_ENTRY and ECLIPSE_EXIT).
//! Checks are only performed if these events have subscribers at the start of the simulation,
//! (typically WsfEventOutput, WsfCSV_EventOutput, and WsfScriptObserver), or if the Enable() method is called.
//! Events are scheduled at the times of eclipse entry and exit.  Times are re-evaluated (because of earth motion),
//! during these event executions.  If a satellite is not in eclipse on a given orbit, the times are re-evaluated
//! every one-quarter orbit for changes.
//!@note If there are no subscribers to the eclipse events at the start of a simulation, the Enable() method must be called
//! before subscribing during a running simulation.
class WSF_SPACE_EXPORT WsfEclipseEventManager : public WsfSimulationExtension
{
public:
   WsfEclipseEventManager();

   bool Initialize() override;

   bool IsEnabled() const { return mIsEnabled; }

   void Enable(double aSimTime);
   void Disable();

private:
   class EclipseEvent : public WsfEvent
   {
   public:
      enum Type
      {
         cENTRY,
         cEXIT,
         cEVALUATE
      };

      EclipseEvent(Type aType, size_t aId, WsfEclipseEventManager& aEclipseManager, WsfSpaceMoverBase* aSpaceMoverPtr);

      void                       SetType(Type aType) { mType = aType; }
      WsfEvent::EventDisposition Execute() override;

   private:
      Type                    mType;
      size_t                  mId;
      WsfEclipseEventManager& mEclipseManager;
      WsfSpaceMoverBase*      mSpaceMoverPtr;
      size_t                  mPlatformIndex;
   };

   void InitiateEclipseEvent(double aSimTime, size_t aId, WsfSpaceMoverBase& aSpaceMover);

   void InitiateMonitoring(double aSimTime, WsfPlatform* aPlatformPtr);

   void UpdateMonitoring(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver);

   void CeaseMonitoring(double aSimTime, WsfPlatform* aPlatformPtr);

   bool                          mIsEnabled;
   UtCallbackHolder              mCallbacks;
   std::map<WsfStringId, size_t> mPlatformToCurrentEventIdMap;
};

//! The scenario extension used to configure the Eclipse reports object.
class WsfConfigureEclipseEventManger : public WsfScenarioExtension
{
public:
   void SimulationCreated(WsfSimulation& aSimulation) override;
};
#endif
