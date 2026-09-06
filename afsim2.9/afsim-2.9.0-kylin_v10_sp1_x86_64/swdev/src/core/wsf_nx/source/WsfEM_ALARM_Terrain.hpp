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

// ****************************************************************************
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
// ****************************************************************************

#ifndef WSFEM_ALARM_TERRAIN_HPP
#define WSFEM_ALARM_TERRAIN_HPP

#include <string>
#include <vector>

class WsfPlatform;
class WsfEnvironment;

namespace wsf
{
class Terrain;
}

//! Terrain handling code from ALARM (terrrain.f90 and friends).
// TODO Remove statics.
class WsfEM_ALARM_Terrain
{
public:
   static void Initialize(wsf::Terrain& aTerrain);

   static void ResetState();

   static void SetUseAFSIM_TerrainMasking(bool aUse);

   static void AdjustAltitude(WsfPlatform* aPlatformPtr, double aLat, double aLon, double& aAlt);

   //! @name from terrain.f90
   //@{
   static int get_nprofile(double ground_range);

   static bool get_terrain_sw() { return terrain_sw; }

   static double get_terrain_height(wsf::Terrain& aTerrain, double lat, double lon, const std::string& units);

   static void profil(wsf::Terrain&        aTerrain,
                      WsfEnvironment&      aEnvironment,
                      double               aztrad,
                      double               eltrad,
                      double               hammsl,
                      double               htmmsl,
                      int                  nprofl,
                      double               rkfact,
                      double               sitlam,
                      double               sitphi,
                      double               tarlam,
                      double               tarphi,
                      std::vector<double>& elvmsl,
                      bool&                masked,
                      std::vector<double>& tanepp,
                      double&              tanmax,
                      std::vector<bool>&   visibl,
                      std::vector<double>& xprofl,
                      std::vector<double>& zprofl);

   static void visclt(wsf::Terrain&        aTerrain,
                      WsfEnvironment&      aEnvironment,
                      double               alphac,
                      double               hammsl,
                      int                  nprofile,
                      double               rkfact,
                      double               sitlam,
                      double               sitphi,
                      std::vector<double>& elvmsl,
                      std::vector<int>&    iend,
                      std::vector<int>&    istart,
                      int&                 nareas,
                      std::vector<double>& rngter,
                      std::vector<double>& tanepp,
                      std::vector<bool>&   visibl,
                      std::vector<double>& xprofl,
                      std::vector<double>& zprofl,
                      std::vector<int>&    lcprofl); // added for site-specific land cover

   static void visble2(wsf::Terrain&        aTerrain,
                       WsfEnvironment&      aEnvironment,
                       double               azin,
                       double               eltrad,
                       double               hammsl,
                       int                  nprofl,
                       double               rkfact,
                       double               sitlam,
                       double               sitphi,
                       bool                 quick_flag,
                       bool                 variable_flag,
                       std::vector<double>& elvmsl,
                       std::vector<double>& tanepp,
                       double&              tanmax,
                       std::vector<bool>&   visibl,
                       std::vector<double>& xprofl,
                       std::vector<double>& zprofl,
                       std::vector<int>&    lcprofl, // added for site-specific land cover
                       bool&                masked);
   //@}

   //! @name From terrain_cell.f90.
   //@{
   static double get_cell_height_deg(wsf::Terrain& aTerrain, double lat_deg, double lon_deg);

   static double get_ground_range_incr(int level_in = -1);

   static int get_land_cover(WsfEnvironment& aEnvironment, double lat_deg, double lon_deg);
   //@}

private:
   static const int    min_points_var;
   static const double deltag;
   static bool         terrain_sw;
   static bool         use_AFSIM_terrain_masking;

   // Moved there to deal with statics
   static bool                initialized;
   static int                 aprofile;
   static double              last_rkfact;
   static double              rearth;
   static std::vector<double> cbetap;
   static std::vector<double> cosbet;
   static std::vector<double> sbetap;
   static std::vector<double> sinbet;
};
#endif
