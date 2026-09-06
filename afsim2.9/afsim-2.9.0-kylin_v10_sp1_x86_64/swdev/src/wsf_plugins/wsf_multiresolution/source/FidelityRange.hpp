// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIDELITY_RANGE_HPP
#define FIDELITY_RANGE_HPP

#include <cassert>

class UtInput;

namespace wsf
{
namespace multiresolution
{
//! @brief A range of fidelity values defined by a lower bound and upper bound
//! @pre It is required that the lower and upper bounds are in ascending order, even if not currently enforced.
struct FidelityRange
{
   //! @brief Constructs a FidelityRange with the default range of [0, 1].
   //! @note This can be removed in C++14.
   constexpr FidelityRange() = default;
   //! @brief Constructs a FidelityRange with the range @a lowerBound to @a upperBound.
   //! @pre @a lowerBound must be less than @a upperBound.
   //! @note This is defined so that in-class initializers can be used. This can be removed in C++14.
   constexpr FidelityRange(const double lowerBound, const double upperBound)
      : mLowerBound(lowerBound)
      , mUpperBound(upperBound)
   {
   }

   //! @return `true` if @a aValue falls within the lower and upper bounds, inclusive of the lower bound and
   //!  exclusive of the upper bound.
   //! @note Special handling is used for the case when @a aValue and the upper bound are both 1. Returns true
   //!  in this case.
   bool Contains(const double aValue) const
   {
      return (aValue >= mLowerBound && aValue < mUpperBound) || (aValue == 1.0 && mUpperBound == 1.0);
   }
   //! @return `true` if @a rhs does not overlap with this object. The two ranges are considered disjoint if
   //! one has a lower bound equal to the other's upper bound.
   bool IsDisjoint(const FidelityRange& rhs) const
   {
      return (rhs.mLowerBound < mLowerBound && rhs.mUpperBound <= mLowerBound) ||
             (rhs.mLowerBound >= mUpperBound && rhs.mUpperBound > mUpperBound);
   }
   //! @return `true` if the ranges defined in @a rhs and this object overlap. The two ranges do not overlap if
   //! one has a lower bound equal to the other's upper bound.
   bool Overlaps(const FidelityRange& rhs) const { return !IsDisjoint(rhs); }

   double mLowerBound = 0.0;
   double mUpperBound = 1.0;
};

//! @brief Reads a fidelity range, which is two values in [0, 1], that are sorted in increasing order.
//! @throw Ut::BadValue Throws for either of the following reasons: Either value is not in [0, 1] or the lower
//!  bound is greater than the upper bound.
FidelityRange ProcessFidelityRange(UtInput& aInput);

} // namespace multiresolution
} // namespace wsf

#endif
