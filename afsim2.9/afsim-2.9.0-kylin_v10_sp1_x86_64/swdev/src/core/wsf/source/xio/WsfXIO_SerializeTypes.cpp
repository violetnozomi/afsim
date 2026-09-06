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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "xio/WsfXIO_SerializeTypes.hpp"

#include "PakSerializeImpl.hpp"
#include "PakTypeDictionary.hpp"
#include "UtScriptData.hpp"
#include "UtVariant.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTaskData.hpp"
#include "WsfTrack.hpp"
#include "WsfWaypoint.hpp"

///////////////////////////////////////////////////////////////////////////////
// Explanation:
//    This file and its header exists to implement the serialization of
//    WSF objects outside of the WSF header files.  Keeping this code
//    separate allows for the removal of XIO from the WSF build if necessary.
//    The function name 'SerializeP()' implies that it is private and should
//    only be called from the related Save() or Load() function.
///////////////////////////////////////////////////////////////////////////////

// Implements Load and Save for a type by calling SerializeP() on an object
#define WSFXIO_IMPLEMENT_LOAD_SAVE(TYPE)                                \
   void Load(PakI& aBuff, TYPE& aObject) { aObject.SerializeP(aBuff); } \
   void Save(PakO& aBuff, TYPE& aObject) { aObject.SerializeP(aBuff); }

void WsfXIO_StringDictionary::Save(PakO& aAr)
{
   ComputeStrings();
   aAr& mStrings;
}
void WsfXIO_StringDictionary::Load(PakI& aAr)
{
   aAr& mStrings;
}

void Save(PakO& aAr, WsfXIO_StringDictionary& aDictionary)
{
   aDictionary.Save(aAr);
}

void Load(PakI& aAr, WsfXIO_StringDictionary& aDictionary)
{
   aDictionary.Load(aAr);
}

WsfXIO_StringDictionary::WsfXIO_StringDictionary()
{
   mEncodingMap[nullptr] = 0;
}

void WsfXIO_StringDictionary::Clear()
{
   mEncodingMap.clear();
   mEncodingMap[nullptr] = 0;
   mStrings.clear();
}

namespace
{
bool IntPairSecondLess(const std::pair<WsfStringId, int>& aLhs, const std::pair<WsfStringId, int>& aRhs)
{
   return aLhs.second < aRhs.second;
}
} // namespace

void WsfXIO_StringDictionary::ComputeStrings()
{
   if (mStrings.size() == mEncodingMap.size())
   {
      return;
   }

   // Sort by ID
   std::vector<std::pair<WsfStringId, int>> mapping;
   mapping.insert(mapping.end(), mEncodingMap.begin(), mEncodingMap.end());
   std::sort(mapping.begin(), mapping.end(), &IntPairSecondLess);

   // Convert ID to string
   mStrings.resize(mapping.size());
   for (size_t i = 0; i < mStrings.size(); ++i)
   {
      mStrings[i] = mapping[i].first;
   }
}

// Converts a WsfXIO_StringId to a WsfStringId.  Should only be called after Load(), and not in combination with Encode()
std::string WsfXIO_StringDictionary::Decode(WsfXIO_StringId& aXIO_Id)
{
   int index = aXIO_Id.mStringId;
   if (index >= 0 && index < (int)mStrings.size())
   {
      return mStrings[aXIO_Id.mStringId];
   }
   return std::string();
}

// Converts a WsfStringId to a WsfXIO_StringId.  Should only be called prior to Save(), and not in combination with Decode()
WsfXIO_StringId WsfXIO_StringDictionary::Encode(WsfStringId aStringId)
{
   std::map<WsfStringId, int>::iterator i = mEncodingMap.find(aStringId);
   if (i != mEncodingMap.end())
   {
      return WsfXIO_StringId(i->second);
   }
   else
   {
      int nextId              = (int)mEncodingMap.size();
      mEncodingMap[aStringId] = nextId;
      return WsfXIO_StringId(nextId);
   }
}

void Load(PakI& aBuff, WsfStringId& aEntityId)
{
   std::string idString;
   aBuff&      idString;
   aEntityId.SetString(idString);
}

void Save(PakO& aBuff, WsfStringId& aEntityId)
{
   aBuff&(std::string)aEntityId.GetString();
}

// namespace
//{
//    bool ConvertToVariant(const UtScriptData& aData, UtVariant& aVariant)
//    {
//       switch (aData.GetType())
//       {
//          case UtScriptData::cBOOL:
//             aVariant = aData.GetBool();
//             break;
//          case UtScriptData::cINT:
//             aVariant = aData.GetInt();
//             break;
//          case UtScriptData::cDOUBLE:
//             aVariant = aData.GetDouble();
//             break;
//          case UtScriptData::cSTRING:
//             aVariant = aData.GetString();
//             break;
//          default:
//             return false;
//       }
//       return true;
//    }
//
//    bool ConvertToScript(const UtVariant& aVariant, UtScriptData& aData)
//    {
//       switch (aVariant.GetType())
//       {
//          case UtVariant::cBOOL:
//             aData.SetBool(aVariant.GetBool());
//             break;
//          case UtVariant::cINT:
//             aData.SetInt(aVariant.GetInt());
//             break;
//          case UtVariant::cDOUBLE:
//             aData.SetDouble(aVariant.GetDouble());
//             break;
//          case UtVariant::cSTRING:
//             aData.SetString(aVariant.GetString());
//             break;
//          default:
//             return false;
//       }
//       return true;
//    }
//
// }

void Load(PakI& aBuff, UtAttributeContainer& aContainer)
{
   std::vector<std::pair<std::string, UtVariant>> attributes;
   aBuff&                                         attributes;
   for (const auto& attribute : attributes)
   {
      aContainer.AssignVar(attribute.first, attribute.second);
   }
}

void Save(PakO& aBuff, UtAttributeContainer& aContainer)
{
   std::vector<std::pair<std::string, UtVariant>> attributes;
   const UtAttributeContainer::AttributeMap&      attributeMap = aContainer.GetAttributeMap();
   for (const auto& attrib : attributeMap)
   {
      const UtAttributeBase* attributePtr = attrib.second.get();
      UtVariant              var;
      if (attributePtr->GetVariant(var))
      {
         attributes.emplace_back(attrib.first, var);
      }
   }
   aBuff& attributes;
}
void Load(PakI& aBuff, WsfAttributeContainer& aContainer)
{
   Load(aBuff, (UtAttributeContainer&)aContainer);
}
void Save(PakO& aBuff, WsfAttributeContainer& aContainer)
{
   Save(aBuff, (UtAttributeContainer&)aContainer);
}

////////////////////////////////////////////////////////////////////////////////
// WsfWaypoint
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void WsfWaypoint::SerializeP(T& aBuff)
{
   using namespace PakSerialization;
   aBuff& mPointType& mLat& mLon& mAlt& mDistanceOrTime& mDistanceAlongRoute& mHeading& mSpeed;
   aBuff& mLinearAccel& mRadialAccel& mClimbRate& mMaximumFlightPathAngle& mPauseTime&  mLabelId;
   aBuff& mScriptId& mGotoId& mNewModeId& Enum(mAltRef) & Enum(mEndOfPathOption);
   aBuff& Enum(mTurnDirection) & Enum(mSwitch) & mRouteId& mNodeId& mPositionInRoute;
   WsfAuxDataEnabled::Serialize(aBuff);
}
WSFXIO_IMPLEMENT_LOAD_SAVE(WsfWaypoint)

////////////////////////////////////////////////////////////////////////////////
// WsfTrack
////////////////////////////////////////////////////////////////////////////////

//! \relates WsfTrack::SerializeP
template<typename T>
void WsfTrack::SerializeP(T& aBuff)
{
   using namespace PakSerialization;
   // UtMeasurementData attributes
   aBuff&             mUpdateTime;
   aBuff&             mLocationWCS;
   aBuff&             mLocationLLA;
   aBuff&             mVelocityWCS;
   aBuff&             mStateCovariancePtr;
   aBuff&             mOriginatorLocationWCS;
   aBuff&             mOriginatorTransformWCS;
   aBuff& mRange&     mBearing;
   aBuff& mElevation& mRangeRate;
   aBuff& mTypeIds&   mSideId;
   // End UtMeasurementData attributes

   // WsfMeasurement attributes
   aBuff& mSpatialDomain;
   // End WsfMeasurement attributes

   aBuff& mTrackId&      Enum(mTrackType) & mOriginatorNameId;
   aBuff&                mOriginatorTypeId;
   aBuff& mSensorNameId& mSensorTypeId;
   aBuff&                mSensorModeId;
   aBuff& mUpdateCount&  mStartTime; // & mUpdateTime;
   aBuff&                mLastUpdateTime;
   aBuff&                mGlobalTrackNumber;
   aBuff&                mMessageDataTag;

   aBuff& mTrackQuality& mSignalToNoise& mPixelCount;
   aBuff&                                mIFF_Status;
   aBuff&                                mSignalListPtr;
   aBuff&                                mEmitterTypeIdListPtr;
   aBuff&                                mResidualCovariancePtr;

   aBuff&                 mFlags;
   aBuff& mFalseTargetId& mTargetIndex;
   aBuff& mTargetName&    mTargetType;
   aBuff&                 mLastSourceName;
   WsfAuxDataEnabled::Serialize(aBuff);
}
WSFXIO_IMPLEMENT_LOAD_SAVE(WsfTrack)

////////////////////////////////////////////////////////////////////////////////
// WsfLocalTrack
////////////////////////////////////////////////////////////////////////////////

//! \relates WsfLocalTrack::SerializeP
template<typename T>
void WsfLocalTrack::SerializeP(T& aBuff)
{
   WsfTrack::SerializeP(aBuff);

   if (!T::cIS_OUTPUT)
   {
      int32_t size(0);
      aBuff&  size;
      for (int32_t i = 0; i < size; ++i)
      {
         WsfTrackId id;
         aBuff&     id;
         mFusedTrackIds.Add(id, id);
      }
   }
   else
   {
      int32_t count = static_cast<int32_t>(mFusedTrackIds.GetCount());
      aBuff&  count;
      for (unsigned int i = 0; i < mFusedTrackIds.GetCount(); ++i)
      {
         aBuff&* mFusedTrackIds.GetEntry(i);
      }
   }
   aBuff& mUseCount& mHighestQualityTrackId& mMirrorTrackId;
}

WSFXIO_IMPLEMENT_LOAD_SAVE(WsfLocalTrack)

////////////////////////////////////////////////////////////////////////////////
// WsfTaskData
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void WsfTaskData::SerializeP(T& aBuff)
{
   using namespace PakSerialization;
   aBuff& mTaskId& mTrackId& mLocalTrackId& mTaskType& mTargetName& mAssignerPlatformName& mAssignerPlatformIndex&
         mAssignerProcessorName& mAssigneePlatformName& mAssigneePlatformIndex& mAssigneeProcessorName& Polymorphic(
            mResourcePtr) &
      mCommName& mAssignTime& mUpdateTime& mObjectsPending& mTaskStatus& mTaskSubStatus;
}
WSFXIO_IMPLEMENT_LOAD_SAVE(WsfTaskData)
