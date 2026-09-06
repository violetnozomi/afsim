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

#ifndef WSFBALLISTICLAUNCHCOMPUTER_HPP
#define WSFBALLISTICLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include "TblLookup.hpp"
#include "WsfLaunchComputer.hpp"

//! A launch computer  for a ballistic (unguided) gravity weapon.
//! This class contains a lookup table for the time and downrange distance required for
//! an iron bomb to fall from a given launch platform altitude and speed, down to a
//! given target altitude.  Since this logic can have no control over the flight path
//! of the vehicle, the lateral impact point cannot be guaranteed.  To account for this,
//! an acceptable_cross_range_error value may be specified.  If the ballistic impact
//! point will be within \<specified lateral distance\> of the target, a value of
//! time-of-flight is returned.  If not, the time-of-flight returned is cFOREVER.

class WSF_MIL_EXPORT WsfBallisticLaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_BALLISTIC_LAUNCH_COMPUTER"; }

   WsfBallisticLaunchComputer();
   WsfBallisticLaunchComputer(const WsfBallisticLaunchComputer& aSrc) = default;
   WsfBallisticLaunchComputer& operator=(const WsfBallisticLaunchComputer&) = delete;
   ~WsfBallisticLaunchComputer() override                                   = default;

   WsfLaunchComputer* Clone() const override;

   bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   void GetBombFlight(double aLnchAlt, double aLnchSpeed, double aTgtAlt, double& aTOF, double& aDownRange);

protected:
   bool NewSizeSpecified() const { return mAltsSet || mSpdsSet || mTgtsSet; }
   bool SizeFullySpecified() const { return mAltsSet && mSpdsSet && mTgtsSet; }
   bool SizedProperly() const { return !NewSizeSpecified() && mSized; }
   void LoadIndepValues();

   bool InitializeTTIData() override;

   bool WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack) override;

private:
   bool   mAltsSet;
   bool   mSpdsSet;
   bool   mTgtsSet;
   bool   mSized;
   int    mNumLaunchAlts;
   int    mNumLaunchSpds;
   int    mNumTargetAlts;
   double mMinLaunchAlt;
   double mDeltaLaunchAlt;
   double mMinLaunchSpd;
   double mDeltaLaunchSpd;
   double mMinTargetAlt;
   double mDeltaTargetAlt;

   TblIndVarE<double> mLaunchAlts;
   TblIndVarE<double> mLaunchSpds;
   TblIndVarE<double> mTargetAlts;

   TblDepVar3<double> mTOFs;
   TblDepVar3<double> mRanges;

   TblLookupLE<double> mLaunchAltLE;
   TblLookupLE<double> mLaunchSpdLE;
   TblLookupLE<double> mTargetAltLE;
};

#endif
