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

#include "WsfScriptEarthMonopoleTerm.hpp"

#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "WsfEarthMonopoleTerm.hpp"

WsfScriptEarthMonopoleTerm::WsfScriptEarthMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEarthMonopoleTerm");

   AddStaticMethod(ut::make_unique<Construct1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct2>("Construct"));
   AddStaticMethod(ut::make_unique<Construct3>("Construct"));
   AddMethod(ut::make_unique<GravitationalParameter>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthMonopoleTerm, WsfEarthMonopoleTerm, Construct1, 0, "WsfEarthMonopoleTerm", "")
{
   auto termPtr = ut::make_unique<WsfEarthMonopoleTerm>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthMonopoleTerm, WsfEarthMonopoleTerm, Construct2, 1, "WsfEarthMonopoleTerm", "string")
{
   auto        termPtr = ut::make_unique<WsfEarthMonopoleTerm>();
   std::string grav    = aVarArgs[0].GetString();
   if (grav == "wgs84")
   {
      termPtr->SetGravitationalParameter(UtEarth::cGRAVITATIONAL_PARAMETER_WGS84);
   }
   else if (grav == "egm96")
   {
      termPtr->SetGravitationalParameter(UtEarth::cGRAVITATIONAL_PARAMETER);
   }
   else
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be 'egm96' or 'wgs84'.");
   }
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthMonopoleTerm, WsfEarthMonopoleTerm, Construct3, 1, "WsfEarthMonopoleTerm", "double")
{
   auto   termPtr = ut::make_unique<WsfEarthMonopoleTerm>();
   double mu      = aVarArgs[0].GetDouble();
   if (mu <= 0.0)
   {
      UT_SCRIPT_ABORT("Gravitational parameter must be positive.");
   }
   termPtr->SetGravitationalParameter(mu);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEarthMonopoleTerm, WsfEarthMonopoleTerm, GravitationalParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGravitationalParameter());
}
