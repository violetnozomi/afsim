// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptIntegratingSpaceMover.hpp"

#include "UtMemory.hpp"
#include "WsfIntegratingPropagator.hpp"
#include "WsfIntegratingSpaceMover.hpp"
#include "WsfOrbitalDynamics.hpp"

WsfScriptIntegratingSpaceMover::WsfScriptIntegratingSpaceMover(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptSpaceMoverClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfIntegratingSpaceMover");

   AddMethod(ut::make_unique<OrbitalDynamics>());
   AddMethod(ut::make_unique<SetOrbitalDynamics>());
   AddMethod(ut::make_unique<OrbitalIntegrator>());
   AddMethod(ut::make_unique<SetOrbitalIntegrator>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptIntegratingSpaceMover, WsfIntegratingSpaceMover, OrbitalDynamics, 0, "WsfOrbitalDynamics", "")
{
   auto dynamicsPtr = aObjectPtr->GetOrbitalDynamicsClone();
   aReturnVal.SetPointer(new UtScriptRef(dynamicsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptIntegratingSpaceMover, WsfIntegratingSpaceMover, SetOrbitalDynamics, 1, "void", "WsfOrbitalDynamics")
{
   auto dynamicsPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalDynamics>();
   if (!aObjectPtr->SetOrbitalDynamics(ut::clone(dynamicsPtr)))
   {
      UT_SCRIPT_ABORT("Unable to change dynamics during propagation.");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptIntegratingSpaceMover, WsfIntegratingSpaceMover, OrbitalIntegrator, 0, "WsfOrbitalIntegrator", "")
{
   auto integratorPtr = aObjectPtr->GetOrbitalIntegratorClone();
   aReturnVal.SetPointer(new UtScriptRef(integratorPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptIntegratingSpaceMover, WsfIntegratingSpaceMover, SetOrbitalIntegrator, 1, "void", "WsfOrbitalIntegrator")
{
   auto integratorPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalIntegrator>();
   if (!aObjectPtr->SetOrbitalIntegrator(ut::clone(integratorPtr)))
   {
      UT_SCRIPT_ABORT("Unable to change integration during propagation.");
   }
}
