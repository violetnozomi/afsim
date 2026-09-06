// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCTD_SensorComponent.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <utility>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"

namespace
{

class CTDSensorComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override
   {
      // This must always be present on RF sensors.
      // It isn't needed on acoustic or optical sensors.
      if (aParent.GetClass() & WsfSensor::cRADIO)
      {
         WsfCTD_SensorComponent::FindOrCreate(aParent);
      }
   }
};

// These tracker settings are for calls to the tracker from within this component. The third argument
// is a function code to indicate why the call is happening. That way when the call comes back to
// this component (in the form of 'Tracker...' calls) we will know the special processing that may
// need to be applied.
enum
{
   cCTD_UPDATES = 1, //!< Close Target Detection detected, undetected or deleted
};

// Tracker settings for Close Target Detection (CTD) updates
WsfSensorTrackerSettings ctdTrackerSettings(WsfSensorTrackerSettings::cSUPPRESS_DETECTION_CHANGE_MESSAGES,
                                            cWSF_COMPONENT_CTD_SENSOR,
                                            cCTD_UPDATES);
} // namespace

// =================================================================================================
//! Register the component factory that handles input for this component.
void WsfCTD_SensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<CTDSensorComponentFactory>());
}

// =================================================================================================
//! Find the instance of this component attached to the specified sensor.
WsfCTD_SensorComponent* WsfCTD_SensorComponent::Find(const WsfSensor& aParent)
{
   WsfCTD_SensorComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfCTD_SensorComponent>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified sensor,
//! and create it if it doesn't exist.
WsfCTD_SensorComponent* WsfCTD_SensorComponent::FindOrCreate(WsfSensor& aParent)
{
   WsfCTD_SensorComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      auto newComponentPtr = ut::make_unique<WsfCTD_SensorComponent>();
      componentPtr         = newComponentPtr.get();
      // Determine if the parent is of type WsfRadarSensor. This may be needed during input processing
      // and it needs to be done here. It cannot be done in 'ComponentParentChanged' because the RTTI
      // information is not fully available at that time.
      componentPtr->mIsWsfRadarSensor = (dynamic_cast<WsfRadarSensor*>(&aParent) != nullptr);
      aParent.GetComponents().AddComponent(newComponentPtr.release());
   }
   return componentPtr;
}

// =================================================================================================
WsfComponent* WsfCTD_SensorComponent::CloneComponent() const
{
   return new WsfCTD_SensorComponent(*this);
}

// =================================================================================================
WsfStringId WsfCTD_SensorComponent::GetComponentName() const
{
   static WsfStringId nullStringId;
   return nullStringId;
}

// =================================================================================================
const int* WsfCTD_SensorComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_CTD_SENSOR, cWSF_COMPONENT_SENSOR_COMPONENT, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* WsfCTD_SensorComponent::QueryInterface(int aRole)
{
   if ((aRole == cWSF_COMPONENT_CTD_SENSOR) || (aRole == cWSF_COMPONENT_SENSOR_COMPONENT))
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
bool WsfCTD_SensorComponent::Initialize(double aSimTime)
{
   bool ok = true;

   mDebugEnabled |= GetSensor()->DebugEnabled();

   // If the mode list is empty then the single implicit mode defined by the template is used.
   if (mModeList.empty())
   {
      mModeList.push_back(mTemplate);
   }

   // Expand the local mode list to match the sensor mode list size. In cases where the modes are generated
   // from user input then they will be the same. In the case of WsfCompositeSensor that isn't true as its
   // mode list is derived from the constituent sensors.
   while (GetSensor()->GetModeCount() > mModeList.size())
   {
      mModeList.push_back(mTemplate);
   }

   // Initialize each of the modes.
   for (unsigned int modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
   {
      CTD_Mode&      mode(mModeList[modeIndex]);
      WsfSensorMode* modePtr = GetSensor()->GetModeEntry(modeIndex);

      // Validate the mode index and the mode->sensor pointer
      if (modeIndex != modePtr->GetModeIndex())
      {
         ut::log::error() << "WsfCTD_SensorComponent mode index mismatch.";
         ok = false;
      }
      if (modePtr->GetSensor() != GetSensor())
      {
         ut::log::error() << "WsfCTD_SensorComponent mode->sensor pointer mismatch.";
         ok = false;
      }

      size_t beamCount = modePtr->GetBeamCount();

      // For the same reason as the mode list size, expand the beam list size.
      while (beamCount > mode.mBeamList.size())
      {
         mode.mBeamList.push_back(mode.mBeamList[0]);
      }

      // Validate the beam index and the beam->mode pointer
      for (size_t beamIndex = 0; beamIndex < mode.mBeamList.size(); ++beamIndex)
      {
         if (beamCount > 0)
         {
            WsfSensorBeam* beamPtr = modePtr->GetBeamEntry(beamIndex);
            if (beamPtr->GetBeamIndex() != beamIndex)
            {
               ut::log::error() << "WsfCTD_SensorComponent beam index mismatch.";
               ok = false;
            }
            if (beamPtr->GetSensorMode() != modePtr)
            {
               ut::log::error() << "WsfCTD_SensorComponent beam->mode pointer mismatch.";
               ok = false;
            }
         }
         if (ok)
         {
            mode.mBeamList[beamIndex].mBeamIndex = beamIndex;
         }
      }
   }
   return ok;
}

// =================================================================================================
bool WsfCTD_SensorComponent::ProcessBeamInput(UtInput& aInput, WsfSensorBeam& aSensorBeam)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "close_target_detection")
   {
      mEnabled = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "acquire_deltas")
         {
            ProcessDeltaInputs(aInput, GetOrCreateCTD_Beam(aSensorBeam).mAcquireDeltas);
         }
         else if (command == "reacquire_deltas")
         {
            ProcessDeltaInputs(aInput, GetOrCreateCTD_Beam(aSensorBeam).mReacquireDeltas);
         }
         else if (command == "debug")
         {
            mDebugEnabled = true;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfCTD_SensorComponent::SelectMode(double aSimTime, WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   if (modePtr != nullptr)
   {
      CTD_Mode& mode(mModeList[modePtr->GetModeIndex()]);
      mode.mTargetResults.clear();
   }
}

// =================================================================================================
// virtual
void WsfCTD_SensorComponent::DeselectMode(double aSimTime, WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   if (modePtr != nullptr)
   {
      CTD_Mode& mode(mModeList[modePtr->GetModeIndex()]);
      mode.mTargetResults.clear();
   }
}

// =================================================================================================
void WsfCTD_SensorComponent::AttemptToDetect(double aSimTime, WsfSensorResult& aResult)
{
   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return;
   }

   WsfSensorMode* modePtr = dynamic_cast<WsfSensorMode*>(aResult.GetReceiver()->GetMode());
   if (modePtr != nullptr)
   {
      CTD_Mode& mode(mModeList[modePtr->GetModeIndex()]);
      CTD_Beam& beam(mode.mBeamList[aResult.mBeamIndex]);
      if (beam.mTargetIndex == 0)
      {
         beam.mTargetIndex = aResult.GetTarget()->GetIndex();

         beam.mCTD_Results.clear();
         for (auto& tgtIndx : beam.mCTD_Platforms)
         {
            if (tgtIndx != beam.mTargetIndex)
            {
               WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(tgtIndx);
               if (tgtPtr != nullptr)
               {
                  tgtPtr->Update(aSimTime); // Ensure the target position is current
                  WsfSensorResult     tgtResult = aResult;
                  WsfSensor::Settings settings;
                  settings.mModeIndex  = aResult.mModeIndex;
                  settings.mRequiredPd = aResult.mRequiredPd;
                  WsfRadarSensor::RadarBeam* beamPtr =
                     dynamic_cast<WsfRadarSensor::RadarBeam*>(modePtr->GetBeamEntry(aResult.mBeamIndex));
                  beamPtr->AttemptToDetect(aSimTime, tgtPtr, settings, tgtResult);
                  if (tgtResult.Detected())
                  {
                     // Store the results for now and use post detection attempt
                     beam.mCTD_Results.push_back(tgtResult);
                  }
               }
            }
         }
         beam.mTargetIndex = 0; // reset index for next target
      }
   }
}

// =================================================================================================
void WsfCTD_SensorComponent::PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return;
   }

   double resultAz    = aResult.mRcvrToTgt.mTrueAz;
   double resultEl    = aResult.mRcvrToTgt.mTrueEl;
   double resultRange = aResult.mRcvrToTgt.mRange;

   bool replaced = false;
   for (auto& mode : mModeList)
   {
      for (auto& beam : mode.mBeamList)
      {
         for (auto& result : beam.mCTD_Results)
         {
            if (result.mSignalToNoise > aResult.mSignalToNoise)
            {
               bool allowReplacement = false;
               auto tgtResult        = mode.mTargetResults.find(aTargetPtr->GetIndex());
               if (tgtResult == mode.mTargetResults.end() ||
                   tgtResult->second.mLastTargetResult != result.GetTarget()->GetIndex())
               {
                  allowReplacement =
                     ((beam.mAcquireDeltas.mAzimuthDelta >= 0.0) || (beam.mAcquireDeltas.mElevationDelta >= 0.0) ||
                      (beam.mAcquireDeltas.mRangeDelta >= 0.0)) &&
                     ((beam.mAcquireDeltas.mAzimuthDelta < 0.0) ||
                      (fabs(resultAz - result.mRcvrToTgt.mTrueAz) <= beam.mAcquireDeltas.mAzimuthDelta)) &&
                     ((beam.mAcquireDeltas.mElevationDelta < 0.0) ||
                      (fabs(resultEl - result.mRcvrToTgt.mTrueEl) <= beam.mAcquireDeltas.mElevationDelta)) &&
                     ((beam.mAcquireDeltas.mRangeDelta < 0.0) ||
                      (fabs(resultRange - result.mRcvrToTgt.mRange) <= beam.mAcquireDeltas.mRangeDelta));
               }

               if (allowReplacement)
               {
                  aResult  = result;
                  replaced = true;
               }
            }
         }
         beam.mCTD_Results.clear();
      }
   }

   if (replaced && mDebugEnabled)
   {
      auto out = ut::log::debug() << "'ctd_detection' interaction replaced detection.";
      out.AddNote() << "Replaced target: " << aTargetPtr->GetName();
      out.AddNote() << "New Target: " << aResult.GetTarget()->GetName();
      aResult.Print(out);
   }
}

// =================================================================================================
bool WsfCTD_SensorComponent::TrackerAllowTracking(double                 aSimTime,
                                                  const TrackerSettings& aSettings,
                                                  const WsfTrackId&      aRequestId,
                                                  size_t                 aObjectId,
                                                  WsfTrack*              aTrackPtr,
                                                  WsfSensorResult&       aResult)
{
   bool allowTracking = true;

   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return allowTracking;
   }

   WsfSensorMode* modePtr = dynamic_cast<WsfSensorMode*>(aResult.GetReceiver()->GetMode());
   if (modePtr)
   {
      CTD_Mode& mode(mModeList[modePtr->GetModeIndex()]);

      if (aTrackPtr != nullptr)
      {
         if (aResult.GetTarget()->GetIndex() != aObjectId)
         {
            CTD_Beam& beam(mode.mBeamList[aResult.mBeamIndex]);

            WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(aObjectId);

            mode.mTargetResults[aObjectId].mTrackingCloseTarget = true;
            mode.mTargetResults[aObjectId].mTargetReacquired    = false;
            mode.mTargetResults[aObjectId].mLastTargetResult    = aResult.GetTarget()->GetIndex();

            double tgtLocWCS[3];
            tgtPtr->GetLocationWCS(tgtLocWCS);
            double trueUnitVecWCS[3];
            UtVec3d::Subtract(trueUnitVecWCS, tgtLocWCS, aResult.mRcvrLoc.mLocWCS);
            double tgtRange = UtVec3d::Normalize(trueUnitVecWCS);
            double tgtAz, tgtEl;
            // aResult.GetReceiver()->GetAntenna()->ComputeAspect(trueUnitVecWCS, tgtAz, tgtEl);
            tgtPtr->ComputeAspect(trueUnitVecWCS, tgtAz, tgtEl);


            mode.mTargetResults[aObjectId].mReacquirePossible = true;
            if (((beam.mReacquireDeltas.mAzimuthDelta >= 0.0) || (beam.mReacquireDeltas.mElevationDelta >= 0.0) ||
                 (beam.mReacquireDeltas.mRangeDelta >= 0.0)) &&
                ((aTrackPtr->BearingValid() &&
                  ((beam.mReacquireDeltas.mAzimuthDelta < 0.0) ||
                   (fabs(tgtAz - aTrackPtr->GetBearing()) >= beam.mReacquireDeltas.mAzimuthDelta))) ||
                 (aTrackPtr->ElevationValid() &&
                  ((beam.mReacquireDeltas.mElevationDelta < 0.0) ||
                   (fabs(tgtEl - aTrackPtr->GetElevation()) >= beam.mReacquireDeltas.mElevationDelta))) ||
                 (aTrackPtr->RangeValid() &&
                  ((beam.mReacquireDeltas.mRangeDelta < 0.0) ||
                   (fabs(tgtRange - aTrackPtr->GetRange()) >= beam.mReacquireDeltas.mRangeDelta)))))
            {
               mode.mTargetResults[aObjectId].mReacquirePossible = false;
            }

            if (mDebugEnabled)
            {
               auto out = ut::log::debug() << "ctd_detection: Target:";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Reacquire possible: " << std::boolalpha
                             << mode.mTargetResults[aObjectId].mReacquirePossible;
            }
         }
         else
         {
            if (mode.mTargetResults[aObjectId].mTrackingCloseTarget)
            {
               mode.mTargetResults[aObjectId].mTargetReacquired = true;
               if (!mode.mTargetResults[aObjectId].mReacquirePossible)
               {
                  allowTracking = false;
               }
               mode.mTargetResults[aObjectId].mTrackingCloseTarget = false;
            }
            else if (mode.mTargetResults[aObjectId].mTargetReacquired && !mode.mTargetResults[aObjectId].mReacquirePossible)
            {
               allowTracking = false;
            }
         }
      }
      else
      {
         if (mode.mTargetResults[aObjectId].mTargetReacquired && !mode.mTargetResults[aObjectId].mReacquirePossible)
         {
            allowTracking = false;
         }
      }
   }

   if (mDebugEnabled && (aResult.GetTarget()->GetIndex() != aObjectId))
   {
      auto out = ut::log::debug() << "ctd_detection: Target allowed to track check.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Target: " << aResult.GetTarget()->GetName();
      out.AddNote() << "Allowed to Track: " << std::boolalpha << allowTracking;
   }

   return allowTracking;
}

// =================================================================================================
void WsfCTD_SensorComponent::TrackerDropTrack(double                 aSimTime,
                                              const TrackerSettings& aSettings,
                                              const WsfTrackId&      aRequestId,
                                              size_t                 aObjectId,
                                              WsfSensorMode*         aModePtr,
                                              WsfTrack*              aTrackPtr)
{
   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return;
   }

   // This is invoked directly or indirectly via WsfSensorTracker::TargetUndetected and TargetDeleted
   if (aSettings.mComponentRole == 0)
   {
      CTD_Mode& mode(mModeList[aModePtr->GetModeIndex()]);

      if (mode.mTargetResults[aObjectId].mTargetReacquired && !mode.mTargetResults[aObjectId].mReacquirePossible)
      {
         mode.mTargetResults[aObjectId].mReacquirePossible   = true;
         mode.mTargetResults[aObjectId].mTargetReacquired    = false;
         mode.mTargetResults[aObjectId].mTrackingCloseTarget = false;
      }

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "ctd_detection: Track dropped:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
      }
   }
}

// =================================================================================================
void WsfCTD_SensorComponent::ProcessDeltaInputs(UtInput& aInput, CTD_Beam::Deltas& aDeltas)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "azimuth_delta")
      {
         double azAngle;
         aInput.ReadValueOfType(azAngle, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(azAngle, 0.0);
         aDeltas.mAzimuthDelta = azAngle;
      }
      else if (command == "elevation_delta")
      {
         double elAngle;
         aInput.ReadValueOfType(elAngle, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(elAngle, 0.0);
         aDeltas.mElevationDelta = elAngle;
      }
      else if (command == "range_delta")
      {
         double rangeDelta;
         aInput.ReadValueOfType(rangeDelta, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(rangeDelta, 0.0);
         aDeltas.mRangeDelta = rangeDelta;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
//! Return the pointer to the mode with the specified name.
//! @param aModeNameId The name of the mode to be returned.
//! @returns Pointer to the requested mode, or nullptr if the mode does not exist.
// private
WsfSensorMode* WsfCTD_SensorComponent::GetMode(WsfStringId aModeNameId) const
{
   WsfSensorMode* modePtr(nullptr);
   size_t         modeIndex = GetSensor()->GetModeIndex(aModeNameId);
   if (modeIndex < GetSensor()->GetModeCount())
   {
      modePtr = GetSensor()->GetModeEntry(modeIndex);
   }
   return modePtr;
}

// =================================================================================================
void WsfCTD_SensorComponent::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return;
   }

   for (auto& mode : mModeList)
   {
      for (auto& beam : mode.mBeamList)
      {
         beam.mCTD_Platforms.insert(aPlatformPtr->GetIndex());
      }
   }
}

// =================================================================================================
void WsfCTD_SensorComponent::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   if (!mEnabled || !mIsWsfRadarSensor)
   {
      return;
   }

   for (auto& mode : mModeList)
   {
      for (auto& beam : mode.mBeamList)
      {
         beam.mCTD_Platforms.erase(aPlatformPtr->GetIndex());
      }
      mode.mTargetResults.erase(aPlatformPtr->GetIndex());
   }
}

// =================================================================================================
void WsfCTD_SensorComponent::TurnOff(double aSimTime)
{
   ResetPlatforms();
}

// =================================================================================================
//! Return the pointer to the local mode with the specified name.
//! @param aModeNameId The name of the mode to be returned.
//! @returns Pointer to the requested mode. If the mode does not exist it will be created.
//! @note This function is only needed when the component is not yet initialized (i.e.: during
//! ProcessInput). During ProcessInput it will create mode list entries as needed. After initialization
//! one can use aSensorMode.GetModeIndex() and directly access the mode list.
// private
WsfCTD_SensorComponent::CTD_Mode& WsfCTD_SensorComponent::GetOrCreateCTD_Mode(const WsfSensorMode& aSensorMode)
{
   // The local modes are maintained in the same order as the real mode list. If the
   // new mode index is larger than the current list size then expand our list as needed.

   // See if a mode_template is being used and return the mTemplate if so.
   if (&aSensorMode == aSensorMode.GetSensor()->GetModeList()->GetModeTemplate())
   {
      return mTemplate;
   }

   // Newly created modes are copies of the template.
   size_t modeIndex(aSensorMode.GetModeIndex());
   if (modeIndex >= mModeList.size())
   {
      mModeList.resize(modeIndex + 1, mTemplate);
   }
   return mModeList[modeIndex];
}

// =================================================================================================
//! Return the pointer to the local beam that corresponds to the real sensor beam..
//! @param aSensorBeam The sensor beam whose CTD_Beam is to be returned.
//! @returns Pointer to the requested beam. If the beam does not exist it will be created.
//! @note This is only use during input processing.
// private
WsfCTD_SensorComponent::CTD_Beam& WsfCTD_SensorComponent::GetOrCreateCTD_Beam(const WsfSensorBeam& aSensorBeam)
{
   WsfSensorMode* modePtr = aSensorBeam.GetSensorMode();
   assert(modePtr != nullptr);
   CTD_Mode& mode(GetOrCreateCTD_Mode(*modePtr));
   assert(!mode.mBeamList.empty()); // A mode is always created with a beam

   // The local beams are maintained in the same order as the real beam list. If the
   // new beam index is larger than the current list size then expand our list as needed.
   // Newly created beams are copies of the first beam.
   size_t beamIndex(aSensorBeam.GetBeamIndex());
   if (beamIndex >= mode.mBeamList.size())
   {
      mode.mBeamList.resize(beamIndex + 1, mode.mBeamList[0]);
   }
   return mode.mBeamList[beamIndex];
}

// =================================================================================================
void WsfCTD_SensorComponent::ResetPlatforms()
{
   for (auto& mode : mModeList)
   {
      for (auto& beam : mode.mBeamList)
      {
         beam.mCTD_Platforms.clear();
      }
      mode.mTargetResults.clear();
   }
}

// =================================================================================================
// Nested class SensorMode
// =================================================================================================

// =================================================================================================
WsfCTD_SensorComponent::CTD_Mode::CTD_Mode()
   : mBeamList(1)
{
}

// =================================================================================================
WsfCTD_SensorComponent::CTD_Mode::CTD_Mode(const CTD_Mode& aSrc)
   : mBeamList(aSrc.mBeamList)
{
}

// =================================================================================================
WsfCTD_SensorComponent::CTD_Mode& WsfCTD_SensorComponent::CTD_Mode::operator=(const CTD_Mode& aRhs)
{
   if (this != &aRhs)
   {
      mBeamList = aRhs.mBeamList;
   }
   return *this;
}
