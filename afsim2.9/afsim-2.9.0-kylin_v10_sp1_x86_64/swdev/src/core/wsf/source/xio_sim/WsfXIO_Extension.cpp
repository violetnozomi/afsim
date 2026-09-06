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

#include "xio_sim/WsfXIO_Extension.hpp"

#include <memory>
#include <string>

#include "UtCallbackHolder.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"
#include "dis/WsfDisInterface.hpp"
#include "ext/WsfExtInterface.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_InputData.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PublishKey.hpp"
#include "xio_sim/WsfXIO_AuxData.hpp"
#include "xio_sim/WsfXIO_DisExtension.hpp"
#include "xio_sim/WsfXIO_EventPipe.hpp"
#include "xio_sim/WsfXIO_ScriptClass.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

namespace
{
// NOTE: In the original incarnation of XIO after the implementation of extensions, WsfXIO_Interface derived
// from WsfScenarioExtension. Unfortunately, this had the side effect of linking in just about everything in
// WSF. This defeated the original intention of making the XIO interface useful as something that could
// be used by ANY application that wanted to communicate with a WSF application without the need to include
// all of WSF.

class SimulationExtension : public WsfSimulationExtension
{
public:
   SimulationExtension(wsf::xio::ApplicationType aApplicationType, const WsfXIO_InputData& aInputData)
      : mApplicationType(aApplicationType)
      , mInputData(aInputData)
   {
   }

   ~SimulationExtension() override = default;

   void AddedToSimulation() override
   {
      mXIO_InterfacePtr = ut::make_unique<WsfXIO_Interface>(mInputData);
      mXIO_InterfacePtr->SetApplicationType(mApplicationType);
      mXIO_SimulationPtr   = ut::make_unique<WsfXIO_Simulation>(mXIO_InterfacePtr.get(), GetSimulation());
      mXIO_DisExtensionPtr = ut::make_unique<WsfXIO_DisExtension>(*mXIO_SimulationPtr);
      mXIO_AuxData         = ut::make_unique<wsf::xio::AuxData>(*mXIO_SimulationPtr);

      WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
      if (eventPipePtr != nullptr)
      {
         mXIO_EventPipePtr = ut::make_unique<wsf::xio::EventPipe>(*eventPipePtr);
         eventPipePtr->AddLogger(mXIO_EventPipePtr.get());
      }
   }

   bool Initialize() override
   {
      bool ok = mXIO_InterfacePtr->Initialize();
      ok &= mXIO_SimulationPtr->Initialize();
      ok &= mXIO_DisExtensionPtr->Initialize();
      return ok;
   }

   wsf::xio::ApplicationType mApplicationType;
   WsfXIO_InputData          mInputData;

   std::unique_ptr<WsfXIO_Interface>    mXIO_InterfacePtr;
   std::unique_ptr<WsfXIO_Simulation>   mXIO_SimulationPtr;
   std::unique_ptr<WsfXIO_DisExtension> mXIO_DisExtensionPtr;
   std::unique_ptr<wsf::xio::EventPipe> mXIO_EventPipePtr;
   std::unique_ptr<wsf::xio::AuxData>   mXIO_AuxData;
};

class XIOScenarioExtension : public WsfScenarioExtension
{
public:
   XIOScenarioExtension(wsf::xio::ApplicationType aApplicationType)
      : mApplicationType(aApplicationType)
      , mInputData()
   {
      mCallbacks += mInputData.ProcessInputEvent.Connect(&XIOScenarioExtension::ProcessInputCallback, this);
   }

   void AddedToScenario() override
   {
      auto* eventPipeExtension = WsfEventPipeExtension::Find(GetScenario());
      if (eventPipeExtension != nullptr)
      {
         wsf::xio::EventPipe::RegisterEvents(*eventPipeExtension);
      }
   }

   bool ProcessInput(UtInput& aInput) override { return mInputData.ProcessInput(aInput); }

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      // The simulation extension is created only if a non-trivial 'xio_interface' block was provided.
      if (mInputData.mInterfaceRequested || (!mInputData.mUDP_Targets.empty()))
      {
         aSimulation.RegisterExtension(GetExtensionName(),
                                       ut::make_unique<SimulationExtension>(mApplicationType, mInputData));
      }
   }

   //! Registered as an WsfXIO_InputData input callback.
   //! ext_interface commands are also valid within the xio_interface block.
   bool ProcessInputCallback(UtInput& aInput)
   {
      bool         myCommand(false);
      WsfExtInput* extInterfacePtr = static_cast<WsfExtInput*>(GetScenario().FindExtension("ext_interface"));
      if (extInterfacePtr != nullptr)
      {
         myCommand = extInterfacePtr->ProcessInput(aInput);
      }
      return myCommand;
   }

   wsf::xio::ApplicationType mApplicationType;
   WsfXIO_InputData          mInputData;
   UtCallbackHolder          mCallbacks;
};

class XIOApplicationExtension : public WsfApplicationExtension
{
public:
   XIOApplicationExtension()
      : mApplicationType(wsf::xio::cAPP_SIMULATION)
   {
   }

   void AddedToApplication(WsfApplication& aApplication) override
   {
      UtScriptTypes* typesPtr = aApplication.GetScriptTypes();
      typesPtr->Register(ut::make_unique<WsfXIO_ScriptClass>("WsfXIO", typesPtr));
      typesPtr->Register(WsfXIO_PublishKey::CreateScriptClass("WsfXIO_PublishKey", typesPtr));
      typesPtr->Register(WsfXIO_Connection::CreateScriptClass(typesPtr));
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<XIOScenarioExtension>(mApplicationType));
   }

private:
   wsf::xio::ApplicationType mApplicationType;
};
} // namespace

// =================================================================================================
//! Register the XIO interface.
//! This is invoked when the application contains 'WSF_REGISTER_EXTENSION(app, xio_interface);'
void WSF_EXPORT Register_xio_interface(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("xio_interface"))
   {
      WSF_REGISTER_EXTENSION(aApplication, dis_interface);
      aApplication.RegisterExtension("xio_interface", ut::make_unique<XIOApplicationExtension>());
      aApplication.ExtensionDepends("xio_interface", "dis_interface", true);
      aApplication.ExtensionDepends("xio_interface", "event_output", false);
   }
}

// =================================================================================================
//! Return a pointer to the WsfXIO_InputData object for a given scenario.
WsfXIO_InputData* WsfXIO_Extension::Find(const WsfScenario& aScenario)
{
   XIOScenarioExtension* extensionPtr = static_cast<XIOScenarioExtension*>(aScenario.FindExtension("xio_interface"));
   return (extensionPtr == nullptr) ? nullptr : (&extensionPtr->mInputData);
}

// =================================================================================================
//! Return a pointer to the WsfXIO_Interface object for a given simulation.
WsfXIO_Interface* WsfXIO_Extension::Find(const WsfSimulation& aSimulation)
{
   SimulationExtension* extensionPtr = static_cast<SimulationExtension*>(aSimulation.FindExtension("xio_interface"));
   return (extensionPtr == nullptr) ? nullptr : (extensionPtr->mXIO_InterfacePtr.get());
}

// =================================================================================================
//! Return a pointer to the WsfXIO_Simulation object for a given simulation.
WsfXIO_Simulation* WsfXIO_Extension::FindSimulation(const WsfSimulation& aSimulation)
{
   SimulationExtension* extensionPtr = static_cast<SimulationExtension*>(aSimulation.FindExtension("xio_interface"));
   return (extensionPtr == nullptr) ? nullptr : (extensionPtr->mXIO_SimulationPtr.get());
}

// =================================================================================================
//! Return a pointer to the WsfXIO_DisExtension object for a given simulation.
WsfXIO_DisExtension* WsfXIO_Extension::FindDisExtension(const WsfSimulation& aSimulation)
{
   SimulationExtension* extensionPtr = static_cast<SimulationExtension*>(aSimulation.FindExtension("xio_interface"));
   return (extensionPtr == nullptr) ? nullptr : (extensionPtr->mXIO_DisExtensionPtr.get());
}
