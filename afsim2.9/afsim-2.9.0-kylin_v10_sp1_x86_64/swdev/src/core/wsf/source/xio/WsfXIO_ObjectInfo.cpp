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

#include "xio/WsfXIO_ObjectInfo.hpp"

#include <algorithm>

#include "PakSerializeImpl.hpp"
// PakTypeDictionary is needed for PakSerialization::Polymorphic(mExtraPtr);
#include "PakTypeDictionary.hpp"
#include "UtVec3.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

//! struct allows deleting vector of pointers with std algorithms.
struct WsfXIO_DeleteObj
{
   WsfXIO_DeleteObj()
      : x(0)
   {
   }
   template<class T>
   void operator()(const T* ptr) const
   {
      delete ptr;
   }
   template<class T, class U>
   void operator()(std::pair<T, U*>& pair) const
   {
      delete pair.second;
      pair.second = 0;
   }
   // Dummy variable just to keep Purify from reporting UMRs
   unsigned int x;
};

// =================================================================================================
// WsfXIO_XmtrRcvrInfo
// =================================================================================================

WsfXIO_EM_XmtrRcvrInfo::~WsfXIO_EM_XmtrRcvrInfo()
{
   delete mAntennaPtr;
   delete mExtraPtr;
}

// =================================================================================================
// WsfXIO_XmtrInfo
// =================================================================================================

WsfXIO_EM_XmtrInfo::~WsfXIO_EM_XmtrInfo()
{
   delete mLinkedRcvrPtr;
}

// =================================================================================================
// WsfXIO_PlatformPartInfo
// =================================================================================================

// virtual
WsfXIO_PlatformPartInfo::~WsfXIO_PlatformPartInfo() {}

void WsfXIO_PlatformPartInfo::InitStateChange(WsfXIO_PartStateChangeRequestPkt& aPkt)
{
   aPkt.mPartType      = GetPartType();
   aPkt.mPartName      = GetName();
   aPkt.mPlatformIndex = static_cast<int32_t>(mPlatformIndex);
   aPkt.mStateChange   = 0;
}

// =================================================================================================
// WsfXIO_PlatformInfo
// =================================================================================================

WsfXIO_PlatformInfo::~WsfXIO_PlatformInfo()
{
   delete mMoverPtr;
   std::for_each(mParts.begin(), mParts.end(), WsfXIO_DeleteObj());
}

WsfXIO_PlatformPartInfo* WsfXIO_PlatformInfo::FindPart(WsfStringId aPartName, int aType)
{
   for (size_t i = 0; i < mParts.size(); ++i)
   {
      if (mParts[i]->GetName() == aPartName && mParts[i]->GetPartType() == aType)
      {
         return mParts[i];
      }
   }
   return nullptr;
}

bool WsfXIO_PlatformInfo::operator==(const WsfXIO_PlatformInfo& aRhs) const
{
   return mEntityId == aRhs.mEntityId;
}

bool WsfXIO_PlatformInfo::operator==(const WsfXIO_EntityId& aRhs) const
{
   return mEntityId == aRhs;
}

bool WsfXIO_PlatformInfo::UpdateState(double aSimTime)
{
   bool updated = false;
   if (aSimTime > mEntityStateBaseTime)
   {
      double  dt = aSimTime - mEntityStateBaseTime;
      UtVec3d accel, vel, location;
      mEntityState.GetAccelerationWCS(accel.GetData());
      mEntityState.GetVelocityWCS(vel.GetData());
      mEntityState.GetLocationWCS(location.GetData());
      mEntityState.IncrementLocationWCS((accel * (dt * dt / 2) + vel * dt).GetData());
      mEntityState.SetVelocityWCS((vel + accel * dt).GetData());
      mEntityStateBaseTime = aSimTime;
      updated              = true;
   }
   return updated;
}

//! Sends a request to delete this platform from the hosting simulation.
void WsfXIO_PlatformInfo::RequestDelete()
{
   WsfXIO_DeletePlatformPkt pkt;
   pkt.mPlatformIndices.push_back(mIndex);
   GetHostConnection()->Send(pkt);
}

void WsfXIO_PlatformInfo::SetHostConnection(WsfXIO_Connection* aConnectionPtr)
{
   mHostConnectionPtr = aConnectionPtr;
   if (mHostConnectionPtr != nullptr)
   {
      mHostId = mHostConnectionPtr->GetConnectionId();
   }
   else
   {
      mHostId = 0;
   }
}

// =================================================================================================
// WsfXIO_TrackManagerInfo
// =================================================================================================

WsfXIO_TrackManagerInfo::WsfXIO_TrackManagerInfo(ObjectType aType)
   : WsfXIO_ObjectInfo(aType)
   , mCorrelationStrategy(nullptr)
   , mFusionStrategy(nullptr)
   , mPlatformPtr(nullptr)
{
}

void WsfXIO_TrackManagerInfo::ChangeFusionStrategy(WsfStringId aMethod) const
{
   if (mPlatformPtr != nullptr)
   {
      WsfXIO_TrackManagerStateChangeRequestPkt pkt;
      pkt.mPlatformIndex  = mPlatformPtr->GetIndex();
      pkt.mFusionStrategy = aMethod;
      mPlatformPtr->GetHostConnection()->Send(pkt);
   }
}

void WsfXIO_TrackManagerInfo::ChangeCorrelationStrategy(WsfStringId aMethod) const
{
   if (mPlatformPtr != nullptr)
   {
      WsfXIO_TrackManagerStateChangeRequestPkt pkt;
      pkt.mPlatformIndex       = mPlatformPtr->GetIndex();
      pkt.mCorrelationStrategy = aMethod;
      mPlatformPtr->GetHostConnection()->Send(pkt);
   }
}

// =================================================================================================
// WsfXIO_PlatformPartInfo
// =================================================================================================

namespace
{
bool RequestStateChange(const WsfXIO_PlatformPartInfo& aPartInfo, WsfXIO_PartStateChangeRequestPkt::StateChange aState)
{
   bool sent = false;
   if (aPartInfo.GetPlatform() != nullptr)
   {
      WsfXIO_Connection* connPtr = aPartInfo.GetPlatform()->GetHostConnection();
      if (connPtr != nullptr)
      {
         WsfXIO_PartStateChangeRequestPkt pkt;
         pkt.mPlatformIndex = static_cast<int32_t>(aPartInfo.GetPlatformIndex());
         pkt.mPartName      = aPartInfo.GetName();
         pkt.mPartType      = aPartInfo.GetPartType();
         pkt.mSelectMode    = nullptr; // 0 means no mode change.
         pkt.mStateChange   = aState;
         connPtr->Send(pkt);
         sent = true;
      }
   }
   return sent;
}
} // namespace

// virtual
void WsfXIO_PlatformPartInfo::UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt)
{
   // turn the platform part on/off but don't trigger call back to XIO.
   mIsTurnedOn = aPkt.mIsTurnedOn;
}

// virtual
void WsfXIO_PlatformPartInfo::UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt)
{
   mIsExternallyControlled = aPkt.mIsExternallyControlled;
}

void WsfXIO_PlatformPartInfo::TurnOn(bool aTurnOn)
{
   RequestStateChange(*this,
                      aTurnOn ? WsfXIO_PartStateChangeRequestPkt::cTURN_ON : WsfXIO_PartStateChangeRequestPkt::cTURN_OFF);
   mIsTurnedOn = aTurnOn;
}

void WsfXIO_PlatformPartInfo::TurnOff()
{
   TurnOn(false);
}

void WsfXIO_PlatformPartInfo::SetDebugEnabled(bool aEnabled)
{
   RequestStateChange(*this,
                      aEnabled ? WsfXIO_PartStateChangeRequestPkt::cTURN_DEBUG_ON :
                                 WsfXIO_PartStateChangeRequestPkt::cTURN_DEBUG_OFF);
   mDebugEnabled = aEnabled;
}

// =================================================================================================
// WsfXIO_ArticulatedPartInfo
// =================================================================================================

// virtual
void WsfXIO_ArticulatedPartInfo::UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt)
{
   WsfXIO_PlatformPartInfo::UpdateFromStateChange(aPkt); // Update base class
}

// virtual
void WsfXIO_ArticulatedPartInfo::UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt)
{
   WsfXIO_PlatformPartInfo::UpdateFromPartState(aPkt); // Update base class
   mYaw   = aPkt.mYaw;
   mPitch = aPkt.mPitch;
   mRoll  = aPkt.mRoll;
}

// =================================================================================================
// WsfXIO_ModedPartInfo
// =================================================================================================

// virtual
void WsfXIO_ModedPartInfo::UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt)
{
   WsfXIO_ArticulatedPartInfo::UpdateFromStateChange(aPkt); // Update base class
   mSelectedModeNames = aPkt.mSelectedModes;
   mCurrentModeName   = aPkt.mCurrentModeName;
}

// virtual
void WsfXIO_ModedPartInfo::UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt)
{
   WsfXIO_ArticulatedPartInfo::UpdateFromPartState(aPkt); // Update base class
}

void WsfXIO_ModedPartInfo::SelectMode(bool aSelect, WsfStringId aModeId)
{
   WsfXIO_PartStateChangeRequestPkt pkt;
   pkt.mPlatformIndex = static_cast<int32_t>(GetPlatformIndex());
   pkt.mPartName      = GetName();
   pkt.mPartType      = GetPartType();
   pkt.mSelectMode    = aModeId;
   pkt.mStateChange   = 0;
   if (!aSelect)
   {
      pkt.mStateChange = WsfXIO_PartStateChangeRequestPkt::cDESELECT_MODE;
   }
   if (mPlatformPtr->GetHostConnection())
   {
      mPlatformPtr->GetHostConnection()->Send(pkt);
   }
}

bool WsfXIO_ModedPartInfo::IsModeSelected(WsfStringId aModeName)
{
   return (std::find(mSelectedModeNames.begin(), mSelectedModeNames.end(), aModeName) != mSelectedModeNames.end());
}

// =================================================================================================
// WsfXIO_SensorInfo
// =================================================================================================

WsfXIO_SensorInfo::~WsfXIO_SensorInfo()
{
   std::for_each(mTransmitters.begin(), mTransmitters.end(), WsfXIO_DeleteObj());
   std::for_each(mReceivers.begin(), mReceivers.end(), WsfXIO_DeleteObj());
}

// virtual
void WsfXIO_SensorInfo::UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt)
{
   WsfXIO_ModedPartInfo::UpdateFromStateChange(aPkt); // Update base class
}

// virtual
void WsfXIO_SensorInfo::UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt)
{
   WsfXIO_ModedPartInfo::UpdateFromPartState(aPkt); // Update base class
   // TODO_XIO Why a swap and not an assignment
   std::swap(mReceivers, aPkt.mReceivers);
   std::swap(mTransmitters, aPkt.mTransmitters);
}

WsfXIO_SensorModeInfo* WsfXIO_SensorInfo::GetCurrentMode()
{
   for (size_t i = 0; i < mModes.size(); ++i)
   {
      if (mModes[i].mModeName == mCurrentModeName)
      {
         return &mModes[i];
      }
   }
   return nullptr;
}

// =================================================================================================
// Part serialization
// =================================================================================================

void Load(PakI& aBuff, WsfXIO_PlatformPartInfo*& aPtr)
{
   WsfXIO_ObjectInfo::ObjectType objectType = WsfXIO_ObjectInfo::cNO_OBJECT;
   aBuff&                        objectType;
   aBuff&                        PakSerialization::Polymorphic(aPtr);
}

void Save(PakO& aBuff, WsfXIO_PlatformPartInfo*& aPtr)
{
   WsfXIO_ObjectInfo::ObjectType objectType = WsfXIO_ObjectInfo::cNO_OBJECT;
   if (aPtr != nullptr)
   {
      objectType = aPtr->GetObjectType();
   }
   aBuff& objectType;
   aBuff& PakSerialization::Polymorphic(aPtr);
}

void Load(PakI& aBuff, WsfXIO_ObjectInfo*& aPtr)
{
   using namespace PakSerialization;
   WsfXIO_ObjectInfo::ObjectType objectType = WsfXIO_ObjectInfo::cNO_OBJECT;
   aBuff&                        objectType;
   if (objectType == WsfXIO_ObjectInfo::cPLATFORM)
   {
      aBuff&(WsfXIO_PlatformInfo*&)aPtr;
   }
   else if (objectType == WsfXIO_ObjectInfo::cTRACK_MANAGER)
   {
      aBuff&(WsfXIO_TrackManagerInfo*&)aPtr;
   }
   else
   {
      aBuff& PakSerialization::Polymorphic((WsfXIO_PlatformPartInfo*&)aPtr);
   }
}

void Save(PakO& aBuff, WsfXIO_ObjectInfo*& aPtr)
{
   using namespace PakSerialization;
   WsfXIO_ObjectInfo::ObjectType objectType = WsfXIO_ObjectInfo::cNO_OBJECT;
   if (aPtr != nullptr)
   {
      objectType = aPtr->GetObjectType();
   }
   aBuff& objectType;
   if (objectType == WsfXIO_ObjectInfo::cPLATFORM)
   {
      aBuff&(WsfXIO_PlatformInfo*&)aPtr;
   }
   else if (objectType == WsfXIO_ObjectInfo::cTRACK_MANAGER)
   {
      aBuff&(WsfXIO_TrackManagerInfo*&)aPtr;
   }
   else
   {
      aBuff& PakSerialization::Polymorphic((WsfXIO_PlatformPartInfo*&)aPtr);
   }
}

// stream extractor
std::istream& operator>>(std::istream& aIn, WsfXIO_PlatformId& aId)
{
   aIn >> aId.mConnectionId >> aId.mIndex;
   return aIn;
}

// stream inserter
std::ostream& operator<<(std::ostream& aOut, const WsfXIO_PlatformId& aId)
{
   aOut << aId.mConnectionId << ' ' << aId.mIndex;
   return aOut;
}
