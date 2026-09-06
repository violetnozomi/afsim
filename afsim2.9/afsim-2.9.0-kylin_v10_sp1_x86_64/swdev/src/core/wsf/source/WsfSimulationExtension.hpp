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

#ifndef WSFSIMULATIONEXTENSION_HPP
#define WSFSIMULATIONEXTENSION_HPP

#include <string>

class UtInput;
#include "WsfExtension.hpp"
class WsfScenario;
class WsfSimulation;

//! A base class for 'simulation extensions'.
//!
//! An simulation extension is a object created and registered with an simulation object. The
//! simulation object will notify registered simulation extensions when the simulation is
//! initialized, started or completed.
//!
//! An application extension may instantiate and register a derived version of this class,
//! typically from within the derived class method WsfApplicationExtension::SimulationCreated().
//! A simulation extension is TYPICALLY necessary only if the application extension also registered
//! an input extension as the simulation extension typically uses the data read by the input
//! extension. There may be cases, however, where an input extension is created but the inputs are
//! only used by instances of extension-created type objects. In this case a simulation extension MAY
//! not be necessary. Your mileage may vary...
//!
//! @note The instance is owned by the simulation object once it is registered.
class WSF_EXPORT WsfSimulationExtension : public WsfExtension
{
public:
   friend class WsfSimulation;

   WsfSimulationExtension();
   ~WsfSimulationExtension() override = default;

   //! Returns the simulation to which this extension is registered.
   //! @note This is not valid until AddedToSimulation() has been called.
   WsfSimulation& GetSimulation() const { return *mSimulationPtr; }

   const WsfScenario& GetScenario() const;

   //! @name Notifications from the simulation.
   //! These are called in the order they are listed.
   //@{
   virtual void AddedToSimulation();
   virtual bool Initialize();
   virtual bool PrepareExtension();
   virtual bool PlatformsInitialized();
   virtual void PendingStart();
   virtual void Start();
   virtual void Complete(double aSimTime);
   //@}

   static void __private_SetSim(WsfSimulationExtension& aExtension, WsfSimulation* aSimPtr)
   {
      aExtension.mSimulationPtr = aSimPtr;
   }

private:
   void AddedToSimulationP(const std::string& aName, WsfSimulation& aSimulation);

   WsfSimulation* mSimulationPtr;
};

//! A cloneable simulation extension.
//! Cloneable extensions read input and are cloned for each simulation.
class WSF_EXPORT WsfCloneableSimulationExtension : public WsfSimulationExtension
{
public:
   WsfCloneableSimulationExtension() = default;

   // Clone an extension
   virtual WsfCloneableSimulationExtension* Clone() const = 0;

   virtual bool ProcessInput(UtInput& aInput) { return false; }
};

#endif // WSFSIMULATIONEXTENSION_HPP
