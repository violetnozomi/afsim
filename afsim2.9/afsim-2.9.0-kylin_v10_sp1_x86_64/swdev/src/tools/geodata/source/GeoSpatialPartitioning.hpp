// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GEOSPATIALPARTITIONING_HPP
#define GEOSPATIALPARTITIONING_HPP

#include "geodata_export.h"

#include <limits>
#include <list>
#include <memory>
#include <utility>

namespace Geodata
{

// Defines a 2D orthogonal edge for space partitioning
class GEODATA_EXPORT SP_Edge
{
public:
   SP_Edge() = default;
   SP_Edge(bool aX, bool aGreaterThan, double aValue)
      : mX(aX)
      , mGreaterThan(aGreaterThan)
      , mValue(aValue)
   {
   }
   std::pair<double, double> CalculateRange(std::list<SP_Edge>& aEdgeList) const;
   void                      CullIn(std::list<SP_Edge>& aEdgeList) const;
   void                      CullOut(std::list<SP_Edge>& aEdgeList) const;
   bool                      CullIn(const SP_Edge& aEdge) const;
   bool                      CullOut(const SP_Edge& aEdge) const;
   // is this range on the inside of the partition?
   bool IsIn(const std::pair<double, double>& aEdgeList) const;
   // is this range on the outside of the partition?  Note that a range can cross the partition and be both inside and out
   bool IsOut(const std::pair<double, double>& aEdgeList) const;
   bool operator==(const SP_Edge& aRHS) const
   {
      return ((mX == aRHS.mX) && (mGreaterThan == aRHS.mGreaterThan) && (mValue == aRHS.mValue));
   }
   bool Matches(const SP_Edge& aRHS) const { return ((mX == aRHS.mX) && (mValue == aRHS.mValue)); }
   bool PointIsIn(double aX, double aY) const;

private:
   bool   mX{true};            // true - X; false - Y
   bool   mGreaterThan{false}; // true - greater than or equal to; false - less than
   double mValue{std::numeric_limits<double>::max()};
};

// This class encapsulates a space partition node for a 2D orthogonal space
// This was written as a simplification of a BSP tree, but might actually be a K-D tree (K=2)
class GEODATA_EXPORT SimpleSP_TreeNode
{
public:
   SimpleSP_TreeNode(size_t aInIndex, size_t aOutIndex, std::list<SP_Edge>& aEdgeList);
   SimpleSP_TreeNode& operator=(const SimpleSP_TreeNode&) = delete;

   void   InsertZone(size_t aIndex, std::list<SP_Edge>& aEdgeList);
   size_t LookUpIndex(double aX, double aY) const;

private:
   SP_Edge                                               mEdge;
   std::pair<size_t, std::unique_ptr<SimpleSP_TreeNode>> mIn;
   std::pair<size_t, std::unique_ptr<SimpleSP_TreeNode>> mOut;
};

} // namespace Geodata

#endif
