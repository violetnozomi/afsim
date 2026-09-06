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

#include "script/WsfScriptFuelClass.hpp"

#include "UtMemory.hpp"
#include "WsfFuel.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptFuelClass::WsfScriptFuelClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptPlatformPartClass(aClassName, aTypesPtr)
{
   SetClassName("WsfFuel");

   AddMethod(ut::make_unique<CurrentMode>());
   AddMethod(ut::make_unique<SelectMode>());

   AddMethod(ut::make_unique<Refuel_1>("Refuel"));
   AddMethod(ut::make_unique<Refuel_2>("Refuel"));

   AddMethod(ut::make_unique<QuantityRemaining>());
   AddMethod(ut::make_unique<QuantityRequired_1>("QuantityRequired"));
   AddMethod(ut::make_unique<QuantityRequired_2>("QuantityRequired"));
   AddMethod(ut::make_unique<QuantityRequired_3>("QuantityRequired"));
   AddMethod(ut::make_unique<SetQuantityRemaining>());

   AddMethod(ut::make_unique<ConsumptionRate_1>("ConsumptionRate"));
   AddMethod(ut::make_unique<ConsumptionRate_2>("ConsumptionRate"));
   AddMethod(ut::make_unique<SetConsumptionRate>());

   AddMethod(ut::make_unique<MaximumQuantity>());
   AddMethod(ut::make_unique<SetMaximumQuantity>());
   AddMethod(ut::make_unique<InitialQuantity>());
   AddMethod(ut::make_unique<SetInitialQuantity>());
   AddMethod(ut::make_unique<BingoQuantity>());
   AddMethod(ut::make_unique<SetBingoQuantity>());
   AddMethod(ut::make_unique<ReserveQuantity>());
   AddMethod(ut::make_unique<SetReserveQuantity>());

   AddMethod(ut::make_unique<TimeToBingo_1>("TimeToBingo"));
   AddMethod(ut::make_unique<TimeToBingo_2>("TimeToBingo"));
   AddMethod(ut::make_unique<TimeToReserve_1>("TimeToReserve"));
   AddMethod(ut::make_unique<TimeToReserve_2>("TimeToReserve"));
   AddMethod(ut::make_unique<TimeToEmpty_1>("TimeToEmpty"));
   AddMethod(ut::make_unique<TimeToEmpty_2>("TimeToEmpty"));

   AddMethod(ut::make_unique<DistanceToBingo_1>("DistanceToBingo"));
   AddMethod(ut::make_unique<DistanceToBingo_2>("DistanceToBingo"));
   AddMethod(ut::make_unique<DistanceToReserve_1>("DistanceToReserve"));
   AddMethod(ut::make_unique<DistanceToReserve_2>("DistanceToReserve"));
   AddMethod(ut::make_unique<DistanceToEmpty_1>("DistanceToEmpty"));
   AddMethod(ut::make_unique<DistanceToEmpty_2>("DistanceToEmpty"));
}

//! string currentFuelMode = fuel.CurrentMode();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, CurrentMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetModeId());
}

//!  fuel.SelectMode(string);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SelectMode, 1, "void", "string")
{
   aObjectPtr->SetMode(aVarArgs[0].GetString());
}

//! bool refueled = Refuel();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, Refuel_1, 0, "bool", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aReturnVal.SetBool(aObjectPtr->Refuel(simTime));
}

//! bool refueled = Refuel(double aAmount);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, Refuel_2, 1, "bool", "double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double amount  = aVarArgs[0].GetDouble();
   aReturnVal.SetBool(aObjectPtr->Refuel(simTime, amount));
}

//! double quantityRemaining = QuantityRemaining();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, QuantityRemaining, 0, "double", "")
{
   double quantityRemaining = aObjectPtr->GetQuantityRemaining();
   aReturnVal.SetDouble(quantityRemaining);
}

//! returns the amount of fuel required to complete the given route using the altitude and speed designated at each
//! waypoint. double quantity = QuantityRequired(WsfRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, QuantityRequired_1, 1, "double", "WsfRoute")
{
   WsfRoute* routePtr = (WsfRoute*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->GetQuantityRequired(*routePtr));
}

//! returns the amount of fuel required to complete the given distance at the platform's current altitude and speed.
//! double quantity = QuantityRequired(double distance);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, QuantityRequired_2, 1, "double", "double")
{
   double distance = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetQuantityRequired(distance));
}

//! returns the amount of fuel required to complete the given distance at the given altitude and speed.
//! double quantity = QuantityRequired(double distance, double alt, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, QuantityRequired_3, 3, "double", "double, double, double")
{
   double distance = aVarArgs[0].GetDouble();
   double altitude = aVarArgs[1].GetDouble();
   double speed    = aVarArgs[2].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetQuantityRequired(distance, altitude, speed));
}

//! SetQuantityRemaining(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetQuantityRemaining, 1, "void", "double")
{
   double remainingQuantity = aVarArgs[0].GetDouble();
   aObjectPtr->SetQuantityRemaining(remainingQuantity);
}

//! double consumptionRate = ConsumptionRate();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, ConsumptionRate_1, 0, "double", "")
{
   double consumptionRate = aObjectPtr->GetConsumptionRate();
   aReturnVal.SetDouble(consumptionRate);
}

//! returns the consumption rate at the given altitude and speed
//! double consumptionRate = ConsumptionRate(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, ConsumptionRate_2, 2, "double", "double, double")
{
   double altitude        = aVarArgs[0].GetDouble();
   double speed           = aVarArgs[1].GetDouble();
   double consumptionRate = aObjectPtr->GetConsumptionRate(altitude, speed);
   aReturnVal.SetDouble(consumptionRate);
}

//! SetConsumptionRate(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetConsumptionRate, 1, "void", "double")
{
   double consumptionRate = aVarArgs[0].GetDouble();
   aObjectPtr->SetConsumptionRate(consumptionRate);
}

//! double maximumQuantity = GetMaximumQuantity();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, MaximumQuantity, 0, "double", "")
{
   double maximumQuantity = aObjectPtr->GetMaximumQuantity();
   aReturnVal.SetDouble(maximumQuantity);
}

//! SetMaxmimumQuantity(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetMaximumQuantity, 1, "void", "double")
{
   double maximumQuantity = aVarArgs[0].GetDouble();
   aObjectPtr->SetMaximumQuantity(maximumQuantity);
}

//! double initialQuantity = GetInitialQuantity();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, InitialQuantity, 0, "double", "")
{
   double initialQuantity = aObjectPtr->GetInitialQuantity();
   aReturnVal.SetDouble(initialQuantity);
}

//! SetInitialQuantity(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetInitialQuantity, 1, "void", "double")
{
   double initialQuantity = aVarArgs[0].GetDouble();
   aObjectPtr->SetInitialQuantity(initialQuantity);
}

//! double bingoQuantity = BingoQuantity();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, BingoQuantity, 0, "double", "")
{
   double bingoQuantity = aObjectPtr->GetBingoQuantity();
   aReturnVal.SetDouble(bingoQuantity);
}

//! SetBingoQuantity(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetBingoQuantity, 1, "void", "double")
{
   double bingoQuantity = aVarArgs[0].GetDouble();
   aObjectPtr->SetBingoQuantity(bingoQuantity);
}

//! double reserveQuantity = ReserveQuantity();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, ReserveQuantity, 0, "double", "")
{
   double reserveQuantity = aObjectPtr->GetReserveQuantity();
   aReturnVal.SetDouble(reserveQuantity);
}

//! SetReserveQuantity(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, SetReserveQuantity, 1, "void", "double")
{
   double reserveQuantity = aVarArgs[0].GetDouble();
   aObjectPtr->SetReserveQuantity(reserveQuantity);
}

//! returns the amount of time after the fuel reaches the bingo amount at the current altitude and speed
//! double timeToBingo = TimeAtBingo();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeAtBingo_1, 0, "double", "")
{
   double timeToBingo = aObjectPtr->TimeAt(WsfFuel::ON_BINGO);
   aReturnVal.SetDouble(timeToBingo);
}

//! returns the amount of time after the fuel reaches the bingo amount at the given altitude and speed
//! double timeToBingo = TimeAtBingo(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeAtBingo_2, 2, "double", "double, double")
{
   double altitude    = aVarArgs[0].GetDouble();
   double speed       = aVarArgs[1].GetDouble();
   double timeToBingo = aObjectPtr->TimeAt(WsfFuel::ON_BINGO, altitude, speed);
   aReturnVal.SetDouble(timeToBingo);
}

//! returns the amount of time after the fuel reaches the reserve amount at the current altitude and speed
//! double timeToReserve = TimeAtReserve();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeAtReserve_1, 0, "double", "")
{
   double timeToReserve = aObjectPtr->TimeAt(WsfFuel::ON_RESERVE);
   aReturnVal.SetDouble(timeToReserve);
}

//! returns the amount of time after the fuel reaches the reserve amount at the given altitude and speed
//! double timeToReserve = TimeAtReserve(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeAtReserve_2, 2, "double", "double, double")
{
   double altitude      = aVarArgs[0].GetDouble();
   double speed         = aVarArgs[1].GetDouble();
   double timeToReserve = aObjectPtr->TimeAt(WsfFuel::ON_RESERVE, altitude, speed);
   aReturnVal.SetDouble(timeToReserve);
}

//! returns the amount of time left until the fuel reaches the bingo amount at the current altitude and speed
//! double timeToBingo = TimeToBingo();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToBingo_1, 0, "double", "")
{
   double timeToBingo = aObjectPtr->TimeTo(WsfFuel::ON_BINGO);
   aReturnVal.SetDouble(timeToBingo);
}

//! returns the amount of time left until the fuel reaches the bingo amount at the given altitude and speed
//! double timeToBingo = TimeToBingo(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToBingo_2, 2, "double", "double, double")
{
   double altitude    = aVarArgs[0].GetDouble();
   double speed       = aVarArgs[1].GetDouble();
   double timeToBingo = aObjectPtr->TimeTo(WsfFuel::ON_BINGO, altitude, speed);
   aReturnVal.SetDouble(timeToBingo);
}

//! returns the amount of time left until the fuel reaches the reserve amount at the current altitude and speed
//! double timeToReserve = TimeToReserve();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToReserve_1, 0, "double", "")
{
   double timeToReserve = aObjectPtr->TimeTo(WsfFuel::ON_RESERVE);
   aReturnVal.SetDouble(timeToReserve);
}

//! returns the amount of time left until the fuel reaches the reserve amount at the given altitude and speed
//! double timeToReserve = TimeToReserve(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToReserve_2, 2, "double", "double, double")
{
   double altitude      = aVarArgs[0].GetDouble();
   double speed         = aVarArgs[1].GetDouble();
   double timeToReserve = aObjectPtr->TimeTo(WsfFuel::ON_RESERVE, altitude, speed);
   aReturnVal.SetDouble(timeToReserve);
}

//! returns the amount of time left until the fuel is empty at the current altitude and speed
//! double timeToEmpty = TimeToEmpty();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToEmpty_1, 0, "double", "")
{
   double timeToEmpty = aObjectPtr->TimeTo(WsfFuel::ON_EMPTY);
   aReturnVal.SetDouble(timeToEmpty);
}

//! returns the amount of time left until the fuel is empty at the given altitude and speed
//! double timeToEmpty = TimeToEmpty(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, TimeToEmpty_2, 2, "double", "double, double")
{
   double altitude    = aVarArgs[0].GetDouble();
   double speed       = aVarArgs[1].GetDouble();
   double timeToEmpty = aObjectPtr->TimeTo(WsfFuel::ON_EMPTY, altitude, speed);
   aReturnVal.SetDouble(timeToEmpty);
}

//! returns the distance after the fuel reaches the reserve amount at the given altitude and speed
//! double distance_m = DistanceAtReserve();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceAtReserve_1, 0, "double", "")
{
   double distance_m = aObjectPtr->DistanceAt(WsfFuel::ON_RESERVE);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance after the the fuel reaches the reserve amount at the given altitude and speed
//! double distance_m = DistanceAtReserve(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceAtReserve_2, 2, "double", "double, double")
{
   double altitude   = aVarArgs[0].GetDouble();
   double speed      = aVarArgs[1].GetDouble();
   double distance_m = aObjectPtr->DistanceAt(WsfFuel::ON_RESERVE, altitude, speed);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance after the fuel reaches the bingo amount at the given altitude and speed
//! double distance_m = DistanceAtBingo();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceAtBingo_1, 0, "double", "")
{
   double distance_m = aObjectPtr->DistanceAt(WsfFuel::ON_BINGO);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance after the the fuel reaches the bingo amount at the given altitude and speed
//! double distance_m = DistanceAtBingo(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceAtBingo_2, 2, "double", "double, double")
{
   double altitude   = aVarArgs[0].GetDouble();
   double speed      = aVarArgs[1].GetDouble();
   double distance_m = aObjectPtr->DistanceAt(WsfFuel::ON_BINGO, altitude, speed);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the fuel reaches the reserve amount at the given altitude and speed
//! double distance_m = DistanceToReserve();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToReserve_1, 0, "double", "")
{
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_RESERVE);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the the fuel reaches the reserve amount at the given altitude and speed
//! double distance_m = DistanceToReserve(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToReserve_2, 2, "double", "double, double")
{
   double altitude   = aVarArgs[0].GetDouble();
   double speed      = aVarArgs[1].GetDouble();
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_RESERVE, altitude, speed);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the fuel reaches the bingo amount at the given altitude and speed
//! double distance_m = DistanceToBingo();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToBingo_1, 0, "double", "")
{
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_BINGO);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the the fuel reaches the bingo amount at the given altitude and speed
//! double distance_m = DistanceToBingo(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToBingo_2, 2, "double", "double, double")
{
   double altitude   = aVarArgs[0].GetDouble();
   double speed      = aVarArgs[1].GetDouble();
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_BINGO, altitude, speed);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the fuel is empty at the current altitude and speed
//! double distance_m = DistanceToEmpty();
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToEmpty_1, 0, "double", "")
{
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_EMPTY);
   aReturnVal.SetDouble(distance_m);
}

//! returns the distance left until the fuel is empty at the given altitude and speed
//! double distance_m = DistanceToEmpty(double altitude, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptFuelClass, WsfFuel, DistanceToEmpty_2, 2, "double", "double, double")
{
   double altitude   = aVarArgs[0].GetDouble();
   double speed      = aVarArgs[1].GetDouble();
   double distance_m = aObjectPtr->DistanceTo(WsfFuel::ON_EMPTY, altitude, speed);
   aReturnVal.SetDouble(distance_m);
}
