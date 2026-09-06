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

#include "WkfAirCombatDataContainer.hpp"

wkf::DataContainer::DataContainer(const wkf::DataContainer& aSrc)
   : mDataList(aSrc.mDataList)
   , mViablePlatforms(aSrc.mViablePlatforms)
{
}

bool wkf::DataContainer::IsPlatformViable(const std::string& aPlatformName) const
{
   return std::find(mViablePlatforms.begin(), mViablePlatforms.end(), aPlatformName) != mViablePlatforms.end();
}

void wkf::DataContainer::RemoveViablePlatform(const std::string& aPlatformName)
{
   auto foundPlatform = std::find(mViablePlatforms.begin(), mViablePlatforms.end(), aPlatformName);
   if (foundPlatform != mViablePlatforms.end())
   {
      mViablePlatforms.erase(foundPlatform);
   }
}

void wkf::DataContainer::Clear()
{
   mDataList.clear();
   mViablePlatforms.clear();
}
