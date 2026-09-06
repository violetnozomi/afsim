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

#include "WsfScriptSixDOF_Mover.hpp"

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_PilotObject.hpp"
#include "maneuvers/WsfSixDOF_Maneuver.hpp"
#include "maneuvers/WsfSixDOF_ManeuverSequence.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
Mover::Mover(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptMoverClass(aClassName, aTypesPtr)
{
   SetClassName("WsfSixDOF_Mover");

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
   AddMethod(ut::make_unique<ActivateSequencer>());
   AddMethod(ut::make_unique<GetActivePilot>());
   AddMethod(ut::make_unique<ActivateSimpleManualPilot>());
   AddMethod(ut::make_unique<ActivateAugmentedManualPilot>());
   AddMethod(ut::make_unique<ActivateSyntheticPilot>());
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
   AddMethod(ut::make_unique<GetForwardGLoadMin>());
   AddMethod(ut::make_unique<GetForwardGLoadMax>());
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
   AddMethod(ut::make_unique<SetForwardGLoadMin>());
   AddMethod(ut::make_unique<SetForwardGLoadMax>());
   AddMethod(ut::make_unique<SetTurnRollInMultiplier>());
   AddMethod(ut::make_unique<GetCurrentTurnRollInMultiplier>());
   AddMethod(ut::make_unique<GetDefaultTurnRollInMultiplier>());
   AddMethod(ut::make_unique<SetRouteAllowableAngleError>());
   AddMethod(ut::make_unique<GetCurrentRouteAllowableAngleError>());
   AddMethod(ut::make_unique<GetDefaultRouteAllowableAngleError>());
   AddMethod(ut::make_unique<EnableAutopilot>());
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
   AddMethod(ut::make_unique<SetAutopilotThrottle>());
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
   AddMethod(ut::make_unique<SetDirectThrottleInput>());
   AddMethod(ut::make_unique<ReleaseDirectThrottleInput>());
   AddMethod(ut::make_unique<EnableDirectSpeedBrakeInput>());
   AddMethod(ut::make_unique<OpenSpeedBrake>());
   AddMethod(ut::make_unique<CloseSpeedBrake>());
   AddMethod(ut::make_unique<ReleaseDirectSpeedBrakeInput>());
   AddMethod(ut::make_unique<GetSpeedBrakePosition>());
   AddMethod(ut::make_unique<EnableDirectStickRightInput>());
   AddMethod(ut::make_unique<EnableDirectStickBackInput>());
   AddMethod(ut::make_unique<EnableDirectRudderRightInput>());
   AddMethod(ut::make_unique<SetDirectStickRightInput>());
   AddMethod(ut::make_unique<SetDirectStickBackInput>());
   AddMethod(ut::make_unique<SetDirectRudderRightInput>());
   AddMethod(ut::make_unique<ReleaseDirectStickRightInput>());
   AddMethod(ut::make_unique<ReleaseDirectStickBackInput>());
   AddMethod(ut::make_unique<ReleaseDirectRudderRightInput>());
   AddMethod(ut::make_unique<SetFlapsPosition>());
   AddMethod(ut::make_unique<GetFlapsPosition>());
   AddMethod(ut::make_unique<SetSpoilersPosition>());
   AddMethod(ut::make_unique<GetSpoilersPosition>());
   AddMethod(ut::make_unique<GetStickRightPosition>());
   AddMethod(ut::make_unique<GetStickBackPosition>());
   AddMethod(ut::make_unique<GetRudderRightPosition>());
   AddMethod(ut::make_unique<GetThrottleMilitaryPosition>());
   AddMethod(ut::make_unique<GetThrottleAfterburnerPosition>());
   AddMethod(ut::make_unique<GetSpeedBrakeControlPosition>());
   AddMethod(ut::make_unique<GetFlapsLeverPosition>());
   AddMethod(ut::make_unique<GetSpoilersLeverPosition>());
   AddMethod(ut::make_unique<GetLift>());
   AddMethod(ut::make_unique<GetDrag>());
   AddMethod(ut::make_unique<GetSideForce>());
   AddMethod(ut::make_unique<GetThrustMagnitude>());
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
   AddMethod(ut::make_unique<CalcDynamicPressure>());
   AddMethod(ut::make_unique<CalcFpsFromAltitudeDynamicPressure>());
   AddMethod(ut::make_unique<GetCurrentManeuver>());
   AddMethod(ut::make_unique<GetManeuverSequence>());
   AddMethod(ut::make_unique<ExecuteManeuver>());
   AddMethod(ut::make_unique<ExecuteManeuverSequence>());
   AddMethod(ut::make_unique<CancelManeuvers>());
   AddMethod(ut::make_unique<Slice_1>("Slice"));
   AddMethod(ut::make_unique<Slice_2>("Slice"));
   AddMethod(ut::make_unique<Slice_1>("Sliceback"));
   AddMethod(ut::make_unique<Slice_2>("Sliceback"));
   AddMethod(ut::make_unique<Prlvl>("Prlvl"));
   AddMethod(ut::make_unique<Prlvlg>("Prlvl"));
   AddMethod(ut::make_unique<Prlvl>("LevelTurn"));
   AddMethod(ut::make_unique<Prlvlg>("LevelTurn"));

   // Undocumented test functions
   AddMethod(ut::make_unique<GetTestSupportObject>("__getTestObject"));
   AddMethod(ut::make_unique<SetTestingIgnoreAllCrashes>());
   AddMethod(ut::make_unique<GetTestingIgnoreAllCrashes>());
   AddMethod(ut::make_unique<SetTestingClampToSeaLevelMinAlt>());
   AddMethod(ut::make_unique<GetTestingClampToSeaLevelMinAlt>());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, AddFuel, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->AddFuel(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTotalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetInternalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetInternalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetExternalFuelCapacity, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetExternalFuelCapacity());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTotalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCurrentManeuver, 0, "WsfSixDOF_Maneuver", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentManeuver(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetManeuverSequence, 0, "WsfSixDOF_ManeuverSequence", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetManeuverSequence(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetInternalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetInternalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetExternalFuelRemaining, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetExternalFuelRemaining());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFuelTankCapacity, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFuelTankCapacity(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFuelInTank, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFuelInTank(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetFuelInTank, 2, "void", "string, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetFuelInTank(aVarArgs[0].GetString(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, StartupEngines, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->StartupEngines(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ShutdownEngines, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ShutdownEngines(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTotalWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCurrentWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEmptyWeight, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEmptyWeight_kg());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCgX, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCenterOfGravity_ft();
   aReturnVal.SetDouble(UtMath::cM_PER_FT * cgVec.X());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCgY, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCenterOfGravity_ft();
   aReturnVal.SetDouble(UtMath::cM_PER_FT * cgVec.Y());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCgZ, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX cgVec = aObjectPtr->GetCenterOfGravity_ft();
   aReturnVal.SetDouble(UtMath::cM_PER_FT * cgVec.Z());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetFlapsPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetFlapsPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFlapsPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlapsPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetSpoilersPosition, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetSpoilersPosition(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetSpoilersPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpoilersPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetStickRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStickRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetStickBackPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetStickBackPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRudderRightPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRudderRightPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetThrottleMilitaryPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrottleMilitaryPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetThrottleAfterburnerPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrottleAfterburnerPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetSpeedBrakeControlPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeedBrakeControlPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFlapsLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlapsLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetSpoilersLeverPosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpoilersLeverPosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, IsProducingThrust, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->IsProducingThrust());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTotalThrust, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalThrust());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineThrust, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEngineThrust(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAfterburnerOn, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetAfterburnerOn());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineFuelFlowRate, 1, "double", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEngineFuelFlowRate(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTotalFuelFlowRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalFuelFlowRate());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, AddFuelTransfer, 3, "bool", "string, string, string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(
      aObjectPtr->AddFuelTransfer(aVarArgs[0].GetString(), aVarArgs[1].GetString(), aVarArgs[2].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, RemoveFuelTransfer, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->RemoveFuelTransfer(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetJokerFuelState, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetJokerFuelState(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetJokerFuelState, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetJokerFuelState());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetJokerFuelReached, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->JokerFuelReached());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetBingoFuelState, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBingoFuelState(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBingoFuelState, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBingoFuelState());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBingoFuelReached, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->BingoFuelReached());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineAfterburnerOn, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineAfterburnerOn(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetFuelFeed, 2, "bool", "string, string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->SetFuelFeed(aVarArgs[0].GetString(), aVarArgs[1].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetFuelFeedAllEngines, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->SetFuelFeed(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetActivePilot, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetActivePilot());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ActivateSimpleManualPilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateSimpleManualPilot();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ActivateAugmentedManualPilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateAugmentedManualPilot();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ActivateSyntheticPilot, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ActivateSyntheticPilot();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, RevertLimitsToDefaults, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->RevertLimitsToDefaults();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitchGLoadMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchGLoadMin());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitchGLoadMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchGLoadMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAlphaMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaMin());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAlphaMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitchRateMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchRateMin());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitchRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitchRateMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetVerticalSpeedMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cM_PER_FT / 60.0 * aObjectPtr->GetVerticalSpeedMin());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetVerticalSpeedMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cM_PER_FT / 60.0 * aObjectPtr->GetVerticalSpeedMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetYawGLoadMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawGLoadMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBetaMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBetaMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetYawRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetYawRateMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRollRateMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRollRateMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBankAngleMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBankAngleMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetForwardGLoadMin, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetForwardAccelMin());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetForwardGLoadMax, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetForwardAccelMax());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetPitchGLoadMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoadMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetPitchGLoadMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAlphaMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAlphaMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAlphaMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAlphaMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetPitchRateMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchRateMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetPitchRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetVerticalSpeedMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetVerticalSpeedMin(60.0 * UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetVerticalSpeedMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetVerticalSpeedMax(60.0 * UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetYawGLoadMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetYawGLoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetBetaMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBetaMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetYawRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetYawRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetRollRateMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetRollRateMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetBankAngleMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetBankAngleMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetForwardGLoadMin, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetForwardAccelMin(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetForwardGLoadMax, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetForwardAccelMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetTurnRollInMultiplier, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetTurnRollInMultiplier(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCurrentTurnRollInMultiplier, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentTurnRollInMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetDefaultTurnRollInMultiplier, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDefaultTurnRollInMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetRouteAllowableAngleError, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetRouteAllowableAngleError(UtMath::cRAD_PER_DEG * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCurrentRouteAllowableAngleError, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cDEG_PER_RAD * aObjectPtr->GetCurrentRouteAllowableAngleError());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetDefaultRouteAllowableAngleError, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cRAD_PER_DEG * aObjectPtr->GetDefaultRouteAllowableAngleError());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotPitchAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotPitchAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotFlightPathAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotFlightPathAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotVerticalSpeed, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotVerticalSpeed(60.0 * UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotAltitude, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotAltitude(UtMath::cFT_PER_M * aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotRollAngle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotRollAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotRollRate, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotRollRate(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetPitchGLoad, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetPitchGLoad(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotPitchRate, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotPitchRate(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotDeltaPitch, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotDeltaPitch(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotDeltaRoll, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotDeltaRoll(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotSpeedKTAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKTAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotSpeedKCAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKCAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotSpeedKIAS, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedKCAS(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotSpeedMach, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedMach(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotThrottle, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotThrottle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotLateralWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotLateralWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotVerticalWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotVerticalWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotSpeedWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotSpeedWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotWaypointMode, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotWaypointMode();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetAutopilotNoControl, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetAutopilotNoControl();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAltitude, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlt_m());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetHeading, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetHeading());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitch, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPitch());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRoll, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetRoll());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetVerticalSpeed, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetVerticalSpeed());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetYawRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cDEG_PER_RAD * aObjectPtr->GetYawRate());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetPitchRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cDEG_PER_RAD * aObjectPtr->GetPitchRate());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRollRate, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cDEG_PER_RAD * aObjectPtr->GetRollRate());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFlightPathAngle, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFlightPathAngle());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetGLoad, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetGLoad());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetNx, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNx_g());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetNy, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNy_g());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetNz, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetNz_g());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetKCAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KCAS());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetKIAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KIAS());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetKTAS, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_KTAS());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetMach, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeed_Mach());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetDynamicPressure, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(UtMath::cPASCAL_PER_PSF * aObjectPtr->GetDynamicPressure_psf());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableDirectThrottleInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectThrottleInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, MoveThrottleToIdle, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToIdle();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, MoveThrottleToFull, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToFull();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, MoveThrottleToMilitary, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToFull();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, MoveThrottleToAfterburner, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->MoveThrottleToAfterburner();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDirectThrottleInput, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectThrottleInput(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectThrottleInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectThrottleInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableDirectSpeedBrakeInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectSpeedBrakeInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, OpenSpeedBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->OpenSpeedBrake();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CloseSpeedBrake, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->CloseSpeedBrake();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectSpeedBrakeInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectSpeedBrakeInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetSpeedBrakePosition, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSpeedBrakePosition());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableDirectStickBackInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectStickBackInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDirectStickBackInput, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectStickBackInput(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectStickBackInput, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectStickBackInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableDirectStickRightInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectStickRightInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDirectStickRightInput, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectStickRightInput(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectStickRightInput, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectStickRightInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableDirectRudderRightInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableDirectRudderRightInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDirectRudderRightInput, 1, "void", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectRudderRightInput(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectRudderRightInput, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectRudderRightInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetCLMaxAtMach, 1, "double", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCLMaxAtMach(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAlphaAtCLMaxAtMach, 1, "double", "double")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaAtCLMaxAtMach_deg(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ActivateSequencer, 1, "bool", "string")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->ActivateSequencer(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetLift, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLift_nt());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetDrag, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDrag_nt());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetSideForce, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSideForce_nt());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetThrustMagnitude, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetThrustMagnitude_nt());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAlpha, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlpha_deg());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBeta, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBeta_deg());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAlphaDot, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAlphaDot_dps());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetBetaDot, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetBetaDot_dps());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableThrustVectoring, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableThrustVectoring(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableControls, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableControls(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, MaxPotentialManeuverGLoad, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->MaxPotentialManeuverGLoad());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotLateralMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotLateralMode());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotLateralModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotLateralModeValue());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotVerticalMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotVerticalMode());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotVerticalModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotVerticalModeValue());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotSpeedMode, 0, "string", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetString(aObjectPtr->GetAutopilotSpeedMode());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetAutopilotSpeedModeValue, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetAutopilotSpeedModeValue());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, TakeDirectControlInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->TakeDirectControlInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ReleaseDirectControlInput, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->ReleaseDirectControlInput();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDirectControlInputs, 4, "void", "double, double, double, double")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDirectControlInputs(aVarArgs[0].GetDouble(),  // aStickX
                                      aVarArgs[1].GetDouble(),  // aStickY
                                      aVarArgs[2].GetDouble(),  // aRudder
                                      aVarArgs[3].GetDouble()); // aThrottleLeverPosition
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, EnableAutopilot, 1, "void", "bool")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->EnableAutopilot(aVarArgs[0].GetBool());
}

const wsf::six_dof::Environment* GetEnvironmentFromMover(wsf::six_dof::Mover* aMover)
{
   if (aMover != nullptr)
   {
      return aMover->GetEnvironment();
   }

   return nullptr;
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKtasFromKcas, 2, "double", "double, double")
{
   double vel_ktas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_kcas = aVarArgs[1].GetDouble();

      vel_ktas = env->CalcKtasFromKcas(altitude_m, speed_kcas);
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKtasFromMach, 2, "double", "double, double")
{
   double vel_ktas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double mach       = aVarArgs[1].GetDouble();

      vel_ktas = env->CalcKtasFromMach(altitude_m, mach);
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKtasFromFps, 1, "double", "double")
{
   double vel_ktas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double speed_fps = aVarArgs[0].GetDouble();
      vel_ktas         = UtMath::cNMPH_PER_FPS * speed_fps;
   }

   aReturnVal.SetDouble(vel_ktas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKcasFromKtas, 2, "double", "double, double")
{
   double vel_kcas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_ktas = aVarArgs[1].GetDouble();

      vel_kcas = env->CalcKcasFromKtas(altitude_m, speed_ktas);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKcasFromMach, 2, "double", "double, double")
{
   double vel_kcas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double mach       = aVarArgs[1].GetDouble();

      vel_kcas = env->CalcKcasFromMach(altitude_m, mach);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcKcasFromFps, 2, "double", "double, double")
{
   double vel_kcas = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_fps  = aVarArgs[1].GetDouble();

      vel_kcas = env->CalcKcasFromFps(altitude_m, speed_fps);
   }

   aReturnVal.SetDouble(vel_kcas);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcMachFromKcas, 2, "double", "double, double")
{
   double mach = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_kcas = aVarArgs[1].GetDouble();

      mach = env->CalcMachFromKcas(altitude_m, speed_kcas);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcMachFromKtas, 2, "double", "double, double")
{
   double mach = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_ktas = aVarArgs[1].GetDouble();

      mach = env->CalcMachFromKtas(altitude_m, speed_ktas);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcMachFromFps, 2, "double", "double, double")
{
   double mach = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_fps  = aVarArgs[1].GetDouble();

      mach = env->CalcMachFromFps(altitude_m, speed_fps);
   }

   aReturnVal.SetDouble(mach);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcFpsFromKcas, 2, "double", "double, double")
{
   double vel_fps = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_kcas = aVarArgs[1].GetDouble();

      vel_fps = env->CalcFpsFromKcas(altitude_m, speed_kcas);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcFpsFromKtas, 1, "double", "double")
{
   double vel_fps = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double speed_ktas = aVarArgs[0].GetDouble();
      vel_fps           = UtMath::cFPS_PER_NMPH * speed_ktas;
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcFpsFromMach, 2, "double", "double, double")
{
   double vel_fps = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double mach       = aVarArgs[1].GetDouble();

      vel_fps = env->CalcFpsFromMach(altitude_m, mach);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcDynamicPressure, 2, "double", "double, double")
{
   double q = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m = aVarArgs[0].GetDouble();
      double speed_mps  = aVarArgs[1].GetDouble();

      q = UtMath::cPASCAL_PER_PSF * env->CalcDynamicPressure_psf(altitude_m, UtMath::cFT_PER_M * speed_mps);
   }

   aReturnVal.SetDouble(q);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CalcFpsFromAltitudeDynamicPressure, 2, "double", "double, double")
{
   double vel_fps = 0;

   const wsf::six_dof::Environment* env = GetEnvironmentFromMover(aObjectPtr);
   if (env != nullptr)
   {
      double altitude_m  = aVarArgs[0].GetDouble();
      double pressure_Pa = aVarArgs[1].GetDouble();

      double pressure_psf = UtMath::cPSF_PER_PASCAL * pressure_Pa;

      vel_fps = env->CalcFpsFromDynamicPressure(altitude_m, pressure_psf);
   }

   aReturnVal.SetDouble(vel_fps);
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetDestroyed, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetDestroyed();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineIsOperating, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineIsOperating());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineIsSmoking, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineIsSmoking());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetEngineAfterburnerIsOn, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetEngineAfterburnerIsOn());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetContrailTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetContrailTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRocketSmokeTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetRocketSmokeTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetDamageSmokeTrailingEffect, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetDamageSmokeTrailingEffect());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetRisingSmokePlumeIsPresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetRisingSmokePlumeIsPresent());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetLaunchFlashSmokeIsPresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetLaunchFlashSmokeIsPresent());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetFlamesArePresent, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetFlamesArePresent());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetIsLightlyDamaged, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetIsLightlyDamaged());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetIsHeavilyDamaged, 0, "bool", "")
{
   aObjectPtr->Update(TIME_NOW);
   aReturnVal.SetBool(aObjectPtr->GetIsHeavilyDamaged());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetLightDamage, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetIsLightlyDamaged();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetHeavyDamage, 0, "void", "")
{
   aObjectPtr->Update(TIME_NOW);
   aObjectPtr->SetIsHeavilyDamaged();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTestSupportObject, 0, "WsfSixDOF_MoverTestObject", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetTestSupportObject(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetTestingIgnoreAllCrashes, 1, "void", "bool")
{
   aObjectPtr->SetTestingIgnoreAllCrashes(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTestingIgnoreAllCrashes, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTestingIgnoreAllCrashes());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, SetTestingClampToSeaLevelMinAlt, 1, "void", "bool")
{
   aObjectPtr->SetTestingClampToSeaLevelMinAlt(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, GetTestingClampToSeaLevelMinAlt, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTestingClampToSeaLevelMinAlt());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ExecuteManeuver, 1, "void", "WsfSixDOF_Maneuver")
{
   auto inputPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::Maneuver>();
   auto argPtr   = ut::clone(inputPtr);
   aObjectPtr->ExecuteManeuver(TIME_NOW, std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, ExecuteManeuverSequence, 1, "void", "WsfSixDOF_ManeuverSequence")
{
   auto inputPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::ManeuverSequence>();
   auto argPtr   = ut::clone(inputPtr);
   aObjectPtr->ExecuteManeuverSequence(TIME_NOW, std::move(argPtr));
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, CancelManeuvers, 0, "void", "")
{
   aObjectPtr->CancelManeuvers();
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, Slice_1, 2, "void", "double, double")
{
   aObjectPtr->CommandSlice(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, Slice_2, 3, "void", "double, double, double")
{
   aObjectPtr->CommandSlice(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, Prlvl, 2, "void", "double, double")
{
   aObjectPtr->CommandLevelTurn(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(Mover, wsf::six_dof::Mover, Prlvlg, 3, "void", "double, double, double")
{
   aObjectPtr->CommandLevelTurn(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
