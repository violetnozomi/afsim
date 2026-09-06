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

#ifndef WSFCOVERAGEASSET_HPP
#define WSFCOVERAGEASSET_HPP

#include <string>

#include "WsfCoverageGridPoint.hpp"

namespace wsf
{
namespace coverage
{

constexpr const char* cNULL_COVERAGE_DEVICE_NAME{"none"};
constexpr const char* cUNIVERSAL_NON_ASSET_CATEGORY{"wsf_coverage_univeral_non_asset_category"};
constexpr const char* cUNIVERSAL_GRID_ASSET_CATEGORY{"wsf_coverage_universal_grid_asset_categroy"};
constexpr const char* cUNIVERSAL_FREE_ASSET_CATEGORY{"wsf_coverage_universal_free_asset_category"};

//! Representation of a grid asset.
//!
//! Grid assets are defined by a platform name, a PointId for the
//! grid point the asset represents, and the name of a device participating in
//! the coverage calculation. In some cases, the device name can be 'none',
//! indicating that the interactions participating in the coverage computation
//! do not need devices on both interacting platforms.
class GridAsset
{
public:
   GridAsset(const std::string& aPlatformName, PointId aPointId, const std::string& aDeviceName)
      : mPlatformName{aPlatformName}
      , mPointId{aPointId}
      , mDeviceName{aDeviceName}
   {
   }

   //! Get the ID of the point in the grid to which this grid asset refers.
   PointId GetPointId() const { return mPointId; }

   //! Get the name of the device to which this grid asset refers.
   const std::string& GetDeviceName() const { return mDeviceName; }

   //! Return the platform name corresponding to this grid asset.
   std::string GetPlatformName() const { return mPlatformName; }

   //! Return if this grid asset refers to a valid grid point.
   bool IsValid() const { return mPointId != cINVALID_POINT_ID; }

   //! Set the name of the coverage object to which this grid asset refers.
   void SetPlatformName(const std::string& aPlatformName) { mPlatformName = aPlatformName; }

   //! Set the ID of the point in the grid to which this grid asset refers.
   void SetPointId(PointId aPointId) { mPointId = aPointId; }

   //! Set the name of the device to which this grid asset refers.
   void SetDeviceName(const std::string& aDeviceName) { mDeviceName = aDeviceName; }

private:
   std::string mPlatformName;
   PointId     mPointId;
   std::string mDeviceName;
};

//! Representation of a free asset.
//!
//! Free assets are defined by a platform name, and the name of a device on that
//! platform that participates in the interaction used in the coverage computation.
//! In some case, the device name can be 'none', indicatgin that the interactions
//! participating in the coverage computation do not need devices on both interacting
//! platforms.
class FreeAsset
{
public:
   FreeAsset(const std::string& aPlatformName, const std::string& aDeviceName)
      : mPlatform{aPlatformName}
      , mDevice{aDeviceName}
   {
   }

   //! Get the name of the platform to which this free asset refers.
   const std::string& GetPlatformName() const { return mPlatform; }

   //! Get the name of the device to which this free asset refers.
   const std::string& GetDeviceName() const { return mDevice; }

   //! Set the name of the platform to which this free asset refers.
   void SetPlatformName(const std::string& aPlatformName) { mPlatform = aPlatformName; }

   //! Set the name of the device to which this free asset refers.
   void SetDeviceName(const std::string& aDeviceName) { mDevice = aDeviceName; }

private:
   std::string mPlatform;
   std::string mDevice;
};

} // namespace coverage
} // namespace wsf

namespace std
{

// This specialization allows GridAsset to be used as a key in std::map and similar containers.
template<>
struct less<wsf::coverage::GridAsset>
{
   bool operator()(const wsf::coverage::GridAsset& aLHS, const wsf::coverage::GridAsset& aRHS) const
   {
      return (aLHS.GetPlatformName() < aRHS.GetPlatformName()) ||
             (aLHS.GetPlatformName() == aRHS.GetPlatformName() && aLHS.GetDeviceName() < aRHS.GetDeviceName()) ||
             (aLHS.GetPlatformName() == aRHS.GetPlatformName() && aLHS.GetDeviceName() == aRHS.GetDeviceName() &&
              aLHS.GetPointId() < aRHS.GetPointId());
   }
};

// This specialization allows FreeAsset to be used as a key in std::map and similar containers.
template<>
struct less<wsf::coverage::FreeAsset>
{
   bool operator()(const wsf::coverage::FreeAsset& aLHS, const wsf::coverage::FreeAsset& aRHS) const
   {
      return (aLHS.GetPlatformName() < aRHS.GetPlatformName() ||
              (aLHS.GetPlatformName() == aRHS.GetPlatformName() && aLHS.GetDeviceName() < aRHS.GetDeviceName()));
   }
};

} // namespace std

#endif // WSFCOVERAGEASSET_HPP
