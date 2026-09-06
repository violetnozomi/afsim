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

#include "WsfEM_ALARM_Geometry.hpp"

#include <cmath>

#include "UtMath.hpp"
#include "WsfEM_ALARM_Fortran.hpp"
#include "WsfEM_ALARM_Terrain.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfPlatform.hpp"

namespace WsfEM_ALARM_Geometry
{
// Compute the geometry between an antenna and a target.
// aXmtrRcvrPtr Pointer to the transmitter/receiver containing the antenna.
// aTargetPtr   Pointer to the target entity. If 0 then the values of tgt_height_msl,
//              tgt_lat and tgt_lon will be used.
// aXmtrPtr     Pointer to the transmitter (only used to get the earth radius scale factor).
void ComputeGeometry(WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                     WsfPlatform*    aTargetPtr,
                     WsfEM_Xmtr*     aXmtrPtr,
                     double&         rdr_height_msl,
                     double&         rdr_lat,
                     double&         rdr_lon,
                     double&         rkfact,
                     double&         tgt_height_msl,
                     double&         tgt_lat,
                     double&         tgt_lon,
                     double&         ground_range,
                     double&         tanept,
                     double&         slant_range,
                     double&         tgt_az,
                     double&         tgt_el,
                     double&         tgt_x,
                     double&         tgt_z)
{
   // use physical_consts, only : pi, halfpi, twopi, rezero

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8),           intent(in)  :: rdr_height_msl     ! meters
   // real(8),           intent(in)  :: rdr_lat            ! radians
   // real(8),           intent(in)  :: rdr_lon            ! radians
   // real(8),           intent(in)  :: rkfact
   // real(8),           intent(in)  :: tgt_height_msl     ! meters
   // real(8),           intent(in)  :: tgt_lat            ! radians
   // real(8),           intent(in)  :: tgt_lon            ! radians

   // real(8),           intent(out) :: ground_range       ! meters
   // real(8),           intent(out) :: slant_range        ! meters
   // real(8),           intent(out) :: tanept
   // real(8),           intent(out) :: tgt_az             ! radians
   // real(8),           intent(out) :: tgt_el             ! radians
   // real(8),           intent(out) :: tgt_x              ! meters
   // real(8),           intent(out) :: tgt_z              ! meters

   // real(8), optional, intent(in)  :: tgt_attitude       ! radians
   // real(8), optional, intent(in)  :: tgt_bank           ! radians
   // real(8), optional, intent(in)  :: tgt_heading        ! radians

   // real(8), optional, intent(out) :: tgt_az_view        ! radians
   // real(8), optional, intent(out) :: tgt_el_view        ! radians

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double rearth, siphis, cophis, siphit, cophit, dlamda, coscos, cosb, anumer, adenom, betat, rtargt, deltaz, betatp;

   bool ovrflo;

   aXmtrRcvrPtr->GetAntenna()->GetLocationLLA(rdr_lat, rdr_lon, rdr_height_msl);
   WsfEM_ALARM_Terrain::AdjustAltitude(aXmtrRcvrPtr->GetPlatform(), rdr_lat, rdr_lon, rdr_height_msl);
   rdr_lat *= UtMath::cRAD_PER_DEG;
   rdr_lon *= UtMath::cRAD_PER_DEG;

   // If a target pointer was not provided then the supplied values are used.
   if (aTargetPtr != nullptr)
   {
      aTargetPtr->GetLocationLLA(tgt_lat, tgt_lon, tgt_height_msl);
      WsfEM_ALARM_Terrain::AdjustAltitude(aTargetPtr, tgt_lat, tgt_lon, tgt_height_msl);
      tgt_lat *= UtMath::cRAD_PER_DEG;
      tgt_lon *= UtMath::cRAD_PER_DEG;
   }

   //-------------------------------------------------------------------
   // Initialize constants.
   //-------------------------------------------------------------------

   rkfact = aXmtrPtr->GetEarthRadiusMultiplier();
   rearth = rkfact * rezero;
   // NOT-USED double rtenna = rearth + rdr_height_msl;

   //-------------------------------------------------------------------
   // Initialize trigonometric constants.
   //-------------------------------------------------------------------

   siphis = sin(rdr_lat);
   cophis = cos(rdr_lat);
   siphit = sin(tgt_lat);
   cophit = cos(tgt_lat);

   dlamda = tgt_lon - rdr_lon;
   coscos = cophit * cos(dlamda);
   cosb   = coscos * cophis + siphit * siphis;

   //-------------------------------------------------------------------
   // For a site and target located very near one another, round-off
   // errors in the determination of COSB above may result in a
   // value of COSB which is slightly larger than one.  If this
   // occurs, the site and target will be assumed to be co-located,
   // and this case will be handled separately.  Initialize the
   // logical variable OVRFLO to .TRUE. to indicate this case must
   // be handled separately.  If special handling is not necessary,
   // OVRFLO will be reset to .FALSE. in the section that follows.
   //-------------------------------------------------------------------

   ovrflo = true;

   if (cosb < 1.0)
   {
      anumer = cophit * sin(dlamda);
      adenom = siphit * cophis - coscos * siphis;
      if (anumer != 0.0 || adenom != 0.0)
      {
         //-------------------------------------------------------------
         // Reset the logical variable OVRFLO to .FALSE. to indicate
         // that special handling will not be necessary for this case.
         //-------------------------------------------------------------

         ovrflo = false;

         //-------------------------------------------------------------
         // Determine BETAT, in radians, the angle between the position
         // vector to the site and the position vector to the target,
         // assuming the center of the earth is the origin of both
         // vectors.  Determine GROUND_RANGE, in meters, the ground
         // range from the site to the target.  Determine TGT_AZ, in
         // radians, the azimuth angle (true bearing) of the target
         // relative to the site.  The Fortran DATAN2 function will not
         // cause a problem in determining the azimuth angle (true
         // bearing) of the target relative to the site.
         //-------------------------------------------------------------

         betat        = acos(cosb);
         ground_range = rezero * betat;
         tgt_az       = atan2(anumer, adenom);

         //-------------------------------------------------------------
         // Determine BETATP, in radians, the angle between the position
         // vector to the site and the position vector to the target,
         // assuming the center of the earth is the origin of both
         // vectors.  It is also assumed here that the radius of the
         // earth has been modified by the factor RKFACT to account for
         // refraction.
         //-------------------------------------------------------------

         betatp = betat / rkfact;

         //-------------------------------------------------------------
         // Determine TGT_X, in meters, and TGT_Z, in meters, the
         // x-coordinate and z-coordinate, respectively, of the target
         // in an x-z coordinate system whose origin is located on the
         // mean sea level surface of the earth at the site.  The
         // positive x-axis extends from the origin toward the target
         // and is tangent to the earth at the site.  The positve z-axis
         // extends from the origin upward parallel to the radial from
         // the center of the earth to the site.
         //-------------------------------------------------------------

         rtargt = rearth + tgt_height_msl;
         tgt_x  = rtargt * sin(betatp);
         tgt_z  = rtargt * cos(betatp) - rearth;

         //-------------------------------------------------------------
         // Determine DELTAZ, in meters, the difference between the
         // z-coordinate of the target and the z-coordinate of the radar
         // antenna in the x-z coordinate system described above.
         //-------------------------------------------------------------

         deltaz = tgt_z - rdr_height_msl;

         //-------------------------------------------------------------
         // Determine SLANT_RANGE, in meters, the range from the radar
         // antenna to the target.  Determine TANEPT, the tangent of the
         // elevation angle from the radar antenna to the target.  Set
         // the elevation angle from the radar antenna to the target in
         // radians.
         //-------------------------------------------------------------

         slant_range = sqrt(tgt_x * tgt_x + deltaz * deltaz);
         tanept      = deltaz / tgt_x;
         tgt_el      = atan(tanept);
      } // end if
   }    // end if

   //-------------------------------------------------------------------
   // Determine if a problem was discovered earlier which requires
   // special handling.
   //-------------------------------------------------------------------

   if (ovrflo)
   {
      //----------------------------------------------------------------
      // The site and target are located near enough to one another to
      // cause a problem.
      //----------------------------------------------------------------

      //----------------------------------------------------------------
      // Set GROUND_RANGE, in meters, the ground range from the site to
      // the target, to zero.
      //----------------------------------------------------------------

      ground_range = 0.0;

      //----------------------------------------------------------------
      // Set TGT_X, in meters, the x-coordinate of the target in the
      // x-z coordinate system described above, to zero.  Set TGT_Z, in
      // meters, the z-coordinate of the target in the x-z coordinate
      // system described above, to TGT_HEIGHT_MSL, in meters, the
      // height of the target above mean sea level.
      //----------------------------------------------------------------

      tgt_x = 0.0;
      tgt_z = tgt_height_msl;

      //----------------------------------------------------------------
      // Set TGT_AZ, in radians, the azimuth angle (true bearing) of the
      // target relative to the site, to zero.
      //----------------------------------------------------------------

      tgt_az = 0.0;

      if (tgt_height_msl > rdr_height_msl)
      {
         //-------------------------------------------------------------
         // The target is directly above the radar antenna.  Determine
         // SLANT_RANGE, in meters, the range from the radar antenna to
         // the target.  Set TANEPT, the tangent of the elevation angle
         // from the radar antenna to the target, to plus "infinity."
         // Set the elevation angle from the radar antenna to the target
         // in radians to (PI/2).
         //-------------------------------------------------------------

         slant_range = tgt_height_msl - rdr_height_msl;
         tanept      = 1.0E+16;
         tgt_el      = halfpi;
      }
      else if (tgt_height_msl < rdr_height_msl)
      {
         //-------------------------------------------------------------
         // The target is directly below the radar antenna.  Determine
         // SLANT_RANGE, in meters, the range from the radar antenna to
         // the target.  Set TANEPT, the tangent of the elevation angle
         // from the radar antenna to the target, to minus "infinity."
         // Set the elevation angle from the radar antenna to the target
         // in radians to (-PI/2).
         //-------------------------------------------------------------

         slant_range = rdr_height_msl - tgt_height_msl;
         tanept      = -1.0E+16;
         tgt_el      = -halfpi;
      }
      else
      {
         //-------------------------------------------------------------
         // The radar antenna and the target are at the same point in
         // space.  To avoid a division by zero in the target signal
         // computations, set SLANT_RANGE to one meter.  Set TANEPT, the
         // tangent of the elevation angle from the radar antenna to the
         // target, to zero.  Set the elevation angle from the radar
         // antenna to the target in radians to zero.
         //-------------------------------------------------------------

         slant_range = 1.0;
         tanept      = 0.0;
         tgt_el      = 0.0;
      } // endif
   }    // endif
}
} // namespace WsfEM_ALARM_Geometry
