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

#include "WsfProcessor.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfComponentListDefines.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
WsfProcessor::WsfProcessor(const WsfScenario& aScenario)
   : WsfPlatformPart(aScenario, cCOMPONENT_ROLE<WsfProcessor>())
   , mComponents()
   , mUpdateInterval(0.0)
{
   WsfPlatformPart::SetInitiallyTurnedOn(true);
   mComponents.SetParentOfComponents(this);
}

// ================================================================================================
//! Copy constructor (for Clone())
// protected
WsfProcessor::WsfProcessor(const WsfProcessor& aSrc)
   : WsfPlatformPart(aSrc)
   , mComponents(aSrc.mComponents)
   , mUpdateInterval(aSrc.mUpdateInterval)
{
   mComponents.SetParentOfComponents(this);
}

// ================================================================================================
const int* WsfProcessor::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_PROCESSOR, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfProcessor::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_PROCESSOR)
   {
      return this;
   }
   return WsfPlatformPart::QueryInterface(aRole);
}

// ================================================================================================
const char* WsfProcessor::GetScriptClassName() const
{
   return "WsfProcessor";
}

// ================================================================================================
// virtual
bool WsfProcessor::PreInitialize(double aSimTime)
{
   bool ok = WsfPlatformPart::PreInitialize(aSimTime);

   // Let component factories inject components.
   WsfComponentFactoryList<WsfProcessor>::PreInitialize(GetScenario(), aSimTime, *this);

   for (auto component : mComponents)
   {
      ok &= component->PreInitialize(aSimTime);
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfProcessor::Initialize(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize(aSimTime);

   ok &= mUpdateInterval.Initialize("update_interval",
                                    this,
                                    &GetPlatform()->GetScriptContext(),
                                    GetSimulation()->GetScriptContext());

   for (auto component : mComponents)
   {
      ok &= component->Initialize(aSimTime);
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfProcessor::Initialize2(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize2(aSimTime);

   for (auto component : mComponents)
   {
      ok &= component->Initialize2(aSimTime);
   }
   return ok;
}

// ================================================================================================
//! Update the state of the processor.
//! This method should perform any actions that are necessary so that the system
//! reflects its condition at the supplied time.
//! @param aSimTime The current simulation time.
// virtual
void WsfProcessor::Update(double /* aSimTime */) {}

// ============================================================================
// See WsfComponent::PreInput about this.
void WsfProcessor::PreInput()
{
   WsfComponentFactoryList<WsfProcessor>::PreInput(GetScenario(), *this);
}

// ================================================================================================
// virtual
bool WsfProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "update_interval")
   {
      mUpdateInterval.ReadValueOfType(aInput, UtInput::cTIME);
      mUpdateInterval.ValueGreater(aInput, 0.0);
   }
   else if (WsfPlatformPart::ProcessInput(aInput))
   {
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
      // First try components already attached. If the input was not recognized by one of them then
      // try the component factories, which may result in the dynamic creation of a new component.
   }
   else if (WsfComponentFactoryList<WsfProcessor>::ProcessInput(GetScenario(), aInput, *this))
   {
      // If not processed by an attached component try the component factories to see if a component
      // should be dynamically attached.
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ================================================================================================
//! Turn the processor off.
//! The processor will no longer be called for received messages or periodic updates
//! until it is turned on again.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnProcessorOff method.
// virtual
void WsfProcessor::TurnOff(double aSimTime)
{
   WsfPlatformPart::TurnOff(aSimTime);
   WSF_CALL_VOID_COMPONENTS(TurnOff, aSimTime);
   WsfObserver::ProcessorTurnedOff(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Turn the processor on.
//! The processor will now be called for received messages and periodic updates.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnProcessorOn method.
// virtual
void WsfProcessor::TurnOn(double aSimTime)
{
   WsfPlatformPart::TurnOn(aSimTime);
   WSF_CALL_VOID_COMPONENTS(TurnOn, aSimTime);
   WsfObserver::ProcessorTurnedOn(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the processor non-Operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfProcessor::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::ProcessorNonOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the processor Operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfProcessor::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::ProcessorOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the processor terminated.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfProcessor::PartBroken(double aSimTime)
{
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::ProcessorBroken(GetSimulation())(aSimTime, this);
}

// ================================================================================================
void WsfProcessor::SetUpdateInterval(double aUpdateInterval)
{
   mUpdateInterval = aUpdateInterval;
}

// ================================================================================================
double WsfProcessor::GetUpdateInterval() const
{
   return mUpdateInterval;
}

// ================================================================================================
//! Process a callback.
//! Notification that the current platform just executed a callback.
//! @param aSimTime  The current simulation time.
//! @param aCallback The callback being executed.
// virtual
void WsfProcessor::ProcessCallback(double /* aSimTime */, const WsfCallback& /* aCallback */) {}
