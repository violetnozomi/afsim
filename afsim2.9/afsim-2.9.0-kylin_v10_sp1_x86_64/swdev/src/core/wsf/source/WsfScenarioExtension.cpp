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

#include "WsfScenarioExtension.hpp"

#include <cassert>

#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

// ================================================================================================
WsfScenarioExtension::WsfScenarioExtension()
   : mScenarioPtr(nullptr)
{
}

// ================================================================================================
WsfScenarioExtension::~WsfScenarioExtension() {}

// ================================================================================================
//! Returns the application which owns the scenario
//! @note This is not valid until AddedToScenario() has been called.
WsfApplication& WsfScenarioExtension::GetApplication() const
{
   assert(mScenarioPtr != nullptr);
   return mScenarioPtr->GetApplication();
}

// ================================================================================================
//! Called when the extension has been added to the scenario.
//! This is typically used to register new component type objects and object factories, e.g.:
//! \code
//!    WsfScenario& scenario(GetScenario());
//!    WsfMoverTypes::Get(scenario).AddCoreType("MY_MOVER", new MyMover(scenario));
//!    WsfAntennaPatterTypes::Get(scenario).AddObjectFactor(MyAntennaPattern::ObjectFactory);
//! \endcode
//! Script classes must NOT be registered here because they are global to the entire application.
//! They should be registered in WsfApplication::AddedToApplication.
//! @note GetScenario() and GetApplication() are now valid.
// virtual
void WsfScenarioExtension::AddedToScenario() {}

// ================================================================================================
//! Process potential input for the extension.
//! This method examines the current input command to determine if it is one that is
//! recognized by the extension. If it is one recognized by the extension then it
//! processes the command and returns 'true', otherwise it returns 'false'.
//! @param aInput a reference to a UtInput object.
//! @returns 'true' if the command was one recognized by the extension or 'false' if not.
//! @throws UtInput::ExceptionBase (or an object derived from it) if the command was recognized
//!         but contains some sort of error.
// virtual
bool WsfScenarioExtension::ProcessInput(UtInput& aInput)
{
   return false;
}

// ================================================================================================
//! Called after a file has been loaded into the scenario.
//! @param aFileName The name of the file thast has been loaded into the scenario.
// virtual
void WsfScenarioExtension::FileLoaded(const std::string& aFileName) {}

// ================================================================================================
//! Called when all inputs are processed.
//! This is called when the application calls WsfScenario::LoadComplete to signify that all input
//! data has been loaded into the scenario. The extension should perform any additional validity
//! checking or other processing on its input data. Constant data may be created and stored here.
//! Unless Complete2() is also implement, the contents of this class should not be changed after this call.
//!
//! The extension should not assume anything about the state of another extension unless this a
//! dependency has been registered to ensure the other extension has been called prior to this, e.g.:
//!
//! \code
//!    app.ExtensionDepends("my_ext", "other_ext");
//! \endcode
//!
//! @returns 'true' if no problems or 'false' if an error occurred that would prevent execution.
// virtual
bool WsfScenarioExtension::Complete()
{
   return true;
}

// ================================================================================================
//! Phase 2 Complete.
//! This is called after all extensions have their Complete() methods called.
//! The contents of this class should remain constant after this call.
// virtual
bool WsfScenarioExtension::Complete2()
{
   return true;
}

// ================================================================================================
//! Called when a simulation has been created.
//! (Technically this is called upon entry to WsfSimulation::Initialize()).
//! This is typically used when a scenario extension also requires a simulation extension. The method
//! would typically register the simulation extension as follows:
//!
//! \code
//!    aSimulation.RegisterExtension("my_ext", new MySimulationExtension());
//! \endcode
//!
//! @param aSimulation The simulation that is being created. See note below!
//! @note This is called upon entry to WsfSimulation::Initialize() and nothing can be assumed
//! about the state of the simulation.
//! @see WsfSimulationExtension, WsfApplicationExtension::SimulationCreated
// virtual
void WsfScenarioExtension::SimulationCreated(WsfSimulation& aSimulation) {}

// ================================================================================================
//! Declare if the extension is required or optional.
//! An extension may be declared optional if this method returns 'false'. An optional extension
//! is one that is not loaded unless required by some other extension.
//! @returns 'true' if required or 'false' if optional.
// virtual
bool WsfScenarioExtension::AlwaysCreate()
{
   return true;
}

// ================================================================================================
//! Private method to bind the extension to the scenario.
//! This is called by WsfScenario::RegisterExtension to bind the extension to the scenario and then
//! invoke the virtual method AddedToScenario().
//! @param aName     The name of the extension.
//! @param aScenario The scenario to which the extension is being added.
// private
void WsfScenarioExtension::AddedToScenarioP(const std::string& aName, WsfScenario& aScenario)
{
   assert(mScenarioPtr == nullptr);
   mScenarioPtr = &aScenario;
   InitializeExtensionName(aName);
   AddedToScenario();
}

// ================================================================================================
// Definition of WsfCloneableScenarioExtension.
// ================================================================================================

// ================================================================================================
// virtual
bool WsfCloneableScenarioExtension::ProcessInput(UtInput& aInput)
{
   return mPrototypePtr->ProcessInput(aInput);
}

// ================================================================================================
// virtual
void WsfCloneableScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::clone(mPrototypePtr));
}

// ================================================================================================
namespace WsfPrivate
{
WSF_EXPORT void RegisterSimulationExtension(WsfSimulation&          aSimulation,
                                            const std::string&      aName,
                                            WsfSimulationExtension* aExtensionPtr)
{
   aSimulation.RegisterExtension(aName, std::unique_ptr<WsfSimulationExtension>(aExtensionPtr));
}
} // namespace WsfPrivate
