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

// DIS Event Identification Record

#ifndef DISEVENTID_HPP
#define DISEVENTID_HPP

#include "dis_export.h"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdio.h>

// An overloaded operator<<(ostream& aOut, const DisEventId) function to write
// the contents of this class to an ostream can be found in DisIO.hpp.  This
// function is not defined here due to portability issues dealing with the
// old and new I/O stream libraries (i.e.: one typically can't use <iostream.h>
// and <iostream> in the same application).  See DisIO.hpp for more details.

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
class GenI;
class GenO;

//! Event identification is the association of an event with one or more
//! PDUs that are transmitted by a specific simulation application
class DIS_EXPORT DisEventId
{
public:
   DisEventId() = default;
   DisEventId(const DisEventId& aSrc);
   DisEventId(DisUint16 aSite, DisUint16 aApplication, DisUint16 aNumber);
   ~DisEventId() = default;

   DisEventId* Clone() const;

   // Set the next event number to be assigned by 'AssignNewNumber()'.
   static void SetNextNumber(DisUint16 aNextNumber);

   // Assigns the next sequential event number.  Site and Application
   // are not modified.
   void AssignNewNumber();

   // Accessors

   DisUint16 GetSite() const;
   DisUint16 GetApplication() const;
   DisUint16 GetNumber() const;

   // Mutators

   void SetSite(DisUint16 aSite);
   void SetApplication(DisUint16 aApplication);
   void SetNumber(DisUint16 aNumber);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Operators
   // The following operators are provided so they can be used in
   // containers that require comparisons...

   bool operator==(const DisEventId& aRhs) const;
   bool operator!=(const DisEventId& aRhs) const;
   bool operator<(const DisEventId& aRhs) const;

   // This operator is provided so this type can be used as
   // a key in the STL and TC hash table containers

   size_t operator()(const DisEventId& aId) const;
   bool   operator()(const DisEventId& aId1, const DisEventId& aId2) const;

#if defined(_WIN32) && _MSC_VER >= 1300
   // NOTE:
   // Microsoft Visual C++ 7.0 requires bucket_size and min_buckets
   // to be defined as follows.  MSVC++ 6.0 and earlier don't support
   // the hash map and wont compile with static constants set in the
   // class definition.
   static const size_t bucket_size = 4;
   static const size_t min_buckets = 8;
#endif

   // Data Validation
   bool IsValid() const;

private:
   static DisUint16 mNextNumber;

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
    * A number value assigned to the event.
    */
   DisUint16 mNumber = 0;
};

// Accessors
inline DisUint16 DisEventId::GetSite() const
{
   return mSite;
}
inline DisUint16 DisEventId::GetApplication() const
{
   return mApplication;
}
inline DisUint16 DisEventId::GetNumber() const
{
   return mNumber;
}

// Mutators

inline void DisEventId::SetSite(DisUint16 aSite)
{
   mSite = aSite;
}
inline void DisEventId::SetApplication(DisUint16 aApplication)
{
   mApplication = aApplication;
}
inline void DisEventId::SetNumber(DisUint16 aNumber)
{
   mNumber = aNumber;
}

// Inline Operators
inline bool DisEventId::operator==(const DisEventId& aRhs) const
{
   return ((mNumber == aRhs.mNumber) && (mApplication == aRhs.mApplication) && (mSite == aRhs.mSite));
}

inline bool DisEventId::operator!=(const DisEventId& aRhs) const
{
   return ((mNumber != aRhs.mNumber) || (mApplication != aRhs.mApplication) || (mSite != aRhs.mSite));
}

inline bool DisEventId::operator()(const DisEventId& aId1, const DisEventId& aId2) const
{
   // Call less operator
   return aId1 < aId2;
}

inline size_t DisEventId::operator()(const DisEventId& aId) const
{
   return static_cast<size_t>(aId.GetSite() << 24 & 0xFF000000) |
          static_cast<size_t>(aId.GetApplication() << 16 & 0x00FF0000) | static_cast<size_t>(aId.GetNumber());
}

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisEventId& aEventId)
{
   aEventId.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEventId& aEventId)
{
   aEventId.Get(aGenI);
   return aGenI;
}
#endif
