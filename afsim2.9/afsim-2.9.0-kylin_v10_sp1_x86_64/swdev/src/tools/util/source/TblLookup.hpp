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

#ifndef TBLLOOKUP_HPP
#define TBLLOOKUP_HPP

// ****************************************************************************
// This defines a set of templates that provide the low level operations for
// performing table lookups for functions of 1, 2, 3 or 4 variables.
//
// These routines were heavily influenced by the St. Louis Flight Simulation
// routines in 'lookup.cpp/.h'.  While this class gives better performance,
// credit must be given to them because several tricks were borrowed.
//
// The focus of these templates is efficiency and avoid the use of virtual
// methods (kinda like STL).  While these templates could be used directly,
// they will most likely be used as part of a higher-level implementation.
//
// The current implementation supports:
//
// *) Linear or logarithmic interpolation
// *) Evenly or unevenly spaced arguments
// *) Either clamp out-of-range values to the nearest endpoint or extrapolate.
//
// The templates are:
//
// *) Independent variables (which must be monotonically increasing)
//
//      TblIndVarE: for equally spaced values
//      TblIndVarU: for unequally spaced values
//
// *) The N-dimensional array of an dependent variable that corresponds to
//    the independent variables:
//
//      TblDepVar
//
// *) Template classes for determining the breakpoints and interpolation
//    factors on the independent variables using linear/logarithmic interpolation:
//
//      TblLookupLU   : Uneven breakpoints, no extrapolation
//      TblLookupLUX  : Uneven breakpoints, allow extrapolation
//      TblLookupLE   : Even breakpoints, no extrapolation
//      TblLookupLEX  : Even breakpoints, allow extrapolation
//      TblLookupLUB  : Same as TblLookupLU but uses binary searching.
//      TblLookupLUBX : Same as TblLookupLUBX but uses binary searching.
//
//    It would be easy to create different types of interpolating functions.
//
// *) Template functions for evaluating the function using the results
//    from a lookup:
//
//      TblEvaluate
//
// For example, lets assume we have two independent variables x, y and there
// are three dependent variable values, f, g and h for each x and y.  The
// values of x and y are unequally spaced and we do not want to use
// extrapolation.
//
// TblIndVar<float> x;
// TblIndVar<float> y;
// TblDepVar2<float> f;
// TblDepVar2<float> g;
// TblDepVar2<float> h;
// TblLookupLU<float> lx;
// TblLookupLU<float> ly;
//
// ... Load x, y, f, g, and h.
//
// ... Find the value of f, g and h at x = 23.5, y = 37.0;
//
// lx.Lookup(x, 23.5);
// ly.Lookup(y, 37.0);
// f1 = TblEvaluate(f, lx, ly);
// g1 = TblEvaluate(g, lx, ly);
// h1 = TblEvaluate(h, lx, ly);
//
// Note that lookups for x and y are performed just once and the results are
// used for 3 evaluations.  The lookup function is fairly intensive for
// unequally spaced arguments so having to perform this once saves time.
//
// ****************************************************************************

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "UtAzElTypes.hpp"


// ***** Independent variable classes

// Unequally spaced breakpoints.

template<class T>
class TblIndVarU
{
public:
   using ContainerType = std::vector<T>;

   using ValueType = T;

   TblIndVarU() = default;

   explicit TblIndVarU(size_t aDim) { Resize(aDim); }

   void Resize(size_t aDim) { mValues.resize(aDim); }

   void Resize(size_t aDim, const T& aValue) { mValues.resize(aDim, aValue); }

   size_t GetSize() const { return static_cast<size_t>(mValues.size()); }

   const T& Get(size_t aIndex) const { return mValues[aIndex]; }

   void Set(const T& aValue, size_t aIndex) { mValues[aIndex] = aValue; }

   const std::vector<T>& GetValues() const { return mValues; }

   std::vector<T>& GetValues() { return mValues; }

   void SetValues(const std::vector<T>& aValues) { mValues = aValues; }

protected:
   std::vector<T> mValues;
};

// Equally spaced breakpoints.

template<class T>
class TblIndVarE
{
public:
   using ValueType = T;

   size_t GetSize() const { return mNumValues; }

   T Get(size_t aIndex) const { return mMinValue + (aIndex * mIncrement); }

   T OneOverIncrement() const { return mOneOverIncrement; }
   T MinOverIncrement() const { return mMinOverIncrement; }

   void SetValues(const T& aMinValue, const T& aMaxValue, size_t aNumValues)
   {
      mMinValue         = aMinValue;
      mMaxValue         = aMaxValue;
      mNumValues        = aNumValues;
      mIncrement        = (mMaxValue - mMinValue) / (mNumValues - 1);
      mOneOverIncrement = (T)(1.0 / mIncrement);
      mMinOverIncrement = mMinValue * mOneOverIncrement;
   }

protected:
   T      mMinValue;
   T      mMaxValue;
   T      mIncrement;
   T      mMinOverIncrement;
   T      mOneOverIncrement;
   size_t mNumValues;
};

// ***** Dependent variable classes.  These are just N-dimensional arrays.

template<class T>
class TblDepVar1
{
public:
   using ValueType     = T;
   using ContainerType = std::vector<T>;

   TblDepVar1() = default;

   explicit TblDepVar1(size_t aDim1) { Resize(aDim1); }

   void Resize(size_t aDim1)
   {
      mDim1 = aDim1;
      mValues.resize(mDim1);
   }

   void GetSize(size_t& aDim1) const { aDim1 = mDim1; }

   T Get(size_t aIndex1) const { return mValues[aIndex1]; }

   void Set(const T& aValue, size_t aIndex1) { mValues[aIndex1] = aValue; }

   std::vector<T>& GetValues() { return mValues; }

   // Only the single dimension form has this as the indexing is unambiguous.
   void SetValues(const std::vector<T>& aValues)
   {
      mValues = aValues;
      mDim1   = static_cast<size_t>(mValues.size());
   }

private:
   std::vector<T> mValues;
   size_t         mDim1;
};

template<class T>
class TblDepVar2
{
public:
   using ValueType     = T;
   using ContainerType = std::vector<T>;

   TblDepVar2() = default;

   explicit TblDepVar2(size_t aDim1, size_t aDim2) { Resize(aDim1, aDim2); }

   void Resize(size_t aDim1, size_t aDim2)
   {
      mDim1 = aDim1;
      mDim2 = aDim2;
      mValues.resize(mDim1 * mDim2);
   }

   void GetSize(size_t& aDim1, size_t& aDim2) const
   {
      aDim1 = mDim1;
      aDim2 = mDim2;
   }

   T Get(size_t aIndex1, size_t aIndex2) const
   {
      size_t index = (aIndex1 * mDim2) + aIndex2;
      return mValues[index];
   }

   void Set(const T& aValue, size_t aIndex1, size_t aIndex2)
   {
      size_t index   = (aIndex1 * mDim2) + aIndex2;
      mValues[index] = aValue;
   }

   std::vector<T>& GetValues() { return mValues; }

private:
   std::vector<T> mValues;
   size_t         mDim1{0};
   size_t         mDim2{0};
};

template<class T>
class TblDepVar3
{
public:
   using ValueType     = T;
   using ContainerType = std::vector<T>;

   TblDepVar3() = default;

   explicit TblDepVar3(size_t aDim1, size_t aDim2, size_t aDim3) { Resize(aDim1, aDim2, aDim3); }

   void Resize(size_t aDim1, size_t aDim2, size_t aDim3)
   {
      mDim1  = aDim1;
      mDim2  = aDim2;
      mDim3  = aDim3;
      mDim23 = mDim2 * mDim3;
      mValues.resize(mDim1 * mDim2 * mDim3);
   }

   void GetSize(size_t& aDim1, size_t& aDim2, size_t& aDim3) const
   {
      aDim1 = mDim1;
      aDim2 = mDim2;
      aDim3 = mDim3;
   }

   T Get(size_t aIndex1, size_t aIndex2, size_t aIndex3) const
   {
      // size_t index = (((aIndex1 * mDim2) + aIndex2) * mDim3) + aIndex3;
      size_t index = (aIndex1 * mDim23) + (aIndex2 * mDim3) + aIndex3;
      return mValues[index];
   }

   void Set(const T& aValue, size_t aIndex1, size_t aIndex2, size_t aIndex3)
   {
      // size_t index = (((aIndex1 * mDim2) + aIndex2) * mDim3) + aIndex3;
      size_t index   = (aIndex1 * mDim23) + (aIndex2 * mDim3) + aIndex3;
      mValues[index] = aValue;
   }

   std::vector<T>& GetValues() { return mValues; }

private:
   std::vector<T> mValues;
   size_t         mDim1{0};
   size_t         mDim2{0};
   size_t         mDim3{0};
   size_t         mDim23;
};

template<class T>
class TblDepVar4
{
public:
   using ValueType     = T;
   using ContainerType = std::vector<T>;

   TblDepVar4() = default;

   explicit TblDepVar4(size_t aDim1, size_t aDim2, size_t aDim3, size_t aDim4) { Resize(aDim1, aDim2, aDim3, aDim4); }

   void Resize(size_t aDim1, size_t aDim2, size_t aDim3, size_t aDim4)
   {
      mDim1   = aDim1;
      mDim2   = aDim2;
      mDim3   = aDim3;
      mDim4   = aDim4;
      mDim34  = mDim3 * mDim4;
      mDim234 = mDim2 * mDim34;
      mValues.resize(mDim1 * mDim2 * mDim3 * mDim4);
   }

   void GetSize(size_t& aDim1, size_t& aDim2, size_t& aDim3, size_t& aDim4) const
   {
      aDim1 = mDim1;
      aDim2 = mDim2;
      aDim3 = mDim3;
      aDim4 = mDim4;
   }

   T Get(size_t aIndex1, size_t aIndex2, size_t aIndex3, size_t aIndex4) const
   {
      // size_t index = (((((aIndex1 * mDim2) + aIndex2) * mDim3) + aIndex3) * mDim4) + aIndex4;
      size_t index = (aIndex1 * mDim234) + (aIndex2 * mDim34) + (aIndex3 * mDim4) + aIndex4;
      return mValues[index];
   }

   void Set(const T& aValue, size_t aIndex1, size_t aIndex2, size_t aIndex3, size_t aIndex4)
   {
      // size_t index = (((((aIndex1 * mDim2) + aIndex2) * mDim3) + aIndex3) * mDim4) + aIndex4;
      size_t index   = (aIndex1 * mDim234) + (aIndex2 * mDim34) + (aIndex3 * mDim4) + aIndex4;
      mValues[index] = aValue;
   }

   std::vector<T>& GetValues() { return mValues; }

private:
   std::vector<T> mValues;
   size_t         mDim1{0};
   size_t         mDim2{0};
   size_t         mDim3{0};
   size_t         mDim4{0};
   size_t         mDim34;
   size_t         mDim234;
};

// ****** Lookup classes on independent variables.
//
// NOTE!
//
// A given instance should not be used for different independent variables
// (i.e.: the same TblIndVar{x} object must be passed on each call to
// Lookup()) nor must the size of the TblIndVar{x} object changed.  If you
// want to share an instance then you must call Reset() prior to each
// Lookup(). The reason for this restriction is that the objects *MAY* retain
// information from search to search in order to improve performance.

template<class T>
class TblLookupL
{
public:
   using ValueType = T;

   const T& GetRatio() const { return mRatio; }

   size_t GetIndex() const { return mIndex; }

   void Reset() { mIndex = 0; }

protected:
   T      mRatio{0.0};
   size_t mIndex{0};
};


// No interpolation with evenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.

template<class T>
class TblLookupE : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarE<T>& aIndVar, const T& aValue)
   {
      T realIndex = aValue * aIndVar.OneOverIncrement() - aIndVar.MinOverIncrement();
      if (realIndex <= 0.0)
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0;
      }
      else if (realIndex >= aIndVar.GetSize() - 1)
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         TblLookupL<T>::mIndex = static_cast<size_t>(realIndex);
         TblLookupL<T>::mRatio = 0.0;
      }
   }
};

// No interpolation with evenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.

template<class T>
class TblLookupEX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarE<T>& aIndVar, const T& aValue)
   {
      T realIndex = aValue * aIndVar.OneOverIncrement() - aIndVar.MinOverIncrement();
      if (realIndex <= 0.0)
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = static_cast<T>(static_cast<ptrdiff_t>(realIndex));
      }
      else if (realIndex >= aIndVar.GetSize() - 1)
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = static_cast<T>(static_cast<ptrdiff_t>(realIndex) - TblLookupL<T>::mIndex);
      }
      else
      {
         TblLookupL<T>::mIndex = static_cast<size_t>(realIndex);
         TblLookupL<T>::mRatio = 0.0;
      }
   }
};

// No interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.
//
// This version uses a linear lookup starting from the last lookup point.

template<class T>
class TblLookupU : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0.0;
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         // Continue the search from the last used index as benchmarks
         // seem to bear out that it helps.
         // Also we use a temporary during the lookup.  Some compilers seem
         // to want to keep storing during the iteration.
         size_t index = TblLookupL<T>::mIndex;
         while (aValue > aIndVar.Get(index))
         {
            ++index;
         }

         while (aValue < aIndVar.Get(index))
         {
            --index;
         }
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = 0.0;
      }
   }
};

// No interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.
//
// This version uses a linear lookup starting from the last lookup point.

template<class T>
class TblLookupUX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         size_t index          = 0;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = static_cast<T>(
            static_cast<ptrdiff_t>((aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index))));
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         size_t index          = aIndVar.GetSize() - 2;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = static_cast<T>(
            static_cast<ptrdiff_t>((aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index))));
      }
      else
      {
         // Continue the search from the last used index as benchmarks
         // seem to bear out that it helps.
         // Also we use a temporary during the lookup.  Some compilers seem
         // to want to keep storing during the iteration.
         size_t index = TblLookupL<T>::mIndex;
         while (aValue > aIndVar.Get(index))
         {
            ++index;
         }

         while (aValue < aIndVar.Get(index))
         {
            --index;
         }
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = 0.0;
      }
   }
};

// No interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.
//
// This version utilizes a binary search.  The break even point when compared
// to the linear version is around 64 elements for random inputs.  Your
// mileage may vary depending on the nature of your inputs.

template<class T>
class TblLookupUB : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0.0;
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         const std::vector<T>&                   values = aIndVar.GetValues();
         typename std::vector<T>::const_iterator pos    = std::upper_bound(values.begin(), values.end(), aValue);
         size_t                                  index  = (size_t)std::distance(values.begin(), pos) - 1;
         TblLookupL<T>::mIndex                          = index;
         TblLookupL<T>::mRatio                          = 0.0;
      }
   }
};

// No interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.
//
// This version utilizes a binary search.  The break even point when compared
// to the linear version is around 64 elements for random inputs.  Your
// mileage may vary depending on the nature of your inputs.

template<class T>
class TblLookupUBX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         size_t index          = 0;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = static_cast<T>(
            static_cast<ptrdiff_t>((aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index))));
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         size_t index          = aIndVar.GetSize() - 2;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = static_cast<T>(
            static_cast<ptrdiff_t>((aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index))));
      }
      else
      {
         const std::vector<T>&                   values = aIndVar.GetValues();
         typename std::vector<T>::const_iterator pos    = std::upper_bound(values.begin(), values.end(), aValue);
         size_t                                  index  = static_cast<size_t>(std::distance(values.begin(), pos)) - 1;
         TblLookupL<T>::mIndex                          = index;
         TblLookupL<T>::mRatio                          = 0.0;
      }
   }
};

// Linear/logarithmic interpolation with evenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.

template<class T>
class TblLookupLE : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarE<T>& aIndVar, const T& aValue)
   {
      T realIndex = aValue * aIndVar.OneOverIncrement() - aIndVar.MinOverIncrement();
      if (realIndex <= 0.0)
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0.0;
      }
      else if (realIndex >= aIndVar.GetSize() - 1)
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         TblLookupL<T>::mIndex = static_cast<size_t>(realIndex);
         TblLookupL<T>::mRatio = realIndex - static_cast<T>(TblLookupL<T>::mIndex);
      }
   }
};

// Linear/logarithmic interpolation with evenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.

template<class T>
class TblLookupLEX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarE<T>& aIndVar, const T& aValue)
   {
      T realIndex = aValue * aIndVar.OneOverIncrement() - aIndVar.MinOverIncrement();
      if (realIndex <= 0.0)
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = realIndex;
      }
      else if (realIndex >= aIndVar.GetSize() - 1)
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = realIndex - static_cast<T>(TblLookupL<T>::mIndex);
      }
      else
      {
         TblLookupL<T>::mIndex = static_cast<size_t>(realIndex);
         TblLookupL<T>::mRatio = realIndex - static_cast<T>(TblLookupL<T>::mIndex);
      }
   }
};

// Linear/logarithmic interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.
//
// This version uses a linear lookup starting from the last lookup point.

template<class T>
class TblLookupLU : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0.0;
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         // Continue the search from the last used index as benchmarks
         // seem to bear out that it helps.
         // Also we use a temporary during the lookup.  Some compilers seem
         // to want to keep storing during the iteration.
         size_t index = TblLookupL<T>::mIndex;
         while (aValue > aIndVar.Get(index))
         {
            ++index;
         }

         while (aValue < aIndVar.Get(index))
         {
            --index;
         }
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
   }
};

// Linear/logarithmic interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.
//
// This version uses a linear lookup starting from the last lookup point.

template<class T>
class TblLookupLUX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         size_t index          = 0;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         size_t index          = aIndVar.GetSize() - 2;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
      else
      {
         // Continue the search from the last used index as benchmarks
         // seem to bear out that it helps.
         // Also we use a temporary during the lookup.  Some compilers seem
         // to want to keep storing during the iteration.
         size_t index = TblLookupL<T>::mIndex;
         while (aValue > aIndVar.Get(index))
         {
            ++index;
         }

         while (aValue < aIndVar.Get(index))
         {
            --index;
         }
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
   }
};

// Linear/logarithmic interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.
//
// This version utilizes a binary search.  The break even point when compared
// to the linear version is around 64 elements for random inputs.  Your
// mileage may vary depending on the nature of your inputs.

template<class T>
class TblLookupLUB : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         TblLookupL<T>::mIndex = 0;
         TblLookupL<T>::mRatio = 0.0;
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         TblLookupL<T>::mIndex = aIndVar.GetSize() - 2;
         TblLookupL<T>::mRatio = 1.0;
      }
      else
      {
         const std::vector<T>&                   values = aIndVar.GetValues();
         typename std::vector<T>::const_iterator pos    = std::upper_bound(values.begin(), values.end(), aValue);
         size_t                                  index  = (size_t)std::distance(values.begin(), pos) - 1;
         TblLookupL<T>::mIndex                          = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
   }
};

// Linear/logarithmic interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints will be extrapolated.
//
// This version utilizes a binary search.  The break even point when compared
// to the linear version is around 64 elements for random inputs.  Your
// mileage may vary depending on the nature of your inputs.

template<class T>
class TblLookupLUBX : public TblLookupL<T>
{
public:
   using ValueType = T;

   void Lookup(const TblIndVarU<T>& aIndVar, const T& aValue)
   {
      if (aValue <= aIndVar.Get(0))
      {
         size_t index          = 0;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
      else if (aValue >= aIndVar.Get(aIndVar.GetSize() - 1))
      {
         size_t index          = aIndVar.GetSize() - 2;
         TblLookupL<T>::mIndex = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
      else
      {
         const std::vector<T>&                   values = aIndVar.GetValues();
         typename std::vector<T>::const_iterator pos    = std::upper_bound(values.begin(), values.end(), aValue);
         size_t                                  index  = static_cast<size_t>(std::distance(values.begin(), pos)) - 1;
         TblLookupL<T>::mIndex                          = index;
         TblLookupL<T>::mRatio = (aValue - aIndVar.Get(index)) / (aIndVar.Get(index + 1) - aIndVar.Get(index));
      }
   }
};

// ***** Evaluation functions

template<class DV, class IV1>
inline DV TblEvaluate(const TblDepVar1<DV>&       aDepVar,
                      const TblLookupL<IV1>&      aLookup1,
                      ut::azel::InterpolationType aInterpolationType = ut::azel::InterpolationType::cLinear)
{
   size_t i1 = aLookup1.GetIndex();
   DV     f0 = aDepVar.Get(i1);
   DV     f1 = aDepVar.Get(i1 + 1);
   IV1    r1 = aLookup1.GetRatio();
   switch (aInterpolationType)
   {
   case ut::azel::InterpolationType::cLinear:
      return f0 + r1 * (f1 - f0);
   case ut::azel::InterpolationType::cLogarithmic:
      return (pow(f1, r1) * pow(f0, (1 - r1)));
   default:
      throw std::invalid_argument("Invalid interpolation type");
   }
}

template<class DV, class IV1, class IV2>
inline DV TblEvaluate(const TblDepVar2<DV>&       aDepVar,
                      const TblLookupL<IV1>&      aLookup1,
                      const TblLookupL<IV2>&      aLookup2,
                      ut::azel::InterpolationType aInterpolationType = ut::azel::InterpolationType::cLinear)
{
   size_t i1  = aLookup1.GetIndex();
   size_t i2  = aLookup2.GetIndex();
   DV     f00 = aDepVar.Get(i1, i2);
   DV     f01 = aDepVar.Get(i1, i2 + 1);
   DV     f10 = aDepVar.Get(i1 + 1, i2);
   DV     f11 = aDepVar.Get(i1 + 1, i2 + 1);
   // DV  a   = f00;
   // DV  b   = f10 - f00;
   // DV  c   = f01 - f00;
   // DV  d   = f11 - a - b - c = f11 - f10 - f01 + f00;
   IV1 r1 = aLookup1.GetRatio();
   IV2 r2 = aLookup2.GetRatio();
   switch (aInterpolationType)
   {
   case ut::azel::InterpolationType::cLinear:
      return f00 + ((f10 - f00) * r1) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * r1) * r2;
   case ut::azel::InterpolationType::cLogarithmic:
   {
      // in the following, we use long double to capture precision of IV1 and IV2
      auto r12 = static_cast<long double>(r1 * r2);
      auto t1  = pow(f11, r12);                 // f11 ^ (r1*r2)
      auto t2  = pow(f10, (r1 - r12));          // f10 ^ (r1*(1-r2))
      auto t3  = pow(f01, (r2 - r12));          // f01 ^ (r2*(1-r1))
      auto t4  = pow(f00, (1 - r1 - r2 + r12)); // f00 ^ ((1-r1)*(1-r2))

      // now return f11^(r1*r2) + f10^(r1*(1-r2)) + f01^(r2*(1-r1)) + f00^((1-r1)*(1-r2))
      return static_cast<DV>(t1 * t2 * t3 * t4);
   }
   default:
      throw std::invalid_argument("Invalid interpolation type");
   }
}

template<class DV, class IV1, class IV2, class IV3>
inline DV TblEvaluate(const TblDepVar3<DV>&       aDepVar,
                      const TblLookupL<IV1>&      aLookup1,
                      const TblLookupL<IV2>&      aLookup2,
                      const TblLookupL<IV3>&      aLookup3,
                      ut::azel::InterpolationType aInterpolationType = ut::azel::InterpolationType::cLinear)
{
   size_t i1   = aLookup1.GetIndex();
   size_t i2   = aLookup2.GetIndex();
   size_t i3   = aLookup3.GetIndex();
   DV     f000 = aDepVar.Get(i1, i2, i3);
   DV     f001 = aDepVar.Get(i1, i2, i3 + 1);
   DV     f010 = aDepVar.Get(i1, i2 + 1, i3);
   DV     f011 = aDepVar.Get(i1, i2 + 1, i3 + 1);
   DV     f100 = aDepVar.Get(i1 + 1, i2, i3);
   DV     f101 = aDepVar.Get(i1 + 1, i2, i3 + 1);
   DV     f110 = aDepVar.Get(i1 + 1, i2 + 1, i3);
   DV     f111 = aDepVar.Get(i1 + 1, i2 + 1, i3 + 1);
   DV     a    = f000;
   DV     b    = f100 - f000;
   DV     c    = f010 - f000;
   DV     d    = f001 - f000;
   DV     e    = f110 - a - b - c;
   DV     f    = f101 - a - b - d;
   DV     g    = f011 - a - c - d;
   DV     h    = f111 - a - b - c - d - e - f - g;
   IV1    r1   = aLookup1.GetRatio();
   IV2    r2   = aLookup2.GetRatio();
   IV3    r3   = aLookup3.GetRatio();
   switch (aInterpolationType)
   {
   case ut::azel::InterpolationType::cLinear:
      return a + (r1 * (b + e * r2)) + (r2 * (c + (g + h * r1) * r3)) + (r3 * (d + f * r1));
   case ut::azel::InterpolationType::cLogarithmic:
   {
      // in the following, we use long double to capture precision of IV1 and IV2
      auto r12  = static_cast<long double>(r1 * r2);
      auto r13  = static_cast<long double>(r1 * r3);
      auto r23  = static_cast<long double>(r2 * r3);
      auto r123 = static_cast<long double>(r12 * r3);

      auto t1 = pow(f111, r123);                                        // f111 ^ (r1*r2*r3)
      auto t2 = pow(f110, (r12 - r123));                                // f110 ^ (r1*r2*(1-r3))
      auto t3 = pow(f101, (r13 - r123));                                // f101 ^ (r1*(1-r2)*r3)
      auto t4 = pow(f100, (r1 - r12 - r13 + r123));                     // f100 ^ (r1*(1-r2)*(1-r3))
      auto t5 = pow(f011, (r23 - r123));                                // f011 ^ ((1-r1)*r2*r3)
      auto t6 = pow(f010, (r2 - r12 - r13 + r123));                     // f010 ^ ((1-r1)*r2*(1-r3)))
      auto t7 = pow(f001, (r3 - r13 - r23 + r123));                     // f001 ^ ((1-r1)*(1-r2)*r3))
      auto t8 = pow(f000, (1 - r1 - r2 - r3 + r12 + r13 + r23 - r123)); // f000 ^ ((1-r1)*(1-r2)*(1-r3))

      // now return f111^(r1*r2*r3) + f110^(r1*r2*(1-r3)) + f101^(r1*(1-r2)*r3)
      //          + f100^(r1*(1-r2)*(1-r3)) + f011^((1-r1)*r2*r3) + f010^((1-r1)*r2*(1-r3)))
      //          + f001^((1-r1)*(1-r2)*r3)) + f000^((1-r1)*(1-r2)*(1-r3))
      return static_cast<DV>(t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8);
   }
   default:
      throw std::invalid_argument("Invalid interpolation type");
   }
}

template<class DV, class IV1, class IV2, class IV3, class IV4>
inline DV TblEvaluate(const TblDepVar4<DV>&       aDepVar,
                      const TblLookupL<IV1>&      aLookup1,
                      const TblLookupL<IV2>&      aLookup2,
                      const TblLookupL<IV3>&      aLookup3,
                      const TblLookupL<IV4>&      aLookup4,
                      ut::azel::InterpolationType aInterpolationType = ut::azel::InterpolationType::cLinear)
{
   size_t i1    = aLookup1.GetIndex();
   size_t i2    = aLookup2.GetIndex();
   size_t i3    = aLookup3.GetIndex();
   size_t i4    = aLookup4.GetIndex();
   DV     f0000 = aDepVar.Get(i1, i2, i3, i4);
   DV     f0001 = aDepVar.Get(i1, i2, i3, i4 + 1);
   DV     f1000 = aDepVar.Get(i1 + 1, i2, i3, i4);
   DV     f1001 = aDepVar.Get(i1 + 1, i2, i3, i4 + 1);
   DV     f0100 = aDepVar.Get(i1, i2 + 1, i3, i4);
   DV     f0101 = aDepVar.Get(i1, i2 + 1, i3, i4 + 1);
   DV     f0010 = aDepVar.Get(i1, i2, i3 + 1, i4);
   DV     f0011 = aDepVar.Get(i1, i2, i3 + 1, i4 + 1);
   DV     f1100 = aDepVar.Get(i1 + 1, i2 + 1, i3, i4);
   DV     f1101 = aDepVar.Get(i1 + 1, i2 + 1, i3, i4 + 1);
   DV     f1010 = aDepVar.Get(i1 + 1, i2, i3 + 1, i4);
   DV     f1011 = aDepVar.Get(i1 + 1, i2, i3 + 1, i4 + 1);
   DV     f0110 = aDepVar.Get(i1, i2 + 1, i3 + 1, i4);
   DV     f0111 = aDepVar.Get(i1, i2 + 1, i3 + 1, i4 + 1);
   DV     f1110 = aDepVar.Get(i1 + 1, i2 + 1, i3 + 1, i4);
   DV     f1111 = aDepVar.Get(i1 + 1, i2 + 1, i3 + 1, i4 + 1);
   DV     a     = f0000;
   DV     b     = f1000 - f0000;
   DV     f     = f1100 - f0100 - b; // + f0000 - f1000;
   DV     g     = f1010 - f0010 - b; // + f0000 - f1000;
   DV     h     = f1001 - f0001 - b; // + f0000 - f1000;

   DV c = f0100 - f0000;
   DV i = f0110 - f0010 - c; // + f0000 - f0100;
   DV j = f0101 - f0001 - c; // + f0000 - f0100;

   DV d = f0010 - f0000;
   DV k = f0011 - f0001 - d; // + f0000 - f0010;

   DV e = f0001 - f0000;
   // DV  f = f1100 - f1000 - f0100 + f0000;
   // DV  g = f1010 - f1000 - f0010 + f0000;
   // DV  h = f1001 - f1000 - f0001 + f0000;
   // DV  i = f0110 - f0100 - f0010 + f0000;
   // DV  j = f0101 - f0100 - f0001 + f0000;
   // DV  k = f0011 - f0010 - f0001 + f0000;
   DV l = -f0000 + f1000 + f0100 - f1100 + f0010 - f1010 - f0110 + f1110;
   DV m = f0100 - f1100 - f0000 + f1000 + f0001 - f1001 - f0101 + f1101;
   DV n = f0010 - f1010 - f0000 + f1000 + f0001 - f1001 - f0011 + f1011;
   DV o = -f0110 - f0000 + f0100 + f0001 - f0101 + f0010 - f0011 + f0111;
   DV p = f0000 - f1000 - f0100 + f1100 - f0010 + f1010 + f0110 - f1110 - f0001 + f1001 + f0101 - f1101 + f0011 -
          f1011 - f0111 + f1111;
   IV1 r1 = aLookup1.GetRatio();
   IV2 r2 = aLookup2.GetRatio();
   IV3 r3 = aLookup3.GetRatio();
   IV4 r4 = aLookup4.GetRatio();
   switch (aInterpolationType)
   {
   case ut::azel::InterpolationType::cLinear:
      return a + r1 * (b + (f * r2) + ((g + l * r2) * r3) + (h * r4)) + r2 * (c + (i * r3) + ((j + m * r1) * r4)) +
             r3 * (d + ((k + n * r1) * r4)) + r4 * (e + ((o + p * r1) * r2 * r3));
   case ut::azel::InterpolationType::cLogarithmic:
   {
      // in the following, we use long double to capture precision of IV1 and IV2
      auto r12   = static_cast<long double>(r1 * r2);
      auto r13   = static_cast<long double>(r1 * r3);
      auto r14   = static_cast<long double>(r1 * r4);
      auto r23   = static_cast<long double>(r2 * r3);
      auto r24   = static_cast<long double>(r2 * r4);
      auto r34   = static_cast<long double>(r3 * r4);
      auto r123  = static_cast<long double>(r12 * r3);
      auto r124  = static_cast<long double>(r12 * r4);
      auto r134  = static_cast<long double>(r13 * r4);
      auto r234  = static_cast<long double>(r23 * r4);
      auto r1234 = static_cast<long double>(r12 * r34);

      auto t1 = pow(f1111, r1234);                                               // f1111 ^ (r1*r2*r3*r4)
      auto t2 = pow(f1110, (r123 - r1234));                                      // f1110 ^ (r1*r2*r3*(1-r4))
      auto t3 = pow(f1101, (r124 - r1234));                                      // f1101 ^ (r1*r2*(1-r3)*r4)
      auto t4 = pow(f1100, (r12 - r123 - r124 + r1234));                         // f1100 ^ (r1*r2*(1-r3)*(1-r4))
      auto t5 = pow(f1011, (r134 - r1234));                                      // f1011 ^ (r1*(1-r2)*r3*r4)
      auto t6 = pow(f1010, (r13 - r123 - r134 + r1234));                         // f1010 ^ (r1*(1-r2)*r3*(1-r4)))
      auto t7 = pow(f1001, (r14 - r124 - r134 + r1234));                         // f1001 ^ (r1*(1-r2)*(1-r3)*r4))
      auto t8 = pow(f1000, (r1 - r12 - r13 - r14 + r123 + r124 + r134 - r1234)); // f1000 ^ (r1*(1-r2)*(1-r3)*(1-r4))

      auto t9  = pow(f0111, (r234 - r1234));                                      // f0111 ^ ((1-r1)*r2*r3*r4)
      auto t10 = pow(f0110, (r23 - r123 - r234 + r1234));                         // f0110 ^ ((1-r1)*r2*r3*(1-r4))
      auto t11 = pow(f0101, (r24 - r124 - r234 + r1234));                         // f0101 ^ ((1-r1)*r2*(1-r3)*r4)
      auto t12 = pow(f0100, (r2 - r12 - r23 - r24 + r123 + r124 + r234 - r1234)); // f0100 ^ ((1-r1)*r2*(1-r3)*(1-r4))
      auto t13 = pow(f0011, (r34 - r134 - r234 + r1234));                         // f0011 ^ ((1-r1)*(1-r2)*r3*r4)
      auto t14 = pow(f0010, (r3 - r13 - r23 - r34 + r123 + r134 + r234 - r1234)); // f0010 ^ ((1-r1)*(1-r2)*r3*(1-r4)))
      auto t15 = pow(f0001, (r4 - r14 - r24 - r34 + r124 + r134 + r234 - r1234)); // f0001 ^ ((1-r1)*(1-r2)*(1-r3)*r4))

      // now compute:  f0000 ^ ((1-r1)*(1-r2)*(1-r3)*(1-r4))
      auto t16 =
         pow(f0000, (1 - r1 - r2 - r3 - r4 + r12 + r13 + r14 + r23 + r24 + r34 - r123 - r124 - r134 - r234 + r1234));

      // now return f111^(r1*r2*r3) + f110^(r1*r2*(1-r3)) + f101^(r1*(1-r2)*r3)
      //          + f100^(r1*(1-r2)*(1-r3)) + f011^((1-r1)*r2*r3) + f010^((1-r1)*r2*(1-r3)))
      //          + f001^((1-r1)*(1-r2)*r3)) + f000^((1-r1)*(1-r2)*(1-r3))
      return static_cast<DV>(t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + t11 + t12 + t13 + t14 + t15 + t16);
   }
   default:
      throw std::invalid_argument("Invalid interpolation type");
   }
}

#endif
