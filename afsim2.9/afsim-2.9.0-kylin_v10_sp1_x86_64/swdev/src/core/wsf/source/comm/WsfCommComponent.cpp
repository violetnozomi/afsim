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

#include "WsfCommComponent.hpp"

#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
WsfPlatform* Component::GetPlatform() const
{
   return GetComponentParent()->GetPlatform();
}

// =================================================================================================
WsfSimulation* Component::GetSimulation() const
{
   return GetComponentParent()->GetSimulation();
}

// =================================================================================================
bool Component::CanInteractWith(const Comm* aXmtrPtr)
{
   if (GetComm()->GetClassId() == aXmtrPtr->GetClassId())
   {
      return true;
   }
   return false;
}

// =================================================================================================
//! Called by model implementations when attempting to send a message.
void Component::AttemptToTransmit(Comm& aComm, double aSimTime, Result& aResult)
{
   for (const auto& component : aComm.GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->AttemptToTransmit(aSimTime, aResult);
      }
   }
}

// =================================================================================================
//! Called by model implementations when attempting to receive a message.
void Component::AttemptToReceive(Comm& aComm, double aSimTime, Result& aResult)
{
   for (const auto& component : aComm.GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->AttemptToReceive(aSimTime, aResult);
      }
   }
}

// =================================================================================================
bool Component::Receive(Comm& aComm, double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   bool ok = true;
   for (const auto& component : aComm.GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         ok &= commComponentPtr->Receive(aSimTime, aXmtrPtr, aMessage);
      }
   }

   return ok;
}

// =================================================================================================
bool Component::Send(Comm& aComm, double aSimTime, const WsfMessage& aMessage, const Address& aAddress)
{
   bool ok = true;
   for (const auto& component : aComm.GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         ok &= commComponentPtr->Send(aSimTime, aMessage, aAddress);
      }
   }

   return ok;
}

} // namespace comm
} // namespace wsf
