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

#include "WsfGeometricSensor.hpp"

#include <algorithm>
#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

static const double cLARGE_VALUE = 1.0E30;

// =================================================================================================
WsfGeometricSensor::WsfGeometricSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mGeometricModeList()
{
   SetClass(cPASSIVE); // Assume this is a passive sensor (TODO - may want to add a command to set this!)

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new GeometricMode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfGeometricSensor::WsfGeometricSensor(const WsfGeometricSensor& aSrc)
   : WsfSensor(aSrc)
   , mGeometricModeList()
{
}

// =================================================================================================
// virtual
WsfSensor* WsfGeometricSensor::Clone() const
{
   return new WsfGeometricSensor(*this);
}

// =================================================================================================
// virtual
bool WsfGeometricSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mGeometricModeList);

   return ok;
}

// =================================================================================================
// virtual
bool WsfGeometricSensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// =================================================================================================
// virtual
void WsfGeometricSensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// =================================================================================================
// virtual
size_t WsfGeometricSensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mGeometricModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfGeometricSensor::GetEM_Rcvr(size_t aIndex) const
{
   return mGeometricModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

// =================================================================================================
// Definition for the nested class that implements the mode of the sensor.
// =================================================================================================

WsfGeometricSensor::GeometricMode::GeometricMode()
   : WsfSensorMode()
   , mAntenna()
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mMinRangeRate(0)
   , mMaxRangeRate(UtMath::cLIGHT_SPEED + 1.0)
{
   mRcvr.SetFrequency(0.1);
   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL ^ cSIGNAL_TO_NOISE);
}

// =================================================================================================
WsfGeometricSensor::GeometricMode::GeometricMode(const GeometricMode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(aSrc.mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mMinRangeRate(aSrc.mMinRangeRate)
   , mMaxRangeRate(aSrc.mMaxRangeRate)
   , mTypeCriteria(aSrc.mTypeCriteria)
{
}

// =================================================================================================
WsfGeometricSensor::GeometricMode& WsfGeometricSensor::GeometricMode::operator=(const GeometricMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorMode::operator=(aRhs);
      mAntenna               = aRhs.mAntenna;
      mRcvr                  = WsfEM_Rcvr(aRhs.mRcvr, &mAntenna);
      mMinRangeRate          = aRhs.mMinRangeRate;
      mMaxRangeRate          = aRhs.mMaxRangeRate;
      mTypeCriteria          = aRhs.mTypeCriteria;
   }
   return *this;
}

// =================================================================================================
// virtual
WsfMode* WsfGeometricSensor::GeometricMode::Clone() const
{
   return new GeometricMode(*this);
}

// =================================================================================================
// virtual
bool WsfGeometricSensor::GeometricMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);
   ok &= mAntenna.Initialize(mSensorPtr);
   TypeDetectionCriteria& defaultCriteria = mTypeCriteria[UtStringIdLiteral("default")];
   if (defaultCriteria.mMaximumRange == cLARGE_VALUE)
   {
      // If the default max range is unset, use the antenna parameter
      defaultCriteria.mMaximumRange = mAntenna.GetMaximumRange();
   }
   if (defaultCriteria.mMaximumRange > mAntenna.GetMaximumRange())
   {
      defaultCriteria.mMaximumRange = mAntenna.GetMaximumRange();
   }
   mMaximumRange = defaultCriteria.mMaximumRange;
   for (auto& tri : mTypeCriteria)
   {
      if (tri.second.mMaximumRange != cLARGE_VALUE)
      {
         mMaximumRange = std::max(mMaximumRange, tri.second.mMaximumRange);
      }
      else
      {
         tri.second.mMaximumRange = mAntenna.GetMaximumRange();
      }
   }
   ok &= mRcvr.Initialize(*mSensorPtr->GetSimulation());

   // Set the debug flag
   mRcvr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// =================================================================================================
// virtual
bool WsfGeometricSensor::GeometricMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "platform_type")
   {
      std::string platformType;
      aInput.ReadValue(platformType);
      WsfStringId platformTypeId = platformType;
      std::string subCmd;
      int         subcmdCount = 0;
      bool        isSubcmd    = true;
      while (isSubcmd)
      {
         aInput.ReadCommand(subCmd);
         if (subCmd == "detection_range")
         {
            double maxRange;
            aInput.ReadValueOfType(maxRange, UtInput::cLENGTH);
            aInput.ValueGreater(maxRange, 0.0);
            mTypeCriteria[platformTypeId].mMaximumRange = maxRange;
            ++subcmdCount;
         }
         else if (subCmd == "pd_range_table")
         {
            UtInputBlock         block(aInput);
            double               previousRange = -1.0E-10;
            auto                 tablePtr      = std::make_shared<PD_RangeTable>();
            std::vector<double>& pds           = tablePtr->mProbability.GetValues();
            std::vector<double>& ranges        = tablePtr->mRange.GetValues();
            while (block.ReadCommand(subCmd))
            {
               aInput.PushBack(subCmd);
               double pd;
               aInput.ReadValue(pd);
               aInput.ValueInClosedRange(pd, 0.0, 1.0);
               pds.push_back(pd);
               double range;
               aInput.ReadValueOfType(range, UtInput::cLENGTH);
               aInput.ValueGreater(range, previousRange);
               ranges.push_back(range);
            }
            if (ranges.size() < 2)
            {
               throw UtInput::BadValue(aInput, "Expected at least two entries in the defined pd_range_table");
            }

            tablePtr->mProbability.Resize(static_cast<int>(pds.size()));
            mTypeCriteria[platformTypeId].mPD_TablePtr = tablePtr;
            ++subcmdCount;
         }
         else if (subcmdCount == 0)
         {
            throw UtInput::BadValue(aInput, "Expected detection_range or pd_range_table for platform_type");
         }
         else
         {
            aInput.PushBack(subCmd);
            isSubcmd = false;
         }
      }
   }
   else if (command == "minimum_range_rate")
   {
      aInput.ReadValueOfType(mMinRangeRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinRangeRate, 0.0);
   }
   else if (command == "maximum_range_rate")
   {
      aInput.ReadValueOfType(mMaxRangeRate, UtInput::cSPEED);
      aInput.ValueGreater(mMaxRangeRate, 0.0);
   }
   else if (mAntenna.ProcessInput(aInput) || mRcvr.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Get the range rate of the target relative to the sensor.
//! This computes the relative speed of the target along the range vector from the sensor to the target.
//! @param aTargetPtr [input] Pointer to target platform.
//! @return The range rate of the target relative to the sensor. The value is
//! negative if the target is closing and positive when receding.
double WsfGeometricSensor::GeometricMode::GetRangeRate(WsfPlatform* aTargetPtr)
{
   // Get the relative WCS location of the target with respect to the sensor (L_t - L_s).
   double relTgtLocWCS[3];
   mRcvr.GetAntenna()->GetRelativeLocationWCS(aTargetPtr, relTgtLocWCS);

   // Get the target Doppler WCS velocity.
   // This will be either the raw target velocity (if 'filtered_doppler_speed') or the target velocity
   // relative to the sensor ('unfiltered_doppler_speed').
   double relTgtVelWCS[3];
   aTargetPtr->GetVelocityWCS(relTgtVelWCS);
   // Get the sensor velocity.
   double snrVelWCS[3];
   mRcvr.GetPlatform()->GetVelocityWCS(snrVelWCS);
   // Compute the relative WCS velocity (V_t - V_s)
   UtVec3d::Subtract(relTgtVelWCS, relTgtVelWCS, snrVelWCS);

   // Compute the magnitude of the velocity vector along the relative position vector.
   // This is the range rate.
   double rangeRate = UtVec3d::DotProduct(relTgtVelWCS, relTgtLocWCS);
   rangeRate /= std::max(UtVec3d::Magnitude(relTgtLocWCS), 1.0);
   return rangeRate;
}

// =================================================================================================
bool WsfGeometricSensor::GeometricMode::AttemptToDetect(double           aSimTime,
                                                        WsfPlatform*     aTargetPtr,
                                                        Settings&        aSettings,
                                                        WsfSensorResult& aResult)
{
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   // Determine if within Doppler limits.
   if ((mMinRangeRate > 0.0) || (mMaxRangeRate < UtMath::cLIGHT_SPEED))
   {
      aResult.mCheckedStatus |= WsfSensorResult::cDOPPLER_LIMITS;
      double dopplerSpeed = fabs(GetRangeRate(aTargetPtr));
      if ((dopplerSpeed < mMinRangeRate) || (dopplerSpeed > mMaxRangeRate))
      {
         aResult.mFailedStatus |= WsfSensorResult::cDOPPLER_LIMITS;
         // Must have xmtr/tgt pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   if ((aResult.mFailedStatus == 0) && (aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr) == 0))
   {
      // The following is essentially a copy of WsfEM_Interaction::BeginOneWayInteraction, with mods to support a variable range.

      WsfEM_Antenna* rcvrAntennaPtr = mRcvr.GetAntenna();

      // Get the range and unit vector from the receiver to the target.

      rcvrAntennaPtr->GetLocationWCS(aResult.mRcvrLoc.mLocWCS);
      aTargetPtr->GetLocationWCS(aResult.mTgtLoc.mLocWCS);
      UtVec3d::Subtract(aResult.mRcvrToTgt.mTrueUnitVecWCS, aResult.mTgtLoc.mLocWCS, aResult.mRcvrLoc.mLocWCS);
      aResult.mRcvrToTgt.mRange = UtVec3d::Normalize(aResult.mRcvrToTgt.mTrueUnitVecWCS);

      // Compute any previously undefined geometry.
      aResult.ComputeUndefinedGeometry();

      // Determine if the target is within range of the receiver.

      aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_RANGE_LIMITS;
      TypeDetectionCriteria& criteria = GetDetectionCriteria(aTargetPtr);
      if ((aResult.mRcvrToTgt.mRange > criteria.mMaximumRange) || (aResult.mRcvrToTgt.mRange < mAntenna.GetMinimumRange()))
      {
         aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_RANGE_LIMITS;
      }

      // Determine if the target is within the altitude limits of the receiver.

      if (aResult.mFailedStatus == 0)
      {
         aTargetPtr->GetLocationLLA(aResult.mTgtLoc.mLat, aResult.mTgtLoc.mLon, aResult.mTgtLoc.mAlt);
         aResult.mTgtLoc.mIsValid = true;
         aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS;
         if (!rcvrAntennaPtr->WithinAltitude(aResult.mTgtLoc.mAlt))
         {
            aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS;
         }
      }

      // Determine if the line-of-sight is masked by the horizon.

      if (aResult.mFailedStatus == 0)
      {
         rcvrAntennaPtr->GetLocationLLA(aResult.mRcvrLoc.mLat, aResult.mRcvrLoc.mLon, aResult.mRcvrLoc.mAlt);
         aResult.mRcvrLoc.mIsValid = true;
         aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
         if (mRcvr.CheckMasking() && mRcvr.IsHorizonMaskingEnabled())
         {
            if (WsfEM_Interaction::MaskedByHorizon(&mRcvr, aTargetPtr, aResult.mEarthRadiusScale))
            {
               aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
            }
         }
      }

      // Determine if the target is within the field of view of the receiver.

      if (aResult.mFailedStatus == 0)
      {
         aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_ANGLE_LIMITS;
         if (!aResult.WithinFieldOfView(rcvrAntennaPtr, aResult.mRcvrLoc, aResult.mTgtLoc, aResult.mRcvrToTgt, aResult.mTgtToRcvr))
         {
            aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_ANGLE_LIMITS;
         }
      }

      if (aResult.mFailedStatus == 0)
      {
         // Compute the target-to-receiver aspect angles from the unit vectors
         // (Not specifically required, but it is printed out by debug and sensor detection events).
         aTargetPtr->ComputeAspect(aResult.mTgtToRcvr.mTrueUnitVecWCS, aResult.mTgtToRcvr.mTrueAz, aResult.mTgtToRcvr.mTrueEl);
         aTargetPtr->ComputeAspect(aResult.mTgtToRcvr.mUnitVecWCS, aResult.mTgtToRcvr.mAz, aResult.mTgtToRcvr.mEl);

         aResult.mPd = 1.0;
         if (aResult.CategoryIsSet())
         {
            aResult.mPd = std::max(0.0, aResult.mPd - aResult.mZoneAttenuationValue);
         }

         // Account for structural masking.
         aResult.ComputeMaskingFactor();
         aResult.mPd *= aResult.mMaskingFactor;

         if (criteria.mPD_TablePtr)
         {
            // Get the Pd from the Pd vs. range table
            TblLookupLU<double> rangeLU;
            rangeLU.Lookup(criteria.mPD_TablePtr->mRange, aResult.mRcvrToTgt.mRange);
            double pd = TblEvaluate(criteria.mPD_TablePtr->mProbability, rangeLU);
            aResult.mPd *= pd;
         }

         // Determine if the Pd exceeds the detection threshold.
         aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         if ((aResult.mPd >= aSettings.mRequiredPd) && mRcvr.CheckMasking() && mRcvr.IsTerrainMaskingEnabled())
         {
            // Check terrain masking only if all other checks succeed
            aResult.MaskedByTerrain();
         }
         else if (aResult.mPd < aSettings.mRequiredPd)
         {
            aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         }
      }
   }

   // Compute component effects.
   bool detected = WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

   // Determine the impact (if any) that an external script should have on detection status
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Detect Result:";
      aResult.Print(out);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// =================================================================================================
// virtual
void WsfGeometricSensor::GeometricMode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
}

// =================================================================================================
// virtual
void WsfGeometricSensor::GeometricMode::Select(double aSimTime)
{
   mRcvr.Activate();
}

// =================================================================================================
//! Returns the TypeDetectionCriteria struct for a target based on its platform type.
WsfGeometricSensor::GeometricMode::TypeDetectionCriteria&
WsfGeometricSensor::GeometricMode::GetDetectionCriteria(WsfPlatform* aTargetPtr)
{
   TypeDetectionCriteria*     criteriaPtr = nullptr;
   int                        depth       = 0;
   const WsfObject::TypeList& types       = aTargetPtr->GetTypeList();
   for (size_t i = 0; i < types.size(); ++i)
   {
      auto tri = mTypeCriteria.find(types[i]);
      if (tri != mTypeCriteria.end())
      {
         criteriaPtr = &tri->second;
         if (depth > 2)
         {
            // Cache the result for next time
            mTypeCriteria[types.front()] = *criteriaPtr;
         }
         break;
      }
      ++depth;
   }
   if (criteriaPtr == nullptr) // Type does not match any types in target's type list; return the default value.
   {
      criteriaPtr = &mTypeCriteria[UtStringIdLiteral("default")];
   }
   return *criteriaPtr;
}
