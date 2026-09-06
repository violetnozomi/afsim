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

#include "WsfEM_ALARM_Antenna.hpp"

#include <cmath>

#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_ALARM_Fortran.hpp"
#include "WsfEM_ALARM_Geometry.hpp"
#include "WsfEM_ALARM_Terrain.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
// Various constants from ALARM.
// =================================================================================================

namespace WsfEM_ALARM_Antenna
{
bool sDebug = false;

antenna::antenna(WsfEM_XmtrRcvr* aXmtrRcvrPtr, WsfEM_Interaction& aInteraction, double tgt_az, double tgt_el, double slant_range)
   : mXmtrRcvrPtr(aXmtrRcvrPtr)
   , mInteraction(aInteraction)
   , mXmtrPtr(aInteraction.GetTransmitter())
   , mEBS_Az(0.0)
   , mEBS_El(0.0)
   , az_point_ang_rad(0.0)
   , el_point_ang_rad(0.0)
{
   // This is a mess, but we need to get the pointing angles in the ALARM coordinate system.
   // The only way to do this is to emulate WsfAntenna::ComputeBeamPosition.
   //
   // Note that the electronic beam steering angles will be used as they were computed by WSF.
   //
   // NOTE: ALARM assumes the radar is not pitched or rolled. Azimuth angles are relative to north
   //       and the radar limits are relative to north.

   WsfEM_Antenna*      antennaPtr = mXmtrRcvrPtr->GetAntenna();
   WsfArticulatedPart* partPtr    = antennaPtr->GetArticulatedPart();

   // Get the absolute NED angles of the uncued antenna. Pitch and roll angles are assumed to be zero.

   double heading;
   double notUsed;
   antennaPtr->GetPlatform()->GetOrientationNED(heading, notUsed, notUsed);
   double absUncuedAz = UtMath::NormalizeAngleMinusPi_Pi(heading + partPtr->GetYaw());
   double absUncuedEl = 0.0;

   // Compute the cued position with respect to the uncued position.
   // This imposes any slewing/cueing limitations.

   double cuedAz    = 0.0;
   double cuedEl    = 0.0;
   int    slewState = WsfArticulatedPart::cNOT_CUED;

   if (partPtr->IsCued() && (partPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED))
   {
      // A cue is active (either a direct cue from the user or a transient cue from the sensor scheduler).
      // Note that we do not use the actual cue because we don't know if it has refraction built in (see
      // WsfDefaultSensorSchedule). Thus, we assume that it was desired to point the cue right at the target.

      cuedAz = UtMath::NormalizeAngleMinus180_180(tgt_az - absUncuedAz);
      cuedEl = tgt_el - absUncuedEl;

      //=========================================================================================
      // Start of code extracted from WsfArticulatedPart::UpdateCuedOrientation.
      //=========================================================================================

      // Limit the cue mode with the slew mode.

      int cueMode = static_cast<int>(partPtr->GetCueMode()) & static_cast<int>(partPtr->GetSlewMode());

      // If the device can slew in azimuth then determine the azimuth to be used.

      if ((cueMode & WsfArticulatedPart::cSLEW_AZ) != 0)
      {
         // Make sure the azimuth is within the cue limits

         double minAzCue = std::max(partPtr->GetMinAzCue(), partPtr->GetMinAzSlew());
         double maxAzCue = std::min(partPtr->GetMaxAzCue(), partPtr->GetMaxAzSlew());
         if (cuedAz < minAzCue)
         {
            cuedAz = minAzCue;
            slewState |= WsfArticulatedPart::cMIN_AZ;
         }
         else if (cuedAz > maxAzCue)
         {
            cuedAz = maxAzCue;
            slewState |= WsfArticulatedPart::cMAX_AZ;
         }
         else
         {
            // Point right at the cue (in azimuth)
            // cuedAz = cuedAz;
            slewState |= WsfArticulatedPart::cCUED_AZ;
         }
      }

      // If the device can slew in elevation then determine the elevation to be used.

      if ((cueMode & WsfArticulatedPart::cSLEW_EL) != 0)
      {
         // Make sure the elevation is within the slew limits

         double minElCue = std::max(partPtr->GetMinElCue(), partPtr->GetMinElSlew());
         double maxElCue = std::min(partPtr->GetMaxElCue(), partPtr->GetMaxElSlew());
         if (cuedEl < minElCue)
         {
            cuedEl = minElCue;
            slewState |= WsfArticulatedPart::cMIN_EL;
         }
         else if (cuedEl > maxElCue)
         {
            cuedEl = maxElCue;
            slewState |= WsfArticulatedPart::cMAX_EL;
         }
         else
         {
            // Point right at the cue (in elevation)
            // cuedEl = cuedEl;
            slewState |= WsfArticulatedPart::cCUED_EL;
         }
      }
      else
      {
         cuedEl = partPtr->GetTilt();
      }

      //=========================================================================================
      // End of code extracted from WsfArticulatedPart::UpdateCuedOrientation.
      //=========================================================================================
   }

   double absCuedAz = UtMath::NormalizeAngleMinus180_180(absUncuedAz + cuedAz);
   double absCuedEl = absUncuedEl + cuedEl;

   // Compute the aspect angles of the target with respect to the cued antenna position.

   double aThisToTgtAz = UtMath::NormalizeAngleMinusPi_Pi(tgt_az - absCuedAz);
   double aThisToTgtEl = tgt_el - absCuedEl;

   // If the antenna scans, attempt to point it right at the target.

   WsfEM_Antenna::ScanMode mScanMode = antennaPtr->GetScanMode();
   WsfEM_Antenna::EBS_Mode mEBS_Mode = antennaPtr->GetEBS_Mode();
   double                  mMinAzScan;
   double                  mMaxAzScan;
   antennaPtr->GetAzimuthScanLimits(mMinAzScan, mMaxAzScan);
   double mMinElScan;
   double mMaxElScan;
   antennaPtr->GetElevationScanLimits(mMinElScan, mMaxElScan);

   //============================================================================================
   // Start of code extracted from WsfEM_Antenna::ComputeBeamPosition
   //============================================================================================

   double beamAz = 0.0;
   double beamEl = 0.0;

   // If the sensor only scans in azimuth (like an EW radar) and if the antenna does not use electronic
   // steering in elevation, then apply the tilt angle. It's not applied when using electronic steering in
   // elevation because it is accounted for in the electronic steering computations at the end of the routine.
   if ((mScanMode == WsfEM_Antenna::cSCAN_AZ) &&
       ((mEBS_Mode == WsfEM_Antenna::cEBS_NONE) || (mEBS_Mode == WsfEM_Antenna::cEBS_AZIMUTH)))
   {
      beamEl = antennaPtr->GetPitch() + mXmtrRcvrPtr->GetBeamTilt();
   }

   // If the beam can scan in azimuth then determine the azimuth to be used.

   bool checkAz = false;
   if (mScanMode & WsfEM_Antenna::cSCAN_AZ)
   {
      checkAz = true;

      // Make sure the azimuth is within the scan limits
      if (aThisToTgtAz < mMinAzScan)
      {
         beamAz = mMinAzScan;
      }
      else if (aThisToTgtAz > mMaxAzScan)
      {
         beamAz = mMaxAzScan;
      }
      else
      {
         // Point right at target (in azimuth)
         beamAz = aThisToTgtAz;
      }
   }

   // If the beam can scan in elevation then determine the elevation to be used.

   bool checkEl = false;
   if (mScanMode & WsfEM_Antenna::cSCAN_EL)
   {
      checkEl = true;

      // Make sure the elevation is within the scan limits
      if (aThisToTgtEl < mMinElScan)
      {
         beamEl = mMinElScan;
      }
      else if (aThisToTgtEl > mMaxElScan)
      {
         beamEl = mMaxElScan;
      }
      else
      {
         // Point right at the target (in elevation)
         beamEl = aThisToTgtEl;
      }
   }

   // If necessary, check the beam position against the slew limits
   if (checkAz || checkEl)
   {
      if (slewState != 0)
      {
         WsfArticulatedPart::SlewMode slewMode = partPtr->GetSlewMode();
         if (checkAz && (slewMode & WsfArticulatedPart::cSLEW_AZ))
         {
            double finalAz = cuedAz + beamAz;
            if (finalAz < partPtr->GetMinAzSlew())
            {
               beamAz = partPtr->GetMinAzSlew() - cuedAz;
            }
            else if (finalAz > partPtr->GetMaxAzSlew())
            {
               beamAz = partPtr->GetMaxAzSlew() - cuedAz;
            }
         }
         if (checkEl && (slewMode & WsfArticulatedPart::cSLEW_EL))
         {
            double finalEl = cuedEl + beamEl;
            if (finalEl < partPtr->GetMinElSlew())
            {
               beamEl = partPtr->GetMinElSlew() - cuedEl;
            }
            else if (finalEl > partPtr->GetMaxElSlew())
            {
               beamEl = partPtr->GetMaxElSlew() - cuedEl;
            }
         }
      }
   }

   //============================================================================================
   // End of code extracted from WsfEM_Antenna::ComputeBeamPosition
   //============================================================================================

   // Now compute the absolute pointing angles (scanning offset from cue + cued position).

   az_point_ang_rad = UtMath::NormalizeAngleMinusPi_Pi(beamAz + absCuedAz);
   el_point_ang_rad = beamEl + absCuedEl;

   // The electronic beam steering limits are simply copied. They shouldn't be significantly different,
   // and in most cases EBS is not modeled when using ALARM.

   if (mXmtrRcvrPtr == mXmtrPtr)
   {
      mEBS_Az = aInteraction.mXmtrBeam.mEBS_Az;
      mEBS_El = aInteraction.mXmtrBeam.mEBS_El;
   }
   else
   {
      mEBS_Az = aInteraction.mRcvrBeam.mEBS_Az;
      mEBS_El = aInteraction.mRcvrBeam.mEBS_El;
   }
   if (sDebug)
   {
      auto logger = ut::log::debug() << "Pointing:";
      logger.AddNote() << "Az: " << az_point_ang_rad * rad2deg << " deg";
      logger.AddNote() << "El: " << el_point_ang_rad * rad2deg << " deg";
   }
}

double antenna::get_az_point_ang()
{
   return az_point_ang_rad;
}

double antenna::get_height_agl()
{
   double lat, lon, alt;
   mXmtrRcvrPtr->GetAntenna()->GetLocationLLA(lat, lon, alt);
   return alt - mXmtrRcvrPtr->GetPlatform()->GetTerrainHeight();
}

double antenna::get_height_msl()
{
   double lat, lon, alt;
   mXmtrRcvrPtr->GetAntenna()->GetLocationLLA(lat, lon, alt);
   WsfEM_ALARM_Terrain::AdjustAltitude(mXmtrRcvrPtr->GetPlatform(), lat, lon, alt);
   return alt;
}

void antenna::get_relative_gain(double az_angle, double el_angle, double& rel_gain)
{
   // -----------------------------------------------------------------------
   // From antenna.f90, get_rel_gain_antenna

   double off_az;
   double off_el;
   double delta_az = az_angle - az_point_ang_rad;
   double delta_el = el_angle - el_point_ang_rad;

   if (delta_az >= -pi && delta_az < pi && delta_el >= -halfpi && delta_el < halfpi)
   {
      // For the given geometry the offset angles are equal to the
      // deltas.
      off_az = delta_az;
      off_el = delta_el;
   }
   else
   {
      // The geometry is more complex, call the offbor routine.
      /*call*/ offbor(az_point_ang_rad, el_point_ang_rad, az_angle, el_angle, off_az, off_el);
   } // endif

   // -----------------------------------------------------------------------

   double abs_gain =
      mXmtrRcvrPtr->GetAntennaGain(mXmtrPtr->GetPolarization(), mXmtrPtr->GetFrequency(), off_az, off_el, mEBS_Az, mEBS_El);
   rel_gain = abs_gain / mXmtrRcvrPtr->GetPeakAntennaGain();
   if (sDebug)
   {
      auto logger = ut::log::debug() << "GRG";
      logger.GetStream().precision(16);
      logger.AddNote() << "BOR: " << az_point_ang_rad * rad2deg << ' ' << el_point_ang_rad * rad2deg << " deg";
      logger.AddNote() << "TGT: " << az_angle * rad2deg << ' ' << el_angle * rad2deg << " deg";
      logger.AddNote() << "REL: " << off_az * rad2deg << ' ' << off_el * rad2deg << " deg";
      logger.AddNote() << "RES: " << rel_gain;
   }
}

//---------------------------------------------------------------------
//     $Id: WsfEM_ALARM_Antenna.cpp,v 1.8 2012/04/17 18:41:35 m191713 Exp $
//
//     Purpose:
//     -------
//     Determines the off-boresight angles in azimuth and elevation of
//     an arbitrary point in space with respect to the radar antenna.
//
//
//     Inputs:
//     ------
//     ALPHAR  -  The radar antenna azimuth angle, in radians.
//
//     EPSLNR  -  The radar antenna elevation angle, in radians.
//
//     ALPHAP  -  The azimuth angle to an arbitrary point, in
//                radians.
//
//     EPSLNP  -  The elevation angle to an arbitrary point, in
//                radians.
//
//
//     Outputs:
//     -------
//     OFFAZP  -  The off-boresight angle in azimuth of the
//                arbitrary point in the antenna pattern, in
//                radians.
//
//     OFFELP  -  The off-boresight angle in elevation of the
//                arbitrary point in the antenna pattern, in
//                radians.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7434
//
//     MODIFICATIONS:
//     -------------
//     - Converted to Fortran90. MWS
//---------------------------------------------------------------------
//!! public
void antenna::offbor(double alphar, double epslnr, double alphap, double epslnp, double& offazp, double& offelp)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(in)  :: alphar     // radians
   // real(8), intent(in)  :: epslnr     // radians
   // real(8), intent(in)  :: alphap     // radians
   // real(8), intent(in)  :: epslnp     // radians

   // real(8), intent(out) :: offazp     // radians
   // real(8), intent(out) :: offelp     // radians

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double coscos, cosepp, cosepr, delalp, rdenom, rnumer, sinarg, sinepp, sinepr, sintmp;

   //-------------------------------------------------------------------
   // Determine SINEPR, the sine of the radar antenna elevation
   // angle, COSEPR, the cosine of the radar antenna elevation angle,
   // SINEPP, the sine of the elevation angle to the arbitrary point,
   // and COSEPP, the cosine of the elevation angle to the arbitray
   // point.
   //-------------------------------------------------------------------

   sinepr = sin(epslnr);
   cosepr = cos(epslnr);
   sinepp = sin(epslnp);
   cosepp = cos(epslnp);

   //-------------------------------------------------------------------
   // Determine DELALP, in radians, the difference between the
   // azimuth angle of the point and the azimuth angle of the radar
   // antenna.  Then determine COSCOS, the cosine of the points's
   // elevation angle times the cosine of the difference between the
   // azimuth angle of the point and the azimuth angle of the radar
   // antenna.
   //-------------------------------------------------------------------

   delalp = alphap - alphar;
   coscos = cosepp * cos(delalp);

   //-------------------------------------------------------------------
   // The tangent of the off-boresight angle in azimuth is the ratio
   // of RNUMER and RDENOM, which are now determined.
   //-------------------------------------------------------------------

   rnumer = cosepp * sin(delalp);
   rdenom = coscos * cosepr + sinepp * sinepr;

   //-------------------------------------------------------------------
   // Initialize OFFAZP, in radians, the point's off-boresight angle
   // in azimuth in the antenna pattern, to zero.  In most cases,
   // the Fortran ATAN2 function will be used to determine OFFAZP,
   // the only exception being when both arguments are zero.  This
   // exception occurs when the point's off-boresight angle in
   // elevation is plus or minus ninety degrees, and the off-boresight
   // angle in azimuth is undefined.
   //-------------------------------------------------------------------

   offazp = 0.0;

   if (rnumer != 0.0 || rdenom != 0.0)
   {
      offazp = atan2(rnumer, rdenom);
   }

   //-------------------------------------------------------------------
   // Determine SINTMP, a temporary value for the sine of the off-
   // boresight angle in elevation.  In most cases this value will
   // be between minus one and plus one, the exceptions being when
   // round-off errors in the computation cause the value to be
   // either slightly less than minus one, or slightly greater than
   // plus one.  This exception can occur when the point's off-
   // boresight angle in elevation is very near plus or minus ninety
   // degrees.  Determine SINARG, the sine of the off-boresight angle
   // in elevation, such that it lies between minus one and plus one.
   // Then determine OFFELP, in radians, the point's off-boresight
   // angle in elevation in the antenna pattern.
   //-------------------------------------------------------------------

   sintmp = sinepp * cosepr - coscos * sinepr;
   sinarg = std::min(1.0, std::max(sintmp, -1.0));
   offelp = asin(sinarg);
}

double get_az_point_ang(antenna& ant_data)
{
   return ant_data.get_az_point_ang();
}

double get_height_agl(antenna& ant_data)
{
   return ant_data.get_height_agl();
}

double get_height_msl(antenna& ant_data)
{
   return ant_data.get_height_msl();
}

void get_relative_gain(antenna& ant_data,
                       double   az_angle, // in, radians
                       double   el_angle, // in, radians
                       double&  rel_gain)  // out, absolute
{
   ant_data.get_relative_gain(az_angle, el_angle, rel_gain);
}
} // namespace WsfEM_ALARM_Antenna
