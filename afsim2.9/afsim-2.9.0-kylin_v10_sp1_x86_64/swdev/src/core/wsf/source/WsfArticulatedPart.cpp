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

#include "WsfArticulatedPart.hpp"

#include <algorithm>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtVec3.hpp"
#include "WsfArticulatedPartEvent.hpp"
#include "WsfMaskingPattern.hpp"
#include "WsfMaskingPatternTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfArticulatedPart::WsfArticulatedPart(const WsfScenario& aScenario, int aPartType)
   : WsfPlatformPart(aScenario, aPartType)
   , UtEntityPart()
   , mMaskingPatternPtr(nullptr)
   , mMaskingPatternName()
   , mMaskingPatternStateId(UtStringIdLiteral("default"))
   , mYaw(0.0)
   , mPitch(0.0)
   , mRoll(0.0)
   , mDX(0.0)
   , mDY(0.0)
   , mDZ(0.0)
   , mTilt(0.0)
   , mAzSlewRate(1.01 * cINFINITE_SLEW_RATE)
   , mElSlewRate(1.01 * cINFINITE_SLEW_RATE)
   , mMaxAzSlewRate(1.01 * cINFINITE_SLEW_RATE)
   , // Why greater than infinite?
   mMaxElSlewRate(1.01 * cINFINITE_SLEW_RATE)
   , mMinAzSlew(-UtMath::cPI)
   , mMaxAzSlew(UtMath::cPI)
   , mMinElSlew(-UtMath::cPI_OVER_2)
   , mMaxElSlew(UtMath::cPI_OVER_2)
   , mSlewMode(cSLEW_FIXED)
   , mAzSlewDirection(cUNDEFINED)
   , mSlewState(cNOT_CUED)
   , mAzCueRate(1.0E+30)
   , // Will be limited by the slew rates
   mElCueRate(1.0E+30)
   , // Will be limited by the slew rates
   mMinAzCue(-UtMath::cPI)
   , // Will be limited by the slew limits
   mMaxAzCue(UtMath::cPI)
   , // Will be limited by the slew limits
   mMinElCue(-UtMath::cPI_OVER_2)
   , // Will be limited by the slew limits
   mMaxElCue(UtMath::cPI_OVER_2)
   , // Will be limited by the slew limits
   mCueMode(cSLEW_AZ_EL)
   , // Will be limited by the slew mode
   mCueType(cCUED_TO_NOTHING)
   , mTransientCueActive(false)
   , mCuedAz(0.0)
   , mCuedEl(0.0)
   , mActualCuedAz(0.0)
   , mActualCuedEl(0.0)
   , mCurrentAz(0.0)
   , mCurrentEl(0.0)
   , mLastAzElUpdateTime(0.0)
   , mLastUpdateTime(0.0)
   , mSlewMethodCoordinated(false)
   , mIsSlewing(false)
   , mIsPrivate(false)
   , mArticulationUpdateInterval(0.0)
   , mArticulationUpdateEventEpoch(1)
   , mArticulationUpdateEventActive(false)
{
   UtVec3d::Set(mCuedLocationWCS, 0.0);
   UtVec3d::Set(mTransientCuedLocationWCS, 0.0);
}

// =================================================================================================
WsfArticulatedPart::WsfArticulatedPart(const WsfArticulatedPart& aSrc)
   : WsfPlatformPart(aSrc)
   , UtEntityPart(aSrc)
   , mMaskingPatternPtr(aSrc.mMaskingPatternPtr)
   , mMaskingPatternName(aSrc.mMaskingPatternName)
   , mMaskingPatternStateId(aSrc.mMaskingPatternStateId)
   , mYaw(aSrc.mYaw)
   , mPitch(aSrc.mPitch)
   , mRoll(aSrc.mRoll)
   , mDX(aSrc.mDX)
   , mDY(aSrc.mDY)
   , mDZ(aSrc.mDZ)
   , mTilt(aSrc.mTilt)
   , mAzSlewRate(aSrc.mAzSlewRate)
   , mElSlewRate(aSrc.mElSlewRate)
   , mMaxAzSlewRate(aSrc.mMaxAzSlewRate)
   , mMaxElSlewRate(aSrc.mMaxElSlewRate)
   , mMinAzSlew(aSrc.mMinAzSlew)
   , mMaxAzSlew(aSrc.mMaxAzSlew)
   , mMinElSlew(aSrc.mMinElSlew)
   , mMaxElSlew(aSrc.mMaxElSlew)
   , mSlewMode(aSrc.mSlewMode)
   , mAzSlewDirection(aSrc.mAzSlewDirection)
   , mSlewState(aSrc.mSlewState)
   , mAzCueRate(aSrc.mAzCueRate)
   , mElCueRate(aSrc.mElCueRate)
   , mMinAzCue(aSrc.mMinAzCue)
   , mMaxAzCue(aSrc.mMaxAzCue)
   , mMinElCue(aSrc.mMinElCue)
   , mMaxElCue(aSrc.mMaxElCue)
   , mCueMode(aSrc.mCueMode)
   , mCueType(aSrc.mCueType)
   , mTransientCueActive(aSrc.mTransientCueActive)
   , mCuedAz(aSrc.mCuedAz)
   , mCuedEl(aSrc.mCuedEl)
   , mActualCuedAz(aSrc.mActualCuedAz)
   , mActualCuedEl(aSrc.mActualCuedEl)
   , mCurrentAz(aSrc.mCurrentAz)
   , mCurrentEl(aSrc.mCurrentEl)
   , mLastAzElUpdateTime(aSrc.mLastAzElUpdateTime)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mSlewMethodCoordinated(aSrc.mSlewMethodCoordinated)
   , mIsSlewing(aSrc.mIsSlewing)
   , mIsPrivate(aSrc.mIsPrivate)
   , mArticulationUpdateInterval(aSrc.mArticulationUpdateInterval)
   , mArticulationUpdateEventEpoch(aSrc.mArticulationUpdateEventEpoch)
   , mArticulationUpdateEventActive(aSrc.mArticulationUpdateEventActive)
{
   UtVec3d::Set(mCuedLocationWCS, aSrc.mCuedLocationWCS);
   UtVec3d::Set(mTransientCuedLocationWCS, aSrc.mTransientCuedLocationWCS);
}

// =================================================================================================
// virtual
WsfArticulatedPart::~WsfArticulatedPart()
{
   // Detach the articulated part from the platform.
   if (GetPlatform() != nullptr)
   {
      GetPlatform()->DetachObserver(this);
   }
}

const int* WsfArticulatedPart::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_ARTICULATED_PART, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfArticulatedPart::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_ARTICULATED_PART)
   {
      return this;
   }
   return WsfPlatformPart::QueryInterface(aRole);
}

// =================================================================================================
// virtual
bool WsfArticulatedPart::Initialize(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize(aSimTime);
   if (GetPlatform() == nullptr)
   {
      auto out = ut::log::error() << "Articulated part must be associated with a platform.";
      out.AddNote() << "Part: " << GetName();
      return false;
   }
   // Registration to receive notification of platform updates already occurred
   // in SetPlatform. No need to call WsfPlatform::AttachObserver here.

   mLastAzElUpdateTime = aSimTime;
   mLastUpdateTime     = aSimTime;

   // Set the currently defined slew rates to be the maximum rates (optionally set with input keywords)
   mAzSlewRate = mMaxAzSlewRate;
   mElSlewRate = mMaxElSlewRate;

   // Get the pointer the masking pattern if defined.
   if (mMaskingPatternName != 0)
   {
      mMaskingPatternPtr = WsfMaskingPatternTypes::Get(GetScenario()).Find(mMaskingPatternName);
      if (mMaskingPatternPtr == nullptr)
      {
         auto out = ut::log::error() << "Undefined masking_pattern.";
         out.AddNote() << "Pattern: " << mMaskingPatternName;
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfArticulatedPart::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "location")
   {
      std::string units;
      double      location[3];
      aInput.ReadValue(location[0]);
      aInput.ReadValue(location[1]);
      aInput.ReadValue(location[2]);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
      UtVec3d::Multiply(location, multiplier);
      UtEntityPart::SetLocation(location);
   }
   else if (command == "yaw")
   {
      double yaw;
      aInput.ReadValueOfType(yaw, UtInput::cANGLE);
      aInput.ValueInClosedRange(yaw, -UtMath::cPI, UtMath::cPI);
      SetYaw(yaw);
   }
   else if (command == "pitch")
   {
      double pitch;
      aInput.ReadValueOfType(pitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(pitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetPitch(pitch);
   }
   else if (command == "roll")
   {
      double roll;
      aInput.ReadValueOfType(roll, UtInput::cANGLE);
      aInput.ValueInClosedRange(roll, -UtMath::cPI, UtMath::cPI);
      SetRoll(roll);
   }
   else if (command == "tilt")
   {
      double tilt;
      aInput.ReadValueOfType(tilt, UtInput::cANGLE);
      aInput.ValueInClosedRange(tilt, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetTilt(tilt);
   }
   else if (command == "azimuth_slew_rate")
   {
      aInput.ReadValueOfType(mMaxAzSlewRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxAzSlewRate, 0.0);
   }
   else if (command == "elevation_slew_rate")
   {
      aInput.ReadValueOfType(mMaxElSlewRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxElSlewRate, 0.0);
   }
   else if (command == "azimuth_slew_limits")
   {
      aInput.ReadValueOfType(mMinAzSlew, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxAzSlew, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinAzSlew, -UtMath::cPI);
      aInput.ValueLessOrEqual(mMaxAzSlew, UtMath::cPI);
      aInput.ValueLessOrEqual(mMinAzSlew, mMaxAzSlew);
   }
   else if (command == "elevation_slew_limits")
   {
      aInput.ReadValueOfType(mMinElSlew, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxElSlew, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinElSlew, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMaxElSlew, UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMinElSlew, mMaxElSlew);
   }
   else if (command == "slew_mode")
   {
      std::string slewMode;
      aInput.ReadValue(slewMode);
      if (slewMode == "fixed")
      {
         mSlewMode = cSLEW_FIXED;
      }
      else if (slewMode == "azimuth")
      {
         mSlewMode = cSLEW_AZ;
      }
      else if (slewMode == "elevation")
      {
         mSlewMode = cSLEW_EL;
      }
      else if ((slewMode == "both") || (slewMode == "azimuth_and_elevation"))
      {
         mSlewMode = cSLEW_AZ_EL;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "slew_method")
   {
      std::string slewMethod;
      aInput.ReadValue(slewMethod);
      if (slewMethod == "coordinated")
      {
         mSlewMethodCoordinated = true;
      }
      else if (slewMethod == "independent")
      {
         mSlewMethodCoordinated = false;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "masking_pattern")
   {
      std::string patternName;
      aInput.ReadValue(patternName);
      mMaskingPatternName = patternName;
   }
   else
   {
      myCommand = WsfPlatformPart::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfArticulatedPart::SetPlatform(WsfPlatform* aPlatformPtr)
{
   // This is an extension of WsfPlatformPart::SetPlatform() which also registers the articulated part
   // with the host platform.

   // If currently attached to another platform then detach from it before attaching to the new platform.
   if (GetPlatform() != nullptr)
   {
      GetPlatform()->DetachObserver(this);
   }

   // Let the base class do its thing.
   WsfPlatformPart::SetPlatform(aPlatformPtr);

   // Set/clear the owning entity attachment in UtEntityPart.
   UtEntityPart::SetOwningEntity(aPlatformPtr);

   // And now attach the part to its new host platform
   if (aPlatformPtr != nullptr)
   {
      aPlatformPtr->AttachObserver(this);
   }
}

// =================================================================================================
//! Set the platform-relative (ECS) roll angle of the articulated part.
void WsfArticulatedPart::SetRoll(double aRoll)
{
   mRoll = aRoll;
   UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
}

// =================================================================================================
//! Set the platform-relative (ECS) pitch angle of the articulated part.
//! @note 'pitch' should not be used for systems that rotate about their vertical axis as it will tilt the scan plane.
//! For electronic systems that rotate 'antenna_tilt' should be used. For other systems 'tilt' should be used.
//! @see SetTilt.
void WsfArticulatedPart::SetPitch(double aPitch)
{
   mPitch = aPitch;
   UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
}

// =================================================================================================
//! Set the platform-relative (ECS) yaw angle of the articulated part.
void WsfArticulatedPart::SetYaw(double aYaw)
{
   mYaw = aYaw;
   UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
}

// =================================================================================================
//! Set the platform-relative (ECS) tilt angle of the articulated part.
//! This is an alternative to using 'pitch' when one doesn't want to change the orientation of the axis of rotation.
//! @see SetPitch for other notes.
void WsfArticulatedPart::SetTilt(double aTilt)
{
   mTilt = aTilt;
   UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
}

// =================================================================================================
//! Get the current WCS Frame Line-Of-Sight unit vector.
void WsfArticulatedPart::GetLOS_UnitVectorWCS(double aUnitLOS_WCS[3])
{
   UpdateOrientation(GetSimulation()->GetSimTime());

   // Get the WCS vector aligned with the boresight (the PCS x-axis). A long vector is used to reduce
   // parallax issues caused by any PCS origin offset.
   double pointingPCS[3] = {100000.0, 0.0, 0.0};
   UtEntityPart::ConvertPCSVectorToWCS(aUnitLOS_WCS, pointingPCS);
   UtVec3d::Normalize(aUnitLOS_WCS);
}

// =================================================================================================
//! Get the current platform-relative (ECS) orientation of an articulated part.
//! If the part is not cued then it simply returns its standard yaw, pitch and roll angles.
//! If the part is cued then it returns the yaw and pitch angles that result from the cue, with roll set to zero.
//! @param aYaw   The azimuth angle with respect to the entity.
//! @param aPitch The elevation angle with respect to the entity.
//! @param aRoll  The roll angle.
//! @note If the part is cued then it assumes the current orientation is correct.
void WsfArticulatedPart::GetCurrentOrientationECS(double& aYaw, double& aPitch, double& aRoll)
{
   aYaw   = mYaw;
   aPitch = mPitch + mTilt;
   aRoll  = mRoll;

   if (UpdateOrientation(GetSimulation()->GetSimTime()))
   {
      // Get the WCS vector aligned with the boresight (the PCS x-axis). A long vector is used to reduce
      // parallax issues caused by any PCS origin offset.
      double pointingPCS[3] = {100000.0, 0.0, 0.0};
      double pointingWCS[3];
      UtEntityPart::ConvertPCSVectorToWCS(pointingWCS, pointingPCS);

      // Now get the aspect angles relative to the entity.
      GetPlatform()->ComputeAspect(pointingWCS, aYaw, aPitch);
      aRoll = 0.0; // Roll is always zero when using a cue
   }
}

// =================================================================================================
//! Update the position and orientation of the part.
//!
//! This should always be called by the host platform before calling any of the
//! geometry computation methods.
//!
//! @param aSimTime The current simulation time.
// virtual
void WsfArticulatedPart::UpdatePosition(double aSimTime)
{
   // Ensure the position of the platform is current. If the platform location is not current then WsfPlatform
   // will invoke our OnPlatformUpdated method via the WsfSinglePlatformObserver interface.
   GetPlatform()->Update(aSimTime);

   // Update the orientation.
   UpdateOrientation(aSimTime);

   // Update the translation.
   double  deltaT = aSimTime - mLastUpdateTime;
   UtVec3d l;
   GetLocation(l.GetData());
   l += UtVec3d{mDX, mDY, mDZ} * deltaT;
   SetLocation(l.GetData());
   mLastUpdateTime = aSimTime;
}

// =================================================================================================
//! Indicate that the 'non-transient' cue is no longer active.
//! @note This does not clear the transient cue.
void WsfArticulatedPart::ClearCueing()
{
   mCueType = cCUED_TO_NOTHING;
   if (!mTransientCueActive)
   {
      // See NOTE in SetCuedLocationWCS as to why the position update is necessary.
      double simTime = GetSimulation()->GetSimTime();
      if (simTime > mLastUpdateTime)
      {
         UpdatePosition(simTime);
      }
      UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
   }
}

// =================================================================================================
//! Return 'true' if the part is slewing to meet a cueing request.
//! @returns true if the part is slewing to meet a cueing request.
bool WsfArticulatedPart::IsSlewing() const
{
   bool isSlewing = false;
   if ((mSlewMode != cSLEW_FIXED) && ((mCueType != cCUED_TO_NOTHING) || mTransientCueActive))
   {
      isSlewing = mIsSlewing;
   }
   return isSlewing;
}

// =================================================================================================
//! Set the angular slew rates, limited by the maximum slew rates.
void WsfArticulatedPart::SetSlewRates(double aAzSlewRate, double aElSlewRate)
{
   mAzSlewRate = std::min(aAzSlewRate, mMaxAzSlewRate);
   mElSlewRate = std::min(aElSlewRate, mMaxElSlewRate);
}

// =================================================================================================
//! Get the actual cued orientation, subject to the slewing limits.
//! @param aAzimuth The actual cued azimuth, subject to the azimuth slewing limits. This will be zero
//! if a cue has not active or azimuth cueing is not allowed.
//! @param aElevation The actual cued elevation, subject to the elevation slewing limits. This will be
//! zero if a cue is not active or elevation cueing is not allowed.
//! @returns 'true' if the device is cued or 'false' if no cue is active.
//! @note: The values returned by this routine are the result of the last call to UpdatePosition().
bool WsfArticulatedPart::GetActualCuedOrientation(double& aAzimuth, double& aElevation) const
{
   bool cueActive = false;
   aAzimuth       = 0.0;
   aElevation     = 0.0;
   if ((mSlewMode != cSLEW_FIXED) && ((mCueType != cCUED_TO_NOTHING) || mTransientCueActive))
   {
      cueActive  = true;
      aAzimuth   = mActualCuedAz;
      aElevation = mActualCuedEl;
   }
   return cueActive;
}

// =================================================================================================
//! Get the current cued orientation.
//!
//! This returns the azimuth and elevation of the part as determined by the last call to UpdatePosition().
//! The return values depending on if the part was or is cued, and if cue/slew rates are imposed.
//! - If cue/slew rates are NOT imposed the return values will be:
//! -- The current az/el cue, subject to the cue limits.
//! -- or 0, 0 if no cue is active.
//! - If cue/slew rates ARE imposed, the return values will reflect the current position.
//! @param aAzimuth   The current cued azimuth.
//! @param aElevation The current cued elevation
//! @note: The values returned by this routine are the result of the last call to UpdatePosition().
void WsfArticulatedPart::GetCurrentCuedOrientation(double& aAzimuth, double& aElevation) const
{
   aAzimuth   = mCurrentAz;
   aElevation = mCurrentEl;
}

// =================================================================================================
//! Get the value set by SetCuedLocationWCS.
//! @param aCuedLocationWCS The current cued location if it was defined by SetCuedLocationWCS.
//! @returns true if the current cue was defined by SetCuedLocationWCS.
bool WsfArticulatedPart::GetCuedLocationWCS(double aCuedLocationWCS[3]) const
{
   bool ok = false;
   if (mCueType == cCUED_TO_LOCATION)
   {
      ok = true;
      UtVec3d::Set(aCuedLocationWCS, mCuedLocationWCS);
   }
   return ok;
}

// =================================================================================================
//! Cue the device to the specified WCS location.
//! This will define the 'non-transient' cue (replacing any existing non-transient cue). If a
//! 'transient' cue is active then this cue will not become effective until the 'transient' cue is cleared.
//! @param aCuedLocationWCS The desired cue location.
void WsfArticulatedPart::SetCuedLocationWCS(const double aCuedLocationWCS[3])
{
   // NOTE: A position update is necessary to ensure any slewing as a result of an existing cue is propagated to
   // the current time.
   double simTime = GetSimulation()->GetSimTime();
   if (simTime > mLastUpdateTime)
   {
      UpdatePosition(simTime);
   }
   UtVec3d::Set(mCuedLocationWCS, aCuedLocationWCS);
   mCueType = cCUED_TO_LOCATION;
}

// =================================================================================================
//! Sets the reference parameters with the cued azimuth and elevation of the device.
//! If the device is not cued to an orientation, no action is done.
//! @param aAzimuth Set to the current cued azimuth.
//! @param aElevation Set to the current cued elevation.
//! @return 'true' if parameters were set correctly
//!         'false' if the device is not cued to an orientation
bool WsfArticulatedPart::GetCuedOrientation(double& aAzimuth, double& aElevation) const
{
   bool ok = false;
   if (mCueType == cCUED_TO_ANGLE)
   {
      aAzimuth   = mCuedAz;
      aElevation = mCuedEl;
      ok         = true;
   }
   return ok;
}

// =================================================================================================
//! Cue the device to the specified azimuth and elevation.
//! This will define the 'non-transient' cue (replacing any existing non-transient cue). If a
//! 'transient' cue is active then this cue will not become effective until the 'transient' cue is cleared.
//! @param aAzimuth The desired angle with respect to the part coordinate system.
//! @param aElevation The desired angle with respect to the part coordinate system.
void WsfArticulatedPart::SetCuedOrientation(double aAzimuth, double aElevation)
{
   // See NOTE in SetCuedLocationWCS as to why the position update is necessary.
   double simTime = GetSimulation()->GetSimTime();
   if (simTime > mLastUpdateTime)
   {
      UpdatePosition(simTime);
   }
   mCuedAz  = UtMath::NormalizeAngleMinusPi_Pi(aAzimuth);
   mCuedEl  = UtMath::Limit(aElevation, UtMath::cPI_OVER_2);
   mCueType = cCUED_TO_ANGLE;
}

// =================================================================================================
//! Gets the part specified azimuth and elevation relative to the horizontal plane
//! (no pitch or roll) of the host platform.
//! @param aAzimuth The desired angle (in radians) with respect to aBaseNED.
//! @param aElevation The desired angle (in radians) with respect to aBaseNED.
//! @param aBaseNED The NED values in which to compare to
void WsfArticulatedPart::GetAbsoluteCuedOrientation(double& aAzimuth, double& aElevation, double aBaseNED[3])
{
   // All cues must be cleared prior to computing the aspect, just in case there was already a cue.
   double currentAz = mCurrentAz;
   double currentEl = mCurrentEl;
   ClearCueing();
   ClearTransientCue();
   UpdatePosition(GetSimulation()->GetSimTime());
   double lat, lon, alt;
   GetLocationLLA(lat, lon, alt);
   UtEntity entity;
   entity.SetLocationLLA(lat, lon, alt);
   entity.SetOrientationNED(aBaseNED[0], aBaseNED[1], aBaseNED[2]);
   double relCueLocWCS[3];
   entity.GetRelativeLocationWCS(aAzimuth, aElevation, 1.0E+6, relCueLocWCS);
   ComputeAspect(relCueLocWCS, aAzimuth, aElevation);

   // Reset to current cue
   SetCuedOrientation(currentAz, currentEl);
}

// =================================================================================================
//! Return the current transient cue location.
//! @returns true if a transient cue is defined.
bool WsfArticulatedPart::GetTransientCuedLocationWCS(double aCuedLocationWCS[3]) const
{
   UtVec3d::Set(aCuedLocationWCS, mTransientCuedLocationWCS);
   return mTransientCueActive;
}

// =================================================================================================
//! Set the 'transient' cue to the specified WCS location.
//! This will define the 'transient' cue (replacing any existing transient cue).
//! If a 'non-transient' cue (as defined SetCuedLocationWCS) is active then it will be temporarily suspended.
//! The 'non-transient' cue will remain active until deactivated by ClearTransientCue().
//! @param aCuedLocationWCS The desired cue location.
//! @note The 'transient' cue can be used temporarily point at some other location (such as the electronic
//! steering in a phased array radar).
void WsfArticulatedPart::SetTransientCuedLocationWCS(const double aCuedLocationWCS[3])
{
   // NOTE: Unlike non-transient cues, setting and clearing the transient cue does not request a position update
   // to the current time before modifying the cue. Transient cues are used by tracking systems to set the position
   // FOR THE CURRENT TIME. If the position were updated before setting the cue, and slcue rates were employed,
   // there would be no way to satisfy the request because deltaT would be zero!

   UtVec3d::Set(mTransientCuedLocationWCS, aCuedLocationWCS);
   mTransientCueActive = true;
}

// =================================================================================================
//! Indicate that the transient cue is no longer active.
//! This clears any transient cue defined by SetTransientCuedLocationWCS(). If the device has a non-
//! transient cue (as defined by SetCuedLocationWCS()) then it becomes the active cue.
void WsfArticulatedPart::ClearTransientCue()
{
   // See NOTE in SetTransientCuedLocationWCS as to why a position update is not requested.
   mTransientCueActive = false;
   if (mCueType == cCUED_TO_NOTHING)
   {
      UtEntityPart::SetOrientation(mYaw, mPitch + mTilt, mRoll);
   }
}

// =================================================================================================
//! Return the masking factor for the specified viewing angles.
//!
//! This is called from WsfEM_Interaction::ComputeMaskingFactor to determine the masking factor for
//! the current interaction. It could obviously be called
//!
//! The first two arguments could have been eliminated but they had already been computed by
//! 'BeginXXXInteraction'. This eliminates the need to recompute them, which involves several
//! trig/math calls.
//!
//! @param aViewAzPCS The azimuth   viewing angle in the part coordinate system.
//! @param aViewElPCS The elevation viewing angle in the part coordinate system.
//! @param aViewVecWCS The unit viewing vector in the world coordinate system. This is used if a
//! platform-relative factor is defined.
double WsfArticulatedPart::GetMaskingPatternFactor(double aViewAzPCS, double aViewElPCS, const double aViewVecWCS[3])
{
   double factor = 1.0;
   if (mMaskingPatternPtr != nullptr)
   {
      factor = mMaskingPatternPtr->GetFactor(mMaskingPatternStateId, this, aViewAzPCS, aViewElPCS, aViewVecWCS);
   }
   return factor;
}

// =================================================================================================
void WsfArticulatedPart::SetPositionRateECS(const double aDX, const double aDY, const double aDZ)
{
   mDX = aDX;
   mDY = aDY;
   mDZ = aDZ;
}

// =================================================================================================
void WsfArticulatedPart::GetCurrentPositionRateECS(double& aDX, double& aDY, double& aDZ) const
{
   aDX = mDX;
   aDY = mDY;
   aDZ = mDZ;
}

// =================================================================================================
void WsfArticulatedPart::EnableArticulationUpdates(double aSimTime, double aInterval)
{
   mLastUpdateTime = aSimTime;
   IncrementArticulationUpdateEventEpoch(); // Invalidate outstanding 'update' events
   // Schedule the event to perform periodic calls to the sensors Update() method.
   GetSimulation()->AddEvent(
      ut::make_unique<WsfArticulatedPartEvent>(aSimTime, WsfArticulatedPartEvent::cUPDATE_ARTICULATION, this));
   SetArticulationUpdateInterval(aInterval);
}

// =================================================================================================
void WsfArticulatedPart::DisableArticulationUpdates(double aSimTime)
{
   IncrementArticulationUpdateEventEpoch(); // Invalidate outstanding 'update' events
}

// =================================================================================================
double WsfArticulatedPart::TimeToCue(double aDesiredAz, double aDesiredEl) const
{
   double deltaAz = aDesiredAz - mCurrentAz;
   double deltaEl = aDesiredEl - mCurrentEl;
   double azRate, elRate, timeToAz, timeToEl;
   TimeToRotate(deltaAz, deltaEl, azRate, elRate, timeToAz, timeToEl);
   return std::max(timeToAz, timeToEl);
}

// =================================================================================================
double WsfArticulatedPart::GetAzimuthRotationRate() const
{
   // By default, limit the cue rates by the slew rates (allows per-mode rates).
   return std::min(mAzCueRate, mAzSlewRate);
}

// =================================================================================================
double WsfArticulatedPart::GetElevationRotationRate() const
{
   // By default, limit the cue rates by the slew rates (allows per-mode rates).
   return std::min(mElCueRate, mElSlewRate);
}

// =================================================================================================
//! Slew the part to the requested orientation.
//!
//! This is called by UpdateCuedOrientation to perform the actual slewing operation needed to reflect the
//! requested cuing operation. This will attempt to update the orientation subject to the angular rates.
//! that are currently in effect.
//!
//! @param aSimTime The current simulation time.
//! @param aDesiredAz The desired azimuth.
//! @param aDesiredEl The desired elevation.
// protected
void WsfArticulatedPart::SlewToOrientation(double aSimTime, double aDesiredAz, double aDesiredEl)
{
   static const double cCUE_HIT_TOLERANCE = 1.0E-6 * UtMath::cRAD_PER_DEG;

   // NOTE: Do not exit this routine early because of a small delta T.  If the rates indicate 'instantaneous'
   // cueing is to be performed then the new orientation must occur in the same timestep (if requested).

   double deltaT = aSimTime - mLastAzElUpdateTime;

   // Limit the cue rates by the slew rates (allows per-mode rates).

   double azRateBase = GetAzimuthRotationRate();
   double elRateBase = GetElevationRotationRate();

   // Determine the amount of change required in each direction.

   double deltaAz = aDesiredAz - mCurrentAz;
   double deltaEl = aDesiredEl - mCurrentEl;

   // Calculate the time to rotate

   double azRate, elRate, timeToAz, timeToEl;
   TimeToRotate(deltaAz, deltaEl, azRate, elRate, timeToAz, timeToEl);

   // Slewing is completed after it changes from slewing to not slewing.
   bool prevSlewing{mIsSlewing};

   mIsSlewing = false;

   // Update the azimuth position, subject to the maximum allowable in this timestep.

   if (azRateBase >= cINFINITE_SLEW_RATE)
   {
      // Instantaneous cueing
      // If changed, then slewing occurred
      if (mCurrentAz != aDesiredAz)
      {
         mCurrentAz  = aDesiredAz;
         prevSlewing = true;
      }
   }
   else
   {
      // Limit to maximum angle change in this timestep.
      double maxDeltaAz = azRate * deltaT;
      if (fabs(deltaAz) <= maxDeltaAz)
      {
         mCurrentAz = aDesiredAz;
      }
      else
      {
         // Must be careful in azimuth because of the possibility of wrap-around.
         if (deltaAz > 0.0)
         {
            mCurrentAz = UtMath::NormalizeAngleMinusPi_Pi(mCurrentAz + maxDeltaAz);
         }
         else
         {
            mCurrentAz = UtMath::NormalizeAngleMinusPi_Pi(mCurrentAz - maxDeltaAz);
         }
         double azRemaining = fabs(mCurrentAz - aDesiredAz);
         if (FullCircle() && (azRemaining > UtMath::cPI))
         {
            azRemaining = UtMath::cTWO_PI - azRemaining;
         }
         mIsSlewing |= (azRemaining > cCUE_HIT_TOLERANCE);
      }
   }

   // Update the elevation position, subject to the maximum allowable in this timestep.

   if (elRateBase >= cINFINITE_SLEW_RATE)
   {
      // Instantaneous cueing
      // If changed, then slewing occurred
      if (mCurrentEl != aDesiredEl)
      {
         mCurrentEl  = aDesiredEl;
         prevSlewing = true;
      }
   }
   else
   {
      // Limit to maximum angle change in this timestep.
      double maxDeltaEl = elRate * deltaT;
      if (fabs(deltaEl) <= maxDeltaEl)
      {
         mCurrentEl = aDesiredEl;
      }
      else
      {
         if (deltaEl > 0.0)
         {
            mCurrentEl = std::min(mCurrentEl + maxDeltaEl, UtMath::cPI_OVER_2);
         }
         else
         {
            mCurrentEl = std::max(mCurrentEl - maxDeltaEl, -UtMath::cPI_OVER_2);
         }
         mIsSlewing |= (fabs(mCurrentEl - aDesiredEl) > cCUE_HIT_TOLERANCE);
      }
   }

   mLastAzElUpdateTime = aSimTime;

   // The part was slewing and is no longer slewing
   if (prevSlewing && !mIsSlewing)
   {
      SlewComplete(aSimTime);
   }
}

// =================================================================================================
void WsfArticulatedPart::TimeToRotate(double& aDeltaAz,
                                      double& aDeltaEl,
                                      double& aAzRate,
                                      double& aElRate,
                                      double& aTimeToAz,
                                      double& aTimeToEl) const
{
   // Get the rotation rate to use

   aAzRate = GetAzimuthRotationRate();
   aElRate = GetElevationRotationRate();

   // If the angle delta is greater than 180 degree, and if the device can rotate a full 360 degrees,
   // take the 'short' way around.

   switch (mAzSlewDirection)
   {
   case cUNDEFINED:
   {
      if (FullCircle())
      {
         if (aDeltaAz > UtMath::cPI)
         {
            aDeltaAz -= UtMath::cTWO_PI;
         }
         else if (aDeltaAz < (-UtMath::cPI))
         {
            aDeltaAz += UtMath::cTWO_PI;
         }
      }
      break;
   }
   case cPOSITIVE:
      if (aDeltaAz < 0.0)
      {
         aDeltaAz += UtMath::cTWO_PI;
      }
      break;
   case cNEGATIVE:
      if (aDeltaAz > 0.0)
      {
         aDeltaAz -= UtMath::cTWO_PI;
      }
      break;
   }

   aTimeToAz = std::fabs(aDeltaAz / aAzRate);
   aTimeToEl = std::fabs(aDeltaEl / aElRate);

   // If 'coordinated' slewing/cueing is used, adjust the rate of the direction that would hit the desired value
   // first so that it reaches its desired value at the same time as the other direction.
   //
   // If 'independent' slewing/cueing is used, each direction is moved independently until it hits its desired value.

   if (mSlewMethodCoordinated)
   {
      // Determine the amount of time required to reach the desired value in each direction and
      // adjust rate of the earliest arriver so it will coincide with the later arriver.

      if (aTimeToAz > aTimeToEl)
      {
         aElRate *= aTimeToEl / aTimeToAz;
      }
      else if (aTimeToAz < aTimeToEl)
      {
         aAzRate *= aTimeToAz / aTimeToEl;
      }
   }
}

// =================================================================================================
//! Update the orientation to reflect the current cue.
//! This is called by UpdateOrientation to attempt to update the orientation to the requested cue.
//! @param aSimTime The current simulation time.
// protected
void WsfArticulatedPart::UpdateCuedOrientation(double aSimTime)
{
   double thisToCuedLocWCS[3];
   double cuedAz = mCuedAz; // Assume cued to an orientation
   double cuedEl = mCuedEl;

   // Compute the aspect of the cued point relative to the to the uncued sensor.

   UtEntityPart::SetOrientation(mYaw, mPitch, mRoll); //! Do NOT add mTilt here as it *SHOULD* get added later
   if (mTransientCueActive)
   {
      UtEntityPart::GetRelativeLocationWCS(mTransientCuedLocationWCS, thisToCuedLocWCS);
      UtEntityPart::ComputeAspect(thisToCuedLocWCS, cuedAz, cuedEl);
   }
   else if (mCueType == cCUED_TO_LOCATION)
   {
      UtEntityPart::GetRelativeLocationWCS(mCuedLocationWCS, thisToCuedLocWCS);
      UtEntityPart::ComputeAspect(thisToCuedLocWCS, cuedAz, cuedEl);
   }

   mSlewState = cNOT_CUED;

   // Limit the cue mode with the slew mode.

   int cueMode = static_cast<int>(mCueMode) & static_cast<int>(mSlewMode);

   // If the device can slew in azimuth then determine the azimuth to be used.

   if ((cueMode & cSLEW_AZ) != 0)
   {
      // Make sure the azimuth is within the cue limits

      double minAzCue = std::max(mMinAzCue, mMinAzSlew);
      double maxAzCue = std::min(mMaxAzCue, mMaxAzSlew);
      if ((cuedAz < minAzCue) || (cuedAz > maxAzCue))
      {
         // Cue is outside the azimuth limit. Set position to the closest limit
         double deltaMin = UtMath::NormalizeAngle0_TwoPi(minAzCue - cuedAz);
         double deltaMax = UtMath::NormalizeAngle0_TwoPi(cuedAz - maxAzCue);
         if (deltaMin <= deltaMax)
         {
            cuedAz = minAzCue; // Set position to the min limit
            mSlewState |= cMIN_AZ;
         }
         else
         {
            cuedAz = maxAzCue; // Set position to the max limit
            mSlewState |= cMAX_AZ;
         }
      }
      else
      {
         // cuedAz = cuedAz;              // Point right at the cue (in azimuth)
         mSlewState |= cCUED_AZ;
      }
   }
   else
   {
      cuedAz = 0.0; // Cannot cue in azimuth
   }

   // If the device can slew in elevation then determine the elevation to be used.

   if ((cueMode & cSLEW_EL) != 0)
   {
      // Make sure the elevation is within the slew limits

      double minElCue = std::max(mMinElCue, mMinElSlew);
      double maxElCue = std::min(mMaxElCue, mMaxElSlew);
      if (cuedEl < minElCue)
      {
         cuedEl = minElCue; // Set position to the min limit
         mSlewState |= cMIN_EL;
      }
      else if (cuedEl > maxElCue)
      {
         cuedEl = maxElCue; // Set position to the max limit
         mSlewState |= cMAX_EL;
      }
      else
      {
         // cuedEl = cuedEl;              // Point right at the cue (in elevation)
         mSlewState |= cCUED_EL;
      }
   }
   else
   {
      cuedEl = mTilt; // Cannot cue in elevation
   }

   // Slew the device to the angles just determined. Note that SlewToOrientation will only update to
   // the values that can be achieved in the current time step.

   mActualCuedAz = cuedAz;
   mActualCuedEl = cuedEl;
   SlewToOrientation(aSimTime, cuedAz, cuedEl);
   UtEntityPart::Rotate(mCurrentAz, mCurrentEl, 0.0);
}

// =================================================================================================
//! Update the orientation to the reflect the orientation as it should be at specified time.
//! This is an internal routine that is called to ensure that the orientation of the part is up-to-date.
//! @returns 'true' if a cue is active.
bool WsfArticulatedPart::UpdateOrientation(double aSimTime)
{
   bool updatePerformed = false;
   if ((mSlewMode != cSLEW_FIXED) && ((mCueType != cCUED_TO_NOTHING) || mTransientCueActive))
   {
      updatePerformed = true;
      UpdateCuedOrientation(GetSimulation()->GetSimTime());
   }
   mLastAzElUpdateTime = aSimTime;
   return updatePerformed;
}

const char* WsfArticulatedPart::GetScriptClassName() const
{
   return "WsfArticulatedPart";
}
