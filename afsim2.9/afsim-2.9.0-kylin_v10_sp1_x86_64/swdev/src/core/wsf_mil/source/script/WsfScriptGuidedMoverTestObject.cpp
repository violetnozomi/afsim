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

#include "WsfScriptGuidedMoverTestObject.hpp"

#include "WsfGuidedMoverTestObject.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptGuidedMoverTestObject::WsfScriptGuidedMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfGuidedMoverTestObject");

   // Undocumented test functions
   AddMethod(ut::make_unique<GetLevelFlightThrust>());
   AddMethod(ut::make_unique<GetLevelFlightDrag>());
   AddMethod(ut::make_unique<GetLevelFlightFuelBurnRate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverTestObject,
                        WsfGuidedMoverTestObject,
                        GetLevelFlightThrust,
                        2,
                        "double",
                        "double,double") // NO_DOC | FOR_TEST_ONLY
{
   aReturnVal.SetDouble(aObjectPtr->GetLevelFlightThrust_lbs(TIME_NOW, aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverTestObject,
                        WsfGuidedMoverTestObject,
                        GetLevelFlightDrag,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   aReturnVal.SetDouble(
      aObjectPtr->GetLevelFlightDrag_lbs(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverTestObject,
                        WsfGuidedMoverTestObject,
                        GetLevelFlightFuelBurnRate,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   aReturnVal.SetDouble(aObjectPtr->GetLevelFlightFuelBurnRate_lbsphr(TIME_NOW,
                                                                      aVarArgs[0].GetDouble(),
                                                                      aVarArgs[1].GetDouble(),
                                                                      aVarArgs[2].GetDouble()));
}
