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

#include "WsfScriptSunMonopoleTerm.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfSunMonopoleTerm.hpp"

WsfScriptSunMonopoleTerm::WsfScriptSunMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSunMonopoleTerm");

   AddStaticMethod(ut::make_unique<Construct1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct2>("Construct"));
   AddMethod(ut::make_unique<GravitationalParameter>());
   AddMethod(ut::make_unique<SunPositionECI>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSunMonopoleTerm, WsfSunMonopoleTerm, Construct1, 0, "WsfSunMonopoleTerm", "")
{
   auto termPtr = ut::make_unique<WsfSunMonopoleTerm>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSunMonopoleTerm, WsfSunMonopoleTerm, Construct2, 1, "WsfSunMonopoleTerm", "double")
{
   auto   termPtr = ut::make_unique<WsfSunMonopoleTerm>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSunMonopoleTerm, WsfSunMonopoleTerm, GravitationalParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGravitationalParameter());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSunMonopoleTerm, WsfSunMonopoleTerm, SunPositionECI, 1, "Vec3", "Calendar")
{
   auto time = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetSunLocationECI(*time)));
}
