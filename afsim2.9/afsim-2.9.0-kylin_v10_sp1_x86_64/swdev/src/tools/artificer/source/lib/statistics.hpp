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
#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include "artificer_export.h"

#include <cmath>
#include <vector>

namespace artificer
{
///@brief Returns the sum of all values in `aValues` when accumulated in a `T` type.
template<typename T, typename CONTAINER>
T AccumulateToType(const CONTAINER& aValues)
{
   T sum = static_cast<T>(0);
   for (auto current = aValues.begin(); current != aValues.end(); ++current)
   {
      sum += static_cast<T>(*current);
   }
   return sum;
}

// FUTURE - Replace with std::accumulate in C++20
///@brief Returns the sum of all values in `aValues` when accumulated in a double.
template<typename CONTAINER>
double Accumulate(const CONTAINER& aValues)
{
   return AccumulateToType<double>(aValues);
}

///@brief Returns the average of all values in `aValues` as a double.
template<typename CONTAINER>
double Average(const CONTAINER& aValues)
{
   return Accumulate(aValues) / static_cast<double>(aValues.size());
}

///@brief Returns the standard deviation of all values in `aValues` as a double.
template<typename CONTAINER>
double StdDev(const CONTAINER& aValues)
{
   const double avg   = Average(aValues);
   const double count = static_cast<double>(aValues.size());
   double       sum   = 0.0;
   for (const auto& current : aValues)
   {
      const double delta = static_cast<double>(current) - avg;
      sum += (delta * delta) / count;
   }
   return std::sqrt(sum);
}

} // namespace artificer

#endif
