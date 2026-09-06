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

#include "DisClockTime.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include "DisTime.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisClockTime::DisClockTime()
   : mHour(0)
   , mTimePastHour(0)
{
}

DisClockTime::DisClockTime(DisUint32 aHours, DisUint32 aTimePastHour)
   : mHour(aHours)
   , mTimePastHour(aTimePastHour)
{
}

DisClockTime::DisClockTime(const DisClockTime& aSrc)
   : mHour(aSrc.mHour)
   , mTimePastHour(aSrc.mTimePastHour)
{
}

void DisClockTime::Get(GenI& aGenI)
{
   aGenI >> mHour;
   aGenI >> mTimePastHour;
}

void DisClockTime::Put(GenO& aGenO) const
{
   aGenO << mHour;
   aGenO << mTimePastHour;
}

void DisClockTime::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisClockTime::ToString() const
{
   using namespace std;
   std::ostringstream ss("");

   unsigned int absFlag;
   double       secondsAfterHour = DisTime::GetSecsAfterHour(GetTimePastHour(), absFlag);
   unsigned int minutes          = static_cast<unsigned int>(secondsAfterHour / 60);
   double       seconds          = secondsAfterHour - minutes * 60;
   ss << GetHour() << ':' << minutes << ':' << seconds;
   if (absFlag)
   {
      ss << " (abs) ";
   }
   else
   {
      ss << " (rel) ";
   }

   return ss.str();
}

bool DisClockTime::operator==(const DisClockTime& aRhs) const
{
   return (mHour == aRhs.mHour) && (mTimePastHour == aRhs.mTimePastHour);
}

bool DisClockTime::operator!=(const DisClockTime& aRhs) const
{
   return !(*this == aRhs);
}

DisClockTime& DisClockTime::operator=(const DisClockTime& aRhs)
{
   if (this != &aRhs)
   {
      mHour         = aRhs.mHour;
      mTimePastHour = aRhs.mTimePastHour;
   }
   return *this;
}

bool DisClockTime::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   return isValid;
}
