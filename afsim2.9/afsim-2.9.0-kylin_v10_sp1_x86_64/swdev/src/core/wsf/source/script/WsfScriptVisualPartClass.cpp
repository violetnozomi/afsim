// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptVisualPartClass.hpp"

#include "UtMemory.hpp"
#include "WsfSimulation.hpp"
#include "WsfVisualPart.hpp"
#include "WsfVisualPartTypes.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptVisualPartClass::WsfScriptVisualPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptArticulatedPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfVisualPart");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptVisualPartClass, WsfScriptVisualPartClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool           isA_TypeOf = false;
   WsfVisualPart* objectPtr  = WsfVisualPartTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != 0)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptVisualPartClass, WsfVisualPart, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != 0)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptVisualPartClass, WsfVisualPart, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != 0)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}
