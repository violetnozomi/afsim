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

#ifndef WSFSCENARIOEXTENSION_HPP
#define WSFSCENARIOEXTENSION_HPP

#include "wsf_export.h"

#include <mutex>

class UtInput;
class WsfApplication;
class WsfApplicationExtension;
#include "WsfExtension.hpp"
class WsfScenario;
class WsfSimulation;
class WsfSimulationExtension;

//! A base class for 'scenario extensions'.
//!
//! A scenario extension is a object created and registered with an scenario object. A scenario object
//! will allow scenario extensions to examine the input stream and process commands as necessary.
//!
//! An application extension may instantiate and register a derived version of this class,
//! typically from within the derived class method WsfApplicationExtension::ScenarioCreated.
//!
//! @note The instance is owned by the scenario object once it is registered.
class WSF_EXPORT WsfScenarioExtension : public WsfExtension
{
public:
   friend class WsfScenario;

   WsfScenarioExtension();
   ~WsfScenarioExtension() override;

   //! Returns the scenario to which this extension is attached.
   //! @note This is not valid until AddedToScenario() has been called.
   WsfScenario& GetScenario() const { return *mScenarioPtr; }

   WsfApplication& GetApplication() const;

   //! @name Notifications from the scenario.
   //! These are called in the order they are listed.
   //@{
   virtual void AddedToScenario();
   virtual bool ProcessInput(UtInput& aInput);
   virtual void FileLoaded(const std::string& aFileName);
   virtual bool Complete();
   virtual bool Complete2();
   virtual void SimulationCreated(WsfSimulation& aSimulation);
   virtual bool AlwaysCreate();
   //@}

private:
   void AddedToScenarioP(const std::string& aName, WsfScenario& aScenario);

   //! Pointer to the scenario to which the extension is attached.
   //! This is nullptr until AddedToScenario() is called.
   WsfScenario* mScenarioPtr;
};

class WsfCloneableSimulationExtension;

//! Implements the WsfScenarioExtension interface given an extension that is cloneable
class WSF_EXPORT WsfCloneableScenarioExtension : public WsfScenarioExtension
{
public:
   WsfCloneableScenarioExtension(WsfCloneableSimulationExtension* aPrototypePtr)
      : mPrototypePtr(aPrototypePtr)
   {
   }
   ~WsfCloneableScenarioExtension() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

protected:
   WsfCloneableSimulationExtension* mPrototypePtr;
};

// Eliminate need for WsfSimulation.hpp include here
namespace WsfPrivate
{
WSF_EXPORT void RegisterSimulationExtension(WsfSimulation&          aSimulation,
                                            const std::string&      aName,
                                            WsfSimulationExtension* aExtensionPtr);
}

template<typename SIMULATION_EXTENSION>
class WsfDefaultScenarioExtension : public WsfScenarioExtension
{
public:
   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      WsfPrivate::RegisterSimulationExtension(aSimulation, GetExtensionName(), new SIMULATION_EXTENSION);
   }
};

//! Implements WsfScenarioExtension for legacy code that does not support multiple simulations
//! WsfLegacyScenarioExtension will provide capability to run a single simulation with the given extension.
//! Subsequent simulations will not have access to this extension
//! CLASS should be castable to WsfSimulationExtension.
template<typename CLASS>
class WsfLegacyScenarioExtension : public WsfScenarioExtension
{
public:
   WsfLegacyScenarioExtension(CLASS* aClassPtr)
      : mClassPtr(aClassPtr)
   {
   }

   ~WsfLegacyScenarioExtension() override { delete mClassPtr; }

   bool ProcessInput(UtInput& aInput) override
   {
      if (mClassPtr != nullptr)
      {
         return mClassPtr->ProcessInput(aInput);
      }
      return false;
   }
   //! 'Creates' the legacy extension only once
   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      std::lock_guard<std::recursive_mutex> lock(mMutex);
      WsfSimulationExtension*               extensionPtr = mClassPtr;
      mClassPtr                                          = nullptr;
      if (extensionPtr != nullptr)
      {
         WsfPrivate::RegisterSimulationExtension(aSimulation, GetExtensionName(), extensionPtr);
      }
   }

protected:
   std::recursive_mutex mMutex;
   CLASS*               mClassPtr;
};

#endif
