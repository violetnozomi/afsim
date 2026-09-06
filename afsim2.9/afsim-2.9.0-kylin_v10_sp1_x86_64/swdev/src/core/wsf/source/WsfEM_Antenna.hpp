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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFEM_ANTENNA_HPP
#define WSFEM_ANTENNA_HPP

#include "wsf_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"

class UtInput;
#include "UtScriptAccessible.hpp"
class WsfArticulatedPart;
class WsfEM_XmtrRcvr;
class WsfFieldOfView;
class WsfPlatform;
#include "WsfSinglePlatformObserver.hpp"

//! An object that represents the physical location and orientation of the antenna.
//!
//! Every transmitter or receiver has an associated 'antenna' which defines several properties
//! about the antenna:
//!
//! - The location and orientation of the antenna with respect to the host platform.
//! - The field of view of the antenna.
//! - The ability of the antenna to 'scan'.
//!
//! One of these objects can be shared to represent an an antenna that is shared between a
//! transmitter and receiver. The WsfEM_Antenna object itself is also associated with a
//! WsfArticulatedPart, which serves at the geometry source for the antenna.
//!
//! Note that this does NOT define the gain pattern. The gain pattern (as defined by WsfAntennaPattern)
//! is attached to the transmitter or receiver that represents the 'beam'. A complex system may have
//! multiple 'beams' that are all transmitted or received through the same antenna!
class WSF_EXPORT WsfEM_Antenna : public UtScriptAccessible, public WsfSinglePlatformObserver
{
public:
   //! The scan mode indicates how the beam scans within the field of view.
   enum ScanMode
   {
      cSCAN_FIXED = 0, //<! The beam is fixed
      cSCAN_AZ    = 1, //<! The beam scans only is azimuth
      cSCAN_EL    = 2, //<! The beam scans only in elevation
      cSCAN_AZ_EL = 3  //<! The beam can scan in both azimuth and elevation
   };

   //! Electronic beam steering capabilities.
   enum EBS_Mode
   {
      cEBS_NONE      = 0, //!< No electronic beam steering
      cEBS_AZIMUTH   = 1, //!< Electronic beam steering in azimuth
      cEBS_ELEVATION = 2, //!< Electronic beam steering in elevation
      cEBS_BOTH      = 3  //!< Electronic beam steering in azimuth and elevation
   };

   //! The scan stabilization indicates in which axes scan stabilization is performed.
   enum ScanStabilization
   {
      cSS_NONE  = 0, //!< Scan is not stabilization
      cSS_PITCH = 1, //!< Scan is stabilized in pitch
      cSS_ROLL  = 2, //!< Scan is stabilized in roll
      cSS_BOTH  = 3  //!< Scan is stabilized in pitch and roll
   };


   WsfEM_Antenna();
   WsfEM_Antenna(const WsfEM_Antenna& aSrc);
   WsfEM_Antenna& operator=(const WsfEM_Antenna& aRhs);
   ~WsfEM_Antenna() override;

   WsfArticulatedPart* GetArticulatedPart() const { return mArticulatedPartPtr; }

   WsfPlatform* GetPlatform() const;

   virtual bool Initialize(WsfArticulatedPart* aArticulatedPartPtr);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void UpdatePosition(double aSimTime);

   const char* GetScriptClassName() const override { return "WsfEM_Antenna"; }

   //! @name Antenna parameter definition methods.
   //@{

   //! Get the height of the antenna with respect articulated part to which the antenna is attached.
   //! @return The antenna height (meters).
   //! @note The antenna height is designed to be used only for things like physically scanning radars.
   //! This z-component of the host articulated part could also be used, but implementation of multi-beam
   //! systems would be more complicated because each would require their own articulated part to represent
   //! their own height. That would messy because each part would have to be updated with cueing information
   //! from the main part.
   double GetHeight() const { return -mOffset[2]; }

   void SetHeight(double aHeight);

   //! Get the antenna pitch (tilt) angle.
   //! @return the antenna pitch angle (radians).
   //! @note The antenna tilt is designed to be used only for things like physically scanning radars. The
   //! tilt angle is the angle above/below the scan plane (the x-y plane) of the host articulated part. The
   //! 'pitch' of the articulated part cannot be used to model antenna tilt because it would tilt the scan plane.
   double GetPitch() const { return mPitch; }

   void SetPitch(double aPitch);

   void SetFieldOfView(std::unique_ptr<WsfFieldOfView> aFieldOfViewPtr);

   //! Return the azimuth field of view.
   void GetAzimuthFieldOfView(double& aMinAzFOV, double& aMaxAzFOV) const;

   void SetAzimuthFieldOfView(double aMinAzFOV, double aMaxAzFOV);

   //! Return the elevation field of view.
   void GetElevationFieldOfView(double& aMinElFOV, double& aMaxElFOV) const;

   void SetElevationFieldOfView(double aMinElFOV, double aMaxElFOV);

   //! Get the range limits
   void GetRangeLimits(double& aMinRange, double& aMaxRange) const
   {
      aMinRange = mMinRange;
      aMaxRange = mMaxRange;
   }

   //! Return the maximum range.
   double GetMaximumRange() const { return mMaxRange; }

   //! Return the minimum range.
   double GetMinimumRange() const { return mMinRange; }

   void SetRangeLimits(double aMinRange, double aMaxRange);

   void SetMaximumRange(double aMaxRange) { mMaxRange = aMaxRange; }

   void SetMinimumRange(double aMinRange) { mMinRange = aMinRange; }

   //! Get the altitude limits
   void GetAltitudeLimits(double& aMinAlt, double& aMaxAlt) const
   {
      aMinAlt = mMinAlt;
      aMaxAlt = mMaxAlt;
   }

   //! Return the maximum altitude.
   double GetMaximumAltitude() const { return mMaxAlt; }

   //! Return the minimum altitude.
   double GetMinimumAltitude() const { return mMinAlt; }

   void SetAltitudeLimits(double aMinAlt, double aMaxAlt);

   void SetMaximumAltitude(double aMaxAltitude) { mMaxAlt = aMaxAltitude; }

   void SetMinimumAltitude(double aMinAltitude) { mMinAlt = aMinAltitude; }

   //! Get the electronic beam steering capabilities.
   EBS_Mode GetEBS_Mode() const { return mEBS_Mode; }

   //! Set the electronic beam steering capabilities.
   void SetEBS_Mode(EBS_Mode aEBS_Mode) { mEBS_Mode = aEBS_Mode; }

   //! Get the azimuth specific steering loss exponent.
   double GetEBS_AzLossExponent() const { return mEBS_AzLossExponent; }

   //! Get the elevation specific steering loss exponent.
   double GetEBS_ElLossExponent() const { return mEBS_ElLossExponent; }

   //! Set the electronic beam steering exponent specific to azimuth dropoff.
   void SetEBS_AzLossExponent(double aEBS_LossExponentAz) { mEBS_AzLossExponent = aEBS_LossExponentAz; }

   //! Set the electronic beam steering exponent specific to elevation dropoff.
   void SetEBS_ElLossExponent(double aEBS_LossExponentEl) { mEBS_ElLossExponent = aEBS_LossExponentEl; }

   //! Get the azimuth electronic beam steering cosine steering limit.
   //! The cosine steering limit used in computing the electronic steering losses (cos(steering limit angle))
   double GetEBS_AzCosSteeringLimit() const { return mEBS_AzCosSteeringLimit; }
   //! Get the electronic beam steering cosine steering limit.
   //! The cosine steering limit used in computing the electronic steering losses (cos(steering limit angle))
   double GetEBS_ElCosSteeringLimit() const { return mEBS_ElCosSteeringLimit; }

   //! Set the electronic beam steering cosine steering limit in azimuth.
   void SetEBS_AzCosSteeringLimit(double aEBS_AzCosSteeringLimit) { mEBS_AzCosSteeringLimit = aEBS_AzCosSteeringLimit; }
   //! Set the electronic beam steering cosine steering limit in elevation.
   void SetEBS_ElCosSteeringLimit(double aEBS_ElCosSteeringLimit) { mEBS_ElCosSteeringLimit = aEBS_ElCosSteeringLimit; }

   //! Get the beam scanning mode
   ScanMode GetScanMode() const { return mScanMode; }

   //! Set the beam scanning mode.
   void SetScanMode(ScanMode aScanMode) { mScanMode = aScanMode; }

   //! Get the scan stabilization mode.
   ScanStabilization GetScanStabilization() const { return mScanStabilization; }

   //! Set the scan stabilization mode.
   void SetScanStabilization(ScanStabilization aScanStabilization) { mScanStabilization = aScanStabilization; }

   //! Get the azimuth scan limits.
   void GetAzimuthScanLimits(double& aMinAzScan, double& aMaxAzScan) const
   {
      aMinAzScan = mMinAzScan;
      aMaxAzScan = mMaxAzScan;
   }

   //! Return the minimum azimuth scan limit.
   double GetMinimumAzimuthScanLimit() const { return mMinAzScan; }

   //! Return the maximum azimuth scan limit.
   double GetMaximumAzimuthScanLimit() const { return mMaxAzScan; }

   void SetAzimuthScanLimits(double aMinAzScan, double aMaxAzScan);

   void SetMaximumAzimuthScanLimit(double aMaxAzScan) { mMaxAzScan = aMaxAzScan; }
   void SetMinimumAzimuthScanLimit(double aMinAzScan) { mMinAzScan = aMinAzScan; }

   //! Get the elevation scan limits.
   void GetElevationScanLimits(double& aMinElScan, double& aMaxElScan) const
   {
      aMinElScan = mMinElScan;
      aMaxElScan = mMaxElScan;
   }

   //! Return the minimum elevation scan limit.
   double GetMinimumElevationScanLimit() const { return mMinElScan; }

   //! Return the maximum elevation scan limit.
   double GetMaximumElevationScanLimit() const { return mMaxElScan; }

   void SetElevationScanLimits(double aMinElScan, double aMaxElScan);

   void SetMaximumElevationScanLimit(double aMaxElScan) { mMaxElScan = aMaxElScan; }
   void SetMinimumElevationScanLimit(double aMinElScan) { mMinElScan = aMinElScan; }

   //@}

   //! @name Location and aspect methods.
   //! Most of these just pass-through to WsfArticulatedPart (UtEntityPart);
   //@{

   void ComputeAspect(const double aThisToTgtLocWCS[3], double& aThisToTgtAz, double& aThisToTgtEl);

   void ComputeBeamAspect(const double aWCS_ToBeamTransform[3][3],
                          const double aThisToTgtWCS[3],
                          double&      aBeamToTgtAz,
                          double&      aBeamToTgtEl) const;

   void ComputeBeamPosition(WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                            double          aThisToTgtAz,
                            double          aThisToTgtEl,
                            double          aWCS_ToBeamTransform[3][3],
                            double&         aEBS_Az,
                            double&         aEBS_El);

   double ComputeBeamSteeringLoss(double aEBS_Az, double aEBS_El);

   void ConvertWCS_ToACS(const double aLocationWCS[3], double aLocationACS[3]);

   void ConvertWCS_ToNED(const double aLocationWCS[3], double aLocationNED[3]);

   void ConvertNED_ToWCS(const double aLocationNED[3], double aLocationWCS[3]);

   void ConvertWCS_VectorToACS(const double aVectorWCS[3], double aVectorACS[3]);

   void ConvertWCS_VectorToNED(const double aVectorWCS[3], double aVectorNED[3]);

   void ConvertNED_VectorToWCS(const double aVectorNED[3], double aVectorWCS[3]);

   void GetLocationLLA(double& aLat, double& aLon, double& aAlt);

   void GetLocationWCS(double aLocationWCS[3]);

   void GetLocationWCS(const double aThisToTgtLocWCS[3], double aTgtLocWCS[3]);

   void GetRelativeLocationNED(WsfPlatform* aTargetPtr, double aLocationNED[3]);

   void GetRelativeLocationNED(const double aTgtLocWCS[3], double aLocationNED[3]);

   void GetRelativeLocationWCS(double aThisToTgtAz, double aThisToTgtEl, double aThisToTgtRange, double aThisToTgtLocWCS[3]);

   void GetRelativeLocationWCS(const double aTgtLocWCS[3], double aThisToTgtLocWCS[3]);

   void GetRelativeLocationWCS(WsfPlatform* aTargetPtr, double aThisToTgtLocWCS[3]);

   //@}

   //! @name Field-of-regard methods.
   //@{

   bool WithinAltitude(double aTgtAlt);

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl);

   bool WithinFieldOfView(WsfPlatform* aTargetPtr,
                          double       aEarthRadiusMultiplier,
                          double&      aTrueThisToTgtAz,
                          double&      aTrueThisToTgtEl,
                          double       aApparentThisToTgtLocWCS[3],
                          double&      aApparentThisToTgtAz,
                          double&      aApparentThisToTgtEl,
                          double       aApparentTgtToThisLocWCS[3]);

   bool WithinFieldOfView(WsfArticulatedPart* aTargetPtr,
                          double              aEarthRadiusMultiplier,
                          double&             aTrueThisToTgtAz,
                          double&             aTrueThisToTgtEl,
                          double              aApparentThisToTgtLocWCS[3],
                          double&             aApparentThisToTgtAz,
                          double&             aApparentThisToTgtEl,
                          double              aApparentTgtToThisLocWCS[3]);

   bool WithinFieldOfView(double  aTgtLocWCS[3],
                          double  aEarthRadiusMultiplier,
                          double& aTrueThisToTgtAz,
                          double& aTrueThisToTgtEl,
                          double  aApparentThisToTgtLocWCS[3],
                          double& aApparentThisToTgtAz,
                          double& aApparentThisToTgtEl,
                          double  aApparentTgtToThisLocWCS[3]);

   const WsfFieldOfView* GetFieldOfView() const { return mFieldOfViewPtr; }
   bool                  DefaultFieldOfView() const { return mDefaultFieldOfView; }

   //! Denote that the antenna's field of view is not default, allowing it to be communicated over
   //! the eventpipe.  This should only be used during initialization, and will have no effect if set
   // during simulation.
   void IndicateNondefaultFieldOfView() { mDefaultFieldOfView = false; }

   //! Is the given range within the range limits.
   //! @param aTgtRange [input] Range to target (meters).
   //! @returns 'true' if in range or 'false' if not.
   bool WithinRange(double aTgtRange) const { return ((aTgtRange >= mMinRange) && (aTgtRange <= mMaxRange)); }
   //@}

protected:
   void ConvertAnglesFromPCS_ToSSCS(double& aAz, double& aEl);
   void ConvertAnglesFromSSCS_ToPCS(double& aAz, double& aEl);
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void UpdateLocationLLA();
   void UpdateLocationWCS();
   void UpdateWCS_ToACS_Transform();
   void UpdateWCS_ToNED_Transform();
   void UpdateWCS_ToSSCS_Transform();

   WsfArticulatedPart* mArticulatedPartPtr;

   //! The offset of the antenna relative to the articulated part.
   double mOffset[3];

   //! The pitch (tilt) angle of the antenna relative to the articulated part.
   double mPitch;

   //! the cosine of the maximum electronic beam steering angle in azimuth.
   double mEBS_AzCosSteeringLimit;

   //! the cosine of the maximum electronic beam steering angle in elevation.
   double mEBS_ElCosSteeringLimit;

   //! The exponent 'n' of cos(x)^n for azimuth component beam losses.
   //! For the case when azimuth dropoff are not uniform.
   double mEBS_AzLossExponent;

   //! The exponent 'n' of cos(x)^n for elevation component beam losses.
   //! For the case when elevation dropoff are not uniform.
   double mEBS_ElLossExponent;

   //! The degrees-of-freedom for electronic beam steering.
   EBS_Mode mEBS_Mode;

   //! @name Scan limits with respect to the current cue.
   //@{
   //! The degrees-of-freedom for scanning.
   ScanMode mScanMode;
   //! The degrees-of-freedom for scan-stabilization
   ScanStabilization mScanStabilization;
   double            mMinAzScan; // radians
   double            mMaxAzScan; // radians
   double            mMinElScan; // radians
   double            mMaxElScan; // radians
   //@}

   //! @name Field of view with respect current cue.
   //! This should encompass the scan limits PLUS whatever the beam pattern might pick up
   //! when at the physical limits of the scan pattern.
   //@{
   ut::CloneablePtr<WsfFieldOfView> mFieldOfViewPtr;

   //! Indicates that the used field of view is unaltered from the default state (360 az x 180 el)
   bool mDefaultFieldOfView;

   double mMinRange; // meters
   double mMaxRange; // meters
   double mMinAlt;   // meters
   double mMaxAlt;   // meters
   //@}

   //! @name Locally computed location values.
   //! These values are invalidated when Update() is called and are recomputed when needed.
   //@{
   //! The WCS location of the antenna (includes the antenna_height contribution).
   double mLocationWCS[3];

   //! The WCS vector from the articulated part to the antenna.
   double mOffsetWCS[3];
   double mLat;
   double mLon;
   double mAlt;

   //! Used to determine if the uncued articulated part orientation has changed.
   double mPartYaw;
   double mPartPitch;
   double mPartRoll;

   //! The transformation from the ECS to the antenna coordinate system (ACS).
   double mECS_ToACS_Transform[3][3];
   //! The transformation from WCS to ACS.
   double mWCS_ToACS_Transform[3][3];
   //! The transformation from WCS to NED.
   double mWCS_ToNED_Transform[3][3];
   //! The transformation from WCS to the stabilized-scan coordinate system (SSCS).
   double mWCS_ToSSCS_Transform[3][3];

   //! 'true' if the antenna WCS location/offset values are valid.
   bool mLocationWCS_IsValid;
   //! 'true' if the antenna LLA values are valid.
   bool mLocationLLA_IsValid;

   bool mECS_ToACS_TransformIsValid;
   bool mWCS_ToACS_TransformIsValid;
   bool mWCS_ToNED_TransformIsValid;
   bool mWCS_ToSSCS_TransformIsValid;
   //@}
};

#endif
