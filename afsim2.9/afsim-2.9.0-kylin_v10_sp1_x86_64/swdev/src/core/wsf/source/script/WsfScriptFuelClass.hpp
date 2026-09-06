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

#ifndef WSFSCRIPTFUELCLASS_HPP
#define WSFSCRIPTFUELCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptPlatformPartClass.hpp"

//! Define script methods for WsfFuel.
class WSF_EXPORT WsfScriptFuelClass : public WsfScriptPlatformPartClass
{
public:
   WsfScriptFuelClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(CurrentMode);
   UT_DECLARE_SCRIPT_METHOD(SelectMode);

   UT_DECLARE_SCRIPT_METHOD(Refuel_1);
   UT_DECLARE_SCRIPT_METHOD(Refuel_2);

   UT_DECLARE_SCRIPT_METHOD(QuantityRemaining);
   UT_DECLARE_SCRIPT_METHOD(QuantityRequired_1);
   UT_DECLARE_SCRIPT_METHOD(QuantityRequired_2);
   UT_DECLARE_SCRIPT_METHOD(QuantityRequired_3);
   UT_DECLARE_SCRIPT_METHOD(SetQuantityRemaining);

   UT_DECLARE_SCRIPT_METHOD(ConsumptionRate_1);
   UT_DECLARE_SCRIPT_METHOD(ConsumptionRate_2);
   UT_DECLARE_SCRIPT_METHOD(SetConsumptionRate);

   UT_DECLARE_SCRIPT_METHOD(MaximumQuantity);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumQuantity);
   UT_DECLARE_SCRIPT_METHOD(InitialQuantity);
   UT_DECLARE_SCRIPT_METHOD(SetInitialQuantity);
   UT_DECLARE_SCRIPT_METHOD(BingoQuantity);
   UT_DECLARE_SCRIPT_METHOD(SetBingoQuantity);
   UT_DECLARE_SCRIPT_METHOD(ReserveQuantity);
   UT_DECLARE_SCRIPT_METHOD(SetReserveQuantity);

   UT_DECLARE_SCRIPT_METHOD(TimeAtBingo_1);
   UT_DECLARE_SCRIPT_METHOD(TimeAtBingo_2);
   UT_DECLARE_SCRIPT_METHOD(TimeAtReserve_1);
   UT_DECLARE_SCRIPT_METHOD(TimeAtReserve_2);

   UT_DECLARE_SCRIPT_METHOD(TimeToBingo_1);
   UT_DECLARE_SCRIPT_METHOD(TimeToBingo_2);
   UT_DECLARE_SCRIPT_METHOD(TimeToReserve_1);
   UT_DECLARE_SCRIPT_METHOD(TimeToReserve_2);
   UT_DECLARE_SCRIPT_METHOD(TimeToEmpty_1);
   UT_DECLARE_SCRIPT_METHOD(TimeToEmpty_2);

   UT_DECLARE_SCRIPT_METHOD(DistanceAtBingo_1);
   UT_DECLARE_SCRIPT_METHOD(DistanceAtBingo_2);
   UT_DECLARE_SCRIPT_METHOD(DistanceAtReserve_1);
   UT_DECLARE_SCRIPT_METHOD(DistanceAtReserve_2);

   UT_DECLARE_SCRIPT_METHOD(DistanceToBingo_1);
   UT_DECLARE_SCRIPT_METHOD(DistanceToBingo_2);
   UT_DECLARE_SCRIPT_METHOD(DistanceToReserve_1);
   UT_DECLARE_SCRIPT_METHOD(DistanceToReserve_2);
   UT_DECLARE_SCRIPT_METHOD(DistanceToEmpty_1);
   UT_DECLARE_SCRIPT_METHOD(DistanceToEmpty_2);
};

#endif
