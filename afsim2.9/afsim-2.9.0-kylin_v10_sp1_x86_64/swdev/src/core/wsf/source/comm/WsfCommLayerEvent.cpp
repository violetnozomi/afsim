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

#include "WsfCommLayerEvent.hpp"

#include <iostream>

#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ==========================================================================
void LayerEvent::ScheduleCommLayerEvent(double         aSimTime,
                                        size_t         aLayerIndex,
                                        Comm*          aXmtrPtr,
                                        layer::Message aMessage,
                                        Message*       aCommMessagePtr)
{
   aXmtrPtr->GetSimulation()->AddEvent(
      ut::make_unique<LayerEvent>(aSimTime, aLayerIndex, aXmtrPtr, aMessage, aCommMessagePtr));
}

//! Note: The message is cloned in a layer event, as these objects may be
//! scheduled beyond the lifetime of the passed message object. As a smart
//! pointer object, they are automatically deallocated when the protocol
//! event being fired is completed. If the passed message object requires
//! an extended lifetime, it must be cloned or moved from the event object.
// ==========================================================================
LayerEvent::LayerEvent(double aSimTime, size_t aLayerIndex, Comm* aXmtrPtr, layer::Message aMessage, Message* aCommMessagePtr)
   : WsfEvent(aSimTime)
   , mCommLayerIndex(aLayerIndex)
   , mXmtrPlatformIndex(aXmtrPtr->GetPlatform()->GetIndex())
   , mXmtrPtr(aXmtrPtr)
   , mLayerMessage(aMessage)
   , mCommMessagePtr(nullptr)
{
   if (aCommMessagePtr)
   {
      auto messagePtr = ut::clone(aCommMessagePtr);
      mCommMessagePtr = std::move(messagePtr);
   }
}

// ==========================================================================
WsfEvent::EventDisposition LayerEvent::Execute()
{
   EventDisposition eventDisposition = cDELETE;
   WsfPlatform*     xmtrPlatformPtr  = GetSimulation()->GetPlatformByIndex(mXmtrPlatformIndex);
   if (xmtrPlatformPtr != nullptr)
   {
      mXmtrPtr->ProcessLayerMessage(GetTime(), mCommLayerIndex, mLayerMessage, mCommMessagePtr.get());
   }
   return eventDisposition;
}

} // namespace comm
} // namespace wsf
