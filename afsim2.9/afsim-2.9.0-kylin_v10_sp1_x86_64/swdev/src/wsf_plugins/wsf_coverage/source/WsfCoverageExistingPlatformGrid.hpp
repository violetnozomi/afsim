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

#ifndef WSFCOVERAGEEXISTINGPLATFORMGRID_HPP
#define WSFCOVERAGEEXISTINGPLATFORMGRID_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <string>

#include "WsfCoverageGrid.hpp"
#include "WsfCoverageGridPoint.hpp"

namespace wsf
{
namespace coverage
{

//! This is a grid specified through a number of existing platforms in the simulation.
//!
//! Unlike grids that create new platforms to represent the interacting points for the grid,
//! this uses existing platforms. These platforms are not required to be stationary, and they
//! are not required to form some pattern in their locations.
class WSF_COVERAGE_EXPORT ExistingPlatformGrid : public Grid
{
public:
   explicit ExistingPlatformGrid(const WsfScenario& aScenario)
      : Grid(aScenario)
   {
      SetType(cTYPE);
   }
   ExistingPlatformGrid(const ExistingPlatformGrid& aOther) = default;
   ~ExistingPlatformGrid() override                         = default;

   ExistingPlatformGrid* Clone() const override { return new ExistingPlatformGrid{*this}; }
   bool                  ProcessInput(UtInput& aInput) override;

   void         PreparePlatforms(WsfSimulation& aSimulation) override;
   const Point* GetPoint(PointId aPointId) const override;
   std::size_t  GetSize() const override { return mPlatforms.size(); }
   void         Invoke(GridAction& aAction) const override;
   GridAsset    GetAsset(PointId aPointId) const override;
   GridAsset    GetAsset(const std::string& aPlatformName) const override;
   void         HasNullDevice(bool& aAnyNull, bool& aAllNull) const override;
   bool         CreatesNewPlatforms() const override { return false; }
   bool         IsStructured() const override { return false; }

   constexpr static const char* cTYPE{"WSF_EXISTING_PLATFORM_GRID"};

protected:
   bool InitializeP() override;

private:
   void WriteHeaderExtension(TextOutputWriter& aWriter) const override;
   void WriteLineExtension(TextOutputWriter& aWriter, PointId aPointId) const override;

   // Implementation details
   std::map<std::string, PointId>     mPlatforms{};   //! Mapping from platform names to point ID
   std::map<PointId, GridAsset>       mAssets{};      //! Maps point ID into the grid assets
   std::map<PointId, Point>           mPoints{};      //! Maps point ID into Point objects with initial LLA of platforms
   std::map<std::string, std::string> mInputAssets{}; //! The assets as specified in the input
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEEXISTINGPLATFORMGRID_HPP
