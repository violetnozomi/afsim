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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

// DIS Entity Identification Record

#ifndef DIS_DISRADIOID_HPP
#define DIS_DISRADIOID_HPP

#include "dis_export.h"

// An overloaded operator<<(ostream& aOut, const DisEntityId) function to write
// the contents of this class to an ostream can be found in DisIO.h.  This
// function is not defined here due to portability issues dealing with the
// old and new I/O stream libraries (i.e.: one typically can't use <iostream.h>
// and <iostream> in the same application).  See DisIO.h for more details.

#include <iosfwd>
#include <iostream>

#include "DisTypes.hpp"

class GenI;
class GenO;

class DisEntityId;

//! The unique designation of an attached or unattached radio in an event or exercise
class DIS_EXPORT DisRadioId
{
public:
   DisRadioId() = default;
   DisRadioId(DisUint16 aSite, DisUint16 aApplication, DisUint16 aEntity, DisUint16 aRadio);
   DisRadioId(const DisEntityId& aId, DisUint16 aRadio);
   ~DisRadioId() = default;

   // Set the next entity number to be assigned by 'AssignNewEntity()'.
   static void SetNextRadio(DisUint16 aNextRadio);

   // Assigns the next sequential entity number.  Site, Application, and
   // Entity are not modified.
   void AssignNewRadio();

   // The following is a convenience method to see if the object is
   // fully 'defined'.  If all fields are non-zero then the object
   // is considered 'defined'.
   bool IsDefined() const;

   // The following is a convenience method to see if the object is
   // fully 'defined' and not referring to all sites or all applications
   // or all entities.  If all fields are defined and less that 0xFFFF then
   // the object is considered to refer to a single entity.
   bool IsSingle() const;

   // Accessors

   DisUint16 GetSite() const;
   DisUint16 GetApplication() const;
   DisUint16 GetEntity() const;
   DisUint16 GetRadio() const;
   void      Get(DisUint16& aSite, DisUint16& aApplication, DisUint16& aEntity, DisUint16& aRadio) const;

   // Mutators

   void SetSite(DisUint16 aSite);
   void SetApplication(DisUint16 aApplication);
   void SetEntity(DisUint16 aEntity);
   void SetRadio(DisUint16 aRadio);
   void Set(DisUint16 aSite, DisUint16 aApplication, DisUint16 aEntity, DisUint16 aRadio);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   std::ostream& Pack(std::ostream& aOs, bool aToNet = false) const;
   std::istream& UnPack(std::istream& aIs, bool aToHost = false);

   // Operators
   // The following operators are provided so they can be used in
   // containers that require comparisons...

   bool operator==(const DisEntityId& aRhs) const;
   bool operator!=(const DisEntityId& aRhs) const;
   bool operator==(const DisRadioId& aRhs) const;
   bool operator!=(const DisRadioId& aRhs) const;
   bool operator<(const DisRadioId& aRhs) const;

   // This operator is provided so this type can be used as
   // a key in the STL and TC hash table containers

   size_t operator()(const DisRadioId& aId) const;
   bool   operator()(const DisRadioId& aId1, const DisRadioId& aId2) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Enumerations

   // Symbolic names defined in IEEE 1278.1, Sec 5.1.4.
   enum
   {
      ALL_SITES      = 0xFFFF,
      ALL_APPLIC     = 0xFFFF,
      ALL_ENTITIES   = 0xFFFF,
      ALL_RADIOS     = 0xFFFF,
      NO_SITE        = 0,
      NO_APPLIC      = 0,
      NO_ENTITY      = 0,
      NO_RADIO       = 0,
      RQST_ASSIGN_ID = 0xFFFE
   };

private:
   static DisUint16 mNextRadio;
   /*!
    * A site is a facility, organizational unit, installation, or geographical location that has one or
    * more simulated applications capable of participating in a distributed event.
    *
    * NOTE: The sub components which make up a site (facility, organizational unit, installation, or
    *       geographical location) may and can be associated with multiple sites simultaneously.
    * Each site participating in an event is assigned a unique ID defined here as "mSite".
    */
   DisUint16 mSite = 0;
   /*!
    * An application is a software program that is used to generate and process distributed simulation data
    * (live, virtual, and constructive). An application sends and/or receives PDUs in the course of an event.
    *
    * Each application participating in an exercise is assigned a unique ID/application number defined here as "mApplication"
    */
   DisUint16 mApplication = 0;
   /*!
    * An entity is an object participating within an exercise. Each entity is assigned a unique ID number which is
    * valid through the entire exercise defined here as "mEntity"
    */
   DisUint16 mEntity = 0;
   /*!
    * Specifies the number of the radio
    */
   DisUint16 mRadio = NO_RADIO;
};

inline bool DisRadioId::IsDefined() const
{
   return (mSite != NO_SITE && mApplication != NO_APPLIC && mEntity != NO_ENTITY && mEntity != RQST_ASSIGN_ID &&
           mRadio != NO_RADIO);
}

inline bool DisRadioId::IsSingle() const
{
   return (mSite != NO_SITE && mSite != ALL_SITES && mApplication != NO_APPLIC && mApplication != ALL_APPLIC &&
           mEntity != NO_ENTITY && mEntity != ALL_ENTITIES && mRadio != NO_RADIO && mRadio != ALL_RADIOS);
}

// Accessors

inline DisUint16 DisRadioId::GetSite() const
{
   return mSite;
}
inline DisUint16 DisRadioId::GetApplication() const
{
   return mApplication;
}
inline DisUint16 DisRadioId::GetEntity() const
{
   return mEntity;
}
inline DisUint16 DisRadioId::GetRadio() const
{
   return mRadio;
}
inline void DisRadioId::Get(DisUint16& aSite, DisUint16& aApplication, DisUint16& aEntity, DisUint16& aRadio) const
{
   aSite        = mSite;
   aApplication = mApplication;
   aEntity      = mEntity;
   aRadio       = mRadio;
}

// Mutators

inline void DisRadioId::SetSite(DisUint16 aSite)
{
   mSite = aSite;
}
inline void DisRadioId::SetApplication(DisUint16 aApplication)
{
   mApplication = aApplication;
}
inline void DisRadioId::SetEntity(DisUint16 aEntity)
{
   mEntity = aEntity;
}
inline void DisRadioId::SetRadio(DisUint16 aRadio)
{
   mRadio = aRadio;
}
inline void DisRadioId::Set(DisUint16 aSite, DisUint16 aApplication, DisUint16 aEntity, DisUint16 aRadio)
{
   mSite        = aSite;
   mApplication = aApplication;
   mEntity      = aEntity;
   mRadio       = aRadio;
}

// Inline Operators
inline bool DisRadioId::operator==(const DisRadioId& aRhs) const
{
   return ((mRadio == aRhs.mRadio) && (mEntity == aRhs.mEntity) && (mApplication == aRhs.mApplication) &&
           (mSite == aRhs.mSite));
}

inline bool DisRadioId::operator!=(const DisRadioId& aRhs) const
{
   return ((mRadio != aRhs.mRadio) || (mEntity != aRhs.mEntity) || (mApplication != aRhs.mApplication) ||
           (mSite != aRhs.mSite));
}

inline bool DisRadioId::operator()(const DisRadioId& aId1, const DisRadioId& aId2) const
{
   // Call less operator
   return aId1 < aId2;
}

inline size_t DisRadioId::operator()(const DisRadioId& aId) const
{
   return static_cast<size_t>(aId.GetSite() << 24 & 0xFF000000) |
          static_cast<size_t>(aId.GetApplication() << 16 & 0x00FF0000) |
          static_cast<size_t>(aId.GetEntity() << 4 & 0x0000FFF0) | static_cast<size_t>(aId.GetRadio() & 0x0000000F);
}


// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisRadioId& aRadioId)
{
   aRadioId.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisRadioId& aRadioId)
{
   aRadioId.Get(aGenI);
   return aGenI;
}

#endif
