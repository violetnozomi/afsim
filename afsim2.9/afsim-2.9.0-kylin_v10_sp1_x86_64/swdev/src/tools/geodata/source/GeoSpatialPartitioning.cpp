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

#include "GeoSpatialPartitioning.hpp"

#include "UtMemory.hpp"

std::pair<double, double> Geodata::SP_Edge::CalculateRange(std::list<SP_Edge>& aEdgeList) const
{
   std::pair<double, double> range{std::numeric_limits<double>::min(), std::numeric_limits<double>::max()};

   for (auto edge : aEdgeList)
   {
      if (edge.mX == mX)
      {
         if (edge.mGreaterThan)
         {
            range.first = edge.mValue;
         }
         else
         {
            range.second = edge.mValue;
         }
      }
   }
   return range;
}

void Geodata::SP_Edge::CullIn(std::list<SP_Edge>& aEdgeList) const
{
   for (auto edge = aEdgeList.begin(); edge != aEdgeList.end();)
   {
      if (CullIn(*edge)) // is of the same dimension
      {
         edge = aEdgeList.erase(edge);
      }
      else
      {
         ++edge;
      }
   }
}

void Geodata::SP_Edge::CullOut(std::list<SP_Edge>& aEdgeList) const
{
   for (auto edge = aEdgeList.begin(); edge != aEdgeList.end();)
   {
      if (CullOut(*edge))
      {
         edge = aEdgeList.erase(edge);
      }
      else
      {
         ++edge;
      }
   }
}

// can the edge be culled, if we are looking at the inside
bool Geodata::SP_Edge::CullIn(const SP_Edge& aEdge) const
{
   if (aEdge.mX != mX) // if not dimensionally aligned
   {
      return false;
   }
   else if ((aEdge.mValue > mValue) && mGreaterThan)
   {
      return false;
   }
   else if ((aEdge.mValue < mValue) && !mGreaterThan)
   {
      return false;
   }
   return true;
}

// can the edge be culled, if we are looking at the outside
bool Geodata::SP_Edge::CullOut(const SP_Edge& aEdge) const
{
   if (aEdge.mX != mX) // if not dimensionally aligned
   {
      return false;
   }
   else if ((aEdge.mValue > mValue) && !mGreaterThan)
   {
      return false;
   }
   else if ((aEdge.mValue < mValue) && mGreaterThan)
   {
      return false;
   }
   return true;
}

bool Geodata::SP_Edge::IsIn(const std::pair<double, double>& aRange) const
{
   return (((mGreaterThan) && (aRange.second > mValue)) || ((!mGreaterThan) && (aRange.first < mValue)));
}

bool Geodata::SP_Edge::IsOut(const std::pair<double, double>& aRange) const
{
   return (((mGreaterThan) && (aRange.first < mValue)) || ((!mGreaterThan) && (aRange.second > mValue)));
}

bool Geodata::SP_Edge::PointIsIn(double aX, double aY) const
{
   if (mX)
   {
      if (mGreaterThan)
      {
         return aX >= mValue;
      }
      else
      {
         return aX < mValue;
      }
   }
   else
   {
      if (mGreaterThan)
      {
         return aY >= mValue;
      }
      else
      {
         return aY < mValue;
      }
   }
}

Geodata::SimpleSP_TreeNode::SimpleSP_TreeNode(size_t aInIndex, size_t aOutIndex, std::list<SP_Edge>& aEdgeList)
{
   mEdge = aEdgeList.back();
   aEdgeList.pop_back();
   mOut.first = aOutIndex;
   mIn.first  = aInIndex;
   if (!aEdgeList.empty())
   {
      mIn.second = ut::make_unique<SimpleSP_TreeNode>(aInIndex, aOutIndex, aEdgeList);
   }
}

void Geodata::SimpleSP_TreeNode::InsertZone(size_t aIndex, std::list<SP_Edge>& aEdgeList)
{
   auto range = mEdge.CalculateRange(aEdgeList);
   if (mEdge.IsIn(range)) // the edge list is (at least partially) inside this partition
   {
      std::list<SP_Edge> el = aEdgeList;
      mEdge.CullIn(el);
      if (mIn.second) // there is other stuff inside the partition
      {
         mIn.second->InsertZone(aIndex, el); // go deeper
      }
      else
      {
         if (!el.empty())
         {
            mIn.second =
               ut::make_unique<SimpleSP_TreeNode>(aIndex, mIn.first, el); // there is nothing else inside this
                                                                          // partition, create our own partitions inside
         }
         if (mIn.first == std::numeric_limits<size_t>::max())
         {
            mIn.first = aIndex;
         }
      }
   }
   if (mEdge.IsOut(range)) // the edge list is (at least partially) outside this partition
   {
      std::list<SP_Edge> el = aEdgeList;
      mEdge.CullOut(el);
      if (mOut.second) // there is other stuff outside the partition
      {
         mOut.second->InsertZone(aIndex, el); // go deeper
      }
      else
      {
         if (!el.empty())
         {
            mOut.second =
               ut::make_unique<SimpleSP_TreeNode>(aIndex, mOut.first, el); // there is nothing else outside this partition,
                                                                           // create our own partitions outside
         }
         if (mOut.first == std::numeric_limits<size_t>::max())
         {
            mOut.first = aIndex;
         }
      }
   }
}

size_t Geodata::SimpleSP_TreeNode::LookUpIndex(double aX, double aY) const
{
   if (mEdge.PointIsIn(aX, aY))
   {
      if (mIn.second)
      {
         return mIn.second->LookUpIndex(aX, aY);
      }
      else
      {
         return mIn.first;
      }
   }
   else
   {
      if (mOut.second)
      {
         return mOut.second->LookUpIndex(aX, aY);
      }
      else
      {
         return mOut.first;
      }
   }
}
