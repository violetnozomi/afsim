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

#include "WsfScriptOrbitalDynamics.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptOrbitalDynamics::WsfScriptOrbitalDynamics(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("WsfOrbitalDynamics", aScriptTypesPtr)
{
   mConstructible = true;

   SetClassName("WsfOrbitalDynamics");

   AddMethod(ut::make_unique<NumTerms>());
   AddMethod(ut::make_unique<Term>());
   AddMethod(ut::make_unique<AddTerm>());
   AddMethod(ut::make_unique<RemoveTerm>());
   AddMethod(ut::make_unique<ComputeAcceleration>());
}

void* WsfScriptOrbitalDynamics::Create(const UtScriptContext& aInstance)
{
   WsfScenario* scenarioPtr = WsfScriptContext::GetSCENARIO(aInstance);
   if (scenarioPtr)
   {
      return new WsfOrbitalDynamics{*scenarioPtr};
   }
   return nullptr;
}

void WsfScriptOrbitalDynamics::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfOrbitalDynamics*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamics, WsfOrbitalDynamics, NumTerms, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int, size_t>(aObjectPtr->GetNumTerms()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamics, WsfOrbitalDynamics, Term, 1, "WsfOrbitalDynamicsTerm", "int")
{
   const WsfOrbitalDynamicsTerm* termPtr = aObjectPtr->GetTerm(ut::cast_to_size_t(aVarArgs[0].GetInt()));
   if (!termPtr)
   {
      UT_SCRIPT_ABORT("Index out of range: no such term.");
   }
   aReturnVal.SetPointer(new UtScriptRef(termPtr->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamics, WsfOrbitalDynamics, AddTerm, 1, "bool", "WsfOrbitalDynamicsTerm")
{
   WsfOrbitalDynamicsTerm* termPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalDynamicsTerm>();
   if (termPtr)
   {
      aObjectPtr->AddTerm(ut::clone(termPtr));
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamics, WsfOrbitalDynamics, RemoveTerm, 1, "bool", "int")
{
   aReturnVal.SetBool(aObjectPtr->RemoveTerm(ut::cast_to_size_t(aVarArgs[0].GetInt())));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamics, WsfOrbitalDynamics, ComputeAcceleration, 4, "Vec3", "double, Calendar, Vec3, Vec3")
{
   double  mass = aVarArgs[0].GetDouble();
   auto    time = aVarArgs[1].GetPointer()->GetAppObject<UtCalendar>();
   auto    pos  = aVarArgs[2].GetPointer()->GetAppObject<UtVec3d>();
   auto    vel  = aVarArgs[3].GetPointer()->GetAppObject<UtVec3d>();
   UtVec3d acc  = aObjectPtr->ComputeAcceleration(mass, *time, *pos, *vel);
   aReturnVal.SetPointer(UtScriptVec3::Create(acc));
}
