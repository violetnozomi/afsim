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

#ifndef WSFDATETIME_HPP
#define WSFDATETIME_HPP

#include "wsf_export.h"

#include <string>

#include "UtCalendar.hpp"
class UtInput;

/**
   A singleton object used to maintain the start date and time of the simulation.
   Year, month and day values are subject to the following constraints:
   <ul>
   <li> Year must be specified as 4 digits (1999, 2000, etc).
   <li> Month must be in the range [1..12]
   <li> Days must be in the range [1..31]
   (modified appropriately for those months that don't have 31 days.
   </ul>
*/

class WSF_EXPORT WsfDateTime
{
public:
   WsfDateTime()                   = default;
   WsfDateTime(const WsfDateTime&) = default;
   ~WsfDateTime()                  = default;

   bool ProcessInput(UtInput& aInput);

   double GetJulianDate(int aYear, int aMonth, int aDay, double aSecondsPastMidnight) const;

   double GetStartJulianDate() const;

   void GetStartDate(int& aYear, int& aMonth, int& aDay) const;

   void GetStartTime(int& aHour, int& aMinute, double& aSecond) const;

   double GetStartTime() const;

   const UtCalendar& GetStartDateAndTime() const;

   bool SetStartDate(int aYear, int aMonth, int aDay);

   bool SetStartTime(int aHour, int aMinute, double aSecond);

   bool SetStartEpoch(double aEpoch);

   void SetCurrentDateAndTime();

   bool UsingSystemTime() const;

#undef GetCurrentTime // Avoid conflict with Windows macro
   UtCalendar GetCurrentTime(double aSimTime) const;
   void       GetCurrentTime(double aSimTime, UtCalendar& aCurrentTime) const;

protected:
   //! operator= declared but not defined to prevent use.
   WsfDateTime& operator=(const WsfDateTime& aRhs) = delete;

private:
   void ProcessStartDate(UtInput& aInput);
   void ProcessStartTime(UtInput& aInput);
   void ProcessStartEpoch(UtInput& aInput);
   void ProcessStartTimeNow();

   void TestDateTime();

   bool       mUsingSystemTime{false};
   UtCalendar mDateTime;
};

#endif
