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

#ifndef WSFTABULARLAUNCHCOMPUTER_HPP
#define WSFTABULARLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include "UtCloneablePtr.hpp"
#include "UtEntity.hpp"
#include "UtTable.hpp"
#include "WsfLaunchComputer.hpp"

//! WsfTabularLaunchComputer provided a simple algorithmic launch computer for
//! surface-to-air, air-to-air and air-to-ground guided weapons that are typically
//! implemented using WsfGuidedMover and WsfGuidanceComputer. It utilizes tables to
//! calculate approximate intercept point, time-of-flight and launch conditions and
//! assumes that guidance will be able to provide the additional guidance to intercept
//! the target.
//!
//! Because it is approximate, it is probably not applicable for missiles that require
//! extremely accurate initial conditions such as purely ballistic missiles or exo- or
//! high endo-atmospheric interceptors.

class WSF_MIL_EXPORT WsfTabularLaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_TABULAR_LAUNCH_COMPUTER"; }

   WsfTabularLaunchComputer();
   WsfTabularLaunchComputer(const WsfTabularLaunchComputer& aSrc);
   ~WsfTabularLaunchComputer() override = default;
   WsfTabularLaunchComputer& operator=(const WsfTabularLaunchComputer&) = delete;

   WsfLaunchComputer* Clone() const override;
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

protected:
   bool InitializeTTIData() override;

private:
   void ComputeIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime);

   double GetAverageSpeed(const double aIntLocWCS[3]);

   void ShowPoint(const double aLocWCS[3], double aRedValue, double aGreenValue, double aBlueValue, int aPointSize);
   void UpdateLaunchPerception(double aCurTime);
   bool UpdateTargetPerception(double aCurTime, const WsfTrack& aTrack);

   //! The time-of-flight to reach an intercept point
   UtCloneablePtr<UtTable::Table> mTimeOfFlightPtr;

   double mAverageSpeed;
   double mDropAdjustmentFactor;

   UtEntity mLauState;
   double   mLauSpeed;

   UtEntity mTgtState;

   bool mShowGraphics;
};

#endif
