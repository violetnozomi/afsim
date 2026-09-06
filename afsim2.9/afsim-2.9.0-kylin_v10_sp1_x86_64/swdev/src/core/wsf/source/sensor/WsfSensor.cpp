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

#include "WsfSensor.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSensorSchedulerTypes.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfSingleSensorObserver.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfUtil.hpp"

// =================================================================================================
WsfSensor::WsfSensor(const WsfScenario& aScenario)
   : WsfArticulatedPart(aScenario, cCOMPONENT_ROLE<WsfSensor>())
   , mThreadSafe(true)
   , mModeListPtr(nullptr)
   , mSchedulerPtr(nullptr)
   , mTrackerPtr(nullptr)
   , mNextUpdateTime(0.0)
   , mMessageLength(0)
   , mMessagePriority(0)
   , mZoneAttenuationModifier(nullptr)
   , mIgnoredCategories()
   , mIgnoredSides()
   , mIgnoredDomains(0)
   , mIgnoreSameSide(false)
   , mComponents()
   , mSensorModeList()
   , mClass(0)
   , mUpdateInterval(0.0)
   , mIsSlave(false)
   , mShowCalibrationData(false)
   , mSendDis(true)
   , mRandom()
   , mContext(*aScenario.GetScriptContext(), "WsfSensor", "SENSOR")
{
   WsfPlatformPart::SetInitiallyTurnedOn(false);
   mComponents.SetParentOfComponents(this);
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfSensor::WsfSensor(const WsfSensor& aSrc)
   : WsfArticulatedPart(aSrc)
   , mThreadSafe(aSrc.mThreadSafe)
   , mModeListPtr(nullptr)
   , mSchedulerPtr(nullptr)
   , mTrackerPtr(nullptr)
   , mNextUpdateTime(0.0)
   , mMessageLength(aSrc.mMessageLength)
   , mMessagePriority(aSrc.mMessagePriority)
   , mZoneAttenuationModifier(aSrc.mZoneAttenuationModifier)
   , mIgnoredCategories(aSrc.mIgnoredCategories)
   , mIgnoredSides(aSrc.mIgnoredSides)
   , mIgnoredDomains(aSrc.mIgnoredDomains)
   , mIgnoreSameSide(aSrc.mIgnoreSameSide)
   , mComponents(aSrc.mComponents)
   , mSensorModeList()
   , mClass(aSrc.mClass)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mIsSlave(aSrc.mIsSlave)
   , mShowCalibrationData(aSrc.mShowCalibrationData)
   , mSendDis(aSrc.mSendDis)
   , mRandom()
   , mContext(aSrc.mContext)
   , mOnDetectionScriptPtr(aSrc.mOnDetectionScriptPtr)
{
   mComponents.SetParentOfComponents(this);
   if (aSrc.mModeListPtr != nullptr)
   {
      mModeListPtr = ut::make_unique<WsfSensorModeList>(*(aSrc.mModeListPtr));
      mModeListPtr->SetSensor(this);
   }
   if (aSrc.mSchedulerPtr != nullptr)
   {
      mSchedulerPtr = ut::clone(aSrc.mSchedulerPtr);
   }
   if (aSrc.mTrackerPtr != nullptr)
   {
      mTrackerPtr = ut::clone(aSrc.mTrackerPtr);
   }
}

// =================================================================================================
// virtual
WsfSensor::~WsfSensor()
{
   NotifyDeleted();
}

// ================================================================================================
const int* WsfSensor::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_SENSOR,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfSensor::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_SENSOR)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_ARTICULATED_PART)
   {
      return (WsfArticulatedPart*)this;
   }
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return (WsfPlatformPart*)this;
   }
   return nullptr;
}

// ================================================================================================
const char* WsfSensor::GetScriptClassName() const
{
   return "WsfSensor";
}

// =================================================================================================
//! Should detection chances against the specified target be allowed?
//!
//! Determine if the specified platform is a member of any category which is
//! to be ignored by this sensor. This is used to eliminate sensing chances that
//! are not of interest. Judicious use of this capability can greatly improve
//! the performance of the simulation.
//!
//! @param aPlatformPtr Pointer to the target platform.
//! @return 'true' if detection chances are allowed.  'false' if the target platform
//! was a member of any category that is ignored by this sensor.
bool WsfSensor::AllowDetectionChancesFor(WsfPlatform* aPlatformPtr)
{
   bool allowDetectionChances = true;
   if (aPlatformPtr->IsExternalShadow())
   {
      allowDetectionChances = false;
   }
   else if ((mIgnoredDomains & (1 << aPlatformPtr->GetSpatialDomain())) != 0)
   {
      allowDetectionChances = false;
   }
   else if (std::find(mIgnoredSides.begin(), mIgnoredSides.end(), aPlatformPtr->GetSideId()) != mIgnoredSides.end())
   {
      allowDetectionChances = false;
   }
   else if (mIgnoredCategories.Intersects(aPlatformPtr->GetCategories()))
   {
      allowDetectionChances = false;
   }
   return allowDetectionChances;
}

// =================================================================================================
//! Add the given category to those ignored by this sensor.
//!
//! @param aCategory The category to add to the set of ignored categories.
void WsfSensor::AddIgnoredCategory(const std::string& aCategory)
{
   mIgnoredCategories.JoinCategory(aCategory);
}

// =================================================================================================
//! Attempt to detect another platform.
//!
//! This provides the common interface used to determine if a sensor can detect another platform.
//! The base class implementation simply invokes the mode-specific AttemptsToDetect method for
//! the currently selected mode.
//!
//! @param aSimTime   [input] The current simulation time.
//! @param aTargetPtr [input] The target to attempt to detect.
//! @param aSettings  [input] A Settings object that indicates the
//! operational settings to be used for the detection attempt.
//! @param aResult   [output] A Result object that contains detailed
//! information about the result detection attempt.
//!
//! @returns true if the target can be detected. This should be the same as testing
//! ((aResult.mCheckedStatus != 0) && (aResult.mFailedStatus == 0)).
// virtual
bool WsfSensor::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult)
{
   assert(aSettings.mModeIndex < mSensorModeList.size());
   if (aSettings.mModeIndex >= mSensorModeList.size())
   {
      return false;
   }
   WsfSensorMode* modePtr = mSensorModeList[aSettings.mModeIndex];
   // Modify the base slewing limits with the mode-specific slewing limits.
   modePtr->UpdateSensorCueingLimits();
   return modePtr->AttemptToDetect(aSimTime, aTargetPtr, aSettings, aResult);
}

// ================================================================================================
// virtual
bool WsfSensor::PreInitialize(double aSimTime)
{
   bool ok = WsfArticulatedPart::PreInitialize(aSimTime);

   // Let component factories inject components.
   WsfComponentFactoryList<WsfSensor>::PreInitialize(GetScenario(), aSimTime, *this);

   for (auto component : mComponents)
   {
      ok &= component->PreInitialize(aSimTime);
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfSensor::Initialize(double aSimTime)
{
   bool ok = WsfArticulatedPart::Initialize(aSimTime);

   // Initialize the random number stream.  Each sensor has a different seed.
   unsigned int seed = GetSimulation()->GetRandom().Uniform<unsigned>();
   mRandom.SetSeed(seed);

   if (mIgnoreSameSide &&
       (std::find(mIgnoredSides.begin(), mIgnoredSides.end(), GetPlatform()->GetSideId()) == mIgnoredSides.end()))
   {
      mIgnoredSides.push_back(GetPlatform()->GetSideId());
   }

   if (mModeListPtr != nullptr)
   {
      // Initialize the mode list.
      ok &= mModeListPtr->Initialize();

      // Reduce future dynamic casting by extracting derived class mode pointers.
      mModeListPtr->GetDerivedModeList(mSensorModeList);

      // Initialize the modes.
      for (auto& modePtr : mSensorModeList)
      {
         ok &= modePtr->Initialize(aSimTime);
      }
   }

   // Initialize the sensor chance scheduler if one has been defined.
   if (mSchedulerPtr != nullptr)
   {
      ok &= mSchedulerPtr->Initialize(aSimTime, this, mTrackerPtr.get());
   }

   // Initialize the sensor tracker if one has been defined.
   if (mTrackerPtr != nullptr)
   {
      ok &= mTrackerPtr->Initialize(aSimTime, this, mSchedulerPtr.get());
   }

   // Initialize the components
   for (auto component : mComponents)
   {
      ok &= component->Initialize(aSimTime);
   }

   // Initialize script context
   ok &= mContext.Initialize(aSimTime, GetPlatform(), this);
   if (ok)
   {
      UtScript* scriptPtr = mContext.FindScript("OnSensorDetectionAttempt");
      if (scriptPtr != nullptr)
      {
         ok &= mContext.ValidateScript(scriptPtr, "bool", "WsfPlatform, WsfSensorInteraction");
         mOnDetectionScriptPtr = scriptPtr;
      }
   }

   mShowCalibrationData |= DebugEnabled();

   return ok;
}

// ============================================================================
// virtual
bool WsfSensor::Initialize2(double aSimTime)
{
   bool ok = WsfArticulatedPart::Initialize2(aSimTime);
   ok &= mContext.Initialize2(aSimTime);
   for (auto component : mComponents)
   {
      ok &= component->Initialize2(aSimTime);
   }
   return ok;
}

// =================================================================================================
// virtual
void WsfSensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Inform the scheduler that a platform has been added to the simulation
   if (mSchedulerPtr != nullptr)
   {
      if (AllowDetectionChancesFor(aPlatformPtr))
      {
         mSchedulerPtr->PlatformAdded(aSimTime, aPlatformPtr);
      }
   }

   for (auto component : mComponents)
   {
      component->PlatformAdded(aSimTime, aPlatformPtr);
   }

   WsfPlatformPart::PlatformAdded(aSimTime, aPlatformPtr);
}

// =================================================================================================
// virtual
void WsfSensor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // Inform the scheduler that a platform has been deleted from the simulation
   if (mSchedulerPtr != nullptr)
   {
      mSchedulerPtr->PlatformDeleted(aPlatformPtr);
   }

   for (auto component : mComponents)
   {
      component->PlatformDeleted(aPlatformPtr);
   }

   WsfPlatformPart::PlatformDeleted(aPlatformPtr);
}

// =================================================================================================
// See WsfComponent::PreInput about this.
void WsfSensor::PreInput()
{
   WsfComponentFactoryList<WsfSensor>::PreInput(GetScenario(), *this);
}

// =================================================================================================
// virtual
bool WsfSensor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "show_calibration_data")
   {
      mShowCalibrationData = true;
   }
   else if (command == "message_length")
   {
      aInput.ReadValueOfType(mMessageLength, UtInput::cDATA_SIZE);
      aInput.ValueGreaterOrEqual(mMessageLength, 0);
   }
   else if (command == "message_priority")
   {
      aInput.ReadValue(mMessagePriority);
      aInput.ValueGreaterOrEqual(mMessagePriority, 0);
   }
   else if (command == "ignore")
   {
      std::string category;
      aInput.ReadValue(category);
      mIgnoredCategories.JoinCategory(WsfStringId(category));
   }
   else if (command == "ignore_side")
   {
      std::string side;
      aInput.ReadValue(side);
      mIgnoredSides.emplace_back(side);
   }
   else if (command == "ignore_domain")
   {
      std::string domain;
      aInput.ReadValue(domain);
      WsfSpatialDomain domainEnum;
      if (!WsfTypes::StringToEnum(domain, domainEnum))
      {
         throw UtInput::BadValue(aInput);
      }
      mIgnoredDomains |= (1 << domainEnum);
   }
   else if (command == "ignore_same_side")
   {
      mIgnoreSameSide = true;
   }
   else if (command == "ignore_nothing")
   {
      mIgnoredCategories.Clear();
      mIgnoredDomains = 0;
      mIgnoredSides.clear();
      mIgnoreSameSide = false;
   }
   else if (command == "modifier_category")
   {
      std::string category;
      aInput.ReadValue(category);
      mZoneAttenuationModifier = category;
   }
   else if (command == "output_dis")
   {
      aInput.ReadValue(mSendDis);
   }
   else if (command == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
      aInput.ValueGreater(mUpdateInterval, 0.0);
   }
   else if (WsfArticulatedPart::ProcessInput(aInput))
   {
   }
   else if (mContext.ProcessInput(aInput))
   {
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
      // NOTE: This needs to stay before the mode list process input check
   }
   else if ((mModeListPtr != nullptr) && mModeListPtr->ProcessInput(aInput))
   {
   }
   else if ((mTrackerPtr != nullptr) && mTrackerPtr->ProcessInput(aInput))
   {
   }
   else if (WsfSensorSchedulerTypes::LoadInstance(aInput, mSchedulerPtr))
   {
   }
   else if (command == "debug_scheduler")
   {
      if (mSchedulerPtr != nullptr)
      {
         mSchedulerPtr->SetDebugEnabled(true);
      }
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
      // First try components already attached. If the input was not recognized by one of them then
      // try the component factories, which may result in the dynamic creation of a new component.
   }
   else if (WsfComponentFactoryList<WsfSensor>::ProcessInput(GetScenario(), aInput, *this))
   {
      // If not processed by an attached component try the component factories to see if a component
      // should be dynamically attached.
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Update the state of the sensor.
//! This method should perform any actions that are necessary so that the system
//! reflects its condition at the supplied time.
//! //! @param aSimTime The current simulation time.
// virtual
void WsfSensor::Update(double aSimTime)
{
   WsfArticulatedPart::UpdatePosition(aSimTime); // Ensure the position is current
   mContext.Update(aSimTime);
}

void WsfSensor::UpdatePosition(double aSimTime)
{
   auto modePtr = GetCurrentMode();
   if (modePtr != nullptr)
   {
      modePtr->UpdateSensorCueingLimits();
   }
   WsfArticulatedPart::UpdatePosition(aSimTime);
}

// =================================================================================================
//! Turn the sensor off.
//!
//! Sensing chances will no longer be performed.
//!
//! @param aSimTime The current simulation time.
//!
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnSensorOff method.
// virtual
void WsfSensor::TurnOff(double aSimTime)
{
   if (IsTurnedOn())
   {
      // Turn off in the following order:
      // 1. Sensor components, as they may need to access sensor data before actual sensor turn off.
      // 2. Scheduler & Tracker (the default tracker needs to issue callbacks based on current sensor data).
      // 3. Sensor Modes.
      // 4. Base class' TurnOff method.
      for (auto component : mComponents)
      {
         component->TurnOff(aSimTime);
      }
      if (mSchedulerPtr != nullptr)
      {
         mSchedulerPtr->TurnOff(aSimTime);
      }
      if (mTrackerPtr != nullptr)
      {
         mTrackerPtr->TurnOff(aSimTime);
      }
      if (mModeListPtr != nullptr)
      {
         mModeListPtr->TurnOff(aSimTime);
      }
      WsfPlatformPart::TurnOff(aSimTime);
      NotifyTurnedOff(aSimTime);
   }
}

// =================================================================================================
//! Turn the sensor on.
//!
//! Sensing chances can now be performed.
//!
//! @param aSimTime The current simulation time.
//!
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnSensorOn method.
// virtual
void WsfSensor::TurnOn(double aSimTime)
{
   if (!IsTurnedOn())
   {
      if (mModeListPtr != nullptr)
      {
         mModeListPtr->TurnOn(aSimTime);

         // We need to make sure the cue mode and cue limits are set so the orientation will be computed
         // correctly even if there are no targets (otherwise the visuals are not correct). The values
         // are first reset to the default values. Then, if a single search mode is active the values will
         // be set to the mode-specific values (Don't know what to do if multi-search!)

         SetCueMode(GetSlewMode());
         SetAzCueLimits(GetMinAzSlew(), GetMaxAzSlew());
         SetElCueLimits(GetMinElSlew(), GetMaxElSlew());

         WsfSensorMode* modePtr = nullptr;
         for (size_t modeIndex = 0; modeIndex < mModeListPtr->GetModeCount(); ++modeIndex)
         {
            WsfSensorMode* tempModePtr = GetModeEntry(modeIndex);
            if (tempModePtr->IsSelected() && (tempModePtr->GetMaximumRequestCount() == 0))
            {
               if (modePtr != nullptr)
               {
                  modePtr = nullptr; // Multiple search modes active - don't do anything.
                  break;
               }
               modePtr = tempModePtr;
            }
         }
         if (modePtr != nullptr)
         {
            modePtr->UpdateSensorCueingLimits();
         }
      }
      if (mSchedulerPtr != nullptr)
      {
         mSchedulerPtr->TurnOn(aSimTime);
      }
      if (mTrackerPtr != nullptr)
      {
         mTrackerPtr->TurnOn(aSimTime);
      }
      mNextUpdateTime = aSimTime;
      WsfPlatformPart::TurnOn(aSimTime);
      for (auto component : mComponents)
      {
         component->TurnOn(aSimTime);
      }
      NotifyTurnedOn(aSimTime);
   }
}

// =================================================================================================
// virtual
void WsfSensor::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::SensorNonOperational(GetSimulation())(aSimTime, this);
}

// =================================================================================================
// virtual
void WsfSensor::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::SensorOperational(GetSimulation())(aSimTime, this);
}

// =================================================================================================
// virtual
void WsfSensor::PartBroken(double aSimTime)
{
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::SensorBroken(GetSimulation())(aSimTime, this);
}

// =================================================================================================
//! Return the number of receivers associated with the sensor.
//!
//! @return The number of receivers associated with the sensor.  This can be zero.
// virtual
size_t WsfSensor::GetEM_RcvrCount() const
{
   return 0;
}

// =================================================================================================
//! Get the attributes for the receiver with a specified index.
//!
//! @param aIndex The index of the receiver whose attributes are being requested.
//! This must be in the range [0, GetEM_RcvrCount() - 1].
//!
//! @note It is an error to call this method if the sensor does not have any receivers
//! (i.e.: GetEM_RcvrCount() == 0).
// virtual
WsfEM_Rcvr& WsfSensor::GetEM_Rcvr(size_t aIndex) const
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Rcvr dummyRcvr(WsfEM_Rcvr::cRF_SENSOR);
   return dummyRcvr;
}

// ================================================================================================
//! Return the number of receivers associated with the sensor.
//!
//! @param aModeIndex The index of the mode containing the receiver.
//! @return The number of receivers associated with the sensor.  This can be zero.
// virtual
size_t WsfSensor::GetEM_RcvrCount(size_t aModeIndex) const
{
   return 0;
}

// ================================================================================================
//! Get the attributes for the receiver with a specified index.
//!
//! @param aModeIndex The index of the mode containing the receiver.
//! @param aIndex     The index of the receiver whose attributes are being requested.
//! This must be in the range [0, GetEM_RcvrCount() - 1].
//!
//! @note It is an error to call this method if the sensor does not have any transmitters
//! (i.e.: GetEM_RcvrCount() == 0).
// virtual
WsfEM_Rcvr& WsfSensor::GetEM_Rcvr(size_t aModeIndex, size_t aIndex) const
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Rcvr dummyRcvr(WsfEM_Rcvr::cRF_SENSOR);
   return dummyRcvr;
}

// =================================================================================================
//! Return the number of transmitters associated with the sensor.
//!
//! @return The number of transmitters associated with the sensor.  This can be zero.
// virtual
size_t WsfSensor::GetEM_XmtrCount() const
{
   return 0;
}

// =================================================================================================
//! Get the attributes for the transmitter with a specified index.
//!
//! @param aIndex The index of the transmitter whose attributes are being requested.
//! This must be in the range [0, GetEM_XmtrCount() - 1].
//!
//! @note It is an error to call this method if the sensor does not have any transmitters
//! (i.e.: GetEM_XmtrCount() == 0).
// virtual
WsfEM_Xmtr& WsfSensor::GetEM_Xmtr(size_t aIndex) const
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_SENSOR);
   return dummyXmtr;
}

// ================================================================================================
//! Return the number of transmitters associated with the sensor.
//!
//! @param aModeIndex The index of the mode containing the transmitter.
//! @return The number of transmitters associated with the sensor.  This can be zero.
// virtual
size_t WsfSensor::GetEM_XmtrCount(size_t aModeIndex) const
{
   return 0;
}

// ================================================================================================
//! Get the attributes for the transmitter with a specified index.
//!
//! @param aModeIndex The index of the mode containing the transmitter.
//! @param aIndex     The index of the transmitter whose attributes are being requested.
//! This must be in the range [0, GetEM_XmtrCount() - 1].
//!
//! @note It is an error to call this method if the sensor does not have any transmitters
//! (i.e.: GetEM_XmtrCount() == 0).
// virtual
WsfEM_Xmtr& WsfSensor::GetEM_Xmtr(size_t aModeIndex, size_t aIndex) const
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_SENSOR);
   return dummyXmtr;
}

// =================================================================================================
// virtual
bool WsfSensor::WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool withinFieldOfView(false);

   // Update the location of the platform
   aPlatformPtr->Update(aSimTime);

   size_t rcvrCount = GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr)
      {
         double trueThisToTgtAz(0.0);
         double trueThisToTgtEl(0.0);
         double apparentThisToTgtLocWCS[3] = {0.0};
         double apparentThisToTgtAz(0.0);
         double apparentThisToTgtEl(0.0);
         double apparentTgtToThisLocWCS[3] = {0.0};

         if (antennaPtr->WithinFieldOfView(aPlatformPtr,
                                           1.0,
                                           trueThisToTgtAz,
                                           trueThisToTgtEl,
                                           apparentThisToTgtLocWCS,
                                           apparentThisToTgtAz,
                                           apparentThisToTgtEl,
                                           apparentTgtToThisLocWCS))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// =================================================================================================
// virtual
bool WsfSensor::WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr)
{
   bool withinFieldOfView(false);

   size_t rcvrCount = GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         // WithinFieldOfView output
         double trueThisToTgtAz(0.0);
         double trueThisToTgtEl(0.0);
         double apparentThisToTgtLocWCS[3] = {0.0};
         double apparentThisToTgtAz(0.0);
         double apparentThisToTgtEl(0.0);
         double apparentTgtToThisLocWCS[3] = {0.0};

         // WithinFieldOfView input
         double tgtLocWCS[3] = {0.0};
         aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS);

         if (antennaPtr->WithinFieldOfView(tgtLocWCS,
                                           1.0,
                                           trueThisToTgtAz,
                                           trueThisToTgtEl,
                                           apparentThisToTgtLocWCS,
                                           apparentThisToTgtAz,
                                           apparentThisToTgtEl,
                                           apparentTgtToThisLocWCS))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// =================================================================================================
//! Return the string ID of the string associated with the name of the current mode.
// virtual
WsfStringId WsfSensor::GetCurrentModeName() const
{
   WsfStringId nameId = WsfModeList::GetDefaultModeNameId();
   if (mModeListPtr)
   {
      nameId = mModeListPtr->GetModeNameId(mModeListPtr->GetCurrentMode());
   }
   return nameId;
}

// =================================================================================================
// virtual
WsfSensorMode* WsfSensor::GetCurrentMode() const
{
   WsfSensorMode* modePtr = nullptr;
   if (mModeListPtr != nullptr)
   {
      modePtr = GetModeEntry(mModeListPtr->GetCurrentMode());
   }
   return modePtr;
}

// =================================================================================================
//! Get the number of modes associated with the sensor.
// virtual
size_t WsfSensor::GetModeCount() const
{
   size_t count = 1;
   if (mModeListPtr != nullptr)
   {
      count = mModeListPtr->GetModeCount();
   }
   return count;
}

// =================================================================================================
// virtual
WsfSensorMode* WsfSensor::GetModeEntry(size_t aModeIndex) const
{
   WsfSensorMode* modePtr = nullptr;
   if (aModeIndex < mSensorModeList.size())
   {
      modePtr = mSensorModeList[aModeIndex];
   }
   return modePtr;
}

// =================================================================================================
//! Return the index of the mode with the specified name (ID).
//! @param aModeNameId The string ID of the name whose index is requested.
//! If this value is zero then 0 will be returned.
//! @returns If 0 was specified for aModeNameId then 0 will be returned.
//! If the mode exists the value will be the index of the mode in the
//! range 0 <= index < GetModeCount(). If the mode does not exist then the value
//! of GetModeCount() will be returned.
// virtual
size_t WsfSensor::GetModeIndex(WsfStringId aModeNameId) const
{
   size_t modeIndex = 0;
   if ((mModeListPtr) && aModeNameId != 0)
   {
      modeIndex = mModeListPtr->GetModeByName(aModeNameId);
   }
   return modeIndex;
}

// =================================================================================================
//! Return the string ID of the string associated with the name of the mode with the specified index.
//! @param aModeIndex The index of the desired mode such that 0 <= aModeIndex < GetModeCount().
// virtual
WsfStringId WsfSensor::GetModeName(size_t aModeIndex) const
{
   WsfStringId nameId = WsfModeList::GetDefaultModeNameId();
   if ((mModeListPtr) && (aModeIndex < mModeListPtr->GetModeCount()))
   {
      nameId = mModeListPtr->GetModeNameId(aModeIndex);
   }
   return nameId;
}

// =================================================================================================
//! Deselect the sensor mode given the string ID of the name of the desired mode.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The string ID of the mode to be deselected.
//! @note This method does nothing unless a the sensor is a multi-mode sensor.
//! Deselecting a mode with a single-mode sensor is accomplished by selecting a new mode.
// virtual
void WsfSensor::DeselectMode(double aSimTime, WsfStringId aModeNameId)
{
   if ((mModeListPtr != nullptr) && mModeListPtr->CanMultiSelect())
   {
      mModeListPtr->Deselect(aSimTime, aModeNameId);
      if (mSchedulerPtr != nullptr)
      {
         mSchedulerPtr->ModeDeselected(aSimTime, aModeNameId);
      }
      size_t modeIndex = mModeListPtr->GetModeByName(aModeNameId);
      if (modeIndex < mModeListPtr->GetModeCount())
      {
         for (auto component : mComponents)
         {
            component->DeselectMode(aSimTime, aModeNameId);
         }
         WsfObserver::SensorModeDeactivated(GetSimulation())(aSimTime, this, mSensorModeList[modeIndex]);
      }
   }
}

// =================================================================================================
//! Select the sensor mode given the string ID of the name of the desired mode.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The string ID of the mode to be selected.
// virtual
void WsfSensor::SelectMode(double aSimTime, WsfStringId aModeNameId)
{
   if (mModeListPtr != nullptr)
   {
      size_t oldModeIndex = mModeListPtr->GetCurrentMode();
      mModeListPtr->Select(aSimTime, aModeNameId);
      size_t modeIndex = mModeListPtr->GetModeByName(aModeNameId);

      // Selecting a new mode for a single-select sensor implicitly deselects the old mode.
      if ((oldModeIndex < mModeListPtr->GetModeCount()) && (!mModeListPtr->CanMultiSelect()))
      {
         if (mSchedulerPtr != nullptr)
         {
            mSchedulerPtr->ModeDeselected(aSimTime, mModeListPtr->GetModeNameId(oldModeIndex));
         }
         for (auto component : mComponents)
         {
            component->DeselectMode(aSimTime, mModeListPtr->GetModeNameId(oldModeIndex));
         }
         WsfObserver::SensorModeDeactivated(GetSimulation())(aSimTime, this, mSensorModeList[oldModeIndex]);
      }

      if (mSchedulerPtr != nullptr)
      {
         mSchedulerPtr->ModeSelected(aSimTime, aModeNameId);
      }
      if (modeIndex < mModeListPtr->GetModeCount())
      {
         // If this is a search mode, set the mode-specific cueing mode and limits so the baseline
         // sensor orientation will get computed even if there are no targets to detect.
         //
         // Also see the logic in 'TurnOn'.
         WsfSensorMode* modePtr = GetModeEntry(modeIndex);
         if (modePtr->GetMaximumRequestCount() == 0)
         {
            modePtr->UpdateSensorCueingLimits();
         }
         for (auto component : mComponents)
         {
            component->SelectMode(aSimTime, aModeNameId);
         }
         WsfObserver::SensorModeActivated(GetSimulation())(aSimTime, this, mSensorModeList[modeIndex]);
      }
   }
}

// =================================================================================================
// virtual
double WsfSensor::GetModeSelectDelay() const
{
   double modeSelectDelay = 0.0;
   if (mModeListPtr != nullptr)
   {
      modeSelectDelay = mModeListPtr->GetModeSelectDelay();
   }
   return modeSelectDelay;
}

// =================================================================================================
//! Schedule a mode change using the simulation event process.
//! @param aSimTime The current simulation time.
//! @param aSelectModeFlag The current mode selection flag, 'true' = select mode, 'false' = deselect mode
//! @param aModeNameId The mode name ID to change to.
// virtual
void WsfSensor::ScheduleModeChange(double aSimTime, bool aSelectModeFlag, WsfStringId aModeNameId)
{
   ModeChangeEvent::EventType eventType = ModeChangeEvent::cSELECT;
   if (!aSelectModeFlag)
   {
      eventType = ModeChangeEvent::cDESELECT;
   }
   GetSimulation()->AddEvent(
      ut::make_unique<ModeChangeEvent>(aSimTime + mModeListPtr->GetModeSelectDelay(), eventType, this, aModeNameId));
   mModeListPtr->SetModeChangeScheduled(true);
}

// =================================================================================================
// virtual
bool WsfSensor::IsModeChangeScheduled() const
{
   return mModeListPtr->IsModeChangeScheduled();
}

// =================================================================================================
//! Get the number of requests being maintained in all modes.
int WsfSensor::GetActiveRequestCount() const
{
   int count = 0;
   if ((mModeListPtr) && (mSchedulerPtr))
   {
      for (size_t modeIndex = 0; modeIndex < mModeListPtr->GetModeCount(); ++modeIndex)
      {
         count += mSchedulerPtr->GetActiveRequestCount(mModeListPtr->GetModeNameId(modeIndex));
      }
   }
   return count;
}

// =================================================================================================
//! Get the number of requests the sensor mode is currently maintaining.
//! @param aModeNameId The name (string ID) of the mode.
//! @returns The number of track requests that are currently being maintained for the specified mode.
// virtual
int WsfSensor::GetActiveRequestCount(WsfStringId aModeNameId) const
{
   int count = 0;
   if (mSchedulerPtr != nullptr)
   {
      count = mSchedulerPtr->GetActiveRequestCount(aModeNameId);
   }
   return count;
}

// =================================================================================================
//! Get the maximum number of requests that can be maintained in all modes.
//! @note This does not consider possible interactions between modes that may limit the number of requests.
// virtual
int WsfSensor::GetMaximumRequestCount() const
{
   int count = 0;
   if ((mModeListPtr) && (mSchedulerPtr))
   {
      for (size_t modeIndex = 0; modeIndex < mModeListPtr->GetModeCount(); ++modeIndex)
      {
         count += mSchedulerPtr->GetMaximumRequestCount(mModeListPtr->GetModeNameId(modeIndex));
      }
   }
   return count;
}

// =================================================================================================
//! Get the maximum number of requests the sensor mode can maintain.
//! @param aModeNameId The name (string ID) of the mode.
//! @returns The maximum number of track requests that can be maintained by the specified mode.
// virtual
int WsfSensor::GetMaximumRequestCount(WsfStringId aModeNameId) const
{
   int count = 0;
   if (mSchedulerPtr != nullptr)
   {
      count = mSchedulerPtr->GetMaximumRequestCount(aModeNameId);
   }
   return count;
}

// =================================================================================================
//! Has a tracking request for the specified track ID been submitted.
// virtual
bool WsfSensor::HaveRequestFor(const WsfTrackId& aTrackId) const
{
   bool haveRequest = false;
   if (mSchedulerPtr != nullptr)
   {
      haveRequest = mSchedulerPtr->HaveRequestFor(aTrackId);
   }
   return haveRequest;
}

// =================================================================================================
//! Request that the sensor start tracking the specified track.
//! @param aSimTime    The current simulation time.
//! @param aTrack      The track from which the sensor is initially supposed to start looking.
//! @param aModeNameId The mode to be used tracking.
//! @return 'true' if the tracking request was honored or 'false' if not.
// virtual
bool WsfSensor::StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId)
{
   bool startedTracking = false;
   if (mSchedulerPtr != nullptr)
   {
      startedTracking = mSchedulerPtr->StartTracking(aSimTime, aTrack, aModeNameId);
   }
   return startedTracking;
}

// =================================================================================================
//! Indicate the sensor should no longer track the specified track.
// virtual
bool WsfSensor::StopTracking(double aSimTime, const WsfTrackId& aTrackId)
{
   bool stoppedTracking = false;
   if (mSchedulerPtr != nullptr)
   {
      stoppedTracking = mSchedulerPtr->StopTracking(aSimTime, aTrackId);
   }
   if (mTrackerPtr != nullptr)
   {
      mTrackerPtr->StopTracking(aSimTime, aTrackId);
   }
   return stoppedTracking;
}

// =================================================================================================
//! Return the number of active tracks the sensor is maintaining.
// virtual
int WsfSensor::GetActiveTrackCount() const
{
   int count = 0;
   if (mTrackerPtr != nullptr)
   {
      count = mTrackerPtr->GetActiveTrackCount();
   }
   return count;
}

// =================================================================================================
//! Return the active tracks the sensor is maintaining.
// virtual
void WsfSensor::GetActiveTrackList(WsfTrackList& aActiveTrackList) const
{
   if (mTrackerPtr != nullptr)
   {
      mTrackerPtr->GetActiveTrackList(aActiveTrackList);
   }
}

// =================================================================================================
//! Return the maximum number of tracks the sensor can maintain.
// virtual
int WsfSensor::GetMaximumTrackCount() const
{
   int count = 0;
   if (mTrackerPtr != nullptr)
   {
      count = mTrackerPtr->GetMaximumTrackCount();
   }
   return count;
}

// =================================================================================================
//! A common method to inform interested parties that the track has been coasted.
//! This will inform simulation observers and attached processors that a track has been dropped.
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being dropped.
// virtual
void WsfSensor::SendTrackCoastMessage(double aSimTime, const WsfTrack* aTrackPtr)
{
   // Inform the simulation observers.
   WsfObserver::SensorTrackCoasted(GetSimulation())(aSimTime, this, aTrackPtr);

   // Send the message to all attached processors.
   //   WsfTrackCoastMessage message(GetPlatform(), aSimTime, aTrackPtr->GetTrackId(), aTrackPtr->GetTargetIndex());
   //   SendMessage(aSimTime, message);
}

// =================================================================================================
//! A common method to inform interested parties that the track has been dropped.
//! This will inform simulation observers and attached processors that a track has been dropped.
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being dropped.
// virtual
void WsfSensor::SendTrackDropMessage(double aSimTime, const WsfTrack* aTrackPtr)
{
   // Inform the simulation observers.
   WsfObserver::SensorTrackDropped(GetSimulation())(aSimTime, this, aTrackPtr);
   NotifyTrackDropped(aSimTime, *aTrackPtr);

   // Send the message to all attached processors.
   WsfTrackDropMessage message(GetPlatform(), aSimTime, aTrackPtr->GetTrackId(), aTrackPtr->GetTargetIndex());
   SendMessage(aSimTime, message);
}

// =================================================================================================
//! A common method to inform interested parties that the track has been updated.
//! This will inform simulation observers and attached processors that a track has been updated.
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being updated.
// virtual
void WsfSensor::SendTrackUpdateMessage(double aSimTime, const WsfTrack* aTrackPtr)
{
   // Inform simulation observers.
   if (aTrackPtr->GetUpdateCount() > 0)
   {
      WsfObserver::SensorTrackUpdated(GetSimulation())(aSimTime, this, aTrackPtr);
      NotifyTrackUpdated(aSimTime, *aTrackPtr);
   }
   else
   {
      WsfObserver::SensorTrackInitiated(GetSimulation())(aSimTime, this, aTrackPtr);
      NotifyTrackInitiated(aSimTime, *aTrackPtr);
   }

   // Send the message to all attached processors.

   WsfTrackMessage message(GetPlatform(), *aTrackPtr);
   SetMessageParameters(aTrackPtr->GetSensorModeId(), message);
   SendMessage(aSimTime, message);
}

// =================================================================================================
//! Set the message parameters.
//! This method will set the length and priority for the report message. The data will come from
//! either the mode or the base sensor.
//! @param aModeNameId The name (ID) of the mode producing the report message.
//! @param aMessage    The message whose length and priority are to be established.
// virtual
void WsfSensor::SetMessageParameters(WsfStringId aModeNameId, WsfMessage& aMessage)
{
   // Determine the size and priority of the message.
   int messageLength   = mMessageLength;
   int messagePriority = mMessagePriority;

   if (mModeListPtr != nullptr)
   {
      WsfSensorMode* modePtr = GetModeEntry(GetModeIndex(aModeNameId));
      if (modePtr != nullptr)
      {
         if (modePtr->GetMessageLength() > 0)
         {
            messageLength = modePtr->GetMessageLength();
         }
         if (modePtr->GetMessagePriority() > 0)
         {
            messagePriority = modePtr->GetMessagePriority();
         }
      }
   }

   aMessage.SetSizeBits(messageLength);
   aMessage.SetPriority(messagePriority);
}

// =================================================================================================
//! The sensor tracker has coasted a track.
//! This method is a hook that is called when a derived sensor (or one of it's constituents such as the tracker)
//! is coasting a track. The default method simply calls SendTrackCoastMessage inform simulation observers
//! and send a message to attached processors.
//!
//! This method is often provided by derived classes that want to use an existing sensor and simply want to
//! change how information is reported.
//!
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being coasted.
// virtual
void WsfSensor::TrackCoasted(double aSimTime, WsfTrack* aTrackPtr)
{
   if (!IsExternallyControlled())
   {
      SendTrackCoastMessage(aSimTime, aTrackPtr);
   }
}

// =================================================================================================
//! The sensor tracker has dropped a track.
//! This method is a hook that is called when a derived sensor (or one of it's constituents such as the tracker)
//! is dropping a track. The default method simply calls SendTrackDropMessage inform simulation observers
//! and send a message to attached processors.
//!
//! This method is often provided by derived classes that want to use an existing sensor and simply want to
//! change how information is reported.
//!
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being dropped.
// virtual
void WsfSensor::TrackDropped(double aSimTime, WsfTrack* aTrackPtr)
{
   if (!IsExternallyControlled())
   {
      SendTrackDropMessage(aSimTime, aTrackPtr);
   }
}

// =================================================================================================
//! The sensor tracker has updated a track.
//! This method is a hook that is called when a derived sensor (or one of it's constituents such as the tracker)
//! has updated a track. The default method simply calls SendTrackUpdateMessage inform simulation observers
//! and send a message to attached processors.
//!
//! This method is often provided by derived classes that want to use an existing sensor and simply want to
//! change how information is reported.
//!
//! @param aSimTime  [input] The current simulation time.
//! @param aTrackPtr [input] A pointer to the track being updated.
// virtual
void WsfSensor::TrackUpdated(double aSimTime, WsfTrack* aTrackPtr)
{
   if (!IsExternallyControlled())
   {
      SendTrackUpdateMessage(aSimTime, aTrackPtr);
   }
}

// =================================================================================================
//! Return the Pd required for the following detection event to be considered successful.
//!
//! This method is typically called during a Monte-Carlo simulation to select the Pd required for
//! a successful detection on an upcoming detection event. By default the returned value is
//! 1.0 - GetSimulation()->GetRandom().Uniform(), however the fixed Pd (stored within the sensor mode) may be selected
//! via simulation input ('use_constant_required_pd true') (thus eliminating any randomness)
//!
//! @note The 'required_pd' is meaningless for sensors that are binary detectors (i.e.: the Pd from a
//! detection attempt is either 0 or 1).
//!
//! @note This only applies to detection attempts made via Update()/PerformScheduledDetections()
//! (i.e.: during a standard simulation run). It does not apply for direct calls to AttemptToDetect().
// static
double WsfSensor::GetRequiredPd(const WsfSensorMode* aModePtr)
{
   double     requiredPd = aModePtr->GetRequiredPd();
   WsfSensor& sensor     = *aModePtr->GetSensor();
   if (!sensor.GetSimulation()->GetSimulationInput().UseConstantRequiredPd())
   {
      requiredPd = 1.0 - sensor.GetRandom().Uniform<double>();
   }
   return requiredPd;
}

// =================================================================================================
//! Adjust the 'Next Update Time' to account for time losses in the simulation.
//!
//! This is called by fixed-frame rate simulations when they need to indicate the
//! frame took too long.
void WsfSensor::AdjustNextUpdateTime(double aNextUpdateTime)
{
   mNextUpdateTime = std::max(aNextUpdateTime, mNextUpdateTime);
}

// =================================================================================================
//! Indicate if the sensor is a slave sensor.
//! @param aSimTime The current simulation time.
//! @param aIsSlave true if the sensor is to be declared a slave sensor.
void WsfSensor::SetIsSlave(double aSimTime, bool aIsSlave)
{
   if (mIsSlave != aIsSlave)
   {
      // Changing the sensor slave status while the sensor is on is not currently allowed because
      // the simulation objects need to be informed and the code changes have not been made. The
      // ability to do this isn't currently required...
      if (IsTurnedOn())
      {
         auto out =
            ut::log::warning() << "Changing sensor slave status is not currently allowed while the sensor is on.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetName();
         return;
      }
      mIsSlave = aIsSlave;
   }
}

// =================================================================================================
//! Set the 'Next Update Time'.
//!
//! This is called by sensor schedulers to perform an out-of-band update to the 'next update time'
//! (an out-of-band update is one that occurs outside Update()/PerformScheduledDetection()). This
//! typically occurs when the user makes a 'StartTracking' request. In such a case the scheduler
//! may need to want to force an Update to occur before the currently scheduled update (as defined
//! by 'next update time'.
void WsfSensor::SetNextUpdateTime(double aSimTime, double aNextUpdateTime)
{
   // The 'next update time' only needs to be changed if the new value is before the current value.
   // If it is later the worst that should happen is that there is a little overhead of doing something
   // that isn't needed.
   if (aNextUpdateTime < mNextUpdateTime)
   {
      mNextUpdateTime       = aNextUpdateTime;
      double updateInterval = std::max(aNextUpdateTime - aSimTime, 1.0E-5);
      GetSimulation()->SetPartUpdateInterval(aNextUpdateTime, this, updateInterval);
   }
}

// =================================================================================================
//! Process detections attempts under the control of a scheduler.
//!
//! This is a convenience method to process any required detections as directed by a sensor
//! scheduler.
//!
//! @param aSimTime The current simulation time.
// virtual
void WsfSensor::PerformScheduledDetections(double aSimTime)
{
   // Make sure that a scheduler and tracker are present.
   assert(mModeListPtr != nullptr);
   assert(mSchedulerPtr != nullptr);
   assert(mTrackerPtr != nullptr);

   WsfTrackId                 requestId;
   Settings                   settings;
   WsfSensorResult            result;
   WsfSensorTracker::Settings stSettings;
   size_t                     targetIndex = 0;

   while (mSchedulerPtr->SelectTarget(aSimTime, mNextUpdateTime, targetIndex, requestId, settings))
   {
      // Perform the sensing chance if the target still exists.
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr)
      {
         WsfSensorMode* modePtr = mSensorModeList[settings.mModeIndex];
         assert(modePtr != nullptr);
         settings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);
         if (modePtr->WithinDetectionRange(aSimTime, targetPtr))
         {
            if (AttemptToDetect(aSimTime, targetPtr, settings, result))
            {
               // Apply errors and indicate target is detected
               modePtr->ApplyMeasurementErrors(result);
               mTrackerPtr->TargetDetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
            }
            else
            {
               mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
            }
            NotifyTargetUpdated(aSimTime, targetPtr, result);
         }
         else
         {
            // Detection chance was culled because it is out of range. We must still report to the tracker
            // because it possible it may be coasting the target.
            result.Reset();
            result.mModeIndex     = settings.mModeIndex;
            result.mCheckedStatus = WsfSensorResult::cRCVR_RANGE_LIMITS;
            result.mFailedStatus  = WsfSensorResult::cRCVR_RANGE_LIMITS;
            mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
         }
      }
      else if (mTrackerPtr->TargetDeleted(aSimTime, stSettings, requestId, targetIndex))
      {
         // Target no longer physically exists and is not being tracked.
         mSchedulerPtr->RemoveTarget(aSimTime, targetIndex);
      }
      WsfArticulatedPart::ClearTransientCue(); // Release any transient cue created by the scheduler.
   }                                           // while (mSchedulerPtr->SelectTarget())

   // Set the update interval so the schedule is called at the desired time.
   double updateInterval = std::max(mNextUpdateTime - aSimTime, 1.0E-5);
   SetUpdateInterval(updateInterval);
}

// =================================================================================================
//! Set the 'classification' of the sensor.
// protected
void WsfSensor::SetClass(int aClass)
{
   mClass = aClass;
}

// =================================================================================================
//! Set the pointer to the 'mode list'.
//! @param aModeListPtr Pointer to the new mode list object. The sensor assumes
//! ownership of the object. Any existing mode list will be deleted.
// protected
void WsfSensor::SetModeList(std::unique_ptr<WsfSensorModeList> aModeListPtr)
{
   mModeListPtr = std::move(aModeListPtr);

   // Set the new owner of the mode list and attached modes.
   if (mModeListPtr != nullptr)
   {
      mModeListPtr->SetSensor(this);
   }
}

// =================================================================================================
//! Check whether a pre-detection script should allow or deny an attempted detection
// public
bool WsfSensor::ScriptAllowDetection(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   if (mOnDetectionScriptPtr)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContext.GetTypes());
      scriptArgs << aTargetPtr << aResult;
      mContext.ExecuteScript(aSimTime, mOnDetectionScriptPtr, scriptRetVal, scriptArgs);
      aResult.mCheckedStatus |= WsfSensorResult::cSCRIPT_CONSTRAINT;
      if (!scriptRetVal.GetBool())
      {
         aResult.mFailedStatus |= WsfSensorResult::cSCRIPT_CONSTRAINT;
      }
   }
   return aResult.Detected();
}

// =================================================================================================
//! Return the context from the script
UtScriptContext* WsfSensor::GetScriptAccessibleContext() const
{
   return &mContext.GetContext();
}

// =================================================================================================
//! Set the context's parent as soon as possible.
//! This allows scripts compiled later to use the parent's variables.
void WsfSensor::SetPlatform(WsfPlatform* aPlatformPtr)
{
   WsfArticulatedPart::SetPlatform(aPlatformPtr);
   if (aPlatformPtr != nullptr)
   {
      mContext.SetParent(&aPlatformPtr->GetScriptContext());
   }
}

// =================================================================================================
//! Define the object that is to assign sensing chances.
//!
//! @param aSchedulerPtr Pointer to the new scheduler object.  The sensor assumes ownership of the object.
//! Any existing scheduler will be deleted.
void WsfSensor::SetScheduler(std::unique_ptr<WsfSensorScheduler> aSchedulerPtr)
{
   mSchedulerPtr = std::move(aSchedulerPtr);
}

// =================================================================================================
//! Define the object that is to assign sensing chances.
//!
//! @param aSchedulerPtr Pointer to the new scheduler object. The sensor assumes ownership of the object.
//! @returns The pointer to the previous scheduler object. The caller assumes ownership of the object.
std::unique_ptr<WsfSensorScheduler> WsfSensor::SwapScheduler(std::unique_ptr<WsfSensorScheduler> aSchedulerPtr)
{
   std::swap(mSchedulerPtr, aSchedulerPtr);
   return aSchedulerPtr;
}

// =================================================================================================
//! Define the object that is to process detections and generate tracks.
//!
//! @param aTrackerPtr Pointer to the new tracker object. The sensor assumes ownership of the object.
//! Any existing tracker will be deleted.
void WsfSensor::SetTracker(std::unique_ptr<WsfSensorTracker> aTrackerPtr)
{
   mTrackerPtr = std::move(aTrackerPtr);
}

// =================================================================================================
//! Define the object that is to process detections and generate tracks.
//!
//! @param aTrackerPtr Pointer to the new tracker object. The sensor assumes ownership of the object.
//! @returns The pointer to the previous tracker object. The caller assumes ownership of the object.
std::unique_ptr<WsfSensorTracker> WsfSensor::SwapTracker(std::unique_ptr<WsfSensorTracker> aTrackerPtr)
{
   std::swap(mTrackerPtr, aTrackerPtr);
   return aTrackerPtr;
}

// =================================================================================================
void WsfSensor::AttachObserver(WsfSingleSensorObserver* aObserverPtr)
{
   std::vector<WsfSingleSensorObserver*>::iterator iter = std::find(mObservers.begin(), mObservers.end(), aObserverPtr);
   if (iter == mObservers.end())
   {
      mObservers.push_back(aObserverPtr);
   }
}

// =================================================================================================
void WsfSensor::DetachObserver(WsfSingleSensorObserver* aObserverPtr)
{
   std::vector<WsfSingleSensorObserver*>::iterator iter = std::find(mObservers.begin(), mObservers.end(), aObserverPtr);
   if (iter != mObservers.end())
   {
      mObservers.erase(iter);
   }
}

// =================================================================================================
void WsfSensor::NotifyDeleted()
{
   for (auto& observer : mObservers)
   {
      observer->OnSensorDeleted(this);
   }
}

// =================================================================================================
void WsfSensor::NotifyTrackInitiated(double aSimTime, const WsfTrack& aTrack)
{
   for (auto& observer : mObservers)
   {
      observer->OnSensorTrackInitiated(aSimTime, this, aTrack);
   }
}

// =================================================================================================
void WsfSensor::NotifyTrackUpdated(double aSimTime, const WsfTrack& aTrack)
{
   for (auto& observer : mObservers)
   {
      observer->OnSensorTrackUpdated(aSimTime, this, aTrack);
   }
}

// =================================================================================================
void WsfSensor::NotifyTrackDropped(double aSimTime, const WsfTrack& aTrack)
{
   for (auto& observer : mObservers)
   {
      observer->OnSensorTrackDropped(aSimTime, this, aTrack);
   }
}

// =================================================================================================
//!
//! Notify observers that a target update has just occurred.
//! @param aSimTime   [input] The current simulation time.
//! @param aTargetPtr [input] The target that is trying to be detected.
//! @param aResult    [input] On input this contains the current relative geometry information.
// virtual
void WsfSensor::NotifyTargetUpdated(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   for (auto& observer : mObservers)
   {
      observer->OnSensorTargetUpdated(aSimTime, this, aTargetPtr, aResult);
   }
}

void WsfSensor::NotifyTurnedOff(double aSimTime)
{
   WsfObserver::SensorTurnedOff(GetSimulation())(aSimTime, this);
   for (auto& observer : mObservers)
   {
      observer->OnSensorTurnedOff(aSimTime, this);
   }
}

void WsfSensor::NotifyTurnedOn(double aSimTime)
{
   WsfObserver::SensorTurnedOn(GetSimulation())(aSimTime, this);
   for (auto& observer : mObservers)
   {
      observer->OnSensorTurnedOn(aSimTime, this);
   }
}

// =================================================================================================
//!
//! Notify observers that a detection attempt has just occurred.
//! @param aSimTime   [input] The current simulation time.
//! @param aTargetPtr [input] The target that is trying to be detected.
//! @param aResult    [input] On input this contains the current relative geometry information.
// virtual
void WsfSensor::NotifySensorDetectionAttempted(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   // TODO: In some cases this function is invoked prior to WsfSensorComponent::PostAttemptToDetect, but in other cases
   // it is invoked afterwards. This is inconsistent behavior because PostAttemptToDetect actually does affect the
   // sensor result's detection status.
   WsfObserver::SensorDetectionAttempt(GetSimulation())(aSimTime, this, aTargetPtr, aResult);
   for (auto& observer : mObservers)
   {
      observer->OnSensorDetectionAttempted(aSimTime, this, aTargetPtr, aResult);
   }
}


// =================================================================================================
// Member function definitions for nested event classes
// =================================================================================================

WsfSensor::ModeChangeEvent::ModeChangeEvent()
   : WsfEvent()
   , mPlatformIndex(0)
   , mEventType(cSELECT)
   , mSensorPtr(nullptr)
   , mModeNameId(nullptr)
{
}

// =================================================================================================
//! Change mode event that changes a sensor's mode as an event.
WsfSensor::ModeChangeEvent::ModeChangeEvent(double aSimTime, EventType aEventType, WsfSensor* aSensorPtr, WsfStringId aModeNameId)
   : WsfEvent(aSimTime)
   , mEventType(aEventType)
   , mSensorPtr(aSensorPtr)
   , mModeNameId(aModeNameId)
{
   mPlatformIndex = mSensorPtr->GetPlatform()->GetIndex();
}

// =================================================================================================
WsfEvent::EventDisposition WsfSensor::ModeChangeEvent::Execute()
{
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      if (mSensorPtr != nullptr)
      {
         if (mEventType == cSELECT)
         {
            mSensorPtr->SelectMode(GetTime(), mModeNameId);
         }
         else if (mEventType == cDESELECT)
         {
            mSensorPtr->DeselectMode(GetTime(), mModeNameId);
         }
         mSensorPtr->GetModeList()->SetModeChangeScheduled(false);
      }
   }
   return cDELETE;
}
