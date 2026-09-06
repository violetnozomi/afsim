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

#ifndef DISFIXEDDATUM_HPP
#define DISFIXEDDATUM_HPP

#include "dis_export.h"

#include <ostream>

#include "DisTypes.hpp"
#include "GenMemIO.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisFixedDatum
{
public:
   DisFixedDatum();
   DisFixedDatum(DisEnum32 aId, const unsigned char aData[4]);
   DisFixedDatum(const DisFixedDatum& aRhs);
   ~DisFixedDatum();

   DisFixedDatum& operator=(const DisFixedDatum& aRhs);

   // Accessors

   DisEnum32 GetId() const;

   // GetData returns a pointer to the data portion.
   // The length of the data portion is 4 bytes.
   // The caller can use a GenMemI object to interpret
   // the data in a platform independent manner.
   void GetData(unsigned char aData[4]) const;

   template<typename T>
   const T GetDataAs() const
   {
      T         result;
      GenMemIO* tempBuf = new GenMemIO(GenBuf::BigEndian, 4);
      for (unsigned int i = 0; i < 4; ++i)
      {
         *tempBuf << mDataBuf->GetBuffer()[i];
      }
      *tempBuf >> result;
      return result;
   }

   // Mutators

   void SetId(DisEnum32 aId);

   // SetData makes a copy of the users data and stores it in this class.
   // The length of the data portion is 4 bytes.
   // The caller can use a GenMemO object to set
   // the data in a platform independent manner.
   void SetData(const unsigned char aData[4]);

   template<typename T>
   void SetDataAs(const T& aData);

   // Returns length of class in bytes
   DisUint16 GetLength() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void Stream(std::ostream& aStream) const;

   // Data Validation
   bool IsValid() const;

private:
   DisEnum32 mId;
   GenMemIO* mDataBuf;
};

// Accessors

inline DisEnum32 DisFixedDatum::GetId() const
{
   return mId;
}

inline DisUint16 DisFixedDatum::GetLength() const
{
   return 8;
}

inline void DisFixedDatum::GetData(unsigned char aData[4]) const
{
   aData[0] = mDataBuf->GetBuffer()[0];
   aData[1] = mDataBuf->GetBuffer()[1];
   aData[2] = mDataBuf->GetBuffer()[2];
   aData[3] = mDataBuf->GetBuffer()[3];
}

// Mutators

inline void DisFixedDatum::SetId(DisEnum32 aId)
{
   mId = aId;
}

inline void DisFixedDatum::SetData(const unsigned char aData[4])
{
   mDataBuf->GetBuffer()[0] = aData[0];
   mDataBuf->GetBuffer()[1] = aData[1];
   mDataBuf->GetBuffer()[2] = aData[2];
   mDataBuf->GetBuffer()[3] = aData[3];
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisFixedDatum& aFixedDatum)
{
   aFixedDatum.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisFixedDatum& aFixedDatum)
{
   aFixedDatum.Get(aGenI);
   return aGenI;
}

template<typename T>
void DisFixedDatum::SetDataAs(const T& aData)
{
   *mDataBuf << aData;
}

#endif
