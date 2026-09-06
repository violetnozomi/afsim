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

#ifndef WSFLAUNCHCOMPUTER_HPP
#define WSFLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

class UtInput;
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
class WsfTrack;
#include "WsfTrackId.hpp"
#include "WsfWeapon.hpp"

//! The WsfLaunchComputer assists in deciding when to fire a given weapon at a WsfTrack.
//! The WsfLaunchComputer is intrinsic to a specific weapon type, and supplies estimated
//! data to be used in deciding whether to fire a weapon against a particular Track.
//! Each weapon type will have unique kinematic capabilities and limitations, so this base
//! class will need to be extended to correctly fire specific weapon types.  The base
//! virtual double TimeToIntercept() function is of generic use, and may be overridden in
//! a derived class, if the base method is not of high enough fidelity.  Other decision
//! criteria accessors may be added to the class later, such as Probability of Kill, etc.

class WSF_MIL_EXPORT WsfLaunchComputer : public WsfObject
{
protected:
   enum
   {
      cVF_LAUNCH_TIME         = 0X0001,
      cVF_LAUNCHER_BEARING    = 0X0002,
      cVF_LAUNCHER_ELEVATION  = 0X0004,
      cVF_LOFT_ANGLE          = 0X0008,
      cVF_BURN_TIME           = 0X0010,
      cVF_TIME_OF_FLIGHT      = 0X0020,
      cVF_INTERCEPT_TIME      = 0X0040,
      cVF_INTERCEPT_POINT     = 0X0080,
      cVF_TARGET_IMPACT_TIME  = 0X0100,
      cVF_TARGET_IMPACT_POINT = 0X0200,
      cVF_END_TIME            = 0X0400,
      cVF_END_POINT           = 0X0800
   };

   bool ResultIsValidFor(unsigned int aDataMask) const { return (mValidityFlags & aDataMask) != 0; }

public:
   static std::string BaseTypeName() { return "WSF_LAUNCH_COMPUTER"; }

   //! Constant indicating a large amount of time, or "will never get there".
   static const double cFOREVER;

   //! Constant indicating an unattainable distance, or "a long way away".
   static const double cOUT_OF_REACH;

   //! A constant indicating a value is undefined.
   static const double cUNDEFINED_DOUBLE;

   enum Constraints
   {
      cMAX_DELTA_ALTITUDE,   //!< Constraint index indicating the highest attainable target altitude.
      cMIN_DELTA_ALTITUDE,   //!< Constraint index indicating the lowest attainable target altitude.
      cMIN_OPENING_SPEED,    //!< Constraint index indicating the minimum opening speed (positive = diverging).
      cMAX_OPENING_SPEED,    //!< Constraint index indicating the maximum opening speed (positive = diverging).
      cMAX_SLANT_RANGE,      //!< Constraint index indicating the longest attainable target slant range.
      cMIN_SLANT_RANGE,      //!< Constraint index indicating the longest attainable (2-D) ground range.
      cMAX_BORESIGHT_ANGLE,  //!< Constraint index indicating the maximum successful off-boresight angle.
      cNUMBER_OF_CONSTRAINTS //!< Used internally as a loop counter limit.
   };

   static bool LoadInstance(UtInput& aInput, WsfLaunchComputer*& aLCPtr);

   WsfLaunchComputer();
   WsfLaunchComputer(const WsfLaunchComputer& aSrc);
   ~WsfLaunchComputer() override = default;
   WsfLaunchComputer& operator=(const WsfLaunchComputer&) = delete;

   WsfLaunchComputer* Clone() const override;
   const char*        GetScriptClassName() const override;
   bool               ProcessInput(UtInput& aInput) override;
   virtual bool       Initialize(double aSimTime, WsfWeapon* aWeaponPtr);
   virtual void       Update(double aSimTime);

   //! Return the weapon with which this launch computer is associated.
   WsfWeapon* GetWeapon() const { return mWeaponPtr; }

   //! Return the platform with which this launch computer is associated.
   WsfPlatform* GetPlatform() const { return mWeaponPtr->GetPlatform(); }

   WsfSimulation* GetSimulation() const;

   void SetMaximumDeltaAltitude(double aMaxDeltaAltitude);
   void SetMinimumDeltaAltitude(double aMinDeltaAltitude);
   void SetMaximumClosingSpeed(double aMaxClosingSpeed);
   void SetMaximumOpeningSpeed(double aMaxOpeningSpeed);
   void SetMinimumClosingSpeed(double aMinClosingSpeed);
   void SetMinimumOpeningSpeed(double aMinOpeningSpeed);
   void SetMaximumSlantRange(double aMaxRange);
   void SetMinimumSlantRange(double aMinRange);
   void SetMaximumOffBoresight(double aMaxAngle);
   void SetMaximumTimeOfFlight(double aMaxTOF) { mMaximumTOF = aMaxTOF; }
   void SetThrustDuration(double aThrustTime) { mBurnDuration = aThrustTime; }
   void SetCoastDuration(double aCoastTime) { mCoastTime = aCoastTime; }
   void SetBurnoutSpeed(double aBurnoutSpeed) { mBurnoutSpeed = aBurnoutSpeed; }
   void SetMinTerminalSpeed(double aMinTerminalSpeed) { mMinTerminalSpeed = aMinTerminalSpeed; }

   virtual double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime);
   //! @name Intercept Results
   //! The following methods are used to determine what data the launch computer has computed and
   //! to retrieve the computed results.
   //!
   //! @note Not all launch computers will compute all types of data. Hence the validity methods...
   //@{
   bool HaveResultsFor(const WsfTrackId& aTrackId) const { return aTrackId == mLastPredictionTrackId; }
   bool HaveResultsFor(double aSimTime, const WsfTrackId& aTrackId, double& aTimeToIntercept) const;
   bool LaunchTimeIsValid() const { return ResultIsValidFor(cVF_LAUNCH_TIME); }
   bool LauncherBearingIsValid() const { return ResultIsValidFor(cVF_LAUNCHER_BEARING); }
   bool LauncherElevationIsValid() const { return ResultIsValidFor(cVF_LAUNCHER_ELEVATION); }
   bool LoftAngleIsValid() const { return ResultIsValidFor(cVF_LOFT_ANGLE); }
   bool BurnTimeIsValid() const { return ResultIsValidFor(cVF_BURN_TIME); }
   bool TimeOfFlightIsValid() const { return ResultIsValidFor(cVF_TIME_OF_FLIGHT); }
   bool InterceptTimeIsValid() const { return ResultIsValidFor(cVF_INTERCEPT_TIME); }
   bool InterceptPointIsValid() const { return ResultIsValidFor(cVF_INTERCEPT_POINT); }
   bool TargetImpactTimeIsValid() const { return ResultIsValidFor(cVF_TARGET_IMPACT_TIME); }
   bool TargetImpactPointIsValid() const { return ResultIsValidFor(cVF_TARGET_IMPACT_POINT); }
   bool EndTimeIsValid() const { return ResultIsValidFor(cVF_END_TIME); }
   bool EndPointIsValid() const { return ResultIsValidFor(cVF_END_POINT); }

   double             GetLaunchTime() const { return mLaunchTime; }
   double             GetLauncherBearing() const { return mLauncherBearing; }
   double             GetLauncherElevation() const { return mLauncherElevation; }
   double             GetLoftAngle() const { return mLoftAngle; }
   double             GetBurnTime() const { return mBurnTime; }
   double             GetTimeOfFlight() const { return mTimeOfFlight; }
   double             GetInterceptTime() const { return mInterceptTime; }
   const WsfGeoPoint& GetInterceptPoint() const { return mInterceptPoint; }
   double             GetTargetImpactTime() const { return mTargetImpactTime; }
   const WsfGeoPoint& GetTargetImpactPoint() const { return mTargetImpactPoint; }
   double             GetEndTime() const { return mEndTime; }
   const WsfGeoPoint& GetEndPoint() const { return mEndPoint; }
   //@}

   //! @name Methods to explicitly set the intercept results.
   //! These are typically used by the scriptable launch computer (WsfScriptLaunchComputer) to define the results.
   //! Other derived classes can also use them, but they have direct access to much of the data as the results are
   //! stored as 'protected' members.
   //@{
   void SetInterceptTime(double aInterceptTime)
   {
      mInterceptTime = aInterceptTime;
      mValidityFlags |= cVF_INTERCEPT_TIME;
   }

   void SetInterceptPoint(const WsfGeoPoint& aInterceptPoint)
   {
      mInterceptPoint = aInterceptPoint;
      mValidityFlags |= cVF_INTERCEPT_POINT;
   }

   void SetInterceptPointWCS(const double aLocationWCS[])
   {
      mInterceptPoint.SetLocationWCS(aLocationWCS);
      mValidityFlags |= cVF_INTERCEPT_POINT;
   }

   void SetLaunchTime(double aLaunchTime)
   {
      mLaunchTime = aLaunchTime;
      mValidityFlags |= cVF_LAUNCH_TIME;
   }

   void SetTimeOfFlight(double aTimeOfFlight)
   {
      mTimeOfFlight = aTimeOfFlight;
      mValidityFlags |= cVF_TIME_OF_FLIGHT;
   }

   void SetTargetImpactTime(double aImpactTime)
   {
      mTargetImpactTime = aImpactTime;
      mValidityFlags |= cVF_TARGET_IMPACT_TIME;
   }

   void SetLoftAngle(double aLoftAngle)
   {
      mLoftAngle = aLoftAngle;
      mValidityFlags |= cVF_LOFT_ANGLE;
   }

   void SetBurnTime(double aBurnTime)
   {
      mBurnTime = aBurnTime;
      mValidityFlags |= cVF_BURN_TIME;
   }

   void SetLauncherBearing(double aBearing)
   {
      mLauncherBearing = aBearing;
      mValidityFlags |= cVF_LAUNCHER_BEARING;
   }

   void SetLauncherElevation(double aElevation)
   {
      mLauncherElevation = aElevation;
      mValidityFlags |= cVF_LAUNCHER_ELEVATION;
   }

   void SetTargetImpactPoint(const WsfGeoPoint& aImpactPoint)
   {
      mTargetImpactPoint = aImpactPoint;
      mValidityFlags |= cVF_TARGET_IMPACT_POINT;
   }

   void SetTargetImpactPointWCS(const double aLocationWCS[])
   {
      mTargetImpactPoint.SetLocationWCS(aLocationWCS);
      mValidityFlags |= cVF_TARGET_IMPACT_POINT;
   }

   void SetEndTime(double aEndTime)
   {
      mEndTime = aEndTime;
      mValidityFlags |= cVF_END_TIME;
   }

   void SetEndPoint(const WsfGeoPoint& aLocation)
   {
      mEndPoint = aLocation;
      mValidityFlags |= cVF_END_POINT;
   }

   void SetEndPointWCS(const double aLocationWCS[])
   {
      mEndPoint.SetLocationWCS(aLocationWCS);
      mValidityFlags |= cVF_END_POINT;
   }
   //@}

   //! @name Methods to set and get results from a track.
   //! Results can be copied into a track (as aux_data) where it can subsequently be retrieved by the launched weapon.
   //! This is mostly used by the ballistic missile capability which needs to know the loft angle and time.
   //@{
   void CopyResultsToTrack(WsfTrack& aTrack);

   static bool GetLauncherBearing(const WsfTrack& aTrack, double& aBearing);
   static bool GetLauncherElevation(const WsfTrack& aTrack, double& aElevation);
   static bool GetLoftAngle(const WsfTrack& aTrack, double& aLoftAngle);
   static bool GetBurnTime(const WsfTrack& aTrack, double& aBurnTime);
   static bool GetInterceptTime(const WsfTrack& aTrack, double& aInterceptTime);
   static bool GetInterceptPoint(const WsfTrack& aTrack, WsfGeoPoint& aPoint);
   //@}

   //! @name Explicit commands for launch computer results.
   //! These commands are used by 'weapon_tools' to force certain conditions to be used.
   //! They also have input commands that can be used to override the launch computer for testing.
   //@{
   void ClearCommandedValues() { mValidityFlags = 0; }
   void SetCommandedLauncherBearing(double aBearing)
   {
      mCommandedLauncherBearing = aBearing;
      mValidityFlags |= cVF_LAUNCHER_BEARING;
   }
   void SetCommandedLauncherElevation(double aElevation)
   {
      mCommandedLauncherElevation = aElevation;
      mValidityFlags |= cVF_LAUNCHER_ELEVATION;
   }
   void SetCommandedLoftAngle(double aLoftAngle)
   {
      mCommandedLoftAngle = aLoftAngle;
      mValidityFlags |= cVF_LOFT_ANGLE;
   }
   void SetCommandedBurnTime(double aBurnTime)
   {
      mCommandedBurnTime = aBurnTime;
      mValidityFlags |= cVF_BURN_TIME;
   }
   void SetCommandedInterceptPoint(const WsfGeoPoint& aPoint)
   {
      mCommandedInterceptPoint = aPoint;
      mValidityFlags |= cVF_INTERCEPT_POINT;
   }
   //@}

   bool DebugEnabled() const { return mDebugEnabled; }
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   bool ShowResults() const { return mShowResults; }

   //! @name Computer generation control.
   //! These methods are used to control if computer generation mode is active. 'weapon_tools' will enable
   //! this when it is generating launch computers.
   //@{
   bool ComputerGenerationMode() const { return mComputerGenerationMode; }
   void SetComputerGenerationMode(bool aComputerGenerationMode) { mComputerGenerationMode = aComputerGenerationMode; }
   //@}

protected:
   static bool GetDataValue(const WsfTrack& aTrack, const std::string& aName, double& aValue, double aMultiplier);

   void InitializeResults(double aSimTime, const WsfTrack& aTrack);

   void UpdateResults(double aSimTime, double aTimeToIntercept, const WsfTrack& aTrack);

   void ClearConstraints();
   void CopyConstraints(const bool aSrc[]);

   double ExpectedFlightDistance() const { return mExpectedFlightDistance; }
   double CurrentOpeningSpeed() const { return mCurrentOpeningSpeed; }

   virtual bool InitializeTTIData();
   virtual bool WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack);

   bool DeltaAltitudeCheck(double aDeltaAltitude) const;
   bool SlantRangeCheck(double aSlantRange) const;
   bool BoresightAngleCheck(WsfPlatform* aPlatPtr, double aLocWCS[3]) const;

   WsfWeapon* mWeaponPtr;

   bool mDebugEnabled;
   bool mShowResults;
   bool mComputerGenerationMode;
   bool mConstraintEnabled[cNUMBER_OF_CONSTRAINTS];

   //! The amount of time a prediction can be locked-in before it must be recomputed.
   //! This is used indirectly by the WsfWeapon::CueToTarget to determine if the launch solution
   //! should be recomputed.
   double mPredictionLockinTime;

   double mMaxDeltaAltitude;
   double mMinDeltaAltitude;
   double mMinOpeningSpeed;
   double mMaxOpeningSpeed;
   double mMaxSlantRange;
   double mMinSlantRange;
   double mCosineMaxBoresight;

   double mMaximumTOF;
   double mAvgThrustingAccel;
   double mBurnoutSpeed;
   double mBurnDuration;
   double mCoastTime;
   double mAvgCoastingAccel;
   double mMinTerminalSpeed;

   //! @name Predicted intercept results
   //@{
   double       mLastPredictionTime;
   WsfTrackId   mLastPredictionTrackId;
   unsigned int mValidityFlags;
   double       mLaunchTime;
   double       mLauncherBearing;
   double       mLauncherElevation;
   double       mLoftAngle;
   double       mBurnTime;
   double       mTimeOfFlight;
   double       mInterceptTime;
   WsfGeoPoint  mInterceptPoint;
   double       mTargetImpactTime;
   WsfGeoPoint  mTargetImpactPoint;
   double       mEndTime;
   WsfGeoPoint  mEndPoint;
   //@}

   //! @name Commanded predicted intercept results
   //@{
   double      mCommandedLauncherBearing;
   double      mCommandedLauncherElevation;
   double      mCommandedLoftAngle;
   double      mCommandedBurnTime;
   WsfGeoPoint mCommandedInterceptPoint;
   //@}

private:
   double mExpectedFlightDistance;
   double mCurrentOpeningSpeed;
   double mThrustingDistance;
   double mCoastingDistance;
};

#endif
