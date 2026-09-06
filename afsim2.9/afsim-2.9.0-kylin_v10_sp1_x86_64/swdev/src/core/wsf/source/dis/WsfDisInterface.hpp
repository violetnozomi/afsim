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

#ifndef WSFDISINTERFACEP_HPP
#define WSFDISINTERFACEP_HPP

#include <bitset>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class DisBeam;
#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
class DisEntityState;
#include "DisEntityType.hpp"
class DisPdu;
class DisPduFactory;
class DisSystem;

#include "UtCallbackHolder.hpp"
class UtCalendar;
#include "UtRandom.hpp"

class WsfDisArticulatedPartList;
#include "WsfDisDevice.hpp"
class WsfDisEntityState;
#include "WsfDisExchange.hpp"
#include "WsfDisIO_Thread.hpp"
class WsfDisFilterAllowance;
class WsfDisPlatform;
class WsfDisDraw;
#include "WsfDisInput.hpp"
class WsfDisVisualization;
#include "wsf_export.h"

#include "WsfEvent.hpp"
class WsfExtInterface;
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"
#include "ext/WsfExtEntityType.hpp"
class WsfTrack;
class WsfSimulation;
class WsfExtEmission;

namespace wsf
{
namespace comm
{
class Comm;
class Result;
} // namespace comm
} // namespace wsf

//! This is the 'private' part of WsfDisInterface.
//!
//! The DIS interface has been split into two parts in order to allow the
//! compilation of a WSF application without requiring the presence of the
//! DIS or GenIO include files (obviously they would still be required for
//! linking!).
//!
//! The private part contains all the references to any of the DIS objects.

class WSF_EXPORT WsfDisInterface : public WsfSimulationExtension, public WsfDisInput
{
public:
   friend class WsfDisPlatform;
   friend class WsfDisEntityState;

   static DisUint16        cSHADOW_SITE;
   static DisUint16        cMIN_PLAYBACK_SITE;
   static DisUint16        cMAX_PLAYBACK_SITE;
   static WsfDisInterface* Find(const WsfSimulation& aSimulation);
   WsfDisInterface(WsfDisInput& aScenario, WsfSimulation& aSim, WsfExtInterface* aExtInterfacePtr);
   void AddedToSimulation() override;
   bool Initialize() override;
   ~WsfDisInterface() override;

   bool PrepareExtension() override;

   using WsfSimulationExtension::GetScenario;

   bool ActivateConnection(double aSimTime);

   WsfDisPlatform* AddExternalPlatform(double aSimTime, std::unique_ptr<WsfDisEntityState> aEntityPtr);

   bool ClearIgnoredSiteAndApp(unsigned short aSite, unsigned short aApp);
   bool ClearIgnoredSitesAndApps();

   WsfDisPlatform* FindDisPlatform(const DisEntityId& aEntityId);

   double GetSimTime() const { return mSimTime; }

   WsfDisPlatform* GetDisPlatform(size_t aPlatformIndex);

   size_t GetDisPlatformCount() const { return mDisPlatforms.size(); }

   WsfExtEntityDeadReckon* GetDeadReckonPtr() { return mDeadReckonPtr; }

   void GetEntityId(WsfPlatform* aPlatformPtr, DisEntityId& aEntityId);

   void GetEntityIdFromIndex(size_t aPlatformIndex, DisEntityId& aEntityId);

   //! Return the orientation threshold (radians).
   double GetEntityOrientationThreshold() const { return mDeadReckonPtr->GetEntityOrientationThreshold(); }

   //! Return the entity position threshold (meters).
   double GetEntityPositionThreshold() const { return mDeadReckonPtr->GetEntityPositionThreshold(); }

   //! Return the heartbeat timer (seconds).
   double GetHeartbeatTimer() const { return mDeadReckonPtr->GetHeartbeatTimer(); }

   //! Return the heartbeat multiplier.
   double GetHeartbeatMultiplier() const { return mHeartbeatMultiplier; }

   double GetTimeFromPdu(DisPdu& aPdu) const;

   //! Return whether we have directed the simulation to autostart when in networked mode.
   //! @note This value only has meaning for networked simulations.
   bool GetAutostart() const { return mAutostart; }

   //! Returns number of sent DIS PDUs.
   unsigned int GetPutCounter() const { return mPutCounter; }

   //! Sets the number of sent DIS PDUs.
   void SetPutCounter(int aCount) { mPutCounter = aCount; }

   //! Returns true if at least device is configured for receiving DIS PDU's
   bool HasInputDevice() const { return mHasInputDevice; }

   //! Returns true if at least device is configured for sending DIS PDU's
   bool HasOutputDevice() const { return mHasOutputDevice; }

   //! Returns true if at least one device is a external device.
   bool HasExternalDevice() const { return mHasExternalDevice; }

   //! Returns true if a device is configured for sending or receiving PDU's
   bool HasDevice() const { return HasInputDevice() || HasOutputDevice(); }

   unsigned int GetDeviceCount() const { return static_cast<unsigned int>(mDevices.size()); }

   WsfDisDevice* GetDevice(unsigned int aIndex) { return mDevices[aIndex]; }
   using DeviceList = std::vector<WsfDisDevice*>;
   DeviceList& GetDeviceList() { return mDevices; }

   //! Returns the send port used for DIS output
   int GetSendPort() const;

   //! Returns the receive port used for DIS input
   int GetRcvPort() const;

   //! Returns initial distribution time
   double GetInitialDistributionInterval() { return mDeadReckonPtr->GetInitialDistributionInterval(); }

   DisPduFactory* GetPduFactory() { return mPduFactoryPtr; }

   DisPdu* GetPdu();

   //! Get the local random number generator.
   ut::Random& GetRandom();

   void PutPdu(double aSimTime, std::unique_ptr<DisPdu> aPduPtr);

   void ImmediatePutPdu(double aSimTime, DisPdu& aPduPtr);

   static void ReadEntityId(UtInput& aInput, DisEntityId& aEntityId);

   static void ReadEntityType(UtInput& aInput, DisEntityType& aEntityType);

   void SetHeartbeatMultiplier(double aValue) { mHeartbeatMultiplier = aValue; }

   void SetPduFactory(DisPduFactory* aFactoryPtr);

   bool GetSuppressDisOutput() { return mSuppressDisOutput; }
   void SetSuppressDisOutput(bool aFlag) { mSuppressDisOutput = aFlag; }

   bool GetSuppressEmissionsData() const { return mSuppressEmissionsData; }

   bool SuppressAllNonStandardPDUs()
   {
      return (mSuppressCME_DetectBeam & mSuppressCME_EntityData & mSuppressCME_PassiveSensor);
   }

   //! @name XIO auto_mapping support.
   //@{
   using DelayEntityStateFn = bool (*)(WsfDisInterface& aInterfacePtr, const DisEntityId& aEntityId);
   using CreatePlatformFn   = WsfPlatform* (*)(WsfDisInterface&   aInterface,
                                             WsfStringId        aPlatformType,
                                             WsfDisEntityState* aEntityStatePtr);

   void SetDelayEntityStateFunction(DelayEntityStateFn aDelayFn) { mEntityStateDelayFunctionPtr = aDelayFn; }
   void SetCreateDisPlatformFunction(CreatePlatformFn aModifyFn) { mCreatePlatformFunctionPtr = aModifyFn; }
   void ResumeEntityStateProcessing(double aSimTime, const DisEntityId& aEntityId);
   //@}

   //@ The remaining methods are called via the simulation observer interface.
   //@{

   void AdvanceTime(double aSimTime);

   void Comment(double aSimTime, WsfPlatform* aPlatformPtr, const std::string& aComment);

   void CommFrequencyChanged(double aSimTime, wsf::comm::Comm* aCommPtr);

   void CommTurnedOff(double aSimTime, wsf::comm::Comm* aCommPtr);

   void CommTurnedOn(double aSimTime, wsf::comm::Comm* aCommPtr);

   void FrameComplete(double aSimTime);

   void FramePlatformsUpdated(double aSimTime);

   void FrameStarting(double aSimTime);

   bool UseSimpleOrientationRate() { return mUseSimpleOrientRates; }
   bool UseWorldToBodyAngularVelocities() { return mUseWorldToBodyAngularVelocities; }
   bool ZeroWorldToBodyAngularVelocities() { return mZeroBodyAngularVelocities; }

   bool IgnoreStopFreezePDU() { return mIgnoreStopFreezePDU; }

   void MessageReceived(double             aSimTime,
                        wsf::comm::Comm*   aXmtrPtr,
                        wsf::comm::Comm*   aRcvrPtr,
                        const WsfMessage&  aMessage,
                        wsf::comm::Result& aResult);

   void MessageTransmitted(double aSimTime, wsf::comm::Comm* aCommPtr, const WsfMessage& aMessage);

   void MoverUpdated(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData);

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   void RequestTimeAdvance(double aSimTime, double aMaxRate, double aTimeStep);

   void SensorDetectionChanged(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetPlatformIndex, WsfSensorResult& aResult);

   void SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorRequestInitiated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void SensorRequestUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void SensorScanUpdated(double aSimTime, WsfSensor* aSensorPtr);

   void SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr);

   void SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr);

   static void SimTimeToDisClockTime(double aSimTime, DisClockTime& aClockTime);

   void SimulationPausing();
   void SimulationResuming();

   void SimulationComplete(double aSimTime);
   void SimulationDestroyed(WsfSimulation* aSimulationPtr);
   void SimulationStarting();
   //@}

   DisUint32 GetNextRequestNumber() { return ++mSerialRequestNumber; }
   DisUint8  GetReliabilityService() const { return mReliabilityService; }

   bool PauseIsInProgress() const { return mPauseIsInProgress; }
   void SendPeriodicPdusWhilePaused(std::vector<unsigned int>& aEntityIndices);
   void SendDateAndTime(WsfDisDevice& aDevice, const DisEntityId& aReceivingId, const UtCalendar& aDateTime);

   bool ShouldJoinExercise() const { return mJoinExercise; }

   void WriteEntityTypes(std::ostream& aStream);

   WsfExtInterface*           GetExtInterface() const { return mExtInterfacePtr; }
   WsfExtEmission*            GetExtEmission() const;
   WsfDisArticulatedPartList* GetArticulatedParts() const { return mArticulatedPartListPtr; }

   // allow real-time filtering
   void EnableFiltering() { mFilteringEnabled = true; }
   void ResetFiltering();

   std::map<DisEntityId, WsfDisPlatform*>& GetEntityIdToDisPlatform() { return mEntityIdToDisPlatform; }

   //! @name Methods for dealing for updating PDU's from WSF objects and vis-versa.
   //@{
   void ExtrapolatePlatformState(double aSimTime, WsfDisPlatform* aDisPlatformPtr);

   void PrepareFinalEntityState(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr);

   bool UpdateAppearanceFromPlatform(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr);

   void UpdatePlatformFromAppearance(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr);

   void UpdatePlatformFromEntityState(double aSimTime, WsfDisPlatform* aDisPlatformPtr, DisEntityState* aEntityStatePtr);

   void UpdatePlatformSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr);

   void SetBeamTypeAndFunction(WsfPlatformPart* aPartPtr,
                               WsfStringId      aModeNameId,
                               int              aBeamNumber,
                               DisBeam*         aBeamPtr,
                               DisEnum8         aDefaultFunction);

   void SetEmitterTypeAndFunction(WsfPlatformPart* aPartPtr, DisSystem* aSystemPtr);

   void UpdateBeamFromXmtr(WsfEM_Xmtr& aXmtr, DisBeam* aBeamPtr, double aMinAz, double aMaxAz, double aMinEl, double aMaxEl);

   void UpdateXmtrFromBeam(WsfDisEmission* aEmissionPtr, DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn);

   void UpdateSystemFromSensor(WsfSensor* aSensorPtr, DisSystem* aSystemPtr);
   //@}

   void UpdateLastSimTime(double aSimTime) { mLastSimTime = aSimTime; }

   bool IsDeferredConnection() const { return mIsDeferredConnection; }

   void   ExecuteDeferredTransmitEvent(double aSimTime, size_t aPlatformIndex);
   double ExecuteSensorUpdateEvent(double aSimTime);
   void   InitializeInputData();

   void ReceivedBadEntityState();

   bool GetEntityType(WsfStringId aTypeName, DisEntityType& aEntityType) const;

private:
   WsfDisPlatform* AddExternalPlatformP(double aSimTime, std::unique_ptr<WsfDisEntityState> aEntityPtr);

   // The following event is scheduled periodically when in simulation pause to send out
   // Entity State and Emission PDUs so external simulations will not time them out.
   class PauseUpdateEvent : public WsfEvent
   {
   public:
      PauseUpdateEvent(double                           aWallTime,
                       double                           aSimTime,
                       double                           aWallUpdateInterval,
                       unsigned int                     aNumPlatformsPerPulse,
                       const std::vector<unsigned int>& aPlatformIndices,
                       WsfDisInterface*                 aInterfacePtr);
      EventDisposition Execute() override;

   private:
      WsfDisInterface*          mInterfacePtr;
      double                    mWallUpdateInterval;
      double                    mSimTime; // Constant during the pause interval.
      unsigned int              mNumberPlatIndicesPerExecute;
      unsigned int              mSendCounter;
      std::vector<unsigned int> mPlatformIndices;
   };

   // The following event is scheduled to activate the network connection
   // at the time specified by the 'deferred_connection_time' command.

   class ActivateDeferredConnectionEvent : public WsfEvent
   {
   public:
      ActivateDeferredConnectionEvent(double aSimTime, WsfDisInterface* aInterfacePtr)
         : WsfEvent(aSimTime)
         , mInterfacePtr(aInterfacePtr)
      {
      }

      EventDisposition Execute() override;

   private:
      WsfDisInterface* mInterfacePtr;
   };

   void AddCallbacks();

   void AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr);

   void AddShadowPlatform(double aSimTime, const WsfPlatform* aPlatformPtr);

   void DeletePlatform(double aSimTime, size_t aPlatformIndex);

   void CleanupDevices();

   bool GetEntityType(const WsfObject* aObjectPtr, DisEntityType& aEntityType) const;

   bool RunFilterChecks(const DisPdu* aPdu);

   DisUint32 ConvertDamageFactorToDamageState(double aDamageFactor);
   double    ConvertDamageStateToDamageFactor(DisUint32 aDamageState);

   StringToStateIdVec* GetAfterburnerSigStateByType(WsfStringId aType, int aStateFlag);
   StringToStateIdVec* GetAfterburnerSigStateByName(WsfStringId aName, int aStateFlag);

   StringToStateIdVec* GetConfigSigStateByType(WsfStringId aType, int aStateFlag);
   StringToStateIdVec* GetConfigSigStateByName(WsfStringId aName, int aStateFlag);

   void UpdatePlatformFromArticulationRecords(double          aSimTime,
                                              WsfPlatform*    aPlatformPtr,
                                              DisEntityState* aNewStatePtr,
                                              DisEntityState* aOldStatePtr);

   void SetAppearance(DisUint32& aAppearance, WsfPlatform* aPlatformPtr, unsigned int aLSB, unsigned int aNumBits);

   WsfExtInterface* mExtInterfacePtr;
   UtCallbackHolder mCallbacks;
   UtCallbackHolder mConditionalCallbacks;

   std::vector<WsfPlatform*> mAddedPlatforms;
   std::vector<unsigned int> mDeletedPlatforms;

   double mHeartbeatMultiplier;

   bool mIsDeferredConnection;

   WsfDisIO_Thread* mIO_ThreadPtr;

   DeviceList   mDevices;
   bool         mHasInputDevice;
   bool         mHasOutputDevice;
   bool         mHasExternalDevice;
   unsigned int mCurrentGetDevice;

   DisPduFactory* mPduFactoryPtr;

   WsfDisExchange mWsfDisExchange;

   unsigned int mPutCounter;

   DisUint32 mSerialRequestNumber;
   DisUint8  mReliabilityService;

   // The following is used for communication between AddExternalPlatform()
   // and PlatformAdded().

   std::unique_ptr<WsfDisEntityState> mExternalEntityStatePtr;

   // The following table is indexed by the simulation index of a platform.
   std::vector<WsfDisPlatform*> mDisPlatforms;

   // A map of entity ID to DIS platform
   std::map<DisEntityId, WsfDisPlatform*> mEntityIdToDisPlatform;

   // A map of deleted platforms with there corresponding entity id
   // Need to be saved in order to clean up track/jam lists, etc.
   std::map<size_t, DisEntityId> mDelPlatformIndexToEntityId;

   std::vector<std::unique_ptr<WsfDisEntityState>> mDelayedEntityStates;

   DelayEntityStateFn mEntityStateDelayFunctionPtr;
   CreatePlatformFn   mCreatePlatformFunctionPtr;
   double             mSimTime;
   double             mLastSimTime;

   bool mFilteringEnabled;

   //! Suppress dis output
   bool mSuppressDisOutput;

   //! Suppress most dis information to stdout
   bool mPauseIsInProgress;

   //! Handles sending CME drawing PDUs
   WsfDisDraw*          mReplayDrawPtr;
   WsfDisVisualization* mVisualizationPtr;

   //! Pointer to the Dead reckon class for easy access
   WsfExtEntityDeadReckon* mDeadReckonPtr;

   WsfDisArticulatedPartList* mArticulatedPartListPtr;

   //////////////////////////////////////////////////////////////////////////
   // Entity State

   //! Map for changing of signatures via DIS Entity Appearance unused bit
   //! fields (bits 17-20).

   unsigned int               mNullEntityIdCount;
   std::map<DisEntityId, int> mBadDataCount;

   //////////////////////////////////////////////////////////////////////////
   // WsfDisPlatform
   std::list<size_t> mSensorUpdateList;
   bool              mSensorUpdateScheduled;
};

#endif
