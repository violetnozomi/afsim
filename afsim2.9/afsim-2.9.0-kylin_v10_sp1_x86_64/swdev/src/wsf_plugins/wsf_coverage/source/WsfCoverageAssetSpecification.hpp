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

#ifndef WSFCOVERAGEASSETSPECIFICATION_HPP
#define WSFCOVERAGEASSETSPECIFICATION_HPP

#include <set>
#include <string>
#include <vector>

class UtInput;

#include "WsfCoverageAsset.hpp"
class WsfSimulation;

#include "wsf_coverage_export.h"

namespace wsf
{
namespace coverage
{

//! This object handles reading input specification of assets for coverage objects.
class WSF_COVERAGE_EXPORT AssetSpecification
{
public:
   AssetSpecification()                                 = default;
   AssetSpecification(const AssetSpecification& aOther) = default;
   ~AssetSpecification()                                = default;

   bool ProcessInput(UtInput& aInput);

   //! AssetClass gives the set of possible classes of assets supported by Coverage objects.
   enum class AssetClass
   {
      cUNKNOWN,
      cPLATFORM,
      cPLATFORM_TYPE,
      cCATEGORY,
      cGROUP
   };

   //! A record read in from the input file.
   struct WSF_COVERAGE_EXPORT Asset
   {
      AssetClass  mType{AssetClass::cUNKNOWN};
      std::string mName;
      std::string mDeviceName;

      bool SetAssetClass(const std::string& aAssetClass);
   };

   std::size_t               NumSpecifications() const { return mFreeAssets.size(); }
   const Asset&              GetAsset(std::size_t aIndex) const { return mFreeAssets[aIndex]; }
   const std::vector<Asset>& GetAssets() const { return mFreeAssets; }

private:
   std::vector<Asset> mFreeAssets{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEASSETSPECIFICATION_HPP
