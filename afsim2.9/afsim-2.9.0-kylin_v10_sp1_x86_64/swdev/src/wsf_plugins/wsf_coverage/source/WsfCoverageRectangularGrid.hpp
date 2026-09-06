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

#ifndef WSFCOVERAGERECTANGULARGRID_HPP
#define WSFCOVERAGERECTANGULARGRID_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <string>

#include "WsfCoverageGrid.hpp"

namespace wsf
{
namespace coverage
{

//! A grid that forms a rectangular lattice of points, with the details of the placement
//! of those points left up to the subclasses.
class WSF_COVERAGE_EXPORT RectangularGrid : public Grid
{
public:
   explicit RectangularGrid(const WsfScenario& aScenario)
      : Grid(aScenario)
   {
   }
   RectangularGrid(const RectangularGrid& aOther);
   ~RectangularGrid() override = default;

   bool                                ProcessInput(UtInput& aInput) override;
   void                                PreparePlatforms(WsfSimulation& aSimulation) override;
   const Point*                        GetPoint(PointId aPointId) const override;
   std::size_t                         GetSize() const override { return mPoints.size(); }
   void                                Invoke(GridAction& aAction) const override;
   GridAsset                           GetAsset(PointId aPointId) const override;
   GridAsset                           GetAsset(const std::string& aPlatformName) const override;
   void                                HasNullDevice(bool& aAnyNull, bool& aAllNull) const override;
   bool                                CreatesNewPlatforms() const override { return true; }
   bool                                IsStructured() const override { return true; }
   std::pair<std::size_t, std::size_t> GetComponentSizes(std::size_t aComponentIndex) const override;
   std::pair<std::size_t, std::size_t> GetPointIndices(PointId aPointId) const override;

   //! Return the platform type of the assets for this grid.
   const std::string& GetAssetPlatformType() const { return mPlatformType; }

   //! Return the device name for the assets for this grid.
   const std::string& GetAssetDeviceName() const { return mDeviceName; }

   //! Return the altitude of the grid points.
   double GetAltitude() const { return mAltitude; }

   //! Return if the altitude reference of the grid points is AGL.
   //!
   //! \returns true if the altitude reference is AGL; false if the reference is MSL.
   bool IsAltitudeAGL() const { return mAltitudeAGL; }

   //! Get the origin point in latitude.
   double GetLatitudeOriginDeg() const { return mLatOriginDeg; }

   //! Get the origin point in longitude.
   double GetLongitudeOriginDeg() const { return mLonOriginDeg; }

protected:
   virtual std::vector<double> GetLatitudeLoopValues() const                     = 0;
   virtual std::vector<double> GetLongitudeLoopValues(double aLatitudeDeg) const = 0;
   bool                        CreateRectangularGrid();
   std::string                 GetPlatformName(PointId aPointId) const;

   std::string                                            mPlatformType{};
   std::string                                            mDeviceName{};
   double                                                 mAltitude{0.0};
   double                                                 mLatOriginDeg{};
   double                                                 mLonOriginDeg{};
   size_t                                                 mLatSize{};
   size_t                                                 mLonSize{};
   std::map<PointId, Point>                               mPoints{};
   std::map<PointId, std::pair<std::size_t, std::size_t>> mIndices{};
   std::map<std::string, GridAsset>                       mAssets{};
   bool                                                   mOriginSet{false};
   bool                                                   mAltitudeAGL{false};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGERECTANGULARGRID_HPP
