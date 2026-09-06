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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfRouteFinder.hpp"

#include <limits>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"


// =================================================================================================
WsfRouteFinder::WsfRouteFinder(WsfSimulation* aSimPtr)
   : mMaxArcLength(std::numeric_limits<double>::max())
   , mImpossibleRouteResponse(SHRINK_AVOIDANCES)
   , mSmallFudge(0.000001)
   , mVerySmallFudge(std::numeric_limits<double>::epsilon())
   , mPercentAvoidancePad(1.005)
   , mAvoidancesChanged(true)
   , mCurrentRouteTime(-1.0)
   , mLastRouteIndex(-1)
   , mRoute()
   , mRouteAvoidances()
   , mReferenceEntity()
   , mGeoSource()
   , mGeoTarget()
   , mSource()
   , mFirst()
   , mTarget()
   , mSourceAvoidancePtr(nullptr)
   , mTargetAvoidancePtr(nullptr)
   , mDraw(*aSimPtr)
   , mAvoidPlatformIdRadius()
   , mAvoidLocationRadius()
   , mAvoidances()
   , mNextAvoidanceId(0)
   , mBestPathDist(0.0)
   , mGraphNodes()
   , mBestPath()
   , mGraphPathStack()
   , mSimulationPtr(aSimPtr)
{
   mDraw.SetId(mDraw.GetNewId());
   mDraw.SetDuration(WsfDraw::cFOREVER);
   mDraw.SetEllipseMode(WsfDraw::cELLIPSE_LINE);
   mDraw.SetLineStyle(WsfDraw::cSOLID);
   mDraw.SetLineSize(2);
   mDraw.SetPointSize(4);
}

// =================================================================================================
WsfRouteFinder::WsfRouteFinder(const WsfRouteFinder& aSrc)
   : mMaxArcLength(aSrc.mMaxArcLength)
   , mImpossibleRouteResponse(aSrc.mImpossibleRouteResponse)
   , mSmallFudge(aSrc.mSmallFudge)
   , mVerySmallFudge(aSrc.mVerySmallFudge)
   , mPercentAvoidancePad(aSrc.mPercentAvoidancePad)
   , mAvoidancesChanged(aSrc.mAvoidancesChanged)
   , mCurrentRouteTime(aSrc.mCurrentRouteTime)
   , mLastRouteIndex(aSrc.mLastRouteIndex)
   , mRoute(aSrc.mRoute)
   , mRouteAvoidances(aSrc.mRouteAvoidances)
   , mReferenceEntity(aSrc.mReferenceEntity)
   , mGeoSource(aSrc.mGeoSource)
   , mGeoTarget(aSrc.mGeoTarget)
   , mSource(aSrc.mSource)
   , mFirst(aSrc.mFirst)
   , mTarget(aSrc.mTarget)
   , mSourceAvoidancePtr(nullptr)
   , mTargetAvoidancePtr(nullptr)
   , mDraw(aSrc.mDraw)
   , mAvoidPlatformIdRadius(aSrc.mAvoidPlatformIdRadius)
   , mAvoidLocationRadius(aSrc.mAvoidLocationRadius)
   , mAvoidances(aSrc.mAvoidances)
   , mNextAvoidanceId(aSrc.mNextAvoidanceId)
   , mBestPathDist(aSrc.mBestPathDist)
   , mGraphNodes(aSrc.mGraphNodes)
   , mBestPath(aSrc.mBestPath)
   , mGraphPathStack(aSrc.mGraphPathStack)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

// =================================================================================================
WsfRouteFinder::~WsfRouteFinder()
{
   delete mSourceAvoidancePtr;
   delete mTargetAvoidancePtr;
   for (auto& graphNodePtr : mGraphNodes)
   {
      delete graphNodePtr;
   }
}

// =================================================================================================
std::unique_ptr<UtScriptClass> WsfRouteFinder::CreateScriptClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptRouteFinderClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
void WsfRouteFinder::Avoid(WsfPlatform& aPlatform, double aRadius)
{
   size_t platformID = aPlatform.GetIndex();
   auto   it         = mAvoidPlatformIdRadius.find(platformID);
   if (it != mAvoidPlatformIdRadius.end())
   {
      if (aRadius > it->second)
      {
         it->second         = aRadius;
         mAvoidancesChanged = true;
      }
   }
   else
   {
      mAvoidPlatformIdRadius[platformID] = aRadius;
      mAvoidancesChanged                 = true;
   }
}

// =================================================================================================
void WsfRouteFinder::Avoid(const WsfGeoPoint& aPoint, double aRadius)
{
   mAvoidLocationRadius.emplace_back(aPoint, aRadius);
   mAvoidancesChanged = true;
}

// =================================================================================================
void WsfRouteFinder::ClearAvoidances()
{
   if (!mAvoidLocationRadius.empty() || !mAvoidPlatformIdRadius.empty())
   {
      mAvoidancesChanged = true;
   }
   mAvoidLocationRadius.clear();
   mAvoidPlatformIdRadius.clear();
}

// =================================================================================================
WsfRoute* WsfRouteFinder::Route(double aTime, const WsfGeoPoint& aFrom, const WsfGeoPoint& aTo, double speed, int branchIndex)
{
   // check for whether or not we need to regenerate the graph
   if ((mCurrentRouteTime == aTime || mAvoidPlatformIdRadius.empty()) && mAvoidancesChanged == false &&
       mSource == GetLocal(aFrom) && mTarget == GetLocal(aTo))
   {
      // the graph will remain the same, don't regenerate
      if (mLastRouteIndex == branchIndex)
      {
         // user asked for same starting branch for route through the graph, return it
         return mRoute.Clone();
      }
      else
      {
         // the graph is the same, but the route will change
         mRoute.Clear();
      }
   }
   else
   {
      // something important has changed, regenerate the graph

      // clear previously generated graph, avoidances, etc
      delete mSourceAvoidancePtr;
      delete mTargetAvoidancePtr;
      for (auto& graphNodePtr : mGraphNodes)
      {
         delete graphNodePtr;
      }
      mGraphNodes.clear();
      mRoute.Clear();
      mRouteAvoidances.Clear();

      // calculate best reference point for converting all points to 2D local NED coordinates
      // use halfway point between source & target locations
      UtVec3d vecA;
      UtVec3d vecB;
      aFrom.GetLocationWCS(vecA.GetData());
      aTo.GetLocationWCS(vecB.GetData());
      vecB.Subtract(vecA); // vecB holds the vector from A to B now
      vecB.Multiply(0.5);
      vecA.Add(vecB); // this now holds the reference point between A and B
      mReferenceEntity.SetLocationWCS(vecA.GetData());

      mGeoSource = aFrom;
      mSource    = GetLocal(mGeoSource);
      mFirst     = mSource;

      mGeoTarget = aTo;
      mTarget    = GetLocal(mGeoTarget);

      mLastRouteIndex = branchIndex;

      mNextAvoidanceId    = 0;
      mSourceAvoidancePtr = new Avoidance(mSource, mNextAvoidanceId++);
      mTargetAvoidancePtr = new Avoidance(mTarget, mNextAvoidanceId++);

      // convert all platform and static avoidance areas to 2d avoidance classes, and store
      UtVec2d vec;
      mAvoidances.clear();
      for (auto& locRadius : mAvoidLocationRadius)
      {
         WsfGeoPoint pt     = locRadius.first;
         double      radius = locRadius.second;
         UtVec2d     loc    = GetLocal(pt);
         vec.Subtract(mSource, loc);
         // try to insert this avoidance (will succeed if not a duplicate)
         LocalInsertAvoidance(Avoidance(loc, radius, vec.Magnitude(), pt, mNextAvoidanceId++));
      }

      for (auto& platformIdRadius : mAvoidPlatformIdRadius)
      {
         size_t       platformIndex = platformIdRadius.first;
         double       radius        = platformIdRadius.second;
         WsfPlatform* platformPtr   = GetSimulation()->GetPlatformByIndex(platformIndex);
         if (platformPtr != nullptr)
         {
            double wcs[3];
            platformPtr->GetLocationWCS(wcs);
            UtVec2d loc = GetLocal(wcs);
            vec.Subtract(mSource, loc);
            WsfGeoPoint pt(wcs);
            // try to insert this avoidance (will succeed if not a duplicate)
            LocalInsertAvoidance(Avoidance(loc, radius, vec.Magnitude(), pt, mNextAvoidanceId++));
         }
      }

      // possibly adjust some avoidance zones if they are troublesome (contain the target or source locations)
      switch (mImpossibleRouteResponse)
      {
      case IGNORE_IMPOSSIBLE_AVOIDANCES:
      {
         std::vector<Avoidance>::iterator it;
         for (it = mAvoidances.begin(); it != mAvoidances.end();)
         {
            if (Contains(it->mCircle, mFirst) ||
                Contains(it->mCircle, mTarget)) // check if mFirst || mTarget is inside the avoidance
            {
               // erase this avoidance
               it = mAvoidances.erase(it);
            }
            else
            {
               ++it;
            }
         }
      }
      break;

      case SHIFT_OUTSIDE_AVOIDANCES:
      {
         // TODO: make this shift target point outside of all avoidances it is inside (just like source point)
         std::vector<WsfRouteFinder::Avoidance*> avoids = GetAvoidancesContaining(mTarget);
         if (!avoids.empty())
         {
            Avoidance* totalAvoidPtr = new Avoidance(*avoids.back());
            // avoids.pop_back();
            while (!avoids.empty())
            {
               Avoidance* nextAvoidPtr = avoids.back();
               avoids.pop_back();
               UtVec2d vecDist = nextAvoidPtr->mCircle.mLoc - totalAvoidPtr->mCircle.mLoc;
               double  dist    = vecDist.Magnitude();
               if (totalAvoidPtr->mCircle.mRadius >= (dist + nextAvoidPtr->mCircle.mRadius))
               {
                  // total already encompasses the next one, ignore it
               }
               else if (nextAvoidPtr->mCircle.mRadius > (dist + totalAvoidPtr->mCircle.mRadius))
               {
                  // next one is larger, swap it in
                  totalAvoidPtr->mCircle = nextAvoidPtr->mCircle;
               }
               else
               {
                  vecDist *=
                     (nextAvoidPtr->mCircle.mRadius / (nextAvoidPtr->mCircle.mRadius + totalAvoidPtr->mCircle.mRadius));
                  totalAvoidPtr->mCircle.mLoc += vecDist;
                  totalAvoidPtr->mCircle.mRadius =
                     (nextAvoidPtr->mCircle.mRadius + totalAvoidPtr->mCircle.mRadius + dist) / 2.0;
               }

               if (avoids.empty())
               {
                  vec = mTarget - totalAvoidPtr->mCircle.mLoc;
                  vec.Normalize();
                  vec.Multiply(totalAvoidPtr->mCircle.mRadius * (mPercentAvoidancePad + mPercentAvoidancePad - 1.0));
                  mTarget                           = totalAvoidPtr->mCircle.mLoc + vec;
                  mTargetAvoidancePtr->mCircle.mLoc = mTarget;
                  avoids                            = GetAvoidancesContaining(mTarget);
               }
            }
         }

         // Avoidance* avoidPtr = GetNearestAvoidanceContaining(mTarget);
         // if (avoidPtr!=0)
         //{
         //    //find a new target location that is outside of this avoidance (& any other overlapping avoidance
         //    circles) vec = mTarget - avoidPtr->mCircle.mLoc; double scale = (avoidPtr->mCircle.mRadius *
         //    mPercentAvoidancePad) / vec.Magnitude(); vec.Multiply(scale); mTarget = avoidPtr->mCircle.mLoc + vec;
         //    mTargetAvoidancePtr->mCircle.mLoc = mTarget;
         // }

         // shift starting point outside of all overlapping avoidances
         // std::vector<WsfRouteFinder::Avoidance*> avoids = GetAvoidancesContaining(mFirst);
         avoids = GetAvoidancesContaining(mFirst);
         if (!avoids.empty())
         {
            Avoidance* totalAvoidPtr = new Avoidance(*avoids.back());
            // avoids.pop_back();
            while (!avoids.empty())
            {
               Avoidance* nextAvoidPtr = avoids.back();
               avoids.pop_back();
               UtVec2d vecDist = nextAvoidPtr->mCircle.mLoc - totalAvoidPtr->mCircle.mLoc;
               double  dist    = vecDist.Magnitude();
               if (totalAvoidPtr->mCircle.mRadius >= (dist + nextAvoidPtr->mCircle.mRadius))
               {
                  // total already encompasses the next one, ignore it
               }
               else if (nextAvoidPtr->mCircle.mRadius > (dist + totalAvoidPtr->mCircle.mRadius))
               {
                  // next one is larger, swap it in
                  totalAvoidPtr->mCircle = nextAvoidPtr->mCircle;
               }
               else
               {
                  vecDist *=
                     (nextAvoidPtr->mCircle.mRadius / (nextAvoidPtr->mCircle.mRadius + totalAvoidPtr->mCircle.mRadius));
                  totalAvoidPtr->mCircle.mLoc += vecDist;
                  totalAvoidPtr->mCircle.mRadius =
                     (nextAvoidPtr->mCircle.mRadius + totalAvoidPtr->mCircle.mRadius + dist) / 2.0;
               }

               if (avoids.empty())
               {
                  vec = mFirst - totalAvoidPtr->mCircle.mLoc;
                  vec.Normalize();
                  vec.Multiply(totalAvoidPtr->mCircle.mRadius * (mPercentAvoidancePad + mPercentAvoidancePad - 1.0));
                  mFirst                            = totalAvoidPtr->mCircle.mLoc + vec;
                  mSourceAvoidancePtr->mCircle.mLoc = mFirst;
                  avoids                            = GetAvoidancesContaining(mFirst);
               }
            }
         }
      }
      break;

      case SHRINK_AVOIDANCES:
      default:
      {
         std::vector<Avoidance*> avoids = GetAvoidancesContaining(mTarget);
         for (Avoidance* ptr : avoids)
         {
            vec = mTarget - ptr->mCircle.mLoc;
            ptr->SetRadius(vec.Magnitude() /
                           (mPercentAvoidancePad + mPercentAvoidancePad -
                            1.0)); // use this instead of accessing radius directly, it updates the avoidance meta data
         }
         avoids = GetAvoidancesContaining(mSource);
         for (Avoidance* ptr : avoids)
         {
            vec = mSource - ptr->mCircle.mLoc;
            ptr->SetRadius(vec.Magnitude() /
                           (mPercentAvoidancePad + mPercentAvoidancePad -
                            1.0)); // use this instead of accessing radius directly, it updates the avoidance meta data
         }
      }
      break;
      };


      // sort all circles we need to avoid (based on their nearest point to me)
      std::sort(mAvoidances.begin(), mAvoidances.end());
      int N = (int)mAvoidances.size();
      for (int i = 0; i < N; ++i)
      {
         for (int j = (i + 1); j < N; ++j)
         {
            if (Intersects(mAvoidances[i].mCircle, mAvoidances[j].mCircle))
            {
               mAvoidances[i].mOverlaps.push_back(&mAvoidances[j]);
               mAvoidances[j].mOverlaps.push_back(&mAvoidances[i]);
            }
         }
      }

      // do all the heavy lifting
      GenerateGraph();
   }

   // a new graph (or a new starting branch) is being used, find the path
   std::vector<GraphBranch*> path;
   if (branchIndex > 0)
   {
      path = FindGraphPathUsingBranch(branchIndex);
   }
   else
   {
      path = FindBestGraphPath();
   }

   // convert the vector of branches to a WsfRoute
   // calculate radial accel where necessary (every other point, to round the avoidance circles)
   // scale altitude for a constant climb/dive across the whole route
   double endAlt = mGeoTarget.GetAlt();
   double delta  = endAlt - mGeoSource.GetAlt();
   double length = mGeoSource.GetDistanceFrom(mGeoTarget);
   for (auto it = path.begin(); it != path.end(); ++it)
   {
      GraphBranch* branchPtr = *it;
      double       rad = branchPtr->mNodePtr->mAvoidPtr->mCircle.mRadius; // radius of turn at end of "branch->mToPoint"
      rad *= mPercentAvoidancePad;                                        // dont forget the padding
      rad             = (rad > 0) ? (rad) : (1); // force a small tight 1 meter radius circle if zero radius
      double radAccel = speed * speed / rad;

      WsfGeoPoint pt1 = GetGlobal(branchPtr->mFromPoint);
      // scale alt based on "distance along route" towards end
      double ptAlt = endAlt - ((pt1.GetDistanceFrom(mGeoTarget) / length) * delta);
      // assert(!(ptAlt != ptAlt));
      pt1.SetAltitudeMSL(ptAlt);
      WsfWaypoint wpt1(pt1.GetLat(), pt1.GetLon(), pt1.GetAlt(), speed);
      wpt1.SetRadialAccel(radAccel);
      mRoute.Append(wpt1);

      WsfGeoPoint pt2 = GetGlobal(branchPtr->mToPoint);
      // scale alt based on "distance along route" towards end
      ptAlt = endAlt - ((pt2.GetDistanceFrom(mGeoTarget) / length) * delta);
      pt2.SetAltitudeMSL(ptAlt);
      WsfWaypoint wpt2(pt2.GetLat(), pt2.GetLon(), pt2.GetAlt(), speed);
      wpt2.SetRadialAccel(radAccel);
      mRoute.Append(wpt2);

      // arc length of turn at end of branch?
      auto jt = std::next(it);
      if (jt != path.end())
      {
         GraphBranch* leavingBranch = *jt;
         // arc information
         UtVec2d  arcBeg    = branchPtr->mToPoint;
         UtVec2d  arcEnd    = leavingBranch->mFromPoint;
         Circle2d arcCircle = branchPtr->mNodePtr->mAvoidPtr->mCircle;

         // determine which direction we travel around the avoidance
         // use the branch leaving the avoidance, not the branch entering it
         UtVec2d v1 = leavingBranch->mFromPoint - arcCircle.mLoc;
         // UtVec2d v2 = branch->mToPoint - branch->mFromPoint;
         UtVec2d           v2 = leavingBranch->mToPoint - leavingBranch->mFromPoint;
         OrientationAround ori =
            CLOCKWISE; // dont care about the case of radius == 0 here, computed arc-length will be zero
         if (v1.CrossProduct(v2) > 0)
         {
            ori = COUNTERCLOCKWISE;
         }

         v1           = arcBeg - arcCircle.mLoc; // vec from circle center to start of arc
         v2           = arcEnd - arcCircle.mLoc; // vec from circle center to end of arc
         double angle = atan2(v1[0] * v2[1] - v1[1] * v2[0], v1[0] * v2[0] + v1[1] * v2[1]); // signed angle

         if ((ori == CLOCKWISE && v1.CrossProduct(v2) > 0) || (ori == COUNTERCLOCKWISE && v1.CrossProduct(v2) < 0))
         {
            // make the arc take the long way around the circle
            angle = (angle < 0) ? (angle + 2 * UtMath::cPI) : (angle - 2 * UtMath::cPI);
         }
         double arcLength = arcCircle.mRadius * std::abs(angle);

         if (arcLength > mMaxArcLength)
         {
            int     numSegments    = 1 + int(arcLength / mMaxArcLength);
            double  angleIncrement = angle / double(numSegments);
            double  cosA           = cos(angleIncrement);
            double  sinA           = sin(angleIncrement);
            UtVec2d arcPt          = v1;
            for (int i = 1; i < numSegments; ++i)
            {
               double x = arcPt[0] * cosA - arcPt[1] * sinA;
               double y = arcPt[0] * sinA + arcPt[1] * cosA;
               arcPt.Set(x, y);
               UtVec2d realPoint = branchPtr->mNodePtr->mAvoidPtr->mCircle.mLoc + arcPt;

               // convert to geodetic way-point with speed & altitude -> then append to route
               WsfGeoPoint aPt = GetGlobal(realPoint);
               ptAlt           = endAlt - ((aPt.GetDistanceFrom(mGeoTarget) / length) *
                                 delta); // scale alt based on "distance along route" towards end
               aPt.SetAltitudeMSL(ptAlt);
               WsfWaypoint aWpt(aPt.GetLat(), aPt.GetLon(), aPt.GetAlt(), speed);
               aWpt.SetRadialAccel(radAccel);
               mRoute.Append(aWpt);
            }
         }

         // branch->mNodePtr->mAvoidPtr->mCircle.mLoc;  //center of turning circle
         // branch->mNodePtr->mAvoidPtr->mCircle.mRadius;  //radius of turning circle
         // branch->mToPoint;                          //begining of arc
         //(*jt)->mFromPoint                          //end of arc
         // branch->mFromPoint;                        //came from this direction (always useful? what if we start on arc?)
      }


      // every leg (of 2 points) on the route has an avoidance (at the end of the leg) to turn or bend around.
      WsfGeoPoint avoidPt = GetGlobal(branchPtr->mNodePtr->mAvoidPtr->mCircle.mLoc);
      mRouteAvoidances.Append(WsfWaypoint(avoidPt.GetLat(), avoidPt.GetLon(), avoidPt.GetAlt(), 0));
   }

   mCurrentRouteTime  = aTime;
   mAvoidancesChanged = false;
   return mRoute.Clone();
}


WsfRoute* WsfRouteFinder::RouteAvoidances()
{
   return mRouteAvoidances.Clone();
}

// =================================================================================================
WsfRouteFinder::OrientationAround WsfRouteFinder::GraphBranch::Orientation()
{
   if (mNodePtr->mAvoidPtr->mCircle.mRadius > 0)
   {
      UtVec2d v1 = mToPoint - mNodePtr->mAvoidPtr->mCircle.mLoc;
      UtVec2d v2 = mToPoint - mFromPoint;
      if (v1.CrossProduct(v2) > 0)
      {
         return WsfRouteFinder::COUNTERCLOCKWISE;
      }
      return WsfRouteFinder::CLOCKWISE;
   }
   return WsfRouteFinder::NEITHER;
}

// =================================================================================================
WsfRouteFinder::GraphNode* WsfRouteFinder::GenerateGraph()
{
   // mBestDist = std::numeric_limits<double>::max();
   GraphNode* srcPtr = new GraphNode(mSourceAvoidancePtr);
   GraphNode* tgtPtr = new GraphNode(mTargetAvoidancePtr);
   mGraphNodes.push_back(srcPtr);
   mGraphNodes.push_back(tgtPtr);
   for (auto& avoidance : mAvoidances)
   {
      mGraphNodes.push_back(new GraphNode(&avoidance));
   }

   Line2d ocw, occw, icw, iccw; // tangent lines (outer/inner) and (counter/clockwise)
   int    N = (int)mGraphNodes.size();
   for (int i = 0; i < N; ++i)
   {
      GraphNode* begNodePtr = mGraphNodes[i];
      bool       beg        = (begNodePtr->mAvoidPtr->mCircle.mRadius > 0);
      for (int j = 0; j < N; ++j)
      {
         if (i != j)
         {
            // add all branches from beg to end, if they are valid
            GraphNode* endNodePtr = mGraphNodes[j];
            bool       end        = (endNodePtr->mAvoidPtr->mCircle.mRadius > 0);
            if (beg || end) // if one of these nodes is an avoidance with a size, check both sides
            {
               if (OuterTangents(begNodePtr->mAvoidPtr->mCircle, endNodePtr->mAvoidPtr->mCircle, ocw, occw))
               {
                  if (!IntersectsAnAvoidance(ocw))
                  {
                     begNodePtr->mBranches.push_back(new GraphBranch(ocw, endNodePtr));
                  }
                  if (!IntersectsAnAvoidance(occw))
                  {
                     begNodePtr->mBranches.push_back(new GraphBranch(occw, endNodePtr));
                  }
               }
            }
            else
            {
               // neither has size? then this is a branch from start point to target point
               Line2d straightShot(begNodePtr->mAvoidPtr->mCircle.mLoc, endNodePtr->mAvoidPtr->mCircle.mLoc);
               if (!IntersectsAnAvoidance(straightShot))
               {
                  begNodePtr->mBranches.push_back(new GraphBranch(straightShot, endNodePtr));
               }
            }

            if (beg && end) // if both of these nodes have size, check inner tangents for branches too
            {
               if (InnerTangents(begNodePtr->mAvoidPtr->mCircle, endNodePtr->mAvoidPtr->mCircle, icw, iccw))
               {
                  if (!IntersectsAnAvoidance(icw))
                  {
                     begNodePtr->mBranches.push_back(new GraphBranch(icw, endNodePtr));
                  }
                  if (!IntersectsAnAvoidance(iccw))
                  {
                     begNodePtr->mBranches.push_back(new GraphBranch(iccw, endNodePtr));
                  }
               }
            }
         }
      }
   }
   return srcPtr;
}

// =================================================================================================
std::vector<WsfRouteFinder::GraphBranch*> WsfRouteFinder::FindBestGraphPath()
{
   mBestPathDist = std::numeric_limits<double>::max();
   mBestPath.clear();
   mGraphPathStack.clear();
   GraphNode* firstPtr = mGraphNodes[0];
   UtVec2d    origin   = firstPtr->mAvoidPtr->mCircle.mLoc;
   FindBestGraphPathRecursive(firstPtr, origin, 0); // first graph node should be starter node (2nd one is end node)
   return mBestPath;
}

// =================================================================================================
// searches mGraphNodes down branch at index
// note: index 0 = best branch
//      index 1 = 2nd best branch
//      etc...
std::vector<WsfRouteFinder::GraphBranch*> WsfRouteFinder::FindGraphPathUsingBranch(unsigned int index)
{
   mBestPathDist = std::numeric_limits<double>::max();
   mBestPath.clear();
   mGraphPathStack.clear();
   GraphNode* firstPtr = mGraphNodes[0];
   UtVec2d    origin   = firstPtr->mAvoidPtr->mCircle.mLoc;

   // limit which branch we use (can not use a branch that does not exist)
   if (index >= firstPtr->mBranches.size())
   {
      index = (int)firstPtr->mBranches.size() - 1;
   }
   std::vector<GraphBranch*> lNotTheseBranches;

   // iterate over best paths that are indexed before the path we want, mark their first node as visited, & keep searching
   while (index > 0)
   {
      FindBestGraphPathRecursive(firstPtr, origin, 0); // first graph node should be starter node (2nd one is end node)
      // save off branch used for this iteration, mark all undesired branches as visited
      lNotTheseBranches.push_back(mBestPath[0]);
      for (auto& lNotThisBranch : lNotTheseBranches)
      {
         lNotThisBranch->mVisited = true;
      }
      mBestPathDist = std::numeric_limits<double>::max();
      mBestPath.clear();
      mGraphPathStack.clear();

      index--;
   }

   FindBestGraphPathRecursive(firstPtr, origin, 0); // first graph node should be starter node (2nd one is end node) ???
                                                    // this comment still apply?

   // clear previous "visited" blocks in case another search is done on the same graph
   for (auto& lNotThisBranch : lNotTheseBranches)
   {
      lNotThisBranch->mVisited = false;
   }

   return mBestPath;
}

// =================================================================================================
void WsfRouteFinder::FindBestGraphPathRecursive(GraphNode* next, UtVec2d& prev, double dist, OrientationAround enteringOrientation)
{
   if (dist > mBestPathDist) // already longer than previously found path, stop looking down this branch
   {
      return;
   }

   Avoidance* leavingAvoidancePtr = next->mAvoidPtr;

   if (leavingAvoidancePtr->mID == mTargetAvoidancePtr->mID)
   {
      // we've reached the target node
      mBestPathDist = dist;
      mBestPath     = mGraphPathStack;
      return;
   }

   for (GraphBranch* branchPtr : next->mBranches)
   {
      if (!branchPtr->mVisited)
      {
         branchPtr->mVisited = true;
         bool possible       = true;

         OrientationAround leavingOrientation = NEITHER;

         if (leavingAvoidancePtr->mCircle.mRadius > 0)
         {
            // first find the clockwise or counterclockwise trip that must be made, then check overlapping avoidances on that arc
            UtVec2d v1         = branchPtr->mFromPoint - leavingAvoidancePtr->mCircle.mLoc;
            UtVec2d v2         = branchPtr->mToPoint - branchPtr->mFromPoint;
            leavingOrientation = CLOCKWISE;
            if (v1.CrossProduct(v2) > 0)
            {
               leavingOrientation = COUNTERCLOCKWISE;
            }

            // cant leave this avoidance in a different orientation that you entered
            if (!(enteringOrientation & leavingOrientation))
            {
               possible            = false;
               branchPtr->mVisited = false;
               continue;
            }

            // now make v1 and v2 the start & end points of the arc, respectively
            v1 = prev - leavingAvoidancePtr->mCircle.mLoc;
            v2 = branchPtr->mFromPoint - leavingAvoidancePtr->mCircle.mLoc;

            for (Avoidance* overlapPtr : leavingAvoidancePtr->mOverlaps)
            {
               // make sure the hop from the previous branch leg to this one is viable (not blocked by an overlapping avoidance)
               UtVec2d v3 = overlapPtr->mCircle.mLoc - leavingAvoidancePtr->mCircle.mLoc;
               if (leavingOrientation == CLOCKWISE && v1.CrossProduct(v3) < 0 && v2.CrossProduct(v3) > 0)
               {
                  // this overlapping avoidance is on the arc between the start & end points
                  possible = false; // branch not possible, check another
                  break;
               }
               else if (leavingOrientation == COUNTERCLOCKWISE && v1.CrossProduct(v3) > 0 && v2.CrossProduct(v3) < 0)
               {
                  // this overlapping avoidance is on the arc between the start & end points
                  possible = false; // branch not possible, check another
                  break;
               }
            }
         }

         if (!possible)
         {
            branchPtr->mVisited = false;
            continue;
         }
         // if this point is reached, the "hop" from the previous branch to this one is viable
         // UtVec2d hop = branchPtr->mFromPoint - prev;
         UtVec2d len = branchPtr->mToPoint - branchPtr->mFromPoint;
         mGraphPathStack.push_back(branchPtr);
         // FindBestGraphPathRecursive(branch->mNodePtr, branch->mToPoint, dist + hop.Magnitude() + len.Magnitude());

         double arcLength = ArcLength(leavingAvoidancePtr->mCircle, prev, branchPtr->mFromPoint, leavingOrientation);
         FindBestGraphPathRecursive(branchPtr->mNodePtr,
                                    branchPtr->mToPoint,
                                    dist + arcLength + len.Magnitude(),
                                    branchPtr->Orientation());

         mGraphPathStack.pop_back();
         branchPtr->mVisited = false;
      }
   }
}

// =================================================================================================
bool WsfRouteFinder::Intersects(const Circle2d& a, const Circle2d& b)
{
   UtVec2d vec    = (a.mLoc - b.mLoc);
   double  distSq = vec.DotProduct(vec);
   double  addR   = a.mRadius + b.mRadius;
   double  subR   = a.mRadius - b.mRadius;
   if (distSq > (addR * addR) || distSq < (subR * subR)) // check for perimeter intersection and whole inclusion
   {
      return false;
   }
   return true;
}

// =================================================================================================
bool WsfRouteFinder::Intersection(const Line2d& line1, const Line2d& line2, UtVec2d& point)
{
   UtVec2d vec1 = line1.IntoVec();
   UtVec2d vec2 = line2.IntoVec();

   double s, t;
   double C = UtVec2d::CrossProduct(vec1, vec2);
   if (C == 0.0)
   {
      // line segments are parallel
      return false;
   }
   s = (-vec1[1] * (line1.mBeg[0] - line2.mBeg[0]) + vec1[0] * (line1.mBeg[1] - line2.mBeg[1])) / C;
   t = (vec2[0] * (line1.mBeg[1] - line2.mBeg[1]) - vec2[1] * (line1.mBeg[0] - line2.mBeg[0])) / C;

   if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
   {
      // line segments intersect, populate the point
      point[0] = line1.mBeg[0] + (t * vec1[0]);
      point[1] = line1.mBeg[1] + (t * vec1[1]);
      return true;
   }
   return false;
}

// =================================================================================================
bool WsfRouteFinder::Contains(const Circle2d& circle, const UtVec2d& point)
{
   UtVec2d vec    = (circle.mLoc - point);
   double  distSq = vec.DotProduct(vec);
   if (distSq > (circle.mRadius * circle.mRadius))
   {
      return false;
   }
   return true;
}

// =================================================================================================
bool WsfRouteFinder::Contains(const Circle2d& a, const Circle2d& b)
{
   UtVec2d vec    = (a.mLoc - b.mLoc);
   double  distSq = vec.DotProduct(vec);
   double  subR   = a.mRadius - b.mRadius;
   if ((subR * subR) > distSq)
   {
      return true;
   }
   return false;
}

// =================================================================================================
bool WsfRouteFinder::Intersects(const Line2d& line, const Circle2d& circle)
{
   // taken from: http://stackoverflow.com/questions/1073336/circle-line-collision-detection
   // If E is the starting point of the ray                        (src)
   //    .. and L is the end point of the ray                      (tgt)
   //    .. and C is the center of sphere you're testing against   (circle.mLoc)
   //    .. and r is the radius of that sphere                     (circle.mRadius)

   UtVec2d d = line.mEnd - line.mBeg;   // d = L - E ; // Direction vector of ray, from start to end
   UtVec2d f = line.mBeg - circle.mLoc; // f = E - C ; // Vector from center sphere to ray start

   double r = circle.mRadius;
   double a = d.DotProduct(d);
   double b = 2 * f.DotProduct(d);
   double c = f.DotProduct(f) - r * r;

   double discriminant = b * b - 4 * a * c;
   if (discriminant < 0)
   {
      // no intersection
      return false;
   }
   else
   {
      // ray didn't totally miss sphere, so there is a solution to the equation.
      discriminant = sqrt(discriminant);
      double t1    = (-b + discriminant) / (2 * a);
      double t2    = (-b - discriminant) / (2 * a);

      // 3x HIT cases:
      //          -o->             --|-->  |            |  --|->
      // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

      // 3x MISS cases:
      //       ->  o                     o ->              | -> |
      // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

      // LBM - I move "completely inside" as a hit case

      if (t1 >= 0 && t1 <= 1)
      {
         // solution on is ON THE RAY.
         return true;
      }
      else if (t2 >= 0 && t2 <= 1)
      {
         // is the solution ON THE RAY if t2 matches same criteria???
         return true;
      }
      else if ((t1 < 0 && t2 > 1) || (t2 < 0 && t1 > 1))
      {
         // the line is completely inside the circle
         return true;
      }
      else
      {
         // solution "out of range" of ray
         return false;
      }
      // use t2 for second point (if you want it)
   }
}

// =================================================================================================
void WsfRouteFinder::DrawRoute(double duration, const UtVec3d& color)
{
   // draw best path (route) found, if any
   double      altOffset = 1000;
   WsfGeoPoint pt1, pt2, prev;
   mDraw.SetDuration(duration);
   mDraw.SetColor(color[0], color[1], color[2]);

   mDraw.BeginLines();
   for (unsigned int i = 0; i < mBestPath.size(); ++i)
   {
      GraphBranch* branchPtr = mBestPath[i];
      // use geo points so we can get LLA and offset altitude higher (green path lines above the blue graph lines)
      pt1 = GetGlobal(branchPtr->mFromPoint);
      pt2 = GetGlobal(branchPtr->mToPoint);
      if (i > 0) // draw connecting hop
      {
         mDraw.VertexLLA(prev.GetLat(), prev.GetLon(), prev.GetAlt() + altOffset);
         mDraw.VertexLLA(pt1.GetLat(), pt1.GetLon(), pt1.GetAlt() + altOffset);
      }
      // draw main path leg
      mDraw.VertexLLA(pt1.GetLat(), pt1.GetLon(), pt1.GetAlt() + altOffset);
      mDraw.VertexLLA(pt2.GetLat(), pt2.GetLon(), pt2.GetAlt() + altOffset);
      prev = pt2;
   }
   mDraw.End();
}

// =================================================================================================
void WsfRouteFinder::DrawGraph(double duration, const UtVec3d& color)
{
   mDraw.SetDuration(duration);
   mDraw.SetColor(color[0], color[1], color[2]);
   double wcs1[3], wcs2[3];
   for (GraphNode* nodePtr : mGraphNodes)
   {
      mDraw.BeginLines();
      for (GraphBranch* branchPtr : nodePtr->mBranches)
      {
         GetGlobal(branchPtr->mFromPoint, wcs1);
         GetGlobal(branchPtr->mToPoint, wcs2);
         mDraw.VertexWCS(wcs1);
         mDraw.VertexWCS(wcs2);
      }
      mDraw.End();
   }
}

// =================================================================================================
void WsfRouteFinder::DrawAvoidances(double duration, const UtVec3d& color)
{
   mDraw.SetDuration(duration);
   mDraw.SetColor(color[0], color[1], color[2]);
   std::vector<Avoidance>::iterator it;
   for (it = mAvoidances.begin(); it != mAvoidances.end(); ++it)
   {
      DrawCircle(it->mOriginalPoint, it->mCircle.mRadius);
   }
}

// =================================================================================================
UtVec2d WsfRouteFinder::GetLocal(const double wcs[3])
{
   double ned[3];
   mReferenceEntity.ConvertWCSToNED(wcs, ned);
   UtVec2d result(ned[1], ned[0]); // east is X axis, north is Y axis
   return result;
}

// =================================================================================================
UtVec2d WsfRouteFinder::GetLocal(const WsfGeoPoint& pt)
{
   double wcs[3];
   pt.GetLocationWCS(wcs);
   return GetLocal(wcs);
}

// =================================================================================================
void WsfRouteFinder::GetGlobal(const UtVec2d& pt, double wcs[3])
{
   double ned[3];
   ned[0] = pt[1]; // Y axis is north
   ned[1] = pt[0]; // X axis is east
   // ned[2] = 0.0;     //mGeoSource
   ned[2] = -mGeoSource.GetAlt();
   mReferenceEntity.ConvertNEDToWCS(ned, wcs);
}

// =================================================================================================
WsfGeoPoint WsfRouteFinder::GetGlobal(const UtVec2d& pt)
{
   double wcs[3];
   GetGlobal(pt, wcs);
   WsfGeoPoint result(wcs);
   return result;
}

// =================================================================================================
void WsfRouteFinder::DrawCircle(const WsfGeoPoint& center, double radius)
{
   double wcs[3];
   center.GetLocationWCS(wcs);
   mDraw.BeginEllipse(0.0, radius, radius);
   mDraw.VertexWCS(wcs);
   mDraw.End();
}

// =================================================================================================
std::pair<UtVec2d, UtVec2d> WsfRouteFinder::TangentPoints(const UtVec2d& src, const Circle2d& circle, bool onEdge)
{
   UtVec2d vec = circle.mLoc - src; // line from point to circle

   double x, y, h;
   h = vec.Magnitude(); // given
   y = circle.mRadius;  // given
   if (!onEdge)
   {
      // make the tangent point fall just outside the circle, not on the edge
      // artificially "grow" the circle by a small bit
      // y += mSmallFudge;
      y *= mPercentAvoidancePad;
   }
   if (y > h)
   {
      h = y + mSmallFudge;
   }
   x            = sqrt(h * h - y * y);   // length of tangent line
   double alpha = asin(y / h);           // angle between vec and tangent line
   double beta  = atan2(vec[1], vec[0]); // angle between vec and horizontal

   double theta1 = beta - alpha;
   double theta2 = beta + alpha;

   UtVec2d tangent1 = src;
   tangent1[0] += x * cos(theta1);
   tangent1[1] += x * sin(theta1);

   UtVec2d tangent2 = src;
   tangent2[0] += x * cos(theta2);
   tangent2[1] += x * sin(theta2);

   // assert(!(tangent1[0] != tangent1[0]));
   // assert(!(tangent1[1] != tangent1[1]));
   // assert(!(tangent2[0] != tangent2[0]));
   // assert(!(tangent2[1] != tangent2[1]));

   return std::make_pair(tangent1, tangent2);
}

// =================================================================================================
double WsfRouteFinder::ArcLength(const Circle2d& circle, const UtVec2d& fromPt, const UtVec2d& toPt)
{
   UtVec2d v1 = fromPt - circle.mLoc; // vec from circle center to start of arc
   UtVec2d v2 = toPt - circle.mLoc;   // vec from circle center to end of arc
   // double angle = acos( v1.DotProduct(v2) / (v1.Magnitude() * v2.Magnitude()) );  //angle without sign (always
   // positive rotation?)
   double angle     = atan2(v1[0] * v2[1] - v1[1] * v2[0], v1[0] * v2[0] + v1[1] * v2[1]);
   double arcLength = circle.mRadius * std::abs(angle);
   return arcLength;
}

// =================================================================================================
double WsfRouteFinder::ArcLength(const Circle2d& circle, const UtVec2d& fromPt, const UtVec2d& toPt, OrientationAround orientation)
{
   UtVec2d v1    = fromPt - circle.mLoc; // vec from circle center to start of arc
   UtVec2d v2    = toPt - circle.mLoc;   // vec from circle center to end of arc
   double  angle = atan2(v1[0] * v2[1] - v1[1] * v2[0], v1[0] * v2[0] + v1[1] * v2[1]);
   if ((orientation == CLOCKWISE && v1.CrossProduct(v2) > 0) ||
       (orientation == COUNTERCLOCKWISE && v1.CrossProduct(v2) < 0))
   {
      // make the arc take the long way around the circle
      angle = (angle < 0) ? (angle + 2 * UtMath::cPI) : (angle - 2 * UtMath::cPI);
   }
   double arcLength = circle.mRadius * std::abs(angle);
   return arcLength;
}

// =================================================================================================
bool WsfRouteFinder::OuterTangents(const Circle2d& a, const Circle2d& b, Line2d& cw, Line2d& ccw, bool onEdge)
{
   // find tangent lines between outside edges of the circles

   if (Contains(a, b))
   {
      // no outside tangent lines, one circle is completely contained within the other
      return false;
   }
   else if (a.mRadius == b.mRadius)
   {
      // outside tangent lines are parallel, no outside intersection point
      // simpler perpendicular calculation possible
      cw.mBeg    = a.mLoc;
      cw.mEnd    = b.mLoc;
      ccw        = cw;
      UtVec2d ab = (b.mLoc - a.mLoc); // line from a to b
      // cw perpendicular line to ab (for cw around circle)
      UtVec2d perp1(-ab[1], ab[0]);
      UtVec2d perp2(ab[1], -ab[0]);

      // scale perp down to radius size
      double aR = a.mRadius;
      if (!onEdge)
      {
         // aR += mSmallFudge;
         aR *= mPercentAvoidancePad;
      }
      double scale = aR / ab.Magnitude();
      perp1.Multiply(scale);
      perp2.Multiply(scale);
      // now shift tangent into place
      cw.mBeg.Add(perp1);
      cw.mEnd.Add(perp1);
      ccw.mBeg.Add(perp2);
      ccw.mEnd.Add(perp2);
      return true;
   }
   else
   {
      // shrink both circles by the smaller radius so the smaller circle is a point
      // find tangent from point to new large circle (that is smaller now)
      // shift the line out to original circles radius
      if (a.mRadius > b.mRadius)
      {
         double aR = a.mRadius;
         double bR = b.mRadius;
         if (!onEdge)
         {
            // aR += mSmallFudge;
            // bR += mSmallFudge;
            aR *= mPercentAvoidancePad;
            bR *= mPercentAvoidancePad;
         }
         double                      r        = aR - bR;
         UtVec2d                     circlePt = a.mLoc;
         UtVec2d                     point    = b.mLoc;
         Circle2d                    temp1(circlePt, r);
         std::pair<UtVec2d, UtVec2d> tanPts   = TangentPoints(point, temp1, true);
         UtVec2d                     shiftCW  = (tanPts.first - circlePt);
         UtVec2d                     shiftCCW = (tanPts.second - circlePt);
         double                      scale    = bR / r;
         shiftCW.Multiply(scale);
         shiftCCW.Multiply(scale);
         cw.mBeg  = tanPts.first + shiftCW;
         cw.mEnd  = point + shiftCW;
         ccw.mBeg = tanPts.second + shiftCCW;
         ccw.mEnd = point + shiftCCW;
      }
      else
      {
         double aR = a.mRadius;
         double bR = b.mRadius;
         if (!onEdge)
         {
            // aR += mSmallFudge;
            // bR += mSmallFudge;
            aR *= mPercentAvoidancePad;
            bR *= mPercentAvoidancePad;
         }
         double  r        = bR - aR;
         UtVec2d circlePt = b.mLoc;
         UtVec2d point    = a.mLoc;

         Circle2d                    temp2(circlePt, r);
         std::pair<UtVec2d, UtVec2d> tanPts   = TangentPoints(point, temp2);
         UtVec2d                     shiftCW  = (tanPts.second - circlePt);
         UtVec2d                     shiftCCW = (tanPts.first - circlePt);
         double                      scale    = aR / r;
         shiftCW.Multiply(scale);
         shiftCCW.Multiply(scale);
         cw.mBeg  = point + shiftCW;
         cw.mEnd  = tanPts.second + shiftCW;
         ccw.mBeg = point + shiftCCW;
         ccw.mEnd = tanPts.first + shiftCCW;

         // assert(!(cw.mBeg[0] != cw.mBeg[0]));
         // assert(!(cw.mBeg[1] != cw.mBeg[1]));
         // assert(!(cw.mEnd[0] != cw.mEnd[0]));
         // assert(!(cw.mEnd[1] != cw.mEnd[1]));
         // assert(!(ccw.mBeg[0] != ccw.mBeg[0]));
         // assert(!(ccw.mBeg[1] != ccw.mBeg[1]));
         // assert(!(ccw.mEnd[0] != ccw.mEnd[0]));
         // assert(!(ccw.mEnd[1] != ccw.mEnd[1]));
      }
      return true;
   }
}

// =================================================================================================
bool WsfRouteFinder::InnerTangents(const Circle2d& a, const Circle2d& b, Line2d& cw, Line2d& ccw, bool onEdge)
{
   // find tangent lines between inside edges of the circles

   if (Intersects(a, b))
   {
      // no inside tangents
      // ignore case where they barely touch at one point
      // we are interested in pathing between them
      return false;
   }
   else
   {
      // grow the larger circle by the smaller circle's radius
      // shrink the smaller circle to a point
      // find tangent from point to new large circle (that is even larger now)
      // shift the line back to original circles edge
      if (a.mRadius >= b.mRadius)
      {
         double aR = a.mRadius;
         double bR = b.mRadius;
         if (!onEdge)
         {
            // aR += mSmallFudge;
            // bR += mSmallFudge;
            aR *= mPercentAvoidancePad;
            bR *= mPercentAvoidancePad;
         }
         double  r         = aR + bR;
         UtVec2d circleLoc = a.mLoc;
         UtVec2d point     = b.mLoc;

         Circle2d                    temp1(circleLoc, r);
         std::pair<UtVec2d, UtVec2d> tanPts   = TangentPoints(point, temp1, true);
         UtVec2d                     shiftCW  = (tanPts.first - circleLoc);
         UtVec2d                     shiftCCW = (tanPts.second - circleLoc);
         double                      scale    = bR / r;
         shiftCW.Multiply(scale);
         shiftCCW.Multiply(scale);

         cw.mBeg  = tanPts.first - shiftCW;
         cw.mEnd  = point - shiftCW;
         ccw.mBeg = tanPts.second - shiftCCW;
         ccw.mEnd = point - shiftCCW;
      }
      else
      {
         double aR = a.mRadius;
         double bR = b.mRadius;
         if (!onEdge)
         {
            // aR += mSmallFudge;
            // bR += mSmallFudge;
            aR *= mPercentAvoidancePad;
            bR *= mPercentAvoidancePad;
         }
         double  r         = aR + bR;
         UtVec2d circleLoc = b.mLoc;
         UtVec2d point     = a.mLoc;

         Circle2d                    temp2(circleLoc, r);
         std::pair<UtVec2d, UtVec2d> tanPts   = TangentPoints(point, temp2, true);
         UtVec2d                     shiftCW  = (tanPts.first - circleLoc);
         UtVec2d                     shiftCCW = (tanPts.second - circleLoc);
         double                      scale    = aR / r;
         shiftCW.Multiply(scale);
         shiftCCW.Multiply(scale);

         cw.mBeg  = point - shiftCW;
         cw.mEnd  = tanPts.first - shiftCW;
         ccw.mBeg = point - shiftCCW;
         ccw.mEnd = tanPts.second - shiftCCW;
      }
      return true;
   }
}


// WsfRouteFinder::Avoidance* WsfRouteFinder::GetNearestAvoidanceIntersectedBy(Line2d& aLine)
//{
//    UtVec2d util;
//    double distNer = util.Subtract(aLine.mBeg, mSource).Magnitude();
//    double distFar = util.Subtract(aLine.mEnd, mSource).Magnitude();
//    if (distFar < distNer)
//    {
//       double temp = distNer;
//       distNer = distFar;
//       distFar = temp;
//    }
//
//    //determine whether to run through the avoidances in forward or reverse order
//    UtVec2d aVec = aLine.IntoVec();
//    UtVec2d aRef = mTarget - mSource;
//    if (aRef.DotProduct(aVec) < 0)   //test if the line passed in is oriented against the reference (source to target)
//    {
//       //it is, go through avoidances in reverse order
//       std::vector<Avoidance>::reverse_iterator rit;
//       for (rit=mAvoidances.rbegin(); rit!=mAvoidances.rend(); ++rit)
//       {
//          if (distNer < rit->mFar && distFar > rit->mNear)   //fast check, for if intersection is possible
//          {
//             if (Intersects(aLine, rit->mCircle))
//             {
//                return &(*rit);
//             }
//          }
//       }
//    }
//    else
//    {
//       //its oriented with
//       std::vector<Avoidance>::iterator it;
//       for (it=mAvoidances.begin(); it!=mAvoidances.end(); ++it)
//       {
//          if (distNer < it->mFar && distFar > it->mNear)   //fast check, for if intersection is possible
//          {
//             if (Intersects(aLine, it->mCircle))
//             {
//                return &(*it);
//             }
//          }
//       }
//    }
//    //the line did not intersect any avoidances
//    return 0;
// }


// WsfRouteFinder::Avoidance* WsfRouteFinder::GetNearestAvoidanceContaining(UtVec2d& aPoint)
//{
//    UtVec2d util;
//    double dist = util.Subtract(aPoint, mSource).Magnitude();
//    std::vector<Avoidance>::iterator it;
//    for (it=mAvoidances.begin(); it!=mAvoidances.end(); ++it)
//    {
//       if (dist < it->mFar && dist > it->mNear)   //fast check, for if containment is even possible
//       {
//          if (Contains(it->mCircle, aPoint))
//          {
//             return &(*it);
//          }
//       }
//    }
//    //the point was not contained by any avoidances
//    return 0;
// }


// =================================================================================================
std::vector<WsfRouteFinder::Avoidance*> WsfRouteFinder::GetAvoidancesIntersectedBy(const Line2d& aLine)
{
   std::vector<WsfRouteFinder::Avoidance*> ret;
   UtVec2d                                 util;
   double                                  distNer = util.Subtract(aLine.mBeg, mSource).Magnitude();
   double                                  distFar = util.Subtract(aLine.mEnd, mSource).Magnitude();
   if (distFar < distNer)
   {
      double temp = distNer;
      distNer     = distFar;
      distFar     = temp;
   }
   std::vector<Avoidance>::iterator it;
   for (it = mAvoidances.begin(); it != mAvoidances.end(); ++it)
   {
      if (distNer < it->mFar && distFar > it->mNear) // fast check, for if intersection is possible
      {
         if (Intersects(aLine, it->mCircle))
         {
            ret.push_back(&(*it));
         }
      }
   }
   return ret;
}

// =================================================================================================
bool WsfRouteFinder::IntersectsAnAvoidance(const Line2d& aLine)
{
   UtVec2d util;
   double  distNer = util.Subtract(aLine.mBeg, mSource).Magnitude();
   double  distFar = util.Subtract(aLine.mEnd, mSource).Magnitude();
   if (distFar < distNer)
   {
      double temp = distNer;
      distNer     = distFar;
      distFar     = temp;
   }
   std::vector<Avoidance>::iterator it;
   for (it = mAvoidances.begin(); it != mAvoidances.end(); ++it)
   {
      if (distNer < it->mFar && distFar > it->mNear) // fast check, for if intersection is possible
      {
         if (Intersects(aLine, it->mCircle))
         {
            return true;
         }
      }
   }
   return false;
}

// =================================================================================================
bool WsfRouteFinder::LocalInsertAvoidance(const Avoidance& avoid)
{
   std::vector<Avoidance>::iterator it;
   for (it = mAvoidances.begin(); it != mAvoidances.end(); ++it)
   {
      if (avoid == *it)
      {
         return false;
      }
   }
   mAvoidances.push_back(avoid);
   return true;
}

// =================================================================================================
std::vector<WsfRouteFinder::Avoidance*> WsfRouteFinder::GetAvoidancesContaining(const UtVec2d& aPoint)
{
   std::vector<WsfRouteFinder::Avoidance*> ret;
   UtVec2d                                 util;
   double                                  dist = util.Subtract(aPoint, mSource).Magnitude();
   std::vector<Avoidance>::iterator        it;
   for (it = mAvoidances.begin(); it != mAvoidances.end(); ++it)
   {
      if (dist < it->mFar && dist > it->mNear) // fast check, for if containment is even possible
      {
         if (Contains(it->mCircle, aPoint))
         {
            ret.push_back(&(*it));
         }
      }
   }
   return ret;
}

// =================================================================================================
WsfScriptRouteFinderClass::WsfScriptRouteFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   SetClassName("WsfRouteFinder");

   AddMethod(ut::make_unique<Avoid_1>("Avoid")); // Avoid(WsfPlatform refLocation, double radius);
   AddMethod(ut::make_unique<Avoid_2>("Avoid")); // Avoid(WsfGeopoint refLocation, double radius);
   AddMethod(ut::make_unique<ClearAvoidances>());
   AddMethod(ut::make_unique<Route_1>("Route"));  // Route (double, WsfGeoPoint, WsfGeoPoint, double)
   AddMethod(ut::make_unique<Route_2>("Route"));  // Route (double, WsfGeoPoint, WsfGeoPoint, double, int)
   AddMethod(ut::make_unique<RouteAvoidances>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<DrawRoute>());
   AddMethod(ut::make_unique<DrawGraph>());
   AddMethod(ut::make_unique<DrawAvoidances>());
   AddMethod(ut::make_unique<ImpossibleRouteResponse>());
   AddMethod(ut::make_unique<SetImpossibleRouteResponse>());
   AddMethod(ut::make_unique<SetMaxArcLength>());
}

WsfScriptRouteFinderClass::~WsfScriptRouteFinderClass() {}

// virtual
void* WsfScriptRouteFinderClass::Create(const UtScriptContext& aContext)
{
   return static_cast<void*>(new WsfRouteFinder(WsfScriptContext::GetSIMULATION(aContext)));
}

// virtual
void* WsfScriptRouteFinderClass::Clone(void* aObjectPtr)
{
   WsfRouteFinder* objPtr = static_cast<WsfRouteFinder*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptRouteFinderClass::Destroy(void* aObjectPtr)
{
   WsfRouteFinder* objPtr = static_cast<WsfRouteFinder*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, Avoid_1, 2, "void", "WsfPlatform, double")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Avoid(*platformPtr, aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, Avoid_2, 2, "void", "WsfGeoPoint, double")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Avoid(*pointPtr, aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, ClearAvoidances, 0, "void", "")
{
   aObjectPtr->ClearAvoidances();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, Route_1, 4, "WsfRoute", "double, WsfGeoPoint, WsfGeoPoint, double")
{
   double       time        = aVarArgs[0].GetDouble();
   WsfGeoPoint* srcPtr      = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfGeoPoint* tgtPtr      = static_cast<WsfGeoPoint*>(aVarArgs[2].GetPointer()->GetAppObject());
   double       speed       = aVarArgs[3].GetDouble();
   WsfRoute*    solutionPtr = aObjectPtr->Route(time, *srcPtr, *tgtPtr, speed);
   aReturnVal.SetPointer(solutionPtr->ScriptRefManage());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, Route_2, 5, "WsfRoute", "double, WsfGeoPoint, WsfGeoPoint, double, int")
{
   double       time        = aVarArgs[0].GetDouble();
   WsfGeoPoint* srcPtr      = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfGeoPoint* tgtPtr      = static_cast<WsfGeoPoint*>(aVarArgs[2].GetPointer()->GetAppObject());
   double       speed       = aVarArgs[3].GetDouble();
   int          branchIndex = aVarArgs[4].GetInt();
   WsfRoute*    solutionPtr = aObjectPtr->Route(time, *srcPtr, *tgtPtr, speed, branchIndex);
   aReturnVal.SetPointer(solutionPtr->ScriptRefManage());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, RouteAvoidances, 0, "WsfRoute", "")
{
   WsfRoute* routeAvoidancesPtr = aObjectPtr->RouteAvoidances();
   aReturnVal.SetPointer(routeAvoidancesPtr->ScriptRefManage());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, DrawRoute, 2, "void", "double, Vec3")
{
   double   duration = aVarArgs[0].GetDouble();
   UtVec3d* colorPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->DrawRoute(duration, *colorPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, DrawGraph, 2, "void", "double, Vec3")
{
   double   duration = aVarArgs[0].GetDouble();
   UtVec3d* colorPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->DrawGraph(duration, *colorPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, DrawAvoidances, 2, "void", "double, Vec3")
{
   double   duration = aVarArgs[0].GetDouble();
   UtVec3d* colorPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->DrawAvoidances(duration, *colorPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, ImpossibleRouteResponse, 0, "string", "")
{
   switch (aObjectPtr->GetImpossibleRouteResponse())
   {
   case WsfRouteFinder::IGNORE_IMPOSSIBLE_AVOIDANCES:
   {
      aReturnVal.SetString("IGNORE");
   }
   break;
   case WsfRouteFinder::SHIFT_OUTSIDE_AVOIDANCES:
   {
      aReturnVal.SetString("SHIFT");
   }
   break;
   case WsfRouteFinder::SHRINK_AVOIDANCES:
   default:
   {
      aReturnVal.SetString("SHRINK");
   }
   break;
   };
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, SetImpossibleRouteResponse, 1, "void", "string")
{
   std::string response = aVarArgs[0].GetString();
   if (response == "IGNORE")
   {
      aObjectPtr->SetImpossibleRouteResponse(WsfRouteFinder::IGNORE_IMPOSSIBLE_AVOIDANCES);
   }
   else if (response == "SHIFT")
   {
      aObjectPtr->SetImpossibleRouteResponse(WsfRouteFinder::SHIFT_OUTSIDE_AVOIDANCES);
   }
   else // if (response == "SHRINK")
   {
      aObjectPtr->SetImpossibleRouteResponse(WsfRouteFinder::SHRINK_AVOIDANCES);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteFinderClass, WsfRouteFinder, SetMaxArcLength, 1, "void", "double")
{
   double length = aVarArgs[0].GetDouble();
   aObjectPtr->SetMaxArcLength(length);
}
