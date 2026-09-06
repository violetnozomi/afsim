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

#include "WsfNavigationCell.hpp"

// =================================================================================================
WsfNavigationCell::WsfNavigationCell()
   : mVerts()
   , mSides()
   , mNeighbors()
   , mVertexIterator()
   , mSideIterator()
   , mNeighborIterator()
   , mCellNormal()
   , mCellCentroid()
   , mCellID(0)
   , mCellNumber(0)
   , mX(0)
   , mY(0)
   , mTessillated(false)
   , mTessilationLevel(0)
   , mWeight(0.0)
   , mModifierList()
   , mNeighborIDs()
   , mNoZoneInteraction(false)
{
}

// =================================================================================================
WsfNavigationCell::WsfNavigationCell(float aX, float aY, float aZ, bool aMirror)
   : mVerts()
   , mSides()
   , mNeighbors()
   , mVertexIterator()
   , mSideIterator()
   , mNeighborIterator()
   , mCellNormal()
   , mCellCentroid()
   , mCellID(0)
   , mCellNumber(0)
   , mX(0)
   , mY(0)
   , mTessillated(false)
   , mTessilationLevel(0)
   , mWeight(0.0)
   , mModifierList()
   , mNeighborIDs()
   , mNoZoneInteraction(false)
{
}

// =================================================================================================
void WsfNavigationCell::AddVertex(const WsfGeoPoint& aPoint)
{
   // push this vert
   mVerts.push_back(aPoint);
   // if there are at least 2 points we can create sides for this poly
   if (mVerts.size() > 1)
   {
      mSides.clear();
      for (unsigned int i = 0; i < mVerts.size() - 1; ++i)
      {
         double pointA[3];
         double pointB[3];

         WsfGeoPoint geoA = mVerts[i];
         WsfGeoPoint geoB = mVerts[i + 1];

         geoA.GetLocationLLA(pointA[0], pointA[1], pointA[2]);
         geoB.GetLocationLLA(pointB[0], pointB[1], pointB[2]);

         UtLineSegment pointAtoB;
         pointAtoB.SetBeginPoint(pointA);
         pointAtoB.SetEndPoint(pointB);
         pointAtoB.SetNormal(); // set the direction vector of the line as well as set the normalized version as well.
         mSides.push_back(pointAtoB);
      }
      // close off the poly by connecting the last vert to the first
      double pointA[3];
      double pointB[3];

      WsfGeoPoint geoA = mVerts[mVerts.size() - 1];
      WsfGeoPoint geoB = mVerts[0];

      geoA.GetLocationLLA(pointA[0], pointA[1], pointA[2]);
      geoB.GetLocationLLA(pointB[0], pointB[1], pointB[2]);

      UtLineSegment pointAtoB;
      pointAtoB.SetBeginPoint(pointA);
      pointAtoB.SetEndPoint(pointB);
      pointAtoB.SetNormal();
      mSides.push_back(pointAtoB);

      // now compute the normals for each point on the poly
      std::vector<UtVec3d> normals;
      for (unsigned int i = 0; i < mVerts.size(); ++i)
      {
         double      newPointA[3];
         double      newPointB[3];
         double      newPointC[3];
         WsfGeoPoint newGeoA;
         WsfGeoPoint newGeoB;
         WsfGeoPoint newGeoC;
         if (i == 0)
         {
            // we know that the first point is part of the line segment between first and second point and first and last point

            newGeoA = mVerts[i];
            newGeoB = mVerts[i + 1];
            newGeoC = mVerts[mVerts.size() - 1];
         }
         else if (i == mVerts.size() - 1)
         {
            // we know that the last point is part of the line segment between first and last point and last and it's
            // previous point
            newGeoA = mVerts[i];
            newGeoB = mVerts[i - 1];
            newGeoC = mVerts[0];
         }
         else
         {
            newGeoA = mVerts[i];
            newGeoB = mVerts[i - 1];
            newGeoC = mVerts[i + 1];
         }

         newGeoA.GetLocationLLA(newPointA[0], newPointA[1], newPointA[2]);
         newGeoB.GetLocationLLA(newPointB[0], newPointB[1], newPointB[2]);
         newGeoC.GetLocationLLA(newPointC[0], newPointC[1], newPointC[2]);

         double normal[3];
         double AtoB[3];
         double AtoC[3];
         UtVec3d::Subtract(AtoB, newPointB, newPointA);
         UtVec3d::Subtract(AtoC, newPointC, newPointA);
         UtVec3d::CrossProduct(normal, AtoB, AtoC);
         UtVec3d::Normalize(normal);

         normals.push_back(normal);
      }
      // compute the normal for the cell
      double                         finalNormal[3] = {0, 0, 0};
      std::vector<UtVec3d>::iterator normalItr      = normals.begin();
      while (normalItr != normals.end())
      {
         UtVec3d normal = *normalItr;
         double  theNormal[3];
         normal.Get(theNormal);
         finalNormal[0] += theNormal[0];
         finalNormal[1] += theNormal[1];
         finalNormal[2] += theNormal[2];
         ++normalItr;
      }
      UtVec3d::Normalize(finalNormal);
      mCellNormal.Set(finalNormal); // this is also the surface normal at the centroid of the cell

      if (mCellNormal[2] < 0.0)
      {
         mCellNormal[0] *= -1;
      }
      // now compute the centroid
      double centroidSum[3] = {0, 0, 0};
      mVertexIterator       = mVerts.begin();
      while (mVertexIterator != mVerts.end())
      {
         WsfGeoPoint vert = *mVertexIterator;
         double      vertLLA[3];
         vert.GetLocationLLA(vertLLA[0], vertLLA[1], vertLLA[2]);
         centroidSum[0] += vertLLA[0];
         centroidSum[1] += vertLLA[1];
         centroidSum[2] += vertLLA[2];
         ++mVertexIterator;
      }
      for (size_t i = 0; i < 3; ++i)
      {
         centroidSum[i] /= mVerts.size();
      }
      mCellCentroid.Set(centroidSum);
   }
}

// =================================================================================================
// return true if the given point lies within this cell
bool WsfNavigationCell::PointInCell(double aX, double aY, double aZ)
{
   unsigned int i;
   double       angle = 0;
   UtVec3d      thePoint(aX, aY, aZ);
   // a quick check for triangles
   if (mSides.size() == 3)
   {
      if (PointOnSameSide(thePoint, 0) && PointOnSameSide(thePoint, 1) && PointOnSameSide(thePoint, 2))
      {
         return true;
      }
   }

   for (i = 0; i < mVerts.size(); ++i)
   {
      double pointOneX = mVerts[i].GetLat() - aX;
      double pointOneY = mVerts[i].GetLon() - aY;
      double pointTwoX = mVerts[(i + 1) % mVerts.size()].GetLat() - aX;
      double pointTwoY = mVerts[(i + 1) % mVerts.size()].GetLon() - aY;
      angle += AngleBetween(pointOneX, pointOneY, pointTwoX, pointTwoY);
   }

   if (fabs(angle) < UtMath::cPI)
   {
      return false;
   }
   else
   {
      return true;
   }
}

// =================================================================================================
double WsfNavigationCell::AngleBetween(double aFirstX, double aFirstY, double aSecondX, double aSecondY)
{
   double dtheta, theta1, theta2;

   theta1 = atan2(aFirstY, aFirstX);
   theta2 = atan2(aSecondY, aSecondX);
   dtheta = theta2 - theta1;
   while (dtheta > UtMath::cPI)
   {
      dtheta -= (2 * UtMath::cPI);
   }
   while (dtheta < -UtMath::cPI)
   {
      dtheta += (2 * UtMath::cPI);
   }
   return (dtheta);
}

// =================================================================================================
bool WsfNavigationCell::PointOnSameSide(UtVec3d aPathEndPoint, int aSideIndex)
{
   double AtoB[3];
   double AtoP[3];
   double AtoP2[3];
   double knownInsidePoint[3];
   if (aSideIndex == 0)
   {
      mVerts[2].GetLocationLLA(knownInsidePoint[0], knownInsidePoint[1], knownInsidePoint[2]);
   }
   else if (aSideIndex == 1)
   {
      mVerts[0].GetLocationLLA(knownInsidePoint[0], knownInsidePoint[1], knownInsidePoint[2]);
   }
   else
   {
      mVerts[1].GetLocationLLA(knownInsidePoint[0], knownInsidePoint[1], knownInsidePoint[2]);
   }

   UtVec3d::Subtract(AtoB, mSides[aSideIndex].GetEndPtr(), mSides[aSideIndex].GetBeginPtr());
   UtVec3d::Subtract(AtoP, aPathEndPoint.GetData(), mSides[aSideIndex].GetBeginPtr());
   UtVec3d::Subtract(AtoP2, knownInsidePoint, mSides[aSideIndex].GetBeginPtr());

   UtVec3d crossProductOne;
   crossProductOne.CrossProduct(AtoB, AtoP);
   UtVec3d crossProductTwo;
   crossProductTwo.CrossProduct(AtoB, AtoP2);

   if (UtVec3d::DotProduct(crossProductOne, crossProductTwo) >= 0)
   {
      return true;
   }

   return false;
}

// =================================================================================================
WsfNavigationCell::PATH_RESULT WsfNavigationCell::ClassifyPathToCell(UtLineSegment&      aMotionPath,
                                                                     WsfNavigationCell** aNextCell,
                                                                     CELL_SIDE&          aCellSide,
                                                                     UtVec3d*            aIntersectionPoint,
                                                                     WsfNavigationCell*  aPrevCell)
{
   double endPoint[3];
   aMotionPath.GetEndPoint(endPoint);

   if (PointInCell(endPoint[0], endPoint[1], endPoint[2]))
   {
      return (ENDING_CELL);
   }

   unsigned int interiorCount = 0;

   // Check our MotionPath against each of the three cell walls
   for (unsigned int i = 0; i < mSides.size(); ++i)
   {
      // Classify the MotionPath endpoints as being either ON_LINE,
      // or to its LEFT_SIDE or RIGHT_SIDE.
      // Since our vertices are in clockwise order,
      // we know that points  to the right of each line are inside the cell.
      // Points to the left are outside.
      // We do this test using the ClassifyPoint function of Line2D

      // If the destination endpoint of the MotionPath
      // is Not on the right side of this wall...
      if (ClassifyPoint(mSides[i], aMotionPath.GetEndPtr()) != WsfNavigationCell::RIGHT_SIDE)
      {
         // ..and the starting endpoint of the MotionPath
         // is Not on the left side of this wall...
         if (ClassifyPoint(mSides[i], aMotionPath.GetBeginPtr()) != WsfNavigationCell::LEFT_SIDE)
         {
            // Check to see if we intersect the wall
            // using the Intersection function of Line2D
            UtLineSegment::LINE_CLASSIFICATION IntersectResult = aMotionPath.Intersects(&mSides[i], aIntersectionPoint);
            // double intersectPt[3];
            // aIntersectionPoint->Get(intersectPt);
            if (IntersectResult == UtLineSegment::SEGMENTS_INTERSECT || IntersectResult == UtLineSegment::A_BISECTS_B)
            {
               // record the link to the next adjacent cell
               // (or 0 if no attachement exists)
               // and the enumerated ID of the side we hit.
               *aNextCell = GetNeighborForSide(i, aIntersectionPoint);
               if (*aNextCell == aPrevCell)
               {
                  continue;
               }
               aCellSide = (CELL_SIDE)i;
               return (EXITING_CELL);
            }
         }
      }
      else
      {
         // The destination endpoint of the MotionPath is on the right side.
         // Increment our interiorCount so we'll know how many walls we were
         // to the right of.
         interiorCount++;
      }
   }

   // An InteriorCount of 3 means the destination endpoint of the MotionPath
   // was on the right side of all walls in the cell.
   // That means it is located within this triangle, and this is our ending cell.
   if (interiorCount == mSides.size())
   {
      return (ENDING_CELL);
   }

   // We only reach here is if the MotionPath does not intersect the cell at all.
   return (NO_RELATIONSHIP);
}

// =================================================================================================
WsfNavigationCell::POINT_CLASSIFICATION WsfNavigationCell::ClassifyPoint(UtLineSegment aSide, UtVec3d aPathEndPoint)
{
   POINT_CLASSIFICATION result = ON_LINE;
   double               AtoB[3];
   double               lineNormal[3] = {0, 0, 0};
   UtVec3d::Subtract(AtoB, aPathEndPoint.GetData(), aSide.GetBeginPtr());
   double distance = UtVec3d::DotProduct(AtoB, lineNormal);
   if (distance > 0.0)
   {
      result = RIGHT_SIDE;
   }
   else if (distance < 0.0)
   {
      result = LEFT_SIDE;
   }
   return result;
}

// =================================================================================================
WsfNavigationCell* WsfNavigationCell::GetNeighborForSide(int aSideIndex, UtVec3d* aIntersectionPoint)
{
   UtLineSegment mySide = mSides[aSideIndex];
   mNeighborIterator    = mNeighbors.begin();
   while (mNeighborIterator != mNeighbors.end())
   {
      WsfNavigationCell* myNeighbor = *mNeighborIterator;

      // loop through all the neighbor's sides to see if any equal the one we are interested in
      myNeighbor->mSideIterator = myNeighbor->mSides.begin();
      while (myNeighbor->mSideIterator != myNeighbor->mSides.end())
      {
         UtLineSegment myNeighborSide = *myNeighbor->mSideIterator;
         if (myNeighborSide == mySide)
         {
            return myNeighbor;
         }
         else if (mySide.IsCollinear(myNeighborSide) && myNeighborSide.PointOnLineSegment(aIntersectionPoint->GetData()))
         {
            return myNeighbor;
         }
         ++myNeighbor->mSideIterator;
      }
      ++mNeighborIterator;
   }
   return nullptr;
}

// =================================================================================================
std::string WsfNavigationCell::GetValueForCellModifier(const std::string& aModifier)
{
   return mModifierList[aModifier].GetString();
}
