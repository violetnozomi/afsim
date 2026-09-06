// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFEVENTPIPE_HPP
#define WSFSIXDOFEVENTPIPE_HPP

#include "WsfEventPipe.hpp"
class WsfEventPipeInterface;

namespace wsf
{
namespace six_dof
{
class Mover;

class EventPipe : public WsfEventPipeLogger
{
public:
   /** Register the event pipe events for SixDOF data.
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
   /** Callback to send a SixDOF message through the event pipe.
    * @param aSimTime sim time to send message at
    * @param aPlatform platform owning the message
    * @param aMover mover to pull data from when sending message
    * @param aMsgType type of SixDOF message to send
    */
   void SixDOF_SendMessage(double aSimTime, const WsfPlatform& aPlatform, Mover& aMover, const std::string& aMsgType);

   /** Send a message through the event pipe
    * @param aSimTime sim time to send the message
    * @param aMessagePtr message to send
    */
   void Send(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendDependent(aSimTime, aPlatform, std::move(aMessagePtr));
   }

   WsfSimulation*         mSimulationPtr;   ///< Pointer to the simulation
   WsfEventPipeInterface* mEventPipePtr;    ///< Pointer to the Event Pipe Interface
   UtCallbackHolder       mSixDOF_Callback; ///< Callback list
   int                    cSixDOF;          ///< Id of the SixDOF Event Option
};

class EventPipeInterface : public WsfSimulationExtension
{
public:
   static EventPipeInterface& Get(const WsfSimulation& aSimulation);
   EventPipeInterface();
   ~EventPipeInterface() = default;

   /** Calls to make when this interface is added to the simulation. */
   void AddedToSimulation() override;

private:
   std::unique_ptr<EventPipe> mEventPipePtr; ///< Pointer to the Event Pipe
};
} // namespace six_dof
} // namespace wsf
#endif
