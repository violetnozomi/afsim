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

#ifndef WSFCOVERAGEZONEBASEDGRID_HPP
#define WSFCOVERAGEZONEBASEDGRID_HPP

#include "WsfCoverageLatLonGrid.hpp"

class WsfZone;

#include "wsf_coverage_export.h"

namespace wsf
{
namespace coverage
{

//! This is a regular grid in latitude and longitude bounded by a WsfZone.
class WSF_COVERAGE_EXPORT ZoneBasedGrid : public LatLonGrid
{
public:
   explicit ZoneBasedGrid(const WsfScenario& aScenario)
      : LatLonGrid(aScenario)
   {
   }
   ZoneBasedGrid(const ZoneBasedGrid& aOther) = default;
   ~ZoneBasedGrid() override                  = default;

   ZoneBasedGrid* Clone() const override { return new ZoneBasedGrid{*this}; }

   bool ProcessInput(UtInput& aInput) override;
   void PreparePlatforms(WsfSimulation& aSimulation) override;
   void InvokeOnComponent(GridAction& aAction, std::size_t aComponentIndex) const override;

   //! Return the name of the that defines the boundary of this grid.
   const std::string& GetZoneName() const { return mZoneName; }

   constexpr static const char* cTYPE{"WSF_ZONE_BASED_GRID"};

protected:
   bool InitializeP() override;

private:
   std::string        mZoneName{};
   std::vector<Point> mCulledPoints{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEZONEBASEDGRID_HPP
