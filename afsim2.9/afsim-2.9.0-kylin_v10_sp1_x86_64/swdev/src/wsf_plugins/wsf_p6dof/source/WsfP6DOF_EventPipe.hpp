// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFP6DOF_EVENTPIPE_HPP
#define WSFP6DOF_EVENTPIPE_HPP

#include "WsfEventPipe.hpp"

class WsfP6DOF_Mover;
class WsfEventPipeInterface;

namespace wsf
{
namespace p6dof
{
class EventPipe : public WsfEventPipeLogger
{
public:
   /** Register the event pipe events for P6dof data.
    * @param aEventPipeExtension event pipe to register with
    */
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);

   /** Register the messages for the serializer.
    * @param aSerializerPtr serializer to register with
    */
   static void RegisterMessages(UtPackSerializer* aSerializerPtr);

   explicit EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);

   /** Update callback subscriptions.
    * @param aNewCombinedOptions new options
    * @param aPreviousCombinedOptions previous options
    */
   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

private:
   /** Callback to send a p6dof message through the event pipe.
    * @param aSimTime sim time to send message at
    * @param aPlatform platform owning the message
    * @param aMover mover to pull data from when sending message
    * @param aMsgType type of p6dof message to send
    */
   void P6DofSendMessage(double aSimTime, const WsfPlatform& aPlatform, WsfP6DOF_Mover& aMover, const std::string& aMsgType);

   /** Send a message through the event pipe
    * @param aSimTime sim time to send the message
    * @param aMessagePtr message to send
    */
   void Send(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendDependent(aSimTime, aPlatform, std::move(aMessagePtr));
   }

   WsfSimulation*         mSimulationPtr; ///< Pointer to the simulation
   WsfEventPipeInterface* mEventPipePtr;  ///< Pointer to the Event Pipe Interface
   UtCallbackHolder       mP6dofCallback; ///< Callback list
   int                    cP6DOF;         ///< Id of the P6DOF Event Option
};

class EventPipeInterface : public WsfSimulationExtension
{
public:
   static EventPipeInterface& Get(const WsfSimulation& aSimulation);
   EventPipeInterface();
   ~EventPipeInterface() override = default;

   /** Calls to make when this interface is added to the simulation. */
   void AddedToSimulation() override;

private:
   std::unique_ptr<EventPipe> mEventPipePtr; ///< Pointer to the Event Pipe
};
} // namespace p6dof
} // namespace wsf
#endif
