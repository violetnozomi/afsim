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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfScriptP6DOF_MoverTestObject.hpp"

#include "P6DofUtils.hpp"
#include "UtMemory.hpp"
#include "WsfP6DOF_MoverTestObject.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptP6DOF_MoverTestObject::WsfScriptP6DOF_MoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfP6DOF_MoverTestObject");

   // Add each of the method objects to the class.

   // Undocumented test functions
   AddMethod(ut::make_unique<GetRangeSinceStart_km>());
   AddMethod(ut::make_unique<SetMasterNoLagTesting>());
   AddMethod(ut::make_unique<SetMasterFreezeLocation>());
   AddMethod(ut::make_unique<SetMasterFreezeAltitude>());
   AddMethod(ut::make_unique<SetMasterFreezeVerticalSpeed>());
   AddMethod(ut::make_unique<SetMasterFreezePitch>());
   AddMethod(ut::make_unique<SetMasterFreezeRoll>());
   AddMethod(ut::make_unique<SetMasterFreezeYaw>());
   AddMethod(ut::make_unique<SetMasterFreezeFuelBurn>());
   AddMethod(ut::make_unique<SetMasterNoAlphaTesting>());
   AddMethod(ut::make_unique<Autotune>());
   AddMethod(ut::make_unique<CanFlyAt>());
   AddMethod(ut::make_unique<CreateAutopilotSupportFile>());
   AddMethod(ut::make_unique<CreateAutopilotSupportFileWithMaxMach>());
   AddMethod(ut::make_unique<CreateAutopilotConfigFile>());
   AddMethod(ut::make_unique<FlyStraightLevelFullThrottle>());
   AddMethod(ut::make_unique<TurnRateAt>());
   AddMethod(ut::make_unique<TurnRateWithConditions>());
   AddMethod(ut::make_unique<GetLevelFlightHorizontalThrust>());
   AddMethod(ut::make_unique<GetLevelFlightDrag>());
   AddMethod(ut::make_unique<GetPitchingMoment>());
   AddMethod(ut::make_unique<GetLevelFlightFuelBurnRate>());
   AddMethod(ut::make_unique<SetVehicleMass>());
   AddMethod(ut::make_unique<GetMaxGLoad>());
   AddMethod(ut::make_unique<MaxInstantaneousGLoad>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, GetRangeSinceStart_km, 0, "double", "") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetRangeSinceStart_km());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterNoLagTesting, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterNoLagTesting(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezeLocation, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeLocation(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezeAltitude, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeAltitude(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        SetMasterFreezeVerticalSpeed,
                        1,
                        "void",
                        "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeVerticalSpeed(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezePitch, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezePitch(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezeRoll, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeRoll(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezeYaw, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeYaw(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterFreezeFuelBurn, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeFuelBurn(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetMasterNoAlphaTesting, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterNoAlphaTesting(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, Autotune, 0, "void", "") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->Autotune();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        CanFlyAt,
                        4,
                        "int",
                        "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetInt(aObjectPtr->CanFlyAt(aVarArgs[0].GetDouble(),
                                          aVarArgs[1].GetDouble(),
                                          aVarArgs[2].GetDouble(),
                                          aVarArgs[3].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        CreateAutopilotSupportFile,
                        1,
                        "void",
                        "string") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotSupportFile(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        CreateAutopilotSupportFileWithMaxMach,
                        2,
                        "void",
                        "string,double") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotSupportFileWithMaxMach(aVarArgs[0].GetString(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        CreateAutopilotConfigFile,
                        1,
                        "void",
                        "string") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotConfigFile(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        FlyStraightLevelFullThrottle,
                        0,
                        "void",
                        "") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->FlyStraightLevelFullThrottle();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        TurnRateAt,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->TurnRateAt(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        TurnRateWithConditions,
                        6,
                        "double",
                        "double,double,double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->TurnRateWithConditions(aVarArgs[0].GetDouble(),
                                                           aVarArgs[1].GetDouble(),
                                                           aVarArgs[2].GetDouble(),
                                                           aVarArgs[3].GetDouble(),
                                                           aVarArgs[4].GetDouble(),
                                                           aVarArgs[5].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        GetLevelFlightHorizontalThrust,
                        4,
                        "double",
                        "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetLevelFlightHorizontalThrust(aVarArgs[0].GetDouble(),
                                                                   aVarArgs[1].GetDouble(),
                                                                   aVarArgs[2].GetDouble(),
                                                                   aVarArgs[3].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        GetLevelFlightDrag,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(
      aObjectPtr->GetLevelFlightDrag(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        GetPitchingMoment,
                        4,
                        "double",
                        "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetPitchingMoment(aVarArgs[0].GetDouble(),
                                                      aVarArgs[1].GetDouble(),
                                                      aVarArgs[2].GetDouble(),
                                                      aVarArgs[3].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        GetLevelFlightFuelBurnRate,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(
      aObjectPtr->GetLevelFlightFuelBurnRate(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, SetVehicleMass, 1, "void", "double")
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetVehicleMass(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject, WsfP6DOF_MoverTestObject, GetMaxGLoad, 0, "double", "") // NO_DOC
                                                                                                                // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetMaxGLoad());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverTestObject,
                        WsfP6DOF_MoverTestObject,
                        MaxInstantaneousGLoad,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(
      aObjectPtr->MaxInstantaneousGLoad(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()));
}
