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

#ifndef CONTOURFILTER2D_HPP
#define CONTOURFILTER2D_HPP

#include <cstddef> // for size_t
#include <list>
#include <vector>

//! Trace a contour line through a 2-D structured grid.

class ContourFilter2D
{
public:
   //! An abstract class that provides the interface for accessing the data.
   //! The caller must provide a concrete implementation for their data.
   class Function2D
   {
   public:
      Function2D()                  = default;
      Function2D(const Function2D&) = default;
      virtual ~Function2D() {}
      virtual double GetX(size_t aX_Index, size_t aY_Index)     = 0;
      virtual double GetY(size_t aX_Index, size_t aY_Index)     = 0;
      virtual double GetValue(size_t aX_Index, size_t aY_Index) = 0;
   };

   //! A 'Point' is one point on an continuous contour line.

   struct Point
   {
      double mX;
      double mY;
   };

   //! A 'Line' represents a continuous contour line.

   using Line = std::vector<Point>;

   //! LineList is just a list of 'Line's.

   using LineList     = std::list<Line>;
   using LineListIter = std::list<Line>::const_iterator;

   ContourFilter2D();

   virtual ~ContourFilter2D();

   void Contour(Function2D* aFunctionPtr,
                size_t      aBegX_Index,
                size_t      aEndX_Index,
                size_t      aBegY_Index,
                size_t      aEndY_Index,
                double      aValue,
                LineList&   aLineList);

private:
   ContourFilter2D(const ContourFilter2D& aSrc) = delete;            // not impl.
   ContourFilter2D& operator=(const ContourFilter2D& aRhs) = delete; // not impl.

   enum EdgeType
   {
      EtBottomEdge = 1,
      EtRightEdge  = 2,
      EtTopEdge    = 3,
      EtLeftEdge   = 4
   };

   struct PendingEdge
   {
      size_t mX_Index;
      size_t mY_Index;
   };

   void TraceContour(EdgeType aEdgeType, size_t aX_IndexAbove, size_t aY_IndexAbove, Line& aLine);

   Function2D* mFunctionPtr;
   size_t      mBegX_Index;
   size_t      mEndX_Index;
   size_t      mBegY_Index;
   size_t      mEndY_Index;
   double      mValue;

   std::vector<PendingEdge> mPendingEdges;
};

#endif
