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

#include "WsfXIO_CommComponent.hpp"

#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfPlatform.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio_sim/WsfXIO_AutoDisMapping.hpp"

namespace wsf
{
namespace xio
{

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
CommComponent* CommComponent::Find(const wsf::comm::Comm& aParent)
{
   CommComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<CommComponent>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
CommComponent* CommComponent::FindOrCreate(wsf::comm::Comm&       aParent,
                                           WsfXIO_AutoDisMapping* aDisMappingPtr,
                                           WsfXIO_Connection*     aConnectionPtr)
{
   CommComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new CommComponent(aDisMappingPtr, aConnectionPtr);
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
CommComponent::CommComponent(WsfXIO_AutoDisMapping* aDisMappingPtr, WsfXIO_Connection* aConnectionPtr)
   : Component()
   , mDisMappingPtr(aDisMappingPtr)
   , mConnectionPtr(aConnectionPtr)
{
}

// =================================================================================================
WsfComponent* CommComponent::CloneComponent() const
{
   return new CommComponent(*this);
}

// =================================================================================================
WsfStringId CommComponent::GetComponentName() const
{
   return UtStringIdLiteral("comm_component_xio");
}

// =================================================================================================
const int* CommComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_XIO, cWSF_COMPONENT_COMM, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* CommComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_XIO)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return (Component*)this;
   }
   return nullptr;
}

// =================================================================================================
bool CommComponent::Receive(double aSimTime, wsf::comm::Comm* aXmtrPtr, wsf::comm::Message& aMessage)
{
   if (mDisMappingPtr->CanTranslateMessage(aMessage.SourceMessage()->GetType()))
   {
      WsfXIO_DisMessagePkt pkt;
      pkt.mSimTime       = aSimTime;
      pkt.mMessagePtr    = aMessage.SourceMessage().get();
      pkt.mPlatformIndex = static_cast<int32_t>(GetComm()->GetPlatform()->GetIndex());
      pkt.mCommName      = GetComm()->GetNameId();
      mConnectionPtr->Send(pkt);
   }
   else
   {
      auto out = ut::log::error() << "Cannot transmit WsfMessage over XIO.";
      out.AddNote() << "Message: " << aMessage.SourceMessage()->GetType();
   }
   return false;
}

} // namespace xio
} // namespace wsf
