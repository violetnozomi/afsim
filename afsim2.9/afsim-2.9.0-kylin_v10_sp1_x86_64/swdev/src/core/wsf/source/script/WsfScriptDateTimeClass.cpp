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

#include "script/WsfScriptDateTimeClass.hpp"

#include "UtMemory.hpp"
#include "UtScriptClass.hpp"
#include "WsfDateTime.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptDateTimeClass::WsfScriptDateTimeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfDateTime");
   AddStaticMethod(ut::make_unique<CurrentTime>());
   AddStaticMethod(ut::make_unique<StartTime>());
   AddStaticMethod(ut::make_unique<SimulationTime>());
}

// virtual
WsfScriptDateTimeClass::~WsfScriptDateTimeClass() {}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDateTimeClass, WsfScriptDateTimeClass, CurrentTime, 0, "Calendar", "")
{
   UtCalendar* calendarPtr = new UtCalendar(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW));
   aReturnVal.SetPointer(new UtScriptRef(calendarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDateTimeClass, WsfScriptDateTimeClass, StartTime, 0, "Calendar", "")
{
   UtCalendar* calendarPtr = new UtCalendar(SIMULATION->GetDateTime().GetStartDateAndTime());
   aReturnVal.SetPointer(new UtScriptRef(calendarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDateTimeClass, WsfScriptDateTimeClass, SimulationTime, 1, "Calendar", "double")
{
   double      simTime     = aVarArgs[0].GetDouble();
   UtCalendar* calendarPtr = new UtCalendar(SIMULATION->GetDateTime().GetCurrentTime(simTime));
   aReturnVal.SetPointer(new UtScriptRef(calendarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
