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

#include "UtQuadTreeGeneric.hpp"

#include "UtBoundingBox.hpp"
#include "UtEntity.hpp"
#include "UtLatLonDatum.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtScriptRef.hpp"

namespace
{
static const double sqrtTwo = sqrt(2.0);
}

UtQuadTreeGeneric::UtQuadTreeGeneric()
{
   UtRectangle rect;
   double      bounds[2] = {-90, -180};
   rect.SetLeast(bounds);
   bounds[0] = 90;
   bounds[1] = 180;
   rect.SetGreatest(bounds);
   mTreePtr = new UtQuadTree(rect, 3);
}

UtQuadTreeGeneric::~UtQuadTreeGeneric()
{
   delete mTreePtr;
}

void UtQuadTreeGeneric::InRange(double aLatitude, double aLongitude, double aRange, UtScriptDataList& aScriptDataList)
{
   UtRectangle bBox = GetBoundingRectangle(aLatitude, aLongitude, aRange);
   GetPlatformsInRangeP(bBox, aRange, aScriptDataList, true);
}

UtRectangle UtQuadTreeGeneric::GetBoundingRectangle(double aLatitude, double aLongitude, double aRange) const
{
   // Form a lat-lon bounding box, using the range * sqrt(2) for the bounding box extents,
   // so that the entire circle with "aRange" as radius will be inscribed in the bounding rectangle.
   double   rectangularRange = aRange * sqrtTwo;
   double   minLatLon[2];
   double   maxLatLon[2];
   double   locNED[3] = {-rectangularRange, -rectangularRange, 0};
   double   alt;
   UtEntity entity;
   entity.SetLocationLLA(aLatitude, aLongitude, 0);
   entity.ConvertNEDToLLA(locNED, minLatLon[0], minLatLon[1], alt);
   locNED[0] = rectangularRange;
   locNED[1] = rectangularRange;
   entity.ConvertNEDToLLA(locNED, maxLatLon[0], maxLatLon[1], alt);
   return UtRectangle(minLatLon, maxLatLon); // Return-value optimization
}

void UtQuadTreeGeneric::GetPlatformsInRangeP(const UtRectangle& aBoundingBox,
                                             const double&      aRange,
                                             UtScriptDataList&  aScriptDataList,
                                             bool               aIsFirstCall)
{
   bool isOverTheDateLine = false;
   bool isOverThePole     = false;

   if (aIsFirstCall)
   {
      CheckBoundingBox(aBoundingBox, aRange, isOverThePole, isOverTheDateLine);
   }

   if ((!isOverTheDateLine) && (!isOverThePole))
   {
      //       UtRectangle rect;
      //       rect.SetGreatest(1, 100);
      //       rect.SetLeast(2, 50);
      //       std::list< UtSpatialNode<2>* > nodeList;
      //       mTreePtr->Search(rect, nodeList);
      mTreePtr->Search(aBoundingBox, mSearchList);
      if (!mSearchList.empty())
      {
         std::list<UtQuadNode*>::const_iterator iter = mSearchList.begin();
         while (iter != mSearchList.end())
         {
            UtQuadNode* nodePtr = *iter;
            GetPlatformsFromNode(*nodePtr, &aBoundingBox, aScriptDataList);
            ++iter;
         }
      }
   }
   else // Special cases: the search box was not contained in the spatial sorter's extents.
   {
      if (mIsWholeEarth)
      {
         GetPlatformsAroundEdges(aBoundingBox, aRange, isOverThePole, aScriptDataList);
      }
      else
      {
         //          throw WsfException("WsfSpatialSorter: Search bounding rectangle out of bounds (try specifying
         //          whole_earth option)");
         ut::log::error() << "Search bounding rectangle out of bounds. Try specifying whole_earth option.";
      }
   }
}

void UtQuadTreeGeneric::CheckBoundingBox(const UtRectangle& aBoundingBox,
                                         const double&      aRange,
                                         bool&              aIsOverThePole,
                                         bool&              aIsOverTheDateLine) const
{
   // Bounding box should be square.  If it is not, assume we have an overlap.
   // Short sides, in either lat or lon, indicate overlap.
   aIsOverTheDateLine = (aBoundingBox.GetGreatest(1) < aBoundingBox.GetLeast(1));
   // static const double cM_PER_DEG = UtMath::cM_PER_NM * 60.0;
   static const double poleRadius = 6356752.3;

   // First (quick) check- see whether the box lat extents are o.k.
   // If not, we are definitely over the pole.
   aIsOverThePole = (aBoundingBox.GetGreatest(0) < aBoundingBox.GetLeast(0));

   // Second check- see if the max lat could be over the pole.
   // Note that this is a quick, worst case check; we may not actually be over the pole.
   if (!aIsOverThePole)
   {
      // Moving up from a corner of the box diagonally towards the pole accounts for the square root of two.
      // Add 10% (giving the 2.1) for good measure.
      double absMaxLat = fabs(aBoundingBox.GetLeast(0)) + 2.1 * sqrtTwo * aRange / poleRadius * UtMath::cDEG_PER_RAD;
      aIsOverThePole   = (absMaxLat > 90.0); // approx.
   }
}

void UtQuadTreeGeneric::GetPlatformsFromNode(const UtQuadNode&  aNode,
                                             const UtRectangle* aBoundingBoxPtr,
                                             UtScriptDataList&  aScriptDataList) const
{
   const SpatialData&          listRef  = aNode.GetData();
   SpatialData::const_iterator listIter = listRef.begin();
   while (listIter != listRef.end())
   {
      if ((aBoundingBoxPtr == nullptr) || (*listIter)->IsWhollyContainedIn(*aBoundingBoxPtr))
      {
         const UtSpatialDatum* spatDatum = *listIter;
         if (spatDatum != nullptr)
         {
            aScriptDataList.push_back(UtScriptData((int)spatDatum->GetId()));
         }
      }
      ++listIter;
   }
}

//! Handle special cases of platforms around the date line and near the poles.
void UtQuadTreeGeneric::GetPlatformsAroundEdges(const UtRectangle& aBoundingBox,
                                                const double&      aRange,
                                                bool               aIsOverThePole,
                                                UtScriptDataList&  aScriptDataList)
{
   UtRectangle subRectangle(aBoundingBox);
   if (aIsOverThePole)
   {
      if (subRectangle.GetLeast(0) > subRectangle.GetGreatest(0))
      {
         subRectangle.SetLeast(0, aBoundingBox.GetGreatest(0));
         subRectangle.SetGreatest(0, aBoundingBox.GetLeast(0));
      }
      if (subRectangle.GetLeast(0) > 0.0) // North hemisphere
      {
         // Search sub-boxes around the pole.
         subRectangle.SetGreatest(0, 90.0);
         subRectangle.SetLeast(1, -180.0);
         subRectangle.SetGreatest(1, 180.0);
         GetPlatformsInRangeP(subRectangle, aRange, aScriptDataList, false);
      }
      else // south pole
      {
         // Search sub-boxes around the pole.
         subRectangle.SetLeast(0, -90.0);
         subRectangle.SetLeast(1, -180.0);
         subRectangle.SetGreatest(1, 180.0);
         GetPlatformsInRangeP(subRectangle, aRange, aScriptDataList, false);
      }
   }
   else
   {
      // Search sub-boxes around the edges.
      subRectangle.SetGreatest(1, 180.0);
      GetPlatformsInRangeP(subRectangle, aRange, aScriptDataList, false);
      subRectangle.SetGreatest(1, aBoundingBox.GetGreatest(1));
      subRectangle.SetLeast(1, -180.0);
      GetPlatformsInRangeP(subRectangle, aRange, aScriptDataList, false);
   }
}

bool UtQuadTreeGeneric::Add(double aLatitude, double aLongitude, int aId)
{
   mTreePtr->Insert(new UtLatLonDatum((unsigned int)aId, aLatitude, aLongitude));
   return true;
}

bool UtQuadTreeGeneric::Remove(int aId)
{
   return mTreePtr->Remove(aId);
}
