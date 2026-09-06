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

#include "WsfCommPhysicalLayer.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfComm.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommLayerEvent.hpp"
#include "WsfCommLayerMessage.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfCommRouter.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ==========================================================================
LayerImp* PhysicalLayer::Clone() const
{
   return new PhysicalLayer(*this);
}

// ==========================================================================
bool PhysicalLayer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "transfer_rate")
   {
      WsfRandomVariable temp(0.0, WsfRandomVariable::cPOSITIVE);
      temp.ProcessInput(aInput, UtInput::cDATA_RATE);
      mTransferRate = temp;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ============================================================================
bool PhysicalLayer::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify  = true;
   bool success = true;
   bool halt    = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->PhysicalReceive(aSimTime, GetIndex(), aMessage);
      halt        = result.first;
      notify &= result.second;
      if (halt)
      {
         break;
      }
   }

   aNotifyNextLayer = notify;
   return success;
}

// ============================================================================
bool PhysicalLayer::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify = true;
   bool halt   = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->PhysicalSend(aSimTime, GetIndex(), aMessage);
      halt        = result.first;
      notify &= result.second;

      if (halt)
      {
         break;
      }
   }

   // Short circuit if protocol usage indicates an abort.
   if (!notify)
   {
      // Notify the upper layer of failure.
      LayerEvent::ScheduleCommLayerEvent(aSimTime, aLayerIndex + 1, Parent(), layer::cUP_NACK_SEND, &aMessage);
      aNotifyNextLayer = false;
      return false;
   }

   // Get a pointer to the receiving comm device
   auto        networkManagerPtr = Parent()->GetSimulation()->GetCommNetworkManager();
   const auto& nextHop           = aMessage.SourceMessage()->GetNextHopAddr();
   auto        targetComm        = networkManagerPtr->GetComm(nextHop);

   if (!targetComm)
   {
      LayerEvent::ScheduleCommLayerEvent(aSimTime, aLayerIndex + 1, Parent(), layer::cUP_NACK_SEND, &aMessage);
      aNotifyNextLayer = false;
      return false;
   }

   // Let the user know a transmission has started in debug.
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Comm has started transmission of message.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Comm: " << Parent()->GetFullName();
      out.AddNote() << "Message: " << *aMessage.SourceMessage();
   }

   // Get the medium model being used to transmit.
   // If the destination is on our router, the router medium is used.
   medium::Medium* mediumPtr  = nullptr;
   auto            xmtrRouter = Parent()->GetRouter();
   auto            rcvrRouter = targetComm->GetRouter();
   if (xmtrRouter && rcvrRouter && (xmtrRouter == rcvrRouter))
   {
      auto mediumContainer = medium::ContainerComponent<router::Router>::Find(*xmtrRouter);
      if (mediumContainer)
      {
         mediumPtr = mediumContainer->GetMedium();
      }
   }
   else
   {
      auto mediumContainer = medium::ContainerComponent<Comm>::Find(*Parent());
      if (mediumContainer)
      {
         mediumPtr = mediumContainer->GetMedium();
      }
   }
   if (!mediumPtr)
   {
      aNotifyNextLayer = false;
      return false;
   }
   // Attempt transmission over the medium.
   auto result =
      mediumPtr->TransmitMessage(aSimTime, Parent()->GetProtocolStack().GetLayer(GetIndex()), aMessage, *Parent());
   if (result != medium::TransmissionResult::cSUCCESS)
   {
      LayerEvent::ScheduleCommLayerEvent(aSimTime, aLayerIndex + 1, Parent(), layer::cUP_NACK_SEND, &aMessage);
      aNotifyNextLayer = false;
      return false;
   }

   // There is no layer below so no need to notify.
   aNotifyNextLayer = false;

   // If made it to this point then indicate success.
   return true;
}

// ============================================================================
bool PhysicalLayer::ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr)
{
   bool success = true;
   switch (aLayerMessage.GetValue())
   {
   case layer::cDOWN_ACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cDOWN_ACK_SEND.GetValue():
   {
      // Typical datalink notification to begin transmission.
      auto notifyNextLayer = false;

      // Call this layer's send method.
      success = Send(aSimTime, GetIndex(), *aCommMessagePtr, notifyNextLayer);
      break;
   }
   case layer::cDOWN_NACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cDOWN_NACK_SEND.GetValue():
   {
      break;
   }
   case layer::cUP_ACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cUP_ACK_SEND.GetValue():
   {
      // Although there is typically no layers below the physical,
      // this is used for handling notification from the medium on
      // message transmission disposition. Pass this message up
      // the stack.
      LayerEvent::ScheduleCommLayerEvent(aSimTime, GetIndex() + 1, Parent(), layer::cUP_ACK_SEND, aCommMessagePtr);
      break;
   }
   case layer::cUP_NACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cUP_NACK_SEND.GetValue():
   {
      // Although there is typically no layers below the physical,
      // this is used for handling notification from the medium on
      // message transmission disposition. Pass this message up
      // the stack.
      LayerEvent::ScheduleCommLayerEvent(aSimTime, GetIndex() + 1, Parent(), layer::cUP_NACK_SEND, aCommMessagePtr);
      break;
   }
   default:
   {
      success = false;
      break;
   }
   }

   return success;
}

// ============================================================================
bool PhysicalLayer::CanReceiveFrom(double aSimTime, Comm& aXmtr, Result& aResult)
{
   auto componentPtr = ComponentHW::Find(*Parent());
   if (componentPtr)
   {
      return componentPtr->CanReceiveFrom(aSimTime, &aXmtr, aResult);
   }

   return false;
}

} // namespace comm
} // namespace wsf
