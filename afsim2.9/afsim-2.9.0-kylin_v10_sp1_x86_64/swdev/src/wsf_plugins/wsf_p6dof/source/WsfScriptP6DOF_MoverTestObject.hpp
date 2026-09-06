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

#ifndef WSFSCRIPTP6DOF_MOVERTESTOBJECT_HPP
#define WSFSCRIPTP6DOF_MOVERTESTOBJECT_HPP

#include "wsf_p6dof_export.h"

#include "script/WsfScriptObjectClass.hpp"

class WSF_P6DOF_EXPORT WsfScriptP6DOF_MoverTestObject : public UtScriptClass
{
public:
   WsfScriptP6DOF_MoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // Insert new methods here.
   UT_DECLARE_SCRIPT_METHOD(GetRangeSinceStart_km);                 // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterNoLagTesting);                 // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeLocation);               // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeAltitude);               // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeVerticalSpeed);          // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezePitch);                  // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeRoll);                   // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeYaw);                    // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterFreezeFuelBurn);               // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetMasterNoAlphaTesting);               // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(Autotune);                              // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(CanFlyAt);                              // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(CreateAutopilotSupportFile);            // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(CreateAutopilotSupportFileWithMaxMach); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(CreateAutopilotConfigFile);             // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(FlyStraightLevelFullThrottle);          // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(TurnRateAt);                            // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(TurnRateWithConditions);                // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightHorizontalThrust);        // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightDrag);                    // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetPitchingMoment);                     // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightFuelBurnRate);            // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SetVehicleMass);                        // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetMaxGLoad);                           // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(MaxInstantaneousGLoad);                 // NO_DOC | FOR_TEST_ONLY
};

#endif
