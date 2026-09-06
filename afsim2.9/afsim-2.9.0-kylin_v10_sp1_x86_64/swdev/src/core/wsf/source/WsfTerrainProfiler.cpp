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

#include "WsfTerrainProfiler.hpp"

#include <cassert>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"

// ==============================================================
WsfTerrainProfiler::Constraints::Constraints()
   : mNomTerrainClear(100.0)
   , mMaxGradient(3.0 * UtMath::cRAD_PER_DEG)
   , mMinGradient(-3.0 * UtMath::cRAD_PER_DEG)
   , mMaxPostSpacing(8000.0)
{
}

// ==============================================================
WsfTerrainProfiler::WsfTerrainProfiler(const wsf::Terrain& aTerrain)
   : mConstraints()
   , mTerrain(aTerrain)
{
}

// ==============================================================
WsfTerrainProfiler::WsfTerrainProfiler(const WsfTerrainProfiler& aSrc)
   : mConstraints(aSrc.mConstraints)
   , mTerrain(aSrc.mTerrain)
{
}

// ==============================================================
WsfTerrainProfiler::~WsfTerrainProfiler() {}

// ==============================================================
bool WsfTerrainProfiler::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "nominal_terrain_clearance")
   {
      aInput.ReadValueOfType(mConstraints.mNomTerrainClear, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mConstraints.mNomTerrainClear, 0.0);
   }
   else if (command == "maximum_post_spacing")
   {
      aInput.ReadValueOfType(mConstraints.mMaxPostSpacing, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mConstraints.mMaxPostSpacing, 0.0);
   }
   else if (command == "maximum_path_gradient")
   {
      aInput.ReadValueOfType(mConstraints.mMaxGradient, UtInput::cANGLE);
      aInput.ValueGreater(mConstraints.mMaxGradient, 0.0);
   }
   else if (command == "minimum_path_gradient")
   {
      aInput.ReadValueOfType(mConstraints.mMinGradient, UtInput::cANGLE);
      aInput.ValueLess(mConstraints.mMinGradient, 0.0);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ==============================================================
void WsfTerrainProfiler::GetPostData(double aLatDeg, double aLonDeg, double& aTerrainAlt, double& aMinPathAlt)
{
   float elev;
   mTerrain.GetElevInterp(aLatDeg, aLonDeg, elev);
   aTerrainAlt = static_cast<double>(elev);
   aMinPathAlt = aTerrainAlt + mConstraints.mNomTerrainClear;
}

// ==============================================================
void WsfTerrainProfiler::ReNumber(OutputRoute& aRoute)
{
   OutputRoute::iterator it    = aRoute.begin();
   size_t                index = 0;
   while (it != aRoute.end())
   {
      it->mIndex = index;
      ++it;
      ++index;
   }
}

// ==============================================================
bool WsfTerrainProfiler::DoubleCheck(OutputRoute& aRoute, bool aPrintErrors)
{
   bool success = true;

   OutputRoute::iterator it     = aRoute.begin();
   OutputRoute::iterator prevIt = aRoute.end();

   size_t prevIndex = prevIt->mIndex;

   while (it != aRoute.end())
   {
      size_t thisIndex = it->mIndex;

      if (prevIt != aRoute.end())
      {
         double hgtRise = it->mNomPathAlt - prevIt->mNomPathAlt;
         double adjRise = it->mAdjPathAlt - prevIt->mAdjPathAlt;

         double hdg, dist;
         UtSphericalEarth::GreatCircleHeadingAndDistance(prevIt->mLatDeg, prevIt->mLonDeg, it->mLatDeg, it->mLonDeg, hdg, dist);

         double nomGradient = atan2(hgtRise, dist);
         double adjGradient = atan2(adjRise, dist);

         if (fabs(hdg - prevIt->mHdgToNextDeg) > 0.1)
         {
            if (aPrintErrors)
            {
               auto out = ut::log::error() << "Terrain Profiler heading mismatch.";
               out.AddNote() << "Heading A: " << prevIt->mHdgToNextDeg;
               out.AddNote() << "Heading B: " << hdg;
               out.AddNote() << "ID: " << prevIndex;
            }
            success = false;
         }

         if (fabs(dist - prevIt->mDistToNext) > 5.0)
         {
            if (aPrintErrors)
            {
               auto out = ut::log::error() << "Terrain Profiler distance mismatch.";
               out.AddNote() << "Distance A: " << prevIt->mDistToNext;
               out.AddNote() << "Distance B: " << dist;
               out.AddNote() << "ID: " << prevIndex;
            }
            success = false;
         }

         if (fabs(nomGradient - prevIt->mNomGradToNext) > 0.01)
         {
            if (aPrintErrors)
            {
               auto out = ut::log::error() << "Terrain Profiler nominal gradient mismatch.";
               out.AddNote() << "Gradient A: " << prevIt->mNomGradToNext;
               out.AddNote() << "Gradient B: " << nomGradient;
               out.AddNote() << "ID: " << prevIndex;
            }
            success = false;
         }

         if (fabs(adjGradient - prevIt->mAdjGradToNext) > 0.01)
         {
            if (aPrintErrors)
            {
               auto out = ut::log::error() << "Terrain Profiler adjusted gradient mismatch.";
               out.AddNote() << "Gradient A: " << prevIt->mAdjGradToNext;
               out.AddNote() << "Gradient B: " << adjGradient;
               out.AddNote() << "ID: " << prevIndex;
            }
            success = false;
         }

         if (((prevIt->mAdjGradToNext) > mConstraints.mMaxGradient) ||
             ((prevIt->mAdjGradToNext) < mConstraints.mMinGradient))
         {
            if (aPrintErrors)
            {
               auto out = ut::log::error() << "Terrain Profiler adjusted gradient failed limits test.";
               out.AddNote() << "Actual: " << prevIt->mAdjGradToNext;
               out.AddNote() << "Min Expected: " << mConstraints.mMinGradient;
               out.AddNote() << "Max Expected: " << mConstraints.mMaxGradient;
               out.AddNote() << "ID: " << prevIndex;
            }
            success = false;
         }

         if (thisIndex != prevIndex + 1)
         {
            if (aPrintErrors)
            {
               ut::log::error() << "Terrain Profiler index mismatch.";
            }
            success = false;
         }
      }

      prevIt    = it;
      prevIndex = thisIndex;

      ++it;
   }

   return success;
}

// ==============================================================
bool WsfTerrainProfiler::CreateProfile(double            aTerrainClearance,
                                       double            aMaxGradient,
                                       double            aMinGradient,
                                       double            aMaxPostSpacing,
                                       const InputRoute& aInputRoute,
                                       OutputRoute&      aOutputRoute,
                                       bool              aPrintErrors)
{
   // Push current state:
   Constraints savedConstraints = mConstraints;

   // Do the job:
   bool result = CreateProfile(aInputRoute, aOutputRoute, aPrintErrors);

   // Pop current state:
   mConstraints = savedConstraints;

   return result;
};


// ==============================================================
bool WsfTerrainProfiler::CreateProfile(const InputRoute& aInputRoute, OutputRoute& aOutputRoute, bool aPrintErrors)
{
   /* Algorithm:
   1.  Copy provided (Lat, Lon) pairs into the Posts list.
       (The constructor will set terrain heights, and min Path Alts.)
   2.  Traverse the Posts list, and insert more Posts at regular (Lat, Lon)
       intervals, maintaining the specified maximum Post spacing, or less.
   3.  Shoot a LineOfSight request to the next Post, from mNomPathAlt to mNomPathAlt.
          i.  If LOS blocked, add another Post between them (binary).  Go to 3.
   4.  Traverse all posts.  Compute nominal gradient values from one post to the next.
   5.  Traverse all Posts, checking and clearing the gradient constraints:
       A. If gradient is too steep (positively), back up to a previous Post, and continue
          backing up until max gradient constraint is satisfied between end Posts.  Raise
          all mAdjPathAlt values of Posts in the middle to "spread" the gradient constantly
          between them.  (If backed up to beginning Post, and still fails, return false.)
       B. If gradient is too steep (negatively), step forward to a next Post, and continue
          stepping forward until min gradient constraint is satisfied between intervening
          Posts.  "Spread" the gradient as above.  (If moved forward to last Post, and
          still fails, return false.)
   Return true, indicating success.  Client may query the class for the resulting route.
   */

   const double cZERO[3] = {0.0, 0.0, 0.0};

   // The input route must be two or more points:
   if (aInputRoute.size() < 2)
   {
      if (aPrintErrors)
      {
         ut::log::error() << "Terrain Profiler must be supplied at least two (Lat, Lon) points.";
      }
      return false;
   }

   aOutputRoute.clear();

   // 1.  Copy provided (Lat, Lon) pairs into the Posts list.
   size_t i;
   for (i = 0; i != aInputRoute.size(); ++i)
   {
      double lat = aInputRoute[i].first;
      double lon = aInputRoute[i].second;
      double terrAlt, pathAlt;
      GetPostData(lat, lon, terrAlt, pathAlt);
      Post post(lat, lon, terrAlt, pathAlt);
      aOutputRoute.insert(aOutputRoute.end(), post);
   }

   // 2.  Traverse the Posts list, and insert more Posts at regular (Lat, Lon)
   //     intervals, maintaining the specified maximum Post spacing, or less.
   OutputRoute::iterator thisIter = aOutputRoute.begin();
   OutputRoute::iterator prevIter = aOutputRoute.end();

   while (thisIter != aOutputRoute.end())
   {
      if (prevIter != aOutputRoute.end())
      {
         UtSphericalEarth::GreatCircleHeadingAndDistance(prevIter->mLatDeg,
                                                         prevIter->mLonDeg,
                                                         thisIter->mLatDeg,
                                                         thisIter->mLonDeg,
                                                         prevIter->mHdgToNextDeg,
                                                         prevIter->mDistToNext);

         if (prevIter->mDistToNext > mConstraints.mMaxPostSpacing)
         {
            // This call will insert a post, and invalidate "thisIter" in the process.
            // But no worries, just take prevIter (which is still valid), and increment it.
            InsertPostsBetween(aOutputRoute, prevIter, thisIter);
            thisIter = prevIter;
            ++thisIter;
         }
      }

      prevIter = thisIter;
      ++thisIter;
   }

   // 3.  Shoot a LineOfSight request to the next Post, from mNomPathAlt to mNomPathAlt.
   //     i.  If LOS blocked, add another Post between them (binary).  Go to 3.

   thisIter = aOutputRoute.begin();
   prevIter = aOutputRoute.end();

   double prevLocWCS[3];
   double thisLocWCS[3];
   double diff[3];
   UtVec3d::Set(prevLocWCS, cZERO);
   UtVec3d::Set(thisLocWCS, cZERO);
   UtVec3d::Set(diff, cZERO);

   while (thisIter != aOutputRoute.end())
   {
      if (prevIter != aOutputRoute.end())
      {
         UtEntity::ConvertLLAToWCS(thisIter->mLatDeg, thisIter->mLonDeg, thisIter->mNomPathAlt, thisLocWCS);

         UtVec3d::Subtract(diff, prevLocWCS, thisLocWCS);
         double radius = UtVec3d::Magnitude(diff) + 10.0; // Add a buffer.

         if (mTerrain.MaskedByTerrain(prevIter->mLatDeg,
                                      prevIter->mLonDeg,
                                      prevIter->mNomPathAlt,
                                      thisIter->mLatDeg,
                                      thisIter->mLonDeg,
                                      thisIter->mNomPathAlt,
                                      radius))
         {
            // LOS is blocked by terrain.  Bisect the two locations, and place another Post:
            // This call will insert a post, and invalidate "thisIter" in the process.
            // But no worries, just take prevIter (which is still valid), and increment it.
            // This will (intentionally) force a RE_TEST of the LOS between *prevIter and the new node.
            InsertOnePostBetween(aOutputRoute, prevIter, thisIter);
            thisIter = prevIter;
         }
      }
      else
      {
         // This is the first pass through, need to set "previous" values for the next pass.
         UtEntity::ConvertLLAToWCS(thisIter->mLatDeg, thisIter->mLonDeg, thisIter->mNomPathAlt, thisLocWCS);
      }

      UtVec3d::Set(prevLocWCS, thisLocWCS);
      prevIter = thisIter;
      ++thisIter;
   }

   // 4.  Traverse all posts.  Compute nominal gradient values from one post to the next.
   thisIter = aOutputRoute.begin();
   prevIter = aOutputRoute.end();

   while (thisIter != aOutputRoute.end())
   {
      if (prevIter != aOutputRoute.end())
      {
         // The NOMINAL gradient to the next post is atan2(thisAlt - prevAlt, distBetween)
         double distBetween = prevIter->mDistToNext;
         assert(distBetween > 0.0);
         double altRise           = thisIter->mTerrainAlt - prevIter->mTerrainAlt;
         double grad              = atan2(altRise, distBetween);
         prevIter->mNomGradToNext = grad;
         prevIter->mAdjGradToNext = grad; // may be overwritten later.
      }

      prevIter = thisIter;
      ++thisIter;
   }

   // 5.  Traverse all Posts, checking and clearing the gradient constraints:

   thisIter = aOutputRoute.begin();

   while (thisIter != aOutputRoute.end())
   {
      double nominalGradient = thisIter->mNomGradToNext;

      bool backUpToClimbGentler     = nominalGradient > mConstraints.mMaxGradient;
      bool flareOutToDescendGentler = nominalGradient < mConstraints.mMinGradient;
      bool isProblem                = backUpToClimbGentler || flareOutToDescendGentler;

      if (isProblem)
      {
         if (thisIter == aOutputRoute.begin() && backUpToClimbGentler)
         {
            if (aPrintErrors)
            {
               ut::log::error() << "Terrain Profiler must be supplied at least two (Lat, Lon) points.";
            }
            return false;
         }

         OutputRoute::iterator compareIt = aOutputRoute.end();
         --compareIt;
         if (thisIter == compareIt && flareOutToDescendGentler)
         {
            if (aPrintErrors)
            {
               ut::log::error() << "Terrain Profiler must be supplied at least two (Lat, Lon) points.";
            }
            return false;
         }

         if (backUpToClimbGentler)
         {
            //     A. If gradient is too steep (positively), back up to a previous Post, and continue
            //        backing up until max gradient constraint is satisfied between end Posts.  Raise
            //        all mAdjPathAlt values of Posts in the middle to "spread" the gradient constantly
            //        between them.  (If backed up to beginning Post, and still fails, return false.)

            OutputRoute::iterator fwdIter = thisIter;
            ++fwdIter;

            double                altitudeToReachFor  = fwdIter->mAdjPathAlt;
            OutputRoute::iterator offsetIter          = thisIter;
            double                altitudeToBeginFrom = offsetIter->mAdjPathAlt;
            double                cumDist             = offsetIter->mDistToNext;

            double adjustedGradient = atan2(altitudeToReachFor - altitudeToBeginFrom, cumDist);

            while (adjustedGradient > mConstraints.mMaxGradient)
            {
               if (offsetIter == aOutputRoute.begin())
               {
                  if (aPrintErrors)
                  {
                     auto out = ut::log::error() << "Terrain Profiler failed.";
                     out.AddNote() << "First to Second Post gradient exceeded limits.";
                     out.AddNote() << "There is now way to fix this by backing up to ease the gradient.";
                  }
                  return false;
               }

               --offsetIter;
               altitudeToBeginFrom = offsetIter->mAdjPathAlt;
               cumDist += offsetIter->mDistToNext;
               adjustedGradient = atan2(altitudeToReachFor - altitudeToBeginFrom, cumDist);
            }

            // We found a Post which will satisfy our gradient constraint.
            // Now go forward from that point to the present, modifying gradient and path alts.
            cumDist = 0.0;
            while (offsetIter != fwdIter)
            {
               offsetIter->mAdjGradToNext = adjustedGradient;
               double newAlt              = altitudeToBeginFrom + adjustedGradient * cumDist;
               assert(newAlt >= offsetIter->mAdjPathAlt);
               offsetIter->mAdjPathAlt = newAlt;
               cumDist += offsetIter->mDistToNext;
               ++offsetIter;
            }
         }
         else
         {
            //     B. If gradient is too steep (negatively), step forward to a next Post, and continue
            //        stepping forward until min gradient constraint is satisfied between starting and
            //        ending Posts.  "Spread" the gradient as above.  (If moved forward to last Post,
            //        and still fails, return false.)

            OutputRoute::iterator downRangeIter         = thisIter;
            double                altitudeToDescendFrom = downRangeIter->mAdjPathAlt;
            double                cumDistFwd            = downRangeIter->mDistToNext;

            ++downRangeIter;
            double altitudeToDescendTo = downRangeIter->mAdjPathAlt;
            double adjustedGradient    = atan2(altitudeToDescendTo - altitudeToDescendFrom, cumDistFwd);

            while (adjustedGradient < mConstraints.mMinGradient)
            {
               cumDistFwd += downRangeIter->mDistToNext;
               ++downRangeIter;
               if (downRangeIter == aOutputRoute.end())
               {
                  if (aPrintErrors)
                  {
                     auto out = ut::log::error() << " Terrain Profiler failed.";
                     out.AddNote() << "Last two posts exceeded negative gradient limit.";
                     out.AddNote() << "There is now way to fix this by flaring out to ease the gradient.";
                  }
                  return false;
               }
               altitudeToDescendTo = downRangeIter->mAdjPathAlt;
               adjustedGradient    = atan2(altitudeToDescendTo - altitudeToDescendFrom, cumDistFwd);
            }

            // We found a future Post which will satisfy our descent gradient constraint.
            // Now go backward from that point to the present, modifying gradient and path alts.
            double cumDistAft = 0.0;

            while (downRangeIter != thisIter)
            {
               double pathAltBeyondDwnRngIter = downRangeIter->mAdjPathAlt;
               --downRangeIter;

               double dist = downRangeIter->mDistToNext;
               cumDistAft += dist;

               double oldAlt        = downRangeIter->mAdjPathAlt;
               double glideSlopeAlt = altitudeToDescendFrom + adjustedGradient * (cumDistFwd - cumDistAft);
               double altToUse;

               if (glideSlopeAlt > oldAlt)
               {
                  altToUse = glideSlopeAlt;
               }
               else
               {
                  altToUse = oldAlt;
               }

               downRangeIter->mAdjPathAlt    = altToUse;
               downRangeIter->mAdjGradToNext = atan2(pathAltBeyondDwnRngIter - altToUse, dist);
            }
         }
      }

      ++thisIter;
   }

   ReNumber(aOutputRoute);

   if (!aPrintErrors)
   {
      return true;
   }
   else
   {
      return DoubleCheck(aOutputRoute, aPrintErrors);
   }
}

// ==============================================================
void WsfTerrainProfiler::InsertPostsBetween_P(OutputRoute&          aOutputRoute,
                                              double                aStartLatitudeDeg,
                                              double                aStartLongitudeDeg,
                                              double                aHeadingDeg,
                                              OutputRoute::iterator aPreviousIter,
                                              OutputRoute::iterator aInsertAtIter,
                                              size_t                aNumberToInsert,
                                              double                aSpacing)
{
   size_t                numInserted   = 0;
   double                distFromStart = aSpacing;
   OutputRoute::iterator insertedItemIter;
   bool                  inserted = false;

   while (numInserted != aNumberToInsert)
   {
      inserted = true;
      double newLat, newLon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(aStartLatitudeDeg,
                                                       aStartLongitudeDeg,
                                                       aHeadingDeg,
                                                       distFromStart,
                                                       newLat,
                                                       newLon);

      double terrAlt, minPathAlt;
      GetPostData(newLat, newLon, terrAlt, minPathAlt);
      insertedItemIter = aOutputRoute.insert(aInsertAtIter, Post(newLat, newLon, terrAlt, minPathAlt));

      aInsertAtIter = insertedItemIter;
      ++aInsertAtIter; // Advance to next container after the one inserted, for next insert.
      aPreviousIter = insertedItemIter;
      --aPreviousIter;

      aPreviousIter->mDistToNext = aSpacing;
      distFromStart += aSpacing;
      ++numInserted;
   }

   if (inserted)
   {
      // We just inserted one or more posts.  The last one we inserted has not
      // been given a distance to its next yet.  insertedItemIter == lastInserted
      OutputRoute::iterator nextIter = insertedItemIter;
      ++nextIter;

      UtSphericalEarth::GreatCircleHeadingAndDistance(insertedItemIter->mLatDeg,
                                                      insertedItemIter->mLonDeg,
                                                      nextIter->mLatDeg,
                                                      nextIter->mLonDeg,
                                                      insertedItemIter->mHdgToNextDeg,
                                                      insertedItemIter->mDistToNext);
   }
}

// ==============================================================
void WsfTerrainProfiler::InsertPostsBetween(OutputRoute&          aOutputRoute,
                                            OutputRoute::iterator aStartIter,
                                            OutputRoute::iterator aStopIter)
{
   double startLatDeg = aStartIter->mLatDeg;
   double startLonDeg = aStartIter->mLonDeg;

   double hdgTowardStopDeg, distToStop;
   UtSphericalEarth::GreatCircleHeadingAndDistance(startLatDeg,
                                                   startLonDeg,
                                                   aStopIter->mLatDeg,
                                                   aStopIter->mLonDeg,
                                                   hdgTowardStopDeg,
                                                   distToStop);

   int    numToInsert = static_cast<int>(distToStop / mConstraints.mMaxPostSpacing);
   double newSpacing  = distToStop / (numToInsert + 1);

   InsertPostsBetween_P(aOutputRoute, startLatDeg, startLonDeg, hdgTowardStopDeg, aStartIter, aStopIter, numToInsert, newSpacing);
}

// ==============================================================
void WsfTerrainProfiler::InsertOnePostBetween(OutputRoute&          aOutputRoute,
                                              OutputRoute::iterator aStartIter,
                                              OutputRoute::iterator aStopIter)
{
   double startLatDeg = aStartIter->mLatDeg;
   double startLonDeg = aStartIter->mLonDeg;

   double hdgTowardStopDeg, distToStop;
   UtSphericalEarth::GreatCircleHeadingAndDistance(startLatDeg,
                                                   startLonDeg,
                                                   aStopIter->mLatDeg,
                                                   aStopIter->mLonDeg,
                                                   hdgTowardStopDeg,
                                                   distToStop);

   InsertPostsBetween_P(aOutputRoute, startLatDeg, startLonDeg, hdgTowardStopDeg, aStartIter, aStopIter, 1, distToStop / 2.0);
}

/*
// =======================================================
// friend of WsfTerrainProfiler
std::ostream& operator<<(std::ostream&             aStream,
                         const WsfTerrainProfiler& aSrc)
{
   // First thing we do is to report the settings we have been given:
   aStream << "# WsfTerrainProfiler has been configured with the following options:" << endl << endl;
   aStream << "# Nominal Terrain Clearance = " << aSrc.mConstraints.mNomTerrainClear << " meters" << endl;
   aStream << "# Maximum Path Gradient     = " << aSrc.mConstraints.mMaxGradient * UtMath::cDEG_PER_RAD << " degrees" <<
endl; aStream << "# Minimum Path Gradient     = " << aSrc.mConstraints.mMinGradient * UtMath::cDEG_PER_RAD << " degrees"
<< endl; aStream << "# Max Allowed Post Spacing  = " << aSrc.mConstraints.mMaxPostSpacing << " meters (great circle)" <<
endl;

   return aStream;
}

// =======================================================
// friend  of WsfTerrainProfiler
std::ostream& operator<<(std::ostream&                          aStream,
                         const WsfTerrainProfiler::OutputRoute& aSrc)
{
   aStream << endl << "# Resulting Posts Are:" << endl;
   aStream << "CumDist, ID, Lat, Lon, DistToNext, TerrAlt, TerrClrAlt, GradToTC, PathAlt, PathGradTo" << endl;

   double cumDist = 0;
   WsfTerrainProfiler::OutputRoute::const_iterator it = aSrc.begin();
   while (it != aSrc.end())
   {
      aStream << cumDist << ", " << it->mIndex << ", " << it->mLatDeg << ", " << it->mLonDeg
         << ", " << it->mDistToNext << ", " << it->mTerrainAlt << ", " << it->mNomPathAlt
         << ", " << it->mNomGradToNext * UtMath::cDEG_PER_RAD << ", " << it->mAdjPathAlt
         << ", " << it->mAdjGradToNext * UtMath::cDEG_PER_RAD << endl;
      cumDist += it->mDistToNext;
      ++it;
   }
   return aStream;
}
*/

// ==================== Sub-Class Post ===================
WsfTerrainProfiler::Post::Post(double aLatDeg, double aLonDeg, double aTerrainAlt, double aNomPathAlt)
   : mLatDeg(aLatDeg)
   , mLonDeg(aLonDeg)
   , mTerrainAlt(aTerrainAlt)
   , mNomPathAlt(aNomPathAlt)
   , mAdjPathAlt(aNomPathAlt)
   , // May be modified later
   mHdgToNextDeg(0.0)
   , mDistToNext(0.0)
   , mNomGradToNext(0.0)
   , mAdjGradToNext(0.0)
   , mIndex(0)
{
}
