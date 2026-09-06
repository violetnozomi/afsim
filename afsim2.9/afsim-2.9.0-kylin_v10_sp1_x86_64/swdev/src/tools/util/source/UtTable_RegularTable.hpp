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

#ifndef UTTABLE_REGULARTABLE_HPP
#define UTTABLE_REGULARTABLE_HPP

//! An implementation of 'regular sample tables', which represent a traditional table that is an N-dimensional matrix.

#include "ut_export.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

#include "UtInput.hpp"
#include "UtSpan.hpp"
#include "UtTable.hpp"
#include "UtTable_Lookup.hpp"

namespace UtTable
{
namespace RegularTable
{
constexpr size_t cMAX_DIMENSIONS = 8;

UT_EXPORT std::unique_ptr<Table> LoadInstance(UtInput&           aInput,
                                              UtInput::ValueType aDV_UnitType,
                                              const ValueCheck&  aDV_ValueCheck,
                                              const Parameters&  aParameters);

UT_EXPORT std::unique_ptr<Table> LoadCurve(UtInput&           aInput,
                                           UtInput::ValueType aDV_UnitType,
                                           const ValueCheck&  aDV_ValueCheck,
                                           const Parameters&  aParameters);

UT_EXPORT std::unique_ptr<Table> LoadCurve(UtInput&           aInput,
                                           const std::string& aIV_Name,
                                           UtInput::ValueType aIV_UnitType,
                                           const std::string& aIV_Units,
                                           const ValueCheck&  aIV_ValueCheck,
                                           UtInput::ValueType aDV_UnitType,
                                           const std::string& aDV_Units,
                                           const ValueCheck&  aDV_ValueCheck);

enum class IndexScheme
{
   eRectangular = 0,
   eMorton      = 1
};

UT_EXPORT void SaveDvTableToBinaryFile(const std::string&         aPath,
                                       IndexScheme                aSaveAsFormat,
                                       const std::vector<size_t>& aDimensions,
                                       ut::span<const double>     aDvValues); // in c++20 convert to std::span<double>

UT_EXPORT void SaveDvTableToBinaryFile(const std::string&         aPath,
                                       IndexScheme                aSaveAsFormat,
                                       const std::vector<size_t>& aDimensions,
                                       ut::span<const float>      aDvValues); // in c++20 convert to std::span<float>

class UT_EXPORT IV_Table_Base;
template<typename T>
class UT_EXPORT IV_Table;

template<size_t Dimensions>
class RectangularIndexer;

class LinearInterpolator;
class NearestInterpolator;

template<size_t Dimensions, typename T, typename Indexer = RectangularIndexer<Dimensions>, typename Interpolator = LinearInterpolator>
class DV_Table_ND;

template<size_t Dimensions, typename DV, typename Indexer = RectangularIndexer<Dimensions>, typename Interpolator = LinearInterpolator>
class Table_ND;

template<typename T>
class UT_EXPORT DV_Table_1D;
template<typename DV, typename IV1>
class UT_EXPORT Table_1D;

//! Identify the owner of value pointers in independent/dependent variable objects.
//!
//! When an independent/dependent variable object is created, it is generally passed a pointer
//! to the values and a value to indicate who 'owns' the memory (i.e.: who is responsible for
//! deleting it).
enum Owner
{
   //! The memory is owned by the caller.
   //! The class will NOT delete the memory.
   cOWNER_CALLER,

   //! The memory is owned by the class.
   //! This is used when the user has dynamically allocated the array containing the values
   //! and is transferring control of the memory to the table object. The o
   cOWNER_CLASS
};

enum LookupMethod
{
   cLM_LU,   //!< linear iteration (clamped to endpoints)
   cLM_LUX,  //!< linear iteration (exptrapolate past endpoints)
   cLM_LUB,  //!< binary-search lookup (clamped to endpoints)
   cLM_LUBX, //!< binary-search lookup (extrapolate past endpoints)
   cLM_LE,   //!< linear equation, y=m*x+b  (clamped to endpoints)
   cLM_LEX,  //!< linear equation, y=m*x+b (exptrapolat past endpoints)
   cLM_DEFAULT
};

//! tag for IV_Table to select the specialization that implements a linear line lookup: y = m*x + b
//! This allows for a large IV_Table of equally spaced N points, without using N memory.
// additional tag types could easily be added to support other common IV data samples, such as logarithmic or polymorphic.
struct UT_EXPORT LinearTag;

namespace detail
{
class UT_EXPORT IV_Holder;

//! An index sequence structure.
template<size_t...>
struct index_sequence
{
};

template<size_t N, size_t... IntSeq>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, IntSeq...>
{
};

template<size_t... IntSeq>
struct make_index_sequence<0, IntSeq...> : index_sequence<IntSeq...>
{
};
} // end namespace detail

} // end namespace RegularTable
} // end namespace UtTable

// ================================================================================================
class UtTable::RegularTable::IV_Table_Base
{
public:
   // even though all derivatives of IV_Table share common data, as a micro-optimization
   // to ensure static downcasting is a no-op, this class is kept empty. It's important
   // that table lookup are extremely efficient, and something as simple as the static
   // downcast causing a pointer shift by a constant was deemed too much.
   // Yes this is premature, and if you want to fix it, go ahead.

   virtual ~IV_Table_Base() = default;

   virtual size_t Size() const = 0;

   virtual const std::string& GetName() const = 0;

   virtual void GetVariable(std::vector<double>& aValues) const = 0;

   virtual void GetVariableLimits(double& aMinValue, double& aMaxValue) const = 0;
};

//! A template class that defines the implementation of an independent variable for a specific numeric type.
template<class T>
class UtTable::RegularTable::IV_Table : public UtTable::RegularTable::IV_Table_Base
{
public:
   IV_Table(const T* aTablePtr, size_t aSize, Owner aOwner, std::string aName)
      : mTable(aTablePtr)
      , mSize(aSize)
      , mOwner(aOwner)
      , mName(std::move(aName))
   {
   }

   virtual ~IV_Table()
   {
      if (mOwner == cOWNER_CLASS)
      {
         delete[] mTable;
      }
   }

   void Lookup(double aIV_Value, LookupState& aState) const
   {
      T value = static_cast<T>(aIV_Value);
      switch (mLookupMethod)
      {
      case cLM_LU:
      case cLM_DEFAULT:
         LookupLU(value, mTable, mSize, aState);
         break;
      case cLM_LUX:
         LookupLUX(value, mTable, mSize, aState);
         break;
      case cLM_LUB:
         LookupLUB(value, mTable, mSize, aState);
         break;
      case cLM_LUBX:
         LookupLUBX(value, mTable, mSize, aState);
         break;
      case cLM_LE:
         LookupLE(value, mTable, mSize, aState);
         break;
      case cLM_LEX:
         LookupLEX(value, mTable, mSize, aState);
         break;
      }
   }

   void SelectLookupMethod(bool aAllowExtrapolation)
   {
      // First determine if the values are evenly spaced. Each value must be at least within
      // 0.001 of the same difference, or  1/100 th of the distance between each element.
      // (Whichever is smaller).
      T    closeEnough = std::min((T).001, (T)(mTable[mSize - 1] - mTable[0]) / (T)(mSize * 100.0));
      bool isEven      = true;
      T    diff        = (mTable[1] - mTable[0]);
      for (size_t i = 2; i < mSize; ++i)
      {
         if (std::abs(mTable[i] - mTable[i - 1] - diff) > closeEnough)
         {
            isEven = false;
            break;
         }
      }

      if (isEven)
      {
         // Equally spaced values
         mLookupMethod = (aAllowExtrapolation ? cLM_LEX : cLM_LE);
      }
      else if (mSize < 64)
      {
         // Unequally spaced values - less than 64 values.
         mLookupMethod = (aAllowExtrapolation ? cLM_LUX : cLM_LU);
      }
      else
      {
         // Unequally spaced values with 64 or greater values. Use binary search.
         mLookupMethod = (aAllowExtrapolation ? cLM_LUBX : cLM_LUB);
      }
   }

   LookupMethod GetLookupMethod() const { return mLookupMethod; }

   size_t Size() const override { return mSize; }

   const std::string& GetName() const override { return mName; }

   T Get(size_t aIndex1) const { return mTable[aIndex1]; }

   void GetVariable(std::vector<double>& aValues) const override
   {
      aValues.resize(mSize);
      for (size_t i = 0; i < mSize; ++i)
      {
         aValues[i] = mTable[i];
      }
   }

   void GetVariableLimits(double& aMinValue, double& aMaxValue) const override
   {
      aMinValue = mTable[0];
      aMaxValue = mTable[mSize - 1];
   }

protected:
   //! Pointer to the independent variable table.
   const T* mTable;

   //! The number of elements in the independent variable table.
   size_t mSize;

   //! The owner of the memory pointed to by mArrayPtr.
   Owner mOwner;

   //! The name of the independent variable represented by this data.
   std::string mName;

   //! The algorithm to be used for the table lookup.
   LookupMethod mLookupMethod{cLM_DEFAULT};
};

namespace UtTable
{
namespace RegularTable
{

template<>
class IV_Table<::UtTable::RegularTable::LinearTag> : public ::UtTable::RegularTable::IV_Table_Base
{
public:
   //! @param aStartY inclusive start of line
   //! @param aEndY inclusive end of line
   //! @param aNumPoints number of points on the line
   //! @param aName name of the independent_variable
   IV_Table(double aStartY, double aEndY, size_t aNumPoints, const std::string& aName)
      : mYIntercept(aStartY)
      , mInvSlope((aNumPoints - 1) / (aEndY - aStartY))
      , mSize(aNumPoints)
      , mEndY(aEndY)
      , mName(aName)
   {
   }

   // linear evenly spaced interpolation, that optionally handles extrapolation.
   void Lookup(double aIV_Value, LookupState& aState) const
   {
      double realIndex = (aIV_Value - mYIntercept) * mInvSlope;
      if (realIndex <= 0.0)
      {
         aState.mIndex = 0;
         aState.mRatio = mAllowExtrapolation ? realIndex : 0.0;
      }
      else if (realIndex >= static_cast<double>(mSize - 1))
      {
         aState.mIndex = mSize - 2;
         aState.mRatio = mAllowExtrapolation ? (realIndex - static_cast<double>(aState.mIndex)) : 1.0;
      }
      else
      {
         aState.mIndex = static_cast<size_t>(realIndex);
         aState.mRatio = realIndex - static_cast<double>(aState.mIndex);
      }
   }

   size_t Size() const override { return mSize; }

   const std::string& GetName() const override { return mName; }

   void GetVariable(std::vector<double>& aValues) const override
   {
      aValues.resize(mSize);
      double slope = (mEndY - mYIntercept) / (mSize - 1);
      for (size_t i = 0; i < mSize; ++i)
      {
         aValues[i] = mYIntercept + slope * i;
      }
   }

   void GetVariableLimits(double& aMinValue, double& aMaxValue) const override
   {
      aMinValue = mYIntercept;
      aMaxValue = mEndY;
   }

   void SelectLookupMethod(bool aAllowExtrapolation) { mAllowExtrapolation = aAllowExtrapolation; }

protected:
   double mYIntercept;
   double mInvSlope; ///< 1 / slope
   //! The number of elements in the independent variable table.
   size_t mSize;
   double mEndY;
   bool   mAllowExtrapolation{false};
   //! The name of the independent variable represented by this data.
   std::string mName;
};

} // end namespace RegularTable
} // end namespace UtTable

// ================================================================================================

template<typename T>
class UtTable::RegularTable::DV_Table_1D
{
public:
   DV_Table_1D(const T* aArrayPtr, size_t aDim1, Owner aOwner)
      : mTable(aArrayPtr)
      , mDim1(aDim1)
      , mOwner(aOwner)
   {
   }

   virtual ~DV_Table_1D()
   {
      if (mOwner == cOWNER_CLASS)
      {
         delete[] mTable;
      }
   }

   size_t Size() const { return mDim1; }

   T Get(size_t aIndex1) const { return mTable[aIndex1]; }

   double Lookup(const LookupState& aIV_State1) const
   {
      size_t i1 = aIV_State1.GetIndex();
      T      f0 = Get(i1);
      T      f1 = Get(i1 + 1);
      double r1 = aIV_State1.GetRatio();
      return f0 + r1 * (f1 - f0);
   }

   void GetVariableLimits(double& aMinValue, double& aMaxValue) const
   {
      size_t size     = mDim1;
      T      minValue = mTable[0];
      T      maxValue = mTable[0];
      for (size_t i = 1; i < size; ++i)
      {
         aMinValue = std::min(minValue, mTable[i]);
         aMaxValue = std::max(maxValue, mTable[i]);
      }
      aMinValue = minValue;
      aMaxValue = maxValue;
   }

   void GetVariable(std::vector<double>& aValues) const
   {
      size_t size = mDim1;
      aValues.resize(size);
      for (size_t i = 0; i < size; ++i)
      {
         aValues[i] = mTable[i];
      }
   }

private:
   const T* mTable;
   size_t   mDim1;
   Owner    mOwner;
};

// ================================================================================================

template<typename DV, typename IV1>
class UtTable::RegularTable::Table_1D : public UtTable::Table
{
public:
   using DV_Type  = typename UtTable::RegularTable::DV_Table_1D<DV>;
   using IV1_Type = typename UtTable::RegularTable::IV_Table<IV1>;

   Table_1D(std::shared_ptr<DV_Type> aDV_Ptr, std::shared_ptr<IV1_Type> aIV1_Ptr)
      : mDV_Ptr(std::move(aDV_Ptr))
      , mIV1_Ptr(std::move(aIV1_Ptr))
   {
      mIV1_DataIndex = 0;
   }

   Table* Clone() const override { return new Table_1D(*this); }

   bool ResolveReferences(const Parameters& aParameters) override
   {
      bool     ok = true;
      DataType dataType;
      ok &= aParameters.GetTypeAndIndex(mIV1_Ptr->GetName(), dataType, mIV1_DataIndex);
      return ok;
   }

   double Lookup(const double* aArguments) const override
   {
      LookupState IV1_State;
      // since mPreviousIndex is only used as a hint,
      // its fine for multi-threaded race conditions to occur
      // when reading/writing mPreviousIndex.
      IV1_State.mIndex = mPreviousIndex;
      mIV1_Ptr->Lookup(aArguments[mIV1_DataIndex], IV1_State);
      mPreviousIndex = IV1_State.mIndex;
      return mDV_Ptr->Lookup(IV1_State);
   }

   bool VariableExists(const std::string& aName) const override { return (aName == mIV1_Ptr->GetName()); }

   void GetVariableNames(std::vector<std::string>& aNames) const override
   {
      aNames.clear();
      aNames.resize(1);
      aNames[0] = mIV1_Ptr->GetName();
   }

   bool GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const override
   {
      bool ok = true;
      if (aName.empty()) // Get limits for dependent variable
      {
         mDV_Ptr->GetVariableLimits(aMinValue, aMaxValue);
      }
      else if (aName == mIV1_Ptr->GetName())
      {
         mIV1_Ptr->GetVariableLimits(aMinValue, aMaxValue);
      }
      else
      {
         ok = false;
      }
      return ok;
   }

   bool GetVariable(const std::string& aName, std::vector<double>& aValues) const override
   {
      bool ok = true;
      if (aName.empty()) // Get values for dependent variable
      {
         mDV_Ptr->GetVariable(aValues);
      }
      // As a special case with define the name "*" to match any independent variable.
      // The original form of UtTable::LoadCurve allowed creating a table that did not have the name of the independent variable.
      else if ((aName == mIV1_Ptr->GetName()) || (aName == "*"))
      {
         mIV1_Ptr->GetVariable(aValues);
      }
      else
      {
         aValues.clear();
         ok = false;
      }
      return ok;
   }

private:
   std::shared_ptr<DV_Type>  mDV_Ptr;
   std::shared_ptr<IV1_Type> mIV1_Ptr;
   size_t                    mIV1_DataIndex;

   /// cache of the last found lookup indexs, as benchmarks
   /// showed that performing linear lookups starting at
   /// the previous found index helps performance.
   /// Since this is only used as hint, it's safe to have
   /// read/write thread races on it, which is why this is mutable.
   /// each cloned instance of the table gets its own copy of a cached previous index.
   mutable size_t mPreviousIndex{0};
};

// ================================================================================================

/// standard rectangular indexing scheme.
/// This is the same indexing scheme that array3D[x][y][z] would normally use.
template<size_t Dimensions>
class UtTable::RegularTable::RectangularIndexer
{
public:
   RectangularIndexer(const std::array<size_t, Dimensions>& aIvSizes)
   {
      if (Dimensions > 1)
      {
         mDimsAccumed.back() = aIvSizes.back();
         for (std::ptrdiff_t i = mDimsAccumed.size() - 2; i >= 0; --i)
         {
            mDimsAccumed[i] = mDimsAccumed[i + 1] * aIvSizes[i + 1];
         }
      }
   }

   // convert D dimensioned index to the 1D index value
   size_t Index(const std::array<size_t, Dimensions>& aIndexs) const
   {
      size_t index = aIndexs.back();
      for (size_t i = 0; i < mDimsAccumed.size(); ++i)
      {
         index += aIndexs[i] * mDimsAccumed[i];
      }
      return index;
   }

private:
   std::array<size_t, Dimensions - 1> mDimsAccumed;
};

class UtTable::RegularTable::LinearInterpolator
{
public:
   //! generic N-Dimensional linear interpolation logic.
   template<typename T, class GetterFunc, size_t Dimensions, size_t... I>
   static double Interpolate(const GetterFunc&                          aGet,
                             const std::array<LookupState, Dimensions>& aIV_States,
                             detail::index_sequence<I...>)
   {
      constexpr size_t NumPoints = 1ULL << Dimensions;
      T                f[NumPoints];

      // permute each corner of the interpolation location
      for (size_t x = 0; x < NumPoints; ++x)
      {
         f[x] = aGet({(aIV_States[I].GetIndex() + ((x & (1ULL << (Dimensions - I - 1))) >> (Dimensions - I - 1)))...});
      }

      // linear interpolate across each dimension
      for (size_t level = 0; level < Dimensions; ++level)
      {
         const size_t halfSize   = 1ULL << (Dimensions - level - 1);
         T            ratio      = static_cast<T>(aIV_States[level].GetRatio());
         T            ratioCompl = 1 - ratio;
         for (size_t i = 0; i < halfSize; ++i)
         {
            f[i] = f[i] * ratioCompl + ratio * f[i + halfSize];
         }
      }
      return f[0];
   }

   //! hand-optimized specialization for 1D
   template<typename T, class GetterFunc>
   static double Interpolate(const GetterFunc& aGet, const std::array<LookupState, 1>& aIV_States, detail::index_sequence<0>)
   {
      size_t i1 = aIV_States[0].GetIndex();
      T      f0 = aGet({i1});
      T      f1 = aGet({i1 + 1});
      double r1 = aIV_States[0].GetRatio();
      return f0 + r1 * (f1 - f0);
   }

   //! hand-optimized specialization for 2D
   template<typename T, class GetterFunc>
   static double Interpolate(const GetterFunc&                 aGet,
                             const std::array<LookupState, 2>& aIV_States,
                             detail::index_sequence<0, 1>)
   {
      size_t i1  = aIV_States[0].GetIndex();
      size_t i2  = aIV_States[1].GetIndex();
      T      f00 = aGet({i1, i2});
      T      f01 = aGet({i1, i2 + 1});
      T      f10 = aGet({i1 + 1, i2});
      T      f11 = aGet({i1 + 1, i2 + 1});
      // T  a   = f00;
      // T  b   = f10 - f00;
      // T  c   = f01 - f00;
      // T  d   = f11 - f10 - f01 + f00;
      double r1 = aIV_States[0].GetRatio();
      double r2 = aIV_States[1].GetRatio();
      return f00 + ((f10 - f00) * r1) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * r1) * r2;
      // return a + (b * r1) + (c * r2) + (d * r1 * r2);
   }

   //! hand-optimized specialization for 3D
   template<typename T, class GetterFunc>
   static double Interpolate(const GetterFunc&                 aGet,
                             const std::array<LookupState, 3>& aIV_States,
                             detail::index_sequence<0, 1, 2>)
   {
      size_t i1   = aIV_States[0].GetIndex();
      size_t i2   = aIV_States[1].GetIndex();
      size_t i3   = aIV_States[2].GetIndex();
      T      f000 = aGet({i1, i2, i3});
      T      f001 = aGet({i1, i2, i3 + 1});
      T      f010 = aGet({i1, i2 + 1, i3});
      T      f011 = aGet({i1, i2 + 1, i3 + 1});
      T      f100 = aGet({i1 + 1, i2, i3});
      T      f101 = aGet({i1 + 1, i2, i3 + 1});
      T      f110 = aGet({i1 + 1, i2 + 1, i3});
      T      f111 = aGet({i1 + 1, i2 + 1, i3 + 1});
      T      a    = f000;
      T      b    = f100 - f000;
      T      c    = f010 - f000;
      T      d    = f001 - f000;
      T      e    = f110 - a - b - c;
      T      f    = f101 - a - b - d;
      T      g    = f011 - a - c - d;
      T      h    = f111 - a - b - c - d - e - f - g;
      double r1   = aIV_States[0].GetRatio();
      double r2   = aIV_States[1].GetRatio();
      double r3   = aIV_States[2].GetRatio();
      return a + (r1 * (b + e * r2)) + (r2 * (c + (g + h * r1) * r3)) + (r3 * (d + f * r1));
      // return a + (b * r1) + (c * r2) + (d * r3) + (e * r1 * r2) + (f * r1 * r3) + (g * r2 * r3) + (h * r1 * r2 * r3);
   }

   //! hand-optimized specialization for 4D
   template<typename T, class GetterFunc>
   static double Interpolate(const GetterFunc&                 aGet,
                             const std::array<LookupState, 4>& aIV_States,
                             detail::index_sequence<0, 1, 2, 3>)
   {
      size_t i1    = aIV_States[0].GetIndex();
      size_t i2    = aIV_States[1].GetIndex();
      size_t i3    = aIV_States[2].GetIndex();
      size_t i4    = aIV_States[3].GetIndex();
      T      f0000 = aGet({i1, i2, i3, i4});
      T      f0001 = aGet({i1, i2, i3, i4 + 1});
      T      f1000 = aGet({i1 + 1, i2, i3, i4});
      T      f1001 = aGet({i1 + 1, i2, i3, i4 + 1});
      T      f0100 = aGet({i1, i2 + 1, i3, i4});
      T      f0101 = aGet({i1, i2 + 1, i3, i4 + 1});
      T      f0010 = aGet({i1, i2, i3 + 1, i4});
      T      f0011 = aGet({i1, i2, i3 + 1, i4 + 1});
      T      f1100 = aGet({i1 + 1, i2 + 1, i3, i4});
      T      f1101 = aGet({i1 + 1, i2 + 1, i3, i4 + 1});
      T      f1010 = aGet({i1 + 1, i2, i3 + 1, i4});
      T      f1011 = aGet({i1 + 1, i2, i3 + 1, i4 + 1});
      T      f0110 = aGet({i1, i2 + 1, i3 + 1, i4});
      T      f0111 = aGet({i1, i2 + 1, i3 + 1, i4 + 1});
      T      f1110 = aGet({i1 + 1, i2 + 1, i3 + 1, i4});
      T      f1111 = aGet({i1 + 1, i2 + 1, i3 + 1, i4 + 1});
      T      a     = f0000;
      T      b     = f1000 - f0000;
      T      f     = f1100 - f0100 - b; // + f0000 - f1000;
      T      g     = f1010 - f0010 - b; // + f0000 - f1000;
      T      h     = f1001 - f0001 - b; // + f0000 - f1000;

      T c = f0100 - f0000;
      T i = f0110 - f0010 - c; // + f0000 - f0100;
      T j = f0101 - f0001 - c; // + f0000 - f0100;

      T d = f0010 - f0000;
      T k = f0011 - f0001 - d; // + f0000 - f0010;

      T e = f0001 - f0000;
      // T      f = f1100 - f1000 - f0100 + f0000;
      // T      g = f1010 - f1000 - f0010 + f0000;
      // T      h = f1001 - f1000 - f0001 + f0000;
      // T      i = f0110 - f0100 - f0010 + f0000;
      // T      j = f0101 - f0100 - f0001 + f0000;
      // T      k = f0011 - f0010 - f0001 + f0000;
      T l = -f0000 + f1000 + f0100 - f1100 + f0010 - f1010 - f0110 + f1110;
      T m = f0100 - f1100 - f0000 + f1000 + f0001 - f1001 - f0101 + f1101;
      T n = f0010 - f1010 - f0000 + f1000 + f0001 - f1001 - f0011 + f1011;
      T o = -f0110 - f0000 + f0100 + f0001 - f0101 + f0010 - f0011 + f0111;
      T p = f0000 - f1000 - f0100 + f1100 - f0010 + f1010 + f0110 - f1110 - f0001 + f1001 + f0101 - f1101 + f0011 -
            f1011 - f0111 + f1111;
      double r1 = aIV_States[0].GetRatio();
      double r2 = aIV_States[1].GetRatio();
      double r3 = aIV_States[2].GetRatio();
      double r4 = aIV_States[3].GetRatio();
      double t1 = r1 * (b + (f * r2) + ((g + l * r2) * r3) + (h * r4)) + r2 * (c + (i * r3) + ((j + m * r1) * r4)) +
                  r3 * (d + ((k + n * r1) * r4)) + r4 * (e + ((o + p * r1) * r2 * r3));
      return a + t1;
      // return a + (b * r1) + (c * r2) + (d * r3) + (e * r4) +
      //        (f * r1 * r2) + (g * r1 * r3) + (h * r1 * r4) + (i * r2 * r3) + (j * r2 * r4) + (k * r3 * r4) +
      //        (l * r1 * r2 * r3) + (m * r1 * r2 * r4) + (n * r1 * r3 * r4) + (o * r2 * r3 * r4) +
      //        (p * r1 * r2 * r3 * r4)
   }
};

class UtTable::RegularTable::NearestInterpolator
{
public:
   //! returns the nearest corner.
   template<typename T, class GetterFunc, size_t Dimensions, size_t... I>
   static double Interpolate(const GetterFunc&                          aGet,
                             const std::array<LookupState, Dimensions>& aIV_States,
                             detail::index_sequence<I...>)
   {
      return aGet({(aIV_States[I].GetIndex() + static_cast<size_t>(aIV_States[I].GetRatio() + 0.5))...});
   }
};

//! Multi-dimensional dependent_variable table.
//! @tparam cDIMENSIONS number of dimension of the the table.
//! @tparam T type of underlying array.
//! @tparam Indexer scheme used to convert the multidimensional indexs
//!         to the 1D array index in the underlying array.
template<size_t cDIMENSIONS, typename T, typename Indexer, typename Interpolator>
class UtTable::RegularTable::DV_Table_ND
{
public:
   DV_Table_ND(const T* aArrayPtr, const std::array<size_t, cDIMENSIONS>& aDims, Owner aOwner)
      : mArrayPtr(aArrayPtr)
      , mDims(aDims)
      , mIndexer(aDims)
      , mOwner(aOwner)
   {
   }

   virtual ~DV_Table_ND()
   {
      if (mOwner == cOWNER_CLASS)
      {
         delete[] mArrayPtr;
      }
   }

   T Get(const std::array<size_t, cDIMENSIONS>& aIndexs) const { return mArrayPtr[mIndexer.Index(aIndexs)]; }

   double Lookup(const std::array<LookupState, cDIMENSIONS>& aIV_States) const
   {
      return Interpolator::template Interpolate<T>([this](const std::array<size_t, cDIMENSIONS>& aIndexs)
                                                   { return Get(aIndexs); },
                                                   aIV_States,
                                                   detail::make_index_sequence<cDIMENSIONS>{});
   }

   void GetVariableLimits(double& aMinValue, double& aMaxValue) const
   {
      if (!mMinMaxValid)
      {
         // local min/max to avoid race-conditions
         T minVal = mArrayPtr[0];
         T maxVal = mArrayPtr[0];

         ForEachValue(
            [&](T aValue)
            {
               minVal = std::min(minVal, aValue);
               maxVal = std::max(maxVal, aValue);
            });

         // assign final min/max to cached members.
         // This assignment is safe to be lock-free, since the computed
         // min/max will always be the same regardless of which thread computed it.
         mMinValue = minVal;
         mMaxValue = maxVal;

         // prevent the compiler or cpu from re-ordering the write to valid
         // to be before the writes to min/max. This is a no-op on X86 platforms.
         std::atomic_thread_fence(std::memory_order_release);

         mMinMaxValid = true;
      }
      aMinValue = mMinValue;
      aMaxValue = mMaxValue;
   }

   void GetVariable(std::vector<double>& aValues) const
   {
      size_t size = Size();
      aValues.resize(size);

      size_t i = 0;
      ForEachValue(
         [&](T aVal)
         {
            aValues[i] = aVal;
            ++i;
         });
   }

   size_t Size() const
   {
      size_t size = 1;
      for (auto dim : mDims)
      {
         size *= dim;
      }
      return size;
   }

private:
   template<class Lambda>
   ut::enable_if_t<std::is_same<Lambda, Lambda>::value && std::is_same<Indexer, RectangularIndexer<cDIMENSIONS>>::value>
   ForEachValue(const Lambda& aFunc) const
   {
      // when using rectangular indexing, data is contiguous, so simply
      // iterate directly over it.
      size_t size = Size();
      for (size_t i = 0; i < size; ++i)
      {
         aFunc(mArrayPtr[i]);
      }
   }

   template<class Lambda>
   ut::enable_if_t<std::is_same<Lambda, Lambda>::value && !std::is_same<Indexer, RectangularIndexer<cDIMENSIONS>>::value>
   ForEachValue(const Lambda& aFunc) const
   {
      // not using the rectangular indexer, which may mean the data isn't contiguos,
      // so we will need to call Get with each IV index
      std::array<size_t, cDIMENSIONS> indexs;
      ForEachValueHelper(aFunc, 0, indexs);
   }

   template<class Lambda>
   void ForEachValueHelper(const Lambda& aFunc, size_t aOnDim, std::array<size_t, cDIMENSIONS>& aIndex) const
   {
      if (aOnDim == cDIMENSIONS)
      {
         aFunc(Get(aIndex));
         return;
      }

      for (size_t i = 0; i < mDims[aOnDim]; ++i)
      {
         aIndex[aOnDim] = i;
         ForEachValueHelper(aFunc, aOnDim + 1, aIndex);
      }
   }

   const T*                        mArrayPtr;
   std::array<size_t, cDIMENSIONS> mDims;
   Indexer                         mIndexer;
   Owner                           mOwner;

protected:
   // Cached results of GetVariableLimits() to avoid re-reading the entire table for each call.
   // These are safe to be mutable since any multithreaded write races will always
   // be writing the same value, so it wouldn't matter which thread in the write
   // race wins.
   mutable bool mMinMaxValid{false};
   mutable T    mMinValue;
   mutable T    mMaxValue;
};

// ================================================================================================

class UT_EXPORT UtTable::RegularTable::detail::IV_Holder
{
public:
   template<class IVTable>
   IV_Holder(std::unique_ptr<IVTable>&& aTable)
      : mLookupFunc(GetLookupFunc<IVTable>())
      , mTable(std::move(aTable))
   {
   }

   // uses devirtualization technique to inline the call to the IV_Table's lookup
   // function without incurring a virtual dispatch. This is done to improve the
   // runtime of lookups, while still allowing type-erasure of the IV_Table.
   // Past RegularTable designs had the IV_Table types as individual template paramaters
   // to the Table, which while allowing perfect inlining of the Lookup call, caused an
   // exponential explosion in compile times as each permutation of IV types had to be
   // stamped for each Dimension.
   void Lookup(const double* aArguments, LookupState& aIV_State) const
   {
      (*mLookupFunc)(*mTable, aArguments[mDataIndex], aIV_State);
   }

   size_t&              GetDataIndex() { return mDataIndex; }
   const IV_Table_Base& GetIV_Table() const { return *mTable; }

private:
   using IvLookupFuncPtr = void (*)(const IV_Table_Base&, double aIV_Value, LookupState& aState);

   // creates a function pointer that calls the IV_Tables Lookup directly instead of using
   // virtual dispatch.
   template<class IVTable>
   IvLookupFuncPtr GetLookupFunc()
   {
      return [](const IV_Table_Base& aBase, double aIV_Value, LookupState& aState)
      {
         // The design of IV_Holder ensures this downcast is safe.
         // The whole reason IV_Holder exists is to perform this optimization,
         // while still allowing type-erasure of the IVTable type.
         // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
         static_cast<const IVTable&>(aBase).Lookup(aIV_Value, aState);
      };
   }

   IvLookupFuncPtr                mLookupFunc;
   std::unique_ptr<IV_Table_Base> mTable;
   size_t                         mDataIndex{0};
};

// ================================================================================================

//! Multi-dimensional regular table.
//! @tparam cDIMENSIONS - Number of dimension of the table.
//! @tparam DV         - Underlying data type used by the dependent_variable.
//! @tparam Indexer    - Indexing scheme used by the dependent_variable for converting
//!                      multidimensional indexs to the 1D array index of the underlying array.
//! @tparam Interpolator - The interpolator method for looking up data in the DV table.
template<size_t cDIMENSIONS, typename DV, typename Indexer, typename Interpolator>
class UtTable::RegularTable::Table_ND : public UtTable::Table
{
private:
   struct Data;

public:
   using DV_Type  = std::unique_ptr<UtTable::RegularTable::DV_Table_ND<cDIMENSIONS, DV, Indexer, Interpolator>>;
   using IV_Array = std::array<UtTable::RegularTable::detail::IV_Holder, cDIMENSIONS>;

   Table_ND(DV_Type&& aDV, IV_Array&& aIVs)
      : mData(std::make_shared<Data>(std::move(aDV), std::move(aIVs)))
   {
   }

   Table* Clone() const override { return new Table_ND(*this); }

   bool ResolveReferences(const Parameters& aParameters) override
   {
      bool     ok = true;
      DataType dataType;
      for (auto& iv : mData->mIVs)
      {
         ok &= aParameters.GetTypeAndIndex(iv.GetIV_Table().GetName(), dataType, iv.GetDataIndex());
      }
      return ok;
   }

   double Lookup(const double* aArguments) const override
   {
      std::array<LookupState, cDIMENSIONS> ivStates;
      for (size_t i = 0; i < cDIMENSIONS; ++i)
      {
         // since mCachedIndexs is only used as a hint,
         // its fine for multi-threaded race conditions to occur
         // when reading/writing mCachedIndexs.
         ivStates[i].mIndex = mCachedIndexs[i];
      }

      auto& data = *mData;
      for (size_t i = 0; i < cDIMENSIONS; ++i)
      {
         data.mIVs[i].Lookup(aArguments, ivStates[i]);
      }
      double retVal = data.mDV->Lookup(ivStates);

      for (size_t i = 0; i < cDIMENSIONS; ++i)
      {
         mCachedIndexs[i] = ivStates[i].mIndex;
      }
      return retVal;
   }

   bool VariableExists(const std::string& aName) const override
   {
      for (const auto& iv : mData->mIVs)
      {
         if (aName == iv.GetIV_Table().GetName())
         {
            return true;
         }
      }
      return false;
   }

   void GetVariableNames(std::vector<std::string>& aNames) const override
   {
      aNames.clear();
      aNames.reserve(cDIMENSIONS);
      for (const auto& iv : mData->mIVs)
      {
         aNames.push_back(iv.GetIV_Table().GetName());
      }
   }

   bool GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const override
   {
      if (aName.empty()) // Get limits for dependent variable
      {
         mData->mDV->GetVariableLimits(aMinValue, aMaxValue);
         return true;
      }
      for (const auto& iv : mData->mIVs)
      {
         if (aName == iv.GetIV_Table().GetName())
         {
            iv.GetIV_Table().GetVariableLimits(aMinValue, aMaxValue);
            return true;
         }
      }
      return false;
   }

   bool GetVariable(const std::string& aName, std::vector<double>& aValues) const override
   {
      if (aName.empty()) // Get values for dependent variable
      {
         mData->mDV->GetVariable(aValues);
         return true;
      }
      for (const auto& iv : mData->mIVs)
      {
         if (aName == iv.GetIV_Table().GetName())
         {
            iv.GetIV_Table().GetVariable(aValues);
            return true;
         }
      }
      return false;
   }

private:
   struct Data
   {
      Data(DV_Type&& aDV, IV_Array&& aIVs)
         : mDV(std::move(aDV))
         , mIVs(std::move(aIVs))
      {
      }

      DV_Type  mDV;
      IV_Array mIVs;
   };
   /// store the data in a shared_ptr because UtTables are immutable so the backing data
   /// does not need to be deep copied during a clone/copy.
   std::shared_ptr<Data> mData;

   /// cache of the last found lookup indexs, as benchmarks
   /// showed that performing linear lookups starting at
   /// the previous found index helps performance.
   /// Since this is only used as hint, it's safe to have
   /// read/write thread races on it, which is why this is mutable.
   /// each cloned instance of the table gets its own copy of a cached indexs array
   mutable std::array<size_t, cDIMENSIONS> mCachedIndexs{};
};


#endif
