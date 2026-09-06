// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSOSM_Interface.hpp"

#include <iostream>

#include "SOSM_Manager.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlugin.hpp"
#include "WsfSOSM_Sensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfSystemLog.hpp"

// =================================================================================================
// Called by the application to register the SOSM application extension.
void WSF_SOSM_EXPORT Register_wsf_sosm(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_sosm"))
   {
      aApplication.RegisterFeature("sosm", "wsf_sosm"); // Indicate feature is present
      aApplication.RegisterExtension("wsf_sosm", ut::make_unique<WsfDefaultApplicationExtension<WsfSOSM_Interface>>());
   }
}

// =================================================================================================
WsfSOSM_Interface::WsfSOSM_Interface()
   : mManagerPtr(nullptr)
   , mSensorTypeMap()
   , mTargetTypeMap()
   , mCallbacks()
{
}

// =================================================================================================
WsfSOSM_Interface::~WsfSOSM_Interface()
{
   delete mManagerPtr;
}

// =================================================================================================
// Called by WsfApplication to indicate the scenario extension has been added to a scenario
void WsfSOSM_Interface::AddedToScenario()
{
   mManagerPtr = new SOSM_Manager(GetScenario().GetAtmosphere());
   // Subscribe to receive notification when SOSM references a file (so we can log it in our log file).
   mCallbacks.Add(mManagerPtr->FileReferenced.Connect(&WsfSOSM_Interface::FileReferenced, this));
   // Register the sensor type
   WsfSensorTypes::Get(GetScenario()).AddCoreType("WSF_SOSM_SENSOR", ut::make_unique<WsfSOSM_Sensor>(GetScenario()));
}

// =================================================================================================
// Called by the scenario to indicate that reading of all scenario input has been completed.
bool WsfSOSM_Interface::Complete()
{
   bool    ok = true;
   TypeMap resultMap;

   // Make sure the sensor type maps and target type maps only contain references to SOSM types that actually exist.
   // Entries where the referenced SOSM does not exist are removed from the map.

   resultMap.clear();
   for (TypeMap::const_iterator smi = mSensorTypeMap.begin(); smi != mSensorTypeMap.end(); ++smi)
   {
      const std::string& sosmType = smi->second;
      if (mManagerPtr->FindSensorType(sosmType) != nullptr)
      {
         resultMap.insert(*smi); // SOSM type exists - copy the entry to the result map.
      }
      else
      {
         auto logWarning = ut::log::warning() << "sosm_interface - 'map_sensor_type.";
         logWarning.AddNote() << "map_sensor_type: " << smi->first << ' ' << smi->second << "' will be ignored.";
         logWarning.AddNote() << smi->first << " is not a valid SOSM sensor type.";
      }
   }
   mSensorTypeMap = resultMap; // Update the map with only those entries that are valid.

   resultMap.clear();
   for (TypeMap::const_iterator tmi = mTargetTypeMap.begin(); tmi != mTargetTypeMap.end(); ++tmi)
   {
      const std::string& sosmType = tmi->second;
      if (mManagerPtr->FindTargetType(sosmType) != nullptr)
      {
         resultMap.insert(*tmi); // SOSM type exists - copy the entry to the result map.
      }
      else
      {
         auto logWarning = ut::log::warning() << "sosm_interface - map_target_type"
                                              << " will be ignored.";
         logWarning.AddNote() << "map_target_type: " << tmi->first << ' ' << tmi->second;
         logWarning.AddNote() << tmi->first << " is not a valid SOSM sensor type.";
      }
   }
   mTargetTypeMap = resultMap; // Update the map with only those entries that are valid.
   return ok;
}

// =================================================================================================
// Called the scenario to determine if the current command is one of ours.
bool WsfSOSM_Interface::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   if (command != "sosm_interface")
   {
      return false;
   }

   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "load")
      {
         std::string fileName;
         aInput.ReadValueQuoted(fileName);
         fileName = aInput.LocateFile(fileName);
         GetScenario().GetSystemLog().WriteLogEntry("file " + fileName);
         mManagerPtr->Load(fileName);
      }
      else if (command == "map_sensor_type")
      {
         std::string ourType;
         std::string sosmType;
         aInput.ReadValue(ourType);
         aInput.ReadValue(sosmType);
         mSensorTypeMap[ourType] = sosmType;
      }
      else if (command == "map_target_type")
      {
         std::string ourType;
         std::string sosmType;
         aInput.ReadValue(ourType);
         aInput.ReadValue(sosmType);
         mTargetTypeMap[ourType] = sosmType;
      }
      else if (!mManagerPtr->ProcessInput(aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return true;
}

// =================================================================================================
// This is call from SOSM whenever it references a file. We simply write an entry into our log file.
void WsfSOSM_Interface::FileReferenced(const std::string& aFileName)
{
   GetScenario().GetSystemLog().WriteLogEntry("file " + aFileName);
}

// =================================================================================================
//! Return the SOSM sensor type to be used to model the specified sensor (mode).
std::string WsfSOSM_Interface::GetSensorType(WsfSensorMode* aSensorModePtr) const
{
   // Search the type hierarchy (from newest to oldest) of the target and try to find a mapping.

   const WsfObject::TypeList& typeList = aSensorModePtr->GetSensor()->GetTypeList();
   for (WsfObject::TypeList::const_iterator tli = typeList.begin(); tli != typeList.end(); ++tli)
   {
      const std::string& ourSensorType = *tli;

      // First check if there is an explicit mapping of '<sensor_type>:<sensor_mode>'
      std::string             ourSensorTypeAndMode = ourSensorType + ':' + aSensorModePtr->GetName();
      TypeMap::const_iterator tmi                  = mSensorTypeMap.find(ourSensorTypeAndMode);
      if (tmi != mSensorTypeMap.end())
      {
         return tmi->second;
      }

      // Next check if there is an explicit mapping of '<sensor_type>'
      tmi = mSensorTypeMap.find(ourSensorType);
      if (tmi != mSensorTypeMap.end())
      {
         return tmi->second;
      }

      // Next check for an implicit mapping (i.e.: SOSM has a sensor type that matches our sensor type)
      if (mManagerPtr->FindSensorType(ourSensorTypeAndMode))
      {
         return ourSensorTypeAndMode;
      }
      else if (mManagerPtr->FindSensorType(ourSensorType))
      {
         return ourSensorType;
      }
   }

   // No mapping exists - issue a warning and just return the unmapped type.

   auto logWarning = ut::log::warning() << "sosm_interface - no mapping exists for sensor type.";
   logWarning.AddNote() << "Sensor Type: " << aSensorModePtr->GetSensor()->GetType();
   logWarning.AddNote() << "mode: " << aSensorModePtr->GetName();
   return aSensorModePtr->GetSensor()->GetType();
}

// =================================================================================================
//! Return the SOSM target type to be used to model the specified target.
std::string WsfSOSM_Interface::GetTargetType(WsfPlatform* aTargetPtr) const
{
   // Search the type hierarchy (from newest to oldest) of the target and try to find a mapping.

   const WsfObject::TypeList& typeList = aTargetPtr->GetTypeList();
   for (WsfObject::TypeList::const_iterator tli = typeList.begin(); tli != typeList.end(); ++tli)
   {
      const std::string& ourType = *tli;

      // Given our type-name (at the current level in the hierarchy), first see if there is an
      // explicit mapping to an SOSM target type.

      // First check for an explicit mapping of '<target-platform-type>'
      TypeMap::const_iterator tmi = mTargetTypeMap.find(ourType);
      if (tmi != mTargetTypeMap.end())
      {
         return tmi->second;
      }

      // Next check for an implicit mapping (i.e.: SOSM has a target type that matches our target platform type)
      if (mManagerPtr->FindTargetType(ourType) != nullptr)
      {
         return ourType;
      }
   }

   // No mapping exists - issue a warning and just return the unmapped type.

   auto logWarning = ut::log::warning() << "sosm_interface - no mapping exists for target platform type.";
   logWarning.AddNote() << "Type: " << aTargetPtr->GetType();
   return aTargetPtr->GetType();
}


extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were
   //! built with the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }
   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to
   //! succeed. As we only have an application reference at load time, one usually registers an application extension,
   //! within which the ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs access
   //! to the simulation, one should also instantiate and register a simulation extension by overriding the
   //! SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_sosm(aApplication); }
}
