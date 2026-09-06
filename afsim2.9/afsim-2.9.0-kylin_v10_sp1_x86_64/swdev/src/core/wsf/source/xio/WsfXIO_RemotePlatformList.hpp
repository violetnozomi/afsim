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

#ifndef WSFXIO_REMOTEPLATFORMLIST_HPP
#define WSFXIO_REMOTEPLATFORMLIST_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "UtMath.hpp"
class WsfClockSource;
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_ObjectInfo.hpp"
class WsfXIO_EntityStateData;
class WsfXIO_EntityStateRawPkt;
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PlatformInfoRequest.hpp"
#include "xio/WsfXIO_PlatformListRequest.hpp"
class WsfXIO_PlatformPartStatePkt;
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_RouteUpdatePkt;

//! WsfXIO_RemotePlatformList allows you to subscribe to platform lists maintained
//! by remote WSF-based applications.  The WsfXIO_RemotePlatformList consolidates
//! these platforms so that you can access them by DisEntityId without needing to
//! know their host application.  Alternatively, you can look up a platform by
//! platform index if you know the platform's host application.
class WSF_EXPORT WsfXIO_RemotePlatformList
{
private:
   class PlatformListRequest;
   class ConnectionInfo;

   typedef std::map<unsigned int, WsfXIO_PlatformInfo*> IndexMap;
   // typedef std::map<WsfXIO_EntityId,WsfXIO_PlatformInfo*>  EntityIdMap;

   typedef std::map<GenUniqueId, ConnectionInfo*>      ConnectionMap;
   typedef std::vector<WsfXIO_CommandChain>            CommandChainList;
   typedef std::map<WsfXIO_EntityId, CommandChainList> CommandChainMap;

public:
   typedef std::multimap<WsfXIO_EntityId, WsfXIO_PlatformInfo*> EntityIdMap;
   typedef std::map<WsfXIO_PlatformId, WsfXIO_PlatformInfo*>    PlatformIdMap;
   typedef std::pair<WsfStringId, unsigned int>                 PlatformNameId;
   typedef std::map<PlatformNameId, WsfXIO_PlatformInfo*>       PlatformNameIdMap;

   //! Information level specifies the depth of information you want from the
   //! platforms you have subscribed to.
   enum InformationLevel
   {
      cBASIC_INFORMATION = 1, //!< Provides Platform identity and list of platform parts.
      cSTATE_INFORMATION = 3, //!< Provides kinematic information in addition to the basic information.
      cROUTE_INFORMATION = 4,
      cNO_CHANGE         = 0x10000000 //! Internal Use only
   };

   //! Send an XIO Packet to all connections maintained by the remote platform
   //! list.
   void Send(WsfXIO_Packet& aPkt);

   //! Instantiate the RemotePlatformList.  You must call AddConnection to subscribe
   //! to remote platforms.
   WsfXIO_RemotePlatformList(WsfXIO_Interface* aInterfacePtr);

   ~WsfXIO_RemotePlatformList();

   void SetReliableEntityState(bool aIsReliable) { mReliableEntityState = aIsReliable; }

   void SetIgnoreExternalPlatforms(bool aIgnore) { mIgnoreExternalPlatforms = aIgnore; }

   void SetInformationLevels(int aPlatformInformationLevel);

   void SetDeadReckonParameters(double aDeadReckonInterval,
                                double aMaxDeadReckonError            = 5.0,
                                double aMaxDeadReckonOrientationError = 3.0 * UtMath::cRAD_PER_DEG);

   void SetInformationLevels(int                  aPlatformInformationLevel,
                             WsfXIO_PlatformInfo* aPlatformPtr,
                             double               aDeadReckonInterval            = 1.0,
                             double               aMaxDeadReckonError            = 5.0,
                             double               aMaxDeadReckonOrientationError = 3.0 * UtMath::cRAD_PER_DEG);

   //! Call this method to clean out the Remote Platform List's memory.
   void Destroy();

   //! Subscribe to an additional XIO connection (presumably from an additional remote
   //! host application).
   bool AddConnection(WsfXIO_Connection* aConnectionPtr, const WsfClockSource* aClockPtr);

   //! Unsubscribe from an XIO Connection (presumably from an additional remote
   //! host application).
   void DropConnection(WsfXIO_Connection* aConnectionPtr);

   //! Get the Connection that supplied the given Dis Entity ID.
   //! Returns 0 pointer if that Dis Entity ID isn't subscribed to.
   WsfXIO_Connection* GetConnectionByEntityId(const WsfXIO_EntityId& aEntityId);

   //! Get Platform Information associated with the given Dis Entity ID.
   //! Returns 0 pointer if that Dis Entity ID isn't subscribed to.
   WsfXIO_PlatformInfo* GetPlatformByEntityId(const WsfXIO_EntityId& aEntityId);

   //! Get Platform information associated with the given platform index hosted
   //! by the specified XIO connection.  Returns 0 pointer if that Dis Entity
   //! ID isn't subscribed to.
   WsfXIO_PlatformInfo* GetPlatformByIndex(const WsfXIO_Connection* aConnectionPtr, const unsigned int aPlatformIndex);

   WsfXIO_PlatformInfo* FindPlatform(WsfXIO_PlatformId aPlaformId);
   WsfXIO_PlatformInfo* FindPlatform(WsfXIO_EntityId aId, int aConnectionId);
   WsfXIO_PlatformInfo* FindPlatformByIndex(int aPlatformIndex, const WsfXIO_Connection* aConnectionPtr = nullptr);
   WsfXIO_PlatformInfo* FindPlatformByName(WsfStringId aPlatformName, const WsfXIO_Connection* aConnectionPtr = nullptr);

   //! Returns the platform's command chain.  CommandChainList::empty() == true
   //! if the command chain data is not available.
   CommandChainList GetCommandChainByEntityId(const WsfXIO_EntityId& aEntityId) const;

   //! Returns the list of all platforms presently recognized by the remote
   //! platform list.  The list returns contains pointers to the source
   //! platforms.  Do not maintain this list beyond scope.
   std::list<WsfXIO_PlatformInfo*> GetCurrentPlatforms() const;

   //! Provides information about a platform when it enters the distributed
   //! simulation environment.
   UtCallbackListN<void(WsfXIO_PlatformInfo&)> PlatformAdded;

   //! Provides information about a platform when it leaves the distributed
   //! simulation environment.
   UtCallbackListN<void(WsfXIO_PlatformInfo&)> PlatformRemoved;

   //! Provides information about a platform when its entity state changes.
   UtCallbackListN<void(WsfXIO_PlatformInfo&)> PlatformEntityStateChanged;

   //! Provides information about a platform when a platform part state changes.
   UtCallbackListN<void(WsfXIO_PlatformInfo&, WsfXIO_PlatformPartInfo&)> PlatformPartStateChanged;

   //! Provides information about a platform when its track manager state changes.
   UtCallbackListN<void(WsfXIO_PlatformInfo&)> TrackManagerStateChanged;

   //! Provides information about a platform when its entity state changes.
   UtCallbackListN<void(WsfXIO_PlatformInfo&, const std::vector<WsfXIO_CommandChain>&)> CommandChainInitialized;

   UtCallbackListN<void(WsfXIO_PlatformInfo&)> RouteUpdated;
   //! Returns the number of connections that the RemotePlatformList presently
   //! is subscribed to.
   unsigned int GetConnectionCount() const { return (unsigned int)mConnectionMap.size(); }

   const PlatformIdMap& GetAllPlatforms() const { return mPlatforms; }
   const EntityIdMap&   GetPlatformsByEntityId() const { return mAllPlatformsMap; }

private:
   WsfXIO_RemotePlatformList(const WsfXIO_RemotePlatformList& aRhs);
   WsfXIO_RemotePlatformList& operator=(const WsfXIO_RemotePlatformList& aRhs);

   void RemoveConnectionInfo(const GenUniqueId& aConnectionId);
   void RemoveConnectionInfo();

   void HandlePlatformAdded(WsfXIO_PlatformInfo* aPlatformPtr);
   void HandlePlatformDeleted(WsfXIO_PlatformInfo* aPlatformPtr);
   void HandleEntityState(WsfXIO_EntityStateRawPkt& aState);
   void HandlePartState(WsfXIO_PlatformPartStatePkt& aState);

   void HandleRouteUpdate(WsfXIO_RouteUpdatePkt& aRouteUpdate);

   //! ConnectionInfo maintains all data relevant to a single connection and
   //! its platform data stream.
   class ConnectionInfo
   {
   public:
      friend class WsfXIO_RemotePlatformList;

      ConnectionInfo(WsfXIO_RemotePlatformList* aParentPtr,
                     int                        aPlatformInfoLevel,
                     WsfXIO_Connection*         aConnectionPtr,
                     const WsfClockSource*      aClockSourcePtr);

      ~ConnectionInfo();

      void Destroy();
      void HandlePlatformList(WsfXIO_PlatformListUpdatePkt& aPkt);
      void HandleInfoResponse(const int aPlatformIndex, WsfXIO_ResponsePkt& aPkt);
      void HandleEntityState(WsfXIO_EntityStateData& aState);
      void HandlePartState(WsfXIO_PlatformPartStatePkt& aState);
      void HandleRouteUpdate(WsfXIO_RouteUpdatePkt& aRouteUpdate);

      void RemoveRequest() { mInterfacePtr->GetRequestManager().RemoveRequest(mPlatformListRequestPtr); }
      void AddRequest() { mInterfacePtr->GetRequestManager().AddRequest(mPlatformListRequestPtr); }

      void SetInformationLevels(WsfXIO_PlatformInfo* aPlatformPtr,
                                int                  aPlatformInformationLevel,
                                double               aDefaultDeadReckonInterval,
                                double               aDefaultMaxDeadReckonError,
                                double               aDefaultMaxDeadReckonOrientationError);

      WsfXIO_PlatformInfo* GetPlatformByIndex(const unsigned int aPlatformIndex);
      WsfXIO_Connection*   GetConnection() { return mConnectionPtr; }

      unsigned int ToUINT(const int aVal) { return static_cast<unsigned int>(aVal); }

   private:
      ConnectionInfo();
      ConnectionInfo(const ConnectionInfo& aRhs);
      ConnectionInfo& operator=(const ConnectionInfo& aRhs);

      WsfXIO_RemotePlatformList* mParentPtr;
      int                        mPlatformInfoLevel;
      WsfXIO_Connection*         mConnectionPtr;
      WsfXIO_Interface*          mInterfacePtr;
      PlatformListRequest*       mPlatformListRequestPtr;
      WsfXIO_RequestManager&     mPlatformInfoRequestManager;
      IndexMap                   mPlatformInfoMap;
      const WsfClockSource*      mClockSourcePtr;
   }; // end ConnectionInfo

   friend class ConnectionInfo;

   class PlatformListRequest : public WsfXIO_PlatformListRequest
   {
   public:
      PlatformListRequest(ConnectionInfo& aConnectionInfo)
         : WsfXIO_PlatformListRequest(aConnectionInfo.GetConnection())
         , mConnectionInfo(aConnectionInfo)
      {
      }

      void HandlePlatformList(WsfXIO_PlatformListUpdatePkt& aPkt) override { mConnectionInfo.HandlePlatformList(aPkt); }

   private:
      ConnectionInfo& mConnectionInfo;
   };

   class PlatformInfoRequest : public WsfXIO_PlatformInfoRequest
   {
   public:
      PlatformInfoRequest(int aPlatformIndex, unsigned int aInformationTypes, ConnectionInfo& aConnectionInfo, bool aIsReliable)
         : WsfXIO_PlatformInfoRequest(aPlatformIndex, aInformationTypes, aConnectionInfo.GetConnection(), aIsReliable)
         , mConnectionInfo(aConnectionInfo)
      {
      }

      void HandleResponse(WsfXIO_ResponsePkt& aPkt) override;

   protected:
      ConnectionInfo& mConnectionInfo;
   }; // end class PlatformInfoRequest

protected:
   unsigned int GetInfoType(int aInfoLevel);

   ConnectionInfo* FindConnection(const GenUniqueId& aId);

   WsfXIO_Interface* mInterfacePtr;

   bool            mReliableEntityState;
   bool            mUseDefaultDR_Params;
   bool            mIgnoreExternalPlatforms;
   ConnectionMap   mConnectionMap;
   EntityIdMap     mAllPlatformsMap;
   CommandChainMap mCommandChainMap;

   PlatformIdMap     mPlatforms;
   PlatformNameIdMap mPlatformsByName;
   UtCallbackHolder  mCallbacks;

   int    mDefaultPlatformInfoLevel;
   double mDefaultDeadReckonInterval;
   double mDefaultMaxDeadReckonError;
   double mDefaultMaxDeadReckonOrientationError;
};

#endif
