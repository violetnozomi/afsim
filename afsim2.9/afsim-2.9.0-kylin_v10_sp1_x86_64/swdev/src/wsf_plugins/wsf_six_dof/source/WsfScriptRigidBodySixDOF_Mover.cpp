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

#include "WsfScriptRigidBodySixDOF_Mover.hpp"

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
RigidBodyMover::RigidBodyMover(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : Mover(aClassName, aTypesPtr)
{
   SetClassName("WsfRigidBodySixDOF_Mover");

   mEqualityComparable = false;

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<SetThrustReverserPosition>());
   AddMethod(ut::make_unique<RetractLandingGear>());
   AddMethod(ut::make_unique<LowerLandingGear>());
   AddMethod(ut::make_unique<GetLandingGearPosition>());
   AddMethod(ut::make_unique<WeightOnWheels>());
   AddMethod(ut::make_unique<WeightOnNoseWheel>());
   AddMethod(ut::make_unique<SetEnableNWS>());
   AddMethod(ut::make_unique<GetNoseWheelAngle>());
   AddMethod(ut::make_unique<EnableDirectBraking>());
   AddMethod(ut::make_unique<SetParkingBrake>());
   AddMethod(ut::make_unique<ApplyLeftGearBrake>());
   AddMethod(ut::make_unique<ApplyRightGearBrake>());
   AddMethod(ut::make_unique<ReleaseWheelBrakes>());
   AddMethod(ut::make_unique<ReleaseDirectBraking>());
   AddMethod(ut::make_unique<ApplyExternalForce>());
   AddMethod(ut::make_unique<RemoveExternalForce>());
   AddMethod(ut::make_unique<GetTaxiSpeedMax>());
   AddMethod(ut::make_unique<GetTaxiYawRateMax>());
   AddMethod(ut::make_unique<SetTaxiSpeedMax>());
   AddMethod(ut::make_unique<SetTaxiYawRateMax>());
   AddMethod(ut::make_unique<SetTaxiMode>());
   AddMethod(ut::make_unique<SetTaxiRadius>());
   AddMethod(ut::make_unique<GetThrustReverserPosition>());
   AddMethod(ut::make_unique<GetLandingGearLeverPosition>());
   AddMethod(ut::make_unique<GetAngleOfControlSurface>());
   AddMethod(ut::make_unique<GetListOfControlSurfaceNames>());
   AddMethod(ut::make_unique<GetAileronsBothPosition>());
   AddMethod(ut::make_unique<GetStabilatorLeftPosition>());
   AddMethod(ut::make_unique<GetStabilatorRightPosition>());
   AddMethod(ut::make_unique<GetRudderPosition>());
   AddMethod(ut::make_unique<GetYawMoment>());
   AddMethod(ut::make_unique<GetPitchMoment>());
   AddMethod(ut::make_unique<GetRollMoment>());

   // Undocumented test functions
   AddMethod(ut::make_unique<GetRigidBodyTestSupportObject>("__getTestObject"));
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, ApplyExternalForce, 2, "void", "double, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyExternalForce(UtMath::cLB_PER_NT * aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, RemoveExternalForce, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RemoveExternalForce();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, EnableDirectBraking, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectBraking();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetParkingBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetParkingBrake();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, ApplyLeftGearBrake, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyLeftGearBrake(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, ApplyRightGearBrake, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyRightGearBrake(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, ReleaseWheelBrakes, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseWheelBrakes();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, ReleaseDirectBraking, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectBraking();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, RetractLandingGear, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RetractLandingGear();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, LowerLandingGear, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->LowerLandingGear();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetLandingGearPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLandingGearPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetAngleOfControlSurface, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAngleOfControlSurface(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetListOfControlSurfaceNames, 0, "Array<string>", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtScriptDataList*        resultVecPtr = new UtScriptDataList;
   std::vector<std::string> surfaceList  = aObjectPtr->GetListOfControlSurfaceNames();
   for (auto& item : surfaceList)
   {
      resultVecPtr->push_back(UtScriptData(item));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetAileronsBothPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAileronsBothPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetStabilatorLeftPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStabilatorLeftPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetStabilatorRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStabilatorRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetRudderPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRudderPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetThrustReverserPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrustReverserControlPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetLandingGearLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLandingGearLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetThrustReverserPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetThrustReverserPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, WeightOnWheels, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetWeightOnWheels());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, WeightOnNoseWheel, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetWeightOnNoseWheel());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetEnableNWS, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetEnableNWS(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetNoseWheelAngle, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNoseWheelAngle());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetTaxiSpeedMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cM_PER_FT * aObjectPtr->GetTaxiSpeedMax());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetTaxiYawRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTaxiYawRateMax());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetTaxiSpeedMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiSpeedMax(UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetTaxiYawRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiYawRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetTaxiMode, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiMode(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, SetTaxiRadius, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiRadius(UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetYawMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawMoment_nm());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetPitchMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchMoment_nm());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover, wsf::six_dof::RigidBodyMover, GetRollMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRollMoment_nm());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMover,
                        wsf::six_dof::RigidBodyMover,
                        GetRigidBodyTestSupportObject,
                        0,
                        "WsfRigidBodySixDOF_MoverTestObject",
                        "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetRigidBodyTestSupportObject(), aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
