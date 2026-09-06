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

#include "WsfScriptSixDOF_MoverTestObject.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_MoverTestObject.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
MoverTestObject::MoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfSixDOF_MoverTestObject");

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
   AddMethod(ut::make_unique<SetMasterFreezeSpeed>());
   AddMethod(ut::make_unique<SetMasterNoAlphaTesting>());
   AddMethod(ut::make_unique<CanFlyAt>());
   AddMethod(ut::make_unique<CreateAutopilotSupportFile>());
   AddMethod(ut::make_unique<CreateAutopilotSupportFileWithMaxMach>());
   AddMethod(ut::make_unique<CreateAutopilotConfigFile>());
   AddMethod(ut::make_unique<FlyStraightLevelFullThrottle>());
   AddMethod(ut::make_unique<TurnRateAt>());
   AddMethod(ut::make_unique<TurnRateWithConditions>());
   AddMethod(ut::make_unique<GetLevelFlightHorizontalThrust>());
   AddMethod(ut::make_unique<GetLevelFlightDrag>());
   AddMethod(ut::make_unique<GetLevelFlightFuelBurnRate>());
   AddMethod(ut::make_unique<SetVehicleMass>());
   AddMethod(ut::make_unique<GetMaxGLoad>());
   AddMethod(ut::make_unique<MaxInstantaneousGLoad>());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, GetRangeSinceStart_km, 0, "double", "") // NO_DOC
                                                                                                                // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetRangeSinceStart_km());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterNoLagTesting, 1, "void", "bool") // NO_DOC
                                                                                                                  // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterNoLagTesting(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeLocation, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeLocation(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeAltitude, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeAltitude(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeVerticalSpeed, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeVerticalSpeed(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezePitch, 1, "void", "bool") // NO_DOC
                                                                                                                 // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezePitch(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeRoll, 1, "void", "bool") // NO_DOC
                                                                                                                // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeRoll(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeYaw, 1, "void", "bool") // NO_DOC
                                                                                                               // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeYaw(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeFuelBurn, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeFuelBurn(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterFreezeSpeed, 1, "void", "bool") // NO_DOC
                                                                                                                 // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterFreezeSpeed(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetMasterNoAlphaTesting, 1, "void", "bool") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetMasterNoAlphaTesting(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, CanFlyAt, 4, "int", "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetInt(aObjectPtr->CanFlyAt(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                          UtMath::cFT_PER_M * aVarArgs[1].GetDouble(),
                                          aVarArgs[2].GetDouble(),
                                          aVarArgs[3].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, CreateAutopilotSupportFile, 1, "void", "string") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotSupportFile(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        CreateAutopilotSupportFileWithMaxMach,
                        2,
                        "void",
                        "string,double") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotSupportFileWithMaxMach(aVarArgs[0].GetString(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, CreateAutopilotConfigFile, 1, "void", "string") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->CreateAutopilotConfigFile(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, FlyStraightLevelFullThrottle, 0, "void", "") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->FlyStraightLevelFullThrottle();
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, TurnRateAt, 3, "double", "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->TurnRateAt(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                               UtMath::cFT_PER_M * aVarArgs[1].GetDouble(),
                                               aVarArgs[2].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        TurnRateWithConditions,
                        6,
                        "double",
                        "double,double,double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->TurnRateWithConditions(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                           UtMath::cLB_PER_KG * aVarArgs[1].GetDouble(),
                                                           aVarArgs[2].GetDouble(),
                                                           UtMath::cFT_PER_M * aVarArgs[3].GetDouble(),
                                                           UtMath::cFT_PER_M * aVarArgs[4].GetDouble(),
                                                           aVarArgs[5].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        GetLevelFlightHorizontalThrust,
                        4,
                        "double",
                        "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);

   double thrust_lbs = aObjectPtr->GetLevelFlightHorizontalThrust(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                                  aVarArgs[1].GetDouble(),
                                                                  aVarArgs[2].GetDouble(),
                                                                  UtMath::cLB_PER_KG * aVarArgs[3].GetDouble());

   aReturnVal.SetDouble(UtMath::cNT_PER_LB * thrust_lbs);
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        GetLevelFlightDrag,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);

   double drag_lbs = aObjectPtr->GetLevelFlightDrag(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                    aVarArgs[1].GetDouble(),
                                                    UtMath::cLB_PER_KG * aVarArgs[2].GetDouble());

   aReturnVal.SetDouble(UtMath::cNT_PER_LB * drag_lbs);
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        GetLevelFlightFuelBurnRate,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);

   double burnRate_pph = aObjectPtr->GetLevelFlightFuelBurnRate(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                                aVarArgs[1].GetDouble(),
                                                                UtMath::cLB_PER_KG * aVarArgs[2].GetDouble());

   aReturnVal.SetDouble(UtMath::cKG_PER_LB / 3600.0 * burnRate_pph);
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, SetVehicleMass, 1, "void", "double")
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->SetVehicleMass(UtMath::cLB_PER_KG * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject, wsf::six_dof::MoverTestObject, GetMaxGLoad, 0, "double", "") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->GetMaxGLoad());
}

UT_DEFINE_SCRIPT_METHOD(MoverTestObject,
                        wsf::six_dof::MoverTestObject,
                        MaxInstantaneousGLoad,
                        3,
                        "double",
                        "double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aReturnVal.SetDouble(aObjectPtr->MaxInstantaneousGLoad(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                          aVarArgs[1].GetDouble(),
                                                          UtMath::cLB_PER_KG * aVarArgs[2].GetDouble()));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
