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

#ifndef WSFNAVIGATIONCELL_HPP
#define WSFNAVIGATIONCELL_HPP

#include "wsf_export.h"

#include <limits>

#include "UtLineSegment.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPathFinder.hpp"

class WSF_EXPORT WsfNavigationCell
{
public:
   enum CELL_VERT
   {
      VERT_A = 0,
      VERT_B,
      VERT_C
   };

   enum CELL_SIDE
   {
      SIDE_AB = 0,
      SIDE_BC,
      SIDE_CA
   };

   enum PATH_RESULT
   {
      NO_RELATIONSHIP, // the path does not cross this cell
      ENDING_CELL,     // the path ends in this cell
      EXITING_CELL     // the path crosses through this cell
   };

   enum POINT_CLASSIFICATION
   {
      ON_LINE,
      RIGHT_SIDE,
      LEFT_SIDE
   };

   WsfNavigationCell();
   WsfNavigationCell(float aX, float aY, float aZ, bool aMirror);

   void SetWeight(double aWeight) { mWeight = aWeight; }

   void AddVertex(const WsfGeoPoint& aPoint);

   bool PointInCell(double aX, double aY,
                    double aZ); // return true if the given point lies within this cell

   double AngleBetween(double aFirstX, double aFirstY, double aSecondX, double aSecondY);

   PATH_RESULT ClassifyPathToCell(UtLineSegment&      aMotionPath,
                                  WsfNavigationCell** aNextCell,
                                  CELL_SIDE&          aCellSide,
                                  UtVec3d*            aIntersectionPoint,
                                  WsfNavigationCell*  aPrevCell);

   POINT_CLASSIFICATION ClassifyPoint(UtLineSegment aSide, UtVec3d aPathEndPoint);

   bool PointOnSameSide(UtVec3d aPathEndPoint, int aSideIndex);

   WsfNavigationCell* GetNeighborForSide(int aSideIndex, UtVec3d* aIntersectionPoint);

   std::string GetValueForCellModifier(const std::string& aModifier);


   std::vector<WsfGeoPoint>        mVerts;     // the vertexes that make up this cell
   std::vector<UtLineSegment>      mSides;     // the sides that make up this cell
   std::vector<WsfNavigationCell*> mNeighbors; // pointers to this cell's neighbors

   // iterators for all the vectors
   std::vector<WsfGeoPoint>::iterator        mVertexIterator;
   std::vector<UtLineSegment>::iterator      mSideIterator;
   std::vector<WsfNavigationCell*>::iterator mNeighborIterator;

   UtVec3d                            mCellNormal;
   UtVec3d                            mCellCentroid;
   unsigned int                       mCellID;
   unsigned int                       mCellNumber; // essentially the same as ID, but pathfinder doesnt use this value
   unsigned int                       mX;
   unsigned int                       mY;
   bool                               mTessillated;
   int                                mTessilationLevel;
   double                             mWeight; // pathing weight
   std::map<WsfStringId, WsfStringId> mModifierList;
   std::vector<unsigned int>          mNeighborIDs;

   bool mNoZoneInteraction; // a flag used while tessilating
};
#endif
