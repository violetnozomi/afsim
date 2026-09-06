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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef UTTABLE_LOOKUP_HPP
#define UTTABLE_LOOKUP_HPP

//! A collection of lookup methods used by UtTable.
#include "ut_export.h"

#include <algorithm>

namespace UtTable
{
class UT_EXPORT LookupState
{
public:
   double GetRatio() const { return mRatio; }
   size_t GetIndex() const { return mIndex; }
   void   Reset() { mIndex = 0; }

   double mRatio{0.0};
   size_t mIndex{0};
};

template<class T>
void LookupLU(T aValue, const T aTable[], size_t aSize, LookupState& aState);

template<class T>
void LookupLUX(T aValue, const T aTable[], size_t aSize, LookupState& aState);

template<class T>
void LookupLUB(T aValue, const T aTable[], size_t aSize, LookupState& aState);

template<class T>
void LookupLUBX(T aValue, const T aTable[], size_t aSize, LookupState& aState);

template<class T>
void LookupLE(T aValue, const T aTable[], size_t aSize, LookupState& aState);

template<class T>
void LookupLEX(T aValue, const T aTable[], size_t aSize, LookupState& aState);
} // namespace UtTable
// ================================================================================================
// ================================================================================================
// Independent variable with unequally spaced values
// ================================================================================================
// ================================================================================================

// ================================================================================================
// Linear interpolation with unevenly-spaced breakpoints.  Values outside the
// range of the breakpoints are clamped to the endpoints.
//
// This version uses a linear lookup starting from the last lookup point.
template<class T>
void UtTable::LookupLU(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   if (aValue <= aTable[0])
   {
      aState.mIndex = 0;
      aState.mRatio = 0.0;
   }
   else if (aValue >= aTable[aSize - 1])
   {
      aState.mIndex = aSize - 2;
      aState.mRatio = 1.0;
   }
   else
   {
      // Continue the search from the last used index.  The flight sim
      // lookup did this and benchmarks seem to bear out that it helps.
      // Also we use a temporary during the lookup.  Some compilers seem
      // to want to keep storing during the iteration.
      size_t index = aState.mIndex;
      while (aValue > aTable[index])
      {
         ++index;
      }
      while (aValue < aTable[index])
      {
         --index;
      }
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
}

// ================================================================================================
//! Linear interpolation with unevenly-spaced breakpoints.  Values outside the
//! range of the breakpoints will be extrapolated.
//!
//! This version uses a linear lookup starting from the last lookup point.
template<class T>
void UtTable::LookupLUX(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   if (aValue <= aTable[0])
   {
      size_t index  = 0;
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
   else if (aValue >= aTable[aSize - 1])
   {
      size_t index  = aSize - 2;
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
   else
   {
      // Continue the search from the last used index.  The flight sim
      // lookup did this and benchmarks seem to bear out that it helps.
      // Also we use a temporary during the lookup.  Some compilers seem
      // to want to keep storing during the iteration.
      size_t index = aState.mIndex;
      while (aValue > aTable[index])
      {
         ++index;
      }
      while (aValue < aTable[index])
      {
         --index;
      }
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
}

// ================================================================================================
//! Linear interpolation with unevenly-spaced breakpoints.  Values outside the
//! range of the breakpoints are clamped to the endpoints.
//!
//! This version utilizes a binary search.  The breakeven point when compared
//! to the linear version is around 64 elements for random inputs.  Your
//! mileage may vary depending on the nature of your inputs.
template<class T>
void UtTable::LookupLUB(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   if (aValue <= aTable[0])
   {
      aState.mIndex = 0;
      aState.mRatio = 0.0;
   }
   else if (aValue >= aTable[aSize - 1])
   {
      aState.mIndex = aSize - 2;
      aState.mRatio = 1.0;
   }
   else
   {
      const T* pos   = std::upper_bound(aTable, aTable + aSize, aValue);
      size_t   index = (pos - aTable) - 1;
      aState.mIndex  = index;
      aState.mRatio  = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
}

// ================================================================================================
//! Linear interpolation with unevenly-spaced breakpoints.  Values outside the
//! range of the breakpoints will be extrapolated.
//!
//! This version utilizes a binary search.  The breakeven point when compared
//! to the linear version is around 64 elements for random inputs.  Your
//! mileage may vary depending on the nature of your inputs.
template<class T>
void UtTable::LookupLUBX(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   if (aValue <= aTable[0])
   {
      size_t index  = 0;
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
   else if (aValue >= aTable[aSize - 1])
   {
      size_t index  = aSize - 2;
      aState.mIndex = index;
      aState.mRatio = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
   else
   {
      const T* pos   = std::upper_bound(aTable, aTable + aSize, aValue);
      size_t   index = (pos - aTable) - 1;
      aState.mIndex  = index;
      aState.mRatio  = (aValue - aTable[index]) / (aTable[index + 1] - aTable[index]);
   }
}

// ================================================================================================
// ================================================================================================
// Independent variable with Equally-space values
// ================================================================================================
// ================================================================================================

// ================================================================================================
//! Linear interpolation with evenly-spaced breakpoints.  Values outside the
//! range of the breakpoints are clamped to the endpoints.
template<class T>
void UtTable::LookupLE(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   double realIndex = (aValue - aTable[0]) / (aTable[1] - aTable[0]);
   if (realIndex <= 0.0)
   {
      aState.mIndex = 0;
      aState.mRatio = 0.0;
   }
   else if (realIndex >= static_cast<double>(aSize - 1))
   {
      aState.mIndex = aSize - 2;
      aState.mRatio = 1.0;
   }
   else
   {
      aState.mIndex = static_cast<size_t>(realIndex);
      aState.mRatio = realIndex - static_cast<double>(aState.mIndex);
   }
}

// ================================================================================================
//! Linear interpolation with evenly-spaced breakpoints.  Values outside the
//! range of the breakpoints will be extrapolated.
template<class T>
void UtTable::LookupLEX(T aValue, const T aTable[], size_t aSize, LookupState& aState)
{
   double realIndex = (aValue - aTable[0]) / (aTable[1] - aTable[0]);
   if (realIndex <= 0.0)
   {
      aState.mIndex = 0;
      aState.mRatio = realIndex;
   }
   else if (realIndex >= static_cast<double>(aSize - 1))
   {
      aState.mIndex = aSize - 2;
      aState.mRatio = realIndex - static_cast<double>(aState.mIndex);
   }
   else
   {
      aState.mIndex = static_cast<size_t>(realIndex);
      aState.mRatio = realIndex - static_cast<double>(aState.mIndex);
   }
}

#endif
