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

#ifndef WSFCOVERAGELATLONGRID_HPP
#define WSFCOVERAGELATLONGRID_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <vector>

#include "WsfCoverageRectangularGrid.hpp"

namespace wsf
{
namespace coverage
{

//! A rectangular Lat/Lon grid.
//!
//! This grid offers a rectangular array of grid points with constant
//! latitude and longitude spacing. The grid points are all at the same
//! altitude. If the spacing does not evenly divide the available range
//! of latitude or longitude, the final grid points may not fall on the
//! maximum values of the provided range.
class WSF_COVERAGE_EXPORT LatLonGrid : public RectangularGrid
{
public:
   explicit LatLonGrid(const WsfScenario& aScenario)
      : RectangularGrid(aScenario)
   {
      SetType(cTYPE);
   }
   LatLonGrid(const LatLonGrid& aOther) = default;
   ~LatLonGrid() override               = default;

   // WsfObject overrides
   LatLonGrid* Clone() const override { return new LatLonGrid{*this}; }
   bool        ProcessInput(UtInput& aInput) override;

   // Query grid setup
   //! Return the minimum latitude in the grid in degrees.
   double GetMinimumLatitudeDeg() const { return mLatMinimumDeg; }

   //! Return the maximum latitude in the grid in degrees.
   double GetMaximumLatitudeDeg() const { return mLatMaximumDeg; }

   //! Return the minimum longitude in the grid in degrees.
   double GetMinimumLongitudeDeg() const { return mLonMinimumDeg; }

   //! Return the maximum longitude in the grid in degrees.
   double GetMaximumLongitudeDeg() const { return mLonMaximumDeg; }

   //! Get the spacing in degrees of the latitude lines of the grid.
   double GetLatitudeSpacingDeg() const { return mLatSpacingDeg; }

   //! Get the spacing in degrees of the longitude lines of the grid.
   double GetLongitudeSpacingDeg() const { return mLonSpacingDeg; }

   constexpr static const char* cTYPE{"WSF_LAT_LON_GRID"};

protected:
   bool InitializeP() override;
   void SetMinimumLatitudeDeg(double aLat) { mLatMinimumDeg = aLat; }
   void SetMaximumLatitudeDeg(double aLat) { mLatMaximumDeg = aLat; }
   void SetMinimumLongitudeDeg(double aLon) { mLonMinimumDeg = aLon; }
   void SetMaximumLongitudeDeg(double aLon) { mLonMaximumDeg = aLon; }
   void SetOrigin(double aLat, double aLon);
   bool IsOriginSet() const { return mOriginSet; }
   bool RemovePoint(PointId aPointId);

private:
   double              TakeLatitudeStep(double aLatitude, bool aForward) const;
   double              TakeLongitudeStep(double aLongitude, double aLatitude, bool aForward) const;
   std::vector<double> GetLatitudeLoopValues() const override;
   std::vector<double> GetLongitudeLoopValues(double aLatitudeDeg) const override;

private:
   double mLatMinimumDeg{};
   double mLatMaximumDeg{};
   double mLatSpacingDeg{-1.0};
   double mLonMinimumDeg{};
   double mLonMaximumDeg{};
   double mLonSpacingDeg{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGELATLONGRID_HPP
