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

// NOTE - Depending on the type of simulation, invoking WsfPlatform::Update(aSimTime) may
// not result in the platform being moved forward to the requested time. For instance, in
// a frame-stepped simulation, movement updates only occur at discrete times. Because this
// may get called in between updates, and because this is attempting to detect collision
// between objects that are potentially moving very fast, platform positions are extrapolated
// forward using the difference between the current simulation time and the time the platform
// was last updated.

#include "WsfWeaponFuse.hpp"

#include <cassert>
#include <iostream>

#include "UtInput.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessorClass.hpp"

namespace
{
const double cDISTANCE_RESOLUTION = 1.00; // meters
const double cCONTACT             = 2.50; // meters (considered a direct hit)

// If the detonation point is within this distance of the ground,
// but not on the ground, then it is considered to be 'in proximity' of the ground.
const double cGROUND_PROXIMITY_LIMIT = 10.0; // meters

const double cIN_THE_BALLPARK = 25.0; // Close enough to an intercept to reduce the delta t.
// (Estimated number of Update_Interval steps to intercept.)
const double cFAR_AWAY = 1.0E12;

const double cMODULUS_EPSILON = 1.0E-3;

std::ostream& operator<<(std::ostream& aOut, const double aVec3d[3])
{
   aOut << "[ " << aVec3d[0] << ' ' << aVec3d[1] << ' ' << aVec3d[2] << " ]";
   return aOut;
}
} // namespace

// ================================================================================================
//! Constructor
WsfWeaponFuse::WsfWeaponFuse(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario)
   , mTargetLossOption(cCOAST)
   , mMSL_Criteria(cNO_LIMIT)
   , mAGL_Criteria(cNO_LIMIT)
   , mMinSpeedCriteria(cSC_NONE)
   , mTgtNameId(nullptr)
   , mUseCurTgtOnly(true)
   , mATG_Mode(false)
   , mOnIntercept(cACTION_DETONATE)
   , mGrossUpdateInterval(1.0)
   , mFineUpdateInterval(0.05)
   , mGrossProximityRange(0.0)
   , mHitProximityRange(100.0)
   , mFuseFunctionRange(0.0)
   , mMSL_Limit(0.0)
   , mAGL_Limit(0.0)
   , mMinSpeedLimit(0.0)
   , mTOF_ToArm(0.0)
   , mMaxTOF(1.0E+10)
   , mCoastTime(2.0)
   , mInvincibleCats()
   ,
   // All of the remaining data is dynamic data that is re-initialized by the copy constructor.
   mTerrainPtr(nullptr)
   , mReason(cNO_REASON)
   , mPCA_HitMinTime(0.0)
   , mPCA_HitMaxTime(0.0)
   , mLauncherIndex(0)
   , mLaunchTime(0.0)
   , mLastUpdateTime(0.0)
   , mTimeStartedMoving(-1.0)
   , mArmTime(-1.0)
   , mIsArmed(false)
   , mProximityHit(false)
   , mAGL_Hit(false)
   , mMSL_Hit(false)
   , mTOF_Hit(false)
   , mMinSpeedHit(false)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mThisWpnAltMSL(0.0)
   , mLastWpnAltMSL(0.0)
   , mThisWpnAltAGL(0.0)
   , mThisWpnSpeed(0.0)
   , mLastWpnSpeed(0.0)
   , mLastAGL_Delta(0.0)
   , mThisAGL_Delta(0.0)
   , mLastMSL_Delta(0.0)
   , mThisMSL_Delta(0.0)
   , mCapturedMaxSpeed(-1.0)
   , mAGL_UpdateCount(0)
   , mMSL_UpdateCount(0)
   , mTgtTrackId()
   , mTgtPtr(nullptr)
   , mTgtIndex(0)
   , mLastTgtIndex(0)
   , mTgtAltAGL(0.0)
   , mLastGoodTgtUpdateTime(-1.0)
   , mTgtAltSet(false)
   , mLastPCA_Time(-1.0)
   , mDetTime(-1.0)
{
   SetFineUpdateInterval(mFineUpdateInterval); // Forces computation of PCA criteria.
   UtVec3d::Set(mThisWpnLocWCS, 0.0);
   UtVec3d::Set(mLastWpnLocWCS, 0.0);
   UtVec3d::Set(mThisWpnVelWCS, 0.0);
   UtVec3d::Set(mTgtLocWCS, 0.0);
   UtVec3d::Set(mTgtVelWCS, 0.0);
   UtVec3d::Set(mLastPCA_WpnLocWCS, 0.0);
   UtVec3d::Set(mLastPCA_TgtLocWCS, 0.0);
   UtVec3d::Set(mDetWpnLocWCS, 0.0);
   UtVec3d::Set(mDetTgtLocWCS, 0.0);
}

// ================================================================================================
//! Copy constructor.
WsfWeaponFuse::WsfWeaponFuse(const WsfWeaponFuse& aSrc)
   : WsfScriptProcessor(aSrc)
   , mTargetLossOption(aSrc.mTargetLossOption)
   , mMSL_Criteria(aSrc.mMSL_Criteria)
   , mAGL_Criteria(aSrc.mAGL_Criteria)
   , mMinSpeedCriteria(aSrc.mMinSpeedCriteria)
   , mTgtNameId(aSrc.mTgtNameId)
   , mUseCurTgtOnly(aSrc.mUseCurTgtOnly)
   , mATG_Mode(aSrc.mATG_Mode)
   , mOnIntercept(aSrc.mOnIntercept)
   , mGrossUpdateInterval(aSrc.mGrossUpdateInterval)
   , mFineUpdateInterval(aSrc.mFineUpdateInterval)
   , mGrossProximityRange(aSrc.mGrossProximityRange)
   , mHitProximityRange(aSrc.mHitProximityRange)
   , mFuseFunctionRange(aSrc.mFuseFunctionRange)
   , mMSL_Limit(aSrc.mMSL_Limit)
   , mAGL_Limit(aSrc.mAGL_Limit)
   , mMinSpeedLimit(aSrc.mMinSpeedLimit)
   , mTOF_ToArm(aSrc.mTOF_ToArm)
   , mMaxTOF(aSrc.mMaxTOF)
   , mCoastTime(aSrc.mCoastTime)
   , mInvincibleCats(aSrc.mInvincibleCats)
   ,
   // All of the remaining data is dynamic run-time data that should not be copy constructed.
   mTerrainPtr(nullptr)
   , mReason(cNO_REASON)
   , mPCA_HitMinTime(0.0)
   , mPCA_HitMaxTime(0.0)
   , mLauncherIndex(0)
   , mLaunchTime(0.0)
   , mLastUpdateTime(0.0)
   , mTimeStartedMoving(-1.0)
   , mArmTime(-1.0)
   , mIsArmed(false)
   , mForceDetonate(false)
   , mProximityHit(false)
   , mAGL_Hit(false)
   , mMSL_Hit(false)
   , mTOF_Hit(false)
   , mMinSpeedHit(false)
   , mAtmosphere(aSrc.mAtmosphere)
   , mThisWpnAltMSL(0.0)
   , mLastWpnAltMSL(0.0)
   , mThisWpnAltAGL(0.0)
   , mThisWpnSpeed(0.0)
   , mLastWpnSpeed(0.0)
   , mLastAGL_Delta(0.0)
   , mThisAGL_Delta(0.0)
   , mLastMSL_Delta(0.0)
   , mThisMSL_Delta(0.0)
   , mCapturedMaxSpeed(-1.0)
   , mAGL_UpdateCount(0)
   , mMSL_UpdateCount(0)
   , mTgtTrackId()
   , mTgtPtr(nullptr)
   , mTgtIndex(0)
   , mLastTgtIndex(0)
   , mTgtAltAGL(0.0)
   , mLastGoodTgtUpdateTime(-1.0)
   , mTgtAltSet(false)
   , mLastPCA_Time(-1.0)
   , mDetTime(-1.0)
{
   SetFineUpdateInterval(mFineUpdateInterval); // Forces computation of PCA criteria.
   UtVec3d::Set(mThisWpnLocWCS, 0.0);
   UtVec3d::Set(mLastWpnLocWCS, 0.0);
   UtVec3d::Set(mThisWpnVelWCS, 0.0);
   UtVec3d::Set(mTgtLocWCS, 0.0);
   UtVec3d::Set(mTgtVelWCS, 0.0);
   UtVec3d::Set(mLastPCA_WpnLocWCS, 0.0);
   UtVec3d::Set(mLastPCA_TgtLocWCS, 0.0);
   UtVec3d::Set(mDetWpnLocWCS, 0.0);
   UtVec3d::Set(mDetTgtLocWCS, 0.0);
}

// ================================================================================================
// virtual
WsfWeaponFuse::~WsfWeaponFuse()
{
   delete mTerrainPtr;
}

// ================================================================================================
WsfProcessor* WsfWeaponFuse::Clone() const
{
   return new WsfWeaponFuse(*this);
}

// ================================================================================================
// virtual
bool WsfWeaponFuse::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   mTerrainPtr                        = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   mLaunchTime                        = aSimTime;
   mLastUpdateTime                    = aSimTime;
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
   if (engagementPtr != nullptr)
   {
      mLauncherIndex = engagementPtr->GetFiringPlatformIndex();
   }

   if (mGrossUpdateInterval < mFineUpdateInterval)
   {
      ok       = false;
      auto out = ut::log::warning() << "The 'gross_update_interval' must be greater than 'fine_update_interval'.";
      out.AddNote() << "Gross: " << mGrossUpdateInterval;
      out.AddNote() << "Fine: " << mFineUpdateInterval;
      out.AddNote() << "Using the set 'fine_update_interval' for both.";

      mGrossUpdateInterval = mFineUpdateInterval;
   }

   // Don't let the fine update interval be less than the simulation timestep as the resulting PCA window
   // may be too small to detect the PCA. (The timestep is zero for an event-stepped simulation).
   if (mFineUpdateInterval < GetSimulation()->GetTimestep())
   {
      SetFineUpdateInterval(GetSimulation()->GetTimestep()); // Set interval and compute PCA window
      mGrossUpdateInterval = std::max(mFineUpdateInterval, mGrossUpdateInterval);
   }
   SetUpdateInterval(mGrossUpdateInterval);

   if (mATG_Mode)
   {
      // Air to ground mode forces detonation upon descending through AGL = <zero, or current target height>.
      // Check for the presence of conflicting constraints:

      if (mAGL_Criteria == cDETONATE_ABOVE)
      {
         ok = false;
         ut::log::error() << "Incompatible inputs with 'air_to_ground_mode' set, and 'detonate_above_height_agl' set.";
      }
      else if (mAGL_Limit != 0.0)
      {
         ok = false;
         ut::log::error()
            << "Incompatible inputs, with 'air_to_ground_mode' set, and non-zero 'detonate_above_height_agl' set.";
      }
      else
      {
         mAGL_Criteria = cDETONATE_BELOW;
         mAGL_Limit    = 0.0;
      }
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfWeaponFuse::Initialize2(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize2(aSimTime);
   ;

   // Get any launch computer data that may be used from the hand-off track.
   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      if (mMaxTOF < 0.0)
      {
         // Maximum time-of-flight determined by the launch computer. 'mMaxTOF' is the
         // NEGATIVE of the time to be added to the flyout time.

         double interceptTime;
         if (WsfLaunchComputer::GetInterceptTime(*trackPtr, interceptTime))
         {
            mMaxTOF = std::max(0.0, interceptTime - aSimTime - mMaxTOF);
         }
      }
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfWeaponFuse::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "time_of_flight_to_arm")
   {
      aInput.ReadValueOfType(mTOF_ToArm, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTOF_ToArm, 0.0);
   }
   else if ((command == "maximum_time_of_flight") || (command == "max_time_of_flight_to_detonate"))
   {
      std::string strValue;
      aInput.ReadValue(strValue);
      if (strValue == "from_launch_computer")
      {
         mMaxTOF = -4.0;
         aInput.ReadValue(strValue);
         if (strValue == "delay")
         {
            aInput.ReadValueOfType(mMaxTOF, UtInput::cTIME);
            aInput.ValueGreater(mMaxTOF, 0.0);
            mMaxTOF = -mMaxTOF;
         }
         else
         {
            aInput.PushBack(strValue);
         }
      }
      else
      {
         aInput.PushBack(strValue);
         aInput.ReadValueOfType(mMaxTOF, UtInput::cTIME);
         aInput.ValueGreater(mMaxTOF, 0.0);
      }
   }
   else if ((command == "detonate_above_height_AGL") || (command == "detonate_above_height_agl"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetAGL_Criteria(cDETONATE_ABOVE, value);
   }
   else if ((command == "detonate_below_height_AGL") || (command == "detonate_below_height_agl"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      SetAGL_Criteria(cDETONATE_BELOW, value);
   }
   else if ((command == "detonate_above_height_MSL") || (command == "detonate_above_height_msl"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetMSL_Criteria(cDETONATE_ABOVE, value);
   }
   else if ((command == "detonate_below_height_MSL") || (command == "detonate_below_height_msl"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      SetMSL_Criteria(cDETONATE_BELOW, value);
   }
   else if (command == "detonate_below_speed")
   {
      aInput.ReadValueOfType(mMinSpeedLimit, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinSpeedLimit, 0.0);
      mMinSpeedCriteria = cSC_SPEED;
   }
   else if (command == "detonate_below_mach")
   {
      aInput.ReadValue(mMinSpeedLimit);
      aInput.ValueGreaterOrEqual(mMinSpeedLimit, 0.0);
      mMinSpeedCriteria = cSC_MACH;
   }
   else if (command == "detonate_on_mach_decreasing_to")
   {
      aInput.ReadValue(mMinSpeedLimit);
      aInput.ValueGreaterOrEqual(mMinSpeedLimit, 0.0);
      mMinSpeedCriteria = cSC_MACH_DECAY;
   }
   else if (command == "hit_proximity_range")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreater(value, 0.0);
      SetHitProximityRange(value);
   }
   else if (command == "gross_proximity_range")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetGrossProximityRange(value);
   }
   else if (command == "fuse_function_range")
   {
      aInput.ReadValueOfType(mFuseFunctionRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mFuseFunctionRange, 0.0);
   }
   else if ((command == "update_interval") || (command == "gross_update_interval"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreater(value, 0.0);
      SetGrossUpdateInterval(value);
   }
   else if (command == "fine_update_interval")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreater(value, 0.0);
      SetFineUpdateInterval(value);
   }
   else if (command == "disable_launcher_proximity_detonation")
   {
      ut::log::warning() << "'disable_launch_proximity_detonation' has been deprecated.";
   }
   else if (command == "do_not_use_current_target")
   {
      SetUseCurrentTarget(false);
   }
   else if (command == "use_current_target")
   {
      SetUseCurrentTarget(true);
   }
   else if (command == "detonate_on_loss_of_target")
   {
      mTargetLossOption = cDETONATE;
   }
   else if (command == "coast_time_on_loss_of_target")
   {
      mTargetLossOption = cCOAST;
      aInput.ReadValueOfType(mCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mCoastTime, 0.0);
   }
   else if (command == "proximity_cancel_on_loss_of_target")
   {
      mTargetLossOption = cCANCEL_PROXIMITY_CHECKS;
   }
   else if (command == "proximity_all_on_loss_of_target")
   {
      mTargetLossOption = cCONSIDER_ALL_PROXIMITY;
   }
   else if (command == "on_intercept")
   {
      std::string option;
      aInput.ReadValue(option);
      if (option == "detonate")
      {
         // This is default behavior
         mOnIntercept = cACTION_DETONATE;
      }
      else if (option == "halt")
      {
         mOnIntercept = cACTION_HALT;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown 'on_intercept' action '" + option + "'.");
      }
   }
   else if (command == "air_to_ground_mode")
   {
      SetAirToGroundMode(true);
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else if (command == "exclusive_target_name")
   {
      std::string targetName;
      aInput.ReadValue(targetName);
      SetTargetName(targetName);
   }
   else if (command == "excluded_category")
   {
      std::string categoryName;
      aInput.ReadValue(categoryName);
      mInvincibleCats.JoinCategory(categoryName);
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

// ================================================================================================
// This is called by the periodic Update for the processor.
// virtual
void WsfWeaponFuse::Update(double aSimTime)
{
   if (!IsTurnedOn())
   {
      return;
   }
   // Ensure the platform location is current.
   GetPlatform()->Update(aSimTime);
   CheckForDetonation(aSimTime);
}

// ================================================================================================
//! Commanded detonation occurs immediately, as long as fuse is armed, but the
//! weapon platform will be removed, even if the command results in a dud.
void WsfWeaponFuse::Detonate(double aSimTime)
{
   mReason        = cCOMMAND;
   mForceDetonate = true;
   CheckForDetonation(aSimTime);
}

// ================================================================================================
//! If the fuse is currently safed, command transition to the armed state.
// virtual
void WsfWeaponFuse::Arm(double aSimTime)
{
   if (mArmTime < 0.0)
   {
      mArmTime = aSimTime;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Fuse Armed.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }
}

// ================================================================================================
void WsfWeaponFuse::SetAirToGroundMode(bool aValue)
{
   mATG_Mode = aValue;
   if (mATG_Mode)
   {
      // In order to cause detonations at the height of the current target,
      // we must be calculating AGL values for a hit (plus the model height offset)
      // The value for mAGL_Limit will NOT be used.
      mAGL_Criteria = cDETONATE_BELOW;
      mAGL_Limit    = 0.0;

      // We have to offset the ground impact by the height of a specified target
      // type.  For this to happen, we must enable EITHER use_current_target, or
      // specify a target name ID, but not both...
      if (!mTgtNameId)
      {
         mUseCurTgtOnly = true;
      }
   }
}

// ================================================================================================
void WsfWeaponFuse::SetAGL_Criteria(AltCriteria aCriteria, double aAltAGL)
{
   mAGL_Limit    = aAltAGL;
   mAGL_Criteria = aCriteria;
}

// ================================================================================================
void WsfWeaponFuse::SetMSL_Criteria(AltCriteria aCriteria, double aAltMSL)
{
   mMSL_Limit    = aAltMSL;
   mMSL_Criteria = aCriteria;
}

// ================================================================================================
void WsfWeaponFuse::SetTargetName(WsfStringId aNameId)
{
   mUseCurTgtOnly = false;
   mTgtNameId     = aNameId;
   mTgtPtr        = nullptr;
   mTgtIndex      = 0;
   mLastTgtIndex  = 0;
}

// ================================================================================================
void WsfWeaponFuse::SetUseCurrentTarget(bool aValue)
{
   mUseCurTgtOnly = aValue;
   mTgtNameId     = nullptr;
   mTgtPtr        = nullptr;
   mTgtIndex      = 0;
   mLastTgtIndex  = 0;
}

// ================================================================================================
void WsfWeaponFuse::SetFineUpdateInterval(double aInterval)
{
   mFineUpdateInterval = aInterval;

   // Set the PCA time window (relative to the time at evaluation) in which a hit will be declared.
   // A hit is generally declared when the current PCA time occurs slightly in the past (i.e.: we have
   // gone slightly past the PCA).
   //
   // We don't want the minimum time to be too large because then we'll end up hitting targets that may be far away.
   mPCA_HitMinTime = std::min(-2.0 * mFineUpdateInterval, -0.001);

   // We allow the maximum time to be slightly in the future because an iteration in which the results are very close
   // are probably more accurate than waiting one more time step.
   mPCA_HitMaxTime = 0.001 * mFineUpdateInterval;
}

// ================================================================================================
// protected
void WsfWeaponFuse::CheckForDetonation(double aSimTime)
{
   // Determine if this is the first motion.
   if (mTimeStartedMoving < 0.0)
   {
      if (!(GetPlatform()->IsStopped() || GetPlatform()->IsPaused()))
      {
         mTimeStartedMoving = aSimTime;
      }
   }

   // Get the current position and speed of the weapon.
   UpdateWeaponState(aSimTime);

   if (mTimeStartedMoving >= 0.0)
   {
      // Decide if it is time to arm the weapon.
      // The fuse arming time is relative to when the platform first starts moving.

      if ((mArmTime < 0.0) && // Fuse in not yet armed
          ((aSimTime - mTimeStartedMoving + 1.0E-4) >= mTOF_ToArm))
      {
         Arm(aSimTime);
      }

      // The time-of-flight criteria is checked even if the fuse is not yet armed.
      // The maximum time of flight is relative to when the platform first starts moving.

      if ((aSimTime - mTimeStartedMoving + 1.0E-4) >= mMaxTOF)
      {
         mTOF_Hit = true;
         mReason  = cTOF_LIMIT;
      }
   }

   bool needFineUpdateInterval = false;
   if ((mArmTime >= 0.0) && // Fuse is armed
       (!mTOF_Hit) && (!mForceDetonate))
   {
      if (mGrossProximityRange != 0.0)
      {
         needFineUpdateInterval |= ProcessProximity(aSimTime);
      }
      if (mAGL_Criteria != cNO_LIMIT)
      {
         needFineUpdateInterval |= ProcessAltAGL(aSimTime);
      }
      if (mMSL_Criteria != cNO_LIMIT)
      {
         needFineUpdateInterval |= ProcessAltMSL(aSimTime);
      }
      if (mMinSpeedCriteria != cSC_NONE)
      {
         ProcessMinSpeed(aSimTime);
      }
   }

   if (mTOF_Hit || mProximityHit || mAGL_Hit || mMSL_Hit || mMinSpeedHit || mForceDetonate)
   {
      PrivateDetonate(aSimTime);
   }
   else if ((needFineUpdateInterval) && (mFineUpdateInterval > 0.0) && (mFineUpdateInterval < mGrossUpdateInterval))
   {
      SetUpdateInterval(mFineUpdateInterval);
   }
   else
   {
      SetUpdateInterval(mGrossUpdateInterval);
   }
   mLastUpdateTime = aSimTime;
}

// ================================================================================================
void WsfWeaponFuse::PermanentlyArrestMotion(double aSimTime)
{
   const double cZERO[] = {0.0, 0.0, 0.0};
   GetPlatform()->SetVelocityWCS(cZERO);
   // This deletes the mover, no further motion is possible.
   GetPlatform()->SetMover(nullptr);
   // Don't need a fuse any more, so turn it off.
   TurnOff(aSimTime);
}

// ================================================================================================
// protected virtual
void WsfWeaponFuse::PrivateDetonate(double aSimTime)
{
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
   if (engagementPtr == nullptr)
   {
      // A platform can have a fuse but not be a weapon (e.g.: the spent stage of a multi-stage
      // rocket is not considered a weapon, but it has a fuse to detect impact with the ground).
      // A platform is designated a 'weapon' if it has an attached weapon effects. If the platform
      // platform does not have an associated weapons effects then we'll simply delete it, which
      // then prevents the spurious WEAPON_FIRED and WEAPON_TERMINATED messages.
      //
      // This is the same convention followed by WsfExplicitWeapon when it launches the platform.
      WsfStringId weaponEffect = WsfWeaponEffects::GetWeaponEffectsType(*GetPlatform());
      if (!weaponEffect)
      {
         if (mOnIntercept == cACTION_DETONATE)
         {
            if (DebugEnabled())
            {
               if (mTOF_Hit || mProximityHit || mAGL_Hit || mMSL_Hit || mMinSpeedHit || mForceDetonate)
               {
                  PrintDetonationReason(aSimTime);
               }
            }
            GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
            return;
         }
         else
         {
            // This permanently deletes the mover!
            PermanentlyArrestMotion(aSimTime);
         }
      }

      // Create our own ad-hoc engagement, assuming we ARE the Weapon Platform:
      WsfTrackId tgtTrackId;
      size_t     tgtIndex = 0;
      if (!mTgtTrackId.IsNull())
      {
         tgtTrackId = mTgtTrackId;
         tgtIndex   = mTgtIndex;
      }
      engagementPtr =
         new WsfWeaponEngagement(aSimTime, GetPlatform()->GetSimulation(), nullptr, tgtTrackId, tgtIndex, weaponEffect);
      if (!WsfWeaponEngagement::Assign(*GetPlatform(), engagementPtr))
      {
         { // RAII block
            auto out = ut::log::error() << "Failed to create weapon engagement object.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Fuse: " << GetName();
         }

         if (mOnIntercept == cACTION_DETONATE)
         {
            GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
         }
         else // if (mOnIntercept == cACTION_HALT)
         {
            PermanentlyArrestMotion(aSimTime);
         }
      }

      // Indicate that the weapon has "fired."
      // This notification is necessary for DIS, so that the fire/detonate event indexes will match.
      const WsfTrack* currentTargetTrackPtr = nullptr;
      if (GetPlatform()->GetTrackManager().GetCurrentTargetTrackId() == tgtTrackId)
      {
         currentTargetTrackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      }
      WsfObserver::WeaponFired(GetSimulation())(aSimTime, engagementPtr, currentTargetTrackPtr);
   }

   // Set the interpolated/extrapolated weapon and target location in the engagement object.
   // If a location hasn't been determined then capture the truth location for debug display purposes.

   if (!UtVec3d::Equals(mDetWpnLocWCS, 0.0))
   {
      engagementPtr->SetWeaponLocationWCS(GetPlatform(), mDetWpnLocWCS);
   }
   else
   {
      GetPlatform()->GetLocationWCS(mDetWpnLocWCS);
      double wpnVelWCS[3];
      GetPlatform()->GetVelocityWCS(wpnVelWCS);
      // Extrapolate the position forward by the time difference between the simulation time
      // and the last platform update time. (See note at top of file).
      double deltaT = std::max(0.0, aSimTime - GetPlatform()->GetLastUpdateTime());
      UtVec3d::AddProduct(mDetWpnLocWCS, mDetWpnLocWCS, deltaT, wpnVelWCS);
   }

   if (mOnIntercept == cACTION_HALT)
   {
      // Freeze the weapon platform at the point of impact.
      GetPlatform()->SetLocationWCS(mDetWpnLocWCS);
      PermanentlyArrestMotion(aSimTime);
   }

   // Verify that our target still exists!
   mTgtPtr = GetSimulation()->GetPlatformByIndex(mTgtIndex);
   if (mTgtPtr != nullptr)
   {
      if (!UtVec3d::Equals(mDetTgtLocWCS, 0.0))
      {
         engagementPtr->SetTargetLocationWCS(mTgtPtr, mDetTgtLocWCS);
      }
      else
      {
         mTgtPtr->GetLocationWCS(mDetTgtLocWCS);
         double tgtVelWCS[3];
         mTgtPtr->GetVelocityWCS(tgtVelWCS);
         // Extrapolate the position forward by the time difference between the simulation time
         // and the last platform update time. (See note at top of file).
         double deltaT = std::max(0.0, aSimTime - mTgtPtr->GetLastUpdateTime());
         UtVec3d::AddProduct(mDetTgtLocWCS, mDetTgtLocWCS, deltaT, tgtVelWCS);
      }
   }

   if (mDetTime >= 0.0)
   {
      engagementPtr->SetCompletionTime(mDetTime);
   }

   WsfWeaponEngagement::GeometryResult result = WsfWeaponEngagement::cDUD;
   if (mArmTime >= 0.0) // Fuse is armed
   {
      // Preliminary result, refine it below if possible...
      result = WsfWeaponEngagement::cFAR_AWAY_IN_AIR;

      // Deduce a specific geometry result:
      if (mProximityHit)
      {
         double missVecWCS[3];
         UtVec3d::Subtract(missVecWCS, mDetWpnLocWCS, mDetTgtLocWCS);
         double missDistance = UtVec3d::Magnitude(missVecWCS);

         // If we don't have an actual target platform (i.e.: shooting against a track which does not
         // contain a truth target index) then set the miss distance in the engagement object as it may
         // not be derivable otherwise. This allows the miss distance to be visible in the event output.

         if ((mTgtPtr == nullptr) && (!UtVec3d::Equals(mDetWpnLocWCS, 0.0)) && (!UtVec3d::Equals(mDetTgtLocWCS, 0.0)))
         {
            engagementPtr->SetMissDistance(missDistance);
         }

         if (missDistance < cCONTACT)
         {
            // The proximity to the target is so close,
            // it must have been a direct hit.
            result = WsfWeaponEngagement::cTARGET_IMPACT;
         }
         else if (mAGL_Criteria == cNO_LIMIT)
         {
            // The weapon detonated close to the target,
            // but we know nothing about ground location,
            // so assume this was an air burst
            result = WsfWeaponEngagement::cTARGET_PROXIMITY_AIR_BURST;
         }
         // Cases below this point know something about AGL height...
         else if ((mThisWpnAltAGL > cDISTANCE_RESOLUTION) && (mThisWpnAltAGL < cGROUND_PROXIMITY_LIMIT))
         {
            // The weapon detonated close to the target, and also above ground
            result = WsfWeaponEngagement::cTARGET_PROXIMITY_ABOVE_GROUND;
         }
         else if (mThisWpnAltAGL < cDISTANCE_RESOLUTION)
         {
            // The weapon detonated close to the target, and also (apparently) on terrain
            result = WsfWeaponEngagement::cTARGET_PROXIMITY_GROUND_IMPACT;
         }
         else
         {
            // The weapon detonated close to the target, but NOT near ground
            result = WsfWeaponEngagement::cTARGET_PROXIMITY_AIR_BURST;
         }
      }
      // Cases below this point do not know anything about target proximity
      // (even though the target may have been KILLED), so assume cFAR_AWAY.
      else if (mThisWpnAltAGL != 0.0)
      {
         if (mThisWpnAltAGL < cDISTANCE_RESOLUTION)
         {
            result = WsfWeaponEngagement::cFAR_AWAY_GROUND_IMPACT;
         }
         else if (mThisWpnAltAGL < cGROUND_PROXIMITY_LIMIT)
         {
            result = WsfWeaponEngagement::cFAR_AWAY_ABOVE_GROUND;
         }
         // else // get the default result = WsfWeaponEngagement::cFAR_AWAY_IN_AIR;
      }
   }

   // Set the extended reason code.
   switch (mReason)
   {
   case cPROXIMITY:
      engagementPtr->SetExtendedResult("Target proximity");
      break;
   case cAGL_LIMIT:
      engagementPtr->SetExtendedResult("AGL limit encountered");
      break;
   case cMSL_LIMIT:
      engagementPtr->SetExtendedResult("MSL limit encountered");
      break;
   case cMIN_SPEED:
      engagementPtr->SetExtendedResult("Minimum speed encountered");
      break;
   case cCOMMAND:
      engagementPtr->SetExtendedResult("Commanded termination");
      break;
   case cTOF_LIMIT:
      engagementPtr->SetExtendedResult("Maximum time-of-flight exceeded");
      break;
   case cCOAST_TIME_LIMIT:
      engagementPtr->SetExtendedResult("Coast time exceeded");
      break;
   default:
      break;
   }

   // Execute the 'on_weapon_detonation' script if it is defined.
   UtScript* scriptPtr = mContextPtr->FindScript("on_weapon_detonation");
   if ((scriptPtr != nullptr) && mContextPtr->ValidateScript(scriptPtr, "void", ""))
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      mContextPtr->ExecuteScript(aSimTime, scriptPtr, retVal, argList);
   }

   engagementPtr->Terminate(aSimTime, result);

   if (DebugEnabled())
   {
      if (mTOF_Hit || mProximityHit || mAGL_Hit || mMSL_Hit || mMinSpeedHit || mForceDetonate)
      {
         PrintDetonationReason(aSimTime);

         double lat;
         double lon;
         double alt;
         int    locCount = 0;
         if (!UtVec3d::Equals(mDetWpnLocWCS, 0.0))
         {
            UtEntity::ConvertWCSToLLA(mDetWpnLocWCS, lat, lon, alt);
            auto out = ut::log::info() << "Weapon LLA:";
            out.GetStream().precision(8);
            out.AddNote() << "Lat: " << lat;
            out.AddNote() << "Lon: " << lon;
            out.AddNote() << "Alt: " << alt;
            out.AddNote() << "WCS: " << mDetWpnLocWCS;
            ++locCount;
         }
         if (!UtVec3d::Equals(mDetTgtLocWCS, 0.0))
         {
            UtEntity::ConvertWCSToLLA(mDetTgtLocWCS, lat, lon, alt);
            auto out = ut::log::info() << "Target LLA:";
            out.GetStream().precision(8);
            out.AddNote() << "Lat: " << lat;
            out.AddNote() << "Lon: " << lon;
            out.AddNote() << "Alt: " << alt;
            out.AddNote() << "WCS: " << mDetTgtLocWCS;
            ++locCount;
         }
         if (locCount == 2)
         {
            double deltaDetLocWCS[3];
            UtVec3d::Subtract(deltaDetLocWCS, mDetWpnLocWCS, mDetTgtLocWCS);
            auto out = ut::log::info() << "Miss distance:";
            out.GetStream().precision(8);
            out.AddNote() << "Missed By: " << UtVec3d::Magnitude(deltaDetLocWCS) << " m";
         }

         std::string resultString;
         WsfWeaponEngagement::EnumToString(resultString, result);
         ut::log::info() << "Geometry Result: " << resultString;
      } // if (... hit ...)
   }    // if (DebugEnabled())

   // We have detonated, so there is no further need to be on.
   TurnOff(aSimTime);
}

// ================================================================================================
// protected
void WsfWeaponFuse::PrintDetonationReason(double aSimTime)
{
   auto out = ut::log::info() << "Detonation Reason:";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   auto det = out.AddNote() << "Detonating due to:";

   if (mTOF_Hit)
   {
      det.AddNote() << "TOF Limit.";
   }
   if (mProximityHit)
   {
      det.AddNote() << "Target Proximity.";
   }
   if (mAGL_Hit)
   {
      det.AddNote() << "AGL Limit.";
   }
   if (mMSL_Hit)
   {
      det.AddNote() << "MSL Limit.";
   }
   if (mMinSpeedHit)
   {
      det.AddNote() << "Minimum Speed Limit.";
   }
   if (mForceDetonate)
   {
      det.AddNote() << "Forced Detonation (coast time expired, or commanded detonation).";
   }
}

// ================================================================================================
//! Check to see if the AGL altitude criteria has been met.
//! @note It is assumed that the caller has checked to ensure that AGL altitude is a criteria.
// protected
bool WsfWeaponFuse::ProcessAltAGL(double aSimTime)
{
   bool closeToLimit = false;

   mLastAGL_Delta = mThisAGL_Delta;
   double limit   = (mATG_Mode) ? mTgtAltAGL : mAGL_Limit;
   mThisAGL_Delta = mThisWpnAltAGL - limit;
   ++mAGL_UpdateCount;
   if (mAGL_UpdateCount >= 2)
   {
      bool lowEnough  = (mAGL_Criteria == cDETONATE_BELOW) && (mThisAGL_Delta < 0.0) && (mLastAGL_Delta >= 0.0);
      bool highEnough = (mAGL_Criteria == cDETONATE_ABOVE) && (mThisAGL_Delta > 0.0) && (mLastAGL_Delta <= 0.0);

      if (lowEnough || highEnough)
      {
         mAGL_Hit = true;
         mReason  = cAGL_LIMIT;

         // Determine where this platform to the point where it intersected the limit:
         double modulus = mLastAGL_Delta / (mLastAGL_Delta - mThisAGL_Delta);
         if ((modulus >= (-cMODULUS_EPSILON)) && (modulus <= (1.0 + cMODULUS_EPSILON)))
         {
            double deltaWpnLocWCS[3];
            UtVec3d::Subtract(deltaWpnLocWCS, mThisWpnLocWCS, mLastWpnLocWCS);
            UtVec3d::AddProduct(mDetWpnLocWCS, mLastWpnLocWCS, modulus, deltaWpnLocWCS);
            double deltaTime = std::max(0.0, modulus * (aSimTime - mLastUpdateTime));
            mDetTime         = mLastUpdateTime + deltaTime;
         }
      }
      else if (mThisAGL_Delta < cIN_THE_BALLPARK * fabs(mLastAGL_Delta - mThisAGL_Delta))
      {
         closeToLimit = true;
      }
   }
   return closeToLimit;
}

// ================================================================================================
//! Check to see if the MSL altitude criteria has been met.
//! @note It is assumed that the caller has checked to ensure that MSL altitude is a criteria.
// protected
bool WsfWeaponFuse::ProcessAltMSL(double aSimTime)
{
   bool closeToLimit = false;

   mLastMSL_Delta = mThisMSL_Delta;
   mThisMSL_Delta = mThisWpnAltMSL - mMSL_Limit;
   ++mMSL_UpdateCount;
   if (mMSL_UpdateCount >= 2)
   {
      bool lowEnough  = (mMSL_Criteria == cDETONATE_BELOW) && (mThisMSL_Delta < 0.0) && (mLastMSL_Delta >= 0.0);
      bool highEnough = (mMSL_Criteria == cDETONATE_ABOVE) && (mThisMSL_Delta > 0.0) && (mLastMSL_Delta <= 0.0);

      if (lowEnough || highEnough)
      {
         mMSL_Hit = true;
         mReason  = cMSL_LIMIT;

         // Determine where this platform to the point where it intersected the limit:
         double modulus = mLastMSL_Delta / (mLastMSL_Delta - mThisMSL_Delta);
         if ((modulus >= (-cMODULUS_EPSILON)) && (modulus <= (1.0 + cMODULUS_EPSILON)))
         {
            double deltaWpnLocWCS[3];
            UtVec3d::Subtract(deltaWpnLocWCS, mThisWpnLocWCS, mLastWpnLocWCS);
            UtVec3d::AddProduct(mDetWpnLocWCS, mLastWpnLocWCS, modulus, deltaWpnLocWCS);
            double deltaTime = std::max(0.0, modulus * (aSimTime - mLastUpdateTime));
            mDetTime         = mLastUpdateTime + deltaTime;
         }
      }
      else if (mThisMSL_Delta < cIN_THE_BALLPARK * fabs(mLastMSL_Delta - mThisMSL_Delta))
      {
         closeToLimit = true;
      }
   }
   return closeToLimit;
}

// ================================================================================================
//! Check to see if the minimum speed criteria has been met.
//! @note It is assumed that the caller has checked to ensure that minimum speed is a criteria.
// protected
void WsfWeaponFuse::ProcessMinSpeed(double aSimTime)
{
   // Two Possibilities:
   // a) For air-to-air missiles, fusing on ascending or descending altitude is not
   // likely to be the best option, but fusing on decaying Mach is more appropriate.
   // b) For surface to air missiles, the minimum speed check is imposed only when
   // the altitude starts decreasing.  Otherwise we might detonate during
   // inter-stage coasting during boost phase.

   if (mMinSpeedCriteria == cSC_MACH_DECAY)
   {
      double mach = mThisWpnSpeed / mAtmosphere.SonicVelocity(mThisWpnAltMSL);
      if (mach > mCapturedMaxSpeed)
      {
         mCapturedMaxSpeed = mach;
      }
      if ((mach < mCapturedMaxSpeed) && (mach < mMinSpeedLimit))
      {
         mMinSpeedHit = true;
         mReason      = cMIN_SPEED;
      }
   }
   else if (mThisWpnAltMSL < mLastWpnAltMSL)
   {
      if (mThisWpnSpeed < mLastWpnSpeed)
      {
         if (mMinSpeedCriteria == cSC_SPEED)
         {
            mMinSpeedHit = (mThisWpnSpeed < mMinSpeedLimit);
         }
         else if (mMinSpeedCriteria == cSC_MACH)
         {
            double mach  = mThisWpnSpeed / mAtmosphere.SonicVelocity(mThisWpnAltMSL);
            mMinSpeedHit = (mach < mMinSpeedLimit);
         }
         if (mMinSpeedHit)
         {
            mReason = cMIN_SPEED;
         }
      }
   }
}

// ================================================================================================
//! Check to see if any target proximity criteria are met.
//! @returns true if at least one target is in proximity and requires a fine update interval.
// protected
bool WsfWeaponFuse::ProcessProximity(double aSimTime)
{
   bool inProximity = false;
   if (mUseCurTgtOnly || (mTgtNameId != 0))
   {
      if (UpdateTargetState(aSimTime))
      {
         // First determine if the objects are 'close enough' to consider.
         // NOTE: mTgtPtr may be 0 here if pursuing a track that isn't correlated to a physical entity.

         double locDiffWCS[3];
         UtVec3d::Subtract(locDiffWCS, mThisWpnLocWCS, mTgtLocWCS);
         double rangeSqr = UtVec3d::MagnitudeSquared(locDiffWCS);
         double maxRange = ComputeGrossProximityRange(mTgtVelWCS);
         if (rangeSqr < (maxRange * maxRange))
         {
            double timeTo = ComputeTimeToPCAorFuse(mTgtLocWCS, mTgtVelWCS);

            if (DebugEnabled())
            {
               ShowPCA_Results(aSimTime, mTgtPtr, mTgtLocWCS, mTgtVelWCS, timeTo);
            }
            inProximity = true;
            if (timeTo <= mPCA_HitMaxTime)
            {
               if (timeTo >= mPCA_HitMinTime)
               {
                  SelectPCA_ForDetonation(aSimTime, mTgtPtr, mTgtLocWCS, mTgtVelWCS, timeTo, 0.0);
               }
               else
               {
                  inProximity = false;
               }
            }
            else if (timeTo < 10.0)
            {
               UtVec3d::AddProduct(mLastPCA_WpnLocWCS, mThisWpnLocWCS, timeTo, mThisWpnVelWCS);
               UtVec3d::AddProduct(mLastPCA_TgtLocWCS, mTgtLocWCS, timeTo, mTgtVelWCS);
               mLastPCA_Time = aSimTime + timeTo;
               // cout << " Last PCA estimate changed to " << mLastPCA_Time << endl;
            }
            else
            {
               mLastPCA_Time = -1.0;
            }
         }
      }
      else if (mTargetLossOption == cDETONATE)
      {
         // Detonate immediately
         mForceDetonate = true;
      }
      else if (mTargetLossOption == cCONSIDER_ALL_PROXIMITY)
      {
         // Revert to processing proximity against all targets.
         mUseCurTgtOnly = false;
         mTgtNameId     = nullptr;
      }
      else if (mTargetLossOption == cCANCEL_PROXIMITY_CHECKS)
      {
         // No valid target, so cancel proximity check completely...
         mGrossProximityRange = 0.0;
      }
      else if ((mLastGoodTgtUpdateTime >= 0.0) && ((aSimTime - mLastGoodTgtUpdateTime) > mCoastTime))
      {
         // Coast time has expired, detonate.
         mForceDetonate = true;
         mReason        = cCOAST_TIME_LIMIT;
      }
   }
   else
   {
      inProximity = CheckAllTargets(aSimTime);
   }
   return inProximity;
}

// ================================================================================================
//! Check all platforms to see if a detonation should occur.
// protected
bool WsfWeaponFuse::CheckAllTargets(double aSimTime)
{
   bool inProximity = false;

   // Find out if there are any platforms of interest in proximity:

   double tgtLocWCS[3];
   double tgtVelWCS[3];
   double locDiffWCS[3];

   // Tell SelectPCA_ForDetonation the 'last PCA' values are not to be used. We are potentially
   // considering multiple platforms and we only maintain 'last PCA' data for a single platform.
   mLastPCA_Time = -1.0;

   for (size_t i = 0; (i < GetSimulation()->GetPlatformCount()) && (!mProximityHit); ++i)
   {
      WsfPlatform* tgtPtr = GetSimulation()->GetPlatformEntry(i);

      // Ignore proximity to self, and (optionally) the launching platform, or invincible categories...
      bool tgtIsMyself     = (tgtPtr == GetPlatform());
      bool tgtIsMyLauncher = (tgtPtr->GetIndex() == mLauncherIndex);
      bool tgtIsInvincible = mInvincibleCats.Intersects(tgtPtr->GetCategories());

      if (!(tgtIsMyself || tgtIsMyLauncher || tgtIsInvincible))
      {
         // The target platforms Update method is NOT called because of the potentially excessive overhead.
         // Instead, we first simply extrapolate its location to see if it is in 'gross proximity'.

         tgtPtr->GetLocationWCS(tgtLocWCS);
         tgtPtr->GetVelocityWCS(tgtVelWCS);
         double deltaT = std::max(0.0, aSimTime - tgtPtr->GetLastUpdateTime());
         UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, deltaT, tgtVelWCS);
         UtVec3d::Subtract(locDiffWCS, mThisWpnLocWCS, tgtLocWCS);
         double rangeSqr = UtVec3d::MagnitudeSquared(locDiffWCS);
         double maxRange = ComputeGrossProximityRange(tgtVelWCS);
         if (rangeSqr < (maxRange * maxRange))
         {
            // We are within the gross proximity range so use fine update interval
            inProximity = true;

            // Request a full update if the range is within .25 of max range.
            if (rangeSqr < 0.125 * (maxRange * maxRange))
            {
               tgtPtr->Update(aSimTime);
               tgtPtr->GetLocationWCS(tgtLocWCS);
               tgtPtr->GetVelocityWCS(tgtVelWCS);
               // Extrapolate the position forward by the time difference between the simulation time
               // and the last platform update time. (See note at top of file).
               deltaT = std::max(0.0, aSimTime - tgtPtr->GetLastUpdateTime());
               UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, deltaT, tgtVelWCS);
            }
            double timeTo = ComputeTimeToPCAorFuse(tgtLocWCS, tgtVelWCS);
            if (DebugEnabled())
            {
               ShowPCA_Results(aSimTime, tgtPtr, tgtLocWCS, tgtVelWCS, timeTo);
            }
            if ((timeTo < mPCA_HitMaxTime) && (timeTo >= mPCA_HitMinTime))
            {
               SelectPCA_ForDetonation(aSimTime, tgtPtr, tgtLocWCS, tgtVelWCS, timeTo, mHitProximityRange);
            }
         }
      }
   } // for loop over all the platforms
   return inProximity;
}

// ================================================================================================
//! Compute the maximum range for which a target is considered in 'gross proximity'.
//!
//! Target proximity calculations will not be performed unless the it is within 'gross proximity' of the weapon.
//! This method will select a range that is appropriate for the current speed of the weapon and target.
//!
//! @param aTgtVelWCS The target velocity vector.
//! @returns The computed gross proximity range (meters).
// protected
double WsfWeaponFuse::ComputeGrossProximityRange(const double aTgtVelWCS[3]) const
{
   // Determine the maximum possible closing speed of the weapon and the target.
   double maxClosingSpeed = mThisWpnSpeed + UtVec3d::Magnitude(aTgtVelWCS);

   // Allow at least 3 seconds for maneuvering, but don't allow it to fall below the user-specified gross_proximity_range.
   double range = std::max(3.0 * maxClosingSpeed, mGrossProximityRange);

   // Don't allow the computed range to fall below hit_proximity_range.
   range = std::max(range, mHitProximityRange);

   return range;
}

// ================================================================================================
//! Compute the closest point of approach between a specific weapon-target pair.
//!
//! Given the position and velocity vector the weapon (member data) and the position and
//! velocity vector of a target (arguments), compute the time to the closest point of approach
//! assuming they each move at a uniform velocity (the uniform velocity case is
//! not perfectly accurate, but is sufficient for small periods of time).
//!
//! @param aTgtLocWCS [input] The location of the target.
//! @param aTgtVelWCS [input] The velocity of the target.
//! @returns The APPROXIMATE time (relative to the current simulation time) when PCA should be
//! encountered. If negative then it occurs in the past.
// protected
double WsfWeaponFuse::ComputeTimeToPCAorFuse(const double aTgtLocWCS[3], const double aTgtVelWCS[3]) const
{
   double timeTo = 1.0E+10;
   double locDiffWCS[3];
   double velDiffWCS[3];

   UtVec3d::Subtract(locDiffWCS, mThisWpnLocWCS, aTgtLocWCS);
   UtVec3d::Subtract(velDiffWCS, mThisWpnVelWCS, aTgtVelWCS);
   double distSquared = UtVec3d::MagnitudeSquared(locDiffWCS);
   double denom       = UtVec3d::MagnitudeSquared(velDiffWCS);

   if (denom > 1.0E-8)
   {
      double numer = -UtVec3d::DotProduct(locDiffWCS, velDiffWCS);
      timeTo       = numer / denom;
   }

   if (mFuseFunctionRange > 0.0)
   {
      double timeToFuse = 1.0E+10;

      // Calculate closing speed of weapon and target
      UtVec3d::Normalize(locDiffWCS);
      double closingSpeed = UtVec3d::DotProduct(velDiffWCS, locDiffWCS);

      double distToGo = sqrt(distSquared) - mFuseFunctionRange;
      timeToFuse      = distToGo / std::abs(closingSpeed);
      if (timeToFuse < timeTo)
      {
         timeTo = timeToFuse;
      }
   }

   return timeTo;
}

// ================================================================================================
//! Select the PCA computations as the one that causes detonation.
//!
//! This updates the fuse to indicate that a proximity detonation has occurred.
//! @note aTgtPtr may be 0.
// protected
void WsfWeaponFuse::SelectPCA_ForDetonation(double       aSimTime,
                                            WsfPlatform* aTgtPtr,
                                            const double aTgtLocWCS[3],
                                            const double aTgtVelWCS[3],
                                            double       aTimeToPCA,
                                            double       aMissDistanceLimit)
{
   // Compute the locations of the weapon and target and PCA.
   double wpnPCA_LocWCS[3];
   double tgtPCA_LocWCS[3];
   UtVec3d::AddProduct(wpnPCA_LocWCS, mThisWpnLocWCS, aTimeToPCA, mThisWpnVelWCS);
   UtVec3d::AddProduct(tgtPCA_LocWCS, aTgtLocWCS, aTimeToPCA, aTgtVelWCS);

   double missDistance = aMissDistanceLimit - mFuseFunctionRange;

   // Weapon can only miss if it is further than the fuse range.
   if (missDistance > 0.0)
   {
      double missVecWCS[3];
      UtVec3d::Subtract(missVecWCS, wpnPCA_LocWCS, tgtPCA_LocWCS);

      double dSquared = UtVec3d::MagnitudeSquared(missVecWCS);
      if (dSquared > (missDistance * missDistance))
      {
         return; // Reject the detonation
      }
   }

   // Indicate that a proximity hit has occurred.
   mProximityHit = true;
   mReason       = cPROXIMITY;

   // Select the target involved in the proximity hit and set the locations at PCA.
   mTgtPtr = aTgtPtr;
   if (aTgtPtr != nullptr)
   {
      mTgtIndex = aTgtPtr->GetIndex();
   }

   // Assume this sample is better than the previous sample.
   mDetTime = aSimTime + aTimeToPCA;
   UtVec3d::Set(mDetWpnLocWCS, wpnPCA_LocWCS);
   UtVec3d::Set(mDetTgtLocWCS, tgtPCA_LocWCS);

   // If a previous sample exists, determine if it is better.

   if (mLastPCA_Time >= 0.0)
   {
      double lastDeltaLocWCS[3];
      UtVec3d::Subtract(lastDeltaLocWCS, mLastPCA_WpnLocWCS, mLastPCA_TgtLocWCS);
      double thisDeltaLocWCS[3];
      UtVec3d::Subtract(thisDeltaLocWCS, wpnPCA_LocWCS, tgtPCA_LocWCS);
      if (UtVec3d::MagnitudeSquared(lastDeltaLocWCS) < UtVec3d::MagnitudeSquared(thisDeltaLocWCS))
      {
         mDetTime = mLastPCA_Time;
         UtVec3d::Set(mDetWpnLocWCS, mLastPCA_WpnLocWCS);
         UtVec3d::Set(mDetTgtLocWCS, mLastPCA_TgtLocWCS);
      }
   }
}

// ================================================================================================
//! Display PCA computation results (for debug purposes);
//! @note aTgtPtr may be 0.
// protected
void WsfWeaponFuse::ShowPCA_Results(double       aSimTime,
                                    WsfPlatform* aTgtPtr,
                                    const double aTgtLocWCS[3],
                                    const double aTgtVelWCS[3],
                                    double       aTimeToPCA) const
{
   double wpnPCA_LocWCS[3];
   double tgtPCA_LocWCS[3];
   double missVecWCS[3];

   // Compute the locations of the weapon and target at PCA and also the distance between them.
   UtVec3d::AddProduct(wpnPCA_LocWCS, mThisWpnLocWCS, aTimeToPCA, mThisWpnVelWCS);
   UtVec3d::AddProduct(tgtPCA_LocWCS, aTgtLocWCS, aTimeToPCA, aTgtVelWCS);
   UtVec3d::Subtract(missVecWCS, wpnPCA_LocWCS, tgtPCA_LocWCS);
   double distAtPCA = UtVec3d::Magnitude(missVecWCS);

   double rangeVecWCS[3];
   UtVec3d::Subtract(rangeVecWCS, mThisWpnLocWCS, aTgtLocWCS);
   double range = UtVec3d::Magnitude(rangeVecWCS);

   auto out = ut::log::debug() << "PCA Results:";
   out.GetStream().precision(10);
   out.AddNote() << "T = " << aSimTime;
   auto note = out.AddNote() << "Platform: " << GetPlatform()->GetName();
   if (aTgtPtr != nullptr)
   {
      note.AddNote() << "Target: " << aTgtPtr->GetName();
   }
   else
   {
      note.AddNote() << "Target: <UNKNOWN>";
   }
   out.AddNote() << "Range: " << range;
   out.AddNote() << "D_pca: " << distAtPCA;
   out.AddNote() << "T_pca: " << aTimeToPCA << " (" << aSimTime + aTimeToPCA << ")";
}

// ================================================================================================
//! This routine is responsible for selecting the target from either the 'current target track' or the 'named target'.
//! @returns 'true' if the target state has been updated or 'false' if the target does not exist.
// protected
bool WsfWeaponFuse::UpdateTargetState(double aSimTime)
{
   bool            locationValid = false;
   const WsfTrack* trackPtr      = nullptr;
   mLastTgtIndex                 = mTgtIndex;
   if (mUseCurTgtOnly)
   {
      // Engaging the target defined by the 'current target track'.

      size_t newTgtIndex = 0;
      trackPtr           = GetPlatform()->GetTrackManager().GetCurrentTarget();
      if (trackPtr != nullptr)
      {
         newTgtIndex = trackPtr->GetTargetIndex();
         mTgtTrackId = trackPtr->GetTrackId();
      }

      if ((newTgtIndex == 0) && (mTgtIndex == 0))
      {
         // No current target and no new target.
      }
      else if (GetSimulation()->PlatformExists(newTgtIndex))
      {
         if (mTgtIndex != newTgtIndex)
         {
            mTgtIndex = newTgtIndex;
            mTgtPtr   = GetSimulation()->GetPlatformByIndex(mTgtIndex);
            assert((mTgtPtr != nullptr) && (mTgtPtr->GetIndex() == newTgtIndex));
         }
      }
      else if (mTgtIndex != 0)
      {
         // New target does not exist but there was a previous target.
         mTgtPtr   = nullptr;
         mTgtIndex = 0;
      }
   }
   else if (mTgtNameId != 0)
   {
      // Engaging a specifically named target

      if ((mTgtPtr != nullptr) && (GetSimulation()->PlatformExists(mTgtIndex)))
      {
         // Continue using an existing platform name...
      }
      else if (mTgtIndex == 0)
      {
         // Try to find the platform for the first time...
         mTgtPtr = GetSimulation()->GetPlatformByName(mTgtNameId);
         if (mTgtPtr != nullptr)
         {
            mTgtIndex = mTgtPtr->GetIndex();
         }
      }
      else
      {
         // Current target no longer exists.
         mTgtPtr = nullptr;
      }
   }

   if (mTgtPtr != nullptr)
   {
      mTgtPtr->Update(aSimTime);
      locationValid          = true;
      mLastGoodTgtUpdateTime = aSimTime;
      mTgtPtr->GetLocationWCS(mTgtLocWCS);
      mTgtPtr->GetVelocityWCS(mTgtVelWCS);
      // Extrapolate the position forward by the time difference between the simulation time
      // and the last platform update time. (See note at top of file).
      double deltaT = std::max(0.0, aSimTime - mTgtPtr->GetLastUpdateTime());
      UtVec3d::AddProduct(mTgtLocWCS, mTgtLocWCS, deltaT, mTgtVelWCS);
   }
   else if (trackPtr != nullptr)
   {
      mTgtIndex = 0;
      UtVec3d::Set(mTgtLocWCS, 0.0);
      UtVec3d::Set(mTgtVelWCS, 0.0);

      // The following logic will set the location if the target track does not have a truth target index.
      // This will occur either because the track is truly not associated with a truth entity OR it is a
      // pre-briefed track with a specified target that has not yet appeared in the simulation because it
      // appears later in the input file.
      //
      // The location will NOT be set if a target index is set (i.e.: a target was specified but now it is
      // dead). This allows coast time logic be applied in the normal way.

      if ((trackPtr != nullptr) && (trackPtr->GetTargetIndex() == 0))
      {
         if (trackPtr->GetExtrapolatedLocationWCS(aSimTime, mTgtLocWCS))
         {
            locationValid          = true;
            mLastGoodTgtUpdateTime = aSimTime;
            if (trackPtr->VelocityValid())
            {
               trackPtr->GetVelocityWCS(mTgtVelWCS);
            }
         }
      }
   }

   // If ATG mode then populate the value to detonate above terrain, corresponding to the ATG target height.
   // ASSUMPTION:  The ATG target stays a constant height above ground, does not vary with time!

   if (mATG_Mode && (!mTgtAltSet) && locationValid)
   {
      double lat;
      double lon;
      double altMSL;
      WsfPlatform::ConvertWCSToLLA(mTgtLocWCS, lat, lon, altMSL);
      float terrAltMSL;
      mTerrainPtr->GetElevInterp(lat, lon, terrAltMSL);
      mTgtAltAGL = altMSL - terrAltMSL;
      mTgtAltSet = true;
   }

   return locationValid;
}

// ================================================================================================
// protected
void WsfWeaponFuse::UpdateWeaponState(double aSimTime)
{
   // Capture the WCS location and velocity.

   UtVec3d::Set(mLastWpnLocWCS, mThisWpnLocWCS);
   GetPlatform()->GetLocationWCS(mThisWpnLocWCS);
   // We don't capture the 'last weapon velocity...'
   GetPlatform()->GetVelocityWCS(mThisWpnVelWCS);
   mLastWpnSpeed = mThisWpnSpeed;
   mThisWpnSpeed = UtVec3d::Magnitude(mThisWpnVelWCS);

   // Extrapolate the position forward by the time difference between the simulation time
   // and the last platform update time. (See note at top of file).
   double deltaT = std::max(0.0, aSimTime - GetPlatform()->GetLastUpdateTime());
   UtVec3d::AddProduct(mThisWpnLocWCS, mThisWpnLocWCS, deltaT, mThisWpnVelWCS);

   // Capture the altitude and the height above terrain.

   double lat;
   double lon;
   mLastWpnAltMSL = mThisWpnAltMSL;
   GetPlatform()->ConvertWCSToLLA(mThisWpnLocWCS, lat, lon, mThisWpnAltMSL);

   // Capture the height above the terrain. First we just get the approximate value.
   // If it results in being 'in proximity' to the AGL target then we get a refined value.

   float terrAlt;
   mTerrainPtr->GetElevApprox(lat, lon, terrAlt);
   mThisWpnAltAGL = mThisWpnAltMSL - terrAlt;

   double limit = 0.0;
   if (mTgtAltSet)
   {
      limit = (mATG_Mode) ? mTgtAltAGL : mAGL_Limit;
   }
   if ((mThisWpnAltAGL - limit) < (mThisWpnSpeed * mGrossUpdateInterval))
   {
      // If we are close to the constraint, interpolate for a more accurate result:
      mTerrainPtr->GetElevInterp(lat, lon, terrAlt);
      mThisWpnAltAGL = mThisWpnAltMSL - terrAlt;
   }
}

// ================================================================================================
// Script Interface
// ================================================================================================

// ================================================================================================
class WSF_MIL_EXPORT WsfScriptWeaponFuseClass : public WsfScriptProcessorClass
{
public:
   WsfScriptWeaponFuseClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);

   UT_DECLARE_SCRIPT_METHOD(Detonate);
   UT_DECLARE_SCRIPT_METHOD(MaximumTimeOfFlight);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateBelowSpeed);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateOnMachDecreasingTo);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateBelowMach);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateBelowHeightAGL);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateAboveHeightAGL);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateBelowHeightMSL);
   UT_DECLARE_SCRIPT_METHOD(GetDetonateAboveHeightMSL);
   UT_DECLARE_SCRIPT_METHOD(TimeOfFlightToArm);
};

// ================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfWeaponFuse::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptWeaponFuseClass>(aClassName, aScriptTypesPtr);
}

// ================================================================================================
WsfScriptWeaponFuseClass::WsfScriptWeaponFuseClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfWeaponFuse");

   AddMethod(ut::make_unique<Detonate>());
   AddMethod(ut::make_unique<MaximumTimeOfFlight>());
   AddMethod(ut::make_unique<GetDetonateBelowSpeed>());
   AddMethod(ut::make_unique<GetDetonateOnMachDecreasingTo>());
   AddMethod(ut::make_unique<GetDetonateBelowMach>());
   AddMethod(ut::make_unique<GetDetonateBelowHeightAGL>());
   AddMethod(ut::make_unique<GetDetonateAboveHeightAGL>());
   AddMethod(ut::make_unique<GetDetonateBelowHeightMSL>());
   AddMethod(ut::make_unique<GetDetonateAboveHeightMSL>());
   AddMethod(ut::make_unique<TimeOfFlightToArm>());
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, Detonate, 0, "void", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Detonate(simTime);
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, MaximumTimeOfFlight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumTimeOfFlight());
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateBelowSpeed, 0, "double", "")
{
   if (aObjectPtr->GetSpeedCriteria() == WsfWeaponFuse::SpeedCriteria::cSC_SPEED)
   {
      aReturnVal.SetDouble(aObjectPtr->GetMinSpeedLimit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateOnMachDecreasingTo, 0, "double", "")
{
   if (aObjectPtr->GetSpeedCriteria() == WsfWeaponFuse::SpeedCriteria::cSC_MACH_DECAY)
   {
      aReturnVal.SetDouble(aObjectPtr->GetMinSpeedLimit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateBelowMach, 0, "double", "")
{
   if (aObjectPtr->GetSpeedCriteria() == WsfWeaponFuse::SpeedCriteria::cSC_MACH)
   {
      aReturnVal.SetDouble(aObjectPtr->GetMinSpeedLimit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateBelowHeightAGL, 0, "double", "")
{
   if (aObjectPtr->GetAGL_Criteria() == WsfWeaponFuse::AltCriteria::cDETONATE_BELOW)
   {
      aReturnVal.SetDouble(aObjectPtr->GetAGL_Limit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateAboveHeightAGL, 0, "double", "")
{
   if (aObjectPtr->GetAGL_Criteria() == WsfWeaponFuse::AltCriteria::cDETONATE_ABOVE)
   {
      aReturnVal.SetDouble(aObjectPtr->GetAGL_Limit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateBelowHeightMSL, 0, "double", "")
{
   if (aObjectPtr->GetMSL_Criteria() == WsfWeaponFuse::AltCriteria::cDETONATE_BELOW)
   {
      aReturnVal.SetDouble(aObjectPtr->GetMSL_Limit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, GetDetonateAboveHeightMSL, 0, "double", "")
{
   if (aObjectPtr->GetMSL_Criteria() == WsfWeaponFuse::AltCriteria::cDETONATE_ABOVE)
   {
      aReturnVal.SetDouble(aObjectPtr->GetMSL_Limit());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

// ================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponFuseClass, WsfWeaponFuse, TimeOfFlightToArm, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeOfFlightToArm());
}
