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

#include "FiresTables.hpp"

#include "BallisticPath.hpp"
#include "BallisticPlatforms.hpp"
#include "FiresLaunchComputer.hpp"
#include "FiresMover.hpp"
#include "FiresTable.hpp"
#include "FiresTableLoader.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfLaunchComputerTypes.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlugin.hpp"
#include "WsfSimulation.hpp"

WSF_FIRES_EXPORT void Register_wsf_fires(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_fires"))
   {
      aApplication.RegisterFeature("fires", "wsf_fires");
      aApplication.GetScriptTypes()->Register(
         ut::make_unique<Fires::ScriptBallisticPathClass>("WsfBallisticPath", aApplication.GetScriptTypes()));
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_fires", ut::make_unique<WsfDefaultApplicationExtension<Fires::FiresTables>>());
      aApplication.ExtensionDepends("wsf_fires", "wsf_mil", true);
   }
}

// static
Fires::FiresTables* Fires::FiresTables::Find(const WsfScenario& aScenario)
{
   return static_cast<FiresTables*>(aScenario.FindExtension("wsf_fires"));
}

namespace Fires
{

FiresTables::FiresTables()
   : mUseFiresMoverExtrapolation(true)
   , mTestEnabled(false)
{
}

void FiresTables::AddedToScenario()
{
   // input types added here
   WsfScenario& scenario(GetScenario());
   WsfLaunchComputerTypes::Get(scenario).Add("WSF_FIRES_LAUNCH_COMPUTER",
                                             ut::make_unique<Fires::FiresLaunchComputer>(&scenario));
   WsfMoverTypes::Get(scenario).Add("WSF_FIRES_MOVER", ut::make_unique<Fires::FiresMover>(scenario));
}

FiresTables::~FiresTables()
{
   // Tables are inserted using the FiresTables() accessor.
   // This class is responsible for deleting them.
   std::map<WsfStringId, FiresTable*>::iterator iter = mTables.begin();
   while (iter != mTables.end())
   {
      delete iter->second;
      ++iter;
   }
}

bool FiresTables::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();

   if (command == "ballistic_types")
   {
      UtInputBlock block(aInput, "end_ballistic_types");
      while (block.ReadCommand(command))
      {
         if (command == "use_fires_extrapolation")
         {
            aInput.ReadValue(mUseFiresMoverExtrapolation);
            myCommand = true;
         }
         else if (command == "target_data")
         {
            UtInputBlock inputBlock(aInput);
            std::string  type;
            aInput.ReadValue(type);
            bool hasAero = false;
            bool hasMass = false;
            while (inputBlock.ReadCommand(command))
            {
               if (command == "aero")
               {
                  std::string aeroType;
                  aInput.ReadCommand(aeroType);
                  mAeroMap[type] = aeroType;
                  hasAero        = true;
               }
               else if (command == "mass")
               {
                  double mass;
                  aInput.ReadValueOfType(mass, UtInput::cMASS);
                  mMassMap[type] = mass;
                  hasMass        = true;
               }
               else
               {
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            if ((!hasAero) || (!hasMass))
            {
               throw UtInput::BadValue(aInput, "Incomplete target_data block (include both aero mapping and mass).");
            }
            AddType(type); // Checks to see if type is already inserted.
            myCommand = true;
         }
         else
         {
            AddType(command);
         }
      }
      myCommand = true;
   }
   else if (command == "test_ballistic_types")
   {
      aInput.ReadValue(mTestEnabled);
      myCommand = true;
   }
   else if (command == "fires_table")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         FiresTable* tablePtr = FiresTableLoader::ProcessTable(aInput);
         if (tablePtr != nullptr)
         {
            // sTables.Get()[tablePtr->GetName()] = tablePtr;
            mTables[tablePtr->GetName()] = tablePtr;
            command                      = aInput.GetCommand();
         }
         else
         {
            break;
         }
      }
   }
   return myCommand;
}

void FiresTables::AddType(const std::string& aTypeName)
{
   WsfStringId commandId = aTypeName;
   if (mTypes.find(commandId) == mTypes.end())
   {
      mTypes.insert(commandId);
   }
}

//! Return whether the given type matches one of the types provided in a "ballistic_types" input block.
bool FiresTables::IsType(const std::string& aType)
{
   return (mTypes.find(aType) != mTypes.end());
}

bool FiresTables::UsesMaxOrd(const std::string& aType)
{
   std::map<WsfStringId, FiresTable*>::iterator iter = mTables.find(aType);
   // From table, lookup max ord and time to impact
   if (iter != mTables.end())
   {
      return iter->second->mUseMaxOrd;
   }
   else
   {
      std::string what = "FiresLaunchComputer:  Type " + aType + " has no table data.";
      throw UtException(what);
   }
}

FiresTable* FiresTables::GetTable(const std::string& aType) const
{
   std::map<WsfStringId, FiresTable*>::const_iterator iter     = mTables.find(aType);
   FiresTable*                                        tablePtr = nullptr;
   if (iter != mTables.end())
   {
      tablePtr = iter->second;
   }
   return tablePtr;
}

//! Return the "max. ord.," or maximum height of the trajectory, and time of flight, for a given weapon type
//! and horizontal range to target.
//! @param aType The weapon type.  This must correspond with a SystemMunition name in one of the fires tables.
//! @param aRange The desired horizontal range of the flyout.
//! @param aMaxOrd (output) The max. ord. (max. height) for the trajectory.
//! @param aTOF (output) The time of flight of the round.
//! @return Whether an acceptable solution was computed.
bool FiresTables::GetMaxOrdAndTOF(const std::string& aType, double aRange, double& aMaxOrd, double& aTOF)
{
   bool                                         ok   = true;
   std::map<WsfStringId, FiresTable*>::iterator iter = mTables.find(aType);

   // From table, lookup max ord and time to impact
   if ((iter != mTables.end()) && (iter->second->mUseMaxOrd))
   {
      FiresTable* tablePtr = iter->second;
      if ((aRange >= tablePtr->GetMinimumRange()) && (aRange <= tablePtr->GetMaximumRange()))
      {
         tablePtr->GetMaxOrdAndTOF(aRange, aMaxOrd, aTOF);
      }
      else
      {
         auto logWarning = ut::log::warning() << "FiresTable: Launch Range and Type is invalid.";
         logWarning.AddNote() << "Launch Range: " << aRange;
         logWarning.AddNote() << "Type: " << aType;
         ok = false;
      }
   }
   else
   {
      ok = false;
   }

   return ok;
}

bool FiresTables::GetElevationAngleAndTOF(const std::string& aType, double aRange, double& aElevationAngle, double& aTOF)
{
   bool                                         ok   = true;
   std::map<WsfStringId, FiresTable*>::iterator iter = mTables.find(aType);

   // From table, lookup max elevation and time to impact
   if ((iter != mTables.end()) && (!iter->second->mUseMaxOrd))
   {
      FiresTable* tablePtr = iter->second;
      if ((aRange >= tablePtr->GetMinimumRange()) && (aRange <= tablePtr->GetMaximumRange()))
      {
         tablePtr->GetElevationAngleAndTOF(aRange, aElevationAngle, aTOF);
      }
      else
      {
         auto logWarning = ut::log::warning() << "FiresLaunchComputer: Launch Range with Type is invalid.";
         logWarning.AddNote() << "Launch Range: " << aRange;
         logWarning.AddNote() << "Type: " << aType;
         ok = false;
      }
   }
   else
   {
      ok = false;
   }

   return ok;
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
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_fires(aApplication); }
}

} // namespace Fires
