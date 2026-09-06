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

#include "WsfEM_Antenna.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfFieldOfViewTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfRectangularFieldOfView.hpp"

// =================================================================================================
WsfEM_Antenna::WsfEM_Antenna()
   : WsfSinglePlatformObserver()
   , mArticulatedPartPtr(nullptr)
   , mPitch(0.0)
   , mEBS_AzCosSteeringLimit(0.0)
   , mEBS_ElCosSteeringLimit(0.0)
   , mEBS_AzLossExponent(1.0)
   , mEBS_ElLossExponent(1.0)
   , mEBS_Mode(cEBS_NONE)
   , mScanMode(cSCAN_FIXED)
   , mScanStabilization(cSS_NONE)
   , mMinAzScan(-UtMath::cPI)
   , mMaxAzScan(UtMath::cPI)
   , mMinElScan(-UtMath::cPI_OVER_2)
   , mMaxElScan(UtMath::cPI_OVER_2)
   , mFieldOfViewPtr(ut::make_unique<WsfRectangularFieldOfView>())
   , // The default FOV
   mDefaultFieldOfView(true)
   , mMinRange(0.0)
   , mMaxRange(std::numeric_limits<double>::max())
   , mMinAlt(-std::numeric_limits<double>::max())
   , mMaxAlt(std::numeric_limits<double>::max())
   , mLat(0.0)
   , mLon(0.0)
   , mAlt(0.0)
   , mPartYaw(-999.0)
   , mPartPitch(-999.0)
   , mPartRoll(-999.0)
   , mLocationWCS_IsValid(false)
   , mLocationLLA_IsValid(false)
   , mECS_ToACS_TransformIsValid(false)
   , mWCS_ToACS_TransformIsValid(false)
   , mWCS_ToNED_TransformIsValid(false)
   , mWCS_ToSSCS_TransformIsValid(false)
{
   UtVec3d::Set(mOffset, 0.0);
   UtVec3d::Set(mLocationWCS, 0.0);
   UtVec3d::Set(mOffsetWCS, 0.0);
   UtMat3d::Set(mECS_ToACS_Transform, 0.0);
   UtMat3d::Set(mWCS_ToACS_Transform, 0.0);
   UtMat3d::Set(mWCS_ToNED_Transform, 0.0);
   UtMat3d::Set(mWCS_ToSSCS_Transform, 0.0);
}

// =================================================================================================
WsfEM_Antenna::WsfEM_Antenna(const WsfEM_Antenna& aSrc)
   : WsfSinglePlatformObserver(aSrc)
   , mArticulatedPartPtr(nullptr)
   , mPitch(aSrc.mPitch)
   , mEBS_AzCosSteeringLimit(aSrc.mEBS_AzCosSteeringLimit)
   , mEBS_ElCosSteeringLimit(aSrc.mEBS_ElCosSteeringLimit)
   , mEBS_AzLossExponent(aSrc.mEBS_AzLossExponent)
   , mEBS_ElLossExponent(aSrc.mEBS_ElLossExponent)
   , mEBS_Mode(aSrc.mEBS_Mode)
   , mScanMode(aSrc.mScanMode)
   , mScanStabilization(aSrc.mScanStabilization)
   , mMinAzScan(aSrc.mMinAzScan)
   , mMaxAzScan(aSrc.mMaxAzScan)
   , mMinElScan(aSrc.mMinElScan)
   , mMaxElScan(aSrc.mMaxElScan)
   , mFieldOfViewPtr(aSrc.mFieldOfViewPtr)
   , mDefaultFieldOfView(aSrc.mDefaultFieldOfView)
   , mMinRange(aSrc.mMinRange)
   , mMaxRange(aSrc.mMaxRange)
   , mMinAlt(aSrc.mMinAlt)
   , mMaxAlt(aSrc.mMaxAlt)
   , mLat(0.0)
   , mLon(0.0)
   , mAlt(0.0)
   , mPartYaw(-999.0)
   , mPartPitch(-999.0)
   , mPartRoll(-999.0)
   , mLocationWCS_IsValid(false)
   , mLocationLLA_IsValid(false)
   , mECS_ToACS_TransformIsValid(false)
   , mWCS_ToACS_TransformIsValid(false)
   , mWCS_ToNED_TransformIsValid(false)
   , mWCS_ToSSCS_TransformIsValid(false)
{
   UtVec3d::Set(mOffset, aSrc.mOffset);
   UtVec3d::Set(mLocationWCS, 0.0);
   UtVec3d::Set(mOffsetWCS, 0.0);
   UtMat3d::Set(mECS_ToACS_Transform, 0.0);
   UtMat3d::Set(mWCS_ToACS_Transform, 0.0);
   UtMat3d::Set(mWCS_ToNED_Transform, 0.0);
   UtMat3d::Set(mWCS_ToSSCS_Transform, 0.0);
}

WsfEM_Antenna& WsfEM_Antenna::operator=(const WsfEM_Antenna& aRhs)
{
   if (this != &aRhs)
   {
      WsfSinglePlatformObserver::operator=(aRhs);
      mArticulatedPartPtr                = nullptr;
      mPitch                             = aRhs.mPitch;
      mEBS_AzCosSteeringLimit            = aRhs.mEBS_AzCosSteeringLimit;
      mEBS_ElCosSteeringLimit            = aRhs.mEBS_ElCosSteeringLimit;
      mEBS_AzLossExponent                = aRhs.mEBS_AzLossExponent;
      mEBS_ElLossExponent                = aRhs.mEBS_ElLossExponent;
      mEBS_Mode                          = aRhs.mEBS_Mode;
      mScanMode                          = aRhs.mScanMode;
      mScanStabilization                 = aRhs.mScanStabilization;
      mMinAzScan                         = aRhs.mMinAzScan;
      mMaxAzScan                         = aRhs.mMaxAzScan;
      mMinElScan                         = aRhs.mMinElScan;
      mMaxElScan                         = aRhs.mMaxElScan;
      mFieldOfViewPtr                    = aRhs.mFieldOfViewPtr;
      mDefaultFieldOfView                = aRhs.mDefaultFieldOfView;
      mMinRange                          = aRhs.mMinRange;
      mMaxRange                          = aRhs.mMaxRange;
      mMinAlt                            = aRhs.mMinAlt;
      mMaxAlt                            = aRhs.mMaxAlt;
      mLat                               = 0.0;
      mLon                               = 0.0;
      mAlt                               = 0.0;
      mPartYaw                           = -999.0;
      mPartPitch                         = -999.0;
      mPartRoll                          = -999.0;
      mLocationWCS_IsValid               = false;
      mLocationLLA_IsValid               = false;
      mECS_ToACS_TransformIsValid        = false;
      mWCS_ToACS_TransformIsValid        = false;
      mWCS_ToNED_TransformIsValid        = false;
      mWCS_ToSSCS_TransformIsValid       = false;

      UtVec3d::Set(mOffset, aRhs.mOffset);
      UtVec3d::Set(mLocationWCS, 0.0);
      UtVec3d::Set(mOffsetWCS, 0.0);
      UtMat3d::Set(mECS_ToACS_Transform, 0.0);
      UtMat3d::Set(mWCS_ToACS_Transform, 0.0);
      UtMat3d::Set(mWCS_ToNED_Transform, 0.0);
      UtMat3d::Set(mWCS_ToSSCS_Transform, 0.0);
   }
   return *this;
}

// =================================================================================================
// virtual
WsfEM_Antenna::~WsfEM_Antenna()
{
   // Tell the subject that we were observer that we're no longer observing!
   if ((mArticulatedPartPtr != nullptr) && (mArticulatedPartPtr->GetPlatform() != nullptr))
   {
      mArticulatedPartPtr->GetPlatform()->DetachObserver(this);
   }
}

// =================================================================================================
//! A convenience method to get the platform associated with the antenna.
WsfPlatform* WsfEM_Antenna::GetPlatform() const
{
   if (mArticulatedPartPtr == nullptr)
   {
      return nullptr;
   }
   return mArticulatedPartPtr->GetPlatform();
}

// =================================================================================================
//! Initialize the antenna.
//! @param aArticulatedPartPtr The articulated part to which the antenna is attached.
// virtual
bool WsfEM_Antenna::Initialize(WsfArticulatedPart* aArticulatedPartPtr)
{
   bool ok             = true;
   mArticulatedPartPtr = aArticulatedPartPtr;
   if ((mArticulatedPartPtr != nullptr) && (mArticulatedPartPtr->GetPlatform() != nullptr))
   {
      mArticulatedPartPtr->GetPlatform()->AttachObserver(this);
      ok &= mFieldOfViewPtr->Initialize(*mArticulatedPartPtr);
   }
   else
   {
      ut::log::error() << "An antenna must be attached to a properly defined articulated part.";
      ok = false;
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfEM_Antenna::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "antenna_height")
   {
      double antennaHeight;
      aInput.ReadValueOfType(antennaHeight, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(antennaHeight, 0.0);
      SetHeight(antennaHeight);
   }
   else if ((command == "antenna_pitch") || (command == "antenna_tilt"))
   {
      double antennaPitch;
      aInput.ReadValueOfType(antennaPitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(antennaPitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetPitch(antennaPitch);
   }
   else if (mFieldOfViewPtr->ProcessInput(aInput))
   {
      mDefaultFieldOfView = false;
   }
   else if (command == "azimuth_scan_limits")
   {
      double minAzScan;
      double maxAzScan;
      aInput.ReadValueOfType(minAzScan, UtInput::cANGLE);
      aInput.ReadValueOfType(maxAzScan, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(minAzScan, -UtMath::cPI);
      aInput.ValueLessOrEqual(maxAzScan, UtMath::cPI);
      aInput.ValueLessOrEqual(minAzScan, maxAzScan);
      SetAzimuthScanLimits(minAzScan, maxAzScan);
   }
   else if (command == "elevation_scan_limits")
   {
      double minElScan;
      double maxElScan;
      aInput.ReadValueOfType(minElScan, UtInput::cANGLE);
      aInput.ReadValueOfType(maxElScan, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(minElScan, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(maxElScan, UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(minElScan, maxElScan);
      SetElevationScanLimits(minElScan, maxElScan);
   }
   else if (command == "minimum_range")
   {
      double minRange;
      double maxRange;
      GetRangeLimits(minRange, maxRange);
      aInput.ReadValueOfType(minRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(minRange, 0.0);
      SetRangeLimits(minRange, maxRange);
   }
   else if (command == "maximum_range")
   {
      double minRange;
      double maxRange;
      GetRangeLimits(minRange, maxRange);
      aInput.ReadValueOfType(maxRange, UtInput::cLENGTH);
      aInput.ValueGreater(maxRange, 0.0);
      SetRangeLimits(minRange, maxRange);
   }
   else if (command == "minimum_altitude")
   {
      double minAlt;
      double maxAlt;
      GetAltitudeLimits(minAlt, maxAlt);
      aInput.ReadValueOfType(minAlt, UtInput::cLENGTH);
      SetAltitudeLimits(minAlt, maxAlt);
   }
   else if (command == "maximum_altitude")
   {
      double minAlt;
      double maxAlt;
      GetAltitudeLimits(minAlt, maxAlt);
      aInput.ReadValueOfType(maxAlt, UtInput::cLENGTH);
      SetAltitudeLimits(minAlt, maxAlt);
   }
   else if (command == "field_of_view")
   {
      // Read the type from the input stream and create it using a factory method.
      std::string type;
      aInput.ReadCommand(type);
      mFieldOfViewPtr = WsfFieldOfViewTypes::Create(type);
      UtInputBlock block(aInput, "end_field_of_view");
      while (block.ReadCommand(command))
      {
         mFieldOfViewPtr->ProcessInput(block.GetInput());
      }
      mDefaultFieldOfView = false;
   }
   else if (command == "scan_mode")
   {
      std::string scanModeStr;
      ScanMode    scanMode;
      aInput.ReadValue(scanModeStr);
      if (scanModeStr == "fixed")
      {
         scanMode = cSCAN_FIXED;
      }
      else if (scanModeStr == "azimuth")
      {
         scanMode = cSCAN_AZ;
      }
      else if (scanModeStr == "elevation")
      {
         scanMode = cSCAN_EL;
      }
      else if ((scanModeStr == "both") || (scanModeStr == "azimuth_and_elevation"))
      {
         scanMode = cSCAN_AZ_EL;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
      SetScanMode(scanMode);
   }
   else if (command == "scan_stabilization")
   {
      std::string axesStr;
      aInput.ReadValue(axesStr);
      if (axesStr == "none")
      {
         SetScanStabilization(cSS_NONE);
      }
      else if (axesStr == "pitch")
      {
         SetScanStabilization(cSS_PITCH);
      }
      else if (axesStr == "roll")
      {
         SetScanStabilization(cSS_ROLL);
      }
      else if (axesStr == "pitch_and_roll")
      {
         SetScanStabilization(cSS_BOTH);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "electronic_beam_steering")
   {
      std::string ebsMode;
      aInput.ReadValue(ebsMode);
      if (ebsMode == "azimuth")
      {
         SetEBS_Mode(cEBS_AZIMUTH);
      }
      else if (ebsMode == "elevation")
      {
         SetEBS_Mode(cEBS_ELEVATION);
      }
      else if ((ebsMode == "both") || (ebsMode == "azimuth_and_elevation"))
      {
         SetEBS_Mode(cEBS_BOTH);
      }
      else if (ebsMode == "none")
      {
         SetEBS_Mode(cEBS_NONE);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid value for electronic_beam_steering: " + ebsMode);
      }
   }
   else if (command == "electronic_beam_steering_limit")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_AzCosSteeringLimit = cos(steeringLimit);
      mEBS_ElCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_limit_azimuth")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_AzCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_limit_elevation")
   {
      double steeringLimit;
      aInput.ReadValueOfType(steeringLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(steeringLimit, 0.0, UtMath::cPI_OVER_2);
      mEBS_ElCosSteeringLimit = cos(steeringLimit);
   }
   else if (command == "electronic_beam_steering_loss_exponent")
   {
      double ebsLossExponent;
      aInput.ReadValue(ebsLossExponent);
      SetEBS_AzLossExponent(ebsLossExponent);
      SetEBS_ElLossExponent(ebsLossExponent);
   }
   else if (command == "electronic_beam_steering_loss_exponent_azimuth")
   {
      double ebsLossExponentAz;
      aInput.ReadValue(ebsLossExponentAz);
      SetEBS_AzLossExponent(ebsLossExponentAz);
   }
   else if (command == "electronic_beam_steering_loss_exponent_elevation")
   {
      double ebsLossExponentEl;
      aInput.ReadValue(ebsLossExponentEl);
      SetEBS_ElLossExponent(ebsLossExponentEl);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Update the state of the antenna to represent its position at the indicated time.
//! @param aSimTime The current simulation time.
// virtual
void WsfEM_Antenna::UpdatePosition(double aSimTime)
{
   assert(mArticulatedPartPtr != nullptr);
   // Ensure the position of the platform part is current. If the platform location is not current then
   // WsfPlatform will invoke our OnPlatformUpdated method via the WsfSinglePlatformObserver interface.
   mArticulatedPartPtr->UpdatePosition(aSimTime);
}

// =================================================================================================
//! Set the height of the antenna with respect to the host articulated part.
//! @param aHeight Offset of the antenna with respect to the articulated part to which it is attached.
void WsfEM_Antenna::SetHeight(double aHeight)
{
   mOffset[2] = -aHeight;
}

// =================================================================================================
//! Set the antenna pitch (tilt) angle with respect to the host articulated part.
//! @param aPitch the antenna pitch angle with respect to the host articulated part (radians).
void WsfEM_Antenna::SetPitch(double aPitch)
{
   mPitch                      = aPitch;
   mWCS_ToACS_TransformIsValid = false;
   mECS_ToACS_TransformIsValid = false;
}

// =================================================================================================
//! Set the field of view on the antenna.
//! @param aFieldOfViewPtr The new field of view object to be used by the antenna.
void WsfEM_Antenna::SetFieldOfView(std::unique_ptr<WsfFieldOfView> aFieldOfViewPtr)
{
   mFieldOfViewPtr = std::move(aFieldOfViewPtr);
   mFieldOfViewPtr->Initialize(*GetArticulatedPart());
}

// =================================================================================================
//! Set the azimuth field of view.
void WsfEM_Antenna::SetAzimuthFieldOfView(double aMinAzFOV, double aMaxAzFOV)
{
   mFieldOfViewPtr->SetAzimuthFieldOfView(aMinAzFOV, aMaxAzFOV);
}

// =================================================================================================
//! Return the azimuth field of view.
void WsfEM_Antenna::GetAzimuthFieldOfView(double& aMinAzFOV, double& aMaxAzFOV) const
{
   mFieldOfViewPtr->GetAzimuthFieldOfView(aMinAzFOV, aMaxAzFOV);
}

// =================================================================================================
//! Set the elevation field of view.
void WsfEM_Antenna::SetElevationFieldOfView(double aMinElFOV, double aMaxElFOV)
{
   mFieldOfViewPtr->SetElevationFieldOfView(aMinElFOV, aMaxElFOV);
}

// =================================================================================================
//! Return the elevation field of view.
void WsfEM_Antenna::GetElevationFieldOfView(double& aMinElFOV, double& aMaxElFOV) const
{
   mFieldOfViewPtr->GetElevationFieldOfView(aMinElFOV, aMaxElFOV);
}

//! Set the range limits
//! @param aMinRange [input] The minimum range.
//! @param aMaxRange [input] The maximum range.
void WsfEM_Antenna::SetRangeLimits(double aMinRange, double aMaxRange)
{
   mMinRange = aMinRange;
   mMaxRange = aMaxRange;
}

// =================================================================================================
//! Set the altitude limits
//! @param aMinAlt [input] The minimum altitude.
//! @param aMaxAlt [input] The maximum altitude.
void WsfEM_Antenna::SetAltitudeLimits(double aMinAlt, double aMaxAlt)
{
   mMinAlt = aMinAlt;
   mMaxAlt = aMaxAlt;
}

// =================================================================================================
//! Set the azimuth scan limits.
void WsfEM_Antenna::SetAzimuthScanLimits(double aMinAzScan, double aMaxAzScan)
{
   mMinAzScan = aMinAzScan;
   mMaxAzScan = aMaxAzScan;
}

// =================================================================================================
//! Set the elevation scan limits.
void WsfEM_Antenna::SetElevationScanLimits(double aMinElScan, double aMaxElScan)
{
   mMinElScan = aMinElScan;
   mMaxElScan = aMaxElScan;
}

// =================================================================================================
//! Given the WCS vector of a target with respect to the antenna, return the azimuth and elevation angles.
//! @param aThisToTgtLocWCS [input] The WCS vector from the antenna to the target.
//! @param aThisToTgtAz     [output] The azimuth of the target with respect to the antenna.
//! @param aThisToTgtEl     [output] The elevation of the target with respect to the antenna.
void WsfEM_Antenna::ComputeAspect(const double aThisToTgtLocWCS[3], double& aThisToTgtAz, double& aThisToTgtEl)
{
   assert(mArticulatedPartPtr != nullptr);

   // NOTE: The antenna coordinate system is just a translation of the articulated part coordinate system.
   // (antenna_tilt is only used for resolving the position of the target within the beam, so there is no
   // rotation here...). Since the incoming vector is a RELATIVE WCS vector and there is no rotation, we
   // can simply call WsfArticulatedPart::ComputeAspect to get the angles.

   mArticulatedPartPtr->ComputeAspect(aThisToTgtLocWCS, aThisToTgtAz, aThisToTgtEl);
}

// =================================================================================================
//! Compute the aspect of target with respect to a beam.
//!
//! @param aWCS_ToBeamTransform [input]  The transformation matrix for converting a WCS vector to the
//!                                      beam coordinate system.
//! @param aThisToTgtWCS        [input]  The relative WCS position of the target with respect to this.
//! @param aBeamToTgtAz         [output] Azimuth of the target with respect to the beam (radians).
//! @param aBeamToTgtEl         [output] Elevation of the target with respect to the beam (radians).

void WsfEM_Antenna::ComputeBeamAspect(const double aWCS_ToBeamTransform[3][3],
                                      const double aThisToTgtWCS[3],
                                      double&      aBeamToTgtAz,
                                      double&      aBeamToTgtEl) const
{
   // The rows of the WCS->Beam transformation matrix are the unit vectors of the beam
   // coordinate system (BCS) in the WCS
   //
   // Determine the components of the relative position vector in the BCS
   // by projecting the relative WCS vector onto the unit vectors of the BCS.

   double beamX = UtVec3d::DotProduct(aWCS_ToBeamTransform[0], aThisToTgtWCS);
   double beamY = UtVec3d::DotProduct(aWCS_ToBeamTransform[1], aThisToTgtWCS);
   double beamZ = UtVec3d::DotProduct(aWCS_ToBeamTransform[2], aThisToTgtWCS);

   // Compute the azimuth and elevation in the beam coordinate system.

   aBeamToTgtAz  = atan2(beamY, beamX);
   double beamXY = sqrt((beamX * beamX) + (beamY * beamY));
   if (beamXY != 0.0)
   {
      aBeamToTgtEl = -atan2(beamZ, beamXY);
   }
   else
   {
      aBeamToTgtEl = UtMath::cPI_OVER_2;
      if (beamZ > 0.0)
      {
         aBeamToTgtEl = -UtMath::cPI_OVER_2;
      }
   }
}

// =================================================================================================
//! Compute the beam position given a target point.
//!
//! This routine computes the WCS-to-Beam transformation matrix that represents the
//! instantaneous position of the beam.
//!
//! @param aXmtrRcvrPtr [input] The transmitter or receiver containing the beam.
//! @param aThisToTgtAz [input] The azimuth   of the target with respect to the CUED position of the articulated part.
//! @param aThisToTgtEl [input] The elevation of the target with respect to the CUED position of the articulated part.
//! @param aWCS_ToBeamTransform [output] The transformation matrix that will take a WCS coordinate and create beam
//! coordinate system (BCS) coordinate.
//! @param aEBS_Az      [output] The amount of electronic steering in azimuth (zero for non-electronically scanned systems).
//! @param aEBS_El      [output] The amount of electronic steering in elevation (zero for non-electronically scanned systems).
void WsfEM_Antenna::ComputeBeamPosition(WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                                        double          aThisToTgtAz,
                                        double          aThisToTgtEl,
                                        double          aWCS_ToBeamTransform[3][3],
                                        double&         aEBS_Az,
                                        double&         aEBS_El)
{
   assert(mArticulatedPartPtr != nullptr);

   double beamAz   = 0.0;
   double beamEl   = 0.0;
   double beamRoll = 0.0;

   // If the sensor only scans in azimuth and if the antenna does not use electronic
   // steering in elevation, then apply the tilt angle. It's not applied when using electronic steering in
   // elevation because it is accounted for in the electronic steering computations at the end of the routine.
   if ((mScanMode == cSCAN_AZ) && ((mEBS_Mode == cEBS_NONE) || (mEBS_Mode == cEBS_AZIMUTH)))
   {
      beamEl = mPitch + aXmtrRcvrPtr->GetBeamTilt();
   }

   // Convert the aspect angles to the stabilized frame if requested.
   double thisToTgtAz = aThisToTgtAz; // Assume unstabilized scan
   double thisToTgtEl = aThisToTgtEl;
   if (mScanStabilization != cSS_NONE)
   {
      ConvertAnglesFromPCS_ToSSCS(thisToTgtAz, thisToTgtEl);
   }

   // If the beam can scan in azimuth then determine the azimuth to be used.

   bool checkAz = false;
   if (mScanMode & cSCAN_AZ)
   {
      checkAz = true;

      // Make sure the azimuth is within the scan limits.

      if ((thisToTgtAz < mMinAzScan) || (thisToTgtAz > mMaxAzScan))
      {
         // Beam is outside scan limits. Set position to the nearest limit.
         double deltaMin = UtMath::NormalizeAngle0_TwoPi(mMinAzScan - thisToTgtAz);
         double deltaMax = UtMath::NormalizeAngle0_TwoPi(thisToTgtAz - mMaxAzScan);
         if (deltaMin <= deltaMax)
         {
            beamAz = mMinAzScan; // Set position to the min limit
         }
         else
         {
            beamAz = mMaxAzScan; // Set position to the max limit
         }
      }
      else
      {
         beamAz = thisToTgtAz; // Point right at target (in azimuth)
      }
   }

   // If the beam can scan in elevation then determine the elevation to be used.

   bool checkEl = false;
   if (mScanMode & cSCAN_EL)
   {
      checkEl = true;

      // Make sure the elevation is within the scan limits
      if (thisToTgtEl < mMinElScan)
      {
         beamEl = mMinElScan; // Set position to the min limit
      }
      else if (thisToTgtEl > mMaxElScan)
      {
         beamEl = mMaxElScan; // Set position to the max limit
      }
      else
      {
         beamEl = thisToTgtEl; // Point right at the target (in elevation)
      }
   }

   // If scan stabilization was requested, convert the angles from the stabilized frame
   // back to the unstablized frame.

   if (mScanStabilization != cSS_NONE)
   {
      ConvertAnglesFromSSCS_ToPCS(beamAz, beamEl);
   }

   // Check the beam position against the slew limits.
   //
   // Note that this is done even if the define did not have a 'slew_mode' for a given direction.
   // The 'slew_mode' indicates in which directions the device can be 'cued'. The limits represent
   // the absolute steering limits for both cueing and scanning. The default for the slew limits are
   // unlimited, so they should not interfere with the scanning UNLESS the user has choosen to do so.

   if (checkAz || checkEl)
   {
      double cuedAz;
      double cuedEl;
      // (JAJ) Should this be GetCurrentCuedOrientation???
      // (JAJ) Since this is for pseudo-scanning we shouldn't have to worry.
      mArticulatedPartPtr->GetActualCuedOrientation(cuedAz, cuedEl);

      if (checkAz)
      {
         double finalAz = cuedAz + beamAz;
         // Normalize back to [-180, 180] to prevent clipping if scan volume spans the +/- 180 deg line in PCS.
         finalAz          = UtMath::NormalizeAngleMinusPi_Pi(finalAz);
         double minAzSlew = mArticulatedPartPtr->GetMinAzSlew();
         double maxAzSlew = mArticulatedPartPtr->GetMaxAzSlew();
         if ((finalAz < minAzSlew) || (finalAz > maxAzSlew))
         {
            // Beam is outside slew limits. Set position to the nearest limit.
            double deltaMin = UtMath::NormalizeAngle0_TwoPi(minAzSlew - finalAz);
            double deltaMax = UtMath::NormalizeAngle0_TwoPi(finalAz - maxAzSlew);
            if (deltaMin <= deltaMax)
            {
               beamAz = minAzSlew - cuedAz; // Set position to the min limit
            }
            else
            {
               beamAz = maxAzSlew - cuedAz; // Set position to the max limit
            }
         }
      }
      if (checkEl)
      {
         double finalEl   = cuedEl + beamEl;
         double minElSlew = mArticulatedPartPtr->GetMinElSlew();
         double maxElSlew = mArticulatedPartPtr->GetMaxElSlew();
         if (finalEl < minElSlew)
         {
            beamEl = minElSlew - cuedEl; // Set position to the min limit
         }
         else if (finalEl > maxElSlew)
         {
            beamEl = maxElSlew - cuedEl; // Set position to the min limit
         }
      }
   }

   mArticulatedPartPtr->ComputeRotationalTransform(beamAz, beamEl, beamRoll, aWCS_ToBeamTransform);

   // If electronic steering is used, compute the aspect of the beam position with respect to the antenna.
   aEBS_Az = 0.0;
   aEBS_El = 0.0;
   if (mEBS_Mode != cEBS_NONE)
   {
      if (!mWCS_ToACS_TransformIsValid)
      {
         UpdateWCS_ToACS_Transform();
      }
      // Transform the X axis of the WCS->BCS transform (the first row) into the ACS as this will represent
      // the pointing vector of the beam in the ACS.
      double beamPointing[3];
      UtMat3d::Transform(beamPointing, mWCS_ToACS_Transform, aWCS_ToBeamTransform[0]);

      // Compute the aspect of the beam pointing vector.
      double EBS_Az(0.0);
      double EBS_El(0.0);
      UtEntity::ComputeAzimuthAndElevation(beamPointing, EBS_Az, EBS_El);
      if (mEBS_Mode & cEBS_AZIMUTH)
      {
         aEBS_Az = EBS_Az;
      }
      if (mEBS_Mode & cEBS_ELEVATION)
      {
         aEBS_El = EBS_El;
      }
   }
}

// =================================================================================================
//! Compute the gain multiplier to account for losses due to electronic beam steering.
//!
//! @param aEBS_Az Beam azimuth   with respect to the antenna (in radians).
//! @param aEBS_El Beam elevation with respect to the antenna (in radians).
//! @return The gain multiplier (0, 1] to account for electronic beam steering losses.
//! A value of 1 indicates no losses.
double WsfEM_Antenna::ComputeBeamSteeringLoss(double aEBS_Az, double aEBS_El)
{
   double gainFactor = 1.0;
   if (mEBS_Mode != cEBS_NONE)
   {
      // Compute the solid angle between using the definition of the dot product.
      //
      // The BCS    pointing vector is: [1, 0, 0].
      // The target pointing vector is: [cos(az)*cos(el), sin(az)*cos(el), sin(el)].
      // So the dot product reduces to: cos(az)*cos(el).
      //
      // If fabs(cosTheta) > 0.8 then then it used the definition of the cross product
      // to get the sine of the angle. I extracted this code and did a comparison involving all octants and
      // I couldn't find a case where it was needed.
      //
      // Also, some models clamp the steering angle to a maximum of 89 degrees. We don't want to see behind
      // us and we don't want the loss factor to go to zero because of potential math problems.

      // In the front hemisphere
      double cosAz = cos(aEBS_Az);
      double cosEl = cos(aEBS_El);
      if (cosAz < 0.0) // In the rear hemisphere
      {
         double ebsAz = UtMath::NormalizeAngleMinusPi_Pi(aEBS_Az);
         ebsAz        = fabs(ebsAz);

         cosAz = cos(UtMath::cPI - ebsAz);

         double theta    = acos(cosAz * cosEl);
         double cosTheta = cos(UtMath::cPI - theta);
         if (cosEl != 0.0)
         {
            cosAz = cosTheta / cosEl;
         }
      }

      // Angles greater than 89.9 are treated as pointing back into the face.
      if (((cosAz * cosEl) > 0.001745328366) && // cos(89.9)
          (cosAz >= mEBS_AzCosSteeringLimit) && (cosEl >= mEBS_ElCosSteeringLimit))
      {
         gainFactor = cosAz * cosEl;
         // Apply the loss exponent if one was specified.
         if ((mEBS_AzLossExponent != 1.0) || (mEBS_ElLossExponent != 1.0))
         {
            gainFactor = pow(cosAz, mEBS_AzLossExponent) * pow(cosEl, mEBS_ElLossExponent);
         }
      }
      else
      {
         // Outside the steering limit. Return a very small gain factor.
         gainFactor = 1.0E-10;
      }
   }
   return gainFactor;
}

// =================================================================================================
//! Given a WCS location, return the position in antenna coordinates.
void WsfEM_Antenna::ConvertWCS_ToACS(const double aLocationWCS[3], double aLocationACS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToACS_TransformIsValid)
   {
      UpdateWCS_ToACS_Transform();
   }
   double relLocationWCS[3];
   GetRelativeLocationWCS(aLocationWCS, relLocationWCS);
   // Rotate the vector in the WCS frame to the ACS frame.
   UtMat3d::Transform(aLocationACS, mWCS_ToACS_Transform, relLocationWCS);
}

// =================================================================================================
//! Given a WCS location, return the NED location relative to the antenna.
void WsfEM_Antenna::ConvertWCS_ToNED(const double aLocationWCS[3], double aLocationNED[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }
   UtEllipsoidalEarth::ConvertECEFToLocal(mLocationWCS, mWCS_ToNED_Transform, aLocationWCS, aLocationNED);
}

// =================================================================================================
//! Given an NED location relative to the antenna, return the WCS location.
void WsfEM_Antenna::ConvertNED_ToWCS(const double aLocationNED[3], double aLocationWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }
   UtEllipsoidalEarth::ConvertLocalToECEF(mLocationWCS, mWCS_ToNED_Transform, aLocationNED, aLocationWCS);
}

// =================================================================================================
//! Convert a vector in the WCS frame to the antenna frame.
void WsfEM_Antenna::ConvertWCS_VectorToACS(const double aVectorWCS[3], double aVectorACS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToACS_TransformIsValid)
   {
      UpdateWCS_ToACS_Transform();
   }
   // Rotate the vector in the WCS frame to the ACS frame.
   UtMat3d::Transform(aVectorACS, mWCS_ToACS_Transform, aVectorWCS);
}

// =================================================================================================
//! Convert a vector in the WCS frame to the antenna NED frame.
void WsfEM_Antenna::ConvertWCS_VectorToNED(const double aVectorWCS[3], double aVectorNED[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }
   // Rotate the vector in the WCS frame to the NED frame.
   UtMat3d::Transform(aVectorNED, mWCS_ToNED_Transform, aVectorWCS);
}

// =================================================================================================
//! Convert a vector in the antenna NED frame to the WCS frame.
void WsfEM_Antenna::ConvertNED_VectorToWCS(const double aVectorNED[3], double aVectorWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }
   // Rotate the vector in the NED frame to the WCS frame.
   UtMat3d::InverseTransform(aVectorWCS, mWCS_ToNED_Transform, aVectorNED);
}

// =================================================================================================
//! Get the absolute latitude/longitude/altitude of the antenna.
void WsfEM_Antenna::GetLocationLLA(double& aLat, double& aLon, double& aAlt)
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached LLA location of the antenna is current.
   if (!mLocationLLA_IsValid)
   {
      UpdateLocationLLA();
   }
   aLat = mLat;
   aLon = mLon;
   aAlt = mAlt;
}

// =================================================================================================
//! Get the absolute WCS location of the antenna.
//! @param aLocationWCS The WCS location of the antenna.
void WsfEM_Antenna::GetLocationWCS(double aLocationWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached WCS location of the antenna is current.
   if (!mLocationWCS_IsValid)
   {
      UpdateLocationWCS();
   }
   UtVec3d::Set(aLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Get the WCS location of a target given its WCS vector relative to the antenna.
//! @param aThisToTgtLocWCS [input] The WCS vector of the target relative to the antenna.
//! @param aTgtLocWCS       [output] The WCS location of the target.
void WsfEM_Antenna::GetLocationWCS(const double aThisToTgtLocWCS[3], double aTgtLocWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached WCS location of the antenna is current.
   if (!mLocationWCS_IsValid)
   {
      UpdateLocationWCS();
   }

   // Compute the WCS location of the target.
   UtVec3d::Add(aTgtLocWCS, mLocationWCS, aThisToTgtLocWCS);
}

// =================================================================================================
//! Return the NED position vector of the target relative to this antenna.
void WsfEM_Antenna::GetRelativeLocationNED(WsfPlatform* aTargetPtr, double aLocationNED[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }

   double positionWCS[3];
   GetRelativeLocationWCS(aTargetPtr, positionWCS);
   UtMat3d::Transform(aLocationNED, mWCS_ToNED_Transform, positionWCS);
}

// =================================================================================================
//! Return the NED position vector of the target relative to this antenna.
void WsfEM_Antenna::GetRelativeLocationNED(const double aTgtLocWCS[3], double aLocationNED[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCS_ToNED_TransformIsValid)
   {
      UpdateWCS_ToNED_Transform();
   }

   double positionWCS[3];
   GetRelativeLocationWCS(aTgtLocWCS, positionWCS);
   UtMat3d::Transform(aLocationNED, mWCS_ToNED_Transform, positionWCS);
}

// =================================================================================================
void WsfEM_Antenna::GetRelativeLocationWCS(double aThisToTgtAz,
                                           double aThisToTgtEl,
                                           double aThisToTgtRange,
                                           double aThisToTgtLocWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // NOTE: The antenna coordinate system is just a translation of the articulated part coordinate system.
   // (antenna_tilt is only used for resolving the position of the target within the beam, so there is no
   // rotation here...). Since we're only concerned with angles and RELATIVE vectors, we can simply call
   // WsfArticulatedPart::GetRelativeLocationWCS().

   mArticulatedPartPtr->GetRelativeLocationWCS(aThisToTgtAz, aThisToTgtEl, aThisToTgtRange, aThisToTgtLocWCS);
}

// =================================================================================================
void WsfEM_Antenna::GetRelativeLocationWCS(const double aTgtLocWCS[3], double aThisToTgtLocWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached WCS location of the antenna is current.
   if (!mLocationWCS_IsValid)
   {
      UpdateLocationWCS();
   }

   // Compute the WCS vector from the antenna to the target.
   UtVec3d::Subtract(aThisToTgtLocWCS, aTgtLocWCS, mLocationWCS);
}

// =================================================================================================
//! Get the relative WCS vector from this to the indicated target.
//! @param aTargetPtr        [input] Pointer to the target.
//! @param aThisToTgtLocWCS [output] The relative WCS vector from this to the target.
void WsfEM_Antenna::GetRelativeLocationWCS(WsfPlatform* aTargetPtr, double aThisToTgtLocWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached WCS location of the antenna is current.
   if (!mLocationWCS_IsValid)
   {
      UpdateLocationWCS();
   }

   // Get the WCS location of the target.
   double tgtLocWCS[3];
   aTargetPtr->GetLocationWCS(tgtLocWCS);

   // Compute the WCS vector from the antenna to the target.
   UtVec3d::Subtract(aThisToTgtLocWCS, tgtLocWCS, mLocationWCS);
}

// =================================================================================================
//! Is the target altitude with the relative altitude limits?
//! @param aTgtAlt [input] Altitude of the target (meters).
//! @returns 'true' if the target is within the altitude limits or 'false' if not.
bool WsfEM_Antenna::WithinAltitude(double aTgtAlt)
{
   assert(mArticulatedPartPtr != nullptr);

   // Ensure the locally cached LLA location of the antenna is current.
   if (!mLocationLLA_IsValid)
   {
      UpdateLocationLLA();
   }
   double relTgtAlt = aTgtAlt - mAlt;
   return ((relTgtAlt >= mMinAlt) && (relTgtAlt <= mMaxAlt));
}

// =================================================================================================
//! Is the target within the field-of-view?
//!
//! @param aThisToTgtAz [input] Azimuth of the target with respect to the device (radians).
//! @param aThisToTgtEl [input] Elevation of the target with respect to the device (radians).
//! @returns 'true' if within the field of view or 'false' if not.
bool WsfEM_Antenna::WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
{
   assert(mArticulatedPartPtr != nullptr);

   // Convert the aspect angles to the stabilized frame if requested.
   double thisToTgtAz = aThisToTgtAz; // Assume unstabilized scan
   double thisToTgtEl = aThisToTgtEl;
   if (mScanStabilization != cSS_NONE)
   {
      ConvertAnglesFromPCS_ToSSCS(thisToTgtAz, thisToTgtEl);
   }

   return mFieldOfViewPtr->WithinFieldOfView(thisToTgtAz, thisToTgtEl);
}

// =================================================================================================
//! Compute azimuth and elevation of the target and determine if it is within the field of view.
//!
//! This method computes the azimuth and elevation angles for a target.  The 'true' values
//! are computed using the actual geometric coordinates.  The 'apparent' values are those
//! computed using the 'Earth Radius Multiplier', which simulates the effects of atmospheric
//! refraction.
//!
//! @param aTargetPtr               [input] Pointer to the target.
//! @param aEarthRadiusMultiplier   [input] A scale factor to be applied to the Earth's radius to
//! simulate the effect of atmospheric refraction.
//! @param aTrueThisToTgtAz         [output] The true (geometric) azimuth of the target with respect
//! to the antenna coordinate system (radians).
//! @param aTrueThisToTgtEl         [output] The true (geometric) elevation of the target with respect
//! to the antenna coordinate system (radians).
//! @param aApparentThisToTgtLocWCS [output] The WCS vector from the antenna to the apparent
//! position of the target (meters).
//! @param aApparentThisToTgtAz     [output] The azimuth of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentThisToTgtEl     [output] The elevation of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentTgtToThisLocWCS [output] The WCS vector from the target to the apparent
//! position of the antenna (meters).
//!
//! @returns 'true' if within the field of view or 'false' if not.
//!
//! @note Sorry this has so many arguments, but various routine need these values and they are
//! too expensive to recompute.
bool WsfEM_Antenna::WithinFieldOfView(WsfPlatform* aTargetPtr,
                                      double       aEarthRadiusMultiplier,
                                      double&      aTrueThisToTgtAz,
                                      double&      aTrueThisToTgtEl,
                                      double       aApparentThisToTgtLocWCS[3],
                                      double&      aApparentThisToTgtAz,
                                      double&      aApparentThisToTgtEl,
                                      double       aApparentTgtToThisLocWCS[3])
{
   // Get the target location
   double tgtLocWCS[3] = {0.0};
   aTargetPtr->GetLocationWCS(tgtLocWCS);

   return mFieldOfViewPtr->WithinFieldOfView(this,
                                             tgtLocWCS,
                                             aEarthRadiusMultiplier,
                                             aTrueThisToTgtAz,
                                             aTrueThisToTgtEl,
                                             aApparentThisToTgtLocWCS,
                                             aApparentThisToTgtAz,
                                             aApparentThisToTgtEl,
                                             aApparentTgtToThisLocWCS);
}

// =================================================================================================
//! Compute azimuth and elevation of the target and determine if it is within the field of view.
//!
//! This method computes the azimuth and elevation angles for a target.  The 'true' values
//! are computed using the actual geometric coordinates.  The 'apparent' values are those
//! computed using the 'Earth Radius Multiplier', which simulates the effects of atmospheric
//! refraction.
//!
//! @param aArticulatedPartPtr      [input] Pointer to the articulated part.
//! @param aEarthRadiusMultiplier   [input] A scale factor to be applied to the Earth's radius to
//! simulate the effect of atmospheric refraction.
//! @param aTrueThisToTgtAz         [output] The true (geometric) azimuth of the target with respect
//! to the antenna coordinate system (radians).
//! @param aTrueThisToTgtEl         [output] The true (geometric) elevation of the target with respect
//! to the antenna coordinate system (radians).
//! @param aApparentThisToTgtLocWCS [output] The WCS vector from the antenna to the apparent
//! position of the target (meters).
//! @param aApparentThisToTgtAz     [output] The azimuth of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentThisToTgtEl     [output] The elevation of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentTgtToThisLocWCS [output] The WCS vector from the target to the apparent
//! position of the antenna (meters).
//!
//! @returns 'true' if within the field of view or 'false' if not.
//!
//! @note Sorry this has so many arguments, but various routine need these values and they are
//! too expensive to recompute.
bool WsfEM_Antenna::WithinFieldOfView(WsfArticulatedPart* aArticulatedPartPtr,
                                      double              aEarthRadiusMultiplier,
                                      double&             aTrueThisToTgtAz,
                                      double&             aTrueThisToTgtEl,
                                      double              aApparentThisToTgtLocWCS[3],
                                      double&             aApparentThisToTgtAz,
                                      double&             aApparentThisToTgtEl,
                                      double              aApparentTgtToThisLocWCS[3])
{
   // Get the articulated part location
   double articulatedPartLocWCS[3] = {0.0};
   aArticulatedPartPtr->GetLocationWCS(articulatedPartLocWCS);

   return mFieldOfViewPtr->WithinFieldOfView(this,
                                             articulatedPartLocWCS,
                                             aEarthRadiusMultiplier,
                                             aTrueThisToTgtAz,
                                             aTrueThisToTgtEl,
                                             aApparentThisToTgtLocWCS,
                                             aApparentThisToTgtAz,
                                             aApparentThisToTgtEl,
                                             aApparentTgtToThisLocWCS);
}

// =================================================================================================
//! Compute azimuth and elevation of the target and determine if it is within the field of view.
//!
//! This method computes the azimuth and elevation angles for a target.  The 'true' values
//! are computed using the actual geometric coordinates.  The 'apparent' values are those
//! computed using the 'Earth Radius Multiplier', which simulates the effects of atmospheric
//! refraction.
//!
//! @param aTgtLocWCS               [input] A target WCS position.
//! @param aEarthRadiusMultiplier   [input] A scale factor to be applied to the Earth's radius to
//! simulate the effect of atmospheric refraction.
//! @param aTrueThisToTgtAz         [output] The true (geometric) azimuth of the target with respect
//! to the antenna coordinate system (radians).
//! @param aTrueThisToTgtEl         [output] The true (geometric) elevation of the target with respect
//! to the antenna coordinate system (radians).
//! @param aApparentThisToTgtLocWCS [output] The WCS vector from the antenna to the apparent
//! position of the target (meters).
//! @param aApparentThisToTgtAz     [output] The azimuth of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentThisToTgtEl     [output] The elevation of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentTgtToThisLocWCS [output] The WCS vector from the target to the apparent
//! position of the antenna (meters).
//!
//! @returns 'true' if within the field of view or 'false' if not.
//!
//! @note Sorry this has so many arguments, but various routine need these values and they are
//! too expensive to recompute.
bool WsfEM_Antenna::WithinFieldOfView(double  aTgtLocWCS[3],
                                      double  aEarthRadiusMultiplier,
                                      double& aTrueThisToTgtAz,
                                      double& aTrueThisToTgtEl,
                                      double  aApparentThisToTgtLocWCS[3],
                                      double& aApparentThisToTgtAz,
                                      double& aApparentThisToTgtEl,
                                      double  aApparentTgtToThisLocWCS[3])
{
   assert(mArticulatedPartPtr != nullptr);

   return mFieldOfViewPtr->WithinFieldOfView(this,
                                             aTgtLocWCS,
                                             aEarthRadiusMultiplier,
                                             aTrueThisToTgtAz,
                                             aTrueThisToTgtEl,
                                             aApparentThisToTgtLocWCS,
                                             aApparentThisToTgtAz,
                                             aApparentThisToTgtEl,
                                             aApparentTgtToThisLocWCS);
}

// =================================================================================================
//! This routine is invoked by the WsfPlatform::Update via the WsfSinglePlatformObserver interface.
//! It invalidates coordinate transformations so location and orientation values are recomputed
//! when next requested.
// protected
// virtual
void WsfEM_Antenna::OnPlatformUpdated(double /* aSimTime */, WsfPlatform* /*aPlatformPtr*/)
{
   mLocationWCS_IsValid         = false;
   mLocationLLA_IsValid         = false;
   mWCS_ToACS_TransformIsValid  = false;
   mWCS_ToNED_TransformIsValid  = false;
   mWCS_ToSSCS_TransformIsValid = false;
}

// =================================================================================================
//! Convert the part-relative az/el to a stabilized scan az/el.
//! @note The angles are converted 'in-place' (the output replaces the input).
// protected
void WsfEM_Antenna::ConvertAnglesFromPCS_ToSSCS(double& aAz, double& aEl)
{
   // Ensure the WCS->SSCS transform is valid
   if (!mWCS_ToSSCS_TransformIsValid)
   {
      UpdateWCS_ToSSCS_Transform();
   }
   double relLocWCS[3];
   mArticulatedPartPtr->GetRelativeLocationWCS(aAz, aEl, 1.0, relLocWCS);

   double relLocSSCS[3];
   UtMat3d::Transform(relLocSSCS, mWCS_ToSSCS_Transform, relLocWCS);
   UtEntity::ComputeAzimuthAndElevation(relLocSSCS, aAz, aEl);
}

// =================================================================================================
//! Convert stabilized scan az/el to part-relative az/el.
//! @note The angles are converted 'in-place' (the output replaces the input).
// protected
void WsfEM_Antenna::ConvertAnglesFromSSCS_ToPCS(double& aAz, double& aEl)
{
   // Ensure the WCS->SSCS transform is valid
   if (!mWCS_ToSSCS_TransformIsValid)
   {
      UpdateWCS_ToSSCS_Transform();
   }
   double range         = 1.0;
   double relLocXY      = range * cos(aEl);
   double relLocSSCS[3] = {relLocXY * cos(aAz), relLocXY * sin(aAz), -range * sin(aEl)};
   double relLocWCS[3];
   UtMat3d::InverseTransform(relLocWCS, mWCS_ToSSCS_Transform, relLocSSCS);

   mArticulatedPartPtr->ComputeAspect(relLocWCS, aAz, aEl);
}

// =================================================================================================
//! Update the locally cached LLA location of the antenna.
// protected
void WsfEM_Antenna::UpdateLocationLLA()
{
   if (!mLocationWCS_IsValid)
   {
      UpdateLocationWCS();
   }
   UtEntity::ConvertWCSToLLA(mLocationWCS, mLat, mLon, mAlt);
   mLocationLLA_IsValid = true;
}

// =================================================================================================
//! Update the locally cached WCS location of the antenna.
// protected
void WsfEM_Antenna::UpdateLocationWCS()
{
   assert(mArticulatedPartPtr != nullptr);

#ifdef TODO_LATER
   // Then intent of the 'antenna_height' (as provided in mOffset) is to act as an addition to the
   // location offset provided in the host articulated part. Rotations due to cueing are to occur
   // about this combined offset.
   //
   // The original implementation was erroneously treating the additional offset as being applied
   // in the articulated part frame - INCLUDING ROTATIONS DUE TO CUEING.

   // Compute the composite offset of the part and the antenna height.
   double offsetECS[3];
   mArticulatedPartPtr->GetLocation(offsetECS);
   UtVec3d::Add(offsetECS, offsetECS, mOffset);

   // Get the unrotated offset.
   mArticulatedPartPtr->GetOwningEntity()->ConvertECSToWCS(offsetECS, mLocationWCS);
#else
   // Convert the antenna offset from a PCS vector to WCS vector.
   mArticulatedPartPtr->ConvertPCSVectorToWCS(mOffsetWCS, mOffset);

   // Get the WCS location of the antenna.
   mArticulatedPartPtr->GetLocationWCS(mOffsetWCS, mLocationWCS);
#endif
   mLocationWCS_IsValid = true;
}

// =================================================================================================
// private
void WsfEM_Antenna::UpdateWCS_ToACS_Transform()
{
   // Determine if the ECS->ACS transformation needs to be recomputed. This needs to occur if either the
   // UNCUED orientation of the host articulated part changes with respect to the platform or if the
   // orientation of the antenna changes with respect to the articulated part.
   //
   // NOTE: The ECS->PCS and PCS->ACS could be maintained separated, but these are rarely changed.

   double yaw;
   double pitch;
   double roll;
   mArticulatedPartPtr->GetOrientation(yaw, pitch, roll);
   if ((!mECS_ToACS_TransformIsValid) || (yaw != mPartYaw) || (pitch != mPartPitch) || (roll != mPartRoll))
   {
      // Compute the ECS->PCS transform.
      mPartYaw   = yaw;
      mPartPitch = pitch;
      mPartRoll  = roll;
      double ECS_ToPCS_Transform[3][3];
      UtEntity::ComputeRotationalTransform(mPartYaw, mPartPitch, mPartRoll, ECS_ToPCS_Transform);

      // Compute the PCS->ACS transform.
      double PCS_ToACS_Transform[3][3];
      UtEntity::ComputeRotationalTransform(0.0, mPitch, 0.0, PCS_ToACS_Transform);

      // Compute the ECS->ACS transform.
      // [ ECS->ACS ] = [ PCS->ACS ] x [ ECS->PCS ]
      UtMat3d::Multiply(mECS_ToACS_Transform, PCS_ToACS_Transform, ECS_ToPCS_Transform);
      mECS_ToACS_TransformIsValid = true;
   }

   // Now update the WCS->ACS transform.
   mArticulatedPartPtr->GetOwningEntity()->ComputeWCSToLocalTransform(mWCS_ToACS_Transform, mECS_ToACS_Transform);
   mWCS_ToACS_TransformIsValid = true;
}

// =================================================================================================
// private
void WsfEM_Antenna::UpdateWCS_ToNED_Transform()
{
   if (!mLocationLLA_IsValid)
   {
      UpdateLocationLLA();
   }
   double tempLocationWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(mLat, mLon, mAlt, mWCS_ToNED_Transform, tempLocationWCS);
   mWCS_ToNED_TransformIsValid = true;
}

// =================================================================================================
//! Update the world to stabilized scan coordinate system transform (WCS->SSCS).
//! @note This routine should only get called if a stabilized scan has been requested.
// private
void WsfEM_Antenna::UpdateWCS_ToSSCS_Transform()
{
   double       heading, pitch, roll;
   WsfPlatform* platformPtr = mArticulatedPartPtr->GetPlatform();
   platformPtr->GetOrientationNED(heading, pitch, roll);

   // Get the current ECS->PCS transform for the articulated part, which includes the baseline
   // part orientation AND cue orientation.

   double ECS_ToPCS_Transform[3][3];
   mArticulatedPartPtr->GetECSToPCSTransform(ECS_ToPCS_Transform);

   // Compute the NED->ECS and NED->PCS transforms for the fully stabilized case (no pitch or roll).

   double NED_ToECS_TransformFullStab[3][3];
   UtEntity::ComputeRotationalTransform(heading, 0.0, 0.0, NED_ToECS_TransformFullStab);
   double NED_ToPCS_TransformFullStab[3][3];
   UtMat3d::Multiply(NED_ToPCS_TransformFullStab, ECS_ToPCS_Transform, NED_ToECS_TransformFullStab);

   // Now compute the NED->SSCS transform that reflects the proper stabilization.

   double NED_ToSSCS_Transform[3][3];
   UtMat3d::Set(NED_ToSSCS_Transform, 0.0); // initilaize
   if (mScanStabilization == cSS_BOTH)
   {
      // Stabilized in both pitch and roll
      UtMat3d::Set(NED_ToSSCS_Transform, NED_ToPCS_TransformFullStab);
   }
   else
   {
      // Single-axis stabilization (pitch or roll, but not-both)

      // Compute the NED->ECS and NED->PCS transforms for the non-stabilized case.

      double NED_ToECS_TransformNoStab[3][3];
      platformPtr->GetNEDToECSTransform(NED_ToECS_TransformNoStab);
      double NED_ToPCS_TransformNoStab[3][3];
      UtMat3d::Multiply(NED_ToPCS_TransformNoStab, ECS_ToPCS_Transform, NED_ToECS_TransformNoStab);

      if (mScanStabilization == cSS_PITCH)
      {
         // Pitch stabilized

         // The result X-axis is the X-axis of the fully-stabilized transform.
         UtVec3d::Set(NED_ToSSCS_Transform[0], NED_ToPCS_TransformFullStab[0]);
         // The X-Y plane of the result will be defined by the X-axis of the fully-stabilized transform and
         // the Y-axis of the non-stabilized transform. Use the cross product to derive the result Z-axis.
         UtVec3d::CrossProduct(NED_ToSSCS_Transform[2], NED_ToPCS_TransformFullStab[0], NED_ToPCS_TransformNoStab[1]);
         double mag = UtVec3d::Normalize(NED_ToSSCS_Transform[2]);
         if (mag < 1.0E-8)
         {
            // The vectors that are supposed to define the X-Y plane are (or nearly) collinear.
            // Choose a different unstabilized axis.
            UtVec3d::CrossProduct(NED_ToSSCS_Transform[2], NED_ToPCS_TransformFullStab[0], NED_ToPCS_TransformNoStab[0]);
            UtVec3d::Normalize(NED_ToSSCS_Transform[2]);
         }
         // Create the result Y-axis that forms the orthogonal system.
         UtVec3d::CrossProduct(NED_ToSSCS_Transform[1], NED_ToSSCS_Transform[2], NED_ToSSCS_Transform[0]);
      }
      else if (mScanStabilization == cSS_ROLL)
      {
         // Roll stabilized

         // The result Y-axis is the Y-axis of the fully-stabilized transform.
         UtVec3d::Set(NED_ToSSCS_Transform[1], NED_ToPCS_TransformFullStab[1]);
         // The X-Y plane of the result will be defined by the X-axis of the none-stabilized transform and
         // the Y-axis of the fully-stabilized transform. Use the cross product to derive the result Z-axis.
         UtVec3d::CrossProduct(NED_ToSSCS_Transform[2], NED_ToPCS_TransformNoStab[0], NED_ToPCS_TransformFullStab[1]);
         double mag = UtVec3d::Normalize(NED_ToSSCS_Transform[2]);
         if (mag < 1.0E-8)
         {
            // The vectors that are supposed to define the X-Y plane are (or nearly) collinear.
            // Choose a different unstabilized axis
            UtVec3d::CrossProduct(NED_ToSSCS_Transform[2], NED_ToPCS_TransformNoStab[1], NED_ToPCS_TransformFullStab[1]);
            UtVec3d::Normalize(NED_ToSSCS_Transform[2]);
         }
         // Create the result X-axis that forms the orthogonal system.
         UtVec3d::CrossProduct(NED_ToSSCS_Transform[0], NED_ToSSCS_Transform[1], NED_ToSSCS_Transform[2]);
      }
   }

   // And FINALLY, compute the WCS->SSCS transform.

   double WCS_ToNED_Transform[3][3];
   platformPtr->GetWCSToNEDTransform(WCS_ToNED_Transform);
   UtMat3d::Multiply(mWCS_ToSSCS_Transform, NED_ToSSCS_Transform, WCS_ToNED_Transform);
   mWCS_ToSSCS_TransformIsValid = true;
}
