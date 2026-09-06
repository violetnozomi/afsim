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

#include "WsfScriptJupiterMonopoleTerm.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfJupiterMonopoleTerm.hpp"

WsfScriptJupiterMonopoleTerm::WsfScriptJupiterMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfJupiterMonopoleTerm");

   AddStaticMethod(ut::make_unique<Construct1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct2>("Construct"));
   AddMethod(ut::make_unique<GravitationalParameter>());
   AddMethod(ut::make_unique<JupiterPositionECI>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJupiterMonopoleTerm, WsfJupiterMonopoleTerm, Construct1, 0, "WsfJupiterMonopoleTerm", "")
{
   auto termPtr = ut::make_unique<WsfJupiterMonopoleTerm>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJupiterMonopoleTerm, WsfJupiterMonopoleTerm, Construct2, 1, "WsfJupiterMonopoleTerm", "double")
{
   auto   termPtr = ut::make_unique<WsfJupiterMonopoleTerm>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJupiterMonopoleTerm, WsfJupiterMonopoleTerm, GravitationalParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGravitationalParameter());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJupiterMonopoleTerm, WsfJupiterMonopoleTerm, JupiterPositionECI, 1, "Vec3", "Calendar")
{
   auto time = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetJupiterLocationECI(*time)));
}
