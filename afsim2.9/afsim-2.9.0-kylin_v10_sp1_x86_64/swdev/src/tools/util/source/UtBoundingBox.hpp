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

#ifndef UTBOUNDINGBOX_HPP
#define UTBOUNDINGBOX_HPP

#include "ut_export.h"

#include <iostream>

#include "UtLog.hpp"

/** A common interface for bounding boxes.
    This class cannot be instantiated.  Instead,
    use UtRectangle and UtRectP (Rectangular Parallelepiped)
    (typedefs of UtBoundingBoxI<Dim>).
*/
class UT_EXPORT UtBoundingBox
{
public:
   enum Axis
   {
      X = 0,
      Y = 1,
      Z = 2
   };

   virtual ~UtBoundingBox() = default;

   //! Return the centroid of the bounding box.
   const double* GetCentroid()
   {
      if (mComputeCentroid)
      {
         ComputeCentroid();
         mComputeCentroid = false;
      }
      return mCentroid;
   }

   //! Return the min. values of the bounding box.
   const double* GetLeast() const { return mLeast; }

   //! Return the max. values of the bounding box.
   const double* GetGreatest() const { return mGreatest; }

   //! return the indicated min. value of the bounding box.
   //!@note This method is made pure virtual to maintain compatibility with
   //!      the Sun/Solaris compiler.
   virtual const double& GetLeast(int aIndex) const = 0;

   //! return the indicated max. value of the bounding box.
   //!@note This method is made pure virtual to maintain compatibility with
   //!      the Sun/Solaris compiler.
   virtual const double& GetGreatest(int aIndex) const = 0;

   //! Set the indicated min. value.
   void SetLeast(int aIndex, const double& aValue)
   {
      mLeast[aIndex]   = aValue;
      mComputeCentroid = true;
   }

   //! Set the indicated max. value.
   void SetGreatest(int aIndex, const double& aValue)
   {
      mGreatest[aIndex] = aValue;
      mComputeCentroid  = true;
   }

   //! Return the indicated centroid value.
   const double& GetCentroid(int aIndex)
   {
      if (mComputeCentroid)
      {
         ComputeCentroid();
         mComputeCentroid = false;
      }
      return mCentroid[aIndex];
   }

   //! Return whether the given bounding box intersects this bounding box.
   virtual bool Intersects(const UtBoundingBox& aBBox) const = 0;

   //! Return whether the given line segment intersects this bounding box.
   virtual bool Intersects(const double* aEndPoint1, const double* aEndPoint2) const = 0;

   //! Return whether the object contains the given bounding box.
   virtual bool Contains(const UtBoundingBox& aBBox) const = 0;

   //! Return whether the extents of the box are well-formed.
   virtual bool ExtentsOK() const = 0;

   //! Print a diagnostic that shows the extent of the bounding box.
   virtual void PrintExtent(ut::log::MessageStream& aStream) const = 0;

protected:
   UtBoundingBox(double aLeast[], double aGreatest[], double aCentroid[])
      : mLeast(aLeast)
      , mGreatest(aGreatest)
      , mCentroid(aCentroid)
      , mComputeCentroid(true)
   {
   }

   //! Compute the centroid of the box (Implemented in UtBoundingBoxI).
   virtual void ComputeCentroid() = 0;

   double* mLeast;
   double* mGreatest;
   double* mCentroid;
   bool    mComputeCentroid;
};

/**
    A construct to express the spatial extent of data.
    This class is intended to represent both rectangles for quad-trees and
    rectangular parallelepipeds for octrees.
    @note This class is based on a dimension template.  This dimension
      can be either 2 or 3.
*/
template<size_t Dim>
class UtBoundingBoxI : public UtBoundingBox
{
public:
   using UtBoundingBox::SetGreatest;
   using UtBoundingBox::SetLeast;

   UtBoundingBoxI()
      : UtBoundingBox(new double[Dim], new double[Dim], new double[Dim])
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mLeast[i]    = 0.0;
         mGreatest[i] = 0.0;
      }
   }

   UtBoundingBoxI(const UtBoundingBoxI& aSrc)
      : UtBoundingBox(new double[Dim], new double[Dim], new double[Dim])
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mLeast[i]    = aSrc.mLeast[i];
         mGreatest[i] = aSrc.mGreatest[i];
      }
   }

   ~UtBoundingBoxI() override
   {
      delete[] mLeast;
      delete[] mGreatest;
      delete[] mCentroid;
   }

   UtBoundingBoxI(const double aLeast[], const double aGreatest[])
      : UtBoundingBox(new double[Dim], new double[Dim], new double[Dim])
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mLeast[i]    = aLeast[i];
         mGreatest[i] = aGreatest[i];
      }
   }

   UtBoundingBoxI(const UtBoundingBox& aSrc)
      : UtBoundingBox(new double[Dim], new double[Dim], new double[Dim])
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mLeast[i]    = aSrc.GetLeast(i);
         mGreatest[i] = aSrc.GetGreatest(i);
      }
   }

   UtBoundingBox* Clone() const { return new UtBoundingBoxI(*this); }

   //! Return whether the given point is contained within the
   //! bounding box.  In the case of a quad-tree, only the first
   //! two numbers of a three-dimensional point are considered
   //! (alternately, a 2-D point may be used).
   bool IsContained(const double aPoint[]) const
   {
      unsigned char i(0);
      bool          isContained(true);

      while ((isContained) && (i < Dim))
      {
         if ((aPoint[i] < mLeast[i]) || (aPoint[i] > mGreatest[i]))
         {
            isContained = false;
            break;
         }
         ++i;
      }

      return isContained;
   }
   //! Computes the square minimum distance from aPoint to the bounding box.
   //! Returns 0 if the point lies in the box.
   double DistanceSquareFrom(const double aPoint[]) const
   {
      double squareSum = 0;
      for (unsigned int i = 0; i < Dim; ++i)
      {
         // a or b are positive if the value lies outside the box
         // so max(a,b) is distance from box in this coordinate
         double a    = aPoint[i] - mGreatest[i];
         double b    = mLeast[i] - aPoint[i];
         double diff = std::max(std::max(a, b), 0.0);
         squareSum += diff * diff;
      }
      return squareSum;
   }

   //! return the indicated min. value of the bounding box.
   const double& GetLeast(int aIndex) const override { return mLeast[aIndex]; }

   //! return the indicated max. value of the bounding box.
   const double& GetGreatest(int aIndex) const override { return mGreatest[aIndex]; }

   //! Return whether the given bounding box intersects the object.
   bool Intersects(const UtBoundingBox& aBBox) const override
   {
      bool intersects = true;

      // check least or greatest values within limits of bounding box
      for (unsigned int i = 0; i < Dim; ++i)
      {
         intersects = (((mLeast[i] >= aBBox.GetLeast(i)) && (mLeast[i] <= aBBox.GetGreatest(i))) ||
                       ((mGreatest[i] >= aBBox.GetLeast(i)) && (mGreatest[i] <= aBBox.GetGreatest(i))) ||
                       ((aBBox.GetLeast(i) >= mLeast[i]) && (aBBox.GetGreatest(i) <= mGreatest[i])) ||
                       ((aBBox.GetLeast(i) <= mLeast[i]) && (aBBox.GetGreatest(i) >= mGreatest[i])));
         if (!intersects)
         {
            break;
         }
      }
      return intersects;
   }

   //! Return whether the given line segment intersects this bounding box.
   bool Intersects(const double* aEndPoint1, const double* aEndPoint2) const override
   {
      char   csClip1 = 0;
      char   csClip2 = 0;
      char   testVal = 1;
      double a;
      double b;
      double c;
      double d;
      double temp;
      int    jdim;

      for (unsigned char i = 0; i < Dim; ++i)
      {
         if (i != 0)
         {
            csClip1 = csClip1 << 1;
            csClip2 = csClip2 << 1;
         }
         if (aEndPoint1[i] < GetLeast(i))
         {
            csClip1 += 1;
         }
         if (aEndPoint2[i] < GetLeast(i))
         {
            csClip2 += 1;
         }
         csClip1 = csClip1 << 1;
         csClip2 = csClip2 << 1;
         if (aEndPoint1[i] > GetGreatest(i))
         {
            csClip1 += 1;
         }
         if (aEndPoint2[i] > GetGreatest(i))
         {
            csClip2 += 1;
         }
      }
      if ((csClip1 == 0) || (csClip2 == 0)) // an endpoint is inside
      {
         return true;
      }
      else if ((csClip1 & csClip2) != 0) // both endpoints are outside of the same bounding plane
      {
         return false;
      }
      else
      {
         a = 0;
         b = 1;
         for (unsigned char j = 0; j < Dim; ++j)
         {
            jdim = Dim - j - 1;
            c    = a;
            d    = b;
            if ((csClip1 ^ csClip2) & testVal) // if there was no transition we will learn nothing from intersection
            {
               if (aEndPoint1[jdim] > aEndPoint2[jdim])
               {
                  c = (GetGreatest(jdim) - aEndPoint1[jdim]) / (aEndPoint2[jdim] - aEndPoint1[jdim]);
               }
               else
               {
                  d = (GetGreatest(jdim) - aEndPoint1[jdim]) / (aEndPoint2[jdim] - aEndPoint1[jdim]);
               }
            }
            testVal = testVal << 1;
            if ((csClip1 ^ csClip2) & testVal) // if there was no transition we will learn nothing from intersection
            {
               if (aEndPoint1[jdim] > aEndPoint2[jdim])
               {
                  d = (GetLeast(jdim) - aEndPoint1[jdim]) / (aEndPoint2[jdim] - aEndPoint1[jdim]);
               }
               else
               {
                  c = (GetLeast(jdim) - aEndPoint1[jdim]) / (aEndPoint2[jdim] - aEndPoint1[jdim]);
               }
            }
            testVal = testVal << 1;

            if (c > d)
            {
               temp = c;
               c    = d;
               d    = temp;
            }
            if (c > a)
            {
               a = c;
            }
            if (d < b)
            {
               b = d;
            }
            if ((b < a) || (a > 1) || (b < 0))
            {
               return false;
            }
         }
      }
      return true;
   }

   //! Return whether the object contains the given bounding box.
   bool Contains(const UtBoundingBox& aBBox) const override
   {
      bool contains = true;

      // check least or greatest values within limits of bounding box
      for (unsigned char i = 0; i < Dim; ++i)
      {
         contains = ((aBBox.GetLeast(i) >= mLeast[i]) && (aBBox.GetGreatest(i) <= mGreatest[i]));
         if (!contains)
         {
            break;
         }
      }
      return contains;
   }

   //! Subdivide this bounding box along one of its axes.
   //! This function is called multiple times to subdivide the box
   //! when an additional level in a spatial tree is being created.
   void SubdivideAlongAxis(Axis aAxis, UtBoundingBoxI<Dim>& aLeftBox, UtBoundingBoxI<Dim>& aRightBox)
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         aLeftBox.mLeast[i]     = mLeast[i];
         aLeftBox.mGreatest[i]  = mGreatest[i];
         aRightBox.mLeast[i]    = mLeast[i];
         aRightBox.mGreatest[i] = mGreatest[i];
      }

      aLeftBox.mGreatest[aAxis]  = aLeftBox.GetCentroid(aAxis);
      aRightBox.mLeast[aAxis]    = aRightBox.GetCentroid(aAxis);
      aLeftBox.mComputeCentroid  = true;
      aRightBox.mComputeCentroid = true;
   }

   size_t GetDimension() const { return Dim; }

   //! Set the "least" point of the box (i.e., the point of the box
   //! with minimum values) to the given value.
   void SetLeast(const double* aLeast)
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mLeast[i] = aLeast[i];
      }
      mComputeCentroid = true;
   }

   //! Set the "greatest" point of the box (i.e., the point of the box
   //! with minimum values) to the given value.
   void SetGreatest(const double* aGreatest)
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mGreatest[i] = aGreatest[i];
      }
      mComputeCentroid = true;
   }

   //! Copy the "least" point of the box (i.e., the point of the box
   //! with minimum values) into the given value.
   void CopyLeast(double* aArray) const
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         aArray[i] = mLeast[i];
      }
   }

   //! Copy the "greatest" point of the box (i.e., the point of the box
   //! with maximum values) into the given value.
   void CopyGreatest(double* aArray) const
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         aArray[i] = mGreatest[i];
      }
   }

   //! Copy the centroid of the box into the given value.
   void CopyCentroid(double* aArray) const
   {
      if (mComputeCentroid)
      {
         ComputeCentroid();
      }
      for (unsigned char i = 0; i < Dim; ++i)
      {
         aArray[i] = mCentroid[i];
      }
   }

   //! Output the box' extents in each of its dimensions.
   void PrintExtent(ut::log::MessageStream& aStream) const override
   {
      for (unsigned short i = 0; i < Dim; ++i)
      {
         auto note = aStream.AddNote() << "Box " << i << ":";
         note.AddNote() << "Dimension: " << i;
         note.AddNote() << "Least: " << mLeast[i];
         note.AddNote() << "Greatest: " << mGreatest[i];
      }
   }

   bool ExtentsOK() const override
   {
      bool ok = true;
      for (unsigned char i = 0; i < Dim; ++i)
      {
         // ok &= (mGreatest[i] > mLeast[i]);  // caused SGI warning
         if (mGreatest[i] <= mLeast[i])
         {
            ok = false;
            break;
         }
      }
      return ok;
   }

protected:
   //! Compute the centroid of the bounding box.
   void ComputeCentroid() override
   {
      for (unsigned char i = 0; i < Dim; ++i)
      {
         mCentroid[i] = (mGreatest[i] + mLeast[i]) / 2.0;
      }
   }

private:
};

using UtRectangle = UtBoundingBoxI<2>;
using UtRectP     = UtBoundingBoxI<3>; // rectangular parallelepiped

#endif
