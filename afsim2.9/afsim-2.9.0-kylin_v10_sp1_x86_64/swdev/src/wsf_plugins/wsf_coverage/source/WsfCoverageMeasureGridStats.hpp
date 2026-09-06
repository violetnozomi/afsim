// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOVERAGEMEASUREGRIDSTATS_HPP
#define WSFCOVERAGEMEASUREGRIDSTATS_HPP

#include "wsf_coverage_export.h"

#include <limits>

namespace wsf
{
namespace coverage
{

class Coverage;
class Measure;

//! A class to compute summary statistics over all the points in a grid.
class WSF_COVERAGE_EXPORT MeasureGridStats
{
public:
   MeasureGridStats(const Coverage& aCoverage, const Measure& aMeasure);

   //! Return the minimum value over the grid.
   double GetMinValue() const { return mMinValue; }

   //! Return the maximum value over the grid.
   double GetMaxValue() const { return mMaxValue; }

   //! Return the mean value over the grid.
   double GetMeanValue() const { return mMeanValue; }

   //! Return the number of values from which the statistics were computed.
   int GetCount() const { return mCount; }

private:
   double mMinValue{std::numeric_limits<double>::max()};
   double mMaxValue{std::numeric_limits<double>::lowest()};
   double mMeanValue{};
   int    mCount{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASUREGRIDSTATS_HPP
