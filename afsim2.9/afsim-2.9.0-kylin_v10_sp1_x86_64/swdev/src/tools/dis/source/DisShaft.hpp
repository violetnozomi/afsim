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

// DIS Underwater Acoustic Shaft System

#ifndef DISSHAFT_HPP
#define DISSHAFT_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisShaft
{
public:
   DisShaft();
   ~DisShaft();

   // Allow compiler to build copy constructor and operator=
   // DisShaft(const DisShaft& aSrc);
   // DisShaft& operator=(const DisShaft& aRhs);

   // Accessors

   DisUint8   GetDataLength();
   DisUint16  GetCurrentShaftRPM() const;
   DisUint16  GetOrderedShaftRPM() const;
   DisFloat32 GetRPMRateOfChange() const;

   // Mutators

   void SetCurrentShaftRPM(DisUint16 aShaftRPM);
   void SetOrderedShaftRPM(DisUint16 aShaftRPM);
   void SetRPMRateOfChange(DisFloat32 aRate);

   // Input/output

   DisUint16 GetLength();
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO);

   bool IsValid() const;

   // The following gets the length as read from the header, not the
   // length computed by adding the length of the base data plus the
   // length required for the specified number of Track/Jam entries.
   DisUint16 GetLengthRead() const;

private:
   DisUint8   mDataLength;
   DisUint16  mCurrentRPM;
   DisUint16  mOrderedRPM;
   DisFloat32 mRate;

   DisUint16 mLengthRead;
};

// Accessors

inline DisUint16 DisShaft::GetCurrentShaftRPM() const
{
   return mCurrentRPM;
}
inline DisUint16 DisShaft::GetOrderedShaftRPM() const
{
   return mOrderedRPM;
}
inline DisFloat32 DisShaft::GetRPMRateOfChange() const
{
   return mRate;
}

inline DisUint16 DisShaft::GetLengthRead() const
{
   return mLengthRead;
}

// Mutators

inline void DisShaft::SetCurrentShaftRPM(DisUint16 aRPM)
{
   mCurrentRPM = aRPM;
}
inline void DisShaft::SetOrderedShaftRPM(DisUint16 aRPM)
{
   mOrderedRPM = aRPM;
}
inline void DisShaft::SetRPMRateOfChange(DisFloat32 aRate)
{
   mRate = aRate;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisShaft& aShaft)
{
   aShaft.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisShaft& aShaft)
{
   aShaft.Get(aGenI);
   return aGenI;
}

#endif
