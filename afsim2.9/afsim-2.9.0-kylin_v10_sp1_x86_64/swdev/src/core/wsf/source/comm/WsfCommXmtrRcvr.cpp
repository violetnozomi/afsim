// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommXmtrRcvr.hpp"

#include "UtMemory.hpp"
#include "WsfCommApplicationLayer.hpp"
#include "WsfCommDatalinkLayer.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "WsfCommTransportLayer.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
XmtrRcvr::XmtrRcvr(const WsfScenario& aScenario, CommType aCommType)
   : Comm(aScenario, aCommType)
{
   // Set the class ID
   SetClassId("WSF_COMM_XMTR_RCVR");

   // Build the protocol stack from lower to upper layers
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cPHYSICAL, new PhysicalLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cDATALINK, new DatalinkLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cNETWORK, new NetworkLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cTRANSPORT, new TransportLayer));
   mProtocolStack.AddLayer(ut::make_unique<Layer>(Layer::LayerType::cAPPLICATION, new ApplicationLayer));

   // Configure medium usage for this model.
   auto containerPtr = medium::ContainerComponent<Comm>::Find(*this);
   if (!containerPtr)
   {
      throw UtException("Failed modification of medium container on WsfCommXmtrRcvr construction.");
   }

   containerPtr->AddForbiddenMedium(medium::cUNGUIDED);
}

// ============================================================================
// virtual
Comm* XmtrRcvr::Clone() const
{
   return new XmtrRcvr(*this);
}

} // namespace comm
} // namespace wsf
