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

#include "dis/WsfDisPlatform.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <memory>

#include "Dis.hpp"
#include "DisBeamEnums.hpp"
#include "DisComment.hpp"
#include "DisEmission.hpp"
#include "DisIO.hpp"
#include "DisOther.hpp"
#include "DisReceiver.hpp"
#include "DisSignal.hpp"
#include "DisTransmitter.hpp"
#include "GenMemO.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommResult.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEarthGravityModel.hpp"
#include "WsfEvent.hpp"
#include "WsfMover.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "dis/WsfDisArticulatedPart.hpp"
#include "dis/WsfDisDevice.hpp"
#include "dis/WsfDisEmission.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlaybackDevice.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"

WsfDisPlatform::InitialSendDisStatusCallback WsfDisPlatform::InitialSendDisStatus;

// ============================================================================
//! Constructor.
//! @param aSimTime            The current simulation time.
//! @param aPlatformPtr        The platform associated with the DIS representation.
//! @param aInterfacePtr       The WsfDisInterface to be used with this representation.
//! @param aEntityStatePtr     The associated entity state for externally controlled entities.
//! @param aEntityId           The DIS Entity Id to be assigned.
//! @param aEntityType         The DIS Entity Type to be assigned.
//! @param aIsIncomingTransfer 'true' if the platform was transferred to the application from another.
//! @param aIsOutgoingTransfer 'true' if the platform will be transferred to external control.
WsfDisPlatform::WsfDisPlatform(double                          aSimTime,
                               WsfPlatform*                    aPlatformPtr,
                               WsfDisInterface*                aInterfacePtr,
                               std::unique_ptr<DisEntityState> aEntityStatePtr,
                               const DisEntityId&              aEntityId,
                               const DisEntityType&            aEntityType,
                               bool                            aIsIncomingTransfer,
                               bool                            aIsOutgoingTransfer)
   : mEmissionPtr(nullptr)
   , mEntityStatePtr(std::move(aEntityStatePtr))
   , mEntityStateUpdateTime(aSimTime)
   , mPlaybackEntityStatePtr(nullptr)
   , mPlaybackEntityStateUpdateTime(0.0)
   , mInterfacePtr(aInterfacePtr)
   , mLastTimeEmissionSent(-1.0E20)
   , mLastTimeEntityStateSent(-1.0E20)
   , mLastTimeMoverUpdated(-1.0E20)
   , mInitialDistributionTime(0.0)
   , mPlatformPtr(aPlatformPtr)
   , mIsIncomingTransfer(aIsIncomingTransfer)
   , mIsOutgoingTransfer(aIsOutgoingTransfer)
   , mHasBeenReplaced(false)
   , mSendEmissionPDU(false)
   , mScheduledInitialCMEData(false)
   , mShadowPlatformPtr(nullptr)
   , mSendDis(true)
   , mSendTransmitterPdus(true)
   , mEntityStateSent(false)
   , mFirstUpdate(true)
   , mSendEmissionPdu(true)
   , mDoExtrapolate(true)
{
   // Allow the mSendDis flag to be updated when the platform is instantiated,
   // else the platform sends DIS PDUs out on initialization.
   InitialSendDisStatus(this, mSendDis);
   if (mInterfacePtr->UseSimpleAccelerations())
   {
      mSimpleAccelerationDataPtr = new SimpleAccelerationData;
   }
   else
   {
      mSimpleAccelerationDataPtr = nullptr;
   }

   // A pointer to an entity state will be passed under the following conditions:
   // 1) It is completely externally controlled.
   // 2) It is externally moved, but systems are still controlled locally
   // 3) It is a playback entity (read from a replay file).

   bool localEntity = (mEntityStatePtr == nullptr);
   bool playbackEntity =
      ((mEntityStatePtr != nullptr) && (WsfDisPlaybackDevice::IsPlaybackEntity(mEntityStatePtr->GetEntityId())));
   if (localEntity || playbackEntity)
   {
      assert(!aPlatformPtr->IsExternallyControlled());

      // Compute a time to send the initial entity state PDU over a distribution interval
      // if the following conditions were met:
      // - initial_distribution_time > 0,
      // - this is an event driven simulation
      // - and the simulation is connected to a network.
      mInitialDistributionTime = aInterfacePtr->GetRandom().Uniform(0.0, aInterfacePtr->GetInitialDistributionInterval());
      mInitialDistributionTime += aInterfacePtr->GetExtInterface()->GetDeferredConnectionTime();

      if (mEntityStatePtr != nullptr)
      {
         // This is a playback entity. Use its' entity state as provided.
         mPlaybackEntityStatePtr = std::move(mEntityStatePtr);
         mEntityStatePtr         = ut::clone(mPlaybackEntityStatePtr);
      }
      else
      {
         // This is native local entity. Create and initialize the entity state PDU.
         mEntityStatePtr = ut::make_unique<DisEntityState>();

         // Initialize the entity ID.
         mEntityStatePtr->SetEntityId(aEntityId);

         // Initialize the entity type. If the supplied value is defined then it will be used.
         DisEntityType entityType(aEntityType);
         if (entityType == DisEntityType())
         {
            WsfExtEntityType extEntityType;
            WsfDisUtil::GetExtInterface(mInterfacePtr)->SelectEntityType(aPlatformPtr, extEntityType);
            entityType = WsfDisExt::ToDis(extEntityType);
         }
         mEntityStatePtr->SetEntityType(entityType);

         mEntityStatePtr->SetDeadreckoningAlgorithm(4); // DRM_RVW
      }
   }
   else // mEntityStatePtr != 0  (external platform)
   {
      assert((aPlatformPtr->IsExternallyControlled() && !(aPlatformPtr->IsExternallyMoved())) ||
             (!aPlatformPtr->IsExternallyControlled() && (aPlatformPtr->IsExternallyMoved())));
   }

   mArticulatedPartList.clear();
}

// ============================================================================
WsfDisPlatform::~WsfDisPlatform()
{
   unsigned int i;
   for (i = 0; i < mArticulatedPartList.size(); ++i)
   {
      delete mArticulatedPartList[i];
   }

   if (mEmissionPtr != nullptr)
   {
      mEmissionPtr->RemoveAllSystemsWithNoDelete();
      delete mEmissionPtr;
   }

   for (i = 0; i < mSensors.size(); ++i)
   {
      delete mSensors[i].mSystemPtr;
   }

   delete mSimpleAccelerationDataPtr;
}

// ============================================================================
void WsfDisPlatform::CreateEmission(double aSimTime, const DisEntityId& aDisEntityId)
{
   // Initialize the emissions PDU.
   mEmissionPtr = new DisEmission();
   mEmissionPtr->SetEmittingEntityId(aDisEntityId);
   mEmissionPtr->SetEventId(DisEventId(aDisEntityId.GetSite(), aDisEntityId.GetApplication(), 0));

   // Allocate a system for each sensor...
   DisUint8 emitterId = 1;
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*mPlatformPtr); !iter.AtEnd(); ++iter)
   {
      // A sensor may have been directed not to send out dis, turn off dis in this case
      mSendEmissionPdu = iter->SendDis();
      if (!mSendEmissionPdu)
      {
         auto out = ut::log::info() << "Emitter on platform will not send out DIS.";
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Emitter: " << emitterId;
      }
      else if (mInterfacePtr->SuppressCME_PassiveSensor() && (iter->GetEM_XmtrCount() == 0))
      {
         // If the sensor is passive (XmtrCount == 0) and suppress Passive Sensor is enabled, then do not send.
      }
      else if (!iter->IsPrivate())
      {
         DisSystem* systemPtr = new DisSystem();
         systemPtr->SetNumber(emitterId);
         mInterfacePtr->SetEmitterTypeAndFunction(*iter, systemPtr);
         mSensors.emplace_back(*iter, systemPtr);
         ++emitterId;
      }
   }

   for (auto component : mComponents)
   {
      component->AllocateEmissionSystems(emitterId);
   }
}

// ============================================================================
int WsfDisPlatform::AddArticulation(int                 aType,
                                    int                 aParentType,
                                    unsigned int        aPublishBitmap,
                                    WsfArticulatedPart* aPartPtr,
                                    int                 aStartCount)
{
   if ((aPartPtr == nullptr) || (aType == 0) || (aPublishBitmap == 0))
   {
      return aStartCount; // part id is invalid, or type is set to entity body, or nothing to publish
   }

   WsfDisArticulatedPart* disPartPtr = new WsfDisArticulatedPart(aType, aParentType, aPublishBitmap, aPartPtr);
   int output = disPartPtr->CreateArticulationRecords(mEntityStatePtr.get(), aStartCount); // update the entity state with
                                                                                           // necessary DisEntityPart

   mArticulatedPartList.push_back(disPartPtr);
   return output;
}

// ============================================================================
//! Force an update of the platform.
//! This is called to ensure
//! that the platform position is up-to-date and that an entity state PDU has been sent.
void WsfDisPlatform::ForceUpdate(double aSimTime)
{
   // If an initial entity state PDU was not sent before sending some other type PDU,
   // force the entity state to be sent first.
   if (aSimTime < mInitialDistributionTime)
   {
      mInitialDistributionTime = aSimTime;
   }

   // Update the platform.
   if (GetPlatform() != nullptr)
   {
      GetPlatform()->Update(aSimTime);
   }

   // If an entity state still hasn't been sent (our MoverUpdated method will not be called if
   // the platform does not have a mover), invoke the MoverUpdated method explicitly...
   if ((mLastTimeEntityStateSent < 0.0) && (!GetPlatform()->IsExternallyControlled()))
   {
      WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEntityDeadReckon()->ForceUpdate(aSimTime, GetPlatform());
   }
}

// ============================================================================
//! Get the current entity state PDU for externally controlled or playback entities.
//!
//! This basically gets the last values set by SetEntityState().
//! @param aEntityStateUpdateTime The simulation time of the last update.
//! @param aEntityStatePtr The entity state PDU associated with this entity.
void WsfDisPlatform::GetEntityState(double& aEntityStateUpdateTime, DisEntityState*& aEntityStatePtr)
{
   // The playback entity state will only exist for playback entities.
   // If that is null then it is assume the standard external entity state exists.
   aEntityStatePtr        = mPlaybackEntityStatePtr.get();
   aEntityStateUpdateTime = mPlaybackEntityStateUpdateTime;
   if (aEntityStatePtr == nullptr)
   {
      aEntityStatePtr        = mEntityStatePtr.get();
      aEntityStateUpdateTime = mEntityStateUpdateTime;
   }
}

// ============================================================================
//! Make the supplied PDU the current PDU for externally controlled or playback entities.
//! This updates the platform to include the effects of any changes resulting from
//! changes in the DIS appearance word or articulation records.
//!
//! @param aSimTime The current simulation time.
//! @param aEntityStatePtr The PDU to be associated with this entity.
void WsfDisPlatform::SetEntityState(double aSimTime, DisEntityState* aEntityStatePtr)
{
   mInterfacePtr->UpdatePlatformFromEntityState(aSimTime, this, aEntityStatePtr);

   if (WsfDisPlaybackDevice::IsPlaybackEntity(aEntityStatePtr->GetEntityId()))
   {
      mPlaybackEntityStatePtr.reset(aEntityStatePtr);
      mPlaybackEntityStateUpdateTime = aSimTime;
   }
   else
   {
      mEntityStatePtr.reset(aEntityStatePtr);
      mEntityStateUpdateTime = aSimTime;
   }

   // If the entity has been deactivated then delete it from the simulation. (IEEE 1278.1-1995, para 4.5.2.1.4)
   if (((aEntityStatePtr->GetAppearance() >> 23) & 1) != 0)
   {
      WsfDisUtil::GetSimulation(mInterfacePtr).DeletePlatform(aSimTime, mPlatformPtr);
   }
}

// ============================================================================
//! Send out the emissions PDUs that represent all the sensors on the platform.
void WsfDisPlatform::SendEmissionsPDU(double aSimTime)
{
   // 'Redsim' rejects Emissions PDU's that contains more that 8 systems. I'm not sure
   // why because I can't find anywhere in the DIS standard that says that is a limit.
   // But oh well...

   static unsigned int cMAX_SYSTEMS_PER_PDU = 8;

   if ((!mInterfacePtr->HasOutputDevice()) || (mInterfacePtr->GetSuppressDisOutput()) ||
       (mInterfacePtr->GetSuppressEmissionsData()) ||
       (aSimTime < WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime()) ||
       (aSimTime < mInitialDistributionTime) || (mPlatformPtr->IsExternallyControlled()) || (mEmissionPtr == nullptr))
   {
      mSendEmissionPDU = false;
      return;
   }

   // If the first entity state has not been sent, then do so now.
   // This should also trigger the emission PDU to be sent, but we'll check to be safe.

   if (mLastTimeEntityStateSent < 0.0)
   {
      ForceUpdate(aSimTime);
      if (mLastTimeEmissionSent >= 0.0)
      {
         return;
      }
   }

   unsigned int i;
   DisEventId   eventId(mEmissionPtr->GetEventId());

   // Pack as many systems into each emissions PDU as possible.

   mEmissionPtr->SetStateUpdateIndicator(DisEmission::StateUpdate);
   mEmissionPtr->RemoveAllSystemsWithNoDelete();

   // TODO:
   //
   // The calls to WsfDisEmission::UpdateSystemFrom<Sensor...> were added to force the PDU to be updated
   // with the current state of the transmitter on EVERY transmission. We were sending updates on mode changes
   // but were missing simple frequency changes because there is no observer for such things. At some point we
   // can maybe make this smarter, but for now we'll have to accept the inefficiency.
   //
   // This isn't perfect because the change isn't transmitted until the next heartbeat, but that's good
   // enough until an observer can get implemented.

   // Add systems for sensors.

   for (i = 0; i < mSensors.size(); ++i)
   {
      SensorData& sensorData = mSensors[i];
      DisSystem*  systemPtr  = sensorData.mSystemPtr;
      bool        addSystem  = true;
      if (systemPtr->GetNumberOfBeams() == 0)
      {
         // A beam count of zero indicates the system has been turned off. Include the system in the
         // emissions PDU only if the previous state was on (beam count != 0).
         addSystem = (sensorData.mLastBeamCountSent != 0);
      }
      sensorData.mLastBeamCountSent = systemPtr->GetNumberOfBeams();
      if (addSystem)
      {
         // See note above about WsfDisEmission::UpdateSystemFromSensor. WsfDisPlatform::UpdateSystemFromSensor
         // calls WsfDisEmission::UpdateSystemFromSensor and then updates the CME 'detect beam' if present.
         UpdateSystemFromSensor(sensorData);
         UpdateBeamsWithSweepPosition(aSimTime, sensorData);
         if ((mEmissionPtr->GetNumberOfSystems() >= cMAX_SYSTEMS_PER_PDU) ||
             (mEmissionPtr->GetLength() + systemPtr->GetLengthOctets() > 1472))
         {
            eventId.AssignNewNumber();
            mEmissionPtr->SetEventId(eventId);
            mInterfacePtr->ImmediatePutPdu(aSimTime, *mEmissionPtr);
            mEmissionPtr->RemoveAllSystemsWithNoDelete();
         }
         mEmissionPtr->AddSystem(systemPtr);
      }
   }

   for (auto component : mComponents)
   {
      component->SendingEmission(aSimTime, mEmissionPtr, eventId);
   }

   // Send the PDU if it contains systems that have not yet been sent.
   if (mEmissionPtr->GetNumberOfSystems() != 0)
   {
      eventId.AssignNewNumber();
      mEmissionPtr->SetEventId(eventId);
      mInterfacePtr->ImmediatePutPdu(aSimTime, *mEmissionPtr);
      mEmissionPtr->RemoveAllSystemsWithNoDelete();
   }
   mSendEmissionPDU      = false;
   mLastTimeEmissionSent = aSimTime;
}

// ============================================================================
//! Given a WSF sensor, locate its corresponding DIS system.
DisSystem* WsfDisPlatform::GetSensorSystem(WsfSensor* aSensorPtr)
{
   DisSystem*  systemPtr = nullptr;
   SensorData* dataPtr   = GetSensorData(aSensorPtr);
   if (dataPtr != nullptr)
   {
      systemPtr = dataPtr->mSystemPtr;
   }
   return systemPtr;
}

// ============================================================================
unsigned char WsfDisPlatform::GetPartIdToBeamId(unsigned int aUniqueId)
{
   // Set the default return value
   unsigned char beamId(0);

   // Determine if the entry is in the collection
   auto iter    = mPartIdToBeamIdMap.find(aUniqueId);
   auto iterEnd = mPartIdToBeamIdMap.end();

   if (iter == iterEnd) // NOT found
   {
      beamId = SetPartIdToBeamIdP(aUniqueId);
   }
   else // found
   {
      beamId = (*iter).second;
   }
   return beamId;
}

// ============================================================================
unsigned int WsfDisPlatform::GetSysBeamIdToPartId(WsfDisPlatform::SB_Key aSB_Key)
{
   // Set the default return value
   unsigned int uniqueId(0u);

   auto iter = mSysBeamToPartIdMap.find(aSB_Key);
   if (iter != mSysBeamToPartIdMap.end())
   {
      uniqueId = (*iter).second;
   }
   return uniqueId;
}

// ============================================================================
void WsfDisPlatform::SetSysBeamIdToPartId(WsfDisPlatform::SB_Key aSB_Key, unsigned int aUniqueId)
{
   // if the key-value pair is not already in the map then add
   auto iter = mSysBeamToPartIdMap.find(aSB_Key);
   if (iter == mSysBeamToPartIdMap.end())
   {
      mSysBeamToPartIdMap.insert(SB_MapTypePair(aSB_Key, aUniqueId));
   }
}

// ============================================================================
// deprecated
void WsfDisPlatform::SetQueryPtr(WsfExchange::Query* aQueryPtr)
{
   SetQueryPtr(std::unique_ptr<WsfExchange::Query>{aQueryPtr});
}

void WsfDisPlatform::SetQueryPtr(std::unique_ptr<WsfExchange::Query> aQueryPtr)
{
   mQueryPtr = std::move(aQueryPtr);
   // We should never need a query struct for a local player...
   assert(mPlatformPtr->IsExternallyControlled());
}

// ============================================================================
void WsfDisPlatform::Comment(double aSimTime, const std::string& aComment)
{
   if ((mInterfacePtr->GetSuppressDisOutput()) || (!mSendDis) || (aSimTime < mInitialDistributionTime))
   {
      return;
   }
   auto commentPtr = ut::make_unique<DisComment>();
   commentPtr->SetOriginatingEntity(GetEntityId());
   commentPtr->AddVariableRecord((unsigned char*)(aComment.c_str()), static_cast<unsigned int>(aComment.length() * 8));

   mInterfacePtr->PutPdu(aSimTime, std::move(commentPtr));
}

// ============================================================================
void WsfDisPlatform::CommFrequencyChanged(double aSimTime, wsf::comm::Comm* aCommPtr) {}

// ============================================================================
void WsfDisPlatform::CommTurnedOff(double aSimTime, wsf::comm::Comm* aCommPtr) {}

// ============================================================================
void WsfDisPlatform::CommTurnedOn(double aSimTime, wsf::comm::Comm* aCommPtr) {}

// ============================================================================
void WsfDisPlatform::MessageReceived(double             aSimTime,
                                     wsf::comm::Comm*   aXmtrPtr,
                                     wsf::comm::Comm*   aRcvrPtr,
                                     const WsfMessage&  aMessage,
                                     wsf::comm::Result& aResult)
{
   if ((aXmtrPtr == nullptr) || (aRcvrPtr == nullptr))
   {
      return;
   }
   if ((mInterfacePtr->GetSuppressDisOutput()) || (!mSendDis) || (aSimTime < mInitialDistributionTime))
   {
      return;
   }
   // Generate the receiver PDU.
   auto rcvrPduPtr = ut::make_unique<DisReceiver>(); // memory deleted by DIS interface or thread
   rcvrPduPtr->SetEntityId(GetEntityId());

   // Determine the index of the receiver.
   for (unsigned int i = 0; i < mPlatformPtr->GetComponentCount<wsf::comm::Comm>(); ++i)
   {
      if (mPlatformPtr->GetComponentEntry<wsf::comm::Comm>(i) == aRcvrPtr)
      {
         rcvrPduPtr->SetRadioId(static_cast<DisUint16>(i + 1));
         break;
      }
   }

   // Determine the index of the transmitter on the its platform

   WsfPlatform* xmtrPlatformPtr = aXmtrPtr->GetPlatform();
   DisEntityId  xmtrEntityId;
   mInterfacePtr->GetEntityId(xmtrPlatformPtr, xmtrEntityId);
   rcvrPduPtr->SetTransmitterEntityId(xmtrEntityId);

   // Get the entity ID of the transmitting platform.

   for (unsigned int j = 0; j < xmtrPlatformPtr->GetComponentCount<wsf::comm::Comm>(); ++j)
   {
      if (xmtrPlatformPtr->GetComponentEntry<wsf::comm::Comm>(j) == aXmtrPtr)
      {
         rcvrPduPtr->SetTransmitterRadioId(static_cast<DisUint16>(j + 1));
         break;
      }
   }

   if (aResult.mRcvdPower > 0.0)
   {
      rcvrPduPtr->SetReceivedPower(static_cast<DisFloat32>(UtMath::SafeLinearToDB(aResult.mRcvdPower) + 30.0));
   }

   // Send the PDU indicating that the receiver is on and receiving
   rcvrPduPtr->SetReceiverState(DisReceiver::RsOnAndReceiving);
   auto rcvrPduCopyPtr = ut::make_unique<DisReceiver>(*rcvrPduPtr); // memory deleted by DIS interface or thread

   mInterfacePtr->PutPdu(aSimTime, std::move(rcvrPduPtr));

   // Send the PDU indicating that the receiver is on and not receiving
   rcvrPduCopyPtr->SetReceiverState(DisReceiver::RsOnAndNotReceiving);

   mInterfacePtr->PutPdu(aSimTime, std::move(rcvrPduCopyPtr));
}

// ============================================================================
void WsfDisPlatform::MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   if (mInterfacePtr->GetSuppressDisOutput())
   {
      return;
   }
   if ((mInterfacePtr->GetSuppressDisOutput()) || (!mSendDis) || (aSimTime < mInitialDistributionTime))
   {
      return;
   }
   // MDif (!mSendTransmitterPdus)
   // MD{
   // MDreturn;
   // MD}
   //  Generate a transmitter PDU and a signal PDU
   auto xmtrPduPtr = ut::make_unique<DisTransmitter>(); // memory deleted by DIS interface or thread
   xmtrPduPtr->SetEntityId(GetEntityId());

   // Determine the index of the transmitter on the platform.
   for (unsigned int i = 0; i < mPlatformPtr->GetComponentCount<wsf::comm::Comm>(); ++i)
   {
      if (mPlatformPtr->GetComponentEntry<wsf::comm::Comm>(i) == aXmtrPtr)
      {
         xmtrPduPtr->SetRadioId(static_cast<DisUint16>(i + 1));
         break;
      }
   }

   // set radio entity type if defined
   WsfExtRadioEntityType aTempType;
   if (WsfDisUtil::GetExtInterface(mInterfacePtr)->GetRadioEntityType(aXmtrPtr, aTempType))
   {
      DisRadioEntityType aRadType;
      aRadType.SetEntityKind(aTempType.GetEntityKind());
      aRadType.SetDomain(aTempType.GetDomain());
      aRadType.SetCountry(aTempType.GetCountry());
      aRadType.SetCategory(aTempType.GetCategory());
      aRadType.SetNomenclatureVersion(aTempType.GetNomenclatureVersion());
      aRadType.SetNomenclature(aTempType.GetNomenclature());
      xmtrPduPtr->SetRadioEntityType(aRadType);
   }

   double                  locationWCS[3];
   wsf::comm::ComponentHW* xmtrHW_Ptr = wsf::comm::ComponentHW::Find(*aXmtrPtr);
   if ((xmtrHW_Ptr != nullptr) && (xmtrHW_Ptr->GetEM_XmtrCount() > 0))
   {
      WsfEM_Xmtr& xmtr = xmtrHW_Ptr->GetEM_Xmtr(0);
      xmtrPduPtr->SetFrequency(static_cast<DisUint64>(xmtr.GetFrequency()));
      xmtrPduPtr->SetFrequencyBandwidth(static_cast<DisFloat32>(xmtr.GetBandwidth()));
      xmtrPduPtr->SetPower(static_cast<DisFloat32>(UtMath::SafeLinearToDB(xmtr.GetPower()) + 30.0));
      xmtr.GetAntenna()->GetLocationWCS(locationWCS);
      double locationECS[3];
      mPlatformPtr->ConvertWCSToECS(locationWCS, locationECS);
      xmtrPduPtr->SetRelativeLocation(static_cast<DisFloat32>(locationECS[0]),
                                      static_cast<DisFloat32>(locationECS[1]),
                                      static_cast<DisFloat32>(locationECS[2]));
   }
   else
   {
      mPlatformPtr->GetLocationWCS(locationWCS);
   }
   xmtrPduPtr->SetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);
   auto xmtrPduCopyPtr = ut::make_unique<DisTransmitter>(*xmtrPduPtr); // memory deleted by DIS interface or thread

   // Save some data before sending the Transmitter PDU since memory is deleted
   auto signalPduPtr = ut::make_unique<DisSignal>(); // memory deleted by DIS interface or thread
   signalPduPtr->SetEntityId(xmtrPduPtr->GetEntityId());
   signalPduPtr->SetRadioId(xmtrPduPtr->GetRadioId());

   // Send the Transmitter PDU indicating that the transmission is active.
   xmtrPduPtr->SetTransmitState(DisTransmitter::TsOnAndTransmitting);

   mInterfacePtr->PutPdu(aSimTime, std::move(xmtrPduPtr));

   // Send the Signal PDU with an empty message.

   mInterfacePtr->PutPdu(aSimTime, std::move(signalPduPtr));

   xmtrPduCopyPtr->SetTransmitState(DisTransmitter::TsOnAndNotTransmitting);
   mInterfacePtr->PutPdu(aSimTime, std::move(xmtrPduCopyPtr));
}

// ============================================================================
void WsfDisPlatform::MoverUpdated(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData)
{
   if (mPlatformPtr->IsExternallyControlled() || mPlatformPtr->IsExternallyMoved())
   {
      return;
   }

   mLastTimeMoverUpdated = aSimTime;

   const bool deadReckonChanged = aDeadReckonData.mLastUpdateType != WsfExtEntityDeadReckon::cNO_CHANGE;
   bool       sendEntityState   = deadReckonChanged;
   // bool sendEmissionsPdu = aDeadReckonData.mLastUpdateType == WsfExtEntityDeadReckon::cHEARTBEAT;
   // double deltaTime = aSimTime - mLastTimeEntityStateSent;

   // There are times when the entity is supposed to pick up exactly where it is in space and not
   // not be dead reckoned from the last sent value, that is why the check is here.
   if (!sendEntityState && mDoExtrapolate) // && (!mPlatformPtr->IsExternallyMoved()))
   {
      std::vector<WsfDisArticulatedPart*>::iterator it;
      for (it = mArticulatedPartList.begin(); it != mArticulatedPartList.end(); ++it)
      {
         if ((*it)->UpdateArticulationRecords(mEntityStatePtr.get(),
                                              mInterfacePtr->GetEntityPositionThreshold(),
                                              mInterfacePtr->GetEntityOrientationThreshold()))
         {
            sendEntityState = true;
         }
      }
   }

   // Check for appearance changes
   if (mInterfacePtr->UpdateAppearanceFromPlatform(aSimTime, mPlatformPtr, mEntityStatePtr.get()))
   {
      sendEntityState = true;
   }

   if (sendEntityState)
   {
      // Set the capabilities that this platform may offer to the rest of the world.
      //  (Payload, Fuel, Repair, Recovery, ADS-B, plus undefined bits.)
      mEntityStatePtr->SetCapabilities(mPlatformPtr->GetCapabilities());

      WsfExtEntityDeadReckon::DR_State& state = aDeadReckonData.mUpdatedState;
      // use state data if not in EGM-96 compliant mode
      double lLocationWCS[3];
      lLocationWCS[0] = state.mLocationWCS[0];
      lLocationWCS[1] = state.mLocationWCS[1];
      lLocationWCS[2] = state.mLocationWCS[2];
      // The WCS value needs to reflect its values are relative to the ellipse.
      if (wsf::EarthGravityModelExtension::Get(mInterfacePtr->GetScenario()).IsEGM96Compliant())
      {
         // WSF altitude is MSL (BUT is assumes it is referenced to the ellipse, which in non-standard).
         // Send out the WCS with altitude actually referenced to the ellipse.
         double lat, lon, altMSL;
         mPlatformPtr->GetLocationLLA(lat, lon, altMSL);
         // retrieve the geoid (MSL) to ellipse delta in meters
         // Returns negative value when geoid is below the ellipse.
         float geoidUndulationCorrection =
            wsf::EarthGravityModelExtension::Get(mInterfacePtr->GetScenario()).GetGeoidUndulation(lat, lon);
         double lAltRefToEllipseMeters = altMSL + (double)geoidUndulationCorrection;
         // recompute the WCS values being packed into DIS Entity State PDU
         double egm96_CorrectedWCS[3];
         UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, lAltRefToEllipseMeters, egm96_CorrectedWCS);
         for (int i = 0; i < 3; ++i)
         {
            lLocationWCS[i] = egm96_CorrectedWCS[i];
         }
      }
      mEntityStatePtr->SetLocation(lLocationWCS[0], lLocationWCS[1], lLocationWCS[2]);
      mEntityStatePtr->SetOrientation(static_cast<float>(state.mOrientationWCS[0]),
                                      static_cast<float>(state.mOrientationWCS[1]),
                                      static_cast<float>(state.mOrientationWCS[2]));
      mEntityStatePtr->SetVelocity(static_cast<float>(state.mVelocityWCS[0]),
                                   static_cast<float>(state.mVelocityWCS[1]),
                                   static_cast<float>(state.mVelocityWCS[2]));
      mEntityStatePtr->SetAcceleration(static_cast<float>(state.mAccelerationWCS[0]),
                                       static_cast<float>(state.mAccelerationWCS[1]),
                                       static_cast<float>(state.mAccelerationWCS[2]));
      mEntityStatePtr->SetAngularVelocity(static_cast<float>(state.mAngularRateWCS[0]),
                                          static_cast<float>(state.mAngularRateWCS[1]),
                                          static_cast<float>(state.mAngularRateWCS[2]));
      // mEntityStatePtr->SetAngularVelocity(0.0F, 0.0F, 0.0F);

      // If it becomes frozen, set the velocity and acceleration to zero.
      // (Is this strictly necessary? The frozen attribute should prevent extrapolation anyhow... JAJ)
      if (mEntityStatePtr->GetAppearance() & (1 << 21)) // Frozen
      {
         mEntityStatePtr->SetVelocity(0.0f, 0.0f, 0.0f);
         mEntityStatePtr->SetAcceleration(0.0f, 0.0f, 0.0f);
      }

      // Update the concealed status of the platform in the appearance bits
      // todo revisit: what is the proper threshold for dis concealment?
      int  appearance          = mEntityStatePtr->GetAppearance();
      bool previouslyConcealed = (((appearance >> 30) & 1) == 1);
      bool concealed           = (GetPlatform()->GetConcealmentFactor() > 0.99);
      if (concealed != previouslyConcealed)
      {
         if (concealed)
         {
            appearance |= (1 << 19);
         }
         else
         {
            appearance &= ~(1 << 19);
         }
      }
      mEntityStatePtr->SetAppearance(appearance);

      if (mSimpleAccelerationDataPtr != nullptr)
      {
         UpdateSimpleAccelerations(aSimTime, aDeadReckonData, deadReckonChanged);
      }

      if (mInterfacePtr->HasOutputDevice() && (!(mInterfacePtr->GetSuppressDisOutput())) &&
          (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime()) &&
          (aSimTime >= mInitialDistributionTime) && (!GetPlatform()->IsExternallyMoved()))
      {
         mInterfacePtr->ImmediatePutPdu(aSimTime, *mEntityStatePtr);

         mEntityStateSent = true;

         // If this is the first time a PDU has been sent then send out the
         // initial CME data used by external viewers to display meaningful
         // names and types (rather than Entity IDs and Entity Types).
         if ((!mInterfacePtr->GetSuppressDisOutput()) && (!mInterfacePtr->mSuppressCME_EntityData) &&
             (!mScheduledInitialCMEData))
         {
            mPlatformPtr->GetSimulation()->AddEvent(
               ut::make_unique<SendInitialCMEDataEvent>(aSimTime, mInterfacePtr, mEntityStatePtr->GetEntityId()));
            mScheduledInitialCMEData = true;
         }
      }

      mLastTimeEntityStateSent = aSimTime;
   }

   // mSendDis checks to see if the individual object was made private on the fly, if so then turn off
   // the sendEntityState flag
   if (((aSimTime - mLastTimeEmissionSent) > (0.999 * aDeadReckonData.mHeartbeatTimer)) &&
       (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime()))
   {
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
//! Updates accelerations and rotation rates based on past state and change in time
void WsfDisPlatform::UpdateSimpleAccelerations(double                              aSimTime,
                                               WsfExtEntityDeadReckon::PlatformDR& aDeadReckonData,
                                               bool                                aDeadReckonChange)
{
   bool                    frozen            = 0 != (mEntityStatePtr->GetAppearance() & (1 << 21));
   SimpleAccelerationData* simplePtr         = mSimpleAccelerationDataPtr;
   double                  aDT               = aSimTime - simplePtr->mLastUpdate;
   double                  angularRateWCS[3] = {0.0, 0.0, 0.0};
   if ((GetPlatform() != nullptr) && (aDT > 1.0E-3) && (simplePtr->mLastUpdate > 0.0) && !frozen)
   {
      // Save off past angular rate state; used in ext dead reckon routines to determine whether or not to
      // send a new entity state PDU.
      aDeadReckonData.mSentState.mPastAngularRateWCS[0] = aDeadReckonData.mSentState.mAngularRateWCS[0];
      aDeadReckonData.mSentState.mPastAngularRateWCS[1] = aDeadReckonData.mSentState.mAngularRateWCS[1];
      aDeadReckonData.mSentState.mPastAngularRateWCS[2] = aDeadReckonData.mSentState.mAngularRateWCS[2];

      double accel[3], oriRate[3];
      for (int i = 0; i < 3; ++i)
      {
         double orientAngleRad     = aDeadReckonData.mUpdatedState.mOrientationWCS[i];
         double pastOrientAngleRad = simplePtr->mPastOrientationWCS[i];
         orientAngleRad            = UtMath::NormalizeAngleMinusPi_Pi(orientAngleRad);
         pastOrientAngleRad        = UtMath::NormalizeAngleMinusPi_Pi(pastOrientAngleRad);
         double deltaAngleRad      = UtMath::NormalizeAngleMinusPi_Pi(orientAngleRad - pastOrientAngleRad);
         oriRate[i]                = deltaAngleRad / aDT;

         accel[i] = (aDeadReckonData.mUpdatedState.mVelocityWCS[i] - simplePtr->mPastVelocity[i]) / aDT;
      }
      // transfer the angular orientation rates into a common buffer for outputting
      for (int j = 0; j < 3; ++j)
      {
         // Save off new angular rate for simple orientation rate calculation condition. May be overridden
         // depending on which flags were set.
         angularRateWCS[j] = oriRate[j];
      }
      if (!mInterfacePtr->UseSimpleOrientationRate())
      {
         // change world (Euler) angular velocities to body axis angular velocities (as per standard)
         // w1 = (delta phi/dt) - ((delta yaw/dt) * sin(theta))
         // w2 = (delta theta/dt)*cos(phi) + ((delta psi/dt)*sin(phi)*cos(theta))
         // w3 = -((delta theta/dt)*sin(phi) + ((delta psi/dt)*cos(phi)*cos(theta))

         // double psiWCS;
         double thetaWCS;
         double phiWCS;
         // GetPlatform()->GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
         // psiWCS   = aDeadReckonData.mUpdatedState.mOrientationWCS[0];
         thetaWCS          = aDeadReckonData.mUpdatedState.mOrientationWCS[1];
         phiWCS            = aDeadReckonData.mUpdatedState.mOrientationWCS[2];
         angularRateWCS[0] = oriRate[2] - (oriRate[0] * sin(thetaWCS));
         angularRateWCS[1] = oriRate[1] * cos(phiWCS) + oriRate[0] * sin(phiWCS) * cos(thetaWCS);
         angularRateWCS[2] = -1.0 * (oriRate[1] * sin(phiWCS)) + oriRate[0] * cos(phiWCS) * cos(thetaWCS);
      }
      else if (mInterfacePtr->ZeroWorldToBodyAngularVelocities())
      {
         // use_body_angular_rate nor use_simple_orientation_rate were specified
         // in the dis_interface block, zero out the angular rates
         angularRateWCS[0] = 0.0;
         angularRateWCS[1] = 0.0;
         angularRateWCS[2] = 0.0;
      }
      // else use the simple orientation rates computed initially
      mEntityStatePtr->SetAcceleration((float)accel[0], (float)accel[1], (float)accel[2]);
      mEntityStatePtr->SetAngularVelocity((float)angularRateWCS[0], (float)angularRateWCS[1], (float)angularRateWCS[2]);
   }
   else
   {
      mEntityStatePtr->SetAcceleration(0.0F, 0.0F, 0.0F);
   }
   // Set the angular rate (WCS) on the extrapolation structure.
   // This is the structure used to extrapolate the "ghost" (or extrapolated) platform.
   // The angular rates here must be applied to the SentState, which is extrapolating the "ghost" platform.
   // When compared to the platform's UpdatedState, then there will be a delta after the platform levels
   // off and one more entity state PDU will be sent at that time to keep external applications from
   // extrapolating the orientation vector until the next update, which could be up to 5 seconds.
   for (size_t a = 0; a < 3; ++a)
   {
      aDeadReckonData.mSentState.mAngularRateWCS[a] = angularRateWCS[a];
   }
   // Update previous state
   simplePtr->mPastVelocity.Set(aDeadReckonData.mUpdatedState.mVelocityWCS);
   simplePtr->mPastOrientationWCS.Set(aDeadReckonData.mUpdatedState.mOrientationWCS);
   simplePtr->mLastUpdate = aSimTime;
}

// ============================================================================
void WsfDisPlatform::PlatformDeleted(double aSimTime)
{
   if (mPlatformPtr->IsExternallyControlled())
   {
      return;
   }

   // TODO-JAJ HACK - mLastTimeEntityStateSent used to mean when it was actually sent, but that
   // TODO is no longer the case as deferred_connection_time and initial_distribution_interval
   // TODO can cause the PDU to be updated, but not actually sent.
   // TODO
   // TODO We don't want to send the final PDU if we've never sent the initial PDU!

   // OLD if (mLastTimeEntityStateSent >= 0.0)
   if (mEntityStateSent)
   {
      if (mLastTimeEmissionSent >= 0.0)
      {
         unsigned int i;
         for (i = 0; i < mSensors.size(); ++i)
         {
            MarkSensorAsOff(mSensors[i]);
         }
         for (auto component : mComponents)
         {
            component->TurnOffEmission(mEmissionPtr);
         }

         if (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime())
         {
            // Note - this must be sent explicitly (without threading) because the platform is going away.
            SendEmissionsPDU(aSimTime);
         }
      }

      // Set final position, velocity, capture appearance, etc.
      mInterfacePtr->PrepareFinalEntityState(aSimTime, mPlatformPtr, mEntityStatePtr.get());

      if (mInterfacePtr->HasOutputDevice() && (!mInterfacePtr->GetSuppressDisOutput()) &&
          (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime()) &&
          (aSimTime >= mInitialDistributionTime) && (!(GetPlatform()->IsExternallyMoved())))
      {
         mInterfacePtr->ImmediatePutPdu(aSimTime, *mEntityStatePtr);
      }
   }
}

// ============================================================================
//! Perform functions that cannot be performed until the platform has been initialized.
//!
//! This is called via the simulation observer interface AFTER the platform has been initialized.
//! Some functions that were formerly taken in the constructor have been moved to here because
//! the initialization process may alter data that is needed by those functions. (e.g.:
//! WSF_COMPOSITE_SENSOR may mark a constituent as 'private', which indicates it is not to be
//! sent out).
void WsfDisPlatform::PlatformInitialized(double aSimTime)
{
   if (mEntityStatePtr == nullptr) // Should not occur
   {
      return;
   }

   WsfPlatform* platformPtr = GetPlatform();

   // A 'local entity' is one whose creating was not the result of receiving an entity state.

   bool playbackEntity = WsfDisPlaybackDevice::IsPlaybackEntity(mEntityStatePtr->GetEntityId());
   bool localEntity =
      ((!platformPtr->IsExternallyControlled()) && (!platformPtr->IsExternallyMoved()) && (!playbackEntity));

   // If this is a local entity then set the force ID (side) and marking characters.

   if (localEntity)
   {
      // Initialize the force ID.
      mEntityStatePtr->SetForceId(WsfDisUtil::GetExtInterface(mInterfacePtr)->GetForceId(platformPtr->GetSideId()));

      // Initialize the marking characters.
      std::string markingStr;
      if (!platformPtr->GetMarkingId().Empty())
      {
         markingStr = platformPtr->GetMarking();
      }
      else
      {
         WsfUtil::GetAuxValue(platformPtr, "marking", markingStr);
      }

      if (!markingStr.empty())
      {
         // Default the marking characters to 0
         DisUint8 markingChars[11] = {0};

         // Use the first 11 characters
         unsigned int count = std::min((unsigned int)(markingStr.length()), 11u);
         for (unsigned int i = 0; i < count; ++i)
         {
            unsigned int asciiValue = (unsigned int)(markingStr[i]);
            markingChars[i]         = DisUint8(asciiValue);
         }

         // Set the marking chars
         mEntityStatePtr->SetMarkingChars(markingChars);
         mEntityStatePtr->SetMarkingCharSet(DisEntityState::ASCII);
      }
   }

   // Attach articulated parts. They are not attached to entities that either are, or will become, external controlled.
   // (An outgoing transfer starts out as not-externally-controlled, but eventually becomes externally controlled)

   if ((!platformPtr->IsExternallyControlled()) && (!mIsOutgoingTransfer))
   {
      mInterfacePtr->GetArticulatedParts()->AddArticulatedParts(platformPtr, this);
   }

   // Create the emissions PDU.
   if (localEntity || platformPtr->IsExternallyMoved())
   {
      if (mEmissionPtr == nullptr)
      {
         CreateEmission(aSimTime, mEntityStatePtr->GetEntityId());
      }
   }
}

// ============================================================================
void WsfDisPlatform::SensorDetectionChanged(double           aSimTime,
                                            WsfSensor*       aSensorPtr,
                                            size_t           aTargetPlatformIndex,
                                            WsfSensorResult& aResult)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (dataPtr->mBeamList.empty()) // Precautionary check (should never be empty in this case)
   {
      return;
   }

   // Return immediately if the 'detected' beam is not being maintained.

   DisBeam* detectBeamPtr = dataPtr->mDetectBeamPtr;
   if (detectBeamPtr == nullptr)
   {
      return;
   }

   DisEntityId entityId;
   mInterfacePtr->GetEntityIdFromIndex(aTargetPlatformIndex, entityId);
   if (entityId.IsDefined())
   {
      // A successful detection (mFailedStatus == 0) is valid only if something was
      // actually checked (mCheckedStatus != 0). If nothing was checked then it is
      // assumed to be a non-detection.

      bool beamChanged = false;
      bool detected    = ((aResult.mCheckedStatus != 0) && (aResult.mFailedStatus == 0));
      if (detected)
      {
         // If the entity is already in another track/jam list, don't add it to the detect list.
         // (Resuming detection after coasting following a previous failed detection)

         bool isBeingTracked = false;
         if ((!mInterfacePtr->UseTrackJamForTrackingRequests()) || // not using beam to hold requests
             dataPtr->mBeamModeName[0].Empty())                    // not a tracker
         {
            const std::list<DisEntityId>& entityList = dataPtr->mTrackList;
            if (std::find(entityList.begin(), entityList.end(), entityId) != entityList.end())
            {
               isBeingTracked = true;
            }
         }

         if (!isBeingTracked)
         {
            const std::list<DisEntityId>& entityList = dataPtr->mDetectList;
            if (std::find(entityList.begin(), entityList.end(), entityId) == entityList.end())
            {
               dataPtr->mDetectList.push_back(entityId);
               beamChanged = true;
            }
         }
      }
      else // ! detected
      {
         // Remove the entry from the detect list if it is present...
         const std::list<DisEntityId>& entityList = dataPtr->mDetectList;
         if (std::find(entityList.begin(), entityList.end(), entityId) != entityList.end())
         {
            dataPtr->mDetectList.remove(entityId);
            beamChanged = true;
         }
      }
      if (beamChanged)
      {
         RebuildTrackJamLists(*dataPtr);
         UpdateSystemFromSensor(*dataPtr);
         SendOrQueueEmissionsPDU(aSimTime);
      }
   }
}

// ============================================================================
void WsfDisPlatform::SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (!aSensorPtr->IsTurnedOn())
   {
      return;
   }

   //// Update the list of beams in the system record.
   // UpdateBeamConfiguration(aSimTime, *dataPtr);

   //// Perform special update for using track/jam list as track request list.
   // UpdateBeamsWithTrackingRequests(*dataPtr);

   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfDisPlatform::SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (!aSensorPtr->IsTurnedOn())
   {
      return;
   }

   // Update the list of beams in the system record.
   UpdateBeamConfiguration(aSimTime, *dataPtr);

   // Perform special update for using track/jam list as track request list.
   UpdateBeamsWithTrackingRequests(*dataPtr);

   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfDisPlatform::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (!aSensorPtr->IsTurnedOn())
   {
      return;
   }

   // Update the list of beams in the system record.
   UpdateBeamConfiguration(aSimTime, *dataPtr);

   // Perform special update for using track/jam list as track request list.
   UpdateBeamsWithTrackingRequests(*dataPtr);

   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfDisPlatform::SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   bool updated = UpdateTrackingRequestList(aSimTime, *dataPtr, nullptr, aTrackPtr);
   if (updated && aSensorPtr->IsTurnedOn())
   {
      // Perform special update for using track/jam list as track request list.
      UpdateBeamsWithTrackingRequests(*dataPtr);

      UpdateSystemFromSensor(*dataPtr);
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
void WsfDisPlatform::SensorRequestInitiated(double          aSimTime,
                                            WsfSensor*      aSensorPtr,
                                            WsfSensorMode*  aModePtr,
                                            const WsfTrack* aTrackPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   bool updated = UpdateTrackingRequestList(aSimTime, *dataPtr, aModePtr, aTrackPtr);
   if (updated && aSensorPtr->IsTurnedOn())
   {
      // Perform special update for using track/jam list as track request list.
      UpdateBeamsWithTrackingRequests(*dataPtr);

      UpdateSystemFromSensor(*dataPtr);
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
void WsfDisPlatform::SensorRequestUpdated(double          aSimTime,
                                          WsfSensor*      aSensorPtr,
                                          WsfSensorMode*  aModePtr,
                                          const WsfTrack* aTrackPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   bool updated = UpdateTrackingRequestList(aSimTime, *dataPtr, aModePtr, aTrackPtr);
   if (updated && aSensorPtr->IsTurnedOn())
   {
      // Perform special update for using track/jam list as track request list.
      UpdateBeamsWithTrackingRequests(*dataPtr);

      UpdateSystemFromSensor(*dataPtr);
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
void WsfDisPlatform::SensorScanUpdated(double aSimTime, WsfSensor* aSensorPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfDisPlatform::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (dataPtr->mBeamList.empty()) // Precautionary check (should never be empty in this case)
   {
      return;
   }

   // Determine the entity ID of the target and remove it from the track/jam list.

   DisEntityId entityId;
   mInterfacePtr->GetEntityIdFromIndex(aTrackPtr->GetTargetIndex(), entityId);
   if (entityId.IsDefined())
   {
      if ((!mInterfacePtr->UseTrackJamForTrackingRequests()) || // not using beam to hold requests
          (dataPtr->mBeamModeName[0] == nullptr))               // not a tracker
      {
         dataPtr->mTrackList.remove(entityId);
         RebuildTrackJamLists(*dataPtr);
      }
      UpdateSystemFromSensor(*dataPtr);
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
void WsfDisPlatform::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   if (dataPtr->mBeamList.empty()) // Precautionary check (should never be empty in this case)
   {
      return;
   }

   // Determine the entity ID of the target and add it from the track/jam list.

   DisEntityId entityId;
   mInterfacePtr->GetEntityIdFromIndex(aTrackPtr->GetTargetIndex(), entityId);
   if (entityId.IsDefined())
   {
      // Remove from the 'detected' beam if it exists.  The 'detected' beam is an optional extra
      // pseudo-beam for identifying objects that have be detected by are not yet being tracked.

      bool rebuildTrackJamLists = false;
      if (dataPtr->mDetectBeamPtr != nullptr)
      {
         // Remove the entry from the detect list if it is present...
         const std::list<DisEntityId>& entityList = dataPtr->mDetectList;
         if (std::find(entityList.begin(), entityList.end(), entityId) != entityList.end())
         {
            dataPtr->mDetectList.remove(entityId);
            rebuildTrackJamLists = true;
         }
      }

      // Add to the first beam to indicate is is now being tracked.
      //
      // Note: If 'use_track_jam_for_tracking_requests' is true AND the sensor is a tracker,
      //       the first beam will not be updated here because (it is being used to contain
      //       the ID of those object for whom a track request exists).

      if ((!mInterfacePtr->UseTrackJamForTrackingRequests()) || // not using beam to hold requests
          (dataPtr->mBeamModeName[0] == nullptr))               // not a tracker
      {
         const std::list<DisEntityId>& entityList = dataPtr->mTrackList;
         if (std::find(entityList.begin(), entityList.end(), entityId) == entityList.end())
         {
            dataPtr->mTrackList.push_back(entityId);
            rebuildTrackJamLists = true;
         }
      }
      if (rebuildTrackJamLists)
      {
         RebuildTrackJamLists(*dataPtr);
      }
      UpdateSystemFromSensor(*dataPtr);
      SendOrQueueEmissionsPDU(aSimTime);
   }
}

// ============================================================================
void WsfDisPlatform::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   // Send out the system with all the beams removed to indicate it is turned off.

   MarkSensorAsOff(*dataPtr); // Clear everything out
   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);

   // Remove from sensor update list (if possible)
   UpdateSensorUpdateList(aSimTime);
}

// ============================================================================
void WsfDisPlatform::SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr)
{
   SensorData* dataPtr = GetSensorData(aSensorPtr);
   if (dataPtr == nullptr)
   {
      return;
   }

   // Update the list of beams in the system record.
   UpdateBeamConfiguration(aSimTime, *dataPtr);

   // Perform special update for using track/jam list as track request list.
   UpdateBeamsWithTrackingRequests(*dataPtr);

   UpdateSystemFromSensor(*dataPtr);
   SendOrQueueEmissionsPDU(aSimTime);

   // Add to sensor update list (if necessary).
   UpdateSensorUpdateList(aSimTime);

   // Force entity state update in case the 1st entity state pdu has not been sent
   if (mLastTimeEntityStateSent < 0.0)
   {
      ForceUpdate(aSimTime);
   }
}

// ============================================================================
bool WsfDisPlatform::PlatformBeingTracked(DisEntityId& anEntityId)
{
   if (anEntityId.IsDefined())
   {
      for (const auto& sensor : mSensors)
      {
         if ((!mInterfacePtr->UseTrackJamForTrackingRequests()) || // not using beam to hold requests
             (sensor.mBeamModeName[0] == nullptr))                 // not a tracker
         {
            const std::list<DisEntityId>& entityList = sensor.mTrackList;
            if (std::find(entityList.begin(), entityList.end(), anEntityId) != entityList.end())
            {
               return true;
            }
         }
      }
   }
   return false;
}

// ============================================================================
void WsfDisPlatform::TrySendCMEData(double simTime)
{
   if (!mInterfacePtr->SuppressCME_EntityData() && mSendDis)
   {
      WsfDisUtil::GetSimulation(mInterfacePtr)
         .AddEvent(ut::make_unique<SendInitialCMEDataEvent>(simTime, mInterfacePtr, GetEntityId()));
   }
}


// ============================================================================
// Private Methods
// ============================================================================

// ============================================================================
//! Given a pointer to a WSF sensor, return a pointer to our sensor data.
//! @note The return value may be zero.
WsfDisPlatform::SensorData* WsfDisPlatform::GetSensorData(WsfSensor* aSensorPtr)
{
   SensorData* dataPtr = nullptr;
   for (auto& sensor : mSensors)
   {
      if (sensor.mSensorPtr == aSensorPtr)
      {
         dataPtr = &sensor;
         break;
      }
   }
   return dataPtr;
}

// ============================================================================
//! Clear out internal data on the sensor to indicate it is off.
//! This is an internal method.
void WsfDisPlatform::MarkSensorAsOff(SensorData& aSensorData)
{
   aSensorData.mBeamList.clear();
   aSensorData.mBeamModeName.clear();
   aSensorData.mDetectList.clear();
   aSensorData.mTrackList.clear();
   aSensorData.mTrackingRequestList.clear();
   aSensorData.mDetectBeamPtr = nullptr;
   aSensorData.mSystemPtr->RemoveAllBeams();
}

// ============================================================================
//! Rebuild the track/jam lists for a sensor using the internally maintained lists.
//!
//! This routine will pack as many entries as will be allowed.
void WsfDisPlatform::RebuildTrackJamLists(SensorData& aSensorData)
{
   // Remove all detect entries before starting the rebuild process. This ensures
   // the maximum possible space is used for the track entries.

   DisBeam* detectBeamPtr = aSensorData.mDetectBeamPtr;
   if (detectBeamPtr != nullptr)
   {
      detectBeamPtr->RemoveAllTargets();
   }

   // First pack as many entries as possible into the track list as these are the highest priority.
   // Whatever is left will be used for detect entries.

   // Note: If 'use_track_jam_for_tracking_requests' is true AND the sensor is a tracker,
   //       the first beam will not be updated here because it is being used to contain
   //       the ID of those object for whom a track request exists.

   if ((!mInterfacePtr->UseTrackJamForTrackingRequests()) || // not using beam to hold requests
       (aSensorData.mBeamModeName[0] == nullptr))            // not a tracker
   {
      DisBeam* trackBeamPtr = aSensorData.mBeamList[0];
      trackBeamPtr->RemoveAllTargets();

      const std::list<DisEntityId>& entityList = aSensorData.mTrackList;
      for (const auto& entityId : entityList)
      {
         if (!trackBeamPtr->CanAddTrackJamEntry(aSensorData.mSystemPtr))
         {
            // A track/jam entry could not be added for some reason so ...
            // Check to see if the High Density Threshold has been met for this beam
            // as this could be one of the reasons why an entry could not be added.
            if (trackBeamPtr->GetNumberOfTargets() == Dis::GetEE_HIGH_DENSITY_THRSH())
            {
               trackBeamPtr->SetHighDensityTrackJam(DisEnum::beam::hdtj::Selected);
               trackBeamPtr->RemoveAllTargets();
            }
            break;
         }
         else // can add entry but check for large system; keeps VESPA happy; this in non-standard for DIS 7
         {
            if (!mInterfacePtr->SuppressAllNonStandardPDUs() && (aSensorData.mSystemPtr->GetLengthOctets() > 1012))
            {
               break;
            }
         }
         trackBeamPtr->AddTarget(DisTrackJam(entityId));
      }
   }

   if (detectBeamPtr != nullptr)
   {
      const std::list<DisEntityId>& entityList = aSensorData.mDetectList;
      for (const auto& entityId : entityList)
      {
         if (!detectBeamPtr->CanAddTrackJamEntry(aSensorData.mSystemPtr))
         {
            // A track/jam entry could not be added for some reason so ...
            // Check to see if the High Density Threshold has been met for this beam
            // as this could be one of the reasons why an entry could not be added.
            if (detectBeamPtr->GetNumberOfTargets() == Dis::GetEE_HIGH_DENSITY_THRSH())
            {
               detectBeamPtr->SetHighDensityTrackJam(DisEnum::beam::hdtj::Selected);
               detectBeamPtr->RemoveAllTargets();
            }
            break;
         }
         else // can add entry but check for large system; keeps VESPA happy; this in non-standard for DIS 7
         {
            if (!mInterfacePtr->SuppressAllNonStandardPDUs() && (aSensorData.mSystemPtr->GetLengthOctets() > 1012))
            {
               break;
            }
         }
         detectBeamPtr->AddTarget(DisTrackJam(entityId));
      }
   }
}

// ============================================================================
//! An internal method to either generate and send emission PDU's immediately or request that the
//! I/O thread send them when possible.
void WsfDisPlatform::SendOrQueueEmissionsPDU(double aSimTime)
{
   if (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime())
   {
      if (!mSendEmissionPDU)
      {
         // See ExecuteDeferredTransmitEvent for the rationale...
         mSendEmissionPDU            = true;
         double deferredTransmitTime = aSimTime + 0.001;
         // Can't capture 'this' in case the scheduled event outlives it
         size_t index           = GetPlatform()->GetIndex();
         auto*  disInterfacePtr = mInterfacePtr;
         WsfDisUtil::GetSimulation(mInterfacePtr)
            .AddEvent(ut::make_unique<WsfOneShotEvent>(deferredTransmitTime,
                                                       [deferredTransmitTime, index, disInterfacePtr]() {
                                                          disInterfacePtr->ExecuteDeferredTransmitEvent(deferredTransmitTime,
                                                                                                        index);
                                                       }));
         // SendEmissionsPDU(aSimTime);
      }
   }
}

// ============================================================================
unsigned char WsfDisPlatform::SetPartIdToBeamIdP(unsigned int aUniqueId)
{
   // Set the default return value
   unsigned char beamId(0);

   // Get the size of the collection
   size_t mapSize = mPartIdToBeamIdMap.size();

   if ((mapSize + 1) <= UCHAR_MAX)
   {
      beamId = static_cast<unsigned char>(mapSize + 1);

      // Add a new entry
      mPartIdToBeamIdMap.insert(IdMapPair(aUniqueId, beamId)); // 1-based
   }
   else
   {
      auto out = ut::log::warning() << "Max beam count exceeded.";
      out.AddNote() << "Entity: " << GetEntityId();
   }

   return beamId;
}

// ============================================================================
// private
void WsfDisPlatform::SendInitialCMEData(double aSimTime)
{
   // Send initial identification data so CME tools can show meaningful names and types

   DisOther otherPdu(0, 0);

   GenMemO genO(GenBuf::BigEndian, 512);

   genO.Reset();
   genO << static_cast<DisUint32>(0x12345678); // mRtsHeader
   genO << static_cast<DisUint32>(1201);       // ET_AUX_BODY_DATA_RQST

   genO << static_cast<DisUint32>(1); // version
   genO << mEntityStatePtr->GetEntityId();
   genO << static_cast<DisUint16>(mPlatformPtr->GetName().size());
   genO << mPlatformPtr->GetName().c_str();

   // Need to derive team/force ID
   if (mPlatformPtr->GetSideId() != 0)
   {
      genO << static_cast<DisUint16>(mPlatformPtr->GetSide().size());
      genO << mPlatformPtr->GetSide().c_str();
   }
   else
   {
      genO << static_cast<DisUint16>(1);
      genO << "1";
   }

   // Use the icon as the 'type'. WsfPlatform defaults the icon to the platform type if an icon was not specified.
   genO << static_cast<DisUint16>(mPlatformPtr->GetIcon().size());
   genO << mPlatformPtr->GetIcon().c_str();

   // No parent for now (could be commander in the future????).
   genO << DisEntityId();

   // Make a copy of the assembled buffer and assign to the PDU
   unsigned long int bufDataLen = genO.GetPutOffset(GenBuf::FromBeg);
   if (bufDataLen != 0)
   {
      const unsigned char* bufDataPtr = genO.GetBuffer();
      // Pad to a multiple of 4 bytes in accordance with DIS standards
      unsigned long int dataLen = ((bufDataLen + 3) / 4) * 4;
      unsigned char*    dataPtr = new unsigned char[dataLen];
      memcpy(dataPtr, bufDataPtr, dataLen);
      if (bufDataLen < dataLen)
      {
         memset(dataPtr + bufDataLen, 0, dataLen - bufDataLen);
      }
      otherPdu.SetData(dataPtr, (DisUint16)dataLen);
   }
   if (mInterfacePtr->HasOutputDevice() && (!mInterfacePtr->GetSuppressDisOutput()) &&
       (aSimTime >= WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime()) &&
       (aSimTime >= mInitialDistributionTime) && (mSendDis))
   {
      mInterfacePtr->ImmediatePutPdu(aSimTime, otherPdu);

      mScheduledInitialCMEData = true;
   }
}

// ============================================================================
// public
void WsfDisPlatform::ResetEntityId(const DisEntityId& aDisEntityId)
{
   if (mEntityStatePtr != nullptr)
   {
      SetEntityId(aDisEntityId);
   }
   if (mEmissionPtr != nullptr)
   {
      mEmissionPtr->SetEmittingEntityId(aDisEntityId);
   }
   if (mPlaybackEntityStatePtr != nullptr)
   {
      mPlaybackEntityStatePtr->SetEntityId(aDisEntityId);
   }
}

// ============================================================================
// public
void WsfDisPlatform::SetEntityId(const DisEntityId& aDisEntityId)
{
   mEntityStatePtr->SetEntityId(aDisEntityId);
}


// ============================================================================
//! Given the current state of the sensor, update the configuration of the beams in the DIS System.
//!
//! This method should be called whenever there is change in the configuration of a sensor, e.g.:
//! -) The sensor is turned on.
//! -) A mode is activated or deactivated.
//! -) A sensor request is added, updated or canceled.
// private
void WsfDisPlatform::UpdateBeamConfiguration(double aSimTime, SensorData& aSensorData)
{
   WsfSensor* sensorPtr = aSensorData.mSensorPtr;
   DisSystem* systemPtr = aSensorData.mSystemPtr;
   systemPtr->RemoveAllBeamsWithNoDelete();

   size_t beamIndex;
   size_t currentBeamCount = aSensorData.mBeamList.size();
   size_t neededBeamCount  = std::max(sensorPtr->GetEM_XmtrCount(), static_cast<size_t>(1));

   if (neededBeamCount > currentBeamCount)
   {
      // Need more beams so add new beams to the list
      aSensorData.mBeamList.resize(neededBeamCount, nullptr);
      for (beamIndex = currentBeamCount; beamIndex < neededBeamCount; ++beamIndex)
      {
         aSensorData.mBeamList[beamIndex] = new DisBeam();
      }
   }
   else if (neededBeamCount < currentBeamCount)
   {
      // Currently have more beams than needed. Delete extraneous beams from the end.
      for (beamIndex = neededBeamCount; beamIndex < currentBeamCount; ++beamIndex)
      {
         delete aSensorData.mBeamList[beamIndex];
      }
      aSensorData.mBeamList.resize(neededBeamCount);
   }

   // Assume none of the beams are 'trackers', and assume that an active scanner hasn't been found.
   bool foundScanner = false;
   aSensorData.mBeamModeName.resize(neededBeamCount, nullptr);
   for (beamIndex = 0; beamIndex < neededBeamCount; ++beamIndex)
   {
      aSensorData.mBeamModeName[beamIndex] = nullptr;
   }

   // Now go through and set the beam ID, beam function and beam parameter index.
   if (sensorPtr->GetEM_XmtrCount() != 0)
   {
      bool               canMultiSelect = false;
      WsfSensorModeList* modeListPtr    = sensorPtr->GetModeList();
      std::vector<int>   modeStartCount;
      if ((modeListPtr != nullptr) && modeListPtr->CanMultiSelect())
      {
         // Toggle the multi-select flag
         canMultiSelect = true;

         std::vector<WsfSensorMode*> modes;
         modeListPtr->GetDerivedModeList(modes);
         modeStartCount.push_back(1); // Performance, first mode is beam 1, obviously
         size_t modeCnt = modeListPtr->GetModeCount();
         for (size_t mi = 1; mi < modeCnt; ++mi)
         {
            int reqCnt = modes.at(mi - 1)->GetMaximumRequestCount();
            modeStartCount.push_back(reqCnt + modeStartCount.at(mi - 1));
         }
      }

      for (beamIndex = 0; beamIndex < neededBeamCount; ++beamIndex)
      {
         WsfEM_Xmtr& xmtr    = sensorPtr->GetEM_Xmtr(beamIndex);
         DisBeam*    beamPtr = aSensorData.mBeamList[beamIndex];

         // Capture mode name and beam index (if provided) that will be used to set the beam type and function.

         WsfStringId    modeNameId    = sensorPtr->GetCurrentModeName();
         size_t         modeBeamIndex = beamIndex;
         WsfSensorMode* modePtr       = nullptr;
         if (xmtr.GetMode() != nullptr)
         {
            modeNameId    = xmtr.GetMode()->GetNameId();
            modeBeamIndex = xmtr.GetIndex();
            modePtr       = sensorPtr->GetModeEntry(xmtr.GetMode()->GetModeIndex());
         }

         // If the sensor implementation has been kind enough to provide enough data about the xmtr/rcvr
         // configuration, use it to determine the beam Id number.
         //
         // For a single-select system (where only one mode can be selected at a time), the beam ID number is just
         // the beam index + 1.
         //
         // For multi-select systems, where multiple modes can be active at once and can turn on and off,
         // the beam index is set according to the defined map for proper regeneration later.

         size_t beamId = beamIndex + 1;
         if (xmtr.GetMode() && canMultiSelect)
         {
            beamId = (modeStartCount.at(xmtr.GetMode()->GetModeIndex()) + beamIndex);
         }
         beamPtr->SetNumber(static_cast<DisUint8>(beamId));

         // Set the beam type and function from user inputs
         mInterfacePtr->SetBeamTypeAndFunction(sensorPtr, modeNameId, static_cast<int>(modeBeamIndex + 1), beamPtr, 5);

         // Determine if this beam is a scanner or a tracker and capture the necessary data.
         if (modePtr != nullptr)
         {
            if ((modePtr->GetMaximumRequestCount() == 0) && // This mode is potentially a scanner
                (modePtr->GetFrameTime() > 0.0))            // Protective coding...
            {
               // TODO - This is may not be exactly correct, but it is good enough for now...
               // TODO - It is assumed that at most one scanning mode is active at any time.

               WsfEM_Antenna* antennaPtr = xmtr.GetAntenna();
               if ((antennaPtr != nullptr) && ((antennaPtr->GetScanMode() == WsfEM_Antenna::cSCAN_AZ) ||
                                               (antennaPtr->GetScanMode() == WsfEM_Antenna::cSCAN_AZ_EL)))
               {
                  double azScan = antennaPtr->GetMaximumAzimuthScanLimit() - antennaPtr->GetMinimumAzimuthScanLimit();
                  if (fabs(azScan - UtMath::cTWO_PI) < 1.0E-6)
                  {
                     if (aSensorData.mScanUpdateTime < 0.0)
                     {
                        // Randomly choose a starting position.
                        aSensorData.mScanFraction   = mInterfacePtr->GetRandom().Uniform<double>();
                        aSensorData.mScanUpdateTime = aSimTime;
                     }
                     aSensorData.mScanUpdateTime = aSimTime;
                     aSensorData.mScanRate       = 1.0 / modePtr->GetFrameTime();

                     // Set the scan position...
                     double deltaTime    = aSimTime - aSensorData.mScanUpdateTime;
                     double scanPosition = aSensorData.mScanFraction + (deltaTime * aSensorData.mScanRate);
                     while (scanPosition >= 1.0)
                     {
                        scanPosition -= 1.0;
                     }
                     aSensorData.mScanFraction = scanPosition;
                     foundScanner              = true;
                  }
               }
            }
            else if ((modePtr->GetMaximumRequestCount() > 0) && // This mode is a tracker
                     (xmtr.GetIndex() == 0))                    // First or only transmitter in the mode

            {
               aSensorData.mBeamModeName[beamIndex] = modePtr->GetNameId();
            }
         }
      }
   }
   else
   {
      // For passive systems, only one beam is provided. This may be extended later.
      DisBeam* beamPtr = aSensorData.mBeamList.front();
      beamPtr->SetNumber(1);
      mInterfacePtr->SetBeamTypeAndFunction(sensorPtr, sensorPtr->GetCurrentModeName(), 1, beamPtr, 5);
   }

   // Create an extra beam for detected targets not yet in track (used by external viewers)
   if ((!mInterfacePtr->SuppressCME_DetectBeam()) && (aSensorData.mDetectBeamPtr == nullptr))
   {
      DisBeam* beamPtr           = new DisBeam();
      aSensorData.mDetectBeamPtr = beamPtr;
      beamPtr->SetNumber(0xFF);
      // The beam function is forced to 3 after the call because that is what is required.
      // This call is only to get the beam type (parameter index);
      mInterfacePtr->SetBeamTypeAndFunction(sensorPtr, sensorPtr->GetCurrentModeName(), 255, beamPtr, 3);
      beamPtr->SetFunction(3);

      // Set the power to some small value not equal to 1mW.  It must not be 1mw because 1mW converted to
      // dBmW is 0 and a well-accepted Boeing standard treats beams with 0 in the power field as being 'off'.

      double power_W    = 0.001001;
      double power_dBmW = 10.0 * log10(power_W / 0.001);
      beamPtr->SetEffectiveRadiatedPower(static_cast<DisFloat32>(power_dBmW));
   }

   // Now go through and add all of allocated beams to the system.
   for (auto& beam : aSensorData.mBeamList)
   {
      // Ensure the system doesn't exceed its maximum length of 255 * 4 = 1020 bytes.
      if ((systemPtr->GetLengthOctets() + beam->GetLengthOctets()) > 1020)
      {
         break;
      }
      systemPtr->AddBeam(beam);
   }
   if (aSensorData.mDetectBeamPtr != nullptr)
   {
      // Ensure the system doesn't exceed its maximum length of 255 * 4 = 1020 bytes.
      if ((systemPtr->GetLengthOctets() + aSensorData.mDetectBeamPtr->GetLengthOctets()) <= 1020)
      {
         systemPtr->AddBeam(aSensorData.mDetectBeamPtr);
      }
   }

   // If a scanner wasn't found then define it so the beam sweep sync will not be updated.
   if (!foundScanner)
   {
      aSensorData.mScanUpdateTime = -1.0;
      aSensorData.mScanFraction   = 0.0;
      aSensorData.mScanRate       = 0.0;
   }
}

// ============================================================================
// private
void WsfDisPlatform::UpdateBeamsWithSweepPosition(double aSimTime, SensorData& aSensorData)
{
   if (aSensorData.mScanUpdateTime >= 0.0)
   {
      // Increment the fraction of the beam scan pattern covered.
      double deltaT = std::max(0.0, aSimTime - aSensorData.mScanUpdateTime);
      aSensorData.mScanFraction += (deltaT * aSensorData.mScanRate);

      // Check for wrap-around.
      while (aSensorData.mScanFraction >= 1.0)
      {
         aSensorData.mScanFraction -= 1.0;
      }

      // Update every beam in the system with the new fractional position.
      float      sweepSync = static_cast<float>(aSensorData.mScanFraction);
      DisSystem* systemPtr = aSensorData.mSystemPtr;
      auto       beamIter(systemPtr->GetBeamBegin());
      while (beamIter != systemPtr->GetBeamEnd())
      {
         DisBeam* beamPtr = *beamIter;
         beamPtr->SetSweepSync(sweepSync);
         ++beamIter;
      }

      aSensorData.mScanUpdateTime = aSimTime;
   }
}

// ============================================================================
//! If 'use_track_jam_for_tracking_request' is true, this will update the tracking beams with the ID's
//! of the objects for which the track request pertains.
// private
void WsfDisPlatform::UpdateBeamsWithTrackingRequests(SensorData& aSensorData)
{
   // The beams are not updated with requests UNLESS 'use_track_jam_for_tracking_request' is true!
   if (!mInterfacePtr->UseTrackJamForTrackingRequests())
   {
      return;
   }

   for (unsigned int beamIndex = 0; beamIndex < aSensorData.mBeamList.size(); ++beamIndex)
   {
      if (aSensorData.mBeamModeName[beamIndex] == nullptr)
      {
         continue; // Not a tracking sensor.
      }
      WsfStringId beamModeName = aSensorData.mBeamModeName[beamIndex];
      DisBeam*    beamPtr      = aSensorData.mBeamList[beamIndex];
      beamPtr->RemoveAllTargets();

      for (const auto& request : aSensorData.mTrackingRequestList)
      {
         if (request.mModeNameId == beamModeName)
         {
            DisEntityId entityId;
            mInterfacePtr->GetEntityIdFromIndex(request.mTargetIndex, entityId);
            if (entityId.IsDefined())
            {
               beamPtr->AddTarget(DisTrackJam(entityId));
            }
         }
      }
   }
}

// ============================================================================
//! Update the DIS System from the sensor.
// private
void WsfDisPlatform::UpdateSystemFromSensor(SensorData& aSensorData)
{
   mInterfacePtr->UpdateSystemFromSensor(aSensorData.mSensorPtr, aSensorData.mSystemPtr);

   // If the detect-beam is present, copy the parameters from the first beam. The beam parameter index,
   // beam function and power are left at their original values as defined in UpdateBeamConfiguratioin.

   if ((aSensorData.mDetectBeamPtr != nullptr) && (!aSensorData.mBeamList.empty()))
   {
      DisBeam* detectBeamPtr = aSensorData.mDetectBeamPtr;
      DisBeam* firstBeamPtr  = aSensorData.mBeamList[0];
      detectBeamPtr->SetFrequency(firstBeamPtr->GetFrequency());
      detectBeamPtr->SetFrequencyRange(firstBeamPtr->GetFrequencyRange());
      detectBeamPtr->SetPulseRepetitionFrequency(firstBeamPtr->GetPulseRepetitionFrequency());
      detectBeamPtr->SetPulseWidth(firstBeamPtr->GetPulseWidth());
      detectBeamPtr->SetAzimuthCenter(firstBeamPtr->GetAzimuthCenter());
      detectBeamPtr->SetAzimuthSweep(firstBeamPtr->GetAzimuthSweep());
      detectBeamPtr->SetElevationCenter(firstBeamPtr->GetElevationCenter());
      detectBeamPtr->SetElevationSweep(firstBeamPtr->GetElevationSweep());
      detectBeamPtr->SetSweepSync(firstBeamPtr->GetSweepSync());
   }
}

// ============================================================================
//! If 'use_track_jam_for_tracking_request', update the list of active tracking requests.
// private
bool WsfDisPlatform::UpdateTrackingRequestList(double          aSimTime,
                                               SensorData&     aSensorData,
                                               WsfSensorMode*  aModePtr,
                                               const WsfTrack* aTrackPtr)
{
   bool updated = false;
   if (mInterfacePtr->UseTrackJamForTrackingRequests() && (aTrackPtr != nullptr) && (aTrackPtr->GetTargetIndex() != 0))
   {
      auto trli = aSensorData.mTrackingRequestList.begin();
      while (trli != aSensorData.mTrackingRequestList.end())
      {
         if (aTrackPtr->GetTrackId() == (*trli).mTrackId)
         {
            break;
         }
         ++trli;
      }

      if (trli == aSensorData.mTrackingRequestList.end())
      {
         if (aModePtr != nullptr)
         {
            // New request.
            TrackingRequest request;
            request.mTrackId     = aTrackPtr->GetTrackId();
            request.mModeNameId  = aModePtr->GetNameId();
            request.mTargetIndex = aTrackPtr->GetTargetIndex();
            aSensorData.mTrackingRequestList.push_back(request);
            updated = true;
         }
      }
      else
      {
         if (aModePtr != nullptr)
         {
            // Request is being updated.
            TrackingRequest& request = *trli;
            if ((request.mTargetIndex != aTrackPtr->GetTargetIndex()) || (request.mModeNameId != aModePtr->GetNameId()))
            {
               request.mTargetIndex = aTrackPtr->GetTargetIndex();
               request.mModeNameId  = aModePtr->GetNameId();
               updated              = true;
            }
         }
         else
         {
            // Request is being canceled.
            aSensorData.mTrackingRequestList.erase(trli);
            updated = true;
         }
      }
   }
   return updated;
}

// ============================================================================
//! This method determines if sensor updates need to be sent that are outside the normal DIS requirement.
//! Such updates are sent only if the 'sensor_update_interval' is sent. This forces the 'beam sweep sync'
//! field to be updated more often for those simulations that require it.
// private
void WsfDisPlatform::UpdateSensorUpdateList(double aSimTime)
{
   if (mInterfacePtr->GetSensorUpdateInterval() <= 0.0)
   {
      return;
   }

   // Determine if the platform has any sensors that are turned on.
   bool needUpdateEvent = false;
   for (SensorData& sensorData : mSensors)
   {
      if (sensorData.mSystemPtr->GetNumberOfBeams() > 0)
      {
         needUpdateEvent = true;
         break;
      }
   }

   // Add or remove the platform from the update list as required.
   size_t             platformIndex    = GetPlatform()->GetIndex();
   std::list<size_t>& sensorUpdateList = mInterfacePtr->mSensorUpdateList;
   auto               suli             = std::find(sensorUpdateList.begin(), sensorUpdateList.end(), platformIndex);
   if (needUpdateEvent)
   {
      if (suli == sensorUpdateList.end())
      {
         sensorUpdateList.push_back(platformIndex);
      }
   }
   else
   {
      if (suli != sensorUpdateList.end())
      {
         sensorUpdateList.erase(suli);
      }
   }

   // Kick off the sensor update processor if not already done.
   if ((!sensorUpdateList.empty()) && (!mInterfacePtr->mSensorUpdateScheduled))
   {
      mInterfacePtr->mSensorUpdateScheduled = true;
      auto* disInterfacePtr = mInterfacePtr; // Can't capture mInterfacePtr in case the scheduled event out-lives 'this'
      WsfDisUtil::GetSimulation(mInterfacePtr)
         .AddEvent(ut::make_unique<WsfRecurringEvent>(aSimTime,
                                                      [disInterfacePtr](WsfEvent& e)
                                                      {
                                                         e.SetTime(disInterfacePtr->ExecuteSensorUpdateEvent(e.GetTime()));
                                                         return WsfEvent::cRESCHEDULE;
                                                      }));
   }
}

// ============================================================================
WsfDisPlatform::SendInitialCMEDataEvent::SendInitialCMEDataEvent(double             aSimTime,
                                                                 WsfDisInterface*   aInterfacePtr,
                                                                 const DisEntityId& aEntityId)
   : WsfEvent(aSimTime)
   , mInterfacePtr(aInterfacePtr)
   , mEntityId(aEntityId)
{
}

// ============================================================================
WsfEvent::EventDisposition WsfDisPlatform::SendInitialCMEDataEvent::Execute()
{
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, mEntityId);
   if (disPlatformPtr != nullptr)
   {
      disPlatformPtr->SendInitialCMEData(GetTime());
   }
   return WsfEvent::cDELETE;
}
