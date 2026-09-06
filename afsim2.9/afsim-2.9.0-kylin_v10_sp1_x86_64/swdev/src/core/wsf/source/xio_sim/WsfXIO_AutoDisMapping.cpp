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

#include "xio_sim/WsfXIO_AutoDisMapping.hpp"

#include <cassert>

#include "DisIO.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "WsfAssociationMessage.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommandChain.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskCancelMessage.hpp"
#include "WsfTaskControlMessage.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisMover.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "ext/WsfExtInterface.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_CommComponent.hpp"
#include "xio_sim/WsfXIO_DisExtension.hpp"
#include "xio_sim/WsfXIO_DisService.hpp"
#include "xio_sim/WsfXIO_Extension.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

namespace WsfXIO_DisExtension_Detail
{

class DisRequest : public WsfXIO_DisRequest
{
public:
   DisRequest(WsfXIO_Connection* aConnectionPtr, WsfXIO_AutoDisMapping* aMappingPtr)
      : WsfXIO_DisRequest(aConnectionPtr)
      , mDisMappingPtr(aMappingPtr)
   {
   }
   WsfXIO_AutoDisMapping* mDisMappingPtr;
   void HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt) override { mDisMappingPtr->HandlePlatformInfo(aPkt); }
   ~DisRequest() override {}
};
} // namespace WsfXIO_DisExtension_Detail

namespace
{
//! Get pointer to the auto DIS mapping instance.
//! This is a bit of a hack, but it isn't used often.
WsfXIO_AutoDisMapping* GetAutoDisMapping(WsfDisInterface& aDisInterface)
{
   WsfXIO_DisExtension* disExtensionPtr = WsfXIO_Extension::FindDisExtension(aDisInterface.GetSimulation());
   assert(disExtensionPtr != nullptr);
   return disExtensionPtr->GetAutoDisMapping();
}

//! Called from WsfDisInterface when a new DIS platform is being created.
WsfPlatform* NewDisPlatformFwd(WsfDisInterface& aDisInterface, WsfStringId aPlatformType, WsfDisEntityState* aEntityStatePtr)
{
   return GetAutoDisMapping(aDisInterface)->NewDisPlatform(aPlatformType, aEntityStatePtr);
}
//! Called from WsfDisInterface.
bool DelayEntityStateFwd(WsfDisInterface& aDisInterface, const DisEntityId& aEntityId)
{
   return GetAutoDisMapping(aDisInterface)->DelayDisEntityState(aEntityId);
}
} // namespace

WsfXIO_AutoDisMapping::WsfXIO_AutoDisMapping(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_DisExtension* aDisExtensionPtr)

   : mSimulation(aXIO_Simulation.GetSimulation())
   , mScenario(aXIO_Simulation.GetSimulation().GetScenario())
   , mDisInterfacePtr(aXIO_Simulation.GetDisInterface())
   , mInterfacePtr(aXIO_Simulation.GetInterface())
   , mDisExtensionPtr(aDisExtensionPtr)
{
   mExtInterfacePtr      = WsfExtInterface::Find(mSimulation);
   mDisRequestManagerPtr = new WsfXIO_DisRequestManager(mInterfacePtr);
}

WsfXIO_AutoDisMapping::~WsfXIO_AutoDisMapping()
{
   for (size_t i = 0; i < mConnections.size(); ++i)
   {
      ConnectionData* dataPtr = mConnections[i];
      mDisRequestManagerPtr->RemoveRequest(dataPtr->mDisRequestPtr);
      delete dataPtr;
   }
   for (RemotePlatformMap::iterator i = mRemotePlatforms.begin(); i != mRemotePlatforms.end(); ++i)
   {
      delete i->second;
   }

   delete mDisRequestManagerPtr;
}

//! Initialize Auto DIS Mapping
//! @return 'true' if Auto DIS mapping is required
bool WsfXIO_AutoDisMapping::Initialize()
{
   mCallbacks += mInterfacePtr->OnConnected.Connect(&WsfXIO_AutoDisMapping::HandleConnect, this);
   mCallbacks += mInterfacePtr->OnDisconnected.Connect(&WsfXIO_AutoDisMapping::HandleDisconnect, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_AutoDisMapping::HandleDisMessage, this);
   mCallbacks += WsfObserver::PlatformInitialized(&mSimulation).Connect(&WsfXIO_AutoDisMapping::PlatformInitialized, this);
   mCallbacks += WsfObserver::PlatformDeleted(&mSimulation).Connect(&WsfXIO_AutoDisMapping::PlatformDeleted, this);

   mMessageTranslators[WsfTrackMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTrackMessage);
   mMessageTranslators[WsfTrackDropMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTrackDropMessage);
   mMessageTranslators[WsfTaskAssignMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTaskAssignMessage);
   mMessageTranslators[WsfTaskCancelMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTaskCancelMessage);
   mMessageTranslators[WsfTaskControlMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTaskControlMessage);
   mMessageTranslators[WsfTaskStatusMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateTaskStatusMessage);
   mMessageTranslators[WsfAssociationMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateAssociationMessage);
   mMessageTranslators[WsfStatusMessage::GetTypeId()] =
      TranslateMessageFunction(this, &WsfXIO_AutoDisMapping::TranslateStatusMessage);

   // Inform the DIS interface that we need to know about certain functions.
   mDisInterfacePtr->SetCreateDisPlatformFunction(NewDisPlatformFwd);
   mDisInterfacePtr->SetDelayEntityStateFunction(DelayEntityStateFwd);
   return true;
}

bool WsfXIO_AutoDisMapping::IsAutoMapped(const DisEntityId& aId)
{
   bool isAutomapped = mInterfacePtr->mAutoMappingDefault;
   for (size_t i = 0; i < mInterfacePtr->mAutoMappedApplications.size(); ++i)
   {
      WsfXIO_EntityId entity = mInterfacePtr->mAutoMappedApplications[i];
      if (entity.GetSite() == aId.GetSite() || entity.GetSite() == DisEntityId::ALL_SITES)
      {
         if (entity.GetApplication() == aId.GetApplication() || entity.GetApplication() == DisEntityId::ALL_APPLIC)
         {
            isAutomapped = true;
         }
      }
   }
   if (isAutomapped)
   {
      for (size_t i = 0; i < mInterfacePtr->mRegularApplications.size(); ++i)
      {
         WsfXIO_EntityId entity = mInterfacePtr->mRegularApplications[i];
         if (entity.GetSite() == aId.GetSite() || entity.GetSite() == DisEntityId::ALL_SITES)
         {
            if (entity.GetApplication() == aId.GetApplication() || entity.GetApplication() == DisEntityId::ALL_APPLIC)
            {
               isAutomapped = false;
            }
         }
      }
   }
   return isAutomapped;
}

//! Called from WsfDisInterface.
bool WsfXIO_AutoDisMapping::DelayDisEntityState(const DisEntityId& aEntityId)
{
   bool doDelay = false;
   if (IsAutoMapped(aEntityId))
   {
      ConnectionData*            connectionDataPtr;
      WsfXIO_DisPlatformInfoPkt* platformDataPtr = FindPlatformData(aEntityId, connectionDataPtr);
      if (platformDataPtr == nullptr)
      {
         doDelay = true;
         if (std::find(mDelayedEntityStates.begin(), mDelayedEntityStates.end(), aEntityId) == mDelayedEntityStates.end())
         {
            mDelayedEntityStates.push_back(aEntityId);
            if (mInterfacePtr->IsDebugEnabled())
            {
               auto out = ut::log::debug() << "xio_interface: auto_dis_mapping: Waiting to process Entity State.";
               out.AddNote() << "Entity: " << aEntityId;
            }
         }
      }
   }
   return doDelay;
}

//! Called from WsfDisInterface.
WsfPlatform* WsfXIO_AutoDisMapping::NewDisPlatform(WsfStringId aPlatformType, WsfDisEntityState* aEntityStatePtr)
{
   if (!IsAutoMapped(aEntityStatePtr->GetEntityId()))
   {
      return nullptr;
   }

   DisEntityId entityId = aEntityStatePtr->GetEntityId();

   ConnectionData*            connectionDataPtr;
   WsfXIO_DisPlatformInfoPkt* dataPtr = FindPlatformData(entityId, connectionDataPtr);

   bool         foundType   = true;
   WsfPlatform* platformPtr = nullptr;
   if (dataPtr != nullptr)
   {
      const WsfPlatform* inputPlatformPtr = mScenario.GetInputPlatformByName(dataPtr->mName);
      if (inputPlatformPtr != nullptr)
      {
         platformPtr = inputPlatformPtr->Clone();
         if (mInterfacePtr->IsDebugEnabled() && (platformPtr != nullptr))
         {
            auto out = ut::log::debug() << "xio_interface: auto_dis_mapping: New platform.";
            out.AddNote() << "Entity: " << entityId;
            out.AddNote() << "Name: " << platformPtr->GetName();
         }
      }
      else
      {
         platformPtr = WsfPlatformTypes::Get(mScenario).Clone(dataPtr->mType);
         if (mInterfacePtr->IsDebugEnabled() && (platformPtr != nullptr))
         {
            auto out = ut::log::debug() << "xio_interface: auto_dis_mapping: New platform.";
            out.AddNote() << "Entity: " << entityId;
            out.AddNote() << "Type: " << platformPtr->GetType();
         }
      }
   }
   if (platformPtr == nullptr)
   {
      foundType   = false;
      platformPtr = WsfPlatformTypes::Get(mScenario).Clone(aPlatformType);
      if (platformPtr == nullptr)
      {
         platformPtr = new WsfPlatform(mScenario);
      }
      if (mInterfacePtr->IsDebugEnabled() && dataPtr != nullptr)
      {
         auto out = ut::log::debug() << "xio_interface: auto_dis_mapping: Failed using platform name/type.";
         out.AddNote() << "Entity: " << entityId;
         out.AddNote() << "Name: " << dataPtr->mName;
         out.AddNote() << "Type: " << dataPtr->mType;
      }
   }

   // Build the platform that will get added to the simulation.
   // Note that all subsystem components are stripped, but comms are added back in with a filter.

   std::vector<wsf::comm::Comm*> comms;
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> ci(*platformPtr); !ci.AtEnd(); ++ci)
   {
      wsf::comm::Comm* commPtr = ci->Clone();
      commPtr->SetName(ci->GetName());
      comms.emplace_back(commPtr);
   }
   mExtInterfacePtr->StripExternalPlatform(platformPtr);

   WsfDisMover* moverPtr = new WsfDisMover(mDisInterfacePtr, platformPtr, aEntityStatePtr);
   platformPtr->SetMover(moverPtr);

   if (dataPtr != nullptr)
   {
      platformPtr->SetName(dataPtr->mName);
      platformPtr->SetSide(dataPtr->mSide);
      if (!foundType)
      {
         platformPtr->SetSide(mExtInterfacePtr->GetSideId(aEntityStatePtr->GetForceId()));
      }
      for (size_t i = 0; i < dataPtr->mCommanders.size(); ++i)
      {
         WsfCommandChain* ccPtr = new WsfCommandChain(dataPtr->mCommanders[i].first, dataPtr->mCommanders[i].second);
         platformPtr->AddComponent(ccPtr);
      }
      for (size_t i = 0; i < comms.size(); ++i)
      {
         wsf::comm::Comm* commPtr = comms[i];
         wsf::xio::CommComponent::FindOrCreate(*commPtr, this, connectionDataPtr->mConnectionPtr);
         platformPtr->AddComponent(commPtr);
      }
   }
   return platformPtr;
}

bool WsfXIO_AutoDisMapping::TranslatePlatformIndex(size_t& aIndex, ConnectionData* aConnectionDataPtr)
{
   bool ok = true;
   // An index of zero does not require translation.
   if (aIndex != 0)
   {
      ok                       = false;
      WsfPlatform* platformPtr = FindRemotePlatform(aIndex, aConnectionDataPtr);
      if (platformPtr != nullptr)
      {
         ok     = true;
         aIndex = platformPtr->GetIndex();
      }
      else
      {
         // See if we can find an index from the list of removed platforms.
         PlatformId                         platformId(aConnectionDataPtr->mConnectionPtr->GetConnectionId(), aIndex);
         RemovedPlatformMap::const_iterator rpi = mRemovedPlatforms.find(platformId);
         if (rpi != mRemovedPlatforms.end())
         {
            ok     = true;
            aIndex = rpi->second;
         }
      }
   }
   return ok;
}

bool WsfXIO_AutoDisMapping::TranslateTrack(double                      aSimTime,
                                           const WsfXIO_DisMessagePkt& aMessagePkt,
                                           ConnectionData*             aConnectionDataPtr,
                                           WsfTrack&                   aTrack)
{
   bool         translated    = false;
   WsfPlatform* originatorPtr = mSimulation.GetPlatformByName(aTrack.GetOriginatorNameId());
   WsfPlatform* targetPtr     = FindRemotePlatform(aTrack.GetTargetIndex(), aConnectionDataPtr);
   if (originatorPtr != nullptr)
   {
      double timeDiff = aSimTime - aMessagePkt.mSimTime;
      aTrack.SetUpdateTime(aTrack.GetUpdateTime() + timeDiff);
      aTrack.SetStartTime(aTrack.GetStartTime() + timeDiff);
      aTrack.SetPlatformOriginator(originatorPtr);
      size_t targetIndex = 0;
      if (targetPtr != nullptr)
      {
         targetIndex = targetPtr->GetIndex();
      }
      aTrack.SetTargetIndex(targetIndex);
      translated = true;
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::ForwardMessage(double                aSimTime,
                                           ConnectionData*       aConnectionData,
                                           WsfXIO_DisMessagePkt& aPkt,
                                           WsfMessage&           aMessage)
{
   // 'Reset' the message values. The addresses have missing data not serialized over XIO
   // that can be reconstituted here. This is important to ensure proper comparisons of addresses.
   // While we don't directly use the destination address here, we should set it to ensure
   // proper usage in the receiving comm device.
   wsf::comm::Address sourceAddress, destinationAddress, nextHopAddress;
   sourceAddress.SetAddress(aMessage.GetSrcAddr().GetAddress(), aMessage.GetSrcAddr().GetSubnetMaskBitLength());
   destinationAddress.SetAddress(aMessage.GetDstAddr().GetAddress(), aMessage.GetDstAddr().GetSubnetMaskBitLength());
   nextHopAddress.SetAddress(aMessage.GetNextHopAddr().GetAddress(), aMessage.GetNextHopAddr().GetSubnetMaskBitLength());
   aMessage.SetSrcAddr(sourceAddress);
   aMessage.SetDstAddr(destinationAddress);
   aMessage.SetNextHopAddr(nextHopAddress);

   bool messageForwarded = false;
   auto networkManager   = mSimulation.GetCommNetworkManager();
   if (!networkManager)
   {
      throw UtException("Missing network manager reference in WsfXIO_AutoDisMapping");
   }

   auto nextHopCommPtr    = networkManager->GetComm(aMessage.GetNextHopAddr());
   auto originatorCommPtr = networkManager->GetComm(aMessage.GetSrcAddr());

   if (nextHopCommPtr && originatorCommPtr)
   {
      wsf::comm::Message commMessage(ut::clone(aMessage));
      messageForwarded = nextHopCommPtr->Receive(mSimulation.GetSimTime(), originatorCommPtr, commMessage);
   }
   return messageForwarded;
}


bool WsfXIO_AutoDisMapping::TranslateTaskAssignMessage(double                aSimTime,
                                                       WsfMessage*           aMessagePtr,
                                                       ConnectionData*       aConnectionData,
                                                       WsfXIO_DisMessagePkt& aPkt)
{
   WsfTaskAssignMessage* taskMessagePtr = (WsfTaskAssignMessage*)aMessagePtr;
   WsfTask&              task           = taskMessagePtr->GetTask();
   size_t                assigneeIndex  = task.GetAssigneePlatformIndex();
   size_t                assignerIndex  = task.GetAssignerPlatformIndex();
   bool                  translated     = TranslatePlatformIndex(assigneeIndex, aConnectionData) &&
                     TranslatePlatformIndex(assignerIndex, aConnectionData) &&
                     TranslateTrack(aSimTime, aPkt, aConnectionData, taskMessagePtr->GetTrack());
   if (translated)
   {
      task.SetAssigneePlatformIndex(assigneeIndex);
      task.SetAssignerPlatformIndex(assignerIndex);
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateTaskStatusMessage(double                aSimTime,
                                                       WsfMessage*           aMessagePtr,
                                                       ConnectionData*       aConnectionData,
                                                       WsfXIO_DisMessagePkt& aPkt)
{
   WsfTaskStatusMessage* taskMessagePtr = (WsfTaskStatusMessage*)aMessagePtr;
   size_t                assigneeIndex  = taskMessagePtr->GetAssigneePlatformIndex();
   size_t                assignerIndex  = taskMessagePtr->GetAssignerPlatformIndex();
   bool                  translated =
      TranslatePlatformIndex(assigneeIndex, aConnectionData) && TranslatePlatformIndex(assignerIndex, aConnectionData);
   if (translated)
   {
      taskMessagePtr->SetAssigneePlatformIndex(assigneeIndex);
      taskMessagePtr->SetAssignerPlatformIndex(assignerIndex);
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateTaskCancelMessage(double                aSimTime,
                                                       WsfMessage*           aMessagePtr,
                                                       ConnectionData*       aConnectionData,
                                                       WsfXIO_DisMessagePkt& aPkt)
{
   WsfTaskCancelMessage* taskMessagePtr = (WsfTaskCancelMessage*)aMessagePtr;
   size_t                assigneeIndex  = taskMessagePtr->GetAssigneePlatformIndex();
   size_t                assignerIndex  = taskMessagePtr->GetAssignerPlatformIndex();
   bool                  translated =
      TranslatePlatformIndex(assigneeIndex, aConnectionData) && TranslatePlatformIndex(assignerIndex, aConnectionData);
   if (translated)
   {
      taskMessagePtr->SetAssigneePlatformIndex(assigneeIndex);
      taskMessagePtr->SetAssignerPlatformIndex(assignerIndex);
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateTrackMessage(double                aSimTime,
                                                  WsfMessage*           aMessagePtr,
                                                  ConnectionData*       aConnectionData,
                                                  WsfXIO_DisMessagePkt& aPkt)
{
   WsfTrackMessage* trackMessagePtr = (WsfTrackMessage*)aMessagePtr;
   return TranslateTrack(aSimTime, aPkt, aConnectionData, *trackMessagePtr->GetTrack());
}

bool WsfXIO_AutoDisMapping::TranslateTrackDropMessage(double                aSimTime,
                                                      WsfMessage*           aMessagePtr,
                                                      ConnectionData*       aConnectionData,
                                                      WsfXIO_DisMessagePkt& aPkt)
{
   WsfTrackDropMessage* dropMessagePtr = (WsfTrackDropMessage*)aMessagePtr;
   size_t               targetIndex    = dropMessagePtr->GetTargetIndex();
   double               dropTime       = dropMessagePtr->GetTime();
   bool                 translated     = TranslatePlatformIndex(targetIndex, aConnectionData);
   if (!translated)
   {
      // This *SHOULD NOT* happen, but if it does its not a big deal. Just set the target index to zero
      // and go on. It is non-required truth information.
      auto out = ut::log::warning() << "WSF_DROP_TRACK_MESSAGE target index no longer exists.";
      out.AddNote() << "Target Index: " << dropMessagePtr->GetTargetIndex();
      out.AddNote() << "Track: " << dropMessagePtr->GetTrackId();
      targetIndex = 0;
   }
   dropMessagePtr->SetTargetIndex(targetIndex);
   TranslateTime(aSimTime, aPkt, dropTime);
   dropMessagePtr->SetTime(dropTime);

   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateTaskControlMessage(double                aSimTime,
                                                        WsfMessage*           aMessagePtr,
                                                        ConnectionData*       aConnectionData,
                                                        WsfXIO_DisMessagePkt& aPkt)
{
   WsfTaskControlMessage* taskMessagePtr = (WsfTaskControlMessage*)aMessagePtr;
   size_t                 assigneeIndex  = taskMessagePtr->GetAssigneePlatformIndex();
   size_t                 assignerIndex  = taskMessagePtr->GetAssignerPlatformIndex();
   bool                   translated =
      TranslatePlatformIndex(assigneeIndex, aConnectionData) && TranslatePlatformIndex(assignerIndex, aConnectionData);
   if (translated)
   {
      taskMessagePtr->SetAssigneePlatformIndex(assigneeIndex);
      taskMessagePtr->SetAssignerPlatformIndex(assignerIndex);
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateAssociationMessage(double                aSimTime,
                                                        WsfMessage*           aMessagePtr,
                                                        ConnectionData*       aConnectionData,
                                                        WsfXIO_DisMessagePkt& aPkt)
{
   WsfAssociationMessage& message       = (WsfAssociationMessage&)*aMessagePtr;
   size_t                 platformIndex = message.GetSensorPlatformIndex();
   bool                   translated    = TranslatePlatformIndex(platformIndex, aConnectionData);
   if (translated)
   {
      message.SetSensorPlatformIndex(platformIndex);
   }
   return translated;
}

bool WsfXIO_AutoDisMapping::TranslateStatusMessage(double                aSimTime,
                                                   WsfMessage*           aMessagePtr,
                                                   ConnectionData*       aConnectionData,
                                                   WsfXIO_DisMessagePkt& aPkt)
{
   WsfStatusMessage* statusMessagePtr = (WsfStatusMessage*)aMessagePtr;
   size_t            platformIndex    = statusMessagePtr->GetPlatformIndex();
   bool              translated       = TranslatePlatformIndex(platformIndex, aConnectionData);
   if (translated)
   {
      statusMessagePtr->SetPlatformIndex(platformIndex);
   }
   return translated;
}

void WsfXIO_AutoDisMapping::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr->IsExternallyControlled())
   {
      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(aPlatformPtr->GetIndex());
      if (disPlatformPtr != nullptr)
      {
         WsfXIO_EntityId entityId = disPlatformPtr->GetEntityId();
         for (auto& remotePlatform : mRemotePlatforms)
         {
            if (remotePlatform.second->mPlatformInfo.mEntityId == entityId)
            {
               remotePlatform.second->mPlatformPtr = aPlatformPtr;
            }
         }
      }
   }
}

void WsfXIO_AutoDisMapping::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // The former version of this routine bracket this loop and only did it if the platform being deleted
   // was externally controlled. Well, it appears we are receiving data from remote sources that are locally
   // controlled. Therefore we need to update the list also.
   for (RemotePlatformMap::iterator i = mRemotePlatforms.begin(); i != mRemotePlatforms.end(); ++i)
   {
      RemotePlatformInfo* infoPtr = i->second;
      if (infoPtr->mPlatformPtr == aPlatformPtr)
      {
         UpdateRemovedPlatforms(infoPtr->mConnectionPtr->mConnectionPtr->GetConnectionId(),
                                infoPtr->mPlatformInfo.mPlatformIndex,
                                aPlatformPtr);
         delete infoPtr;
         mRemotePlatforms.erase(i);
         break;
      }
   }
}

void WsfXIO_AutoDisMapping::TranslateTime(double aSimTime, WsfXIO_DisMessagePkt& aPkt, double& aRemoteSimTime)
{
   aRemoteSimTime += (aSimTime - aPkt.mSimTime);
}

WsfPlatform* WsfXIO_AutoDisMapping::FindRemotePlatform(size_t aRemoteIndex, ConnectionData* aDataPtr)
{
   WsfPlatform*        platformPtr = nullptr;
   RemotePlatformInfo* infoPtr     = FindPlatform(aDataPtr->mConnectionPtr->GetConnectionId(), aRemoteIndex);
   if (infoPtr != nullptr)
   {
      platformPtr = infoPtr->mPlatformPtr;
   }
   return platformPtr;
}

WsfXIO_AutoDisMapping::RemotePlatformInfo* WsfXIO_AutoDisMapping::FindPlatform(int aConnectionId, size_t aPlatformIndex)
{
   RemotePlatformInfo*         platformInfoPtr = nullptr;
   RemotePlatformMap::iterator i               = mRemotePlatforms.find(PlatformId(aConnectionId, aPlatformIndex));
   if (i != mRemotePlatforms.end())
   {
      platformInfoPtr = i->second;
   }
   return platformInfoPtr;
}

void WsfXIO_AutoDisMapping::HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt)
{
   ConnectionData*             connectionDataPtr = GetConnection((WsfXIO_Connection*)aPkt.GetSender());
   PlatformId                  platformId(connectionDataPtr->mConnectionPtr->GetConnectionId(), aPkt.mPlatformIndex);
   RemotePlatformMap::iterator iter = mRemotePlatforms.find(platformId);

   if (aPkt.mIsAlive)
   {
      if (iter == mRemotePlatforms.end())
      {
         RemotePlatformInfo* infoPtr    = new RemotePlatformInfo;
         infoPtr->mConnectionPtr        = connectionDataPtr;
         infoPtr->mPlatformInfo         = aPkt;
         WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->FindDisPlatform(aPkt.mEntityId.Convert<DisEntityId>());
         if (disPlatformPtr != nullptr)
         {
            infoPtr->mPlatformPtr = disPlatformPtr->GetPlatform();
         }
         else
         {
            infoPtr->mPlatformPtr = nullptr;
         }
         mRemotePlatforms.insert(make_pair(platformId, infoPtr));
      }

      connectionDataPtr->mPlatforms.push_back(aPkt);
      for (size_t j = 0; j < mDelayedEntityStates.size(); ++j)
      {
         if (mDelayedEntityStates[j] == aPkt.mEntityId)
         {
            if (mInterfacePtr->IsDebugEnabled())
            {
               auto out = ut::log::debug() << "xio_interface: auto_dis_mapping: Resuming processing of Entity State.";
               out.AddNote() << "Entity: " << aPkt.mEntityId.Convert<DisEntityId>();
            }
            mDisInterfacePtr->ResumeEntityStateProcessing(mSimulation.GetSimTime(), aPkt.mEntityId.Convert<DisEntityId>());
         }
      }
   }
   else
   {
      if (iter != mRemotePlatforms.end())
      {
         UpdateRemovedPlatforms(platformId.first, platformId.second, iter->second->mPlatformPtr);
         delete iter->second;
         mRemotePlatforms.erase(iter);
      }
      RemovePlatformFromConnection(connectionDataPtr, aPkt.mPlatformIndex);
   }
}

WsfXIO_AutoDisMapping::ConnectionData* WsfXIO_AutoDisMapping::GetConnection(WsfXIO_Connection* aConnectionPtr)
{
   ConnectionData* connectionDataPtr = nullptr;
   for (size_t i = 0; i < mConnections.size(); ++i)
   {
      if (mConnections[i]->mConnectionPtr == aConnectionPtr)
      {
         connectionDataPtr = mConnections[i];
      }
   }
   return connectionDataPtr;
}

WsfXIO_AutoDisMapping::ConnectionData* WsfXIO_AutoDisMapping::FindConnectionData(WsfXIO_Connection* aConnectionPtr)
{
   ConnectionData* connectionDataPtr = nullptr;
   for (size_t i = 0; i < mConnections.size(); ++i)
   {
      if (mConnections[i]->mConnectionPtr == aConnectionPtr)
      {
         connectionDataPtr = mConnections[i];
      }
   }
   return connectionDataPtr;
}

void WsfXIO_AutoDisMapping::HandleDisMessage(WsfXIO_DisMessagePkt& aPkt)
{
   ConnectionData* connectionDataPtr = FindConnectionData((WsfXIO_Connection*)aPkt.GetSender());
   if (aPkt.mMessagePtr != nullptr && connectionDataPtr != nullptr)
   {
      double                         simTime     = mSimulation.GetSimTime();
      WsfStringId                    messageType = aPkt.mMessagePtr->GetType();
      MessageTranslatorMap::iterator i           = mMessageTranslators.find(messageType);
      if (i != mMessageTranslators.end())
      {
         WsfMessage* messagePtr = aPkt.mMessagePtr->Clone();
         // 'Fix' references originating from other simulation instances, potentially.
         messagePtr->SetSimulation(&mSimulation);
         WsfPlatform* platformPtr = FindRemotePlatform(aPkt.mPlatformIndex, connectionDataPtr);
         if (platformPtr)
         {
            messagePtr->SetOriginator(platformPtr);
         }
         if (i->second(simTime, messagePtr, connectionDataPtr, aPkt))
         {
            ForwardMessage(simTime, connectionDataPtr, aPkt, *messagePtr);
            if (mInterfacePtr->IsDebugEnabled())
            {
               auto out = ut::log::debug() << "XIO: Translating.";
               out.AddNote() << "Message: " << aPkt.mMessagePtr->GetType();
            }
         }
         else if (mInterfacePtr->IsDebugEnabled())
         {
            auto out = ut::log::debug() << "XIO: Failed to translate message.";
            out.AddNote() << "Message: " << aPkt.mMessagePtr->GetType();
         }
         delete messagePtr;
      }
      else
      {
         auto out = ut::log::error() << "XIO: Message has no translator.";
         out.AddNote() << "Message: " << aPkt.mMessagePtr->GetType();
      }
   }
}

WsfXIO_DisPlatformInfoPkt* WsfXIO_AutoDisMapping::FindPlatformData(const DisEntityId& aEntity,
                                                                   ConnectionData*&   aConnectionPtr)
{
   aConnectionPtr                             = nullptr;
   WsfXIO_DisPlatformInfoPkt* platformDataPtr = nullptr;
   for (size_t i = 0; i < mConnections.size() && platformDataPtr == nullptr; ++i)
   {
      ConnectionData* connectionDataPtr = mConnections[i];
      for (size_t j = 0; j < connectionDataPtr->mPlatforms.size(); ++j)
      {
         if ((connectionDataPtr->mPlatforms[j].mEntityId == aEntity) &&
             !connectionDataPtr->mPlatforms[j].mIsExternallyControlled)
         {
            platformDataPtr = &connectionDataPtr->mPlatforms[j];
            aConnectionPtr  = connectionDataPtr;
            break;
         }
      }
   }
   return platformDataPtr;
}

void WsfXIO_AutoDisMapping::HandleConnect(WsfXIO_Connection* aConnectionPtr)
{
   if (!aConnectionPtr->IsReliable())
   {
      return;
   }
   ConnectionData* connectionDataPtr = new ConnectionData;
   connectionDataPtr->mConnectionPtr = aConnectionPtr;
   connectionDataPtr->mDisRequestPtr = new WsfXIO_DisExtension_Detail::DisRequest(aConnectionPtr, this);
   mDisRequestManagerPtr->AddRequest(connectionDataPtr->mDisRequestPtr);
   mConnections.push_back(connectionDataPtr);
}

void WsfXIO_AutoDisMapping::HandleDisconnect(WsfXIO_Connection* aConnectionPtr)
{
   if (!aConnectionPtr->IsReliable())
   {
      return;
   }
   for (size_t i = 0; i < mConnections.size(); ++i)
   {
      ConnectionData* connectionDataPtr = mConnections[i];
      if (connectionDataPtr->mConnectionPtr == aConnectionPtr)
      {
         mConnections.erase(mConnections.begin() + i);
         delete connectionDataPtr;
         int connectionId = aConnectionPtr->GetConnectionId();
         for (RemotePlatformMap::iterator iter = mRemotePlatforms.begin(); iter != mRemotePlatforms.end();)
         {
            if (iter->first.first == connectionId)
            {
               WsfPlatform* platformPtr = iter->second->mPlatformPtr;
               if (platformPtr != nullptr)
               {
                  UpdateRemovedPlatforms(connectionId, iter->second->mPlatformInfo.mPlatformIndex, platformPtr);
                  // Prevent messages from getting routed back to us.
                  for (unsigned int j = 0; j < platformPtr->GetComponentCount<wsf::comm::Comm>(); ++j)
                  {
                     platformPtr->GetComponentEntry<wsf::comm::Comm>(j)
                        ->GetComponents()
                        .DeleteComponent<wsf::xio::CommComponent>();
                  }
               }
               delete iter->second;
               mRemotePlatforms.erase(iter++);
            }
            else
            {
               ++iter;
            }
         }
      }
   }
}

//! Removes a platform from the list of platforms maintained by a connection.
void WsfXIO_AutoDisMapping::RemovePlatformFromConnection(ConnectionData* aConnectionDataPtr, unsigned int aRemoteIndex)
{
   for (size_t j = 0; j < aConnectionDataPtr->mPlatforms.size(); ++j)
   {
      if (aConnectionDataPtr->mPlatforms[j].mPlatformIndex == static_cast<int>(aRemoteIndex))
      {
         aConnectionDataPtr->mPlatforms.erase(aConnectionDataPtr->mPlatforms.begin() + j);
         break;
      }
   }
}

//! Update the 'removed platforms' list as necessary.
void WsfXIO_AutoDisMapping::UpdateRemovedPlatforms(int aConnectionId, size_t aRemoteIndex, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr != nullptr)
   {
      PlatformId platformId(aConnectionId, aRemoteIndex);
      if (mRemovedPlatforms.find(platformId) == mRemovedPlatforms.end())
      {
         mRemovedPlatforms.insert(std::make_pair(platformId, aPlatformPtr->GetIndex()));
      }
   }
}
