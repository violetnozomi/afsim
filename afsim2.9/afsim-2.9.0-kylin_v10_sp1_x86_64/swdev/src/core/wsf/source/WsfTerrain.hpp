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

#ifndef WSFTERRAIN_HPP
#define WSFTERRAIN_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>
#include <vector>

#include "UtCallback.hpp"
#include "UtLog.hpp"
class GeoElevationTile;
#include "GeoElevationTileManager.hpp"
class GeodeticEarthDatabase;

class UtInput;

class VisualDatabase;

class WsfCodedZone;
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
class WsfZone;

namespace wsf
{

class Terrain;

//! Manages interface to terrain for a set of WsfTerrain objects
class WSF_EXPORT TerrainInterface
{
public:
   enum DataType
   {
      cDTED,
      cFLOAT_GRID,
      cVISUAL_DATABASE,
      // cGEOID_UNDULATED_DATABASE,
      cGEODETIC_EARTH_DATABASE, // Support .eld database files
      cGEOTIFF,
      cBATHYMETRY
   };


   class RectInput
   {
   public:
      bool ProcessRect(UtInput& aInput);

      // protected:
      int mSWLat;
      int mSWLon;
      int mNELat;
      int mNELon;
   };

   struct DtedInput : public RectInput
   {
      std::string mDirName;
      int         mLevel;
      bool        mRaw;
   };

   struct FloatGridInput : public RectInput
   {
      bool        mVegetation;
      std::string mDirName;
      std::string mFileName;
   };

   struct GeotiffInput : public RectInput
   {
      std::string mDirName;
   };

   struct BathymetryInput
   {
      std::string mFileName;
   };

   struct Query
   {
      enum QueryType
      {
         cNONE,
         cHEIGHT,
         cHORIZON,
         cMASKING,
         cTEST_EXTREMA_IN_ZONE
      };

      QueryType     mType        = QueryType::cNONE;
      double        mEarthRadius = 0.0;
      double        mLat         = 0.0;
      double        mLon         = 0.0;
      double        mAlt         = 0.0;
      double        mLat2        = 0.0;
      double        mLon2        = 0.0;
      double        mAlt2        = 0.0;
      WsfCodedZone* mZonePtr     = nullptr;
   };

   friend class Terrain;

   TerrainInterface(WsfScenario& aScenarioPtr);
   ~TerrainInterface();

   static TerrainInterface* Get(const WsfScenario& aScenario);

   bool ProcessInput(UtInput& aInput);
   bool Initialize();

   WsfScenario* GetScenario() { return mScenarioPtr; }

   //! Access for DTED DB Information
   //@{
   std::vector<int>*             GetDTEDLevel() { return &mDTEDLevel; }
   int                           GetDTEDLevelByIndex(size_t aIndex);
   std::vector<std::string>*     GetDTEDPath() { return &mDTEDPath; }
   void                          GetDTEDPathByIndex(size_t aIndex, std::string& aPath);
   const std::vector<DtedInput>& GetDTEDInputs() const { return mDtedInputs; }
   //@}

   //! Access for geotiff DB Information
   //@{
   const std::vector<GeotiffInput>& GetGeotiffInputs() const { return mGeotiffInputs; }
   //@}

   //! Access for Earth Radius Multiplier.
   //@{
   double GetEarthRadiusMultiplier() { return mEarthRadiusMultiplier; }
   //@}

   void PerformQueries(WsfSimulation& aSimulation);
   bool IsEnabled() const { return mIsEnabled; }
   bool IsVegetationEnabled() const { return mIsVegetationEnabled; }
   bool IsGeodeticDatabase() const { return mGeodeticDatabase; }
   bool IsDTED() const { return mDTED; }
   bool IsBathymetryEnabled() const { return mIsBathymetryEnabled; }

   //! @name External Services access.
   //! These methods support the External Services and are not part of
   //! the WSF public interface.
   //@{
   GeoElevationTileManager& GetTileManager();
   //@}

   GeoElevationTileManager& GetVegetationManager();
   GeoElevationTileManager& GetManager();
   GeoElevationTileManager& GetBathymetryManager();

   bool AddDtedDirectory(const std::string& aDirName, int aLevel);
   bool AddRawDtedDirectory(const std::string& aDirName, int aLevel, int aSwLat, int aSwLon, int aNeLat, int aNeLon);

   bool AddGeotiffDirectory(const std::string& aDirName);

   bool AddBathymetry(const std::string& aFileName);

   int GetElevApproxP(double aLat, double aLon, float& aElev, GeoElevationTileManager& aTileManager, GeoElevationTile*& aTilePtr);

   void GetGeodeticElev(double aLat, double aLon, float& aElev);

   void GeodeticComputeNormal(double aLat, double aLon, double aNormal[3]);

   void GetNormalNED(double aLat, double aLon, double aNormal[3], GeoElevationTile*& aTilePtr);
   void GetNormalENU(double aLat, double aLon, double aNormal[3], GeoElevationTile*& aTilePtr);

   void DisableTerrain();
   void EnableTerrain();

   bool MaskedByTerrain(double aLat1,
                        double aLon1,
                        double aAlt1,
                        double aLat2,
                        double aLon2,
                        double aAlt2,
                        double aMaxRange,
                        double aRadiusScale = 1.0);

   bool MaskedByVegetation(double aLat1,
                           double aLon1,
                           double aAlt1,
                           double aLat2,
                           double aLon2,
                           double aAlt2,
                           double aMaxRange,
                           double aRadiusScale = 1.0);

   bool MaskedByUnderwaterTerrain(double aLat1,
                                  double aLon1,
                                  double aAlt1,
                                  double aLat2,
                                  double aLon2,
                                  double aAlt2,
                                  double aMaxRange,
                                  double aRadiusScale = 1.0);

protected:
   void ValidateDTED();

   bool SetFloatGridDirectory(const std::string&       aDirName,
                              double                   aSwLat,
                              double                   aSwLon,
                              double                   aNeLat,
                              double                   aNeLon,
                              GeoElevationTileManager& aTileManager);

   bool SetFloatGridFile(const std::string& aFileName, GeoElevationTileManager& aTileManager);

   int GetElevInterpP(double aLat, double aLon, float& aElev, GeoElevationTileManager& aTileManager, GeoElevationTile*& aTilePtr);

   bool MaskedByTerrainP(double                   aLat1,
                         double                   aLon1,
                         double                   aAlt1,
                         double                   aLat2,
                         double                   aLon2,
                         double                   aAlt2,
                         double                   aMaxRange,
                         GeoElevationTileManager& TileManager,
                         double                   aRadiusScale);

   bool MaskedByTerrainFastP(double                   aLat1,
                             double                   aLon1,
                             double                   aAlt1,
                             double                   aLat2,
                             double                   aLon2,
                             double                   aAlt2,
                             double                   aMaxRange,
                             GeoElevationTileManager& aTileManager);

   bool ListTheFiles(const std::string& aGeodeticEarthFilePathname);

   bool DirectoryExists(const std::string& aDirName, const std::string& aCommand);
   bool LoadTile(double aLat, double aLon, GeoElevationTile*& aTilePtr);

   int  LoadCME_File(const char* aCfgFile);
   bool LoadCME_Terrain();

   void ReadString(std::istream& aIn, std::string& aString);

private:
   WsfScenario* mScenarioPtr;

   DataType mDataType                 = cDTED;
   double   mMinAllowableStepSize     = 0.0;    // meters
   double   mMaxTerrainHeight         = 8850.0; // Height of Mt. Everest (in meters)
   double   mMaxDataSize              = -1.0;   // Not used by default
   double   mEarthRadiusMultiplier    = 1.0;
   bool     mIgnoreMissingCME_Terrain = false;
   bool     mDisabledByUser           = false;
   // Terrain / Vegetation has been defined and hasn't been disabled by the user
   bool mIsEnabled           = false;
   bool mIsVegetationEnabled = false;
   // Terrain has been defined (implicitly or explicitly)
   bool mIsDefined           = false;
   bool mDebugTerrainMasking = false;
   bool mDebugTerrainExtrema = false;
   bool mUseFastMaskingCheck = false;

   std::vector<std::string> mDTEDPath;  // path pointing to dted database
   std::vector<int>         mDTEDLevel; //

   // added for reading .eld files for Mission System Lab databases
   int mGeodeticEarthDBNumber = 4; // Geodetic earth database number
   // Geodetic earth database name
   std::string  mGeodeticEarthDBName = "EDWARDS"; // Default EDWARDS data base
   std::string  mGeodeticEarthFilePathname;
   unsigned int mGeodeticDatabaseType = 0;

   std::string                 mVisualDBFile; // file name pointing to elevation database
   bool                        mDTED             = false;
   bool                        mGeodeticDatabase = false;
   std::vector<DtedInput>      mDtedInputs;
   std::vector<FloatGridInput> mFloatGridInputs;
   std::vector<GeotiffInput>   mGeotiffInputs;
   bool                        mLoadCME_Terrain = false;
   std::string                 mCME_Path;
   bool                        mValidateDTED = false;
   std::vector<Query>          mQueryList;

   VisualDatabase*        mVisualDB_Ptr        = nullptr;
   GeodeticEarthDatabase* mGeodeticEarthDB_Ptr = nullptr; // Geodetic earth database class pointer

   std::unique_ptr<GeoElevationTileManager> mGeoTileManagerPtr           = nullptr;
   std::unique_ptr<GeoElevationTileManager> mGeoTileVegetationManagerPtr = nullptr;
   std::once_flag                           mGeoTileManagerFlag;
   std::once_flag                           mGeoTileVegetationManagerFlag;

   // Bathymetry
   bool                                     mBathymetryDefined   = false;
   bool                                     mIsBathymetryEnabled = false;
   std::vector<BathymetryInput>             mBathmetryInputs;
   std::unique_ptr<GeoElevationTileManager> mGeoTileBathymetryManagerPtr = nullptr;
   std::once_flag                           mGeoTileBathymetryManagerFlag;
};

//! Implements the terrain database and query operations.
//!
//! This class provides the following major functions:
//! - To get the height of the terrain at a location.
//! - To get the unit normal vector to the terrain at a location.
//! - To get vegetation information over a particular location.
//! - To determine if the line of sight between two objects is masked by the terrain.
//! - To determine height extrema (min, max) within a (LatA, LatB, LonA, LonB) rectangle.
//!
//! An instance of a Terrain object provides the context in which to perform
//! the first two operations.  The object maintains a concept of a 'current tile',
//! which represents the tile that contains the latitude and longitude involved
//! in the last query operation. When a query is performed the object will determine
//! if the current tile is the correct tile and if not, load the correct tile and
//! then perform the requested query.  Typically a simulation object (mover, sensor, etc)
//! instantiates a Terrain object and makes queries on the object as it moves along.
//! Therefore, the Terrain object only needs to switch tiles when the owning
//! object crosses a tile boundary.
//!
//! The terrain masking and height extrema checks are static methods.
//!
//! The class maintains static data that represents the terrain database.  The query
//! methods reference this database to get their terrain data.  Note that tiles are
//! shared between Terrain objects through a reference counting mechanism. Thus,
//! only a single instance of a specific tile will actually exist in memory.

class WSF_EXPORT Terrain
{
public:
   friend class TerrainInterface;

   enum ElevationType
   {
      cINVALID,
      cTERRAIN,
      cVEGETATION,
      cBATHYMETRY
   };

   Terrain(TerrainInterface* aInterfacePtr);
   Terrain(const Terrain& aSrc);
   Terrain& operator=(const Terrain& aRhs) = delete;
   ~Terrain();

   bool Initialize(TerrainInterface* aInterfacePtr);
   bool Initialize(WsfSimulation& aSimulation);

   int GetElevApprox(double aLat, double aLon, float& aElev);

   int GetElevApprox(const double aWCS[3], float& aElev);

   int GetElevInterp(double aLat, double aLon, float& aElev);

   int GetElevInterp(const double aWCS[3], float& aElev);

   int GetVegElevApprox(double aLat, double aLon, float& aElev);

   int GetVegElevInterp(double aLat, double aLon, float& aElev);

   int GetBathymetryElevApprox(double aLat, double aLon, float& aElev);

   int GetBathymetryElevInterp(double aLat, double aLon, float& aElev);

   void GetNormalNED(double aLat, double aLon, double aNormal[3])
   {
      if (mInterfacePtr)
      {
         mInterfacePtr->GetNormalNED(aLat, aLon, aNormal, mTerrainTilePtr);
      }
   }

   void GetNormalENU(double aLat, double aLon, double aNormal[3])
   {
      if (mInterfacePtr)
      {
         mInterfacePtr->GetNormalENU(aLat, aLon, aNormal, mTerrainTilePtr);
      }
   }

   bool TerrainHitAlongLOS(const double aEyepointWCS[3], double aTerminusWCS[3]);

   //! Is terrain enabled?
   //! This can be used by a caller to bypass
   //! expensive terrain-related calls if terrain is not available.
   //! @returns 'true' if enabled or 'false' if not.

   bool IsEnabled() { return (mInterfacePtr && mInterfacePtr->IsEnabled()); }

   bool IsVegetationEnabled() { return (mInterfacePtr && mInterfacePtr->IsVegetationEnabled()); }

   bool IsBathymetryEnabled() { return (mInterfacePtr && mInterfacePtr->IsBathymetryEnabled()); }

   bool MaskedByTerrain(double aLat1,
                        double aLon1,
                        double aAlt1,
                        double aLat2,
                        double aLon2,
                        double aAlt2,
                        double aMaxRange,
                        double aRadiusScale = 1.0);

   bool MaskedByTerrainFast(double aLat1, double aLon1, double aAlt1, double aLat2, double aLon2, double aAlt2, double aMaxRange);

   bool MaskedByVegetation(double aLat1,
                           double aLon1,
                           double aAlt1,
                           double aLat2,
                           double aLon2,
                           double aAlt2,
                           double aMaxRange,
                           double aRadiusScale = 1.0);

   bool MaskedByVegetationFast(double aLat1, double aLon1, double aAlt1, double aLat2, double aLon2, double aAlt2, double aMaxRange);

   struct Post
   {
      double mHeightM = 0.0;
      double mLatDeg  = 0.0;
      double mLonDeg  = 0.0;
   };

   bool GetHeightExtremaIn(WsfSimulation& aSimulation, WsfZone* aZonePtr, Post& aMin, Post& aMax);

   bool GetHeightExtremaIn(WsfSimulation& aSimulation,
                           double         aLatSouth,
                           double         aLonWest,
                           double         aLatNorth,
                           double         aLonEast,
                           Post&          aMin,
                           Post&          aMax,
                           WsfZone*       aZonePtr = nullptr); // optional

   // added for reading Flight Simulation geodetic databases
   bool IsGeodeticDatabase() { return (mInterfacePtr && mInterfacePtr->IsGeodeticDatabase()); }
   bool DTEDTypeDatabase() { return (mInterfacePtr && mInterfacePtr->IsDTED()); }

protected:
   GeoElevationTile* GetTilePtr() { return mTerrainTilePtr; }

   GeoElevationTileManager& GetManager();

   bool LoadTile(double aLat, double aLon) { return mInterfacePtr->LoadTile(aLat, aLon, mTerrainTilePtr); }

   bool ComputeTerrainImpactP(double aLocationWCS_1[3], double aLocationWCS_2[3], bool aFirstPass = false);

private:
   void GetEdgeExtremaInZone(WsfZone*                   aZonePtr,
                             const std::vector<double>& aPostLatitudes,
                             const std::vector<double>& aPostLongitudes,
                             bool&                      aMinFound,
                             bool&                      aMaxFound,
                             float&                     aMinElevF,
                             float&                     aMaxElevF,
                             Post&                      aMin,
                             Post&                      aMax);

   void CreateLocalsFromBoundaries(std::vector<double>  aSortedValues,
                                   double               aStartValue,
                                   double               aEndValue,
                                   std::vector<double>& aLocalValues);


   GeoElevationTile* mTerrainTilePtr;
   GeoElevationTile* mVegetationTilePtr;
   GeoElevationTile* mBathymetryXYZ_TilePtr;

   TerrainInterface* mInterfacePtr; // soft pointer
};

} // namespace wsf

#endif
