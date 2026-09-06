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

#include "WsfDirectionFinderProcessor.hpp"

#include <limits>
#include <string>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtLatPos.hpp"
#include "UtLineSegment.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCovariance.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfKalmanFilter.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfUtil.hpp"

#ifdef DEBUG_DIR_FIND_VERBOSE
#include "Y:\DebugEkf.h"
#endif

WsfDirectionFinderProcessor::WsfDirectionFinderProcessor(WsfScenario& aScenario)
   : WsfLinkedProcessor(aScenario)
   , mTrackList()
   , mFusedTrackList()
   , mCorrelation()
   , mPrototypeFilterPtr(nullptr)
   , mTerrainPtr(nullptr)
   , mReferencePoint()
   , mCenterLat(0.0)
   , mCenterLon(0.0)
   , mCenterRad(0.0)
   , mFuseAllCollects(false)
   , mUseTruthAltitude(false)
   , mPurgeInterval(0.0)
   , mMaxExpectedError(1.0e+10)
   , mMinBaselineDistance(10000.0)
   , mTest(false)
   , mMaxTimeDifference(UtMath::cDOUBLE_MAX)
   , mBypassFilter(false)
   , mUseStatisticalLocation(false)
   , mRandom()
{
}

WsfDirectionFinderProcessor::WsfDirectionFinderProcessor(const WsfDirectionFinderProcessor& aSrc)
   : WsfLinkedProcessor(aSrc)
   , mTrackList()
   , mFusedTrackList()
   , mCorrelation(aSrc.mCorrelation)
   , mPrototypeFilterPtr(aSrc.mPrototypeFilterPtr)
   , mTerrainPtr(nullptr)
   , mReferencePoint(aSrc.mReferencePoint)
   , mCenterLat(aSrc.mCenterLat)
   , mCenterLon(aSrc.mCenterLon)
   , mCenterRad(aSrc.mCenterRad)
   , mFuseAllCollects(aSrc.mFuseAllCollects)
   , mUseTruthAltitude(aSrc.mUseTruthAltitude)
   , mPurgeInterval(aSrc.mPurgeInterval)
   , mMaxExpectedError(aSrc.mMaxExpectedError)
   , mMinBaselineDistance(aSrc.mMinBaselineDistance)
   , mTest(aSrc.mTest)
   , mMaxTimeDifference(aSrc.mMaxTimeDifference)
   , mBypassFilter(aSrc.mBypassFilter)
   , mUseStatisticalLocation(aSrc.mUseStatisticalLocation)
   , mRandom(aSrc.mRandom)
{
}

// virtual
WsfDirectionFinderProcessor::~WsfDirectionFinderProcessor()
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "WsfDirectionFinderProcessor Finished.";
      out.AddNote() << "Direction Finder: " << GetPlatform()->GetName();
      out.AddNote() << "Remaining collects: " << mTrackList.size();
   }
   delete mTerrainPtr;
}

// virtual
bool WsfDirectionFinderProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   WsfFilter*  filterPtr = nullptr;
   std::string command   = aInput.GetCommand();

   if ((command == "fuse_all_collects") || (command == "fuse_all_measurements"))
   {
      aInput.ReadValue(mFuseAllCollects);
   }
   else if ((command == "measurement_replacement_interval") || (command == "collect_replacement_interval"))
   {
      aInput.ReadValueOfType(mPurgeInterval, UtInput::cTIME);
   }
   else if (command == "maximum_expected_error")
   {
      aInput.ReadValueOfType(mMaxExpectedError, UtInput::cLENGTH);
      aInput.ValueGreater(mMaxExpectedError, 0.0);
   }
   else if (command == "use_truth_altitude")
   {
      aInput.ReadValue(mUseTruthAltitude);
   }
   else if (command == "use_statistical_location")
   {
      aInput.ReadValue(mUseStatisticalLocation);
   }
   else if (command == "test")
   {
      aInput.ReadValue(mTest);
   }
   else if (WsfFilterTypes::Get(GetScenario()).LoadInstance(aInput, filterPtr))
   {
      delete mPrototypeFilterPtr;
      mPrototypeFilterPtr = filterPtr;
   }
   else if (command == "maximum_time_difference")
   {
      aInput.ReadValueOfType(mMaxTimeDifference, UtInput::cTIME);
      aInput.ValueGreater(mMaxTimeDifference, 0.0);
   }
   else if (command == "minimum_baseline_distance")
   {
      aInput.ReadValueOfType(mMinBaselineDistance, UtInput::cLENGTH);
      aInput.ValueGreater(mMinBaselineDistance, 0.0);
   }
   else if (command == "filter_bypass")
   {
      mBypassFilter = true;
   }
   else
   {
      myCommand = WsfLinkedProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfDirectionFinderProcessor::Initialize(double aSimTime)
{
   mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   mCorrelation.Initialize(&GetPlatform()->GetTrackManager());

   // Initialize the random number stream.  Each instance has a different seed.
   unsigned int seed = GetSimulation()->GetRandom().Uniform<unsigned>();
   mRandom.SetSeed(seed);

   return WsfLinkedProcessor::Initialize(aSimTime);
}

// virtual
bool WsfDirectionFinderProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   WsfStringId type(aMessage.GetType());
   if (type == WsfTrackMessage::GetTypeId())
   {
      const WsfTrackMessage& message  = static_cast<const WsfTrackMessage&>(aMessage);
      WsfTrack*              trackPtr = message.GetTrack();

      // We don't want to ignore all tracks for this platform, we just want ignore tracks that originated
      // in this process.  A problem occurs when there are two comms, an input comm
      // and output - there is no way to limit the output
      // to JUST go to a comm, the commander.  It is routed back to all on-board processes, including this.
      //   It really looks like off-board tracks are getting re-id'ed with this platform's originator ID?
      // So we can use origin-ID.  We want the same target but not the same (host-based) ID, and we want same time stamp
      {
         // Don't re-process the same host-ID and time
         auto it = std::find_if(std::begin(mTrackList),
                                std::end(mTrackList),
                                [trackPtr](const std::unique_ptr<WsfTrack>& aTrackPtr)
                                {
                                   return (trackPtr->GetTrackId() == aTrackPtr->GetTrackId()) &&
                                          (aTrackPtr->GetUpdateTime() == trackPtr->GetUpdateTime());
                                });

         if (it != std::end(mTrackList))
         {
            return true;
         }
      }

      // Correlate the new track update with an existing track
      WsfLocalTrack* fusedTrackPtr = mCorrelation.Correlate(aSimTime, *trackPtr, mFusedTrackList);
      if (fusedTrackPtr != nullptr)
      {
         bool fused = false;
         for (auto tIter = mTrackList.begin(); tIter != mTrackList.end();)
         {
            if ((*tIter)->GetTargetName() == trackPtr->GetTargetName())
            {
               FuseResult fuseResult = FuseBearingElevation(*fusedTrackPtr, **tIter, *trackPtr);
               fused                 = fuseResult == FUSE_SUCCESS;
#ifdef DEBUG_DIR_FIND_VERBOSE
               std::string senderXXXX = aMessage.GetOriginator();
               DebugEkf::DebugFuser::WriteFuseTry(aSimTime, GetPlatform(), savedTrackPtr, trackPtr, ToString(fuseResult));
#endif
               if (fused)
               {
                  FuseSideType(*fusedTrackPtr, **tIter);
                  FuseSideType(*fusedTrackPtr, *trackPtr);

                  // send along
                  WsfTrackMessage msg(GetPlatform(), *fusedTrackPtr);
                  SendMessage(aSimTime, msg);

                  // Note: we delete the track here in order to make sure we have two
                  // new measurements to evaluate.
                  tIter = mTrackList.erase(tIter);
                  break; // We've found a successful pair
               }
               else if ((mPurgeInterval > 0.0) &&
                        (GetSimulation()->GetSimTime() - (*tIter)->GetUpdateTime() > mPurgeInterval))
               {
                  tIter = mTrackList.erase(tIter);
               }
               else
               {
                  ++tIter;
               }
            }
            else
            {
               ++tIter;
            }
         }

         if (!fused)
         {
            // save the track for later evaluation
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Direction Finder: Saving track from platform.";
               out.AddNote() << "Platform: " << trackPtr->GetOriginatorNameId();
               out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
            }
            mTrackList.emplace_back(trackPtr->Clone());
         }
      }
      else // Track was not found in fused track list (not correlated).
      {
         CreateLocalTrack(*trackPtr);

         // PH: The first track comes through here.  The next track (above) find the "fused" track, but
         //  it doesn't have the origin to triangulate with, if it isn't saved.  If we add the origin here, the
         //  next report will triangulate into this fused track.
         // TODO: I haven't turned this on yet, because it needs to be tested!!!
         // bool fuse1stAlso = !true;
         // if (fuse1stAlso)
         //{
         //   auto out = ut::log::debug() << "Direction Finder: Saving track from platform.";
         //   out.AddNote() << "Platform: " << trackPtr->GetOriginatorNameId();
         //   out.AddNote() << "Track: " << trackPtr->GetTrackId();
         //   mTrackList.push_back(trackPtr->Clone());
         //}
      }
   }
   return true;
}

WsfLocalTrack* WsfDirectionFinderProcessor::CreateLocalTrack(const WsfTrack& aTrack)
{
   auto fusedTrackPtr = ut::make_unique<WsfLocalTrack>(aTrack);
   fusedTrackPtr->Initialize(aTrack.GetUpdateTime(), GetPlatform()->GetNextTrackId(), *GetSimulation());
   fusedTrackPtr->Correlate(aTrack.GetTrackId());
   fusedTrackPtr->SetTargetName(aTrack.GetTargetName());
   if (DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Direction Finder: Creating track.";
      logDebug.AddNote() << "Track ID: " << fusedTrackPtr->GetTrackId();
      logDebug.AddNote() << "Target: " << fusedTrackPtr->GetTargetName();
      logDebug.AddNote() << "Originator: " << aTrack.GetOriginatorNameId();
   }

   auto* tempPtr = fusedTrackPtr.get();
   mFusedTrackList.AddTrack(std::move(fusedTrackPtr));
   return tempPtr;
}

//! Use a triangulation algorithm to fuse tracks from two tracks containing
//! only bearing and maybe elevation data.
//! @param aFusedTrack The fused track that is the result of the triangulation between the two tracks,
//! taking into account previous results in the error computation.
//! @param aTrack1 One of two bearing-only tracks that are considered in the fusion.
//! @param aTrack2 The other bearing-only track considered in the fusion of two reports.
//! @note This algorithm uses the truth location of the target and calculated error values to determine a
//! the fused target location.  It is very difficult to remove non-linearities and effects of slowly-rotating NED
//! coordinate systems in order to properly implement the curved earth triangulation algorithm.  So this effects-based
//! algorithm is implemented instead.
WsfDirectionFinderProcessor::FuseResult WsfDirectionFinderProcessor::FuseBearingElevation(WsfLocalTrack& aFusedTrack,
                                                                                          WsfTrack&      aTrack1,
                                                                                          WsfTrack&      aTrack2)
{
   // We need two independent measurements to triangulate.
   // bool isTriangulated = true;

   // PH: When the target was stationary, subsequent time reports still triangulated
   // to the same point.  With a moving target, we need to time-align the data.  In keeping
   // with the effects-based algorithm, we replace the stale track with a same-time track,
   // so the two incoming measurements are valid at the same time.  Then, the same algorithm
   // below remains valid.
   double t1 = aTrack1.GetUpdateTime();
   double t2 = aTrack2.GetUpdateTime();

   if (!UtMath::NearlyEqual(t1, t2, mMaxTimeDifference))
   {
#ifdef DEBUG_DIR_FIND_VERBOSE
      // Write a line so comparable files have same # lines
      DebugEkf::WriteNoFused(aTrack1, aTrack2, aFusedTrack);
#endif
      return NO_FUSE_NON_SYNC;
   }

   // Process the tracks such that the first track has the smaller update time.
   WsfTrack* tmpTrack1Ptr = &aTrack1;
   WsfTrack* tmpTrack2Ptr = &aTrack2;

   if (t1 > t2)
   {
      tmpTrack1Ptr = &aTrack2;
      tmpTrack2Ptr = &aTrack1;
   }

   // References to preserve some of the syntax below.
   WsfTrack& tmpTrack1 = *tmpTrack1Ptr;
   WsfTrack& tmpTrack2 = *tmpTrack2Ptr;

   // Find equivalent wcs vectors (getting everything into the same coordinate system).
   double origin2[3];
   tmpTrack2.GetOriginatorLocationWCS(origin2);

   double origin1[3];
   tmpTrack1.GetOriginatorLocationWCS(origin1);

   double baselineVecWCS[3];
   UtVec3d::Subtract(baselineVecWCS, origin2, origin1);

   // If not fuse all, means that some are filtered out before trying
   if (!mFuseAllCollects)
   {
      double baselineDistance = UtVec3d::Magnitude(baselineVecWCS);
      if (aFusedTrack.LocationValid())
      {
         double targetLocationWCS[3];
         aFusedTrack.GetLocationWCS(targetLocationWCS);

         // Get the interior angle between the two range vectors.
         // this will lead to a condition factor we can use to determine
         // if our results are good enough to continue.
         double targetVecWCS1[3];
         double targetVecWCS2[3];
         UtVec3d::Subtract(targetVecWCS1, origin1, targetLocationWCS);
         UtVec3d::Subtract(targetVecWCS2, origin2, targetLocationWCS);
         double range1           = UtVec3d::Magnitude(targetVecWCS1);
         double range2           = UtVec3d::Magnitude(targetVecWCS2);
         double cosInteriorAngle = UtMath::NormalizeSinCosMinusOne_One(
            (baselineDistance * baselineDistance - range1 * range1 - range2 * range2) / (-2.0 * range1 * range2));
         double interiorAngle  = acos(cosInteriorAngle);
         double thresholdAngle = 5.0 * std::max(tmpTrack1.GetBearingError(), tmpTrack2.GetBearingError());
         if (interiorAngle < thresholdAngle)
         {
            if (mTest)
            {
               auto out = ut::log::info() << "Direction Finder: Pair did not pass angle threshold test.";
               out.AddNote() << "Pair: " << GetPlatform()->GetName() << ":" << GetName();
               out.AddNote() << "Interior Angle: " << interiorAngle;
               out.AddNote() << "Threshold: " << thresholdAngle;
            }
            return NO_FUSE_ANGLE_THRESHOLD;
         }
      }
      else
      {
         // This is a "minimum" baseline test, as in don't triangulate is sensors are superimposed.
         if (baselineDistance < mMinBaselineDistance)
         {
            if (mTest)
            {
               auto out = ut::log::info() << "Direction Finder: Pair did not pass baseline distance threshold test.";
               out.AddNote() << "Pair: " << GetPlatform()->GetName() << ":" << GetName();
               out.AddNote() << "Baseline Distance: " << baselineDistance;
               out.AddNote() << "Threshold: " << mMinBaselineDistance;
            }
            return NO_FUSE_BASELINE_DIST;
         }
      }
   }

   bool computeElevation = (!tmpTrack1.ElevationValid()) && (!tmpTrack2.ElevationValid());

   // Create WCS vectors for each track.
   double vecNED1[3];
   double vecNED2[3];
   double elevation1 = 0.0;
   double elevation2 = 0.0;
   if (tmpTrack1.ElevationValid())
   {
      elevation1 = tmpTrack1.GetElevation();
   }

   if (tmpTrack2.ElevationValid())
   {
      elevation2 = tmpTrack2.GetElevation();
   }

   GetNED_Vector(tmpTrack1.GetBearing(), elevation1, vecNED1);
   GetNED_Vector(tmpTrack2.GetBearing(), elevation2, vecNED2);

   // Now calculate the range error vectors (preliminary to calculating range error).
   double vec1NEDErr1[3];
   double vec1NEDErr2[3];

   GetNED_Vector(tmpTrack1.GetBearing() + tmpTrack1.GetBearingError(), elevation1, vec1NEDErr1);
   GetNED_Vector(tmpTrack2.GetBearing() + tmpTrack2.GetBearingError(), elevation2, vec1NEDErr2);

   double vec2NEDErr1[3];
   double vec2NEDErr2[3];

   GetNED_Vector(tmpTrack1.GetBearing() - tmpTrack1.GetBearingError(), elevation1, vec2NEDErr1);
   GetNED_Vector(tmpTrack2.GetBearing() - tmpTrack2.GetBearingError(), elevation2, vec2NEDErr2);

   mReferencePoint.SetLocationWCS(origin2);
   double vecWCS2[3];
   mReferencePoint.ConvertNEDToWCS(vecNED2, vecWCS2);
   double vec1WCSErr2[3];
   double vec2WCSErr2[3];
   mReferencePoint.ConvertNEDToWCS(vec1NEDErr2, vec1WCSErr2);
   mReferencePoint.ConvertNEDToWCS(vec2NEDErr2, vec2WCSErr2);

   mReferencePoint.SetLocationWCS(origin1);
   double vecWCS1[3];
   mReferencePoint.ConvertNEDToWCS(vecNED1, vecWCS1);
   double vec1WCSErr1[3];
   double vec2WCSErr1[3];
   mReferencePoint.ConvertNEDToWCS(vec1NEDErr1, vec1WCSErr1);
   mReferencePoint.ConvertNEDToWCS(vec2NEDErr1, vec2WCSErr1);

   UtLineSegment segWCS1(origin1, vecWCS1);
   UtLineSegment segWCS2(origin2, vecWCS2);

   double locationWCS[3];
   bool   success = GetIntersectionPoint(segWCS1, segWCS2, locationWCS, true);

   if (!success)
   {
      if (mTest)
      {
         auto out = ut::log::info() << "Direction Finder: No intersection of target lines. No fusion.";
         out.AddNote() << "Pair: " << GetPlatform()->GetName() << ":" << GetName();
      }
      return NO_FUSE_NO_INTERSECTION;
   }

   // New check for divergence, wrong quadrant.  Just see if the intercept point
   // is behind the baseline.  Use track 1 azimuth to new
   double dir1toInterceptPointWCS[3];
   UtVec3d::Subtract(dir1toInterceptPointWCS, locationWCS, origin1);
   double dir1MeasurmentWCS[3];

   // Note: the vector "vecWCS1" is a point, not dir (after converting from NED)
   UtVec3d::Subtract(dir1MeasurmentWCS, vecWCS1, origin1);

   // It's possible (mathematically) that the IP is on the sensor origin, which means there is no vector.
   // The magnitude of the other vector (vecWCS1) is determined in a local function (arbitrary 10k? visual?)
   double mag2Ip   = UtVec3d::Magnitude(dir1toInterceptPointWCS);
   double mag2Mt   = UtVec3d::Magnitude(dir1MeasurmentWCS);
   double dividend = mag2Ip * mag2Mt;

   // Check for divergent vectors, if we have this our fusion will put the target in the wrong quadrant
   // The algorithm is to compute the dot product between target vector 1 and the baseline distance vector.
   // If the result is less than zero, a divergent condition exists.
   bool wrongWay = false;
   if (dividend > 0.000001) // Prevent div by 0
   {
      double cosOld2New = UtVec3d::DotProduct(dir1MeasurmentWCS, dir1toInterceptPointWCS) / dividend;

      if (cosOld2New < 0.0)
      {
         //[-90,90] > 0, [-180,-90]^[90,180] < 0
         wrongWay = true;
      }
   }

   if (wrongWay)
   {
#ifdef DEBUG_DIR_FIND_VERBOSE
      static bool hasDrew = false;
      if (!hasDrew)
      {
         hasDrew = true;
         DebugEkf::Draw(origin1, vecWCS1, locationWCS);
         DebugEkf::Draw(origin2, vecWCS2, locationWCS);
      }
#endif
      if (mTest)
      {
         auto out = ut::log::info() << "Direction Finder: Bearing lines divergent. No fusion.";
         out.AddNote() << "Pair: " << GetPlatform()->GetName() << ":" << GetName();
      }
      return NO_FUSE_DIVERGENT_BEARINGS;
   }

   // Either refine the estimate of the altitude or guess,
   // depending on the information in the tracks.
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(tmpTrack1.GetTargetIndex());

   // Use terrain if platform is land or surface domain
   // (if no mover, WsfPlatform assumes land domain)
   double alt = CorrectForAltitude(locationWCS, targetPtr, computeElevation, aFusedTrack.GetDefaultAltitude());

   UtLineSegment seg1WCSErr1(origin1, vec1WCSErr1);
   UtLineSegment seg1WCSErr2(origin2, vec1WCSErr2);
   UtLineSegment seg2WCSErr1(origin1, vec2WCSErr1);
   UtLineSegment seg2WCSErr2(origin2, vec2WCSErr2);

   double location1WCSErr1[3];
   double location1WCSErr2[3];
   double location2WCSErr1[3];
   double location2WCSErr2[3];


   if (!GetIntersectionPoint(segWCS1, seg1WCSErr2, location1WCSErr1) && DebugEnabled())
   {
      ut::log::debug() << "Direction Finder: Error-lines 1 did not intersect.";
   }
   if (!GetIntersectionPoint(segWCS2, seg1WCSErr1, location1WCSErr2) && DebugEnabled())
   {
      ut::log::debug() << "Direction Finder: Error-lines 2 did not intersect.";
   }
   if (!GetIntersectionPoint(segWCS1, seg2WCSErr2, location2WCSErr1) && DebugEnabled())
   {
      ut::log::debug() << "Direction Finder: Error-lines 3 did not intersect.";
   }
   if (!GetIntersectionPoint(segWCS2, seg2WCSErr1, location2WCSErr2) && DebugEnabled())
   {
      ut::log::debug() << "Direction Finder: Error-lines 4 did not intersect.";
   }

   double rangeErrorVec1[3];
   double rangeErrorVec2[3];

   if (mTest)
   {
      WsfDraw draw(*GetSimulation());
      draw.SetId(GetPlatform()->GetNameId());
      draw.SetLineSize(1);

      // Draw green lines from the each measurement origin to target location.
      draw.SetColor(0.0, 1.0, 0.0);
      draw.Erase(GetPlatform()->GetNameId());

      draw.BeginLines();
      draw.VertexWCS(vecWCS1);
      draw.VertexWCS(origin1);
      draw.VertexWCS(vecWCS2);
      draw.VertexWCS(origin2);

      // Draw red lines indicating the extent of the expected error around each measurement.
      draw.SetColor(1.0, 0.0, 0.0);
      draw.VertexWCS(origin1);
      draw.VertexWCS(vec1WCSErr1);
      draw.VertexWCS(origin1);
      draw.VertexWCS(vec2WCSErr1);
      draw.VertexWCS(origin2);
      draw.VertexWCS(vec1WCSErr2);
      draw.VertexWCS(origin2);
      draw.VertexWCS(vec2WCSErr2);

      // Draw blue lines indicating the extent of the expected 3D errors.  The intersection
      // of the two lines indicates the computed target location.
      draw.SetLineSize(3);
      draw.SetColor(0.0, 0.0, 1.0);
      draw.VertexWCS(location1WCSErr1);
      draw.VertexWCS(location2WCSErr1);
      draw.VertexWCS(location1WCSErr2);
      draw.VertexWCS(location2WCSErr2);
      draw.End();

      // Draw points to indicate each of the indicated error extents; then draw a point at the computed target location.
      draw.BeginPoints();
      draw.SetColor(0.1, 0.1, 0.1);
      draw.SetPointSize(5);
      draw.VertexWCS(location1WCSErr1);
      draw.VertexWCS(location2WCSErr1);
      draw.VertexWCS(location1WCSErr2);
      draw.VertexWCS(location2WCSErr2);
      draw.VertexWCS(locationWCS);
      draw.End();
   }

   CorrectForAltitude(alt, location1WCSErr1);
   CorrectForAltitude(alt, location1WCSErr2);
   CorrectForAltitude(alt, location2WCSErr1);
   CorrectForAltitude(alt, location2WCSErr2);

   // Compute the vectors with magnitude value of half the range error.
   UtVec3d::Subtract(rangeErrorVec1, location2WCSErr1, location1WCSErr1);
   UtVec3d::Subtract(rangeErrorVec2, location2WCSErr2, location2WCSErr1);

   // From these, compute the range errors.
   double rangeError1 = UtVec3d::Magnitude(rangeErrorVec1);
   double rangeError2 = UtVec3d::Magnitude(rangeErrorVec2);

   if (std::max(rangeError1, rangeError2) > mMaxExpectedError)
   {
      if (mTest)
      {
         auto out = ut::log::info() << "Direction Finder: Pair did not pass maximum range error test.";
         out.AddNote() << "Pair: " << GetPlatform()->GetName() << ":" << GetName();
         out.AddNote() << "Max Error Range: " << std::max(rangeError1, rangeError2);
         out.AddNote() << "Expected Error: " << mMaxExpectedError;
      }
      return NO_FUSE_EXCEED_RANGE_ERROR;
   }

   tmpTrack1.SetRangeError(rangeError1);
   tmpTrack2.SetRangeError(rangeError2);

   // Set the originator location for the next iteration
   // (orig. location of each bearing report needs to be offset from the last).
   aFusedTrack.SetUpdateTime(std::max(t1, t2));
   aFusedTrack.SetUpdateCount(tmpTrack2.GetUpdateCount()); //?????

   // simple-minded approach
   if (!tmpTrack1.ElevationValid())
   {
      tmpTrack1.SetElevationError(tmpTrack1.GetBearingError());
   }
   if (!tmpTrack2.ElevationValid())
   {
      tmpTrack2.SetElevationError(tmpTrack2.GetBearingError());
   }

   // Filter these measurements with a zero process noise Kalman filter,
   // if a specific noise and filter haven't been specified.
   WsfFilter* filterPtr = aFusedTrack.GetFilter();
   if (filterPtr == nullptr)
   {
      AddFilterToFusedTrack(aFusedTrack);
      filterPtr = aFusedTrack.GetFilter();
   }

   tmpTrack1.SetLocationWCS(locationWCS);

   UtCovariance measurementCovar1(3, 3);

   measurementCovar1.ComputeMeasurementCovarianceWCS(tmpTrack1);
   tmpTrack1.SetMeasurementCovariance(tmpTrack1.GetUpdateTime(), measurementCovar1);

   tmpTrack2.SetLocationWCS(locationWCS);

   UtCovariance measurementCovar2(3, 3);
   measurementCovar2.ComputeMeasurementCovarianceWCS(tmpTrack2);
   tmpTrack2.SetMeasurementCovariance(tmpTrack2.GetUpdateTime(), measurementCovar2);

   // Update the filter with the estimates at the update times of the two tracks.
   // Note:  This may have the effect of being too over-confident about the target location,
   // as we are essentially only providing 4 data (bearing x 2, elevation x 2) but assuming 6.
   UtMeasurementData estimateOut;
   filterPtr->Update(tmpTrack1.GetUpdateTime(), tmpTrack1, estimateOut); // earliest time
   filterPtr->Update(tmpTrack2.GetUpdateTime(), tmpTrack2, estimateOut); // latest time

   UtVec3d filteredLocWCS = estimateOut.GetLocationWCS();
   UtVec3d filteredVelWCS = estimateOut.GetVelocityWCS();

   // Correct for the calculated altitude
   CorrectForAltitude(alt, filteredLocWCS.GetData());

   // Set the track data based on these calculations:
   aFusedTrack.SetTargetIndex(tmpTrack1.GetTargetIndex());
   aFusedTrack.SetTargetName(tmpTrack1.GetTargetName());
   aFusedTrack.SetRangeValid(false);
   aFusedTrack.SetBearingValid(false);

   if (mBypassFilter)
   {
      // Set to the intersection point (aka measurement)
      aFusedTrack.SetLocationWCS(locationWCS);
      aFusedTrack.SetVelocityValid(false);

      // Even if we bypass the filter, we may want "some" covariance.
      if (filterPtr->IsStable())
      {
         const WsfCovariance* covarPtr = filterPtr->GetStateCovariance();
         if (covarPtr != nullptr) // should be true
         {
            aFusedTrack.SetStateCovariance(GetSimulation()->GetSimTime(), *covarPtr);
         }
      }
   }
   else if (filterPtr->IsStable())
   {
      const WsfCovariance* covarPtr = filterPtr->GetStateCovariance();
      if (covarPtr != nullptr) // should be true
      {
         aFusedTrack.SetStateCovariance(GetSimulation()->GetSimTime(), *covarPtr);
         if (mUseStatisticalLocation)
         {
            const WsfCovariance& covar  = *covarPtr;
            double               sigmaX = sqrt(covar(0, 0));
            double               sigmaY = sqrt(covar(1, 1));
            double               sigmaZ = sqrt(covar(2, 2));
            double               truthLocWCS[3];
            targetPtr->GetLocationWCS(truthLocWCS);
            filteredLocWCS[0] = mRandom.Normal(truthLocWCS[0], sigmaX);
            filteredLocWCS[1] = mRandom.Normal(truthLocWCS[1], sigmaY);
            filteredLocWCS[2] = mRandom.Normal(truthLocWCS[2], sigmaZ);

            // Now we need to go back and adjust the altitude.
            CorrectForAltitude(filteredLocWCS.GetData(), targetPtr, computeElevation, aFusedTrack.GetDefaultAltitude());
         }
      }
      aFusedTrack.SetLocationWCS(filteredLocWCS.GetData());
      aFusedTrack.SetVelocityWCS(filteredVelWCS.GetData());
      aFusedTrack.SetVelocityValid(true);
   }
   else
   {
      // Set to the intersection point
      aFusedTrack.SetLocationWCS(locationWCS);
      aFusedTrack.SetVelocityValid(false);
   }

   aFusedTrack.SetWCS_LocationValid(true);
   aFusedTrack.SetTrackType(WsfTrack::cPROCESSED);
   aFusedTrack.SetOriginatorLocationWCS(origin2);
   aFusedTrack.Set3D(true); // For purposes of additional fusion.

   // PH: Added 2nd condition, the target was null once.
   if (DebugEnabled() && GetSimulation()->PlatformExists(tmpTrack1.GetTargetIndex()))
   {
      UtVec3d targetLocWCS;
      targetPtr = GetSimulation()->GetPlatformByIndex(tmpTrack1.GetTargetIndex());
      targetPtr->GetLocationWCS(targetLocWCS.GetData());
      UtVec3d diffVec   = targetLocWCS - filteredLocWCS;
      double  rangeDiff = diffVec.Magnitude();
      double  lat, lon;
      UtEllipsoidalEarth::ConvertECEFToLLA(locationWCS, lat, lon, alt);

      auto out = ut::log::debug() << "Direction Finder Location:";
      out.AddNote() << "Raw Lat: " << UtLatPos(lat);
      out.AddNote() << "Raw Lon: " << UtLonPos(lon);
      out.AddNote() << "Raw Alt: " << alt << " m";

      UtEllipsoidalEarth::ConvertECEFToLLA(filteredLocWCS.GetData(), lat, lon, alt);
      out.AddNote() << "Filtered Lat: " << UtLatPos(lat);
      out.AddNote() << "Filtered Lon: " << UtLonPos(lon);
      out.AddNote() << "Filtered Alt: " << alt << " m";

      if (filterPtr->IsStable())
      {
         const WsfCovariance* covarPtr = filterPtr->GetStateCovariance();
         if (covarPtr != nullptr)
         {
            UtVec3d              targetVecWCS = filteredLocWCS - origin2;
            double               range        = targetVecWCS.Magnitude();
            const WsfCovariance& covar        = *covarPtr;
            double               stDev[3]     = {sqrt(covar(0, 0)), sqrt(covar(1, 1)), sqrt(covar(2, 2))};
            double               rangeError   = fabs(UtVec3d::DotProduct(filteredLocWCS, stDev)) / range;
            out.AddNote() << "Difference: " << rangeDiff;
            out.AddNote() << "Expected: " << rangeError;
         }
      }
   }

#ifdef DEBUG_DIR_FIND_VERBOSE
   // if (!Equals(t1, t2, 0.01)) {
   //    DebugEkf::PrintMisFused(tmpTrack1, tmpTrack2, aFusedTrack);
   //    misFusedCnt++;
   // }
   DebugEkf::WriteTrackFused(tmpTrack1, tmpTrack2, aFusedTrack, locationWCS);
#endif

   return FUSE_SUCCESS;
}

void WsfDirectionFinderProcessor::AddFilterToFusedTrack(WsfLocalTrack& aFusedTrack)
{
   WsfFilter* filterPtr = nullptr;
   if (mPrototypeFilterPtr == nullptr)
   {
      filterPtr = new WsfKalmanFilter();

      // No process noise for a non-moving target.
      double noise[3] = {0.0, 0.0, 0.0};
      static_cast<WsfKalmanFilter*>(filterPtr)->SetFilterProcessNoise(noise);

      // todo take advantage of the fact that we might be tracking a non-moving target.
      // static_cast<WsfKalmanFilter*>(filterPtr)->SetTargetType(WsfKalmanFilter::cSITTER);
      aFusedTrack.SetFilter(filterPtr);
   }
   else
   {
      // A prototype filter with possibly non-zero process noise.
      filterPtr = mPrototypeFilterPtr->Clone();
      filterPtr->Initialize(aFusedTrack.GetUpdateTime(), aFusedTrack.GetSimulation());
      aFusedTrack.SetFilter(filterPtr);
   }
}

bool WsfDirectionFinderProcessor::FuseSideType(WsfTrack& aFusedTrack, const WsfTrack& aBearingElevationTrack)
{
   bool fused = false;
   if (aBearingElevationTrack.SideIdValid() && (!aFusedTrack.SideIdValid()))
   {
      aFusedTrack.SetSideId(aBearingElevationTrack.GetSideId());
      aFusedTrack.SetSideIdValid(true);
      fused = true;
   }

   if (aBearingElevationTrack.TypeIdValid() && (!aFusedTrack.TypeIdValid()))
   {
      aFusedTrack.SetTypeId(aBearingElevationTrack.GetTypeId());
      aFusedTrack.SetTypeIdValid(true);
      fused = true;
   }

   return fused;
}

bool WsfDirectionFinderProcessor::GetIntersectionPoint(const UtLineSegment& aTargetVec1,
                                                       const UtLineSegment& aTargetVec2,
                                                       double               aIntersectionPoint[3],
                                                       bool                 aDraw)
{
   double pointWCS1[3];
   double pointWCS2[3];

   // Check to see whether these lines are coplanar; if so, use 2D intersection
   UtVec3d intersectionPoint;
   double  modulus1, modulus2;
   aTargetVec1.PointOfClosestApproach(aTargetVec2, pointWCS1, modulus1, pointWCS2, modulus2);
   UtLineSegment joinWCS(pointWCS1, pointWCS2);
   joinWCS.GetPointAtModulus(0.5, aIntersectionPoint);

   if (mTest && aDraw)
   {
      WsfDraw draw(*GetSimulation());
      //      mDraw.Erase(1);
      //      mDraw.SetId(1);
      draw.SetId(GetPlatform()->GetNameId());
      draw.SetColor(0.5, 0.5, 0.5);
      draw.SetPointSize(5);
      draw.BeginPoints();
      draw.VertexWCS(pointWCS1);
      draw.VertexWCS(pointWCS2);
      draw.End();
      draw.BeginLines();
      draw.VertexWCS(pointWCS1);
      draw.VertexWCS(pointWCS2);
      draw.BeginPoints();
      draw.VertexWCS(aIntersectionPoint);
      draw.End();
   }
   return true;
}

void WsfDirectionFinderProcessor::GetNED_Vector(double aBearing, double aElevation, double aVecNED[3])
{
   double maxRange = 1000000.0; // 1000 km
   double sinB     = sin(aBearing);
   double cosB     = cos(aBearing);
   double sinE     = sin(aElevation);
   double cosE     = cos(aElevation);
   aVecNED[0]      = maxRange * cosB * cosE;
   aVecNED[1]      = maxRange * sinB * cosE;
   aVecNED[2]      = -maxRange * sinE;
}

void WsfDirectionFinderProcessor::CorrectForAltitude(double aAlt,
                                                     double aLocationWCS[3]) // in/out
{
   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aLocationWCS, lat, lon, alt);
   UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, aAlt, aLocationWCS);
}

double WsfDirectionFinderProcessor::CorrectForAltitude(double       aTargetLocWCS[3], // in / out
                                                       WsfPlatform* aTargetPtr,
                                                       bool         aComputeElevation,
                                                       double       aDefaultElevation)
{
   double lat, lon, alt = 0.0;
   UtEllipsoidalEarth::ConvertECEFToLLA(aTargetLocWCS, lat, lon, alt);
   if ((aTargetPtr != nullptr) && mUseTruthAltitude)
   {
      double targetLat, targetLon, targetAlt;
      aTargetPtr->GetLocationLLA(targetLat, targetLon, targetAlt);
      alt = targetAlt;
   }
   else if ((aTargetPtr != nullptr) && ((aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND) ||
                                        (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)))
   {
      if (mTerrainPtr->IsEnabled())
      {
         float elev = 0.0;
         mTerrainPtr->GetElevApprox(lat, lon, elev);
         alt = std::max(alt, static_cast<double>(elev));
      }
      else
      {
         alt = std::max(alt, 0.0);
      }
   }
   else if (aComputeElevation)
   {
      alt = aDefaultElevation;
   }
   UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, alt, aTargetLocWCS);
   return alt;
}
