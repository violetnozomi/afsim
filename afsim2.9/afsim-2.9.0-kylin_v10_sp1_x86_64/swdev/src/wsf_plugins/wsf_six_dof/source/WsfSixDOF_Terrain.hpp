// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFTERRAIN_HPP
#define WSFSIXDOFTERRAIN_HPP

#include "wsf_six_dof_export.h"

#include <string>
#include <vector>

class UtInput;
#include "UtVec3dX.hpp"

namespace wsf
{
class TerrainInterface;

namespace six_dof
{
//! Terrain provides height of terrain for SixDOF objects.
class WSF_SIX_DOF_EXPORT Terrain
{
public:
   explicit Terrain(TerrainInterface* aWsfTerrain);

   ~Terrain() = default;

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
   Terrain()                    = default;
   Terrain(const Terrain& aSrc) = default;

   struct TerrainLocation
   {
      double lat_deg    = 0.0;
      double lon_deg    = 0.0;
      double minlat_deg = 0.0;
      double maxlat_deg = 0.0;
      double minlon_deg = 0.0;
      double maxlon_deg = 0.0;
      double alt_ft     = 0.0;
   };

   struct TerrainRegion
   {
      double                       minlat_deg = 0.0;
      double                       maxlat_deg = 0.0;
      double                       minlon_deg = 0.0;
      double                       maxlon_deg = 0.0;
      std::vector<TerrainLocation> locationList;
   };

   bool ProcessRegion(UtInput& aInput);
   bool ProcessLocation(UtInput& aInput, TerrainRegion& aRegion);

   bool HeightOfRegionLocation_ft(double aLatitude, double aLongitude, double& aAlt_ft) const;

   bool WithinRegion(const TerrainRegion& aRegion, double aLatitude, double aLongitude) const;

   bool WithinLocation(const TerrainLocation& aLocation, double aLatitude, double aLongitude, double& aAlt_ft) const;

private:
   std::vector<TerrainRegion> mTerrainRegionList;
   wsf::TerrainInterface*     mWsfTerrain = nullptr; // Used to serve common terrain data to SixDOF movers
};
} // namespace six_dof
} // namespace wsf

#endif
