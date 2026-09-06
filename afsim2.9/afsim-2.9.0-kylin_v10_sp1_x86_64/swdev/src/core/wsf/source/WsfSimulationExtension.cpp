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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfSimulationExtension.hpp"

#include <cassert>

#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
WsfSimulationExtension::WsfSimulationExtension()
   : mSimulationPtr(nullptr)
{
}

// ================================================================================================
//! Returns the scenario in which this extension is registered.
//! @note Do not call this method before registering this extension
const WsfScenario& WsfSimulationExtension::GetScenario() const
{
   assert(mSimulationPtr != nullptr);
   return mSimulationPtr->GetScenario();
}

// ================================================================================================
//! Called in response to WsfSimulation::RegisterExtension.
//! The extension has now been registered in the simulation.
// virtual
void WsfSimulationExtension::AddedToSimulation() {}

// ================================================================================================
//! Called in response to WsfSimulation::Initialize.
//! This is called to allow the extension to perform any initialization that is required.
//! @note The extension should not assume that another extension has been initialized unless it
//! this extension has been registered as a dependency of the other extension.
// virtual
bool WsfSimulationExtension::Initialize()
{
   return true;
}

// ================================================================================================
//! Called in response to a call to WsfSimulation::Initialize and WsfSimulation::PrepareSimulation.
//!
//! A simulation extension will typically implement either WsfSimulationExtension::Initialize or
//! WsfSimulationExtension::PrepareExtension, or a combination of both. The main difference is that:
//!
//! - WsfSimulationExtension::Initialize is called only when a simulation is initialized after
//! being loaded from input files (the normal mode of operation).
//!
//! - WsfSimulation::PrepareExecution is called in the former case AND when a simulation is reloaded
//! from a snapshot.
//!
//! The primary criteria for deciding which method to use is to look at the extension and see if
//! it uses any external resources such as files or sockets. If the extension uses external resources
//! and wants to support snapshotting then it MUST use this this method so it can (re)establish
//! connections to external resources (A snapshot cannot save the state of an open file or socket
//! and so it must be reopened.)
// virtual
bool WsfSimulationExtension::PrepareExtension()
{
   return true;
}

// ================================================================================================
//! Called during WsfSimulation::Initialize, via WsfSimulation::AddInputPlatforms(),
//! after all platforms have undergone Initialize() calls.
//! This is called to allow any extension to perform processing that may be predicated on platforms
//! being added to the simulation, with all platforms and components having
//! undergone calls to Initialize(), but prior to calls to Initialize2() having been performed.
// virtual
bool WsfSimulationExtension::PlatformsInitialized()
{
   return true;
}

// ================================================================================================
//! Called in response to a call to WsfSimulation::Initialize complete and pending start state.
//! PendingStart is typically called immediately after the simulation is initialized in the main event loop.
//! The simulation is in its pending start state, although the extension may also add additional platforms.
//! using WsfSimulation::AddPlatform.
// virtual
void WsfSimulationExtension::PendingStart() {}

// ================================================================================================
//! Called in response to a call to WsfSimulation::Start.
//! WsfSimulation::Start is typically called immediately before entry in the main event loop.
//! The simulation is in its initial state, although the extension may also add additional platforms.
//! using WsfSimulation::AddPlatform.
// virtual
void WsfSimulationExtension::Start() {}

// ================================================================================================
//! Called in response to a call to WsfSimulation::Complete.
//! This method is used to release any resources (close files or sockets, delete objects, etc.) that
//! were allocated by the Initialize/PrepareExtension methods or otherwise allocated during the
//! execution of the simulation. No other events or observer callbacks will be dispatched after
//! this is called.
//! @param aSimTime The final simulation time of the simulation.
// virtual
void WsfSimulationExtension::Complete(double aSimTime) {}

// ================================================================================================
//! Private method to bind the extension to the simulation.
//! This is called by WsfSimulation::RegisterExtension to bind the extension to the simulation and
//! then invoke the virtual method AddedToSimulation().
//! @param aName       The name of the extension.
//! @param aSimulation The simulation to which the extension is being added.
// private
void WsfSimulationExtension::AddedToSimulationP(const std::string& aName, WsfSimulation& aSimulation)
{
   assert(mSimulationPtr == nullptr);
   mSimulationPtr = &aSimulation;
   InitializeExtensionName(aName);
   AddedToSimulation();
}

// ================================================================================================
WsfCloneableScenarioExtension::~WsfCloneableScenarioExtension()
{
   delete mPrototypePtr;
}
