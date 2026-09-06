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

#ifndef UTQUADTREE_HPP
#define UTQUADTREE_HPP

#include "util_script_export.h"

#include <string>
#include <vector>

class UtEntity;
#include "UtLatLonDatum.hpp"
class UtScriptClass;
#include "UtScriptAccessible.hpp"
#include "UtScriptData.hpp"
#include "UtSpatialTree.hpp"

//--------------------------------------------------------------
//! UtQuadTreeGeneric provides a script interface to a QuadTree.
//--------------------------------------------------------------
class UTIL_SCRIPT_EXPORT UtQuadTreeGeneric
{
public:
   UtQuadTreeGeneric();
   ~UtQuadTreeGeneric();

   typedef std::list<int> ObjectIndexList;

   bool Add(double aLatitude, double aLongitude, int aId);

   bool Remove(int aId);

   void InRange(double aLatitude, double aLongitude, double aRange, UtScriptDataList& aScriptDataList);

   const char* GetScriptClassName() const;

private:
   UtRectangle GetBoundingRectangle(double aLatitude, double aLongitude, double aRange) const;

   void GetPlatformsInRangeP(const UtRectangle& aBoundingBox,
                             const double&      aRange,
                             UtScriptDataList&  aScriptDataList,
                             bool               aIsFirstCall);

   void CheckBoundingBox(const UtRectangle& aBoundingBox,
                         const double&      aRange,
                         bool&              aIsOverThePole,
                         bool&              aIsOverTheDateLine) const;

   void GetPlatformsFromNode(const UtQuadNode&  aNode,
                             const UtRectangle* aBoundingBoxPtr,
                             UtScriptDataList&  aScriptDataList) const; // out

   void GetPlatformsAroundEdges(const UtRectangle& aBoundingBox,
                                const double&      aRange,
                                bool               aIsOverThePole,
                                UtScriptDataList&  aScriptDataList); // out

   bool                   mIsWholeEarth;
   std::list<UtQuadNode*> mSearchList;
   UtQuadTree*            mTreePtr;
};

UT_MAP_CLASS_TO_SCRIPT_NAME(UtQuadTreeGeneric, "QuadTree");

#endif
