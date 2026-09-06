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

#ifndef UTCALENDAR_HPP
#define UTCALENDAR_HPP

#include "ut_export.h"

/** An object used to represent a date and time.
   All time is Universal Time (UT), unless otherwise requested in the
   GetLocalTime(...) methods.

   Year, month and day values are subject to the following constraints:

   - Year must be specified as 4 digits (1999, 2000, etc).
   - Month must be in the range [1..12]
   - Days must be in the range [1..31] (modified appropriately for those months that don't have 31 days.
   - Epoch is in a "Two Line Element" format, either the standard one with two-digit years
      (e.g.,   03020.000000 == Jan 20, 2003, UT=00:00:00), or a modified one
      (e.g., 2003020.000000 == Jan 20, 2003, UT=00:00:00; same as above).

   @note On Time:  Internally all attributes are referenced to UTC.  In the interface, all
      non-Julian day accessors and mutators (date, time, epoch) assume UTC reference.  Conversely,
      Julian day accessors and mutators are referenced to UT1 unless explicitly stated in the name
      (e.g., GetJ2000UTC_Date, where the J2000 date is referred to UTC instead of UT1).
      The difference between  UTC and UT1 is the "delta UT1" (mDeltaUT1), which is optionally set in the
      constructor or by mutator (SetDeltaUT1).  By default mDeltaUT1 is zero (i.e., we don't care about
      differences between the two, which are of order 1 second).  It is important to keep this distinction in mind
      when testing or examining calendar quantities.
      The methods GetJ2000TT_Date and GetJ2000TDB_Date provide Terrestrial Time-referenced (TT) and Barycentric
   Time-referenced (TDB) Julian dates, respectively.  These use the "delta Atomic Time" (deltaAT), which is also
   optionally provided in the constructor.  This is a fixed offset in integer seconds, varying year-to-year, that
   include leap seconds. Delta AT, together with delta UT1, provide the reference to a true monotonically increasing
   time.  In the case of TT, this is referenced to earth-based atomic clocks; TDB uses TT and the motion of the earth to
   provide a relativistically stable time reference.
*/

#include <array>
#include <iosfwd>
#include <string>

class UT_EXPORT UtCalendar
{
public:
   static constexpr int                 cSEC_IN_DAY      = 86400;
   static constexpr int                 cSEC_IN_HOUR     = 3600;
   static constexpr int                 cSEC_IN_MIN      = 60;
   static constexpr int                 cMIN_IN_HOUR     = 60;
   static constexpr int                 cDEGREES_IN_HOUR = 15;
   static constexpr int                 cDAYS_IN_WEEK    = 7;
   static constexpr int                 cJD_J2000        = 2451545;
   static constexpr std::array<int, 13> cDAYS_PER_MONTH  = {{-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
   static constexpr std::array<int, 13> cDAYS_BEFORE_MONTH = {{-1, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}};
   static constexpr std::array<const char*, 13> cMONTH_NAMES = {
      {nullptr, "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"}};
   static constexpr std::array<const char*, 7> cWEEK_DAY_NAMES = {
      {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"}};

   enum DayOfWeek
   {
      cSUNDAY,
      cMONDAY,
      cTUESDAY,
      cWEDNESDAY,
      cTHURSDAY,
      cFRIDAY,
      cSATURDAY
   };
   // Values of months as maintained by UtCalendar (for reference)
   enum Month
   {
      cJANUARY = 1,
      cFEBRUARY,
      cMARCH,
      cAPRIL,
      cMAY,
      cJUNE,
      cJULY,
      cAUGUST,
      cSEPTEMBER,
      cOCTOBER,
      cNOVEMBER,
      cDECEMBER
   };

   UtCalendar(double aDeltaUT1 = 0.0, // UT1 and UTC are equivalent by default
              int    aDeltaAT  = 32);     // Typical default value

   ~UtCalendar()       = default;
   UtCalendar& operator=(const UtCalendar& aSrc);

   bool operator==(const UtCalendar& aRhs) const;
   bool operator!=(const UtCalendar& aRhs) const;
   bool operator<(const UtCalendar& aRhs) const;

   void AdvanceTimeBy(double aDeltaSeconds);

   int GetDayOfYear() const;

   double GetEpochDayOfYear() const;

   double GetTimeSince(const UtCalendar& aRefCalendar) const;

   double GetEarthAngleApprox() const;

   double GetEarthAngle() const;

   double GetGAST_Approx() const;

   double GetGAST() const;

   double GetGMST() const;

   double GetGMST_1982() const;

   double GetJ2000_Date() const;

   double GetJ2000UTC_Date() const;

   double GetJ2000TT_Date() const;

   double GetJ2000TDB_Date() const;

   double GetJulianDate() const;

   double GetJulianUTC_Date() const;

   double GetJulianTT_Date() const;

   double GetJulianTDB_Date() const;

   double GetModifiedJulianDate() const;

   double GetModernJulianDate() const;

   double GetDeltaTDB() const;

   /**
      Return the difference between UT1 and UTC (UT1 - UTC).
   */
   double GetDeltaUT1() const { return mDeltaUT1; }

   /**
      Return the difference between TT and UT1 (TT - UT1).
   */
   double GetDeltaT() const { return mDeltaT; }

   int GetDeltaAT() const;

   static int GetMonth(const std::string& aMonth);

   static bool GetTime(const std::string& aHMS, int& aHour, int& aMinute, double& aSecond);

   static double GetJ2000_Date(int aYear, int aMonth, int aDay, double aSecondsPastMidnight, double aDeltaUT1 = 0.0);

   static double GetJulianDate(int aYear, int aMonth, int aDay, double aSecondsPastMidnight, double aDeltaUT1 = 0.0);


   static double GetModifiedJulianDate(int aYear, int aMonth, int aDay, double aSecondsPastMidnight, double aDeltaUT1 = 0.0);

   static double GetModernJulianDate(int aYear, int aMonth, int aDay, double aSecondsPastMidnight, double aDeltaUT1 = 0.0);

   static bool ReadDayOfWeek(std::string& aDayOfWeek, DayOfWeek& aEnum);

   void GetDate(int& aYear, int& aMonth, int& aDay) const;

   void GetLocalDate(int aGMT_Offset, int& aYear, int& aMonth, int& aDay) const;

   void GetTime(int& aHour, int& aMinute, double& aSecond) const;

   void GetLocalMeanTime(int aGMT_Offset, int& aHour, int& aMinute, double& aSecond) const;

   DayOfWeek GetDayOfWeek() const;

   double GetTime() const;

   double GetLocalMeanTime(int aGMT_Offset) const;

   /** Return the calendar year. */
   int GetYear() const { return mYear; }

   /** Return the calendar month.*/
   int GetMonth() const { return mMonth; }

   /** Return the day of the month.*/
   int GetDay() const { return mDay; }

   /** Return the number of hours past midnight.*/
   int GetHour() const { return mHour; }

   /** Return the number of minutes past the hour.*/
   int GetMinute() const { return mMinute; }

   /** Return the number of seconds past the current minute.*/
   double GetSecond() const { return mSecond; }

   double GetEpoch() const;

   bool        IsLeapYear() const;
   static bool IsLeapYear(int aYear);

   /** Set current local time */
   void SetCurrentDateAndTime();

   bool SetDate(int aYear, int aMonth, int aDay);

   bool SetTime(int aHour, int aMinute, double aSecond = 0.0);

   bool SetTime(double aSecondsPastMidnight);

   void SetJ2000_Date(double aJ2000_Date);

   void SetJulianDate(double aJulianDate);

   bool SetEpoch(double aEpoch);

   void SetDeltaUT1(double aDeltaUT1);

   void SetDeltaAT(int aDeltaAT);

   std::string GetISO8601(int aFractionalSecondDigits = 3, bool aUseFullFormat = true) const;

   static UtCalendar ParseISO8601(const std::string&);

   friend UT_EXPORT std::ostream& operator<<(std::ostream& os, const UtCalendar& aCalendar);

private:
   void IncrementDay(int aNumDays);

   void SetJ2000_Date();

   // Attributes
   int    mYear;
   int    mMonth; // 1-12
   int    mDay;
   int    mIntegralDay; // Same as integer value of J2000UTC_Date + 0.5.
   int    mHour;
   int    mMinute;
   double mSecond;
   double mSecondsPastMidnight;
   double mJ2000UTC_Date; // (Based on UTC)
   double mDeltaUT1;      // Difference between UT1 and UTC (see http://maia.usno.navy.mil/ser7/deltat.data)
   double mDeltaT;        // Difference between TT and UT1
};

#endif
