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

#include "WsfComponent.hpp"

// ================================================================================================
//! Perform pre-initializion function.
//! This is called by WsfPlatform::Initialize prior to actually initializing the components on a
//! platform (i.e.: calling Initialize and Initialize2 for each component). The primary reason for
//! this method is to allow components to add other components that will then be subsequently
//! initialized. The component may examine the component list and add components, but it must not
//! assume ANYTHING about the actual state of the component.
//! @param aSimTime The current simulation time.
//! @returns true if successful or false if unsuccessful (bad input data, etc.)
// virtual
bool WsfComponent::PreInitialize(double aSimTime)
{
   return true;
}

// ================================================================================================
//! Perform phase 1 initialization.
//! This is called by WsfPlatform::Initialize() during phase 1 initialization of the platform. The
//! component should complete validation of input data that could not be done by ProcessInput, as
//! well as any other functions to prepare for simulation use (subject to the next paragraph).
//!
//! The order in which components are initialized is defined by GetComponentInitializationOrder.
//! In general a component implementation of this method should not be dependent on the state of
//! another component, i.e.: it should not assume that the other components Initialize() method has
//! been called. If such a dependency exists one should use Initialize2() if possible, or specify
//! a value GetComponentInitializationOrder to force things to initialize in a particular order.
//!
//! @param aSimTime The current simulation time.
//! @returns true if successful or false if unsuccessful (bad input data, etc.)
//! @note This method should NOT add any other components to the platform. This MUST be done in PreInitialize().
// virtual
bool WsfComponent::Initialize(double aSimTime)
{
   return true;
}

// ================================================================================================
//! Perform phase 2 initialization.
//! This is called by WsfPlatform::Initialize() during phase 2 of platform initialization. This
//! method is typically used when a component may have some dependency on another component having
//! completed its phase 1 initialization. The component should be ready for use in the simulation
//! upon completion of this method.
//!
//! The order in which components are initialized is defined by GetComponentInitializationOrder.
//!
//! @param aSimTime The current simulation time.
//! @returns true if successful or false if unsuccessful.
//! @note This method MUST NOT add any other components to the platform. This MUST be done in PreInitialize().
// virtual
bool WsfComponent::Initialize2(double aSimTime)
{
   return true;
}

// ================================================================================================
//! Perform pre-input processing.
//! This is called in certain contexts just before input processing is FIRST done for a component
//! (See WsfObjectTypeListBase::LoadComponentP.) The primary motivation is to allow the newly created
//! component to invoke the type/role-specific PreInput method in the component factories, which may
//! in turn add any sub-components that MAY be required for input processing or for run-time.
//!
//! This is currently implemented only for platform level components such as WsfComm, WsfProcessor,
//! WsfSensor and some components implemented in optional projects. You can look at the PreInput
//! methods in each of these and see they are very simple.
//!
//! The reason for this is that WsfObjectTypeListBase::LoadComponentP cannot simply invoke the PreInput
//! methods in the component factories. PreInput is only defined in the template WsfComponentFactoryT<>,
//! and the PreInput method in the template defines a method whose argument is a reference to the
//! object being created. There is no way for LoadComponentP to do this in general. Delegating it to
//! to component solves the problem.
//!
//! @note This method may get called more than once as input objects derive from other input type-objects.
void WsfComponent::PreInput() {}

// ================================================================================================
//! Evaluate the current input command and determine if it is one processed by the component.
//! The component should get the current command from the input stream and determine if it is
//! one that it recognizes. If it does not recognize the command then it should leave the
//! command in the input stream and return false. If it does recognize the command then it
//! should process the command and return true. If invalid arguments are encountered then it
//! should throw an exception.
//! @param aInput The input stream.
//! @returns true if the current command was recognized (and processed), or false if the command
//! was not recognized.
//! @throws UtInput::ExceptionBase if an invalid input was encountered.
// virtual
bool WsfComponent::ProcessInput(UtInput& aInput)
{
   return false;
}
