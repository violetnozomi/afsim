// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISLIVEENTITYID_HPP
#define DISLIVEENTITYID_HPP

#include "dis_export.h"

#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <string>

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "Filterable.hpp"
#include "UtScriptAccessible.hpp"

class GenI;
class GenO;

/**!
 * DIS Live Entity Identification Record: The unique designation of each entity in an
 * event or exercise that is not contained in a Live Entity PDU
 **/
class DIS_EXPORT DisLiveEntityId : public UtScriptAccessible
{
public:
   DisLiveEntityId();
   DisLiveEntityId(const DisLiveEntityId& aSrc);
   DisLiveEntityId(DisUint8 aSite, DisUint8 aApplication, DisUint16 aEntity);
   ~DisLiveEntityId() = default;

   const char* GetScriptClassName() const override;

   // Set the next entity number to be assigned by 'AssignNewEntity()'.
   static void SetNextEntity(DisUint16 aNextEntity);

   // Assigns the next sequential entity number.  Site and Application
   // are not modified.
   // WARNING: Not Thread safe - could result in two equal DisLiveEntityId's
   //          if called from two different threads.
   void AssignNewEntity();

   // The following is a convenience method to see if the object is
   // fully 'defined'.  If all fields are non-zero then the object
   // is considered 'defined'.
   bool IsDefined() const;

   // The following is a convenience method to see if the object is
   // fully 'defined' and not referring to all sites or all applications
   // or all entities.  If all fields are defined and less that 0xFFFF then
   // the object is considered to refer to a single entity.
   bool IsSingle() const;

   DisLiveEntityId* Clone() const;

   // Accessors
   DisUint8  GetSite() const;
   DisUint8  GetApplication() const;
   DisUint16 GetEntity() const;
   void      Get(DisUint8& aSite, DisUint8& aApplication, DisUint16& aEntity) const;

   // Mutators
   void SetSite(DisUint8 aSite);
   void SetApplication(DisUint8 aApplication);
   void SetEntity(DisUint16 aEntity);
   void Set(DisUint8 aSite, DisUint8 aApplication, DisUint16 aEntity);

   // Input/output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Operators
   DisLiveEntityId& operator=(const DisLiveEntityId& aRhs);

   // Data Validation
   bool        IsValid() const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisLiveEntityId& aRhs) const;
   bool operator!=(const DisLiveEntityId& aRhs) const;
   bool operator<(const DisLiveEntityId& aRhs) const;

   // This operator is provided so this type can be used as
   // a key in the STL and TC hash table containers

   size_t operator()(const DisLiveEntityId& aId) const;
   bool   operator()(const DisLiveEntityId& aId1, const DisLiveEntityId& aId2) const;

   // NOTE:
   // Microsoft Visual C++ 7.0 requires bucket_size and min_buckets
   // to be defined as follows.  MSVC++ 6.0 and earlier don't support
   // the hash map and wont compile with static constants set in the
   // class definition.
#if defined(_WIN32) && _MSC_VER >= 1300
   static const size_t bucket_size = 4;
   static const size_t min_buckets = 8;
#endif

   static const DisUint8        cLIVE_NO_SITE   = 0;
   static const DisUint8        cLIVE_NO_APPLIC = 0;
   static const DisLiveEntityId cLIVE_ENTITY_ID_UNKNOWN;
   static const DisLiveEntityId cLIVE_MUNITION_NOT_TRACKED;
   static const DisLiveEntityId cLIVE_NO_ENTITY_IMPACTED;
   static const DisLiveEntityId cLIVE_NO_LOCATION;
   static const DisLiveEntityId cLIVE_NO_SPECIFIC_ENTITY;
   static const DisLiveEntityId cLIVE_TARGET_ID_UNKNOWN;
   static const DisLiveEntityId cLIVE_D_SPOT_NO_ENTITY;

private:
   static DisUint16 mNextEntity;
   /*!
    * A site is a facility, organizational unit, installation, or geographical location that has one or
    * more simulated applications capable of participating in a distributed event.
    *
    * NOTE: The sub components which make up a site (facility, organizational unit, installation, or
    *       geographical location) may and can be associated with multiple sites simultaneously.
    * Each site participating in an event is assigned a unique ID defined here as "mSite".
    */
   DisUint8 mSite;
   /*!
    * An application is a software program that is used to generate and process distributed simulation data
    * (live, virtual, and constructive). An application sends and/or receives PDUs in the course of an event.
    *
    * Each application participating in an exercise is assigned a unique ID/application number defined here as "mApplication"
    */
   DisUint8 mApplication;
   /*!
    * An entity is an object participating within an exercise. Each entity is assigned a unique ID number which is
    * valid through the entire exercise defined here as "mEntity"
    */
   DisUint16 mEntity;
};

// Accessors

inline DisUint8 DisLiveEntityId::GetSite() const
{
   return mSite;
}
inline DisUint8 DisLiveEntityId::GetApplication() const
{
   return mApplication;
}
inline DisUint16 DisLiveEntityId::GetEntity() const
{
   return mEntity;
}

// Mutators
inline void DisLiveEntityId::SetSite(DisUint8 aSite)
{
   mSite = aSite;
}
inline void DisLiveEntityId::SetApplication(DisUint8 aApplication)
{
   mApplication = aApplication;
}
inline void DisLiveEntityId::SetEntity(DisUint16 aEntity)
{
   mEntity = aEntity;
}
inline void DisLiveEntityId::Set(DisUint8 aSite, DisUint8 aApplication, DisUint16 aEntity)
{
   mSite        = aSite;
   mApplication = aApplication;
   mEntity      = aEntity;
}

// Inline Operators
inline bool DisLiveEntityId::operator==(const DisLiveEntityId& aRhs) const
{
   return ((mEntity == aRhs.mEntity) && (mApplication == aRhs.mApplication) && (mSite == aRhs.mSite));
}

inline bool DisLiveEntityId::operator!=(const DisLiveEntityId& aRhs) const
{
   return ((mEntity != aRhs.mEntity) || (mApplication != aRhs.mApplication) || (mSite != aRhs.mSite));
}

inline bool DisLiveEntityId::operator()(const DisLiveEntityId& aId1, const DisLiveEntityId& aId2) const
{
   // Call less operator
   return aId1 < aId2;
}

inline size_t DisLiveEntityId::operator()(const DisLiveEntityId& aId) const
{
   return static_cast<size_t>(aId.GetSite() << 24 & 0xFF000000) |
          static_cast<size_t>(aId.GetApplication() << 16 & 0x00FF0000) | static_cast<size_t>(aId.GetEntity());
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisLiveEntityId& aEntityId)
{
   aEntityId.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisLiveEntityId& aEntityId)
{
   aEntityId.Get(aGenI);
   return aGenI;
}
#endif
