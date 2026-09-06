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

#ifndef UTVEC2_HPP
#define UTVEC2_HPP

#include "ut_export.h"

#include <cmath>
#include <cstddef> // for size_t

// This is a very simple 2-D numeric vector class that provides normal
// arithmetic operations as well as dot, cross, magnitude and normalize.  It
// is a template class with pre-defined typedef's for float and double
// (UtVec2f and UtVec2d) respectively.  It could be used for other data types,
// but it is probably only useful for float and double.
//
// Almost all operators are implemented two ways: utilizing classic C++
// operator overloading AND explicit methods like Add, Subtract, etc.  The
// reason for this is that the classic C++ operator+ (and -, *, /) must do a
// costly return-by-value.  I have attempted to facilitate return-by-value
// optimization, but it still isn't as good as explicit methods.
//
// For example:
//   v3.Add(v1, v2) runs significantly faster than v3 = v1 + v2.
//
// Also, using the explicit operators would enable one to write code that
// could be migrated to Java where operator overloading isn't available.
//
// This class also provides static methods that allow similar operations
// on 2 element arrays.  This is mostly used where one is already using
// arrays and don't want the overhead of creating a temporary UtVec object.
//
// The syntax is very similar to the explicit object notation:
//
//   v3.Add(v1, v2);               // v3 = v1 + v2 where v1, v2 and v3 are UtVec2d
//
// becomes:
//
//   UtVec2d::Add(a3, a1, a2);     // a3 = a1 + a2 where a1, a2 and a3 are double[2]
//
// ALSO NOTE: Vector * Vector and Vector / Vector are not provided.

template<typename T>
class UtVec2
{
public:
   // Constructors

   UtVec2()
   {
      mVec[0] = static_cast<T>(0);
      mVec[1] = static_cast<T>(0);
   }

   UtVec2(T aVal)
   {
      mVec[0] = aVal;
      mVec[1] = aVal;
   }

   UtVec2(const T aVal[])
   {
      mVec[0] = aVal[0];
      mVec[1] = aVal[1];
   }

   UtVec2(T aX, T aY)
   {
      mVec[0] = aX;
      mVec[1] = aY;
   }

   UtVec2(const UtVec2<double>& aSrc)
   {
      mVec[0] = static_cast<T>(aSrc[0]);
      mVec[1] = static_cast<T>(aSrc[1]);
   }

   UtVec2(const UtVec2<float>& aSrc)
   {
      mVec[0] = static_cast<T>(aSrc[0]);
      mVec[1] = static_cast<T>(aSrc[1]);
   }

   // Special method to return a 'const T*' to the underlying array.
   // This allows the data to be used with routines that use arrays.

   const T* GetData() const { return mVec; }

   // This allows the data to be used with routines that use non-const arrays.

   T* GetData() { return mVec; }

   // **********************************************************************
   //                         Explicit operators
   // **********************************************************************

   // Accessors

   void Get(UtVec2<T>& aVec) const
   {
      aVec[0] = mVec[0];
      aVec[1] = mVec[1];
   }

   void Get(T aVec[]) const
   {
      aVec[0] = mVec[0];
      aVec[1] = mVec[1];
   }

   void Get(T& aX, T& aY) const
   {
      aX = mVec[0];
      aY = mVec[1];
   }

   T Get(size_t aIndex) const { return mVec[aIndex]; }

   T Get(int aIndex) const { return mVec[aIndex]; }

   // Mutators

   UtVec2<T>& Set(const UtVec2<T>& aRhs)
   {
      mVec[0] = aRhs.mVec[0];
      mVec[1] = aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Set(const T aRhs[])
   {
      mVec[0] = aRhs[0];
      mVec[1] = aRhs[1];
      return *this;
   }

   UtVec2<T>& Set(T aX, T aY)
   {
      mVec[0] = aX;
      mVec[1] = aY;
      return *this;
   }

   // Scalar assignment

   UtVec2<T>& Set(T aRhs)
   {
      mVec[0] = aRhs;
      mVec[1] = aRhs;
      return *this;
   }

   // Comparison operators

   // Element .vs. element
   bool Equals(const UtVec2<T>& aRhs) const { return ((mVec[0] == aRhs.mVec[0]) && (mVec[1] == aRhs.mVec[1])); }

   // Element .vs. scalar
   bool Equals(T aRhs) const { return ((mVec[0] == aRhs) && (mVec[1] == aRhs)); }

   // Single operand arithmetic methods. (i.e.: this = this op operand
   //
   // The return value is a reference to '*this' to allow chaining.

   UtVec2<T>& Add(const UtVec2<T>& aRhs)
   {
      mVec[0] += aRhs.mVec[0];
      mVec[1] += aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Add(T aRhs)
   {
      mVec[0] += aRhs;
      mVec[1] += aRhs;
      return *this;
   }

   UtVec2<T>& Subtract(const UtVec2<T>& aRhs)
   {
      mVec[0] -= aRhs.mVec[0];
      mVec[1] -= aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Subtract(T aRhs)
   {
      mVec[0] -= aRhs;
      mVec[1] -= aRhs;
      return *this;
   }

   UtVec2<T>& Multiply(T aRhs)
   {
      mVec[0] *= aRhs;
      mVec[1] *= aRhs;
      return *this;
   }

   UtVec2<T>& Divide(T aRhs)
   {
      mVec[0] /= aRhs;
      mVec[1] /= aRhs;
      return *this;
   }

   // Two operand arithmetic methods. (i.e.: this = operand1 op operand2)
   // These should be used instead of the classic C++ operator(+,-,*,/)
   // to eliminate return value optimization.
   //
   // The return value is a reference to '*this' to allow chaining.

   UtVec2<T>& Add(const UtVec2<T>& aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs.mVec[0] + aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] + aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Add(const UtVec2<T>& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] + aRhs;
      mVec[1] = aLhs.mVec[1] + aRhs;
      return *this;
   }

   UtVec2<T>& Add(T aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs + aRhs.mVec[0];
      mVec[1] = aLhs + aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Subtract(const UtVec2<T>& aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs.mVec[0] - aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] - aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Subtract(const UtVec2<T>& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] - aRhs;
      mVec[1] = aLhs.mVec[1] - aRhs;
      return *this;
   }

   UtVec2<T>& Subtract(T aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs - aRhs.mVec[0];
      mVec[1] = aLhs - aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Multiply(const UtVec2<T>& aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs.mVec[0] * aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] * aRhs.mVec[1];
      return *this;
   }

   UtVec2<T>& Multiply(const UtVec2<T>& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] * aRhs;
      mVec[1] = aLhs.mVec[1] * aRhs;
      return *this;
   }

   UtVec2<T>& Multiply(T aLhs, const UtVec2<T>& aRhs)
   {
      mVec[0] = aLhs * aRhs.mVec[0];
      mVec[1] = aLhs * aRhs.mVec[1];
      return *this;
   }

   // Higher-level operations...


   // Compute the dot product of aLhs dot aRhs

   static T DotProduct(const UtVec2<T>& aLhs, const UtVec2<T>& aRhs)
   {
      return ((aLhs.mVec[0] * aRhs.mVec[0]) + (aLhs.mVec[1] * aRhs.mVec[1]));
   }

   // Compute the dot product of this dot aV

   T DotProduct(const UtVec2<T>& aV) const { return ((mVec[0] * aV.mVec[0]) + (mVec[1] * aV.mVec[1])); }

   // Compute the cross product of 2 vectors
   // Cross product is only defined for 3d vectors.
   // This function computes the 'z' value of crossing the 2d vectors in 3d space
   static T CrossProduct(const UtVec2<T>& aLhs, const UtVec2<T>& aRhs) { return aLhs[0] * aRhs[1] - aLhs[1] * aRhs[0]; }

   // Compute the cross product of this and aRhs

   T CrossProduct(const UtVec2<T>& aRhs) { return CrossProduct(*this, aRhs); }

   // Returns the vector clockwise 90 degrees
   void RotateCW()
   {
      T tmp(mVec[0]);
      mVec[0] = mVec[1];
      mVec[1] = -tmp;
   }

   // Returns the vector counterclockwise 90 degrees
   void RotateCCW()
   {
      T tmp(mVec[0]);
      mVec[0] = -mVec[1];
      mVec[1] = tmp;
   }

   // Return the magnitude of 'this'.

   T Magnitude() const { return static_cast<T>(sqrt((mVec[0] * mVec[0]) + (mVec[1] * mVec[1]))); }

   // Return the magnitude squared of 'this'.  This avoids the square root.

   T MagnitudeSquared() const { return ((mVec[0] * mVec[0]) + (mVec[1] * mVec[1])); }

   // Replaces 'this' with the normalized vector.
   // The return value is the magnitude of the original vector.

   T Normalize()
   {
      T magnitude = Magnitude();
      if (magnitude > static_cast<T>(0.0))
      {
         mVec[0] /= magnitude;
         mVec[1] /= magnitude;
      }
      return magnitude;
   }

   // Replaces the vector supplied as the argument with the normalized
   // 'this'.  'this' is not modified.
   //
   // The return value is the magnitude of the original vector.

   T Normalize(UtVec2<T>& aOut) const
   {
      T magnitude = Magnitude();
      if (magnitude != static_cast<T>(0.0))
      {
         aOut.mVec[0] = mVec[0] / magnitude;
         aOut.mVec[1] = mVec[1] / magnitude;
      }
      else
      {
         aOut.mVec[0] = static_cast<T>(0.0);
         aOut.mVec[1] = static_cast<T>(0.0);
      }
      return magnitude;
   }

   // **********************************************************************
   //                   Classic C++ operator overloading
   // **********************************************************************

   // Vector assignment
   const UtVec2<T>& operator=(const UtVec2<double>& aRhs)
   {
      mVec[0] = static_cast<T>(aRhs[0]);
      mVec[1] = static_cast<T>(aRhs[1]);
      return *this;
   }

   const UtVec2<T>& operator=(const UtVec2<float>& aRhs)
   {
      mVec[0] = static_cast<T>(aRhs[0]);
      mVec[1] = static_cast<T>(aRhs[1]);
      return *this;
   }

   // Scalar assignment
   UtVec2<T>& operator=(T aRhs)
   {
      mVec[0] = aRhs;
      mVec[1] = aRhs;
      return *this;
   }

   // Comparison operators

   // Element .vs. element
   bool operator==(const UtVec2<T>& aRhs) const { return Equals(aRhs); }

   //// x is higher priority than y, in comparison
   bool operator<(const UtVec2<T>& aRhs) const
   {
      return (mVec[0] < aRhs.mVec[0]) || ((mVec[0] == aRhs.mVec[0]) && (mVec[1] < aRhs.mVec[1]));
   }


   // Element .vs. scalar
   bool operator==(T aRhs) const { return Equals(aRhs); }

   // Addition operators

   UtVec2<T>& operator+=(const UtVec2<T>& aRhs)
   {
      mVec[0] += aRhs.mVec[0];
      mVec[1] += aRhs.mVec[1];
      return *this;
   }

   UtVec2<T> operator+(const UtVec2<T>& aRhs) const
   {
      return UtVec2<T>(mVec[0] + aRhs.mVec[0], mVec[1] + aRhs.mVec[1]);
   }

   UtVec2<T>& operator+=(T aRhs)
   {
      mVec[0] += aRhs;
      mVec[1] += aRhs;
      return *this;
   }

   UtVec2<T> operator+(T aRhs) const { return UtVec2<T>(mVec[0] + aRhs, mVec[1] + aRhs); }

   // Subtraction operators

   UtVec2<T>& operator-=(const UtVec2<T>& aRhs)
   {
      mVec[0] -= aRhs.mVec[0];
      mVec[1] -= aRhs.mVec[1];
      return *this;
   }

   UtVec2<T> operator-(const UtVec2<T>& aRhs) const
   {
      return UtVec2<T>(mVec[0] - aRhs.mVec[0], mVec[1] - aRhs.mVec[1]);
   }

   UtVec2<T>& operator-=(T aRhs)
   {
      mVec[0] -= aRhs;
      mVec[1] -= aRhs;
      return *this;
   }

   UtVec2<T> operator-(T aRhs) const { return UtVec2<T>(mVec[0] - aRhs, mVec[1] - aRhs); }

   UtVec2<T> operator-() const { return UtVec2<T>(-mVec[0], -mVec[1]); }

   // Multiplication operators

   UtVec2<T>& operator*=(T aRhs)
   {
      mVec[0] *= aRhs;
      mVec[1] *= aRhs;
      return *this;
   }

   UtVec2<T> operator*(T aRhs) const { return UtVec2<T>(mVec[0] * aRhs, mVec[1] * aRhs); }

   // Division operators

   UtVec2<T>& operator/=(T aRhs)
   {
      mVec[0] /= aRhs;
      mVec[1] /= aRhs;
      return *this;
   }

   UtVec2<T> operator/(T aRhs) const { return UtVec2<T>(mVec[0] / aRhs, mVec[1] / aRhs); }

   // Element access operators

   const T& operator[](size_t aIndex) const { return mVec[aIndex]; }

   const T& operator[](int aIndex) const { return mVec[aIndex]; }

   // Return a modifiable reference to a particular element.

   T& operator[](size_t aIndex) { return mVec[aIndex]; }

   T& operator[](int aIndex) { return mVec[aIndex]; }

   // Start of the static methods to provide the same functionality on
   // 2 element arrays (Except operator overloading)

   // Assignment operations

   // Vector assignment, aR = aX

   static void Set(T aR[2], const T aX[2])
   {
      aR[0] = aX[0];
      aR[1] = aX[1];
   }

   static void Set(T aR[2], const T aX, const T aY)
   {
      aR[0] = aX;
      aR[1] = aY;
   }

   static void Set(T aR[2], T aS)
   {
      aR[0] = aS;
      aR[1] = aS;
   }

   // Comparison

   // Element-wise equality
   static bool Equals(const T aX[2], const T aY[2]) { return ((aX[0] == aY[0]) && (aX[1] == aY[1])); }

   // Element vs. Scalar equality

   static bool Equals(const T aX[2], const T aS) { return ((aX[0] == aS) && (aX[1] == aS)); }

   // Element-wise addition, aR = aX + aY

   static void Add(T aR[2], const T aX[2], const T aY[2])
   {
      aR[0] = aX[0] + aY[0];
      aR[1] = aX[1] + aY[1];
   }

   // Element-wise addition of a product, aR = aX + (k * aY)

   static void AddProduct(T aR[2], const T aX[2], const T& aK, const T aY[2])
   {
      aR[0] = aX[0] + aK * aY[0];
      aR[1] = aX[1] + aK * aY[1];
   }

   // Add a scalar to each element aR = aX + aS

   static void Add(T aR[2], const T aX[2], const T aS)
   {
      aR[0] = aX[0] + aS;
      aR[1] = aX[1] + aS;
   }

   // Add a scalar to each element aR += aS

   static void Add(T aR[2], const T aS)
   {
      aR[0] += aS;
      aR[1] += aS;
   }

   // Element-wise subtraction, aR = aX - aY

   static void Subtract(T aR[2], const T aX[2], const T aY[2])
   {
      aR[0] = aX[0] - aY[0];
      aR[1] = aX[1] - aY[1];
   }

   // Subtract a scalar from each element, aR = aX - aS

   static void Subtract(T aR[2], const T aX[2], const T aS)
   {
      aR[0] = aX[0] - aS;
      aR[1] = aX[1] - aS;
   }

   // Subtract a scalar from each element aR -= aS

   static void Subtract(T aR[2], const T aS)
   {
      aR[0] -= aS;
      aR[1] -= aS;
   }

   // Element-wise multiplication

   static void Multiply(T aR[2], const T aX[2], const T aY[2])
   {
      aR[0] = aX[0] * aY[0];
      aR[1] = aX[1] * aY[1];
   }

   // Multiply each element by a scalar, aR = aX * aS

   static void Multiply(T aR[2], const T aX[2], const T aS)
   {
      aR[0] = aX[0] * aS;
      aR[1] = aX[1] * aS;
   }

   // Multiply each element by a scalar, aR *= aS

   static void Multiply(T aR[2], const T aS)
   {
      aR[0] *= aS;
      aR[1] *= aS;
   }

   // Element-wise division

   static void Divide(T aR[2], const T aX[2], const T aY[2])
   {
      aR[0] = aX[0] / aY[0];
      aR[1] = aX[1] / aY[1];
   }

   // Divide each element by a scalar, aR = aX / aS

   static void Divide(T aR[2], const T aX[2], const T aS)
   {
      aR[0] = aX[0] / aS;
      aR[1] = aX[1] / aS;
   }

   // Divide each element by a scalar, aR /= aS

   static void Divide(T aR[2], const T aS)
   {
      aR[0] /= aS;
      aR[1] /= aS;
   }

   // Higher level operations

   // Compute the dot product: aX dot aY

   static T DotProduct(const T aX[2], const T aY[2]) { return ((aX[0] * aY[0]) + (aX[1] * aY[1])); }

   // Return the magnitude of 'aX'.

   static T Magnitude(const T aX[2]) { return static_cast<T>(sqrt((aX[0] * aX[0]) + (aX[1] * aX[1]))); }

   // Return the magnitude squared of 'aX'.  This avoids the square root.

   static T MagnitudeSquared(const T aX[2]) { return ((aX[0] * aX[0]) + (aX[1] * aX[1])); }

   // Replaces 'aX' with the normalized vector.
   // The return value is the magnitude of the 'aX'.

   static T Normalize(T aX[2])
   {
      T magnitude = Magnitude(aX);
      if (magnitude > static_cast<T>(0))
      {
         aX[0] /= magnitude;
         aX[1] /= magnitude;
      }
      return magnitude;
   }

   // Replaces 'aR' with the normalized 'aX'.
   //
   // The return value is the magnitude of the original vector.

   static T Normalize(T aR[2], const T aX[2])
   {
      T magnitude = Magnitude(aX);
      if (magnitude != static_cast<T>(0))
      {
         aR[0] = aX[0] / magnitude;
         aR[1] = aX[1] / magnitude;
      }
      else
      {
         aR[0] = (T)0.0;
         aR[1] = (T)0.0;
      }
      return magnitude;
   }

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aArchive)
   {
      aArchive& mVec[0] & mVec[1];
   }

private:
   T mVec[2];
};

// These could be declared as friends by the class and use element access on
// the RHS, but some compilers (MS VC++ version 5 in particular) didn't work.

template<class T>
inline UtVec2<T> operator+(T aLhs, const UtVec2<T>& aRhs)
{
   return UtVec2<T>(aLhs + aRhs.Get(0), aLhs + aRhs.Get(1));
}

template<class T>
inline UtVec2<T> operator-(T aLhs, const UtVec2<T>& aRhs)
{
   return UtVec2<T>(aLhs - aRhs.Get(0), aLhs - aRhs.Get(1));
}

template<class T>
inline UtVec2<T> operator*(T aLhs, const UtVec2<T>& aRhs)
{
   return UtVec2<T>(aLhs * aRhs.Get(0), aLhs * aRhs.Get(1));
}

using UtVec2f = UtVec2<float>;
using UtVec2d = UtVec2<double>;

#endif
