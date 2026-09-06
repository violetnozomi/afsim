// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFATG_LAUNCHCOMPUTER_HPP
#define WSFATG_LAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <list>
#include <vector>

class UtInput;
#include "WsfATG_WeaponLAR.hpp"
#include "WsfLaunchComputer.hpp"
class WsfPlatform;
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfWeapon;

//! Launch Computer which supplies information to launch an air to ground weapon.
//! A WsfATG_LaunchComputer instance provides information used to launch a guided airborne
//! weapon against a ground track.  User input specifies the weapon type, a collection of
//! one or more LARs to use in varying flight conditions, and one or more target track
//! IDs.  During flight, the LAR collection will be examined to choose the closest match
//! to the current flight conditions.  User stream input will configure the processor to run in
//! Auto mode, against the current platform target, using a single specified weapon.
//! After each firing, a flag must be re-set to allow the next subsequent shot (this
//! prevents all ordinance from being expended upon a single target).

class WSF_MIL_EXPORT WsfATG_LaunchComputer : public WsfLaunchComputer
{
public:
   using ATGWeaponLARCollection = std::vector<WsfATG_WeaponLAR*>;

   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_ATG_LAUNCH_COMPUTER"; }

   WsfATG_LaunchComputer();
   ~WsfATG_LaunchComputer() override;

   WsfLaunchComputer* Clone() const override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;
   bool               ProcessInput(UtInput& aInput) override;

   void Update(double aSimTime) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   void SelectLAR(double aLaunchAlt, double aLaunchSpeed);

   const ATGWeaponLARCollection& GetWeaponLARs() { return mWeaponLARs; }
   int                           GetBestFitLAR_Index() { return mBestFitLAR; }
   WsfATG_WeaponLAR*             GetBestFitLAR() { return mWeaponLARs[mBestFitLAR]; }

protected:
   WsfATG_LaunchComputer(const WsfATG_LaunchComputer& aSrc);
   WsfATG_LaunchComputer& operator=(const WsfATG_LaunchComputer&) = delete; // Not implemented

   bool InitializeTTIData() override;
   bool WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack) override;

private:
   ATGWeaponLARCollection mWeaponLARs;
   int                    mBestFitLAR;
   double                 mLastUpdate;
   bool                   mDebugLARs{false};
};

#endif
