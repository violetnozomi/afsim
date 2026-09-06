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

#include "WsfLASER_XmtrRcvr.hpp"

#include "UtMemory.hpp"
#include "WsfCommComponentHW_LASER.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommPhysicalLayerLaser.hpp"

namespace wsf
{
namespace comm
{

// ****************************************************************************
// Public

// ============================================================================
LASER_XmtrRcvr::LASER_XmtrRcvr(WsfScenario& aScenario, CommType aCommType)
   : RadioXmtrRcvr(aScenario, aCommType)
{
   // Set the class ID.
   SetClassId("WSF_LASER_XMTR_RCVR");

   // Replace the physical layer with the laser-specific one.
   // While we know that the original implementation only has one
   // instance of the physical layer, there is no restriction on
   // repeating layers, so this is the proper way to handle this.
   // Note - clearing and re-adding layers is also a valid approach.
   auto physicalLayerPtr = GetProtocolStack().GetLayer<PhysicalLayer>();
   while (physicalLayerPtr)
   {
      mProtocolStack.RemoveLayer(physicalLayerPtr->GetIndex());
      physicalLayerPtr = GetProtocolStack().GetLayer<PhysicalLayer>();
   }

   auto layerLaserImp = ut::make_unique<PhysicalLayerLaser>();
   // The physical layer should be the lowest layer, such that it is first (index 0).
   mProtocolStack.InsertLayer(0U, ut::make_unique<Layer>(Layer::LayerType::cPHYSICAL, layerLaserImp.release()));

   // Remove the hardware component inserted by the base class.
   wsf::comm::ComponentHW* componentPtr = wsf::comm::ComponentHW::Find(*this);
   if (componentPtr != nullptr)
   {
      GetComponents().RemoveComponent(componentPtr);
      delete componentPtr;
   }

   // Add the correct hardware component.
   ComponentHW_LASER::FindOrCreate(*this);
}

// ============================================================================
// virtual
Comm* LASER_XmtrRcvr::Clone() const
{
   return new LASER_XmtrRcvr(*this);
}

} // namespace comm
} // namespace wsf
