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

#ifndef P6DOFTERRAIN_H
#define P6DOFTERRAIN_H

#include "p6dof_export.h"

#include <string>
#include <vector>

#include "UtVec3dX.hpp"

class UtInput;

//! P6DofTerrain provides height of terrain for Pseudo 6DOF objects. This base
// class returns a zero height of terrain for all locations. Specific terrain
// models should derive from this class.

class P6DOF_EXPORT P6DofTerrain
{
public:
   explicit P6DofTerrain(const std::string& aFilename);

   ~P6DofTerrain() = default;

   P6DofTerrain* Clone() const;

   bool ReadTerrainFile(const std::string& aFilename);

   bool ProcessInput(UtInput& aInput);

   // HeightOfTerrain_m returns the height of the terrain in meters above sea level.
   // This function should be overloaded by derived classes.
   double HeightOfTerrain_m(double aLatitude, double aLongitude) const;

   // HeightOfTerrain_m returns the height of the terrain in meters above sea level.
   // It also provides the normal vector (in NED coords) to the terrain (pointing up),
   // the speed of the surface in NED (if moving, such as an aircraft carrier), and a
   // friction multiplier at the location. The friction multiplier is used to relate
   // how the friction at the location compares to nominal tire to runway friction.
   // This function should be overloaded by derived classes.
   double HeightOfTerrain_m(double    aLatitude,
                            double    aLongitude,
                            UtVec3dX& aUpVectorNED,
                            UtVec3dX& aSpeedVectorNED_mps,
                            double&   aFrictionMultiplier) const;

protected:
   P6DofTerrain()                         = default;
   P6DofTerrain(const P6DofTerrain& aSrc) = default;

   struct TerrainLocation
   {
      double lat_deg;
      double lon_deg;
      double minlat_deg;
      double maxlat_deg;
      double minlon_deg;
      double maxlon_deg;
      double alt_ft;
   };

   struct TerrainRegion
   {
      double                       minlat_deg;
      double                       maxlat_deg;
      double                       minlon_deg;
      double                       maxlon_deg;
      std::vector<TerrainLocation> locationList;
   };

   bool ProcessRegion(UtInput& aInput);
   bool ProcessLocation(UtInput& aInput, TerrainRegion& aRegion);

   bool HeightOfRegionLocation_ft(double aLatitude, double aLongitude, double& aAlt_ft) const;

   bool WithinRegion(const TerrainRegion& aRegion, double aLatitude, double aLongitude) const;

   bool WithinLocation(const TerrainLocation& aLocation, double aLatitude, double aLongitude, double& aAlt_ft) const;

   std::vector<TerrainRegion> mTerrainRegionList;
};

#endif
