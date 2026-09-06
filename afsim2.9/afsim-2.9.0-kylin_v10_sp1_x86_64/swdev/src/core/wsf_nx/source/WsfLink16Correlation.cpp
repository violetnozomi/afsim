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

#include "WsfLink16Correlation.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfNonExportableUtil.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTypes.hpp"

namespace
{
const double sPositionErrorDataMiles[16] = {
   29.61, //  0, zero and one are not listed, as they typically fall below the minimum allowable TQ
   29.61, //  1
   29.61, //  2, first valid value
   14.78, //  3
   11.82, //  4
   8.87,  //  5
   5.92,  //  6
   2.93,  //  7
   1.18,  //  8
   0.59,  //  9
   0.10,  // 10
   0.05,  // 11
   0.02,  // 12
   0.01,  // 13
   0.006, // 14
   0.003  // 15
};

const double sPPLI_PositionErrorDataMiles[16] = {
   7.38, //  0
   7.38, //  1
   3.69, //  2
   1.85, //  3
   0.92, //  4
   0.65, //  5
   0.46, //  6
   0.33, //  7
   0.23, //  8
   0.16, //  9
   0.12, // 10
   0.08, // 11
   0.06, // 12
   0.04, // 13
   0.03, // 14
   0.02  // 15
};

const double cM_PER_DM = 6000.0 * UtMath::cM_PER_FT;
// const double cDM_PER_M     = 1.0 / cM_PER_DM;
const double cMPS_PER_DMH = cM_PER_DM / 3600.0;
// const double cDMH_PER_MPS  = 1.0 / cMPS_PER_DMH;
} // namespace

WsfLink16Correlation::WsfLink16Correlation()
   : WsfCorrelationStrategy()
   , mWindowSizeMultiplier(1.0)
   , mMinimumWindowSize(0.5 * cM_PER_DM)
   , mMinimumTQ(7)
   , mMaximumTQ(10)
   , mMinimumPPLI_TQ(2)
   , mMaximumPPLI_TQ(11)
   , mRestrictedTQ(4)
   , mCourseDifferential(UtMath::cPI_OVER_4)
   , mMinimumSpeed(10.0 * cMPS_PER_DMH)
   , mSpeedDifferential(40.0)
   , // percent
   mSpeedDeltaAir(200.0 * cMPS_PER_DMH)
   , mSpeedDeltaSurface(20.0 * cMPS_PER_DMH)
   , mAltitudeDifferential(10000.0 * UtMath::cM_PER_FT)
   , mMinDistance(1.0E30)
{
}

// virtual
bool WsfLink16Correlation::ProcessInput(UtInput& aInput)
{
   bool        processed = true;
   std::string command   = aInput.GetCommand();
   // todo
   if (command == "window_size_multiplier")
   {
      aInput.ReadValue(mWindowSizeMultiplier);
      aInput.ValueInClosedRange(mWindowSizeMultiplier, 1.0, 3.0);
   }
   else if (command == "minimum_window_size")
   {
      aInput.ReadValueOfType(mMinimumWindowSize, UtInput::cLENGTH);
      aInput.ValueInClosedRange(mMinimumWindowSize, 0.0, 2.0 * cM_PER_DM);
   }
   else if (command == "minimum_track_quality")
   {
      aInput.ReadValue(mMinimumTQ);
      aInput.ValueInClosedRange(mMinimumTQ, 3U, 7U);
   }
   else if (command == "maximum_track_quality")
   {
      aInput.ReadValue(mMaximumTQ);
      aInput.ValueInClosedRange(mMaximumTQ, 8U, 15U);
   }
   else if (command == "restricted_track_quality")
   {
      aInput.ReadValue(mRestrictedTQ);
      aInput.ValueInClosedRange(mRestrictedTQ, 2U, 6U);
   }
   else if (command == "course_differential")
   {
      aInput.ReadValueOfType(mCourseDifferential, UtInput::cANGLE);
      aInput.ValueInClosedRange(mCourseDifferential, 15.0 * UtMath::cRAD_PER_DEG, UtMath::cPI_OVER_2);
   }
   else if (command == "percent_speed_differential")
   {
      aInput.ReadValue(mSpeedDifferential);
      aInput.ValueInClosedRange(mSpeedDifferential, 10.0, 100.0);
   }
   else if (command == "altitude_differential")
   {
      aInput.ReadValueOfType(mAltitudeDifferential, UtInput::cLENGTH);
      aInput.ValueInClosedRange(mAltitudeDifferential, 5000.0 * UtMath::cM_PER_FT, 50000.0 * UtMath::cM_PER_FT);
   }
   else if (command == "minimum_ppli_track_quality")
   {
      aInput.ReadValue(mMinimumPPLI_TQ);
      aInput.ValueInClosedRange(mMinimumPPLI_TQ, 1U, 5U);
   }
   else if (command == "maximum_ppli_track_quality")
   {
      aInput.ReadValue(mMaximumPPLI_TQ);
      aInput.ValueInClosedRange(mMaximumPPLI_TQ, 1U, 15U);
   }
   // Note: the following are not explicitly denoted as variable in the 6016 spec
   else if (command == "minimum_speed")
   {
      aInput.ReadValueOfType(mMinimumSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinimumSpeed, 0.0);
   }
   else if (command == "air_delta_speed")
   {
      aInput.ReadValueOfType(mSpeedDeltaAir, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mSpeedDeltaAir, 0.0);
   }
   else if (command == "surface_delta_speed")
   {
      aInput.ReadValueOfType(mSpeedDeltaSurface, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mSpeedDeltaSurface, 0.0);
   }
   else
   {
      processed = WsfCorrelationStrategy::ProcessInput(aInput);
   }
   return processed;
}

// virtual
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
WsfLocalTrack* WsfLink16Correlation::CorrelateImpl(double aSimTime, const WsfTrack& aNewTrack, WsfLocalTrackList& aTrackList)
{
   mMinDistance                       = 1.0E30; // reset min distance
   double         currentBestDistance = mMinDistance;
   WsfLocalTrack* correlatedTrackPtr  = nullptr;
   for (unsigned trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
   {
      WsfLocalTrack* candidateTrackPtr = aTrackList.GetTrackEntry(trackNum);
      if (candidateTrackPtr->IsFusible() && RunCorrelationTest(aSimTime, aNewTrack, *candidateTrackPtr))
      {
         if (mMinDistance < currentBestDistance)
         {
            correlatedTrackPtr = candidateTrackPtr;
         }
      }
   }
   return correlatedTrackPtr;
}

bool WsfLink16Correlation::RunCorrelationTest(double aSimTime, const WsfTrack& aNewTrack, const WsfLocalTrack& aLocalTrack)
{
   // cout<<"<Correlator::RunCorrelationTest>"<<endl;
   //  Retrieve new track attributes
   int envNew = aNewTrack.GetSpatialDomain();
   // int tqNew  = aNewTrack.GetTrackQuality();
   int tqNew = 0;
   if (aNewTrack.GetAuxDataConst().AttributeExists("link_16_track_quality"))
   {
      tqNew = aNewTrack.GetAuxDataConst().GetInt("link_16_track_quality");
   }
   else
   {
      tqNew = static_cast<int>(WsfNonExportableUtil::GetLink16_TrackQuality(aSimTime, aNewTrack) * 15.0);
   }

   // Retrieve current MTL track attributes
   int env = aLocalTrack.GetSpatialDomain();
   int tq  = static_cast<int>(WsfNonExportableUtil::GetLink16_TrackQuality(aSimTime, aLocalTrack) * 15.0);

   // Not sure why the following was here.
   // if (idNew == 0) {
   //   return false;
   //}
   // Check for same environment category,
   if (envNew != env)
   {
      return false;
   }
   // Check for environment not subsurface
   if (envNew == WSF_SPATIAL_DOMAIN_SUBSURFACE)
   {
      return false;
   }

   // Check for hostile/friend identity conflict
   // if (((idNew == TrackFusionTypes::HOSTILE) && (id == TrackFusionTypes::FRIENDLY)) || ((idNew ==
   // TrackFusionTypes::FRIENDLY) && (id == TrackFusionTypes::HOSTILE))) {
   //   return false;
   //}
   if (aNewTrack.IFF_StatusValid() && aLocalTrack.IFF_StatusValid())
   {
      WsfTrack::IFF_Status idNew = aNewTrack.GetIFF_Status();
      WsfTrack::IFF_Status id    = aLocalTrack.GetIFF_Status();
      if (((idNew == WsfTrack::cIFF_FOE) && (id == WsfTrack::cIFF_FRIEND)) ||
          ((idNew == WsfTrack::cIFF_FRIEND) && (id == WsfTrack::cIFF_FOE)))
      {
         return false;
      }
   }

   if (!AltitudeTest(aNewTrack, aLocalTrack))
   {
      return false;
   }

   // Check course
   if (!CourseTest(aNewTrack, aLocalTrack))
   {
      return false;
   }

   // Check speed
   if (!SpeedTest(aNewTrack, aLocalTrack))
   {
      return false;
   }

   // Check position
   // Calculate distance between tracks in ft.
   double deltaDist = GetDistanceBetweenTracks(aLocalTrack, aNewTrack);

   // Check for failure of position test
   // if (aNewTrack.GetTrackType() == TrackPack::TrackTypeSelftold)
   if (aNewTrack.GetAuxDataConst().AttributeExists("source_track_number")) // PPLI from STN
   {
      if (!PositionalTest(tqNew, tq, deltaDist, mMinimumPPLI_TQ, mMaximumPPLI_TQ, sPPLI_PositionErrorDataMiles))
      {
         return false;
      }
   }
   else
   {
      if (!PositionalTest(tqNew, tq, deltaDist, mMinimumTQ, mMaximumTQ, sPositionErrorDataMiles))
      {
         return false;
      }
   }

   // Check correlation restrictions
   // mjm not worrying about this now.
   // int mode2New = 0;
   // if (aNewTrack.GetTrackIff())
   //{
   //   int mode2    = aNewTrack.GetTrackIff()->GetIffModeCode(TrackPack::IffMode2);
   //   if (mode2 > -1)
   //   {
   //      mode2New = mode2;
   //   }
   //}

   // int mode2lcl = 0;
   // if (aLocalTrack.GetTrackIff())
   //{
   //    int mode2 = aLocalTrack.GetTrackIff()->GetIffModeCode(TrackPack::IffMode2);
   //    if (mode2 > -1)
   //    {
   //       mode2lcl = mode2;
   //    }
   // }
   // if (CorrelationRestrictionDifference(idNew, mode2New, id, mode2lcl) == OPERATOR_ALERT)
   //{
   //    return false;
   // }

   // Check delta distance for a closer match
   if (deltaDist < mMinDistance)
   {
      // Save off nearest track data
      mMinDistance = deltaDist;
      return true;
   }
   return false;
}

bool WsfLink16Correlation::RunDecorrelationTest(double aSimTime, const WsfTrack& aNewTrack, const WsfLocalTrack& aLocalTrack)
{
   return false;
}

// PositionalTest Equation from MIL-STD-6016C
// Table 4.11-5 : Summary of Correlation Tests
//
// D <= [a*SQRT(L^2 + R^2) + b] (dm = data mile),  where...
//    D = Distance in dm between two tracks
//    a = "windowsize multiplier" parameter; "a = windowSize"
//    L = MIN([MAX(E(localTQ), E(d))], E(c)) "c = minTQ"  "d = maxTQ"
//    R = MIN([MAX(E(remoteTQ), E(d))], E(c)) "c = minTQ"  "d = maxTQ"
//
//    E(localTQ)  = positional error associated with local track quality
//                  localTQ from MIL-STD-6016C : Table 4.11-6
//    E(remoteTQ) = positional error associated with remote track quality
//                  remoteTQ from MIL-STD-6016C : Table 4.11-6, or
//                  remote geodetic position quality Qpg from Table 4.4-4
//    E(c)        = positional error with TQ = c, "MIN TQ" parameter,
//                  from Table 4.4-2 or Qpg = j,
//                  "MIN Qpg" parameter, from Table 4.11-6 "c = minTQ"
//
//    E(d)        = positional error associated with TQ = d,
//                  "MAX TQ" parameter, from Table 4.11-6, or Qpg = k,
//                  "MAX Qpg" parameter, from Table 4.11-6 "d = maxTQ"
//    b           = "minimum window size" parameter  "b =  minWindowSize"
//
// Return values:  0 = failure
//                 non-zero = pass with distance between two tracks (D)
//
bool WsfLink16Correlation::PositionalTest(unsigned int  aLocalTQ,
                                          unsigned int  aRemoteTQ,
                                          double        aDistance,
                                          unsigned int  aMinTQ,
                                          unsigned int  aMaxTQ,
                                          const double* aTQ_ToDM_ConversionTable,
                                          double        aScaleFactor) const
{
   bool testPass = false;

   // Make sure we have a valid TQ or the test has no value.
   if (aLocalTQ > 0 && aRemoteTQ > 0)
   {
      // Limit the local track TQ
      if (aLocalTQ > aMaxTQ)
      {
         aLocalTQ = aMaxTQ;
      }
      else if (aLocalTQ < aMinTQ)
      {
         aLocalTQ = aMinTQ;
      }

      // Limit the remote track TQ
      if (aRemoteTQ > aMaxTQ)
      {
         aRemoteTQ = aMaxTQ;
      }
      else if (aRemoteTQ < aMinTQ)
      {
         aRemoteTQ = aMinTQ;
      }

      double L           = aTQ_ToDM_ConversionTable[aLocalTQ] * cM_PER_DM;
      double R           = aTQ_ToDM_ConversionTable[aRemoteTQ] * cM_PER_DM;
      double deltaTracks = mWindowSizeMultiplier * sqrt(L * L + R * R) + mMinimumWindowSize;

      testPass = ((aScaleFactor * aDistance) <= deltaTracks);
   }

   return testPass;
}

//! Helper method for CourseTest; assumes valid velocity and location.
double WsfLink16Correlation::GetCourse(const WsfTrack& aTrack) const
{
   double course = 0.0;
   if (aTrack.GetAuxDataConst().AttributeExists("link16_course"))
   {
      course = aTrack.GetAuxDataConst().GetDouble("link16_course");
   }
   else
   {
      double velWCS[3];
      aTrack.GetVelocityWCS(velWCS);
      double locWCS[3];
      aTrack.GetLocationWCS(locWCS);
      mConverter.SetLocationWCS(locWCS);
      mConverter.SetVelocityWCS(velWCS);
      double velNED[3];
      mConverter.GetVelocityNED(velNED);
      course = atan2(velNED[1], velNED[0]) * UtMath::cDEG_PER_RAD;
   }
   return course;
}

// If both tracks have speed >= 10 dmh
// MIN(|remoteCourse - localCourse|), 360 - |remoteCourse - localCourse|) <= f
//
// localCourse  = local course in degrees
// remoteCourse = remote course in degrees
// f            = "course differential" parameter
bool WsfLink16Correlation::CourseTest(const WsfTrack& aRemoteTrack, const WsfTrack& aLocalTrack) const
{
   // Default this test to true so that if the minimum
   // speed is not met, this test will not be used to
   // determine the correlation between the two tracks.
   bool testPass = true;
   if (aRemoteTrack.VelocityValid() && aLocalTrack.VelocityValid())
   {
      double localSpeed  = aLocalTrack.GetSpeed();
      double remoteSpeed = aRemoteTrack.GetSpeed();

      if (localSpeed >= mMinimumSpeed && (remoteSpeed >= mMinimumSpeed) && aRemoteTrack.LocationValid() &&
          aLocalTrack.LocationValid())
      {
         // Course must be in degrees
         double localCourse  = GetCourse(aLocalTrack) * UtMath::cRAD_PER_DEG;
         double remoteCourse = GetCourse(aRemoteTrack) * UtMath::cRAD_PER_DEG;
         double deltaAngle   = fabs(localCourse - remoteCourse);
         double minAngle     = std::min(deltaAngle, UtMath::cTWO_PI - deltaAngle);
         testPass            = (minAngle <= mCourseDifferential);
      }
   }
   return testPass;
}

// |1-(spdSlowerTrack / spdFasterTrack)| * 100% <= MAX(g, [g(k/spdFasterTrack)])
//
// spdSlowerTrack = speed of the slower track in dmh
// spdFasterTrack = speed of the faster track in dmh
// g = "speed differential"parameter     "g = speedDifferential"
// k = alternate speed test factor       "k = maxQpg"
//   = 200 dmh for air tracks
//   = 20 dmh for surface tracks
bool WsfLink16Correlation::SpeedTest(const WsfTrack& aRemoteTrack, const WsfTrack& aLocalTrack) const
{
   bool testPass = false;

   if (aRemoteTrack.VelocityValid() && aLocalTrack.VelocityValid())
   {
      // Need to check these values against what the tracker and
      // DL are actually sending.  The case value do not seem correct
      double k = 0.0;
      switch (aRemoteTrack.GetSpatialDomain())
      {
      // Air tracks
      case WSF_SPATIAL_DOMAIN_AIR:
      case WSF_SPATIAL_DOMAIN_SPACE:
      {
         k        = mSpeedDeltaAir;
         testPass = true;
         break;
      }
      // Surface tracks
      case WSF_SPATIAL_DOMAIN_SURFACE:
      case WSF_SPATIAL_DOMAIN_SUBSURFACE:
      case WSF_SPATIAL_DOMAIN_LAND:
      {
         k        = mSpeedDeltaSurface;
         testPass = true;
         break;
      }
      // All other tracks
      default:
         break;
      }

      if (testPass)
      {
         double localSpeed     = aLocalTrack.GetSpeed();
         double remoteSpeed    = aRemoteTrack.GetSpeed();
         double spdSlowerTrack = std::min(localSpeed, remoteSpeed);
         double spdFasterTrack = std::max(localSpeed, remoteSpeed);

         // Compute the maximum speed % difference
         double perctDiff = std::max(mSpeedDifferential, mSpeedDifferential * (k / spdFasterTrack));

         testPass = (fabs(1 - (spdSlowerTrack / spdFasterTrack)) * 100.0) <= perctDiff;
      }
   }
   else
   {
      testPass = true; // not enough info
   }

   return testPass;
}

// For air tracks, if both tracks have an Altitude Source = 1 or 3, or if the
// local track Altitude Source is 1 or 3 and the remote track is an IU reported
// in a PPLI message:
//
//    |remoteAlt - localAlt| <= h
//
// localAlt = local altitude in kft
// remoteAlt = remote altitude in kft
// h = "altitude differential" parameter
bool WsfLink16Correlation::AltitudeTest(const WsfTrack& aNewTrack, const WsfTrack& aLocalTrack) const
{
   // Default is true in case the altitude source is invalid for the test.
   bool testPass = true;

   // Do we need to make sure the data are available from the altitude source flag in the L16 message (TODO)?
   if (aNewTrack.Is3D() && aLocalTrack.Is3D() && (aNewTrack.GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR) &&
       (aLocalTrack.GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR))
   {
      // Check altitude
      double lat1, lon1, alt1, lat2, lon2, alt2;
      aNewTrack.GetLocationLLA(lat1, lon1, alt1);
      aLocalTrack.GetLocationLLA(lat2, lon2, alt2);

      // Altitude source must be from sensor or automatic altitude report
      // in order to use altitude as valid correlation test.
      // Note: check for 3D track prior to doing the altitude test.
      double deltaAlt = alt1 - alt2;
      testPass        = (fabs(deltaAlt) <= mAltitudeDifferential);
   }

   return testPass;
}

double WsfLink16Correlation::GetDistanceBetweenTracks(const WsfTrack& aTrack1, const WsfTrack& aTrack2) const
{
   if (aTrack1.LocationValid() && aTrack2.LocationValid())
   {
      double lat1, lon1, alt1, lat2, lon2, alt2;
      aTrack1.GetLocationLLA(lat1, lon1, alt1);
      aTrack2.GetLocationLLA(lat2, lon2, alt2);

      double dist    = 99999999.9;
      double heading = 0.0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(lat1,
                                                      lon1,
                                                      lat2,
                                                      lon2,
                                                      heading, // degrees, not used
                                                      dist);   // meters
      return dist;
   }
   return 99999999.9;
}
