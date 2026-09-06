// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfUCI_Computer.hpp"

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"

#include "uci/base/Accessor.h"

#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_InterfaceExtension.hpp"
#include "WsfUCI_MessageBaseType.hpp"
#include "WsfUCI_MessageFactory.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{

UCI_Computer::UCI_Computer(WsfScenario& aScenario)
   : UCI_Component(aScenario,
                   cWSF_COMPONENT_UCI_COMPUTER)
{
}

UCI_Computer::UCI_Computer(const UCI_Computer& aSrc)
   : UCI_Component(aSrc)
{
}

bool UCI_Computer::PreInitialize(double aSimTime)
{
   return RegisterWithInterface(aSimTime) && WsfPlatformPart::PreInitialize(aSimTime);
}

bool UCI_Computer::Initialize(double aSimTime)
{
   auto uciInterfaceExtensionPtr = UCI_InterfaceExtension::Get(GetSimulation());
   auto uciInterfacePtr = uciInterfaceExtensionPtr->GetInterface(); 
   mMessageFactoryPtr = ut::make_unique<UCI_MessageFactory>(*uciInterfacePtr);
   return UCI_Component::Initialize(aSimTime);
}

const int* UCI_Computer::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL,
                                cWSF_COMPONENT_UCI_COMPUTER
                              };
   return roles;
}

// virtual
WsfComponent* UCI_Computer::CloneComponent() const
{
   return new UCI_Computer(*this);
}

// virtual
void* UCI_Computer::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_COMPUTER)
   {
      return this;
   }
   else if (aRole == cWSF_COMPONENT_UCI)
   {
      return static_cast<UCI_Component*>(this);
   }
   else if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return static_cast<WsfPlatformPart*>(this);
   }
   else
   {
      return nullptr;
   }
}

bool UCI_Computer::ProcessMessage(double            aSimTime,
                                  const WsfMessage& aMessage)
{
   if (aSimTime >= GetSimulation().GetEndTime())
   {
      return false;
   }
   
   // Is this also a UCI Base type?
   auto basePtr = dynamic_cast<const UCI_MessageBaseType*>(&aMessage);
   bool processed = (basePtr != nullptr) && CanHandleMessage();
   if (processed)
   {
      try
      {
         basePtr->Send(UCI_Util::GetTimeStamp(GetSimulation(), aSimTime));

         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Message sent.";
            debug.AddNote() << "T= " << aSimTime;
            debug.AddNote() << "Parent Component: " << GetComponentParent()->GetName();
            debug.AddNote() << "Message Type: " << aMessage.GetType();
         }
      }
      catch (const uci::base::UCIException& ex)
      {
         ut::log::error() << ex.what() << ":" << ex.getErrorCode();
         processed = false;
      }
   }
   else if (DebugEnabled() && !CanHandleMessage())
   {
      auto debug = ut::log::debug() << "Message not sent.";
      debug.AddNote() << "Computer is turned off and/or not operational.";
      debug.AddNote() << "Component: " << GetName();
   }
   return processed;
}

void wsf::UCI_Computer::ReceiveUCI_Message(const uci::type::MessageType* aMessagePtr)
{
   if (CanHandleMessage())
   {
      if (GetInternalLinks().HasLinks())
      {
         auto msgPtr = mMessageFactoryPtr->Create(aMessagePtr);

         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Message received.";
            if (msgPtr != nullptr)
            {
               debug.AddNote() << "Message Type: " << msgPtr->GetType();
            }
            else
            {
               debug.AddNote() << "Message Type: " << aMessagePtr->getAccessorType();
            }
         }
         if (msgPtr != nullptr)
         {
            SendMessage(GetSimulation().GetSimTime(), *msgPtr);
         }

      }
   }
   else if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Message not received.";
      debug.AddNote() << "Computer is turned off and/or not operational.";
      debug.AddNote() << "Component: " << GetName();
   }
}

} // wsf
