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

#include "WsfDisInterface.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

#include "Dis.hpp"
#include "DisAppearance.hpp"
#include "DisAppearanceEnums.hpp"
#include "DisDatumEnums.hpp"
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisIO.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "DisVariableDatum.hpp"
#include "GenIP.hpp"
#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSleep.hpp"
#include "UtStringUtil.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfComm.hpp"
#include "WsfCommObserver.hpp"
#include "WsfDateTime.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEarthGravityModel.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfShadowMover.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfUtil.hpp"
#include "dis/WsfDisActionRequest.hpp"
#include "dis/WsfDisActionRequestR.hpp"
#include "dis/WsfDisArticulatedPart.hpp"
#include "dis/WsfDisDraw.hpp"
#include "dis/WsfDisEmission.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisFileDevice.hpp"
#include "dis/WsfDisFilterAllowance.hpp"
#include "dis/WsfDisMover.hpp"
#include "dis/WsfDisPduFactory.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisPlaybackDevice.hpp"
#include "dis/WsfDisSetData.hpp"
#include "dis/WsfDisUDP_Device.hpp"
#include "dis/WsfDisVisualization.hpp"
#include "ext/WsfExtEmission.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptContext.hpp"


DisUint16 WsfDisInterface::cSHADOW_SITE = 0x7FFF;

WsfDisInterface* WsfDisInterface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfDisInterface*>(aSimulation.FindExtension("dis_interface"));
}

// ============================================================================
WsfDisInterface::WsfDisInterface(WsfDisInput& aDIS_Input, WsfSimulation& aSim, WsfExtInterface* aExtInterfacePtr)
   : WsfDisInput(aDIS_Input)
   , mCallbacks()
   , mHeartbeatMultiplier(-2.4)
   , mIsDeferredConnection(false)
   , mIO_ThreadPtr(nullptr)
   , mHasInputDevice(false)
   , mHasOutputDevice(false)
   , mHasExternalDevice(false)
   , mCurrentGetDevice(0)
   , mPduFactoryPtr(nullptr)
   , mWsfDisExchange()
   , mPutCounter(0)
   , mSerialRequestNumber(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged)
   , mExternalEntityStatePtr(nullptr)
   , mDisPlatforms()
   , mEntityIdToDisPlatform()
   , mDelPlatformIndexToEntityId()
   , mEntityStateDelayFunctionPtr(nullptr)
   , mCreatePlatformFunctionPtr(nullptr)
   , mSimTime(0.0)
   , mLastSimTime(0.0)
   , mFilteringEnabled(false)
   , mSuppressDisOutput(false)
   , mPauseIsInProgress(false)
   , mReplayDrawPtr(nullptr)
   , mVisualizationPtr(nullptr)
   , mDeadReckonPtr(nullptr)
   , mNullEntityIdCount(0)
   , mSensorUpdateScheduled(false)
{
   mExtInterfacePtr = aExtInterfacePtr;
   assert(mExtInterfacePtr != nullptr);

   // If suppressing non-standard info then reset High Density Threshold back to 10 entities/beam per the DIS standard
   if (SuppressAllNonStandardPDUs())
   {
      Dis::SetTargetsInTjFieldDflt(10U);
   }

   Dis::Init();

   mIO_ThreadPtr     = new WsfDisIO_Thread(this);
   mPduFactoryPtr    = new WsfDisPduFactory(this);
   mVisualizationPtr = nullptr;

   mArticulatedPartListPtr = new WsfDisArticulatedPartList(this);

   // Subscribe to callbacks; these are applicable for both threaded and non-threaded
   mCallbacks.Add(WsfObserver::AdvanceTime(&aSim).Connect(&WsfDisInterface::AdvanceTime, this));
   mCallbacks.Add(WsfObserver::CommFrequencyChanged(&aSim).Connect(&WsfDisInterface::CommFrequencyChanged, this));
   mCallbacks.Add(WsfObserver::CommTurnedOff(&aSim).Connect(&WsfDisInterface::CommTurnedOff, this));
   mCallbacks.Add(WsfObserver::CommTurnedOn(&aSim).Connect(&WsfDisInterface::CommTurnedOn, this));

   mCallbacks.Add(WsfObserver::MessageReceived(&aSim).Connect(&WsfDisInterface::MessageReceived, this));
   mCallbacks.Add(WsfObserver::MessageTransmitted(&aSim).Connect(&WsfDisInterface::MessageTransmitted, this));
   mCallbacks.Add(WsfObserver::PlatformAdded(&aSim).Connect(&WsfDisInterface::PlatformAdded, this));
   mCallbacks.Add(WsfObserver::PlatformDeleted(&aSim).Connect(&WsfDisInterface::PlatformDeleted, this));
   mCallbacks.Add(WsfObserver::PlatformInitialized(&aSim).Connect(&WsfDisInterface::PlatformInitialized, this));
   mCallbacks.Add(WsfObserver::RequestTimeAdvance(&aSim).Connect(&WsfDisInterface::RequestTimeAdvance, this));
   mCallbacks.Add(WsfObserver::SensorDetectionChanged(&aSim).Connect(&WsfDisInterface::SensorDetectionChanged, this));
   mCallbacks.Add(WsfObserver::SensorFrequencyChanged(&aSim).Connect(&WsfDisInterface::SensorFrequencyChanged, this));
   mCallbacks.Add(WsfObserver::SensorModeActivated(&aSim).Connect(&WsfDisInterface::SensorModeActivated, this));
   mCallbacks.Add(WsfObserver::SensorModeDeactivated(&aSim).Connect(&WsfDisInterface::SensorModeDeactivated, this));
   mCallbacks.Add(WsfObserver::SensorRequestCanceled(&aSim).Connect(&WsfDisInterface::SensorRequestCanceled, this));
   mCallbacks.Add(WsfObserver::SensorRequestInitiated(&aSim).Connect(&WsfDisInterface::SensorRequestInitiated, this));
   mCallbacks.Add(WsfObserver::SensorRequestUpdated(&aSim).Connect(&WsfDisInterface::SensorRequestUpdated, this));
   mCallbacks.Add(WsfObserver::SensorScanUpdated(&aSim).Connect(&WsfDisInterface::SensorScanUpdated, this));
   mCallbacks.Add(WsfObserver::SensorTrackDropped(&aSim).Connect(&WsfDisInterface::SensorTrackDropped, this));
   mCallbacks.Add(WsfObserver::SensorTrackInitiated(&aSim).Connect(&WsfDisInterface::SensorTrackInitiated, this));
   mCallbacks.Add(WsfObserver::SensorTurnedOff(&aSim).Connect(&WsfDisInterface::SensorTurnedOff, this));
   mCallbacks.Add(WsfObserver::SensorTurnedOn(&aSim).Connect(&WsfDisInterface::SensorTurnedOn, this));
   mCallbacks.Add(WsfObserver::SimulationComplete(&aSim).Connect(&WsfDisInterface::SimulationComplete, this));
   mCallbacks.Add(WsfObserver::SimulationPausing(&aSim).Connect(&WsfDisInterface::SimulationPausing, this));
   mCallbacks.Add(WsfObserver::SimulationResuming(&aSim).Connect(&WsfDisInterface::SimulationResuming, this));
   mCallbacks.Add(WsfObserver::SimulationStarting(&aSim).Connect(&WsfDisInterface::SimulationStarting, this));

   // mDeviceConfigurations.push_back(DeviceConfiguration());

   mDeadReckonPtr = mExtInterfacePtr->GetEntityDeadReckon();
}

// ============================================================================
void WsfDisInterface::AddedToSimulation()
{
   for (auto component : mComponents)
   {
      component->AddedToDisInterface(this);
   }
}

// ============================================================================
bool WsfDisInterface::Initialize()
{
   for (auto component : mComponents)
   {
      if (!component->Initialize(0))
      {
         return false;
      }
   }
   return true;
}

// ============================================================================
// virtual
WsfDisInterface::~WsfDisInterface()
{
   SimulationComplete(mLastSimTime + 0.1);
   delete mIO_ThreadPtr;
   delete mPduFactoryPtr;

   delete mReplayDrawPtr;
   delete mVisualizationPtr;

   delete mArticulatedPartListPtr;
   mCallbacks.Clear();
   mConditionalCallbacks.Clear();

   WsfVisualization::Destroy();
   WsfExchange::Transactor::ResetNextSerialRequestId();
}

// ============================================================================
//! Activate the network connection.
//!
//! This method is called by WsfDisInterface::ActivateDeferredConnectionEvent::Execute(),
//! or during Initialize(), to activate the network connection.
bool WsfDisInterface::ActivateConnection(double aSimTime)
{
   // Check for no connections

   if (mDeviceConfigurations.empty() || (!mDeviceConfigurations.front().IsDefined()))
   {
      return true;
   }

   bool hasInput    = mHasInputDevice;
   bool hasOutput   = mHasOutputDevice;
   bool hasExternal = mHasExternalDevice;
   CleanupDevices();
   mHasInputDevice    = hasInput;
   mHasOutputDevice   = hasOutput;
   mHasExternalDevice = hasExternal;

   bool ok = true;

   // (Re)establish the network connection
   WsfDisPlaybackDevice::Reset(); // Reset next site number
   try
   {
      auto out = ut::log::info() << "Activating DIS connection.";
      out.AddNote() << "T = " << aSimTime;

      for (DeviceConfiguration& config : mDeviceConfigurations)
      {
         WsfDisDevice* devicePtr = nullptr;
         if (config.mDeviceType == WsfDisDevice::cDEVICE_MULTICAST)
         {
            devicePtr = new WsfDisUDP_Device(this,
                                             config.mAddress,
                                             config.mInterfaceAddress,
                                             config.mSendPort,
                                             config.mRecvPort,
                                             mMulticastTimeToLive);
            out.AddNote() << "Multicast: " << config.mAddress << " " << config.mInterfaceAddress;
            mDeadReckonPtr->UseInitialDistributionInterval();

            // if multicast, give the router time to update it's state table and
            // start sending us dis before returning control to the application.
            UtSleep::Sleep(0.25);
         }
         if (config.mDeviceType == WsfDisDevice::cDEVICE_BROADCAST)
         {
            GenSockets::GenIP addressIP(config.mAddress);
            if (!addressIP.IsValidForm() || !addressIP.IsBroadcast())
            {
               auto note = out.AddNote("error") << "Invalid broadcast IP address.";
               note.AddNote() << "Address: " << config.mAddress;
            }
            else
            {
               out.AddNote() << "Broadcast: " << config.mAddress;
            }
            devicePtr = new WsfDisUDP_Device(this, config.mAddress, config.mSendPort, config.mRecvPort);
            mDeadReckonPtr->UseInitialDistributionInterval();
         }
         else if (config.mDeviceType == WsfDisDevice::cDEVICE_UNICAST)
         {
            devicePtr = new WsfDisUDP_Device(this, config.mAddress, config.mSendPort, config.mRecvPort);
            out.AddNote() << "Unicast: " << config.mAddress;
            mDeadReckonPtr->UseInitialDistributionInterval();
         }
         else if (config.mDeviceType == WsfDisDevice::cDEVICE_RECORD)
         {
            std::string recordFile = GetSimulation().SubstituteOutputFileVariables(config.mAddress);
            devicePtr              = new WsfDisFileDevice(recordFile, this);

            // Send an initial date and time indication for proper
            // initialization of CME / IDE visualization tools.
            SendDateAndTime(*devicePtr, DisEntityId(), GetSimulation().GetDateTime().GetStartDateAndTime());
            out.AddNote() << "Record: " << recordFile;
         }
         else if (config.mDeviceType == WsfDisDevice::cDEVICE_PLAYBACK)
         {
            devicePtr = new WsfDisPlaybackDevice(config.mAddress, this);
            out.AddNote() << "Playback: " << config.mAddress;
         }
         if ((config.mDeviceType != WsfDisDevice::cDEVICE_RECORD) && (config.mDeviceType != WsfDisDevice::cDEVICE_PLAYBACK))
         {
            if (config.mSendPort != config.mRecvPort)
            {
               out.AddNote() << "Sending Port: " << config.mSendPort;
               out.AddNote() << "Receiving Port: " << config.mRecvPort;
            }
            else
            {
               out.AddNote() << "Port: " << config.mSendPort;
            }

            if (mJoinExercise)
            {
               // join exercise is used for a scaled-and-stepped NRT simulation. Per the IEEE std,
               // we need to wait for a start pdu
               mAutostart = false;
               GetSimulation().SetIsExternallyStarted(true);

               DisEntityId recvId(0xFFFF, 0xFFFF, 0xFFFF);
               DisPdu*     joinPduPtr = nullptr;
               if (GetReliabilityService() == DisEnum::Pdu::Reliability::Acknowledged)
               {
                  // send a reliable join exercise request pdu, default request 1x for 5 seconds
                  WsfDisActionRequestR* requestPtr = new WsfDisActionRequestR(this);
                  requestPtr->MakeJoinExerciseRequest(recvId,
                                                      aSimTime,
                                                      1.0,
                                                      5.0,
                                                      UtStringUtil::ToString((int)Dis::GetExercise()));
                  joinPduPtr = requestPtr;
               }
               else
               {
                  // send a join exercise request pdu, default request 1x for 5 seconds
                  WsfDisActionRequest* requestPtr = new WsfDisActionRequest(this);
                  requestPtr->MakeJoinExerciseRequest(recvId,
                                                      aSimTime,
                                                      1.0,
                                                      5.0,
                                                      UtStringUtil::ToString((int)Dis::GetExercise()));
                  joinPduPtr = requestPtr;
               }
               if (devicePtr)
               {
                  devicePtr->PutPdu(aSimTime, *joinPduPtr);
               }
            }
         }
         if (devicePtr != nullptr)
         {
            // initialize the filter allowances and add them to the device
            int numAllow = static_cast<int>(config.mFilterAllowances.size());
            for (int a = 0; a < numAllow; ++a)
            {
               // grab the entity types and force ids for any entity_pdu filters
               // they should be valid and initialized now
               WsfDisFilterAllowance* allowancePtr = config.mFilterAllowances[a];

               if (allowancePtr->mFilterType == WsfDisFilterAllowance::cENTITYTYPE)
               {
                  WsfExtEntityType entityType;
                  if (mExtInterfacePtr->GetEntityType(allowancePtr->mEntityTypeName, entityType))
                  {
                     allowancePtr->mEntityType = WsfDisExt::ToDis(entityType);
                  }
                  else
                  {
                     auto note = out.AddNote("error") << "Filter allowance error. Unknown entity type.";
                     note.AddNote() << "Entity Type: " << allowancePtr->mEntityTypeName;
                  }
               }

               if (allowancePtr->mFilterType == WsfDisFilterAllowance::cENTITYFORCE)
               {
                  allowancePtr->mEntityForceId = mExtInterfacePtr->GetForceId(allowancePtr->mEntitySide);
                  if (allowancePtr->mEntityForceId == 0)
                  {
                     auto note = out.AddNote("error") << "Filter allowance error. Unknown force.";
                     note.AddNote() << "Force: " << allowancePtr->mEntitySide;
                  }
               }

               if (allowancePtr->mTrackedFlag == WsfDisFilterAllowance::cTRACKED)
               {
                  allowancePtr->mTrackingForceId = mExtInterfacePtr->GetForceId(allowancePtr->mTrackingSide);
                  if (allowancePtr->mTrackingForceId == 0)
                  {
                     auto note = out.AddNote("error") << "Filter allowance error. Unknown tracking force.";
                     note.AddNote() << "Tracking Force: " << allowancePtr->mTrackingSide;
                  }
               }

               // save off, just in case
               config.mFilterAllowances[a] = allowancePtr;

               // add allowance to device
               devicePtr->AddFilterAllowance(allowancePtr);
            }

            mDevices.push_back(devicePtr);
         }
      }
   }
   catch (std::exception&)
   {
      ut::log::error() << "Unable to open dis_interface device.";
      ok = false;
   }

   if (ok && !mSuppressCME_DrawData && !mSuppressNonStandardData && !mReplayDrawPtr)
   {
      mReplayDrawPtr = new WsfDisDraw(mUseDeprecatedCME_DrawData, this);
   }
   if (ok && !mSuppressCME_DrawData && !mSuppressNonStandardData)
   {
      mVisualizationPtr = new WsfDisVisualization(this);
   }

   return ok;
}

// ============================================================================
// private
void WsfDisInterface::AddCallbacks()
{
   // Clear callbacks to make sure these aren't multiply connected
   mConditionalCallbacks.Clear();
   WsfExtEntityDeadReckon* deadReckon = mExtInterfacePtr->GetEntityDeadReckon();
   deadReckon->RequestService(true);
   mConditionalCallbacks.Add(deadReckon->MoverUpdated.Connect(&WsfDisInterface::MoverUpdated, this));
   mConditionalCallbacks.Add(WsfObserver::Comment(&GetSimulation()).Connect(&WsfDisInterface::Comment, this));
   // if (mMultiThreaded)
   //{
   //    mConditionalCallbacks.Add(GetSimulation().GetObserver().FrameComplete.Connect(&WsfDisInterface::FrameComplete, this));
   //    mConditionalCallbacks.Add(GetSimulation().GetObserver().FramePlatformsUpdated.Connect(&WsfDisInterface::FramePlatformsUpdated,
   //    this)); mConditionalCallbacks.Add(GetSimulation().GetObserver().FrameStarting.Connect(&WsfDisInterface::FrameStarting,
   //    this));
   // }
}

// ============================================================================
// private
void WsfDisInterface::ResumeEntityStateProcessing(double aSimTime, const DisEntityId& aEntityId)
{
   // Loop over all the DelayedEntityStates and see if we can find one that had the matching DisEntityId
   for (auto iter = mDelayedEntityStates.begin(); iter != mDelayedEntityStates.end();)
   {
      if ((*iter)->GetEntityId() == aEntityId)
      {
         // Check to see if the Entity State should continue to be delayed or not
         if ((mEntityStateDelayFunctionPtr == nullptr) || (!(*mEntityStateDelayFunctionPtr)(*this, (*iter)->GetEntityId())))
         {
            // Process the Entity state now, remove from mDelayedEntityStates and send to AddExternalPlatformP
            // The temp entityStatePtr is used in case AddExternalPlatformP() attempted to access mDelayedEntityStates
            auto entityStatePtr = std::move(*iter);
            iter                = mDelayedEntityStates.erase(iter);
            AddExternalPlatformP(aSimTime, std::move(entityStatePtr));
         }
         else
         {
            ++iter;
         }
      }
      else
      {
         ++iter;
      }
   }
}

// ============================================================================
//! Create a DIS platform extension that corresponds to the specified Entity State PDU.
//! @param aSimTime The current simulation time.
//! @param aEntityPtr The entity state PDU.
//! @return Pointer to the DIS platform extension for the PDU.
WsfDisPlatform* WsfDisInterface::AddExternalPlatform(double aSimTime, std::unique_ptr<WsfDisEntityState> aEntityPtr)
{
   if (mEntityStateDelayFunctionPtr != nullptr)
   {
      if ((*mEntityStateDelayFunctionPtr)(*this, aEntityPtr->GetEntityId()))
      {
         auto find = [&aEntityPtr](const std::unique_ptr<WsfDisEntityState>& entityState)
         { return entityState->GetEntityId() == aEntityPtr->GetEntityId(); };
         auto it = std::find_if(mDelayedEntityStates.begin(), mDelayedEntityStates.end(), find);
         // If an Entity State is found with this DisEntityId, then remove the old entity state (ensures uniqueness for
         // each DisEntityId)

         if (it != mDelayedEntityStates.end())
         {
            mDelayedEntityStates.erase(it);
         }
         // Add the new entity state to mDelayedEntityStates
         mDelayedEntityStates.push_back(std::move(aEntityPtr));
         return nullptr;
      }
   }
   return AddExternalPlatformP(aSimTime, std::move(aEntityPtr));
}

// ============================================================================
// private
WsfDisPlatform* WsfDisInterface::AddExternalPlatformP(double aSimTime, std::unique_ptr<WsfDisEntityState> aEntityPtr)
{
   // EM::If this platform is to be a DisMover, but is to be a 'local' platform so as to have
   // all its components driven by WSF, set it up as IsExternallyMoved, IsIndestructable,
   // and NOT IsExternallyControlled.  Have not decided about the naming convention BAR-TBD.

   bool isExternallyMoved = mExtInterfacePtr->IsEntityExternallyMoved(WsfDisExt::ToExt(aEntityPtr->GetEntityId()),
                                                                      WsfDisExt::ToExt(aEntityPtr->GetEntityType()));
   bool isPlaybackEntity  = WsfDisPlaybackDevice::IsPlaybackEntity(aEntityPtr->GetEntityId());
   WsfDisPlatform* disPlatformPtr = nullptr;

   std::ostringstream oss;
   if (!isExternallyMoved)
   {
      oss << "<dis>" << aEntityPtr->GetEntityId();
   }
   else
   {
      oss << "<disMvr>" << aEntityPtr->GetEntityId();
   }

   std::string  platformName(oss.str());
   WsfStringId  nameId      = platformName;
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByName(nameId);
   if (platformPtr == nullptr)
   {
      if ((!isPlaybackEntity) && (!mTerseMode))
      {
         auto out = ut::log::debug() << "Adding platform for external entity.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Entity: " << aEntityPtr->GetEntityId();
         out.AddNote() << "Type: " << aEntityPtr->GetEntityType();
      }

      // Find the WSF platform type associated with the entity type in the PDU.

      WsfStringId platformTypeId = mExtInterfacePtr->SelectPlatformType(WsfDisExt::ToExt(aEntityPtr->GetEntityType()));

      // Attempt to clone a platform of the type associated with the entity type.
      // If it fails then we'll create a default platform.

      const std::string& platformType(platformTypeId);

      // Clone a platform of the appropriate type.
      if (mCreatePlatformFunctionPtr != nullptr)
      {
         platformPtr = (*mCreatePlatformFunctionPtr)(*this, platformType, aEntityPtr.get());
      }

      if (platformPtr == nullptr)
      {
         platformPtr = mExtInterfacePtr->CreateExternalPlatform(platformType);
         // EM::if this is a Dis platform that is to be local and just to be a DisMover do not strip its components
         if (platformPtr != nullptr)
         {
            if (isPlaybackEntity)
            {
               WsfDisPlaybackDevice::PreparePlaybackPlatform(aSimTime, platformPtr, aEntityPtr.get());
            }
            else if (!isExternallyMoved)
            {
               WsfExtInterface::StripExternalPlatform(platformPtr);
            }

            // Create the platform name.
            platformPtr->SetName(platformName);
            platformPtr->SetSide(mExtInterfacePtr->GetSideId(aEntityPtr->GetForceId()));

            // Attach the DIS mover to this platform.  It will simply extrapolate from the last entity state.

            WsfDisMover* moverPtr = new WsfDisMover(this, platformPtr, aEntityPtr.get());
            platformPtr->SetMover(moverPtr);
         }
      }

      // When the simulation 'AddPlatform' is method it will end up resulting in a call to the PlatformAdded()
      // in this class (because this is a simulation observer).  We've got to tell that method to use the
      // supplied entity state rather than creating one.  Setting mExternalEntityStatePtr indicates that this
      // platform is externally controlled.

      // However, if this platform is just to be externally moved then make it NOT externally controlled, but
      // leave it as indestructible and set it to be isExternallyMoved.  This platform is a local platform
      // with all defined components.
      if (isExternallyMoved)
      {
         platformPtr->SetIsExternallyControlled(false);
         platformPtr->SetIsExternallyMoved(true);
         platformPtr->SetIsIndestructible(true);
      }
      else if (!isPlaybackEntity)
      {
         // Standard external entity
         platformPtr->SetIsExternallyControlled(true);
         platformPtr->SetIsIndestructible(true);
      }
      mExternalEntityStatePtr = std::move(aEntityPtr);

      if (GetSimulation().AddPlatform(aSimTime, platformPtr))
      {
         size_t platformIndex = platformPtr->GetIndex();
         disPlatformPtr       = mDisPlatforms[platformIndex];
         if (disPlatformPtr)
         {
            // Set for age out timing.
            disPlatformPtr->SetEntityStateUpdateTime(aSimTime);
         }

         // The following logic will create a local entity that 'shadows' to external entity.
         // This is useful for debugging to make sure that our perception of the platform location
         // represents what the entity state says it is...
         //
         // NOTE: The site part of the entity ID is set to cSHADOW_SITE = 0x7FFF.
         // We ignore these on receipt (see WsfDisEntityState)

         if (!isPlaybackEntity)
         {
            if ((mShadowAll) || (mShadowNameId.find(platformPtr->GetNameId()) != mShadowNameId.end()) ||
                (mShadowTypeId.find(platformPtr->GetTypeId()) != mShadowTypeId.end()))
            {
               AddShadowPlatform(aSimTime, platformPtr);
            }
         }
      }
      else
      {
         // AddPlatform failed, indicating that there was some problem initializing or adding the platform.
         delete platformPtr;
         platformPtr = nullptr;
      }
   }

   return disPlatformPtr;
}

// ============================================================================
// private
void WsfDisInterface::AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (HasDevice())
   {
      // mExternalEntityStatePtr will be set if this class is adding an externally controlled platform
      // If the new platform is externally controlled, but mExternalEntityStatePtr is not set, the platform
      // must belong to a different interface.

      // mExternalEntityStatePtr should be zero if this class isn't creating a DIS platform or
      // or this DIS platform is not being externally moved

#ifndef NDEBUG
      // Don't define unless assertions are on... otherwise we get compile warnings.
      bool shouldHaveEntityState = (aPlatformPtr->IsExternallyControlled() || aPlatformPtr->IsExternallyMoved());
#endif
      if (mExternalEntityStatePtr == nullptr)
      {
         // Platform shouldn't not be externally controlled or externally moved.
         assert(!shouldHaveEntityState);
      }
      else
      {
         // Platform should be externally controlled, externally moved, or be a playback entity.
         assert(shouldHaveEntityState || WsfDisPlaybackDevice::IsPlaybackEntity(mExternalEntityStatePtr->GetEntityId()));
      }

      size_t platformIndex = aPlatformPtr->GetIndex();

      // Make sure the vector is big enough for the new element

      size_t currentSize = mDisPlatforms.size();
      if (platformIndex >= currentSize)
      {
         // Double the size like the normal implementation of <vector>

         size_t newSize = 64;
         if (currentSize != 0)
         {
            newSize = currentSize + currentSize;
         }
         mDisPlatforms.resize(newSize, nullptr);
      }

      bool createDisPlatform = true;
      if (!aPlatformPtr->IsExternallyControlled())
      {
         createDisPlatform = !mExtInterfacePtr->IsPrivate(aPlatformPtr);
      }
      else
      {
         // Do not create a DIS platform if the platform is externally controlled by something else
         createDisPlatform = (mExternalEntityStatePtr != nullptr);
      }

      if (createDisPlatform)
      {
         // If the platform being created is in response to an incoming platform (object) transfer then the entity Id
         // and entity type must be the same as the ones from the Fire PDU.
         //
         // If the platform being created is in response to an outgoing platform (object) transfer then the platform is
         // created as normal but it is treated somewhat like an externally created entity. It will actually
         // be made external when the Fire PDU is issued.

         DisEntityId   entityId;
         DisEntityType entityType;
         bool          incomingTransfer = false;
         bool          outgoingTransfer = false;

         for (auto component : mComponents)
         {
            if (component != nullptr)
            {
               if (component->TransferPlatformToInternalControl(aSimTime, aPlatformPtr, entityId, entityType))
               {
                  incomingTransfer = true;
                  break;
               }
               else if (component->TransferPlatformToExternalControl(aSimTime, aPlatformPtr))
               {
                  outgoingTransfer = true;
                  break;
               }
            }
         }

         // If this is an incoming platform (object)  transfer then the initial entity state PDU may have already been
         // received. If so, we must get rid of the existing platform (it is probably marked external) as it will be
         // replaced

         if (incomingTransfer || outgoingTransfer)
         {
            // Transfer of external to internal (local) control via the Fire PDU.
            // or Transfer of from internal (local) control to external control.
         }
         else if (mExternalEntityStatePtr != nullptr)
         {
            entityId = mExternalEntityStatePtr->GetEntityId();
         }
         if (!entityId.IsDefined())
         {
            uint16_t entity(0);
            mExtInterfacePtr->SelectEntityId(aPlatformPtr, entity);
            entityId.SetEntity(entity);
            entityId.SetApplication(mExtInterfacePtr->GetApplicationId());
            entityId.SetSite(mExtInterfacePtr->GetSiteId());
         }

         // Create the DIS platform associated with this platform.
         WsfDisPlatform* disPlatformPtr = new WsfDisPlatform(aSimTime,
                                                             aPlatformPtr,
                                                             this,
                                                             std::move(mExternalEntityStatePtr),
                                                             entityId,
                                                             entityType,
                                                             incomingTransfer,
                                                             outgoingTransfer);
         mDisPlatforms[platformIndex]   = disPlatformPtr;

         // Set the marking field on the WsfPlatform if externally controlled
         if (aPlatformPtr->IsExternallyControlled())
         {
            DisUint8 markingChars[11] = {0};
            disPlatformPtr->GetEntityState()->GetMarkingChars(markingChars);
            std::string markingStr;
            for (unsigned char markingChar : markingChars)
            {
               markingStr.push_back((unsigned int)markingChar);
            }
            aPlatformPtr->SetMarking(markingStr);
         }

         // Also update the DIS Entity ID -> DIS Platform mapping.
         mEntityIdToDisPlatform[disPlatformPtr->GetEntityId()] = disPlatformPtr;

         for (auto component : mComponents)
         {
            component->PlatformCreated(disPlatformPtr);
         }

         if (mLogCreatedEntities)
         {
            auto out = ut::log::info() << "Created DIS entity.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Entity: " << disPlatformPtr->GetEntityId();
            out.AddNote() << "Type: " << disPlatformPtr->GetEntityType();
            out.AddNote() << "Force: " << static_cast<int>(disPlatformPtr->GetForceId())
                          << (aPlatformPtr->IsExternallyControlled() ? " (external)" : " (local)");
            out.AddNote() << "WSF Name: " << aPlatformPtr->GetName();
            out.AddNote() << "WSF Type: " << aPlatformPtr->GetType();
            out.AddNote() << "WSF Side: " << aPlatformPtr->GetSide();
         }
      }

      mExternalEntityStatePtr = nullptr;
   }
}

// ============================================================================
// private
void WsfDisInterface::AddShadowPlatform(double aSimTime, const WsfPlatform* aPlatformPtr)
{
   // NOTE: We don't want a clone of the input platform type because that would also give us all
   //       the associated systems. We only want a platform to which will we attach the shadow mover.
   WsfPlatform* shadowPtr = new WsfPlatform(GetScenario());
   if (shadowPtr != nullptr)
   {
      shadowPtr->SetIsExternallyControlled(false);
      shadowPtr->SetIsExternalShadow(true);
      shadowPtr->SetIsIndestructible(true);
      shadowPtr->SetConcealmentFactor(1.0F);
      std::string shadowName(aPlatformPtr->GetName());
      shadowName += "_shadow";
      shadowPtr->SetName(shadowName);
      shadowPtr->SetType(aPlatformPtr->GetType());
      shadowPtr->SetSide(aPlatformPtr->GetSide());
      shadowPtr->SetIcon(aPlatformPtr->GetIcon());
      WsfMover* shadowMoverPtr = new WsfShadowMover(aPlatformPtr);
      shadowPtr->SetMover(shadowMoverPtr);
      DisUint16 saveSite = Dis::GetSite();
      Dis::SetSite(cSHADOW_SITE);

      if (!GetSimulation().AddPlatform(aSimTime, shadowPtr))
      {
         ut::log::warning() << "Unable to add shadow platform to simulation";
         delete shadowPtr;
      }
      Dis::SetSite(saveSite);
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[aPlatformPtr->GetIndex()];
      disPlatformPtr->SetShadowPlatform(shadowPtr);
   }
   else
   {
      auto out = ut::log::warning() << "Unable to add shadow platform to simulation.";
      out.AddNote() << "Platform type " << aPlatformPtr->GetType() << " is not defined for this simulation.";
   }
}
// ============================================================================
//! Remove a site/application from the ignored site/application list.
bool WsfDisInterface::ClearIgnoredSiteAndApp(unsigned short aSite, unsigned short aApp)
{
   bool ok = true;
   if (!mIgnoredSiteAndApps.empty())
   {
      if (mIgnoredSiteAndApps.find(std::pair<unsigned short, unsigned short>(aSite, aApp)) != mIgnoredSiteAndApps.end())
      {
         mIgnoredSiteAndApps.erase(mIgnoredSiteAndApps.find(std::pair<unsigned short, unsigned short>(aSite, aApp)));
      }
      else
      {
         ok = false;
      }
   }
   else
   {
      ok = false;
   }
   return ok;
}

// ============================================================================
// Clear list of all site/applications that were being filtered out.
bool WsfDisInterface::ClearIgnoredSitesAndApps()
{
   mIgnoredSiteAndApps.clear();
   return true;
}

// ============================================================================
// private
void WsfDisInterface::DeletePlatform(double aSimTime, size_t aPlatformIndex)
{
   if (HasDevice())
   {
      // NOTE: This can be called indirectly after simulation completed when the simulation deletes
      //       any remaining platforms. Our SimulationComplete method is called prior to the final
      //       platform cleanup and has deleted all of the WsfDisPlatform objects. Thus, the following
      //       will do nothing.

      WsfDisPlatform* disPlatformPtr = mDisPlatforms[aPlatformIndex];
      if (disPlatformPtr != nullptr)
      {
         // When participating in a real-time exercise many like to see immediate feedback if a local platform is broken.
         WsfPlatform* platformPtr = disPlatformPtr->GetPlatform();
         if (HasExternalDevice() && GetSimulation().IsActive() && (!platformPtr->IsExternallyControlled()) &&
             (!platformPtr->IsExternalShadow()) && (platformPtr->GetDamageFactor() >= 1.0))
         {
            auto out = ut::log::info() << "Platform broken.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << platformPtr->GetName();
         }

         DisEntityId entityId(disPlatformPtr->GetEntityId());            // Save data that will be deleted...
         bool        hasBeenReplaced(disPlatformPtr->HasBeenReplaced()); // ...

         disPlatformPtr->PlatformDeleted(aSimTime);
         delete mDisPlatforms[aPlatformIndex];
         mDisPlatforms[aPlatformIndex] = nullptr;

         // Inform the playback devices that a platform has been deleted as they need to know this.
         WsfDisPlaybackDevice::EntityDeleted(entityId);

         // If this has been replaced by another as the result of an incoming transfer of ownership, then data
         // tables associated with the entity ID should not be updated because some other platform has assumed
         // the entity ID.

         if (!hasBeenReplaced)
         {
            // Save the platform index to entity id relationship for future reference
            mDelPlatformIndexToEntityId[aPlatformIndex] = entityId;
            auto iter                                   = mEntityIdToDisPlatform.find(entityId);
            if (iter != mEntityIdToDisPlatform.end())
            {
               mEntityIdToDisPlatform.erase(iter);
            }
            mExtInterfacePtr->FreeEntityId(entityId.GetEntity());
         }
      }
   }
}

// ============================================================================
//! Find the DIS platform with the specified entity ID.
//!
//! @return Pointer to the DIS platform, or zero if th platform does not exist.
WsfDisPlatform* WsfDisInterface::FindDisPlatform(const DisEntityId& aEntityId)
{
   WsfDisPlatform* disPlatformPtr = nullptr;
   auto            iter           = mEntityIdToDisPlatform.find(aEntityId);
   if (iter != mEntityIdToDisPlatform.end())
   {
      disPlatformPtr = iter->second;
   }
   return disPlatformPtr;
}

// ============================================================================
//! Get the DIS platform extension given a platform index.
//!
//! @return Pointer to the DIS platform extension.
//! @return 0 if the input platform index is 0 or if the platform no longer exists.
WsfDisPlatform* WsfDisInterface::GetDisPlatform(size_t aPlatformIndex)
{
   WsfDisPlatform* disPlatformPtr = nullptr;
   if ((aPlatformIndex != 0) && (aPlatformIndex < mDisPlatforms.size()))
   {
      disPlatformPtr = mDisPlatforms[aPlatformIndex];
   }
   return disPlatformPtr;
}

// ============================================================================
//! Return the DIS Entity ID of the given platform.
void WsfDisInterface::GetEntityId(WsfPlatform* aPlatformPtr, DisEntityId& aEntityId)
{
   bool available = false;
   if (aPlatformPtr != nullptr)
   {
      size_t platformIndex = aPlatformPtr->GetIndex();
      if (mDisPlatforms.size() > platformIndex)
      {
         WsfDisPlatform* disPlatformPtr = mDisPlatforms[aPlatformPtr->GetIndex()];
         if (disPlatformPtr)
         {
            available = true;
            aEntityId = disPlatformPtr->GetEntityId();
         }
      }
   }
   if (!available)
   {
      aEntityId.Set(0, 0, 0);
   }
}

// ============================================================================
//! Return the DIS Entity ID of the given platform index.
//! @note This will return an id for either "live" and "dead" entities
void WsfDisInterface::GetEntityIdFromIndex(size_t aPlatformIndex, DisEntityId& aEntityId)
{
   // Set default values
   aEntityId.Set(0, 0, 0);

   // See if there is a better answer
   if (aPlatformIndex != 0)
   {
      WsfDisPlatform* disPlatformPtr = nullptr;
      if (mDisPlatforms.size() > aPlatformIndex)
      {
         disPlatformPtr = mDisPlatforms[aPlatformIndex];
      }
      if (disPlatformPtr != nullptr)
      {
         aEntityId = disPlatformPtr->GetEntityId();
      }
      else
      {
         // look in the "dead" list
         auto iter = mDelPlatformIndexToEntityId.find(aPlatformIndex);
         if (iter != mDelPlatformIndexToEntityId.end())
         {
            aEntityId = iter->second;
         }
      }
   }
}

// ============================================================================
//! Given an object, select a DIS entity type.
// private
bool WsfDisInterface::GetEntityType(const WsfObject* aObjectPtr, DisEntityType& aEntityType) const
{
   WsfExtEntityType entityType;
   bool             found = mExtInterfacePtr->GetEntityType(aObjectPtr, entityType);
   aEntityType            = WsfDisExt::ToDis(entityType);
   return found;
}

// ============================================================================
//! Given a type name, select a DIS entity type.
bool WsfDisInterface::GetEntityType(WsfStringId aTypeName, DisEntityType& aEntityType) const
{
   WsfExtEntityType extEntityType;
   if (mExtInterfacePtr->GetEntityType(aTypeName, extEntityType))
   {
      aEntityType = WsfDisExt::ToDis(extEntityType);
      return true;
   }
   return false;
}

// ====================================================================================
void WsfDisInterface::CleanupDevices()
{
   for (auto& device : mDevices)
   {
      delete device;
   }
   mDevices.clear();
   mHasInputDevice    = false;
   mHasOutputDevice   = false;
   mHasExternalDevice = false;
}

// ============================================================================
//! Return the receive port for DIS inputs
int WsfDisInterface::GetRcvPort() const
{
   return -1;
}

// ============================================================================
//! Return the send port for DIS inputs
int WsfDisInterface::GetSendPort() const
{
   return -1;
}

// ============================================================================
//! Return the simulation time corresponding to a timestamp in a PDU
//! @param aPdu The PDU from which the time is be extracted.
//! @return The equivalent simulation time.
double WsfDisInterface::GetTimeFromPdu(DisPdu& aPdu) const
{
   // Unless the 'use_pdu_time' command is specified, the time associated with an
   // inbound PDU is the current simulation time.

   double simTime = mSimTime;
   if (mUsePduTime)
   {
      // Bias the inbound time by the deferred connection time.
      double deferredConnectionTime = mExtInterfacePtr->GetDeferredConnectionTime();
      simTime = aPdu.GetBestAvailableTime(mSimTime - deferredConnectionTime, 1) + deferredConnectionTime;
   }
   return simTime;
}

// ============================================================================
//! Attempt to receive a PDU from a connection.
//! @return The PDU to received.  'null' if no PDU can be received by a connection.
DisPdu* WsfDisInterface::GetPdu()
{
   unsigned int deviceCount = static_cast<unsigned int>(mDevices.size());
   if (deviceCount == 0)
   {
      return nullptr;
   }
   DisPdu*      pduPtr      = nullptr;
   unsigned int firstDevice = mCurrentGetDevice = mCurrentGetDevice % deviceCount;
   do
   {
      WsfDisDevice* devicePtr = mDevices[mCurrentGetDevice];
      pduPtr                  = devicePtr->GetPdu(GetPduFactory());
      if (pduPtr != nullptr)
      {
         break;
      }
      mCurrentGetDevice = (mCurrentGetDevice + 1) % deviceCount;
   } while (mCurrentGetDevice != firstDevice);
   return pduPtr;
}

// ============================================================================
//! Send a PDU out over the current connection.
//! @param aSimTime The current simulation time.
//! @param aPduPtr The PDU to be sent.
//! @note aPduPtr is deleted after sending.
void WsfDisInterface::PutPdu(double aSimTime, std::unique_ptr<DisPdu> aPduPtr)
{
   if (HasOutputDevice())
   {
      if (mPutCounter == UINT_MAX)
      {
         mPutCounter = 0;
      }
      ++mPutCounter;
      double deferredConnectionTime = mExtInterfacePtr->GetDeferredConnectionTime();
      double deferredSendPDUTime    = mExtInterfacePtr->GetDeferredPDUSendTime();
      // PDU's aren't sent until we've hit the deferred connection time and when we do,
      // we bias the time so the first PDU shows up at time 0 to external participants.
      // PDU's aren't sent until we've hit the deferred simTime
      if ((aSimTime >= deferredConnectionTime) && (aSimTime >= deferredSendPDUTime))
      {
         if (mMultiThreaded)
         {
            mIO_ThreadPtr->PutPdu(aSimTime - deferredConnectionTime, std::move(aPduPtr));
         }
         else
         {
            for (auto& device : mDevices)
            {
               device->PutPdu(aSimTime - deferredConnectionTime, *aPduPtr);
            }
         }
      }
   }
}

// ============================================================================
//! Immediately send a PDU out over the current connections.
//! @param aSimTime The current simulation time.
//! @param aPdu The PDU to be sent.
//! @note If multi-threaded the PDU is queued by the thread for sending.
void WsfDisInterface::ImmediatePutPdu(double aSimTime, DisPdu& aPdu)
{
   double deferredConnectionTime = mExtInterfacePtr->GetDeferredConnectionTime();
   if (aSimTime >= mExtInterfacePtr->GetDeferredPDUSendTime())
   {
      if (mMultiThreaded)
      {
         mIO_ThreadPtr->ImmediatePutPdu(aSimTime - deferredConnectionTime, &aPdu);
      }
      else
      {
         for (auto& device : mDevices)
         {
            device->PutPdu(aSimTime - deferredConnectionTime, aPdu);
         }
      }
   }
}

// ============================================================================
//! Read a DIS Entity Id from an input stream.
//! @param aInput      The input stream
//! @param aEntityId   The value read from the stream.
// static
void WsfDisInterface::ReadEntityId(UtInput& aInput, DisEntityId& aEntityId)
{
   std::string entityIdString;
   aInput.ReadValue(entityIdString);
   WsfExtEntityId extId;
   WsfExtInterface::ConvertInputToEntityId(entityIdString, extId);
   aEntityId = WsfDisExt::ToDis(extId);
}

// ============================================================================
//! Read a DIS Entity Type from an input stream.
//! @param aInput      The input stream
//! @param aEntityType The value read from the stream.
// static
void WsfDisInterface::ReadEntityType(UtInput& aInput, DisEntityType& aEntityType)
{
   std::string entityTypeString;
   aInput.ReadValue(entityTypeString);
   WsfExtEntityType extType;
   WsfExtInterface::ConvertInputToEntityType(entityTypeString, extType);
   aEntityType = WsfDisExt::ToDis(extType);
}

// ============================================================================
//! Change the PDU factory used to create incoming PDUs.
//!
//! WsfDisInterface will delete the factory when it is no longer needed.
void WsfDisInterface::SetPduFactory(DisPduFactory* aFactory)
{
   delete mPduFactoryPtr;
   mPduFactoryPtr = aFactory;
}

// ============================================================================
// ============================================================================
// Start of simulation observer callbacks.

void WsfDisInterface::AdvanceTime(double aSimTime)
{
   // The derived Process() methods invoke WsfDisInterface::GetTimeFromPdu() to get
   // the time associated with the PDU.  GetTimeFromPdu() requires knowledge of the
   // current simulation time but there is no way to pass this through Process().
   // Therefore we save the current simulation time as a member variable which can

   mSimTime = aSimTime;

   // Get the next PDU from the device and process it.
   if (HasInputDevice())
   {
      DisPdu* pduPtr = nullptr;
      if (mMultiThreaded)
      {
         mIO_ThreadPtr->AdvanceTime(aSimTime);
         pduPtr = mIO_ThreadPtr->GetPdu();
      }
      else
      {
         pduPtr = GetPdu();
      }

      // Loop through all pending PDUs
      while (pduPtr != nullptr)
      {
         // Run the various PDU filters to see whether the pdu should be processed or rejected.
         bool passedFilterCheck = RunFilterChecks(pduPtr);

         // The pdu should be from the current exercise
         // We can cache this number, as it will not change as the simulation is running. LBM: WHY BOTHER?
         if (pduPtr->GetExerciseIdentifier() == Dis::GetExercise() && passedFilterCheck)
         {
            // If the PDU to process is of type Stop/Freeze then reset the
            // mWaitForStartPDU flag; the worker thread continue to process
            // incoming PDUs until a Start PDU is received
            if (mMultiThreaded && (pduPtr->GetPduType() == DisEnum::Pdu::Type::StopFreeze))
            {
               mIO_ThreadPtr->WaitForStartPDU(true);
            }

            // A non-zero return code from Process() indicates that the PDU has been processed
            // and is to be deleted.  A zero return code indicates that the PDU should be retained.
            if (pduPtr->Process() != 0)
            {
               delete pduPtr;
            }
         }
         else
         {
            delete pduPtr;
         }

         // Get the next PDU to process
         if (mMultiThreaded)
         {
            pduPtr = mIO_ThreadPtr->GetPdu();
         }
         else
         {
            pduPtr = GetPdu();
         }
      }
   }
}

//! Implement PDU rejection filtering by one of the following methods:
//! <ul>
//! <li> Sending DIS Site and Application
//! <li> Entity Kind and Domain, of the DIS Entity Type record of the DIS Entity State PDU.
//! <li> Full DIS Entity Type of the DIS Entity State PDU.
//! </ul>
bool WsfDisInterface::RunFilterChecks(const DisPdu* aPduPtr)
{
   // Filter out by site AND application based on user request.
   bool passedFilterCheck = true;

   if (mFilteringEnabled) // Don't proceed unless some form if filtering is enabled
   {
      if (!mIgnoredSiteAndApps.empty() &&
          mIgnoredSiteAndApps.find(std::pair<unsigned short, unsigned short>(
             static_cast<unsigned short>(aPduPtr->GetOriginatingEntity().GetSite()),
             static_cast<unsigned short>(aPduPtr->GetOriginatingEntity().GetApplication()))) != mIgnoredSiteAndApps.end())
      {
         passedFilterCheck = false;
         mPduFactoryPtr->IncrementFilteredPduCount();
      }
      else if (!mIgnoredPduTypes.empty() && mIgnoredPduTypes.find(aPduPtr->GetPduType()) != mIgnoredPduTypes.end())
      {
         passedFilterCheck = false;
         mPduFactoryPtr->IncrementFilteredPduCount();
      }

      if (((!mIgnoredKindAndDomain.empty()) || (!mIgnoredTypes.empty()) || (!mRangeFilteredPlatforms.empty())) &&
          passedFilterCheck)
      {
         // If this is an entity state and we have specified in user input that we
         // wish to ignore given (kind, domain) pairs, or specific entity types,
         // check to see if we should filter out the entity state.
         // We check both these filters here so we don't have to cast the PDU twice.
         // Note that this filter only relates to entity state pdus,
         // but filtering these will prevent creation of the dis platforms,
         // so that other pdus from these entities will largely be ignored.
         if (aPduPtr->GetClass() == DisEnum::Pdu::Type::EntityState)
         {
            const DisEntityState* esPtr = static_cast<const DisEntityState*>(aPduPtr);

            if (!mIgnoredTypes.empty())
            {
               if (mIgnoredTypes.find(WsfDisExt::ToExt(esPtr->GetEntityType())) != mIgnoredTypes.end())
               {
                  passedFilterCheck = false;
                  mPduFactoryPtr->IncrementFilteredPduCount();
               }
            }
            if (passedFilterCheck && (!mIgnoredKindAndDomain.empty()))
            {
               DisUint8 kind   = esPtr->GetEntityType().GetEntityKind();
               DisUint8 domain = esPtr->GetEntityType().GetDomain();
               if (mIgnoredKindAndDomain.find(std::make_pair(kind, domain)) != mIgnoredKindAndDomain.end())
               {
                  passedFilterCheck = false;
                  mPduFactoryPtr->IncrementFilteredPduCount();
               }
            }
            if (passedFilterCheck && (!mRangeFilteredPlatforms.empty()))
            {
               double targetLocWCS[3];
               esPtr->GetLocation(targetLocWCS[0], targetLocWCS[1], targetLocWCS[2]);
               unsigned platformNum = 0;
               while (platformNum < mRangeFilteredPlatforms.size())
               {
                  WsfPlatform* platformPtr = nullptr;
                  if (std::get<1>(mRangeFilteredPlatforms[platformNum]) == 0)
                  {
                     platformPtr = GetSimulation().GetPlatformByName(std::get<0>(mRangeFilteredPlatforms[platformNum]));
                     if (platformPtr != nullptr)
                     {
                        std::get<1>(mRangeFilteredPlatforms[platformNum]) = platformPtr->GetIndex();
                     }
                  }
                  else
                  {
                     platformPtr = GetSimulation().GetPlatformByIndex(std::get<1>(mRangeFilteredPlatforms[platformNum]));
                  }
                  if (platformPtr != nullptr)
                  {
                     // Get range squared from source to target.
                     double locWCS[3];
                     double targetVec[3];
                     platformPtr->GetLocationWCS(locWCS);
                     UtVec3d::Subtract(targetVec, targetLocWCS, locWCS);
                     double rangeSquared = UtVec3d::MagnitudeSquared(targetVec);
                     passedFilterCheck   = (rangeSquared < std::get<2>(mRangeFilteredPlatforms[platformNum]));

                     // only need to pass one check, not all.
                     if (passedFilterCheck)
                     {
                        break;
                     }
                     ++platformNum;
                  }
                  else
                  {
                     auto out = ut::log::warning()
                                << "Platform specified for range filtering, does not (or, does no longer) exist.";
                     out.AddNote() << "Platform: " << std::get<0>(mRangeFilteredPlatforms[platformNum]);

                     mRangeFilteredPlatforms.erase(mRangeFilteredPlatforms.begin() + platformNum);
                  }
               }
            }
         }
      }
   }
   return passedFilterCheck;
}

// ============================================================================
void WsfDisInterface::Comment(double aSimTime, WsfPlatform* aPlatformPtr, const std::string& aComment)
{
   mLastSimTime = aSimTime;
   if (HasOutputDevice() && (!aPlatformPtr->IsExternallyControlled()))
   {
      size_t          platformIndex  = aPlatformPtr->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->Comment(aSimTime, aComment);
      }
   }
}

// ============================================================================
void WsfDisInterface::CommFrequencyChanged(double aSimTime, wsf::comm::Comm* aCommPtr)
{
   mLastSimTime = aSimTime;
   if ((HasDevice() || mIsDeferredConnection) && (!aCommPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aCommPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->CommFrequencyChanged(aSimTime, aCommPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::CommTurnedOff(double aSimTime, wsf::comm::Comm* aCommPtr)
{
   mLastSimTime = aSimTime;
   if ((HasDevice() || mIsDeferredConnection) && (!aCommPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aCommPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->CommTurnedOff(aSimTime, aCommPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::CommTurnedOn(double aSimTime, wsf::comm::Comm* aCommPtr)
{
   mLastSimTime = aSimTime;
   if ((HasDevice() || mIsDeferredConnection) && (!aCommPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aCommPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->CommTurnedOn(aSimTime, aCommPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::FrameComplete(double aSimTime)
{
   mLastSimTime = aSimTime;
}

// ============================================================================
void WsfDisInterface::FramePlatformsUpdated(double aSimTime)
{
   mLastSimTime = aSimTime;

   // Delete platforms
   size_t delSize(mDeletedPlatforms.size());
   for (size_t delIndex = 0; delIndex < delSize; ++delIndex)
   {
      DeletePlatform(aSimTime, mDeletedPlatforms[delIndex]);
   }
   mDeletedPlatforms.clear();

   // Add platforms
   size_t addSize(mAddedPlatforms.size());
   for (size_t addIndex = 0; addIndex < addSize; ++addIndex)
   {
      AddPlatform(aSimTime, mAddedPlatforms[addIndex]);
   }
   mAddedPlatforms.clear();
}

// ============================================================================
void WsfDisInterface::FrameStarting(double aSimTime)
{
   mLastSimTime = aSimTime;
}

// ============================================================================
bool WsfDisInterface::PrepareExtension()
{
   bool ok = false;
   // Ensure Ext interface is initialized
   mExtInterfacePtr->Initialize();
   Dis::SetApplication(mExtInterfacePtr->GetApplicationId());
   Dis::SetSite(mExtInterfacePtr->GetSiteId());

   mHasInputDevice    = false;
   mHasOutputDevice   = false;
   mHasExternalDevice = false;
   for (auto& deviceConfiguration : mDeviceConfigurations)
   {
      mHasInputDevice    = mHasInputDevice || deviceConfiguration.IsInputDevice();
      mHasOutputDevice   = mHasOutputDevice || deviceConfiguration.IsOutputDevice();
      mHasExternalDevice = mHasExternalDevice || deviceConfiguration.IsExternalDevice();
   }

   // Reset various DIS parameters.

   DisEventId::SetNextNumber(0);

   if (!HasInputDevice())
   {
      mAutostart = true;
   }
   if (!HasDevice())
   {
      mCallbacks.Clear();
      mConditionalCallbacks.Clear();
   }

   try
   {
      // Add callbacks not already added in the constructor
      if (HasDevice())
      {
         AddCallbacks();
      }

      if (mExtInterfacePtr->GetDeferredConnectionTime() == 0.0)
      {
         if (!mAutostart)
         {
            GetSimulation().SetIsExternallyStarted(true);
         }
         ok = ActivateConnection(0.0);
      }
      else
      {
         ok = true;
      }

      // Start the worker thread if multi-threaded
      if (mMultiThreaded)
      {
         mIO_ThreadPtr->Initialize(mAutostart);
      }
   }
   catch (std::exception&)
   {
      ut::log::error() << "Unable to open dis_interface device.";
   }

   // Assign values for various DIS parameters if not defined by the user.

   if (ok)
   {
      mHeartbeatMultiplier = mInputHeartbeatMultiplier;
      if (mHeartbeatMultiplier < 0.0)
      {
         mHeartbeatMultiplier = 2.4; // default heartbeat multiplier
      }

      // If connecting at a later time, then create the event to establish the connection.
      double deferredConnectionTime = mExtInterfacePtr->GetDeferredConnectionTime();
      if (deferredConnectionTime > 0.0)
      {
         // Connecting at a later time.
         if (HasDevice())
         {
            mIsDeferredConnection = true;

            // Queue up the event to reestablish the network connection.
            // Other events are scheduled at the deferred connection time which depend on the connection, so we subtract
            // a small amount
            GetSimulation().AddEvent(
               ut::make_unique<ActivateDeferredConnectionEvent>(std::max(0.0, deferredConnectionTime - 1.0E-6), this));
         }
      }

      mFilteringEnabled = !(mIgnoredSiteAndApps.empty() && mIgnoredKindAndDomain.empty() && mIgnoredTypes.empty() &&
                            mRangeFilteredPlatforms.empty() && mIgnoredPduTypes.empty());

      ok = mWsfDisExchange.Initialize(this);
   }

   // If the simulation is already started, prime the dis interface now
   WsfSimulation& sim = GetSimulation();

   double simTime = sim.GetSimTime();
   for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
   {
      PlatformAdded(simTime, sim.GetPlatformEntry(i));
   }
   for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
   {
      PlatformInitialized(simTime, sim.GetPlatformEntry(i));
   }

   for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
   {
      WsfPlatform* platPtr = sim.GetPlatformEntry(i);
      for (WsfComponentList::RoleIterator<WsfSensor> iter(*platPtr); !iter.AtEnd(); ++iter)
      {
         if (iter->IsTurnedOn())
         {
            SensorTurnedOn(simTime, *iter);
         }
      }
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*platPtr); !iter.AtEnd(); ++iter)
      {
         if (iter->IsTurnedOn())
         {
            CommTurnedOn(simTime, *iter);
         }
      }
   }

   for (auto component : mComponents)
   {
      component->PrepareComponent(simTime);
   }

   return ok;
}

// ============================================================================
void WsfDisInterface::MessageReceived(double             aSimTime,
                                      wsf::comm::Comm*   aXmtrPtr,
                                      wsf::comm::Comm*   aRcvrPtr,
                                      const WsfMessage&  aMessage,
                                      wsf::comm::Result& aResult)
{
   mLastSimTime             = aSimTime;
   WsfPlatform* platformPtr = aRcvrPtr->GetPlatform();
   if (HasDevice() && (!mSuppressCommData) && (!platformPtr->IsExternallyControlled()))
   {
      size_t          platformIndex  = platformPtr->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->MessageReceived(aSimTime, aXmtrPtr, aRcvrPtr, aMessage, aResult);
      }
   }
}

// ============================================================================
void WsfDisInterface::MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   mLastSimTime             = aSimTime;
   WsfPlatform* platformPtr = aXmtrPtr->GetPlatform();
   if (HasDevice() && (!mSuppressCommData) && (!platformPtr->IsExternallyControlled()))
   {
      size_t          platformIndex  = platformPtr->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->MessageTransmitted(aSimTime, aXmtrPtr, aMessage);
      }
   }
}

// ============================================================================
void WsfDisInterface::MoverUpdated(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData)
{
   mLastSimTime             = aSimTime;
   WsfPlatform* platformPtr = aDeadReckonData.mPlatformPtr;
   if (!platformPtr->IsExternallyControlled() && HasDevice())
   {
      size_t          platformIndex  = platformPtr->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->MoverUpdated(aSimTime, aDeadReckonData);
      }
   }
}

// ============================================================================
void WsfDisInterface::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   mLastSimTime = aSimTime;
   AddPlatform(aSimTime, aPlatformPtr);
}

// ============================================================================
void WsfDisInterface::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // A platform is being deleted from the simulation.  We must delete our associated
   // DIS platform and remove entries from tables that reference the deleted DIS platform.

   mLastSimTime = aSimTime;
   DeletePlatform(aSimTime, aPlatformPtr->GetIndex());
}

// ============================================================================
void WsfDisInterface::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice())
   {
      WsfDisPlatform* disPlatformPtr = GetDisPlatform(aPlatformPtr->GetIndex());
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->PlatformInitialized(aSimTime);
      }
   }
}

// ============================================================================
void WsfDisInterface::RequestTimeAdvance(double aSimTime, double aMaxRate, double aTimeStep)
{
   // if we are in a scaled-and-stepped NRT simulation, request time advance to
   // the next step
   if (mJoinExercise)
   {
      DisEntityId recvId(0xFFFF, 0xFFFF, 0xFFFF);
      if (GetReliabilityService() == DisEnum::Pdu::Reliability::Acknowledged)
      {
         WsfDisActionRequestR request(this);
         request.MakeTimeAdvanceRequest(recvId, aSimTime, aMaxRate, aTimeStep);
         ImmediatePutPdu(aSimTime, request);
      }
      else
      {
         WsfDisActionRequest request(this);
         request.MakeTimeAdvanceRequest(recvId, aSimTime, aMaxRate, aTimeStep);
         ImmediatePutPdu(aSimTime, request);
      }
   }
}

// ============================================================================
// Filtering may have been enabled after start of run-time.  If ignore site and app list was cleared
// reset the filtering flag based on other filtering criteria.
void WsfDisInterface::ResetFiltering()
{
   mFilteringEnabled = !(mIgnoredSiteAndApps.empty() && mIgnoredKindAndDomain.empty() && mIgnoredTypes.empty() &&
                         mRangeFilteredPlatforms.empty() && mIgnoredPduTypes.empty());
}

// ============================================================================
void WsfDisInterface::SensorDetectionChanged(double           aSimTime,
                                             WsfSensor*       aSensorPtr,
                                             size_t           aTargetPlatformIndex,
                                             WsfSensorResult& aResult)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorDetectionChanged(aSimTime, aSensorPtr, aTargetPlatformIndex, aResult);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorFrequencyChanged(aSimTime, aSensorPtr, aModePtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorModeActivated(aSimTime, aSensorPtr, aModePtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorModeDeactivated(aSimTime, aSensorPtr, aModePtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorRequestCanceled(aSimTime, aSensorPtr, aTrackPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorRequestInitiated(double          aSimTime,
                                             WsfSensor*      aSensorPtr,
                                             WsfSensorMode*  aModePtr,
                                             const WsfTrack* aTrackPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorRequestInitiated(aSimTime, aSensorPtr, aModePtr, aTrackPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorRequestUpdated(double          aSimTime,
                                           WsfSensor*      aSensorPtr,
                                           WsfSensorMode*  aModePtr,
                                           const WsfTrack* aTrackPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorRequestUpdated(aSimTime, aSensorPtr, aModePtr, aTrackPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorScanUpdated(double aSimTime, WsfSensor* aSensorPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorScanUpdated(aSimTime, aSensorPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorTrackDropped(aSimTime, aSensorPtr, aTrackPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mLastSimTime = aSimTime;
   if (HasDevice() && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorTrackInitiated(aSimTime, aSensorPtr, aTrackPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   mLastSimTime = aSimTime;
   if ((HasDevice() || mIsDeferredConnection) && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorTurnedOff(aSimTime, aSensorPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr)
{
   mLastSimTime = aSimTime;
   if ((HasDevice() || mIsDeferredConnection) && (!aSensorPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aSensorPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisPlatforms[platformIndex];
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->SensorTurnedOn(aSimTime, aSensorPtr);
      }
   }
}

// ============================================================================
void WsfDisInterface::SimulationDestroyed(WsfSimulation* aSimulationPtr)
{
   // If the simulation is destroyed without completing, force SimulationComplete execution
   SimulationComplete(mLastSimTime + 0.1);
}

// ============================================================================
void WsfDisInterface::SimulationComplete(double aSimTime)
{
   if (mMultiThreaded)
   {
      mIO_ThreadPtr->SetReceivePDUs(false);
      mIO_ThreadPtr->WaitUntilWorkDone();
   }

   mLastSimTime = std::max(mLastSimTime, aSimTime);
   for (WsfDisPlatform* disPlatformPtr : mDisPlatforms)
   {
      if (disPlatformPtr != nullptr)
      {
         disPlatformPtr->PlatformDeleted(aSimTime);
         delete disPlatformPtr;
      }
   }

   // Stop the thread since the device pointer is being freed
   // But wait until it has completed it's work
   if (mMultiThreaded)
   {
      mIO_ThreadPtr->SimulationComplete();
   }

   mDisPlatforms.clear();
   mEntityIdToDisPlatform.clear();
   WsfDisPlaybackDevice::Reset();
   CleanupDevices();
}

// ============================================================================
void WsfDisInterface::SimulationStarting()
{
   if (mMultiThreaded)
   {
      mIO_ThreadPtr->SimulationStarting();
   }
}

// ===========================================================================
void WsfDisInterface::SimTimeToDisClockTime(double aSimTime, DisClockTime& aClockTime)
{
   int    hours   = static_cast<int>(aSimTime) / 3600;
   double seconds = aSimTime - (3600 * hours);
   aClockTime.SetHour(hours);
   aClockTime.SetTimePastHour(DisTime::GetTimeStamp(seconds));
}


// ============================================================================
// virtual
WsfEvent::EventDisposition WsfDisInterface::ActivateDeferredConnectionEvent::Execute()
{
   GetSimulation()->SetIsExternallyStarted(!mInterfacePtr->GetAutostart());
   // Assume we want realtime if input device is configured
   if (mInterfacePtr->HasInputDevice())
   {
      GetSimulation()->SetRealtime(GetTime(), true);
   }
   mInterfacePtr->ActivateConnection(GetTime());
   return cDELETE;
}

// ============================================================================
bool WsfDisInterface::DeviceConfiguration::IsComplete()
{
   bool isComplete = false;
   // All devices must have an 'address'
   if (!mAddress.empty())
   {
      if ((mDeviceType == WsfDisDevice::cDEVICE_PLAYBACK) || (mDeviceType == WsfDisDevice::cDEVICE_RECORD))
      {
         isComplete = true;
      }
      else if ((mDeviceType == WsfDisDevice::cDEVICE_MULTICAST) || (mDeviceType == WsfDisDevice::cDEVICE_BROADCAST) ||
               (mDeviceType == WsfDisDevice::cDEVICE_UNICAST))
      {
         // network devices must have send/receive ports
         isComplete = ((mRecvPort != 0) && (mSendPort != 0));
      }
   }
   return isComplete;
}

// ==============================================================================
// ================== BEGIN Simulation Pause Activity ===========================
// ==============================================================================

void WsfDisInterface::SimulationPausing()
{
   const unsigned int cMAX_PDUS_PER_PULSE             = 5;
   const double       cTIME_TO_SEND_ALL_ENTITY_STATES = 5.0; // seconds
   // (equates to DIS standard heartbeat interval)

   mPauseIsInProgress = true;

   // The default behavior is not to send out entity states while paused.
   // If the flag is not set, then skip this processing.
   if (!mSendPeriodicsWhilePaused)
   {
      return;
   }

   double wallTime = GetSimulation().GetWallTime();
   double simTime  = GetSimulation().GetSimTime();

   std::vector<unsigned int> localActiveIndices;
   for (unsigned int iPlatIndex = 0; iPlatIndex != mDisPlatforms.size(); ++iPlatIndex)
   {
      if (mDisPlatforms[iPlatIndex] != nullptr)
      {
         localActiveIndices.push_back(iPlatIndex);
      }
   }

   // If no platforms to send, then skip queuing the event.
   if (localActiveIndices.empty())
   {
      return;
   }

   double totalNumberOfPulses = (double)localActiveIndices.size() / (double)cMAX_PDUS_PER_PULSE;
   int    integerNumberOfPulses;
   if (totalNumberOfPulses <= 1)
   {
      integerNumberOfPulses = 1;
   }
   else
   {
      integerNumberOfPulses = static_cast<int>(totalNumberOfPulses + 0.5);
   }

   double pulseInterval = cTIME_TO_SEND_ALL_ENTITY_STATES / integerNumberOfPulses;
   assert(pulseInterval > 0.0);
   assert(pulseInterval <= cTIME_TO_SEND_ALL_ENTITY_STATES);
   unsigned int numPlatformsSentPerPulse =
      static_cast<unsigned int>(double(localActiveIndices.size()) / integerNumberOfPulses);

   GetSimulation().AddWallEvent(
      ut::make_unique<PauseUpdateEvent>(wallTime, simTime, pulseInterval, numPlatformsSentPerPulse, localActiveIndices, this));
}

// ===================================================
void WsfDisInterface::SimulationResuming()
{
   mPauseIsInProgress = false;
}

// ===================================================
void WsfDisInterface::SendPeriodicPdusWhilePaused(std::vector<unsigned int>& aEntityIndices)
{
   if (mPauseIsInProgress)
   {
      for (unsigned int i = 0; i != aEntityIndices.size(); ++i)
      {
         WsfDisPlatform* platPtr = GetDisPlatform(aEntityIndices[i]);
         if (platPtr != nullptr)
         {
            // Copy the entity state, set the paused bit, and send it out:
            DisEntityState esPdu(*platPtr->GetEntityState());
            esPdu.GetAppearanceRecord().SetFrozen(DisEnum::Appearance::FrozenType::Frozen);
            ImmediatePutPdu(mSimTime, esPdu);

            // If this platform has an emission associated with it, send it also:
            if (platPtr->GetEmissionPDU())
            {
               platPtr->SendEmissionsPDU(mSimTime);
            }
         }
      }
   }
}

// ===================================================
//! Send the start date and time in a SetData PDU.
void WsfDisInterface::SendDateAndTime(WsfDisDevice& aDevice, const DisEntityId& aReceivingId, const UtCalendar& aDateTime)
{
   WsfDisSetData data(this);
   data.SetOriginatingEntity(DisEntityId(mExtInterfacePtr->GetSiteId(), mExtInterfacePtr->GetApplicationId(), 0));
   data.SetReceivingEntity(aReceivingId);
   std::ostringstream oss;
   oss << std::setw(2) << std::setfill('0') << aDateTime.GetMonth();
   oss << std::setw(2) << std::setfill('0') << aDateTime.GetDay();
   oss << std::setw(4) << aDateTime.GetYear();

   unsigned char* dateField = new unsigned char[9];
   memcpy(dateField, oss.str().c_str(), 9);
   DisVariableDatum date;
   date.SetId(DisEnum::DatumTag::DATE_MMDDYYYY);
   date.SetDataPtr(dateField, 72);
   data.GetData().AddVariableDatum(date);
   oss.str(""); // Now ready to re-use oss.
   oss << std::setw(2) << std::setfill('0') << aDateTime.GetHour();
   oss << std::setw(2) << std::setfill('0') << aDateTime.GetMinute();
   oss << std::setw(2) << std::setfill('0') << static_cast<int>(aDateTime.GetSecond());

   unsigned char* timeField = new unsigned char[7];
   memcpy(timeField, oss.str().c_str(), 7);
   DisVariableDatum time;
   time.SetId((DisEnum::DatumTag::TIME_HHMMSS));
   time.SetDataPtr(timeField, 56);
   data.GetData().AddVariableDatum(time);
   aDevice.PutPdu(mSimTime, data);
}

//////////////////////////////////////////////////////////////////////////
// Entity State Methods
//////////////////////////////////////////////////////////////////////////

// ============================================================================
//! Called from WsfDisMover to update the platform state by extrapolating from the last received entity state.
void WsfDisInterface::ExtrapolatePlatformState(double aSimTime, WsfDisPlatform* aDisPlatformPtr)
{
   double          entityStateUpdateTime;
   DisEntityState* entityStatePtr;

   aDisPlatformPtr->GetEntityState(entityStateUpdateTime, entityStatePtr);
   WsfPlatform* platformPtr = aDisPlatformPtr->GetPlatform();
   if ((entityStatePtr == nullptr) || (platformPtr == nullptr))
   {
      return;
   }

   // Assume the entity is not moving (static, frozen or inactive).
   // Get just the position and orientation. Assume the velocity and acceleration are zero.

   double locationWCS[3];
   double velocityWCS[3]     = {0.0, 0.0, 0.0};
   double accelerationWCS[3] = {0.0, 0.0, 0.0};
   float  psi, theta, phi;
   entityStatePtr->GetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);
   entityStatePtr->GetOrientation(psi, theta, phi);

   DisUint32 frozenOrInactive = (1 << 21) | (1 << 23);
   DisEnum8  algorithm        = entityStatePtr->GetDeadreckoningAlgorithm();
   if ((algorithm != 1) &&                                          // Not static ...
       ((entityStatePtr->GetAppearance() & frozenOrInactive) == 0)) // ... and not (frozen or inactive)
   {
      float vx, vy, vz;
      float ax = 0.0F, ay = 0.0F, az = 0.0F;
      entityStatePtr->GetVelocity(vx, vy, vz);
      if ((algorithm == 4) || // DRM(RVW)
          (algorithm == 5))   // DRM(FVW)
      {
         entityStatePtr->GetAcceleration(ax, ay, az);
      }
      double dt      = aSimTime - entityStateUpdateTime;
      double halfDt2 = 0.5 * dt * dt;
      locationWCS[0] += (vx * dt) + (ax * halfDt2);
      locationWCS[1] += (vy * dt) + (ay * halfDt2);
      locationWCS[2] += (vz * dt) + (az * halfDt2);
      velocityWCS[0]     = vx + (ax * dt);
      velocityWCS[1]     = vy + (ay * dt);
      velocityWCS[2]     = vz + (az * dt);
      accelerationWCS[0] = ax;
      accelerationWCS[1] = ay;
      accelerationWCS[2] = az;
   }
   platformPtr->SetLocationWCS(locationWCS);


   // The alt here is assumed referenced from the ellipse, since alt was supplied via DIS.
   // WSF is incorrect in assuming the MSL is referenced to the ellipse, but for now accepting that error factor.
   // Since the WCS at lat, lon was set using the alt reference to the ellipse from the DIS PDU,
   // subtract the ellipse to geoid correction to get MSL reference to the geoid (WSF will assume it is reference from
   // the ellipse and will ignore those errors for now).
   if (wsf::EarthGravityModelExtension::Get(mExtInterfacePtr->GetScenario()).IsEGM96Compliant())
   {
      // WCS uses an alt reference to the ellipse.
      // need it MSL
      double lat, lon, ellipticalAlt;
      UtEllipsoidalEarth::ConvertECEFToLLA(locationWCS, lat, lon, ellipticalAlt);
      // do I need this????? since I know msl alt.
      float geoidUndulationCorrection =
         wsf::EarthGravityModelExtension::Get(mExtInterfacePtr->GetScenario()).GetGeoidUndulation(lat, lon);
      // subtract off the gravity correction factor when receiving.
      double altMSL = ellipticalAlt - geoidUndulationCorrection;
      // recompute the WCS with alt adjusted back to MSL.
      // WSF uses MSL but thinks it is referenced to ellipse; accepting that error for now.
      // But DIS compliance sends us an altitude referenced to the ellipse, so need to update
      // with computed MSL alt.  Setting the LocationLLA which will in turn set LocationWCS.
      platformPtr->SetLocationLLA(lat, lon, altMSL);
      double lWCS[3];
      UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, altMSL, lWCS);
      platformPtr->SetLocationWCS(lWCS);
   }

   platformPtr->SetVelocityWCS(velocityWCS);
   platformPtr->SetAccelerationWCS(accelerationWCS);
   platformPtr->SetOrientationWCS(psi, theta, phi);
   platformPtr->SetTime(aSimTime);

   // Determine if the object should be clamped to the terrain.
   //
   // For now we only check entities than come in over a 'playback'.
   // In theory we should probably do this for an source...

   bool allowTerrainClamping = (WsfDisPlaybackDevice::IsPlaybackEntity(entityStatePtr->GetEntityId()));
   if (allowTerrainClamping)
   {
      bool clampToTerrain = false;

      const DisEntityType& entityType = entityStatePtr->GetEntityType();
      switch (entityType.GetEntityKind())
      {
      case 1:                                 // Platform
      case 3:                                 // Life form
      case 4:                                 // Environmental
      case 5:                                 // Cultural Feature
      case 8:                                 // Expendable
      case 9:                                 // Sensor/Emitter
         if ((entityType.GetDomain() == 1) || // Land
             (entityType.GetDomain() == 3))   // Surface
         {
            clampToTerrain = true;
         }
         break;
      case 2:                               // Payload
         if (entityType.GetCategory() == 3) // Fixed
         {
            clampToTerrain = true;
         }
         break;
      default:
         break;
      }

      if (clampToTerrain)
      {
         double lat, lon, alt;
         platformPtr->GetLocationLLA(lat, lon, alt);

         if (wsf::TerrainInterface::Get(GetScenario())->IsEnabled())
         {
            // If terrain is enabled we assume the slopes are approximately the same,
            // so the velocity and acceleration vectors and orientation are good enough.
            platformPtr->SetLocationLLA(lat, lon, platformPtr->GetTerrainHeight() + 0.1);
         }
         else
         {
            // If there is no terrain then the vertical components of velocity and acceleration
            // are set to zero, and pitch and roll are set to zero.
            //
            // Note that this does not maintain the magnitude of the vectors, but that is probably
            // correct because extrapolation should take us to the same lat, lon in either case.

            double velNED[3];
            double aclNED[3];
            double heading, pitch, roll;
            platformPtr->GetVelocityNED(velNED);
            platformPtr->GetAccelerationNED(aclNED);
            platformPtr->GetOrientationNED(heading, pitch, roll);
            velNED[2] = 0.0;
            aclNED[2] = 0.0;
            platformPtr->SetLocationLLA(lat, lon, 0.1);
            platformPtr->SetVelocityNED(velNED);
            platformPtr->GetAccelerationNED(aclNED);
            platformPtr->SetOrientationNED(heading, 0.0, 0.0);
         }
      }
   }
}

// ============================================================================
//! Prepare an entity state for its final transmission.
//! This performs the following operations:
//! -) Captures the final platform position and orientation
//! -) Sets the velocity and acceleration to zero
//! -) Captures the final damage state
//! -) Marks the entity as frozen and inactive.
void WsfDisInterface::PrepareFinalEntityState(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr)
{
   // Capture the final position and orientation of the entity.
   // The velocity and acceleration will be zero'd to prevent extrapolation

   double locWCS[3];
   double psiWCS;
   double thetaWCS;
   double phiWCS;
   aPlatformPtr->GetLocationWCS(locWCS);
   aPlatformPtr->GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
   aEntityStatePtr->SetLocation(locWCS[0], locWCS[1], locWCS[2]);
   aEntityStatePtr->SetOrientation(static_cast<float>(psiWCS), static_cast<float>(thetaWCS), static_cast<float>(phiWCS));
   aEntityStatePtr->SetVelocity(0.0F, 0.0F, 0.0F);
   aEntityStatePtr->SetAcceleration(0.0F, 0.0F, 0.0F);
   aEntityStatePtr->SetAngularVelocity(0.0F, 0.0F, 0.0F);

   // Update the appearance flags with the damage state

   DisEnum8  entityKind = aEntityStatePtr->GetEntityType().GetEntityKind();
   DisUint32 appearance = aEntityStatePtr->GetAppearance();

   // Damage state
   if ((entityKind == static_cast<DisEnum8>(0)) || // Other
       (entityKind == static_cast<DisEnum8>(1)) || // platform
       (entityKind == static_cast<DisEnum8>(2)) || // payload
       (entityKind == static_cast<DisEnum8>(3)) || // life form (health)
       (entityKind == static_cast<DisEnum8>(5)) || // cultural feature
       (entityKind == static_cast<DisEnum8>(9)))   // sensor/emitter
   {
      // Clear
      appearance &= ~(3 << 3);

      // Set
      DisUint32 damageState = ConvertDamageFactorToDamageState(aPlatformPtr->GetDamageFactor());
      appearance |= (damageState << 3);
   }

   // Clear then set frozen and state bit
   appearance &= ~((1 << 21) | // Frozen
                   (1 << 23)); // State (inactive)
   appearance |= ((1 << 21) |  // Frozen
                  (1 << 23));  // Deactivated
   aEntityStatePtr->SetAppearance(appearance);
}


// ============================================================================
//! Update the DIS appearance word from the platform state.
bool WsfDisInterface::UpdateAppearanceFromPlatform(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr)
{
   // Check for a change in life state.

   DisEnum8 entityKind = aEntityStatePtr->GetEntityType().GetEntityKind();
   // DisEnum8  domain     = aEntityStatePtr->GetEntityType().GetDomain();
   DisUint32 appearance = aEntityStatePtr->GetAppearance();

   // Bits 0-2
   SetAppearance(appearance, aPlatformPtr, 0, 3);

   // Bits 3-4
   // Damage state
   if ((entityKind == static_cast<DisEnum8>(0)) || // Other
       (entityKind == static_cast<DisEnum8>(1)) || // platform
       (entityKind == static_cast<DisEnum8>(2)) || // munition
       (entityKind == static_cast<DisEnum8>(3)) || // life form (health)
       (entityKind == static_cast<DisEnum8>(5)) || // cultural feature
       (entityKind == static_cast<DisEnum8>(6)) || // supply
       (entityKind == static_cast<DisEnum8>(8)) || // expendable
       (entityKind == static_cast<DisEnum8>(9)))   // sensor/emitter
   {
      DisUint32 oldDamageState = (appearance >> 3) & 3;
      DisUint32 newDamageState = ConvertDamageFactorToDamageState(aPlatformPtr->GetDamageFactor());
      if (newDamageState != oldDamageState)
      {
         // NOTE: The inactive bit (23) is NOT set here if the platform is broken (It will be set by PlatformDeleted).
         //       This ensures that only one PDU will be sent with the inactive bit set.

         // Clear the damage state and frozen bits
         appearance &= ~((3 << 3) |  // Damage
                         (1 << 21)); // Frozen

         // Set the new damage state
         appearance |= (newDamageState << 3);
      }
   }
   else
   {
      SetAppearance(appearance, aPlatformPtr, 3, 2);
   }

   // Bits 5-20
   SetAppearance(appearance, aPlatformPtr, 5, 16);

   // don't touch bit 21

   // Bit 22
   SetAppearance(appearance, aPlatformPtr, 22, 1);

   // don't touch bit 23

   // Bit 24-29
   SetAppearance(appearance, aPlatformPtr, 24, 6);

   // Concealed stationary - bit 30
   if (entityKind == static_cast<DisEnum8>(3)) // life form
   {
      bool previouslyConcealed = (((appearance >> 30) & 1) == 1);
      bool concealed           = (aPlatformPtr->GetConcealmentFactor() > 0.99f);
      if (concealed != previouslyConcealed)
      {
         if (concealed)
         {
            appearance |= (1 << 30);
         }
         else
         {
            appearance &= ~(1 << 30);
         }
      }
   }
   else
   {
      SetAppearance(appearance, aPlatformPtr, 30, 1);
   }

   // Bit 31
   SetAppearance(appearance, aPlatformPtr, 31, 1);

   // Process any change in appearance
   bool stateChanged = false;
   if (appearance != aEntityStatePtr->GetAppearance())
   {
      aEntityStatePtr->SetAppearance(appearance);
      stateChanged = true;
   }
   return stateChanged;
}

// ============================================================================
//! Update the platform state from the DIS appearance word.
// static
void WsfDisInterface::UpdatePlatformFromAppearance(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr)
{
   // Update the damage factor from the damage state...

   DisEnum8  entityKind = aEntityStatePtr->GetEntityType().GetEntityKind();
   DisEnum8  domain     = aEntityStatePtr->GetEntityType().GetDomain();
   DisUint32 appearance = aEntityStatePtr->GetAppearance();

   // bits 0-2
   aPlatformPtr->SetAppearance(0, 3, (appearance >> 0) & 7);

   // Damage state - bits 3-4
   if ((entityKind == static_cast<DisEnum8>(0)) || // Other
       (entityKind == static_cast<DisEnum8>(1)) || // platform
       (entityKind == static_cast<DisEnum8>(2)) || // munition
       (entityKind == static_cast<DisEnum8>(3)) || // life form (health)
       (entityKind == static_cast<DisEnum8>(5)) || // cultural feature
       (entityKind == static_cast<DisEnum8>(6)) || // supply
       (entityKind == static_cast<DisEnum8>(8)) || // expendable
       (entityKind == static_cast<DisEnum8>(9)))   // sensor/emitter
   {
      DisUint32 damageState  = (appearance >> 3) & 3;
      double    damageFactor = ConvertDamageStateToDamageFactor(damageState);
      if (fabs(aPlatformPtr->GetDamageFactor() - damageFactor) > 1.0E-6)
      {
         aPlatformPtr->SetDamageFactor(damageFactor);
      }
   }

   // bits 5-15
   aPlatformPtr->SetAppearance(5, 11, (appearance >> 5) & 2047); // To mask 11 bits, use (2^11-1) = 2047

   // Update the signature(s) from the afterburner state. bit 16
   if (((entityKind == static_cast<DisEnum8>(1)) && // platform
        (domain == static_cast<DisEnum8>(2))))      // - air
   {
      bool afterburner         = (((appearance >> 16) & 1) == 1);
      bool previousAfterburner = (aPlatformPtr->GetAppearance(16, 1) == 1u);
      if (afterburner != previousAfterburner)
      {
         aPlatformPtr->SetAppearance(16, 1, afterburner); // set the afterburner factor on the platform

         int afterburnerNum = ((appearance >> 16) & 1);

         StringToStateIdVec* sigPairVecPtr = GetAfterburnerSigStateByType(aPlatformPtr->GetTypeId(), afterburnerNum);
         if (sigPairVecPtr == nullptr)
         {
            sigPairVecPtr = GetAfterburnerSigStateByName(aPlatformPtr->GetNameId(), afterburnerNum);
         }

         if (sigPairVecPtr != nullptr)
         {
            auto sigPairIter = sigPairVecPtr->begin();
            for (; sigPairIter < sigPairVecPtr->end(); ++sigPairIter)
            {
               aPlatformPtr->GetSignatureList().SetState(sigPairIter->second, sigPairIter->first);
            }
         }
      }
   }
   else
   {
      aPlatformPtr->SetAppearance(16, 1, (appearance >> 16) & 1);
   }

   // Update the signature(s) from the configuration Id (bits 17-20, unused per DIS SISO-REF-010-2006).
   // Bits 17-20 are used in some kind/domain's. Note the following...
   //
   // When a change in bits 17-20 are detected, we let all entities pass through to GetConfigChange
   // and assume the user has not mapped types for which this feature isn't valid. This probably
   // introduces a little extra overhead, but the list is typically empty or very small.
   int configState         = (appearance >> 17) & 0xF; // bits 17-20
   int previousConfigState = aPlatformPtr->GetAppearance(17, 4);
   if (configState != previousConfigState)
   {
      aPlatformPtr->SetAppearance(17, 4, configState); // set the configuration number on the platform

      StringToStateIdVec* sigPairVecPtr = GetConfigSigStateByType(aPlatformPtr->GetTypeId(), configState);
      if (sigPairVecPtr == nullptr)
      {
         sigPairVecPtr = GetConfigSigStateByName(aPlatformPtr->GetNameId(), configState);
      }

      if (sigPairVecPtr != nullptr)
      {
         auto sigPairIter = sigPairVecPtr->begin();
         for (; sigPairIter < sigPairVecPtr->end(); ++sigPairIter)
         {
            aPlatformPtr->GetSignatureList().SetState(sigPairIter->second, sigPairIter->first);
         }
      }
   }

   // Update the concealment factor from the concealment state.

   if ((entityKind == static_cast<DisEnum8>(0)) ||  // Other
       ((entityKind == static_cast<DisEnum8>(1)) && // platform
        (domain == static_cast<DisEnum8>(1))) ||    // - land
       (entityKind == static_cast<DisEnum8>(9)))    // sensor/emitter
   {
      bool concealed           = (((appearance >> 19) & 1) == 1);
      bool previouslyConcealed = (aPlatformPtr->GetConcealmentFactor() > 0.99f);
      if (concealed != previouslyConcealed)
      {
         if (concealed)
         {
            aPlatformPtr->SetConcealmentFactor(1.0);
         }
         else
         {
            aPlatformPtr->SetConcealmentFactor(0.0);
         }
      }
   }

   // bits 21-31
   aPlatformPtr->SetAppearance(21, 11, (appearance >> 21 & 2047)); // To mask 11 bits, use (2^11-1) = 2047
}

// ============================================================================
void WsfDisInterface::UpdatePlatformFromEntityState(double          aSimTime,
                                                    WsfDisPlatform* aDisPlatformPtr,
                                                    DisEntityState* aEntityStatePtr)
{
   aDisPlatformPtr->GetPlatform()->SetCapabilities(aEntityStatePtr->GetCapabilities());

   UpdatePlatformFromAppearance(aSimTime, aDisPlatformPtr->GetPlatform(), aEntityStatePtr);
   UpdatePlatformFromArticulationRecords(aSimTime,
                                         aDisPlatformPtr->GetPlatform(),
                                         aEntityStatePtr,
                                         aDisPlatformPtr->GetEntityState());
}

// ============================================================================
void WsfDisInterface::UpdatePlatformSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr)
{
   // Get the DIS PDU data
   unsigned int emitterNameDIS     = aSystemPtr->GetName();
   unsigned int emitterIdNumberDIS = aSystemPtr->GetNumber();

   WsfExtEmission* extEmissionPtr = GetExtEmission();
   // Have we defined the dis emitter type as having a corresponding WSF sensor type?
   WsfStringId sensorTypeId = extEmissionPtr->GetPartTypeId(aSystemPtr->GetName());

   bool updatedSystem = false;
   // update an existing system
   WsfSensor* sensorPtr = extEmissionPtr->GetSensor(aPlatformPtr, sensorTypeId, emitterNameDIS, emitterIdNumberDIS);
   if (sensorPtr == nullptr)
   {
      for (auto component : mComponents)
      {
         if (component->UpdateEmissionSystem(aPlatformPtr, aEmissionPtr, aSystemPtr))
         {
            updatedSystem = true;
            break;
         }
      }
   }
   else
   {
      updatedSystem = true;
      aEmissionPtr->UpdateSensor(aSystemPtr, sensorPtr);
      if (extEmissionPtr->GetDebugMask() & (WsfExtEmission::cLEVEL_ONE_DEBUG | WsfExtEmission::cLEVEL_TWO_DEBUG))
      {
         auto out = ut::log::debug() << "Emission PDU: Updating sensor.";
         out.AddNote() << "T = " << GetSimTime();
         out.AddNote() << "Sensor: " << sensorPtr->GetName() << "." << sensorPtr->GetType();
         out.AddNote() << "DIS: " << emitterNameDIS << "." << emitterIdNumberDIS;
      }
   }

   if (!updatedSystem)
   {
      // Can't find one, so add a sensor
      sensorPtr = extEmissionPtr->AddSensor(GetSimTime(), aPlatformPtr, sensorTypeId, emitterNameDIS, emitterIdNumberDIS);
      if (sensorPtr != nullptr)
      {
         updatedSystem = true;
         aEmissionPtr->UpdateSensor(aSystemPtr, sensorPtr);
         if (extEmissionPtr->GetDebugMask() & (WsfExtEmission::cLEVEL_ONE_DEBUG | WsfExtEmission::cLEVEL_TWO_DEBUG))
         {
            auto out = ut::log::debug() << "Emission PDU: Added sensor.";
            out.AddNote() << "T = " << GetSimTime();
            out.AddNote() << "Sensor: " << sensorPtr->GetName() << "." << sensorPtr->GetType();
            out.AddNote() << "DIS: " << emitterNameDIS << "." << emitterIdNumberDIS;
         }
      }
   }

   if (!updatedSystem)
   {
      // Ask components to add the system
      for (auto component : mComponents)
      {
         if (component->AddEmissionSystem(aPlatformPtr, aEmissionPtr, aSystemPtr))
         {
            updatedSystem = true;
            break;
         }
      }
   }
}

// ============================================================================
void WsfDisInterface::SetAppearance(DisUint32& aAppearance, WsfPlatform* aPlatformPtr, unsigned int aLSB, unsigned int aNumBits)
{
   // Get the mask
   unsigned int position    = std::min(std::max(0u, aLSB), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, aNumBits), 32u);

   // Get new and old values
   DisUint32 bits    = ((aAppearance >> position) & WsfUtil::GenerateBitMask(endPosition - position));
   DisUint32 newBits = aPlatformPtr->GetAppearance(position, aNumBits);

   // If appearance has changed
   if (newBits != bits)
   {
      aAppearance &= ~(WsfUtil::GenerateBitMask(endPosition - position) << position); // zero
      aAppearance |= (newBits << position);                                           // or with the desired flags
   }
}

// ============================================================================
//! Convert a WSF damage factor [0.0 - 1.0] to a DIS Damage State [0, 1, 2, 3]
DisUint32 WsfDisInterface::ConvertDamageFactorToDamageState(double aDamageFactor)
{
   DisUint32 damageState = 0;
   if (aDamageFactor != 0.0)
   {
      if (aDamageFactor < 0.5)
      {
         damageState = 1;
      }
      else if (aDamageFactor < 1.0)
      {
         damageState = 2;
      }
      else
      {
         damageState = 3;
      }
   }
   return damageState;
}

// ============================================================================
//! Convert a DIS Damage State [0, 1, 2, 3] to an WSF damage factor [0.0 - 1.0]
// private static
double WsfDisInterface::ConvertDamageStateToDamageFactor(DisUint32 aDamageState)
{
   double damageFactor = 0.0;
   if (aDamageState != 0)
   {
      if (aDamageState <= 3)
      {
         damageFactor = static_cast<double>(aDamageState) / 3.0;
      }
      else
      {
         damageFactor = 1.0;
      }
   }
   return damageFactor;
}

// ============================================================================
//! Get the signature type and state given the platform type and afterburner state.
//! @param aType The platform type Id.
//! @param aStateFlag The afterburner state.
//! @return The pointer to the signature type and state pair.
// private static
WsfDisInterface::StringToStateIdVec* WsfDisInterface::GetAfterburnerSigStateByType(WsfStringId aType, int aStateFlag)
{
   StringToStateIdVec* sigPairVecPtr(nullptr);
   auto                iter = mAfterBurnerByType.find(std::make_pair(aType, aStateFlag));
   if (iter != mAfterBurnerByType.end())
   {
      sigPairVecPtr = &((*iter).second);
   }
   return sigPairVecPtr;
}

// ============================================================================
//! Get the signature type and state given the platform name and afterburner state.
//! @param aType The platform name Id.
//! @param aStateFlag The afterburner state.
//! @return The pointer to the signature type and state pair.
// private static
WsfDisInterface::StringToStateIdVec* WsfDisInterface::GetAfterburnerSigStateByName(WsfStringId aName, int aStateFlag)
{
   StringToStateIdVec* sigPairVecPtr(nullptr);
   auto                iter = mAfterBurnerByName.find(std::make_pair(aName, aStateFlag));
   if (iter != mAfterBurnerByName.end())
   {
      sigPairVecPtr = &((*iter).second);
   }
   return sigPairVecPtr;
}

// ============================================================================
//! Get the signature type and state given the platform type and configuration state Id.
//! @param aType The platform type Id.
//! @param aStateFlag The configuration state Id.
//! @return The pointer to the signature type and state pair.
// private static
WsfDisInterface::StringToStateIdVec* WsfDisInterface::GetConfigSigStateByType(WsfStringId aType, int aStateFlag)
{
   StringToStateIdVec* sigPairVecPtr(nullptr);
   auto                iter = mConfigIdByType.find(std::make_pair(aType, aStateFlag));
   if (iter != mConfigIdByType.end())
   {
      sigPairVecPtr = &((*iter).second);
   }
   return sigPairVecPtr;
}

// ============================================================================
//! Get the signature type and state given the platform name and configuration state Id.
//! @param aType The platform name Id.
//! @param aStateFlag The configuration state Id.
//! @return The pointer to the signature type and state pair.
// private static
WsfDisInterface::StringToStateIdVec* WsfDisInterface::GetConfigSigStateByName(WsfStringId aName, int aStateFlag)
{
   StringToStateIdVec* sigPairVecPtr(nullptr);
   auto                iter = mConfigIdByName.find(std::make_pair(aName, aStateFlag));
   if (iter != mConfigIdByName.end())
   {
      sigPairVecPtr = &((*iter).second);
   }
   return sigPairVecPtr;
}

// ============================================================================
// private static
void WsfDisInterface::UpdatePlatformFromArticulationRecords(double          aSimTime,
                                                            WsfPlatform*    aPlatformPtr,
                                                            DisEntityState* aNewStatePtr,
                                                            DisEntityState* aOldStatePtr)
{
   unsigned int newPartCount = aNewStatePtr->GetEntityPartCount();
   if (newPartCount == 0)
   {
      return;
   }
   if (mDebugAppearanceUpdates)
   {
      auto out = ut::log::debug() << "Platform part count update.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      out.AddNote() << "Part Count: " << newPartCount;
   }
   unsigned int oldPartCount = 0;
   if (aOldStatePtr != nullptr)
   {
      oldPartCount = aOldStatePtr->GetEntityPartCount();
   }

   // Determine if articulation updates need to be tracked for this platform type.
   const ArticulationVec*          partListPtr = nullptr;
   ArticulationMap::const_iterator ami         = mArticulationByName.find(aPlatformPtr->GetNameId());
   if (ami != mArticulationByName.end())
   {
      partListPtr = &(ami->second);
   }
   else
   {
      ami = mArticulationByType.find(aPlatformPtr->GetTypeId());
      if (ami != mArticulationByType.end())
      {
         partListPtr = &(ami->second);
      }
   }
   if (partListPtr == nullptr)
   {
      return;
   }

   std::vector<WsfStringId> scriptIds;
   auto&                    context = aPlatformPtr->GetScriptContext();

   for (unsigned int npi = 0; npi < newPartCount; ++npi)
   {
      std::vector<DisEntityPart*>::const_iterator newIter(aNewStatePtr->GetEntityPartBegin());
      const DisEntityPart*                        newPartPtr = newIter[npi];
      if (mDebugAppearanceUpdates)
      {
         auto out = ut::log::debug() << "Platform part information.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Type Designator: " << static_cast<int>(newPartPtr->GetTypeDesignator());
         out.AddNote() << "Change Indicator: " << static_cast<int>(newPartPtr->GetChangeIndicator());
         out.AddNote() << "Attached To: " << newPartPtr->GetPartAttachedTo();
         out.AddNote() << "Parameter Type: " << newPartPtr->GetParameterType();
         out.AddNote() << "Part Value: " << newPartPtr->GetFloat32Value();
      }

      if (newPartPtr->GetTypeDesignator() == 0) // We only deal with articulated parts, not attached parts
      {
         bool partChanged = true;
         for (unsigned int opi = 0; opi < oldPartCount; ++opi)
         {
            std::vector<DisEntityPart*>::const_iterator oldIter(aOldStatePtr->GetEntityPartBegin());
            const DisEntityPart*                        oldPartPtr = oldIter[opi];
            if ((newPartPtr->GetPartAttachedTo() == oldPartPtr->GetPartAttachedTo()) &&
                (newPartPtr->GetParameterType() == oldPartPtr->GetParameterType()))
            {
               partChanged = (newPartPtr->GetChangeIndicator() != oldPartPtr->GetChangeIndicator());
               break;
            }
         }

         // If a part has changed, determine if it is one being tracked. If it is, update the corresponding script
         // variable and schedule the corresponding script call.

         if (partChanged)
         {
            // Determine if changes for this part are being tracked.
            for (const Articulation& articulation : *partListPtr)
            {
               if (articulation.mPartNumber == static_cast<int>(newPartPtr->GetParameterType()))
               {
                  if (!articulation.mVariableId.IsNull())
                  {
                     UtScriptData* varPtr = context.GetContext().FindVar(articulation.mVariableId);
                     if (varPtr != nullptr)
                     {
                        varPtr->SetDouble(static_cast<double>(newPartPtr->GetFloat32Value()));
                        if (std::find(scriptIds.begin(), scriptIds.end(), articulation.mScriptId) == scriptIds.end())
                        {
                           scriptIds.push_back(articulation.mScriptId);
                        }
                        if (mDebugAppearanceUpdates)
                        {
                           auto out = ut::log::debug() << "Platform part changed.";
                           out.AddNote() << "T = " << aSimTime;
                           out.AddNote() << "Platform: " << aPlatformPtr->GetName();
                           out.AddNote() << "Parameter Type: " << newPartPtr->GetParameterType();
                           out.AddNote() << "Variable: " << articulation.mVariableId;
                        }
                     }
                  }
                  else
                  {
                     auto out = ut::log::error() << "Unable to set appearance script variable.";
                     out.AddNote() << "Platform: " << aPlatformPtr->GetName();
                     out.AddNote() << "Variable: " << articulation.mVariableId;
                  }
               }
            }
         }
      }
   }

   // Invoke any scripts that need to be called.
   if (!scriptIds.empty())
   {
      for (auto& scriptId : scriptIds)
      {
         if (!context.ExecuteScript(aSimTime, scriptId))
         {
            auto out = ut::log::error() << "Unable to execute appearance update.";
            out.AddNote() << "Platform: " << aPlatformPtr->GetName();
            out.AddNote() << "Script: " << scriptId;
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////


// ============================================================================
//! This is called by the internal DeferredTransmitEvent to send out an Emission PDU.
//!
//! Numerous events that require transmitting an Emissions PDU may occur almost simultaneously.
//! In order to avoid sending a PDU with each event, the first event will schedule an event that
//! will trigger a very short  time (0.001 seconds) in the future. The event will call this
//! routine while will then send the PDU.  All of events that occur during the first event and
//! the time the transmission event will simply update the DIS system/beams as required. The
//! transmitted PDU will then contain the sum of the effects of the events in a single PDU.
//!
//! This is necessary to account for such a situation as when a single detection could cause a
//! transition into detect state and then into track state (1-out-of-1). Without this, two PDU's
//! would be sent for the same time, which causes problems to some receivers.
void WsfDisInterface::ExecuteDeferredTransmitEvent(double aSimTime, size_t aPlatformIndex)
{
   WsfDisPlatform* disPlatformPtr = GetDisPlatform(aPlatformIndex);
   if (disPlatformPtr != nullptr)
   {
      disPlatformPtr->SendEmissionsPDU(aSimTime);
   }
}

// ============================================================================
//! This is called by the internal SensorUpdateEvent to send out Emission PDU's
//! in support of the 'sensor_update_interval'.
double WsfDisInterface::ExecuteSensorUpdateEvent(double aSimTime)
{
   double nextUpdateTime = aSimTime + mSensorUpdateInterval; // Assume nothing in list.
   if (!mSensorUpdateList.empty())
   {
      // Sensor updates are distributed throughout the sensor update interval. If there are a lot
      // of sensors then a batch will be sent with each event in order to avoid too much event scheduling.

      double timeStep = GetSimulation().GetTimestep();
      if (timeStep <= 0.0)
      {
         timeStep = 0.050; // Assume 20 Hz if not time-stepped.
      }

      double updateIncrement = mSensorUpdateInterval / (double)mSensorUpdateList.size();
      nextUpdateTime         = aSimTime + std::max(timeStep, updateIncrement);
      double time            = aSimTime;
      while (time < nextUpdateTime)
      {
         time += updateIncrement;

         size_t          platformIndex  = mSensorUpdateList.front();
         WsfDisPlatform* disPlatformPtr = GetDisPlatform(platformIndex);
         if (disPlatformPtr != nullptr)
         {
            disPlatformPtr->SendEmissionsPDU(aSimTime);
            // Move the front entry to the back.
            mSensorUpdateList.splice(mSensorUpdateList.end(),    // target position
                                     mSensorUpdateList,          // source list
                                     mSensorUpdateList.begin()); // source position
         }
         else
         {
            // Platform no longer exists - just remove it from the list.
            // (This should not exist because 'PlatformDeleted' should have already removed it.
            mSensorUpdateList.pop_front();
         }
      }
   }
   return nextUpdateTime;
}


// ============================================================================
//! Update a DIS Beam record from a Transmitter object.
// public
void WsfDisInterface::UpdateBeamFromXmtr(WsfEM_Xmtr& aXmtr,
                                         DisBeam*    aBeamPtr,
                                         double      aMinAz,
                                         double      aMaxAz,
                                         double      aMinEl,
                                         double      aMaxEl)
{
   aBeamPtr->SetFrequency(static_cast<float>(aXmtr.GetFrequency()));
   aBeamPtr->SetFrequencyRange(static_cast<float>(aXmtr.GetBandwidth()));

   double power_W = aXmtr.GetPower();
   power_W /= aXmtr.GetInternalLoss();
   power_W *= aXmtr.GetPeakAntennaGain();
   power_W           = std::max(power_W, 0.001001); // Must be greater than 1 milliwatt
   double power_dBmW = 10.0 * log10(power_W / 0.001);
   aBeamPtr->SetEffectiveRadiatedPower(static_cast<float>(power_dBmW));

   double prf = aXmtr.GetPulseRepetitionFrequency();
   double pw  = aXmtr.GetPulseWidth(); // seconds
   if (prf <= 1.0)                     // Continuous wave
   {
      prf = 0.0;
      pw  = 0.0;
   }
   aBeamPtr->SetPulseRepetitionFrequency(static_cast<float>(prf));
   aBeamPtr->SetPulseWidth(static_cast<float>(pw * 1.0E6));

   WsfEM_Antenna*      antennaPtr = aXmtr.GetAntenna();
   WsfArticulatedPart* partPtr    = antennaPtr->GetArticulatedPart();
   double              yaw;
   double              pitch;
   double              roll;
   partPtr->GetCurrentOrientationECS(yaw, pitch, roll);
   double minAz = aMinAz;
   double maxAz = aMaxAz;
   double minEl = aMinEl;
   double maxEl = aMaxEl;
   GetExtInterface()->GetEmission()->UpdateBeamLimits(aXmtr, minAz, maxAz, minEl, maxEl);

   double azCue(0.0);
   double elCue(0.0);
   if ((partPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED) && (!partPtr->IsCued()) &&
       (!partPtr->TransientCueActive()))
   {
      partPtr->GetCurrentCuedOrientation(azCue, elCue);
   }

   // TODO - Future needs to have the cued orientation set with the yaw, pitch separated
   //        Capability is not in the DIS 6 standard though.
   aBeamPtr->SetAzimuthCenter(static_cast<DisFloat32>(yaw + azCue + 0.5 * (maxAz + minAz)));
   aBeamPtr->SetAzimuthSweep(static_cast<DisFloat32>(0.5 * (maxAz - minAz)));
   aBeamPtr->SetElevationCenter(static_cast<DisFloat32>(pitch + elCue + 0.5 * (maxEl + minEl)));
   aBeamPtr->SetElevationSweep(static_cast<DisFloat32>(0.5 * (maxEl - minEl)));
}

void WsfDisInterface::UpdateXmtrFromBeam(WsfDisEmission* aEmissionPtr, DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn)
{
   for (auto component : mComponents)
   {
      if (component->UpdateXmtrFromBeam(aEmissionPtr, aBeamPtr, aXmtrPtr, aBeamOn))
      {
         // a component did the update for us
         return;
      }
   }

   // no components could update the xmtr
   WsfEM_Antenna* antennaPtr = aXmtrPtr->GetAntenna();
   if (antennaPtr != nullptr)
   {
      // Some phased array engagement radars are mounted on a 'pedestal' or 'turn-table'. At the start of an
      // engagement the pedestal is rotated to point the antenna face along the 'attack-axis'. The azimuth/elevation
      // beam centers in the incoming PDU will have this rotation angle (as well as other effects). It is very
      // important to reconstruct the yaw angle because the ability of a passive sensor to detect one of these systems is
      // highly dependent on which way it is pointing! The problem does not exist for simple 360 degree search
      // radars because at some point during their scan they would always point at the passive sensor.

      if (aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_SENSOR)
      {
         // We don't mess with simple 360 degree scan system.
         // The DIS standard says these should always have a center of 0.0.
         if (aBeamPtr->GetAzimuthCenter() != 0.0F)
         {
            // The xmtr[Min/Max][Az/El] represent the limits about the part coordinate system.
            double xmtrMinAz = 0.0;
            double xmtrMaxAz = 0.0;
            double xmtrMinEl = 0.0;
            double xmtrMaxEl = 0.0;
            GetExtEmission()->UpdateBeamLimits(*aXmtrPtr, xmtrMinAz, xmtrMaxAz, xmtrMinEl, xmtrMaxEl);
            double xmtrAzCenter = 0.5 * (xmtrMaxAz + xmtrMinAz);
            double xmtrElCenter = 0.5 * (xmtrMaxEl + xmtrMinEl);
            double beamAzCenter = aBeamPtr->GetAzimuthCenter();
            double beamElCenter = aBeamPtr->GetElevationCenter();

            // Set the yaw/pitch of the center based on the difference between the beam center and the
            // defined center in the transmitter.

            double yaw   = beamAzCenter - xmtrAzCenter;
            double pitch = beamElCenter - xmtrElCenter;
            // TODO - Future needs to have the cued orientation set with the yaw, pitch separated
            //        Capability is not in the DIS 6 standard though.
            if (aXmtrPtr->GetArticulatedPart()->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED)
            {
               aXmtrPtr->GetArticulatedPart()->SetCuedOrientation(yaw, pitch);
            }
            else
            {
               aXmtrPtr->GetArticulatedPart()->SetOrientation(yaw, pitch, 0.0);
            }
            if (GetExtEmission()->GetDebugMask() & WsfExtEmission::cLEVEL_TWO_DEBUG)
            {
               auto out = ut::log::debug() << "Transmitter part coordinate system raw data.";
               out.AddNote() << "Min Az: " << xmtrMinAz * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Max Az: " << xmtrMaxAz * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Min El: " << xmtrMinEl * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Max El: " << xmtrMaxEl * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Center Ax: " << xmtrAzCenter * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Center El: " << xmtrElCenter * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Beam Center Az: " << beamAzCenter * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Beam Center El: " << beamElCenter * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Yaw: " << yaw * UtMath::cDEG_PER_RAD;
               out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD;
            }
         }
      }
   }
}

// ============================================================================
//! Update a DIS System record from a Sensor object.
// public
void WsfDisInterface::UpdateSystemFromSensor(WsfSensor* aSensorPtr, DisSystem* aSystemPtr)
{
   double xyz[3] = {0.0, 0.0, 0.0};
   aSensorPtr->GetLocation(xyz);
   aSystemPtr->SetLocation(static_cast<DisFloat32>(xyz[0]),
                           static_cast<DisFloat32>(xyz[1]),
                           static_cast<DisFloat32>(xyz[2]));

   size_t beamCount = aSystemPtr->GetNumberOfBeams();
   size_t xmtrCount = aSensorPtr->GetEM_XmtrCount();
   if (beamCount == 0)
   {
      // System is being turned off.
   }
   else if (xmtrCount == 0)
   {
      // Passive sensor...
      //
      // A beam is produced even for passive sensors so external applications
      // can display where the sensor is looking and what it is tracking.

      auto beamIter(aSystemPtr->GetBeamBegin());
      if (beamIter != aSystemPtr->GetBeamEnd())
      {
         DisBeam* beamPtr = *beamIter;
         beamPtr->SetFrequency(250.0E+6);
         beamPtr->SetFrequencyRange(0.0F);

         // Set the power to some small value not equal to 1mW.  It must not be 1mw because 1mW converted
         // to dBmW is 0 and a well-accepted Boeing standard treats beams with 0 in the power field as
         // being 'off'.

         double power_W    = 0.001001;
         double power_dBmW = 10.0 * log10(power_W / 0.001);
         beamPtr->SetEffectiveRadiatedPower(static_cast<DisFloat32>(power_dBmW));

         double yaw;
         double pitch;
         double roll;
         aSensorPtr->GetCurrentOrientationECS(yaw, pitch, roll);
         // Assume no receiver is present.
         double minAz = 0.0;
         double maxAz = 0.0;
         double minEl = 0.0;
         double maxEl = 0.0;
         if (aSensorPtr->GetEM_RcvrCount() != 0)
         {
            WsfEM_Rcvr& rcvr = aSensorPtr->GetEM_Rcvr(0);
            GetExtInterface()->GetEmission()->UpdateBeamLimits(rcvr, minAz, maxAz, minEl, maxEl);
         }
         else
         {
            minAz = -15.0 * UtMath::cRAD_PER_DEG;
            maxAz = 15.0 * UtMath::cRAD_PER_DEG;
            minEl = -15.0 * UtMath::cRAD_PER_DEG;
            maxEl = 15.0 * UtMath::cRAD_PER_DEG;
         }
         beamPtr->SetAzimuthCenter(static_cast<DisFloat32>(yaw + 0.5 * (maxAz + minAz)));
         beamPtr->SetAzimuthSweep(static_cast<DisFloat32>(0.5 * (maxAz - minAz)));
         beamPtr->SetElevationCenter(static_cast<DisFloat32>(pitch + 0.5 * (maxEl + minEl)));
         beamPtr->SetElevationSweep(static_cast<DisFloat32>(0.5 * (maxEl - minEl)));
      }
   }
   else
   {
      // Only update beams for which transmitters exist. (There may be one more beam that transmitters
      // if the extra CME detect beam is included). There should not be few beams than transmitters, but
      // if there is then the data from the additional transmitters will be ignored.
      size_t xmtrIndex = 0;
      auto   beamIter(aSystemPtr->GetBeamBegin());
      while ((beamIter != aSystemPtr->GetBeamEnd()) && (xmtrIndex < xmtrCount))
      {
         double minAz = 0.0;
         double maxAz = 0.0;
         double minEl = 0.0;
         double maxEl = 0.0;

         DisBeam*    beamPtr = *beamIter;
         WsfEM_Xmtr& xmtr    = aSensorPtr->GetEM_Xmtr(xmtrIndex);

         // If this is a sensor which is pointed at a single location (such as a SAR) or tracks a single
         // object, then attempt to use the beam widths from the antenna definition.

         if (xmtr.GetMode())
         {
            WsfSensorMode* modePtr = aSensorPtr->GetModeEntry(xmtr.GetMode()->GetModeIndex());
            if ((modePtr) && (modePtr->GetMaximumRequestCount() <= 1)) // Not a multi-target tracking sensor.
            {
               WsfAntennaPattern* patternPtr = xmtr.GetAntennaPattern(xmtr.GetPolarization(), xmtr.GetFrequency());
               if (patternPtr)
               {
                  double azBeamwidth = patternPtr->GetAzimuthBeamwidth(xmtr.GetFrequency(), 0.0, 0.0);
                  double elBeamwidth = patternPtr->GetElevationBeamwidth(xmtr.GetFrequency(), 0.0, 0.0);

                  // Don't use the beamwidth in directions that we scan. (e.g. for a height finder we don't use the
                  // elevation beamwidth because the elevation limits should come from the scan limits later on).
                  switch (xmtr.GetAntenna()->GetScanMode())
                  {
                  case WsfEM_Antenna::cSCAN_AZ:
                     azBeamwidth = 0.0;
                     break;
                  case WsfEM_Antenna::cSCAN_EL:
                     elBeamwidth = 0.0;
                     break;
                  case WsfEM_Antenna::cSCAN_AZ_EL:
                     azBeamwidth = 0.0;
                     elBeamwidth = 0.0;
                     break;
                  default:
                     break;
                  }

                  // Use a beamwidth for the limit if it is defined (or not canceled out above...)

                  if (azBeamwidth != 0.0)
                  {
                     maxAz = 0.5 * azBeamwidth;
                     minAz = -maxAz;
                  }
                  if (elBeamwidth != 0.0)
                  {
                     maxEl = 0.5 * elBeamwidth;
                     minEl = -maxEl;
                  }
               }
            }
         }

         GetExtInterface()->GetEmission()->UpdateBeamLimits(xmtr, minAz, maxAz, minEl, maxEl);
         UpdateBeamFromXmtr(xmtr, beamPtr, minAz, maxAz, minEl, maxEl);
         ++beamIter;
         ++xmtrIndex;
      }
   }
}


// ============================================================================
//! Called by WsfDisInterface to reinitialize input to default values.
void WsfDisInterface::InitializeInputData()
{
   mSensorUpdateList.clear();
   mSensorUpdateScheduled = false;
}

void WsfDisInterface::ReceivedBadEntityState()
{
   ++mNullEntityIdCount;
   if (mNullEntityIdCount <= mMaxBadPDU_Count)
   {
      auto out = ut::log::warning() << "Entity State PDU received with no entity ID. Ignored.";
      if (mNullEntityIdCount == mMaxBadPDU_Count)
      {
         out.AddNote() << "Future occurrences of this message will be suppressed.";
      }
   }
}

// ============================================================================
//! Set the beam type (AKA 'beam parameter index') and beam function in a DIS Beam record.
//!
//! @param aPartPtr         Pointer to the part (comm, sensor, etc) that contains the beam.
//! @param aModeNameId      The name (ID) of the mode in which the beam is a part.
//! @param aBeamNumber      The 1-based mode number of the beam within the mode.
//! @param aBeamPtr         Pointer to the DIS Beam to be updated.
//! @param aDefaultFunction The default beam function to be used if one was not defined by the inputs.
void WsfDisInterface::SetBeamTypeAndFunction(WsfPlatformPart* aPartPtr,
                                             WsfStringId      aModeNameId,
                                             int              aBeamNumber,
                                             DisBeam*         aBeamPtr,
                                             DisEnum8         aDefaultFunction)
{
   WsfStringId partTypeId   = aPartPtr->GetTypeId();
   DisInt16    beamType     = GetExtInterface()->GetEmission()->GetBeamType(partTypeId, aModeNameId, aBeamNumber);
   DisEnum8    beamFunction = GetExtInterface()->GetEmission()->GetBeamFunction(partTypeId, aModeNameId, aBeamNumber);
   if (beamFunction == 0)
   {
      beamFunction = aDefaultFunction;
   }

   // The DIS Beam Parameter Index is used to store what WSF calls the 'beam_type'.
   aBeamPtr->SetParameterIndex(beamType);
   aBeamPtr->SetFunction(beamFunction);
}

// ============================================================================
//! Set the emitter type (AKA 'emitter name') and function in the specified DIS System record.
//!
//! @param aPartPtr   Pointer to the associated WSF part (comm, sensor, etc)
//!                   that represents the system.
//! @param aSystemPtr Pointer to the DIS System to be updated.
void WsfDisInterface::SetEmitterTypeAndFunction(WsfPlatformPart* aPartPtr, DisSystem* aSystemPtr)
{
   WsfStringId partTypeId      = aPartPtr->GetTypeId();
   DisEnum16   emitterType     = GetExtEmission()->GetEmitterType(partTypeId);
   DisEnum8    emitterFunction = GetExtEmission()->GetEmitterFunction(partTypeId);
   // Allow an application, such as one used to test hardware test bench modules
   // to override the emitter id.
   WsfDisEmission::OverrideTypeAndFunction(aPartPtr, aSystemPtr, emitterType, emitterFunction);

   // The DIS Emitter Name is used to store what WSF calls the 'emitter_type'.
   aSystemPtr->SetName(emitterType);
   aSystemPtr->SetFunction(emitterFunction);
}

WsfExtEmission* WsfDisInterface::GetExtEmission() const
{
   return mExtInterfacePtr->GetEmission();
}

ut::Random& WsfDisInterface::GetRandom()
{
   return mExtInterfacePtr->GetRandom();
}


// ===================================================
WsfDisInterface::PauseUpdateEvent::PauseUpdateEvent(double                           aWallTime,
                                                    double                           aSimTime,
                                                    double                           aWallUpdateInterval,
                                                    unsigned int                     aNumberOfIndicesToSend,
                                                    const std::vector<unsigned int>& aPlatformIndices,
                                                    WsfDisInterface*                 aInterfacePtr)
   : WsfEvent()
   , mInterfacePtr(aInterfacePtr)
   , mWallUpdateInterval(aWallUpdateInterval)
   , mSimTime(aSimTime)
   , mNumberPlatIndicesPerExecute(aNumberOfIndicesToSend)
   , mSendCounter(0)
   , mPlatformIndices(aPlatformIndices)
{
   SetTime(aWallTime + aWallUpdateInterval);
}

// ===================================================
WsfEvent::EventDisposition WsfDisInterface::PauseUpdateEvent::Execute()
{
   if (mInterfacePtr->PauseIsInProgress())
   {
      // Assemble a small array (from a larger array) of platform Indices
      // that tell which platform indices to send entity states out:
      std::vector<unsigned int> indexesToSend;
      for (unsigned int i = 0; i < mNumberPlatIndicesPerExecute; ++i)
      {
         unsigned int largeArrayIndex = mSendCounter++ % mPlatformIndices.size();
         indexesToSend.push_back(mPlatformIndices[largeArrayIndex]);
         if (mSendCounter >= mPlatformIndices.size())
         {
            mSendCounter = 0; // to prevent overflow
         }
      }

      if (indexesToSend.empty())
      {
         return WsfEvent::cDELETE;
      }

      // Send the PDUs, and reschedule the event:
      mInterfacePtr->SendPeriodicPdusWhilePaused(indexesToSend);
      SetTime(GetTime() + mWallUpdateInterval);
      return WsfEvent::cRESCHEDULE;
   }
   else
   {
      return WsfEvent::cDELETE;
   }
}

// ==============================================================================
// ==================== END Simulation Pause Activity ===========================
// ==============================================================================
