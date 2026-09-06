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

#ifndef DISSPECRECORD_HPP
#define DISSPECRECORD_HPP

#include "dis_export.h"

#include <ostream>

#include "DisTypes.hpp"

class GenI;
class GenO;

// DisSpecRecord is a representation of a Record Set exists within a DIS Record Specification record

class DIS_EXPORT DisSpecRecord
{
public:
   DisSpecRecord();
   DisSpecRecord(const DisSpecRecord& aSrc);
   ~DisSpecRecord();

   // Accessors
   DisUint32 GetId() const;
   DisUint32 GetSerialNumber() const;
   DisUint16 GetLength() const;
   DisUint16 GetCount() const;

   // User is expected to pass in a buffer of the correct type.
   // For most Dis I/O work this will be a Big Endian buffer.
   void GetValues(GenO& aGenO) const;

   // Set id and serial number
   void SetId(DisUint32 aId);
   void SetSerialNumber(DisUint32 aSerialNumber);
   void AssignNewSerialNumber();

   // Set the data.
   // User is expected to pass in a buffer of the correct type.
   // For most Dis I/O work this will be a Big Endian buffer.
   // Also, the user is expected to set the id and serial number
   // using the functions above
   void SetValues(DisUint16 aRecLengthBytes, DisUint16 aRecCount, GenI& aGenI);

   // Input/output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;
   void Stream(std::ostream& aStream) const;

   //! Data Validation
   bool IsValid() const;

   // Length of the record set as read/written
   DisUint16 GetLengthOfSet() const;

   // Operators
   DisSpecRecord& operator=(const DisSpecRecord& aRhs);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisSpecRecord& aRhs) const;
   bool operator<(const DisSpecRecord& aRhs) const;

   // This is provided so this type can be used as a key for TCHash
   operator unsigned int() const;

private:
   void             Copy(const DisSpecRecord& aSrc);
   static DisUint32 sSerialNumber;

   DisEnum32 mId;
   DisUint32 mSerialNumber;
   DisUint16 mLengthBits; // in bits
   DisUint16 mCount;

   unsigned int   mSizeBytes; // in bytes
   unsigned char* mValues;
};

// Accessors

inline DisEnum32 DisSpecRecord::GetId() const
{
   return mId;
}

inline DisUint32 DisSpecRecord::GetSerialNumber() const
{
   return mSerialNumber;
}

// return value is in bytes
inline DisUint16 DisSpecRecord::GetLength() const
{
   return (mLengthBits + 7U) / 8U;
}

inline DisUint16 DisSpecRecord::GetCount() const
{
   return mCount;
}


// Mutators

inline void DisSpecRecord::SetId(DisEnum32 aId)
{
   mId = aId;
}

inline void DisSpecRecord::SetSerialNumber(DisUint32 aSerialNumber)
{
   mSerialNumber = aSerialNumber;
}


// Inline Operators

inline bool DisSpecRecord::operator==(const DisSpecRecord& aRhs) const
{
   return ((mSerialNumber == aRhs.mSerialNumber) && (mId == aRhs.mId));
}

inline DisSpecRecord::operator unsigned int() const
{
   return mSerialNumber;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisSpecRecord& aEntityId)
{
   aEntityId.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisSpecRecord& aEntityId)
{
   aEntityId.Get(aGenI);
   return aGenI;
}
#endif
