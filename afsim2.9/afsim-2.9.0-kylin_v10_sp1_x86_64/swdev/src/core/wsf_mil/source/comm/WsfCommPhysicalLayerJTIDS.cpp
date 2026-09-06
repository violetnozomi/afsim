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

#include "WsfCommPhysicalLayerJTIDS.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommResult.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ==========================================================================
LayerImp* PhysicalLayerJTIDS::Clone() const
{
   return new PhysicalLayerJTIDS(*this);
}

// ============================================================================
//! Receive the message in this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aXmtrPtr         [input] The comm device that sent the message.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool PhysicalLayerJTIDS::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   // Default return value
   bool ok = false;

   if (aXmtrPtr != nullptr)
   {
      ok = CanReceiveFrom(aSimTime, *aXmtrPtr, aMessage.GetResult());
   }

   aNotifyNextLayer = ok;
   return ok;
}

// ============================================================================
//! Send the message to this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aLayerIndex      [input] The index in the protocol stack for this layer.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer below this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool PhysicalLayerJTIDS::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   // There is no layer below so no need to notify
   aNotifyNextLayer = false;

   // If made it to this point then send succeeded
   return true;
}

// ============================================================================
//! Process a layer event.
// virtual
bool PhysicalLayerJTIDS::ProcessLayerMessage(double aSimTime, layer::Message aMessage, Message* aCommMessagePtr)
{
   return false;
}

} // namespace comm
} // namespace wsf
