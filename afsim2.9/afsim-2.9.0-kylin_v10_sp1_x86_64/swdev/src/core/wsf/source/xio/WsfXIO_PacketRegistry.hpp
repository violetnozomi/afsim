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

#ifndef WSFXIO_PACKETREGISTRY_HPP
#define WSFXIO_PACKETREGISTRY_HPP

#include "wsf_export.h"

#include <climits>
#include <vector>

#include "GenUniqueId.hpp"
class PakProcessor;
#include "WsfLocalTrack.hpp"
#include "WsfRoute.hpp"
#include "WsfTaskData.hpp"
#include "WsfTrack.hpp"
#include "WsfTypes.hpp"
#include "WsfXIO.hpp"
#include "xio/WsfXIO_Data.hpp"
#include "xio/WsfXIO_Defines.hpp"
#include "xio/WsfXIO_ObjectInfo.hpp"
#include "xio/WsfXIO_Packet.hpp"
#include "xio/WsfXIO_PublishKey.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"

// See WsfXIO_Packet.hpp for packet ID reservations

//! Performs the registration of XIO messages
class WSF_EXPORT WsfXIO_PacketRegistry
{
public:
   static int  GetPacketVersion() { return sPacketVersion; }
   static void RegisterPackets(PakProcessor& aProcessor);
   static void RegisterClasses();

private:
   static int sPacketVersion;
};

// Encodes a pointer as a 64-bit integer
// The application should check any pointers for validity
class WsfXIO_EncodedPointer
{
public:
   WsfXIO_EncodedPointer() { mData.mInteger = 0; }
   WsfXIO_EncodedPointer(void* aPointer)
   {
      mData.mInteger = 0; // zero bytes not used by pointer for 32-bit systems
      mData.mPointer = aPointer;
   }
   WsfXIO_EncodedPointer(int64_t aInteger) { mData.mInteger = aInteger; }

   WsfXIO_EncodedPointer(const WsfXIO_EncodedPointer& aRhs) { mData.mInteger = aRhs.mData.mInteger; }
   void* GetPointer() { return mData.mPointer; }

   void SetPointer(void* aPointer)
   {
      mData.mInteger = 0; // zero bytes not used by pointer for 32-bit systems
      mData.mPointer = aPointer;
   }
   operator bool() const { return mData.mInteger != 0; }

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff.SerializeBuffer((char*)&mData.mInteger, sizeof(int64_t));
   }
   bool operator<(const WsfXIO_EncodedPointer& aRhs) { return mData.mInteger < aRhs.mData.mInteger; }
        operator int64_t() const { return mData.mInteger; }

private:
   union PointerOrInteger
   {
      int64_t mInteger;
      void*   mPointer;
   };
   PointerOrInteger mData;
};

//! Sent at regular intervals over the UDP connections
class WSF_EXPORT WsfXIO_HeartbeatPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_HeartbeatPkt, WsfXIO_Packet, 1)
   {
      using namespace PakSerialization;
      aBuff& mApplicationName& Enum(mApplicationType) & mTCP_Port;
   }

   //! Application's user-defined name
   WsfStringId mApplicationName;
   //! Member of WsfXIO::ApplicationType
   wsf::xio::ApplicationType mApplicationType;
   //! Port for connecting via TCP, 0 if N/A
   uint16_t mTCP_Port;
};

//! Sent at regular intervals over the UDP connections
class WSF_EXPORT WsfXIO_InitializePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_InitializePkt, WsfXIO_Packet, 2)
   {
      using namespace PakSerialization;
      aBuff& mStage& mApplicationName& Enum(mApplicationType);
   }

   //! Stage of the handshake (0, 1, or 2)
   int32_t mStage;
   //! Application's user-defined name
   WsfStringId mApplicationName;
   //! One of WsfXIO::ApplicationType
   wsf::xio::ApplicationType mApplicationType;
};

//! Informs application about a request to cancel a subscription,
//! or a notification that a subscription was canceled
class WSF_EXPORT WsfXIO_SubscriptionCancelPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SubscriptionCancelPkt, WsfXIO_Packet, 3) { aBuff& mIsRequest& mSubscriptionId; }
   bool    mIsRequest;
   int32_t mSubscriptionId;
};

//! Basic information required for subscription requests.
class WSF_EXPORT WsfXIO_RequestDataPkt : public WsfXIO_Packet
{
public:
   WsfXIO_RequestDataPkt(int aPacketId)
      : WsfXIO_Packet(aPacketId)
   {
   }
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_Packet::Serialize(aBuff);
      aBuff& mSubscriptionRequestId& mReliable;
   }
   int32_t mSubscriptionRequestId;
   bool    mReliable;
};

//! A request for data from an application
class WSF_EXPORT WsfXIO_RequestPkt : public WsfXIO_RequestDataPkt
{
public:
   enum SubscriptionType : std::int16_t
   {
      cPLATFORM_LIST,
      cDRAW
   };

   XIO_DEFINE_PACKET(WsfXIO_RequestPkt, WsfXIO_RequestDataPkt, 4) { aBuff& PakSerialization::Enum(mSubscriptionType); }

   SubscriptionType mSubscriptionType;
};

//! Base class for packets that communicate between a WsfXIO_Request and WsfXIO_ServiceSession.  Usage of this packet
//! type provides automatic routing back to the requester or session.
class WSF_EXPORT WsfXIO_ResponsePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET_CTOR(WsfXIO_ResponsePkt, WsfXIO_Packet, 5), mFromProvider(false), mCompletesRequest(false),
      mRequestId(0)
   {
   }
   XIO_DEFINE_PACKET_SERIALIZE() { aBuff& mFromProvider& mCompletesRequest& mRequestId; }
   bool mFromProvider;
   //! If true, the receipt of this response indicates the request has been satisfied and should be closed
   bool mCompletesRequest;
   //! Request ID.  If this is 0, the packet is for any receiver.
   int32_t mRequestId;
};

//! Informs remote applications whether the simulation is behind real-time
class WSF_EXPORT WsfXIO_TimeBehindPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TimeBehindPkt, WsfXIO_Packet, 42) { aBuff& mIsBehind; }

   bool mIsBehind;
};

class WSF_EXPORT WsfXIO_PlatformListUpdatePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PlatformListUpdatePkt, WsfXIO_Packet, 6) { aBuff& mPlatformsAdded& mPlatformsDeleted; }

   struct WSF_EXPORT PlatformData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mName& mSide& mIndex& mIcon& mEntityId& mIsExternallyControlled;
      }
      WsfStringId     mName;
      int             mIndex;
      WsfStringId     mSide;
      WsfStringId     mIcon;
      WsfXIO_EntityId mEntityId;
      bool            mIsExternallyControlled;
   };

   std::vector<PlatformData> mPlatformsAdded;
   std::vector<int32_t>      mPlatformsDeleted;
};

//! Command the application to delete a platform
class WSF_EXPORT WsfXIO_DeletePlatformPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_DeletePlatformPkt, WsfXIO_Packet, 7) { aBuff& mPlatformIndices; }

   //! List of platform indices to delete
   std::vector<int32_t> mPlatformIndices;
};

//! Requests that an application begin sending track information for a specified platform
class WSF_EXPORT WsfXIO_RequestTracksPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET_CTOR(WsfXIO_RequestTracksPkt, WsfXIO_RequestDataPkt, 8), mUpdateInterval(0.0), mRealtimeInterval(false)
   {
   }
   XIO_DEFINE_PACKET_SERIALIZE()
   {
      aBuff& mPlatformIndex& mSensorNameId& mSendInitialTracks& mRequiredData& mUpdateInterval& mRealtimeInterval;
   }
   enum TrackData
   {
      cRAW_TRACKS           = 0x1,
      cRAW_COASTED_TRACKS   = 0x2,
      cRAW_DROPPED_TRACKS   = 0x4,
      cLOCAL_TRACKS         = 0x8,
      cLOCAL_DROPPED_TRACKS = 0xe,
      cALL                  = 0x1f
   };

   //! Index of the platform whose tracks are requested
   int32_t mPlatformIndex;
   //! ID of the sensor whose tracks are requested
   //! Null if the platform's track manager's tracks are requested
   WsfStringId mSensorNameId;
   //! 'true' if tracks already in the track manager should be sent
   //! after the request is successful.
   bool mSendInitialTracks;
   //! Some combination of TrackData values
   int32_t mRequiredData;
   //! Update interval for sending track updates.  0 indicates send on track update.
   //! If this is zero, XIO may broadcast track updates to multiple receivers.
   double mUpdateInterval;
   //! 'True' if mUpdateInterval represents a change in real-time, not simulation-time
   bool mRealtimeInterval;
};

//! A local track
class WSF_EXPORT WsfXIO_LocalTrackPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_LocalTrackPkt, WsfXIO_ResponsePkt, 9) { aBuff& mPlatformIndex& mTrack; }
   void ReceiveCleanup() override { delete mTrack; }

   //! Index of the platform sending the track
   int32_t mPlatformIndex;

   const WsfLocalTrack* mTrack;
};

//! A raw track
class WSF_EXPORT WsfXIO_RawTrackPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RawTrackPkt, WsfXIO_ResponsePkt, 10) { aBuff& mPlatformIndex& mSensorNameId& mTrack; }
   void ReceiveCleanup() override { delete mTrack; }

   //! Index of the platform sending the track
   int32_t mPlatformIndex;
   //! NameId of the sensor producing this track.
   //! Only valid for sensor tracks (not track-manager tracks)
   WsfStringId mSensorNameId;

   const WsfTrack* mTrack;
};

//! Notification of a track coast
class WSF_EXPORT WsfXIO_TrackCoastPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TrackCoastPkt, WsfXIO_ResponsePkt, 11)
   {
      aBuff& mIsRawTrack& mPlatformIndex& mSensorNameId& mTrackId;
   }
   //! true if the dropped track was a raw track, it was a local track otherwise
   bool mIsRawTrack;
   //! Platform sending the track drop
   int32_t mPlatformIndex;
   //! NameId of the sensor producing the track drop.
   //! Only valid for sensor tracks (not track-manager tracks)
   WsfStringId mSensorNameId;
   //! ID of the dropped track
   WsfTrackId mTrackId;
};

//! Notification of a track drop
class WSF_EXPORT WsfXIO_TrackDropPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TrackDropPkt, WsfXIO_ResponsePkt, 12)
   {
      aBuff& mIsRawTrack& mPlatformIndex& mSensorNameId& mTrackId;
   }
   //! true if the dropped track was a raw track, it was a local track otherwise
   bool mIsRawTrack;
   //! Platform sending the track drop
   int32_t mPlatformIndex;
   //! NameId of the sensor producing the track drop.
   //! Only valid for sensor tracks (not track-manager tracks)
   WsfStringId mSensorNameId;
   //! ID of the dropped track
   WsfTrackId mTrackId;
};

//! Requests an application to send back information
class WSF_EXPORT WsfXIO_InformationRequestPkt : public WsfXIO_Packet
{
public:
   enum RequestType
   {
      cPLATFORM_LIST,      // Responds with WsfXIO_PlatformListPkt
      cSCRIPT_LIST,        // Responds with WsfXIO_ScriptListPkt
      cPLATFORM_TYPE_LIST, // Responds with WsfXIO_PlatformTypeListPkt
      cMOVER_PROPERTIES    // Responds with WsfXIO_MoverPropertiesPkt
   };
   XIO_DEFINE_PACKET(WsfXIO_InformationRequestPkt, WsfXIO_Packet, 13)
   {
      aBuff& PakSerialization::Enum(mType) & mPlatformIndex;
   }

   //! Type of information being requested
   RequestType mType;
   //! For cSCRIPT_LIST
   int32_t mPlatformIndex;
};

//! Defines the platforms controlled by an application.
//! This is a possible response to WsfXIO_InformationRequestPkt
class WSF_EXPORT WsfXIO_PlatformListPkt : public WsfXIO_Packet
{
public:
   struct WSF_EXPORT PlatformInfo
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mIndex& mName& mEntityId;
      }
      //! Locally unique platform identifier
      int mIndex;
      //! Name of the platform
      std::string mName;
      //! DIS Entity ID of the platform if available
      WsfXIO_EntityId mEntityId;
   };

   XIO_DEFINE_PACKET(WsfXIO_PlatformListPkt, WsfXIO_Packet, 14) { aBuff& mPlatformData; }

   //! List of platforms modelled by the application
   std::vector<PlatformInfo> mPlatformData;
};

// Packet 15 is no longer assigned and can be reused.
// Packet 16 is no longer assigned and can be reused.
// Packet 17 is no longer assigned and can be reused.
// Packet 19 is no longer assigned and can be reused.
// Packet 19 is no longer assigned and can be reused.
// Packet 20 is no longer assigned and can be reused.

//! Query response for simple or failed queries
class WSF_EXPORT WsfXIO_QueryResolutionPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_QueryResolutionPkt, WsfXIO_Packet, 21) { aBuff& mQueryId& PakSerialization::Enum(mResult); }

   GenUniqueId           mQueryId;
   wsf::xio::QueryResult mResult;
};

class WSF_EXPORT WsfXIO_PlatformCommandChainPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PlatformCommandChainPkt, WsfXIO_ResponsePkt, 22) { aBuff& mCommandChains; }

   std::vector<WsfXIO_CommandChain> mCommandChains;
};

class WSF_EXPORT WsfXIO_PlatformPartInfoPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PlatformPartInfoPkt, WsfXIO_ResponsePkt, 23) { aBuff& mPlatformInfoPtr; }
   void                 ReceiveCleanup() override { delete mPlatformInfoPtr; }
   WsfXIO_PlatformInfo* mPlatformInfoPtr;
};

//! Requests that an application begin sending information for a specified platform
class WSF_EXPORT WsfXIO_RequestPlatformInfoPkt : public WsfXIO_RequestDataPkt
{
public:
   typedef uint32_t InfoType;
   //! Bit mask for various types of information that can be requested.
   enum
   {
      cPLATFORM_INFO      = 0x40000000,
      cCOMMAND_CHAIN_INFO = 0x20000000,
      cENTITY_STATE_INFO  = 0x10000000,
      cROUTE_INFO         = 0x08000000,
      cSENSOR_INFO        = 0x00000001,
      cPROCESSOR_INFO     = 0x00000002,
      cCOMM_INFO          = 0x00000004,
      cMOVER_INFO         = 0x00000008,
      // The PART__TYPE_x bits are assigned for other types of platform parts implemented by extensions.
      cPART_1_INFO     = 0x00000010, // assigned
      cPART_2_INFO     = 0x00000020, // available
      cPART_3_INFO     = 0x00000040, // available
      cPART_4_INFO     = 0x00000080, // available
      cALL_PART_X_INFO = cPART_1_INFO | cPART_2_INFO | cPART_3_INFO | cPART_4_INFO,
      cPART_INFO       = cSENSOR_INFO | cPROCESSOR_INFO | cCOMM_INFO | cMOVER_INFO | cALL_PART_X_INFO,

      cALL_INFO = cCOMMAND_CHAIN_INFO | cPLATFORM_INFO | cPART_INFO
   };
   XIO_DEFINE_PACKET(WsfXIO_RequestPlatformInfoPkt, WsfXIO_RequestDataPkt, 24)
   {
      aBuff& mPlatformIndex& mInfoTypes& mUseDeadReckonDefaults& mDeadReckonInterval& mMaxDeadReckonError& mMaxDeadReckonOrientationError;
   }
   //! Index of the platform from which information is requested
   int32_t mPlatformIndex;
   //! Type of information requested.  A combination of InfoType flags.
   InfoType mInfoTypes;
   //! If 'true', use the default dead-reckon parameters in Ext, may require less overhead
   bool mUseDeadReckonDefaults;
   //! Interval at which dead reckoning is evaluated
   double mDeadReckonInterval;
   //! Maximum positional error before new entity state is sent
   double mMaxDeadReckonError;
   //! Maximum angular orientation error before new entity state is sent
   double mMaxDeadReckonOrientationError;
};

//! Inform other applications about a platform part state change
class WSF_EXPORT WsfXIO_PartStateChangePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PartStateChangePkt, WsfXIO_ResponsePkt, 25)
   {
      aBuff& mPlatformIndex& mPartName& mPartType& mIsTurnedOn& mDebugOn& mQuantityRemaining& mSelectedModes& mCurrentModeName;
   }

   //! Index of the owning platform
   int32_t mPlatformIndex;
   //! NameID of the platform part
   WsfStringId mPartName;
   //! Type of the platform part
   int32_t mPartType;
   //! 'true' if the part is turned on
   bool mIsTurnedOn;
   //! 'true' if debug is on
   bool mDebugOn;
   //! Quantity remaining for objects
   double mQuantityRemaining;
   //! List of selected mode names
   std::vector<WsfStringId> mSelectedModes;
   //! Current mode name
   WsfStringId mCurrentModeName;
};

//! Requests that an application changes either state or mode of a platform part.
class WSF_EXPORT WsfXIO_PartStateChangeRequestPkt : public WsfXIO_Packet
{
public:
   enum StateChange
   {
      cSTATE_COMMAND            = 0x01000000,
      cDEBUG_COMMAND            = 0x02000000,
      cMODE_COMMAND             = 0x03000000,
      cCUE_COMMAND              = 0x04000000,
      cXMTR_RCVR_COMMAND        = 0x05000000,
      cTRACKING_REQUEST_COMMAND = 0x06000000,
      cCOMMAND_MASK             = 0xff000000,
      cNO_CHANGE                = 0,
      cTURN_ON                  = cSTATE_COMMAND | 1,
      cTURN_OFF                 = cSTATE_COMMAND | 2,
      cTURN_DEBUG_ON            = cDEBUG_COMMAND | 1,
      cTURN_DEBUG_OFF           = cDEBUG_COMMAND | 2,
      cDESELECT_MODE            = cMODE_COMMAND | 1,
      cCUE_AZ                   = cCUE_COMMAND | 1,
      cCUE_EL                   = cCUE_COMMAND | 2,
      cCUE_AZ_EL                = cCUE_AZ | cCUE_EL,
      cCUE_WCS                  = cCUE_COMMAND | 4,
      cCHANGE_XMTR              = cXMTR_RCVR_COMMAND | 1,
      cCHANGE_RCVR              = cXMTR_RCVR_COMMAND | 2,
      cXMTR_RCVR_MASK           = cCHANGE_XMTR | cCHANGE_RCVR,
      cSET_POWER                = 0x400,
      cSET_BANDWIDTH            = 0x800,
      cSET_FREQUENCY            = 0x1000,
      cSET_DETECTION_THRESHOLD  = 0x2000,
      cSET_NOISE_FIGURE         = 0x4000,
      cSTART_TRACKING           = cTRACKING_REQUEST_COMMAND | 0x01,
      cSTOP_TRACKING            = cTRACKING_REQUEST_COMMAND | 0x02
   };

   XIO_DEFINE_PACKET(WsfXIO_PartStateChangeRequestPkt, WsfXIO_Packet, 26)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mPartName& mPartType& mStateChange& mSelectMode& mCueAz& mCueEl& mCueWCS& mXmtrIndex&
         mPower& mBandwidth& mFrequency& mDetectionThreshold& mNoiseFigure& mTrackId& mTrackHostIndex;
   }
   //! Index of the owning platform
   int32_t mPlatformIndex;
   //! NameID of the platform part
   WsfStringId mPartName;
   //! Type of the platform part
   int32_t mPartType;
   //! What kind of state change is applied
   int32_t mStateChange;
   //! NameID of the new mode to select or deselect (0 for no change)
   //! or mode to start tracking with
   WsfStringId mSelectMode;
   //! Azimuth to cue (relative to the part), if cCUE_AZ is set
   float mCueAz;
   //! Azimuth to cue (relative to the part), if cCUE_EL is set
   float mCueEl;
   //! Location to cue in WCS, if cCUE_WCS is set.
   UtVec3d mCueWCS;
   //! Index of transmitter that change applies to. If cCHANGE_RCVR is set, change
   //! will apply to linked receiver of this transmitter.
   int32_t mXmtrIndex;
   //! New power to apply to xmtr
   double mPower;
   //! New bandwidth to apply to xmtr/rcvr
   double mBandwidth;
   //! New frequency to apply to xmtr/rcvr
   double mFrequency;
   //! New detection threshold to apply to rcvr
   double mDetectionThreshold;
   //! New noise figure to apply to rcvr
   double mNoiseFigure;
   //! For cSTART_TRACKING or cSTOP_TRACKING
   WsfTrackId mTrackId;
   //! For cSTART_TRACKING or cSTOP_TRACKING, defines the platform index on which the raw track is defined
   //! This may be set to zero for local tracks.
   int32_t mTrackHostIndex;
};

class WSF_EXPORT WsfXIO_ReRoutePlatformPkt : public WsfXIO_Packet
{
public:
   enum RouteType
   {
      cFOLLOW_ROUTE,
      cGO_TO_HEADING,
      cGO_TO_SPEED,
      cGO_TO_ALTITUDE,
      cGO_TO_LOCATION,
      cSET_LOCATION,
      cRETURN_TO_ROUTE,
      cUPDATE_DEFAULT_ROUTE
   };
   typedef WsfXIO_Packet BaseType;
   static const int      cPACKET_ID = 27;
   WsfXIO_ReRoutePlatformPkt(int aPacketId = cPACKET_ID)
      : BaseType(aPacketId)
      , mRateOfChange(0)
      , mKeepRoute(false)
      , mRelativeHeading(false)
      , mRoutePtr(nullptr)
   {
   }
   template<typename T>
   void Serialize(T& aBuff)
   {
      BaseType::Serialize(aBuff);
      using namespace PakSerialization;
      aBuff& mPlatformIndex&                                                       Enum(mType) &
         mHeading& mSpeed& mAltitude& mRateOfChange& mKeepRoute& mRelativeHeading& Array(mLocationWCS, 3) & mRoutePtr;
   }
   void ReceiveCleanup() override { delete mRoutePtr; }
   //! Index of platform to change route
   int32_t mPlatformIndex;
   //! Type of route information
   RouteType mType;
   //! Heading if type is cGO_TO_HEADING
   float mHeading;
   //! Speed if type is cGO_TO_SPEED
   float mSpeed;
   //! Altitude if type is cGO_TO_ALTITUDE
   float mAltitude;
   //! Rate of Change if type is cGO_TO_ALTITUDE || cGO_TO_HEADING || cGO_TO_SPEED
   float mRateOfChange;
   //! Keep Route if type is cGO_TO_ALTITUDE || cGO_TO_SPEED
   bool mKeepRoute;
   //! Use Relative Heading if type is cGO_TO_HEADING
   bool mRelativeHeading;
   //! Location for cSET_LOCATION and cGO_TO_LOCATION
   double mLocationWCS[3];
   //! New route to give platform.  Valid for type cFOLLOW_ROUTE
   WsfRoute* mRoutePtr;
};

// Packet 28 is assigned in WsfXIO_SimPacketRegistry.

class WSF_EXPORT WsfXIO_RequestDisDataPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RequestDisDataPkt, WsfXIO_RequestDataPkt, 29) {}
};

class WSF_EXPORT WsfXIO_DisPlatformInfoPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_DisPlatformInfoPkt, WsfXIO_ResponsePkt, 30)
   {
      aBuff& mPlatformIndex& mName& mType& mSide& mEntityId& mIsExternallyControlled& mIsAlive& mCommanders;
   }
   //! Pair containing (Command chain nameID, Commander nameID)
   typedef std::pair<WsfStringId, WsfStringId> Commander;
   typedef std::vector<Commander>              CommanderList;
   //! Platform Index
   int32_t mPlatformIndex;
   //! NameID of the platform
   WsfStringId mName;
   //! TypeID of the platform
   WsfStringId mType;
   //! SideId of the platform
   WsfStringId mSide;
   //! DIS Id of the platform
   WsfXIO_EntityId mEntityId;
   //! 'true' if the platform is an externally controlled entity
   bool mIsExternallyControlled;
   //! 'true' if the platform is in the simulation
   bool mIsAlive;
   //! List of commanders
   CommanderList mCommanders;
};

// Packet 31 is assigned in WsfXIO_SimPacketRegistry.

//! Request information about current tasks(WsfXIO_TaskInitPkt), future task changes (WsfXIO_TaskUpdatePkt),
//! and optionally override the simulation's task management logic.
class WSF_EXPORT WsfXIO_TaskRequestPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TaskRequestPkt, WsfXIO_RequestDataPkt, 32)
   {
      aBuff& mPlatformIndex& mTaskProcessorName& mTakeControl;
   }
   //! Index of the platform owning the task manager
   int32_t mPlatformIndex;
   //! Name of the task processor
   WsfStringId mTaskProcessorName;
   //! 'true' if the application is requesting full control of the task manager
   //!      i.e. Disabling existing task manager logic
   bool mTakeControl;
};

//! Defines current state of the task manager processor.  Response to WsfXIO_TaskRequestPkt.
class WSF_EXPORT WsfXIO_TaskInitPkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TaskInitPkt, WsfXIO_ResponsePkt, 33) { aBuff& mReceivedTasks& mAssignedTasks; }
   //! List of tasks received by this task manager
   std::vector<WsfTaskData> mReceivedTasks;
   //! List of tasks assigned by this task manager
   std::vector<WsfTaskData> mAssignedTasks;
};

//! Defines a change in a task.  Assignment, cancellation, or completion.  Response to WsfXIO_TaskRequestPkt.
class WSF_EXPORT WsfXIO_TaskUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TaskUpdatePkt, WsfXIO_ResponsePkt, 34)
   {
      aBuff& mUpdateType& mTaskPtr& mTrackPtr& mTaskStatus;
   }
   void ReceiveCleanup() override
   {
      delete mTrackPtr;
      delete mTaskPtr;
   }

   typedef int32_t UpdateType;

   //! Allowed UpdateType values
   enum
   {
      cTASK_ACCEPTED  = 1,
      cTASK_ASSIGNED  = 2,
      cTASK_CANCELED  = 3,
      cTASK_COMPLETED = 4,
      //! The following status types are reserved for extensions.
      cTASK_UPDATE_TYPE_1 = 5,
      cTASK_UPDATE_TYPE_2 = 6,
      cTASK_UPDATE_TYPE_3 = 7,
      cTASK_UPDATE_TYPE_4 = 8
   };

   //! Type of task update
   UpdateType mUpdateType;
   //! The Task
   const WsfTaskData* mTaskPtr;
   //! Track associated with task (valid for cTASK_ASSIGNED & cTASK_ACCEPTED)
   const WsfTrack* mTrackPtr;
   //! Status of task (valid for cTASK_COMPLETED)
   WsfStringId mTaskStatus;
};

//! Requests a task manager processor to create or modify a task.
class WSF_EXPORT WsfXIO_TaskCommandPkt : public WsfXIO_Packet
{
public:
   typedef WsfXIO_Packet BaseType;

   static const int cPACKET_ID = 35;
   WsfXIO_TaskCommandPkt(int aPacketId = cPACKET_ID)
      : BaseType(aPacketId)
      , mResourcePtr(nullptr)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      BaseType::Serialize(aBuff);
      DoSerialize(aBuff);
   }

   template<typename T>
   void DoSerialize(T& aBuff)
   {
      aBuff& mCommandType& mAssignerPlatformIndex&                                               mAssignerProcessorName;
      aBuff& mTrackId& mTargetNameId& mTaskType& mAssigneePlatformIndex& mAssigneeProcessorName& PakSerialization::Polymorphic(
         mResourcePtr);
      aBuff& mCommName& mStatus& mOperatingLevel;
   }

   void ReceiveCleanup() override { delete mResourcePtr; }

   typedef int32_t CommandType;

   //! Valid command types.
   enum
   {
      cTASK_ASSIGN,            //!< Assign a task
      cTASK_CANCEL,            //!< Cancel an assigned task
      cTASK_COMPLETE,          //!< Report a received task as complete
      cTASK_PROGRESS,          //!< Report progress on a received task
      cCHANGE_OPERATING_LEVEL, //!< Change operating level
      // The remainder are reserved for extensions
      cTASK_COMMAND_1, // assigned
      cTASK_COMMAND_2, // available
      cTASK_COMMAND_3, // available
      cTASK_COMMAND_4  // available
   };

   //!{ Note: '[]' denotes optional

   //! Type of task update.  Required.
   CommandType mCommandType;
   //! Platform Index of the assigner platform.  Required.
   unsigned int mAssignerPlatformIndex;
   //! Name of the assigner's task manager processor.  Required.
   WsfStringId mAssignerProcessorName;
   //! The subject track ID.  *Required.
   WsfTrackId mTrackId;
   //! The name of the target platform for this track. Used if mTrackId is not given.
   WsfStringId mTargetNameId;
   //! Type of task.  For cTASK_ASSIGN cTASK_COMPLETE cTASK_PROGRESS cCHANGE_OPERATING_LEVEL [cTASK_CANCEL]
   WsfStringId mTaskType;
   //! Index of the assigned platform.  For cTASK_ASSIGN, cCHANGE_OPERATING_LEVEL.
   //! A value of 0 assumes the assignee is the assigner
   unsigned int mAssigneePlatformIndex;
   //! Name of the processor on the assigned platform.  For cTASK_ASSIGN.
   WsfStringId mAssigneeProcessorName;
   //! Name of the resource to route the task to.  For cTASK_ASSIGN.
   WsfTaskResource* mResourcePtr;
   //! Name of the comm device to use.  For [cTASK_ASSIGN]
   WsfStringId mCommName;
   //! Status message.  For [cTASK_COMPLETE] [cTASK_PROGRESS]
   WsfStringId mStatus;
   //! For only cCHANGE_OPERATING_LEVEL
   int32_t mOperatingLevel;
   //!}
};

class WSF_EXPORT WsfXIO_TypeQueryPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TypeQueryPkt, WsfXIO_Packet, 36)
   {
      using namespace PakSerialization;
      aBuff& mQueryId& Enum(mTypeLocation) & mPartType& mRequestedTypeName& mPlatformName;
   }
   enum TypeLocation
   {
      //! Object is defined as a platform part unassociated with a platform
      cPART_TYPE,
      //! Object is a member of a platform type named by mPlatformName
      cPLATFORM_TYPE_MEMBER,
      //! Object is a member of a live platform named by mPlatformName;
      cPLATFORM_MEMBER,
      //! Object is a platform
      cPLATFORM,
      //! Object is a platform_type
      cPLATFORM_TYPE
   };
   //! Unique ID for the query
   GenUniqueId mQueryId;
   //! Specifies where the type is defined
   TypeLocation mTypeLocation;
   //! Type of platform part when using cPART_TYPE, cPLATFORM_TYPE_MEMBER, or cPLATFORM_MEMBER
   int32_t mPartType;
   //! Name of the requested system
   WsfStringId mRequestedTypeName;
   //! Must be valid for cPLATFORM_TYPE_MEMBER or cPLATFORM_MEMBER
   WsfStringId mPlatformName;
};


class WSF_EXPORT WsfXIO_TypeQueryResolutionPkt : public WsfXIO_QueryResolutionPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TypeQueryResolutionPkt, WsfXIO_QueryResolutionPkt, 37)
   {
      using namespace PakSerialization;
      aBuff& mObjectType& mRequestedType& mObjectInfo;
   }
   void                          ReceiveCleanup() override { delete mObjectInfo; }
   WsfXIO_ObjectInfo::ObjectType mObjectType;
   WsfStringId                   mRequestedType;
   WsfXIO_ObjectInfo*            mObjectInfo;
};

class WSF_EXPORT WsfXIO_TypeListQueryPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TypeListQueryPkt, WsfXIO_Packet, 38)
   {
      using namespace PakSerialization;
      aBuff& mQueryId& mRequestTypes;
   }

   typedef int32_t RequestType;

   //! Valid request types
   enum
   {
      cPLATFORM_TYPES,
      cSENSOR_TYPES,
      cCOMM_TYPES,
      cPROCESSOR_TYPES,
      cFUEL_TYPES,
      cMOVER_TYPES,
      // Request types reserved for extensions
      cREQUEST_TYPE_1, // assigned
      cREQUEST_TYPE_2, // available
      cREQUEST_TYPE_3, // available
      cREQUEST_TYPE_4  // available
   };
   GenUniqueId mQueryId;
   RequestType mRequestTypes;
};

class WSF_EXPORT WsfXIO_TypeListQueryResolutionPkt : public WsfXIO_QueryResolutionPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TypeListQueryResolutionPkt, WsfXIO_QueryResolutionPkt, 39)
   {
      using namespace PakSerialization;
      aBuff& mListType& mTypes;
   }
   WsfXIO_TypeListQueryPkt::RequestType mListType;
   std::vector<WsfStringId>             mTypes;
};

//! Requests messages sent or received by a platform
class WSF_EXPORT WsfXIO_MessageRequestPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_MessageRequestPkt, WsfXIO_RequestDataPkt, 40) { aBuff& mRequestedMessages& mPlatformIndex; }
   enum RequestedMessageType : std::uint16_t
   {
      cRECEIVED_MESSAGES = 0x1,
      cSENT_MESSAGES     = 0x2,
      cALL_MESSAGES      = cRECEIVED_MESSAGES | cSENT_MESSAGES
   };
   //! One of RequestedMessageType
   uint16_t mRequestedMessages;
   //! Index of the platform sending/receiving the message
   std::int32_t mPlatformIndex;
};

// Packet 41 is assigned in WsfXIO_SimPacketRegistry.
// Packet 42 is no longer assigned and can be reused.
// Packet 43 is no longer assigned and can be reused.
// Packet 44 is no longer assigned and can be reused.

class WSF_EXPORT WsfXIO_RequestSimTimePkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RequestSimTimePkt, WsfXIO_RequestDataPkt, 45)
   {
      using namespace PakSerialization;
      aBuff& Enum(mTimerType) & mTimerInterval;
   }

   enum TimerType : std::uint16_t
   {
      //! Specifies receipt of updates based on a simulation time interval
      //! 1.0 s interval / 10x clock rate = 10 updates per second
      cSIM_TIME_INTERVAL,
      //! Specifies receipt of updates based on a simulation time interval adjusted by clock rate
      //! 1.0 s interval / 10x clock rate = 1 update per second
      cADJUSTED_SIM_TIME_INTERVAL,
      //! Specifies receipt of updates only when switching simulation state
      cNO_TIME_INTERVAL,
      //! Specifies that the interval is in real-time seconds.  Useful for obtaining intermittent sim time updates.
      cREAL_TIME_INTERVAL
   };

   TimerType mTimerType;
   //! Requested interval of simulation time updates
   double mTimerInterval;
};

class WSF_EXPORT WsfXIO_SimTimePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SimTimePkt, WsfXIO_ResponsePkt, 46)
   {
      using namespace PakSerialization;
      aBuff& Enum(mState) & mSimTime& mClockRate& mEndTime& mCurrentRunNumber& mFinalRunNumber;
   }

   enum SimulationState
   {
      //! Simulation is executing at a multiple(mClockRate) of realtime
      cREALTIME,
      //! Simulation is executing at non-realtime
      cNON_REALTIME,
      //! Simulation is paused
      cPAUSED,
      //! Simulation is complete
      cCOMPLETE
   };

   SimulationState mState;
   double          mSimTime;
   double          mClockRate;
   double          mEndTime;
   int32_t         mCurrentRunNumber;
   int32_t         mFinalRunNumber;
};

class WSF_EXPORT WsfXIO_EntityStateData
{
public:
   template<typename AR>
   void SerializeState(AR& aBuff)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mSimTime& Array(mLocation, 3) & Array(mVelocity, 3);
      aBuff&                           Array(mAcceleration, 3) & Array(mOrientation, 3);
   }
   int    mPlatformIndex;
   double mSimTime;
   double mLocation[3];
   double mVelocity[3];
   double mAcceleration[3];
   double mOrientation[3];
};

class WSF_EXPORT WsfXIO_EntityStatePkt : public WsfXIO_EntityStateData, public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_EntityStatePkt, WsfXIO_ResponsePkt, 47) { WsfXIO_EntityStateData::SerializeState(aBuff); }
};


class WSF_EXPORT WsfXIO_SubscriptionPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SubscriptionPkt, WsfXIO_Packet, 48) { aBuff& mCancel& mSubscriptionIds& mKeyFilters; }

   bool                              mCancel;
   std::vector<GenUniqueId>          mSubscriptionIds;
   std::vector<WsfXIO_PublishFilter> mKeyFilters;
};

class WSF_EXPORT WsfXIO_PublicationPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PublicationPkt, WsfXIO_Packet, 49) { aBuff& mSubscriberIds& mKey& mData; }

   std::vector<GenUniqueId> mSubscriberIds; // If empty, deliver to all.
   WsfXIO_PublishKey        mKey;
   std::vector<char>        mData;
};

class WSF_EXPORT WsfXIO_SimTimeCommandPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SimTimeCommandPkt, WsfXIO_Packet, 50) { aBuff& mCommandType& mSimTime; }
   enum CommandType
   {
      //! Sets a maximum bound in simulation time that the simulation will execute.
      //! Upon reaching the bound, the simulation will wait.
      cSET_MAX_SIM_TIME,
      //! Currently unused.  Use WsfXIO_TimeSynchronization.
      cREQUEST_TIME_MANAGEMENT,
      //! Change the simulation's clock rate
      cSET_CLOCK_RATE,
      //! Pause the simulation
      cPAUSE,
      //! Resume the paused simulation
      cRESUME,
      //! Advance the simulation by mSimTime (seconds)
      cADVANCE_TIME,
      //! Sets the end-time of the simulation.
      cSET_END_TIME,
      //! Commands the simulation to execute in non-realtime
      cSET_NON_REALTIME,
      //! Commands the simulation to execute in realtime
      cSET_REALTIME
   };

   int32_t mCommandType;
   double  mSimTime;
};

//! Provides a more generic way to pass data between a request and a service session
class WSF_EXPORT WsfXIO_DataResponsePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_DataResponsePkt, WsfXIO_ResponsePkt, 51) { aBuff& mData; }
   WsfXIO_Data mData;
};

//! Requests that an application changes the state of a track manager.
class WSF_EXPORT WsfXIO_TrackManagerStateChangeRequestPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TrackManagerStateChangeRequestPkt, WsfXIO_Packet, 52)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mCorrelationStrategy& mFusionStrategy;
   }
   bool IsCorrelationStrategyChanged() const { return mCorrelationStrategy != 0; }
   bool IsFusionStrategyChanged() const { return mFusionStrategy != 0; }

   int32_t     mPlatformIndex;
   WsfStringId mCorrelationStrategy;
   WsfStringId mFusionStrategy;
};

//! Sent after an application changes the state of a track manager.
class WSF_EXPORT WsfXIO_TrackManagerStateChangePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_TrackManagerStateChangePkt, WsfXIO_ResponsePkt, 53)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mCorrelationStrategy& mFusionStrategy;
   }
   int32_t     mPlatformIndex;
   WsfStringId mCorrelationStrategy;
   WsfStringId mFusionStrategy;
};

// Packet 54 is no longer assigned and can be reused.

//! Requests and responds to requests of service availability.  Communicates the services
//! available by an application at a finer grained detail than the application type.
class WSF_EXPORT WsfXIO_ServiceAvailableQuery : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_ServiceAvailableQuery, WsfXIO_Packet, 55)
   {
      aBuff& mPacketType& mServiceType& mVersionInfo;
   }
   enum PacketType
   {
      cREQUEST               = 0,
      cRESPOND_AVAILABLE     = 1,
      cRESPOND_NOT_AVAILABLE = 2
   };
   int         mPacketType;
   std::string mServiceType;
   //! Optional version information supplied with cRESPOND_AVAILABLE
   std::string mVersionInfo;
};


class WSF_EXPORT WsfXIO_EntityStateRawPkt : public WsfXIO_EntityStateData, public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_EntityStateRawPkt, WsfXIO_Packet, 56) { WsfXIO_EntityStateData::SerializeState(aBuff); }
};

class WSF_EXPORT WsfXIO_RouteUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RouteUpdatePkt, WsfXIO_ResponsePkt, 57)
   {
      aBuff& mPlatformIndex& mRoutePtr& mTargetWaypointIndex;
   }
   void ReceiveCleanup() override { delete mRoutePtr; }

   //! Index of the platform whose route is updated
   int32_t mPlatformIndex;
   //! The updated route
   WsfRoute* mRoutePtr;
   //! Target waypoint index
   int mTargetWaypointIndex;
};

//! Requests that an application begin sending state information for a specified platform part
class WSF_EXPORT WsfXIO_RequestPlatformPartInfoPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RequestPlatformPartInfoPkt, WsfXIO_RequestDataPkt, 58)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mPartType& mPartName& mUpdateInterval& mRealtimeInterval;
   }
   //! Index of the platform from which information is requested
   int32_t mPlatformIndex;
   //! Type of platform part
   int32_t mPartType;
   //! Name of platform part.
   WsfStringId mPartName;
   //! Update rate of state updates
   double mUpdateInterval;
   //! Update-rate is in real-time
   bool mRealtimeInterval;
};

//! Requests that an application begin sending state information for a specified platform part
class WSF_EXPORT WsfXIO_PlatformPartStatePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PlatformPartStatePkt, WsfXIO_Packet, 59)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mPartType& mPartName& mYaw& mPitch& mRoll& mIsExternallyControlled& mTransmitters& mReceivers;
   }
   void ReceiveCleanup() override
   {
      for (size_t i = 0; i < mTransmitters.size(); ++i)
      {
         delete mTransmitters[i];
      }
      for (size_t i = 0; i < mReceivers.size(); ++i)
      {
         delete mReceivers[i];
      }
   }

   //! Index of the platform from which information is requested
   int32_t mPlatformIndex;
   //! Type of platform part
   int32_t mPartType;
   //! Name of platform part.
   WsfStringId mPartName;
   //! Orientation
   double mYaw;
   double mPitch;
   double mRoll;
   //! Is platform part externally controlled
   bool mIsExternallyControlled;
   //! List of active transmitters and receivers
   std::vector<WsfXIO_EM_XmtrInfo*> mTransmitters;
   std::vector<WsfXIO_EM_RcvrInfo*> mReceivers;
};

//! Requests that an application send a list of valid scripts for a given platform or a global script listing
class WSF_EXPORT WsfXIO_ScriptListPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_ScriptListPkt, WsfXIO_Packet, 60)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mScripts;
   }
   struct WSF_EXPORT Argument
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mArgumentType& mArgumentName;
      }
      std::string mArgumentType;
      std::string mArgumentName;
   };
   struct WSF_EXPORT Script
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         using namespace PakSerialization;
         aBuff& mName& mArguments& mPartName& mPartType;
      }
      std::string           mName;
      std::vector<Argument> mArguments;

      std::string mPartName;
      int32_t     mPartType;
   };
   //! Index of the platform from which information is requested
   //! 0 for global script list
   int32_t             mPlatformIndex = 0;
   std::vector<Script> mScripts;
};


//! Contains information about objects being drawn with WsfDraw
class WSF_EXPORT WsfXIO_DrawPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_DrawPkt, WsfXIO_Packet, 62) { aBuff& mDictionary& mRemovedIds& mEntities; }
   enum ElementType
   {
      cLINE,
      cPOINT,
      cICON,
      cELLIPSE,
      cNONE, // this corresponds to WsfDraw::cERASE
      cELLIPSOID,
      cQUADRILATERAL,
      cTEXT,
      cTIMER
   };
   enum VertexType
   {
      cUNSET_VERTEX,
      cABSOLUTE_WCS,
      cRELATIVE_ZERO,
      cRELATIVE_ECS,
      cRELATIVE_NED,
      cABSOLUTE_SCREEN,
   };

   struct WSF_EXPORT Vertex
   {
      template<typename AR>
      void Serialize(AR& aAr)
      {
         aAr& mVertexType& mRelativePlatformIndex;
         if (mVertexType != cRELATIVE_ZERO)
         {
            aAr& mPosition;
         }
      }
      int8_t  mVertexType;
      int32_t mRelativePlatformIndex;
      UtVec3d mPosition;
   };
   struct WSF_EXPORT Entity
   {
      template<typename AR>
      void Serialize(AR& aAr)
      {
         aAr& mDrawType& mId& PakSerialization::Array(mColor, 4) & mDuration& mLayerName;
         if (mDrawType == cLINE)
         {
            aAr& mVerts[0] & mVerts[1] & mLineSize& mLineStyle;
         }
         else if (mDrawType == cPOINT)
         {
            aAr& mVerts[0] & mPointSize;
         }
         else if (mDrawType == cICON)
         {
            aAr& mVerts[0] & mHeading& mIconName;
         }
         else if (mDrawType == cELLIPSE)
         {
            aAr& mVerts[0] & mHeading& mLineSize& mLineStyle& mEllipseMode& mAxisA& mAxisB;
         }
         else if (mDrawType == cELLIPSOID)
         {
            aAr& mVerts[0] & mHeading& mPitch& mRoll& mLineSize& mLineStyle& mEllipseMode& mAxisA& mAxisB& mAxisC;
         }
         else if (mDrawType == cTEXT)
         {
            aAr& mVerts[0] & mTextSize& mText;
         }
         else if (mDrawType == cTIMER)
         {
            aAr& mVerts[0] & mPointSize;
         }
      }
      int32_t         mDrawType;
      uint32_t        mId;
      float           mDuration;
      WsfXIO_StringId mLayerName;
      // std::string mLayerName;
      WsfXIO_StringId mText;
      uint8_t         mColor[4];
      uint8_t         mLineSize;
      uint8_t         mLineStyle;
      uint8_t         mPointSize;
      uint8_t         mTextSize;
      Vertex          mVerts[2];
      float           mHeading;
      float           mPitch;
      float           mRoll;
      int             mEllipseMode;
      float           mAxisA;
      float           mAxisB;
      float           mAxisC;
      WsfXIO_StringId mIconName;
      // std::string mIconName;
   };
   WsfXIO_StringDictionary mDictionary;
   // shape-id, layer
   std::vector<std::pair<uint32_t, WsfXIO_StringId>> mRemovedIds;
   std::vector<Entity>                               mEntities;
};

//! Contains a list of data about the simulation's platform types.
class WSF_EXPORT WsfXIO_PlatformTypeListPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_PlatformTypeListPkt, WsfXIO_Packet, 63) { aBuff& mPlatformTypes; }
   struct WSF_EXPORT PlatformTypeInfo
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mTypeName& mIcon;
      }
      WsfStringId mTypeName;
      std::string mIcon;
   };

   std::vector<PlatformTypeInfo> mPlatformTypes;
};

//! Used by the IWARS control panel to request the tracks of contributing sensors.
class WSF_EXPORT WsfXIO_ExtendedTrackInfoRequestPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_ExtendedTrackInfoRequestPkt, WsfXIO_Packet, 64) { aBuff& mQueryId& mTrackId; }

   //! A unique ID query Id for the request.
   GenUniqueId mQueryId;

   //! The track ID of the local track whose contributing sensor tracks are being requested.
   //! The track must be in the master local track list of the platform whose name is
   //! is given by the 'Owner Platform Id' field of the track ID.
   WsfTrackId mTrackId;
};

//! The response from a WsfXIO_ExtendedTrackInfoRequestPkt.
class WSF_EXPORT WsfXIO_ExtendedTrackInfoPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_ExtendedTrackInfoPkt, WsfXIO_Packet, 65)
   {
      aBuff& mQueryId& mTrackId& mLocalTrack& mTracks;
   }
   void ReceiveCleanup() override
   {
      for (size_t i = 0; i < mTracks.size(); ++i)
      {
         delete mTracks[i];
         mTracks[i] = nullptr;
      }
   }

   //! The unique query ID provided in the associated request packet.
   GenUniqueId mQueryId;

   //! The track ID of the local track whose contributing sensor tracks are being provided.
   //! This is same as the value provided in the associated request packet.
   WsfTrackId mTrackId;

   //! The local track about which contributors were requested.
   WsfTrack* mLocalTrack;

   //! The sensor tracks that are contributing to the specified local track.
   std::vector<WsfTrack*> mTracks;
};

class WSF_EXPORT WsfXIO_RequestSensorDataPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RequestSensorDataPkt, WsfXIO_RequestDataPkt, 66)
   {
      aBuff& mPlatformIndex& mSensorNameId& mRequiredData;
   }
   enum SensorData
   {
      cTRACKING_REQUESTS = 0x1
   };

   //! Index of the platform which owns the sensor
   int32_t mPlatformIndex;
   //! Name of the sensor whose tracks are requested
   WsfStringId mSensorNameId;
   //! Bit combination of required data
   int32_t mRequiredData;
};

class WSF_EXPORT WsfXIO_SensorTrackingRequestUpdatePkt : public WsfXIO_ResponsePkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SensorTrackingRequestUpdatePkt, WsfXIO_ResponsePkt, 67)
   {
      aBuff& mPlatformIndex& mSensorNameId& mClear& mAddedRequests& mRemovedRequests;
   }
   struct WSF_EXPORT TrackingRequest
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mTrackId& mModeIndex;
      }
      WsfTrackId mTrackId;
      int        mModeIndex;
   };

   //! Index of the platform which owns the sensor
   int32_t mPlatformIndex;
   //! Name of the sensor whose tracks are requested
   WsfStringId mSensorNameId;

   //! True if the tracking request list has been cleared.
   bool                         mClear;
   std::vector<TrackingRequest> mAddedRequests;
   std::vector<WsfTrackId>      mRemovedRequests;
};


class WSF_EXPORT WsfXIO_RequestScriptDataPkt : public WsfXIO_RequestDataPkt
{
public:
   enum InformationType
   {
      cCONTEXT_INFO, // Results in WsfXIO_RequestScriptDataResponsePkt
      cCONTEXT_VARIABLES,
      cSCOPE_DETAILS,
      cVARIABLE_QUERY,
      cSCRIPT_DETAILS,
      cGET_PLATFORM_CONTEXT,
      cBREAKPOINT_LISTING,
      cCALLSTACK_LISTING
   };
   struct VariableQuery
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mContext& mStackFrame& mExpressionReferenceNumber& mVariableName& mEntryAddresses;
      }

      VariableQuery()
         : mStackFrame(-2)
         , mExpressionReferenceNumber(0)
      {
      }
      WsfXIO_EncodedPointer mContext;    // null for selecting a stack frame
      int32_t               mStackFrame; // Choose a stack frame as context.
      // -1 may be used to indicate 'top' frame.
      // -2 indicates no stack frame -- allows lookup of instance variables only
      // -3 indicates a request for a 'global_enumerate' script
      int32_t     mExpressionReferenceNumber; // If non-zero, request for a previously executed expression
      std::string mVariableName;
      // The address of the entry to query
      // A script value may have a nested entry structure
      // for instance:  Array<int>  will have a child entry for each value in the array
      //                Map<int,int> has an entry for each key,value pair
      //                   and the key value pair entry has two entries (the key and the value)
      // std::vector<int>        mEntryAddress;
      std::vector<std::vector<int32_t>> mEntryAddresses;
   };

   XIO_DEFINE_PACKET(WsfXIO_RequestScriptDataPkt, WsfXIO_RequestDataPkt, 68)
   {
      aBuff& mInformationType& mScriptContext& mScope& mScript& mVariableQueries;
   }
   int32_t               mInformationType;
   int32_t               mPlatformIndex;
   WsfXIO_EncodedPointer mScriptContext; // for cCONTEXT_INFO
   WsfXIO_EncodedPointer mScope;         // for cSCOPE_DETAILS
   WsfXIO_EncodedPointer mScript;        // for cSCRIPT_DETAILS
   // std::pair<int, std::string> mQueryVariables;    // for cVARIABLE_QUERY
   std::vector<VariableQuery> mVariableQueries; // for cVARIABLE_QUERY
};

class WSF_EXPORT WsfXIO_RequestScriptDataResponsePkt : public WsfXIO_ResponsePkt
{
public:
   struct VariableValue
   {
      VariableValue()
         : mEntryCount(0)
      {
      }
      ~VariableValue()
      {
         // for (size_t i = 0; i < mSubEntries.size(); ++i)
         //{
         // delete mSubEntries[i];
         // }
      }
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mEntryName& mValueText& mTypeName& mEntryCount; // & mSubEntries;
      }
      bool IsNull() const { return mValueText.empty() && mTypeName.empty() && mEntryCount == 0; }
      // This is empty for most values.  May be set to indicate the name of an attribute.
      std::string mEntryName;
      // Either a complete representation of the variable value
      // or an abbreviated form
      std::string mValueText;
      // Type of variable.  Empty if this value is not valid.
      // May be the special value "MAP_ITERATOR" for key/value pair entries in a map
      std::string mTypeName;
      // Number of child entries available.
      int mEntryCount;
      //// If the query resulted in an expression evaluation,
      //// this can be used later for querying more sub-entries
      //// will be 0 if no expression was evaluated
      // int                           mExpressionReferenceNumber;
      //  List of entry values returned if mExpandEntryDepth was > 0
      // std::vector<VariableValue*>   mSubEntries;
   };

   struct VariableQueryResult
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mValues;
      }
      std::vector<VariableValue> mValues;
   };

   XIO_DEFINE_PACKET_CTOR(WsfXIO_RequestScriptDataResponsePkt, WsfXIO_ResponsePkt, 69)
   {
      mCompletesRequest = true;
      mScriptDetailsPtr = nullptr;
      mBreakpoints      = nullptr;
      mCallstackData    = nullptr;
   }
   XIO_DEFINE_PACKET_SERIALIZE()
   {
      aBuff& mContexts& mScopeData& mContextVariables& mVariableValues& mScriptDetailsPtr& mBreakpoints& mCallstackData;
   }
   void ReceiveCleanup() override
   {
      for (size_t i = 0; i < mVariableValues.size(); ++i)
      {
         delete mVariableValues[i];
      }
      delete mBreakpoints;
      delete mCallstackData;
   }

   struct ScriptContextData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mContextName& mScriptContext& mParentContext& mScope;
      }
      std::string           mContextName;
      WsfXIO_EncodedPointer mScriptContext;
      WsfXIO_EncodedPointer mParentContext;
      WsfXIO_EncodedPointer mScope;
   };
   struct ScriptScopeData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mParentScope& mScripts;
      }
      WsfXIO_EncodedPointer mParentScope;

      std::vector<WsfXIO_EncodedPointer> mScripts;
   };
   struct ContextVariableData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mNames;
      }
      std::vector<std::string> mNames;
   };
   struct ScriptDetails
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mScriptName& mPrototype& mArgNames& mLocalVariableNames& mStaticVariableCount& mSourceFilePath& mFirstLine& mLastLine;
      }
      std::string              mScriptName;
      std::string              mPrototype;
      std::vector<std::string> mArgNames;
      std::vector<std::string> mLocalVariableNames;
      size_t                   mStaticVariableCount;
      std::string              mSourceFilePath;
      size_t                   mFirstLine;
      size_t                   mLastLine;
   };

   struct BreakpointData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mScriptId& mFilePath& mLineNumber;
      }
      WsfXIO_EncodedPointer mScriptId;
      std::string           mFilePath;
      size_t                mLineNumber;
   };

   struct CallstackData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mCallstackEntries;
      }
      std::vector<BreakpointData> mCallstackEntries;
   };

   std::vector<ScriptContextData> mContexts;
   ScriptScopeData                mScopeData;
   ContextVariableData            mContextVariables;
   //! List of variable values requested from cVARIABLE_QUERY
   //! An entry will be null if that variable does not exist
   // std::vector<VariableValue*>      mVariableValues;
   std::vector<VariableQueryResult*> mVariableValues;
   ScriptDetails*                    mScriptDetailsPtr;
   std::vector<BreakpointData>*      mBreakpoints;
   CallstackData*                    mCallstackData;
};


class WSF_EXPORT WsfXIO_ScriptCommandPkt : public WsfXIO_Packet
{
public:
   enum CommandType
   {
      cSET_BREAKPOINT,
      cCLEAR_BREAKPOINT,
      cRESUME,
      cSTEP_INTO,
      cSTEP_OVER,
      cSTEP_OUT
   };
   XIO_DEFINE_PACKET(WsfXIO_ScriptCommandPkt, WsfXIO_Packet, 70)
   {
      aBuff& mCommandType& mLineNumber& mBreakpointId& mFilePath& mCondition;
   }

   int         mCommandType;
   size_t      mLineNumber;   // for cSET_BREAKPOINT
   int         mBreakpointId; // for cSET_BREAKPOINT and cCLEAR_BREAKPOINT [client defined]
   std::string mFilePath;     // for cSET_BREAKPOINT
   std::string mCondition;    // for cSET_BREAKPOINT
};

class WSF_EXPORT WsfXIO_RequestScriptSessionPkt : public WsfXIO_RequestDataPkt
{
public:
   XIO_DEFINE_PACKET(WsfXIO_RequestScriptSessionPkt, WsfXIO_RequestDataPkt, 71) {}
};

class WSF_EXPORT WsfXIO_RequestScriptSessionResponsePkt : public WsfXIO_ResponsePkt
{
public:
   enum MessageType
   {
      cRUNNING,
      cPAUSED,        // Paused due to a step or some other command
      cAT_BREAKPOINT, // Paused due to a breakpoint
      cAT_EXCEPTION,  // Paused due to an exception
      cBREAKPOINT_ADDED
   };
   XIO_DEFINE_PACKET_CTOR(WsfXIO_RequestScriptSessionResponsePkt, WsfXIO_ResponsePkt, 72) { mInitialData = nullptr; }
   XIO_DEFINE_PACKET_SERIALIZE()
   {
      aBuff&                                                                         PakSerialization::Enum(mState) &
         mLineNumber& mFilePath& mBreakpointId& mExceptionDescription& mInitialData& mSimTime;
   }

   void ReceiveCleanup() override { delete mInitialData; }

   MessageType mState;
   // if cBREAKPOINT_ADDED || cPAUSED || cAT_BREAK, this is the location
   size_t      mLineNumber;
   std::string mFilePath;
   int         mBreakpointId; // For cBREAKPOINT_ADDED
   std::string mExceptionDescription;
   double      mSimTime;
   struct InitialData
   {
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mGlobalEnumerateScripts;
      }
      // The list of global enumeration scripts available
      std::vector<std::string> mGlobalEnumerateScripts;
   };
   // The first response will fill this value with some general information about the system
   InitialData* mInitialData;
};
#endif
