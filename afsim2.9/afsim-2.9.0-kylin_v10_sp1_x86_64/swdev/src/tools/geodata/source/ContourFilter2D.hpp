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

#ifndef CONTOURFILTER2D_HPP
#define CONTOURFILTER2D_HPP

#include "geodata_export.h"

#include <list>
#include <vector>

//! Trace a contour line through a 2-D structured grid.

class GEODATA_EXPORT ContourFilter2D
{
public:
   //! An abstract class that provides the interface for accessing the data.
   //! The caller must provide a concrete implementation for their data.
   class GEODATA_EXPORT Function2D
   {
   public:
      Function2D()                  = default;
      Function2D(const Function2D&) = default;
      virtual ~Function2D() {}
      virtual double GetX(unsigned int aX_Index, unsigned int aY_Index)     = 0;
      virtual double GetY(unsigned int aX_Index, unsigned int aY_Index)     = 0;
      virtual double GetValue(unsigned int aX_Index, unsigned int aY_Index) = 0;
   };

   //! A 'Point' is one point on an continuous contour line.

   struct GEODATA_EXPORT Point
   {
      double mX;
      double mY;
   };

   //! A 'Line' represents a continuous contour line.

   typedef std::vector<Point> Line;

   //! LineList is just a list of 'Line's.

   typedef std::list<Line>                 LineList;
   typedef std::list<Line>::const_iterator LineListIter;

   ContourFilter2D();

   virtual ~ContourFilter2D();

   void Contour(Function2D*  aFunctionPtr,
                unsigned int aBegX_Index,
                unsigned int aEndX_Index,
                unsigned int aBegY_Index,
                unsigned int aEndY_Index,
                double       aValue,
                LineList&    aLineList);

private:
   ContourFilter2D(const ContourFilter2D& aSrc);            // not impl.
   ContourFilter2D& operator=(const ContourFilter2D& aRhs); // not impl.

   enum EdgeType
   {
      EtBottomEdge = 1,
      EtRightEdge  = 2,
      EtTopEdge    = 3,
      EtLeftEdge   = 4
   };

   struct PendingEdge
   {
      unsigned int mX_Index;
      unsigned int mY_Index;
   };

   void TraceContour(EdgeType aEdgeType, unsigned int aX_IndexAbove, unsigned int aY_IndexAbove, Line& aLine);

   Function2D*  mFunctionPtr;
   unsigned int mBegX_Index;
   unsigned int mEndX_Index;
   unsigned int mBegY_Index;
   unsigned int mEndY_Index;
   double       mValue;

   std::vector<PendingEdge> mPendingEdges;
};

#endif
