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

#include "script/WsfScriptSimulationClass.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtScriptAtmosphere.hpp"
#include "UtScriptColor.hpp"
#include "WsfApplication.hpp"
#include "WsfCommandChain.hpp"
#include "WsfDateTime.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWaypointMover.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneTypes.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
#include "script/WsfScriptEvent.hpp"
#include "script/WsfScriptManager.hpp"

WsfScriptSimulationClass::WsfScriptSimulationClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfSimulation");

   // Add each of the method objects to the class.

   AddStaticMethod(ut::make_unique<ClockRate>());
   AddStaticMethod(ut::make_unique<SetClockRate>());
   AddStaticMethod(ut::make_unique<IsRealtime>());
   AddStaticMethod(ut::make_unique<PlatformCount>());
   AddStaticMethod(ut::make_unique<PlatformEntry>());                    // PlatformEntry(index)
   AddStaticMethod(ut::make_unique<FindPlatform_1>("FindPlatform"));     // FindPlatform(string)
   AddStaticMethod(ut::make_unique<FindPlatform_2>("FindPlatform"));     // FindPlatform(index)
   AddStaticMethod(ut::make_unique<AddPlatform>());                      // AddPlatform(WsfPlatform)
   AddStaticMethod(ut::make_unique<CreatePlatform>());                   // CreatePlatform(string,string)
   AddStaticMethod(ut::make_unique<DeletePlatform_1>("DeletePlatform")); // DeletePlatform(string)
   AddStaticMethod(ut::make_unique<DeletePlatform_2>("DeletePlatform")); // DeletePlatform(index)
   AddStaticMethod(ut::make_unique<PlatformName>());                     // PlatformName(index)
   AddStaticMethod(ut::make_unique<PlatformType>());                     // PlatformType(index)
   AddStaticMethod(ut::make_unique<SetStartDate>());                     // SetStartDate(year, month, day)
   AddStaticMethod(ut::make_unique<SetStartTime>());                     // SetStartTime(hour, minute, second)
   AddStaticMethod(ut::make_unique<SetStartEpoch>());                    // SetStartEpoch(YYDDD.F)
   AddStaticMethod(ut::make_unique<RandomSeed>());                       // RandomSeed()
   AddStaticMethod(ut::make_unique<RunNumber>());                        // RunNumber()
   AddStaticMethod(ut::make_unique<GetAtmosphere>());                    // GetAtmosphere()
   AddStaticMethod(ut::make_unique<Terminate>());                        // Terminate()
   AddStaticMethod(ut::make_unique<ScriptExists>());                     // ScriptExists(string)
   AddStaticMethod(ut::make_unique<Execute_1>("Execute"));               // Execute(string)
   AddStaticMethod(ut::make_unique<Execute_2>("Execute"));               // Execute(string, Array<Object>)
   AddStaticMethod(ut::make_unique<ExecuteAtTime_1>("ExecuteAtTime"));   // ExecuteAtTime(double aSimTime, string)
   AddStaticMethod(ut::make_unique<ExecuteAtTime_2>("ExecuteAtTime")); // ExecuteAtTime(double aSimTime, string, Array<Object>)
   AddStaticMethod(ut::make_unique<ExpandPathVariables>());            // ExpandPathVariables(string)
   AddStaticMethod(ut::make_unique<EndTime>());                        // EndTime()
   AddStaticMethod(ut::make_unique<SetEndTime>());                     // SetEndTime(double)
   AddStaticMethod(ut::make_unique<TestFeature>());                    // TestFeature(string) NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<Zone>());        // NO_DOC | DEPRECATED - moved to WsfScriptZoneClass
   AddStaticMethod(ut::make_unique<CreateGroup>()); // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   AddStaticMethod(ut::make_unique<Group>());       // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   AddStaticMethod(ut::make_unique<Groups>());      // NO_DOC | DEPRECATED - moved to WsfScriptGroupClass
   AddStaticMethod(ut::make_unique<MainInputFiles>());
   AddStaticMethod(ut::make_unique<ClassificationString>());
   AddStaticMethod(ut::make_unique<ClassificationColor>());
   AddStaticMethod(ut::make_unique<Name>());
}

// =================================================================================================
// virtual
void* WsfScriptSimulationClass::Create(const UtScriptContext& /*aContext*/)
{
   return nullptr;
}

// =================================================================================================
// virtual
void WsfScriptSimulationClass::Destroy(void* aObjectPtr) {}

// =================================================================================================
// double clock_rate = ClockRate();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ClockRate, 0, "double", "")
{
   aReturnVal.SetDouble(SIMULATION->GetClockRate());
}

// =================================================================================================
// SetClockRate(double);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, SetClockRate, 1, "void", "double")
{
   double rate = std::max(aVarArgs[0].GetDouble(), 0.0001);
   SIMULATION->SetClockRate(rate);
}

// =================================================================================================
// bool realtime = IsRealTime();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, IsRealtime, 0, "bool", "")
{
   aReturnVal.SetBool(SIMULATION->IsRealTime());
}

// =================================================================================================
//! int count = PlatformCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, PlatformCount, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(SIMULATION->GetPlatformCount()));
}

// =================================================================================================
//! WsfPlatform platform = PlatformEntry(int aPlatformIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, PlatformEntry, 1, "WsfPlatform", "int")
{
   WsfPlatform*   platformPtr = nullptr;
   int            entry       = aVarArgs[0].GetInt();
   WsfSimulation* simPtr      = SIMULATION;
   if (entry >= 0 && entry < (int)simPtr->GetPlatformCount())
   {
      platformPtr = simPtr->GetPlatformEntry(entry);
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

// =================================================================================================
//! WsfPlatform platform = FindPlatform(string aPlatformName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, FindPlatform_1, 1, "WsfPlatform", "string")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

// =================================================================================================
//! WsfPlatform platform = FindPlatform(int aPlatformIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, FindPlatform_2, 1, "WsfPlatform", "int")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aVarArgs[0].GetInt());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

// =================================================================================================
//! bool AddPlatform(WsfPlatform aPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, AddPlatform, 2, "WsfPlatform", "WsfPlatform, string")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      // NOTE: The input platform is cloned rather than added directly because the input reference *SHOULD*
      //       be pointing to a 'managed' platform that will be deleted when the object reference goes away.
      //       (See CreatePlatform below).
      platformPtr = platformPtr->Clone();
      platformPtr->SetName(aVarArgs[1].GetString());
      double simTime = WsfScriptContext::GetTIME_NOW(aContext);
      if (!SIMULATION->AddPlatform(simTime, platformPtr))
      {
         delete platformPtr;
         platformPtr = nullptr;
      }
   }
   // The return value is a reference to the added platform. This will be a null reference if a null reference
   // was provided, the input reference referred to an active platform (can't clone a running platform),
   // or if the platform could not be added to the simulation.
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

// =================================================================================================
//! CreatePlatform(string aPlatformName, string aPlatformType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, CreatePlatform, 1, "WsfPlatform", "string")
{
   WsfPlatform* platformPtr =
      (WsfPlatform*)WsfScriptContext::GetSCENARIO(aContext)->CloneType("platform_type", aVarArgs[0].GetString());
   if (platformPtr != nullptr)
   {
      // Allows manipulation of the platform before it is really added to the simulation
      platformPtr->AssignToSimulation(SIMULATION);
   }
   // The memory management option on the script reference is defined so that the platform created by
   // this method will be deleted when the last reference is deleted. This ensures the platform object
   // will be deleted if AddPlatform is called or if AddPlatform fails to add it to the simulation.
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! DeletePlatform(string aPlatformName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, DeletePlatform_1, 1, "void", "string")
{
   WsfSimulation* simPtr = SIMULATION;
   if (simPtr != nullptr)
   {
      WsfPlatform* platformPtr = simPtr->GetPlatformByName(aVarArgs[0].GetString());
      if (platformPtr != nullptr)
      {
         double simTime = WsfScriptContext::GetTIME_NOW(aContext);
         simPtr->DeletePlatform(simTime, platformPtr);
      }
   }
}

// =================================================================================================
//! DeletePlatform(int aPlatformIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, DeletePlatform_2, 1, "void", "int")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aVarArgs[0].GetInt());
   if (platformPtr != nullptr)
   {
      double simTime = WsfScriptContext::GetTIME_NOW(aContext);
      SIMULATION->DeletePlatform(simTime, platformPtr);
   }
}

// =================================================================================================
//! string platformName = PlatformName(int aPlatformIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, PlatformName, 1, "string", "int")
{
   aReturnVal.SetString(SIMULATION->GetPlatformNameId(aVarArgs[0].GetInt()));
}

// =================================================================================================
//! string platformType = PlatformType(int aPlatformIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, PlatformType, 1, "string", "int")
{
   aReturnVal.SetString(SIMULATION->GetPlatformTypeId(aVarArgs[0].GetInt()));
}

// =================================================================================================
// Current random seed
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, RandomSeed, 0, "int", "")
{
   aReturnVal.SetInt(SIMULATION->GetRandom().GetSeed());
}

// =================================================================================================
// Current run number
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, RunNumber, 0, "int", "")
{
   aReturnVal.SetInt(SIMULATION->GetRunNumber());
}

// =================================================================================================
//! SetStartDate(int aYear, int aMonth, int aDay)
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, SetStartDate, 3, "void", "int, int, int")
{
   int year  = aVarArgs[0].GetInt();
   int month = aVarArgs[1].GetInt();
   int day   = aVarArgs[2].GetInt();
   SIMULATION->GetDateTime().SetStartDate(year, month, day);
}

// =================================================================================================
//! SetStartTime(int aHour, int aMinute, int aSecond)
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, SetStartTime, 3, "void", "int, int, int")
{
   int hour   = aVarArgs[0].GetInt();
   int minute = aVarArgs[1].GetInt();
   int second = aVarArgs[2].GetInt();
   SIMULATION->GetDateTime().SetStartTime(hour, minute, second);
}

// =================================================================================================
//! SetStartEpoch(double aYYYYDDD.F)
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, SetStartEpoch, 1, "void", "double")
{
   double epoch = aVarArgs[0].GetDouble();
   SIMULATION->GetDateTime().SetStartEpoch(epoch);
}

// =================================================================================================
// Atmosphere atm = GetAtmosphere();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, GetAtmosphere, 0, "Atmosphere", "")
{
   aReturnVal.SetPointer(UtScriptAtmosphere::Create(SCENARIO->GetAtmosphere()));
}

// =================================================================================================
// End the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Terminate, 0, "void", "")
{
   SIMULATION->RequestTermination();
}

// =================================================================================================
// returns true if the given script method exists in the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ScriptExists, 1, "bool", "string")
{
   WsfStringId scriptId = aVarArgs[0].GetString();
   aReturnVal.SetBool(aContext.GetRoot()->FindScript(scriptId.GetString()) != nullptr);
}

// =================================================================================================
// executes the given script method containing no arguments
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Execute_1, 1, "Object", "string")
{
   WsfScriptContext& globalContext = SIMULATION->GetScriptContext();
   double            simTime       = WsfScriptContext::GetTIME_NOW(aContext);
   UtScriptData      rv(0);

   if (!globalContext.ExecuteScript(simTime, aVarArgs[0].GetString(), rv, UtScriptDataList()))
   {
      auto out = ut::log::error() << "Could not execute script.";
      out.AddNote() << "Script: " << aVarArgs[0].GetString();
   }
   aReturnVal = rv;
}

// =================================================================================================
// executes the given script method containing arguments
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Execute_2, 2, "Object", "string,Array<Object>")
{
   WsfScriptContext& globalContext = SIMULATION->GetScriptContext();
   double            simTime       = WsfScriptContext::GetTIME_NOW(aContext);
   UtScriptData      rv(0);
   UtScriptDataList* argsPtr = (UtScriptDataList*)aVarArgs[1].GetPointer()->GetAppObject();
   if (!globalContext.ExecuteScript(simTime, aVarArgs[0].GetString(), rv, *argsPtr))
   {
      auto out = ut::log::error() << "Could not execute script.";
      out.AddNote() << "Script: " << aVarArgs[0].GetString();
   }
   aReturnVal = rv;
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ExecuteAtTime_1, 2, "bool", "double, string")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   auto simPtr = WsfScriptContext::GetSIMULATION(aContext);
   if (simPtr->GetScriptContext().FindScript(aVarArgs[1].GetString()) != nullptr)
   {
      double time = aVarArgs[0].GetDouble();
      simPtr->AddEventT<WsfScriptEvent>(time, simPtr, aVarArgs[1].GetString(), UtScriptDataList{});
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ExecuteAtTime_2, 3, "bool", "double, string, Array<Object>")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   // Argument 3: script args
   auto simPtr = WsfScriptContext::GetSIMULATION(aContext);
   if (simPtr->GetScriptContext().FindScript(aVarArgs[1].GetString()) != nullptr)
   {
      double time    = aVarArgs[0].GetDouble();
      auto   argsPtr = aVarArgs[2].GetPointer()->GetAppObject<UtScriptDataList>();
      simPtr->AddEventT<WsfScriptEvent>(time, simPtr, aVarArgs[1].GetString(), *argsPtr);
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
// Expand a string with path variables
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ExpandPathVariables, 1, "string", "string")
{
   aReturnVal.SetString(SCENARIO->GetInput().SubstitutePathVariables(aVarArgs[0].GetString()));
}

// =================================================================================================
// Returns the scheduled end-time of the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, EndTime, 0, "double", "")
{
   aReturnVal.SetDouble(SIMULATION->GetEndTime());
}

// =================================================================================================
// Sets the scheduled end-time of the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, SetEndTime, 1, "void", "double")
{
   double newEndTime = std::max(aVarArgs[0].GetDouble(), SIMULATION->GetSimTime());
   SIMULATION->SetEndTime(newEndTime);
}

// =================================================================================================
// Returns true if the specified feature is present
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, TestFeature, 1, "bool", "string") // NO_DOC | FOR_TEST_ONLY
{
   aReturnVal.SetBool(false); // Assume feature not present
   const std::vector<WsfApplication::Feature>& features = SCENARIO->GetApplication().GetRegisteredFeatures();
   std::string                                 tstFeature(aVarArgs[0].GetString());
   for (const auto& feature : features)
   {
      if (tstFeature == feature.mName)
      {
         aReturnVal.SetBool(true);
         break;
      }
   }
}

// =================================================================================================
// returns a zone definition object with the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Zone, 1, "WsfZone", "string") // NO_DOC | DEPRECATED
{
   WsfZoneDefinition* zonePtr = (WsfZoneDefinition*)WsfZoneTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
// creates a group of the given type with the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, CreateGroup, 2, "WsfGroup", "string,string") // NO_DOC | DEPRECATED
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().CreateGroup(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr));
}

// =================================================================================================
// returns a pointer to the group matching the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Group, 1, "WsfGroup", "string") // NO_DOC | DEPRECATED
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr));
}

// =================================================================================================
// returns a list of names of all groups in the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Groups, 0, "Array<string>", "") // NO_DOC | DEPRECATED
{
   std::vector<UtScriptData>*       resultVecPtr = new std::vector<UtScriptData>();
   const WsfGroupManager::GroupMap& groupsList   = SIMULATION->GetGroupManager().GetGroups();
   for (const auto& i : groupsList)
   {
      resultVecPtr->emplace_back(i.first);
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, MainInputFiles, 0, "Array<string>", "")
{
   UtScriptDataList*               resultVecPtr = new UtScriptDataList;
   const std::vector<std::string>& files        = SCENARIO->GetInputFiles();
   for (const auto& file : files)
   {
      resultVecPtr->push_back(UtScriptData(file));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ClassificationString, 0, "string", "")
{
   aReturnVal.SetString(SCENARIO->GetClassificationString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, ClassificationColor, 0, "Color", "")
{
   aReturnVal.SetPointer(UtScriptColor::Create(SCENARIO->GetClassificationColor()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptSimulationClass, WsfSimulation, Name, 0, "string", "")
{
   aReturnVal.SetString(SCENARIO->GetSimulationName());
}
