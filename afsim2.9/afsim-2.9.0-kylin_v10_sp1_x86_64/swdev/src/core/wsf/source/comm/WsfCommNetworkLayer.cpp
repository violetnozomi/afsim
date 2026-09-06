// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommNetworkLayer.hpp"

#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommTransportLayer.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
LayerImp* NetworkLayer::Clone() const
{
   return new NetworkLayer(*this);
}

// ============================================================================
// virtual
bool NetworkLayer::Initialize2(double aSimTime)
{
   return true;
}

// ============================================================================
//! Receive the message in this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aXmtrPtr         [input] The comm device that sent the message.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool NetworkLayer::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   bool myMessage = false;

   // The comm interface protocols determine if the received message actually
   // belongs to this interface, i.e. if this comm is the destination of this
   // message. Further layer processing only occurs beyond this point in the
   // stack if the message is determined to be destined for this comm.
   //
   // The router (and its protocols) are queried regardless of the above outcome.
   // The message may be destined for this interface or not, but it may still
   // need to be retransmitted/routed/forwarded elsewhere.
   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->NetworkReceive(aSimTime, GetIndex(), aMessage);

      myMessage |= result.second;

      if (result.first)
      {
         // Other protocols should be prevented from processing
         break;
      }
   }

   // If any of the existing protocols failed to take ownership and indicate the
   // message was destined for this interface, do the default check to see
   // if the message destination address matches this interface.
   if (aMessage.SourceMessage()->GetDstAddr() == Parent()->GetAddress())
   {
      myMessage = true;
   }

   // If this message belongs to this interface, we will notify the next layer
   aNotifyNextLayer = myMessage;

   // Inform the router of the message reception. Even if the message should not
   // be processed further in the receive stack, the message may require forwarding/routing
   // to another destination. In addition, a message destined for this interface may also
   // still require additional forwarding due to router protocols (such as multicast).
   // This only occurs with an active router associated with this interface.
   auto routerPtr = Parent()->GetRouter();
   if (routerPtr && routerPtr->IsActive())
   {
      bool abortForward = false;

      // If the router receive method returns true, this message requires forwarding.
      // We need to push this message over to the send side of the stack and process
      // the message so it can get to the next hops(s). Even if the router's protocols
      // don't indicate the need to forward a message for their utility, we still
      // attempt a message transmission if this message is not destined for this interface.
      if (routerPtr->Receive(aSimTime, aXmtrPtr, Parent(), aMessage, abortForward) || !myMessage)
      {
         // Some protocols use unique addressing that would always result in non-matching
         // addressing. While the default behavior is to forward such messages, the protocol
         // may prevent this by setting abortForward to true.
         if (!abortForward)
         {
            ProcessLayerMessage(aSimTime, layer::cNETWORK_FORWARD, &aMessage);
         }
      }
   }

   return myMessage;
}

// ============================================================================
//! Send the message to this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aLayerIndex      [input] The index in the protocol stack for this layer.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer below this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool NetworkLayer::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify  = true;
   bool success = true;
   bool halt    = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->NetworkSend(aSimTime, GetIndex(), aMessage);
      halt        = result.first;
      notify &= result.second;
      if (halt)
      {
         break;
      }
   }

   aNotifyNextLayer = notify;

   // Allow the router to determine the best path to the destination, if applicable.
   auto routerPtr = Parent()->GetRouter();
   if (routerPtr)
   {
      // Package the data for the router
      router::Router::SendData data;
      data.SetXmtr(Parent());
      data.GetMessages().push_back(aMessage);

      success &= routerPtr->Send(aSimTime, data);

      // If the router returned multiple messages, such as in the case of broadcast,
      // multicast, or other multiple message protocol actions, send those messages as well.
      if (success)
      {
         for (size_t i = 1; i < data.GetMessages().size(); ++i)
         {
            auto additionalMessagePtr = ut::clone(data.GetMessages()[i]);
            ProcessLayerMessage(aSimTime, layer::cNETWORK_MULTIPLE_SEND, additionalMessagePtr.get());
         }

         aMessage = data.GetMessages().front();
      }
      aNotifyNextLayer &= success;
   }
   else
   {
      aNotifyNextLayer = false;
      success          = false;
   }

   // If message processing failed, this message will not be forwarded to subsequent layers,
   // and is being dropped.
   if (!success)
   {
      // This is a placeholder for transport layer capabilities.
      // Future capability should provide an appropriate layer event
      // and the conversion of transport layer capabilities into a proper
      // comm protocol/component.
   }

   return success;
}

// ============================================================================
//! Process a layer event.
// virtual
bool NetworkLayer::ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr)
{
   bool success = true;
   switch (aLayerMessage.GetValue())
   {
   case layer::cNETWORK_FORWARD.GetValue():
   {
      // A message was received that needs forwarded to another interface.
      // This message is processed in this layer normally as if it originated
      // from this interface, except the message is now being sent instead of
      // received.
      // <NetworkLayer>::Receive() -> <NetworkLayer>::Send()
      auto index    = GetIndex();
      auto layerPtr = Parent()->GetProtocolStack().GetLayer(index);

      if (layerPtr)
      {
         // Notify the observer
         auto destinationPtr =
            Parent()->GetSimulation()->GetCommNetworkManager()->GetComm(aCommMessagePtr->SourceMessage()->GetDstAddr());
         WsfObserver::MessageHop(
            Parent()->GetSimulation())(aSimTime, Parent(), destinationPtr, *aCommMessagePtr->SourceMessage());
         aCommMessagePtr->GetTraceRoute().push_back(Parent()->GetAddress());
         layerPtr->Send(aSimTime, *aCommMessagePtr);
      }

      break;
   }
   case layer::cNETWORK_MULTIPLE_SEND.GetValue():
   {
      // While being sent, a message was determined to required multiple
      // instances of that message in order to satisfy the send request.
      // Any additional messages are provided here so that they can be sent as
      // well without traversing the entire stack. This message will be sent
      // directly to the next layer for processing.
      auto index = GetIndex();
      --index;
      auto layerPtr = Parent()->GetProtocolStack().GetLayer(index);

      if (layerPtr)
      {
         layerPtr->Send(aSimTime, *aCommMessagePtr);
      }

      break;
   }
   case layer::cDOWN_ACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cDOWN_ACK_SEND.GetValue():
   {
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
      break;
   }
   case layer::cUP_NACK_RECEIVE.GetValue():
   {
      break;
   }
   case layer::cUP_NACK_SEND.GetValue():
   {
      break;
   }
   default:
   {
      success = false;
   }
   }

   return success;
}

} // namespace comm
} // namespace wsf
