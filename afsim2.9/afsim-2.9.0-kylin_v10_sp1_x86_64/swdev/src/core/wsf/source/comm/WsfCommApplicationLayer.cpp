// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommApplicationLayer.hpp"

#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ****************************************************************************
// Public

// ============================================================================
LayerImp* ApplicationLayer::Clone() const
{
   return new ApplicationLayer(*this);
}

// ============================================================================
//! Receive the message in this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aXmtrPtr         [input] The comm device that sent the message.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool ApplicationLayer::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify  = true;
   bool halt    = false;
   bool success = true;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->ApplicationReceive(aSimTime, GetIndex(), aMessage);
      notify &= result.second;
      halt = result.first;
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
bool ApplicationLayer::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   bool notify  = true;
   bool success = true;
   bool halt    = false;

   auto protocols = Parent()->GetSortedProtocols();
   for (auto protocol : protocols)
   {
      auto result = protocol->ApplicationSend(aSimTime, GetIndex(), aMessage);
      notify &= result.second;
      halt = result.first;
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
bool ApplicationLayer::ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr)
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

} // namespace comm
} // namespace wsf
