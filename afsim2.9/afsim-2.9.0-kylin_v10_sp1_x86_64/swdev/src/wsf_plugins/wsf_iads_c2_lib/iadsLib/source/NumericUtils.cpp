// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <cmath>
#include <algorithm>
#include <limits>
#include <iadsLib/NumericUtils.hpp>

namespace NumericUtils
{
   IADSLIB_EXPORT bool NearlyEqual(double lhs, double rhs)
   {
      double a1 = std::fabs(lhs);
      double a2 = std::fabs(rhs);
      double biggest = std::max(a1, a2);
      double smallest = std::min(a1, a2);

      double adjustedEps = std::numeric_limits <double>::epsilon() * biggest;

      return ((biggest - smallest) <= adjustedEps);
   }

   IADSLIB_EXPORT bool LessThan(double lhs, double rhs)
   {
      return !NearlyEqual(lhs, rhs) && (lhs < rhs);
   }

   IADSLIB_EXPORT bool LessEqual(double lhs, double rhs)
   {
      return NearlyEqual(lhs, rhs) || (lhs < rhs);
   }

   IADSLIB_EXPORT bool GreaterThan(double lhs, double rhs)
   {
      return !NearlyEqual(lhs, rhs) && (lhs > rhs);
   }

   IADSLIB_EXPORT bool GreaterThanEqual(double lhs, double rhs)
   {
      return NearlyEqual(lhs, rhs) || (lhs > rhs);
   }
}
