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

#ifndef WSFXIO_SERIALIZETYPES_HPP
#define WSFXIO_SERIALIZETYPES_HPP

#include "wsf_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtVec3.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"
#include "WsfXIO_Defines.hpp"

// Implement serialization for some basic types
class DisEntityId;
class WsfAttributeContainer;
class WsfLocalTrack;
class WsfTaskData;
class WsfTrack;
class WsfWaypoint;
class WsfXIO_StringDictionary;

#define WSFXIO_DEFINE_LOAD_SAVE(TYPE)                \
   void WSF_EXPORT Load(PakI& aBuff, TYPE& aObject); \
   void WSF_EXPORT Save(PakO& aBuff, TYPE& aObject)

//! For use with WsfXIO_StringDictionary to group strings together in packets, saving space.
//! This should be treated like a black-box, using WsfXIO_StringDictionary::Encode() and
//! WsfXIO_StringDictionary::Decode() for value access.
struct WsfXIO_StringId
{
   friend class WsfXIO_StringDictionary;
   WsfXIO_StringId()
      : mStringId()
   {
   }
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mStringId;
   }

protected:
   WsfXIO_StringId(int aStringId)
      : mStringId(aStringId)
   {
   }
   int mStringId;
};

struct WsfXIO_EntityId
{
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mSite& mApplication& mEntity;
   }
   template<typename ENTITY_TYPE>
   WsfXIO_EntityId& operator=(ENTITY_TYPE& aRhs)
   {
      mSite        = aRhs.GetSite();
      mApplication = aRhs.GetApplication();
      mEntity      = aRhs.GetEntity();
      return *this;
   }
   WsfXIO_EntityId()
      : mSite(0)
      , mApplication(0)
      , mEntity(0)
   {
   }
   template<typename ENTITY_TYPE>
   WsfXIO_EntityId(const ENTITY_TYPE& aSrc)
   {
      (*this) = aSrc;
   }
   template<typename ENTITY_TYPE>
   ENTITY_TYPE Convert() const
   {
      return ENTITY_TYPE(mSite, mApplication, mEntity);
   }
   bool operator==(const WsfXIO_EntityId& aRhs) const
   {
      return mSite == aRhs.mSite && mApplication == aRhs.mApplication && mEntity == aRhs.mEntity;
   }
   bool operator<(const WsfXIO_EntityId& aRhs) const
   {
      if (mSite < aRhs.mSite)
      {
         return true;
      }
      if (mSite > aRhs.mSite)
      {
         return false;
      }
      if (mApplication < aRhs.mApplication)
      {
         return true;
      }
      if (mApplication > aRhs.mApplication)
      {
         return false;
      }
      return mEntity < aRhs.mEntity;
   }

   uint16_t GetSite() const { return mSite; }
   uint16_t GetApplication() const { return mApplication; }
   uint16_t GetEntity() const { return mEntity; }

   uint16_t mSite;
   uint16_t mApplication;
   uint16_t mEntity;
};

struct WsfXIO_EntityType
{
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mEntityKind& mDomain& mCountry& mCategory& mSubcategory& mSpecific& mExtra;
   }
   template<typename ENTITY_TYPE>
   WsfXIO_EntityType& operator=(ENTITY_TYPE& aRhs)
   {
      mEntityKind  = aRhs.GetEntityKind();
      mDomain      = aRhs.GetDomain();
      mCountry     = aRhs.GetCountry();
      mCategory    = aRhs.GetCategory();
      mSubcategory = aRhs.GetSubcategory();
      mSpecific    = aRhs.GetSpecific();
      mExtra       = aRhs.GetExtra();
      return *this;
   }
   WsfXIO_EntityType()
      : mEntityKind(0)
      , mDomain(0)
      , mCountry(0)
      , mCategory(0)
      , mSubcategory(0)
      , mSpecific(0)
      , mExtra(0)
   {
   }
   template<typename ENTITY_TYPE>
   WsfXIO_EntityType(const ENTITY_TYPE& aSrc)
   {
      (*this) = aSrc;
   }
   template<typename ENTITY_TYPE>
   ENTITY_TYPE Convert()
   {
      return ENTITY_TYPE(mEntityKind, mDomain, mCountry, mCategory, mSubcategory, mSpecific, mExtra);
   }

   uint8_t  GetEntityKind() const { return mEntityKind; }
   uint8_t  GetDomain() const { return mDomain; }
   uint16_t GetCountry() const { return mCountry; }
   uint8_t  GetCategory() const { return mCategory; }
   uint8_t  GetSubcategory() const { return mSubcategory; }
   uint8_t  GetSpecific() const { return mSpecific; }
   uint8_t  GetExtra() const { return mExtra; }

   uint8_t  mEntityKind;
   uint8_t  mDomain;
   uint16_t mCountry;
   uint8_t  mCategory;
   uint8_t  mSubcategory;
   uint8_t  mSpecific;
   uint8_t  mExtra;
};

//! Sometimes it is necessary to put multiple strings into a packet.  This maps the strings to integers
//! so strings are only transmitted once per packet.
class WSF_EXPORT WsfXIO_StringDictionary
{
public:
   WsfXIO_StringDictionary();
   void Save(PakO& aAr);
   void Load(PakI& aAr);

   WsfXIO_StringId Encode(WsfStringId aStringId);
   std::string     Decode(WsfXIO_StringId& aXIO_Id);
   void            Clear();

protected:
   void                       ComputeIds();
   void                       ComputeStrings();
   std::map<WsfStringId, int> mEncodingMap;
   // Maps WsfXIO_StringId to std::string
   std::vector<std::string> mStrings;
};

WSFXIO_DEFINE_LOAD_SAVE(WsfXIO_StringDictionary);
WSFXIO_DEFINE_LOAD_SAVE(DisEntityId);
WSFXIO_DEFINE_LOAD_SAVE(WsfStringId);
// WSFXIO_DEFINE_LOAD_SAVE(PakSerialization::WsfXIO_StringIdSerialize);
WSFXIO_DEFINE_LOAD_SAVE(WsfWaypoint);
WSFXIO_DEFINE_LOAD_SAVE(WsfAttributeContainer);
WSFXIO_DEFINE_LOAD_SAVE(WsfTrack);
WSFXIO_DEFINE_LOAD_SAVE(WsfLocalTrack);
WSFXIO_DEFINE_LOAD_SAVE(WsfTaskData);

template<typename BUFF>
inline void Serialize(BUFF& aBuff, UtVec3d& aVec3)
{
   aBuff& PakSerialization::Array(aVec3.GetData(), 3);
}

#endif
