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

#include "WsfApplicationExtension.hpp"

#include <cassert>

#include "WsfApplication.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"

// ================================================================================================
//! Constructor.
WsfApplicationExtension::WsfApplicationExtension()
   : mApplicationPtr(nullptr)
{
}

// ================================================================================================
// virtual
WsfApplicationExtension::~WsfApplicationExtension() {}

// ================================================================================================
//! Called in response to WsfApplication::RegisterExtension.
//! This is typically used to register script classes and methods, e.g.:
//! \code
//!     UtScriptTypes* scriptTypesPtr(aApplication.GetScriptTypes());
//!     scriptTypesPtr->Register(MySensor::CreateScriptClass("MY_SENSOR", scriptTypesPtr));
//! \endcode
//! @note GetApplication() is now valid.
// virtual
void WsfApplicationExtension::AddedToApplication(WsfApplication& aApplication) {}

// ================================================================================================
//! Called in response to the creation of a WsfScenario object.
//! This is called at the end of the WsfScenario constructor. A derived class may provide this
//! method to do the following:
//! <ul>
//! <li>
//! Register a scenario extension to register new types and access the input stream to interpret commands, e.g.:
//! \code
//!    aScenario.RegisterExtension("my_extension", new MyScenarioExtension);
//! \endcode
//! <li>
//! If a scenario extension is not being created, register new type objects or object factories, e.g.:
//! \code
//!    aScenario.GetMoverTypes().AddCoreType("MY_MOVER", new MyMover(aScenario));
//!    aScenario.GetAntennaPatternTypes().AddObjectFactor(MyAntennaPattern::ObjectFactory);
//! \endcode
//! </ul>
//! @param aScenario The scenario object that is being created.
//! @see WsfScenarioExtension.
// virtual
void WsfApplicationExtension::ScenarioCreated(WsfScenario& aScenario) {}

// ================================================================================================
//! Called in response to the creation of a WsfSimulation object.
//! This is actually called when the WsfSimulation::Initialize() is called. A derived class will
//! override this method if it needs to concern itself the creation of a simulation.
//!
//! Notes concerning creation of simulation extensions:
//! <ul>
//! <li>
//! If an application extension requires a simulation extension but not a scenario extension (probably
//! a rare occurrence), then the simulation extension should be created here, e.g.:
//! \code
//!    aSimulation.RegisterExtension("my_extension", new MySimulationExtension);
//! \endcode
//! <li>
//! If, however, the application extension requires both a scenario extension (created
//! by WsfApplicationExtension::ScenarioCreated) and a simulation extension, the simulation extension
//! should be created in WsfScenarioExtension::SimulationCreated. This ensures that any dependencies
//! registered by WsfApplication::ExtensionDepends are honored.
//! </ul>
//! @param aSimulation The simulation object that is being created.
//! @see WsfSimulationExtension.
// virtual
void WsfApplicationExtension::SimulationCreated(WsfSimulation& aSimulation) {}

// ================================================================================================
//! Called by WsfApplication::ProcessCommandLine to examine the current argument and process if necessary.
//! If the application extension recognizes the argument, this method should return the number
//! of entries processed.
//! @param aOptions The application options used to start the application
//! @param aArgc The number of arguments remaining in the argument list.
//! @param aArgv Any array of the arguments remaining in the argument list.
//! @returns The number of arguments processed.
// virtual
int WsfApplicationExtension::ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[])
{
   return 0;
}

// ================================================================================================
//! Called by WsfApplication::ShowUsage to let the application display its supported arguments.
//! The usage should be displayed on standard output.
// virtual
void WsfApplicationExtension::PrintCommandLineArguments() const {}

// ================================================================================================
//! If the extension provides input processing, this method can output the associated grammar.
//! This is not necessary if the extension installs a .ag file in the appropriate location.
//! @param aOut The output stream to which the grammar should be written.
// virtual
void WsfApplicationExtension::PrintGrammar(std::ostream& aOut) {}

// ================================================================================================
//! Called by WsfApplication::ProcessCommandLineCommands to let the application extension
//! process any commands included in the command line.
// virtual
void WsfApplicationExtension::ProcessCommandLineCommands() {}

// ================================================================================================
//! Called in response to WsfApplication::RegisterExtension
void WsfApplicationExtension::AddedToApplicationP(const std::string& aName, WsfApplication& aApplication)
{
   assert(mApplicationPtr == nullptr); // register only once
   InitializeExtensionName(aName);
   mApplicationPtr = &aApplication;
   AddedToApplication(aApplication);
}

// ================================================================================================
namespace WsfPrivate
{
WSF_EXPORT void RegisterScenarioExtension(WsfScenario&                          aScenario,
                                          const std::string&                    aName,
                                          std::unique_ptr<WsfScenarioExtension> aExtensionPtr)
{
   aScenario.RegisterExtension(aName, std::move(aExtensionPtr));
}
} // namespace WsfPrivate
