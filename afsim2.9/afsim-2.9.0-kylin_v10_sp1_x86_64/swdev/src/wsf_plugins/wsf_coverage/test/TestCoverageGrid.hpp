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

#ifndef WSFCOVERAGETESTTESTCOVERAGEGRID_HPP
#define WSFCOVERAGETESTTESTCOVERAGEGRID_HPP

#include <map>
#include <string>
#include <vector>

#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"

//! A test grid that contains 4 points once initialized covering a grid
//! in lat/lon with extending from 0 to 90 in lon, and 0 to 45 in lat.
class TestGrid : public wsf::coverage::Grid
{
public:
   explicit TestGrid(const WsfScenario&                       aScenario,
                     const std::vector<wsf::coverage::Point>& aPoints = std::vector<wsf::coverage::Point>{});
   TestGrid(const TestGrid& aOther) = default;
   ~TestGrid() override             = default;

   TestGrid* Clone() const override { return new TestGrid{*this}; }

   void                        PreparePlatforms(WsfSimulation& aSimulation) override {}
   const wsf::coverage::Point* GetPoint(wsf::coverage::PointId aPointId) const;
   std::size_t                 GetSize() const { return mPoints.size(); }
   void                        Invoke(wsf::coverage::GridAction& aAction) const;
   wsf::coverage::GridAsset    GetAsset(wsf::coverage::PointId aPointId) const override;
   wsf::coverage::GridAsset    GetAsset(const std::string& aPlatformName) const override;
   void                        HasNullDevice(bool& aAnyNull, bool& aAllNull) const override;
   bool                        CreatesNewPlatforms() const override { return true; }

   constexpr static std::size_t GetNumLats() { return 2; }
   constexpr static std::size_t GetNumLons() { return 2; }
   constexpr static double      GetMinLat() { return 0.0; }
   constexpr static double      GetMeanLat() { return 22.5; }
   constexpr static double      GetMaxLat() { return 45.0; }
   constexpr static double      GetMinLon() { return 0.0; }
   constexpr static double      GetMeanLon() { return 45.0; }
   constexpr static double      GetMaxLon() { return 90.0; }

protected:
   bool InitializeP() override;

private:
   std::string GetPlatformName(wsf::coverage::PointId aPointId) const;

   std::vector<wsf::coverage::Point>               mPoints{};
   std::map<std::string, wsf::coverage::GridAsset> mAssets{};
};

#endif // WSFCOVERAGETESTTESTCOVERAGEGRID_HPP
