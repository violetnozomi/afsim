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

#include "ContourFilter2D.hpp"

#include <cassert>
#include <iostream>

// This is a C++ implementation of the line contouring algorithm extracted
// from PLOT3D subroutine CON2L.  PLOT3D is a Computational Fluid Dynamics
// (CFD) visualization program from NASA Ames Research Center.
//
// The original code was in FORTRAN and had lots of 'spaghetti'. This version
// *SHOULD* be a little easier to understand (at the expense of more code).

// The algorithm operates on a subarea of a rectangular array of points (which
// may be the whole array).  The 'topology' of the array is as follows:
//
//         |                   |
//  jend --+----+----+----+----+--
//         |    |    |    |    |
//         |    |    |    |    |
//         +----+----+----+----+
//    ^    |    |    |    |    |
//    |    |    |    |    |    |
//         +----+----+----+----+
//    j    |    |    |    |    |
//         |    |    |    |    |
//         +----+----+----+----+
//         |    |    |    |    |
//         |    |    |    |    |
//  jbeg --+----+----+----+----+--
//         |                   |
//
//        ibeg      i ->      iend
//
// For our purposes, increasing 'i' corresponds to increasing X and
// increasing 'j' corresponds to increasing Y.
//
// A contour line must either:
//
// a) start on an edge of the subarea and exit through an edge of the subarea.
// b) start in the interior and close on itself.
//
// This is really just a statement of the fact that if a contour line enters a
// cell, it must also exit the cell or exit the subarea.
//
// The algorithm starts by first scanning the interior cell edges of lines
// of constant 'j' from bottom-to-top, left-to-right and marking those edges
// where the function value INCREASES through the contour level.  These edges
// are later processed to generate the contour lines that start in interior.
//
// The algorithm then continues by scanning the exterior edges of the subarea
// locating edges where the function value increases through the contour
// level.  Once an edge is found the resulting contour is traced through the
// subarea until it exits.  It should be noted that any edge marked by the
// first stage and traversed by this stage is 'unmarked'.
//
// It is important to note that the subarea edges are scanned in a specific
// order and direction as follows:
//
//   1) lower edge, left-to-right
//   2) right edge, bottom-to-top
//   3) upper edge, right-to-left
//   4) left  edge, top-to-bottom
//
// The reason for this is that the algorithm that traces the line through the
// subarea must know which on which edge the line enters a cell in order to
// determine the line though which it exits the cell.  If we label a cell:
//
//                      D     3     C
//                       +---------+
//                       |         |
//                       |         |
//                     4 |         | 2
//                       |         |
//                       |         |
//                       +---------+
//                      A     1     B
//
// where:
//
//   Side 1 is the directed segment from point A to point B
//   Side 2 is the directed segment from point B to point C
//   Side 3 is the directed segment from point C to point D
//   Side 4 is the directed segment from point D to point A
//
// then contour lines that enter on cells on the bottom edge of the subarea
// are entering through edge 1.  Similarly, those that enter on the right edge
// of the subarea are entering through side 2; on the top edge, side 3 and on
// the left edge, side 4.
//
// Once it is known on which side of a cell a line enters (i.e.: where the
// function value INCREASES through the contour level), the remaining sides
// are checked in a COUNTER-CLOCKWISE order looking for the side where the
// function value DECREASES through the contour level.  This is the side on
// which the contour line exits the cell and tells us which cell is to be
// considered next.
//
// A somewhat subtle point is that the side number of a cell through which a
// line exits is NOT the same side number in adjacent cell where the line will
// enter.  For instance, if a line EXITS a cell through edge 3 then it will
// ENTER the cell above through edge 1.

ContourFilter2D::ContourFilter2D()
   : mFunctionPtr(nullptr)
   , mBegX_Index(0)
   , mEndX_Index(0)
   , mBegY_Index(0)
   , mEndY_Index(0)
   , mValue(0.0)
   , mPendingEdges()
{
}

ContourFilter2D::~ContourFilter2D() {}

void ContourFilter2D::Contour(Function2D* aFunctionPtr,
                              size_t      aBegX_Index,
                              size_t      aEndX_Index,
                              size_t      aBegY_Index,
                              size_t      aEndY_Index,
                              double      aValue,
                              LineList&   aLineList)
{
   if ((aBegY_Index >= aEndY_Index) || (aBegX_Index >= aEndX_Index))
   {
      return;
   }

   // Transfer arguments to local storage...

   mFunctionPtr = aFunctionPtr;
   mBegY_Index  = aBegY_Index;
   mEndY_Index  = aEndY_Index;
   mBegX_Index  = aBegX_Index;
   mEndX_Index  = aEndX_Index;
   mValue       = aValue;

   // Locate cell edges in the interior where the function value increases
   // through the contour level.

   mPendingEdges.clear();

   size_t yIndex;
   size_t xIndex;
   bool   wasLess;

   for (yIndex = mBegY_Index + 1; yIndex <= mEndY_Index - 1; ++yIndex)
   {
      wasLess = false;
      for (xIndex = mBegX_Index; xIndex <= mEndX_Index; ++xIndex)
      {
         if (mFunctionPtr->GetValue(xIndex, yIndex) <= mValue)
         {
            wasLess = true;
         }
         else if (wasLess)
         {
            wasLess = false;
            PendingEdge pendingEdge;
            pendingEdge.mY_Index = yIndex;
            pendingEdge.mX_Index = xIndex;
            mPendingEdges.push_back(pendingEdge);
         }
      }
   }

   // Scan the edges looking for where the function value increases through
   // the contour level.

   // Process the bottom edge; left-to-right.

   wasLess = false;
   yIndex  = mBegY_Index;
   for (xIndex = mBegX_Index; xIndex <= mEndX_Index; ++xIndex)
   {
      if (mFunctionPtr->GetValue(xIndex, yIndex) <= mValue)
      {
         wasLess = true;
      }
      else if (wasLess)
      {
         wasLess = false;
         aLineList.emplace_back();
         TraceContour(EtBottomEdge, xIndex, yIndex, aLineList.back());
      }
   }

   // Process the right edge; bottom-to-top.

   wasLess = false;
   xIndex  = mEndX_Index;
   for (yIndex = mBegY_Index; yIndex <= mEndY_Index; ++yIndex)
   {
      if (mFunctionPtr->GetValue(xIndex, yIndex) <= mValue)
      {
         wasLess = true;
      }
      else if (wasLess)
      {
         wasLess = false;
         aLineList.emplace_back();
         TraceContour(EtRightEdge, xIndex, yIndex, aLineList.back());
      }
   }

   // Process the top edge; right-to-left.

   wasLess = false;
   yIndex  = mEndY_Index;
   for (xIndex = mEndX_Index; xIndex >= mBegX_Index; --xIndex)
   {
      if (mFunctionPtr->GetValue(xIndex, yIndex) <= mValue)
      {
         wasLess = true;
      }
      else if (wasLess)
      {
         wasLess = false;
         aLineList.emplace_back();
         TraceContour(EtTopEdge, xIndex, yIndex, aLineList.back());
      }
      // Because we are using unsigned values for the indices, we must be
      // careful to avoid decrementing through zero and wrapping around.
      if (xIndex == 0)
      {
         break;
      }
   }

   // Process the left edge; top-to-bottom.

   wasLess = false;
   xIndex  = mBegX_Index;
   for (yIndex = mEndY_Index; yIndex >= mBegY_Index; --yIndex)
   {
      if (mFunctionPtr->GetValue(xIndex, yIndex) <= mValue)
      {
         wasLess = true;
      }
      else if (wasLess)
      {
         wasLess = false;
         aLineList.emplace_back();
         TraceContour(EtLeftEdge, xIndex, yIndex, aLineList.back());
      }
      // Because we are using unsigned values for the indices, we must be
      // careful to avoid through zero and wrapping around.
      if (yIndex == 0)
      {
         break;
      }
   }

   // Process any remaining unprocessed interior contours.

   for (auto& pendingEdge : mPendingEdges)
   {
      yIndex = pendingEdge.mY_Index;
      xIndex = pendingEdge.mX_Index;
      if ((yIndex <= mEndY_Index) && (xIndex <= mEndX_Index))
      {
         pendingEdge.mY_Index = mEndY_Index + 1; // mark it...
         pendingEdge.mX_Index = mEndX_Index + 1; // ...as processed
         aLineList.emplace_back();
         TraceContour(EtBottomEdge, xIndex, yIndex, aLineList.back());
      }
   }
}

// Trace a contour line until it exits the area or closes on itself.

// private
void ContourFilter2D::TraceContour(EdgeType aEdgeType, size_t aX_IndexAbove, size_t aY_IndexAbove, Line& aLine)
{
   EdgeType edgeType    = aEdgeType;
   size_t   xIndexAbove = aX_IndexAbove;
   size_t   yIndexAbove = aY_IndexAbove;
   double   xBelow;
   double   yBelow;
   double   valueBelow;
   double   firstX      = 0.0;
   double   firstY      = 0.0;
   bool     done        = false;
   size_t   onGridPoint = 0;
   Point    newPoint;

   while (!done)
   {
      // Determine the location of the entry point and the next cell/side
      // to be processed.

      assert((xIndexAbove >= mBegX_Index) && (xIndexAbove <= mEndX_Index));
      assert((yIndexAbove >= mBegY_Index) && (yIndexAbove <= mEndY_Index));

      double valueAbove = mFunctionPtr->GetValue(xIndexAbove, yIndexAbove);
      assert(valueAbove > mValue);
      double xAbove = mFunctionPtr->GetX(xIndexAbove, yIndexAbove);
      double yAbove = mFunctionPtr->GetY(xIndexAbove, yIndexAbove);

      if (edgeType == EtBottomEdge)
      {
         // Remove this entry from the list of interior crossings to be
         // processed.  This is only done on the bottom edge...

         for (auto& pendingEdge : mPendingEdges)
         {
            if ((xIndexAbove == pendingEdge.mX_Index) && (yIndexAbove == pendingEdge.mY_Index))
            {
               // Mark this entry as processed...
               pendingEdge.mX_Index = mEndX_Index + 1;
               pendingEdge.mY_Index = mEndY_Index + 1;
               break;
            }
         }

         valueBelow = mFunctionPtr->GetValue(xIndexAbove - 1, yIndexAbove);
         assert(valueBelow <= mValue);
         xBelow = mFunctionPtr->GetX(xIndexAbove - 1, yIndexAbove);
         yBelow = yAbove;

         // Check right, then top, then left

         if (yIndexAbove == mEndY_Index)
         {
            done = true;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove, yIndexAbove + 1) <= mValue)
         {
            edgeType = EtLeftEdge;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove - 1, yIndexAbove + 1) <= mValue)
         {
            edgeType = EtBottomEdge;
            ++yIndexAbove;
         }
         else
         {
            edgeType = EtRightEdge;
            --xIndexAbove;
            ++yIndexAbove;
         }
         assert((xIndexAbove >= mBegX_Index) && (xIndexAbove <= mEndX_Index));
         assert((yIndexAbove >= mBegY_Index) && (yIndexAbove <= mEndY_Index));
      }
      else if (edgeType == EtRightEdge)
      {
         valueBelow = mFunctionPtr->GetValue(xIndexAbove, yIndexAbove - 1);
         assert(valueBelow <= mValue);
         xBelow = xAbove;
         yBelow = mFunctionPtr->GetY(xIndexAbove, yIndexAbove - 1);

         // Check top, then left, then bottom

         if (xIndexAbove == mBegX_Index)
         {
            done = true;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove - 1, yIndexAbove) <= mValue)
         {
            edgeType = EtBottomEdge;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove - 1, yIndexAbove - 1) <= mValue)
         {
            edgeType = EtRightEdge;
            --xIndexAbove;
         }
         else
         {
            edgeType = EtTopEdge;
            --xIndexAbove;
            --yIndexAbove;
         }
         assert((xIndexAbove >= mBegX_Index) && (xIndexAbove <= mEndX_Index));
         assert((yIndexAbove >= mBegY_Index) && (yIndexAbove <= mEndY_Index));
      }
      else if (edgeType == EtTopEdge)
      {
         valueBelow = mFunctionPtr->GetValue(xIndexAbove + 1, yIndexAbove);
         assert(valueBelow <= mValue);
         xBelow = mFunctionPtr->GetX(xIndexAbove + 1, yIndexAbove);
         yBelow = yAbove;

         // Check left, then bottom, then right

         if (yIndexAbove == mBegY_Index)
         {
            done = true;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove, yIndexAbove - 1) <= mValue)
         {
            edgeType = EtRightEdge;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove + 1, yIndexAbove - 1) <= mValue)
         {
            edgeType = EtTopEdge;
            --yIndexAbove;
         }
         else
         {
            edgeType = EtLeftEdge;
            ++xIndexAbove;
            --yIndexAbove;
         }
         assert((xIndexAbove >= mBegX_Index) && (xIndexAbove <= mEndX_Index));
         assert((yIndexAbove >= mBegY_Index) && (yIndexAbove <= mEndY_Index));
      }
      else
      {
         assert(edgeType == EtLeftEdge);

         valueBelow = mFunctionPtr->GetValue(xIndexAbove, yIndexAbove + 1);
         assert(valueBelow <= mValue);
         xBelow = xAbove;
         yBelow = mFunctionPtr->GetY(xIndexAbove, yIndexAbove + 1);

         // Check bottom, then right, then top

         if (xIndexAbove == mEndX_Index)
         {
            done = true;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove + 1, yIndexAbove) <= mValue)
         {
            edgeType = EtTopEdge;
         }
         else if (mFunctionPtr->GetValue(xIndexAbove + 1, yIndexAbove + 1) <= mValue)
         {
            edgeType = EtLeftEdge;
            ++xIndexAbove;
         }
         else
         {
            edgeType = EtBottomEdge;
            ++xIndexAbove;
            ++yIndexAbove;
         }
         assert((xIndexAbove >= mBegX_Index) && (xIndexAbove <= mEndX_Index));
         assert((yIndexAbove >= mBegY_Index) && (yIndexAbove <= mEndY_Index));
      }

      // Interpolate to get the location of the entry point

      double t = (mValue - valueBelow) / (valueAbove - valueBelow);
      double x = xBelow + t * (xAbove - xBelow);
      double y = yBelow + t * (yAbove - yBelow);

      // This tests for a contour point coinciding with a grid point.  In this
      // case the contour routine comes up with the same physical point twice.
      // If we don't trap it, it can (in some cases significantly) increase
      // the number of points in a contour line.  Also, if this happens on the
      // first point in a line, the second point could be misinterpreted as
      // the end of a (circling) contour line.

      if (t == 0.0F)
      {
         ++onGridPoint;
      }

      newPoint.mX = x;
      newPoint.mY = y;
      if (aLine.empty())
      {
         // First point in the contour line.

         firstX = x;
         firstY = y;
         aLine.push_back(newPoint);
      }
      else if (onGridPoint != 2)
      {
         aLine.push_back(newPoint);
         if ((x == firstX) && (y == firstY))
         {
            done = true;
         }
      }
      else
      {
         size_t lastPoint = aLine.size() - 1;
         if ((x == aLine[lastPoint].mX) && (y == aLine[lastPoint].mY))
         {
            // ignore this point
            onGridPoint = 0;
         }
         else
         {
            onGridPoint = 1;
            aLine.push_back(newPoint);
            if ((x == firstX) && (y == firstY))
            {
               done = true;
            }
         }
      }
   }
}
