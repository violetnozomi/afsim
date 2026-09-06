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

#include "xio/WsfXIO_RemotePlatformList.hpp"

#include <climits>

#include "WsfClockSource.hpp"
#include "WsfSensor.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

WsfXIO_RemotePlatformList::WsfXIO_RemotePlatformList(WsfXIO_Interface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
   , mDefaultPlatformInfoLevel(cBASIC_INFORMATION)
   , mDefaultDeadReckonInterval(1.0)
   , mDefaultMaxDeadReckonError(5.0)
   , mDefaultMaxDeadReckonOrientationError(3.0 * UtMath::cRAD_PER_DEG)
{
   mReliableEntityState     = true;
   mUseDefaultDR_Params     = true;
   mIgnoreExternalPlatforms = false;
}

WsfXIO_RemotePlatformList::~WsfXIO_RemotePlatformList()
{
   // Destroy should already have been called.  This is a last resort fool-proofing.
   // Destroy() could throw an exception.
   Destroy();
}

void WsfXIO_RemotePlatformList::Destroy()
{
   // delete all connections, which includes unsubscribing and de-allocating memory.
   RemoveConnectionInfo();

   //   assert(mConnectionMap.empty()); // if the sim ends naturally, and hai is killed, not an issue.
   //   assert(mAllPlatformsMap.empty()); // if sim ends naturally, and hai is killed, not an issue.
}

void WsfXIO_RemotePlatformList::SetInformationLevels(int aPlatformInformationLevel)
{
   mDefaultPlatformInfoLevel = aPlatformInformationLevel;
}

void WsfXIO_RemotePlatformList::SetInformationLevels(int                  aPlatformInformationLevel,
                                                     WsfXIO_PlatformInfo* aPlatformPtr,
                                                     double               aDeadReckonInterval,
                                                     double               aMaxDeadReckonError,
                                                     double               aMaxDeadReckonOrientationError)
{
   WsfXIO_Connection* connectionPtr     = aPlatformPtr->GetHostConnection();
   ConnectionInfo*    connectionInfoPtr = FindConnection(connectionPtr->GetApplicationId());
   if (connectionInfoPtr != nullptr)
   {
      connectionInfoPtr->SetInformationLevels(aPlatformPtr,
                                              aPlatformInformationLevel,
                                              aDeadReckonInterval,
                                              aMaxDeadReckonError,
                                              aMaxDeadReckonOrientationError);
   }
}

void WsfXIO_RemotePlatformList::SetDeadReckonParameters(double aDeadReckonInterval,
                                                        double aMaxDeadReckonError,
                                                        double aMaxDeadReckonOrientationError)
{
   mUseDefaultDR_Params                  = false;
   mDefaultDeadReckonInterval            = aDeadReckonInterval;
   mDefaultMaxDeadReckonError            = aMaxDeadReckonError;
   mDefaultMaxDeadReckonOrientationError = aMaxDeadReckonOrientationError;
}

void WsfXIO_RemotePlatformList::ConnectionInfo::SetInformationLevels(WsfXIO_PlatformInfo* aPlatformPtr,
                                                                     int                  aPlatformInformationLevel,
                                                                     double               aDefaultDeadReckonInterval,
                                                                     double               aDefaultMaxDeadReckonError,
                                                                     double aDefaultMaxDeadReckonOrientationError)
{
   const WsfXIO_RequestManager::RequestMap& requests = mPlatformInfoRequestManager.GetRequests();
   for (WsfXIO_RequestManager::RequestMap::const_iterator iter = requests.begin(); iter != requests.end(); ++iter)
   {
      PlatformInfoRequest* infoRequestPtr = (PlatformInfoRequest*)iter->second;
      if (aPlatformPtr->GetIndex() == infoRequestPtr->GetPlatformIndex())
      {
         auto infoType = infoRequestPtr->GetInfoType();
         if (aPlatformInformationLevel != cNO_CHANGE)
         {
            infoType = mParentPtr->GetInfoType(aPlatformInformationLevel);
         }
         double drInterval =
            aDefaultDeadReckonInterval < 0 ? mParentPtr->mDefaultDeadReckonInterval : aDefaultDeadReckonInterval;
         double drError =
            aDefaultMaxDeadReckonError < 0 ? mParentPtr->mDefaultMaxDeadReckonError : aDefaultMaxDeadReckonError;
         double drOError = aDefaultMaxDeadReckonOrientationError < 0 ? mParentPtr->mDefaultMaxDeadReckonOrientationError :
                                                                       aDefaultMaxDeadReckonOrientationError;
         if (drInterval != infoRequestPtr->GetDeadReckonInterval() || drError != infoRequestPtr->GetMaxDeadReckonError() ||
             drOError != infoRequestPtr->GetMaxDeadReckonOrientationError() || infoRequestPtr->GetInfoType() != infoType)
         {
            // Don't send platform-info again
            infoType &= ~WsfXIO_RequestPlatformInfoPkt::cPLATFORM_INFO;
            // purge the old request and make a new one
            mPlatformInfoRequestManager.RemoveRequest(infoRequestPtr);
            auto requestPtr = ut::make_unique<PlatformInfoRequest>(aPlatformPtr->GetIndex(),
                                                                   infoType,
                                                                   *this,
                                                                   mParentPtr->mReliableEntityState);
            if (!mParentPtr->mUseDefaultDR_Params)
            {
               requestPtr->SetDeadReckonParameters(drInterval, drError, drOError);
            }
            mPlatformInfoRequestManager.AddRequest(requestPtr.release());
            break;
         }
      }
   }
}

bool WsfXIO_RemotePlatformList::AddConnection(WsfXIO_Connection* aConnectionPtr, const WsfClockSource* aClockPtr)
{
   if (mConnectionMap.find(aConnectionPtr->GetApplicationId()) != mConnectionMap.end())
   {
      return false;
   }
   assert(aClockPtr);
   ConnectionInfo* info = new ConnectionInfo(this, mDefaultPlatformInfoLevel, aConnectionPtr, aClockPtr);
   info->AddRequest();

   // add to the list of connections.
   const GenUniqueId& id = aConnectionPtr->GetApplicationId();
   mConnectionMap.insert(ConnectionMap::value_type(id, info));

   if (mCallbacks.Empty())
   {
      mCallbacks.Add(aConnectionPtr->GetInterface().Connect(&WsfXIO_RemotePlatformList::HandleEntityState, this));
      mCallbacks.Add(aConnectionPtr->GetInterface().Connect(&WsfXIO_RemotePlatformList::HandlePartState, this));
      mCallbacks.Add(aConnectionPtr->GetInterface().Connect(&WsfXIO_RemotePlatformList::HandleRouteUpdate, this));
   }
   return true;
}

void WsfXIO_RemotePlatformList::DropConnection(WsfXIO_Connection* aConnectionPtr)
{
   RemoveConnectionInfo(aConnectionPtr->GetApplicationId());
}

void WsfXIO_RemotePlatformList::RemoveConnectionInfo()
{
   for (ConnectionMap::iterator iter = mConnectionMap.begin(); iter != mConnectionMap.end(); ++iter)
   {
      ConnectionInfo* infoPtr = iter->second;
      infoPtr->Destroy();
      delete infoPtr;
   }
   mConnectionMap.clear();
}

void WsfXIO_RemotePlatformList::RemoveConnectionInfo(const GenUniqueId& aConnectionId)
{
   ConnectionMap::iterator iter = mConnectionMap.find(aConnectionId);
   if (iter != mConnectionMap.end())
   {
      ConnectionInfo* infoPtr = iter->second;

      for (EntityIdMap::iterator i = mAllPlatformsMap.begin(); i != mAllPlatformsMap.end();)
      {
         if (i->second->GetHostConnection() == infoPtr->GetConnection())
         {
            mAllPlatformsMap.erase(i++);
         }
         else
         {
            ++i;
         }
      }

      infoPtr->Destroy();
      delete infoPtr;
      mConnectionMap.erase(aConnectionId);
   }
}

//! Returns the connection which is sending the entity with the ID aEntityId.
//! @note Warning: This routine should not be used if multiple connections can send the same entity.
WsfXIO_Connection* WsfXIO_RemotePlatformList::GetConnectionByEntityId(const WsfXIO_EntityId& aEntityId)
{
   WsfXIO_Connection*    connectionPtr = nullptr;
   EntityIdMap::iterator foundIt       = mAllPlatformsMap.lower_bound(aEntityId);
   if (foundIt != mAllPlatformsMap.end() && foundIt->first == aEntityId)
   {
      connectionPtr = foundIt->second->GetHostConnection();
   }
   return connectionPtr;
}

//! Returns a WsfXIO_PlatformInfo for a platform with the given WsfXIO_EntityId if one exists
//! @note Warning: This routine should not be used if multiple connections can send the same entity.
WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::GetPlatformByEntityId(const WsfXIO_EntityId& aEntityId)
{
   WsfXIO_PlatformInfo*  platformPtr = nullptr;
   EntityIdMap::iterator foundIt     = mAllPlatformsMap.lower_bound(aEntityId);
   if (foundIt != mAllPlatformsMap.end() && foundIt->first == aEntityId)
   {
      platformPtr = foundIt->second;
   }
   return platformPtr;
}

WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::GetPlatformByIndex(const WsfXIO_Connection* aConnectionPtr,
                                                                   const unsigned int       aPlatformIndex)
{
   WsfXIO_PlatformInfo*    platformPtr = nullptr;
   ConnectionMap::iterator connIter    = mConnectionMap.find(aConnectionPtr->GetApplicationId());
   if (connIter != mConnectionMap.end())
   {
      platformPtr = connIter->second->GetPlatformByIndex(aPlatformIndex);
   }
   return platformPtr;
}

WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::FindPlatform(WsfXIO_PlatformId aPlaformId)
{
   WsfXIO_PlatformInfo*    platformPtr = nullptr;
   PlatformIdMap::iterator i           = mPlatforms.find(aPlaformId);
   if (i != mPlatforms.end())
   {
      platformPtr = i->second;
   }
   return platformPtr;
}

//! Returns the platform info for a platform with the given entity ID and connection ID
WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::FindPlatform(WsfXIO_EntityId aId, int aConnectionId)
{
   EntityIdMap::iterator i   = mAllPlatformsMap.lower_bound(aId);
   EntityIdMap::iterator end = mAllPlatformsMap.upper_bound(aId);
   for (; i != end; ++i)
   {
      if (i->second->GetConnectionId() == aConnectionId)
      {
         return i->second;
      }
   }
   return nullptr;
}

//! Returns the platform with remote index aPlatformIndex hosted by connection aConnectionPtr (optional)
//! note: If aConnectionPtr is not given, there may be multiple platforms sharing the same index.
WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::FindPlatformByIndex(int                      aPlatformIndex,
                                                                    const WsfXIO_Connection* aConnectionPtr)
{
   WsfXIO_PlatformInfo* platformPtr  = nullptr;
   int                  connectionId = 0;
   if (aConnectionPtr != nullptr)
   {
      connectionId = aConnectionPtr->GetConnectionId();
   }
   WsfXIO_PlatformId       id(aPlatformIndex, connectionId);
   PlatformIdMap::iterator i = mPlatforms.lower_bound(id);
   if (i != mPlatforms.end())
   {
      WsfXIO_PlatformId foundId = i->second->GetId();
      if ((int)foundId.mIndex == aPlatformIndex && (connectionId == 0 || foundId.mConnectionId == connectionId))
      {
         platformPtr = i->second;
      }
   }
   return platformPtr;
}

//! Returns the platform with name aPlatformName hosted by aConnectionPtr (optional).
//! note: If aConnectionPtr is not given, there may be multiple platforms sharing the same name.
WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::FindPlatformByName(WsfStringId              aPlatformName,
                                                                   const WsfXIO_Connection* aConnectionPtr)
{
   WsfXIO_PlatformInfo* platformPtr  = nullptr;
   int                  connectionId = 0;
   if (aConnectionPtr != nullptr)
   {
      connectionId = aConnectionPtr->GetConnectionId();
   }
   PlatformNameId              nameId(aPlatformName, connectionId);
   PlatformNameIdMap::iterator i = mPlatformsByName.lower_bound(nameId);
   if (i != mPlatformsByName.end())
   {
      WsfXIO_PlatformId foundId = i->second->GetId();
      if (i->second->GetName() == nameId.first && (connectionId == 0 || foundId.mConnectionId == connectionId))
      {
         platformPtr = i->second;
      }
   }
   return platformPtr;
}

std::list<WsfXIO_PlatformInfo*> WsfXIO_RemotePlatformList::GetCurrentPlatforms() const
{
   std::list<WsfXIO_PlatformInfo*> platList;
   for (EntityIdMap::const_iterator iter = mAllPlatformsMap.begin(); iter != mAllPlatformsMap.end(); ++iter)
   {
      platList.push_back(iter->second);
   }
   return platList;
}

std::vector<WsfXIO_CommandChain> WsfXIO_RemotePlatformList::GetCommandChainByEntityId(const WsfXIO_EntityId& aEntityId) const
{
   CommandChainMap::const_iterator iter = mCommandChainMap.find(aEntityId);
   if (iter != mCommandChainMap.end())
   {
      return iter->second;
   }
   else
   {
      return CommandChainList();
   }
}

WsfXIO_RemotePlatformList::ConnectionInfo* WsfXIO_RemotePlatformList::FindConnection(const GenUniqueId& aId)
{
   WsfXIO_RemotePlatformList::ConnectionInfo* connectionInfoPtr = nullptr;
   ConnectionMap::iterator                    i                 = mConnectionMap.find(aId);
   if (i != mConnectionMap.end())
   {
      connectionInfoPtr = i->second;
   }
   return connectionInfoPtr;
}

unsigned int WsfXIO_RemotePlatformList::GetInfoType(int aInfoLevel)
{
   unsigned int infoTypes = WsfXIO_RequestPlatformInfoPkt::cPART_INFO | WsfXIO_RequestPlatformInfoPkt::cPLATFORM_INFO |
                            WsfXIO_RequestPlatformInfoPkt::cCOMMAND_CHAIN_INFO;
   if (cSTATE_INFORMATION == (aInfoLevel & cSTATE_INFORMATION))
   {
      infoTypes |= WsfXIO_RequestPlatformInfoPkt::cENTITY_STATE_INFO;
   }
   if (aInfoLevel & cROUTE_INFORMATION)
   {
      infoTypes |= WsfXIO_RequestPlatformInfoPkt::cROUTE_INFO;
   }

   return infoTypes;
}

void WsfXIO_RemotePlatformList::HandlePlatformAdded(WsfXIO_PlatformInfo* aPlatformPtr)
{
   WsfXIO_PlatformId id                                                        = aPlatformPtr->GetId();
   mPlatforms[id]                                                              = aPlatformPtr;
   mPlatformsByName[PlatformNameId(aPlatformPtr->GetName(), id.mConnectionId)] = aPlatformPtr;
   PlatformAdded(*aPlatformPtr);
}

void WsfXIO_RemotePlatformList::HandlePlatformDeleted(WsfXIO_PlatformInfo* aPlatformPtr)
{
   PlatformRemoved(*aPlatformPtr);
   WsfXIO_PlatformId id = aPlatformPtr->GetId();
   mPlatforms.erase(id);
   mPlatformsByName.erase(PlatformNameId(aPlatformPtr->GetName(), id.mConnectionId));
}

//! Send an XIO Packet to all connections maintained by the remote platform list.
void WsfXIO_RemotePlatformList::Send(WsfXIO_Packet& aPkt)
{
   ConnectionMap::iterator iter    = mConnectionMap.begin();
   ConnectionMap::iterator iterEnd = mConnectionMap.end();
   for (; iter != iterEnd; ++iter)
   {
      WsfXIO_Connection* connPtr = iter->second->GetConnection();
      mInterfacePtr->Send(aPkt, connPtr);
   }
}

void WsfXIO_RemotePlatformList::HandleEntityState(WsfXIO_EntityStateRawPkt& aState)
{
   ConnectionInfo* infoPtr = FindConnection(aState.mApplicationId);
   if (infoPtr != nullptr)
   {
      infoPtr->HandleEntityState(aState);
   }
}

void WsfXIO_RemotePlatformList::HandlePartState(WsfXIO_PlatformPartStatePkt& aState)
{
   ConnectionInfo* infoPtr = FindConnection(aState.mApplicationId);
   if (infoPtr != nullptr)
   {
      infoPtr->HandlePartState(aState);
   }
}

void WsfXIO_RemotePlatformList::HandleRouteUpdate(WsfXIO_RouteUpdatePkt& aRouteUpdate)
{
   ConnectionInfo* infoPtr = FindConnection(aRouteUpdate.mApplicationId);
   if (infoPtr != nullptr)
   {
      infoPtr->HandleRouteUpdate(aRouteUpdate);
   }
}

// =================================================================================================
// Nested class WsfXIO_RemotePlatformList::ConnectionInfo
// =================================================================================================

WsfXIO_RemotePlatformList::ConnectionInfo::ConnectionInfo(WsfXIO_RemotePlatformList* aParentPtr,
                                                          int                        aPlatformInfoLevel,
                                                          WsfXIO_Connection*         aConnectionPtr,
                                                          const WsfClockSource*      aClockSourcePtr)
   : mParentPtr(aParentPtr)
   , mPlatformInfoLevel(aPlatformInfoLevel)
   , mConnectionPtr(aConnectionPtr)
   , mInterfacePtr(&(aConnectionPtr->GetInterface()))
   , mPlatformListRequestPtr(nullptr)
   , mPlatformInfoRequestManager(aConnectionPtr->GetInterface().GetRequestManager())
   , mClockSourcePtr(aClockSourcePtr)
{
   mPlatformListRequestPtr = new PlatformListRequest(*this);
}

WsfXIO_RemotePlatformList::ConnectionInfo::~ConnectionInfo() {}

void WsfXIO_RemotePlatformList::ConnectionInfo::Destroy()
{
   IndexMap::iterator iter    = mPlatformInfoMap.begin();
   IndexMap::iterator iterEnd = mPlatformInfoMap.end();
   for (; iter != iterEnd; ++iter)
   {
      WsfXIO_PlatformInfo* infoPtr = iter->second;
      mParentPtr->HandlePlatformDeleted(infoPtr);

      // erase the parent's map to the shallow copy.
      delete infoPtr;
   }

   // erase the mappings to now deleted memory.
   mPlatformInfoMap.clear();
}

WsfXIO_PlatformInfo* WsfXIO_RemotePlatformList::ConnectionInfo::GetPlatformByIndex(const unsigned int aPlatformIndex)
{
   WsfXIO_PlatformInfo* infoPtr = nullptr;
   IndexMap::iterator   i       = mPlatformInfoMap.find(aPlatformIndex);
   if (i != mPlatformInfoMap.end())
   {
      infoPtr = i->second;
   }
   return infoPtr;
}

void WsfXIO_RemotePlatformList::ConnectionInfo::HandlePlatformList(WsfXIO_PlatformListUpdatePkt& aPkt)
{
   if (aPkt.GetSender() != mConnectionPtr)
   {
      return;
   }
   for (size_t i = 0; i < aPkt.mPlatformsAdded.size(); ++i)
   {
      if (!(mParentPtr->mIgnoreExternalPlatforms && aPkt.mPlatformsAdded[i].mIsExternallyControlled))
      {
         auto infoRequestPtr = ut::make_unique<PlatformInfoRequest>(aPkt.mPlatformsAdded[i].mIndex,
                                                                    mParentPtr->GetInfoType(mPlatformInfoLevel),
                                                                    *this,
                                                                    mParentPtr->mReliableEntityState);
         if (!mParentPtr->mUseDefaultDR_Params)
         {
            infoRequestPtr->SetDeadReckonParameters(mParentPtr->mDefaultDeadReckonInterval,
                                                    mParentPtr->mDefaultMaxDeadReckonError,
                                                    mParentPtr->mDefaultMaxDeadReckonOrientationError);
         }
         mPlatformInfoRequestManager.AddRequest(infoRequestPtr.release());
      }
   }
   for (size_t i = 0; i < aPkt.mPlatformsDeleted.size(); ++i)
   {
      unsigned int index =
         (aPkt.mPlatformsDeleted[i] > -1) ? static_cast<unsigned int>(aPkt.mPlatformsDeleted[i]) : UINT_MAX;
      IndexMap::iterator iter = mPlatformInfoMap.find(index);
      if (iter != mPlatformInfoMap.end())
      {
         mParentPtr->HandlePlatformDeleted(iter->second);
         // remove the platform from the local database.
         mPlatformInfoMap.erase(iter);
      }
   }
}

void WsfXIO_RemotePlatformList::PlatformInfoRequest::HandleResponse(WsfXIO_ResponsePkt& aPkt)
{
   mConnectionInfo.HandleInfoResponse(mPlatformIndex, aPkt);
}

void WsfXIO_RemotePlatformList::ConnectionInfo::HandleInfoResponse(const int aPlatformIndex, WsfXIO_ResponsePkt& aPkt)
{
   // Basic information comes across once and is equivalent to initializing
   // the platform.
   if (aPkt.ID() == WsfXIO_PlatformPartInfoPkt::cPACKET_ID)
   {
      WsfXIO_PlatformPartInfoPkt& pkt                    = (WsfXIO_PlatformPartInfoPkt&)aPkt;
      mPlatformInfoMap[pkt.mPlatformInfoPtr->GetIndex()] = pkt.mPlatformInfoPtr;

      // assign the platform info its host connection.
      pkt.mPlatformInfoPtr->SetHostConnection(mConnectionPtr);

      // assign each platform part a pointer to its platform info.
      std::vector<WsfXIO_PlatformPartInfo*>::iterator iter    = pkt.mPlatformInfoPtr->BeginParts();
      std::vector<WsfXIO_PlatformPartInfo*>::iterator iterEnd = pkt.mPlatformInfoPtr->EndParts();
      for (; iter != iterEnd; ++iter)
      {
         (*iter)->SetPlatformPtr(pkt.mPlatformInfoPtr);
      }

      // Add the platform to the parent's consolidated database.
      mParentPtr->mAllPlatformsMap.insert(std::make_pair(pkt.mPlatformInfoPtr->GetEntityId(), pkt.mPlatformInfoPtr));
      // Notify the subscribers.
      mParentPtr->HandlePlatformAdded(pkt.mPlatformInfoPtr);
      pkt.mPlatformInfoPtr = nullptr;
   }
   // Advanced information (Entity state) comes across regular and updates a platform
   // that was already initialized.
   else if (aPkt.ID() == WsfXIO_EntityStatePkt::cPACKET_ID)
   {
      HandleEntityState((WsfXIO_EntityStatePkt&)aPkt);
   }
   // if a platform part state changed.
   else if (aPkt.ID() == WsfXIO_PartStateChangePkt::cPACKET_ID)
   {
      WsfXIO_PartStateChangePkt& pkt         = (WsfXIO_PartStateChangePkt&)aPkt;
      WsfXIO_PlatformInfo*       platformPtr = GetPlatformByIndex(pkt.mPlatformIndex);
      if (platformPtr != nullptr)
      {
         std::vector<WsfXIO_PlatformPartInfo*>::iterator iter    = platformPtr->BeginParts();
         std::vector<WsfXIO_PlatformPartInfo*>::iterator iterEnd = platformPtr->EndParts();
         for (; iter != iterEnd; ++iter)
         {
            WsfXIO_PlatformPartInfo* infoPtr = (*iter);
            if (static_cast<int>(infoPtr->GetPlatformIndex()) == pkt.mPlatformIndex &&
                infoPtr->GetName() == pkt.mPartName && infoPtr->GetPartType() == pkt.mPartType)
            {
               infoPtr->UpdateFromStateChange(pkt);
#ifdef TODO_XIO // Make sure WsfXIO_PlatformPartInfo::UpdateFromStateChange works
               // turn the platform part on/off but don't trigger call back to XIO.
               infoPtr->SetTurnedOnFlag(pkt.mIsTurnedOn);
               if (infoPtr->IsSensor())
               {
                  WsfXIO_ModedPartInfo* modedInfoPtr = (WsfXIO_ModedPartInfo*)infoPtr;
                  modedInfoPtr->mSelectedModeNames   = pkt.mSelectedModes;
                  modedInfoPtr->mCurrentModeName     = pkt.mCurrentModeName;
               }
#endif
               mParentPtr->PlatformPartStateChanged(*platformPtr, *infoPtr);
               break;
            }
         }
      }
   }
   // if a platform part state changed.
   else if (aPkt.ID() == WsfXIO_PlatformCommandChainPkt::cPACKET_ID)
   {
      WsfXIO_PlatformCommandChainPkt& pkt     = (WsfXIO_PlatformCommandChainPkt&)aPkt;
      WsfXIO_PlatformInfo*            infoPtr = GetPlatformByIndex(aPlatformIndex);
      assert(infoPtr != nullptr);
      mParentPtr->mCommandChainMap[infoPtr->GetEntityId()] = pkt.mCommandChains;
      mParentPtr->CommandChainInitialized(*infoPtr, pkt.mCommandChains);
   }
   // if a track manager state changed.
   else if (aPkt.ID() == WsfXIO_TrackManagerStateChangePkt::cPACKET_ID)
   {
      WsfXIO_TrackManagerStateChangePkt& pkt     = (WsfXIO_TrackManagerStateChangePkt&)aPkt;
      WsfXIO_PlatformInfo*               infoPtr = GetPlatformByIndex(aPlatformIndex);
      assert(infoPtr != nullptr);
      infoPtr->GetTrackManagerInfo().SetCorrelationStrategy(pkt.mCorrelationStrategy);
      infoPtr->GetTrackManagerInfo().SetFusionStrategy(pkt.mFusionStrategy);
      mParentPtr->TrackManagerStateChanged(*infoPtr);
   }
}

void WsfXIO_RemotePlatformList::ConnectionInfo::HandleEntityState(WsfXIO_EntityStateData& aState)
{
   WsfXIO_PlatformInfo* platformInfoPtr = GetPlatformByIndex(aState.mPlatformIndex);
   if (platformInfoPtr != nullptr)
   {
      UtEntity& state = platformInfoPtr->GetState();
      state.SetLocationWCS(aState.mLocation);
      state.SetVelocityWCS(aState.mVelocity);
      state.SetOrientationWCS(aState.mOrientation[0], aState.mOrientation[1], aState.mOrientation[2]);
      state.SetAccelerationWCS(aState.mAcceleration);
      if (mClockSourcePtr != nullptr)
      {
         platformInfoPtr->SetStateRefTime(mClockSourcePtr->GetClock(1.0E30));
      }
      // Notify the subscribers.
      mParentPtr->PlatformEntityStateChanged(*platformInfoPtr);
   }
}
void WsfXIO_RemotePlatformList::ConnectionInfo::HandlePartState(WsfXIO_PlatformPartStatePkt& aState)
{
   WsfXIO_PlatformInfo* platformInfoPtr = GetPlatformByIndex(aState.mPlatformIndex);
   if (platformInfoPtr != nullptr)
   {
      WsfXIO_PlatformPartInfo* partPtr = platformInfoPtr->FindPart(aState.mPartName, aState.mPartType);
      if (partPtr != nullptr)
      {
         partPtr->UpdateFromPartState(aState);
#ifdef TODO_XIO // Make sure WsfXIO_PlatformPartInfo::UpdateFromPartState works
         partPtr->mIsExternallyControlled = aState.mIsExternallyControlled;
         if (aState.mPartType == cCOMPONENT_ROLE<WsfSensor>())
         {
            WsfXIO_ArticulatedPartInfo* articulatedPartPtr = (WsfXIO_ArticulatedPartInfo*)part;
            articulatedPartPtr->mYaw                       = aState.mYaw;
            articulatedPartPtr->mPitch                     = aState.mPitch;
            articulatedPartPtr->mRoll                      = aState.mRoll;
         }
         if (aState.mPartType == cCOMPONENT_ROLE<WsfSensor>())
         {
            WsfXIO_SensorInfo* sensorPtr = (WsfXIO_SensorInfo*)part;
            std::swap(sensorPtr->mReceivers, aState.mReceivers);
            std::swap(sensorPtr->mTransmitters, aState.mTransmitters);
         }
#endif
         mParentPtr->PlatformPartStateChanged(*platformInfoPtr, *partPtr);
      }
   }
}

void WsfXIO_RemotePlatformList::ConnectionInfo::HandleRouteUpdate(WsfXIO_RouteUpdatePkt& aRouteUpdate)
{
   WsfXIO_PlatformInfo* platformInfoPtr = GetPlatformByIndex(aRouteUpdate.mPlatformIndex);
   if (platformInfoPtr != nullptr)
   {
      delete platformInfoPtr->mRoutePtr;
      platformInfoPtr->mRoutePtr = (aRouteUpdate.mRoutePtr != nullptr) ? aRouteUpdate.mRoutePtr->Clone() : nullptr;
      platformInfoPtr->mTargetWaypointIndex = aRouteUpdate.mTargetWaypointIndex;
      {
         // Notify the subscribers.
         mParentPtr->RouteUpdated(*platformInfoPtr);
      }
   }
}
