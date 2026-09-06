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

#ifndef WSFDISPLATFORM_HPP
#define WSFDISPLATFORM_HPP

#include "wsf_export.h"

#include <list>
#include <map>
#include <memory>
#include <utility>
#include <vector>

class DisBeam;
class DisEmission;
#include "DisEntityState.hpp"
class DisEventId;
#include "ext/WsfExtEntityDeadReckon.hpp"
class DisSystem;
#include "UtVec3.hpp"
#include "WsfComponent.hpp"
class WsfDisArticulatedPart;
class WsfDisInterface;

#include "WsfExchange.hpp"

class WsfPlatform;
#include "WsfSensor.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"

class WsfDisPlatform;

namespace wsf
{
namespace comm
{
class Comm;
class Result;
} // namespace comm
} // namespace wsf

class WSF_EXPORT WsfDisPlatformComponent : public WsfComponentT<WsfDisPlatform>
{
public:
   virtual void SendingEmission(double aSimTime, DisEmission* aEmissionPtr, DisEventId& aPreviousNumber) {}
   virtual void TurnOffEmission(DisEmission* aEmissionPtr) {}
   virtual void AllocateEmissionSystems(DisUint8& emitterId) {}
};


//! An internal class used by WsfDisInterface to maintain information about a platform.

class WSF_EXPORT WsfDisPlatform
{
public:
   using Component     = WsfDisPlatformComponent;
   using ComponentList = WsfComponentListT<WsfDisPlatformComponent>;

   enum PDUTypeType
   {
      cPDUTYPE_NOT_DEFINED  = 0x00000000,
      cPDUTYPE_SIGNAL       = 0x00000010,
      cPDUTYPE_EMISSION     = 0x00000020,
      cPDUTYPE_ENTITY_STATE = 0x00000040,
      cPDUTYPE_TRANSMITTER  = 0x00000080
   };

   WsfDisPlatform(double                          aSimTime,
                  WsfPlatform*                    aPlatformPtr,
                  WsfDisInterface*                aInterfacePtr,
                  std::unique_ptr<DisEntityState> aEntityStatePtr,
                  const DisEntityId&              aEntityId,
                  const DisEntityType&            aEntityType,
                  bool                            aIsIncomingTransfer,
                  bool                            aIsOutgoingTransfer);

   ~WsfDisPlatform();

   //! @name Platform methods.
   //@{

   //! Get the associated platform.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Create the initial emission pdu pointer
   void CreateEmission(double aSimTime, const DisEntityId& aDisEntityId);

   //! Was this platform created in response to an incoming object transfer.
   bool IsIncomingTransfer() const { return mIsIncomingTransfer; }

   //! Has this platform been replaced in response to an incoming object transfer.
   bool HasBeenReplaced() const { return mHasBeenReplaced; }

   //! Indicate if this platform has been replaced in response to an incoming object transfer.
   void SetHasBeenReplaced(bool aHasBeenReplaced) { mHasBeenReplaced = aHasBeenReplaced; }

   //! Get the DIS Entity Id for this platform
   //! @return The DIS Entity Id (Site, App., Entity) for this platform.
   const DisEntityId& GetEntityId() const { return mEntityStatePtr->GetEntityId(); }

   //! Get the DIS Entity Type for this platform.
   //! @return The DIS Entity Type (kind, domain, country, category, subcategory, specific, extra).
   const DisEntityType& GetEntityType() const { return mEntityStatePtr->GetEntityType(); }

   //! Get the DIS Force Id for this platform.
   //! @return The DIS Force Id for this platform.
   DisUint8 GetForceId() const { return mEntityStatePtr->GetForceId(); }

   WsfPlatform* GetShadowPlatform() const { return mShadowPlatformPtr; }
   void         SetShadowPlatform(WsfPlatform* aPlatformPtr) { mShadowPlatformPtr = aPlatformPtr; }

   //@}

   WsfDisInterface* GetInterface() const { return mInterfacePtr; }

   void AddComponent(Component* aComponentPtr) { mComponents.AddComponent(aComponentPtr); }

   const ComponentList& GetComponents() const { return mComponents; }

   //! @name Entity State PDU methods.
   //@{

   int AddArticulation(int aType, int aParentType, unsigned int aPublishBitmap, WsfArticulatedPart* aPartPtr, int aStartCount);

   void ForceUpdate(double aSimTime);

   void GetEntityState(double& aEntityStateUpdateTime, DisEntityState*& aEntityStatePtr);

   void SetEntityState(double aSimTime, DisEntityState* aEntityStatePtr);

   //! Get the pointer to the entity state.
   DisEntityState* GetEntityState() { return mEntityStatePtr.get(); }

   //! Returns last time the entity's state was updated
   double GetEntityStateUpdateTime() const { return mEntityStateUpdateTime; }

   //! Store the time when the entity's state is updated
   void SetEntityStateUpdateTime(double aSimTime) { mEntityStateUpdateTime = aSimTime; }

   //! Return the last time when an entity state PDU was sent.
   double GetLastTimeEntityStateSent() const { return mLastTimeEntityStateSent; }

   //! Return the time the initial state PDU is to be sent.
   double GetInitialDistributionTime() const { return mInitialDistributionTime; }

   //! Return the last time the mover was updated.
   double GetLastTimeMoverUpdated() const { return mLastTimeMoverUpdated; }

   //@}

   //! @name Emission PDU methods.
   //@{

   bool GetEmissionPDU() const { return mSendEmissionPDU; }
   void SendEmissionsPDU(double aSimTime);

   DisSystem* GetSensorSystem(WsfSensor* aSensorPtr);

   //! System-Beam key - a combination of the unique IDs.
   using SB_Key             = std::pair<unsigned char, unsigned char>;
   using SB_MapType         = std::map<SB_Key, unsigned int>;
   using SB_MapTypeIterator = std::map<SB_Key, unsigned int>::iterator;
   using SB_MapTypePair     = std::pair<SB_Key, unsigned int>;

   unsigned char GetPartIdToBeamId(unsigned int aUniqueId);

   unsigned int GetSysBeamIdToPartId(SB_Key aSB_Key);
   void         SetSysBeamIdToPartId(SB_Key aSB_Key, unsigned int aUniqueId);
   //@}

   //! @name Simulation observer methods.
   //@{

   void Comment(double aSimTime, const std::string& aComment);

   void CommFrequencyChanged(double aSimTime, wsf::comm::Comm* aCommPtr);
   void CommTurnedOff(double aSimTime, wsf::comm::Comm* aCommPtr);
   void CommTurnedOn(double aSimTime, wsf::comm::Comm* aCommPtr);


   void MessageReceived(double             aSimTime,
                        wsf::comm::Comm*   aXmtrPtr,
                        wsf::comm::Comm*   aRcvrPtr,
                        const WsfMessage&  aMessage,
                        wsf::comm::Result& aResult);

   void MessageTransmitted(double aSimTime, wsf::comm::Comm* aCommPtr, const WsfMessage& aMessage);

   void MoverUpdated(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData);

   void PlatformDeleted(double aSimTime);

   void PlatformInitialized(double aSimTime);

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

   void ResetEntityId(const DisEntityId& aDisEntityId);

   void SetEntityId(const DisEntityId& aDisEntityId);

   //@}

   //! @name Miscellaneous methods.
   //@{

   //! Returns indicator whether or not other platform is being tracked by this one
   bool PlatformBeingTracked(DisEntityId& anEntityId);
   //! Returns indicator whether or not DIS PDUs will be sent for the platform.
   bool GetSendDis() const { return mSendDis; }
   //! Ability to set whether or not DIS PDUs are to be sent for this platform once the run has started.
   void SetSendDis(bool aFlag) { mSendDis = aFlag; }
   //! Ability to set whether or not transmitter PDUs are to be sent for this platform
   //! once the run has started.
   void SetSendTransmitterPdus(bool aFlag) { mSendTransmitterPdus = aFlag; }

   //! Get/Set the associated WsfExchange::Query information.
   WsfExchange::Query* GetQueryPtr() { return mQueryPtr.get(); }
   WSF_DEPRECATED void SetQueryPtr(WsfExchange::Query* aQueryPtr);
   void                SetQueryPtr(std::unique_ptr<WsfExchange::Query> aQueryPtr);

   //@}

   //! Set do not extrapolate flag
   void SetDoExtrapolate(bool aFlag) { mDoExtrapolate = aFlag; }

   void TrySendCMEData(double simTime);

   void SendOrQueueEmissionsPDU(double aSimTime);

   // Allow applications to set SendDIS status of platform at creation time.
   using InitialSendDisStatusCallback = UtCallbackListN<void(WsfDisPlatform*, bool&)>;
   static InitialSendDisStatusCallback InitialSendDisStatus;

private:
   class TrackingRequest
   {
   public:
      WsfTrackId  mTrackId;
      size_t      mTargetIndex;
      WsfStringId mModeNameId;
   };

   //! A class used to hold data needed by the interface that is related to a sensor
   class SensorData
   {
   public:
      SensorData(WsfSensor* aSensorPtr, DisSystem* aSystemPtr)
         : mSensorPtr(aSensorPtr)
         , mSystemPtr(aSystemPtr)
         , mScanUpdateTime(-1.0)
         , mScanFraction(0.0)
         , mScanRate(0.0)
         , mBeamList()
         , mBeamModeName()
         , mDetectBeamPtr(nullptr)
         , mDetectList()
         , mTrackList()
         , mTrackingRequestList()
         , mLastBeamCountSent(0)
      {
      }

      WsfSensor* mSensorPtr;
      DisSystem* mSystemPtr;
      //! The time when the scan position was last updated.
      double mScanUpdateTime;
      //! The percentage [0..1) of time the scan is through its pattern from its origin
      //! as of the last update time.
      double mScanFraction;
      //! The fraction of the scan pattern executed per unit time (1.0 / frame_rate).
      double mScanRate;

      std::vector<DisBeam*> mBeamList;
      //! If the associated beam is the first (and probably only) beam of a tracker, then this holds
      //! the mode name. In any other case, this will be 0. This is used to aid in filing the beam
      //! with track requests if 'use_track_jam_for_track_request true' is specified.
      std::vector<WsfStringId> mBeamModeName;

      DisBeam* mDetectBeamPtr;

      //! The full list of things being detected but not tracked.
      std::list<DisEntityId> mDetectList;

      //! The full list of things being tracked.
      std::list<DisEntityId> mTrackList;

      //! The list of currently active tracking requests.
      std::list<TrackingRequest> mTrackingRequestList;

      //! The number of beams sent in the last DIS system record.
      DisUint8 mLastBeamCountSent;
   };

   class SendInitialCMEDataEvent : public WsfEvent
   {
   public:
      SendInitialCMEDataEvent(double aSimTime, WsfDisInterface* aInterfacePtr, const DisEntityId& aEntityId);

      EventDisposition Execute() override;

   private:
      WsfDisInterface* mInterfacePtr;
      DisEntityId      mEntityId;
   };

   struct SimpleAccelerationData
   {
      SimpleAccelerationData()
         : mLastUpdate(0)
      {
      }
      double  mLastUpdate;
      UtVec3d mPastVelocity;
      UtVec3d mPastOrientationWCS;
   };

   SensorData* GetSensorData(WsfSensor* aSensorPtr);

   void MarkSensorAsOff(SensorData& aSensorData);

   void RebuildTrackJamLists(SensorData& aSensorData);

   unsigned char SetPartIdToBeamIdP(unsigned int aUniqueId);

   void SendInitialCMEData(double aSimTime);

   void UpdateBeamConfiguration(double aSimTime, SensorData& aSensorData);

   void UpdateBeamsWithSweepPosition(double aSimTime, SensorData& aSensorData);

   void UpdateBeamsWithTrackingRequests(SensorData& aSensorData);

   void UpdateSensorUpdateList(double aSimTime);

   void UpdateSystemFromSensor(SensorData& aSensorData);

   bool UpdateTrackingRequestList(double aSimTime, SensorData& aSensorData, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void UpdateSimpleAccelerations(double                              aSimTime,
                                  WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData,
                                  bool                                aDeadReckonChange);

   // The heartbeat timer differs depending on how many times an entity state has
   // been sent:
   //
   // a) It is zero when an entity state has yet to be sent.  This forces the
   //    entity state to be sent on the first call to MoverUpdated.
   // b) After MoverUpdate has been called once, it is set to a random value that
   //    is some fraction of the DIS heartbeat timer.  This spreads entity state
   //    updates out for non-moving or very slow moving entities.
   // c) On the second and subsequent updates it will be the DIS heartbeat timer
   //    +/- 10%.  Again, this prevents clustering of updates.

   double mHeartbeatTimer;

   double mHeartbeatTimerOverride;
   //! Pointer to the DIS emission PDU
   DisEmission* mEmissionPtr;

   //! Pointer to the DIS entity state PDU
   std::unique_ptr<DisEntityState> mEntityStatePtr;

   //! Update time of entity state PDU
   double mEntityStateUpdateTime;

   //! Pointer to the DIS entity state from the playback device.
   std::unique_ptr<DisEntityState> mPlaybackEntityStatePtr;

   //! Update time of the playback entity state PDU.
   double mPlaybackEntityStateUpdateTime;

   //! Pointer to the DIS interface
   WsfDisInterface* mInterfacePtr;

   //! Simulation time when emission PDU was sent for this platform
   double mLastTimeEmissionSent;

   //! Simulation time when entity state PDU was sent for this platform
   //! @note This is no longer true. It is the time when the last entity state PDU
   //! update was performed. Due to certain conditions (deferred connection or initial
   //! distribution interval), it may not actually be sent.
   double mLastTimeEntityStateSent;

   //! Simulation time of last mover update for this platform
   double mLastTimeMoverUpdated;

   //! Time to send on initial distribution interval
   double mInitialDistributionTime;

   //! Parameters for forced simple acceleration dis data
   SimpleAccelerationData* mSimpleAccelerationDataPtr;

   //! Pointer to the platform with which the DIS platform is associated.
   WsfPlatform* mPlatformPtr;

   //! True if this platform was constructed in response to an inbound object transfer.
   bool mIsIncomingTransfer;

   //! True if this platform represents an outbound object transfer.
   bool mIsOutgoingTransfer;

   //! True if this platform was replaced in response to an inbound object transfer.
   bool mHasBeenReplaced;

   //! True when an emission PDU needs to be sent for this DIS platform
   bool mSendEmissionPDU;

   //! True when the initial CME 'other' PDU has been scheduled to be sent.
   bool mScheduledInitialCMEData;

   //! Pointer to a shadow platform
   WsfPlatform* mShadowPlatformPtr;

   std::vector<WsfDisArticulatedPart*> mArticulatedPartList;

   using IdMap         = std::map<unsigned int, unsigned char>;
   using IdMapIterator = std::map<unsigned int, unsigned char>::iterator;
   using IdMapPair     = std::pair<unsigned int, unsigned char>;

   IdMap      mPartIdToBeamIdMap;
   SB_MapType mSysBeamToPartIdMap;

   std::vector<SensorData> mSensors;

   //! True when pdus are to be sent, allows for pdus to be sent out intermittently.
   bool mSendDis;
   //! True when transmitter pdus are to be sent, allows for transmitter pdus to be shut off.
   bool mSendTransmitterPdus;
   //! True if at least one entity state PDU has REALLY been sent.
   bool mEntityStateSent;
   bool mFirstUpdate;
   bool mSendEmissionPdu;
   bool mDoExtrapolate;

   std::unique_ptr<WsfExchange::Query> mQueryPtr{nullptr};

   ComponentList mComponents;
};

#endif
