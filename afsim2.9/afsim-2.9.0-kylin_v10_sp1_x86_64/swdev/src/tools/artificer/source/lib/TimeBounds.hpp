// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef TIMEBOUNDS_HPP
#define TIMEBOUNDS_HPP

#include "artificer_export.h"

#include <algorithm>
#include <vector>

#include <UtOptional.hpp>

namespace artificer
{
///@brief Represents the upper and lower bounds of a value, or in other words a line segment along an axis.
///@note TimeBounds are always contiguous.  They may contain 0, 1 or infinity values.
struct ARTIFICER_EXPORT TimeBounds
{
   /// @brief Initialize bounds to contain no values
   TimeBounds() = default;
   /// @brief Initialize bounds to contain a range of values.  Argument order doesn't matter.
   TimeBounds(const int64_t aValue1, const int64_t aValue2);

   /// @brief Updates the bounds to include the given value if not already contained.
   TimeBounds& Union(int64_t aValue);

   /// @brief Updates the bounds to include the given range if it exists and is not already contained.
   /// @note The resulting range is contiguous - any space between the two ranges is added!
   TimeBounds& Union(const TimeBounds& aOther);

   /// @brief Updates the bounds to contain the range contained by both this and `aOther`.
   TimeBounds& Intersection(const TimeBounds& aOther);

   /// @brief Returns true if this range and the other range overlap.  Edges inclusive.
   bool Intersects(const TimeBounds& aOther) const
   {
      return HasValues() && aOther.HasValues() && aOther.max >= min && aOther.min <= max;
   }

   /// @brief Returns true if this object contains a range.  False otherwise.
   inline bool HasValues() const { return min.has_value() && max.has_value(); }

   /// @brief The delta between this object's minimum and maximum, or in other words the length of the range.
   ut::optional<int64_t> Delta() const;
   /// @brief Subtracts the intersection of a series of range values from the delta of a given range.
   /// Does not consider overlap between multiple ranges in `aRanges`
   ut::optional<int64_t> DeltaExcluding(const std::vector<TimeBounds>& aRanges) const;

   ut::optional<int64_t> min;
   ut::optional<int64_t> max;
};


bool ARTIFICER_EXPORT operator==(const TimeBounds& aA, const TimeBounds& aB);
bool ARTIFICER_EXPORT operator!=(const TimeBounds& aA, const TimeBounds& aB);


} // namespace artificer

#endif
