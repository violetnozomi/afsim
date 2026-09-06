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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfEM_Interaction.hpp"

#include <algorithm>
#include <cassert>
#include <iomanip>

#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfComm.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfLOS_Manager.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfZoneAttenuation.hpp"

// =================================================================================================
//! Initialize an interaction between a receiver and a target platform.
//!
//! This form is typically used where a passive sensor is attempting to
//! detect a platform.
//!
//! @param aRcvrPtr [input] The receiver.
//! @param aTgtPtr  [input] The target platform.
//! @returns 0 if the target is within the geometric limitations of the
//! receiver and not masked by the Earth's horizon.
unsigned int WsfEM_Interaction::BeginOneWayInteraction(WsfEM_Rcvr* aRcvrPtr, WsfPlatform* aTgtPtr)
{
   mXmtrPtr            = nullptr;
   mRcvrPtr            = aRcvrPtr;
   mTgtPtr             = aTgtPtr;
   mRcvrNoisePower     = aRcvrPtr->GetNoisePower();
   mDetectionThreshold = aRcvrPtr->GetDetectionThreshold();
   mEarthRadiusScale   = aRcvrPtr->GetEarthRadiusMultiplier();

   if (CategoryIsSet())
   {
      ComputeZoneAttenuation(aRcvrPtr->GetPlatform(), aTgtPtr, false);
   }

   WsfEM_Antenna* rcvrAntennaPtr = mRcvrPtr->GetAntenna();

   // Get the range and unit vector from the receiver to the target.

   rcvrAntennaPtr->GetLocationWCS(mRcvrLoc.mLocWCS);
   aTgtPtr->GetLocationWCS(mTgtLoc.mLocWCS);
   UtVec3d::Subtract(mRcvrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
   mRcvrToTgt.mRange = UtVec3d::Normalize(mRcvrToTgt.mTrueUnitVecWCS);
   mTgtToRcvr.mRange = mRcvrToTgt.mRange;

   // Determine if the target is within range of the receiver.

   mCheckedStatus |= cRCVR_RANGE_LIMITS;
   if (!rcvrAntennaPtr->WithinRange(mRcvrToTgt.mRange))
   {
      mFailedStatus |= cRCVR_RANGE_LIMITS;
      return mFailedStatus;
   }

   // Determine if the target is within the altitude limits of the receiver.

   aTgtPtr->GetLocationLLA(mTgtLoc.mLat, mTgtLoc.mLon, mTgtLoc.mAlt);
   mTgtLoc.mIsValid = true;
   mCheckedStatus |= cRCVR_ALTITUDE_LIMITS;
   if (!rcvrAntennaPtr->WithinAltitude(mTgtLoc.mAlt))
   {
      mFailedStatus |= cRCVR_ALTITUDE_LIMITS;
      return mFailedStatus;
   }

   // Determine if the line-of-sight is masked by the horizon.

   rcvrAntennaPtr->GetLocationLLA(mRcvrLoc.mLat, mRcvrLoc.mLon, mRcvrLoc.mAlt);
   mRcvrLoc.mIsValid = true;
   mCheckedStatus |= cRCVR_HORIZON_MASKING;
   if (mRcvrPtr->CheckMasking() && mRcvrPtr->IsHorizonMaskingEnabled())
   {
      if (MaskedByHorizon(mRcvrPtr, mTgtPtr, mEarthRadiusScale))
      {
         mFailedStatus |= cRCVR_HORIZON_MASKING;
         return mFailedStatus;
      }
   }

   // Determine if the target is within the field of view of the receiver.

   mCheckedStatus |= cRCVR_ANGLE_LIMITS;
   if (!WithinFieldOfView(rcvrAntennaPtr, mRcvrLoc, mTgtLoc, mRcvrToTgt, mTgtToRcvr))
   {
      mFailedStatus |= cRCVR_ANGLE_LIMITS;
      return mFailedStatus;
   }

   // Compute the target-to-receiver aspect angles from the unit vectors

   mTgtPtr->ComputeAspect(mTgtToRcvr.mTrueUnitVecWCS, mTgtToRcvr.mTrueAz, mTgtToRcvr.mTrueEl);
   mTgtPtr->ComputeAspect(mTgtToRcvr.mUnitVecWCS, mTgtToRcvr.mAz, mTgtToRcvr.mEl);

   // Compute the masking factor, which accounts for obstruction due to structure.

   ComputeMaskingFactor();

   return mFailedStatus;
}

// =================================================================================================
//! Initialize an interaction between a transmitter and a receiver.
//!
//! This is typically used in one of the following situations:
//! - A passive sensing device.
//! - A communications transmission.
//!
//! @param aXmtrPtr [input] The transmitter.
//! @param aRcvrPtr [input] The receiver.
//! @param aCheckXmtrLimits [input] 'true' if transmitter geometric limits should be checked.
//! This should be 'true' for comm and interference interactions and 'false' for Passive-type
//! interactions.
//! @param aCheckRcvrLimits [input] 'true' if receiver geometric limits should be checked.
//! This should be 'true' for comm, passive-type interactions and 'false' for interference-type interactions.
//! @param aCheckMaskingFactor [input] 'true' if the masking 'factor', which we don't want for
//! a bistatic calculation between the transmitter and receiver.  This function is void of a
//! target, so it should have never called masking factor.  But other users expect it, so it stays.
//!
//! @returns 0 if the transmitter and receiver are within each others
//! geometric limits and are not masked by the Earth's horizon.
unsigned int WsfEM_Interaction::BeginOneWayInteraction(WsfEM_Xmtr* aXmtrPtr,
                                                       WsfEM_Rcvr* aRcvrPtr,
                                                       bool        aCheckXmtrLimits,
                                                       bool        aCheckRcvrLimits,
                                                       bool        aCheckMaskingFactor)
{
   mXmtrPtr = aXmtrPtr;
   mRcvrPtr = aRcvrPtr;
   mTgtPtr  = nullptr;
   // A potential bug.  Tgt may be set coming in for bistatic, but it
   // has to be temp nulled here for ComputeMaskingFactor to work correctly.
   // This assert assures we don't remove the nulling.
   assert(mTgtPtr == nullptr);
   mRcvrNoisePower     = aRcvrPtr->GetNoisePower();
   mDetectionThreshold = aRcvrPtr->GetDetectionThreshold();
   mEarthRadiusScale   = mXmtrPtr->GetEarthRadiusMultiplier();

   WsfEM_Antenna* rcvrAntennaPtr = mRcvrPtr->GetAntenna();
   WsfEM_Antenna* xmtrAntennaPtr = mXmtrPtr->GetAntenna();

   if (CategoryIsSet())
   {
      ComputeZoneAttenuation(aRcvrPtr->GetPlatform(), aXmtrPtr->GetPlatform(), false);
   }

   // Compute the range and unit vectors between the transmitter and receiver.

   rcvrAntennaPtr->GetLocationWCS(mRcvrLoc.mLocWCS);
   xmtrAntennaPtr->GetLocationWCS(mXmtrLoc.mLocWCS);
   UtVec3d::Subtract(mRcvrToXmtr.mTrueUnitVecWCS, mXmtrLoc.mLocWCS, mRcvrLoc.mLocWCS);
   mRcvrToXmtr.mRange = UtVec3d::Normalize(mRcvrToXmtr.mTrueUnitVecWCS);
   mXmtrToRcvr.mRange = mRcvrToXmtr.mRange;

   // NOTE: Even though the limits may not be 'checked', they are marked as such if
   // they have been computed.  (The Print() method uses the flag).

   mCheckedStatus |= cRCVR_RANGE_LIMITS; // See NOTE above
   if (aCheckRcvrLimits)
   {
      // If the transmitter is an interferer then we don't check the range limits of the
      // receiver.  The receivers range limit is used to determine how far away it
      // can detect targets.  This has very little to do with how effective the interferer
      // might be...

      if (!rcvrAntennaPtr->WithinRange(mRcvrToXmtr.mRange))
      {
         mFailedStatus |= cRCVR_RANGE_LIMITS;
         return mFailedStatus;
      }
   }

   mCheckedStatus |= cXMTR_RANGE_LIMITS; // See NOTE above
   if (aCheckXmtrLimits)
   {
      if (!xmtrAntennaPtr->WithinRange(mXmtrToRcvr.mRange))
      {
         mFailedStatus |= cXMTR_RANGE_LIMITS;
         return mFailedStatus;
      }
   }

   rcvrAntennaPtr->GetLocationLLA(mRcvrLoc.mLat, mRcvrLoc.mLon, mRcvrLoc.mAlt);
   mRcvrLoc.mIsValid = true;
   xmtrAntennaPtr->GetLocationLLA(mXmtrLoc.mLat, mXmtrLoc.mLon, mXmtrLoc.mAlt);
   mXmtrLoc.mIsValid = true;

   mCheckedStatus |= cRCVR_ALTITUDE_LIMITS; // See NOTE above
   if (aCheckRcvrLimits)
   {
      if (!rcvrAntennaPtr->WithinAltitude(mXmtrLoc.mAlt))
      {
         mFailedStatus |= cRCVR_ALTITUDE_LIMITS;
         return mFailedStatus;
      }
   }

   mCheckedStatus |= cXMTR_ALTITUDE_LIMITS; // See NOTE above
   if (aCheckXmtrLimits)
   {
      if (!xmtrAntennaPtr->WithinAltitude(mRcvrLoc.mAlt))
      {
         mFailedStatus |= cXMTR_ALTITUDE_LIMITS;
         return mFailedStatus;
      }
   }

   // Determine if the line-of-sight is masked by the horizon.

   mCheckedStatus |= cRCVR_HORIZON_MASKING;
   if (mRcvrPtr->CheckXmtrMasking() && mRcvrPtr->IsHorizonMaskingEnabled())
   {
      if (MaskedByHorizon(mXmtrPtr, mRcvrPtr))
      {
         mFailedStatus |= cRCVR_HORIZON_MASKING;
         return mFailedStatus;
      }
   }

   // Determine if the transmitter is within the field of view of the receiver.

   bool ignoreLimits;
   bool withinFOV;

   ignoreLimits = (!aCheckRcvrLimits);
   withinFOV    = WithinFieldOfView(rcvrAntennaPtr, mRcvrLoc, mXmtrLoc, mRcvrToXmtr, mXmtrToRcvr, ignoreLimits);
   mCheckedStatus |= cRCVR_ANGLE_LIMITS; // See NOTE above
   if (aCheckRcvrLimits && (!withinFOV))
   {
      mFailedStatus |= cRCVR_ANGLE_LIMITS;
      return mFailedStatus;
   }

   // Determine if the receiver is within the field of view of the transmitter.

   ignoreLimits = (!aCheckXmtrLimits);
   withinFOV    = WithinFieldOfView(xmtrAntennaPtr, mXmtrLoc, mRcvrLoc, mXmtrToRcvr, mRcvrToXmtr, ignoreLimits);
   mCheckedStatus |= cXMTR_ANGLE_LIMITS; // See NOTE above
   if (aCheckXmtrLimits && (!withinFOV))
   {
      mFailedStatus |= cXMTR_ANGLE_LIMITS;
      return mFailedStatus;
   }

   // Compute the apparent aspect angles.  We don't have to worry about the true angles
   // because the symmetry of the WithinFieldOfView calls.

   xmtrAntennaPtr->ComputeAspect(mXmtrToRcvr.mUnitVecWCS, mXmtrToRcvr.mAz, mXmtrToRcvr.mEl);
   rcvrAntennaPtr->ComputeAspect(mRcvrToXmtr.mUnitVecWCS, mRcvrToXmtr.mAz, mRcvrToXmtr.mEl);

   // Compute the masking factor, which accounts for obstruction due to structure.

   if (aCheckMaskingFactor)
   {
      ComputeMaskingFactor();
   }

   return mFailedStatus;
}

// =================================================================================================
//! Initialize an interaction between a transmitter, a target and a receiver.
//!
//! This is typically used to model a radar system (or something that sends
//! out a signal and looks at the reflection from a target).
//!
//! @param aXmtrPtr [input] The transmitter.
//! @param aTgtPtr  [input] The target.
//! @param aRcvrPtr [input] The receiver.
//! @returns 0 if the target is within the geometric limits of the transmitter and
//! receiver and not masked by the Earth's horizon.
unsigned int WsfEM_Interaction::BeginTwoWayInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr)
{
   mXmtrPtr            = aXmtrPtr;
   mRcvrPtr            = aRcvrPtr;
   mTgtPtr             = aTgtPtr;
   mRcvrNoisePower     = aRcvrPtr->GetNoisePower();
   mDetectionThreshold = aRcvrPtr->GetDetectionThreshold();
   mEarthRadiusScale   = mXmtrPtr->GetEarthRadiusMultiplier();

   WsfEM_Antenna* rcvrAntennaPtr = mRcvrPtr->GetAntenna();
   WsfEM_Antenna* xmtrAntennaPtr = mXmtrPtr->GetAntenna();

   mBistatic = (xmtrAntennaPtr != rcvrAntennaPtr);

   if (mBistatic)
   {
      // The tx to rx data wasn't set, which was needed for bistatic.  We also want
      // to do the check to see if there is masking between the TX and RX.  But
      // we don't want to use "the antenna" range, because the "time" signal is
      // traveling over an implied omni directional comm
      // TODO: the function BeginOneWayInteraction performs redundant calculations
      // which is OK but inefficient.
      int status = BeginOneWayInteraction(aXmtrPtr, aRcvrPtr, false, false, false);
      // Restore the target!!! The above function may set it to null.
      mTgtPtr = aTgtPtr;
      if (status)
      {
         // mFailedStatus is already set with the reason.  Get out now before it's too late!
         return mFailedStatus;
      }
   }

   if (CategoryIsSet())
   {
      ComputeZoneAttenuation(aRcvrPtr->GetPlatform(), aTgtPtr, true);
   }

   // Get the range and unit vector from the receiver to the target.

   rcvrAntennaPtr->GetLocationWCS(mRcvrLoc.mLocWCS);
   aTgtPtr->GetLocationWCS(mTgtLoc.mLocWCS);
   UtVec3d::Subtract(mRcvrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
   mRcvrToTgt.mRange = UtVec3d::Normalize(mRcvrToTgt.mTrueUnitVecWCS);
   mTgtToRcvr.mRange = mRcvrToTgt.mRange;

   // Determine if the target is within range of the receiver.

   mCheckedStatus |= cRCVR_RANGE_LIMITS;
   if (!rcvrAntennaPtr->WithinRange(mRcvrToTgt.mRange))
   {
      mFailedStatus |= cRCVR_RANGE_LIMITS;
      return mFailedStatus;
   }

   mCheckedStatus |= cXMTR_RANGE_LIMITS;
   if (mBistatic)
   {
      // Bistatic interaction.  Get the range and unit from the transmitter to the target.
      xmtrAntennaPtr->GetLocationWCS(mXmtrLoc.mLocWCS);
      UtVec3d::Subtract(mXmtrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mXmtrLoc.mLocWCS);
      mXmtrToTgt.mRange = UtVec3d::Normalize(mXmtrToTgt.mTrueUnitVecWCS);
      mTgtToXmtr.mRange = mXmtrToTgt.mRange;
      if (!xmtrAntennaPtr->WithinRange(mXmtrToTgt.mRange))
      {
         mFailedStatus |= cXMTR_RANGE_LIMITS;
         return mFailedStatus;
      }
   }

   // Determine if the target is within altitude limits.

   aTgtPtr->GetLocationLLA(mTgtLoc.mLat, mTgtLoc.mLon, mTgtLoc.mAlt);
   mTgtLoc.mIsValid = true;
   mCheckedStatus |= cRCVR_ALTITUDE_LIMITS;
   if (!rcvrAntennaPtr->WithinAltitude(mTgtLoc.mAlt))
   {
      mFailedStatus |= cRCVR_ALTITUDE_LIMITS;
      return mFailedStatus;
   }

   mCheckedStatus |= cXMTR_ALTITUDE_LIMITS;
   if (mBistatic)
   {
      if (!xmtrAntennaPtr->WithinAltitude(mTgtLoc.mAlt))
      {
         mFailedStatus |= cXMTR_ALTITUDE_LIMITS;
         return mFailedStatus;
      }
   }

   // Determine if the line-of-sight is masked by the horizon.

   mCheckedStatus |= cRCVR_HORIZON_MASKING;
   rcvrAntennaPtr->GetLocationLLA(mRcvrLoc.mLat, mRcvrLoc.mLon, mRcvrLoc.mAlt);
   mRcvrLoc.mIsValid = true;
   if (mRcvrPtr->CheckMasking() && mRcvrPtr->IsHorizonMaskingEnabled())
   {
      if (MaskedByHorizon(mRcvrPtr, mTgtPtr, mEarthRadiusScale))
      {
         mFailedStatus |= cRCVR_HORIZON_MASKING;
         return mFailedStatus;
      }
   }

   mCheckedStatus |= cXMTR_HORIZON_MASKING;
   if (mBistatic)
   {
      xmtrAntennaPtr->GetLocationLLA(mXmtrLoc.mLat, mXmtrLoc.mLon, mXmtrLoc.mAlt);
      if (mXmtrPtr->CheckMasking() && mXmtrPtr->IsHorizonMaskingEnabled())
      {
         if (MaskedByHorizon(mXmtrPtr, mTgtPtr, mEarthRadiusScale))
         {
            mFailedStatus |= cXMTR_HORIZON_MASKING;
            return mFailedStatus;
         }
      }
   }
   else
   {
      // Monostatic.
      mXmtrLoc = mRcvrLoc;
   }

   // Determine if the target is within the field of view of the receiver.

   mCheckedStatus |= cRCVR_ANGLE_LIMITS;
   if (!WithinFieldOfView(rcvrAntennaPtr, mRcvrLoc, mTgtLoc, mRcvrToTgt, mTgtToRcvr))
   {
      mFailedStatus |= cRCVR_ANGLE_LIMITS;
      return mFailedStatus;
   }

   // Compute the target-to-receiver aspect angles from the unit vectors.

   mTgtPtr->ComputeAspect(mTgtToRcvr.mTrueUnitVecWCS, mTgtToRcvr.mTrueAz, mTgtToRcvr.mTrueEl);
   mTgtPtr->ComputeAspect(mTgtToRcvr.mUnitVecWCS, mTgtToRcvr.mAz, mTgtToRcvr.mEl);

   // If Bistatic then determine if the target is within the field of view of the transmitter

   mCheckedStatus |= cXMTR_ANGLE_LIMITS;
   if (mBistatic)
   {
      if (!WithinFieldOfView(xmtrAntennaPtr, mXmtrLoc, mTgtLoc, mXmtrToTgt, mTgtToXmtr))
      {
         mFailedStatus |= cXMTR_ANGLE_LIMITS;
         return mFailedStatus;
      }

      // Compute the target-to-transmitter aspect angles from the unit vectors

      mTgtPtr->ComputeAspect(mTgtToXmtr.mTrueUnitVecWCS, mTgtToXmtr.mTrueAz, mTgtToXmtr.mTrueEl);
      mTgtPtr->ComputeAspect(mTgtToXmtr.mUnitVecWCS, mTgtToXmtr.mAz, mTgtToXmtr.mEl);
   }
   else
   {
      // For a monostatic operation, the transmitter-to-target values can be had directly
      // from the receiver-to-target values.

      mXmtrToTgt = mRcvrToTgt;
      mTgtToXmtr = mTgtToRcvr;
   }

   // Compute the masking factor, which accounts for obstruction due to structure.

   ComputeMaskingFactor();

   return mFailedStatus;
}

// =================================================================================================
unsigned int WsfEM_Interaction::BeginTwoWayOTH_Interaction(WsfEM_Xmtr*  aXmtrPtr,
                                                           WsfPlatform* aTgtPtr,
                                                           WsfEM_Rcvr*  aRcvrPtr,
                                                           double       aReflectionLocWCS[3])
{
   mXmtrPtr            = aXmtrPtr;
   mRcvrPtr            = aRcvrPtr;
   mTgtPtr             = aTgtPtr;
   mRcvrNoisePower     = aRcvrPtr->GetNoisePower();
   mDetectionThreshold = aRcvrPtr->GetDetectionThreshold();
   mEarthRadiusScale   = mXmtrPtr->GetEarthRadiusMultiplier();

   WsfEM_Antenna* rcvrAntennaPtr = mRcvrPtr->GetAntenna();
   WsfEM_Antenna* xmtrAntennaPtr = mXmtrPtr->GetAntenna();

   mBistatic = (xmtrAntennaPtr != rcvrAntennaPtr);

   if (CategoryIsSet())
   {
      ComputeZoneAttenuation(aRcvrPtr->GetPlatform(), aTgtPtr, true);
   }

   // Get the range from the receiver to the target.
   // Get the unit vector from the Rcvr to the Reflection Point
   rcvrAntennaPtr->GetLocationWCS(mRcvrLoc.mLocWCS);
   aTgtPtr->GetLocationWCS(mTgtLoc.mLocWCS);
   UtVec3d::Subtract(mRcvrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
   mRcvrToTgt.mRange = UtVec3d::Normalize(mRcvrToTgt.mTrueUnitVecWCS);
   mTgtToRcvr.mRange = mRcvrToTgt.mRange;

   // Determine if the target is within range of the receiver.

   mCheckedStatus |= cRCVR_RANGE_LIMITS;
   if (!rcvrAntennaPtr->WithinRange(mRcvrToTgt.mRange))
   {
      mFailedStatus |= cRCVR_RANGE_LIMITS;
      return mFailedStatus;
   }

   mCheckedStatus |= cXMTR_RANGE_LIMITS;
   if (mBistatic)
   {
      // Bistatic interaction.  Get the range and unit from the transmitter to the target.
      xmtrAntennaPtr->GetLocationWCS(mXmtrLoc.mLocWCS);
      UtVec3d::Subtract(mXmtrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mXmtrLoc.mLocWCS);
      mXmtrToTgt.mRange = UtVec3d::Normalize(mXmtrToTgt.mTrueUnitVecWCS);
      mTgtToXmtr.mRange = mXmtrToTgt.mRange;
      if (!xmtrAntennaPtr->WithinRange(mXmtrToTgt.mRange))
      {
         mFailedStatus |= cXMTR_RANGE_LIMITS;
         return mFailedStatus;
      }
   }

   // Determine if the target is within altitude limits.

   aTgtPtr->GetLocationLLA(mTgtLoc.mLat, mTgtLoc.mLon, mTgtLoc.mAlt);
   mTgtLoc.mIsValid = true;
   mCheckedStatus |= cRCVR_ALTITUDE_LIMITS;
   if (!rcvrAntennaPtr->WithinAltitude(mTgtLoc.mAlt))
   {
      mFailedStatus |= cRCVR_ALTITUDE_LIMITS;
      return mFailedStatus;
   }

   mCheckedStatus |= cXMTR_ALTITUDE_LIMITS;
   if (mBistatic)
   {
      if (!xmtrAntennaPtr->WithinAltitude(mTgtLoc.mAlt))
      {
         mFailedStatus |= cXMTR_ALTITUDE_LIMITS;
         return mFailedStatus;
      }
   }

   // Determine if the line-of-sight is masked by the horizon.
   mCheckedStatus |= cRCVR_HORIZON_MASKING;
   rcvrAntennaPtr->GetLocationLLA(mRcvrLoc.mLat, mRcvrLoc.mLon, mRcvrLoc.mAlt);
   mRcvrLoc.mIsValid = true;
   if (mRcvrPtr->CheckMasking() && mRcvrPtr->IsHorizonMaskingEnabled())
   {
      if (MaskedByHorizon(mRcvrPtr, mTgtPtr, aReflectionLocWCS, mEarthRadiusScale))
      {
         mFailedStatus |= cRCVR_HORIZON_MASKING;
         return mFailedStatus;
      }
   }

   mCheckedStatus |= cXMTR_HORIZON_MASKING;
   if (mBistatic)
   {
      xmtrAntennaPtr->GetLocationLLA(mXmtrLoc.mLat, mXmtrLoc.mLon, mXmtrLoc.mAlt);
      if (mXmtrPtr->CheckMasking() && mXmtrPtr->IsHorizonMaskingEnabled())
      {
         if (MaskedByHorizon(mXmtrPtr, mTgtPtr, aReflectionLocWCS, mEarthRadiusScale))
         {
            mFailedStatus |= cXMTR_HORIZON_MASKING;
            return mFailedStatus;
         }
      }
   }
   else
   {
      // Monostatic.
      mXmtrLoc = mRcvrLoc;
   }

   // Determine if the target is within the field of view of the receiver.

   mCheckedStatus |= cRCVR_ANGLE_LIMITS;
   if (!WithinFieldOfView(rcvrAntennaPtr, mRcvrLoc, mTgtLoc, mRcvrToTgt, mTgtToRcvr))
   {
      mFailedStatus |= cRCVR_ANGLE_LIMITS;
      return mFailedStatus;
   }

   // Compute the target-to-receiver aspect angles from the unit vectors.

   mTgtPtr->ComputeAspect(mTgtToRcvr.mTrueUnitVecWCS, mTgtToRcvr.mTrueAz, mTgtToRcvr.mTrueEl);
   mTgtPtr->ComputeAspect(mTgtToRcvr.mUnitVecWCS, mTgtToRcvr.mAz, mTgtToRcvr.mEl);

   // If Bistatic then determine if the target is within the field of view of the transmitter

   mCheckedStatus |= cXMTR_ANGLE_LIMITS;
   if (mBistatic)
   {
      if (!WithinFieldOfView(xmtrAntennaPtr, mXmtrLoc, mTgtLoc, mXmtrToTgt, mTgtToXmtr))
      {
         mFailedStatus |= cXMTR_ANGLE_LIMITS;
         return mFailedStatus;
      }

      // Compute the target-to-transmitter aspect angles from the unit vectors

      mTgtPtr->ComputeAspect(mTgtToXmtr.mTrueUnitVecWCS, mTgtToXmtr.mTrueAz, mTgtToXmtr.mTrueEl);
      mTgtPtr->ComputeAspect(mTgtToXmtr.mUnitVecWCS, mTgtToXmtr.mAz, mTgtToXmtr.mEl);
   }
   else
   {
      // For a monostatic operation, the transmitter-to-target values can be had directly
      // from the receiver-to-target values.

      mXmtrToTgt = mRcvrToTgt;
      mTgtToXmtr = mTgtToRcvr;
   }

   return mFailedStatus;
}

// =================================================================================================
//! Initialize a generic interaction.
//! This method can be used when one of the other 'Begin...Interaction' methods is not appropriate.
//! It simply sets the pointers to the transmitter, target and receiver (which are private) in the interaction structure.
//! @param aXmtrPtr [input] The transmitter (can be zero if there is no transmitter).
//! @param aTgtPtr  [input] The target (can be zero if there is no target).
//! @param aRcvrPtr [input] The receiver (can be zero if there is no receiver).
//! @returns 0.
unsigned int WsfEM_Interaction::BeginGenericInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr)
{
   mXmtrPtr = aXmtrPtr;
   mTgtPtr  = aTgtPtr;
   mRcvrPtr = aRcvrPtr;

   if (CategoryIsSet())
   {
      ComputeZoneAttenuation(aRcvrPtr->GetPlatform(), aTgtPtr, false);
   }

   mBistatic = false;
   if ((mXmtrPtr != nullptr) && (mRcvrPtr != nullptr) && (mTgtPtr != nullptr))
   {
      mBistatic = (mRcvrPtr->GetAntenna() != mXmtrPtr->GetAntenna());
   }
   mEarthRadiusScale = 1.0;
   if (mXmtrPtr != nullptr)
   {
      mEarthRadiusScale = mXmtrPtr->GetEarthRadiusMultiplier();
   }
   else if (mRcvrPtr != nullptr)
   {
      mEarthRadiusScale = mRcvrPtr->GetEarthRadiusMultiplier();
   }
   return 0;
}

// =================================================================================================
//! Compute the attenuation loss between two points.
double WsfEM_Interaction::ComputeAttenuationFactor(Geometry aGeometry)
{
   // Most interactions involve a transmitter, so use the attenuation model from there. But if a transmitter is
   // not involved (i.e.: a passive optical sensor), use the attenuation model from the receiver.
   double attnFactor = 1.0;
   if ((mXmtrPtr != nullptr) && (mXmtrPtr->GetAttenuationModel() != nullptr))
   {
      WsfEnvironment& environment = mXmtrPtr->GetPlatform()->GetScenario().GetEnvironment();
      attnFactor = mXmtrPtr->GetAttenuationModel()->ComputeAttenuationFactor(*this, environment, aGeometry);
   }
   else if ((mRcvrPtr != nullptr) && (mRcvrPtr->GetAttenuationModel() != nullptr))
   {
      WsfEnvironment& environment = mRcvrPtr->GetPlatform()->GetScenario().GetEnvironment();
      attnFactor = mRcvrPtr->GetAttenuationModel()->ComputeAttenuationFactor(*this, environment, aGeometry);
   }

   // Determine if zone attenuation needs to be considered...
   if (CategoryIsSet())
   {
      if (mZoneAttenuationValue != 0.0)
      {
         attnFactor = std::max(0.0, attnFactor - mZoneAttenuationValue);
      }
   }

   return attnFactor;
}

// =================================================================================================
//! Compute the masking factor.
// private
void WsfEM_Interaction::ComputeMaskingFactor()
{
   double xmtrFactor = 1.0;
   double rcvrFactor = 1.0;
   if (mRcvrPtr == nullptr)
   {
      assert(mRcvrPtr != nullptr); // Should never happen
   }
   else if (mTgtPtr == nullptr)
   {
      // xmtr-rcvr interaction.
      assert(mXmtrPtr != nullptr);
      WsfArticulatedPart* xmtrPartPtr = mXmtrPtr->GetAntenna()->GetArticulatedPart();
      xmtrFactor = xmtrPartPtr->GetMaskingPatternFactor(mXmtrToRcvr.mAz, mXmtrToRcvr.mEl, mXmtrToRcvr.mUnitVecWCS);
      WsfArticulatedPart* rcvrPartPtr = mRcvrPtr->GetAntenna()->GetArticulatedPart();
      rcvrFactor = rcvrPartPtr->GetMaskingPatternFactor(mRcvrToXmtr.mAz, mRcvrToXmtr.mEl, mRcvrToXmtr.mUnitVecWCS);
   }
   else if (mXmtrPtr == nullptr)
   {
      // rcvr-tgt interaction
      WsfArticulatedPart* rcvrPartPtr = mRcvrPtr->GetAntenna()->GetArticulatedPart();
      rcvrFactor = rcvrPartPtr->GetMaskingPatternFactor(mRcvrToTgt.mAz, mRcvrToTgt.mEl, mRcvrToTgt.mUnitVecWCS);
   }
   else
   {
      // xmtr-tgt-rcvr interaction
      WsfArticulatedPart* xmtrPartPtr = mXmtrPtr->GetAntenna()->GetArticulatedPart();
      xmtrFactor = xmtrPartPtr->GetMaskingPatternFactor(mXmtrToTgt.mAz, mXmtrToTgt.mEl, mXmtrToTgt.mUnitVecWCS);
      rcvrFactor = xmtrFactor; // Assume monostatic
      if (mBistatic)
      {
         WsfArticulatedPart* rcvrPartPtr = mRcvrPtr->GetAntenna()->GetArticulatedPart();
         rcvrFactor = rcvrPartPtr->GetMaskingPatternFactor(mRcvrToTgt.mAz, mRcvrToTgt.mEl, mRcvrToTgt.mUnitVecWCS);
      }
   }
   mMaskingFactor = xmtrFactor * rcvrFactor;
}

// =================================================================================================
//! Compute the azimuth and elevation to be used for infrared signature determination.
//!
//! This updates mInfraredSigAz and mInfraredSigEl.
void WsfEM_Interaction::ComputeInfraredSigAzEl()
{
   mInfraredSigAz = mTgtToRcvr.mAz;
   mInfraredSigEl = mTgtToRcvr.mEl;
}

// =================================================================================================
//! Compute the azimuth and elevation to be used for optical signature determination.
//!
//! This updates mOpticalSigAz and mOpticalSigEl.
void WsfEM_Interaction::ComputeOpticalSigAzEl()
{
   mOpticalSigAz = mTgtToRcvr.mAz;
   mOpticalSigEl = mTgtToRcvr.mEl;
}

// =================================================================================================
//! Compute the azimuth and elevation to be used for radar signature determination.
//!
//! This updates mRadarSigAz and mRadarSigEl.
void WsfEM_Interaction::ComputeRadarSigAzEl()
{
   if (mBistatic)
   {
      double bistaticVec[3];
      UtVec3d::Add(bistaticVec, mTgtToXmtr.mUnitVecWCS, mTgtToRcvr.mUnitVecWCS);
      mTgtPtr->ComputeAspect(bistaticVec, mRadarSigAz, mRadarSigEl);
   }
   else
   {
      mRadarSigAz = mTgtToRcvr.mAz;
      mRadarSigEl = mTgtToRcvr.mEl;
   }
}

// =================================================================================================
//! Given the power density at the antenna, compute the power at the output of the receiver.
//!
//! This routine is useful if the caller wants to perform their own signal propagation.
//!
//! @param aPowerDensityAtAntenna [input] The power density (Watts/m^2) at the antenna.
//! @returns The power at the output to the receiver.  It also stores the result in mRcvdPower.
//!
//! @note This method does not apply polarization effects because the polarization of the signal from
//!       the transmitter may have changed due to reflection. The application of these effects is left
//!       to the caller.
//! @note This method does not apply bandwidth effects because the bandwidth of the signal from
//!       the transmitter may have changed due to reflection. The application of these effects is left
//!       to the caller.
double WsfEM_Interaction::ComputeRF_ReceivedPower(double aPowerDensityAtAntenna)
{
   // If necessary, compute the aspect of the target with respect to the current receiver
   // main beam position.
   if (mRcvrBeam.mGain < 0.0)
   {
      ComputeReceiverBeamAspect();
   }
   mRcvdPower = mRcvrPtr->ComputeReceivedPower(mRcvrBeam.mAz,
                                               mRcvrBeam.mEl,
                                               mRcvrBeam.mEBS_Az,
                                               mRcvrBeam.mEBS_El,
                                               aPowerDensityAtAntenna,
                                               mXmtrPtr->GetPolarization(),
                                               mXmtrPtr->GetFrequency(),
                                               mRcvrBeam.mGain);
   return mRcvdPower;
}

// =================================================================================================
//! Compute the power radiated from a transmitter in the direction of the target.
//!
//! This routine is useful if the caller wants to perform their own signal propagation from
//! the transmitter to the target.
//!
//! @returns The radiated power in the directory of the target (Watts). It also stores the
//! the result in mXmtdPower.
double WsfEM_Interaction::ComputeRF_TransmittedPower()
{
   // If necessary, compute the aspect of the target with respect to the current transmitter
   // main beam position.
   if (mXmtrBeam.mGain < 0.0)
   {
      ComputeTransmitterBeamAspect();
   }

   double freq = (mRcvrPtr != nullptr) ? mRcvrPtr->GetFrequency() : 0.0;
   mXmtdPower =
      mXmtrPtr->ComputeRadiatedPower(mXmtrBeam.mAz, mXmtrBeam.mEl, mXmtrBeam.mEBS_Az, mXmtrBeam.mEBS_El, freq, mXmtrBeam.mGain);
   return mXmtdPower;
}

// =================================================================================================
//! This method implements a simple one way link equation.
//!
//! This is useful for things like passive sensors, interferers and communications.
//!
//! @returns The power at the output to the receiver (Watts).
double WsfEM_Interaction::ComputeRF_OneWayPower()
{
   // Compute the signal radiated towards the receiver.

   ComputeRF_TransmittedPower();
   mRcvdPower = mXmtdPower; // Assume range has been computed
   if (mRcvrToXmtr.mRange > 0.0)
   {
      // Propagate the signal to the receiver.

      double range = mRcvrToXmtr.mRange;

      // Determine the loss due to atmospheric absorption
      double xmtrToRcvrAtten = ComputeAttenuationFactor(cXMTR_TO_RCVR);

      mAbsorptionFactor = xmtrToRcvrAtten;

      double p_density_at_rcvr = (mXmtdPower * xmtrToRcvrAtten) / (UtMath::cFOUR_PI * range * range);
      mPowerDensityAtTarget    = p_density_at_rcvr;

      // Receive the signal.
      ComputeRF_ReceivedPower(p_density_at_rcvr);

      // Compute the pattern propagation factor and adjust the signal for the gain/loss.
      // NOTE: This must be done AFTER calling ComputeRF_ReceivedPower because the computation may require the
      //       use of the computed antenna gains.
      ComputeRF_PropagationFactor();
      mRcvdPower *= mPropagationFactor;

      // Account for possible polarization mismatches
      mRcvdPower *= mRcvrPtr->GetPolarizationEffect(mXmtrPtr->GetPolarization());
   }

   // Account for possible Bandwidth mismatches
   mRcvdPower *= mRcvrPtr->GetBandwidthEffect(mXmtrPtr->GetFrequency(), mXmtrPtr->GetBandwidth());

   // Account for structural interference.
   mRcvdPower *= mMaskingFactor;

   return mRcvdPower;
}

// =================================================================================================
//! This method implements the two-way radar equation.
//!
//! @param aTargetCrossSection [input] The cross section of the target (m^2).
//! @returns The power at the output from the receiver (Watts).
double WsfEM_Interaction::ComputeRF_TwoWayPower(double aTargetCrossSection)
{
   // Compute the signal radiated towards the target.

   ComputeRF_TransmittedPower();
   mRcvdPower = mXmtdPower; // Assume range has been computed

   // Assume a monostatic interaction.
   double xmtrRange = std::max(mXmtrToTgt.mRange, 1.0);
   double rcvrRange = xmtrRange;

   // Compute the atmospheric losses for the monostatic system
   double xmtrToTgtAtten = ComputeAttenuationFactor(cXMTR_TO_TARGET);
   double tgtToRcvrAtten = xmtrToTgtAtten;

   if (mBistatic)
   {
      // We've already got the xmtr-to-target, so get the target-to-receiver.
      rcvrRange      = std::max(mRcvrToTgt.mRange, 1.0);
      tgtToRcvrAtten = ComputeAttenuationFactor(cTARGET_TO_RCVR);
   }

   mAbsorptionFactor = xmtrToTgtAtten * tgtToRcvrAtten;

   // Propagate the signal to the target.
   double p_density_at_tgt = (mXmtdPower * xmtrToTgtAtten) / (UtMath::cFOUR_PI * xmtrRange * xmtrRange);
   mPowerDensityAtTarget   = p_density_at_tgt;

   // Compute the reflected signal
   double p_reflected = p_density_at_tgt * aTargetCrossSection;

   // Propagate the reflected signal to the receiver.
   double p_density_at_rcvr = (p_reflected * tgtToRcvrAtten) / (UtMath::cFOUR_PI * rcvrRange * rcvrRange);

   // Receive the signal.
   ComputeRF_ReceivedPower(p_density_at_rcvr);

   // Compute the pattern propagation factor and adjust the signal for the gain/loss.
   // NOTE: This must be done AFTER calling ComputeRF_ReceivedPower because the computation may require the
   //       use of the computed antenna gains.
   ComputeRF_PropagationFactor();
   mRcvdPower *= mPropagationFactor;

   // Polarization effects are not applied because we don't know how the signal polarization changes when
   // reflected by the target.

   // Bandwidth effects are not applied because we are assuming in a two way interaction that there is a matched

   // Account for structural interference.
   mRcvdPower *= mMaskingFactor;

   return mRcvdPower;
}

// =================================================================================================
//! Determine if the horizon obscures the view between a transmitter and receiver.
//!
//! @param aXmtrPtr [input] The transmitter.
//! @param aRcvrPtr [input] The receiver.
//! @returns 'true' if the horizon obscures the view between the objects.
// static
bool WsfEM_Interaction::MaskedByHorizon(const WsfEM_Xmtr* aXmtrPtr, const WsfEM_Rcvr* aRcvrPtr)
{
   double lat1;
   double lon1;
   double alt1;
   aXmtrPtr->GetAntenna()->GetLocationLLA(lat1, lon1, alt1);
   double trnHgt1 = aXmtrPtr->GetPlatform()->GetTerrainHeight();

   double lat2;
   double lon2;
   double alt2;
   aRcvrPtr->GetAntenna()->GetLocationLLA(lat2, lon2, alt2);
   double trnHgt2 = aRcvrPtr->GetPlatform()->GetTerrainHeight();

   return WsfEM_Util::MaskedByHorizon(lat1, lon1, alt1, trnHgt1, lat2, lon2, alt2, trnHgt2, aXmtrPtr->GetEarthRadiusMultiplier());
}

// =================================================================================================
//! Determine if the horizon obscures the view between a transmitter and receiver.
//!
//! @param aXmtrRcvrPtr      [input] Pointer to the transmitter/receiver object.
//! @param aPlatformPtr      [input] Pointer to the target platform.
//! @param aEarthRadiusScale [input] The Earth radius scale factor.
//! @returns 'true' if the horizon obscures the view between the objects.
// static
bool WsfEM_Interaction::MaskedByHorizon(const WsfEM_XmtrRcvr* aXmtrRcvrPtr, WsfPlatform* aPlatformPtr, double aEarthRadiusScale)
{
   double lat1;
   double lon1;
   double alt1;
   aXmtrRcvrPtr->GetAntenna()->GetLocationLLA(lat1, lon1, alt1);
   double trnHgt1 = aXmtrRcvrPtr->GetPlatform()->GetTerrainHeight();

   double lat2;
   double lon2;
   double alt2;
   aPlatformPtr->GetLocationLLA(lat2, lon2, alt2);
   double trnHgt2 = aPlatformPtr->GetTerrainHeight();

   return WsfEM_Util::MaskedByHorizon(lat1, lon1, alt1, trnHgt1, lat2, lon2, alt2, trnHgt2, aEarthRadiusScale);
}

// =================================================================================================
// Used in the Over the Horizon Case
// Checks for masking from the Xmtr/Rcvr to the Reflection Point, and then checks for Masking from the Reflection Point
// to the Target. If either case is true, then no detection can occur.
bool WsfEM_Interaction::MaskedByHorizon(const WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                                        WsfPlatform*          aPlatformPtr,
                                        double                aReflectionPointWCS[3],
                                        double                aEarthRadiusScale)
{
   double lat1;
   double lon1;
   double alt1;
   double lat2;
   double lon2;
   double alt2;

   aXmtrRcvrPtr->GetAntenna()->GetLocationLLA(lat1, lon1, alt1);
   double trnHgt1 = aXmtrRcvrPtr->GetPlatform()->GetTerrainHeight();

   aPlatformPtr->GetLocationLLA(lat2, lon2, alt2);
   double trnHgt2 = aPlatformPtr->GetTerrainHeight();

   bool maskedByHorizon = true;     // Assume at least one object is below ground
   if (((alt1 - trnHgt1) > -1.0) && // Both objects are above ground (with a little slop for safety)
       ((alt2 - trnHgt2) > -1.0))
   {
      // NOTE: The follow code is an exact copy from the other MaskedByHorizon method above.
      //       See it for comments

      // the Lat-Lon-Alt of the reflection point and check for horizon masking to each location
      double refl_lat, refl_lon, refl_alt;
      UtEntity::ConvertWCSToLLA(aReflectionPointWCS, refl_lat, refl_lon, refl_alt);

      // Xmtr/Rcvr -> Reflection Point
      maskedByHorizon =
         UtSphericalEarth::MaskedByHorizon(lat1, lon1, alt1, refl_lat, refl_lon, refl_alt, aEarthRadiusScale);
      if (maskedByHorizon)
      {
         return maskedByHorizon; // No need to check the other path
      }

      // Reflection Point -> Target
      maskedByHorizon =
         UtSphericalEarth::MaskedByHorizon(refl_lat, refl_lon, refl_alt, lat2, lon2, alt2, aEarthRadiusScale);
   }
   return maskedByHorizon;
}

// =================================================================================================
//! Does the terrain mask any part of the computation?
bool WsfEM_Interaction::MaskedByTerrain()
{
   bool maskedByTerrain = false;
   if (mRcvrPtr->CheckMasking() && mRcvrPtr->IsTerrainMaskingEnabled())
   {
      if (mTgtPtr == nullptr)
      {
         // One way interaction involving a transmitter and a receiver
         mCheckedStatus |= cRCVR_TERRAIN_MASKING;
         if (!mRcvrPtr->GetSimulation()->GetLOS_Manager()->IsTargetVisible(mRcvrPtr->GetAntenna(),
                                                                           mXmtrPtr->GetAntenna(),
                                                                           0.0,
                                                                           mEarthRadiusScale))
         {
            mFailedStatus |= cRCVR_TERRAIN_MASKING;
            maskedByTerrain = true;
         }
      }
      else
      {
         // Two way interaction (xmtr-tgt-rcvr) or a one-way rcvr-tgt interaction.
         mCheckedStatus |= cRCVR_TERRAIN_MASKING;
         if (!mRcvrPtr->GetSimulation()->GetLOS_Manager()->IsTargetVisible(mRcvrPtr->GetAntenna(), mTgtPtr, 0.0, mEarthRadiusScale))
         {
            mFailedStatus |= cRCVR_TERRAIN_MASKING;
            maskedByTerrain = true;
         }
         else if (mBistatic)
         {
            // Two-way bistatic interaction.
            mCheckedStatus |= cXMTR_TERRAIN_MASKING;
            if (!mRcvrPtr->GetSimulation()->GetLOS_Manager()->IsTargetVisible(mXmtrPtr->GetAntenna(),
                                                                              mTgtPtr,
                                                                              0.0,
                                                                              mEarthRadiusScale))
            {
               mFailedStatus |= cXMTR_TERRAIN_MASKING;
               maskedByTerrain = true;
            }
         }
      }
   }
   return maskedByTerrain;
}

// =================================================================================================
//! A convenience method to fill any un-computed geometry data.
//! This is used internally and by event output for preparing geometry data for display.
//!
//! Using this method allows interactions to be aborted early in processing (such as concealment
//! or Doppler) prior to calling Begin...WayInteraction(). Eliminating the Begin...WayInteraction
//! call can save a lot of time, but it also means that the geometry data has not be computed and
//! stored in the interaction object. By calling this from a display routine, it will ensure the
//! necessary values are computed without incurring the overhead of ALWAYS computing them even
//! when an interaction has been aborted early.
void WsfEM_Interaction::ComputeUndefinedGeometry()
{
   if (mRcvrPtr == nullptr)
   {
      return; // Must at least have a receiver.
   }

   // Compute undefined position data.

   if ((mTgtPtr != nullptr) && (!mTgtLoc.mIsValid))
   {
      mTgtPtr->GetLocationWCS(mTgtLoc.mLocWCS);
      mTgtPtr->GetLocationLLA(mTgtLoc.mLat, mTgtLoc.mLon, mTgtLoc.mAlt);
      mTgtLoc.mIsValid = true;
   }

   if ((mXmtrPtr != nullptr) && (!mXmtrLoc.mIsValid))
   {
      if (mXmtrPtr->GetAntenna() != nullptr)
      {
         mXmtrPtr->GetAntenna()->GetLocationWCS(mXmtrLoc.mLocWCS);
         mXmtrPtr->GetAntenna()->GetLocationLLA(mXmtrLoc.mLat, mXmtrLoc.mLon, mXmtrLoc.mAlt);
         mXmtrLoc.mIsValid = true;
      }
   }

   if ((mRcvrPtr != nullptr) && (!mRcvrLoc.mIsValid))
   {
      if (mRcvrPtr->GetAntenna() != nullptr)
      {
         mRcvrPtr->GetAntenna()->GetLocationWCS(mRcvrLoc.mLocWCS);
         mRcvrPtr->GetAntenna()->GetLocationLLA(mRcvrLoc.mLat, mRcvrLoc.mLon, mRcvrLoc.mAlt);
         mRcvrLoc.mIsValid = true;
      }
   }

   // Compute undefined relative geometry information.
   //
   // This checks to see if of the range and angle limits were not computed and passed. If any were not
   // computed AND passed, then ALL are recomputed. The reason that they must pass is that some of the
   // aspect angles are computed only after everything has passed. (See the Begin<>Interaction methods)

   if (mTgtPtr == nullptr)
   {
      // Should be an xmtr-rcvr interaction (receiver pointer is known to be valid, not sure about the transmitter)
      if ((mXmtrPtr != nullptr) && // Just to make sure
          mXmtrLoc.mIsValid && mRcvrLoc.mIsValid)
      {
         unsigned int limitsMask = cRCVR_RANGE_LIMITS | cRCVR_ANGLE_LIMITS | cXMTR_RANGE_LIMITS | cXMTR_ANGLE_LIMITS |
                                   cRCVR_HORIZON_MASKING | cXMTR_HORIZON_MASKING;
         if (((mCheckedStatus & limitsMask) != limitsMask) || ((mFailedStatus & limitsMask) != 0))
         {
            UtVec3d::Subtract(mRcvrToXmtr.mTrueUnitVecWCS, mXmtrLoc.mLocWCS, mRcvrLoc.mLocWCS);
            mRcvrToXmtr.mRange = UtVec3d::Normalize(mRcvrToXmtr.mTrueUnitVecWCS);
            mXmtrToRcvr.mRange = mRcvrToXmtr.mRange;
            WithinFieldOfView(mRcvrPtr->GetAntenna(), mRcvrLoc, mXmtrLoc, mRcvrToXmtr, mXmtrToRcvr, true);
            WithinFieldOfView(mXmtrPtr->GetAntenna(), mXmtrLoc, mRcvrLoc, mXmtrToRcvr, mRcvrToXmtr, true);
            mXmtrPtr->GetAntenna()->ComputeAspect(mXmtrToRcvr.mUnitVecWCS, mXmtrToRcvr.mAz, mXmtrToRcvr.mEl);
            mRcvrPtr->GetAntenna()->ComputeAspect(mRcvrToXmtr.mUnitVecWCS, mRcvrToXmtr.mAz, mRcvrToXmtr.mEl);
         }
      }
      else
      {
         mRcvrToXmtr.mRange = -1.0;
         mXmtrToRcvr.mRange = -1.0;
      }
   }
   else if (mXmtrPtr == nullptr)
   {
      // Must be a rcvr-tgt interaction (receiver and target pointers are known to be good)
      if (mRcvrLoc.mIsValid && mTgtLoc.mIsValid)
      {
         unsigned int limitsMask = cRCVR_RANGE_LIMITS | cRCVR_ANGLE_LIMITS | cRCVR_HORIZON_MASKING;
         if (((mCheckedStatus & limitsMask) != limitsMask) || ((mFailedStatus & limitsMask) != 0))
         {
            UtVec3d::Subtract(mRcvrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
            mRcvrToTgt.mRange = UtVec3d::Normalize(mRcvrToTgt.mTrueUnitVecWCS);
            mTgtToRcvr.mRange = mRcvrToTgt.mRange;
            WithinFieldOfView(mRcvrPtr->GetAntenna(), mRcvrLoc, mTgtLoc, mRcvrToTgt, mTgtToRcvr, true);
            mTgtPtr->ComputeAspect(mTgtToRcvr.mTrueUnitVecWCS, mTgtToRcvr.mTrueAz, mTgtToRcvr.mTrueEl);
            mTgtPtr->ComputeAspect(mTgtToRcvr.mUnitVecWCS, mTgtToRcvr.mAz, mTgtToRcvr.mEl);
         }
      }
      else
      {
         mRcvrToTgt.mRange = -1.0;
         mTgtToRcvr.mRange = -1.0;
      }
   }
   else
   {
      // Must be an xmtr-tgt-rcvr interaction (transmitter, target and receiver pointers are known to be good).
      if (mXmtrLoc.mIsValid && mTgtLoc.mIsValid && mRcvrLoc.mIsValid)
      {
         unsigned int limitsMask = cRCVR_RANGE_LIMITS | cRCVR_ANGLE_LIMITS | cXMTR_RANGE_LIMITS | cXMTR_ANGLE_LIMITS |
                                   cRCVR_HORIZON_MASKING | cXMTR_HORIZON_MASKING;
         if (((mCheckedStatus & limitsMask) != limitsMask) || ((mFailedStatus & limitsMask) != 0))
         {
            UtVec3d::Subtract(mRcvrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
            mRcvrToTgt.mRange = UtVec3d::Normalize(mRcvrToTgt.mTrueUnitVecWCS);
            mTgtToRcvr.mRange = mRcvrToTgt.mRange;
            if (mBistatic)
            {
               UtVec3d::Subtract(mXmtrToTgt.mTrueUnitVecWCS, mTgtLoc.mLocWCS, mXmtrLoc.mLocWCS);
               mXmtrToTgt.mRange = UtVec3d::Normalize(mXmtrToTgt.mTrueUnitVecWCS);
               mTgtToXmtr.mRange = mXmtrToTgt.mRange;
            }

            WithinFieldOfView(mRcvrPtr->GetAntenna(), mRcvrLoc, mTgtLoc, mRcvrToTgt, mTgtToRcvr, true);
            mTgtPtr->ComputeAspect(mTgtToRcvr.mTrueUnitVecWCS, mTgtToRcvr.mTrueAz, mTgtToRcvr.mTrueEl);
            mTgtPtr->ComputeAspect(mTgtToRcvr.mUnitVecWCS, mTgtToRcvr.mAz, mTgtToRcvr.mEl);
            if (mBistatic)
            {
               WithinFieldOfView(mXmtrPtr->GetAntenna(), mXmtrLoc, mTgtLoc, mXmtrToTgt, mTgtToXmtr, true);
               mTgtPtr->ComputeAspect(mTgtToXmtr.mTrueUnitVecWCS, mTgtToXmtr.mTrueAz, mTgtToXmtr.mTrueEl);
               mTgtPtr->ComputeAspect(mTgtToXmtr.mUnitVecWCS, mTgtToXmtr.mAz, mTgtToXmtr.mEl);
            }
            else
            {
               mXmtrToTgt = mRcvrToTgt;
               mTgtToXmtr = mTgtToRcvr;
            }
         }
      }
      else
      {
         mXmtrToTgt.mRange = -1.0;
         mTgtToXmtr.mRange = -1.0;
         mRcvrToTgt.mRange = -1.0;
         mTgtToRcvr.mRange = -1.0;
      }
   }
}

// =================================================================================================
//! Get the Doppler speed of the target relative to the receiver.
//! This computes the relative speed of the target along the range vector from the receiver to the target.
//! @param aFilterOwnshipFromDoppler [input] Flag to indicate if ownship Doppler is filtered.
//! @return The Doppler speed of the target relative to the receiver. The value is
//! negative if the target is closing and positive when receding.
double WsfEM_Interaction::ComputeTargetDopplerSpeed(bool aFilterOwnshipFromDoppler)
{
   // Get the relative WCS location of the target with respect to the sensor (L_t - L_s).
   double relTgtLocWCS[3];
   // mRcvrPtr->GetAntenna()->GetRelativeLocationWCS(mTgtPtr, relTgtLocWCS);
   UtVec3d::Subtract(relTgtLocWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);

   // Get the target Doppler WCS velocity.
   // This will be either the raw target velocity (if 'filtered_doppler_speed') or the target velocity
   // relative to the sensor ('unfiltered_doppler_speed').
   double relTgtVelWCS[3];
   mTgtPtr->GetVelocityWCS(relTgtVelWCS);
   if (!aFilterOwnshipFromDoppler)
   {
      // Get the sensor velocity.
      double snrVelWCS[3];
      mRcvrPtr->GetPlatform()->GetVelocityWCS(snrVelWCS);
      // Compute the relative WCS velocity (V_t - V_s)
      UtVec3d::Subtract(relTgtVelWCS, relTgtVelWCS, snrVelWCS);
   }

   // Compute the magnitude of the velocity vector along the relative position vector.
   // This is the Doppler speed.
   double dopplerSpeed = UtVec3d::DotProduct(relTgtVelWCS, relTgtLocWCS);
   dopplerSpeed /= std::max(UtVec3d::Magnitude(relTgtLocWCS), 1.0);
   return dopplerSpeed;
}

// ================================================================================================
//! Compute the Target Doppler frequency for this interaction.
//! @param aFilterOwnshipFromDoppler [input] Flag to indicate if ownship Doppler is filtered.
//! @return The true Doppler frequency of the target (may be negative).
// public
double WsfEM_Interaction::ComputeTargetDopplerFrequency(bool aFilterOwnshipFromDoppler)
{
   double fDoppler(0.0);
   if (mTgtPtr != nullptr)
   {
      if ((!mTgtLoc.mIsValid) || (!mRcvrLoc.mIsValid) || !(mXmtrToTgt.mRange > 0.0))
      {
         ComputeUndefinedGeometry();
      }
      double relTgtLocWCS[3];
      UtVec3d::Subtract(relTgtLocWCS, mTgtLoc.mLocWCS, mRcvrLoc.mLocWCS);
      double relTgtVelWCS[3];
      mTgtPtr->GetVelocityWCS(relTgtVelWCS);
      if (!aFilterOwnshipFromDoppler)
      {
         // Get the sensor velocity.
         double snrVelWCS[3];
         mRcvrPtr->GetPlatform()->GetVelocityWCS(snrVelWCS);
         // Compute the relative WCS velocity (V_t - V_s)
         UtVec3d::Subtract(relTgtVelWCS, relTgtVelWCS, snrVelWCS);
      }

      // Compute the magnitude of the velocity vector along the relative position vector.
      // This is the Doppler speed.
      double dopplerSpeed = UtVec3d::DotProduct(relTgtVelWCS, relTgtLocWCS);
      dopplerSpeed /= std::max(UtVec3d::Magnitude(relTgtLocWCS), 1.0);

      // Convert to frequency shift
      fDoppler = (-2.0 * dopplerSpeed) / (UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency());
   }
   return fDoppler;
}

// =================================================================================================
//! Print the interaction data to standard output.
// virtual
void WsfEM_Interaction::Print(ut::log::MessageStream& aMsgStream)
{
   { // RAII block
      auto note = aMsgStream.AddNote() << "Geometry:";
      PrintGeometry(note);
   }
   if (mRadarSig > 0.0)
   {
      auto note = aMsgStream.AddNote() << "Radar_Sig: " << UtMath::LinearToDB(mRadarSig) << " dBsm (" << mRadarSig
                                       << " m^2)";
      note.AddNote() << "Az: " << PrintableAngle(mRadarSigAz) << " deg";
      note.AddNote() << "El: " << PrintableAngle(mRadarSigEl) << " deg";
   }

   if (mOpticalSig > 0.0)
   {
      auto note = aMsgStream.AddNote() << "Optical_Sig: " << UtMath::LinearToDB(mOpticalSig) << " dBsm (" << mOpticalSig
                                       << " m^2)";
      note.AddNote() << "Az: " << PrintableAngle(mOpticalSigAz) << " deg";
      note.AddNote() << "El: " << PrintableAngle(mOpticalSigEl) << " deg";
   }

   if (mInfraredSig > 0.0)
   {
      auto note = aMsgStream.AddNote() << "Infrared_Sig: " << mInfraredSig << " w/sr";
      note.AddNote() << "Az: " << PrintableAngle(mInfraredSigAz) << " deg";
      note.AddNote() << "El: " << PrintableAngle(mInfraredSigEl) << " deg";
      if (mSignalToNoise > 0.0)
      {
         note.AddNote() << "Background_radiant_intensity: " << mBackgroundRadiantIntensity << " w/sr";
         note.AddNote() << "Contrast_radiant_intensity: " << mContrastRadiantIntensity << " w/sr";
      }
   }

   bool printMaskingFactor = ((mMaskingFactor >= 0.0) && (mMaskingFactor < 1.0));
   if ((mAbsorptionFactor > 0.0) || (mPropagationFactor > 0.0) || printMaskingFactor)
   {
      if (mAbsorptionFactor > 0.0)
      {
         aMsgStream.AddNote() << "Absorption_factor: " << UtMath::LinearToDB(mAbsorptionFactor) << " dB ("
                              << mAbsorptionFactor << ")";
      }
      if (mPropagationFactor > 0.0)
      {
         auto note = aMsgStream.AddNote() << "Propagation_factor_F^4: " << UtMath::LinearToDB(mPropagationFactor)
                                          << " dB (" << mPropagationFactor << ")";
         note.AddNote() << "F: " << pow(mPropagationFactor, 0.25);
      }
      if (printMaskingFactor)
      {
         aMsgStream.AddNote() << "Masking_Factor: " << mMaskingFactor;
      }
   }

   if ((mXmtdPower > 0.0) || (mRcvdPower > 0.0) || (mPixelCount > 0.0) || (mSignalToNoise > 0.0))
   {
      if (mXmtdPower > 0.0)
      {
         aMsgStream.AddNote() << "Xmtd_Power: " << UtMath::LinearToDB(mXmtdPower) << " dBw";
      }
      if (mRcvdPower > 0.0)
      {
         aMsgStream.AddNote() << "Rcvd_Power: " << UtMath::LinearToDB(mRcvdPower) << " dBw";
      }
      if (mRcvrNoisePower > 0.0)
      {
         aMsgStream.AddNote() << "Rcvr_Noise: " << UtMath::LinearToDB(mRcvrNoisePower) << " dBw";
      }
      if (mClutterPower > 0.0)
      {
         aMsgStream.AddNote() << "Clutter_Power: " << UtMath::LinearToDB(mClutterPower) << " dBw";
      }
      if (mInterferencePower > 0.0)
      {
         aMsgStream.AddNote() << "Interference_Power: " << UtMath::LinearToDB(mInterferencePower) << " dBw";
      }
      if (mPixelCount > 0.0)
      {
         aMsgStream.AddNote() << "Pixel_Count: " << mPixelCount;
      }

      if (mSignalToNoise > 0.0)
      {
         aMsgStream.AddNote() << "S/I: " << UtMath::LinearToDB(mSignalToNoise) << " dB";
         if (mDetectionThreshold > 0.0)
         {
            aMsgStream.AddNote() << "Threshold: " << UtMath::LinearToDB(mDetectionThreshold) << " dB";
         }
         if ((mRcvdPower > 0.0) && (mRcvrNoisePower > 0.0))
         {
            double noisePower   = mRcvrNoisePower;
            double clutterPower = std::max(mClutterPower, 0.0);
            double intfPower    = std::max(mInterferencePower, 0.0);
            // Signal power used to be mRcvdPower, but that didn't always contain all the parts that were
            // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
            double signalPower = mSignalToNoise * (noisePower + clutterPower + intfPower);
            aMsgStream.AddNote() << "S/N: " << UtMath::LinearToDB(signalPower / noisePower) << " dB";
            aMsgStream.AddNote() << "S/(N+C): " << UtMath::LinearToDB(signalPower / (noisePower + clutterPower)) << " dB";
            aMsgStream.AddNote() << "S/(N+C+I): "
                                 << UtMath::LinearToDB(signalPower / (noisePower + clutterPower + intfPower)) << " dB";
         }
      }
   }

   if (mComponents.HasComponents())
   {
      for (auto component : mComponents)
      {
         auto note = aMsgStream.AddNote() << "Component: " << component->GetComponentName();
         component->Print(note, *this);
      }
   }

   PrintStatus(aMsgStream);

   if (mComponents.HasComponents())
   {
      for (auto component : mComponents)
      {
         auto note = aMsgStream.AddNote() << "Component: " << component->GetComponentName();
         component->PrintStatus(aMsgStream, *this);
      }
   }
}

// =================================================================================================
//! Print the geometry portions of the output provided by Print().
//! This method is provided so models that want to provide a custom debug print method can still
//! produce the standard geometry printing provided Print().
// public
void WsfEM_Interaction::PrintGeometry(ut::log::MessageStream& aMsgStream)
{
   ComputeUndefinedGeometry();
   if (mTgtPtr == nullptr)
   {
      // One way operation involving a transmitter and a receiver.
      if ((mXmtrPtr != nullptr) && (mRcvrPtr != nullptr))
      {
         PrintAbsXmtrRcvrData(aMsgStream, "Xmtr:", GetTransmitter(), mXmtrLoc);
         PrintAbsXmtrRcvrData(aMsgStream, "Rcvr:", GetReceiver(), mRcvrLoc);
         PrintRelXmtrRcvrData(aMsgStream, "Xmtr->Rcvr:", GetTransmitter(), mXmtrToRcvr);
         PrintRelXmtrRcvrData(aMsgStream, "Rcvr->Xmtr:", GetReceiver(), mRcvrToXmtr);
         PrintBeamData(aMsgStream, "XmtrBeam:", GetTransmitter(), mXmtrBeam);
         PrintBeamData(aMsgStream, "RcvrBeam:", GetReceiver(), mRcvrBeam);
      }
   }
   else if (mXmtrPtr == nullptr)
   {
      // One way operation involving a receiver and a target
      if (mRcvrPtr != nullptr)
      {
         PrintAbsXmtrRcvrData(aMsgStream, "Rcvr:", GetReceiver(), mRcvrLoc);
         PrintAbsTargetData(aMsgStream, "Tgt:", GetTarget(), mTgtLoc);
         PrintRelXmtrRcvrData(aMsgStream, "Rcvr->Tgt:", GetReceiver(), mRcvrToTgt);
         PrintRelTargetData(aMsgStream, "Tgt->Rcvr:", GetTarget(), mTgtToRcvr);
         PrintBeamData(aMsgStream, "RcvrBeam:", GetReceiver(), mRcvrBeam);
      }
   }
   else if (mRcvrPtr != nullptr)
   {
      // Two way operation involving a transmitter, target and a receiver.
      if (mBistatic)
      {
         PrintAbsXmtrRcvrData(aMsgStream, "Xmtr:", GetTransmitter(), mXmtrLoc);
         PrintAbsXmtrRcvrData(aMsgStream, "Rcvr:", GetReceiver(), mRcvrLoc);
         PrintAbsTargetData(aMsgStream, "Tgt:", GetTarget(), mTgtLoc);
         PrintRelXmtrRcvrData(aMsgStream, "Xmtr->Tgt:", GetTransmitter(), mXmtrToTgt);
         PrintRelTargetData(aMsgStream, "Tgt->Xmtr: ", GetTarget(), mTgtToXmtr);
         PrintRelXmtrRcvrData(aMsgStream, "Rcvr->Tgt: ", GetReceiver(), mRcvrToTgt);
         PrintRelTargetData(aMsgStream, "Tgt->Rcvr: ", GetTarget(), mTgtToRcvr);
      }
      else
      {
         PrintAbsXmtrRcvrData(aMsgStream, "Xmtr/Rcvr:", GetReceiver(), mRcvrLoc);
         PrintAbsTargetData(aMsgStream, "Tgt:", GetTarget(), mTgtLoc);
         PrintRelXmtrRcvrData(aMsgStream, "Xmtr/Rcvr->Tgt:", GetReceiver(), mRcvrToTgt);
         PrintRelTargetData(aMsgStream, "Tgt->Xmtr/Rcvr:", GetTarget(), mTgtToRcvr);
      }
      PrintBeamData(aMsgStream, "XmtrBeam:", GetTransmitter(), mXmtrBeam);
      PrintBeamData(aMsgStream, "RcvrBeam:", GetReceiver(), mRcvrBeam);
   }
}

void WsfEM_Interaction::PrintStatus(ut::log::MessageStream& aMsgStream) const
{
   if (mFailedStatus == 0)
   {
      aMsgStream.AddNote() << "Status: Normal";
   }
   else
   {
      auto note = aMsgStream.AddNote() << "Status:";
      PrintStatusP(note);
   }
}

// =================================================================================================
//! Print the failed portions of debug print data provided by Print().
//! This method is provided so models that want to provide a custom debug print method can still
//! produce the standard geometry printing provided Print().
// public
void WsfEM_Interaction::PrintStatusP(ut::log::MessageStream& aMsgStream) const
{
   // part of the larger virtual Print() and PrintStatus is itself virtual
   // not sure how to make atomic with the Print that calls this.
   // for now each will get its own debug tag

   if ((mFailedStatus & cXMTR_RANGE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Xmtr_Range_Limits_Exceeded";
   }
   if ((mFailedStatus & cRCVR_RANGE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Rcvr_Range_Limits_Exceeded";
   }
   if ((mFailedStatus & cXMTR_ALTITUDE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Xmtr_Altitude_Limits_Exceeded";
   }
   if ((mFailedStatus & cRCVR_ALTITUDE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Rcvr_Altitude_Limits_Exceeded";
   }
   if ((mFailedStatus & cXMTR_ANGLE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Xmtr_Angle_Limits_Exceeded";
   }
   if ((mFailedStatus & cRCVR_ANGLE_LIMITS) != 0)
   {
      aMsgStream.AddNote() << "Rcvr_Angle_Limits_Exceeded";
   }
   if ((mFailedStatus & cXMTR_HORIZON_MASKING) != 0)
   {
      aMsgStream.AddNote() << "Xmtr_Masked_By_Horizon";
   }
   if ((mFailedStatus & cRCVR_HORIZON_MASKING) != 0)
   {
      aMsgStream.AddNote() << "Rcvr_Masked_By_Horizon";
   }
   if ((mFailedStatus & cXMTR_TERRAIN_MASKING) != 0)
   {
      aMsgStream.AddNote() << "Xmtr_Masked_By_Terrain";
   }
   if ((mFailedStatus & cRCVR_TERRAIN_MASKING) != 0)
   {
      aMsgStream.AddNote() << "Rcvr_Masked_By_Terrain";
   }
   if ((mFailedStatus & cSIGNAL_LEVEL) != 0)
   {
      aMsgStream.AddNote() << "Insufficient_Signal";
   }
}

// =================================================================================================
//! A convenience method to return a printable angle.
// public
double WsfEM_Interaction::PrintableAngle(double aAngle) const
{
   double angle = aAngle * UtMath::cDEG_PER_RAD;
   if (fabs(aAngle) < 1.0E-12)
   {
      angle = 0.0;
   }
   return angle;
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintAbsTargetData(ut::log::MessageStream& aMsgStream,
                                           const char*             aPrefix,
                                           WsfPlatform*            aTgtPtr,
                                           const LocationData&     aLocData)
{
   PrintLocationData(aMsgStream, aPrefix, aTgtPtr, aLocData);
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintRelTargetData(ut::log::MessageStream& aMsgStream,
                                           const char*             aPrefix,
                                           WsfPlatform*            aTgtPtr,
                                           const RelativeData&     aRelData)
{
   auto out = aMsgStream.AddNote() << aPrefix;
   if (aRelData.mRange > 0.0)
   {
      PrintRange(out, aRelData.mRange);
      double otherLocNED[3];
      aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mTrueUnitVecWCS);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      out.AddNote() << "Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
      out.AddNote() << "El: " << PrintableAngle(otherElNED) << " deg";
      if (mEarthRadiusScale != 1.0)
      {
         aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mUnitVecWCS);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         auto note   = out.AddNote() << "Apparent:";
         note.AddNote() << "Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
         note.AddNote() << "El: " << PrintableAngle(otherElNED) << " deg";
      }
   }
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintAbsXmtrRcvrData(ut::log::MessageStream& aMsgStream,
                                             const char*             aPrefix,
                                             WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                                             const LocationData&     aLocData)
{
   PrintLocationData(aMsgStream, aPrefix, aXmtrRcvrPtr->GetPlatform(), aLocData);
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintRelXmtrRcvrData(ut::log::MessageStream& aMsgStream,
                                             const char*             aPrefix,
                                             WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                                             const RelativeData&     aRelData)
{
   auto out = aMsgStream.AddNote() << aPrefix;
   if (aRelData.mRange > 0.0)
   {
      PrintRange(out, aRelData.mRange);
      double otherLocNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mTrueUnitVecWCS, otherLocNED);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      out.AddNote() << "Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
      out.AddNote() << "El: " << PrintableAngle(otherElNED) << " deg";
      if (mEarthRadiusScale != 1.0)
      {
         aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mUnitVecWCS, otherLocNED);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         auto note   = out.AddNote() << "Apparent:";
         note.AddNote() << "Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
         note.AddNote() << "El: " << PrintableAngle(otherElNED) << " deg";
      }
   }
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintBeamData(ut::log::MessageStream& aMsgStream,
                                      const char*             aPrefix,
                                      WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                                      const BeamData&         aBeamData)
{
   auto out = aMsgStream.AddNote() << aPrefix;
   if (aBeamData.mGain >= 0.0)
   {
      double beamPointNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aBeamData.mWCS_ToBeamTransform[0], beamPointNED);
      double magnitudeNE = sqrt(beamPointNED[0] * beamPointNED[0] + beamPointNED[1] * beamPointNED[1]);
      double beamPointAz = atan2(beamPointNED[1], beamPointNED[0]);
      double beamPointEl = atan2(-beamPointNED[2], magnitudeNE);
      out.AddNote() << "Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(beamPointAz)) << " deg";
      out.AddNote() << "El: " << PrintableAngle(beamPointEl) << " deg";
      { // RAII block
         auto note = out.AddNote() << "TgtRel";
         note.AddNote() << "Az: " << PrintableAngle(aBeamData.mAz) << " deg";
         note.AddNote() << "El: " << PrintableAngle(aBeamData.mEl) << " deg";
      }
      if (aBeamData.mGain > 0.0)
      {
         if (aXmtrRcvrPtr->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
         {
            double cosTheta = cos(aBeamData.mEBS_Az) * cos(aBeamData.mEBS_El);
            cosTheta        = std::max(std::min(cosTheta, 1.0), -1.0); // For safety
            out.AddNote() << "EBS Az: " << PrintableAngle(aBeamData.mEBS_Az) << " deg";
            out.AddNote() << "El: " << PrintableAngle(aBeamData.mEBS_El) << " deg";
            out.AddNote() << "Omega: " << PrintableAngle(acos(cosTheta)) << " deg";
         }
         out.AddNote() << "Gain: " << UtMath::LinearToDB(aBeamData.mGain) << " dB";
      }
   }
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintLocationData(ut::log::MessageStream& aMsgStream,
                                          const char*             aPrefix,
                                          WsfPlatform*            aPlatformPtr,
                                          const LocationData&     aLocationData) const
{
   double lat;
   double lon;
   double alt;

   auto out = aMsgStream.AddNote() << aPrefix << std::setprecision(8);
   if (aLocationData.mIsValid)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(aLocationData.mLocWCS, lat, lon, alt);
   }
   else
   {
      aPlatformPtr->GetLocationLLA(lat, lon, alt);
   }

   out.AddNote() << "Lat: " << UtLatPos(lat);
   out.AddNote() << "Lon: " << UtLonPos(lon);
   out.AddNote() << "Alt: " << alt << " m";

   double yaw;
   double pitch;
   double roll;
   aPlatformPtr->GetOrientationNED(yaw, pitch, roll);
   out.AddNote() << "Heading: " << PrintableAngle(yaw) << " deg";
   out.AddNote() << "Pitch: " << PrintableAngle(pitch) << " deg";
   out.AddNote() << "Roll: " << PrintableAngle(roll) << " deg";
   out.AddNote() << "Speed: " << aPlatformPtr->GetSpeed() << " m/s";
}

// =================================================================================================
// private
void WsfEM_Interaction::PrintRange(ut::log::MessageStream& aMsgStream, double aRange) const
{
   aMsgStream.AddNote() << "Range: " << aRange * 0.001 << " km (" << aRange / UtMath::cM_PER_NM << " nm)";
}

// =================================================================================================
//! Reset the interaction data so it can be reused.
// virtual
void WsfEM_Interaction::Reset()
{
   mCheckedStatus        = 0;
   mFailedStatus         = 0;
   mBistatic             = false;
   mXmtrLoc.mIsValid     = false;
   mRcvrLoc.mIsValid     = false;
   mTgtLoc.mIsValid      = false;
   mRcvrToXmtr.mRange    = -1.0;
   mXmtrToRcvr.mRange    = -1.0;
   mRcvrToTgt.mRange     = -1.0;
   mTgtToRcvr.mRange     = -1.0;
   mXmtrToTgt.mRange     = -1.0;
   mTgtToXmtr.mRange     = -1.0;
   mRcvrBeam.mGain       = -1.0;
   mXmtrBeam.mGain       = -1.0;
   mMaskingFactor        = 1.0;
   mInfraredSig          = -1.0;
   mRadarSig             = -1.0;
   mOpticalSig           = -1.0;
   mOpticalReflectivity  = -1.0;
   mXmtdPower            = 0.0;
   mPowerDensityAtTarget = 0.0;
   mRcvdPower            = 0.0;
   mRcvrNoisePower       = 0.0;
   mClutterPower         = 0.0;
   mInterferencePower    = 0.0;
   mInterferenceFactor   = 0.0;
   mSignalToNoise        = 0.0;
   mPropagationFactor    = 0.0;
   mAbsorptionFactor     = 0.0;
   mDetectionThreshold   = 0.0;
   mPixelCount           = 0.0;
   mCategoryId           = nullptr;
   mZoneAttenuationValue = 0.0;
   mXmtrPtr              = nullptr;
   mRcvrPtr              = nullptr;
   mTgtPtr               = nullptr;

   for (auto component : mComponents)
   {
      component->Reset();
   }
}

// =================================================================================================
//! Attempt to point the receiver antenna at the target.
//!
//! This routine will position the receiver antenna.  If the antenna has been cued then
//! it will be pointed at the cued location.  If it has not been cued then the
//! method will attempt to point it right at the target, subject to the limitations
//! of the antenna.
//!
//! @note This can be called only after Begin...Interaction().
void WsfEM_Interaction::SetReceiverBeamPosition()
{
   assert(mRcvrPtr != nullptr);
   WsfEM_Antenna* antennaPtr = mRcvrPtr->GetAntenna();

   if (mTgtPtr != nullptr)
   {
      // xmtr-tgt-rcvr or rcvr-tgt interaction
      antennaPtr->ComputeBeamPosition(mRcvrPtr,
                                      mRcvrToTgt.mAz,
                                      mRcvrToTgt.mEl,
                                      mRcvrBeam.mWCS_ToBeamTransform,
                                      mRcvrBeam.mEBS_Az,
                                      mRcvrBeam.mEBS_El);
   }
   else if (mXmtrPtr != nullptr)
   {
      // rcvr-xmtr interaction
      antennaPtr->ComputeBeamPosition(mRcvrPtr,
                                      mRcvrToXmtr.mAz,
                                      mRcvrToXmtr.mEl,
                                      mRcvrBeam.mWCS_ToBeamTransform,
                                      mRcvrBeam.mEBS_Az,
                                      mRcvrBeam.mEBS_El);
   }
   else
   {
      assert((mTgtPtr != nullptr) || (mXmtrPtr != nullptr));
   }
   ComputeReceiverBeamAspect();
}

// =================================================================================================
//! Set the receiver beam position to a defined position.
//!
//! This is typically used in interference interactions to copy a beam position from another
//! interaction into the interaction that is being used to compute the interference effect.
//!
//! @param aBeamData [input] The transformation matrix that defines the beam position on the receiver.
//!
//! @note This can be called only after a successful Begin...Interaction() and with valid aBeamData
//!       (i.e. aBeamData.mGain > 0.0).
void WsfEM_Interaction::SetReceiverBeamPosition(const BeamData& aBeamData)
{
   if (aBeamData.mGain < 0.0)
   {
      ut::log::error() << "WsfEM_Interaction::SetReceiverBeamPosition: Uninitialized receiver Beam Data found.";
      assert(false);
   }
   UtMat3d::Set(mRcvrBeam.mWCS_ToBeamTransform, aBeamData.mWCS_ToBeamTransform);
   mRcvrBeam.mEBS_Az = aBeamData.mEBS_Az;
   mRcvrBeam.mEBS_El = aBeamData.mEBS_El;
   ComputeReceiverBeamAspect();
}

// =================================================================================================
//! Attempt to point the transmit antenna at the target.
//!
//! This routine will position the transmit antenna.  If the antenna has been cued then
//! it will be pointed at the cued location.  If it has not been cued then the
//! method will attempt to point it right at the target, subject to the limitations
//! of the antenna.
//!
//! @note This can be called only after Begin...Interaction().
void WsfEM_Interaction::SetTransmitterBeamPosition()
{
   assert(mXmtrPtr != nullptr);
   WsfEM_Antenna* antennaPtr = mXmtrPtr->GetAntenna();

   if (mTgtPtr != nullptr)
   {
      // xmtr-tgt-rcvr interaction (monostatic or bistatic)
      antennaPtr->ComputeBeamPosition(mXmtrPtr,
                                      mXmtrToTgt.mAz,
                                      mXmtrToTgt.mEl,
                                      mXmtrBeam.mWCS_ToBeamTransform,
                                      mXmtrBeam.mEBS_Az,
                                      mXmtrBeam.mEBS_El);
   }
   else if (mRcvrPtr != nullptr)
   {
      // rcvr-xmtr interaction
      antennaPtr->ComputeBeamPosition(mXmtrPtr,
                                      mXmtrToRcvr.mAz,
                                      mXmtrToRcvr.mEl,
                                      mXmtrBeam.mWCS_ToBeamTransform,
                                      mXmtrBeam.mEBS_Az,
                                      mXmtrBeam.mEBS_El);
   }
   else
   {
      assert((mTgtPtr != nullptr) || (mRcvrPtr != nullptr));
   }
   ComputeTransmitterBeamAspect();
}

// =================================================================================================
//! Set the transmitter beam position to a defined position.
//!
//! This is typically used in passive-type interactions to copy the beam position from the transmitter in
//! another interaction (such as a radar or interference interaction) to the transmitter beam position
//! in the current interaction.
//!
//! @param aBeamData [input] The transformation matrix that defines the beam position on the receiver.
//!
//! @note This can be called only after a successful Begin...Interaction() and with valid aBeamData
//!       (i.e. aBeamData.mGain > 0.0).
void WsfEM_Interaction::SetTransmitterBeamPosition(const BeamData& aBeamData)
{
   if (aBeamData.mGain < 0.0)
   {
      ut::log::error() << "WsfEM_Interaction::SetTransmitterBeamPosition: Uninitialized transmitter Beam Data found.";
      assert(false);
   }
   UtMat3d::Set(mXmtrBeam.mWCS_ToBeamTransform, aBeamData.mWCS_ToBeamTransform);
   mXmtrBeam.mEBS_Az = aBeamData.mEBS_Az;
   mXmtrBeam.mEBS_El = aBeamData.mEBS_El;
   ComputeTransmitterBeamAspect();
}

// =================================================================================================
//! Compute the pattern propagation factor.
// private
void WsfEM_Interaction::ComputeRF_PropagationFactor()
{
   mPropagationFactor = 1.0;
   if ((mXmtrPtr != nullptr) && (mXmtrPtr->GetPropagationModel() != nullptr) &&
       (mRcvrPtr != nullptr) && // Not yet supported for monostatic interactions
       (!mBistatic))            // Not yet supported for bistatic interactions
   {
      mPropagationFactor =
         mXmtrPtr->GetPropagationModel()->ComputePropagationFactor(*this,
                                                                   mXmtrPtr->GetPlatform()->GetScenario().GetEnvironment());
   }
}

// =================================================================================================
//! Compute the aspect of the 'target' with respect to the receiver.
//!
//! This routine is called by SetReceiverBeamPosition() and ComputeRF_ReceivedPower().
// private
void WsfEM_Interaction::ComputeReceiverBeamAspect()
{
   assert(mRcvrPtr != nullptr);
   WsfEM_Antenna* antennaPtr = mRcvrPtr->GetAntenna();
   if (mTgtPtr != nullptr)
   {
      // xmtr-tgt-rcvr or rcvr-tgt interaction
      antennaPtr->ComputeBeamAspect(mRcvrBeam.mWCS_ToBeamTransform, mRcvrToTgt.mUnitVecWCS, mRcvrBeam.mAz, mRcvrBeam.mEl);
   }
   else if (mXmtrPtr != nullptr)
   {
      // rcvr-xmtr interaction
      antennaPtr->ComputeBeamAspect(mRcvrBeam.mWCS_ToBeamTransform, mRcvrToXmtr.mUnitVecWCS, mRcvrBeam.mAz, mRcvrBeam.mEl);
   }
   else
   {
      assert((mTgtPtr != nullptr) || (mXmtrPtr != nullptr));
      mRcvrBeam.mAz = 0.0;
      mRcvrBeam.mEl = 0.0;
   }
   mRcvrBeam.mGain = 0.0; // Indicate aspect has been computed.
}

// =================================================================================================
//! Compute the aspect of the target object with respect to the current transmitter beam position.
//!
//! This routine is called by SetTransmitterBeamPosition() and ComputeRF_TransmittedPower().
// private
void WsfEM_Interaction::ComputeTransmitterBeamAspect()
{
   assert(mXmtrPtr != nullptr);
   WsfEM_Antenna* antennaPtr = mXmtrPtr->GetAntenna();
   if (mTgtPtr != nullptr)
   {
      // xmtr-tgt-rcvr interaction (monostatic or bistatic)
      antennaPtr->ComputeBeamAspect(mXmtrBeam.mWCS_ToBeamTransform, mXmtrToTgt.mUnitVecWCS, mXmtrBeam.mAz, mXmtrBeam.mEl);
   }
   else if (mRcvrPtr != nullptr)
   {
      // rcvr-xmtr interaction
      antennaPtr->ComputeBeamAspect(mXmtrBeam.mWCS_ToBeamTransform, mXmtrToRcvr.mUnitVecWCS, mXmtrBeam.mAz, mXmtrBeam.mEl);
   }
   else
   {
      assert((mTgtPtr != nullptr) || (mRcvrPtr != nullptr));
      mXmtrBeam.mAz = 0.0;
      mXmtrBeam.mEl = 0.0;
   }
   mXmtrBeam.mGain = 0.0; // Indicate aspect has been computed.
}

// =================================================================================================
void WsfEM_Interaction::ComputeZoneAttenuation(WsfPlatform* aFromPtr, WsfPlatform* aTargetPtr, bool aIsTwoWay)
{
   // make sure we have valid platforms
   if (aTargetPtr == nullptr || aFromPtr == nullptr)
   {
      return;
   }
   // check to see if line of sight collides with any zones..
   double myPlatLocLLA[3];
   double targetLocLLA[3];
   aTargetPtr->GetLocationLLA(targetLocLLA[0], targetLocLLA[1], targetLocLLA[2]);
   aFromPtr->GetLocationLLA(myPlatLocLLA[0], myPlatLocLLA[1], myPlatLocLLA[2]);

   mZoneAttenuationValue =
      mRcvrPtr->GetSimulation()->GetZoneAttenuation().ComputeAttenuation(mCategoryId, myPlatLocLLA, targetLocLLA, aIsTwoWay);
}

// =================================================================================================
//! Determine if a target is within the field of view.
//!
//! This routine does several things:
//! - Computes the true aspect angles of the target with respect to the source.
//! - Computes the apparent aspect angles of the target with respect to the source.
//! - Checks to see if the apparent target position is within the field-of-view of the source.
//! - Computes the true aspect unit vector of the source with respect to the target.
//! - Computes the apparent aspect unit vector of the source with respect to the target.
//!
//! The latter two are performed only if it is determined that the target is in the
//! field-of-view of the source or if the 'aIgnoreLimits=true' is specified. The need for
//! this kludge is driven by the need to reduce re-computation of values.
//!
//! @param aSrcPtr   [input] Pointer to the source antenna.
//! @param aSrcLoc   [input] Source location data (WCS and LLA must be valid)
//! @param aTgtLoc   [input] Target location data (WCS and LLA must be valid)
//! @param aSrcToTgt Source-to-target relative data.
//!                  [input]  mTrueUnitVec WCS must be valid.
//!                  [output] Unit vectors and angles computed.
//! @param aTgtToSrc Target-to-source relative data. This unit vectors will be computed only
//! if the target is within the field-of-view of the source
//!                  or if aIgnoreLimits == true.
//!                  The caller is always required to compute the angles from the unit vectors
//!                  because the target orientation is not known and it would require two
//!                  routines to handle a targets of type WsfPlatform and WsfEM_Antenna.
//! @param aIgnoreLimits [input] If 'true' then the check of the field of view limits
//!                      is not performed.  This is used to simply force the computation
//!                      of the aTgtToSrc values.  This is needed by passive sensors and Interference
//!                      because the angles are needed but the limits should not be checked.
// private
bool WsfEM_Interaction::WithinFieldOfView(WsfEM_Antenna* aSrcPtr,
                                          LocationData&  aSrcLoc,
                                          LocationData&  aTgtLoc,
                                          RelativeData&  aSrcToTgt,
                                          RelativeData&  aTgtToSrc,
                                          bool           aIgnoreLimits /* = false */)
{
   bool withinFieldOfView = false;
   bool useGeometricView  = true;

   // Compute the true (geometric) azimuth and elevation of the target WRT the source.
   aSrcPtr->ComputeAspect(aSrcToTgt.mTrueUnitVecWCS, aSrcToTgt.mTrueAz, aSrcToTgt.mTrueEl);

   // Compute the true unit vector from the target to the source (just the reverse of the source to target unit vector).
   UtVec3d::Multiply(aTgtToSrc.mTrueUnitVecWCS, aSrcToTgt.mTrueUnitVecWCS, -1.0);

   if (mEarthRadiusScale != 1.0)
   {
      // Account for atmospheric refraction by determining the unit vectors that point to the 'apparent' locations.
      double apparentSrcLocWCS[3];
      double apparentTgtLocWCS[3];
      bool   useApparentView = WsfEM_Util::ComputeApparentPosition(mEarthRadiusScale,
                                                                 aSrcLoc.mLocWCS,
                                                                 aTgtLoc.mLocWCS,
                                                                 aSrcLoc.mAlt,
                                                                 aTgtLoc.mAlt,
                                                                 apparentSrcLocWCS,
                                                                 apparentTgtLocWCS);
      if (useApparentView)
      {
         useGeometricView = false;
         UtVec3d::Subtract(aSrcToTgt.mUnitVecWCS, apparentTgtLocWCS, aSrcLoc.mLocWCS);
         UtVec3d::Normalize(aSrcToTgt.mUnitVecWCS);
         aSrcPtr->ComputeAspect(aSrcToTgt.mUnitVecWCS, aSrcToTgt.mAz, aSrcToTgt.mEl);
         withinFieldOfView = aSrcPtr->WithinFieldOfView(aSrcToTgt.mAz, aSrcToTgt.mEl);
         if (withinFieldOfView || aIgnoreLimits)
         {
            // Compute the WCS unit vector from the target to the apparent source position (needed for signature lookups).
            UtVec3d::Subtract(aTgtToSrc.mUnitVecWCS, apparentSrcLocWCS, aTgtLoc.mLocWCS);
            UtVec3d::Normalize(aTgtToSrc.mUnitVecWCS);
         }
      }
   }

   // If the view wasn't computed and checked using the refracted value
   // then check the view using the geometric values.

   if (useGeometricView)
   {
      // The apparent source-to-target view is the same as the geometric view.
      UtVec3d::Set(aSrcToTgt.mUnitVecWCS, aSrcToTgt.mTrueUnitVecWCS);
      aSrcToTgt.mAz     = aSrcToTgt.mTrueAz;
      aSrcToTgt.mEl     = aSrcToTgt.mTrueEl;
      withinFieldOfView = aSrcPtr->WithinFieldOfView(aSrcToTgt.mAz, aSrcToTgt.mEl);
      if (withinFieldOfView || aIgnoreLimits)
      {
         // The apparent target-to-source view is the same as the geometric view.
         UtVec3d::Set(aTgtToSrc.mUnitVecWCS, aTgtToSrc.mTrueUnitVecWCS);
      }
   }

   return withinFieldOfView;
}
