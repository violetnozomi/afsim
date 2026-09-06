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

#ifndef WSFXIO_AUTODISMAPPING_HPP
#define WSFXIO_AUTODISMAPPING_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

class DisEntityId;
#include "UtCallbackHolder.hpp"
class WsfDisEntityState;
class WsfDisInterface;
class WsfExtInterface;
class WsfMessage;
class WsfPlatform;
class WsfScenario;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfXIO_Connection;
class WsfXIO_DisExtension;
class WsfXIO_DisRequest;
class WsfXIO_DisRequestManager;
#include "xio/WsfXIO_PacketRegistry.hpp"
class WsfXIO_Simulation;
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

//! Overrides the creation of DIS platforms using type information received over XIO
class WSF_EXPORT WsfXIO_AutoDisMapping
{
public:
   WsfXIO_AutoDisMapping(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_DisExtension* aDisExtensionPtr);

   ~WsfXIO_AutoDisMapping();

   bool Initialize();

   bool DelayDisEntityState(const DisEntityId& aEntityId);

   WsfPlatform* NewDisPlatform(WsfStringId aPlatformType, WsfDisEntityState* aEntityStatePtr);

   bool CanTranslateMessage(WsfStringId aMessageType) const
   {
      return mMessageTranslators.find(aMessageType) != mMessageTranslators.end();
   }

   void HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt);

   struct ConnectionData
   {
      // To be replaced by map
      WsfXIO_DisPlatformInfoPkt* FindPlatformData(int aIndex)
      {
         for (size_t i = 0; i < mPlatforms.size(); ++i)
         {
            if (mPlatforms[i].mPlatformIndex == aIndex)
            {
               return &mPlatforms[i];
            }
         }
         return nullptr;
      }
      WsfXIO_DisRequest*                     mDisRequestPtr;
      WsfXIO_Connection*                     mConnectionPtr;
      std::vector<WsfXIO_DisPlatformInfoPkt> mPlatforms;
   };

   ConnectionData* FindConnectionData(WsfXIO_Connection* aConnectionPtr);

   //! Data pertaining to a platform owned by an external simulation
   struct RemotePlatformInfo
   {
      //! Information about the platform
      WsfXIO_DisPlatformInfoPkt mPlatformInfo;
      //! Connection to the simulation which owns the platform
      ConnectionData* mConnectionPtr;
      //! Pointer to the platform controlled by DIS
      WsfPlatform* mPlatformPtr;
   };

   void HandleConnect(WsfXIO_Connection* aConnectionPtr);

   void HandleDisconnect(WsfXIO_Connection* aConnectionPtr);

   bool TranslateTrack(double                      aSimTime,
                       const WsfXIO_DisMessagePkt& aMessagePkt,
                       ConnectionData*             aConnectionDataPtr,
                       WsfTrack&                   aTrack);

   bool TranslatePlatformIndex(size_t& aIndex, ConnectionData* aConnectionDataPtr);

   void TranslateTime(double aSimTime, WsfXIO_DisMessagePkt& aPkt, double& aRemoteSimTime);

   bool ForwardMessage(double aSimTime, ConnectionData* aConnectionData, WsfXIO_DisMessagePkt& aPkt, WsfMessage& aMessage);

   struct TranslateMessageFunction
   {
      typedef bool (WsfXIO_AutoDisMapping::*MethodPtr)(double, WsfMessage*, ConnectionData*, WsfXIO_DisMessagePkt&);

      TranslateMessageFunction() {}

      TranslateMessageFunction(WsfXIO_AutoDisMapping* aThisPtr, MethodPtr aMethod)
         : mThisPtr(aThisPtr)
         , mMethod(aMethod)
      {
      }

      bool operator()(double aSimTime, WsfMessage* aMessagePtr, ConnectionData* aConnectionPtr, WsfXIO_DisMessagePkt& aMessagePkt)
      {
         return (mThisPtr->*mMethod)(aSimTime, aMessagePtr, aConnectionPtr, aMessagePkt);
      }

      WsfXIO_AutoDisMapping* mThisPtr;
      MethodPtr              mMethod;
   };

   RemotePlatformInfo* FindPlatform(int aConnectionId, size_t aPlatformIndex);

   WsfPlatform* FindRemotePlatform(size_t aRemoteIndex, ConnectionData* aDataPtr);

   WsfXIO_DisPlatformInfoPkt* FindPlatformData(const DisEntityId& aEntity, ConnectionData*& aConnectionPtr);

   void HandleDisMessage(WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTrackMessage(double                aSimTime,
                              WsfMessage*           aMessagePtr,
                              ConnectionData*       aConnectionData,
                              WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTrackDropMessage(double                aSimTime,
                                  WsfMessage*           aMessagePtr,
                                  ConnectionData*       aConnectionData,
                                  WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTaskAssignMessage(double                aSimTime,
                                   WsfMessage*           aMessagePtr,
                                   ConnectionData*       aConnectionData,
                                   WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTaskStatusMessage(double                aSimTime,
                                   WsfMessage*           aMessagePtr,
                                   ConnectionData*       aConnectionData,
                                   WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTaskCancelMessage(double                aSimTime,
                                   WsfMessage*           aMessagePtr,
                                   ConnectionData*       aConnectionData,
                                   WsfXIO_DisMessagePkt& aPkt);

   bool TranslateTaskControlMessage(double                aSimTime,
                                    WsfMessage*           aMessagePtr,
                                    ConnectionData*       aConnectionData,
                                    WsfXIO_DisMessagePkt& aPkt);

   bool TranslateAssociationMessage(double                aSimTime,
                                    WsfMessage*           aMessagePtr,
                                    ConnectionData*       aConnectionData,
                                    WsfXIO_DisMessagePkt& aPkt);

   bool TranslateStatusMessage(double                aSimTime,
                               WsfMessage*           aMessagePtr,
                               ConnectionData*       aConnectionData,
                               WsfXIO_DisMessagePkt& aPkt);

   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

protected:
   ConnectionData* GetConnection(WsfXIO_Connection* aConnectionPtr);

   bool IsAutoMapped(const DisEntityId& aId);

   void RemovePlatformFromConnection(ConnectionData* aDataPtr, unsigned int aRemoteIndex);

   void UpdateRemovedPlatforms(int aConnectionId, size_t aRemoteIndex, WsfPlatform* aPlatformPtr);

   typedef std::map<WsfStringId, TranslateMessageFunction> MessageTranslatorMap;
   //! ID of a remote platform (connection ID, remote platform index)
   typedef std::pair<int, size_t>                    PlatformId;
   typedef std::map<PlatformId, RemotePlatformInfo*> RemotePlatformMap;
   typedef std::map<PlatformId, size_t>              RemovedPlatformMap;

   WsfSimulation&               mSimulation;
   const WsfScenario&           mScenario;
   WsfDisInterface*             mDisInterfacePtr;
   WsfExtInterface*             mExtInterfacePtr;
   WsfXIO_Interface*            mInterfacePtr;
   WsfXIO_DisExtension*         mDisExtensionPtr;
   std::vector<ConnectionData*> mConnections;
   std::vector<WsfXIO_EntityId> mDelayedEntityStates;
   WsfXIO_DisRequestManager*    mDisRequestManagerPtr;
   MessageTranslatorMap         mMessageTranslators;

   //! Maps a PlatformID to RemotePlatformInfo
   RemotePlatformMap mRemotePlatforms;

   //! We may get messages from remote sources that contain platform indices local to them,
   //! even after the platforms have been removed (mostly track drop, but possibly others).
   //! Therefore, we must retain knowledge even after the platforms no longer exist.
   RemovedPlatformMap mRemovedPlatforms;

   UtCallbackHolder mCallbacks;
};
#endif
