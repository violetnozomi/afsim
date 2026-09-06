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

// Programming note:
//
// Note that there is a subtle problem with the deletion of the singleton that is solved in a somewhat
// unusual manner.  If a programmer creates a static Terrain object (not very useful, but certainly
// not prohibited), deletion of that object is done *AFTER* the program completes - along with all the
// other static objects - which includes the singleton tile manager.  And here's the problem - we can't
// guarantee which is deleted first.  If the manager is deleted first we must ensure that the deletion
// of a static terrain object doesn't try to reference the deleted tile manager!  Hence we must do a
// little fancy footwork.  Our singleton is just a front-end to the DtedTileManager.  The constructor
// simply saves the pointer to the DtedTileManager and the destructor zeros it.  The Terrain
// destructor checks the pointer and only calls the manager if it is non-zero.

#include "WsfTerrain.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib> // for 'getenv'
#include <cstring>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>

#include "BathymetryXYZ_TileManager.hpp"
#include "DtedDir.hpp"
#include "DtedTile.hpp"
#include "DtedTileManager.hpp"
#include "FloatGridTile.hpp"
#include "FloatGridTileManager.hpp"
#include "GeoRect.hpp"
#include "GeodeticEarthDatabase.hpp"
#include "GeodeticEarthElevCompute.hpp"
#include "GeotiffTileManager.hpp"
#include "UtCast.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "VisualDatabase.hpp"
#include "WsfCodedZone.hpp"
#include "WsfDtedRect.hpp"
#include "WsfEarthGravityModel.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneReference.hpp"

namespace
{
constexpr double cEARTH_HIGH_POINT = 8850.0; // Mt. Everest
// The following are used for validation checks within the code.
// The level 1 standard state an accuracy of +/- 30 m. A little slop will be allowed.

constexpr float cMIN_ELEV_ALLOWED_F       = -500.0f; // -418 m near Dead Sea
constexpr float cMAX_ELEV_ALLOWED_F       = 8950.0f; // Mt Everest is 8848 m
constexpr float cSLIGHTLY_ABOVE_TERRAIN_F = 0.1f;    // Meter tolerance.

constexpr double cMIN_ELEV_ALLOWED = -500.0; // -418 m near Dead Sea
constexpr double cMAX_ELEV_ALLOWED = 8950.0; // Mt Everest is 8848 m

constexpr double cMAX_OCEAN_DEPTH_ALLOWED =
   11034.0; // 36200.79 ft Challenger Deep is approximately 36,200 feet deep (ref NOAA)

constexpr double cVECTOR_INSERTION_TOLERANCE = 0.000001;

void AddToVector(double aValue, std::vector<double>& aValues)
{
   for (size_t i = 0; i != aValues.size(); ++i)
   {
      // Had a round-off error issue, so put in a tolerance test:
      if (fabs(aValue - aValues[i]) < cVECTOR_INSERTION_TOLERANCE)
      {
         return;
      }
   }
   aValues.push_back(aValue);
}

// =================================================================================================
double M2F(double aValue)
{
   return aValue * UtMath::cFT_PER_M;
}

void ConvertWCSToLL(const double aWCS[3], double& aLat, double& aLon)
{
   // This was extracted from UtSphericalEarth::ConvertECEFToLLA.
   // We didn't need altitude and not computing it gets rid of a sqrt();

   aLat = atan2(aWCS[2], (sqrt((aWCS[0] * aWCS[0]) + (aWCS[1] * aWCS[1]))));
   aLon = atan2(aWCS[1], aWCS[0]);
   aLat *= UtMath::cDEG_PER_RAD;
   aLon *= UtMath::cDEG_PER_RAD;
}

// =================================================================================================
// Return the nominal latitude, longitude and resync step sizes for the specified tile.
void GetTileParameters(GeoElevationTile* aTilePtr,
                       double&           aLatInterval,
                       double&           aLonInterval,
                       double&           aMetersPerDegreeLat,
                       double&           aMetersPerDegreeLon,
                       double&           aResyncInterval)
{
   static float resyncTable[] = {
      10000.0F, 10000.0F, 10000.0F, 10000.0F, 10000.0F, //  0 -  4
      10000.0F, 10000.0F, 10000.0F, 10000.0F, 10000.0F, //  5 -  9
      10000.0F, 10000.0F, 10000.0F, 10000.0F, 10000.0F, // 10 - 14
      10000.0F, 10000.0F, 10000.0F, 10000.0F, 9900.0F,  // 15 - 19
      9700.0F,  9500.0F,  9400.0F,  9200.0F,  9000.0F,  // 20 - 24
      8800.0F,  8600.0F,  8500.0F,  8300.0F,  8200.0F,  // 25 - 29
      8000.0F,  7900.0F,  7700.0F,  7600.0F,  7400.0F,  // 30 - 34
      7300.0F,  7200.0F,  7100.0F,  6900.0F,  6800.0F,  // 35 - 39
      6700.0F,  6600.0F,  6400.0F,  6300.0F,  6100.0F,  // 40 - 44
      6100.0F,  5900.0F,  5800.0F,  5700.0F,  5600.0F,  // 45 - 49
      5600.0F,  5500.0F,  5300.0F,  5200.0F,  5100.0F,  // 50 - 54
      5000.0F,  4900.0F,  4800.0F,  4700.0F,  4600.0F,  // 55 - 59
      4400.0F,  4300.0F,  4200.0F,  4000.0F,  4000.0F,  // 60 - 64
      3800.0F,  3700.0F,  3600.0F,  3400.0F,  3300.0F,  // 65 - 69
      3200.0F,  3000.0F,  3000.0F,  2900.0F,  2700.0F,  // 70 - 74
      2600.0F,  2400.0F,  2200.0F,  1800.0F,  1500.0F,  // 75 - 79
      1000.0F,  200.0F,   200.0F,   200.0F,   100.0F    // 80 - 84
   };

   aLatInterval = aTilePtr->GetLatInterval();
   aLonInterval = aTilePtr->GetLonInterval();

   // If we have loaded a level 0 or a dummy tile, we need to set a step size
   // that isn't so large.  Otherwise the first step will takes us a long ways
   // into the next tile which would cause us to lose a lot of information.
   // If it is level 0 or a dummy tile then we'll use level 1 parameters.
   //
   // Level 1 has a latitude resolution of 3".  Anything greater than 5" will
   // be assumed to be level 0 or a dummy tile.

   double midLat = fabs(0.5 * (aTilePtr->GetSWLat() + aTilePtr->GetNELat()));
   if (aLatInterval > (5.0 / 3600.0))
   {
      aLatInterval = 3.0 / 3600.0;
      if (midLat < 50.0)
      {
         aLonInterval = 3.0 / 3600.0;
      }
      else if (midLat < 70.0)
      {
         aLonInterval = 6.0 / 3600.0;
      }
      else if (midLat < 75.0)
      {
         aLonInterval = 9.0 / 3600.0;
      }
      else if (midLat < 80.0)
      {
         aLonInterval = 12.0 / 3600.0;
      }
      else
      {
         aLonInterval = 18.0 / 3600.0;
      }
   }

   // Compute the nominal meters per degree latitude and longitude.

   aMetersPerDegreeLat = UtMath::cTWO_PI * UtSphericalEarth::cEARTH_RADIUS / 360.0;
   aMetersPerDegreeLon = aMetersPerDegreeLat * cos(midLat * UtMath::cRAD_PER_DEG);

   // Get the maximum distance between updates of the lat/lon.

   int intLat      = static_cast<int>(std::min(midLat, 84.0));
   aResyncInterval = resyncTable[intLat];
}

} // end anonymous namespace

namespace wsf
{

// =================================================================================================
int TerrainInterface::GetDTEDLevelByIndex(size_t aIndex)
{
   int level = -1;
   if (aIndex < mDTEDLevel.size())
   {
      if (aIndex != ut::npos)
      {
         level = mDTEDLevel[aIndex];
      }
   }
   return level;
}

// =================================================================================================
void TerrainInterface::GetDTEDPathByIndex(size_t aIndex, std::string& aPath)
{
   std::string path = "";
   if (aIndex < mDTEDPath.size())
   {
      if (aIndex != ut::npos)
      {
         path = mDTEDPath[aIndex];
      }
   }
   aPath = path;
}

// =================================================================================================
//! See if the supplied command is one of mine.
//! @param aInput The input stream.
//! @return 'true' if the command was a terrain related command or false' if not.
//! @see WsfObject::ProcessInput.
bool TerrainInterface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "terrain")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "disable")
         {
            mDisabledByUser = true;
            mIsEnabled      = false;
         }
         else if (command == "minimum_allowable_step_size")
         {
            aInput.ReadValueOfType(mMinAllowableStepSize, UtInput::cLENGTH);
            aInput.ValueGreater(mMinAllowableStepSize, 0.0);
         }
         else if (command == "maximum_terrain_height")
         {
            aInput.ReadValueOfType(mMaxTerrainHeight, UtInput::cLENGTH);
         }
         else if (command == "maximum_data_size")
         {
            aInput.ReadValueOfType(mMaxDataSize, UtInput::cDATA_SIZE);
            mMaxDataSize /= 8; // bits to bytes.
         }
         else if (command == "dted")
         {
            mDataType = cDTED;
            DtedInput input;
            input.mRaw = false;
            aInput.ReadValue(input.mLevel);
            aInput.ValueInClosedRange(input.mLevel, 0, 2);
            aInput.ReadValueQuoted(input.mDirName);
            input.mDirName = aInput.SubstitutePathVariables(input.mDirName);
            mDtedInputs.push_back(input);
            mDTEDPath.push_back(input.mDirName);
            mDTEDLevel.push_back(input.mLevel);
            mDTED = true;
         }
         else if (command == "visual_db")
         {
            mDataType = cVISUAL_DATABASE;
            aInput.ReadValueQuoted(mVisualDBFile);
         }
         else if ((command == "raw_dted") || (command == "raw-dted")) // For CME compatibility
         {
            mDataType = cDTED;
            DtedInput input;
            input.mRaw = true;
            aInput.ReadValue(input.mLevel);
            aInput.ValueInClosedRange(input.mLevel, 0, 2);
            aInput.ReadValueQuoted(input.mDirName);
            input.mDirName = aInput.LocateFile(input.mDirName);
            input.ProcessRect(aInput);

            mDtedInputs.push_back(input);
            mDTEDPath.push_back(input.mDirName);
            mDTEDLevel.push_back(input.mLevel);
            mDTED = true;
         }
         else if ((command == "eld_earth_db") || (command == "geodetic_earth_db"))
         {
            mDataType = cGEODETIC_EARTH_DATABASE;

            GeodeticEarthDatabase::SetDBNames();

            aInput.ReadValue(mGeodeticEarthDBName);

            bool ok = GeodeticEarthDatabase::ValidateDBName(mGeodeticEarthDBName);

            if (!ok)
            {
               throw UtInput::BadValue(aInput, "Invalid Geodetic DB name used, " + mGeodeticEarthDBName);
            }
            else
            {
               // std::string GeodeticEarthFilePathname = "";
               aInput.ReadValue(mGeodeticEarthFilePathname);

               if (mGeodeticEarthFilePathname.empty())
               {
                  throw UtInput::BadValue(aInput, "Invalid Geodetic pathname entered, ");
               }
               else
               {
                  std::string flatOrRound = "";
                  mGeodeticDatabaseType   = 0;
                  aInput.ReadValue(flatOrRound);
                  UtStringUtil::ToUpper(flatOrRound);
                  if (flatOrRound == "ROUND")
                  {
                     mGeodeticDatabaseType = 2;
                  }
                  else if (flatOrRound == "FLAT")
                  {
                     mGeodeticDatabaseType = 1;
                  }
                  else
                  {
                     throw UtInput::BadValue(aInput, "Missing round or flat indicator for type of database being used");
                  }
                  mGeodeticDatabase = true;
               }
            }
         }
         else if ((command == "float_grid") || (command == "float_grid_vegetation"))
         {
            FloatGridInput input;
            mDataType = cFLOAT_GRID;
            std::string dirName;
            aInput.ReadValueQuoted(input.mDirName);
            input.mDirName = aInput.SubstitutePathVariables(input.mDirName);
            input.ProcessRect(aInput);
            input.mVegetation = (command == "float_grid_vegetation");
            mFloatGridInputs.push_back(input);
         }
         else if ((command == "float_grid_file") || (command == "float_grid_vegetation_file"))
         {
            mDataType = cFLOAT_GRID;
            FloatGridInput input;
            aInput.ReadValueQuoted(input.mFileName);
            input.mFileName   = aInput.SubstitutePathVariables(input.mFileName);
            input.mVegetation = (command != "float_grid_file");
         }
         else if (command == "geotiff")
         {
            GeotiffInput input;
            mDataType = cGEOTIFF;
            aInput.ReadValueQuoted(input.mDirName);
            input.mDirName = aInput.SubstitutePathVariables(input.mDirName);
            mGeotiffInputs.push_back(input);
         }
         else if (command == "bathymetry")
         {
            BathymetryInput input;
            mDataType = cBATHYMETRY;
            aInput.ReadValueQuoted(input.mFileName);
            input.mFileName = aInput.SubstitutePathVariables(input.mFileName);
            mBathmetryInputs.push_back(input);
         }
         else if (command == "load_cme_terrain")
         {
            // CheckTileManager(cDTED);  // todo if cme supports multiple formats, must revisit this check.
            mDataType        = cDTED;
            mLoadCME_Terrain = true;
         }
         else if (command == "cme_path")
         {
            aInput.ReadValueQuoted(mCME_Path);
         }
         else if (command == "ignore_missing_cme_terrain")
         {
            mIgnoreMissingCME_Terrain = true;
         }
         else if (command == "debug_masking")
         {
            mDebugTerrainMasking = true;
         }
         else if (command == "no_debug_masking")
         {
            mDebugTerrainMasking = false;
         }
         else if (command == "debug_extrema")
         {
            mDebugTerrainExtrema = true;
         }
         else if (command == "no_debug_extrema")
         {
            mDebugTerrainExtrema = false;
         }
         else if (command == "earth_radius_multiplier")
         {
            // Earth radius scale factor for query_height and query_horizon
            aInput.ReadValue(mEarthRadiusMultiplier);
         }
         else if (command == "use_fast_masking_check")
         {
            mUseFastMaskingCheck = true;
         }
         else if (command == "validate_terrain")
         {
            mValidateDTED = true;
         }
         else if (command == "dted_checksum")
         {
            bool evaluateDTED_Checksum = false;
            aInput.ReadValue(evaluateDTED_Checksum);
            DtedTile::SetEvaluateChecksum(evaluateDTED_Checksum);
         }
         else if (command == "query_height")
         {
            Query q;
            aInput.ReadValueOfType(q.mLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(q.mLon, UtInput::cLONGITUDE);
            q.mType = Query::cHEIGHT;
            mQueryList.push_back(q);
         }
         else if (command == "query_horizon")
         {
            Query q;
            q.mType = Query::cHORIZON;
            aInput.ReadValueOfType(q.mLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(q.mLon, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(q.mAlt, UtInput::cLENGTH);
            aInput.ReadValueOfType(q.mLat2, UtInput::cLATITUDE);
            aInput.ReadValueOfType(q.mLon2, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(q.mAlt2, UtInput::cLENGTH);
            q.mEarthRadius = mEarthRadiusMultiplier;
            mQueryList.push_back(q);
         }
         else if (command == "query_masking")
         {
            Query q;
            q.mType = Query::cMASKING;
            aInput.ReadValueOfType(q.mLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(q.mLon, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(q.mAlt, UtInput::cLENGTH);
            aInput.ReadValueOfType(q.mLat2, UtInput::cLATITUDE);
            aInput.ReadValueOfType(q.mLon2, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(q.mAlt2, UtInput::cLENGTH);
            q.mEarthRadius = mEarthRadiusMultiplier;
            mQueryList.push_back(q);
         }
         else if (command == "test_extrema_in_zone_function")
         {
            UtInputBlock inputBlock2(aInput);

            std::unique_ptr<WsfCodedZone> zone(new WsfCodedZone);

            while (inputBlock2.ReadCommand())
            {
               if (!zone->ProcessInput(aInput))
               {
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            Query q;
            q.mType    = Query::cTEST_EXTREMA_IN_ZONE;
            q.mZonePtr = zone.release();
            mQueryList.push_back(q);
         }
         else if (wsf::EarthGravityModelExtension::Get(*GetScenario()).ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

bool TerrainInterface::RectInput::ProcessRect(UtInput& aInput)
{
   aInput.ReadValue(mSWLat);
   aInput.ValueInClosedRange(mSWLat, -90, 90);
   aInput.ReadValue(mSWLon);
   aInput.ValueInClosedRange(mSWLon, -180, 180);
   aInput.ReadValue(mNELat);
   aInput.ValueInClosedRange(mNELat, -90, 90);
   aInput.ValueGreater(mNELat, mSWLat);
   aInput.ReadValue(mNELon);
   aInput.ValueInClosedRange(mNELon, -180, 180);
   aInput.ValueGreater(mNELon, mSWLon);
   return true;
}

// =================================================================================================
//! Add a NIMA DTED directory to the list of directories containing elevation data.
//!
//! The directory tree should be in the EXACT format as copied from a NIMA CD (i.e.:
//! The specified directory must contain a file named 'dmed' and a sub-directory
//! named 'dted' which contains the longitudinal directories)
//! The 'dmed' file is expected to be intact and reflect the limits of what is
//! contained in the directory.
//!
//! @param aDirName The name of the directory.  It must contain a valid 'dmed' file
//! and a directory 'dted' which contains the longitudinal directories.
//! @param aLevel   The DTED level (0, 1, or 2)
//! @return 'true' if successful and 'false' if not.
bool TerrainInterface::AddDtedDirectory(const std::string& aDirName, int aLevel)
{
   bool ok = false;
   if (DirectoryExists(aDirName + "/dted", "dted") && (mDataType == cDTED) &&
       (static_cast<DtedTileManager&>(GetManager()).AddDirectory(aDirName.c_str(), aLevel) == 0))
   {
      GetScenario()->GetSystemLog().WriteLogEntry("terrain directory dted " + aDirName);
      mIsDefined = true;
      if (!mDisabledByUser)
      {
         mIsEnabled = true;
      }
      ok = true;
   }
   return ok;
}

// =================================================================================================
//! Add a NIMA DTED directory to the list of directories containing elevation data.
//!
//! This method is used when the 'dmed' file does not exist or does not represent
//! the true contents of the directory (such as when multiple DTED CDs have been
//! copied into the same directory). The bounding box of the region must be specified.
//!
//! @param aDirName The name of the directory. The immediate sub-directories must
//! be the longitudinal directories.
//! @param aLevel   The DTED level (0, 1, or 2)
//! @param aSwLat   The latitude of the southwest corner of the bounding box.
//! @param aSwLon   The longitude of the southwest corner of the bounding box.
//! @param aNeLat   The latitude of the northeast corner of the bounding box.
//! @param aNeLon   The longitude of the northeast corner of the bounding box.
//! @return 'true' if successful and 'false' if not.
bool TerrainInterface::AddRawDtedDirectory(const std::string& aDirName, int aLevel, int aSwLat, int aSwLon, int aNeLat, int aNeLon)
{
   bool ok = false;
   if (DirectoryExists(aDirName, "raw_dted") && (mDataType == cDTED) &&
       (static_cast<DtedTileManager&>(GetManager()).AddDirectory(aDirName.c_str(), aLevel, aSwLat, aSwLon, aNeLat, aNeLon) ==
        0))
   {
      GetScenario()->GetSystemLog().WriteLogEntry("terrain directory raw_dted " + aDirName);
      mIsDefined = true;
      if (!mDisabledByUser)
      {
         mIsEnabled = true;
      }
      ok = true;
   }
   return ok;
}

bool TerrainInterface::AddGeotiffDirectory(const std::string& aDirName)
{
   bool ok = false;
   if (mDataType == cGEOTIFF)
   {
      UtPath tiffDir(aDirName);
      if (tiffDir.Exists())
      {
         if (tiffDir.IsDirectory()) // its a dir
         {
            if (static_cast<Geodata::GeotiffTileManager&>(GetManager()).AddDirectory(aDirName))
            {
               GetScenario()->GetSystemLog().WriteLogEntry("terrain directory geotiff " + aDirName);
               mIsDefined = true;
               if (!mDisabledByUser)
               {
                  mIsEnabled = true;
               }
               ok = true;
            }
         }
         else if (tiffDir.IsFile()) // its a file
         {
            if (static_cast<Geodata::GeotiffTileManager&>(GetManager()).AddFile(aDirName))
            {
               GetScenario()->GetSystemLog().WriteLogEntry("terrain file geotiff " + aDirName);
               mIsDefined = true;
               if (!mDisabledByUser)
               {
                  mIsEnabled = true;
               }
               ok = true;
            }
         }
      }
      else
      {
         auto out = ut::log::error() << " Terrain directory or file (tiff) does not exist.";
         out.AddNote() << "Path: " << aDirName;
      }
   }
   return ok;
}

bool TerrainInterface::AddBathymetry(const std::string& aFileName)
{
   bool ok = false;
   if (mDataType == cBATHYMETRY)
   {
      UtPath file(aFileName);
      if (file.IsFile()) // it's a file
      {
         if (static_cast<Geodata::BathymetryXYZ_TileManager&>(GetBathymetryManager()).AddFile(aFileName))
         {
            GetScenario()->GetSystemLog().WriteLogEntry("terrain file bathymetry " + aFileName);
            mBathymetryDefined = true;
            if (!mDisabledByUser)
            {
               mIsBathymetryEnabled = true;
            }
            ok = true;
         }
      }
      else
      {
         auto out = ut::log::error() << " Bathymetry file does not exist.";
         out.AddNote() << "Path: " << aFileName;
      }
   }
   return ok;
}

// =================================================================================================
//! Set a Grid-Float processed DEM directory tree.
//! The bounding box of the region must be specified.
//!
//! @param aDirName The name of the directory. The immediate sub-directories must
//! be the longitudinal directories.
//! @param aSwLat   The latitude of the southwest corner of the bounding box.
//! @param aSwLon   The longitude of the southwest corner of the bounding box.
//! @param aNeLat   The latitude of the northeast corner of the bounding box.
//! @param aNeLon   The longitude of the northeast corner of the bounding box.
//! @param aTileManager The tile manager
//! @return 'true' if successful and 'false' if not.
bool TerrainInterface::SetFloatGridDirectory(const std::string&       aDirName,
                                             double                   aSwLat,
                                             double                   aSwLon,
                                             double                   aNeLat,
                                             double                   aNeLon,
                                             GeoElevationTileManager& aTileManager)

{
   bool ok = false;
   if (DirectoryExists(aDirName, "float_grid") && (mDataType == cFLOAT_GRID) &&
       (static_cast<FloatGridTileManager&>(aTileManager).AddDirectory(aDirName.c_str(), aSwLat, aSwLon, aNeLat, aNeLon) == 0))
   {
      GetScenario()->GetSystemLog().WriteLogEntry("terrain directory float_grid " + aDirName);
      mIsDefined = true;
      if (!mDisabledByUser)
      {
         if (aTileManager.GetType() == Terrain::cTERRAIN)
         {
            mIsEnabled = true;
         }
         else
         {
            mIsVegetationEnabled = true;
         }
      }
      ok = true;
   }
   return ok;
}

// =================================================================================================
//! Set a Grid-Float processed DEM directory tree.
//! The bounding box of the region must be specified.
//!
//! @param aFileName The name of the file.  The program must be able to find the file, independent of
//! any directory entries you may have made; so there must be an absolute or relative path associated with it.
//! @param aTileManager The tile manager
//! @return 'true' if successful and 'false' if not.
bool TerrainInterface::SetFloatGridFile(const std::string& aFileName, GeoElevationTileManager& aTileManager)
{
   bool ok = false;
   if ((mDataType == cFLOAT_GRID) && (static_cast<FloatGridTileManager&>(aTileManager).AddFile(aFileName.c_str()) == 0))
   {
      GetScenario()->GetSystemLog().WriteLogEntry("terrain file float_grid " + aFileName);
      mIsDefined = true;
      if (!mDisabledByUser)
      {
         if (aTileManager.GetType() == Terrain::cTERRAIN)
         {
            mIsEnabled = true;
         }
         else
         {
            mIsVegetationEnabled = true;
         }
      }
      ok = true;
   }
   return ok;
}

// =================================================================================================
//! Disable terrain processing even if terrain has been loaded.
//! All subsequent terrain calls will operate as though no terrain
//! has been loaded.
// static
void TerrainInterface::DisableTerrain()
{
   mIsEnabled           = false;
   mIsBathymetryEnabled = false;
}

// =================================================================================================
//! (Re)enable terrain processing.  If terrain has been defined
//! and terrain processing has not been disabled by the user then
//! subsequent terrain processing calls will utilize the terrain.

// static
void TerrainInterface::EnableTerrain()
{
   mIsEnabled = false;
   if (mIsDefined && (!mDisabledByUser))
   {
      mIsEnabled = true;
   }
   mIsBathymetryEnabled = false;
   if (mBathymetryDefined && (!mDisabledByUser))
   {
      mIsBathymetryEnabled = true;
   }
}

// =================================================================================================
GeoElevationTileManager& TerrainInterface::GetVegetationManager()
{
   std::call_once(mGeoTileVegetationManagerFlag,
                  [this]()
                  {
                     if (mDataType == cDTED)
                     {
                        auto dtedTileManagerPtr = ut::make_unique<DtedTileManager>();
                        dtedTileManagerPtr->SetTileCacheSize(-1);
                        mGeoTileVegetationManagerPtr = std::move(dtedTileManagerPtr);
                     }
                     else
                     {
                        mGeoTileVegetationManagerPtr = ut::make_unique<FloatGridTileManager>();
                     }
                     mGeoTileVegetationManagerPtr->SetType(Terrain::cVEGETATION);
                  });
   return *mGeoTileVegetationManagerPtr;
}

// =================================================================================================
// private
GeoElevationTileManager& TerrainInterface::GetManager()
{
   std::call_once(mGeoTileManagerFlag,
                  [this]()
                  {
                     if (mDataType == cDTED)
                     {
                        auto dtedTileManagerPtr = ut::make_unique<DtedTileManager>();
                        dtedTileManagerPtr->SetTileCacheSize(-1);
                        mGeoTileManagerPtr = std::move(dtedTileManagerPtr);
                     }
                     else if (mDataType == cGEOTIFF)
                     {
                        mGeoTileManagerPtr = ut::make_unique<Geodata::GeotiffTileManager>();
                     }
                     else
                     {
                        mGeoTileManagerPtr = ut::make_unique<FloatGridTileManager>();
                     }
                     mGeoTileManagerPtr->SetType(Terrain::cTERRAIN);
                  });
   return *mGeoTileManagerPtr;
}

// =================================================================================================
GeoElevationTileManager& TerrainInterface::GetBathymetryManager()
{
   std::call_once(mGeoTileBathymetryManagerFlag,
                  [this]()
                  {
                     if (mDataType == cBATHYMETRY)
                     {
                        mGeoTileBathymetryManagerPtr = ut::make_unique<Geodata::BathymetryXYZ_TileManager>();
                        mGeoTileBathymetryManagerPtr->SetTileCacheSize(-1); // Never purge tiles
                        mGeoTileBathymetryManagerPtr->SetType(Terrain::cBATHYMETRY);
                     }
                  });
   return *mGeoTileBathymetryManagerPtr;
}

// =================================================================================================
//@Note This method applies both to the vegetation layer, if present, and the terrain elevation layer.
bool TerrainInterface::MaskedByTerrainP(double                   aLat1,
                                        double                   aLon1,
                                        double                   aAlt1,
                                        double                   aLat2,
                                        double                   aLon2,
                                        double                   aAlt2,
                                        double                   aMaxRange,
                                        GeoElevationTileManager& aTileManager, // Used for a call to MaskedByTerrainFastP
                                        double                   aRadiusScale /* = 1.0 */)
{
   bool maskedByTerrain = false;

   // If terrain processing is not enabled then simply make sure the altitudes not below
   if ((!mIsEnabled && !mIsBathymetryEnabled) || (mDataType == cVISUAL_DATABASE) || (mDataType == cGEODETIC_EARTH_DATABASE))
   {
      return maskedByTerrain;
   }

   // Dispatch the masking check to the fast version if the radius scale
   // is 1.0 and the user indicated use the fast version when possible.
   if ((aRadiusScale == 1.0) && mUseFastMaskingCheck)
   {
      return MaskedByTerrainFastP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, aTileManager);
   }

   // Arrange the points so the first point is the lowest

   double lat1 = aLat1;
   double lon1 = aLon1;
   double alt1 = aAlt1;
   double lat2 = aLat2;
   double lon2 = aLon2;
   double alt2 = aAlt2;

   if (alt2 < alt1)
   {
      lat1 = aLat2;
      lon1 = aLon2;
      alt1 = aAlt2;
      lat2 = aLat1;
      lon2 = aLon1;
      alt2 = aAlt1;
   }

   // If either point is below the surface then obviously they can't see each other.  Altitudes
   // that are only slightly below the surface are adjusted to be slightly above the surface in
   // order to allow for numerical imprecision.
   //
   // NOTE: The 1st point is checked last so the tile does not have to be reloaded at the
   //       start of the iteration.
   //
   // NOTE: We don't perform the check if a point is above the highest point on
   //       earth.  This is especially important for things like satellites because
   //       performing the check causes a tile to be loaded.  Since satellites
   //       cover cover a lot of ground, this would cause a lot of terrain to get
   //       loaded unnecessarily.

   Terrain terrain(this);
   float   terrainHeightF;

   if (alt2 < cEARTH_HIGH_POINT)
   {
      if (mDataType != cBATHYMETRY)
      {
         terrain.GetElevInterp(lat2, lon2, terrainHeightF);
      }
      else
      {
         terrain.GetBathymetryElevInterp(lat2, lon2, terrainHeightF);
      }
      double heightAboveTerrain = alt2 - (double)terrainHeightF;
      if (heightAboveTerrain < 0.0)
      {
         if (heightAboveTerrain < -1.0)
         {
            return true;
         }
         alt2 = (double)(terrainHeightF + cSLIGHTLY_ABOVE_TERRAIN_F);
      }
   }

   if (alt1 < cEARTH_HIGH_POINT)
   {
      if (mDataType != cBATHYMETRY)
      {
         terrain.GetElevInterp(lat1, lon1, terrainHeightF);
      }
      else
      {
         terrain.GetBathymetryElevInterp(lat1, lon1, terrainHeightF);
      }
      double heightAboveTerrain = alt1 - double(terrainHeightF);
      if (heightAboveTerrain < 0.0)
      {
         if (heightAboveTerrain < -1.0)
         {
            return true;
         }
         alt1 = (double)(terrainHeightF + cSLIGHTLY_ABOVE_TERRAIN_F);
      }
   }

   // There is a small possibility that the height adjustments made above may
   // have caused 'alt1' to become greater than 'alt2'.

   if (alt2 < alt1)
   {
      std::swap(lat1, lat2);
      std::swap(lon1, lon2);
      std::swap(alt1, alt2);
   }

   // If the lowest point is reasonable high above the high point then we assume
   // that terrain really has no effect or is masked by atmospherics.

   if (alt1 > 50000.0)
   {
      return false;
   }

   // Compute the Cartesian coordinates for each point (assuming a spherical earth).

   double wcs1[3];
   double wcs2[3];

   UtSphericalEarth::ConvertLLAToECEF(lat1, lon1, alt1, wcs1);
   UtSphericalEarth::ConvertLLAToECEF(lat2, lon2, alt2, wcs2);

   // It helps to envision a triangle with the following attributes
   //
   // side A : The vector from the center of the earth and to point 1.
   // side B : The vector from point 1 to point 2.  This is the sight line.
   // side C : The vector from the center of the earth to the current point on the sight line.
   // angle A: The angle opposite side A.
   // angle B: The angle opposite side B.
   // angle C: The angle opposite side C.
   //
   // We'll also be making extensive use of the law of sines:
   //
   //     side A         side B         side C
   //  ------------ = ------------ = ------------
   //  sin(angle A)   sin(angle B)   sin(angle C)
   //
   // Given the following notation:
   //
   //   A := angle A;  a := length of side A;
   //   B := angle B;  b := length of side B;
   //   C := angle C;  c := length of side C;
   //
   // The law of cosines can then be used to derive various sides and angles:
   //
   //           b^2 + c^2 - a^2
   //   cos A = ---------------    a^2 = b^2 + c^2 - 2bc cos A
   //                 2bc
   //
   //           c^2 + a^2 - b^2
   //   cos B = ---------------    b^2 = c^2 + a^2 - 2ca cos B
   //                 2ca
   //
   //           a^2 + b^2 - c^2
   //   cos C = ---------------    c^2 = a^2 + b^2 - 2ab cos C
   //                 2ab
   //
   // Note that the following are fixed for a given geometry:
   //
   // a) side A
   // b) angle C

   // Determine the maximum value for angle B.  This is simply the angle that side C will
   // sweep through as it terminus point on the sight line goes from point 1 to point 2.

   double vecSideA[3];
   double vecSideB[3];
   double vecSideC[3];

   UtVec3d::Set(vecSideA, wcs1);
   UtVec3d::Subtract(vecSideB, wcs2, wcs1);
   UtVec3d::Set(vecSideC, wcs2);

   double lenSideA = UtVec3d::Magnitude(vecSideA);
   double lenSideB = UtVec3d::Normalize(vecSideB);
   double lenSideC = UtVec3d::Magnitude(vecSideC);

   // Use law of cosines to compute angle B

   double cosAngleB =
      ((lenSideC * lenSideC) + (lenSideA * lenSideA) - (lenSideB * lenSideB)) / (2.0 * lenSideC * lenSideA);
   cosAngleB     = std::max(-1.0, std::min(cosAngleB, 1.0));
   double angleB = acos(cosAngleB);

   // There is no need to proceed if the points are coincident or if one is directly above the
   // other.  This is done by checking to see if angle B is very small (less than 1 arc-second,
   // which is the DTED level 2 resolution, or the current latitude interval, whichever is smaller).

   static const double oneArcSecond = 4.8481368E-6; // 1.0 / 3600.0 * UtMath::cRAD_PER_DEG;
   double              incAngleB    = oneArcSecond;

   // Select the desired tile type
   GeoElevationTile*& tilePtr = terrain.mTerrainTilePtr;
   if (aTileManager.GetType() == Terrain::cVEGETATION)
   {
      tilePtr = terrain.mVegetationTilePtr;
   }
   else if (aTileManager.GetType() == Terrain::cBATHYMETRY)
   {
      tilePtr = terrain.mBathymetryXYZ_TilePtr;
   }

   if (tilePtr != nullptr)
   {
      incAngleB = std::min(tilePtr->GetLatInterval() * UtMath::cRAD_PER_DEG, oneArcSecond);
   }
   if (angleB < incAngleB)
   {
      return false;
   }

   // Use law of cosines to compute angle C

   double cosAngleC =
      ((lenSideA * lenSideA) + (lenSideB * lenSideB) - (lenSideC * lenSideC)) / (2.0 * lenSideA * lenSideB);
   cosAngleC        = std::max(-1.0, std::min(cosAngleC, 1.0));
   double angleC    = acos(cosAngleC);
   double sinAngleC = sin(angleC);

   // Compute the same parameters for the 'scaled earth' triangle.
   // We have to actually maintain two triangles.  One is used to keep track
   // of the latitude and longitude for DTED lookup and the other is used for
   // the actual line-of-sight check.

   double scaledEarthRadius = aRadiusScale * UtSphericalEarth::cEARTH_RADIUS;
   double lenSideAP         = alt1 + scaledEarthRadius;
   double lenSideCP         = alt2 + scaledEarthRadius;
   double angleBP           = angleB / aRadiusScale;

   // Use the law of cosines to solve for side B

   double lenSideBP_2 = (lenSideCP * lenSideCP) + (lenSideAP * lenSideAP) - (2.0 * lenSideCP * lenSideAP * cos(angleBP));
   double lenSideBP   = sqrt(std::max(lenSideBP_2, 1.0));

   double cosAngleCP =
      ((lenSideAP * lenSideAP) + (lenSideBP * lenSideBP) - (lenSideCP * lenSideCP)) / (2.0 * lenSideAP * lenSideBP);
   cosAngleCP        = std::max(-1.0, std::min(cosAngleCP, 1.0));
   double angleCP    = acos(cosAngleCP);
   double sinAngleCP = sin(angleCP);

   if (mDebugTerrainMasking)
   {
      // Compute angle A

      double cosAngleA =
         ((lenSideB * lenSideB) + (lenSideC * lenSideC) - (lenSideA * lenSideA)) / (2.0 * lenSideB * lenSideC);
      cosAngleA     = std::max(-1.0, std::min(cosAngleA, 1.0));
      double angleA = acos(cosAngleA);
      auto   out    = ut::log::debug() << "Terrain Masking Trig Info:";
      out.AddNote() << "Side A: " << lenSideA;
      out.AddNote() << "Side B: " << lenSideB;
      out.AddNote() << "Side C: " << lenSideC;
      out.AddNote() << "Angle A: " << angleA * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Angle B: " << angleB * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Angle C: " << angleC * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Angle Sum: " << (angleA + angleB + angleC) * UtMath::cDEG_PER_RAD << " deg";
   }

   // Retain the values of the full angle B and the unit vector from point 1 to point 2.

   double maxAngleB = angleB;
   double unitVecSideB[3];
   UtVec3d::Set(unitVecSideB, vecSideB);
#ifndef NDEBUG
   double maxLenSideB = lenSideB;
#endif

   double lastEyeHeight    = alt1;
   double maxTerrainHeight = mMaxTerrainHeight;

   // Determine the initial angle steps

   double lat              = 0.0;
   double lon              = 0.0;
   double latStep          = 0.0;
   double lonStep          = 0.0;
   int    stepsUntilResync = 0;

   double sinAngleB;
   double baseAngleB    = -9999;
   double sinBaseAngleB = 0.0;
   double cosBaseAngleB = 0.0;
   double sinAngleBP;
   double cosAngleBP;
   double sinBaseAngleBP = 0.0;
   double cosBaseAngleBP = 0.0;
   angleB                = incAngleB;
   while (angleB < maxAngleB)
   {
      // Determine if it's time to perform a full recalculation of the sin() and cos()
      // of angleB.  We don't need full double precision accuracy in order to maintain
      // positional accuracy of 0.01 meters.  So what we do is periodically use the
      // math library to get values at full precision and then calculate intermediate
      // values using a 3rd order Taylor series.
      //
      // Experiment has shown that if we force recalculation every 0.5 degrees then
      // the error will never exceed about 2.4E-10.  Objects near the Earth will have
      // coordinate values of about 1.0E+7 (even the Moon is only about 4.0E+8 meters
      // away). So if we multiply the two we'll get position errors far less than 1 meter.

      if ((angleB - baseAngleB) > (0.5 * UtMath::cRAD_PER_DEG))
      {
         // Compute values for unscaled triangle
         sinAngleB     = sin(angleB);
         cosAngleB     = cos(angleB);
         baseAngleB    = angleB;
         sinBaseAngleB = sinAngleB;
         cosBaseAngleB = cosAngleB;
         // Compute values for scaled triangle
         sinAngleBP     = sin(angleB / aRadiusScale);
         cosAngleBP     = cos(angleB / aRadiusScale);
         sinBaseAngleBP = sinAngleBP;
         cosBaseAngleBP = cosAngleBP;
         if (mDebugTerrainMasking)
         {
            ut::log::debug() << "Updating sin/cos values.";
         }
      }
      else
      {
         //                             h^2            h^3             h^n
         // f(x+h) = f(x) + h * f'(x) + --- * f''(x) + --- * f'''(x) + --- * f(n)(x)
         //                              2!             3!              n!
         //
         //        = f(x) + t1 * f'(x) + t2 * f''(x) + t3 * f'''(x)
         //
         // For f(x) = sin(x), f'(x) =  cos(x), f''(x) = -sin(x), f'''(x) = -cos(x)
         //
         //     sin(x+h) = sin(x) + t1 * cos(x) - t2 * sin(x) - t3 * cos(x)
         //              = sin(x) * (1 - t2) + cos(x) * (t1 - t3)
         //
         // For f(x) = cos(x), f'(x) = -sin(x), f''(x) = -cos(x), f'''(x) =  sin(x)
         //
         //     cos(x+h) = cos(x) - t1 * sin(x) - t2 * cos(x) + t3 * sin(x)
         //              = cos(x) * (1 - t2) - sin(x) * (t1 - t3)

         // First do the unscaled triangle
         double h    = angleB - baseAngleB;
         double t1   = h;                            // h^1 / 1!
         double t2   = 0.5 * h * h;                  // h^2 / 2!
         double t3   = 0.33333333333333333 * t2 * h; // h^3 / 3!
         double t0t2 = 1.0 - t2;
         double t1t3 = t1 - t3;
         sinAngleB   = sinBaseAngleB * t0t2 + cosBaseAngleB * t1t3;
         cosAngleB   = cosBaseAngleB * t0t2 - sinBaseAngleB * t1t3;

         // And then do the scaled triangle
         h /= aRadiusScale;
         t1         = h;                            // h^1 / 1!
         t2         = 0.5 * h * h;                  // h^2 / 2!
         t3         = 0.33333333333333333 * t2 * h; // h^3 / 3!
         t0t2       = 1.0 - t2;
         t1t3       = t1 - t3;
         sinAngleBP = sinBaseAngleBP * t0t2 + cosBaseAngleBP * t1t3;
         cosAngleBP = cosBaseAngleBP * t0t2 - sinBaseAngleBP * t1t3;
      }

      // Use law of sines to determine length of side C.

      //   angleA    = UtMath::cPI - angleB - angleC;
      //   sinAngleA = sin(angleA);
      //
      // Can be replaced by noting:
      //
      //   sin(A) = sin(Pi - B - C) = sin((Pi - C) - B)
      //          = sin(X - Y)  where X = Pi - C, Y = B
      //          = sin(X) * cos(Y) - cos(X) * sin(Y)
      //          = sin(Pi - C) * cos(B) - cos(Pi - C) * sin(B);
      //          = sin(C) * cos(B) + cos(C) * sin(B)

      double sinAngleA = sinAngleC * cosAngleB + cosAngleC * sinAngleB;
      lenSideB         = lenSideA * sinAngleB / sinAngleA;

      assert(lenSideB < (maxLenSideB + 1.0));

      // If the caller specified a range at which to stop checking and we've passed
      // that point then we're done...

      if ((aMaxRange > 0.0) && (lenSideB > aMaxRange))
      {
         return false;
      }

      // Compute the position of the intermediate point along the sight vector;

      vecSideB[0] = vecSideA[0] + (unitVecSideB[0] * lenSideB);
      vecSideB[1] = vecSideA[1] + (unitVecSideB[1] * lenSideB);
      vecSideB[2] = vecSideA[2] + (unitVecSideB[2] * lenSideB);

      // Compute the height above the bald earth of the intermediate point along the sight vector.
      // Note that we must used the scaled triangle.

      double sinAngleAP = sinAngleCP * cosAngleBP + cosAngleCP * sinAngleBP;
      lenSideCP         = lenSideAP * sinAngleCP / sinAngleAP;
      double eyeHeight  = lenSideCP - scaledEarthRadius;

      // Compute the latitude and longitude at the new point on the sight line
      // and get the terrain height at that point.
      //
      // Note that we try to avoid full recalculation of the latitude and longitude
      // at each step.  This is expensive (sqrt and 2 atan2's) and can be replaced
      // by a simple increment for short distances.
      //
      // The lat/lon calculations were extracted from UtSphericalEarth::ConvertECEFToLLA.
      // We don't need the altitude here, so it bypasses a sqrt call.

      --stepsUntilResync;
      if (stepsUntilResync >= 0)
      {
         lat += latStep;
         lon += lonStep;
      }
      else
      {
         ConvertWCSToLL(vecSideB, lat, lon);
      }

      if (((mDataType == cBATHYMETRY) && (terrain.GetBathymetryElevInterp(lat, lon, terrainHeightF) != 0)) ||
          ((mDataType != cBATHYMETRY) && (terrain.GetElevInterp(lat, lon, terrainHeightF) != 0)))
      {
         // Force the step sizes to be adjusted and the lat/lon to be fully recomputed
         stepsUntilResync = 0;
      }

      if (mDebugTerrainMasking)
      {
         auto out = ut::log::debug() << "Terrain Masking Position Info:";
         out.AddNote() << "Lat: " << lat << " deg";
         out.AddNote() << "Lon: " << lon << " deg";
         out.AddNote() << "Ground Height: " << terrainHeightF;
         out.AddNote() << "Eye Height: " << eyeHeight;
      }

      if (eyeHeight < double(terrainHeightF))
      {
         // We're masked by the terrain
         maskedByTerrain = true;
         break;
      }
      else if ((eyeHeight > lastEyeHeight) && (eyeHeight > maxTerrainHeight))
      {
         // The altitude on the sight line is increasing and we're above the
         // regional maximum height.... nothing else can block us.
         break;
      }

      // Increment to the next angle.  Recompute the step sizes if a resync is
      // required or a tile switch has occurred.
      if (stepsUntilResync <= 0)
      {
         double latInterval;
         double lonInterval;
         double metersPerDegreeLat;
         double metersPerDegreeLon;
         double resyncInterval;
         GetTileParameters(tilePtr, latInterval, lonInterval, metersPerDegreeLat, metersPerDegreeLon, resyncInterval);

         if (fabs(lat) <= 89.0)
         {
            // Determine angle and WCS position at the end of the resync interval.

            double nextAngleB    = angleB + (resyncInterval / UtSphericalEarth::cEARTH_RADIUS);
            double sinNextAngleB = sin(nextAngleB);
            double cosNextAngleB = cos(nextAngleB);
            sinAngleA            = sinAngleC * cosNextAngleB + cosAngleC * sinNextAngleB;
            lenSideB             = lenSideA * sinNextAngleB / sinAngleA;

            vecSideB[0] = vecSideA[0] + (unitVecSideB[0] * lenSideB);
            vecSideB[1] = vecSideA[1] + (unitVecSideB[1] * lenSideB);
            vecSideB[2] = vecSideA[2] + (unitVecSideB[2] * lenSideB);

            // Determine the delta lat/lon to the end of the resync interval (degrees)

            double deltaLat;
            double deltaLon;
            ConvertWCSToLL(vecSideB, deltaLat, deltaLon);
            deltaLat -= lat;
            deltaLon -= lon;

            // Check for crossing the dateline

            if (deltaLon < -180.0)
            {
               deltaLon = 360.0 + deltaLon; // Eastbound crossing
            }
            else if (deltaLon > 180.0)
            {
               deltaLon = 360.0 - deltaLon; // Westbound crossing
            }

            // Determine the approximate heading (radians)

            double deltaX  = deltaLon * metersPerDegreeLon;
            double deltaY  = deltaLat * metersPerDegreeLat;
            double heading = atan2(deltaX, deltaY);

            // Compute the length in meters of each side and the diagonal (meters).

            double xSide    = lonInterval * metersPerDegreeLon;
            double ySide    = latInterval * metersPerDegreeLat;
            double stepSize = sqrt((xSide * xSide) + (ySide * ySide));

            // Compute the X and Y components of the step based on the heading (meters).

            double xStep = stepSize * sin(heading);
            double yStep = stepSize * cos(heading);

            // Adjust the steps so they are limited to size of the corresponding side

            if (fabs(xStep) > xSide)
            {
               double ratio = fabs(xSide / xStep);
               xStep *= ratio;
               yStep *= ratio;
               stepSize *= ratio;
            }
            if (fabs(yStep) > ySide)
            {
               double ratio = fabs(ySide / yStep);
               xStep *= ratio;
               yStep *= ratio;
               stepSize *= ratio;
            }

            //
            stepSize = std::max(stepSize, mMinAllowableStepSize);

            // Compute the maximum number of steps we'll take between full
            // latitude/longitude updates.

            stepsUntilResync = static_cast<int>((resyncInterval + 0.5 * stepSize) / stepSize);
            stepsUntilResync = std::max(stepsUntilResync, 1);

            // Compute the step sizes needed for incrementing the position.

            incAngleB = (resyncInterval / UtSphericalEarth::cEARTH_RADIUS) / stepsUntilResync;
            latStep   = deltaLat / stepsUntilResync;
            lonStep   = deltaLon / stepsUntilResync;
         }
         else
         {
            // If we're near the pole then we must simply update the lat/lon each
            // and every pass as the above calculations would have problems if we
            // passed over the pole.

            stepsUntilResync = 0;
            incAngleB        = latInterval * UtMath::cRAD_PER_DEG;
            latStep          = 0.0;
            lonStep          = 0.0;
         }

         if (mDebugTerrainMasking)
         {
            auto out = ut::log::debug() << "Terrain Masking Recalculated Step Size:";
            out.AddNote() << "Lat Interval Angle: " << latInterval;
            out.AddNote() << "Lat Interval Distance: " << latInterval * metersPerDegreeLat;
            out.AddNote() << "Lon Interval Angle: " << lonInterval;
            out.AddNote() << "Lon Interval Distance: " << lonInterval * metersPerDegreeLon;
            out.AddNote() << "Resync Interval: " << resyncInterval;
            out.AddNote() << "Inc Angle B: " << incAngleB * UtMath::cDEG_PER_RAD;
            out.AddNote() << "Inc Distance B: " << incAngleB * UtSphericalEarth::cEARTH_RADIUS;
            out.AddNote() << "Lat Step: " << latStep;
            out.AddNote() << "Lon Step: " << lonStep;
            out.AddNote() << "Steps Until Resync: " << stepsUntilResync;
         }
      }

      angleB += incAngleB;
   }
   return maskedByTerrain;
}

// =================================================================================================
// private
//@Note This method applies both to the vegetation layer, if present, and the terrain elevation layer.
bool TerrainInterface::MaskedByTerrainFastP(double                   aLat1,
                                            double                   aLon1,
                                            double                   aAlt1,
                                            double                   aLat2,
                                            double                   aLon2,
                                            double                   aAlt2,
                                            double                   aMaxRange,
                                            GeoElevationTileManager& aTileManager)
{
   bool badData = false;

   // If terrain processing is not enabled then simply make sure the altitudes not below
   if (!mIsEnabled || (mDataType == cVISUAL_DATABASE) || (mDataType == cGEODETIC_EARTH_DATABASE))
   {
      return false;
   }

   GeoElevationTile* currTilePtr        = nullptr;
   float             terrainHeightF     = 0.f;
   double            heightAboveTerrain = 0.0;

   // Arrange the points so the first point is the lowest
   double lat1 = aLat1;
   double lon1 = aLon1;
   double alt1 = aAlt1;
   double lat2 = aLat2;
   double lon2 = aLon2;
   double alt2 = aAlt2;

   if (alt2 < alt1)
   {
      lat1 = aLat2;
      lon1 = aLon2;
      alt1 = aAlt2;
      lat2 = aLat1;
      lon2 = aLon1;
      alt2 = aAlt1;
   }

   currTilePtr = aTileManager.LoadElevationTile(lat2, lon2);
   currTilePtr->GetElevInterp(lat2, lon2, terrainHeightF);
   heightAboveTerrain = alt2 - (double)terrainHeightF;
   if (heightAboveTerrain < 0.0)
   {
      if (heightAboveTerrain < -1.0)
      {
         return true;
      }
      alt2 = (double)(terrainHeightF + cSLIGHTLY_ABOVE_TERRAIN_F);
   }

   double latInterval = currTilePtr->GetLatInterval();

   currTilePtr = aTileManager.LoadElevationTile(lat1, lon1);
   currTilePtr->GetElevInterp(lat1, lon1, terrainHeightF);
   // Check to see if we're over a 'hole' in the DTED data
   if (terrainHeightF < cMIN_ELEV_ALLOWED_F)
   {
      badData = true;
   }

   latInterval = std::min(latInterval, currTilePtr->GetLatInterval());

   heightAboveTerrain = alt1 - double(terrainHeightF);
   if (heightAboveTerrain < 0.0)
   {
      if (heightAboveTerrain < -1.0)
      {
         return true;
      }
      alt1 = (double)(terrainHeightF + cSLIGHTLY_ABOVE_TERRAIN_F);
   }

   // There is a small possibility that the height adjustments made above may
   // have caused 'alt1' to become greater than 'alt2'.
   if (alt2 < alt1)
   {
      std::swap(lat1, lat2);
      std::swap(lon1, lon2);
      std::swap(alt1, alt2);
   }

   // If the lowest point is reasonable high above the high point then we assume
   // that terrain really has no effect or is masked by atmospherics.
   if (alt1 > 50000.0)
   {
      return false;
   }

   // Convert the source and destination positions to ECEF coordinates.
   double posECEF1[3];
   double posECEF2[3];
   UtSphericalEarth::ConvertLLAToECEF(lat1, lon1, alt1, posECEF1);
   UtSphericalEarth::ConvertLLAToECEF(lat2, lon2, alt2, posECEF2);

   // Calculate the unit vector between the source and destination positions.
   double unitSrcToDst[3];
   UtVec3d::Set(unitSrcToDst, posECEF2[0] - posECEF1[0], posECEF2[1] - posECEF1[1], posECEF2[2] - posECEF1[2]);
   const double srcToDstDist = UtVec3d::Magnitude(unitSrcToDst);

   // If we are beyond the specified maximum range, return.
   if ((aMaxRange > 0.0) && (srcToDstDist >= aMaxRange))
   {
      return true;
   }
   UtVec3d::Normalize(unitSrcToDst);

   // Set Up Variables for main loop

   double currPosECEF[3];
   double currLat;
   double currLon;
   double currAlt;
   double stepDist;
   double currDist = 0;

   UtVec3d::Set(currPosECEF, posECEF1);

   // Start of main loop

   while (true)
   {
      if (badData)
      {
         stepDist = 30.0;
         badData  = false;
      }
      else
      {
         static const double maxLatInterval = 1.0 / 3600.0; // One arcsecond == Level 2 DTED (degrees)
         if (latInterval < maxLatInterval)
         {
            // The step distance is less than the ~30m DTED level 2 limit.
            // Use the smaller value in the step distance computation.
            stepDist =
               std::max(heightAboveTerrain / 3.0, latInterval * UtMath::cRAD_PER_DEG * UtSphericalEarth::cEARTH_RADIUS);
         }
         else // Clamp to the one arcsecond (~30m) limit
         {
            stepDist = std::max(heightAboveTerrain / 3.0, 30.0);
         }
      }

      currDist += stepDist;
      if (currDist >= srcToDstDist)
      {
         break;
      }

      currPosECEF[0] += stepDist * unitSrcToDst[0];
      currPosECEF[1] += stepDist * unitSrcToDst[1];
      currPosECEF[2] += stepDist * unitSrcToDst[2];

      UtSphericalEarth::ConvertECEFToLLA(currPosECEF, currLat, currLon, currAlt);

      // Interpolate tempHeight from DTED file data, if we've gone off the
      // map (status != 0) load the new tile and try again.
      if (currTilePtr->GetElevInterp(currLat, currLon, terrainHeightF) != 0)
      {
         currTilePtr = aTileManager.LoadElevationTile(currLat, currLon);
         currTilePtr->GetElevInterp(currLat, currLon, terrainHeightF);
      }

      // We need to account for the 'holes' in the DTED map data.
      // Approximately -400 meters MSL is the lowest point on Earth (Dead Sea shore in Israel)
      // In short: if the calculated height is below the Dead Sea level, we
      // know it's bad data, so don't update terrainHeightF (use the last good
      // data point known).
      if (terrainHeightF < cMIN_ELEV_ALLOWED_F)
      {
         badData = true;
      }
      else
      {
         heightAboveTerrain = currAlt - double(terrainHeightF);
         if (heightAboveTerrain < 0.0)
         {
            return true;
         }
      }
   }
   return false;
}

//  Do not need any more, but leave in until Dave is sure he does not want
//  to list the files.  Use only to strip off the path to the .eld files.
bool TerrainInterface::ListTheFiles(const std::string& aGeodeticEarthFilePathname)
{
   std::string pathName = aGeodeticEarthFilePathname;

   if (aGeodeticEarthFilePathname.find(".eld") != std::string::npos)
   {
      // Allocate one-member name list and put name in name list
      mDTEDPath.push_back(pathName);
   }
   // Input file is a list
   else
   {
      std::ifstream inputFile;
      inputFile.open(pathName, std::ios_base::in);
      if (!inputFile.is_open())
      {
         auto out = ut::log::error() << "Could not open file.";
         out.AddNote() << "File: " << aGeodeticEarthFilePathname;
         return false;
      }
      // Read lines into name list
      else
      {
         std::string stringBuffer;

         while (inputFile >> stringBuffer)
         {
            auto position = stringBuffer.find_last_of("\\");
            if (position != std::string::npos)
            {
               pathName = stringBuffer.substr(position);
               break;
            }

            position = stringBuffer.find_last_of("/");
            if (position != std::string::npos)
            {
               pathName = stringBuffer.substr(position);
               break;
            }

            if (pathName.find('\n') == std::string::npos)
            {
               pathName += '\n';
            }

            mDTEDPath.push_back(pathName);
            break;
         }

         if (inputFile.eof())
         {
            auto out = ut::log::error() << "File is empty.";
            out.AddNote() << "File: " << aGeodeticEarthFilePathname;
            return false;
         }
      }

      inputFile.close();
   }
   return true;
}

//! Check to see if the current tile encloses the provided latitude and longitude.
//! If not load the tile that does.
//! @param aLat Latitude in the range [-90, 90].
//! @param aLon Longitude in the range [-180, 180].
//! @param aTilePtr [output] The tile containing aLat,aLon
//! @return Returns true if a tile was loaded, false if a tile was not loaded.
//! @note This method loads a tile for "bare earth" terrain only, not the vegetation layer.
// protected
bool TerrainInterface::LoadTile(double aLat, double aLon, GeoElevationTile*& aTilePtr)
{
   bool loaded = false;
   if (aTilePtr == nullptr)
   {
      aTilePtr = GetManager().LoadElevationTile(aLat, aLon);
      loaded   = true;
   }
   else if (((aLat < aTilePtr->GetSWLat()) || (aTilePtr->GetNELat() < aLat)) ||
            ((aLon < aTilePtr->GetSWLon()) || (aTilePtr->GetNELon() < aLon)))
   {
      GetManager().UnloadElevationTile(aTilePtr);
      aTilePtr = GetManager().LoadElevationTile(aLat, aLon);
      loaded   = true;
   }
   return loaded;
}

// =================================================================================================
// private
bool TerrainInterface::LoadCME_Terrain()
{
   bool ok                = true;
   mIsDefined             = false;
   int         status     = 0;
   const char* cmeRelBase = mCME_Path.empty() ? ::getenv("CME_REL_BASE") : mCME_Path.c_str();
   if (cmeRelBase != nullptr)
   {
      std::string fileName;

      fileName = cmeRelBase;
      fileName += "/maps/geodata.txt";
      status = LoadCME_File(fileName.c_str());
      if (status < 0) // geodata file not found
      {
         fileName = cmeRelBase;
         fileName += "/maps/geodata.dat";
         status = LoadCME_File(fileName.c_str());
      }
      if (status > 0) // terrain loading error
      {
         ok = false;
      }

      fileName = cmeRelBase;
      fileName += "/site/geodata.txt";
      status = LoadCME_File(fileName.c_str());
      if (status < 0) // geodata file not found
      {
         fileName = cmeRelBase;
         fileName += "/site/geodata.dat";
         status = LoadCME_File(fileName.c_str());
      }
      if (status > 0) // terrain loading error
      {
         ok = false;
      }
   }

   status = LoadCME_File("geodata.txt");
   if (status < 0) // geodata file not found
   {
      status = LoadCME_File("geodata.dat");
   }
   if (status > 0) // terrain error
   {
      ok = false;
   }

   if ((cmeRelBase == nullptr) && // CME_REL_BASE not defined
       (status != 0))             // Local data not loaded
   {
      ok       = false;
      auto out = ut::log::error() << "Undefined environment variable CME_REL_BASE.";
      out.AddNote() << "Variable must be defined when specifying load_cme_terrain.";
   }
   return ok;
}

// =================================================================================================
//! This is a cut down version of '%GeoData::LoadConfigurationFile' from the geodata project.
//! This allows us to read our normal 'geodata.dat' files.  All of the directives except
//! 'dted' and 'raw_dted' are ignored.
//! @returns 0 File open and processed successfully
//!          <0 File name not specified or doesn't exist
//!          >0 terrain data load error.
// private
int TerrainInterface::LoadCME_File(const char* aCfgFile)
{
   int status = -1; // File not specified or could not be opened
   if ((aCfgFile != nullptr) && (aCfgFile[0] != '\0'))
   {
      std::ifstream ifs(aCfgFile, std::ios_base::in);
      if (ifs)
      {
         ut::log::info() << "GeoData Configuration: '" << aCfgFile << "'";
         GetScenario()->GetSystemLog().WriteLogEntry("terrain geodata " + std::string(aCfgFile));
         status = 0;
         std::string cmd;
         while (ifs >> cmd)
         {
            if (cmd == "adrg")
            {
               std::string dirName;
               ReadString(ifs, dirName);
            }
            else if ((cmd == "cadrg") || (cmd == "cib"))
            {
               std::string dirName;
               ReadString(ifs, dirName);
            }
            else if (cmd == "dted")
            {
               int         level;
               std::string dirName;
               ifs >> level;
               ReadString(ifs, dirName);
               if (ifs)
               {
                  if (!AddDtedDirectory(dirName, level))
                  {
                     auto out = ut::log::error() << "Add failed for NIMA DTED directory.";
                     out.AddNote() << "Directory: " << dirName;
                     status = 1;
                  }
               }
            }
            else if ((cmd == "raw-dted") || (cmd == "raw_dted"))
            {
               int         level;
               std::string dirName;
               int         swLat, swLon, neLat, neLon;
               ifs >> level;
               ReadString(ifs, dirName);
               ifs >> swLat >> swLon >> neLat >> neLon;
               if (ifs)
               {
                  if (!AddRawDtedDirectory(dirName, level, swLat, swLon, neLat, neLon))
                  {
                     auto out = ut::log::error() << "Add failed for NIMA DTED directory.";
                     out.AddNote() << "Directory: " << dirName;
                     status = 1;
                  }
               }
            }
            else if ((cmd == "living_earth") || (cmd == "living-earth"))
            {
               std::string title;
               std::string file;
               float       swLat, swLon, neLat, neLon;
               ReadString(ifs, title);
               ReadString(ifs, file);
               ifs >> swLat >> swLon >> neLat >> neLon;
            }
            else if (cmd == "simple")
            {
               std::string title;
               std::string file;
               ReadString(ifs, title);
               ReadString(ifs, file);
            }
            else if ((cmd == "terrain_database") || (cmd == "terrain-database"))
            {
               std::string title;
               ReadString(ifs, title);
               // Ignore the rest... it might be a string or a list.
            }
            else if (cmd == "usermap")
            {
               std::string dirName;
               ReadString(ifs, dirName);
            }
            else if (cmd == "usermapset")
            {
               std::string key;
               std::string dirName;
               ReadString(ifs, key);
               ReadString(ifs, dirName);
               while ((ifs) && (dirName != "end_usermapset"))
               {
                  ReadString(ifs, dirName);
               }
            }
            else if (cmd == "usermaplod")
            {
               std::string dirName;
               ReadString(ifs, dirName);
            }
            else if (cmd == "usermaplodset")
            {
               std::string key;
               std::string dirName;
               ReadString(ifs, key);
               ReadString(ifs, dirName);
               while ((ifs) && (dirName != "end_usermaplodset"))
               {
                  ReadString(ifs, dirName);
               }
            }
            else if ((cmd == "user-map") || (cmd == "user_map"))
            {
               std::string title;
               std::string file;
               ReadString(ifs, title);
               ReadString(ifs, file);
            }
            else if (cmd == "wdb")
            {
               std::string dir;
               ReadString(ifs, dir);
            }
            else if ((cmd == "whole-earth") || (cmd == "whole_earth"))
            {
               std::string title;
               std::string file;
               ReadString(ifs, title);
               ReadString(ifs, file);
            }
            else if (cmd == "xml_block")
            {
               while ((ifs >> cmd) && (cmd != "end_xml_block"))
               {
               }
            }
            else if ((cmd == "ignore_home_geodata") || (cmd == "ignore_site_geodata") || (cmd == "ignore_default_geodata"))
            {
            }
            else if (cmd[0] != '#')
            {
               auto out = ut::log::warning() << "Invalid map configuration command.";
               out.AddNote() << "Command: " << cmd;
               out.AddNote() << "Command ignored.";
            }

            // Once a command has been processed (valid or otherwise), the
            // remainder of the line is skipped.  The allows allows old
            // versions of the code to read new files that have added new
            // options as long as the new options are present on the same
            // line as the last valid parameter.

            ifs.ignore(9999, '\n');
         }
      }
   }
   return status;
}

// =================================================================================================
// Read a string that may be quoted.
// private
void TerrainInterface::ReadString(std::istream& aIn, std::string& aString)
{
   aString = "";

   char endQuote;
   aIn >> endQuote;
   if ((endQuote == '"') || (endQuote == '\''))
   {
      char ch;
      while (aIn.good())
      {
         aIn.get(ch);
         if (ch == endQuote)
         {
            break;
         }
         aString += ch;
      }
   }
   else
   {
      aIn.putback(endQuote);
      aIn >> aString;
   }
}

// =================================================================================================
//! Validate DTED data for correctness.
//! This method examines ALL of the loaded DTED to see if the elevation data is reasonable.
// private
void TerrainInterface::ValidateDTED()
{
   DtedTileManager* managerPtr = dynamic_cast<DtedTileManager*>(mGeoTileManagerPtr.get());
   if (managerPtr == nullptr)
   {
      return;
   }
   ut::log::info() << "Starting DTED validation.";
   DtedTile* tilePtr = new DtedTile();

   // NOTE: The first directory is a dummy 'catch-all' entry - thus we start at 1.
   size_t dirCount = managerPtr->GetDtedDirectoryCount();
   for (size_t dirIndex = 1; dirIndex < dirCount; ++dirIndex)
   {
      DtedDir* dirPtr = managerPtr->GetDtedDirectory(dirIndex);
      { // RAII block
         auto out = ut::log::info() << "Validating DTED directory.";
         out.AddNote() << "Directory: " << dirPtr->GetDirName();
         out.AddNote() << "Level: " << dirPtr->GetLevel();
         out.AddNote() << "From: " << UtLatPos(dirPtr->GetSWLat()) << ", " << UtLonPos(dirPtr->GetSWLon());
         out.AddNote() << "To: " << UtLatPos(dirPtr->GetNELat()) << ", " << UtLonPos(dirPtr->GetNELon());
      }

      unsigned int rowCount = dirPtr->GetNumRows();
      unsigned int colCount = dirPtr->GetNumCols();
      for (unsigned int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
      {
         for (unsigned int colIndex = 0; colIndex < colCount; ++colIndex)
         {
            if (dirPtr->LoadTile(tilePtr, rowIndex, colIndex) == 0)
            {
               unsigned int latCount = tilePtr->GetLatPoints();
               unsigned int lonCount = tilePtr->GetLonPoints();
               for (unsigned int latIndex = 0; latIndex < latCount; ++latIndex)
               {
                  for (unsigned int lonIndex = 0; lonIndex < lonCount; ++lonIndex)
                  {
                     float elev = tilePtr->GetElev(latIndex, lonIndex);
                     if ((elev < cMIN_ELEV_ALLOWED) || (elev > cMAX_ELEV_ALLOWED))
                     {
                        auto out = ut::log::error() << "Suspicious DTED data.";
                        out.AddNote() << "Lat: " << UtLatPos(tilePtr->GetLatOfIndex(latIndex));
                        out.AddNote() << "Lon: " << UtLonPos(tilePtr->GetLonOfIndex(lonIndex));
                        out.AddNote() << "Elevation: " << elev;
                     }
                  }
               }
            }
         }
      }
   }
   ut::log::info() << "DTED validation complete.";

   delete tilePtr;
}

// =================================================================================================
//! Check to see if the indicated directory exists
// private
bool TerrainInterface::DirectoryExists(const std::string& aDirName, const std::string& aCommand)
{
   bool        dirExists = false;
   struct stat statBuf;
   if (stat(aDirName.c_str(), &statBuf) == 0)
   {
      if ((statBuf.st_mode & S_IFDIR) != 0)
      {
         std::string foundSubDirectoryName;
         for (int degrees = 0; degrees <= 180; ++degrees)
         {
            std::stringstream degreeString;
            degreeString << std::setfill('0') << std::setw(3) << degrees;
            std::string subDirectoryEast = aDirName + "/e" + degreeString.str();
            std::string subDirectoryWest = aDirName + "/w" + degreeString.str();

            if (stat(subDirectoryEast.c_str(), &statBuf) == 0)
            {
               foundSubDirectoryName = "e" + degreeString.str();
               dirExists             = true;
               break;
            }
            else if (stat(subDirectoryWest.c_str(), &statBuf) == 0)
            {
               foundSubDirectoryName = "w" + degreeString.str();
               dirExists             = true;
               break;
            }
         }

         if (dirExists)
         {
            auto out = ut::log::info() << "Found sub-directory in terrain directory.";
            out.AddNote() << "In Directory: " << aDirName;
            out.AddNote() << "Sub-Directory: " << foundSubDirectoryName;
            out.AddNote() << "Assuming sub-directory is valid.";
         }
         else
         {
            auto out =
               ut::log::error() << "No 'e<000-180>' or 'w<000-180>' sub-directories exist in terrain directory.";
            out.AddNote() << "Directory: " << aDirName;
         }
      }
      else
      {
         auto out = ut::log::error() << "Terrain directory is not a directory.";
         out.AddNote() << "Directory: " << aDirName;
      }
   }
   else
   {
      auto out = ut::log::error() << " Terrain directory does not exist.";
      out.AddNote() << "Directory: " << aDirName;
   }
   return dirExists;
}

// =================================================================================================
bool TerrainInterface::Initialize()
{
   bool ok = true;

   if (mLoadCME_Terrain)
   {
      if ((!LoadCME_Terrain()) && (!mIgnoreMissingCME_Terrain))
      {
         ut::log::error() << "Unable to load CME terrain.";
         ok = false;
      }
   }

   for (FloatGridInput input : mFloatGridInputs)
   {
      if (!input.mDirName.empty())
      {
         if (!input.mVegetation)
         {
            if (!SetFloatGridDirectory(input.mDirName, input.mSWLat, input.mSWLon, input.mNELat, input.mNELon, GetManager()))
            {
               auto out = ut::log::error() << "Add failed for float-grid (DEM) terrain layer.";
               out.AddNote() << "Directory: " << input.mDirName;
               ok = false;
            }
         }
         else
         {
            if (!SetFloatGridDirectory(input.mDirName,
                                       input.mSWLat,
                                       input.mSWLon,
                                       input.mNELat,
                                       input.mNELon,
                                       GetVegetationManager()))
            {
               auto out = ut::log::error() << "Add failed for float-grid (DEM) vegetation layer.";
               out.AddNote() << "Directory: " << input.mDirName;
               ok = false;
            }
         }
      }
      else
      {
         if (!input.mVegetation)
         {
            if (!SetFloatGridFile(input.mFileName, GetManager()))
            {
               auto out = ut::log::error() << "Add failed for float-grid (DEM) terrain layer.";
               out.AddNote() << "File: " << input.mFileName;
               ok = false;
            }
         }
         else
         {
            if (!SetFloatGridFile(input.mFileName, GetVegetationManager()))
            {
               auto out = ut::log::error() << "Add failed for float-grid (DEM) vegetation layer.";
               out.AddNote() << "File: " << input.mFileName;
               ok = false;
            }
         }
      }
   }

   // GeotiffTileManager data-structures are optimized toward first-in (BSP tree); since the preference here is towards
   // last in, input are processed in reverse
   for (auto input = mGeotiffInputs.rbegin(); input != mGeotiffInputs.rend(); ++input)
   {
      if (!AddGeotiffDirectory(input->mDirName))
      {
         auto out = ut::log::error() << "Add failed for geotiff (DEM) terrain layer.";
         out.AddNote() << "Directory: " << input->mDirName;
         ok = false;
      }
   }

   // BathymetryXYZ_TileManager data-structures are optimized toward first-in (BSP tree); since the preference here is
   // towards last in, input are processed in reverse
   for (auto input = mBathmetryInputs.rbegin(); input != mBathmetryInputs.rend(); ++input)
   {
      if (!AddBathymetry(input->mFileName))
      {
         auto out = ut::log::error() << "Add failed for bathymetry file.";
         out.AddNote() << "File: " << input->mFileName;
         ok = false;
      }
   }

   if (mDTED)
   {
      for (DtedInput input : mDtedInputs)
      {
         if (input.mRaw)
         {
            if (!AddRawDtedDirectory(input.mDirName, input.mLevel, input.mSWLat, input.mSWLon, input.mNELat, input.mNELon))
            {
               auto out = ut::log::error() << "Add failed for NIMA DTED.";
               out.AddNote() << "Directory: " << input.mDirName;
               ok = false;
            }
         }
         else
         {
            if (!AddDtedDirectory(input.mDirName, input.mLevel))
            {
               auto out = ut::log::error() << " Add failed for NIMA DTED.";
               out.AddNote() << "Directory: " << input.mDirName;
               ok = false;
            }
         }
      }
   }

   if (!mVisualDBFile.empty())
   {
      mVisualDB_Ptr = new VisualDatabase;
      if (mVisualDB_Ptr->Initialize(mVisualDBFile.c_str()) < 0)
      {
         auto out = ut::log::error() << "Failed to load visual database.";
         out.AddNote() << "File: " << mVisualDBFile;
         ok = false;
      }
   }

   if (mGeodeticDatabase)
   {
      mGeodeticEarthDB_Ptr = new GeodeticEarthDatabase();
      if (mGeodeticEarthDB_Ptr != nullptr)
      {
         mGeodeticEarthDB_Ptr->SetDatabase(mGeodeticEarthDBName);
         mGeodeticEarthDB_Ptr->SetGeodeticEarthFilePathname(mGeodeticEarthFilePathname);
         mGeodeticEarthDB_Ptr->SetDBType(mGeodeticDatabaseType);

         ok = ListTheFiles(mGeodeticEarthFilePathname);
         if (ok)
         {
            ok = mGeodeticEarthDB_Ptr->OpenTheDatabase();
            if (ok)
            {
               mIsEnabled = mIsDefined = true;
            }
            else
            {
               auto out = ut::log::error() << "Failed to load Geodetic earth database.";
               out.AddNote() << "File: " << mGeodeticEarthDBName;
            }
         }
      }
   }

   if (ok && mValidateDTED)
   {
      ValidateDTED();
   }
   return ok;
}

// =================================================================================================
void TerrainInterface::PerformQueries(WsfSimulation& aSimulation)
{
   for (Query q : mQueryList)
   {
      Terrain terrain(this);
      if (q.mType == Query::cHEIGHT)
      {
         float terrainHeightF;
         auto  out = ut::log::info() << "Terrain Height Query:";
         out.AddNote() << "Lat: " << q.mLat;
         out.AddNote() << "Lon: " << q.mLon;
         terrain.GetElevApprox(q.mLat, q.mLon, terrainHeightF);
         out.AddNote() << "Closest Height: " << terrainHeightF;
         terrain.GetElevInterp(q.mLat, q.mLon, terrainHeightF);
         out.AddNote() << "Interp: " << terrainHeightF << " m (" << terrainHeightF * UtMath::cFT_PER_M << " ft)";
      }
      else if (q.mType == Query::cHORIZON)
      {
         bool maskedByHorizon =
            UtSphericalEarth::MaskedByHorizon(q.mLat, q.mLon, q.mAlt, q.mLat2, q.mLon2, q.mAlt2, q.mEarthRadius);
         auto out = ut::log::info() << "Horizon Masking Query:";
         out.AddNote() << "Lat A: " << q.mLat;
         out.AddNote() << "Lon A: " << q.mLon;
         out.AddNote() << "Alt A: " << q.mAlt;
         out.AddNote() << "Lat B: " << q.mLat2;
         out.AddNote() << "Lon B: " << q.mLon2;
         out.AddNote() << "Alt B: " << q.mAlt2;
         out.AddNote() << "Masked: " << std::boolalpha << maskedByHorizon;
      }
      else if (q.mType == Query::cMASKING)
      {
         auto out = ut::log::info() << "Terrain Masking Query:";
         bool maskedByTerrain =
            terrain.MaskedByTerrain(q.mLat, q.mLon, q.mAlt, q.mLat2, q.mLon2, q.mAlt2, 0.0, q.mEarthRadius);
         out.AddNote() << "Lat A: " << q.mLat;
         out.AddNote() << "Lon A: " << q.mLon;
         out.AddNote() << "Alt A: " << q.mAlt;
         out.AddNote() << "Lat B: " << q.mLat2;
         out.AddNote() << "Lon B: " << q.mLon2;
         out.AddNote() << "Alt B: " << q.mAlt2;
         out.AddNote() << "Masked: " << std::boolalpha << maskedByTerrain;
      }
      else if (q.mType == Query::cTEST_EXTREMA_IN_ZONE)
      {
         double swLat;
         double swLon;
         double neLat;
         double neLon;
         q.mZonePtr->GetLatLonExtrema(swLat, swLon, neLat, neLon);

         Terrain::Post minHeight;
         Terrain::Post maxHeight;
         bool          success = terrain.GetHeightExtremaIn(aSimulation, q.mZonePtr, minHeight, maxHeight);

         auto out = ut::log::info() << "Zone Extrema Test Query:";
         out.AddNote() << "SW Lat: " << swLat;
         out.AddNote() << "SW Lon: " << swLon;
         out.AddNote() << "NE Lat: " << neLat;
         out.AddNote() << "NE Lon: " << neLon;
         out << std::setprecision(6);
         out.AddNote() << "Min: " << minHeight.mHeightM << " m (" << minHeight.mHeightM * UtMath::cFT_PER_M << " ft)";
         out << std::setprecision(17);
         out.AddNote() << "Min at Lat: " << minHeight.mLatDeg;
         out.AddNote() << "Min at Lon: " << minHeight.mLonDeg;
         out << std::setprecision(6);
         out.AddNote() << "Max: " << maxHeight.mHeightM << " m (" << maxHeight.mHeightM * UtMath::cFT_PER_M << " ft)";
         out << std::setprecision(17);
         out.AddNote() << "Max at Lat: " << maxHeight.mLatDeg;
         out.AddNote() << "Max at Lon: " << maxHeight.mLonDeg;
         out.AddNote() << "Success: " << std::boolalpha << success;
      }
      else
      {
         auto out = ut::log::warning() << "Query type not recognized by terrain interface.";
         out.AddNote() << "Type: " << q.mType;
      }
   }
}

TerrainInterface::TerrainInterface(WsfScenario& aScenarioPtr)
   : mScenarioPtr(&aScenarioPtr)
{
}

// =================================================================================================
TerrainInterface::~TerrainInterface()
{
   delete mVisualDB_Ptr;
   delete mGeodeticEarthDB_Ptr;
}

TerrainInterface* TerrainInterface::Get(const WsfScenario& aScenario)
{
   return aScenario.GetTerrainInterface();
}

// =================================================================================================
GeoElevationTileManager& TerrainInterface::GetTileManager()
{
   return GetManager();
}

// =================================================================================================
int TerrainInterface::GetElevApproxP(double                   aLat,
                                     double                   aLon,
                                     float&                   aElev,
                                     GeoElevationTileManager& aTileManager,
                                     GeoElevationTile*&       aTilePtr)
{
   // Return immediately if no terrain is loaded or if terrain processing has been enabled.
   aElev = 0.0f;

   if (((!mIsEnabled) && (aTileManager.GetType() == Terrain::cTERRAIN)) ||
       ((!mIsVegetationEnabled) && (aTileManager.GetType() == Terrain::cVEGETATION)) ||
       ((!mIsBathymetryEnabled) && (aTileManager.GetType() == Terrain::cBATHYMETRY)))
   {
      return 0;
   }

   int   status    = 0;
   float dtedDelta = 0.0f;

   switch (mDataType)
   {
   case cDTED:
   case cFLOAT_GRID:
   case cGEOTIFF:
   case cBATHYMETRY:
   {
      if (aTilePtr == nullptr)
      {
         aTilePtr = aTileManager.LoadElevationTile(aLat, aLon);

         // We should always get a tile - even if it is a 'dummy' tile.
         assert(aTilePtr != nullptr);
      }

      if (aTilePtr->GetElevApprox(aLat, aLon, dtedDelta) != 0)
      {
         aTileManager.UnloadElevationTile(aTilePtr);
         aTilePtr = aTileManager.LoadElevationTile(aLat, aLon);
         // We should always get a tile - even if it is a 'dummy' tile.
         assert(aTilePtr != nullptr);
         aTilePtr->GetElevApprox(aLat, aLon, dtedDelta);
         status = 1; // Indicate a tile switch occurred
      }
      aElev += dtedDelta;
      break;
   }
   case cVISUAL_DATABASE:
   {
      assert(mVisualDB_Ptr != nullptr);
      static double norm[3];
      status = mVisualDB_Ptr->ComputeElev(aLat, aLon, dtedDelta, norm);
      if (dtedDelta == -9999.f)
      {
         dtedDelta = 0.f;
      }
      aElev += dtedDelta;
      break;
   }
   case cGEODETIC_EARTH_DATABASE:
   {
      float elevMeters = 0;
      GetGeodeticElev(aLat, aLon, elevMeters);
      aElev = elevMeters;
   }
   break;
   default:
      break;
   }
   return status;
}

// =================================================================================================
int TerrainInterface::GetElevInterpP(double                   aLat,
                                     double                   aLon,
                                     float&                   aElev,
                                     GeoElevationTileManager& aTileManager,
                                     GeoElevationTile*&       aTilePtr)
{
   // Return immediately if no terrain is loaded or if terrain processing has been enabled.
   aElev = 0.0f;

   if (((!mIsEnabled) && (aTileManager.GetType() == Terrain::cTERRAIN)) ||
       ((!mIsVegetationEnabled) && (aTileManager.GetType() == Terrain::cVEGETATION)) ||
       ((!mIsBathymetryEnabled) && (aTileManager.GetType() == Terrain::cBATHYMETRY)))
   {
      return 0;
   }

   int   status    = 0;
   float dtedDelta = 0.0f;

   switch (mDataType)
   {
   case cDTED:
   case cFLOAT_GRID:
   case cGEOTIFF:
   case cBATHYMETRY:
   {
      if (aTilePtr == nullptr)
      {
         aTilePtr = aTileManager.LoadElevationTile(aLat, aLon);
         // We should always get a tile - even if it is a 'dummy' tile.
         assert(aTilePtr != nullptr);
      }

      if (aTilePtr->GetElevInterp(aLat, aLon, dtedDelta) != 0)
      {
         aTileManager.UnloadElevationTile(aTilePtr);
         aTilePtr = aTileManager.LoadElevationTile(aLat, aLon);
         // We should always get a tile - even if it is a 'dummy' tile.
         assert(aTilePtr != nullptr);
         aTilePtr->GetElevInterp(aLat, aLon, dtedDelta);
         status = 1; // Indicate a tile switch occurred
      }
      aElev += dtedDelta;
      break;
   }
   case cVISUAL_DATABASE:
   {
      assert(mVisualDB_Ptr != nullptr);
      static double norm[3];
      status = mVisualDB_Ptr->ComputeElev(aLat, aLon, dtedDelta, norm);
      if (dtedDelta == -9999.f)
      {
         dtedDelta = 0.f;
      }
      aElev += dtedDelta;
      break;
   }
   case cGEODETIC_EARTH_DATABASE:
   {
      float elevMeters = 0;
      GetGeodeticElev(aLat, aLon, elevMeters);
      aElev = elevMeters;
   }
   break;
   default:
      break;
   }
   return status;
}

// =================================================================================================
//! Get the unit normal vector to the terrain in North-East-Down coordinates.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aNormal [output] The resulting normal vector or [0, 0, -1] if
//! @param aTilePtr [output] The tile containing aLat,aLon
//! there is no terrain at the requested point or if terrain processing has
//! been disabled.
void TerrainInterface::GetNormalNED(double aLat, double aLon, double aNormal[3], GeoElevationTile*& aTilePtr)
{
   // Return immediately if no terrain is loaded or if terrain  processing has been enabled.

   if (!mIsEnabled)
   {
      UtVec3d::Set(aNormal, 0.0, 0.0, -1.0);
      return;
   }

   switch (mDataType)
   {
   case cDTED:
   case cFLOAT_GRID:
   case cGEOTIFF:
   case cBATHYMETRY:
   {
      LoadTile(aLat, aLon, aTilePtr);

      unsigned int latIndex = aTilePtr->GetSWLatIndex(aLat);
      unsigned int lonIndex = aTilePtr->GetSWLonIndex(aLon);
      assert(latIndex < aTilePtr->GetLatPoints());
      assert(lonIndex < aTilePtr->GetLonPoints());

      // Clamp the indices so we always have at least one point above...

      if (latIndex >= (aTilePtr->GetLatPoints() - 1))
      {
         --latIndex;
      }
      if (lonIndex >= (aTilePtr->GetLonPoints() - 1))
      {
         --lonIndex;
      }

      double swElev = aTilePtr->GetElev(latIndex, lonIndex);
      double seElev = aTilePtr->GetElev(latIndex, lonIndex + 1);
      double nwElev = aTilePtr->GetElev(latIndex + 1, lonIndex);
      double neElev = aTilePtr->GetElev(latIndex + 1, lonIndex + 1);

      // Compute the approximate resolution length of the cell.

      double swLat   = aTilePtr->GetLatOfIndex(latIndex);
      double circum  = UtSphericalEarth::cEARTH_RADIUS * 2.0 * UtMath::cPI;
      double latSize = circum * aTilePtr->GetLatInterval() / 360.0;
      double lonSize = cos(swLat * UtMath::cRAD_PER_DEG) * circum * aTilePtr->GetLonInterval() / 360.0;

      // Compute the normal vector.  Remember that +Z is down!!!

      // In the following code, v1 is a vector from the NW corner to the SE corner,
      // and v2 is a vector from the SW corner to the NE corner.
      // Then, when we take the cross product, it's useful to imagine that these
      // two vectors are actually at the same origin.
      double v1[3] = {-latSize, lonSize, (nwElev - seElev)};
      double v2[3] = {-latSize, -lonSize, (neElev - swElev)};
      UtVec3d::CrossProduct(aNormal, v2, v1);
      UtVec3d::Normalize(aNormal);
      break;
   }
   case cVISUAL_DATABASE:
   {
      assert(mVisualDB_Ptr != nullptr);
      float elev = 0.f;
      mVisualDB_Ptr->ComputeElev(aLat, aLon, elev, aNormal);
      if (elev != -9999.f)
      {
         // TODO: figure out why it is inconsistent with normals computed from DTED
         // convert to NED
         double temp = aNormal[0];
         aNormal[0]  = aNormal[1];
         aNormal[1]  = temp;
         aNormal[2]  = -aNormal[2];
      }
      else
      {
         UtVec3d::Set(aNormal, 0.0, 0.0, -1.0);
      }
      break;
   }

   case cGEODETIC_EARTH_DATABASE:
   {
      double normal[3];
      GeodeticComputeNormal(aLat, aLon, normal);
      aNormal[0] = 0.0;  // normal[1];
      aNormal[1] = 0.0;  // normal[0];
      aNormal[2] = -1.0; // normal[2];
   }
   break;
   default:
      break;
   }
}

// =================================================================================================
//! Get the unit normal vector to the terrain in East-North-Up coordinates.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aNormal [output] The resulting normal vector or [0, 0, 1] if
//! @param aTilePtr [output] The tile containing aLat, aLon
//! there is no terrain at the requested point or if terrain processing has
//! been disabled.
void TerrainInterface::GetNormalENU(double aLat, double aLon, double aNormal[3], GeoElevationTile*& aTilePtr)
{
   // Return immediately if no terrain is loaded or if terrain  processing has been enabled.

   if (!mIsEnabled)
   {
      UtVec3d::Set(aNormal, 0.0, 0.0, 1.0);
      return;
   }

   switch (mDataType)
   {
   case cDTED:
   case cFLOAT_GRID:
   case cGEOTIFF:
   case cBATHYMETRY:
   {
      // This will make sure the correct tile is loaded.
      LoadTile(aLat, aLon, aTilePtr);

      unsigned int latIndex = aTilePtr->GetSWLatIndex(aLat);
      unsigned int lonIndex = aTilePtr->GetSWLonIndex(aLon);
      assert(latIndex < aTilePtr->GetLatPoints());
      assert(lonIndex < aTilePtr->GetLonPoints());

      // Clamp the indices so we always have at least one point above...

      if (latIndex >= (aTilePtr->GetLatPoints() - 1))
      {
         --latIndex;
      }
      if (lonIndex >= (aTilePtr->GetLonPoints() - 1))
      {
         --lonIndex;
      }

      double swElev = aTilePtr->GetElev(latIndex, lonIndex);
      double seElev = aTilePtr->GetElev(latIndex, lonIndex + 1);
      double nwElev = aTilePtr->GetElev(latIndex + 1, lonIndex);
      double neElev = aTilePtr->GetElev(latIndex + 1, lonIndex + 1);

      // Compute the approximate resolution length of the cell.

      double swLat   = aTilePtr->GetLatOfIndex(latIndex);
      double circum  = UtSphericalEarth::cEARTH_RADIUS * 2.0 * UtMath::cPI;
      double latSize = circum * aTilePtr->GetLatInterval() / 360.0;
      double lonSize = cos(swLat * UtMath::cRAD_PER_DEG) * circum * aTilePtr->GetLonInterval() / 360.0;

      // Compute the normal vector...

      double v1[3] = {lonSize, latSize, neElev - swElev};
      double v2[3] = {lonSize, -latSize, nwElev - seElev};
      UtVec3d::CrossProduct(aNormal, v1, v2);
      UtVec3d::Normalize(aNormal);
      break;
   }
   case cVISUAL_DATABASE:
   {
      assert(mVisualDB_Ptr != nullptr);
      float elev = 0.f;
      mVisualDB_Ptr->ComputeElev(aLat, aLon, elev, aNormal);
      if (elev == -9999.f)
      {
         UtVec3d::Set(aNormal, 0.0, 0.0, 1.0);
      }
      break;
   }

   case cGEODETIC_EARTH_DATABASE:
   {
      double normal[3];
      GeodeticComputeNormal(aLat, aLon, normal);
      aNormal[0] = 0.0;  // normal[0];
      aNormal[1] = 0.0;  // normal[1];
      aNormal[2] = -1.0; // normal[2];
   }
   break;
   default:
      break;
   }
}

// =================================================================================================
void TerrainInterface::GetGeodeticElev(double aLat, double aLon, float& aElev)
{
   double posNft, posEft;

   if (mGeodeticEarthDB_Ptr != nullptr)
   {
      // GenFeet returns position in feet.
      mGeodeticEarthDB_Ptr->GenFeet(&posNft, &posEft, mGeodeticEarthDB_Ptr->GetDatabase(), aLat, aLon);

      // If valid terrain data, which is set at the beginning when the file is first open
      if (GeodeticEarthElevCompute::ValidTerrainData() != 0)
      {
         float elevationX; // generic units; depends on the database, some databases send in feet
         float elevationY; // generic units; depends on the database, some databases send in meters

         // is the database defined in meters
         if (GeodeticEarthElevCompute::InMeters() != 0)
         {
            elevationX = (float)(UtMath::cM_PER_FT * posEft);
            elevationY = (float)(UtMath::cM_PER_FT * posNft);
         }
         else
         {
            elevationX = (float)posEft;
            elevationY = (float)posNft;
         }
         float elnorm[3];
         float elevationZ;
         if (mGeodeticEarthDB_Ptr->GetDBType() == 1)
         {
            // this database uses feet
            // returns feet
            // x, y inputs are for flat earth databases
            elevationZ = GeodeticEarthElevCompute::elcompute(0, nullptr, elevationX, elevationY, elnorm);
         }
         else
         {
            // all other types use meters; this is a round earth database when using lat/lon inputs
            elevationZ = GeodeticEarthElevCompute::elcompute(0, nullptr, (float)aLon, (float)aLat, elnorm);
         }

         if ((mGeodeticEarthDB_Ptr->GetDBType() == 1) && (elevationZ < 0.0))
         {
            // off the database; limit it to 0.0 altitude
            aElev = 0.0;
         }
         else
         {
            if (GeodeticEarthElevCompute::InMeters() != 0)
            {
               aElev = (float)elevationZ;
            }
            else
            {
               aElev = (float)(elevationZ * UtMath::cM_PER_FT);
            }
         }
      }
      else
      {
         aElev = 0.0;
      }
   }
   else
   {
      aElev = 0.0;
   }
}

// =================================================================================================
//! Is the line of sight between two points masked by the terrain?
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @param aRadiusScale Effective earth radius multiplier (default = 1.0)
//! @return 'true' if masked by terrain or 'false' if not.
bool TerrainInterface::MaskedByTerrain(double aLat1,
                                       double aLon1,
                                       double aAlt1,
                                       double aLat2,
                                       double aLon2,
                                       double aAlt2,
                                       double aMaxRange,
                                       double aRadiusScale /* = 1.0 */)
{
   bool maskedByTerrain(false);

   // At least one altitude is below 0 MSL; if bathymetry is loaded then check
   if ((aAlt1 < 0.0) || (aAlt2 < 0.0))
   {
      maskedByTerrain =
         IsBathymetryEnabled() &&
         MaskedByTerrainP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, GetBathymetryManager(), aRadiusScale);
   }

   // If not blocked check terrain if loaded
   if (!maskedByTerrain)
   {
      maskedByTerrain =
         IsEnabled() && MaskedByTerrainP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, GetManager(), aRadiusScale);
   }

   return maskedByTerrain;
}

// =================================================================================================
//! Is the line of sight between two points masked by the vegetation layer?
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @param aRadiusScale Effective earth radius multiplier (default = 1.0)
//! @return 'true' if masked by vegetation layer or 'false' if not.
// static
bool TerrainInterface::MaskedByVegetation(double aLat1,
                                          double aLon1,
                                          double aAlt1,
                                          double aLat2,
                                          double aLon2,
                                          double aAlt2,
                                          double aMaxRange,
                                          double aRadiusScale /* = 1.0 */)
{
   return IsVegetationEnabled() &&
          MaskedByTerrainP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, GetVegetationManager(), aRadiusScale);
}

// =================================================================================================
//! Is the line of sight between two points masked by the underwater terrain?
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @param aRadiusScale Effective earth radius multiplier (default = 1.0)
//! @return 'true' if masked by terrain or 'false' if not.
bool TerrainInterface::MaskedByUnderwaterTerrain(double aLat1,
                                                 double aLon1,
                                                 double aAlt1,
                                                 double aLat2,
                                                 double aLon2,
                                                 double aAlt2,
                                                 double aMaxRange,
                                                 double aRadiusScale /* = 1.0 */)
{
   return IsBathymetryEnabled() &&
          MaskedByTerrainP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, GetBathymetryManager(), aRadiusScale);
}

// =================================================================================================
void TerrainInterface::GeodeticComputeNormal(double aLat, double aLon, double aNormal[3])
{
   double posNft, posEft;

   // GenFeet returns position in feet.
   mGeodeticEarthDB_Ptr->GenFeet(&posNft, &posEft, mGeodeticEarthDB_Ptr->GetDatabase(), aLat, aLon);

   // If valid terrain data, which is set at the beginning when the file is first open
   if (GeodeticEarthElevCompute::ValidTerrainData() != 0)
   {
      float elevationX; // generic units; depends on the database
      float elevationY; // generic units; depends on the database

      // is the database defined in meters
      if (GeodeticEarthElevCompute::InMeters() != 0)
      {
         elevationX = (float)(UtMath::cM_PER_FT * posEft);
         elevationY = (float)(UtMath::cM_PER_FT * posNft);
      }
      else
      {
         elevationX = (float)posEft;
         elevationY = (float)posNft;
      }
      float elnorm[3];
      // returns feet
      // float elevationZ;
      if (mGeodeticEarthDB_Ptr->GetDBType() == 1)
      {
         /*elevationZ = */ GeodeticEarthElevCompute::elcompute(0, nullptr, elevationX, elevationY, elnorm);
      }
      else
      {
         GeodeticEarthElevCompute::elcompute(0, nullptr, (float)elevationX, (float)elevationY, elnorm);
      }
      aNormal[0] = (double)elnorm[0]; // 0.0
      aNormal[1] = (double)elnorm[1]; // 0.0;
      aNormal[2] = (double)elnorm[2]; //-1.0;
   }
   else
   {
      UtVec3d::Set(aNormal, 0.0, 0.0, -1.0);
   }
}

// =================================================================================================
//! Get the terrain height by interpolating from the bounding sample points.
//!
//! The terrain height is determined by performing bi-linear interpolation
//! using the four corner points of the cell that contains the requested point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled (or if at sea level).
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetElevInterp(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   return mInterfacePtr->GetElevInterpP(aLat, aLon, aElev, mInterfacePtr->GetManager(), mTerrainTilePtr);
}

// =================================================================================================
//! Get the terrain height by interpolating from the bounding sample points.
//!
//! The terrain height is determined by performing bi-linear interpolation
//! using the four corner points of the cell that contains the requested point.
//! @param aWCS WCS location of the point
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled (or if at sea level).
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetElevInterp(const double aWCS[3], float& aElev)
{
   if (!mInterfacePtr->IsEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   double lat, lon;
   ConvertWCSToLL(aWCS, lat, lon);
   return GetElevInterp(lat, lon, aElev);
}

// =================================================================================================
//! Get the vegetation layer height by interpolating from the bounding sample points.
//!
//! The terrain height is determined by performing bi-linear interpolation
//! using the four corner points of the cell that contains the requested point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled (or if at sea level).
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetVegElevInterp(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsVegetationEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   return mInterfacePtr->GetElevInterpP(aLat, aLon, aElev, mInterfacePtr->GetVegetationManager(), mVegetationTilePtr);
}

// =================================================================================================
//! Get the bathymetry depth by interpolating from the bounding sample points.
//!
//! The terrain height is determined by performing bi-linear interpolation
//! using the four corner points of the cell that contains the requested point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain elevation at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled (or if at sea level). Positive is up.
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetBathymetryElevInterp(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsBathymetryEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   // If the tile is a dummy tile then reset pointer
   if (mBathymetryXYZ_TilePtr && mBathymetryXYZ_TilePtr->IsDummyTile())
   {
      mBathymetryXYZ_TilePtr = nullptr;
   }
   return mInterfacePtr->GetElevInterpP(aLat, aLon, aElev, mInterfacePtr->GetBathymetryManager(), mBathymetryXYZ_TilePtr);
}

// =================================================================================================
//! Is the line of sight between two points masked by the terrain?
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @param aRadiusScale Effective earth radius multiplier (default = 1.0)
//! @return 'true' if masked by terrain or 'false' if not.
// static
bool Terrain::MaskedByTerrain(double aLat1,
                              double aLon1,
                              double aAlt1,
                              double aLat2,
                              double aLon2,
                              double aAlt2,
                              double aMaxRange,
                              double aRadiusScale /* = 1.0 */)
{
   bool maskedByTerrain(false);

   // At least one altitude is below 0 MSL; if bathymetry is loaded then check
   if ((aAlt1 < 0.0) || (aAlt2 < 0.0))
   {
      maskedByTerrain = mInterfacePtr && mInterfacePtr->IsBathymetryEnabled() &&
                        mInterfacePtr->MaskedByTerrainP(aLat1,
                                                        aLon1,
                                                        aAlt1,
                                                        aLat2,
                                                        aLon2,
                                                        aAlt2,
                                                        aMaxRange,
                                                        mInterfacePtr->GetBathymetryManager(),
                                                        aRadiusScale);
   }

   // If not blocked check terrain if loaded
   if (!maskedByTerrain)
   {
      maskedByTerrain =
         mInterfacePtr && mInterfacePtr->IsEnabled() &&
         mInterfacePtr->MaskedByTerrainP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, mInterfacePtr->GetManager(), aRadiusScale);
   }

   return maskedByTerrain;
}

// =================================================================================================
//! Is the line of sight between two points masked by the vegetation layer?
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @param aRadiusScale Effective earth radius multiplier (default = 1.0)
//! @return 'true' if masked by vegetation layer or 'false' if not.
// static
bool Terrain::MaskedByVegetation(double aLat1,
                                 double aLon1,
                                 double aAlt1,
                                 double aLat2,
                                 double aLon2,
                                 double aAlt2,
                                 double aMaxRange,
                                 double aRadiusScale /* = 1.0 */)
{
   return mInterfacePtr && mInterfacePtr->IsVegetationEnabled() &&
          mInterfacePtr->MaskedByTerrainP(aLat1,
                                          aLon1,
                                          aAlt1,
                                          aLat2,
                                          aLon2,
                                          aAlt2,
                                          aMaxRange,
                                          mInterfacePtr->GetVegetationManager(),
                                          aRadiusScale);
}

// =================================================================================================
//! Compute height extrema within a polygonal (Lat, Lon)-defined zone.
//! CAUTION:  This function is CPU and memory-intensive, and is NOT recommended
//! for real-time use, due to danger of overframing.
//! @param aSimulation The Simulation
//! @param aZonePtr The zone region to check for extrema.
//! @param aMin Returned (Lat, Lon) and Height of lowest terrain sample.
//! @param aMax Returned (Lat, Lon) and Height of highest terrain sample.
// static
bool Terrain::GetHeightExtremaIn(WsfSimulation& aSimulation, WsfZone* aZonePtr, Post& aMin, Post& aMax)
{
   double southLat, northLat, westLon, eastLon;
   aZonePtr->GetLatLonExtrema(southLat, westLon, northLat, eastLon);
   return GetHeightExtremaIn(aSimulation, southLat, westLon, northLat, eastLon, aMin, aMax, aZonePtr);
}

// =================================================================================================
//! Compute height extrema within a range of (Lat, Lon) tiles.  Optional WsfZoneReference
//! will ignore any terrain outside the zone (if pointer is non-zero).  CAUTION:  This
//! function is CPU and memory-intensive, and is NOT recommended for real-time use, due
//! to danger of overframing!
//! @param aSimulation The Simulation
//! @param aLatSouth Latitude of the south western edge of the rectangle [-90, 90].
//! @param aLonWest Longitude of the south western edge of the rectangle [-180, 180].
//! @param aLatNorth Latitude of the north eastern edge of the rectangle [-90, 90].
//! @param aLonEast Longitude of the south eastern edge of the rectangle [-180, 180].
//! @param aMin Returned (Lat, Lon) and Height of lowest terrain sample.
//! @param aMax Returned (Lat, Lon) and Height of highest terrain sample.
//! @param aZonePtr (Optional) The zone will be checked for IsInside() at (lat, lon) before use of this sample.
// static
bool Terrain::GetHeightExtremaIn(WsfSimulation& aSimulation,
                                 double         aLatSouth,
                                 double         aLonWest,
                                 double         aLatNorth,
                                 double         aLonEast,
                                 Post&          aMin,
                                 Post&          aMax,
                                 WsfZone*       aZonePtr)
{
   if (aLatSouth >= aLatNorth)
   {
      return false;
   }
   // if (aLonWest  >= aLonEast ) return false; // (May cross prime meridian.)

   static const double eyePointNotUsed[]  = {0.0, 0.0, 0.0};
   static const double lookHeadingNotUsed = 0.0;

   // const float cMIN_FLOAT_ELEV_ALLOWED = -500.0; // -418 m near Dead Sea
   // const float cMAX_FLOAT_ELEV_ALLOWED = 8950.0; // Mt Everest is 8848 m

   bool minFound = false;
   bool maxFound = false;

   float maxElevF = cMIN_ELEV_ALLOWED_F; // will start at a minimum, and increase
   float minElevF = cMAX_ELEV_ALLOWED_F; // will start at a maximum, and decrease
   float elevF;

   aMax.mHeightM = cMIN_ELEV_ALLOWED;
   aMin.mHeightM = cMAX_ELEV_ALLOWED;

   // Note:  This function selects a collection of DTED tiles at integer (lat, lon) boundaries
   // which may extend past the floating point values for (lat, lon) supplied, in order to
   // completely encompass desired region.
   WsfDtedRect dtedRect(aLatSouth, aLonWest, aLatNorth, aLonEast);

   std::vector<WsfZone::Point> points;
   points.emplace_back(aLatSouth, aLonWest);
   points.emplace_back(aLatNorth, aLonWest);
   points.emplace_back(aLatNorth, aLonEast);
   points.emplace_back(aLatSouth, aLonEast);
   WsfCodedZone rectZone(points);

   if (aZonePtr == nullptr)
   {
      // Due to integer-boundary issue commented on above, we must create a Zone reference
      // of the actual floating point (lat, lon) rectangle to test against.
      if (rectZone.IsInitialized())
      {
         aZonePtr = &rectZone;
      }
   }

   std::vector<double> postLatitudes;
   std::vector<double> postLongitudes;

   double latLLCorner;
   double lonLLCorner;

   double locWCS[3];

   double lat;
   double lon;

   GeoElevationTile* terrainTilePtr;

   if (mInterfacePtr->mDebugTerrainExtrema)
   {
      auto out = ut::log::debug() << "Querying elevation extrema for LL rectangle.";
      out << std::setprecision(16);
      out.AddNote() << "SW Lat: " << dtedRect.GetSWLat();
      out.AddNote() << "SW Lon: " << dtedRect.GetSWLon();
      out.AddNote() << "NE Lat: " << dtedRect.GetNELat();
      out.AddNote() << "NE Lon: " << dtedRect.GetNELon();
   }

   for (unsigned int tileIndex = 0; tileIndex < dtedRect.GetTileCount(); ++tileIndex)
   {
      bool success = dtedRect.GetTile(tileIndex, latLLCorner, lonLLCorner);
      if (success)
      {
         terrainTilePtr = GetManager().LoadElevationTile(latLLCorner + 0.5, lonLLCorner + 0.5);

         // This test is faulty if all we do is deal with dummy data.
         if (!terrainTilePtr->IsDummyTile())
         {
            // (Do not consider a dummy tile for extrema.)

            // The GetLatIndex() and GetLonIndex() functions do not check to assure
            // that the arguments actually are inside the boundaries of the tile.
            // We must do that here.

            double       minLat        = std::max(aLatSouth, terrainTilePtr->GetLatOfIndex(0));
            unsigned int minLatIndex   = terrainTilePtr->GetLatIndex(minLat);
            unsigned int maxLatIndex   = terrainTilePtr->GetLatIndex(aLatNorth);
            unsigned int latLimitIndex = minLatIndex + terrainTilePtr->GetLatPoints();
            if (maxLatIndex > latLimitIndex)
            {
               maxLatIndex = latLimitIndex;
            }

            double       minLon        = std::max(aLonWest, terrainTilePtr->GetLonOfIndex(0));
            unsigned int minLonIndex   = terrainTilePtr->GetLonIndex(minLon);
            unsigned int maxLonIndex   = terrainTilePtr->GetLonIndex(aLonEast);
            unsigned int lonLimitIndex = minLonIndex + terrainTilePtr->GetLonPoints();
            if (maxLonIndex > lonLimitIndex)
            {
               maxLonIndex = lonLimitIndex;
            }


            for (unsigned int lonIndex = minLonIndex; lonIndex <= maxLonIndex; ++lonIndex)
            {
               lon = terrainTilePtr->GetLonOfIndex(lonIndex);
               AddToVector(lon, postLongitudes);

               for (unsigned int latIndex = minLatIndex; latIndex <= maxLatIndex; ++latIndex)
               {
                  lat = terrainTilePtr->GetLatOfIndex(latIndex);
                  AddToVector(lat, postLatitudes);

                  if (terrainTilePtr->Contains(lat, lon))
                  {
                     elevF = terrainTilePtr->GetElev(latIndex, lonIndex);

                     // Remove NULL elevation values.
                     if (elevF == -32767.f)
                     {
                        // Ignore.
                     }
                     else
                     {
                        bool usePoint = true;

                        if (aZonePtr != nullptr)
                        {
                           UtEntity::ConvertLLAToWCS(lat, lon, 0.0, locWCS);
                           if (!aZonePtr->PointIsInside(&aSimulation, locWCS, eyePointNotUsed, lookHeadingNotUsed))
                           {
                              usePoint = false;
                           }
                        }

                        if (usePoint)
                        {
                           if (elevF > maxElevF)
                           {
                              maxFound      = true;
                              maxElevF      = elevF;
                              aMax.mHeightM = (double)elevF;
                              aMax.mLatDeg  = lat;
                              aMax.mLonDeg  = lon;
                              if (mInterfacePtr->mDebugTerrainExtrema)
                              {
                                 // Previously the lonIndex and latIndex were also printed.
                                 // They are excluded due to of lack of relevance to the user.
                                 // - Reuben French
                                 auto out = ut::log::debug() << "Rises to:";
                                 out.AddNote() << "Max Height: " << aMax.mHeightM << " m ("
                                               << static_cast<int>(M2F(aMax.mHeightM)) << " ft)";
                                 out.AddNote() << "Lat: " << lat;
                                 out.AddNote() << "Lon: " << lon;
                              }
                           }

                           if (elevF < minElevF)
                           {
                              if (elevF <= cMIN_ELEV_ALLOWED_F)
                              {
                                 // ignore
                              }
                              else
                              {
                                 minFound      = true;
                                 minElevF      = elevF;
                                 aMin.mHeightM = (double)elevF;
                                 aMin.mLatDeg  = lat;
                                 aMin.mLonDeg  = lon;
                                 if (mInterfacePtr->mDebugTerrainExtrema)
                                 {
                                    // Previously the lonIndex and latIndex were also printed.
                                    // They are excluded due to of lack of relevance to the user.
                                    // - Reuben French
                                    auto out = ut::log::debug() << "Sinks to:";
                                    out.AddNote() << "Min Height: " << aMin.mHeightM << " m ("
                                                  << static_cast<int>(M2F(aMin.mHeightM)) << " ft)";
                                    out.AddNote() << "Lat: " << lat;
                                    out.AddNote() << "Lon: " << lon;
                                 }
                              }
                           }
                        }
                     } // end if elevation result bogus
                  }    // end if containsResult
               }       // end for latIndex
            }          // end for lonIndex
         }             // end if isDummyTile
         GetManager().UnloadElevationTile(terrainTilePtr);
      }
   }

   // So far all we got were the interior points.  Now compute the edge/boundary conditions.
   GetEdgeExtremaInZone(aZonePtr, postLatitudes, postLongitudes, minFound, maxFound, minElevF, maxElevF, aMin, aMax);

   return minFound && maxFound;
}

// =================================================================================================
//! Compute height extrema within the EDGES of a polygonal zone of (Lat, Lon) vertices.
//! Function will consider only edge conditions, not internal terrain posts.
//! CAUTION:  This function is CPU and memory-intensive, and is not recommended
//! for real-time use, due to danger of over-framing!
//! @param aZonePtr The Polygonal zone of clockwise (Lat, Lon) vertices to be queried for terrain extrema.
//! @param aMinFound A boolean indicating whether a minimum was found or not (only modified if a Post was lowered).
//! @param aMaxFound A boolean indicating whether a maximum was found or not (only modified if a Post was raised).
//! @param aMin Returned (Lat, Lon) and Height of lowest terrain sample (only modified if lowered).
//! @param aMax Returned (Lat, Lon) and Height of highest terrain sample (only modified if raised).
void Terrain::GetEdgeExtremaInZone(WsfZone*                   aZonePtr,
                                   const std::vector<double>& aPostLatitudes,
                                   const std::vector<double>& aPostLongitudes,
                                   bool&                      aMinFound,
                                   bool&                      aMaxFound,
                                   float&                     aMinElevF,
                                   float&                     aMaxElevF,
                                   Post&                      aMin,
                                   Post&                      aMax)
{
   std::vector<double> sortedLats = aPostLatitudes;
   std::sort(sortedLats.begin(), sortedLats.end());

   std::vector<double> sortedLons = aPostLongitudes;
   std::sort(sortedLons.begin(), sortedLons.end());

   // Traverse each vertex of the zone, and for each vertex pair,
   // check the edge values of any post boundaries this line
   // segment crosses over.
   double startLat = 0.0;
   double startLon = 0.0;
   double endLat   = 0.0;
   double endLon   = 0.0;

   WsfZoneDefinition*                 zdPtr  = dynamic_cast<WsfZoneDefinition*>(aZonePtr);
   const std::vector<WsfZone::Point>& ptsRef = zdPtr->GetPoints();

   // The indexing limit is expanded by one, to allow for wrap-around of the closed zone.
   for (unsigned int iPoint = 0; iPoint != ptsRef.size() + 1; ++iPoint)
   {
      if (iPoint == ptsRef.size())
      {
         endLat = ptsRef[0].mX;
         endLon = ptsRef[0].mY;
      }
      else
      {
         endLat = ptsRef[iPoint].mX;
         endLon = ptsRef[iPoint].mY;
      }

      if (iPoint > 0)
      {
         // We have a starting (lat, lon), and an ending one.
         // Marry the endpoints to a corresponding segment of the "boundary" latitudes and longitudes
         bool interpOnLat = startLat != endLat;
         bool interpOnLon = startLon != endLon;
         assert(interpOnLat || interpOnLon);

         // Now traverse the (lat, lon) line segment from one end to the other, picking off
         // each and every post of interest.
         std::vector<double> segmentInterpolants;

         if (interpOnLat)
         {
            std::vector<double> localLats;
            CreateLocalsFromBoundaries(sortedLats, startLat, endLat, localLats);

            for (unsigned int localLatIndex = 0; localLatIndex != localLats.size(); ++localLatIndex)
            {
               double val   = localLats[localLatIndex];
               double delta = endLat - startLat;
               if (fabs(delta) > 0.00001)
               {
                  double interpolant = (val - startLat) / delta;
                  AddToVector(interpolant, segmentInterpolants);
               }
            }
         }

         if (interpOnLon)
         {
            std::vector<double> localLons;
            CreateLocalsFromBoundaries(sortedLons, startLon, endLon, localLons);

            for (unsigned int localLonIndex = 0; localLonIndex != localLons.size(); ++localLonIndex)
            {
               double val   = localLons[localLonIndex];
               double delta = endLon - startLon;
               if (fabs(delta) > 0.00001)
               {
                  double interpolant = (val - startLon) / (endLon - startLon);
                  AddToVector(interpolant, segmentInterpolants);
               }
            }
         }

         sort(segmentInterpolants.begin(), segmentInterpolants.end());

         // Now traverse the segment in question from one end to the other, along each
         // interpolated (lat, lon) point, and get the interpolated altitude.
         Terrain instance(*this);

         for (unsigned int iInterpolant = 0; iInterpolant != segmentInterpolants.size(); ++iInterpolant)
         {
            double interp = segmentInterpolants[iInterpolant];
            if ((interp >= 0.0) && (interp <= 1.0))
            {
               double pointLat = startLat + interp * (endLat - startLat);
               double pointLon = startLon + interp * (endLon - startLon);

               //  If the height value is beyond current extrema,
               // then push the max post up, or lower the min post down:

               float elevF;
               instance.GetElevInterp(pointLat, pointLon, elevF);
               if (elevF != -32767.f)
               {
                  if (elevF > aMaxElevF)
                  {
                     auto out = ut::log::debug() << "Edge rising:";
                     if (mInterfacePtr->mDebugTerrainExtrema)
                     {
                        auto from = out.AddNote() << "From:";
                        from.AddNote() << "Height: " << aMax.mHeightM << " m (" << static_cast<int>(M2F(aMax.mHeightM))
                                       << " ft)";
                        from.AddNote() << "Lat: " << aMax.mLatDeg;
                        from.AddNote() << "Lon: " << aMax.mLonDeg;
                     }
                     aMaxFound     = true;
                     aMaxElevF     = elevF;
                     aMax.mHeightM = (double)elevF;
                     aMax.mLatDeg  = pointLat;
                     aMax.mLonDeg  = pointLon;
                     if (mInterfacePtr->mDebugTerrainExtrema)
                     {
                        auto to = out.AddNote() << "To:";
                        to.AddNote() << "Height: " << aMax.mHeightM << " m (" << static_cast<int>(M2F(aMax.mHeightM))
                                     << " ft)";
                        to.AddNote() << "Lat: " << aMax.mLatDeg;
                        to.AddNote() << "Lon: " << aMax.mLonDeg;
                     }
                     if (!mInterfacePtr->mDebugTerrainExtrema)
                     {
                        out.Clear();
                     }
                  }

                  if (elevF < aMinElevF)
                  {
                     if (elevF <= cMIN_ELEV_ALLOWED_F)
                     {
                        // ignore
                     }
                     else
                     {
                        auto out = ut::log::debug() << "Edge sinking:";
                        if (mInterfacePtr->mDebugTerrainExtrema)
                        {
                           auto from = out.AddNote() << "From:";
                           from.AddNote() << "Height: " << aMin.mHeightM << " m ("
                                          << static_cast<int>(M2F(aMin.mHeightM)) << " ft)";
                           from.AddNote() << "Lat: " << aMin.mLatDeg;
                           from.AddNote() << "Lon: " << aMin.mLonDeg;
                        }
                        aMinFound     = true;
                        aMinElevF     = elevF;
                        aMin.mHeightM = (double)elevF;
                        aMin.mLatDeg  = pointLat;
                        aMin.mLonDeg  = pointLon;
                        if (mInterfacePtr->mDebugTerrainExtrema)
                        {
                           auto to = out.AddNote() << "To:";
                           to.AddNote() << "Height: " << aMin.mHeightM << " m (" << static_cast<int>(M2F(aMin.mHeightM))
                                        << " ft)";
                           to.AddNote() << "Lat: " << aMin.mLatDeg;
                           to.AddNote() << "Lon: " << aMin.mLonDeg;
                        }
                        if (!mInterfacePtr->mDebugTerrainExtrema)
                        {
                           out.Clear();
                        }
                     }
                  }
               }
            }
         }
      }
      startLat = endLat;
      startLon = endLon;
   }
}

// =================================================================================================
// ===============================================================================
// static
void Terrain::CreateLocalsFromBoundaries(std::vector<double>  aSortedValues,
                                         double               aStartValue,
                                         double               aEndValue,
                                         std::vector<double>& aLocalValues)
{
   // Traverse the aSortedValues array, and pick off all the aLocalValues
   // within the range of (start, end), plus ONE value outside the range.

   double startValue = aStartValue;
   double endValue   = aEndValue;
   if (startValue > endValue)
   {
      startValue = aEndValue;
      endValue   = aStartValue;
   }

   bool inRange = false;

   double pastValue = 0.0;

   auto iter = aSortedValues.begin();
   while (iter != aSortedValues.end())
   {
      double thisValue = *iter;
      if ((thisValue > startValue) && !inRange)
      {
         inRange = true;
         aLocalValues.push_back(pastValue);
      }
      else if (thisValue >= endValue)
      {
         assert(inRange);
         aLocalValues.push_back(thisValue);
         break;
      }

      if (inRange)
      {
         aLocalValues.push_back(thisValue);
      }

      pastValue = thisValue;
      ++iter;
   }
}

// ===============================================================================
//! Is the line of sight between two points masked by the terrain?
//! This version varies the step size with height above terrain, but
//! is only valid for an earth scale radius of 1.0.
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @return 'true' if masked by terrain or 'false' if not.
// static
bool Terrain::MaskedByTerrainFast(double aLat1, double aLon1, double aAlt1, double aLat2, double aLon2, double aAlt2, double aMaxRange)
{
   return mInterfacePtr &&
          mInterfacePtr->MaskedByTerrainFastP(aLat1, aLon1, aAlt1, aLat2, aLon2, aAlt2, aMaxRange, mInterfacePtr->GetManager());
}

// =================================================================================================
//! Is the line of sight between two points masked by the vegetation?
//! This version varies the step size with height above vegetation, but
//! is only valid for an earth scale radius of 1.0.
//!
//! @param aLat1 Latitude of the first point [-90, 90]
//! @param aLon1 Longitude of the first point [-180, 180]
//! @param aAlt1 Altitude MSL of the first point.
//! @param aLat2 Latitude of the second point [-90, 90]
//! @param aLon2 Longitude of the second point [-180, 180]
//! @param aAlt2 Altitude MSL of the second point.
//! @param aMaxRange Distance at which to stop checking.
//! @return 'true' if masked by vegetation or 'false' if not.
// static
bool Terrain::MaskedByVegetationFast(double aLat1, double aLon1, double aAlt1, double aLat2, double aLon2, double aAlt2, double aMaxRange)
{
   return mInterfacePtr && mInterfacePtr->MaskedByTerrainFastP(aLat1,
                                                               aLon1,
                                                               aAlt1,
                                                               aLat2,
                                                               aLon2,
                                                               aAlt2,
                                                               aMaxRange,
                                                               mInterfacePtr->GetVegetationManager());
}

// =================================================================================================
//! Get the terrain "pixel" along a line-of-sight ray.  This algorithm is a brute force approach, and
//! serves as a placeholder for a more elegant solution to come later (?).  This will be a CPU-
//! intensive operation.  Algorithm will march outward from eyepoint to terminus, so please limit the
//! maximum range as much as is possible.  Does not include possible cloud occulting, use the
//! WsfEnvironment for that, prior to using this function.
//! @param aEyepointWCS  Eyepoint of the beginning of the LOS segment.
//! @param aTerminusWCS  Endpoint of the LOS segment.  Will be modified only if return value is true.
//! @return 'true' If aTerminusWCS was moved closer to aEyepointWCS, to place at terrain intersect.
bool Terrain::TerrainHitAlongLOS(const double aEyepointWCS[3], double aTerminusWCS[3])
{
   double losUnit[3];
   UtVec3d::Subtract(losUnit, aTerminusWCS, aEyepointWCS);
   assert(UtVec3d::MagnitudeSquared(losUnit) > 4.0);
   double losMaxDist = UtVec3d::Normalize(losUnit);

   double latA, lonA, altA, latB, lonB, altB;
   UtEntity::ConvertWCSToLLA(aEyepointWCS, latA, lonA, altA);
   UtEntity::ConvertWCSToLLA(aTerminusWCS, latB, lonB, altB);

   if (!MaskedByTerrain(latA, lonA, altA, latB, lonB, altB, losMaxDist))
   {
      return false;
   }

   // (Implementation below was copied from WsfSAR_Sensor::SAR_Mode::ComputeImageCenter.)
   double eyePointWCS[3];
   UtVec3d::Set(eyePointWCS, aEyepointWCS);
   bool impacted = ComputeTerrainImpactP(eyePointWCS, aTerminusWCS, true);
   return impacted;
}

// =================================================================================================
// ================================================================================================
//! Computes terrain impact between two LOS points (Recursive).
//! @param aLocationWCS_1 WCS location of point 1
//! @param aLocationWCS_2 WCS location of point 2
//! @param aFirstPass     Set to true if first pass (default = false)
//! @return 'true' if image center calculation is complete or 'false' if not.
bool Terrain::ComputeTerrainImpactP(double aLocationWCS_1[3], double aLocationWCS_2[3], bool aFirstPass)
{
   // Get slant range squared
   double deltaLocWCS[3] = {0.0};
   UtVec3d::Subtract(deltaLocWCS, aLocationWCS_2, aLocationWCS_1);
   double slantRangeSquared = UtVec3d::MagnitudeSquared(deltaLocWCS);

   // Check for termination
   if (slantRangeSquared < 100.0)
   {
      return true;
   }

   // Masked by terrain check
   double lat1, lon1, alt1;
   UtEntity::ConvertWCSToLLA(aLocationWCS_1, lat1, lon1, alt1);
   double lat2, lon2, alt2;
   UtEntity::ConvertWCSToLLA(aLocationWCS_2, lat2, lon2, alt2);

   if (Terrain::MaskedByTerrain(lat1, lon1, alt1, lat2, lon2, alt2, 0.0))
   {
      // Move b = midpoint
      double temp[3] = {0.0};
      UtVec3d::Multiply(temp, deltaLocWCS, 0.5);
      UtVec3d::Add(aLocationWCS_2, aLocationWCS_1, temp);
   }
   else
   {
      if (aFirstPass)
      {
         return true;
      }

      // b
      double temp[3] = {0.0};
      UtVec3d::Multiply(temp, deltaLocWCS, 1.5);
      UtVec3d::Add(aLocationWCS_2, aLocationWCS_1, temp);

      // a
      UtVec3d::Add(aLocationWCS_1, aLocationWCS_1, deltaLocWCS);
   }
   return ComputeTerrainImpactP(aLocationWCS_1, aLocationWCS_2);
}

// =================================================================================================
Terrain::Terrain(TerrainInterface* aInterfacePtr)
   : mTerrainTilePtr(nullptr)
   , mVegetationTilePtr(nullptr)
   , mBathymetryXYZ_TilePtr(nullptr)
   , mInterfacePtr(aInterfacePtr)
{
   // Initialization of terrain manager deferred until
   // first call of GetManager(), as ProcessInput may change
   // the type of manager created.
}

// =================================================================================================
//! Copy constructor.
//! If the source object has a tile loaded then the new
//! object will load the same tile.
Terrain::Terrain(const Terrain& aSrc)
   : mTerrainTilePtr(nullptr)
   , mVegetationTilePtr(nullptr)
   , mBathymetryXYZ_TilePtr(nullptr)
   , mInterfacePtr(aSrc.mInterfacePtr)
{
   // is this necessary?  Doesn't seem to be called.
   //    if (aSrc.mTerrainTilePtr != nullptr)
   //    {
   //       // Create a new reference to the tile.
   //       GeoElevationTile* tilePtr = aSrc.mTerrainTilePtr;
   //       double lat = 0.5 * (tilePtr->GetSWLat() + tilePtr->GetNELat());
   //       double lon = 0.5 * (tilePtr->GetSWLon() + tilePtr->GetNELon());
   //       mTerrainTilePtr = GetManager().LoadElevationTile(lat, lon);
   //    }
   //    if (aSrc.mVegetationTilePtr != nullptr)
   //    {
   //       // Create a new reference to the tile.
   //       GeoElevationTile* tilePtr = aSrc.mVegetationTilePtr;
   //       double lat = 0.5 * (tilePtr->GetSWLat() + tilePtr->GetNELat());
   //       double lon = 0.5 * (tilePtr->GetSWLon() + tilePtr->GetNELon());
   //       mVegetationTilePtr = GetManager().LoadElevationTile(lat, lon);
   //    }
}

// =================================================================================================
//! Delete a terrain object.
//!
//! The current tile (if any) is unloaded.
Terrain::~Terrain()
{
   // See programming note at the top of the file.
   if (mInterfacePtr != nullptr)
   {
      mInterfacePtr->GetManager().UnloadElevationTile(mTerrainTilePtr);
   }

   if (mVegetationTilePtr != nullptr)
   {
      mInterfacePtr->GetVegetationManager().UnloadElevationTile(mVegetationTilePtr);
   }

   if (mBathymetryXYZ_TilePtr != nullptr)
   {
      mInterfacePtr->GetBathymetryManager().UnloadElevationTile(mBathymetryXYZ_TilePtr);
   }
}

//
// // =================================================================================================
// //! Assignment operator.
// //!
// //! If the right hand side object has a tile loaded then the left hand
// //! side object will load the same tile.
// Terrain& Terrain::operator=(const Terrain& aRhs)
// {
//    if (this != &aRhs)
//    {
//       mTerrainTilePtr = 0;
//       if (aRhs.mTerrainTilePtr != nullptr)
//       {
//          // Create a new reference to the tile.
//          GeoElevationTile* tilePtr = aRhs.mTerrainTilePtr;
//          if (dynamic_cast<DtedTile*>(tilePtr) != nullptr)
//          {
//             sDataType = cDTED;
//          }
//          else
//          {
//             sDataType = cFLOAT_GRID;
//          }
//          double lat = 0.5 * (tilePtr->GetSWLat() + tilePtr->GetNELat());
//          double lon = 0.5 * (tilePtr->GetSWLon() + tilePtr->GetNELon());
//          mTerrainTilePtr = GetManager().LoadElevationTile(lat, lon);
//       }
//       mVegetationTilePtr = 0;
//       if (aRhs.mVegetationTilePtr != nullptr)
//       {
//          // Create a new reference to the tile.
//          GeoElevationTile* tilePtr = aRhs.mVegetationTilePtr;
//          if (dynamic_cast<DtedTile*>(tilePtr) != nullptr)
//          {
//             sDataType = cDTED;
//          }
//          else
//          {
//             sDataType = cFLOAT_GRID;
//          }
//          double lat = 0.5 * (tilePtr->GetSWLat() + tilePtr->GetNELat());
//          double lon = 0.5 * (tilePtr->GetSWLon() + tilePtr->GetNELon());
//          mVegetationTilePtr = GetManager().LoadElevationTile(lat, lon);
//       }
//    }
//    return *this;
// }

// =================================================================================================
//! Get the terrain height at the closest sample point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled.
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetElevApprox(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsEnabled())
   {
      aElev = 0.0f;
      return 0;
   }
   return mInterfacePtr->GetElevApproxP(aLat, aLon, aElev, mInterfacePtr->GetManager(), mTerrainTilePtr);
}

// =================================================================================================
//! Get the terrain height at the closest sample point.
//! @param aWCS WCS location of the point
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled.
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetElevApprox(const double aWCS[3], float& aElev)
{
   if (!mInterfacePtr->IsEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   double lat, lon;
   ConvertWCSToLL(aWCS, lat, lon);
   return GetElevApprox(lat, lon, aElev);
}

// =================================================================================================
//! Get the vegetation layer height at the closest sample point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain height at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled.
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetVegElevApprox(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsVegetationEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   return mInterfacePtr->GetElevApproxP(aLat, aLon, aElev, mInterfacePtr->GetVegetationManager(), mVegetationTilePtr);
}

// =================================================================================================
//! Get the bathymetry depth at the closest sample point.
//! @param aLat Latitude in the range [-90, 90]
//! @param aLon Longitude in the range [-180, 180]
//! @param aElev [output] The terrain elevation at the point. This will be
//! 0 if there is no terrain in for the requested point or if terrain
//! processing has been disabled. Positive is up.
//! @return 0 if the requested location is in the current tile (or if there
//! is no terrain) or non-zero if a tile switch was required.
int Terrain::GetBathymetryElevApprox(double aLat, double aLon, float& aElev)
{
   if (!mInterfacePtr->IsBathymetryEnabled())
   {
      aElev = 0.0;
      return 0;
   }
   // If the tile is a dummy tile then reset pointer
   if (mBathymetryXYZ_TilePtr && mBathymetryXYZ_TilePtr->IsDummyTile())
   {
      mBathymetryXYZ_TilePtr = nullptr;
   }
   return mInterfacePtr->GetElevApproxP(aLat, aLon, aElev, mInterfacePtr->GetBathymetryManager(), mBathymetryXYZ_TilePtr);
}

// =================================================================================================
bool Terrain::Initialize(WsfSimulation& aSimulation)
{
   return Initialize(aSimulation.GetTerrainInterface());
}

// =================================================================================================
bool Terrain::Initialize(TerrainInterface* aInterfacePtr)
{
   mInterfacePtr = aInterfacePtr;
   return true;
}

// =================================================================================================
GeoElevationTileManager& Terrain::GetManager()
{
   return mInterfacePtr->GetManager();
}

} // namespace wsf
