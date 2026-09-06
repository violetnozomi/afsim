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

#ifndef WSFSPACEEVENTPIPE_HPP
#define WSFSPACEEVENTPIPE_HPP

#include "WsfEvent.hpp"
#include "WsfEventPipe.hpp"

class UtOrbitalElements;
class WsfEventPipeInterface;
class WsfOrbitalEvent;
class WsfSpaceMoverBase;

namespace wsf
{
namespace space
{
class EventPipe : public WsfEventPipeLogger
{
public:
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);
   static void RegisterMessages(UtPackSerializer* aSerializerPtr);
   explicit EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);

   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

private:
   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);
   void ManeuverBegin(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver);
   void ManeuverEnd(double aSimTime, WsfSpaceMoverBase* aSpaceMover, const WsfOrbitalEvent& aManeuver);

   void UpdateElements(double aSimTime, WsfSpaceMoverBase* aSpaceMover);
   void OrbitalElementsUpdate(double                   aSimTime,
                              const WsfPlatform&       aPlatform,
                              const UtOrbitalElements& aOrbitalElements,
                              bool                     aManeuver = true);

   void Send(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendDependent(aSimTime, aPlatform, std::move(aMessagePtr));
   }

   class OrbitRedrawEvent : public WsfEvent
   {
   public:
      OrbitRedrawEvent(wsf::space::EventPipe& aEventPipe, size_t aPlatformIndex)
         : mEventPipe(aEventPipe)
         , mPlatformIndex{aPlatformIndex}
      {
      }

      ~OrbitRedrawEvent() override = default;

      WsfEvent::EventDisposition Execute() override;

   private:
      wsf::space::EventPipe& mEventPipe;
      size_t                 mPlatformIndex;
   };

   WsfSimulation*         mSimulationPtr;
   WsfEventPipeInterface* mEventPipePtr;

   UtCallbackHolder mSpaceCallback;

   int cSPACE;
};

class EventPipeInterface : public WsfSimulationExtension
{
public:
   static EventPipeInterface& Get(const WsfSimulation& aSimulation);
   EventPipeInterface();
   ~EventPipeInterface() override = default;

   void AddedToSimulation() override;

private:
   std::unique_ptr<EventPipe> mEventPipePtr;
};
} // namespace space
} // namespace wsf
#endif
