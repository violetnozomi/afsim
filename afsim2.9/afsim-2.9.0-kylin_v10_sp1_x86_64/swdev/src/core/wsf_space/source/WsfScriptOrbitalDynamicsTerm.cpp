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

#include "WsfScriptOrbitalDynamicsTerm.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"

WsfScriptOrbitalDynamicsTerm::WsfScriptOrbitalDynamicsTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalDynamicsTerm");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<TermType>());
   AddMethod(ut::make_unique<ComputeAcceleration>());
}

void WsfScriptOrbitalDynamicsTerm::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfOrbitalDynamicsTerm*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamicsTerm, WsfOrbitalDynamicsTerm, TermType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTermType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalDynamicsTerm, WsfOrbitalDynamicsTerm, ComputeAcceleration, 4, "Vec3", "double, Calendar, Vec3, Vec3")
{
   double  mass = aVarArgs[0].GetDouble();
   auto    time = aVarArgs[1].GetPointer()->GetAppObject<UtCalendar>();
   auto    pos  = aVarArgs[2].GetPointer()->GetAppObject<UtVec3d>();
   auto    vel  = aVarArgs[3].GetPointer()->GetAppObject<UtVec3d>();
   UtVec3d acc  = aObjectPtr->ComputeAcceleration(mass, *time, *pos, *vel);
   aReturnVal.SetPointer(UtScriptVec3::Create(acc));
}
