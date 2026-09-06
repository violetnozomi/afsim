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

#include "WsfEM_ALARM_Terrain.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "UtMath.hpp"
#include "WsfEM_ALARM_Fortran.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfTerrain.hpp"

const int WsfEM_ALARM_Terrain::min_points_var(100);
// Assume Level 1 DTED (see 'get_ground_range_incr' in terrain_cell.f90)
const double        WsfEM_ALARM_Terrain::deltag(pi* rezero / (180.0 * 1200.0));
bool                WsfEM_ALARM_Terrain::terrain_sw(false);                // TODO
bool                WsfEM_ALARM_Terrain::use_AFSIM_terrain_masking(false); // TODO
bool                WsfEM_ALARM_Terrain::initialized = false;
int                 WsfEM_ALARM_Terrain::aprofile    = 0;
double              WsfEM_ALARM_Terrain::last_rkfact = 0.0;
double              WsfEM_ALARM_Terrain::rearth      = 0.0;
std::vector<double> WsfEM_ALARM_Terrain::cbetap;
std::vector<double> WsfEM_ALARM_Terrain::cosbet;
std::vector<double> WsfEM_ALARM_Terrain::sbetap;
std::vector<double> WsfEM_ALARM_Terrain::sinbet;

// =================================================================================================
// static
void WsfEM_ALARM_Terrain::Initialize(wsf::Terrain& aTerrain)
{
   terrain_sw = aTerrain.IsEnabled();
}

// =================================================================================================
// static
void WsfEM_ALARM_Terrain::SetUseAFSIM_TerrainMasking(bool aUse)
{
   use_AFSIM_terrain_masking = aUse;
}

// =================================================================================================
// static
void WsfEM_ALARM_Terrain::ResetState()
{
   terrain_sw                = false;
   use_AFSIM_terrain_masking = false;
   initialized               = false;
   aprofile                  = 0;
   last_rkfact               = 0.0;
   rearth                    = 0.0;
   cbetap.clear();
   cosbet.clear();
   sbetap.clear();
   sinbet.clear();
}

// =================================================================================================
//! Adjust the incoming altitude so it more closely corresponds to that which would have been
//! determined by stand-alone ALARM.
//!
//! WSF utilizes bi-linear interpolation the get the altitude between terrain posts while ALARM
//! does not. This can lead to some minor differences in the results (especially at close ranges),
//! making it hard to compare results.
//!
//! This isn't strictly necessary, but it was found to be useful when making sure everything worked
//! in the converted code.
//!
//! @param aPlatformPtr The platform containing the object represented by the incoming position.
//! @param aLat         The latitude of the point whose altitude is to be adjusted.
//! @param aLon         The longitude of the point whose altitude is to be adjusted.
//! @param aAlt         On input this is the WSF MSL altitude. On output it will be the ALARM
//!                     MSL altitude.
// static
void WsfEM_ALARM_Terrain::AdjustAltitude(WsfPlatform* aPlatformPtr,
                                         double       aLat, // degrees
                                         double       aLon, // degrees
                                         double&      aAlt)      // meters
{
   if (terrain_sw)
   {
      aAlt -= aPlatformPtr->GetTerrainHeight();
      aAlt += get_cell_height_deg(aPlatformPtr->GetTerrain(), aLat, aLon);
   }
}

// =================================================================================================
// static
int WsfEM_ALARM_Terrain::get_nprofile(double ground_range)
{
   // implicit none

   // real(8), intent(in) :: ground_range

   //--------------------------------------------------------------------
   // Determine NPROFILE, the number of DTED points in the terrain profile
   // between the radar site and the target.
   //--------------------------------------------------------------------

   int nprofile = nint(ground_range / deltag) - 1;

   if (nprofile < 1 && ground_range > deltag)
   {
      nprofile = 1;
   }
   if (nprofile < 0)
   {
      nprofile = 0;
   }

   return nprofile;
}

//----------------------------------------------------------------------
//     Purpose:
//     -------
//     Get the height of the terrain at the specified latitude and
//     longitude.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     12-Jun-1998    - Initially generated...
//
//----------------------------------------------------------------------

// static
double WsfEM_ALARM_Terrain::get_terrain_height(wsf::Terrain& aTerrain, double lat, double lon, const std::string& units)
{
   // use physical_consts, only : rad2deg

   // implicit none

   ////--------------------------------------------------------------------
   //// Passed parameters
   ////--------------------------------------------------------------------

   // real(8),            intent(in)  :: lat
   // real(8),            intent(in)  :: lon
   // character(len = *), intent(in)  :: units              // RAD or DEG

   //--------------------------------------------------------------------
   // Local variables
   //--------------------------------------------------------------------
   double lat_deg, lon_deg;

   //--------------------------------------------------------------------

   double terrain_height = 0.0;
   if (terrain_sw)
   {
      if (units == "RAD")
      {
         lat_deg = lat * rad2deg;
         lon_deg = lon * rad2deg;
      }
      else
      {
         lat_deg = lat;
         lon_deg = lon;
      }

      //-----------------------------------------------------------------
      // Return if the lat and lon are NOT within the expected bounds.
      //-----------------------------------------------------------------
      if (lon_deg < -180.0 || lon_deg > 180.0 || lat_deg < -90.0 || lat_deg > 90.0)
      {
         return 0.0;
      }

      terrain_height = get_cell_height_deg(aTerrain, lat_deg, lon_deg);
   }
   return terrain_height;
}

//----------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines various terrain profile parameters as well as the
//     masking status of the target.
//
//
//     Inputs:
//     ------
//     HAMMSL  -  The radar height, in meters above mean sea level.
//
//     HTMMSL  -  Target height in meters above mean sea-level
//
//     RKFACT  -  Refractivity factor.
//
//     SITLAM  -  Radar site longitude expressed in radians.
//                SITLAM greater than or equal to zero indicates
//                east longitude and SITLAM less than zero indicates
//                west longitude.
//
//     SITPHI  -  Radar site latitude expressed in radians.
//
//     TARLAM  -  Target longitude expressed in radians.
//
//     TARPHI  -  Target latitude expressed in radians.
//
//
//     Outputs:
//     -------
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in
//                the terrain profile between the radar and the
//                target.
//
//     MASKED  -  Logical indicating whether the target is masked.
//
//     NPROFL  -  The number of points in the terrain profile between
//                the radar and the target.
//
//     TANEPP  -  TANEPP(I), I = 1,2,...,NPROFL, is the tangent of
//                the elevation angle from the radar to the I-th
//                point in the terrain profile between the radar
//                and the target.
//
//     TANMAX  -  TANMAX = max TANEPP(I), I = 1,2,...,NPROFL, where
//                TANEPP(I) is the tangent of the elevation angle
//                from the radar to the I-th point in the terrain
//                profile between the radar and the target.
//
//     VISIBL  -  VISIBL(I), I = 0,1,...,NPROFL, logical indicating
//                if the I-th point in the terrain profile is
//                visible from the radar.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the
//                z-coordinate, (vertical), in meters, of the I-th
//                point in the terrain profile between the radar and
//                the target.  ZPROFL(0) is the height of the
//                terrain at the radar.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     John Langenderfer, Division 397
//     4031 Colonel Glenn Highway
//     Beavercreek, Ohio  45431-1673
//     (937) 431-2220
//
//
//     Modified:
//     --------
//     05-Sep-2003  - Changed array argument to assumed size to properly
//                    handle the times when nprofl is zero.
//                    See SPCR #1303.
//     12-Jul-2006  - Correct lat/lon comments and adjust to call new
//                    VISBLE routine.  See SPCR #1362.
//----------------------------------------------------------------------
//!! public

// static
void WsfEM_ALARM_Terrain::profil(wsf::Terrain&        aTerrain,
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
                                 std::vector<double>& zprofl)
{
   // implicit none

   //--------------------------------------------------------------------
   // Passed parameters
   //--------------------------------------------------------------------
   // real(8), intent(in)  :: aztrad     ! target az (radians)
   // real(8), intent(in)  :: eltrad     ! target el (radians)
   // real(8), intent(in)  :: hammsl     ! radar height (meters)
   // real(8), intent(in)  :: htmmsl     ! target height (meters)
   // integer, intent(in)  :: nprofl
   // real(8), intent(in)  :: rkfact     ! refractivity factor
   // real(8), intent(in)  :: sitlam     ! site longitude (radians)
   // real(8), intent(in)  :: sitphi     ! site latitude (radians)
   // real(8), intent(in)  :: tarlam     ! target longitude (radians)
   // real(8), intent(in)  :: tarphi     ! target latitude (radians)

   // real(8), intent(out) :: elvmsl(*)
   // logical, intent(out) :: masked
   // real(8), intent(out) :: tanepp(*)
   // real(8), intent(out) :: tanmax
   // logical, intent(out) :: visibl(*)
   // real(8), intent(out) :: xprofl(0:*)
   // real(8), intent(out) :: zprofl(0:*)

   //--------------------------------------------------------------------
   // Local variables
   //--------------------------------------------------------------------
   bool off = false;

   //--------------------------------------------------------------------
   // Set the logical variable MASKED to .FALSE. to indicate that the
   // target is not masked.  If the target is masked, this variable
   // will be changed after the call to subroutine VISBLE.
   //--------------------------------------------------------------------

   masked = false;

   //--------------------------------------------------------------------
   // Call subroutine VISBLE to determine the terrain profile.
   //--------------------------------------------------------------------

   // BA
   std::vector<int> lcprofl;
   lcprofl.resize(xprofl.size());
   // BA

   /*call*/ visble2(aTerrain,
                    aEnvironment,
                    aztrad,
                    eltrad,
                    hammsl,
                    nprofl,
                    rkfact,
                    sitlam,
                    sitphi,
                    off,
                    off,
                    elvmsl,
                    tanepp,
                    tanmax,
                    visibl,
                    xprofl,
                    zprofl,
                    lcprofl,
                    masked);
}


//----------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines if a clutter subpatch is visible from the radar site.
//
//
//     Inputs:
//     ------
//     ALPHAC  -  The azimuth angle to the clutter in radians.
//
//     VISIBL  -  VISIBL(I), I = 0,1,...,NPROFL, logical indicating
//                if the I-th point in the terrain profile is
//                visible from the radar.
//
//
//     Outputs:
//     -------
//     DRATIO  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//
//     IEND    -  The vector of ending point indices for the visible
//                areas in the terrain profile.
//
//     ISTART  -  The vector of starting point indices for the
//                visible areas in the terrain profile.
//
//     NAREAS  -  The number of visible terrain areas along the
//                terrain profile.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - Subroutine VISBLE called to determine if the
//       terrain in the terrain profile defined by
//       COALPC, and SIALPC is visible from the radar.  JCL
//
//     ALARM 3.2:
//     ---------
//     - Increased data block number to 14 to accommodate the
//       standardized rcs data DATASRCS.  (SPCR-1067, JCL)
//
//     12-Jul-2006 - Modified to call the new VISBLE2 routine.
//                   See SPCR #1362.
//----------------------------------------------------------------------
//!! public

// static
void WsfEM_ALARM_Terrain::visclt(wsf::Terrain&        aTerrain,
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
                                 std::vector<int>&    lcprofl) // added for site-specific land cover
{
   // implicit none

   //--------------------------------------------------------------------
   // Passed parameters
   //--------------------------------------------------------------------

   // real(8), intent(in)  :: alphac                 // radians
   // real(8), intent(in)  :: hammsl                 // meters
   // integer, intent(in)  :: nprofile
   // real(8), intent(in)  :: rkfact
   // real(8), intent(in)  :: sitlam                 // radians
   // real(8), intent(in)  :: sitphi                 // radians
   //
   // real(8), intent(out) :: elvmsl(nprofile)       // meters
   // integer, intent(out) :: iend(nprofile)
   // integer, intent(out) :: istart(nprofile)
   // integer, intent(out) :: nareas
   // real(8), intent(out) :: rngter(0:nprofile)     // meters
   // real(8), intent(out) :: tanepp(nprofile)
   // logical, intent(out) :: visibl(nprofile)
   // real(8), intent(out) :: xprofl(0:nprofile)     // meters
   // real(8), intent(out) :: zprofl(0:nprofile)     // meters

   //--------------------------------------------------------------------
   // Local variables
   //--------------------------------------------------------------------
   int i;

   double deltax, deltaz, dummy, dummy2;

   bool vislst, off, dummy3;

   nareas    = 1;
   istart[1] = 0;
   vislst    = true;
   off       = false;
   dummy     = 0.0;
   dummy2    = 0.0;
   dummy3    = false;

   //--------------------------------------------------------------------
   // Get the terrain profile data.
   //--------------------------------------------------------------------

   /*call*/ visble2(aTerrain,
                    aEnvironment,
                    alphac,
                    dummy2,
                    hammsl,
                    nprofile,
                    rkfact,
                    sitlam,
                    sitphi,
                    off,
                    off,
                    elvmsl,
                    tanepp,
                    dummy,
                    visibl,
                    xprofl,
                    zprofl,
                    lcprofl,
                    dummy3);

   //--------------------------------------------------------------------
   // Determine the number of visible regions, NAREAS, and save the
   // indexes in ISTART and IEND.
   //--------------------------------------------------------------------

   // do i = 1, nprofile
   for (i = 1; i <= nprofile; ++i)
   {
      if (visibl[i])
      {
         deltax    = xprofl[i];
         deltaz    = zprofl[i] - hammsl;
         rngter[i] = sqrt(deltax * deltax + deltaz * deltaz);

         if (!vislst)
         {
            nareas         = nareas + 1;
            istart[nareas] = i;

         } // end if
      }
      else
      {
         if (vislst)
         {
            iend[nareas] = i - 1;
         }
         if (!terrain_sw)
         {
            return;
         } // early exit

      } // end if

      vislst = visibl[i];

   } // end do

   if (vislst)
   {
      iend[nareas] = nprofile;
   }
}

//----------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines if the terrain in the terrain profile defined by AZIN
//     is visible from the radar.
//
//
//     Inputs:
//     ------
//     AZIN    -  The azimuth angle (+/- pi wrt N) of the target
//                relative to the site (radians).
//
//     ELTRAD  -  The elevation angle (+/- pi/2) to the target
//                relative to the site horizon (radians).
//
//     HAMMSL  -  The site height, in meters above mean sea level.
//
//     NPROFL  -  The number of points in the terrain profile between
//                the site and the target.
//
//     RKFACT  -  Refractivity factor.
//
//     SITLAM  -  Radar site longitude expressed in radians.
//                SITLAM greater than or equal to zero indicates
//                east longitude and SITLAM less than zero indicates
//                west longitude.
//
//     SITPHI  -  Site latitude expressed in radians.  SITPHI greater
//                than or equal to zero indicates nother latitude and
//                less than zero indicates southern latitude.
//
//     QUICK_FLAG - When true, the visibility routine returns when the
//                  target becomes visible.  The entire profile is
//                  not computed.
//
//     VARIABLE_FLAG - When true, a the step size through the profile
//                     is varied to reduce the time to determine target
//                     masking.  This will cause some DTED points to be
//                     ignored.
//
//     Outputs:
//     -------
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in
//                the terrain profile between the radar and the
//                target.
//
//     TANEPP  -  TANEPP(I), I = 1,2,...,NPROFL, is the tangent of
//                the elevation angle from the radar to the I-th
//                point in the terrain profile between the radar
//                and the target.
//
//     TANMAX  -  TANMAX = max TANEPP(I), I = 1,2,...,NPROFL, where
//                TANEPP(I) is the tangent of the elevation angle
//                from the radar to the I-th point in the terrain
//                profile between the radar and the target.
//
//     VISIBL  -  VISIBL(I), I = 0,1,...,NPROFL, logical indicating
//                if the I-th point in the terrain profile is
//                visible from the radar.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the
//                z-coordinate, (vertical), in meters, of the I-th
//                point in the terrain profile between the radar and
//                the target.  ZPROFL(0) is the height of the
//                terrain at the radar.
//
//     MASKED  -  True when the target is masked by terrain features.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modified:
//     --------
//     12-Jul-2006  - Initially created... See SPCR #1362.
//----------------------------------------------------------------------

// static
void WsfEM_ALARM_Terrain::visble2(wsf::Terrain&        aTerrain,
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
                                  bool&                masked)
{
   // use physical_consts, only : rezero, halfpi, twopi, pi, deg2rad
   // use error_check_mod, only : fatal_error

   // implicit none

   //!--------------------------------------------------------------------
   //! Passed parameters
   //!--------------------------------------------------------------------
   // real(8), intent(in)  :: azin              ! target az, +/- pi wrt N (radians)
   // real(8), intent(in)  :: eltrad            ! target el (radians) needed for quick & variable
   // real(8), intent(in)  :: hammsl            ! radar height (meters MSL)
   // integer, intent(in)  :: nprofl            ! points in profile arrays
   // real(8), intent(in)  :: rkfact            ! refractivity factor
   // real(8), intent(in)  :: sitlam            ! site longitude (radians)
   // real(8), intent(in)  :: sitphi            ! site latitude (radians)
   // logical, intent(in)  :: quick_flag        ! quit at first masking
   // logical, intent(in)  :: variable_flag     ! vary step size for speed

   // real(8), intent(out) :: elvmsl(*)         ! elevations (meters MSL)
   // real(8), intent(out) :: tanepp(*)         ! tan to ter
   // real(8), intent(out) :: tanmax            ! max tan in profile
   // logical, intent(out) :: visibl(*)         ! ter point visible flag
   // real(8), intent(out) :: xprofl(0:*)       ! x coord (meters)
   // real(8), intent(out) :: zprofl(0:*)       ! z coord (meters)
   // logical, intent(out) :: masked            ! masking flag

   //--------------------------------------------------------------------
   // Local variables
   //--------------------------------------------------------------------
   // real(8), parameter :: ang_breaks(3) = (/0.25d0, 0.5d0, 1.0d0/)*deg2rad
   // C++NOTE: Unused element at index zero added to allow 1-based indexing
   const double ang_breaks[] = {0.0, 0.25 * deg2rad, 0.5 * deg2rad, 1.0 * deg2rad};

   const std::string RAD_STR("RAD");

   const int hunk = 512;

   // NOTE-C++ The following have been moved to class static variables
   // static bool                initialized = false;
   // static int                 aprofile = 0;
   // static double              last_rkfact = 0.0;
   // static double              rearth      = 0.0;
   // static std::vector<double> cbetap;
   // static std::vector<double> cosbet;
   // static std::vector<double> sbetap;
   // static std::vector<double> sinbet;

   double betap, betapp, coazin, cophis, grangp, siazin, siphis, stphii, terlam = 0.0, terphi = 0.0, terang, el_delta;

   int i;

   //-------------------------------------------------------------------

   if ((!initialized) || (rkfact != last_rkfact) || (nprofl > aprofile))
   {
      if (nprofl > aprofile)
      {
         //-----------------------------------------------------------------
         // Allocate arrays.  Memory is allocated in hunks to minimize
         // allocate/deallocate thrashing if nprofl is gradually creeping
         // up.
         //-----------------------------------------------------------------
         aprofile = aprofile + (std::abs(nprofl / hunk) + 1) * hunk;

         // NOTE: Arrays that are start at index 1 will still have element 0 allocated
         //       (but ignored) so as to allow the code to continue 1-based indexing.
         cbetap.resize(aprofile + 1); // 1:aprofile
         cosbet.resize(aprofile + 1); // 1:aprofile
         sbetap.resize(aprofile + 1); // 1:aprofile
         sinbet.resize(aprofile + 1); // 1:aprofile

         // if (i != 0) then
         //    call fatal_error('visble', 'Can''t allocate memory')
         // end if
      }

      //-----------------------------------------------------------------
      // Initialize constants.
      //-----------------------------------------------------------------
      rearth = rkfact * rezero;

      grangp = 0.0;

      for (i = 1; i <= aprofile; ++i)
      {
         grangp    = grangp + deltag;
         betap     = grangp / rezero;
         betapp    = betap / rkfact;
         sinbet[i] = sin(betap);
         cosbet[i] = cos(betap);
         sbetap[i] = sin(betapp);
         cbetap[i] = cos(betapp);
      }

      initialized = true;
   }

   last_rkfact = rkfact;

   //--------------------------------------------------------------------
   // Initialize trigonometric constants.  SIAZIN is the sine of the
   // azimuth angle of the input point relative to the site.  COAZIN is
   // the cosine respectively of the azimuth angle of the input point
   // relative to the site.
   //--------------------------------------------------------------------

   siazin = sin(azin);
   coazin = cos(azin);

   //--------------------------------------------------------------------
   // Initialize SIPHIS and COPHIS, the sine and cosine, respectively, of
   // the radar site latitude expressed in radians.
   //--------------------------------------------------------------------

   siphis = sin(sitphi);
   cophis = cos(sitphi);

   //--------------------------------------------------------------------
   // Set TANMAX, the tangent of the maximum elevation angle from the
   // radar to a point in the terrain profile, such that the initial
   // maximum elevation angle is approximately minus ninety degrees.
   //--------------------------------------------------------------------

   tanmax = -1.0E+32;

   //--------------------------------------------------------------------
   // Set the zero position of ZPROFL to the terrain height at the radar
   // site.
   //--------------------------------------------------------------------

   xprofl[0] = 0.0; // ADDED-C++
   zprofl[0] = get_terrain_height(aTerrain, sitphi, sitlam, RAD_STR);

   //  do i = 1, nprofl
   i = 1;
   while (i <= nprofl)
   {
      //-----------------------------------------------------------------
      // Determine STPHII, the sine of the latitude of the I-th point in
      // the terrain profile.
      //-----------------------------------------------------------------

      stphii = cophis * sinbet[i] * coazin + siphis * cosbet[i];

      if (fabs(stphii) < 1.0)
      {
         //--------------------------------------------------------------
         // The I-th point in the terrain profile is not located at the
         // north or south pole of the earth.  Determine TERPHI, in
         // radians, the latitude of the I-th point in the terrain
         // profile.
         //--------------------------------------------------------------

         terphi = asin(stphii);
      }
      else
      {
         //--------------------------------------------------------------
         // The I-th point in the terrain profile is located at either
         // the north or the south pole.  Set TERPHI, in radians, the
         // latitude of the I-th point in the terrain profile, to either
         // plus or minus pi over two.
         //--------------------------------------------------------------

         terphi = dsign(halfpi, stphii);
      }

      //-----------------------------------------------------------------
      // Determine TERLAM, in radians, the longitude of the I-th point in
      // the terrain profile.
      //-----------------------------------------------------------------

      terlam = sitlam + atan2(siazin * sinbet[i], cophis * cosbet[i] - siphis * sinbet[i] * coazin);

      //-----------------------------------------------------------------
      // In some instances the value of TERLAM computed above may not lie
      // between plus pi and minus pi.  In these instances, determine a
      // new value for TERLAM by adding or subtracting two pi from the
      // previously determined value.
      //-----------------------------------------------------------------

      if (terlam >= pi)
      {
         terlam = terlam - twopi;
      }
      else if (terlam < -pi)
      {
         terlam = terlam + twopi;
      }

      //-----------------------------------------------------------------
      // Determine ELVMSL(I), in meters, the terrain height above mean
      // sea level at the I-th point in the terrain profile.
      //-----------------------------------------------------------------

      elvmsl[i] = get_terrain_height(aTerrain, terphi, terlam, RAD_STR);

      // BA compute land cover for this cell
      lcprofl[i] = get_land_cover(aEnvironment, terphi * UtMath::cDEG_PER_RAD, terlam * UtMath::cDEG_PER_RAD);
      // BA

      //-----------------------------------------------------------------
      // Determine XPROFL(I), in meters, the x-coordinate of the I-th
      // point in the terrain profile, and ZPROFL(I), in meters, the
      // z-coordinate of the I-th point in the terrain profile.
      //-----------------------------------------------------------------

      xprofl[i] = (rearth + elvmsl[i]) * sbetap[i];
      zprofl[i] = (rearth + elvmsl[i]) * cbetap[i] - rearth;

      //-----------------------------------------------------------------
      // Initialize VISIBL(I) to .FALSE. to indicate that the I-th point
      // in the terrain profile is not visible from the radar.  Then
      // determine TANEPP(I), the tangent of the elevation angle from the
      // radar to the I-th point in the terrain profile.
      //-----------------------------------------------------------------

      visibl[i] = false;
      tanepp[i] = (zprofl[i] - hammsl) / xprofl[i];

      if (tanepp[i] > tanmax)
      {
         //--------------------------------------------------------------
         // The elevation angle from the radar to the I-th point in the
         // terrain profile is greater than the current maximum elevation
         // angle.  Set TANMAX, the tangent of the maximum elevation
         // angle, to TANEPP(I), the tangent of the elevation angle from
         // the radar to the I-th point in the terrain profile.  Set
         // VISIBL(I) to .TRUE. to indicate that the I-th point in the
         // terrain profile is visible from the radar.
         //--------------------------------------------------------------

         tanmax    = tanepp[i];
         visibl[i] = true;
      }

      //--------------------------------------------------------------
      // When maximum elevation to terrain greater than the elevation
      // to target, set masked flag and return when in quick mode.
      //--------------------------------------------------------------
      terang = atan(tanmax);
      if (quick_flag && terang > eltrad)
      {
         masked = true;
         return;
      }

      //----------------------------------------------------------------
      // In the variable step mode, adjust the step size based on the
      // elevation angle to the terrain patch.  The larger the angle the
      // larger the step.
      //----------------------------------------------------------------
      if (!variable_flag)
      {
         i = i + 1;
      }
      else
      {
         el_delta = eltrad - terang;
         if (i <= min_points_var || el_delta <= ang_breaks[1])
         {
            i = i + 1;
         }
         else if (el_delta <= ang_breaks[2])
         {
            i = i + 2;
         }
         else if (el_delta <= ang_breaks[3])
         {
            i = i + 5;
         }
         else
         {
            i = i + 10;
         }
      }

   } // end do

   // Note: AFSIM sometimes shows a terrain blockage when ALARM shows it to be clear.
   // The reason for this discrepancy is under investigation.
   if (use_AFSIM_terrain_masking)
   {
      // test for consistency of masking checks:
      masked = aTerrain.MaskedByTerrain(sitphi * UtMath::cDEG_PER_RAD,
                                        sitlam * UtMath::cDEG_PER_RAD,
                                        zprofl[0],
                                        terphi * UtMath::cDEG_PER_RAD,
                                        terlam * UtMath::cDEG_PER_RAD,
                                        zprofl[nprofl],
                                        1.0e30,
                                        1.3333333333333);
   }
   else // using native ALARM
   {
      terang = atan(tanmax);
      if (terang > eltrad)
      {
         masked = true;
      }
   }
}

// =================================================================================================
// From 'terrain_cell.f90'.
// =================================================================================================

// static
double WsfEM_ALARM_Terrain::get_cell_height_deg(wsf::Terrain& aTerrain, double lat_deg, double lon_deg)
{
   float height;
   aTerrain.GetElevApprox(lat_deg, lon_deg, height);
   return height;
}

#include "WsfEnvironment.hpp"

// static
int WsfEM_ALARM_Terrain::get_land_cover(WsfEnvironment& aEnvironment, double lat_deg, double lon_deg)
{
   return aEnvironment.GetLandCover(lat_deg, lon_deg);
}

// static
double WsfEM_ALARM_Terrain::get_ground_range_incr(int level_in /* = -1 */)
{
   return deltag;
}
