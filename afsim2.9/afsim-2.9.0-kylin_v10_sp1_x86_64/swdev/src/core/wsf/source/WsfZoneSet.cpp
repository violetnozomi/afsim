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

// ================================================================================================
// ================================================================================================
// PROGRAMMING NOTE: See the programming notes in WsfZone.cpp about the use of Initialize()!!!!
// ================================================================================================
// ================================================================================================

#include "WsfZoneSet.hpp"

#include <cassert>
#include <memory>

#include "GeoIntersectDatabase.hpp"
#include "GeoShapeFile.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfScenario.hpp"
#include "WsfTerrain.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneReference.hpp"

// =================================================================================================
// Elevation callback needed by the GeoShapeFile loader.
class WsfTerrainElevationCallback : public GeoShapeFile::ElevationCallback
{
public:
   WsfTerrainElevationCallback(wsf::Terrain& aTerrain)
      : mTerrain(aTerrain)
   {
   }

   float operator()(double aLat, double aLon) override
   {
      float elev = 0.f;
      mTerrain.GetElevInterp(aLat, aLon, elev);
      return elev;
   }

   wsf::Terrain mTerrain;
};

// =================================================================================================
//! Constructor.
WsfZoneSet::WsfZoneSet()
   : WsfZone()
   , mIntersectDBPtr(nullptr)
   , mAttenuationFileName(nullptr)
   , mUseDTED(false)
   , mHeightParameter(nullptr)
   , mConstHeight(0)
   , mBaseAltitudeParameter(nullptr)
   , mConstBaseAltitude(0)
   , mGeocentric(false)
   , mBaseAltitiudeIsAGL(false)
   , mTerrainPtr(nullptr)
{
}

// =================================================================================================
//! Copy constructor.
WsfZoneSet::WsfZoneSet(const WsfZoneSet& aSrc)
   : WsfZone(aSrc)
   , mIntersectDBPtr(nullptr)
   , mAttenuationFileName(aSrc.mAttenuationFileName)
   , mUseDTED(aSrc.mUseDTED)
   , mHeightParameter(aSrc.mHeightParameter)
   , mConstHeight(aSrc.mConstHeight)
   , mBaseAltitudeParameter(aSrc.mBaseAltitudeParameter)
   , mConstBaseAltitude(aSrc.mConstBaseAltitude)
   , mGeocentric(aSrc.mGeocentric)
   , mBaseAltitiudeIsAGL(aSrc.mBaseAltitiudeIsAGL)
   , mTerrainPtr(nullptr)
{
   for (const auto& zonePtr : aSrc.mZonePtrs)
   {
      mZonePtrs.push_back(zonePtr->Clone());
   }
   for (const auto& exclusionZonePtr : aSrc.mExclusionZonePtrs)
   {
      mExclusionZonePtrs.push_back(exclusionZonePtr->Clone());
   }
   if (aSrc.mTerrainPtr)
   {
      mTerrainPtr = new wsf::Terrain(*aSrc.mTerrainPtr);
   }
   if (aSrc.mIntersectDBPtr != nullptr && aSrc.mAttenuationFileName > nullptr)
   {
      assert(mTerrainPtr);
      // construct an intersect database unique to this object using the same parameters as the source.
      ConstructZoneDatabase();
   }
}

// =================================================================================================
//! Destructor
// virtual
WsfZoneSet::~WsfZoneSet()
{
   for (auto& zonePtr : mZonePtrs)
   {
      delete zonePtr;
   }

   for (auto& exclusionZonePtr : mExclusionZonePtrs)
   {
      delete exclusionZonePtr;
   }

   if (mIntersectDBPtr != nullptr)
   {
      delete mIntersectDBPtr;
   }
   delete mTerrainPtr;
}

// =================================================================================================
//! Standard Clone() method.
// virtual
WsfZone* WsfZoneSet::Clone() const
{
   return new WsfZoneSet(*this);
}

// =================================================================================================
// virtual
bool WsfZoneSet::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "zone") || (command == "exclude_zone"))
   {
      UtInputBlock                       inputBlock(aInput);
      std::unique_ptr<WsfZoneDefinition> zonePtr(new WsfZoneDefinition());
      while (inputBlock.ReadCommand())
      {
         if (!zonePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (command == "zone")
      {
         mZonePtrs.push_back(zonePtr.get());
      }
      else
      {
         mExclusionZonePtrs.push_back(zonePtr.get());
      }
      zonePtr.release();
   }
   else if ((command == "use_zone") || (command == "use_exclude_zone"))
   {
      std::string zoneName;
      aInput.ReadValue(zoneName);
      if (command == "use_zone")
      {
         mZonePtrs.push_back(new WsfZoneReference(zoneName));
      }
      else
      {
         mExclusionZonePtrs.push_back(new WsfZoneReference(zoneName));
      }
   }
   else if (command == "attenuation_parameters")
   {
      std::string fileName;
      bool        useDTED = false;
      std::string heightParameter;
      std::string baseAltitudeParameter;
      double      constHeight       = 0;
      double      constBaseAltitude = 0;
      bool        geoCentric        = false;
      std::string projectionType("geodetic");

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         UtInput& input = inputBlock.GetInput();
         if (command == "file")
         {
            aInput.ReadValueQuoted(fileName);
            if (fileName.size() < 4 || fileName.substr(fileName.size() - 4) != ".shp")
            {
               std::string shpFile = aInput.LocateFile(fileName + ".shp");
               fileName            = shpFile.substr(0, shpFile.size() - 4);
            }
         }
         else if (command == "use_dted")
         {
            useDTED = true;
         }
         else if (command == "height_parameter")
         {
            input.ReadValue(heightParameter);
         }
         else if (command == "base_altitude_parameter")
         {
            input.ReadValue(baseAltitudeParameter);
         }
         else if (command == "constant_height")
         {
            input.ReadValueOfType(constHeight, UtInput::cLENGTH);
         }
         else if (command == "projection")
         {
            input.ReadCommand(projectionType);
            geoCentric = (projectionType == "geocentric") ? true : false;
         }
         else if (command == "sensor_modifier")
         {
            // sensor_modifier <category-name> <modifier-value>
            std::string categoryName;
            input.ReadCommand(categoryName);
            double modifierValue;
            input.ReadValue(modifierValue);
            WsfStringId nameId    = categoryName;
            mModifierList[nameId] = modifierValue;
         }
         else if (command == "comm_modifier")
         {
            // comm_modifier <category-name> <modifier-value>
            std::string categoryName;
            input.ReadCommand(categoryName);
            double modifierValue;
            input.ReadValue(modifierValue);
            WsfStringId nameId    = categoryName;
            mModifierList[nameId] = modifierValue;
         }
         else if (command == "constant_base_altitude")
         {
            input.ReadValueOfType(constBaseAltitude, UtInput::cLENGTH);
            std::string altRef;
            // Check for optional 'msl' or 'agl' altitude reference ('msl' assumed if not specified).
            aInput.ReadValue(altRef);
            mBaseAltitiudeIsAGL = false; // Assume 'msl'
            if (altRef == "agl")
            {
               mBaseAltitiudeIsAGL = true;
            }
            else if (altRef != "msl")
            {
               aInput.PushBack(altRef);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (fileName.empty())
      {
         throw UtInput::BadValue(aInput, "attenuation_parameter block must have a file defined.");
         ////attempt to open the provided file
         // std::string shapeFileName;
         // shapeFileName = fileName;
         // shapeFileName += ".shp";
         //
         // std::ifstream inputFile(shapeFileName.c_str(), ifstream::binary | ifstream::in);
         // if (!inputFile)
         //{
         //    throw UtInput::BadValue(aInput, "The input shape file was not found");
         // }
      }

      mAttenuationFileName   = fileName;
      mUseDTED               = useDTED;
      mHeightParameter       = heightParameter;
      mBaseAltitudeParameter = baseAltitudeParameter;
      mConstHeight           = constHeight;
      mConstBaseAltitude     = constBaseAltitude;
      mGeocentric            = geoCentric;
   }
   else
   {
      myCommand = WsfZone::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfZoneSet::PointIsInside(WsfSimulation* aSimulationPtr,
                               const double   aViewedPointWCS[3],
                               const double   aEyePointWCS[3],
                               const double   aLookHeading,
                               const double   aDeltaDownRange)
{
   assert(mIsInitialized);
   bool included = false;
   for (std::vector<WsfZone*>::const_iterator zli = mZonePtrs.begin(); zli != mZonePtrs.end(); ++zli)
   {
      if ((*zli)->PointIsInside(aSimulationPtr, aViewedPointWCS, aEyePointWCS, aLookHeading, aDeltaDownRange))
      {
         included = true;
         break;
      }
   }
   bool excluded = false;
   for (std::vector<WsfZone*>::const_iterator zli = mExclusionZonePtrs.begin(); zli != mExclusionZonePtrs.end(); ++zli)
   {
      if ((*zli)->PointIsInside(aSimulationPtr, aViewedPointWCS, aEyePointWCS, aLookHeading, aDeltaDownRange))
      {
         excluded = true;
         break;
      }
   }

   return included && !excluded;
}

// =================================================================================================
//! return the geopoint of the reference point
// virtual
WsfGeoPoint WsfZoneSet::Reference()
{
   if (!mZonePtrs.empty())
   {
      return mZonePtrs[0]->Reference();
   }

   return WsfGeoPoint();
}

// =================================================================================================
WsfZone* WsfZoneSet::GetZoneAtIndex(int aIndex)
{
   return mZonePtrs[aIndex];
}

// =================================================================================================
WsfZone* WsfZoneSet::GetExclusionZoneAtIndex(const int aIndex)
{
   return mExclusionZonePtrs[aIndex];
}

// =================================================================================================
double WsfZoneSet::CheckIntersections(const double aLLA1[3], const double aLLA2[3])
{
   if (mIntersectDBPtr != nullptr)
   {
      return mIntersectDBPtr->Penetration(aLLA1, aLLA2);
   }
   return 0.0;
}

// =================================================================================================
//! draw the zone using WsfDraw
// virtual
void WsfZoneSet::DebugDrawZone(WsfDraw& aDraw)
{
   for (auto& zonePtr : mZonePtrs)
   {
      zonePtr->DebugDrawZone(aDraw);
   }
}

// =================================================================================================
//! draw the centroid using WsfDraw in an x
// virtual
void WsfZoneSet::DebugDrawCentroid(WsfDraw& aDraw)
{
   assert(mIsInitialized);
   aDraw.SetColor(0.0, 0.0, 0.0);
   aDraw.SetLineSize(1);
   aDraw.SetDuration(5.0);

   aDraw.BeginLines();
   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX - 0.1, mCentroid.mY + 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX + 0.1, mCentroid.mY + 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX + 0.1, mCentroid.mY - 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX - 0.1, mCentroid.mY - 0.1, 0.0);
   aDraw.End();
}

// =================================================================================================
//! draw the bounding box using WsfDraw
// virtual
void WsfZoneSet::DebugDrawBounds(WsfDraw& aDraw)
{
   assert(mIsInitialized);

   aDraw.SetColor(0.0, 1.0, 0.0);
   aDraw.SetLineSize(3);
   aDraw.SetDuration(5.0);

   aDraw.BeginLines();
   aDraw.VertexLLA(mMinX, mMinY, 0.0);
   aDraw.VertexLLA(mMinX, mMaxY, 0.0);

   aDraw.VertexLLA(mMinX, mMaxY, 0.0);
   aDraw.VertexLLA(mMaxX, mMaxY, 0.0);

   aDraw.VertexLLA(mMaxX, mMaxY, 0.0);
   aDraw.VertexLLA(mMaxX, mMinY, 0.0);

   aDraw.VertexLLA(mMaxX, mMinY, 0.0);
   aDraw.VertexLLA(mMinX, mMinY, 0.0);
   aDraw.End();

   for (auto& zonePtr : mZonePtrs)
   {
      zonePtr->DebugDrawBounds(aDraw);
   }
}

// =================================================================================================
// protected
bool WsfZoneSet::ConstructZoneDatabase()
{
   if (!mAttenuationFileName.IsNull())
   {
      WsfTerrainElevationCallback* elevationCallbackPtr =
         (mUseDTED && mTerrainPtr) ? new WsfTerrainElevationCallback(*mTerrainPtr) : nullptr;
      GeoShapeFile gsf(mAttenuationFileName.GetString().c_str(),
                       mHeightParameter.GetString().c_str(),
                       mConstHeight,
                       mBaseAltitudeParameter.GetString().c_str(),
                       mConstBaseAltitude,
                       mGeocentric,
                       mBaseAltitiudeIsAGL,
                       elevationCallbackPtr);

      double cxyz[3];
      gsf.GetCenter(cxyz[0], cxyz[1], cxyz[2]);

      mIntersectDBPtr = new GeoIntersectDatabase(gsf, 11);
   }
   return true;
}

// =================================================================================================
// virtual
void WsfZoneSet::Initialize(const WsfScenario& aScenario)
{
   if (mIsInitialized)
   {
      return;
   }

   if ((!mAttenuationFileName.IsNull()) && mUseDTED && (wsf::TerrainInterface::Get(aScenario) != nullptr))
   {
      mTerrainPtr = new wsf::Terrain(wsf::TerrainInterface::Get(aScenario));
   }

   ConstructZoneDatabase();

   for (auto& zonePtr : mZonePtrs)
   {
      zonePtr->Initialize(aScenario);
   }
   for (auto& exclusionZonePtr : mExclusionZonePtrs)
   {
      exclusionZonePtr->Initialize(aScenario);
   }

   CalculateBoundingBox();
   CalculateArea();
   CalculateCentroid();

   mIsInitialized = true;
}

// =================================================================================================
// private
void WsfZoneSet::CalculateBoundingBox()
{
   double zoneMinX, zoneMaxX, zoneMinY, zoneMaxY, zoneMinAlt, zoneMaxAlt;
   for (auto& zonePtr : mZonePtrs)
   {
      zonePtr->MinPoint(zoneMinX, zoneMinY, zoneMinAlt);
      zonePtr->MaxPoint(zoneMaxX, zoneMaxY, zoneMaxAlt);

      // min point
      if (zoneMinX < mMinX)
      {
         mMinX = zoneMinX;
      }
      if (zoneMinY < mMinY)
      {
         mMinY = zoneMinY;
      }

      // max point
      if (zoneMaxX > mMaxX)
      {
         mMaxX = zoneMaxX;
      }
      if (zoneMaxY > mMaxY)
      {
         mMaxY = zoneMaxY;
      }

      if (mMinAlt == cUNSPECIFIED || zoneMinAlt < mMinAlt)
      {
         mMinAlt = zoneMinAlt;
      }
      if (mMaxAlt == cUNSPECIFIED || zoneMaxAlt < mMaxAlt)
      {
         mMaxAlt = zoneMaxAlt;
      }
   }
}

// =================================================================================================
// private
void WsfZoneSet::CalculateArea()
{
   mArea = 0.0;
   for (auto& zonePtr : mZonePtrs)
   {
      mArea += zonePtr->Area();
   }
}

// =================================================================================================
// private
void WsfZoneSet::CalculateCentroid()
{
   double xSum = 0.0;
   double ySum = 0.0;
   for (auto& zonePtr : mZonePtrs)
   {
      xSum += zonePtr->Centroid().GetLat();
      ySum += zonePtr->Centroid().GetLon();
   }
   mCentroid.mX = xSum / mZonePtrs.size();
   mCentroid.mY = ySum / mZonePtrs.size();
}
