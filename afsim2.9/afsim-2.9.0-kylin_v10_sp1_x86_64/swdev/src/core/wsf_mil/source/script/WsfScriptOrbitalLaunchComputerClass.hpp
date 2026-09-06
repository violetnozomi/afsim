// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTORBITALLAUNCHCOMPUTERCLASS_HPP
#define WSFSCRIPTORBITALLAUNCHCOMPUTERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptLaunchComputerClass.hpp"

//! Script interface for WsfOrbitalLaunchComputer
class WSF_MIL_EXPORT WsfScriptOrbitalLaunchComputerClass : public WsfScriptLaunchComputerClass
{
public:
   WsfScriptOrbitalLaunchComputerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(ComputeLEO_Insertion);
   UT_DECLARE_SCRIPT_METHOD(LaunchHeading);
   // LoftAngle is from the base class...
   UT_DECLARE_SCRIPT_METHOD(Inclination);
   UT_DECLARE_SCRIPT_METHOD(InsertionTimeOfFlight);
   UT_DECLARE_SCRIPT_METHOD(InsertionLocation);
   UT_DECLARE_SCRIPT_METHOD(InsertionSpeed);
   UT_DECLARE_SCRIPT_METHOD(InsertionSpeedFixed);
   UT_DECLARE_SCRIPT_METHOD(InsertionHeading);
   UT_DECLARE_SCRIPT_METHOD(InsertionPitch);
   UT_DECLARE_SCRIPT_METHOD(InsertionResidualDeltaV);
   UT_DECLARE_SCRIPT_METHOD(InsertionFuelRemaining);
   UT_DECLARE_SCRIPT_METHOD(InsertionTimeRemaining);
   UT_DECLARE_SCRIPT_METHOD(OrbitalElements);

   UT_DECLARE_SCRIPT_METHOD(ComputeOrbitalElements);
};

#endif
