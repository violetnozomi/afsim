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

#include "TimeBounds.hpp"

namespace artificer
{
TimeBounds::TimeBounds(const int64_t aValue1, const int64_t aValue2)
{
   Union(aValue1);
   Union(aValue2);
}

TimeBounds& TimeBounds::Union(int64_t aValue)
{
   min = min.has_value() ? std::min<int64_t>(aValue, *min) : aValue;
   max = max.has_value() ? std::max<int64_t>(aValue, *max) : aValue;
   return *this;
}

TimeBounds& TimeBounds::Union(const TimeBounds& aOther)
{
   if (aOther.min.has_value())
   {
      Union(*aOther.min);
   }
   if (aOther.max.has_value())
   {
      Union(*aOther.max);
   }
   return *this;
}

TimeBounds& TimeBounds::Intersection(const TimeBounds& aOther)
{
   if (!Intersects(aOther))
   {
      min.reset();
      max.reset();
   }
   else if (aOther.max != max || aOther.min != min)
   {
      max = std::min(*aOther.max, *max);
      min = std::max(*aOther.min, *min);
   }
   return *this;
}

ut::optional<int64_t> TimeBounds::Delta() const
{
   return (min.has_value() && max.has_value()) ? *max - *min : ut::optional<int64_t>();
}

ut::optional<int64_t> TimeBounds::DeltaExcluding(const std::vector<TimeBounds>& aRanges) const
{
   if (!HasValues())
   {
      return ut::optional<int64_t>();
   }
   int64_t remaining = *max - *min;
   for (const auto& range : aRanges)
   {
      if (range.HasValues())
      {
         // Note that this does not consider the case of two subtracted range values overlapping.
         TimeBounds intersect(*this);
         intersect.Intersection(range);
         if (intersect.HasValues())
         {
            remaining -= *intersect.Delta();
         }
         // else ranges do not overlap, no effect.
      }
   }
   return remaining;
}

bool operator==(const TimeBounds& aA, const TimeBounds& aB)
{
   return aA.min == aB.min && aA.max == aB.max;
}

bool operator!=(const TimeBounds& aA, const TimeBounds& aB)
{
   return !(aA == aB);
}

} // namespace artificer
