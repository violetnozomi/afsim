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

#include "UtConvexHull.hpp"

#include <algorithm>

namespace
{
// Comparison functions for std::sort()
bool AscendingUtVec2d_X(const UtVec2d& aV1, const UtVec2d& aV2)
{
   return aV1[0] > aV2[0];
}
} // namespace

std::vector<UtVec2d> UtConvexHull::ConvexHull(std::vector<UtVec2d>& aPoints)
{
   // Need more than 3 points to cull any points from set to make a convex hull
   if (aPoints.size() <= 3)
   {
      return aPoints;
   }

   // Step one in partitioning the points is to sort the raw data
   std::sort(aPoints.begin(), aPoints.end(), AscendingUtVec2d_X);

   // Take the far left and far right points, remove them from the
   // sorted sequence and store them in special members
   const UtVec2d left = aPoints.front(); // UtVec2d with leftmost x field
   aPoints.erase(aPoints.begin());
   const UtVec2d right = aPoints.back(); // UtVec2d with rightmost x field
   aPoints.pop_back();

   // Now put the remaining points in one of the two output sequences:
   // those above the line from left->right, and those below the line
   std::vector<UtVec2d> upperPartitionPoints;
   std::vector<UtVec2d> lowerPartitionPoints;
   for (size_t i = 0; i < aPoints.size(); ++i)
   {
      if (UtConvexHull::Direction(left, right, aPoints[i]) < 0)
      {
         upperPartitionPoints.push_back(aPoints[i]);
      }
      else
      {
         lowerPartitionPoints.push_back(aPoints[i]);
      }
   }

   // Create the hull, build its two halves separately
   std::vector<UtVec2d> lowerHull;
   UtConvexHull::BuildHalfHull(lowerPartitionPoints, lowerHull, left, right, 1.0);
   std::vector<UtVec2d> upperHull;
   UtConvexHull::BuildHalfHull(upperPartitionPoints, upperHull, left, right, -1.0);

   // Join the two halves by adding points from upperHull into lowerHull and return it.
   // Don't include the first or last point from 'upper', as they match the last & first points of 'lower' respectively.
   // upperHull has to at least have 2 points in it: left & right.
   for (std::vector<UtVec2d>::reverse_iterator ii = upperHull.rbegin() + 1; (ii + 1) != upperHull.rend(); ++ii)
   {
      lowerHull.push_back(*ii);
   }

   // lowerHull now holds the complete hull
   return lowerHull;
}

// Basically, this is a 2d cross product to find if p2 is to the left or right of line p0-p1.
double UtConvexHull::Direction(const UtVec2d aP0, const UtVec2d aP1, const UtVec2d aP2)
{
   return (((aP0[0] - aP1[0]) * (aP2[1] - aP1[1])) - ((aP2[0] - aP1[0]) * (aP0[1] - aP1[1])));
}

void UtConvexHull::BuildHalfHull(std::vector<UtVec2d>  aInput,
                                 std::vector<UtVec2d>& aOutput,
                                 const UtVec2d         aLeft,
                                 const UtVec2d         aRight,
                                 const double          aFactor)
{
   // The hull always starts with the left point, and ends with the
   // right point. Init input and output accordingly
   aInput.push_back(aRight);
   aOutput.push_back(aLeft);
   size_t inputIndex = 0;
   while (aInput.size() > inputIndex)
   {
      // Keep adding the leftmost point to the hull, and test if a convexity
      // violation has occurred. If so, fix it by removing the second-to-last point
      // in the output sequence until convexity is true
      aOutput.push_back(aInput[inputIndex++]);
      while (aOutput.size() >= 3)
      {
         const size_t end = aOutput.size() - 1;
         if ((aFactor * UtConvexHull::Direction(aOutput[end - 2], aOutput[end], aOutput[end - 1])) <= 0)
         {
            aOutput.erase(aOutput.begin() + end - 1);
         }
         else
         {
            break;
         }
      }
   }
}
