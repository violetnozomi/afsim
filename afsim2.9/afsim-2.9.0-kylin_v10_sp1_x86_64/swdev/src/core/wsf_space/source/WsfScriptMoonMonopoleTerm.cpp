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

#include "WsfScriptMoonMonopoleTerm.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfMoonMonopoleTerm.hpp"

WsfScriptMoonMonopoleTerm::WsfScriptMoonMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfMoonMonopoleTerm");

   AddStaticMethod(ut::make_unique<Construct1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct2>("Construct"));
   AddStaticMethod(ut::make_unique<Construct3>("Construct"));
   AddMethod(ut::make_unique<GravitationalParameter>());
   AddMethod(ut::make_unique<InterpolationInterval>());
   AddMethod(ut::make_unique<MoonPositionECI>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, Construct1, 0, "WsfMoonMonopoleTerm", "")
{
   auto termPtr = ut::make_unique<WsfMoonMonopoleTerm>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, Construct2, 1, "WsfMoonMonopoleTerm", "double")
{
   auto   termPtr = ut::make_unique<WsfMoonMonopoleTerm>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, Construct3, 2, "WsfMoonMonopoleTerm", "double, double")
{
   auto   termPtr = ut::make_unique<WsfMoonMonopoleTerm>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   double interval = aVarArgs[1].GetDouble();
   if (interval < 0.0)
   {
      UT_SCRIPT_ABORT("Interpolation interval must be non-negative.");
   }
   termPtr->SetInterpolationInterval(interval);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, GravitationalParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGravitationalParameter());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, InterpolationInterval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInterpolationInterval());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoonMonopoleTerm, WsfMoonMonopoleTerm, MoonPositionECI, 1, "Vec3", "Calendar")
{
   auto time = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetMoonPositionECI(*time)));
}
