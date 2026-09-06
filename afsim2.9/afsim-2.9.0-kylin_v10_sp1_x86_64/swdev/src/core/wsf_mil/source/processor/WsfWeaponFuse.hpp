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

#ifndef WSFWEAPONFUSE_HPP
#define WSFWEAPONFUSE_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <string>

#include "UtAtmosphere.hpp"
class UtInput;
class UtScriptClass;
class UtScriptTypes;
#include "WsfCategoryList.hpp"
class WsfPlatform;
#include "script/WsfScriptProcessor.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"

namespace wsf
{
class Terrain;
}

//! A class for a fuse, which initiates the detonation of a weapon.
//!
//! This provides a simple implementation of a fuse which can initiate
//! detonation based on:
//!
//! - Crossing a specified Mean Sea Level (MSL) Altitude.
//! - Crossing a specified Above Ground Level (AGL) Altitude.
//! - Exceeding a specified Time of Flight
//! - Proximity to a target.
//! - Explicit commanded
//!
//! The fuse can have delayed arming (mTOF_ToArm > 0). If the fuse is detonated
//! before it is armed, the detonation will be considered a "dud".
//! Altitude criteria and target proximity are not considered until the fuse is armed.
//!
//! @note This does not implement a real proximity fuse which uses some sort of sensing
//! mechanism. It simply considers when the weapon has reached its point of closest approach.

class WSF_MIL_EXPORT WsfWeaponFuse : public WsfScriptProcessor
{
public:
   //! The AGL/MSL detonation criteria
   enum AltCriteria
   {
      cNO_LIMIT,
      cDETONATE_ABOVE,
      cDETONATE_BELOW
   };

   //! The Minimum Speed detonation criteria
   enum SpeedCriteria
   {
      cSC_NONE,
      cSC_SPEED,
      cSC_MACH,
      cSC_MACH_DECAY
   };

   //! Reason for the final state of the fuse object.
   enum Reason
   {
      cNO_REASON,        //!< Fuse has not attempted detonation yet.
      cPROXIMITY,        //!< Proximity to a target initiated the detonation.
      cAGL_LIMIT,        //!< Crossing an Above Ground Level (AGL) limit initiated detonation.
      cMSL_LIMIT,        //!< Crossing a Mean Sea Level (MSL) limit initiated detonation.
      cMIN_SPEED,        //!< Weapon speed fell below its minimum 'effective' value, initiating detonation.
      cTOF_LIMIT,        //!< Maximum Time Of Flight (TOF) was exceeded, initiating detonation.
      cCOAST_TIME_LIMIT, //!< Coast time limit exceeded, initiating detonation.
      cCOMMAND           //!< Detonated by command.
   };

   enum OnIntercept
   {
      cACTION_DETONATE, //!< Default normal behavior, upon Intercept, detonate.
      cACTION_HALT      //!< Upon Intercept, halt/freeze motion.
   };

   WsfWeaponFuse(WsfScenario& aScenario);
   WsfWeaponFuse(const WsfWeaponFuse& aSrc);
   ~WsfWeaponFuse() override;
   WsfWeaponFuse& operator=(const WsfWeaponFuse&) = delete;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   const char* GetScriptClassName() const override { return "WsfWeaponFuse"; }

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   void          Update(double aSimTime) override;

   void Detonate(double aSimTime);
   void Arm(double aSimTime);

   //! Access the reason why the fuse arrived at its current State.
   Reason GetReason() const { return mReason; }

   //! Access the values used to determine detonation based on Time, Altitude, and Speed
   double        GetMaximumTimeOfFlight() const { return mMaxTOF; }
   SpeedCriteria GetSpeedCriteria() const { return mMinSpeedCriteria; }
   double        GetMinSpeedLimit() const { return mMinSpeedLimit; }
   AltCriteria   GetMSL_Criteria() const { return mMSL_Criteria; }
   AltCriteria   GetAGL_Criteria() const { return mAGL_Criteria; }
   double        GetAGL_Limit() const { return mAGL_Limit; }
   double        GetMSL_Limit() const { return mMSL_Limit; }
   double        GetTimeOfFlightToArm() const { return mTOF_ToArm; }

   //! @name The following methods are only guaranteed to work before Initialize().
   //@
   void SetAirToGroundMode(bool aValue);
   void SetAGL_Criteria(AltCriteria aCriteria, double aAltAGL);
   void SetMSL_Criteria(AltCriteria aCriteria, double aAltMSL);
   void SetTargetName(WsfStringId aNameId);
   void SetUseCurrentTarget(bool aValue);
   void SetGrossProximityRange(double aRange) { mGrossProximityRange = aRange; }
   void SetHitProximityRange(double aRange) { mHitProximityRange = aRange; }
   void SetGrossUpdateInterval(double aInterval) { mGrossUpdateInterval = aInterval; }
   void SetFineUpdateInterval(double aInterval);
   void SetTOFToArm(double aTOF) { mTOF_ToArm = aTOF; }
   //@}

protected:
   //! What to do if the current target/named target is lost.
   //! Proximity check instruction:  What to do if the current target/named target is lost?
   enum TargetLossOption
   {
      cDETONATE,                //!< Detonate.
      cCOAST,                   //!< Coast for a specified period of time while waiting for the target to reappear.
      cCANCEL_PROXIMITY_CHECKS, //!< Quit considering proximity.
      cCONSIDER_ALL_PROXIMITY   //!< Revert to checking all existing platforms (CPU Intensive)
   };

   void         CheckForDetonation(double aSimTime);
   virtual void PrivateDetonate(double aSimTime);
   void         PrintDetonationReason(double aSimTime);
   bool         ProcessAltAGL(double aSimTime);
   bool         ProcessAltMSL(double aSimTime);
   void         ProcessMinSpeed(double aSimTime);
   bool         ProcessProximity(double aSimTime);

   bool CheckAllTargets(double aSimTime);

   double ComputeGrossProximityRange(const double aTgtVelWCS[3]) const;

   double ComputeTimeToPCAorFuse(const double aTgtLocWCS[3], const double aTgtVelWCS[3]) const;

   void SelectPCA_ForDetonation(double       aSimTime,
                                WsfPlatform* aTgtPtr,
                                const double aTgtLocWCS[3],
                                const double aTgtVelWCS[3],
                                double       aTimeToPCA,
                                double       aMissDistanceLimit);

   void ShowPCA_Results(double       aSimTime,
                        WsfPlatform* aTgtPtr,
                        const double aTgtLocWCS[3],
                        const double aTgtVelWCS[3],
                        double       aTimeToPCA) const;

   bool UpdateTargetState(double aSimTime);
   void UpdateWeaponState(double aSimTime);

   void PermanentlyArrestMotion(double aSimTime);


private:
   //! @name Input data
   //@{
   TargetLossOption mTargetLossOption;
   AltCriteria      mMSL_Criteria;
   AltCriteria      mAGL_Criteria;
   SpeedCriteria    mMinSpeedCriteria;

   WsfStringId mTgtNameId;
   bool        mUseCurTgtOnly;
   bool        mATG_Mode;
   OnIntercept mOnIntercept; // defaulted to cACTION_DETONATE.

   double mGrossUpdateInterval;
   double mFineUpdateInterval;
   double mGrossProximityRange;
   double mHitProximityRange;
   double mFuseFunctionRange; // mandatory detonation upon approaching

   double          mMSL_Limit;
   double          mAGL_Limit;
   double          mMinSpeedLimit;
   double          mTOF_ToArm;
   double          mMaxTOF;
   double          mCoastTime;
   WsfCategoryList mInvincibleCats;
   //@}

   //! @name Dynamic data.
   //! This data is NOT copy constructed.
   //@{

   // General dynamic data.

   wsf::Terrain* mTerrainPtr;
   Reason        mReason;
   //
   // The following two variables define the minimum and maximum time offsets from PCA
   // when a hit will be declared.
   double mPCA_HitMinTime;
   double mPCA_HitMaxTime;
   size_t mLauncherIndex;
   double mLaunchTime;
   double mLastUpdateTime;
   double mTimeStartedMoving;
   double mArmTime;
   bool   mIsArmed;
   bool   mForceDetonate;
   bool   mProximityHit;
   bool   mAGL_Hit;
   bool   mMSL_Hit;
   bool   mTOF_Hit;
   bool   mMinSpeedHit;

   // Dynamic weapon data.
   UtAtmosphere mAtmosphere;
   double       mThisWpnAltMSL;
   double       mLastWpnAltMSL;
   double       mThisWpnAltAGL;
   double       mThisWpnSpeed;
   double       mLastWpnSpeed;
   double       mThisWpnLocWCS[3];
   double       mLastWpnLocWCS[3];
   double       mThisWpnVelWCS[3];
   double       mLastAGL_Delta;
   double       mThisAGL_Delta;
   double       mLastMSL_Delta;
   double       mThisMSL_Delta;
   double       mCapturedMaxSpeed;
   unsigned int mAGL_UpdateCount;
   unsigned int mMSL_UpdateCount;

   // Dynamic target data.
   WsfTrackId   mTgtTrackId;
   WsfPlatform* mTgtPtr;
   size_t       mTgtIndex;
   size_t       mLastTgtIndex;
   double       mTgtLocWCS[3];
   double       mTgtVelWCS[3];
   double       mTgtAltAGL;
   double       mLastGoodTgtUpdateTime;
   bool         mTgtAltSet;

   // PCA Data from previous update
   double mLastPCA_Time;
   double mLastPCA_WpnLocWCS[3];
   double mLastPCA_TgtLocWCS[3];

   // Proximity detonation data.
   double mDetTime;
   double mDetWpnLocWCS[3];
   double mDetTgtLocWCS[3];
   //@}
};

#endif
