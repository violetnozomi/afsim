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

#ifndef DISCLOCKTIME_HPP
#define DISCLOCKTIME_HPP

#include "dis_export.h"

#include <iostream>
#include <stdio.h>

#include "DisTypes.hpp"

class GenI;
class GenO;

// DIS Clock Time Record represents time measurements that surpass one hour
class DIS_EXPORT DisClockTime
{
public:
   DisClockTime();
   DisClockTime(const DisClockTime& aSrc);
   DisClockTime(DisUint32 aHours, DisUint32 aTimePastHour);
   ~DisClockTime() = default;

   // Accessors

   DisInt32  GetHour() const;
   DisUint32 GetTimePastHour() const;

   // Mutators

   void SetHour(DisInt32 aHour);
   void SetTimePastHour(DisUint32 aTimePastHour);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Operators

   DisClockTime& operator=(const DisClockTime& aRhs);
   bool          operator==(const DisClockTime& aRhs) const;
   bool          operator!=(const DisClockTime& aRhs) const;

   //! Data Validation
   bool IsValid() const;

private:
   /*!
    * Specifies the number of hours since 0000 h 1 January 1970 UTC.
    */
   DisInt32 mHour;
   /*!
    * Specifies the time past the hour indicated in the hour field.
    */
   DisUint32 mTimePastHour;
};

// Accessors

inline DisInt32 DisClockTime::GetHour() const
{
   return mHour;
}
inline DisUint32 DisClockTime::GetTimePastHour() const
{
   return mTimePastHour;
}

// Mutators

inline void DisClockTime::SetHour(DisInt32 aHour)
{
   mHour = aHour;
}
inline void DisClockTime::SetTimePastHour(DisUint32 aTimePastHour)
{
   mTimePastHour = aTimePastHour;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisClockTime& aClockTime)
{
   aClockTime.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisClockTime& aClockTime)
{
   aClockTime.Get(aGenI);
   return aGenI;
}
#endif
