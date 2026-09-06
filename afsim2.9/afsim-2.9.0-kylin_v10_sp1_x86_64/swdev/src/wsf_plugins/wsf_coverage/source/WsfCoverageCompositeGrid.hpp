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

#ifndef WSFCOVERAGECOMPOSITEGRID_HPP
#define WSFCOVERAGECOMPOSITEGRID_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <utility>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "WsfCoverageGrid.hpp"

namespace wsf
{
namespace coverage
{

//! A coverage grid formed from the composition of multiple sub-grids.
//!
//! The implementation strategy is to defer as much to the owned instances of the
//! sub-grids. In a few cases, the PointId or GridAsset from the sub-grid is
//! modified to give the correct PointId relative to the parent grid.
class WSF_COVERAGE_EXPORT CompositeGrid : public Grid
{
public:
   explicit CompositeGrid(const WsfScenario& aScenario)
      : Grid(aScenario)
   {
   }
   CompositeGrid(const CompositeGrid& aOther) = default;
   ~CompositeGrid() override                  = default;

   CompositeGrid* Clone() const override { return new CompositeGrid{*this}; }

   bool                                ProcessInput(UtInput& aInput) override;
   void                                PreparePlatforms(WsfSimulation& aSimulation) override;
   const Point*                        GetPoint(PointId aPointId) const override;
   std::size_t                         GetSize() const override { return mPoints.size(); }
   void                                Invoke(GridAction& aAction) const override;
   GridAsset                           GetAsset(PointId aPointId) const override;
   GridAsset                           GetAsset(const std::string& aPlatformName) const override;
   void                                HasNullDevice(bool& aAnyNull, bool& aAllNull) const override;
   bool                                CreatesNewPlatforms() const override { return true; }
   bool                                IsStructured() const override;
   std::size_t                         GetNumComponents() const override;
   std::string                         GetComponentName(std::size_t aComponentIndex) const override;
   std::pair<std::size_t, std::size_t> GetComponentSizes(std::size_t aComponentIndex) const override;
   std::pair<std::size_t, std::size_t> GetPointIndices(PointId aPointId) const override;

   //! Invoke the given action on the given component.
   //!
   //! This will invoke the given @p aAction on the component with the given @p aComponentIndex.
   //! This will effectively call InvokeOnComponent on the subgrid, but will then translate the
   //! points from the subgrid into the equivalent point on the parent grid. This is important for
   //! cases where InvokeOnComponent calls the provided action more times than when calling
   //! Invoke for the selected subgrid.
   //!
   //! \param aAction         - The action to invoke for each point of the subgrid.
   //! \param aComponentIndex - The index of the component on which to invoke the given @p aAction.
   void InvokeOnComponent(GridAction& aAction, std::size_t aComponentIndex) const override;

   constexpr static const char* cTYPE{"WSF_COMPOSITE_GRID"};

protected:
   bool InitializeP() override;

private:
   //! The sub-grids.
   std::vector<ut::CloneablePtr<Grid>> mSubGrids{};

   //! The grid assets.
   std::map<PointId, GridAsset> mAssets{};

   //! The grid points.
   std::map<PointId, Point> mPoints{};

   //! Map from platform name to PointId.
   std::map<std::string, PointId> mNameToId{};

   //! Map from point ID to component.
   std::map<PointId, std::size_t> mIdToComponent{};

   //! Map from parent point ID to component Point ID
   std::map<PointId, PointId> mParentToChild{};

   // Needs to be a vector since there is no guarantee that the child IDs are unique
   //! Map from component point ID to parent point Id.
   std::vector<std::map<PointId, PointId>> mChildToParent{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGECOMPOSITEGRID_HPP
