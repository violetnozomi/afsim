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

// DIS Underwater Acoustic APA System

#ifndef DISAPA_HPP
#define DISAPA_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisAPA
{
public:
   DisAPA();
   ~DisAPA();

   // Allow compiler to build copy constructor and operator=
   // DisAPA(const DisAPA& aSrc);
   // DisAPA& operator=(const DisAPA& aRhs);

   // Accessors

   DisUint8  GetDataLength();
   DisUint16 GetIndex() const;
   DisUint16 GetValue() const;

   // Mutators

   void SetIndex(DisUint16 aIndex);
   void SetValue(DisUint16 aValue);

   // Input/output

   DisUint16 GetLength();
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO);

   // The following gets the length as read from the header, not the
   // length computed by adding the length of the base data plus the
   // length required for the specified number of Track/Jam entries.
   DisUint16 GetLengthRead() const;

   //! Data Validation
   bool IsValid() const;

private:
   DisUint8  mDataLength;
   DisUint16 mIndex;
   DisUint16 mValue;

   DisUint16 mLengthRead;
};

// Accessors

inline DisUint16 DisAPA::GetIndex() const
{
   return mIndex;
}
inline DisUint16 DisAPA::GetValue() const
{
   return mValue;
}

inline DisUint16 DisAPA::GetLengthRead() const
{
   return mLengthRead;
}

// Mutators

inline void DisAPA::SetIndex(DisUint16 aIndex)
{
   mIndex = aIndex;
}
inline void DisAPA::SetValue(DisUint16 aValue)
{
   mValue = aValue;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisAPA& aAPA)
{
   aAPA.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisAPA& aAPA)
{
   aAPA.Get(aGenI);
   return aGenI;
}

#endif
