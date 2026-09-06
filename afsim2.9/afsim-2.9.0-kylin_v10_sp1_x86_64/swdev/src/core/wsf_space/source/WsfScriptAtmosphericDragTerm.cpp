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

#include "WsfScriptAtmosphericDragTerm.hpp"

#include "WsfAtmosphereSimulationExtension.hpp"
#include "WsfAtmosphericDragTerm.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptAtmosphericDragTerm::WsfScriptAtmosphericDragTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAtmosphericDragTerm");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<DragCoefficient>());
   AddMethod(ut::make_unique<CrossSectionalArea>());
   AddMethod(ut::make_unique<AtmosphereModelName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphericDragTerm,
                        WsfAtmosphericDragTerm,
                        Construct,
                        3,
                        "WsfAtmosphericDragTerm",
                        "double, double, string")
{
   auto   termPtr = ut::make_unique<WsfAtmosphericDragTerm>();
   double Cd      = aVarArgs[0].GetDouble();
   if (Cd <= 0.0)
   {
      UT_SCRIPT_ABORT("Drag coefficient must be positive.");
   }
   termPtr->SetDragCoefficient(Cd);
   double A = aVarArgs[1].GetDouble();
   if (A <= 0.0)
   {
      UT_SCRIPT_ABORT("Cross sectional area must be positive.");
   }
   termPtr->SetCrossSectionalArea(A);
   std::string modelName = aVarArgs[2].GetString();
   if (wsf::space::AtmosphereSimulationExtension::Get(*SIMULATION).GetAtmosphere(modelName) == nullptr)
   {
      UT_SCRIPT_ABORT("Unknown atmosphere model.");
   }
   termPtr->SetAtmosphereModelName(modelName);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphericDragTerm, WsfAtmosphericDragTerm, DragCoefficient, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDragCoefficient());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphericDragTerm, WsfAtmosphericDragTerm, CrossSectionalArea, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCrossSectionalArea());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphericDragTerm, WsfAtmosphericDragTerm, AtmosphereModelName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAtmosphereModelName());
}
