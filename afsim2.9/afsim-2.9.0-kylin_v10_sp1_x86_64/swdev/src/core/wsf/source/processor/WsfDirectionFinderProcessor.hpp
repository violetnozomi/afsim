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

#ifndef WSFDIRECTIONFINDERPROCESSOR_HPP
#define WSFDIRECTIONFINDERPROCESSOR_HPP

#include "wsf_export.h"

#include <list>

////#ifndef DEBUG_DIR_FIND_VERBOSE
////#define DEBUG_DIR_FIND_VERBOSE
////#endif

#ifdef DEBUG_DIR_FIND_VERBOSE
#include <cstring>
#include <sstream>
#endif

#include "UtEntity.hpp"
class UtLineSegment;
#include "UtRandom.hpp"
#include "WsfCovariance.hpp"
#include "WsfDraw.hpp"
#include "WsfFilter.hpp"
#include "WsfLinkedProcessor.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPerfectCorrelation.hpp"
#include "WsfTrackList.hpp"

namespace wsf
{
class Terrain;
}

//! Attempt to fuse multiple tracks containing bearing-only measurements.
//! This processor should be hooked up with bearing-only track-reporting sensors.
//! It processes incoming WsfTrack messages, fusing together incoming bearing-only
//! track reports.  Once two or more reports from the same target are fused into a
//! location, the resultant fused track is sent out in a WsfTrackMessage.  This
//! could then be sent on to a WsfTrackProcessor, for instance.
class WSF_EXPORT WsfDirectionFinderProcessor : public WsfLinkedProcessor
{
public:
   enum FuseResult
   {
      FUSE_SUCCESS,
      NO_FUSE_ANGLE_THRESHOLD,
      NO_FUSE_BASELINE_DIST,
      NO_FUSE_DIVERGENT_BEARINGS,
      NO_FUSE_NO_INTERSECTION,
      NO_FUSE_EXCEED_RANGE_ERROR,
      NO_FUSE_NON_SYNC
   };

#ifdef DEBUG_DIR_FIND_VERBOSE
   inline static std::string ToString(FuseResult result)
   {
      std::stringstream ss;
      ss << result;
      return ss.str();
   }
   inline friend std::ostream& operator<<(std::ostream& aOut, const FuseResult& aType)
   {
      if (aType == FUSE_SUCCESS)
      {
         aOut << "FUSE_SUCCESS";
      }
      else if (aType == NO_FUSE_ANGLE_THERSHOLD)
      {
         aOut << "NO_FUSE_ANGLE_THERSHOLD";
      }
      else if (aType == NO_FUSE_BASELINE_DIST)
      {
         aOut << "NO_FUSE_BASELINE_DIST";
      }
      else if (aType == NO_FUSE_DIVERGENT_BEARINGS)
      {
         aOut << "NO_FUSE_DIVERGENT_BEARINGS";
      }
      else if (aType == NO_FUSE_NO_INTERSECTION)
      {
         aOut << "NO_FUSE_NO_INTERSECTION";
      }
      else if (aType == NO_FUSE_EXCEED_RANGE_ERROR)
      {
         aOut << "NO_FUSE_EXCEED_RANGE_ERROR";
      }
      else if (aType == NO_FUSE_NON_SYNC)
      {
         aOut << "NO_FUSE_NON_SYNC";
      }
      return aOut;
   }
#endif

public:
   WsfDirectionFinderProcessor(WsfScenario& aScenario);
   WsfDirectionFinderProcessor(const WsfDirectionFinderProcessor& aSrc);
   WsfDirectionFinderProcessor& operator=(const WsfDirectionFinderProcessor&) = delete;
   ~WsfDirectionFinderProcessor() override;

   bool          ProcessInput(UtInput& aInput) override;
   bool          Initialize(double aSimTime) override;
   WsfProcessor* Clone() const override { return new WsfDirectionFinderProcessor(*this); }
   bool          ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

protected:
   bool FuseSideType(WsfTrack& aFusedTrack, const WsfTrack& aBearingElevationTrack);

   WsfLocalTrack* CreateLocalTrack(const WsfTrack& aTrack);

   void AddFilterToFusedTrack(WsfLocalTrack& aTrack);

   double GetPurgeInterval() const { return mPurgeInterval; }

   double GetMaxExpectedError() const { return mMaxExpectedError; }

   bool BypassFilter() const { return mBypassFilter; }

   bool UseTruthAltitude() const { return mUseTruthAltitude; }

   wsf::Terrain& GetTerrain() const { return *mTerrainPtr; }

   double CorrectForAltitude(double       aTargetLocWCS[3], // in / out
                             WsfPlatform* aTargetPtr,
                             bool         aComputeElevation,
                             double       aDefaultElevation);

   std::list<std::unique_ptr<WsfTrack>> mTrackList;
   WsfLocalTrackList                    mFusedTrackList;
   WsfPerfectCorrelation                mCorrelation;

   //! Same type of filter as in track manager.
   //! If not specified, the original Kalman filter will be used
   //! with no process noise (stationary)
   WsfFilter* mPrototypeFilterPtr;

   wsf::Terrain* mTerrainPtr;

private:
   FuseResult FuseBearingElevation(WsfLocalTrack& aFusedTrack,
                                   WsfTrack&      aSavedTrack, // for internal use
                                   WsfTrack&      aBearingElevationTrack);

   bool GetIntersectionPoint(const UtLineSegment& aTargetVec1,
                             const UtLineSegment& aTargetVec2,
                             double               aIntersectionPoint[3],
                             bool                 aDraw = false);


   void GetNED_Vector(double aBearing, double aElevation, double aVecNED[3]);

   void CorrectForAltitude(double aAlt,
                           double aLocationWCS[3]); // in/out

   UtEntity mReferencePoint;
   double   mCenterLat;
   double   mCenterLon;
   double   mCenterRad;
   bool     mFuseAllCollects;
   bool     mUseTruthAltitude;
   double   mPurgeInterval;
   double   mMaxExpectedError;
   double   mMinBaselineDistance;
   bool     mTest;

   //! Used to prevent fusing non-synchronous
   //! measurements.
   double mMaxTimeDifference;

   //! It appears the filter may not converge, perhaps because
   //! the measurement noise is calculated in the triangulation, and may
   //! not be correctly used in the filter.
   bool mBypassFilter;

   //! We can sidestep potential nonlinearities, either in the DF
   //! algorithm or the filter, by simply drawing on the true location
   //! of the target.  This is an effects-based soluiton.
   bool       mUseStatisticalLocation;
   ut::Random mRandom;
};

#endif
