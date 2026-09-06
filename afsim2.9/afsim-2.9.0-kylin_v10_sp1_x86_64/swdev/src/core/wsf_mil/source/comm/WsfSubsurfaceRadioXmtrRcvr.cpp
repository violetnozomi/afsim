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

#include "WsfSubsurfaceRadioXmtrRcvr.hpp"

#include "WsfCommComponentHW.hpp"
#include "WsfCommComponentHW_Subsurface.hpp"
#include "WsfScenario.hpp"

// ============================================================================
WsfSubsurfaceRadioXmtrRcvr::WsfSubsurfaceRadioXmtrRcvr(WsfScenario& aScenario, CommType aCommType)
   : wsf::comm::RadioXmtrRcvr(aScenario, aCommType)
{
   // Set the class ID
   SetClassId("WSF_SUBSURFACE_RADIO_XMTR_RCVR");

   // Remove the hardware component
   wsf::comm::ComponentHW* componentPtr = wsf::comm::ComponentHW::Find(*this);
   if (componentPtr != nullptr)
   {
      GetComponents().RemoveComponent(componentPtr);
      delete componentPtr;
   }

   // Add new hardware component
   wsf::comm::ComponentHW_Subsurface::FindOrCreate(*this);
}

// ============================================================================
wsf::comm::Comm* WsfSubsurfaceRadioXmtrRcvr::Clone() const
{
   return new WsfSubsurfaceRadioXmtrRcvr(*this);
}
