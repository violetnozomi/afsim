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

#include "WsfDateTime.hpp"

#include <cmath>
#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"

/**
   Return the Julian date of the start date and time.
   @returns The number of days since 12:00:00 UTC on 1 January 4713 B.C., corresponding to the
   start date and time.
*/
double WsfDateTime::GetStartJulianDate() const
{
   return mDateTime.GetJulianDate();
}

/**
   Return the Julian date (i.e.: the number of days since 12:00:00 UTC
   on 1 January 4713 B.C.)
   @param aYear The year of interest.
   @param aMonth The month of interest.
   @param aDay The day of interest.
   @param aSecondsPastMidnight The number of seconds past midnight on the day of interest.
*/
double WsfDateTime::GetJulianDate(int aYear, int aMonth, int aDay, double aSecondsPastMidnight) const
{
   return UtCalendar::GetJulianDate(aYear, aMonth, aDay, aSecondsPastMidnight);
}

/** Get the start date of the simulation.
   @param aYear   The start year (output).
   @param aMonth  The start month (output).
   @param aDay    The start day (output).
*/
void WsfDateTime::GetStartDate(int& aYear, int& aMonth, int& aDay) const
{
   mDateTime.GetDate(aYear, aMonth, aDay);
}

/** Get the start time as the number of hours, minutes and seconds past
   midnight of the start day.
   @param aHour   The start hour (output).
   @param aMinute The start minute (output).
   @param aSecond The start second (output).
*/
void WsfDateTime::GetStartTime(int& aHour, int& aMinute, double& aSecond) const
{
   mDateTime.GetTime(aHour, aMinute, aSecond);
}

/** Get the start date and time of the simulation.
   @returns The start date and time in the form of a const UtCalendar reference.
*/
const UtCalendar& WsfDateTime::GetStartDateAndTime() const
{
   return mDateTime;
}

/** Get the time as the number of seconds past midnight of the start day.
   @returns The number of seconds past midnight of the start day.
   (i.e. Midnight= 0.0, 1AM=3600.0, Noon=43200.0, 11:59:59PM=86399.0)
*/
double WsfDateTime::GetStartTime() const
{
   return mDateTime.GetTime();
}

/** Set the start date.  See the NOTE at the top of the
   file concerning the valid values.
   @param aYear   The year to which the date will be set.
   @param aMonth  The month to which the date will be set.
   @param aDay    The day to which the date will be set.
*/
bool WsfDateTime::SetStartDate(int aYear, int aMonth, int aDay)
{
   return mDateTime.SetDate(aYear, aMonth, aDay);
   mUsingSystemTime = false;
}

/** Set the start time as the Epoch (year*1000 + day "." fractional day)
   @param aEpoch The epoch of the start time and date.
*/
bool WsfDateTime::SetStartEpoch(double aEpoch)
{
   return mDateTime.SetEpoch(aEpoch);
   mUsingSystemTime = false;
}

void WsfDateTime::SetCurrentDateAndTime()
{
   mDateTime.SetCurrentDateAndTime();
   mUsingSystemTime = true;
}

/** Set the start time as the number of hours, minutes and seconds past
   midnight of the start day.
   @param aHour      The hour to which the time will be set.
   @param aMinute    The minute to which the time will be set.
   @param aSecond    The second to which the time will be set.
*/
bool WsfDateTime::SetStartTime(int aHour, int aMinute, double aSecond)
{
   return mDateTime.SetTime(aHour, aMinute, aSecond);
   mUsingSystemTime = false;
}

/** Process simulation input commands 'start_date' and 'start_time'.
 */
bool WsfDateTime::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "start_date")
   {
      ProcessStartDate(aInput);
   }
   else if (command == "start_time")
   {
      ProcessStartTime(aInput);
   }
   else if (command == "start_epoch") // use Two-Line element format
   {
      ProcessStartEpoch(aInput);
   }
   else if (command == "start_time_now")
   {
      ProcessStartTimeNow();
   }
   else if (command == "test_date_time")
   {
      TestDateTime();
   }
   else if (command == "delta_universal_time")
   {
      double deltaUT1;
      aInput.ReadValueOfType(deltaUT1, UtInput::cTIME);
      aInput.ValueInClosedRange(deltaUT1, -0.9, 0.9); // By convention, this value never exceeds +/- 0.9 seconds.
      mDateTime.SetDeltaUT1(deltaUT1);
   }
   else if (command == "delta_atomic_time")
   {
      int deltaAT;
      aInput.ReadValue(deltaAT);
      mDateTime.SetDeltaAT(deltaAT);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

#undef GetCurrentTime // Avoid conflict with Windows macro

//! Returns the absolute time as a UtCalendar, given the current simulation time.
//! @param aSimTime The current simulation time, in seconds.
//! @note This method is deprecated; the preferred version is the two parameter version below
UtCalendar WsfDateTime::GetCurrentTime(double aSimTime) const
{
   // Using the start time; reference point is Greenwich Mean

   // Use the start time
   UtCalendar currentCalendar = mDateTime;

   // Advance the time
   currentCalendar.AdvanceTimeBy(aSimTime);

   // Return the current time
   return currentCalendar;
}

//! This is an optimized implementation of GetCurrentTime.
//! @param aSimTime The current simulation time, in seconds.
//! @param aCurrentTime (in/out) The time at aSimTime as a UtCalendar.
void WsfDateTime::GetCurrentTime(double aSimTime, UtCalendar& aCurrentTime) const
{
   double simTime = aCurrentTime.GetTimeSince(mDateTime);
   if (std::abs(simTime) > aSimTime)
   {
      // Use mDateTime as the reference
      aCurrentTime = mDateTime;
      aCurrentTime.AdvanceTimeBy(aSimTime);
   }
   else
   {
      // use the object's time as the reference (faster)
      aCurrentTime.SetDeltaUT1(mDateTime.GetDeltaUT1() * UtCalendar::cSEC_IN_DAY);
      aCurrentTime.SetDeltaAT(mDateTime.GetDeltaAT());
      aCurrentTime.AdvanceTimeBy(aSimTime - simTime);
   }
}

//! Return whether this object was initialized with system / wall clock time.
bool WsfDateTime::UsingSystemTime() const
{
   return mUsingSystemTime;
}

// private
void WsfDateTime::ProcessStartDate(UtInput& aInput)
{
   std::string word;
   aInput.ReadValue(word);
   int month = UtCalendar::GetMonth(word);
   int day;
   int year;
   aInput.ReadValue(day);
   aInput.ReadValue(year);
   if (!SetStartDate(year, month, day))
   {
      throw UtInput::BadValue(aInput);
   }
}

// private
void WsfDateTime::ProcessStartEpoch(UtInput& aInput)
{
   double aEpoch;
   aInput.ReadValue(aEpoch);
   if (!SetStartEpoch(aEpoch))
   {
      throw UtInput::BadValue(aInput);
   }
}

// private
void WsfDateTime::ProcessStartTime(UtInput& aInput)
{
   std::string word;
   aInput.ReadValue(word);
   int    hour;
   int    minute;
   double second;
   if (UtCalendar::GetTime(word, hour, minute, second))
   {
      if (!SetStartTime(hour, minute, second))
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      throw UtInput::BadValue(aInput);
   }
}

// private
void WsfDateTime::ProcessStartTimeNow()
{
   SetCurrentDateAndTime();
}

// private
void WsfDateTime::TestDateTime()
{
   ut::log::info() << mDateTime;
}
