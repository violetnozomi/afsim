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

#ifndef WSFTERRAINPROFILER_HPP
#define WSFTERRAINPROFILER_HPP

#include <iostream>
#include <list>
#include <vector>

class UtInput;

#include "wsf_export.h"

#include "WsfTerrain.hpp"

//! Terrain Profiling Utility.  This class is a utility to translate a collection
//! of (latitude, longitude) pairs into a collection of (latitude, longitude,
//! altitude) Posts suspended above the local terrain, with imposed maximum and
//! minimum gradient limits between each (flight path angle constraints).  The
//! Posts may then be used as a terrain avoidance flight profile.  Does not
//! laterally displace the path, nor does the class concern itself with either
//! lateral or vertical flight dynamics (beyond gradient), and so does not
//! guarantee that the profile is flyable by the aircraft.  NOTE:  Due to
//! expensive terrain Line-Of-Sight checks, this utility is not recommended for
//! virtual run-time use.  Recommended usage would be after the
//! WsfTerrainPathFinder has found a relatively low-altitude path from A to B,
//! and then this class post-processes the returned path to produce the
//! recommended flown path.

class WSF_EXPORT WsfTerrainProfiler
{
public:
   WsfTerrainProfiler(const wsf::Terrain& aTerrain);
   WsfTerrainProfiler(const WsfTerrainProfiler& aSrc);
   WsfTerrainProfiler& operator=(const WsfTerrainProfiler&) = delete;
   ~WsfTerrainProfiler();

   //! Used to set parameters (if desired) via stream input.
   //! Values may be set directly via accessors, instead.
   bool ProcessInput(UtInput& aInput);

   using LatLonPair = std::pair<double, double>;
   using InputRoute = std::vector<LatLonPair>;

   struct Post
   {
      Post(double aLatDeg, double aLonDeg, double aTerrainAlt, double aNomPathAlt);

      double mLatDeg; // deg
      double mLonDeg; // deg

      double mTerrainAlt; // MSL, meters
      double mNomPathAlt; // > mTerrainHeight.  Aircraft should not descend below here.
      double mAdjPathAlt; // >= mMinPathAlt.  Aircraft should strive to pass at this altitude.

      double mHdgToNextDeg;
      double mDistToNext; // Great circle distance to the next post.

      double mNomGradToNext; // Gradient from this post to next post via mMinPathAlt values.
      double mAdjGradToNext; // Gradient from this post to next post via mAdjPathAlt values.

      size_t mIndex;
   };

   using OutputRoute = std::list<Post>;

   // This is a stand-alone function which does not rely on any class state information.
   bool CreateProfile(double            aTerrainClearance,
                      double            aMaxGradient,
                      double            aMinGradient,
                      double            aMaxPostSpacing,
                      const InputRoute& aInputRoute,
                      OutputRoute&      aOutputRoute,
                      bool              aPrintErrors = false);

   // This function does use pre-set constraints (set via accessors or stream input).
   bool CreateProfile(const InputRoute& aInputRoute, OutputRoute& aOutputRoute, bool aPrintErrors = false);

   double NominalTerrainClearance() const { return mConstraints.mNomTerrainClear; }
   double MinimumGradient() const { return mConstraints.mMinGradient; }
   double MaximumGradient() const { return mConstraints.mMaxGradient; }
   double MaximumPostSpacing() const { return mConstraints.mMaxPostSpacing; }

   void SetNominalTerrainClearance(double aNomTerrainClear) { mConstraints.mNomTerrainClear = aNomTerrainClear; }
   void SetMinimumGradient(double aMinGradient) { mConstraints.mMinGradient = aMinGradient; }
   void SetMaximumGradient(double aMaxGradient) { mConstraints.mMaxGradient = aMaxGradient; }
   void SetMaximumPostSpacing(double aMaxPostSpacing) { mConstraints.mMaxPostSpacing = aMaxPostSpacing; }

   // friend std::ostream& operator<<(std::ostream&                          aStream,
   //                                 const WsfTerrainProfiler::OutputRoute& aSrc);
   // friend std::ostream& operator<<(std::ostream&                          aStream,
   //                                 const WsfTerrainProfiler&              aSrc);

protected:
   void InsertPostsBetween(OutputRoute& aRoute, OutputRoute::iterator aStartIter, OutputRoute::iterator aStopIter);

   void InsertOnePostBetween(OutputRoute& aRoute, OutputRoute::iterator aStartIter, OutputRoute::iterator aStopIter);

   void ReNumber(OutputRoute& aRoute);

   bool DoubleCheck(OutputRoute& aRoute, bool aPrintErrors = false);

private:
   struct Constraints
   {
      Constraints();
      double mNomTerrainClear;
      double mMaxGradient;
      double mMinGradient;
      double mMaxPostSpacing; // Great Circle spacing
   };

   void GetPostData(double aLatDeg, double aLonDeg, double& aTerrainAlt, double& aMinPathAlt);

   void InsertPostsBetween_P(OutputRoute&          aRoute,
                             double                aStartLatitudeDeg,
                             double                aStartLongitudeDeg,
                             double                aHeadingDeg,
                             OutputRoute::iterator aPreviousIter,
                             OutputRoute::iterator aInsertAtIter,
                             size_t                aNumberToInsert,
                             double                aSpacing);

   Constraints  mConstraints;
   wsf::Terrain mTerrain;
};

#endif
