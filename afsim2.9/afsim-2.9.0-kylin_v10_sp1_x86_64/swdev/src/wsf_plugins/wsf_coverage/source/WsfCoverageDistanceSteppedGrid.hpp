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

#ifndef WSFCOVERAGEDISTANCESTEPPEDGRID_HPP
#define WSFCOVERAGEDISTANCESTEPPEDGRID_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageRectangularGrid.hpp"

namespace wsf
{
namespace coverage
{

//! A grid that specifies its separation between points as a length across the surface of the Earth.
class WSF_COVERAGE_EXPORT DistanceSteppedGrid : public RectangularGrid
{
public:
   explicit DistanceSteppedGrid(const WsfScenario& aScenario)
      : RectangularGrid(aScenario)
   {
   }
   DistanceSteppedGrid(const DistanceSteppedGrid& aOther) = default;
   ~DistanceSteppedGrid() override                        = default;

   DistanceSteppedGrid* Clone() const override { return new DistanceSteppedGrid{*this}; }
   bool                 ProcessInput(UtInput& aInput) override;

   //! Return the distance in meters between adjacent grid points in the N-S direction.
   double GetLatStepDistance() const { return mLatStepDistance; }

   //! Return the distance in meters between adjacent grid points in the E-W direction.
   double GetLonStepDistance() const { return mLonStepDistance; }

   //! Return the number of grid points in the N-S direction.
   int GetNumLatSteps() const { return mNumLatSteps; }

   //! Return the number of grid points in the E-W direction.
   int GetNumLonSteps() const { return mNumLonSteps; }

   //! Return the index in the grid of the origin in the N-S direction.
   int GetLatOriginIndex() const { return mLatOriginIndex; }

   //! Return the index in the grid of the origin in the E-W direction.
   int GetLonOriginIndex() const { return mLonOriginIndex; }

   constexpr static const char* cTYPE{"WSF_DISTANCE_STEPPED_GRID"};

protected:
   bool InitializeP() override;

private:
   double              TakeLatitudeStep(double aLatitude, bool aForward) const;
   double              TakeLongitudeStep(double aLongitude, double aLatitude, bool aForward) const;
   std::vector<double> GetLatitudeLoopValues() const override;
   std::vector<double> GetLongitudeLoopValues(double aLatitudeDeg) const override;

   double mLatStepDistance{-1.0};
   double mLonStepDistance{-1.0};
   int    mNumLatSteps{-1};
   int    mNumLonSteps{-1};
   int    mLatOriginIndex{0};
   int    mLonOriginIndex{0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEDISTANCESTEPPEDGRID_HPP
