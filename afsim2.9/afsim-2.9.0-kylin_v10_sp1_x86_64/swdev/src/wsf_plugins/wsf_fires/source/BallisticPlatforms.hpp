// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRESBALLISTICPLATFORMS_HPP
#define FIRESBALLISTICPLATFORMS_HPP

#include "wsf_fires_export.h"

#include <set>
#include <string>
class DisFire;
class WsfPlatform;
class WsfSimulation;
class WsfWeaponEngagement;

namespace Fires
{
//! A types database for platforms that should be considered
//! ballistic targets.  It is used as a quick types lookup,
//! in case the user wants to internally propagate the target
//! or provide some other special processing.
class WSF_FIRES_EXPORT BallisticPlatforms
{
public:
   static WsfPlatform* CreatePlatform(const std::string& aTypeName, const DisFire& aFire, WsfSimulation* aSimPtr);

   static WsfPlatform* CreatePlatform(const std::string&         aTypeName,
                                      const WsfWeaponEngagement& aEngagement,
                                      WsfSimulation*             aSimPtr);

private:
   static WsfPlatform* CreatePlatformP(const std::string& aTypeName, WsfSimulation* aSimPtr);

   static bool InitializePlatform(WsfPlatform* aWeaponPtr, const double aLocationWCS[3], double aInitialBearing, double aRange);
};
}; // namespace Fires

#endif
