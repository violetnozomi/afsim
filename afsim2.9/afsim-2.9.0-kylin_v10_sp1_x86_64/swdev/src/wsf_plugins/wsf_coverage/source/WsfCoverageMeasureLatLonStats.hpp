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

#ifndef WSFCOVERAGEMEASURELATLONSTATS_HPP
#define WSFCOVERAGEMEASURELATLONSTATS_HPP

#include "wsf_coverage_export.h"

#include <string>
#include <vector>

namespace wsf
{
namespace coverage
{

class Coverage;
class Measure;
class Point;

//! Class to project measured values onto latitude or longitude.
//!
//! The computes the minimum, maximum and mean values of a quantity that
//! was measured over some coverage grid. The statistics are computed for
//! either the set of latitude or longitude values present in the grid.
//! After the projection, independent values are considered to be the
//! same if they are withing a given bin size. This is intended to
//! avoid numerical issues with floating-point comparison, and should
//! typically be set to a small value.
class WSF_COVERAGE_EXPORT MeasureLatLonStats
{
public:
   //! The projection direction.
   enum class Projection
   {
      cLATITUDE,
      cLONGITUDE
   };

   MeasureLatLonStats(const Coverage& aCoverage, const Measure& aMeasure, Projection aProjection, double aMinimumBinSize);

   //! Return the projection direction.
   Projection GetProjection() const { return mProjection; }

   //! Return the string description of the projection direction.
   std::string GetProjectionDescription() const;

   //! Return the number of projected data points.
   std::size_t GetSize() const { return mData.size(); }

   //! Return the latitude or longitude for the given index.
   double GetVariable(std::size_t aIndex) const { return mData[aIndex].mVariable; }

   //! Return the minimum value of the measured quantity for the given index.
   double GetMinValue(std::size_t aIndex) const { return mData[aIndex].mMinimum; }

   //! Return the maximum value of the measured quantity for the given index.
   double GetMaxValue(std::size_t aIndex) const { return mData[aIndex].mMaximum; }

   //! Return the mean value of the measured quantity for the given index.
   double GetMeanValue(std::size_t aIndex) const { return mData[aIndex].mMean; }

   //! Return the number of points that contributed to the mean for the given index.
   int GetCount(std::size_t aIndex) const { return mData[aIndex].mCount; }

private:
   int           BinIndexFromValue(double aValue) const;
   double        BinValueFromIndex(int aIndex) const;
   static double ProjectLatitude(const Point& aPoint);
   static double ProjectLongitude(const Point& aPoint);

   //! A function used to project from the LLA space of the grid Point to a single lat or lon.
   using ProjectorFunction = double (*)(const Point&);

   //! Convenience object to collect statistics data, and provide a simple operation.
   struct BinData
   {
      BinData(double aVariable, double aValue)
         : mVariable{aVariable}
         , mMinimum{aValue}
         , mMaximum{aValue}
         , mMean{aValue}
         , mCount{1}
      {
      }

      void Accumulate(const BinData& aOther);
      void Finalize();

      double mVariable;
      double mMinimum;
      double mMaximum;
      double mMean;
      int    mCount;
   };

   Projection           mProjection;     //! The direction of projection.
   double               mMinimumBinSize; //! The bin size.
   std::vector<BinData> mData{};         //! The computed statistics.
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURELATLONSTATS_HPP
