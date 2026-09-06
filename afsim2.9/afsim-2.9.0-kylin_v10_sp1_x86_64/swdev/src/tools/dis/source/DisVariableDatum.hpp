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

#ifndef DISVARIABLEDATUM_HPP
#define DISVARIABLEDATUM_HPP

#include "dis_export.h"

#include <ostream>

#include "DisTypes.hpp"
#include "GenMemIO.hpp"
#include "math.h"
class DisClockTime;

class GenI;
class GenO;

/**!
 * A Variable Datum record shall be used for all data values in a Datum Specification record that are over 32 bits
 * in length or require more than one numeric value.
 */
class DIS_EXPORT DisVariableDatum
{
public:
   DisVariableDatum();
   DisVariableDatum(GenI& aGenI);
   DisVariableDatum(DisEnum32 aId, const unsigned char* aDataPtr, DisUint32 aDataLength);

   DisVariableDatum(const DisVariableDatum& aSrc);
   ~DisVariableDatum();

   // This constructor creates a DisVariableDatum from a DisClockTime
   DisVariableDatum(DisEnum32 aId, const DisClockTime& aDisClockTime);

   // Accessors
   DisEnum32 GetId() const;

   // This accessor is only valid if the VariableDatum has been
   // examined for an Id that indicates it does contain a DIS Simulation Time.
   void GetClockTimeFromBuffer(DisClockTime& aDisClockTime) const;

   // GetData returns a pointer to the variable data portion and the length
   // of the data portion in bits.
   // The caller can use a GenMemIOobject to interpret
   // the data in a platform independent manner.
   // NOTE: THE LENGTH IS IN BITS!!!
   void GetData(const unsigned char*& aDataPtr, unsigned int& aDataLength) const;

   // This version will populate your object for you with data from this class
   template<typename T>
   void GetDataAs(T& aUserObj) const
   {
      unsigned int numBytes = 8 * static_cast<unsigned int>((mDataLength + 63) / 64);
      GenMemIO     genMemIO(mDataPtr, numBytes, GenBuf::BigEndian, numBytes);
      genMemIO >> aUserObj;
   }
   template<typename T>
   const T GetDataAs() const
   {
      T            userObj;
      unsigned int numBytes = 8 * static_cast<unsigned int>((mDataLength + 63) / 64);
      GenMemIO     genMemIO(mDataPtr, numBytes, GenBuf::BigEndian, numBytes);
      genMemIO >> userObj;
      return userObj;
   }

   // Mutators
   void SetId(DisEnum32 aId);

   // SetData makes a copy of the users data and stores it in this class.
   // NOTE: THE LENGTH IS IN BITS!!!
   void SetData(const unsigned char* aDataPtr, DisUint32 aDataLength);

   template<typename T>
   void SetDataAs(const T& aUserObj, DisUint32 aDataLength)
   {
      unsigned int numBytes = PrepareMemory(aDataLength);
      if (numBytes)
      {
         GenMemIO genMemIO(mDataPtr, numBytes, GenBuf::BigEndian, 0);
         genMemIO << aUserObj;
      }
   }
   // SetDataPtr allows the caller to replace the data portion of this class.  USE WITH GREAT CARE!
   // CAUTION....  The memory pointed to the supplied pointer becomes the
   // CAUTION....  property of the object of this class and will be deleted when the objected
   // CAUTION....  is deleted or if the pointer is later replaced.
   // CAUTION....  ALSO, The allocated data pointed to by the pointer aDataPtr must end on a 64 bit boundary.
   // NOTE:        THE LENGTH IS IN BITS!!!  It represents the length of the valid user data and does
   // NOTE:        necessarily need to end on a byte boundary.
   void SetDataPtr(unsigned char* aDataPtr, DisUint32 aDataLength);

   // Returns length of class in bytes
   DisUint16 GetLength() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   void SetDatumString(const std::string& aString);

   // Operators
   DisVariableDatum& operator=(const DisVariableDatum& aRhs);

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

private:
   void         Copy(const DisVariableDatum& aRhs);
   unsigned int PrepareMemory(DisUint32 aDataLength);
   void         GetMemberData(GenI& aGenI);

   //! Deletes mDataPtr contents if available
   bool DeleteMemory();

   //! This field specifies the type of variable datum to be communicated
   DisEnum32 mId = 0;

   //! This field specifies the number of bits used in the Variable Datum Value
   DisUint32 mDataLength = 0; // in BITS!

   //! This field specifies the value for a particular Variable Datum ID
   unsigned char* mDataPtr;
};

// Accessors

inline DisEnum32 DisVariableDatum::GetId() const
{
   return mId;
}

inline void DisVariableDatum::GetData(const unsigned char*& aDataPtr, unsigned int& aDataLength) const
{
   aDataPtr    = mDataPtr;
   aDataLength = mDataLength;
}

inline DisUint16 DisVariableDatum::GetLength() const
{
   // Returns length of class in bytes
   return static_cast<DisUint16>(8 + 8 * static_cast<DisUint16>((mDataLength + 63) / 64));
}

// Mutators

inline void DisVariableDatum::SetId(DisEnum32 aId)
{
   mId = aId;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisVariableDatum& aVariableDatum)
{
   aVariableDatum.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisVariableDatum& aVariableDatum)
{
   aVariableDatum.Get(aGenI);
   return aGenI;
}

#endif
