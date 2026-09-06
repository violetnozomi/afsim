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

#ifndef WSFARTICULATEDPART_HPP
#define WSFARTICULATEDPART_HPP

#include "UtCallback.hpp"
#include "UtEntityPart.hpp"
class UtInput;
#include "wsf_export.h"

#include "UtMath.hpp"
#include "WsfComponent.hpp"
class WsfMaskingPattern;
class WsfPlatform;
#include "WsfPlatformPart.hpp"
class WsfScenario;
#include "WsfSinglePlatformObserver.hpp"

//! A base class for platform part that represents an articulated part of the platform.
//!
//! This class is intended to be the base class for those parts of a platform that
//! have a position and orientation on the platform. The class provides the means
//! to specify the location and orientation of the part and to 'cue' the part.
//!
//! Example uses of this class would be sensor and comm systems.
class WSF_EXPORT WsfArticulatedPart : public WsfPlatformPart, public UtEntityPart, public WsfSinglePlatformObserver
{
public:
   static constexpr double cINFINITE_SLEW_RATE = 1.0E+12 * UtMath::cRAD_PER_DEG;
   static constexpr double cALMOST_FULL_CIRCLE = 359.999 * UtMath::cRAD_PER_DEG;

   //! An enumeration that indicates the what degrees-of-freedom exist for slewing
   //! Note: Do not change these values. cSLEW_AZ_EL must equal cSLEW_AZ | cSLEW_EL.
   enum SlewMode
   {
      cSLEW_FIXED = 0, //<! The part cannot be slewed
      cSLEW_AZ    = 1, //<! The part can be slewed only in azimuth
      cSLEW_EL    = 2, //<! The part can be slewed only in elevation
      cSLEW_AZ_EL = 3  //<! The part can be slewed in both azimuth and elevation
   };

   enum SlewDirection
   {
      cUNDEFINED = 0, //<! The slew direction is undefined
      cPOSITIVE  = 1, //<! Slew direction is through positive angle
      cNEGATIVE  = 2  //<! Slew direction is through negative angle
   };

   enum // slewState
   {
      cNOT_CUED         = 0x0000,
      cMIN_AZ           = 0x0001,
      cMAX_AZ           = 0x0002,
      cMIN_EL           = 0x0004,
      cMAX_EL           = 0x0008,
      cAT_LIMIT_AZ_MASK = 0x0003,
      cAT_LIMIT_EL_MASK = 0x000B,
      cAT_LIMIT_MASK    = 0x000F, // "and" (&) with this for any at limit state
      cCUED_AZ          = 0x0010,
      cCUED_EL          = 0x0020,
      cCUED_AZ_EL       = 0x0030, // = cCUED_AZ + cCUED_EL
      cCUED_MASK        = 0x0030  // "and" (&) with this for any cued state
   };

   enum CueType
   {
      cCUED_TO_NOTHING,  //!< No cue has been specified
      cCUED_TO_LOCATION, //!< The part is cued to a specified location
      cCUED_TO_ANGLE     //!< The part is cued to specified az/el angles
   };

   WsfArticulatedPart(const WsfScenario& aScenario, int aPartType);
   WsfArticulatedPart(const WsfArticulatedPart& aSrc);
   ~WsfArticulatedPart() override;

   //! @name Common infrastructure methods.
   //@{
   const int* GetComponentRoles() const override;
   void*      QueryInterface(int aRole) override;

   const char* GetScriptClassName() const override;
   bool        Initialize(double aSimTime) override;
   bool        ProcessInput(UtInput& aInput) override;
   void        SetPlatform(WsfPlatform* aPlatformPtr) override;
   //@}

   virtual void UpdatePosition(double aSimTime);

   //! @name Overrides of WsfSinglePlatformObserver.
   //@{
   //! A callback from the platform subject.
   //! This class attaches itself as an observer to the associated platform and
   //! through this method is notified whenever the platform is updated (moved).
   void OnPlatformUpdated(double /* aSimTime */, WsfPlatform* /*aPlatformPtr*/) override { InvalidateTransform(); }
   //@}

   //! @name Slewing methods.
   //@{
   //! Get the allowed slewing mode.
   SlewMode GetSlewMode() const { return mSlewMode; }

   //! Get the current azimuth slew rate.
   double GetAzSlewRate() const { return mAzSlewRate; }
   //! Get the current elevation slew rate.
   double GetElSlewRate() const { return mElSlewRate; }

   //! Get the maximum allowable azimuth slew rate.
   double GetMaxAzSlewRate() const { return mMaxAzSlewRate; }
   //! Get the maximum allowable elevation slew rate.
   double GetMaxElSlewRate() const { return mMaxElSlewRate; }

   //! Gets the minimum azimuth slew limit.
   double GetMinAzSlew() const { return mMinAzSlew; }
   //! Gets the maximum azimuth slew limit.
   double GetMaxAzSlew() const { return mMaxAzSlew; }
   //! Gets the minimum elevation slew limit.
   double GetMinElSlew() const { return mMinElSlew; }
   //! Gets the maximum elevation slew limit.
   double GetMaxElSlew() const { return mMaxElSlew; }

   //! Get the slew limit state.
   int GetSlewState() const { return mSlewState; }

   //! Set the allowed slewing mode.
   void SetSlewMode(SlewMode aSlewMode) { mSlewMode = aSlewMode; }

   void SetSlewRates(double aAzSlewRate, double aElSlewRate);

   //! Set the azimuth slew limits.
   void SetAzSlewLimits(double aMinAzSlew, double aMaxAzSlew)
   {
      mMinAzSlew = aMinAzSlew;
      mMaxAzSlew = aMaxAzSlew;
   }

   //! Set the elevation slew limits.
   void SetElSlewLimits(double aMinElSlew, double aMaxElSlew)
   {
      mMinElSlew = aMinElSlew;
      mMaxElSlew = aMaxElSlew;
   }

   //! Set the azimuth slew direction.
   void SetAzimuthSlewDirection(SlewDirection aAzSlewDirection) { mAzSlewDirection = aAzSlewDirection; }

   bool IsSlewing() const;

   //! Is the target within the slew limits?
   //!
   //! @param aThisToTgtAz [input] Azimuth of the target with respect to the device (radians).
   //! @param aThisToTgtEl [input] Elevation of the target with respect to the device (radians).
   //! @returns 'true' if within the slew limits or 'false' if not.
   bool WithinSlewLimits(double aThisToTgtAz, double aThisToTgtEl) const
   {
      return ((aThisToTgtAz >= mMinAzSlew) && (aThisToTgtAz <= mMaxAzSlew) && (aThisToTgtEl >= mMinElSlew) &&
              (aThisToTgtEl <= mMaxElSlew));
   }
   //@}

   //! @name Cue limit methods.
   //! The cue limits are maintained separate and are forced to be subset of the slew limits.
   //! For most uses the cue limits end up being the same as the slew limits, but there are
   //! some multi-mode sensors which limit the cueing capability in some modes.
   //@{
   //! Get the allowed cueing mode.
   SlewMode GetCueMode() const { return mCueMode; }

   //! Get the azimuth cue rate.
   double GetAzCueRate() const { return mAzCueRate; }
   //! Get the elevation cue rate.
   double GetElCueRate() const { return mElCueRate; }
   //! Gets the minimum azimuth cue limit.
   double GetMinAzCue() const { return mMinAzCue; }
   //! Gets the maximum azimuth cue limit.
   double GetMaxAzCue() const { return mMaxAzCue; }
   //! Gets the minimum elevation cue limit.
   double GetMinElCue() const { return mMinElCue; }
   //! Gets the maximum elevation cue limit.
   double GetMaxElCue() const { return mMaxElCue; }

   //! Set the allowed cueing mode.
   void SetCueMode(SlewMode aCueMode) { mCueMode = aCueMode; }

   //! Set the angular cueing rates.
   void SetCueRates(double aAzCueRate, double aElCueRate)
   {
      mAzCueRate = aAzCueRate;
      mElCueRate = aElCueRate;
   }

   //! Set the azimuth cueing limits.
   void SetAzCueLimits(double aMinAzCue, double aMaxAzCue)
   {
      mMinAzCue = aMinAzCue;
      mMaxAzCue = aMaxAzCue;
   }

   //! Set the elevation cueing limits.
   void SetElCueLimits(double aMinElCue, double aMaxElCue)
   {
      mMinElCue = aMinElCue;
      mMaxElCue = aMaxElCue;
   }
   //@}

   //! @name Orientation methods.
   //! All angles are in radians!
   //@{
   double GetRoll() const { return mRoll; }
   void   SetRoll(double aRoll);

   double GetPitch() const { return mPitch; }
   void   SetPitch(double aPitch);

   double GetYaw() const { return mYaw; }
   void   SetYaw(double aYaw);

   double GetTilt() const { return mTilt; }
   void   SetTilt(double aTilt);

   void GetCurrentOrientationECS(double& aYaw, double& aPitch, double& aRoll);

   void GetLOS_UnitVectorWCS(double aUnitLOS_WCS[3]);
   //@}

   //! @name Cueing methods.
   //! An articulated part can have two types of cues:
   //! - A non-transient cue.
   //! - A transient cue.
   //! The difference between the two is that the transient cue temporarily overrides the non-transient
   //! cue until it is cleared.
   //@{
   CueType GetCueType() const { return mCueType; }

   //! Get the cued status.
   bool IsCued() const { return mCueType != cCUED_TO_NOTHING; }

   void ClearCueing();

   bool GetActualCuedOrientation(double& aAzimuth, double& aElevation) const;

   void GetCurrentCuedOrientation(double& aAzimuth, double& aElevation) const;

   bool GetCuedLocationWCS(double aCuedLocationWCS[3]) const;

   void SetCuedLocationWCS(const double aCuedLocationWCS[3]);

   bool GetCuedOrientation(double& aAzimuth, double& aElevation) const;

   void SetCuedOrientation(double aAzimuth, double aElevation);

   void GetAbsoluteCuedOrientation(double& aAzimuth, double& aElevation, double aBaseNED[3]);

   void ClearTransientCue();

   bool TransientCueActive() const { return mTransientCueActive; }

   bool GetTransientCuedLocationWCS(double aCuedLocationWCS[3]) const;

   void SetTransientCuedLocationWCS(const double aCuedLocationWCS[3]);
   //@}

   //! @name Structure masking methods.
   //@{
   WsfStringId GetMaskingPatternState() const { return mMaskingPatternStateId; }
   void SetMaskingPatternState(WsfStringId aMaskingPatternStateId) { mMaskingPatternStateId = aMaskingPatternStateId; }
   double GetMaskingPatternFactor(double aViewAzPCS, double aViewElPCS, const double aViewVecWCS[3]);
   //@}

   //! @name External visibility control.
   //!
   //! An articulated part that is declared 'private' will not be visible to external simulations.
   //! This is primarily used to hide certain sensors and interferers from the DIS/HLA interface that
   //! are presented to the interface by some other means. (e.g.: WsfCompositeSensor in synchronous mode)
   //!
   //! @note This must be done prior to initialization.
   //@{
   void SetIsPrivate(bool aIsPrivate) { mIsPrivate = aIsPrivate; }
   bool IsPrivate() const { return mIsPrivate; }
   //@}

   //! @name Visual Articulation Methods.
   //! These methods are used to control part velocity for dynamic articulation.
   //! This is primarily used only for visualization purposes.
   //@{
   void SetPositionRateECS(const double aDX, const double aDY, const double aDZ);

   void GetCurrentPositionRateECS(double& aDX, double& aDY, double& aDZ) const;

   double GetDX() const { return mDX; }
   double GetDY() const { return mDY; }
   double GetDZ() const { return mDZ; }

   void EnableArticulationUpdates(double aSimTime, double aInterval);
   void DisableArticulationUpdates(double aSimTime);

   //! Set the time interval between the periodic calls to Update().
   //! A value of less than or equal to zero indicates periodic updates are not to be performed.
   //! @param aUpdateInterval The new update interval.
   //! @note Users should call WsfSimulation::GetInstance().SetArticulationUpdateInterval() to set the interval.
   void SetArticulationUpdateInterval(double aUpdateInterval) { mArticulationUpdateInterval = aUpdateInterval; }

   double GetArticulationUpdateInterval() const { return mArticulationUpdateInterval; }

   //! Get the current update event 'epoch'.
   //! The event 'epoch' is used to recognize and ignore obsolete events.
   unsigned int GetArticulationUpdateEventEpoch() const { return mArticulationUpdateEventEpoch; }

   //! Increment the 'epoch' for the update event.
   //! This should be called whenever the update interval is changed or if the processor is turned off or on.
   void IncrementArticulationUpdateEventEpoch() { ++mArticulationUpdateEventEpoch; }

   //! Is an 'Update' event active?
   bool ArticulationUpdateEventActive() const { return mArticulationUpdateEventActive; }

   //! Indicate if the 'Update' event is active.
   void SetArticulationUpdateEventActive(bool aUpdateEventActive)
   {
      mArticulationUpdateEventActive = aUpdateEventActive;
   }
   //@}

   //! Calculate the time it takes to rotate a part given the requested Azimuth and Elevation.
   //!@param aDesiredAz the desired azimuth (radians).
   //!@param aDesiredEl the desired elevation (radians).
   //!@return the maximum time to slew to the given azimuth or elevation.
   virtual double TimeToCue(double aDesiredAz, double aDesiredEl) const;

   UtCallbackListN<void(double)> SlewComplete;

protected:
   bool FullCircle() const { return ((mMaxAzSlew - mMinAzSlew) >= cALMOST_FULL_CIRCLE); }

   virtual double GetAzimuthRotationRate() const;
   virtual double GetElevationRotationRate() const;

   void SlewToOrientation(double aSimTime, double aDesiredAz, double aDesiredEl);

   virtual void TimeToRotate(double& aDeltaAz,
                             double& aDeltaEl,
                             double& aAzRate,
                             double& aElRate,
                             double& aTimeToAz,
                             double& aTimeToEl) const;

   void UpdateCuedOrientation(double aSimTime);

   bool UpdateOrientation(double aSimTime);

   //! The masking factor.
   //@{
   WsfMaskingPattern* mMaskingPatternPtr;
   WsfStringId        mMaskingPatternName;
   WsfStringId        mMaskingPatternStateId;
   //@}

   //! The orientation of the device coordinate system.
   //@{
   double mYaw;
   double mPitch;
   double mRoll;
   //@}

   //! The velocity of the part
   //@{
   double mDX;
   double mDY;
   double mDZ;
   //@}

   //! The tilt angle above below the scan plane
   double mTilt;

   //! The slewing rates and limits of the device with respect to the device coordinate system.
   //@{
   double mAzSlewRate;
   double mElSlewRate;
   double mMaxAzSlewRate;
   double mMaxElSlewRate;
   double mMinAzSlew;
   double mMaxAzSlew;
   double mMinElSlew;
   double mMaxElSlew;
   //@}

   SlewMode      mSlewMode;
   SlewDirection mAzSlewDirection;
   int           mSlewState;

   //! The cueing rates and limits of the device with respect to the device coordinate system.
   //@{
   double mAzCueRate;
   double mElCueRate;
   double mMinAzCue;
   double mMaxAzCue;
   double mMinElCue;
   double mMaxElCue;
   //@}

   //! The directions (az and/or el) in which cueing can be performed.
   SlewMode mCueMode;

   //! The type of the current cue.
   //! The current cue is temporarily suspended if a transient cue is active.
   CueType mCueType;
   bool    mTransientCueActive; //!< 'true' if the transient cue is defined

   double mCuedLocationWCS[3];          //!< The cued location  if mCueType is cCUED_TO_LOCATION
   double mCuedAz;                      //!< The cued azimuth   if mCueType == cCUED_TO_ANGLE
   double mCuedEl;                      //!< The cued elevation if mCueType == cCUED_TO_ANGLE
   double mTransientCuedLocationWCS[3]; //!< The transient cue, if mTransientCueActive is true

   //! The actual cued location, subject to the slewing limits.
   //@{
   double mActualCuedAz;
   double mActualCuedEl;
   //@}

   double mCurrentAz;
   double mCurrentEl;

   double mLastAzElUpdateTime; //!< The time when the current az/el were last updated

   double mLastUpdateTime; //!< The time when the x/y/z position was last updated

   //! 'true' if slcue rates are dynamically adjusted so the target angles are reached simultaneously.
   bool mSlewMethodCoordinated;

   //! 'true' if the part is slewing to satisfy a cueing request.
   bool mIsSlewing;

   //! 'true' if the part is considered 'private' and not to visible to external simulations.
   bool mIsPrivate;

private:
   //! Assignment operator declared but not defined to prevent use.
   WsfArticulatedPart& operator=(const WsfArticulatedPart& aRhs) = delete;

   //! The update interval is time between the periodic calls to Update() by the simulation executive.
   //! If less than or equal to zero then the simulation executive should not make the periodic calls.
   //!
   //! This should be the time between sensing chances.
   double       mArticulationUpdateInterval;
   unsigned int mArticulationUpdateEventEpoch;
   bool         mArticulationUpdateEventActive;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfArticulatedPart, cWSF_COMPONENT_ARTICULATED_PART)

#endif
