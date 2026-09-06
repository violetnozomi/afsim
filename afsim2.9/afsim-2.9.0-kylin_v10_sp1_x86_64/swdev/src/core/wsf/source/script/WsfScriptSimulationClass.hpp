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

#ifndef WSFSCRIPTSIMULATIONCLASS_HPP
#define WSFSCRIPTSIMULATIONCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

//! Define script methods for WsfSimulation.
class WSF_EXPORT WsfScriptSimulationClass : public UtScriptClass
{
public:
   WsfScriptSimulationClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSimulationClass() override = default;

   // See the base class for a description of Create, Clone, and Destroy.

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   // Simulation management methods

   UT_DECLARE_SCRIPT_METHOD(ClockRate);        // Get the simulation clock rate (for realtime)
   UT_DECLARE_SCRIPT_METHOD(SetClockRate);     // Set the simulation clock rate (for realtime)
   UT_DECLARE_SCRIPT_METHOD(IsRealtime);       // See if simulation is running in realtime mode
   UT_DECLARE_SCRIPT_METHOD(PlatformCount);    // Get the number of platforms in the platform list
   UT_DECLARE_SCRIPT_METHOD(PlatformEntry);    // Get the active platform entry
   UT_DECLARE_SCRIPT_METHOD(FindPlatform_1);   // FindPlatform(string)
   UT_DECLARE_SCRIPT_METHOD(FindPlatform_2);   // FindPlatform(index)
   UT_DECLARE_SCRIPT_METHOD(AddPlatform);      // AddPlatform(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(CreatePlatform);   // CreatePlatform(string, string)
   UT_DECLARE_SCRIPT_METHOD(DeletePlatform_1); // DeletePlatform(string)
   UT_DECLARE_SCRIPT_METHOD(DeletePlatform_2); // DeletePlatform(index)
   UT_DECLARE_SCRIPT_METHOD(PlatformName);     // PlatformName(index)
   UT_DECLARE_SCRIPT_METHOD(PlatformType);     // PlatformType(index)
   UT_DECLARE_SCRIPT_METHOD(SetStartDate);     // SetStartDate(month, day, year)
   UT_DECLARE_SCRIPT_METHOD(SetStartTime);     // SetStartTime(seconds past midnight)
   UT_DECLARE_SCRIPT_METHOD(SetStartEpoch);    // SetStartEpoch(YYYYDDDD.F)
   UT_DECLARE_SCRIPT_METHOD(RandomSeed);       // Current random seed
   UT_DECLARE_SCRIPT_METHOD(RunNumber);        // Current run number
   UT_DECLARE_SCRIPT_METHOD(GetAtmosphere);    // Scenario-owned atmosphere
   UT_DECLARE_SCRIPT_METHOD(Terminate);        // End the simulation
   UT_DECLARE_SCRIPT_METHOD(ScriptExists);
   UT_DECLARE_SCRIPT_METHOD(Execute_1);
   UT_DECLARE_SCRIPT_METHOD(Execute_2);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_1);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_2);
   UT_DECLARE_SCRIPT_METHOD(ExpandPathVariables);
   UT_DECLARE_SCRIPT_METHOD(EndTime);
   UT_DECLARE_SCRIPT_METHOD(SetEndTime);
   UT_DECLARE_SCRIPT_METHOD(TestFeature); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(Zone);        // NO_DOC | DEPRECATED - moved to WsfScriptZoneClass
   UT_DECLARE_SCRIPT_METHOD(CreateGroup); // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   UT_DECLARE_SCRIPT_METHOD(Group);       // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   UT_DECLARE_SCRIPT_METHOD(Groups);      // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   UT_DECLARE_SCRIPT_METHOD(MainInputFiles);
   UT_DECLARE_SCRIPT_METHOD(ClassificationString);
   UT_DECLARE_SCRIPT_METHOD(ClassificationColor);
   UT_DECLARE_SCRIPT_METHOD(Name); // Gets the name set by the simulation_name command
};

#endif
