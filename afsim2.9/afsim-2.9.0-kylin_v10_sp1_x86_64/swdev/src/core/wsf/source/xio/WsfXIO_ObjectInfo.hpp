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

#ifndef WSFXIO_OBJECTINFO_HPP
#define WSFXIO_OBJECTINFO_HPP

#include "wsf_export.h"

#include <cassert>
#include <vector>

#include "GenUniqueId.hpp"
#include "PakIntTypes.hpp"
#include "PakSerializeFwd.hpp"
#include "UtEntity.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfPath.hpp"
#include "WsfPathConstraints.hpp"
class WsfRoute;
#include "WsfStringId.hpp"
#include "WsfTypes.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Defines.hpp"
class WsfXIO_PlatformPartStatePkt;
class WsfXIO_PartStateChangePkt;
class WsfXIO_PartStateChangeRequestPkt;
class WsfXIO_PlatformInfo;
#include "xio/WsfXIO_PlatformId.hpp"
class WsfXIO_RemotePlatformList;
#include "xio/WsfXIO_SerializeTypes.hpp"

// =================================================================================================
//! This file defines several XIO objects that parallel their WSF counter parts.
//!
//! A WSF/XIO simulation will populate these objects (see xio_sim/WsfXIO_ObjectInfoLoad) and
//! transmit them to a requestor. A requestor need not be a WSF simulation. They can receive
//! these objects and not be required to utilize WSF objects.
// =================================================================================================

//! Base class for extensions that need to store extra data on a standard object type.
//! The standard object type must have a member to store extra data.
//!
//! The derived class MUST register itself using:
//!   PakTypeDictionary::GetInstance().RegisterType<T>(++typeId);
class WSF_EXPORT WsfXIO_ExtraInfo
{
public:
   WsfXIO_ExtraInfo()          = default;
   virtual ~WsfXIO_ExtraInfo() = default;

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mDummy;
   }
   bool mDummy;
};

//! XIO representation of WsfEM_Antenna.
class WSF_EXPORT WsfXIO_AntennaInfo
{
public:
   template<typename T>
   void Serialize(T& aBuff)
   {
      using namespace PakSerialization;
      aBuff& mPitch& mEBS_AzCosSteeringLimit& mEBS_ElCosSteeringLimit& mEBS_AzLossExponent& mEBS_ElLossExponent& Enum(
         mEBS_Mode) &
         Enum(mScanMode) &
         mMinAzScan& mMaxAzScan& mMinElScan& mMaxElScan& mMinAzFOV& mMaxAzFOV& mMinElFOV& mMaxElFOV& mMinRange&
            mMaxRange& mMinAlt& mMaxAlt;
   }

   //! The pitch (tilt) angle of the antenna relative to the articulated part.
   double mPitch;
   //! The cosine of the maximum electronic beam steering angle in azimuth.
   double mEBS_AzCosSteeringLimit;
   //! The cosine of the maximum electronic beam steering angle in elevation.
   double mEBS_ElCosSteeringLimit;
   //! The azimuth exponent 'n' of cos(x)^n for computing electronic beam steering losses.
   double mEBS_AzLossExponent;
   //! The elevation exponent 'n' of cos(x)^n for computing electronic beam steering losses.
   double mEBS_ElLossExponent;
   //! The degrees-of-freedom for electronic beam steering.
   WsfEM_Antenna::EBS_Mode mEBS_Mode;
   //! @name Scan limits with respect to the current cue.
   //@{
   WsfEM_Antenna::ScanMode mScanMode;
   double                  mMinAzScan; // radians
   double                  mMaxAzScan; // radians
   double                  mMinElScan; // radians
   double                  mMaxElScan; // radians
   //@}
   //! @name Field of view with respect current cue.
   //! This should encompass the scan limits PLUS whatever the beam pattern might pick up
   //! when at the physical limits of the scan pattern.
   //@{
   double mMinAzFOV; // radians
   double mMaxAzFOV; // radians
   double mMinElFOV; // radians
   double mMaxElFOV; // radians
   double mMinRange; // meters
   double mMaxRange; // meters
   double mMinAlt;   // meters
   double mMaxAlt;   // meters
                     //@}
};

//! XIO representation of Wsf_EM_XmtrRcvr.
class WSF_EXPORT WsfXIO_EM_XmtrRcvrInfo
{
public:
   WsfXIO_EM_XmtrRcvrInfo() = default;
   ~WsfXIO_EM_XmtrRcvrInfo();

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mBeamTilt& mAntennaPtr& PakSerialization::Polymorphic(mExtraPtr);
   }

   double              mBeamTilt;
   WsfXIO_AntennaInfo* mAntennaPtr{nullptr};
   WsfXIO_ExtraInfo*   mExtraPtr{nullptr};
};

//! XIO representation of a WsfEM_Rcvr
class WSF_EXPORT WsfXIO_EM_RcvrInfo : public WsfXIO_EM_XmtrRcvrInfo
{
public:
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_EM_XmtrRcvrInfo::Serialize(aBuff);
      aBuff& mFrequency& mBandwidth& mPeakAntennaGain& mDetectionThreshold& mInternalLoss;
      aBuff& mNoiseFigure&                                                  mNoisePower;
   }
   double mFrequency;
   double mBandwidth;
   double mPeakAntennaGain;
   double mDetectionThreshold;
   double mInternalLoss;
   double mNoiseFigure;
   double mNoisePower;
};

//! XIO representation of a WsfEM_Xmtr
class WSF_EXPORT WsfXIO_EM_XmtrInfo : public WsfXIO_EM_XmtrRcvrInfo
{
public:
   WsfXIO_EM_XmtrInfo() = default;
   ~WsfXIO_EM_XmtrInfo();

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_EM_XmtrRcvrInfo::Serialize(aBuff);
      aBuff& mFrequency& mBandwidth&                            mPower;
      aBuff& mPulseCompressionRatio& mPulseRepetitionFrequency& mPulseRepetitionInterval;
      aBuff& mPulseWidth& mInternalLoss& mPeakAntennaGain&      mLinkedRcvrPtr;
      aBuff&                                                    mUsePeakPower;
   }

   double mFrequency;
   double mBandwidth;
   double mPower;
   double mPulseCompressionRatio;
   double mPulseRepetitionFrequency;
   double mPulseRepetitionInterval;
   double mPulseWidth;
   double mInternalLoss;
   double mPeakAntennaGain;
   bool   mUsePeakPower;

   WsfXIO_EM_RcvrInfo* mLinkedRcvrPtr{nullptr};
};

// XIO representation of a WsfObject.
class WSF_EXPORT WsfXIO_ObjectInfo
{
public:
   // ObjectType used to be an enum, but it needs to be an int because it is based on component role.
   using ObjectType = int32_t;

   enum
   {
      cPLATFORM_PART    = 0x40000000,
      cARTICULATED_PART = cPLATFORM_PART | 0x20000000,
      // Allows one to isolate the WSF component role for platform/articulated parts.
      // It is 23 bits wide, so the role must be less than 8388608 (2^23).
      cCOMPONENT_ROLE_MASK = 0x007fffff,
      // Picks up the general type of a platform part.
      cPART_TYPE_MASK = 0x607fffff,
      // The 'DERIVED_TYPE' field allows one to uniquely identify a particular type of part within a
      // category of platform parts. For instance, one could uniquely identify a particular implementation
      // of a sensor. It is 6 bits wide so it allows for 64 values [0..63]
      cDERIVED_TYPE_MASK  = 0x1f800000,
      cDERIVED_TYPE_BEGIN = 0x00800000, // The first non-zero value
      // Start of the
      cNO_OBJECT      = 0,
      cPLATFORM       = cWSF_COMPONENT_PLATFORM,
      cTRACK_MANAGER  = cWSF_COMPONENT_TRACK_MANAGER,
      cMOVER_PART     = cPLATFORM_PART | cWSF_COMPONENT_MOVER,
      cCOMM_PART      = cARTICULATED_PART | cWSF_COMPONENT_COMM,
      cPROCESSOR_PART = cPLATFORM_PART | cWSF_COMPONENT_PROCESSOR,
      cSENSOR_PART    = cARTICULATED_PART | cWSF_COMPONENT_SENSOR
      // XIO components may add additional objects.
   };

   WsfXIO_ObjectInfo(ObjectType aObjectType)
      : mObjectType(aObjectType)
   {
   }

   bool       IsPlatform() const { return mObjectType == cPLATFORM; }
   bool       IsTrackManager() const { return mObjectType == cTRACK_MANAGER; }
   bool       IsPlatformPart() const { return (mObjectType & cPLATFORM_PART) != 0; }
   bool       IsArticulatedPart() const { return (mObjectType & cARTICULATED_PART) != 0; }
   bool       IsSensor() const { return (mObjectType & cPART_TYPE_MASK) == cSENSOR_PART; }
   bool       IsComm() const { return (mObjectType & cPART_TYPE_MASK) == cCOMM_PART; }
   bool       IsProcessor() const { return (mObjectType & cPART_TYPE_MASK) == cPROCESSOR_PART; }
   bool       IsMover() const { return (mObjectType & cPART_TYPE_MASK) == cMOVER_PART; }
   ObjectType GetObjectType() const { return mObjectType; }

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mObjectType;
   }

private:
   WsfXIO_ObjectInfo& operator=(const WsfXIO_ObjectInfo& aRhs) = delete;
   WsfXIO_ObjectInfo(const WsfXIO_ObjectInfo& aRhs);

   //! The type of platform part info, equal to cPART_INFO_ID
   ObjectType mObjectType;
};

class WSF_EXPORT WsfXIO_PlatformPartInfo : public WsfXIO_ObjectInfo
{
public:
   static const ObjectType cPART_INFO_ID = cPLATFORM_PART;

   WsfXIO_PlatformPartInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_ObjectInfo(aType)
   {
   }
   virtual ~WsfXIO_PlatformPartInfo();

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ObjectInfo::Serialize(aBuff);
      aBuff& mPartType& mPlatformIndex& mName& mType&              mBasicType;
      aBuff& mIsTurnedOn& mIsOperational& mIsExternallyControlled& mDebugEnabled;
   }

   void         InitStateChange(WsfXIO_PartStateChangeRequestPkt& aPkt);
   virtual void UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt);
   virtual void UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt);

   //! Type of platform part
   int GetPartType() const { return mPartType; }
   //! Index of the platform
   size_t GetPlatformIndex() const { return mPlatformIndex; }
   //! NameID of the platform part
   WsfStringId GetName() const { return mName; }
   //! TypeID of the platform part
   WsfStringId GetInstantiationType() const { return mType; }
   //! Most basic type of the platform part.  ex: 'WSF_RADAR_SENSOR' or 'WSF_TASK_PROCESSOR'
   WsfStringId GetBasicType() const { return mBasicType; }
   //! 'true' if the platform part is turned on
   bool IsTurnedOn() const { return mIsTurnedOn; }
   //! Notify the platform host to turn this platform part on.
   void TurnOn(bool aTurnOn = true);
   //! Notify the platform host to turn this platform part off.
   void TurnOff();
   //! Change debug state
   void SetDebugEnabled(bool aEnabled);

   //! 'true' if the platform part is operational
   bool IsOperational() const { return mIsOperational; }

   //! 'true' if the platform part is externally controlled
   bool IsExternallyControlled() const { return mIsExternallyControlled; }

   //! Set owning platform pointer.  Use with caution.
   void SetPlatformPtr(WsfXIO_PlatformInfo* aPlatformPtr) { mPlatformPtr = aPlatformPtr; }

   //! Get owning platform pointer.
   WsfXIO_PlatformInfo* GetPlatform() const { return mPlatformPtr; }

   void SetTurnedOnFlag(const bool aTurnedOn) { mIsTurnedOn = aTurnedOn; }

   friend class WsfXIO_RemotePlatformList;
   friend struct RequestStateChangeFunctor;

   int         mPartType;
   size_t      mPlatformIndex;
   WsfStringId mName;
   WsfStringId mType;
   WsfStringId mBasicType;
   bool        mIsTurnedOn;
   bool        mIsOperational;
   bool        mIsExternallyControlled;
   bool        mDebugEnabled;

protected:
   WsfXIO_PlatformInfo* mPlatformPtr;
};

//! Contains information about an articulated part
class WSF_EXPORT WsfXIO_ArticulatedPartInfo : public WsfXIO_PlatformPartInfo
{
public:
   static const ObjectType cPART_INFO_ID = cARTICULATED_PART;

   WsfXIO_ArticulatedPartInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_PlatformPartInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_PlatformPartInfo::Serialize(aBuff);
      aBuff& mYaw& mPitch& mRoll& mTilt;
   }

   void UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt) override;
   void UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt) override;

   //! Orientation of the articulated part
   //@{
   double mYaw;
   double mPitch;
   double mRoll;
   double mTilt;
   //@}
private:
   WsfXIO_ArticulatedPartInfo(const WsfXIO_ArticulatedPartInfo&);
};


//! Contains information about a part which contains modes (sensor)
class WSF_EXPORT WsfXIO_ModedPartInfo : public WsfXIO_ArticulatedPartInfo
{
public:
   WsfXIO_ModedPartInfo(ObjectType aType)
      : WsfXIO_ArticulatedPartInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ArticulatedPartInfo::Serialize(aBuff);
      aBuff& mModeNames& mSelectedModeNames& mCurrentModeName;
   }

   void UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt) override;
   void UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt) override;

   void SelectMode(bool aSelect, WsfStringId aModeId);
   bool IsModeSelected(WsfStringId aModeName);

   std::vector<WsfStringId> mModeNames;
   std::vector<WsfStringId> mSelectedModeNames;
   WsfStringId              mCurrentModeName;

private:
   WsfXIO_ModedPartInfo(const WsfXIO_ModedPartInfo&);
};

//! Provides information about a comm
class WSF_EXPORT WsfXIO_CommInfo : public WsfXIO_ArticulatedPartInfo
{
public:
   static const ObjectType cPART_INFO_ID = cCOMM_PART;
   WsfXIO_CommInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_ArticulatedPartInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ArticulatedPartInfo::Serialize(aBuff);
      aBuff& mNetworkName;
   }

   WsfStringId mNetworkName;

private:
   WsfXIO_CommInfo(const WsfXIO_CommInfo&);
};

class WSF_EXPORT WsfXIO_ModeInfo
{
public:
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mModeName;
   }
   WsfStringId mModeName;
};

class WSF_EXPORT WsfXIO_SensorModeInfo : public WsfXIO_ModeInfo
{
public:
   enum ModeType
   {
      cSEARCH = 1,
      cTRACK  = 2
   };

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ModeInfo::Serialize(aBuff);
      aBuff& mFrameTime& mModeType;
   }
   double  mFrameTime;
   int32_t mModeType;
};

//! Provides information about a sensor
class WSF_EXPORT WsfXIO_SensorInfo : public WsfXIO_ModedPartInfo
{
public:
   static const ObjectType cPART_INFO_ID = cSENSOR_PART;
   WsfXIO_SensorInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_ModedPartInfo(aType)
   {
   }
   ~WsfXIO_SensorInfo() override;

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ModedPartInfo::Serialize(aBuff);
      aBuff& mSensorClass& mTransmitters& mReceivers& mModes;
   }
   void UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt) override;
   void UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt) override;

   WsfXIO_SensorModeInfo* GetCurrentMode();

   int                                mSensorClass;
   std::vector<WsfXIO_EM_XmtrInfo*>   mTransmitters;
   std::vector<WsfXIO_EM_RcvrInfo*>   mReceivers;
   std::vector<WsfXIO_SensorModeInfo> mModes;

private:
   WsfXIO_SensorInfo(const WsfXIO_SensorInfo&);
};

//! Contains information about a processor
class WSF_EXPORT WsfXIO_ProcessorInfo : public WsfXIO_PlatformPartInfo
{
public:
   static const ObjectType cPART_INFO_ID = cPROCESSOR_PART;
   WsfXIO_ProcessorInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_PlatformPartInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_PlatformPartInfo::Serialize(aBuff);
   }

private:
   WsfXIO_ProcessorInfo(const WsfXIO_ProcessorInfo&);
};

//! Contains information about a track manager
class WSF_EXPORT WsfXIO_TrackManagerInfo : public WsfXIO_ObjectInfo
{
public:
   friend class WsfXIO_PlatformInfo;
   friend class WsfXIO_RemotePlatformList;

   static const ObjectType cPART_INFO_ID = cTRACK_MANAGER;
   WsfXIO_TrackManagerInfo(ObjectType aType = cTRACK_MANAGER);

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ObjectInfo::Serialize(aBuff);
      aBuff& mCorrelationStrategy& mFusionStrategy;
   }

   WsfStringId GetCorrelationStrategy() const { return mCorrelationStrategy; }
   WsfStringId GetFusionStrategy() const { return mFusionStrategy; }
   //! change the fusion strategy and report the change by broadcasting a packet.
   void ChangeFusionStrategy(WsfStringId aMethod) const;
   //! change the correlation strategy and report the change by broadcasting a packet.
   void ChangeCorrelationStrategy(WsfStringId aMethod) const;

   // The following methods were protected but were made public due to problems with gcc 3 and vs2003.
   //! Sets the fusion strategy without broadcasting a packet.
   void SetFusionStrategy(WsfStringId aMethod) { mFusionStrategy = aMethod; }
   //! Sets the correlation strategy without broadcasting a packet.
   void SetCorrelationStrategy(WsfStringId aMethod) { mCorrelationStrategy = aMethod; }

private:
   WsfXIO_TrackManagerInfo& operator=(const WsfXIO_TrackManagerInfo& aRhs) = delete;
   WsfXIO_TrackManagerInfo(const WsfXIO_TrackManagerInfo& aRhs);

   WsfStringId          mCorrelationStrategy;
   WsfStringId          mFusionStrategy;
   WsfXIO_PlatformInfo* mPlatformPtr;
};

void WSF_EXPORT Load(PakI& aBuff, WsfXIO_PlatformPartInfo*& aPtr);
void WSF_EXPORT Save(PakO& aBuff, WsfXIO_PlatformPartInfo*& aPtr);

//! Represents a serialized WsfZOne.
struct WsfXIO_Zone
{
   WsfStringId mName;
   WsfStringId mGlobalName;
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mName& mGlobalName;
   }
};

class WSF_EXPORT WsfXIO_MoverInfo : public WsfXIO_PlatformPartInfo
{
public:
   WsfXIO_MoverInfo()
      : WsfXIO_PlatformPartInfo(cMOVER_PART)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_PlatformPartInfo::Serialize(aBuff);
      aBuff& PakSerialization::Enum(mSwitchType) & mTurnFailOption& mTurnFailThreshold& mConstraints;
   }

   WsfPath::Switch    mSwitchType;
   int                mTurnFailOption;
   double             mTurnFailThreshold;
   WsfPathConstraints mConstraints;

private:
   WsfXIO_MoverInfo(const WsfXIO_MoverInfo&);
};

class WSF_EXPORT WsfXIO_PlatformInfo : public WsfXIO_ObjectInfo
{
public:
   enum PlatformFlags
   {
      cEXTERNALLY_CONTROLLED = 1
   };

   static const ObjectType cPART_INFO_ID = cPLATFORM;
   WsfXIO_PlatformInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_ObjectInfo(aType)
      , mFlags(0)
      , mStateValid(false)
      , mEntityStateBaseTime(1.0E12)
      , mEntityType()
      , mTrackManagerInfo()
      , mRoutePtr()
      , mTargetWaypointIndex()
      , mMoverPtr()
      , mAuxDataPtr(nullptr)
      , mHostId(0)
      , mHostConnectionPtr(nullptr)
   {
   }

   virtual ~WsfXIO_PlatformInfo();

   bool operator==(const WsfXIO_PlatformInfo& aRhs) const;
   bool operator==(const WsfXIO_EntityId& aRhs) const;

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mIndex& mFlags& mName& mType;
      aBuff&                        mParts;
      aBuff& PakSerialization::Enum(mSpatialDomain) & mIcon& mSide& mMarking& mEntityId& mEntityType;
      aBuff& mTrackManagerInfo& mCommanders&                                             mZoneNames;
      aBuff& mRoutePtr&                                                                  mTargetWaypointIndex;
      aBuff& mMoverPtr&                                                                  mAuxDataPtr;
      if (!T::cIS_OUTPUT)
      {
         mTrackManagerInfo.mPlatformPtr = this;
      }
   }

   void               SetHostConnection(WsfXIO_Connection* aConnectionPtr);
   WsfXIO_Connection* GetHostConnection() const { return mHostConnectionPtr; }
   int                GetConnectionId() const { return mHostId; }

   WsfXIO_PlatformId GetId() const { return WsfXIO_PlatformId(mIndex, mHostId); }

   //! Index if the platform is in the simulation
   int32_t GetIndex() const { return mIndex; }
   //! NameID of the platform or platform type
   WsfStringId GetName() const { return mName; }
   //! NameID of the base type of the platform
   WsfStringId GetType() const { return mType; }
   //! Platform icon
   WsfStringId GetIcon() const { return mIcon; }
   //! SideID of the platform
   WsfStringId GetSide() const { return mSide; }
   WsfStringId GetMarking() const { return mMarking; }
   //! Spatial domain of the platform
   WsfSpatialDomain GetSpatialDomain() const { return mSpatialDomain; }

   using PlatformPartInfo = WsfXIO_PlatformPartInfo;
   using PartList         = std::vector<WsfXIO_PlatformPartInfo*>;
   void                     AddPart(WsfXIO_PlatformPartInfo* aPartPtr) { mParts.push_back(aPartPtr); }
   unsigned int             GetPartCount() const { return (unsigned int)mParts.size(); }
   WsfXIO_PlatformPartInfo* GetPartEntry(unsigned int aEntry) const { return mParts[aEntry]; }

   PartList::const_iterator BeginParts() const { return mParts.begin(); }
   PartList::const_iterator EndParts() const { return mParts.end(); }
   PartList::iterator       BeginParts() { return mParts.begin(); }
   PartList::iterator       EndParts() { return mParts.end(); }
   const PartList&          GetParts() const { return mParts; }
   WsfXIO_PlatformPartInfo* FindPart(WsfStringId aPartName, int aType);

   //! Returns the number of zones the platform uses.
   unsigned int GetZoneCount() const { return (unsigned int)mZoneNames.size(); }

   //! Returns begin index to the list of Zone names.
   std::vector<WsfXIO_Zone>::const_iterator BeginZones() const { return mZoneNames.begin(); }
   std::vector<WsfXIO_Zone>::const_iterator EndZones() const { return mZoneNames.end(); }

   bool UpdateState(double aSimTime);
   void SetStateRefTime(double aBaseTime) { mEntityStateBaseTime = aBaseTime; }
   bool IsStateValid() const { return mEntityStateBaseTime < 1.0E12; }

   const UtEntity& GetState() const { return mEntityState; }
   UtEntity&       GetState() { return mEntityState; }

   WsfXIO_EntityId   GetEntityId() const { return mEntityId; }
   WsfXIO_EntityType GetEntityType() const { return mEntityType; }

   const WsfXIO_TrackManagerInfo& GetTrackManagerInfo() const { return mTrackManagerInfo; }
   WsfXIO_TrackManagerInfo&       GetTrackManagerInfo() { return mTrackManagerInfo; }

   using CommandChain = std::pair<WsfStringId, WsfStringId>;
   const std::vector<CommandChain>& GetCommanders() const { return mCommanders; }

   WsfAttributeContainer* GetAuxData() const { return mAuxDataPtr; }

   void RequestDelete();

   int32_t          mIndex;
   int32_t          mFlags;
   WsfStringId      mName;
   WsfStringId      mType;
   WsfStringId      mIcon;
   WsfStringId      mSide;
   WsfStringId      mMarking;
   WsfSpatialDomain mSpatialDomain;

   std::vector<WsfXIO_PlatformPartInfo*> mParts;

   bool              mStateValid;
   double            mEntityStateBaseTime;
   UtEntity          mEntityState;
   WsfXIO_EntityId   mEntityId;
   WsfXIO_EntityType mEntityType;

   WsfXIO_TrackManagerInfo mTrackManagerInfo;

   std::vector<CommandChain> mCommanders;
   std::vector<WsfXIO_Zone>  mZoneNames;
   WsfRoute*                 mRoutePtr;
   int                       mTargetWaypointIndex;
   WsfXIO_MoverInfo*         mMoverPtr;

   WsfAttributeContainer* mAuxDataPtr; // Aux Data is only sent once, not updated
private:
   int                mHostId;
   WsfXIO_Connection* mHostConnectionPtr;

   WsfXIO_PlatformInfo(const WsfXIO_PlatformInfo& aRhs);
   WsfXIO_PlatformInfo& operator=(const WsfXIO_PlatformInfo& aRhs) = delete;
};

struct WsfXIO_CommandChain
{
   using LinkList = std::vector<WsfStringId>;
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mChainName& mCommander& mSubordinates;
   }
   WsfStringId mChainName;
   WsfStringId mCommander;
   LinkList    mSubordinates;
};

void WSF_EXPORT Load(PakI& aBuff, WsfXIO_ObjectInfo*& aPtr);
void WSF_EXPORT Save(PakO& aBuff, WsfXIO_ObjectInfo*& aPtr);

#endif
