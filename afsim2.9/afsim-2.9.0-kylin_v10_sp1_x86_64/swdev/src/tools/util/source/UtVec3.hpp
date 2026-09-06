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

#ifndef UTVEC3_HPP
#define UTVEC3_HPP

#include "ut_export.h"

#include <cmath>
#include <cstddef> // for size_t
#include <ostream>

#include "UtMath.hpp"

namespace ut
{
namespace detail
{
class Vec3Tag
{
};
} // namespace detail

//! This is a very simple 3-D numeric vector class that provides normal
//! arithmetic operations as well as dot, cross, magnitude and normalize.  It
//! is a template class with pre-defined typedef's for float and double
//! (UtVec3f and UtVec3d) respectively.  It could be used for other data types,
//! but it is probably only useful for float and double.
//!
//! Almost all operators are implemented two ways: utilizing classic C++
//! operator overloading AND explicit methods like Add, Subtract, etc.  The
//! reason for this is that the classic C++ operator+ (and -, *, /) must do a
//! costly return-by-value.  I have attempted to facilitate return-by-value
//! optimization, but it still isn't as good as explicit methods.
//!
//! For example:
//!   v3.Add(v1, v2) runs significantly faster than v3 = v1 + v2.
//!
//! Also, using the explicit operators would enable one to write code that
//! could be migrated to Java where operator overloading isn't available.
//!
//! This class also provides static methods that allow similar operations
//! on 3 element arrays.  This is mostly used where one is already using
//! arrays and don't want the overhead of creating a temporary UtVec object.
//!
//! The syntax is very similar to the explicit object notation:
//!
//!   v3.Add(v1, v2);               // v3 = v1 + v2 where v1, v2 and v3 are UtVec3d
//!
//! becomes:
//!
//!   UtVec3d::Add(a3, a1, a2);     // a3 = a1 + a2 where a1, a2 and a3 are double[3]
//!
//! ALSO NOTE: Vector * Vector and Vector / Vector are not provided.
//! @tparam T: The type of the data to store in the vector
//! @tparam Tag: A discriminator to allow the creation of distinct, non-convertible vector types. See UtCoords.hpp.
template<class T, class Tag = ut::detail::Vec3Tag>
class Vec3
{
public:
   // Constructors

   Vec3()
   {
      mVec[0] = static_cast<T>(0);
      mVec[1] = static_cast<T>(0);
      mVec[2] = static_cast<T>(0);
   }

   Vec3(T aVal)
   {
      mVec[0] = aVal;
      mVec[1] = aVal;
      mVec[2] = aVal;
   }

   UT_DEPRECATED operator Vec3<T>()
   {
      return Vec3<T>(*this);
   }

   template<class OtherTag>
   explicit Vec3(const Vec3<T, OtherTag>& aVal)
   {
      mVec[0] = aVal[0];
      mVec[1] = aVal[1];
      mVec[2] = aVal[2];
   }

   Vec3(const double aVal[])
   {
      mVec[0] = static_cast<T>(aVal[0]);
      mVec[1] = static_cast<T>(aVal[1]);
      mVec[2] = static_cast<T>(aVal[2]);
   }
   Vec3(const float aVal[])
   {
      mVec[0] = static_cast<T>(aVal[0]);
      mVec[1] = static_cast<T>(aVal[1]);
      mVec[2] = static_cast<T>(aVal[2]);
   }

   Vec3(T aX, T aY, T aZ)
   {
      mVec[0] = aX;
      mVec[1] = aY;
      mVec[2] = aZ;
   }

   Vec3(const Vec3<double, Tag>& aSrc)
   {
      mVec[0] = static_cast<T>(aSrc[0]);
      mVec[1] = static_cast<T>(aSrc[1]);
      mVec[2] = static_cast<T>(aSrc[2]);
   }

   Vec3(const Vec3<float, Tag>& aSrc)
   {
      mVec[0] = static_cast<T>(aSrc[0]);
      mVec[1] = static_cast<T>(aSrc[1]);
      mVec[2] = static_cast<T>(aSrc[2]);
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

   void Get(Vec3& aVec) const
   {
      aVec[0] = mVec[0];
      aVec[1] = mVec[1];
      aVec[2] = mVec[2];
   }

   void Get(float aVec[]) const
   {
      aVec[0] = static_cast<float>(mVec[0]);
      aVec[1] = static_cast<float>(mVec[1]);
      aVec[2] = static_cast<float>(mVec[2]);
   }

   void Get(double aVec[]) const
   {
      aVec[0] = static_cast<double>(mVec[0]);
      aVec[1] = static_cast<double>(mVec[1]);
      aVec[2] = static_cast<double>(mVec[2]);
   }

   void Get(T& aX, T& aY, T& aZ) const
   {
      aX = mVec[0];
      aY = mVec[1];
      aZ = mVec[2];
   }

   T Get(size_t aIndex) const { return mVec[aIndex]; }

   T Get(int aIndex) const { return mVec[aIndex]; }

   // Mutators

   Vec3& Set(const Vec3& aRhs)
   {
      mVec[0] = aRhs.mVec[0];
      mVec[1] = aRhs.mVec[1];
      mVec[2] = aRhs.mVec[2];
      return *this;
   }

   Vec3& Set(const T aRhs[])
   {
      mVec[0] = aRhs[0];
      mVec[1] = aRhs[1];
      mVec[2] = aRhs[2];
      return *this;
   }

   Vec3& Set(T aX, T aY, T aZ)
   {
      mVec[0] = aX;
      mVec[1] = aY;
      mVec[2] = aZ;
      return *this;
   }

   // Scalar assignment

   Vec3& Set(T aRhs)
   {
      mVec[0] = aRhs;
      mVec[1] = aRhs;
      mVec[2] = aRhs;
      return *this;
   }

   void Set(size_t aIndex, T aValue) { mVec[aIndex] = aValue; }

   void Set(int aIndex, T aValue) { mVec[aIndex] = aValue; }

   // Comparison operators

   // Element .vs. element
   bool Equals(const Vec3& aRhs) const
   {
      return ((mVec[0] == aRhs.mVec[0]) && (mVec[1] == aRhs.mVec[1]) && (mVec[2] == aRhs.mVec[2]));
   }

   // Element .vs. scalar
   bool Equals(T aRhs) const { return ((mVec[0] == aRhs) && (mVec[1] == aRhs) && (mVec[2] == aRhs)); }

   // Single operand arithmetic methods. (i.e.: this = this op operand
   //
   // The return value is a reference to '*this' to allow chaining.

   Vec3& Add(const Vec3& aRhs)
   {
      mVec[0] += aRhs.mVec[0];
      mVec[1] += aRhs.mVec[1];
      mVec[2] += aRhs.mVec[2];
      return *this;
   }

   Vec3& Add(T aRhs)
   {
      mVec[0] += aRhs;
      mVec[1] += aRhs;
      mVec[2] += aRhs;
      return *this;
   }

   Vec3& Subtract(const Vec3& aRhs)
   {
      mVec[0] -= aRhs.mVec[0];
      mVec[1] -= aRhs.mVec[1];
      mVec[2] -= aRhs.mVec[2];
      return *this;
   }

   Vec3& Subtract(T aRhs)
   {
      mVec[0] -= aRhs;
      mVec[1] -= aRhs;
      mVec[2] -= aRhs;
      return *this;
   }

   Vec3& Multiply(T aRhs)
   {
      mVec[0] *= aRhs;
      mVec[1] *= aRhs;
      mVec[2] *= aRhs;
      return *this;
   }

   Vec3& Divide(T aRhs)
   {
      mVec[0] /= aRhs;
      mVec[1] /= aRhs;
      mVec[2] /= aRhs;
      return *this;
   }

   // Two operand arithmetic methods. (i.e.: this = operand1 op operand2)
   // These should be used instead of the classic C++ operator(+,-,*,/)
   // to eliminate return value optimization.
   //
   // The return value is a reference to '*this' to allow chaining.

   Vec3& Add(const Vec3& aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs.mVec[0] + aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] + aRhs.mVec[1];
      mVec[2] = aLhs.mVec[2] + aRhs.mVec[2];
      return *this;
   }

   Vec3& Add(const Vec3& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] + aRhs;
      mVec[1] = aLhs.mVec[1] + aRhs;
      mVec[2] = aLhs.mVec[2] + aRhs;
      return *this;
   }

   Vec3& Add(T aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs + aRhs.mVec[0];
      mVec[1] = aLhs + aRhs.mVec[1];
      mVec[2] = aLhs + aRhs.mVec[2];
      return *this;
   }

   Vec3& Subtract(const Vec3& aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs.mVec[0] - aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] - aRhs.mVec[1];
      mVec[2] = aLhs.mVec[2] - aRhs.mVec[2];
      return *this;
   }

   Vec3& Subtract(const Vec3& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] - aRhs;
      mVec[1] = aLhs.mVec[1] - aRhs;
      mVec[2] = aLhs.mVec[2] - aRhs;
      return *this;
   }

   Vec3& Subtract(T aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs - aRhs.mVec[0];
      mVec[1] = aLhs - aRhs.mVec[1];
      mVec[2] = aLhs - aRhs.mVec[2];
      return *this;
   }

   Vec3& Multiply(const Vec3& aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs.mVec[0] * aRhs.mVec[0];
      mVec[1] = aLhs.mVec[1] * aRhs.mVec[1];
      mVec[2] = aLhs.mVec[2] * aRhs.mVec[2];
      return *this;
   }

   Vec3& Multiply(const Vec3& aLhs, T aRhs)
   {
      mVec[0] = aLhs.mVec[0] * aRhs;
      mVec[1] = aLhs.mVec[1] * aRhs;
      mVec[2] = aLhs.mVec[2] * aRhs;
      return *this;
   }

   Vec3& Multiply(T aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs * aRhs.mVec[0];
      mVec[1] = aLhs * aRhs.mVec[1];
      mVec[2] = aLhs * aRhs.mVec[2];
      return *this;
   }

   // Higher-level operations...

   // Compute the cross product 'this' = aLhs x aRhs.
   // The return value is a reference to '*this' to allow chaining.

   Vec3& CrossProduct(const Vec3& aLhs, const Vec3& aRhs)
   {
      mVec[0] = aLhs.mVec[1] * aRhs.mVec[2] - aLhs.mVec[2] * aRhs.mVec[1];
      mVec[1] = aLhs.mVec[2] * aRhs.mVec[0] - aLhs.mVec[0] * aRhs.mVec[2];
      mVec[2] = aLhs.mVec[0] * aRhs.mVec[1] - aLhs.mVec[1] * aRhs.mVec[0];
      return *this;
   }

   // Compute the dot product of aLhs dot aRhs

   static T DotProduct(const Vec3& aLhs, const Vec3& aRhs)
   {
      return ((aLhs.mVec[0] * aRhs.mVec[0]) + (aLhs.mVec[1] * aRhs.mVec[1]) + (aLhs.mVec[2] * aRhs.mVec[2]));
   }

   // Compute the dot product of this dot aV

   T DotProduct(const Vec3& aV) const
   {
      return ((mVec[0] * aV.mVec[0]) + (mVec[1] * aV.mVec[1]) + (mVec[2] * aV.mVec[2]));
   }

   // Return the magnitude of 'this'.

   T Magnitude() const { return static_cast<T>(sqrt((mVec[0] * mVec[0]) + (mVec[1] * mVec[1]) + (mVec[2] * mVec[2]))); }

   // Return the magnitude squared of 'this'.  This avoids the square root.

   T MagnitudeSquared() const { return ((mVec[0] * mVec[0]) + (mVec[1] * mVec[1]) + (mVec[2] * mVec[2])); }

   // Replaces 'this' with the normalized vector.
   // The return value is the magnitude of the original vector.

   T Normalize()
   {
      T magnitude = Magnitude();
      if (magnitude > static_cast<T>(0.0))
      {
         mVec[0] /= magnitude;
         mVec[1] /= magnitude;
         mVec[2] /= magnitude;
      }
      return magnitude;
   }

   // Returns the normalized vector.  'this' is not modified.

   Vec3 GetNormal() const
   {
      Vec3 aOut;
      Normalize(aOut);
      return aOut;
   }

   // Replaces the vector supplied as the argument with the normalized
   // 'this'.  'this' is not modified.
   //
   // The return value is the magnitude of the original vector.

   T Normalize(Vec3& aOut) const
   {
      T magnitude = Magnitude();
      if (magnitude != static_cast<T>(0.0))
      {
         aOut.mVec[0] = mVec[0] / magnitude;
         aOut.mVec[1] = mVec[1] / magnitude;
         aOut.mVec[2] = mVec[2] / magnitude;
      }
      else
      {
         aOut.mVec[0] = static_cast<T>(0.0);
         aOut.mVec[1] = static_cast<T>(0.0);
         aOut.mVec[2] = static_cast<T>(0.0);
      }
      return magnitude;
   }

   // Returns the angle between 'this' vector and the vector supplied
   // as the argument.

   T AngleWith(const Vec3& aRhs) const
   {
      T lMagnitude = Magnitude();
      T rMagnitude = aRhs.Magnitude();
      if ((lMagnitude == 0.0) || (rMagnitude == 0.0))
      {
         return 0.0;
      }
      T dp = DotProduct(mVec, aRhs) / (lMagnitude * rMagnitude);
      // Limit(dp, -1.0, 1.0);
      if (dp < -1.0)
      {
         dp = -1.0;
      }
      else if (dp > 1.0)
      {
         dp = 1.0;
      }
      return acos(dp);
   }

   // **********************************************************************
   //                   Classic C++ operator overloading
   // **********************************************************************

   // Vector assignment
   const Vec3& operator=(const Vec3<double, Tag>& aRhs)
   {
      mVec[0] = static_cast<T>(aRhs[0]);
      mVec[1] = static_cast<T>(aRhs[1]);
      mVec[2] = static_cast<T>(aRhs[2]);
      return *this;
   }

   const Vec3& operator=(const Vec3<float, Tag>& aRhs)
   {
      mVec[0] = static_cast<T>(aRhs[0]);
      mVec[1] = static_cast<T>(aRhs[1]);
      mVec[2] = static_cast<T>(aRhs[2]);
      return *this;
   }

   // Scalar assignment
   Vec3& operator=(T aRhs)
   {
      mVec[0] = aRhs;
      mVec[1] = aRhs;
      mVec[2] = aRhs;
      return *this;
   }

   // Comparison operators

   // Element .vs. element
   bool operator==(const Vec3& aRhs) const { return Equals(aRhs); }

   // Element .vs. scalar
   bool operator==(T aRhs) const { return Equals(aRhs); }

   // Element .vs. element
   bool operator!=(const Vec3& aRhs) const { return !Equals(aRhs); }

   // Element .vs. scalar
   bool operator!=(T aRhs) const { return !Equals(aRhs); }

   // Addition operators

   Vec3& operator+=(const Vec3& aRhs)
   {
      mVec[0] += aRhs.mVec[0];
      mVec[1] += aRhs.mVec[1];
      mVec[2] += aRhs.mVec[2];
      return *this;
   }

   Vec3 operator+(const Vec3& aRhs) const
   {
      return Vec3(mVec[0] + aRhs.mVec[0], mVec[1] + aRhs.mVec[1], mVec[2] + aRhs.mVec[2]);
   }

   Vec3& operator+=(T aRhs)
   {
      mVec[0] += aRhs;
      mVec[1] += aRhs;
      mVec[2] += aRhs;
      return *this;
   }

   Vec3 operator+(T aRhs) const { return Vec3(mVec[0] + aRhs, mVec[1] + aRhs, mVec[2] + aRhs); }

   // Subtraction operators

   Vec3& operator-=(const Vec3& aRhs)
   {
      mVec[0] -= aRhs.mVec[0];
      mVec[1] -= aRhs.mVec[1];
      mVec[2] -= aRhs.mVec[2];
      return *this;
   }

   Vec3 operator-(const Vec3& aRhs) const
   {
      return Vec3(mVec[0] - aRhs.mVec[0], mVec[1] - aRhs.mVec[1], mVec[2] - aRhs.mVec[2]);
   }

   Vec3& operator-=(T aRhs)
   {
      mVec[0] -= aRhs;
      mVec[1] -= aRhs;
      mVec[2] -= aRhs;
      return *this;
   }

   Vec3 operator-(T aRhs) const { return Vec3(mVec[0] - aRhs, mVec[1] - aRhs, mVec[2] - aRhs); }

   Vec3 operator-() const { return Vec3(-mVec[0], -mVec[1], -mVec[2]); }

   friend Vec3<T, Tag> operator+(T aLhs, const Vec3<T, Tag>& aRhs)
   {
      return Vec3<T, Tag>(aLhs + aRhs.Get(0), aLhs + aRhs.Get(1), aLhs + aRhs.Get(2));
   }

   friend Vec3<T, Tag> operator-(T aLhs, const Vec3<T, Tag>& aRhs)
   {
      return Vec3<T, Tag>(aLhs - aRhs.Get(0), aLhs - aRhs.Get(1), aLhs - aRhs.Get(2));
   }

   friend Vec3<T, Tag> operator*(T aLhs, const Vec3<T, Tag>& aRhs)
   {
      return Vec3<T, Tag>(aLhs * aRhs.Get(0), aLhs * aRhs.Get(1), aLhs * aRhs.Get(2));
   }

   // Multiplication operators

   Vec3& operator*=(T aRhs)
   {
      mVec[0] *= aRhs;
      mVec[1] *= aRhs;
      mVec[2] *= aRhs;
      return *this;
   }

   Vec3 operator*(T aRhs) const { return Vec3(mVec[0] * aRhs, mVec[1] * aRhs, mVec[2] * aRhs); }

   // Division operators

   Vec3& operator/=(T aRhs)
   {
      mVec[0] /= aRhs;
      mVec[1] /= aRhs;
      mVec[2] /= aRhs;
      return *this;
   }

   Vec3 operator/(T aRhs) const { return Vec3(mVec[0] / aRhs, mVec[1] / aRhs, mVec[2] / aRhs); }

   // Element access operators

   const T& operator[](size_t aIndex) const { return mVec[aIndex]; }

   const T& operator[](int aIndex) const { return mVec[aIndex]; }

   // Return a modifiable reference to a particular element.

   T& operator[](size_t aIndex) { return mVec[aIndex]; }

   T& operator[](int aIndex) { return mVec[aIndex]; }

   // Stream insertion operator

   friend std::ostream& operator<<(std::ostream& aOut, const Vec3& aVec)
   {
      aOut << '(' << aVec[0] << ", " << aVec[1] << ", " << aVec[2] << ')';
      return aOut;
   }

   // Start of the static methods to provide the same functionality on
   // 3 element arrays (Except operator overloading)

   // Assignment operations

   // Vector assignment, aR = aX

   static void Set(T aR[3], const T aX[3])
   {
      aR[0] = aX[0];
      aR[1] = aX[1];
      aR[2] = aX[2];
   }

   static void Set(T aR[3], const T aX, const T aY, const T aZ)
   {
      aR[0] = aX;
      aR[1] = aY;
      aR[2] = aZ;
   }

   static void Set(T aR[3], T aS)
   {
      aR[0] = aS;
      aR[1] = aS;
      aR[2] = aS;
   }

   // Comparison

   // Element-wise equality
   static bool Equals(const T aX[3], const T aY[3])
   {
      return ((aX[0] == aY[0]) && (aX[1] == aY[1]) && (aX[2] == aY[2]));
   }

   // Element vs. Scalar equality

   static bool Equals(const T aX[3], const T aS) { return ((aX[0] == aS) && (aX[1] == aS) && (aX[2] == aS)); }

   // Element-wise addition, aR = aX + aY

   static void Add(T aR[3], const T aX[3], const T aY[3])
   {
      aR[0] = aX[0] + aY[0];
      aR[1] = aX[1] + aY[1];
      aR[2] = aX[2] + aY[2];
   }

   // Element-wise addition of a product, aR = aX + (k * aY)

   static void AddProduct(T aR[3], const T aX[3], const T& aK, const T aY[3])
   {
      aR[0] = aX[0] + aK * aY[0];
      aR[1] = aX[1] + aK * aY[1];
      aR[2] = aX[2] + aK * aY[2];
   }

   // Add a scalar to each element aR = aX + aS

   static void Add(T aR[3], const T aX[3], const T aS)
   {
      aR[0] = aX[0] + aS;
      aR[1] = aX[1] + aS;
      aR[2] = aX[2] + aS;
   }

   // Add a scalar to each element aR += aS

   static void Add(T aR[3], const T aS)
   {
      aR[0] += aS;
      aR[1] += aS;
      aR[2] += aS;
   }

   // Element-wise subtraction, aR = aX - aY

   static void Subtract(T aR[3], const T aX[3], const T aY[3])
   {
      aR[0] = aX[0] - aY[0];
      aR[1] = aX[1] - aY[1];
      aR[2] = aX[2] - aY[2];
   }

   // Subtract a scalar from each element, aR = aX - aS

   static void Subtract(T aR[3], const T aX[3], const T aS)
   {
      aR[0] = aX[0] - aS;
      aR[1] = aX[1] - aS;
      aR[2] = aX[2] - aS;
   }

   // Subtract a scalar from each element aR -= aS

   static void Subtract(T aR[3], const T aS)
   {
      aR[0] -= aS;
      aR[1] -= aS;
      aR[2] -= aS;
   }

   // Element-wise multiplication

   static void Multiply(T aR[3], const T aX[3], const T aY[3])
   {
      aR[0] = aX[0] * aY[0];
      aR[1] = aX[1] * aY[1];
      aR[2] = aX[2] * aY[2];
   }

   // Multiply each element by a scalar, aR = aX * aS

   static void Multiply(T aR[3], const T aX[3], const T aS)
   {
      aR[0] = aX[0] * aS;
      aR[1] = aX[1] * aS;
      aR[2] = aX[2] * aS;
   }

   // Multiply each element by a scalar, aR *= aS

   static void Multiply(T aR[3], const T aS)
   {
      aR[0] *= aS;
      aR[1] *= aS;
      aR[2] *= aS;
   }

   // Element-wise division

   static void Divide(T aR[3], const T aX[3], const T aY[3])
   {
      aR[0] = aX[0] / aY[0];
      aR[1] = aX[1] / aY[1];
      aR[2] = aX[2] / aY[2];
   }

   // Divide each element by a scalar, aR = aX / aS

   static void Divide(T aR[3], const T aX[3], const T aS)
   {
      aR[0] = aX[0] / aS;
      aR[1] = aX[1] / aS;
      aR[2] = aX[2] / aS;
   }

   // Divide each element by a scalar, aR /= aS

   static void Divide(T aR[3], const T aS)
   {
      aR[0] /= aS;
      aR[1] /= aS;
      aR[2] /= aS;
   }

   // Higher level operations

   // Compute the cross product: aR = aX cross aY

   static void CrossProduct(T aR[3], const T aX[3], const T aY[3])
   {
      aR[0] = (aX[1] * aY[2]) - (aX[2] * aY[1]);
      aR[1] = (aX[2] * aY[0]) - (aX[0] * aY[2]);
      aR[2] = (aX[0] * aY[1]) - (aX[1] * aY[0]);
   }

   // Compute the dot product: aX dot aY

   static T DotProduct(const T aX[3], const T aY[3]) { return ((aX[0] * aY[0]) + (aX[1] * aY[1]) + (aX[2] * aY[2])); }

   // Return the magnitude of 'aX'.

   static T Magnitude(const T aX[3])
   {
      return static_cast<T>(sqrt((aX[0] * aX[0]) + (aX[1] * aX[1]) + (aX[2] * aX[2])));
   }

   // Return the magnitude squared of 'aX'.  This avoids the square root.

   static T MagnitudeSquared(const T aX[3]) { return ((aX[0] * aX[0]) + (aX[1] * aX[1]) + (aX[2] * aX[2])); }

   // Replaces 'aX' with the normalized vector.
   // The return value is the magnitude of the 'aX'.

   static T Normalize(T aX[3])
   {
      T magnitude = Magnitude(aX);
      if (magnitude > static_cast<T>(0))
      {
         aX[0] /= magnitude;
         aX[1] /= magnitude;
         aX[2] /= magnitude;
      }
      return magnitude;
   }

   // Replaces 'aR' with the normalized 'aX'.
   //
   // The return value is the magnitude of the original vector.

   static T Normalize(T aR[3], const T aX[3])
   {
      T magnitude = Magnitude(aX);
      if (magnitude != static_cast<T>(0))
      {
         aR[0] = aX[0] / magnitude;
         aR[1] = aX[1] / magnitude;
         aR[2] = aX[2] / magnitude;
      }
      else
      {
         aR[0] = static_cast<T>(0.0);
         aR[1] = static_cast<T>(0.0);
         aR[2] = static_cast<T>(0.0);
      }
      return magnitude;
   }

   //! Rotate aPoint around aNormalizedAxis (right-handed) by aAngle radians
   static Vec3 Rotate(const Vec3& aNormalizedAxis, T aAngle, const Vec3& aPoint)
   {
      // quaternion is created (u, s)
      T    s = cos(T(-0.5) * aAngle);
      Vec3 u(aNormalizedAxis * sin(T(0.5) * aAngle));
      // apply rotation
      const T two(2);
      Vec3    uCross;
      uCross.CrossProduct(u, aPoint);
      return two * u.DotProduct(aPoint) * u + (s * s - u.DotProduct(u)) * aPoint + two * s * uCross;
   }

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mVec[0] & mVec[1] & mVec[2];
   }

protected:
   T mVec[3];
};

#if defined(SWDEV_ALL_USE_DLL)
template class UT_EXPORT ut::Vec3<float>;
template class UT_EXPORT ut::Vec3<double>;
#endif


} // namespace ut

template<typename T>
using UtVec3 = ut::Vec3<T>;

using UtVec3f = UtVec3<float>;
using UtVec3d = UtVec3<double>;

#endif
