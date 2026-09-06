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

#include "WsfUCI_Component.hpp"

#include <string>
#include <limits>

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfUCI.hpp"
#include "WsfUCI_InterfaceExtension.hpp"

namespace wsf
{

UCI_Component::UCI_Component(const WsfScenario& aScenario,
                             int                aPartType)
   : WsfPlatformPart(aScenario, aPartType)
{
}

UCI_Component::UCI_Component(const UCI_Component& aSrc)
   : WsfPlatformPart(aSrc)
   , mSubsystemUUID(aSrc.mSubsystemUUID)
   , mSubsystemLabel(aSrc.mSubsystemLabel)
{
}

// virtual
UCI_Component::~UCI_Component()
{
   DeregisterWithInterface();
}


double wsf::UCI_Component::GetUpdateInterval() const
{
   return std::numeric_limits<double>::max();
}

void wsf::UCI_Component::SetUpdateInterval(double aUpdateInterval)
{
   // do nothing by default
}

// Get the UCI Interface. This will return a valid reference as long as it is used only with components actually placed on platforms.
UCI_Interface* UCI_Component::GetInterface()
{
   if (mUCI_InterfacePtr == nullptr)
   {
      mUCI_InterfacePtr = (UCI_InterfaceExtension::Get(GetSimulation()))->GetInterface();
   }
   return mUCI_InterfacePtr;
}

const int* UCI_Component::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL
                              };
   return roles;
}

// virtual
void* UCI_Component::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI)
   {
      return this;
   }
   else if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return static_cast<WsfPlatformPart*>(this);
   }
   else
   {
      return 0;
   }
}

bool UCI_Component::RegisterWithInterface(double aSimTime)
{
   UCI_Interface* interfacePtr = GetInterface();
   bool canRegister = (interfacePtr != nullptr);
   if (canRegister)
   {
      interfacePtr->RegisterComponent(*this);
      mIsRegistered = true;
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Component registered.";
         debug.AddNote() << "Component: " << GetName();
      }
   }
   return canRegister;
}

void UCI_Component::DeregisterWithInterface()
{
   if (mIsRegistered && mIsInitialized)
   {
      GetInterface()->DeregisterComponent(*this);
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Component deregistered.";
         debug.AddNote() << "Component: " << GetName();
      }
   }
}

void UCI_Component::RegisterCapabilitiesWithInterface()
{
   UCI_Interface* interfacePtr = GetInterface();
   bool canRegister = (interfacePtr != nullptr);
   if (canRegister)
   {
      interfacePtr->RegisterCapabilities(*this);
   }
}

void UCI_Component::DeregisterCapabilitiesWithInterface()
{
   if (mIsRegistered && mIsInitialized)
   {
      GetInterface()->DeregisterCapabilities(*this);
   }
}

//! Save whether this component was successfully initialized.
//! In overriding this method, this base class method should be called last.
//! (see destructor).
bool wsf::UCI_Component::Initialize2(double aSimTime)
{
   mIsInitialized = WsfPlatformPart::Initialize2(aSimTime);
   return mIsInitialized;
}

bool wsf::UCI_Component::ProcessInput(UtInput& aInput)
{
   auto command = aInput.GetCommand();
   bool myCommand{ true };
   if (command == "subsystem_uuid")
   {
      aInput.ReadValue(mSubsystemUUID);
   }
   else if (command == "subsystem_descriptor")
   {
      aInput.ReadValue(mSubsystemLabel);
   }
   else
   {
      myCommand = WsfPlatformPart::ProcessInput(aInput);
   }

   return myCommand;
}

} // wsf
