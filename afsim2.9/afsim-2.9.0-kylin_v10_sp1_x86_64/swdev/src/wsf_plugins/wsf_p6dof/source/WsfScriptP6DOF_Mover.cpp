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

#include "WsfScriptP6DOF_Mover.hpp"

#include "P6DofScenario.hpp"
#include "P6DofUtils.hpp"
#include "UtMemory.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"
#include "maneuvers/WsfManeuver.hpp"
#include "maneuvers/WsfManeuverSequence.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptP6DOF_MoverClass::WsfScriptP6DOF_MoverClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptMoverClass(aClassName, aTypesPtr)
{
   SetClassName("WsfP6DOF_Mover");

   mEqualityComparable = false;

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<GetAltitude>());
   AddMethod(ut::make_unique<GetHeading>());
   AddMethod(ut::make_unique<GetPitch>());
   AddMethod(ut::make_unique<GetRoll>());
   AddMethod(ut::make_unique<GetVerticalSpeed>());
   AddMethod(ut::make_unique<GetFlightPathAngle>());
   AddMethod(ut::make_unique<GetYawRate>());
   AddMethod(ut::make_unique<GetPitchRate>());
   AddMethod(ut::make_unique<GetRollRate>());
   AddMethod(ut::make_unique<GetGLoad>());
   AddMethod(ut::make_unique<GetNx>());
   AddMethod(ut::make_unique<GetNy>());
   AddMethod(ut::make_unique<GetNz>());
   AddMethod(ut::make_unique<GetKCAS>());
   AddMethod(ut::make_unique<GetKIAS>());
   AddMethod(ut::make_unique<GetKTAS>());
   AddMethod(ut::make_unique<GetMach>());
   AddMethod(ut::make_unique<GetDynamicPressure>());
   AddMethod(ut::make_unique<GetTotalWeight>());
   AddMethod(ut::make_unique<GetCurrentWeight>());
   AddMethod(ut::make_unique<GetEmptyWeight>());
   AddMethod(ut::make_unique<GetCgX>());
   AddMethod(ut::make_unique<GetCgY>());
   AddMethod(ut::make_unique<GetCgZ>());
   AddMethod(ut::make_unique<StartupEngines>());
   AddMethod(ut::make_unique<ShutdownEngines>());
   AddMethod(ut::make_unique<SetThrustReverserPosition>());
   AddMethod(ut::make_unique<IsProducingThrust>());
   AddMethod(ut::make_unique<GetTotalThrust>());
   AddMethod(ut::make_unique<GetEngineThrust>());
   AddMethod(ut::make_unique<GetAfterburnerOn>());
   AddMethod(ut::make_unique<GetEngineFuelFlowRate>());
   AddMethod(ut::make_unique<GetEngineAfterburnerOn>());
   AddMethod(ut::make_unique<SetFuelFeed>());
   AddMethod(ut::make_unique<SetFuelFeedAllEngines>());
   AddMethod(ut::make_unique<AddFuel>());
   AddMethod(ut::make_unique<GetTotalFuelCapacity>());
   AddMethod(ut::make_unique<GetInternalFuelCapacity>());
   AddMethod(ut::make_unique<GetExternalFuelCapacity>());
   AddMethod(ut::make_unique<GetTotalFuelRemaining>());
   AddMethod(ut::make_unique<GetInternalFuelRemaining>());
   AddMethod(ut::make_unique<GetExternalFuelRemaining>());
   AddMethod(ut::make_unique<GetFuelTankCapacity>());
   AddMethod(ut::make_unique<GetFuelInTank>());
   AddMethod(ut::make_unique<SetFuelInTank>());
   AddMethod(ut::make_unique<GetTotalFuelFlowRate>());
   AddMethod(ut::make_unique<AddFuelTransfer>());
   AddMethod(ut::make_unique<RemoveFuelTransfer>());
   AddMethod(ut::make_unique<SetJokerFuelState>());
   AddMethod(ut::make_unique<GetJokerFuelState>());
   AddMethod(ut::make_unique<GetJokerFuelReached>());
   AddMethod(ut::make_unique<SetBingoFuelState>());
   AddMethod(ut::make_unique<GetBingoFuelState>());
   AddMethod(ut::make_unique<GetBingoFuelReached>());
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
   AddMethod(ut::make_unique<ActivateSequencer>());
   AddMethod(ut::make_unique<GetCurrentManeuver>());
   AddMethod(ut::make_unique<GetManeuverSequence>());
   AddMethod(ut::make_unique<ExecuteManeuver>());
   AddMethod(ut::make_unique<ExecuteManeuverSequence>());
   AddMethod(ut::make_unique<CancelManeuvers>());
   AddMethod(ut::make_unique<GetActivePilot>());
   AddMethod(ut::make_unique<ActivateManualPilot>());
   AddMethod(ut::make_unique<ActivateSyntheticPilot>());
   AddMethod(ut::make_unique<ActivateHardwareAutopilot>());
   AddMethod(ut::make_unique<ActivateGuidanceAutopilot>());
   AddMethod(ut::make_unique<GetPitchGLoadMin>());
   AddMethod(ut::make_unique<GetPitchGLoadMax>());
   AddMethod(ut::make_unique<GetAlphaMin>());
   AddMethod(ut::make_unique<GetAlphaMax>());
   AddMethod(ut::make_unique<GetPitchRateMin>());
   AddMethod(ut::make_unique<GetPitchRateMax>());
   AddMethod(ut::make_unique<GetVerticalSpeedMin>());
   AddMethod(ut::make_unique<GetVerticalSpeedMax>());
   AddMethod(ut::make_unique<GetYawGLoadMax>());
   AddMethod(ut::make_unique<GetBetaMax>());
   AddMethod(ut::make_unique<GetYawRateMax>());
   AddMethod(ut::make_unique<GetRollRateMax>());
   AddMethod(ut::make_unique<GetBankAngleMax>());
   AddMethod(ut::make_unique<GetForwardAccelMin>());
   AddMethod(ut::make_unique<GetForwardAccelMax>());
   AddMethod(ut::make_unique<GetTaxiSpeedMax>());
   AddMethod(ut::make_unique<GetTaxiYawRateMax>());
   AddMethod(ut::make_unique<RevertLimitsToDefaults>());
   AddMethod(ut::make_unique<SetPitchGLoadMin>());
   AddMethod(ut::make_unique<SetPitchGLoadMax>());
   AddMethod(ut::make_unique<SetAlphaMin>());
   AddMethod(ut::make_unique<SetAlphaMax>());
   AddMethod(ut::make_unique<SetPitchRateMin>());
   AddMethod(ut::make_unique<SetPitchRateMax>());
   AddMethod(ut::make_unique<SetVerticalSpeedMin>());
   AddMethod(ut::make_unique<SetVerticalSpeedMax>());
   AddMethod(ut::make_unique<SetYawGLoadMax>());
   AddMethod(ut::make_unique<SetBetaMax>());
   AddMethod(ut::make_unique<SetYawRateMax>());
   AddMethod(ut::make_unique<SetRollRateMax>());
   AddMethod(ut::make_unique<SetBankAngleMax>());
   AddMethod(ut::make_unique<SetForwardAccelMin>());
   AddMethod(ut::make_unique<SetForwardAccelMax>());
   AddMethod(ut::make_unique<SetTaxiSpeedMax>());
   AddMethod(ut::make_unique<SetTaxiYawRateMax>());
   AddMethod(ut::make_unique<SetTurnRollInMultiplier>());
   AddMethod(ut::make_unique<GetCurrentTurnRollInMultiplier>());
   AddMethod(ut::make_unique<GetDefaultTurnRollInMultiplier>());
   AddMethod(ut::make_unique<SetRouteAllowableAngleError>());
   AddMethod(ut::make_unique<GetCurrentRouteAllowableAngleError>());
   AddMethod(ut::make_unique<GetDefaultRouteAllowableAngleError>());
   AddMethod(ut::make_unique<EnableAutopilot>());
   AddMethod(ut::make_unique<SetTaxiMode>());
   AddMethod(ut::make_unique<SetTaxiRadius>());
   AddMethod(ut::make_unique<SetAutopilotPitchAngle>());
   AddMethod(ut::make_unique<SetAutopilotFlightPathAngle>());
   AddMethod(ut::make_unique<SetAutopilotVerticalSpeed>());
   AddMethod(ut::make_unique<SetAutopilotAltitude>());
   AddMethod(ut::make_unique<SetAutopilotRollAngle>());
   AddMethod(ut::make_unique<SetAutopilotRollRate>());
   AddMethod(ut::make_unique<SetPitchGLoad>());
   AddMethod(ut::make_unique<SetAutopilotPitchRate>());
   AddMethod(ut::make_unique<SetAutopilotDeltaPitch>());
   AddMethod(ut::make_unique<SetAutopilotDeltaRoll>());
   AddMethod(ut::make_unique<SetAutopilotSpeedKTAS>());
   AddMethod(ut::make_unique<SetAutopilotSpeedKCAS>());
   AddMethod(ut::make_unique<SetAutopilotSpeedKIAS>());
   AddMethod(ut::make_unique<SetAutopilotSpeedMach>());
   AddMethod(ut::make_unique<SetAutopilotLateralWaypointMode>());
   AddMethod(ut::make_unique<SetAutopilotVerticalWaypointMode>());
   AddMethod(ut::make_unique<SetAutopilotSpeedWaypointMode>());
   AddMethod(ut::make_unique<SetAutopilotWaypointMode>());
   AddMethod(ut::make_unique<SetAutopilotNoControl>());
   AddMethod(ut::make_unique<GetAutopilotLateralMode>());
   AddMethod(ut::make_unique<GetAutopilotLateralModeValue>());
   AddMethod(ut::make_unique<GetAutopilotVerticalMode>());
   AddMethod(ut::make_unique<GetAutopilotVerticalModeValue>());
   AddMethod(ut::make_unique<GetAutopilotSpeedMode>());
   AddMethod(ut::make_unique<GetAutopilotSpeedModeValue>());
   AddMethod(ut::make_unique<GetEngineIsOperating>());
   AddMethod(ut::make_unique<GetEngineIsSmoking>());
   AddMethod(ut::make_unique<GetEngineAfterburnerIsOn>());
   AddMethod(ut::make_unique<GetContrailTrailingEffect>());
   AddMethod(ut::make_unique<GetRocketSmokeTrailingEffect>());
   AddMethod(ut::make_unique<GetDamageSmokeTrailingEffect>());
   AddMethod(ut::make_unique<GetLaunchFlashSmokeIsPresent>());
   AddMethod(ut::make_unique<GetRisingSmokePlumeIsPresent>());
   AddMethod(ut::make_unique<GetFlamesArePresent>());
   AddMethod(ut::make_unique<GetIsLightlyDamaged>());
   AddMethod(ut::make_unique<GetIsHeavilyDamaged>());
   AddMethod(ut::make_unique<SetLightDamage>());
   AddMethod(ut::make_unique<SetHeavyDamage>());
   AddMethod(ut::make_unique<SetDestroyed>());
   AddMethod(ut::make_unique<EnableThrustVectoring>());
   AddMethod(ut::make_unique<EnableControls>());
   AddMethod(ut::make_unique<TakeDirectControlInput>());
   AddMethod(ut::make_unique<ReleaseDirectControlInput>());
   AddMethod(ut::make_unique<SetDirectControlInputs>());
   AddMethod(ut::make_unique<EnableDirectThrottleInput>());
   AddMethod(ut::make_unique<MoveThrottleToIdle>());
   AddMethod(ut::make_unique<MoveThrottleToFull>());
   AddMethod(ut::make_unique<MoveThrottleToMilitary>());
   AddMethod(ut::make_unique<MoveThrottleToAfterburner>());
   AddMethod(ut::make_unique<ReleaseDirectThrottleInput>());
   AddMethod(ut::make_unique<EnableDirectSpeedBrakeInput>());
   AddMethod(ut::make_unique<OpenSpeedBrake>());
   AddMethod(ut::make_unique<CloseSpeedBrake>());
   AddMethod(ut::make_unique<ReleaseDirectSpeedBrakeInput>());
   AddMethod(ut::make_unique<GetSpeedBrakePosition>());
   AddMethod(ut::make_unique<SetFlapsPosition>());
   AddMethod(ut::make_unique<GetFlapsPosition>());
   AddMethod(ut::make_unique<SetSpoilersPosition>());
   AddMethod(ut::make_unique<GetSpoilersPosition>());
   AddMethod(ut::make_unique<GetStickRightPosition>());
   AddMethod(ut::make_unique<GetStickBackPosition>());
   AddMethod(ut::make_unique<GetRudderRightPosition>());
   AddMethod(ut::make_unique<GetThrottleMilitaryPosition>());
   AddMethod(ut::make_unique<GetThrottleAfterburnerPosition>());
   AddMethod(ut::make_unique<GetThrustReverserPosition>());
   AddMethod(ut::make_unique<GetSpeedBrakeControlPosition>());
   AddMethod(ut::make_unique<GetFlapsLeverPosition>());
   AddMethod(ut::make_unique<GetSpoilersLeverPosition>());
   AddMethod(ut::make_unique<GetLandingGearLeverPosition>());
   AddMethod(ut::make_unique<GetAngleOfControlSurface>());
   AddMethod(ut::make_unique<GetListOfControlSurfaceNames>());
   AddMethod(ut::make_unique<GetAileronsBothPosition>());
   AddMethod(ut::make_unique<GetStabilatorLeftPosition>());
   AddMethod(ut::make_unique<GetStabilatorRightPosition>());
   AddMethod(ut::make_unique<GetRudderPosition>());
   AddMethod(ut::make_unique<GetLift>());
   AddMethod(ut::make_unique<GetDrag>());
   AddMethod(ut::make_unique<GetSideForce>());
   AddMethod(ut::make_unique<GetThrustMagnitude>());
   AddMethod(ut::make_unique<GetYawMoment>());
   AddMethod(ut::make_unique<GetPitchMoment>());
   AddMethod(ut::make_unique<GetRollMoment>());
   AddMethod(ut::make_unique<GetAlpha>());
   AddMethod(ut::make_unique<GetAlphaDot>());
   AddMethod(ut::make_unique<GetBeta>());
   AddMethod(ut::make_unique<GetBetaDot>());
   AddMethod(ut::make_unique<GetCLMaxAtMach>());
   AddMethod(ut::make_unique<GetAlphaAtCLMaxAtMach>());
   AddMethod(ut::make_unique<MaxPotentialManeuverGLoad>());
   AddMethod(ut::make_unique<CalcKtasFromKcas>());
   AddMethod(ut::make_unique<CalcKtasFromMach>());
   AddMethod(ut::make_unique<CalcKtasFromFps>());
   AddMethod(ut::make_unique<CalcKcasFromKtas>());
   AddMethod(ut::make_unique<CalcKcasFromMach>());
   AddMethod(ut::make_unique<CalcKcasFromFps>());
   AddMethod(ut::make_unique<CalcMachFromKcas>());
   AddMethod(ut::make_unique<CalcMachFromKtas>());
   AddMethod(ut::make_unique<CalcMachFromFps>());
   AddMethod(ut::make_unique<CalcFpsFromKcas>());
   AddMethod(ut::make_unique<CalcFpsFromKtas>());
   AddMethod(ut::make_unique<CalcFpsFromMach>());
   AddMethod(ut::make_unique<CalcDynamicPressure_lbft2>());
   AddMethod(ut::make_unique<CalcFpsFromAltitudeDynamicPressure>());

   // Undocumented test functions
   AddMethod(ut::make_unique<GetTestSupportObject>("__getTestObject"));
   AddMethod(ut::make_unique<SetTestingIgnoreAllCrashes>());
   AddMethod(ut::make_unique<GetTestingIgnoreAllCrashes>());
   AddMethod(ut::make_unique<SetTestingClampToSealLevelMinAlt>());
   AddMethod(ut::make_unique<GetTestingClampToSealLevelMinAlt>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ApplyExternalForce, 2, "void", "double, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyExternalForce(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, RemoveExternalForce, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RemoveExternalForce();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableDirectBraking, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectBraking();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetParkingBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetParkingBrake();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ApplyLeftGearBrake, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyLeftGearBrake(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ApplyRightGearBrake, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ApplyRightGearBrake(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ReleaseWheelBrakes, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseWheelBrakes();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ReleaseDirectBraking, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectBraking();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, AddFuel, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->AddFuel(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTotalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetInternalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetInternalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetExternalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetExternalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTotalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetInternalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetInternalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetExternalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetExternalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFuelTankCapacity, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFuelTankCapacity(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFuelInTank, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFuelInTank(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetFuelInTank, 2, "void", "string, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetFuelInTank(aVarArgs[0].GetString(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, StartupEngines, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->StartupEngines(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ShutdownEngines, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ShutdownEngines(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTotalWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCurrentWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEmptyWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEmptyWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCgX, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCg();
   aReturnVal.SetDouble(cgVec.X());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCgY, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCg();
   aReturnVal.SetDouble(cgVec.Y());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCgZ, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCg();
   aReturnVal.SetDouble(cgVec.Z());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, RetractLandingGear, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RetractLandingGear();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, LowerLandingGear, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->LowerLandingGear();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetLandingGearPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLandingGearPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAngleOfControlSurface, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAngleOfControlSurface(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetListOfControlSurfaceNames, 0, "Array<string>", "")
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

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAileronsBothPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAileronsBothPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetStabilatorLeftPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStabilatorLeftPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetStabilatorRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStabilatorRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRudderPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRudderPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetFlapsPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetFlapsPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFlapsPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlapsPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetSpoilersPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetSpoilersPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetSpoilersPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpoilersPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetStickRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStickRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetStickBackPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStickBackPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRudderRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRudderRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetThrottleMilitaryPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrottleMilitaryPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetThrottleAfterburnerPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrottleAfterburnerPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetThrustReverserPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrustReverserControlPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetSpeedBrakeControlPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeedBrakeControlPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFlapsLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlapsLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetSpoilersLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpoilersLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetLandingGearLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLandingGearLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetThrustReverserPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetThrustReverserPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, WeightOnWheels, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetWeightOnWheels());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, WeightOnNoseWheel, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetWeightOnNoseWheel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, IsProducingThrust, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->IsProducingThrust());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTotalThrust, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalThrust());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineThrust, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEngineThrust(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAfterburnerOn, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetAfterburnerOn());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineFuelFlowRate, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEngineFuelFlowRate(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTotalFuelFlowRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelFlowRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, AddFuelTransfer, 3, "bool", "string, string, string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(
      aObjectPtr->AddFuelTransfer(aVarArgs[0].GetString(), aVarArgs[1].GetString(), aVarArgs[2].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, RemoveFuelTransfer, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->RemoveFuelTransfer(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetJokerFuelState, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetJokerFuelState(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetJokerFuelState, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetJokerFuelState());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetJokerFuelReached, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->JokerFuelReached());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetBingoFuelState, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBingoFuelState(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBingoFuelState, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBingoFuelState());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBingoFuelReached, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->BingoFuelReached());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineAfterburnerOn, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineAfterburnerOn(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetFuelFeed, 2, "bool", "string, string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->SetFuelFeed(aVarArgs[0].GetString(), aVarArgs[1].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetFuelFeedAllEngines, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->SetFuelFeed(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetEnableNWS, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetEnableNWS(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetNoseWheelAngle, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNoseWheelAngle());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetActivePilot, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetActivePilot());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ActivateManualPilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateManualPilot();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ActivateSyntheticPilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateSyntheticPilot();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ActivateHardwareAutopilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateHardwareAutopilot();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ActivateGuidanceAutopilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateGuidanceAutopilot();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, RevertLimitsToDefaults, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RevertLimitsToDefaults();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchGLoadMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchGLoadMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchGLoadMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchGLoadMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAlphaMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAlphaMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchRateMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchRateMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchRateMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetVerticalSpeedMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetVerticalSpeedMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetVerticalSpeedMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetVerticalSpeedMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetYawGLoadMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawGLoadMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBetaMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBetaMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetYawRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawRateMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRollRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRollRateMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBankAngleMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBankAngleMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetForwardAccelMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetForwardAccelMin());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetForwardAccelMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetForwardAccelMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTaxiSpeedMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTaxiSpeedMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTaxiYawRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTaxiYawRateMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetPitchGLoadMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoadMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetPitchGLoadMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAlphaMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAlphaMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAlphaMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAlphaMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetPitchRateMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchRateMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetPitchRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetVerticalSpeedMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetVerticalSpeedMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetVerticalSpeedMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetVerticalSpeedMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetYawGLoadMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetYawGLoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetBetaMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBetaMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetYawRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetYawRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetRollRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetRollRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetBankAngleMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBankAngleMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetForwardAccelMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetForwardAccelMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetForwardAccelMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetForwardAccelMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTaxiSpeedMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiSpeedMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTaxiYawRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiYawRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTurnRollInMultiplier, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTurnRollInMultiplier(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCurrentTurnRollInMultiplier, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentTurnRollInMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetDefaultTurnRollInMultiplier, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDefaultTurnRollInMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetRouteAllowableAngleError, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetRouteAllowableAngleError(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCurrentRouteAllowableAngleError, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentRouteAllowableAngleError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetDefaultRouteAllowableAngleError, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDefaultRouteAllowableAngleError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTaxiMode, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiMode(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTaxiRadius, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTaxiRadius(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotPitchAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotPitchAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotFlightPathAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotFlightPathAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotVerticalSpeed, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotVerticalSpeed(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotAltitude, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotAltitude(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotRollAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotRollAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotRollRate, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotRollRate(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetPitchGLoad, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoad(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotPitchRate, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotPitchRate(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotDeltaPitch, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotDeltaPitch(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotDeltaRoll, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotDeltaRoll(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotSpeedKTAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKTAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotSpeedKCAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKCAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotSpeedKIAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKCAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotSpeedMach, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedMach(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotLateralWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotLateralWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotVerticalWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotVerticalWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotSpeedWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetAutopilotNoControl, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotNoControl();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAltitude, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlt_m());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetHeading, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetHeading());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitch, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitch());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRoll, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRoll());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetVerticalSpeed, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetVerticalSpeed());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetYawRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRollRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRollRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFlightPathAngle, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlightPathAngle());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetGLoad, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetGLoad());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetNx, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNx_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetNy, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNy_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetNz, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNz_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetKCAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KCAS());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetKIAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KIAS());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetKTAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KTAS());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetMach, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_Mach());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetDynamicPressure, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDynamicPressure_psf());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableDirectThrottleInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectThrottleInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, MoveThrottleToIdle, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToIdle();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, MoveThrottleToFull, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToFull();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, MoveThrottleToMilitary, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToFull();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, MoveThrottleToAfterburner, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToAfterburner();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ReleaseDirectThrottleInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectThrottleInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableDirectSpeedBrakeInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectSpeedBrakeInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, OpenSpeedBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->OpenSpeedBrake();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CloseSpeedBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->CloseSpeedBrake();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ReleaseDirectSpeedBrakeInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectSpeedBrakeInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetSpeedBrakePosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeedBrakePosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCLMaxAtMach, 1, "double", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCLMaxAtMach(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAlphaAtCLMaxAtMach, 1, "double", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaAtCLMaxAtMach_deg(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ActivateSequencer, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->ActivateSequencer(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetLift, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLift_lbs() * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetDrag, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDrag_lbs() * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetSideForce, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSideForce_lbs() * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetThrustMagnitude, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrustMagnitude_lbs() * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetYawMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawMoment_ftlbs() * UtMath::cNT_PER_LB * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetPitchMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchMoment_ftlbs() * UtMath::cNT_PER_LB * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRollMoment, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRollMoment_ftlbs() * UtMath::cNT_PER_LB * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAlpha, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlpha_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBeta, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBeta_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAlphaDot, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaDot_dps());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetBetaDot, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBetaDot_dps());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableThrustVectoring, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableThrustVectoring(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableControls, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableControls(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, MaxPotentialManeuverGLoad, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetP6DofVehicle()->MaxPotentialManeuverGLoad());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotLateralMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotLateralMode());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotLateralModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotLateralModeValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotVerticalMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotVerticalMode());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotVerticalModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotVerticalModeValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotSpeedMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotSpeedMode());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetAutopilotSpeedModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotSpeedModeValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, TakeDirectControlInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->TakeDirectControlInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ReleaseDirectControlInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectControlInput();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetDirectControlInputs, 4, "void", "double, double, double, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectControlInputs(aVarArgs[0].GetDouble(),  // aStickX
                                      aVarArgs[1].GetDouble(),  // aStickY
                                      aVarArgs[2].GetDouble(),  // aRudder
                                      aVarArgs[3].GetDouble()); // aThrottleLeverPosition
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, EnableAutopilot, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableAutopilot(aVarArgs[0].GetBool());
}

P6DofAtmosphere* GetAtmosphereFromMover(WsfP6DOF_Mover* aMover)
{
   P6DofAtmosphere* atm = nullptr;

   if (aMover != nullptr)
   {
      P6DofVehicle* vehicle = aMover->GetP6DofVehicle();
      if (vehicle != nullptr)
      {
         P6DofScenario* scenario = vehicle->GetScenario();
         if (scenario != nullptr)
         {
            atm = scenario->GetAtmosphere();
         }
      }
   }

   return atm;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKtasFromKcas, 2, "double", "double, double")
{
   double vel_ktas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_kcas  = aVarArgs[1].GetDouble();
      vel_ktas           = atm->CalcKtasFromKcas(altitude_ft, speed_kcas);
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKtasFromMach, 2, "double", "double, double")
{
   double vel_ktas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double mach        = aVarArgs[1].GetDouble();
      vel_ktas           = atm->CalcKtasFromMach(altitude_ft, mach);
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKtasFromFps, 1, "double", "double")
{
   double vel_ktas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double speed_fps = aVarArgs[0].GetDouble();
      vel_ktas         = atm->CalcKtasFromFps(speed_fps);
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKcasFromKtas, 2, "double", "double, double")
{
   double vel_kcas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_ktas  = aVarArgs[1].GetDouble();
      vel_kcas           = atm->CalcKcasFromKtas(altitude_ft, speed_ktas);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKcasFromMach, 2, "double", "double, double")
{
   double vel_kcas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double mach        = aVarArgs[1].GetDouble();
      vel_kcas           = atm->CalcKcasFromMach(altitude_ft, mach);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcKcasFromFps, 2, "double", "double, double")
{
   double vel_kcas = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_fps   = aVarArgs[1].GetDouble();
      vel_kcas           = atm->CalcKcasFromFps(altitude_ft, speed_fps);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcMachFromKcas, 2, "double", "double, double")
{
   double mach = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_kcas  = aVarArgs[1].GetDouble();
      mach               = atm->CalcMachFromKcas(altitude_ft, speed_kcas);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcMachFromKtas, 2, "double", "double, double")
{
   double mach = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_ktas  = aVarArgs[1].GetDouble();
      mach               = atm->CalcMachFromKtas(altitude_ft, speed_ktas);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcMachFromFps, 2, "double", "double, double")
{
   double mach = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_fps   = aVarArgs[1].GetDouble();
      mach               = atm->CalcMachFromFps(altitude_ft, speed_fps);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcFpsFromKcas, 2, "double", "double, double")
{
   double vel_fps = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_kcas  = aVarArgs[1].GetDouble();
      vel_fps            = atm->CalcFpsFromKcas(altitude_ft, speed_kcas);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcFpsFromKtas, 1, "double", "double")
{
   double vel_fps = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double speed_ktas = aVarArgs[0].GetDouble();
      vel_fps           = atm->CalcFpsFromKtas(speed_ktas);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcFpsFromMach, 2, "double", "double, double")
{
   double vel_fps = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double mach        = aVarArgs[1].GetDouble();
      vel_fps            = atm->CalcFpsFromMach(altitude_ft, mach);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcDynamicPressure_lbft2, 2, "double", "double, double")
{
   double q = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft = aVarArgs[0].GetDouble();
      double speed_fps   = aVarArgs[1].GetDouble();
      q                  = atm->CalcDynamicPressure_lbft2(altitude_ft, speed_fps);
   }

   aReturnVal.SetDouble(q);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CalcFpsFromAltitudeDynamicPressure, 2, "double", "double, double")
{
   double vel_fps = 0;

   P6DofAtmosphere* atm = GetAtmosphereFromMover(aObjectPtr);
   if (atm != nullptr)
   {
      double altitude_ft    = aVarArgs[0].GetDouble();
      double pressure_lbft2 = aVarArgs[1].GetDouble();
      vel_fps               = atm->CalcFpsFromAltitudeDynamicPressure(altitude_ft, pressure_lbft2);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetDestroyed, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDestroyed();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineIsOperating, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineIsOperating());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineIsSmoking, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineIsSmoking());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetEngineAfterburnerIsOn, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineAfterburnerIsOn());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetContrailTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetContrailTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRocketSmokeTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetRocketSmokeTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetDamageSmokeTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetDamageSmokeTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetRisingSmokePlumeIsPresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetRisingSmokePlumeIsPresent());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetLaunchFlashSmokeIsPresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetLaunchFlashSmokeIsPresent());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetFlamesArePresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetFlamesArePresent());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetIsLightlyDamaged, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetIsLightlyDamaged());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetIsHeavilyDamaged, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetIsHeavilyDamaged());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetLightDamage, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetIsLightlyDamaged();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetHeavyDamage, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetIsHeavilyDamaged();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetCurrentManeuver, 0, "WsfManeuver", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentManeuver(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetManeuverSequence, 0, "WsfManeuverSequence", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetManeuverSequence(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ExecuteManeuver, 1, "void", "WsfManeuver")
{
   auto inputPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuver>();
   auto argPtr   = ut::clone(inputPtr);
   aObjectPtr->ExecuteManeuver(TIME_NOW, std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, ExecuteManeuverSequence, 1, "void", "WsfManeuverSequence")
{
   auto inputPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfManeuverSequence>();
   auto argPtr   = ut::clone(inputPtr);
   aObjectPtr->ExecuteManeuverSequence(TIME_NOW, std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, CancelManeuvers, 0, "void", "")
{
   aObjectPtr->CancelManeuvers();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTestSupportObject, 0, "WsfP6DOF_MoverTestObject", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetTestSupportObject(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTestingIgnoreAllCrashes, 1, "void", "bool")
{
   aObjectPtr->SetTestingIgnoreAllCrashes(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTestingIgnoreAllCrashes, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTestingIgnoreAllCrashes());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, SetTestingClampToSealLevelMinAlt, 1, "void", "bool")
{
   aObjectPtr->SetTestingClampToSealLevelMinAlt(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptP6DOF_MoverClass, WsfP6DOF_Mover, GetTestingClampToSealLevelMinAlt, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTestingClampToSealLevelMinAlt());
}
