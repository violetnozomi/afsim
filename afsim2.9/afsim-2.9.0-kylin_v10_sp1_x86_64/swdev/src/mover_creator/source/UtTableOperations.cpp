// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtTableOperations.hpp"

#include "UtMath.hpp"

double Designer::util::CalcSlope(double aValueMin, double aValueMax, double aRangeMin, double aRangeMax)
{
   return (aRangeMax - aRangeMin) / (aValueMax - aValueMin);
}

double Designer::util::LookupValue(const std::map<double, double>& aMap, double aKey, ExtrapolationMethod aForwardMethod, ExtrapolationMethod aReverseMethod)
{
   if (aMap.size() == 0) { return 0.0; }

   auto it = aMap.lower_bound(aKey);

   if (it != aMap.end())
   {
      if (it->first == aKey)
      {
         return it->second;
      }
      if (it != aMap.begin())
      {
         auto it_prev = std::prev(it, 1);

         return UtMath::Lerp(aKey, it_prev->first, it->first, it_prev->second, it->second);
      }
      // Reverse Extrapolate
      if (aReverseMethod == ExtrapolationMethod::cZERO)
      {
         return 0;
      }
      if (aReverseMethod == ExtrapolationMethod::cCLAMP)
      {
         return it->second;
      }
      // Extrapolate
      if (aMap.size() > 1)
      {
         auto it_next = std::next(it, 1);

         // Find the slope between the first two points in the map
         double slope = CalcSlope(it->first, it_next->first, it->second, it_next->second);

         return it->second - slope * (it->first - aKey);

      }
      return it->second;
   }

   // Forward Extrapolate

   if (aForwardMethod == ExtrapolationMethod::cZERO)
   {
      return 0;
   }
   if (aReverseMethod == ExtrapolationMethod::cCLAMP)
   {
      return aMap.rbegin()->second;
   }

   // Extrapolate
   if (aMap.size() > 1)
   {
      auto iter = aMap.rbegin();
      auto iter_prev = std::next(iter, 1);

      // Find the slope between the last two
      double slope = CalcSlope(iter_prev->first, iter->first, iter_prev->second, iter->second);

      return iter->second + slope * (aKey - iter->first);
   }
   return it->second;

}
