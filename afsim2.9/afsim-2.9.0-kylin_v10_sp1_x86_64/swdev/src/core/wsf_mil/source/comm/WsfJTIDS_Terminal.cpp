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

#include "WsfJTIDS_Terminal.hpp"

#include "UtAttributeContainer.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommComponentHW_JTIDS.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommPhysicalLayerJTIDS.hpp"
#include "WsfCommResult.hpp"
#include "WsfCommSlotGroupLayerJTIDS.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

// ****************************************************************************
// Public

// ============================================================================
WsfJTIDS_Terminal::WsfJTIDS_Terminal(WsfScenario& aScenario)
   : Comm(aScenario)
   , MessageSent()
   , mCallbacks()
{
   // Set the class ID
   SetClassId("WSF_JTIDS_TERMINAL");

   // Build the protocol stack from lower to upper layers
   mProtocolStack.AddLayer(
      ut::make_unique<wsf::comm::Layer>(wsf::comm::Layer::LayerType::cPHYSICAL, new wsf::comm::PhysicalLayerJTIDS));
   mProtocolStack.AddLayer(
      ut::make_unique<wsf::comm::Layer>(wsf::comm::Layer::LayerType::cDATALINK, new wsf::comm::SlotGroupLayerJTIDS));

   // Add the hardware component
   wsf::comm::ComponentHW_JTIDS::FindOrCreate(*this);

   //! Set all JTIDS comm objects to exist on the JTIDS network
   mInputNetwork = "JTIDS";
}

// virtual
WsfJTIDS_Terminal::~WsfJTIDS_Terminal()
{
   mCallbacks.Clear();
}

// ============================================================================
// static
WsfStringId WsfJTIDS_Terminal::GetJTIDSClassId()
{
   return "WSF_JTIDS_TERMINAL";
}

// ============================================================================
// virtual
wsf::comm::Comm* WsfJTIDS_Terminal::Clone() const
{
   return new WsfJTIDS_Terminal(*this);
}

// ============================================================================
// virtual
bool WsfJTIDS_Terminal::Initialize(double aSimTime)
{
   // Call base class first
   bool ok = Comm::Initialize(aSimTime);

   // Add a call back to notify when a slot group sent a message
   auto slotGroupLayerPtr = GetProtocolStack().GetLayer<wsf::comm::SlotGroupLayerJTIDS>();
   if (slotGroupLayerPtr != nullptr)
   {
      mCallbacks.Add(slotGroupLayerPtr->MessageSent.Connect(&WsfJTIDS_Terminal::HandleMessageSent, this));
   }

   return ok;
}

// ============================================================================
// virtual
void WsfJTIDS_Terminal::TurnOff(double aSimTime)
{
   wsf::comm::Comm::TurnOff(aSimTime);

   auto slotGroupLayerPtr = GetProtocolStack().GetLayer<wsf::comm::SlotGroupLayerJTIDS>();
   if (slotGroupLayerPtr != nullptr)
   {
      slotGroupLayerPtr->TurnOff(aSimTime);
   }
}

// ============================================================================
bool WsfJTIDS_Terminal::ReceiveSetup(double aSimTime, wsf::comm::Comm* aXmtrPtr, wsf::comm::Message& aMessage)
{
   if (!aXmtrPtr)
   {
      return false;
   }

   // Do not receive from non-JTIDS comms.
   if (aXmtrPtr->GetClassId() != GetJTIDSClassId())
   {
      return false;
   }

   return wsf::comm::Comm::ReceiveSetup(aSimTime, aXmtrPtr, aMessage);
}

// ============================================================================
bool WsfJTIDS_Terminal::SendSetup(double aSimTime, wsf::comm::Message& aMessage, const wsf::comm::Address& aAddress)
{
   if (!wsf::comm::Comm::SendSetup(aSimTime, aMessage, aAddress))
   {
      return false;
   }

   // Assign to the message group the command chain information needed by the layers
   // to determine which slot group to use.
   aMessage.GetAuxData().AssignString("command_chain", aMessage.SourceMessage()->GetGroup());

   return true;
}

// ****************************************************************************
bool WsfJTIDS_Terminal::ReceiveActions(double aSimTime, bool aReceiveStatus, Comm* aXmtrPtr, wsf::comm::Message& aMessage)
{
   if (aMessage.GetResult().mCheckedStatus != 0)
   {
      WsfObserver::MessageDeliveryAttempt(
         GetSimulation())(aSimTime, aXmtrPtr, this, *aMessage.SourceMessage(), aMessage.GetResult());
      if (aMessage.GetResult().GetTransmitter() != nullptr)
      {
         aMessage.GetResult().GetTransmitter()->NotifyListeners(aSimTime, aMessage.GetResult()); // Inform listeners for
                                                                                                 // possible ESM detection
      }
   }

   if (aReceiveStatus)
   {
      WsfObserver::MessageReceived(
         GetSimulation())(aSimTime, aXmtrPtr, this, *aMessage.SourceMessage(), aMessage.GetResult());
      SendMessage(aSimTime, *aMessage.SourceMessage()); // Send the message to on-board recipients
   }

   return aReceiveStatus;
}

// ****************************************************************************
// Protected

// ****************************************************************************
void WsfJTIDS_Terminal::PrintDebugReception(double aSimTime, const wsf::comm::Message& aMessage) const
{
   // Get the transmitter.
   const auto& sender  = aMessage.SourceMessage()->GetSrcAddr();
   auto        xmtrPtr = GetSimulation()->GetCommNetworkManager()->GetComm(sender);

   auto out = ut::log::debug() << "Xmtr and Rcvr:";
   out.AddNote() << "T = " << aSimTime;
   if (xmtrPtr)
   {
      out.AddNote() << "Xmtr: " << xmtrPtr->GetPlatform()->GetName();
   }
   else
   {
      out.AddNote() << "Xmtr: NOT FOUND";
   }

   out.AddNote() << "Rcvr: " << GetPlatform()->GetName();
   out.AddNote() << "Message: " << aMessage.SourceMessage()->GetSerialNumber();
   out.AddNote() << "Destination: " << aMessage.SourceMessage()->GetDstAddr().GetAddress();
}

// ============================================================================
WsfJTIDS_Terminal::WsfJTIDS_Terminal(const WsfJTIDS_Terminal& aSrc)
   : Comm(aSrc)
   , MessageSent()
   , mCallbacks()
{
}

// ****************************************************************************
// Private

// ============================================================================
void WsfJTIDS_Terminal::HandleMessageSent(double              aSimTime,
                                          const WsfMessage*   aMessagePtr,
                                          int                 aSlotNumber,
                                          WsfJTIDS_SlotGroup* aSlotGroup)
{
   // Just forward the message on from the slot group
   MessageSent(aSimTime, aMessagePtr, aSlotNumber, aSlotGroup);
}
