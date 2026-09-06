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

#include "WsfScriptEarthJ2Term.hpp"

#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "WsfEarthJ2Term.hpp"

WsfScriptEarthJ2Term::WsfScriptEarthJ2Term(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEarthJ2Term");

   AddStaticMethod(ut::make_unique<Construct1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct2>("Construct"));
   AddMethod(ut::make_unique<GravitationalParameter>());
   AddMethod(ut::make_unique<MeanRadius>());
   AddMethod(ut::make_unique<J2>());
   AddStaticMethod(ut::make_unique<DefaultJ2>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, Construct1, 0, "WsfEarthJ2Term", "")
{
   auto termPtr = ut::make_unique<WsfEarthJ2Term>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, Construct2, 3, "WsfEarthJ2Term", "double, double, double")
{
   auto   termPtr = ut::make_unique<WsfEarthJ2Term>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   double re = aVarArgs[1].GetDouble();
   if (re <= 0.0)
   {
      UT_SCRIPT_ABORT("Mean radius must be positive.");
   }
   termPtr->SetEarthMeanRadius(re);
   double j2 = aVarArgs[2].GetDouble();
   termPtr->SetJ2(j2);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, GravitationalParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGravitationalParameter());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, MeanRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEarthMeanRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, J2, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetJ2());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthJ2Term, WsfEarthJ2Term, DefaultJ2, 0, "double", "")
{
   aReturnVal.SetDouble(WsfEarthJ2Term::GetDefaultJ2Value());
}
