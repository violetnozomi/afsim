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

#include "WsfCoverageAssetSpecification.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

bool AssetSpecification::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command;
   aInput.GetCommand(command);
   Asset inputAsset{};
   if (inputAsset.SetAssetClass(command))
   {
      aInput.ReadCommand(inputAsset.mName);
      aInput.ReadCommand(inputAsset.mDeviceName);
      mFreeAssets.emplace_back(inputAsset);
   }
   else
   {
      retval = false;
   }
   return retval;
}

//! Set the AssetClass of this Asset from the string identifier of that class.
//!
//! This will set the asset class of this Asset from the provided @p aAssetClass
//! identifier. If there is no such recognized class, this method sets the
//! asset class of this Asset to cUNKNOWN.
//!
//! \param aAssetClass - A string identifier for the asset class.
//! \returns           - true if the asset class was recognized and this object's class was set;
//!                      false otherwise.
bool AssetSpecification::Asset::SetAssetClass(const std::string& aAssetClass)
{
   if (aAssetClass == "platform")
   {
      mType = AssetClass::cPLATFORM;
   }
   else if (aAssetClass == "platform_type")
   {
      mType = AssetClass::cPLATFORM_TYPE;
   }
   else if (aAssetClass == "category")
   {
      mType = AssetClass::cCATEGORY;
   }
   else if (aAssetClass == "group")
   {
      mType = AssetClass::cGROUP;
   }
   else
   {
      mType = AssetClass::cUNKNOWN;
   }
   return mType != AssetClass::cUNKNOWN;
}

} // namespace coverage
} // namespace wsf
