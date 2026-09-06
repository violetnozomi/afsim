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

#include "WsfSpaceEventPipe.hpp"

#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtPackReflect.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceEventPipeClasses.hpp"
#include "WsfSpaceEventPipeClassesRegister.hpp"
#include "WsfSpaceEventPipeSchema.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceObserver.hpp"

void wsf::space::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfSpaceEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("SPACE", true);

   // Register the callback that gets invoked by WsfEventPipeInterface::AddedToSimulation to register
   // our message types with the serializer.
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

//! A callback from WsfEventPipeInterface::AddedToSimulation to register our message types with the serializer.
void wsf::space::EventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_space_events_types(*aSerializerPtr);
}

wsf::space::EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   cSPACE = aEventPipeInterfacePtr->GetEventId("SPACE");
}

void wsf::space::EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                                const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(cSPACE))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cSPACE))
      {
         mSpaceCallback += WsfObserver::PlatformInitialized(mSimulationPtr).Connect(&EventPipe::PlatformAdded, this);
         mSpaceCallback += WsfObserver::OrbitColorChanged(mSimulationPtr).Connect(&EventPipe::UpdateElements, this);
         mSpaceCallback += WsfObserver::OrbitalManeuverCompleted(mSimulationPtr).Connect(&EventPipe::ManeuverEnd, this);
         mSpaceCallback += WsfObserver::OrbitalManeuverCanceled(mSimulationPtr).Connect(&EventPipe::ManeuverEnd, this);
         mSpaceCallback += WsfObserver::OrbitalManeuverInitiated(mSimulationPtr).Connect(&EventPipe::ManeuverBegin, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cSPACE))
   {
      mSpaceCallback.Clear();
   }
}

void wsf::space::EventPipe::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   WsfMover* mover = aPlatformPtr->GetMover();
   if (mover != nullptr && mover->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE)
   {
      WsfSpaceMoverBase* spaceMoverPtr = dynamic_cast<WsfSpaceMoverBase*>(mover);
      if (spaceMoverPtr)
      {
         OrbitalElementsUpdate(aSimTime, *aPlatformPtr, spaceMoverPtr->GetOrbitalState().GetOrbitalElements());

         double updateInterval = spaceMoverPtr->GetElementOutputUpdateInterval();
         if (updateInterval > 0.0)
         {
            auto eventPtr = ut::make_unique<OrbitRedrawEvent>(*this, aPlatformPtr->GetIndex());
            eventPtr->SetTime(aSimTime + updateInterval);
            mSimulationPtr->AddEvent(std::move(eventPtr));
         }
      }
   }
}

void wsf::space::EventPipe::ManeuverBegin(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver)
{
   // begin event updates at a rate appropriate for the maneuver
}

void wsf::space::EventPipe::ManeuverEnd(double aSimTime, WsfSpaceMoverBase* aSpaceMover, const WsfOrbitalEvent& aManeuver)
{
   OrbitalElementsUpdate(aSimTime, *aSpaceMover->GetPlatform(), aSpaceMover->GetOrbitalState().GetOrbitalElementsTOD());
   // end any event updates
}

void wsf::space::EventPipe::UpdateElements(double aSimTime, WsfSpaceMoverBase* aSpaceMover)
{
   OrbitalElementsUpdate(aSimTime, *aSpaceMover->GetPlatform(), aSpaceMover->GetOrbitalState().GetOrbitalElementsTOD());
}

void wsf::space::EventPipe::OrbitalElementsUpdate(double                   aSimTime,
                                                  const WsfPlatform&       aPlatform,
                                                  const UtOrbitalElements& aOrbitalElements,
                                                  bool                     aManeuver)
{
   auto msgPtr = ut::make_unique<WsfEventPipe::MsgOrbitalElements>();
   msgPtr->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
   msgPtr->semiMajorAxis(aOrbitalElements.GetSemiMajorAxis());
   msgPtr->eccentricity(aOrbitalElements.GetEccentricity());
   msgPtr->trueAnomaly(aOrbitalElements.GetTrueAnomaly());
   msgPtr->raan(aOrbitalElements.GetRAAN());
   msgPtr->inclination(aOrbitalElements.GetInclination());
   msgPtr->argumentOfPeriapsis(aOrbitalElements.GetArgumentOfPeriapsis());
   msgPtr->resultOfManeuver(aManeuver);
   auto  moverPtr   = dynamic_cast<WsfSpaceMoverBase*>(aPlatform.GetMover());
   auto  orbitColor = moverPtr->GetOrbitColorValue();
   float redF, blueF, greenF;
   orbitColor.Get(redF, greenF, blueF);
   WsfEventPipe::Vec3f color;
   color.x(redF);
   color.y(greenF);
   color.z(blueF);
   msgPtr->orbitColor(color);
   msgPtr->orbitColorValid(moverPtr->OrbitColorSet());

   Send(aSimTime, aPlatform, std::move(msgPtr));
}

// =================================================================================================
//! Return a reference to the simulation extension.
// static
wsf::space::EventPipeInterface& wsf::space::EventPipeInterface::Get(const WsfSimulation& aSimulation)
{
   WsfSimulationExtension* extensionPtr = aSimulation.FindExtension("wsf_space_eventpipe");
   return static_cast<EventPipeInterface&>(*extensionPtr);
}

wsf::space::EventPipeInterface::EventPipeInterface()
   : mEventPipePtr(nullptr)
{
}

// =================================================================================================
void wsf::space::EventPipeInterface::AddedToSimulation()
{
   // If the event_pipe extension is available, hook in the handers for our events.
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }
}

WsfEvent::EventDisposition wsf::space::EventPipe::OrbitRedrawEvent::Execute()
{
   WsfEvent::EventDisposition retval{WsfEvent::EventDisposition::cDELETE};

   if (mEventPipe.mSimulationPtr->PlatformExists(mPlatformIndex))
   {
      WsfPlatform* platformPtr = mEventPipe.mSimulationPtr->GetPlatformByIndex(mPlatformIndex);
      if (platformPtr->GetSpatialDomain() == WsfSpatialDomain::WSF_SPATIAL_DOMAIN_SPACE)
      {
         double eventTime = GetTime();
         platformPtr->Update(eventTime);

         WsfSpaceMoverBase* moverPtr = dynamic_cast<WsfSpaceMoverBase*>(platformPtr->GetMover());
         if (moverPtr)
         {
            mEventPipe.OrbitalElementsUpdate(eventTime, *platformPtr, moverPtr->GetOrbitalState().GetOrbitalElements(), false);
            SetTime(eventTime + moverPtr->GetElementOutputUpdateInterval());
            retval = WsfEvent::EventDisposition::cRESCHEDULE;
         }
      }
   }

   return retval;
}
