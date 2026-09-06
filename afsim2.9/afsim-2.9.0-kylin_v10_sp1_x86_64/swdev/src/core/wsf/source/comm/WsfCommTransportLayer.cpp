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

#include "WsfCommTransportLayer.hpp"

#include "UtInput.hpp"
#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfMessage.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
LayerImp* TransportLayer::Clone() const
{
   return new TransportLayer(*this);
}

// ============================================================================
//! Receive the message in this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aXmtrPtr         [input] The comm device that sent the message.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool TransportLayer::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify  = true;
   bool success = true;
   bool halt    = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->TransportReceive(aSimTime, GetIndex(), aMessage);
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
//! Send the message to this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aLayerIndex      [input] The index in the protocol stack for this layer.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer below this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool TransportLayer::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   //! Indicate the transport features utilized with this message, for processing
   //! by other layers. In the future, this should probably be ported to a protocol
   //! implementation with more robust capabilities.
   aMessage.GetTransportFeatures() = mTransportFeature;

   bool notify  = true;
   bool success = true;
   bool halt    = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->TransportSend(aSimTime, GetIndex(), aMessage);
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
//! Process a layer event.
// virtual
bool TransportLayer::ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr)
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

// ============================================================================
//! Process Input for this layer.
//! @param aInput          [input] The UtInput object.
//! @return 'true' if input belongs to this layer.
bool TransportLayer::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   return myCommand;
}
} // namespace comm
} // namespace wsf
