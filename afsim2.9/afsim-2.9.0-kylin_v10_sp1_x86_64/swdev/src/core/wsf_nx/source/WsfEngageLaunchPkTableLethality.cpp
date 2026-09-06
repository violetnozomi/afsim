// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfEngageLaunchPkTableLethality.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include "UtLog.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtScanDir.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfWeaponEngagement.hpp"

namespace
{
// Independent variable indices for table lookups.
enum
{
   cIV_ALTITUDE,
   cIV_TARGET_SPEED,
   cIV_DOWN_RANGE,
   cIV_CROSS_RANGE,
   cIV_COUNT // Number of independent variables
};

UtTable::Parameters sParameters;
} // namespace

WsfEngageLaunchPkTableLethality::WsfEngageLaunchPkTableLethality(WsfScenario& aScenario)
   : WsfWeaponEffects(aScenario)
   , mDefaultPk(0.0)
   , mPkTableFileFilter("*")
{
   // Use ONLY the Launch Pk value for this class:
   SetUseLaunchPk();

   if (sParameters.Empty())
   {
      // Register allowable independent variables.
      sParameters.AddRealParameter("altitude", UtInput::cLENGTH, UtTable::ValueGE(0.0), cIV_ALTITUDE);
      sParameters.AddRealParameter("target_speed", UtInput::cSPEED, UtTable::ValueGE(0.0), cIV_TARGET_SPEED);
      sParameters.AddRealParameter("down_range", UtInput::cLENGTH, UtTable::NoCheck(), cIV_DOWN_RANGE);
      sParameters.AddRealParameter("cross_range", UtInput::cLENGTH, UtTable::NoCheck(), cIV_CROSS_RANGE);
   }
}

// protected
WsfEngageLaunchPkTableLethality::WsfEngageLaunchPkTableLethality(const WsfEngageLaunchPkTableLethality& aSrc)
   : WsfWeaponEffects(aSrc)
   , mPkTablesPath(aSrc.mPkTablesPath)
   , mDefaultPk(aSrc.mDefaultPk)
   , mPkTableFileFilter(aSrc.mPkTableFileFilter)
   , mPkTables(aSrc.mPkTables)
{
}

//! Method to calculate the Launch Pk, in response to engagement
//! conditions during launch.  Will be called during Initialize().
//! @param aSimTime simulated time for the weapon effect.
// protected virtual
void WsfEngageLaunchPkTableLethality::CalcLaunchPk(double aSimTime)
{
   double launchPk = mDefaultPk; // Initialize Pk to default value as a fall-back

   WsfPlatform* firingPlatformPtr = GetEngagement()->GetFiringPlatform();
   WsfPlatform* targetPlatformPtr = GetEngagement()->GetTargetPlatform();
   WsfStringId  launcherType(firingPlatformPtr->GetTypeId());
   // fallback to WSF_PLATFORM type if target platform was not specified or has been removed from simulation.
   WsfStringId targetType(targetPlatformPtr ? targetPlatformPtr->GetTypeId() : UtStringIdLiteral("WSF_PLATFORM"));

   auto key  = std::make_pair(launcherType, targetType);
   auto pkit = mPkTables.find(key);
   if (pkit != mPkTables.end())
   {
      double args[cIV_COUNT];

      args[cIV_ALTITUDE]     = targetPlatformPtr->GetAltitude();
      args[cIV_TARGET_SPEED] = targetPlatformPtr->GetSpeed();

      double tgtLocWCS[3], launchLocWCS[3];
      firingPlatformPtr->GetLocationWCS(launchLocWCS);
      targetPlatformPtr->GetLocationWCS(tgtLocWCS);

      double groundRange    = UtMeasurementUtil::GroundRange(launchLocWCS, tgtLocWCS);
      double bearing        = firingPlatformPtr->RelativeBearing(tgtLocWCS);
      args[cIV_DOWN_RANGE]  = groundRange * cos(bearing);
      args[cIV_CROSS_RANGE] = groundRange * sin(bearing);

      launchPk = pkit->second->Lookup(args);
   }
   else if (DebugEnabled())
   {
      auto logger =
         ut::log::warning() << "WsfEngageLaunchPkTableLethality::CalcLaunchPk() could not find lookup table.";
      logger.AddNote() << "Launcher Type: " << launcherType;
      logger.AddNote() << "Target Type: " << targetType;
      logger.AddNote() << "Using default Pk value of " << mDefaultPk << ".";
   }

   SetLaunchPk(launchPk);
}


// virtual
WsfWeaponEffects* WsfEngageLaunchPkTableLethality::Clone() const
{
   return new WsfEngageLaunchPkTableLethality(*this);
}

// virtual
bool WsfEngageLaunchPkTableLethality::PreInitialize()
{
   if (mPkTablesPath.empty())
   {
      ut::log::error() << "WsfEngageLaunchPkTableLethality did not specify pk_tables_path.";
      return false;
   }

   // Read in Pk tables from path specified
   UtPath path(mPkTablesPath);
   if (!path.IsDirectory())
   {
      auto logger = ut::log::error() << "WsfEngageLaunchPkTableLethality pk_tables_path is not a valid directory.";
      logger.AddNote() << "Path: " << mPkTablesPath;
      return false;
   }

   UtScanDir::DirEntries dirEntries;
   UtScanDir::ScanDirRecursive(mPkTablesPath, dirEntries, mPkTableFileFilter);

   std::string errorString;
   for (auto& dir : dirEntries)
   {
      if (!dir.mDirectory)
      {
         if (!LoadPkTable(dir.mFilename, errorString))
         {
            auto logger = ut::log::warning() << "WsfEngageLaunchPkTableLethality file is not a valid Pk table file.";
            logger.AddNote() << "File: " << dir.mFilename;
            logger.AddNote() << "Error: " << errorString;
         }
      }
   }

   // Process Pk tables read in from files into UtTable structures
   for (auto& pkTableInput : mPkTableInputs)
   {
      std::unique_ptr<UtTable::Table> pkTablePtr;
      if (!pkTableInput.second->CreateTableFromInput(pkTablePtr, mDefaultPk, errorString))
      {
         auto logger = ut::log::warning() << "WsfEngageLaunchPkTableLethality table inputs are malformed for pair.";
         logger.AddNote() << "Using default constant Pk: " << mDefaultPk;
         logger.AddNote() << "Pair: " << pkTableInput.first.first << ", " << pkTableInput.first.second;
         logger.AddNote() << "Error: " << errorString;
      }
      mPkTables.emplace(pkTableInput.first, UtCloneablePtr<UtTable::Table>(std::move(pkTablePtr)));
   }

   return WsfWeaponEffects::PreInitialize();
}

// virtual
bool WsfEngageLaunchPkTableLethality::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   if (GetIncidentalDamageAllowed())
   {
      // If incidental damage was allowed, display a warning to the user and suggest a workaround. This script can be
      // used to not apply incidental damage to platforms because disallowing the command entirely isn't appropriate for
      // the time being.
      auto out = ut::log::warning() << "Weapon effect has incidental damage enabled.";
      out.AddNote() << "Name: " << GetType();
      out.AddNote() << "Type: " << GetBaseType();
      out.AddNote() << "To avoid inappropriate incidental damage being applied to platforms, consider disabling "
                       "incidental damage or using 'on_weapon_target_engagement' to degrade Pk values.";
   }

   return WsfWeaponEffects::Initialize(aSimTime, aEngagementPtr);
}

// virtual
bool WsfEngageLaunchPkTableLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand(false);
   std::string command(aInput.GetCommand());

   if (command == "pk_tables_path")
   {
      myCommand = true;
      aInput.ReadValueQuoted(mPkTablesPath);
   }
   else if (command == "default_pk")
   {
      myCommand = true;
      aInput.ReadValue(mDefaultPk);
      aInput.ValueInClosedRange(mDefaultPk, 0.0, 1.0);
   }
   else if (command == "file_filter")
   {
      myCommand = true;
      aInput.ReadValueQuoted(mPkTableFileFilter);
   }
   else if ((command == "use_intercept_pk") || (command == "launch_pk") || (command == "intercept_pk") ||
            (command == "use_pk_table"))
   {
      throw UtInput::BadValue(aInput, "WsfEngageLaunchPkTableLethality is incompatible with the " + command + " option.");
   }
   else
   {
      // Test the possibility that command is for the base class.
      myCommand = WsfWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

bool WsfEngageLaunchPkTableLethality::LoadPkTable(const std::string& aFileName, std::string& aErrorString)
{
   static const std::string cSITE_PLATFORM_TYPE_TOKEN   = "Site Platform Type";
   static const std::string cTARGET_PLATFORM_TYPE_TOKEN = "Target Platform Type";
   static const std::string cLENGTH_UNITS_TOKEN         = "Length Units";
   static const std::string cSPEED_UNITS_TOKEN          = "Speed Units";
   static const std::string cALTITUDE_TOKEN             = "Altitude";
   static const std::string cSPEED_TOKEN                = "Speed";

   // Open Pk table file for reading
   std::ifstream ifs(aFileName.c_str());

   if (!ifs)
   {
      aErrorString = "Could not open file for reading";
      return false;
   }

   // Read file header
   std::string sitePlatformType, targetPlatformType, lengthUnits, speedUnits;
   double      altitudeValue, speedValue;
   if (!ReadHeaderLine(ifs, cSITE_PLATFORM_TYPE_TOKEN, sitePlatformType, aErrorString) ||
       !ReadHeaderLine(ifs, cTARGET_PLATFORM_TYPE_TOKEN, targetPlatformType, aErrorString) ||
       !ReadHeaderLine(ifs, cLENGTH_UNITS_TOKEN, lengthUnits, aErrorString) ||
       !ReadHeaderLine(ifs, cSPEED_UNITS_TOKEN, speedUnits, aErrorString) ||
       !ReadHeaderLine(ifs, cALTITUDE_TOKEN, altitudeValue, aErrorString) ||
       !ReadHeaderLine(ifs, cSPEED_TOKEN, speedValue, aErrorString))
   {
      return false;
   }

   if (!UtInput::ValidateUnits(lengthUnits, UtInput::cLENGTH))
   {
      aErrorString = "Invalid length units: " + lengthUnits;
      return false;
   }

   if (!UtInput::ValidateUnits(speedUnits, UtInput::cSPEED))
   {
      aErrorString = "Invalid speed units:" + speedUnits;
      return false;
   }

   // Look for Pk Table for specified site platform type and target platform type. If it doesn't exist, create it.
   auto key = std::make_pair(sitePlatformType, targetPlatformType);
   auto it  = mPkTableInputs.find(key);
   if (it == mPkTableInputs.end())
   {
      auto result = mPkTableInputs.insert(std::make_pair(key, ut::make_unique<PkTableInput>()));
      it          = result.first;
   }

   if (!it->second->ReadPkTableInput(ifs,
                                     UtLengthValue(altitudeValue, lengthUnits),
                                     UtSpeedValue(speedValue, speedUnits),
                                     lengthUnits))
   {
      aErrorString = "Invalid Cross Range/Down Range/Pk table input";
      return false;
   }

   return true;
}

template<typename T>
bool WsfEngageLaunchPkTableLethality::ReadHeaderLine(std::ifstream&     aFile,
                                                     const std::string& aExpectedHeaderField,
                                                     T&                 aHeaderValue,
                                                     std::string&       aErrorString)
{
   std::string headerField;
   std::getline(aFile, headerField, ':');
   if (aExpectedHeaderField != headerField)
   {
      aErrorString = "Header token \"" + aExpectedHeaderField + "\" not found";
      return false;
   }

   std::string headerValueStr;
   std::getline(aFile, headerValueStr);
   UtStringUtil::TrimWhiteSpace(headerValueStr);
   std::istringstream iss(headerValueStr);
   iss >> aHeaderValue;

   return true;
}

bool WsfEngageLaunchPkTableLethality::PkTableInput::ReadPkTableInput(std::ifstream&     aFile,
                                                                     double             aAltitude,
                                                                     double             aTargetSpeed,
                                                                     const std::string& aLengthUnits)
{
   std::unique_ptr<CrDrPkTable> tablePtr(new CrDrPkTable);

   // Read in cross ranges along column headers
   std::string crossRangesStr;
   std::getline(aFile, crossRangesStr);
   {
      std::istringstream iss(crossRangesStr);

      double cr;
      while (iss >> cr)
      {
         tablePtr->mCrossRanges.push_back(UtLengthValue(cr, aLengthUnits));
      }
   }

   // Read in down range followed by Pk values along row
   while (!aFile.eof())
   {
      std::string rowStr;
      std::getline(aFile, rowStr);
      if (rowStr.empty())
      {
         continue;
      } // ignore empty lines
      std::istringstream iss(rowStr);

      double dr;
      iss >> dr;
      tablePtr->mDownRanges.push_back(UtLengthValue(dr, aLengthUnits));

      double pk;
      while (iss >> pk)
      {
         tablePtr->mPkValues.push_back(pk);
      }
   }

   size_t numCr   = tablePtr->mCrossRanges.size();
   size_t numDr   = tablePtr->mDownRanges.size();
   size_t numPk   = tablePtr->mPkValues.size();
   bool   isValid = ((numCr > 1) && (numDr > 1) && (numCr * numDr == numPk));

   if (!isValid)
   {
      return false;
   }

   mCrDrPkTables[aAltitude][aTargetSpeed] = std::move(tablePtr);
   return true;
}

bool WsfEngageLaunchPkTableLethality::PkTableInput::CreateTableFromInput(std::unique_ptr<UtTable::Table>& aPkTablePtr,
                                                                         double       aDefaultConstant,
                                                                         std::string& aErrorString)
{
   // Push a string into UtInput object containing table data and pass to UtTable for processing
   // Formatting is not necessary but could aid in debugging if necessary
   std::ostringstream oss;
   oss << "irregular_table\n";
   // Define independent variables
   // TODO: For IVs and DV, set precision and extrapolation?
   oss << "   independent_variable altitude units meters\n";
   oss << "   independent_variable target_speed units m/s\n";
   oss << "   independent_variable down_range units meters\n";
   oss << "   independent_variable cross_range units meters\n";
   // Define table, level by level
   for (auto& alts : mCrDrPkTables)
   {
      oss << "      altitude " << alts.first << std::endl;
      for (auto& speeds : alts.second)
      {
         oss << "         target_speed " << speeds.first << std::endl;
         auto& pkTablePtr = speeds.second;
         auto  pk         = pkTablePtr->mPkValues.begin();
         for (auto& dr : pkTablePtr->mDownRanges)
         {
            oss << "            down_range " << dr << std::endl;
            std::ostringstream cross;
            cross << "               cross_range";
            std::ostringstream pkoss;
            pkoss << "               values     ";
            for (auto& cr : pkTablePtr->mCrossRanges)
            {
               cross << std::setw(10) << cr;
               pkoss << std::setw(10) << *pk++;
            }
            cross << std::endl;
            pkoss << std::endl;
            oss << cross.str() << pkoss.str();
         }
      }
   }
   oss << "end_irregular_table\n";

   UtInput input;
   input.PushInputString(oss.str());

   try
   {
      aPkTablePtr = UtTable::LoadInstance(input, UtInput::cNON_DIMENSIONAL, UtTable::ValueGE_LE(0.0, 1.0), sParameters);
   }
   catch (UtException& e)
   {
      aErrorString = e.what();
      // Error loading in table, use constant value as backup
      aPkTablePtr = ut::make_unique<UtTable::ConstantTable>(aDefaultConstant);
      return false;
   }

   return true;
}
