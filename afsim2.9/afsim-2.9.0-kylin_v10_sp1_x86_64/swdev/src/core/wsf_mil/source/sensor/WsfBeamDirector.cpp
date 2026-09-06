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

#include "WsfBeamDirector.hpp"

#include <iostream>
#include <string>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfImageMessage.hpp"
#include "WsfIntersectMesh.hpp"
#include "WsfIntersectMeshTypes.hpp"
#include "WsfIntersectProcessor.hpp" // deprecated
#include "WsfMessage.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"

WsfBeamDirector::WsfBeamDirector(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mCue()
   , mTargetOffset("")
   , mCommentsEnabled(false)
   , mLastModeType(WsfBeamDirectorMode::cUNDEFINED)
   , mWsfBeamDirectorModeList()
{
   SetClass(cACTIVE | cINFRARED); // This is an active IR sensor

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new WsfBeamDirectorMode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());

   // Must assign a tracker; do it here.
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// virtual
WsfSensor* WsfBeamDirector::Clone() const
{
   return new WsfBeamDirector(*this);
}

// virtual
bool WsfBeamDirector::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Attach the entity part to the platform.
   SetPlatform(GetPlatform());

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mWsfBeamDirectorModeList);

   return ok;
}

bool WsfBeamDirector::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "comments")
   {
      aInput.ReadValue(mCommentsEnabled);
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfBeamDirector::TurnOn(double aSimTime)
{
   // Note that the following line to select in mode 0 is critical to proper operation of the beam director.
   // Otherwise it would turn on in the previous mode (usually cued); here we need the sensor to re-acquire.
   SelectMode(aSimTime, GetModeEntry(0)->GetNameId());
   WsfSensor::TurnOn(aSimTime);
}

// virtual
void WsfBeamDirector::TurnOff(double aSimTime)
{
   WsfSensor::TurnOff(aSimTime);
   if (GetCurrentMode()->IsTracking())
   {
      mSchedulerPtr->StopTracking(aSimTime, GetCurrentMode()->GetTrack().GetTrackId());
   }
   GetCurrentMode()->Deselect(aSimTime);
   WsfStringId messageType("BEAM_DIRECTOR_TURN_OFF");
   WsfMessage  turnOffMessage(messageType, GetPlatform());
   SendMessage(aSimTime, turnOffMessage);
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Turn-off.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Beam director: " << GetName();
   }
}

// virtual
void WsfBeamDirector::AcquireTarget(double aSimTime, const WsfTrack& aTrack)
{
   // Set the last mode to break-lock to prevent transition messages
   mLastModeType = WsfBeamDirectorMode::cBREAKLOCK;

   // Always start with the first mode.
   GetCurrentMode()->SetTrack(aTrack);
   SelectMode(aSimTime, GetModeEntry(0)->GetNameId());
}

void WsfBeamDirector::PointAtWCS(double aSimTime, double aPointAtWCS[3])
{
   // Inform observers that we have updated the scan geometry.
   WsfObserver::SensorScanUpdated(GetSimulation())(aSimTime, this);

   if (DebugEnabled())
   {
      double lat;
      double lon;
      double alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);

      auto out = ut::log::debug() << "Point at WCS:";
      auto lla = out.AddNote() << "Platform LLA:";
      lla.AddNote() << "Lat: " << lat;
      lla.AddNote() << "Lon: " << lon;
      lla.AddNote() << "Alt: " << alt;

      double locWCS[3];
      GetPlatform()->GetLocationWCS(locWCS);
      double targetLocWCS[3];
      UtVec3d::Add(targetLocWCS, aPointAtWCS, locWCS);
      UtEntity::ConvertWCSToLLA(targetLocWCS, lat, lon, alt);
      auto wcs = out.AddNote() << "Track LLA:";
      wcs.AddNote() << "Lat: " << lat;
      wcs.AddNote() << "Lon: " << lon;
      wcs.AddNote() << "Alt: " << alt;
   }
}

// virtual
//! A method that directs the sensor to point at a track, instead of at a location.  This distinction allows for
//! automatic tracking of the target by the sensor.
void WsfBeamDirector::PointAtTarget(double aSimTime, const WsfWeapon::FireTarget& aTarget)
{
   assert(aTarget.mTrackPtr != nullptr); // must have a track
   if (DebugEnabled() || mCommentsEnabled)
   {
      std::string comment = GetPlatform()->GetName() + ": " + GetName() + ": received point-at-track-command.";
      if (mCommentsEnabled)
      {
         GetPlatform()->Comment(aSimTime, comment);
      }
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Received point-at-track-command.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Beam Director: " << GetName();
      }
   }
   if (!IsTurnedOn())
   {
      // Turn on the sensor and start it in scan mode; the mode will retrieve the cue directive.
      GetSimulation()->TurnPartOn(aSimTime, this);
   }
   else
   {
      if (mCue.GetTrackId() != aTarget.mTrackPtr->GetTrackId()) // New track; reset to acquire new target
      {
         if (GetCurrentMode()->IsTracking())
         {
            mSchedulerPtr->StopTracking(aSimTime, GetCurrentMode()->GetTrack().GetTrackId());
         }
         GetCurrentMode()->Deselect(aSimTime);
         AcquireTarget(aSimTime, *aTarget.mTrackPtr);
      }
      else if (GetCurrentMode()->GetType() ==
               WsfBeamDirectorMode::cSCANNING) // ONLY during scanning; otherwise we are in a
                                               // tracking loop and take the cue from the sensor.
      {
         GetCurrentMode()->SetTrack(*aTarget.mTrackPtr);
      }
   }
   mCue          = *aTarget.mTrackPtr;
   mTargetOffset = aTarget.mTargetOffset;
   // Otherwise the track is supplied from this sensor in a tracking loop.
}

// Embedded mode class

const WsfImage& WsfBeamDirector::WsfBeamDirectorMode::GetImage()
{
   if (mImagePtr == nullptr)
   {
      mImagePtr = new WsfImage(GetSensor());
   }
   return *mImagePtr;
}

bool WsfBeamDirector::IsInFOR(WsfPlatform* aTargetPtr)
{
   bool isInFOR = false;
   if (aTargetPtr != nullptr)
   {
      double locWCS[3];
      GetLocationWCS(locWCS);
      double targetLocWCS[3];
      aTargetPtr->GetLocationWCS(targetLocWCS);
      double targetVecWCS[3];
      UtVec3d::Subtract(targetVecWCS, targetLocWCS, locWCS);
      double az;
      double el;
      ComputeAspect(targetVecWCS, az, el);
      isInFOR = WithinSlewLimits(az, el);
   }
   return isInFOR;
}

WsfBeamDirector::WsfBeamDirectorMode::WsfBeamDirectorMode(const WsfBeamDirectorMode& aSrc)
   : WsfSensorMode(aSrc)
   , mTransmitter(aSrc.mTransmitter)
   , mXcvrAntenna(aSrc.mXcvrAntenna)
   , mReceiver(aSrc.mReceiver)
   , mType(aSrc.mType)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mAngularResolution(aSrc.mAngularResolution)
   , mTargetRadialExtent(aSrc.mTargetRadialExtent)
   , mTrack(aSrc.mTrack)
   , mImagePtr(nullptr)
   , mMinPixelsForCue(aSrc.mMinPixelsForCue)
   , mPerfectCorrelation(aSrc.mPerfectCorrelation)
   , mTargetsInFOV()
{
   for (size_t i = 0; i < 2; ++i)
   {
      mScanFOV_Az[i] = aSrc.mScanFOV_Az[i];
      mScanFOV_El[i] = aSrc.mScanFOV_El[i];
   }
}

WsfBeamDirector::WsfBeamDirectorMode::~WsfBeamDirectorMode()
{
   delete mImagePtr;
}

// =================================================================================================
//! @copydoc WsfSensor::AttemptToDetect
//!
//! See the base class for documentation of the arguments and return value.
bool WsfBeamDirector::WsfBeamDirectorMode::AttemptToDetect(double               aSimTime,
                                                           WsfPlatform*         aTargetPtr,
                                                           WsfSensor::Settings& aSettings,
                                                           WsfSensorResult&     aResult)
{
   GetSensor()->UpdatePosition(aSimTime);
   mTransmitter.UpdatePosition(aSimTime);
   mXcvrAntenna.UpdatePosition(aSimTime);
   mReceiver.UpdatePosition(aSimTime);

   aResult.Reset();
   aResult.mModeIndex = GetModeIndex();
   if (GetSensor()->DebugEnabled())
   {
      double locWCS[3];
      aTargetPtr->GetLocationWCS(locWCS);
      double rcvrLocWCS[3];
      mReceiver.GetAntenna()->GetLocationWCS(rcvrLocWCS);
      double targetVecWCS[3];
      UtVec3d::Subtract(targetVecWCS, locWCS, rcvrLocWCS);
      double range = UtVec3d::Magnitude(targetVecWCS);

      auto out = ut::log::debug() << "Attempting to detect vs. target location.";
      out.AddNote() << "Loc: " << locWCS[0] << "   " << locWCS[1] << "   " << locWCS[2];
      auto note = out.AddNote() << "Receiver Loc: ";
      note.AddNote() << rcvrLocWCS[0] << "   " << rcvrLocWCS[1] << "   " << rcvrLocWCS[2];
      out.AddNote() << "Range to target: " << range;
   }

   PointAtTargetUpdate(aSimTime); // calls PointAtWCS(), which checks the gimbal angles.
   aResult.BeginTwoWayInteraction(&mTransmitter, aTargetPtr, &mReceiver);
   if (IsTracking() && ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS) != 0u))
   {
      // Clear this result, as we assume we are locked onto the target.
      // We will only come off if the gimbals can't support it or we are occluded.
      aResult.mFailedStatus = aResult.mFailedStatus & (aResult.mFailedStatus ^ WsfEM_Interaction::cRCVR_ANGLE_LIMITS);
   }
   bool detected = false;

   // Determine if wrong target, assuming perfect correlation.
   if (mPerfectCorrelation && (aResult.mFailedStatus == 0))
   {
      if (aTargetPtr->GetNameId() != mTrack.GetTargetName()) // mIntendedTargetName)
      {
         aResult.mFailedStatus = 1; // No enum for this.
         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Detection was against wrong target when assuming perfect correlation.";
            out.AddNote() << "Wrong Target: " << aTargetPtr->GetName();
            out.AddNote() << "Intended Target: " << mTrack.GetTargetName();
         }
      }
   }

   // Determine if gimbals are falling behind.
   if (aResult.mFailedStatus == 0)
   {
      if ((mType != cSCANNING) && GetSensor()->IsSlewing())
      {
         aResult.mFailedStatus = 1;
         WsfMessage slewRateMsg("SLEW_RATE_EXCEEDED", GetSensor()->GetPlatform());
         GetSensor()->SendMessage(aSimTime, slewRateMsg);
      }
   }

   // Determine if concealed.
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Detection failed concealment test.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Target: " << aTargetPtr->GetName();
         }
      }
   }

   if (aResult.mFailedStatus == 0)
   {
      // Perform the actual detection.
      mTransmitter.UpdatePosition(aSimTime);
      mXcvrAntenna.UpdatePosition(aSimTime);
      mReceiver.UpdatePosition(aSimTime);

      double azError    = 0.0;
      double elError    = 0.0;
      double rangeError = 0.0;

      // Modify az-el errors for beam divergence.
      ut::Random& random = GetSensor()->GetSimulation()->GetRandom();
      azError += random.Gaussian() * mAngularResolution;
      elError += random.Gaussian() * mAngularResolution;

      double targetVecWCS[3];
      GetSensor()->GetRelativeLocationWCS(aTargetPtr, targetVecWCS);
      double targetDistance = UtVec3d::Magnitude(targetVecWCS);

      // Use max range only for now (range error should always be << the target distance).
      // Could use atmospheric extinction and target opacity in a more sophisticated model.
      if (targetDistance < GetMaxRange())
      {
         // I am using the rcvrAz to target to mean either
         // 1) If scanning, this is actually the scan center to target error.
         // 2) If cued, it is the true az-el to target.
         double rcvrAzToTarget = 0.0;
         double rcvrElToTarget = 0.0;

         // If cued, we assume we can stay on target;
         // This assumption is contingent on the slew rate checks succeeding.
         // If they do not, the earlier call to PointAtTrackUpdate will cause entry
         // into breaklock state, and this code will not have been reached.
         bool inFOV = true;
         // 3/9/09 mjm todo revisit for Beam Director upgrades (scan vs. cued).
         GetSensor()->ComputeAspect(targetVecWCS, rcvrAzToTarget, rcvrElToTarget);
         // if (mType != cCUED)
         if (!IsTracking()) // Should be on target for all tracking modes.
         {
            // Is the az-el within the scan volume if not cued?
            // The device should already be oriented so that its scan is centered on the
            // track location
            double minAzFOV;
            double maxAzFOV;
            mXcvrAntenna.GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
            double minElFOV;
            double maxElFOV;
            mXcvrAntenna.GetElevationFieldOfView(minElFOV, maxElFOV);

            inFOV = ((rcvrAzToTarget >= minAzFOV) && (rcvrAzToTarget <= maxAzFOV) && (rcvrElToTarget >= minElFOV) &&
                     (rcvrElToTarget <= maxElFOV));
         }

         if (inFOV)
         {
            double targetAzToPlatform = 0.0;
            double targetElToPlatform = 0.0;
            UtVec3d::Multiply(targetVecWCS, targetVecWCS, -1.0);
            aTargetPtr->ComputeAspect(targetVecWCS, targetAzToPlatform, targetElToPlatform);
            double crossSection =
               WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, targetAzToPlatform, targetElToPlatform);

            // We assume the beam is much smaller than the target extent
            // If the beam is outside the target extent, it cannot be detected.
            double extent = sqrt(crossSection) / 2.0;
            mTargetRadialExtent =
               extent / targetDistance; // assuming target is small; otherwise, atan2(extent, targetDistance);

            if (mTargetRadialExtent > 0.1)
            {
               mTargetRadialExtent = atan2(extent, targetDistance);
            }

            // Detection occurs of within beam divergence extent.
            // This should really be based on the scan pattern and radial extent of the target,
            // as well as the beam divergence.
            if ((fabs(azError) <= mTargetRadialExtent) && (fabs(elError) <= mTargetRadialExtent))
            {
               detected    = true;
               aResult.mPd = 1.0;
               aResult.mMeasurement.SetTypeId(aTargetPtr->GetTypeId());
               aResult.mMeasurement.SetTypeIdValid(true);

               // TODO This is very simplified
               // We need to consider offsets for target zones as well as
               // scanning within the FOV.
               aResult.mRcvrToTgt.mAz    = rcvrAzToTarget;
               aResult.mRcvrToTgt.mEl    = rcvrElToTarget;
               aResult.mRcvrToTgt.mRange = targetDistance;
               aResult.mOpticalSig       = crossSection;
               aResult.mOpticalSigAz     = 2.0 * mTargetRadialExtent;
               aResult.mOpticalSigEl     = 2.0 * mTargetRadialExtent;

               double rcvrToTgtLocWCS[3];
               GetSensor()->GetRelativeLocationWCS(aResult.mRcvrToTgt.mAz + azError,
                                                   aResult.mRcvrToTgt.mEl + elError,
                                                   aResult.mRcvrToTgt.mRange + rangeError,
                                                   rcvrToTgtLocWCS);
               double reportedLocWCS[3];
               GetSensor()->GetLocationWCS(rcvrToTgtLocWCS, reportedLocWCS);
               aResult.mMeasurement.SetLocationWCS(reportedLocWCS);

               // For now, perfect velocity.
               // Velocity error would be computed based on position errors and pulse length (in time).
               double velWCS[3];
               aTargetPtr->GetVelocityWCS(velWCS);
               aResult.mMeasurement.SetVelocityWCS(velWCS);

               if (GetSensor()->DebugEnabled())
               {
                  auto out = ut::log::debug() << "Detection for target successful.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Target: " << aTargetPtr->GetName();
                  out.AddNote() << "AZ to Target: " << rcvrAzToTarget * UtMath::cDEG_PER_RAD << " deg";
                  out.AddNote() << "EL to Target: " << rcvrElToTarget * UtMath::cDEG_PER_RAD << " deg";
                  aResult.Print(out);
               }
            }
            else if (GetSensor()->DebugEnabled())
            {
               ut::log::debug() << "Detection failed 'radial extent' test.";
            }

            // If the Pd exceeds the detection threshold then perform terrain masking.
            if (detected)
            {
               // Check terrain masking only if all other tests passed.
               if (aResult.MaskedByTerrain())
               {
                  detected = false;
                  if (GetSensor()->DebugEnabled())
                  {
                     ut::log::debug() << "Detection failed terrain masking test.";
                  }
               }
            }

            // After all this, check for any mesh intersections on the target
            if (detected)
            {
               WsfIntersectMesh* meshPtr =
                  static_cast<WsfIntersectMesh*>(aTargetPtr->FindComponentByRole(cWSF_COMPONENT_INTERSECT_MESH));
               if (meshPtr != nullptr)
               {
                  WsfStringId targetOffsetName = static_cast<WsfBeamDirector*>(GetSensor())->GetTargetOffset();
                  if (!meshPtr->TargetOffsetExists(targetOffsetName))
                  {
                     if (GetSensor()->DebugEnabled())
                     {
                        auto out = ut::log::debug() << "Mesh name not found for target.";
                        out.AddNote() << "Platform: " << GetPlatform()->GetName();
                        out.AddNote() << "Sensor: " << GetSensor()->GetName();
                        out.AddNote() << "Mesh Name: " << targetOffsetName;
                        out.AddNote() << "Target: " << aTargetPtr->GetName();
                     }
                     targetOffsetName = meshPtr->GetTargetOffsetName(1);
                  }
                  else
                  {
                     // If no target offset was provided, pick the first one in the offset list:
                     targetOffsetName = meshPtr->GetTargetOffsetName(1);
                     if (GetSensor()->DebugEnabled())
                     {
                        auto out = ut::log::debug()
                                   << "No target offset was provided, mesh name being used for target.";
                        out.AddNote() << "Platform: " << GetPlatform()->GetName();
                        out.AddNote() << "Sensor: " << GetSensor()->GetName();
                        out.AddNote() << "Mesh Name: " << targetOffsetName;
                        out.AddNote() << "Target: " << aTargetPtr->GetName();
                     }
                  }

                  double surfaceDotProduct;
                  int    materialCode;
                  double range;
                  bool   success = meshPtr->Intersect(GetPlatform(),
                                                    targetOffsetName,
                                                    0.0, // for now use zero jitter...
                                                    surfaceDotProduct,
                                                    materialCode,
                                                    range);
                  // We must be on target (materialcode != 0) with dot incidence angle in proper quadrants
                  if (success)
                  {
                     detected = (materialCode != 0) && (surfaceDotProduct > 0.0);
                     if ((!detected) && (GetSensor()->DebugEnabled()))
                     {
                        std::string comment = "Beam Director: " + GetSensor()->GetName() + aTargetPtr->GetName() + ":" +
                                              targetOffsetName + " occluded.";
                        { // RAII block
                           auto out = ut::log::debug() << "Beam director:";
                           out.AddNote() << "Sensor: " << GetSensor()->GetName();
                           out.AddNote() << "Target: " << aTargetPtr->GetName();
                           out.AddNote() << "Target Offset Name: " << targetOffsetName;
                           out.AddNote() << "Target occluded.";
                        }
                        GetPlatform()->Comment(aSimTime, comment);
                     }
                  }
               }
               // The following code uses the intersect processor and is DEPRECATED
               //------------------------------------------------------------------------------------------------------------
               else if (aTargetPtr->GetAuxDataConst().AttributeExists("INTERSECT_PROCESSOR_NAME"))
               {
                  std::string            ipName = aTargetPtr->GetAuxDataConst().GetString("INTERSECT_PROCESSOR_NAME");
                  WsfIntersectProcessor* ipPtr =
                     static_cast<WsfIntersectProcessor*>(aTargetPtr->GetComponent<WsfProcessor>(ipName));
                  std::string targetOffsetName = "";
                  if (mTrack.GetAuxDataConst().AttributeExists("TARGET_OFFSET"))
                  {
                     targetOffsetName = mTrack.GetAuxDataConst().GetString("TARGET_OFFSET");
                  }
                  else if (mTrack.GetAuxDataConst().AttributeExists("TARGET_REGION")) // DEPRECATED; use TARGET_OFFSET instead
                  {
                     targetOffsetName = mTrack.GetAuxDataConst().GetString("TARGET_REGION");
                  }
                  if (!targetOffsetName.empty())
                  {
                     if (!ipPtr->TargetOffsetExists(targetOffsetName))
                     {
                        if (GetSensor()->DebugEnabled())
                        {
                           auto out = ut::log::debug() << "Mesh name not found for target.";
                           out.AddNote() << "Platform: " << GetPlatform()->GetName();
                           out.AddNote() << "Sensor: " << GetSensor()->GetName();
                           out.AddNote() << "Mesh Name: " << targetOffsetName;
                           out.AddNote() << "Target: " << aTargetPtr->GetName();
                        }
                        targetOffsetName = ipPtr->GetTargetOffsetName(1);
                     }
                  }
                  else
                  {
                     // If no target offset was provided, pick the first one in the offset list:
                     targetOffsetName = ipPtr->GetTargetOffsetName(1);
                     if (GetSensor()->DebugEnabled())
                     {
                        auto out = ut::log::debug()
                                   << "No target offset was provided, mesh name being used for target.";
                        out.AddNote() << "Platform: " << GetPlatform()->GetName();
                        out.AddNote() << "Sensor: " << GetSensor()->GetName();
                        out.AddNote() << "Mesh Name: " << targetOffsetName;
                        out.AddNote() << "Target: " << aTargetPtr->GetName();
                     }
                  }

                  double surfaceDotProduct;
                  int    materialCode;
                  double range;
                  bool   success = ipPtr->Intersect(GetPlatform(),
                                                  targetOffsetName,
                                                  0.0, // for now use zero jitter...
                                                  surfaceDotProduct,
                                                  materialCode,
                                                  range);
                  // We must be on target (materialcode != 0) with dot incidence angle in proper quadrants
                  if (success)
                  {
                     detected = (materialCode != 0) && (surfaceDotProduct > 0.0);
                     if ((!detected) && (GetSensor()->DebugEnabled()))
                     {
                        std::string comment = "Beam Director: " + GetSensor()->GetName() + aTargetPtr->GetName() + ":" +
                                              targetOffsetName + " occluded.";
                        { // RAII block
                           auto out = ut::log::debug() << "Beam director: ";
                           out.AddNote() << "Sensor: " << GetSensor()->GetName();
                           out.AddNote() << "Target: " << aTargetPtr->GetName();
                           out.AddNote() << "Target Offset Name: " << targetOffsetName;
                           out.AddNote() << "Target occluded.";
                        }
                        GetPlatform()->Comment(aSimTime, comment);
                     }
                  }
               }
               // End deprecated code
               //------------------------------------------------------------------------------------------------------------
            }

            if (detected)
            {
               // Create a new track.
               CreateTrackFromResult(aSimTime, aResult, aTargetPtr);
               if (mTargetsInFOV.find(aTargetPtr->GetNameId()) == mTargetsInFOV.end())
               {
                  mTargetsInFOV.insert(aTargetPtr->GetNameId());
               }
               if (IsImaging())
               {
                  CreateImageFromResult(aSimTime, aResult, aTargetPtr);
                  WsfImageMessage imageMessage(GetSensor()->GetPlatform(), *mImagePtr);
                  GetSensor()->SendMessage(aSimTime, imageMessage);
               }
               GetSensor()->SendTrackUpdateMessage(aSimTime, &mTrack);
            }
            else if (mTargetsInFOV.find(aTargetPtr->GetNameId()) != mTargetsInFOV.end())
            {
               mTargetsInFOV.erase(aTargetPtr->GetNameId());
            }
         }
         else if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Detection failed 'in FOV' test.";
            out.AddNote() << "AZ to Target: " << rcvrAzToTarget;
            out.AddNote() << "EL to Target: " << rcvrElToTarget;
         }
      }
      else if (GetSensor()->DebugEnabled())
      {
         ut::log::info() << "Detection failed range test.";
      }
   }

   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   return detected;
}

// virtual
bool WsfBeamDirector::WsfBeamDirectorMode::Initialize(double aSimTime)
{
   // The beam director reports a distinct set of data
   SetCapabilities(cRANGE | cBEARING | cELEVATION | cLOCATION | cVELOCITY | cTYPE);

   mMaximumRequestCount = 1; // This sensor can track one target at a time.

   bool ok = WsfSensorMode::Initialize(aSimTime);
   ok &= mXcvrAntenna.Initialize(mSensorPtr);
   if (ok && (mType == cUNDEFINED))
   {
      auto out = ut::log::error() << "Must specify a type for this mode.";
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Mode: " << GetName();
      ok = false;
   }
   mTransmitter.SetAntenna(&mXcvrAntenna);
   mReceiver.SetAntenna(&mXcvrAntenna);

   // Transmitter attributes are not relevant.
   mTransmitter.SetPower(10);          // watts
   mTransmitter.SetFrequency(2.0e+14); // hz (yes, this is an optical transmitter...)
   ok &= mTransmitter.Initialize(*GetSimulation());

   // Neither are receiver attributes...
   mReceiver.SetFrequency(2.0e+14);
   ok &= mReceiver.Initialize(*GetSimulation());

   mTransmitter.SetEarthRadiusMultiplier(1.0); // The default is 4/3

   // Indicate the receiver is 'linked' with the transmitter.
   mTransmitter.SetLinkedReceiver(&mReceiver);

   mXcvrAntenna.GetAzimuthFieldOfView(mScanFOV_Az[0], mScanFOV_Az[1]);
   mXcvrAntenna.GetElevationFieldOfView(mScanFOV_El[0], mScanFOV_El[1]);

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Laser Sensor Mode:";
      out.AddNote() << "Beam Director: " << this->GetName();
      out.AddNote() << "Initialize with FOV: " << mScanFOV_Az[0] << " " << mScanFOV_Az[1];
   }

   ok &= mTransmitter.Initialize(*GetSimulation());
   ok &= mReceiver.Initialize(*GetSimulation());

   return ok;
}

// virtual
bool WsfBeamDirector::WsfBeamDirectorMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "angular_resolution")
   {
      aInput.ReadValueOfType(mAngularResolution, UtInput::cANGLE);
      aInput.ValueGreater(mAngularResolution, 0.0);
   }
   else if (command == "perfect_correlation")
   {
      aInput.ReadValue(mPerfectCorrelation);
   }
   else if (command == "type")
   {
      std::string type;
      aInput.ReadValue(type);
      if (type == "scanning")
      {
         mType = cSCANNING;
      }
      else if (type == "locked")
      {
         mType = cLOCKED;
      }
      else if (type == "cued")
      {
         mType = cCUED;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "min_pixels_for_cue")
   {
      aInput.ReadValue(mMinPixelsForCue);
      aInput.ValueGreater(mMinPixelsForCue, 0);
   }
   else if (mXcvrAntenna.ProcessInput(aInput))
   {
   }
   else if (mTransmitter.ProcessInput(aInput))
   {
   }
   else if (mReceiver.ProcessInput(aInput))
   {
   }
   else if (WsfSensorMode::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
void WsfBeamDirector::WsfBeamDirectorMode::Deselect(double aSimTime)
{
   // If tracking, pass the track to the next mode:
   WsfBeamDirector* lsPtr = static_cast<WsfBeamDirector*>(GetSensor());
   lsPtr->SetCue(mTrack);        // Save in the sensor for retrieval by the next mode.
   lsPtr->mLastModeType = mType; // Save in the sensor so that we can detect mode changes when the next mode is selected.
   mTransmitter.Deactivate();
   mReceiver.Deactivate();
}

// virtual
void WsfBeamDirector::WsfBeamDirectorMode::Select(double aSimTime)
{
   WsfBeamDirector* lsPtr = static_cast<WsfBeamDirector*>(GetSensor());
   mTrack                 = lsPtr->GetCue();
   mTargetsInFOV.clear();
   // Assume any target previously detected is currently detected.
   if (mTrack.GetTargetName() != 0)
   {
      mTargetsInFOV.insert(mTrack.GetTargetName());
   }
   if (lsPtr->mLastModeType == cCUED && (mType == cLOCKED))
   {
      // Send a message that we are no longer cued to the image
      WsfMessage lostCueMessage("LOST_IMAGE_CUE", lsPtr->GetPlatform());
      lsPtr->SendMessage(aSimTime, lostCueMessage);
      if (GetSensor()->DebugEnabled() || lsPtr->mCommentsEnabled)
      {
         std::string comment = lsPtr->GetPlatform()->GetName() + ": " + lsPtr->GetName() +
                               ": Lost image cue; dropping back to general target lock.";
         if (lsPtr->mCommentsEnabled)
         {
            GetPlatform()->Comment(aSimTime, comment);
         }
         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Lost image cue. dropping back to general target lock.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << lsPtr->GetPlatform()->GetName();
            out.AddNote() << "Beam Director: " << lsPtr->GetName();
         }
      }
   }
   if (GetSensor()->DebugEnabled() || lsPtr->mCommentsEnabled)
   {
      std::string comment =
         lsPtr->GetPlatform()->GetName() + ": " + lsPtr->GetName() + ": New mode selected (" + GetName() + ")";
      if (lsPtr->mCommentsEnabled)
      {
         GetPlatform()->Comment(aSimTime, comment);
      }
      if (GetSensor()->DebugEnabled())
      {
         auto out = ut::log::debug() << "New mode selected.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << lsPtr->GetPlatform()->GetName();
         out.AddNote() << "Beam Director: " << lsPtr->GetName();
         out.AddNote() << "Mode: " << GetName();
      }
   }
   // Handle transition messages:
   if (mType == cCUED)
   {
      // send cued msg.
      WsfMessage cuedToImageMessage("CUED_TO_IMAGE", lsPtr->GetPlatform());
      if (lsPtr->DebugEnabled())
      {
         auto out = ut::log::debug() << "Sending CUED_TO_IMAGE message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << lsPtr->GetPlatform()->GetName();
      }
      lsPtr->SendMessage(aSimTime, cuedToImageMessage);
      if (GetSensor()->DebugEnabled() || lsPtr->mCommentsEnabled)
      {
         std::string comment = lsPtr->GetPlatform()->GetName() + ": " + lsPtr->GetName() + ": Cueing to image of " +
                               lsPtr->GetCurrentMode()->GetTrack().GetTargetName();
         if (lsPtr->mCommentsEnabled)
         {
            GetPlatform()->Comment(aSimTime, comment);
         }
         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Cueing to image of target.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << lsPtr->GetPlatform()->GetName();
            out.AddNote() << "Beam Director: " << lsPtr->GetName();
            out.AddNote() << "Target: " << lsPtr->GetCurrentMode()->GetTrack().GetTargetName();
         }
      }
   }
   else if (mType == cLOCKED)
   {
      if (lsPtr->mLastModeType == cCUED)
      {
         // Send a message that we are no longer cued to the image
         WsfMessage lostCueMessage("LOST_IMAGE_CUE", lsPtr->GetPlatform());
         lsPtr->SendMessage(aSimTime, lostCueMessage);
         if (GetSensor()->DebugEnabled() || lsPtr->mCommentsEnabled)
         {
            std::string comment = lsPtr->GetPlatform()->GetName() + ": " + lsPtr->GetName() +
                                  ": Lost image cue; dropping back to general target lock.";
            if (lsPtr->mCommentsEnabled)
            {
               GetPlatform()->Comment(aSimTime, comment);
            }
            if (GetSensor()->DebugEnabled())
            {
               auto out = ut::log::debug() << "Lost image cue. Dropping back to general target lock.";
               out.AddNote() << "T = " << aSimTime;
            }
         }
      }
      else if (lsPtr->mLastModeType == cSCANNING)
      {
         WsfMessage lockonMessage("LOCKED_ON", lsPtr->GetPlatform());
         lsPtr->SendMessage(aSimTime, lockonMessage);
      }
   }
   else if (mType == cSCANNING)
   {
      // Start tracking; this initiates detections and allows mode changes (very important!!)
      if (lsPtr->mLastModeType == cLOCKED)
      {
         // send breaklock
         WsfMessage breaklockMessage("BREAKLOCK", lsPtr->GetPlatform());
         lsPtr->SendMessage(aSimTime, breaklockMessage);
      }
      lsPtr->StartTracking(aSimTime, mTrack, GetNameId());
   }
   mTransmitter.Activate();
   mReceiver.Deactivate();
}

//! Update the pointing at a given track by using a kinematic update.
void WsfBeamDirector::WsfBeamDirectorMode::PointAtTargetUpdate(double aSimTime)
{
   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "LaserSensor: Cueing sensor to track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Track Target: " << mTrack.GetTargetName();
   }
   double trackLoc[3];
   mTrack.GetLocationWCS(trackLoc);
   double trackVel[3];
   mTrack.GetVelocityWCS(trackVel);
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(mTrack.GetTargetIndex());

   if (targetPtr != nullptr)
   {
      targetPtr->Update(aSimTime);
   }

   mTrack.KinematicUpdate(aSimTime);

   if (GetSensor()->DebugEnabled() && (targetPtr != nullptr))
   {
      double locWCS[3];
      targetPtr->GetLocationWCS(locWCS);
      double diffWCS[3];
      UtVec3d::Subtract(diffWCS, locWCS, trackLoc);
      double difference = UtVec3d::Magnitude(diffWCS);
      auto   out        = ut::log::debug() << "LaserSensor: Extrapolated track location difference:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Difference: " << difference;
      { // RAII block
         auto target = out.AddNote() << "Target: " << targetPtr->GetName();
         target.AddNote() << "Loc: " << locWCS[0] << " " << locWCS[1] << " " << locWCS[2];
      }
      { // RAII block
         auto track = out.AddNote() << "Track: " << targetPtr->GetName();
         track.AddNote() << "Loc: " << trackLoc[0] << " " << trackLoc[1] << " " << trackLoc[2];
      }
   }

   PointAtWCS(aSimTime, trackLoc);
}

//! A private method whereby we hide the details of pointing.  Pointing the lidar works differently from
//!  the SimpleSensor.  We choose to orient the sensor explicitly, either to the scan center to simulate scanning,
//!  or to the target center to simulate tracking.  This arrangement results in a better visual display.
void WsfBeamDirector::WsfBeamDirectorMode::PointAtWCS(double aSimTime, double aPointAtWCS[3])
{
   // If scanning we only need to point once in order to find the scan center, because
   // we are not simulating the actual scan sequence, only the scan fov.
   // Otherwise we are locked on and cueing to a point within the image produced by the lidar.
   //  Compute slew rates for the device for locked modes.
   double targetVecWCS[3];
   GetSensor()->GetRelativeLocationWCS(aPointAtWCS, targetVecWCS);
   WsfBeamDirector* lsPtr = static_cast<WsfBeamDirector*>(GetSensor());

   lsPtr->PointAtWCS(aSimTime, targetVecWCS); // without regard to mode
}

// virtual
void WsfBeamDirector::WsfBeamDirectorMode::Update(double aSimTime)
{
   mXcvrAntenna.UpdatePosition(aSimTime);
   mTransmitter.UpdatePosition(aSimTime);
   mReceiver.UpdatePosition(aSimTime);

   WsfBeamDirector* lsPtr = static_cast<WsfBeamDirector*>(mSensorPtr);
   // Perform any required detection attempts.
   // Note: PerformScheduledDetections calls "AttemptToDetect" for each platform.
   lsPtr->PerformScheduledDetections(aSimTime);

   mLastUpdateTime = aSimTime;
}

// virtual
void WsfBeamDirector::Update(double aSimTime)
{
   WsfSensor::Update(aSimTime);
   GetCurrentMode()->Update(aSimTime);
   ;
}

void WsfBeamDirector::WsfBeamDirectorMode::CreateTrackFromResult(double                 aSimTime,
                                                                 const WsfSensorResult& aResult, // in
                                                                 WsfPlatform*           aTargetPtr)
{
   mTrack.SetUpdateTime(aSimTime);
   mTrack.SetWCS_LocationValid(true);
   double                locWCS[3];
   const WsfMeasurement& measurement = aResult.mMeasurement;
   measurement.GetLocationWCS(locWCS);
   mTrack.SetLocationWCS(locWCS);
   mTrack.SetVelocityValid(true);
   double velWCS[3];
   measurement.GetVelocityWCS(velWCS);
   mTrack.SetVelocityWCS(velWCS);
   mTrack.SetTypeIdValid(true);
   mTrack.SetTypeId(measurement.GetTypeId());
   mTrack.SetTargetIndex(aTargetPtr->GetIndex());
   mTrack.SetTargetName(aTargetPtr->GetNameId());

   mTrack.SetFusible(true);
   if (GetSensor()->DebugEnabled())
   {
      double lat;
      double lon;
      double alt;
      UtEntity::ConvertWCSToLLA(locWCS, lat, lon, alt);
      auto out = ut::log::debug() << "Target detected at location:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Target: " << aTargetPtr->GetName();
      out.AddNote() << "Lat: " << lat;
      out.AddNote() << "Lon: " << lon;
      out.AddNote() << "Alt: " << alt;
   }
}

void WsfBeamDirector::WsfBeamDirectorMode::CreateImageFromResult(double                 aSimTime,
                                                                 const WsfSensorResult& aResult, // in
                                                                 WsfPlatform*           aTargetPtr)
{
   // !!! Note: The image object must be created on the fly:
   // !!! If aggregated with the sensor, it attempts to get the sensors mode,
   // !!! which hasn't been defined yet!
   if (mImagePtr == nullptr)
   {
      mImagePtr = new WsfImage(GetSensor());
   }
   WsfImage image(GetSensor());
   image.SetImageTime(aSimTime);
   image.SetImageNumber(1);
   WsfImage::Object object(aTargetPtr);
   double           loc[3];
   aTargetPtr->GetLocationWCS(loc);
   object.SetLocationWCS(loc);
   object.SetTruthType(aTargetPtr->GetTypeId());
   double linearPixelsSize = aResult.mRcvrToTgt.mRange * mAngularResolution;
   object.SetPixelCount(static_cast<unsigned>(aResult.mOpticalSig / (linearPixelsSize * linearPixelsSize)));
   image.AddObject(object);
   *mImagePtr = image;
}
