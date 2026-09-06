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

#include "WsfRadioXmtrRcvr.hpp"

#include "UtMemory.hpp"
#include "WsfCommApplicationLayer.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommDatalinkLayer.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommTransportLayer.hpp"

namespace wsf
{
namespace comm
{

// ****************************************************************************
// Public

// ============================================================================
RadioXmtrRcvr::RadioXmtrRcvr(WsfScenario& aScenario, CommType aCommType)
   : Comm(aScenario, aCommType)
{
   // Set the class ID
   SetClassId("WSF_RADIO_XMTR_RCVR");

   // Build the protocol stack from lower to upper layers
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cPHYSICAL, new PhysicalLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cDATALINK, new DatalinkLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cNETWORK, new NetworkLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cTRANSPORT, new TransportLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cAPPLICATION, new ApplicationLayer));

   // Add the hardware component
   ComponentHW::FindOrCreate(*this);

   // Configure medium usage for this model.
   auto containerPtr = medium::ContainerComponent<Comm>::Find(*this);
   if (!containerPtr)
   {
      throw UtException("Failed modification of medium container on WsfRadioXmtrRcvr construction.");
   }

   containerPtr->AddForbiddenMedium(medium::cGUIDED);
   containerPtr->AddValidMedium(medium::cUNGUIDED);
   containerPtr->SetDefaultMedium(medium::cUNGUIDED);
}

// ============================================================================
// virtual
Comm* RadioXmtrRcvr::Clone() const
{
   return new RadioXmtrRcvr(*this);
}

// ============================================================================
bool RadioXmtrRcvr::ReceiveSetup(double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   if (!Comm::ReceiveSetup(aSimTime, aXmtrPtr, aMessage))
   {
      return false;
   }

   // Because EM based communications are reliant upon position for correct
   // EM interaction calculations, ensure the comm position is updated at this time.
   WsfArticulatedPart::UpdatePosition(aSimTime);

   return true;
}

// ============================================================================
bool RadioXmtrRcvr::CanSendTo(double aSimTime, Comm* aRcvrPtr, Message* aMessagePtr /*=nullptr*/)
{
   //! Short circuit the checks for EM interactions and receiver interaction
   //! checks if this is a bridged connection on the same platform.
   auto routerPtr = GetRouter();
   if (routerPtr && routerPtr->HasInterface(aRcvrPtr))
   {
      return true;
   }

   if (!aRcvrPtr->CanReceive())
   {
      return false;
   }

   //! EM interactions and receiver interaction
   bool         canSend    = false;
   ComponentHW* xmtrHW_Ptr = ComponentHW::Find(*this);
   ComponentHW* rcvrHW_Ptr = ComponentHW::Find(*aRcvrPtr);
   if ((rcvrHW_Ptr != nullptr) && (xmtrHW_Ptr != nullptr))
   {
      if (rcvrHW_Ptr->CanInteractWith(this))
      {
         if (aMessagePtr != nullptr)
         {
            canSend = xmtrHW_Ptr->CanSendTo(aSimTime, aRcvrPtr, aMessagePtr->GetResult());
         }
         else
         {
            Result result;
            canSend = xmtrHW_Ptr->CanSendTo(aSimTime, aRcvrPtr, result);
         }
      }
   }

   return canSend;
}

} // namespace comm
} // namespace wsf
