// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptManeuverSequence.hpp"

#include "UtMemory.hpp"
#include "WsfManeuver.hpp"
#include "WsfManeuverSequence.hpp"

WsfScriptManeuverSequence::WsfScriptManeuverSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfManeuverSequence");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<IsLoop>());
   AddMethod(ut::make_unique<SetLoop>());
   AddMethod(ut::make_unique<Clear>());
   AddMethod(ut::make_unique<Append>());
   AddMethod(ut::make_unique<InsertNext>());
   AddMethod(ut::make_unique<GetCurrentManeuver>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, Construct, 0, "WsfManeuverSequence", "")
{
   aReturnVal.SetPointer(new UtScriptRef(new WsfManeuverSequence{}, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, IsLoop, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLoop());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, SetLoop, 1, "void", "bool")
{
   aObjectPtr->SetLoop(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, Clear, 0, "void", "")
{
   aObjectPtr->Clear();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, Append, 1, "void", "WsfManeuver")
{
   auto mvrPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuver>();
   auto argPtr = ut::clone(mvrPtr);
   aObjectPtr->Append(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, InsertNext, 1, "void", "WsfManeuver")
{
   auto mvrPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuver>();
   auto argPtr = ut::clone(mvrPtr);
   aObjectPtr->InsertNext(std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverSequence, WsfManeuverSequence, GetCurrentManeuver, 0, "WsfManeuver", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentManeuver(), aReturnClassPtr));
}
