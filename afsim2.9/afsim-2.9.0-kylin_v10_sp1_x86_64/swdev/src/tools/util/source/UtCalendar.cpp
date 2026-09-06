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

#include "UtCalendar.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "UtEarth.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"

constexpr std::array<int, 13>         UtCalendar::cDAYS_PER_MONTH;
constexpr std::array<int, 13>         UtCalendar::cDAYS_BEFORE_MONTH;
constexpr std::array<const char*, 13> UtCalendar::cMONTH_NAMES;
constexpr std::array<const char*, 7>  UtCalendar::cWEEK_DAY_NAMES;

namespace
{
// Determine if a leap second insertion is valid at the specified time of year.
// A formal specification reading allows leap seconds at the end of every month,
// but IERS notes a preference to add leap seconds at the end of June or December,
// which in practice is the only time they have been added.
bool IsValidLeapSecondDate(int aMonth, int aDayOfMonth)
{
   return (aMonth == 6 && aDayOfMonth == UtCalendar::cDAYS_PER_MONTH[6]) ||
          (aMonth == 12 && aDayOfMonth == UtCalendar::cDAYS_PER_MONTH[12]);
}

bool IsValidLeapSecondTimePoint(int aMonth, int aDayOfMonth, int aHour, int aMinute)
{
   return (aHour == 23) && (aMinute == 59) && IsValidLeapSecondDate(aMonth, aDayOfMonth);
}
} // namespace

//! Constructor with optional default values for delta UT1 (= UT1 - UTC) and
//! delta AT (TAI - UTC).  If these values are not provided, it is assumed that
//! differences between UTC and UT1 (usually less than 1 second) are not important,
//! and either the default date will be used, or fine differences in ECI locations are
//! also not important, and the values for the default epoch date (6/1/2003) will be used.
UtCalendar::UtCalendar(double aDeltaUT1, //  =  0.0,
                       int    aDeltaAT)     //  =  32
   : mYear(2003)
   , mMonth(6)
   , mDay(1)
   , mIntegralDay(1247)
   , mHour(12)
   , mMinute(0)
   , mSecond(0.0)
   , mSecondsPastMidnight(43200.0)
   , mJ2000UTC_Date(1247.0)
   , mDeltaUT1(aDeltaUT1 / cSEC_IN_DAY)
   , mDeltaT((32.184 + aDeltaAT - aDeltaUT1) / cSEC_IN_DAY)
{
}

UtCalendar& UtCalendar::operator=(const UtCalendar& aSrc)
{
   if (this != &aSrc)
   {
      mYear                = aSrc.mYear;
      mMonth               = aSrc.mMonth;
      mDay                 = aSrc.mDay;
      mIntegralDay         = aSrc.mIntegralDay;
      mHour                = aSrc.mHour;
      mMinute              = aSrc.mMinute;
      mSecond              = aSrc.mSecond;
      mSecondsPastMidnight = aSrc.mSecondsPastMidnight;
      mJ2000UTC_Date       = aSrc.mJ2000UTC_Date;
      mDeltaUT1            = aSrc.mDeltaUT1;
      mDeltaT              = aSrc.mDeltaT;
   }
   return *this;
}

bool UtCalendar::operator==(const UtCalendar& aRhs) const
{
   return (GetJ2000_Date() == aRhs.GetJ2000_Date());
}

bool UtCalendar::operator!=(const UtCalendar& aRhs) const
{
   return !(*this == aRhs);
}

bool UtCalendar::operator<(const UtCalendar& aRhs) const
{
   bool isLess = true;

   if (mJ2000UTC_Date > aRhs.mJ2000UTC_Date)
   {
      isLess = false;
   }
   else if (mJ2000UTC_Date == aRhs.mJ2000UTC_Date)
   {
      if (mSecondsPastMidnight >= aRhs.mSecondsPastMidnight)
      {
         isLess = false;
      }
   }
   return isLess;
}

//! Return the standard TLE epoch associated with the calendar time.
//!@note To convert to a "modified" TLE epoch, add the current century (1900 or 2000) to the returned value.
double UtCalendar::GetEpoch() const
{
   // Get time since Jan 1, midnight, from the current Julian date.
   int    dayOfYear = GetDayOfYear();
   int    year      = (mYear % 100) * 1000;
   double epoch     = mSecondsPastMidnight / cSEC_IN_DAY + year + dayOfYear;
   return epoch;
}

//! Return the standard TLE Epoch day of the year.
double UtCalendar::GetEpochDayOfYear() const
{
   // Get time since Jan 1, midnight, from the current Julian date.
   int    dayOfYear = GetDayOfYear();
   double epochDOY  = mSecondsPastMidnight / cSEC_IN_DAY + dayOfYear;
   return epochDOY;
}

/**Set the epoch, in TLE (Two-Line Element) format, or  a modified TLE format.
   This format is as follows:
   \li   YYDDD.FFFFFFFF (TLE format)
   \li YYYYDDD.FFFFFFFF (modified TLE format)
   where YY is a two-digit year, or alternatively, YYYY is a four digit year;
   DDD is a three-digit day of the year (001 - 365); and FFFFFFFF is an eight-digit
   fractional day referenced to UT midnight.
   @param aEpoch The epoch time in a TLE or modified TLE format.
*/
bool UtCalendar::SetEpoch(double aEpoch)
{
   int    year             = static_cast<int>(aEpoch / 1000);
   int    dayOfYear        = static_cast<int>(aEpoch - year * 1000);
   double secsPastMidnight = (aEpoch - static_cast<int>(aEpoch)) * cSEC_IN_DAY; // Ref to UTC
   if (year < 57)
   {
      year += 2000;
   }
   else if (year < 100)
   {
      year += 1900;
   }
   bool isValid = ((year >= 0) && (year < 10000) && // arbitrary upper limit.
                   (dayOfYear < 367));
   if (isValid)
   {
      mYear  = year;
      mMonth = 1;
      mDay   = 1;
      IncrementDay(dayOfYear - 1);
      SetTime(secsPastMidnight);
   }

   return isValid;
}

/** Set the delta UT1 time (difference between UT1 and UTC, the absolute value of which is usually less than one second).
   @note Internally this is stored in fractional day format.
*/
void UtCalendar::SetDeltaUT1(double aDeltaUT1)
{
   double deltaUT1 = aDeltaUT1 / cSEC_IN_DAY; // sec->days
   double dDelUT1  = mDeltaUT1 - deltaUT1;
   mDeltaUT1       = deltaUT1;
   mDeltaT += dDelUT1;
}

/** Set the delta AT time (difference between Atomic time and UTC).
   @note Internally this is stored in fractional day format.
*/
void UtCalendar::SetDeltaAT(int aDeltaAT)
{
   mDeltaT = (32.184 + static_cast<double>(aDeltaAT)) / cSEC_IN_DAY - mDeltaUT1;
}

/** Advance the calendar time by a specified number of seconds.

    Use this method in a time-based simulation to mark the advance of the
    simulation clock.
    @param aDeltaSeconds The number of seconds by which the calendar is advanced.
*/
void UtCalendar::AdvanceTimeBy(double aDeltaSeconds)
{
   if (aDeltaSeconds > 0.0)
   {
      if (aDeltaSeconds > cSEC_IN_DAY)
      {
         int days = static_cast<int>(aDeltaSeconds / cSEC_IN_DAY);
         IncrementDay(days);
         aDeltaSeconds -= days * cSEC_IN_DAY;
      }
      mSecond += aDeltaSeconds;
      mSecondsPastMidnight += aDeltaSeconds;
      mJ2000UTC_Date += aDeltaSeconds / cSEC_IN_DAY;
      int deltaMinutes = (int)(mSecond / 60.0);
      if (deltaMinutes > 0)
      {
         mMinute += deltaMinutes;
         mSecond -= deltaMinutes * 60.0;
         int deltaHours = mMinute / 60;
         if (deltaHours > 0)
         {
            mHour += deltaHours;
            mMinute %= 60;
            if (mHour > 23)
            {
               mHour %= 24;
               mSecondsPastMidnight -= cSEC_IN_DAY;
               IncrementDay(1);
            }
         }
      }
   }
   else if (aDeltaSeconds < 0.0)
   {
      if (aDeltaSeconds < -cSEC_IN_DAY)
      {
         int days = static_cast<int>(aDeltaSeconds / cSEC_IN_DAY);
         IncrementDay(days);
         aDeltaSeconds -= days * cSEC_IN_DAY;
      }
      mSecond += aDeltaSeconds;
      mSecondsPastMidnight += aDeltaSeconds;
      mJ2000UTC_Date += aDeltaSeconds / cSEC_IN_DAY;
      int deltaMinutes = (int)(mSecond / 60.0);
      mSecond -= deltaMinutes * 60.0;
      if (mSecond < 0.0)
      {
         mSecond += 60.0;
         deltaMinutes -= 1;
      }
      if (deltaMinutes < 0)
      {
         mMinute += deltaMinutes;
         int deltaHours = mMinute / 60;
         mMinute -= deltaHours * 60;
         if (mMinute < 0)
         {
            mMinute += 60;
            deltaHours -= 1;
         }
         if (deltaHours < 0)
         {
            mHour += deltaHours;
            if (mHour < 0)
            {
               mHour += 24;
               mSecondsPastMidnight += cSEC_IN_DAY;
               IncrementDay(-1);
            }
         }
      }
   }
}

// private
// Increment the day, Julian date, month, and year to reflect an advance of the given number of days.
void UtCalendar::IncrementDay(int aNumDays)
{
   mIntegralDay += aNumDays;

   if (aNumDays > 0)
   {
      while (aNumDays > 0)
      {
         ++mDay;
         --aNumDays;
         bool incrementMonth = (mDay > cDAYS_PER_MONTH[mMonth]);
         // check for leap year
         if ((mMonth == 2) && (IsLeapYear()))
         {
            incrementMonth = mDay > 29;
         }
         if (incrementMonth)
         {
            mDay = 1;
            ++mMonth;
            bool incrementYear = (mMonth > 12);
            if (incrementYear)
            {
               ++mYear;
               mMonth = 1;
            }
         }
      }
   }
   else
   {
      while (aNumDays < 0)
      {
         --mDay;
         ++aNumDays;
         bool decrementMonth = (mDay < 1);
         if (decrementMonth)
         {
            --mMonth;
            // check for leap year
            if ((mMonth == 2) && (IsLeapYear()))
            {
               mDay = 29;
            }
            else
            {
               bool decrementYear = (mMonth < 1);
               if (decrementYear)
               {
                  --mYear;
                  mMonth = 12;
               }
               mDay = cDAYS_PER_MONTH[mMonth];
            }
         }
      }
   }
   SetJ2000_Date();
}

/**
   Return the time since the input calendar date and time.
   @return The time difference in seconds.
*/
double UtCalendar::GetTimeSince(const UtCalendar& aRefCalendar) const
{
   double timeSince = mSecondsPastMidnight - aRefCalendar.mSecondsPastMidnight;
   if (mIntegralDay != aRefCalendar.mIntegralDay)
   {
      timeSince += (mIntegralDay - aRefCalendar.mIntegralDay) * cSEC_IN_DAY;
   }
   return timeSince;
}

/** Return the earth angle corresponding to the current UT of the calendar.
    @return Earth angle in radians.
    @note This version uses the approximate calculation for GAST but is much faster than GetEarthAngle.
*/
double UtCalendar::GetEarthAngleApprox() const
{
   return (GetGAST_Approx() / cSEC_IN_DAY * UtMath::cTWO_PI);
}

/** Return the earth angle corresponding to the current UT of the calendar.
    @return Earth angle in radians.
    @note This version uses the precise calculation for GAST but is much slower than GetEarthAngleApprox.
*/
double UtCalendar::GetEarthAngle() const
{
   return (ut::EarthCentralBody::GetGAST(*this) / cSEC_IN_DAY * UtMath::cTWO_PI);
}

/**
   Return the Greenwich Mean sidereal time (GMST) for the calendar time and date.
   @return The GMST for the current time and date in seconds.
   @note This method carries the signature extension "_1982" according to the convention
   by Vallado (Fundamentals of Astrodynamics and Applications, 4th ed., p. 303, eq. 5-4.), p.233,
   to distinguish it from the updated algorithm in GetGMST.  This method
   is used in the True Equator Mean Equinox (TEME) coordinate conversions (see UtECI_Conversion).
*/
double UtCalendar::GetGMST_1982() const
{
   // Reference:  The 1992 Astronomical Almanac, page B6.
   static const double omega_E = UtEarth::cOMEGA / UtMath::cTWO_PI * cSEC_IN_DAY;
   // Earth rotations per sidereal day (non-constant)
   double UT    = fmod(GetJ2000_Date() + .5, 1.);
   double t_cen = (GetJ2000_Date() - UT) / 36525.;
   double gmst  = 24110.54841 + t_cen * (8640184.812866 + t_cen * (0.093104 - t_cen * 6.2E-6));
   gmst         = fmod(gmst + cSEC_IN_DAY * omega_E * UT, cSEC_IN_DAY);
   if (gmst < 0.0)
   {
      gmst += cSEC_IN_DAY;
   }
   return gmst;
}

/**
   Return the Greenwich Mean sidereal time (GMST) for the calendar time and date.
   @return The GMST for the current time and date in seconds.
   @note  The following algorithm is somewhat less precise but is easier to understand:
   \code
   double d = Julian date - cJD_J2000; // Number of days since 2000 January 1, 12h UT (d0)
   static const double hoursPerSiderealDay = 24.06570982441908;
   // GMST is the number of sidereal days since d0, plus the reference offset at d0.
   double gmst = 18.697374558 + hoursPerSiderealDay * d;
   gmst = fmod(gmst, 24.0)*cSEC_IN_HOUR;
   if (gmst < 0.0)
   {
      gmst+=cSEC_IN_DAY;
   }
   return gmst;
   \endcode
*/
double UtCalendar::GetGMST() const
{
   // Reference:  USNO Circular #179 (2005), Equation 2.12 (optimized to not store powers in time):
   double                  t_cen              = GetJ2000TT_Date() / 36525.;
   static constexpr double cSEC_PER_ARCSECOND = 1.0 / 15.0;
   double                  earthAngle =
      (fmod(0.7790572732640 + 0.00273781191135448 * (GetJ2000_Date()) + fmod(GetJ2000_Date(), 1.0), 1.0)) * UtMath::cTWO_PI;
   double gmst =
      86400 * earthAngle / UtMath::cTWO_PI +
      cSEC_PER_ARCSECOND *
         (0.014506 +
          ((((-0.0000000368 * t_cen - 0.000029956) * t_cen - 0.00000044) * t_cen + 1.3915817) * t_cen + 4612.156534) *
             t_cen);
   gmst = fmod(gmst, cSEC_IN_DAY);
   if (gmst < 0.0)
   {
      gmst += cSEC_IN_DAY;
   }
   return gmst;
}

/**
   Return the Greenwich Apparent sidereal time (GAST) for the calendar time and date.
   @return The GAST for the current time and date in seconds.
   @note The calculation of the equation of the equinoxes (eqeq) is approximate,
         hence the "_Approx" signature extension. The GetGast() method provides
         a precise calculation.
*/
double UtCalendar::GetGAST_Approx() const
{
   // Compute the GAST correction to GMST.
   // psi -0.000319 sin O - 0.000024 sin 2L
   double d       = GetJ2000_Date(); // days since 2000 January 1, 12h UT
   double O       = 125.04 - 0.052954 * d;
   double L       = 280.47 + 0.98565 * d;
   double delPsi  = -0.000319 * sin(O * UtMath::cRAD_PER_DEG) - 0.000024 * sin(2.0 * L * UtMath::cRAD_PER_DEG);
   double epsilon = 23.4393 - 0.0000004 * d;
   double eqeq    = delPsi * cos(epsilon * UtMath::cRAD_PER_DEG);
   // add the GAST correction to GMST.
   double gast = GetGMST() + eqeq * cSEC_IN_HOUR;
   return gast;
}

/**
   Return the Greenwich Apparent sidereal time (GAST) for the calendar time and date.
   @return The GAST for the current time and date in seconds.
   @note This version produces a precise result but is computationally intensive.
*/
double UtCalendar::GetGAST() const
{
   return ut::EarthCentralBody::GetGAST(*this);
}

/** Return the Julian date (number of days since 12:00:00 UTC on 1 January 4713 BC), referenced to UT1.

  For example:
   - 12:00:00 UTC on 1 January 2000 is cJD_J2000
   - 00:00:00 UTC on 1 January 2000 is 2451544.5
*/
double UtCalendar::GetJulianDate() const
{
   return GetJ2000_Date() + cJD_J2000;
}

/** Return the Julian date (number of days since 12:00:00 UTC on 1 January 4713 BC), referenced to UTC.

  For example:
   - 12:00:00 UTC on 1 January 2000 is 2451545.0
   - 00:00:00 UTC on 1 January 2000 is 2451544.5
*/
double UtCalendar::GetJulianUTC_Date() const
{
   return GetJ2000UTC_Date() + cJD_J2000;
}

/** Return the Julian date (number of days since 12:00:00 UTC on 1 January 4713 BC), referenced to TT.

  For example:
   - 12:00:00 UTC on 1 January 2000 is 2451545.0
   - 00:00:00 UTC on 1 January 2000 is 2451544.5
*/
double UtCalendar::GetJulianTT_Date() const
{
   return GetJ2000TT_Date() + cJD_J2000;
}

/** Return the Julian date (number of days since 12:00:00 UTC on 1 January 4713 BC), referenced to TDB.
 */
double UtCalendar::GetJulianTDB_Date() const
{
   return GetJ2000TDB_Date() + cJD_J2000;
}

/** Return the J2000 Date (number of days since 12:00:00 UTC on 1 January 2000).
 */
double UtCalendar::GetJ2000UTC_Date() const
{
   return mJ2000UTC_Date;
}

/** Return the J2000 Date (number of days since 12:00:00 UTC on 1 January 2000),
   referenced to UT1.
*/
double UtCalendar::GetJ2000_Date() const
{
   return (mJ2000UTC_Date + mDeltaUT1);
}

/** Return the J2000 Date (number of days since 12:00:00 UTC on 1 January 2000),
   referenced to Terrestrial Time (TT).
*/
double UtCalendar::GetJ2000TT_Date() const
{
   return (GetJ2000_Date() + mDeltaT);
}

/** Return the J2000 Date (number of days since 12:00:00 UTC on 1 January 2000),
   referenced to Barycentric Time (TDB).  This date is used in some calculations,
   including nutation elements and GMST.
*/
double UtCalendar::GetJ2000TDB_Date() const
{
   return (GetJ2000TT_Date() + GetDeltaTDB() / cSEC_IN_DAY);
}

/** Return the number of days since November 17, 1858, 0h UT (= JD2400000.5).
 */
// static
double UtCalendar::GetModifiedJulianDate(int    aYear,
                                         int    aMonth,
                                         int    aDay,
                                         double aSecondsPastMidnight,
                                         double aDeltaUT1) // = 0.0)
{
   double j2000_Date = GetJ2000_Date(aYear, aMonth, aDay, aSecondsPastMidnight, aDeltaUT1);
   return (j2000_Date + 51544.5);
}

/** Return the number of days since November 17, 1858, 0h UT (= JD2400000.5).
 */
double UtCalendar::GetModifiedJulianDate() const
{
   return (GetJ2000_Date() + 51544.5);
}

/** Return the number of days since January 1, 1970, 0h UT (= JD2440587.5).
 */
double UtCalendar::GetModernJulianDate() const
{
   return (GetJ2000_Date() + 10957.5);
}

/** Return the difference between TDB and TT (TDB - TT).
 */
double UtCalendar::GetDeltaTDB() const
{
   double t = (GetJ2000TT_Date()) / 36525.0;

   /*
      Expression given in USNO Circular 179, eq. 2.6.
   */

   double secDiff = 0.001657 * sin(628.3076 * t + 6.2401) + 0.000022 * sin(575.3385 * t + 4.2970) +
                    0.000014 * sin(1256.6152 * t + 6.1969) + 0.000005 * sin(606.9777 * t + 4.0212) +
                    0.000005 * sin(52.9691 * t + 0.4444) + 0.000002 * sin(21.3299 * t + 5.5431) +
                    0.000010 * t * sin(628.3076 * t + 4.2490);

   return secDiff;
}

/** Return the difference between Atomic Time (AT) and UTC.
 */
int UtCalendar::GetDeltaAT() const
{
   return static_cast<int>((mDeltaT + mDeltaUT1) * cSEC_IN_DAY - 32.184);
}

/** Return the number of days since January 1, 1970, 0h UT (= JD2440587.5).
 */
// static
double UtCalendar::GetModernJulianDate(int    aYear,
                                       int    aMonth,
                                       int    aDay,
                                       double aSecondsPastMidnight,
                                       double aDeltaUT1) // = 0.0)
{
   double julianDate = GetJulianDate(aYear, aMonth, aDay, aSecondsPastMidnight, aDeltaUT1);
   return (julianDate - 2440587.5);
}

/** Get the enumeration value for the day of the week.  Returns 'true' if the string is a valid day of week.
 */
// static
bool UtCalendar::ReadDayOfWeek(std::string& aDayOfWeek, DayOfWeek& aEnum)
{
   for (int i = 0; i < 7; ++i)
   {
      if (cWEEK_DAY_NAMES[i] == aDayOfWeek)
      {
         aEnum = (DayOfWeek)i;
         return true;
      }
   }
   return false;
}

/** Return the Julian date (number of days since 12:00:00 UTC on 1 January 4713 BC).

  For example:
   - 12:00:00 UTC on 1 January 2000 is 2451545.0
   - 00:00:00 UTC on 1 January 2000 is 2451544.5

  This is a convenience method which returns the Julian date using values passed
  as arguments rather than the values stored in the object.
*/
// static
double UtCalendar::GetJulianDate(int    aYear,
                                 int    aMonth,
                                 int    aDay,
                                 double aSecondsPastMidnight,
                                 double aDeltaUT1) // = 0.0)
{
   return (GetJ2000_Date(aYear, aMonth, aDay, aSecondsPastMidnight, aDeltaUT1) + cJD_J2000);
}

/** Return the J2000 date (number of days since 12:00:00 UT1 on 1 January 2000).

  This is a convenience method which returns the J2000 date using values passed
  as arguments rather than the values stored in the object.
*/
// static
double UtCalendar::GetJ2000_Date(int    aYear,
                                 int    aMonth,
                                 int    aDay,
                                 double aSecondsPastMidnight,
                                 double aDeltaUT1) // = 0.0)
{
   double j2000 = 367 * aYear - 7 * (aYear + (aMonth + 9) / 12) / 4 - 3 * (((aYear + (aMonth - 9) / 7) / 100) + 1) / 4 +
                  275 * aMonth / 9 + aDay;
   j2000 += ((aSecondsPastMidnight + aDeltaUT1) / 86400.0) - 730516.5;
   return j2000;
}

/** Get the current date, referenced to the given GMT offset.
   @param aGMT_Offset The GMT offset to local time (for instance, CDT is -5).
   @param aYear       The current year (output).
   @param aMonth      The current month (output).
   @param aDay        The current day (output).
*/
void UtCalendar::GetLocalDate(int aGMT_Offset, int& aYear, int& aMonth, int& aDay) const
{
   UtCalendar localTime(*this);
   localTime.AdvanceTimeBy(aGMT_Offset * cSEC_IN_HOUR);
   aYear  = localTime.mYear;
   aMonth = localTime.mMonth;
   aDay   = localTime.mDay;
}

/** Get the current date.
   @param aYear   The current year (output).
   @param aMonth  The current month (output).
   @param aDay    The current day (output).
*/
void UtCalendar::GetDate(int& aYear, int& aMonth, int& aDay) const
{
   aYear  = mYear;
   aMonth = mMonth;
   aDay   = mDay;
}

/** Get the time as the number of hours, minutes and seconds past midnight.
   @param aHour   The current hour (output).
   @param aMinute The current minute (output).
   @param aSecond The current second (output).
*/
void UtCalendar::GetTime(int& aHour, int& aMinute, double& aSecond) const
{
   aHour   = mHour;
   aMinute = mMinute;
   aSecond = mSecond;
}

/** Get the current day of the week
 */
UtCalendar::DayOfWeek UtCalendar::GetDayOfWeek() const
{
   static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
   int        y   = mYear;
   int        m   = mMonth;
   int        d   = mDay;
   if (m < 3)
   {
      --y;
   }
   return (DayOfWeek)((y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7);
}

/** Get the time as the number of hours, minutes and seconds past midnight,
   taking into account a GMT offset.
   @param aGMT_Offset The offset in hours from Greenwich Mean Time.  For example,
   Central Daylight Time is offset -5 from GMT.  This offset must not exceed +-12 hours.
   @param aHour      The current hour (output).
   @param aMinute    The current minute (output).
   @param aSecond    The current second (output).
*/
void UtCalendar::GetLocalMeanTime(int aGMT_Offset, int& aHour, int& aMinute, double& aSecond) const
{
   double localSecondsPastMidnight = GetLocalMeanTime(aGMT_Offset);
   aHour                           = static_cast<int>(localSecondsPastMidnight) / cSEC_IN_HOUR;
   aMinute                         = static_cast<int>(localSecondsPastMidnight) / cSEC_IN_MIN - (aHour * cMIN_IN_HOUR);
   aSecond                         = localSecondsPastMidnight - (aHour * cSEC_IN_HOUR) - (aMinute * cSEC_IN_MIN);
}

/** Get the time as the number of seconds past midnight of the current day.
   (i.e. Midnight= 0.0, 1AM=3600.0, Noon=43200.0, 11:59:59PM=86399.0)
*/
double UtCalendar::GetTime() const
{
   return mSecondsPastMidnight;
}

/** Get the time as the number of seconds past midnight of the current day,
    taking into account a GMT offset.
   (i.e. local Midnight= 0.0, 1AM=3600.0, Noon=43200.0, 11:59:59PM=86399.0)
   @param aGMT_Offset The offset in hours from Greenwich Mean Time.  For example,
   Central Daylight Time is offset -5 from GMT.  This offset must not exceed +-12 hours.
   @return The number of seconds past midnight of the current day, according to local
   mean time.
*/
double UtCalendar::GetLocalMeanTime(int aGMT_Offset) const
{
   double localSecsPastMidnight = mSecondsPastMidnight + (aGMT_Offset * cSEC_IN_HOUR);
   if (localSecsPastMidnight > cSEC_IN_DAY)
   {
      localSecsPastMidnight -= cSEC_IN_DAY;
   }
   else if (localSecsPastMidnight < 0.0)
   {
      localSecsPastMidnight += cSEC_IN_DAY;
   }
   return localSecsPastMidnight;
}

/** Return the integer day of the current calendar year. */
int UtCalendar::GetDayOfYear() const
{
   int dayOfYear = cDAYS_BEFORE_MONTH[mMonth];
   dayOfYear += mDay;
   if (mMonth > cFEBRUARY && IsLeapYear())
   {
      ++dayOfYear;
   }
   return dayOfYear;
}

/** Return 'true' if aYear is a leap year */
bool UtCalendar::IsLeapYear(int aYear)
{
   // Assume that it is a leap year, but take into account that a century
   // year is NOT a leap year unless it is also divisible by 400.
   //
   // (1900 IS NOT a leap year, 2000 IS a leap year)

   return (((aYear % 4) == 0) && (((aYear % 100) != 0) || ((aYear % 400) == 0)));
}

/** Return whether the current calendar year is a leap year. */
bool UtCalendar::IsLeapYear() const
{
   return IsLeapYear(mYear);
}

/** Set the current date and time (UTC) based on the system clock */
void UtCalendar::SetCurrentDateAndTime()
{
   auto timePointNow   = std::chrono::system_clock::now();
   auto timeSinceEpoch = timePointNow.time_since_epoch();
   auto tvalue         = std::chrono::system_clock::to_time_t(timePointNow);

   // Convert from time_t to struct tm
   // Note: Once c++ 20 becomes available, duration_cast may be used to retrieve month, day, year.
   struct tm* now = std::gmtime(&tvalue);
   mDay           = now->tm_mday;        // 1-31
   mMonth         = now->tm_mon + 1;     // 0-11
   mYear          = now->tm_year + 1900; // Past 1900

   mHour   = now->tm_hour;
   mMinute = now->tm_min;
   mSecond = now->tm_sec;

   auto fractionalSeconds =
      static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeSinceEpoch).count() % 1000000000) / 1.0e9;
   mSecond += fractionalSeconds;

   // Set seconds since midnight and Julian date
   SetTime(mHour, mMinute, mSecond);
}

/** Set the current date.
   See the NOTE at the top of the file concerning the valid values.
   @param aYear   The year to which the calendar will be set.
   @param aMonth  The month to which the calendar will be set.
   @param aDay    The day to which the calendar will be set.
*/
bool UtCalendar::SetDate(int aYear, int aMonth, int aDay)
{
   bool ok = false;
   if ((aMonth >= 1) && (aMonth <= 12) && (aDay >= 1) && (aYear >= 1900))
   {
      // Assume January, March, May, July, August, October or December
      int maxDay = cDAYS_PER_MONTH[aMonth];
      if (aMonth == cFEBRUARY && IsLeapYear(aYear))
      {
         maxDay = 29;
      }
      if (aDay <= maxDay)
      {
         ok     = true;
         mYear  = aYear;
         mMonth = aMonth;
         mDay   = aDay;
         SetJ2000_Date();
      }
   }
   return ok;
}

/** Set the current time as the number of seconds past midnight of the current day
   @param aSecondsPastMidnight    The seconds past midnight (referred to UTC) to which the calendar will be set.
*/
bool UtCalendar::SetTime(double aSecondsPastMidnight)
{
   if ((aSecondsPastMidnight >= 0.0) && (aSecondsPastMidnight < 86401.0))
   {
      if (aSecondsPastMidnight >= 86400.0 && !IsValidLeapSecondDate(mMonth, mDay))
      {
         auto out = ut::log::error() << "Invalid time point for leap second.";
         out.AddNote() << "Year: " << mYear;
         out.AddNote() << "Month: " << mMonth;
         out.AddNote() << "Day: " << mDay;
         out.AddNote() << "Total Seconds: " << aSecondsPastMidnight;
         return false;
      }
      mSecondsPastMidnight = aSecondsPastMidnight;
      mHour                = static_cast<int>(aSecondsPastMidnight / 3600.0);
      mMinute              = static_cast<int>(aSecondsPastMidnight / 60.0 - mHour * 60);
      mSecond              = aSecondsPastMidnight - mHour * 3600 - mMinute * 60;
      SetJ2000_Date();
      return true;
   }
   return false;
}

/** Set the current time as the number of hours, minutes and seconds past the hour.
   @param aHour      The hour to which the calendar will be set.
   @param aMinute    The minute to which the calendar will be set.
   @param aSecond    The second to which the calendar will be set.
*/
bool UtCalendar::SetTime(int aHour, int aMinute, double aSecond)
{
   if ((aHour >= 0) && (aHour <= 23) && (aMinute >= 0) && (aMinute <= 59) && (aSecond >= 0.0) && (aSecond < 61.0))
   {
      if (aSecond >= 60.0 && !IsValidLeapSecondTimePoint(mMonth, mDay, aHour, aMinute))
      {
         auto out = ut::log::error() << "Invalid time point for leap second.";
         out.AddNote() << "Year: " << mYear;
         out.AddNote() << "Month: " << mMonth;
         out.AddNote() << "Day: " << mDay;
         out.AddNote() << "Setting time: " << aHour << ":" << aMinute << ":" << aSecond;
         return false;
      }
      mHour   = aHour;
      mMinute = aMinute;
      mSecond = aSecond;

      mSecondsPastMidnight = (aHour * 3600) + (aMinute * 60) + aSecond;
      SetJ2000_Date();
      return true;
   }
   return false;
}

//! Provide a diagnostic output.
std::ostream& operator<<(std::ostream& os, const UtCalendar& aCalendar)
{
   os.precision(12);
   os << "Year=" << aCalendar.mYear << " Month=" << aCalendar.mMonth << " Day=" << aCalendar.mDay;
   os << " Hour=" << aCalendar.mHour << " Minute=" << aCalendar.mMinute << " Second=" << aCalendar.mSecond;
   os << " PastMidnight=" << aCalendar.mSecondsPastMidnight;
   os << " J2000_Date=" << aCalendar.GetJ2000_Date();
   os << " Epoch= " << aCalendar.GetEpoch() << "\n";
   os.precision(6);
   return os;
}

//! Given a valid Julian date, set the proper day, month, year, and seconds past midnight.
//! @param aJulianDate The Julian date, referenced to UT1, to be set in the object.
void UtCalendar::SetJulianDate(double aJulianDate)
{
   // From http://quasar.as.utexas.edu/BillInfo/JulianDatesG.html
   /*
     Z = JD+0.5
     W = (Z - 1867216.25)/36524.25
     X = W/4
     A = Z+1+W-X
     B = A+1524
     C = (B-122.1)/365.25
     D = 365.25xC
     E = (B-D)/30.6001
     F = 30.6001xE
     Day of month = B-D-F
     Month = E-1 or E-13 (must get number less than or equal to 12)
     Year = C-4715 (if Month is January or February) or C-4716 (otherwise)
   */
   double Z = aJulianDate + 0.5;
   int    W = (int)((Z - 1867216.25) / 36524.25);
   int    X = W / 4;
   int    A = (int)Z + 1 + W - X;
   int    B = A + 1524;
   int    C = (int)(((double)B - 122.1) / 365.25);
   int    D = (int)(365.25 * (double)C);
   int    E = (int)((double)(B - D) / 30.6001);
   int    F = (int)(30.6001 * (double)E);
   mDay     = B - D - F;
   mMonth   = E - 1;
   if (mMonth > 12)
   {
      mMonth = E - 13;
   }
   if (mMonth < 3)
   {
      mYear = C - 4715;
   }
   else
   {
      mYear = C - 4716;
   }
   SetTime((Z - (int)Z) * 86400.0); // Ref to UT1
   AdvanceTimeBy(-mDeltaUT1 * cSEC_IN_DAY);
}

//! Given a valid J2000 date, set the proper day, month, year, and seconds past midnight
void UtCalendar::SetJ2000_Date(double aJ2000_Date)
{
   SetJulianDate(aJ2000_Date + cJD_J2000);
}

// private
void UtCalendar::SetJ2000_Date()
{
   mJ2000UTC_Date = GetJ2000_Date(mYear, mMonth, mDay, mSecondsPastMidnight);
   mIntegralDay   = static_cast<int>(mJ2000UTC_Date + 0.5);
}

// static
//! Return the numeric month (1-12) equivalent to the provided string.
//! Valid months are expressed in the following three character formats:
//! jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec.
//! @return The numeric month (1-12).  If the provided string is not valid, 0 is returned.
int UtCalendar::GetMonth(const std::string& aMonth)
{
   std::string word(aMonth);
   UtStringUtil::ToLower(word);
   int month = 0;
   for (int i = 1; i <= 12; ++i)
   {
      if (word == cMONTH_NAMES[i])
      {
         month = i;
      }
   }
   return month;
}

// static
//! Return the time as a string in HH:MM:SS.ffff format.
bool UtCalendar::GetTime(const std::string& aHMS, int& aHour, int& aMinute, double& aSecond)
{
   char               ch1;
   char               ch2;
   std::istringstream iss(aHMS);
   return ((iss >> aHour) && (iss >> ch1) && (ch1 == ':') && (iss >> aMinute) && (iss >> ch2) && (ch2 == ':') &&
           (iss >> aSecond));
}

/**
Return a string representation in ISO 8601 format (YYYY-MM-DDTHH:MM:SS.sssZ)
@return The ISO 8601 string representation
@note The generated string always uses UTC/Zulu.
*/
std::string UtCalendar::GetISO8601(int aFractionalSecondDigits, bool aUseFullFormat) const
{
   std::ostringstream iso;
   iso << std::setfill('0');
   iso << std::setw(4) << mYear;
   if (aUseFullFormat)
   {
      iso << "-";
   }
   iso << std::setw(2) << mMonth;
   if (aUseFullFormat)
   {
      iso << "-";
   }
   iso << std::setw(2) << mDay;
   iso << "T";
   iso << std::setw(2) << mHour;
   if (aUseFullFormat)
   {
      iso << ":";
   }
   iso << std::setw(2) << mMinute;
   if (aUseFullFormat)
   {
      iso << ":";
   }
   iso << std::setw(2) << (int)mSecond;
   if (0 < aFractionalSecondDigits)
   {
      int fracSeconds = (int)((mSecond - (int)mSecond) * std::pow(10, aFractionalSecondDigits));
      iso << "." << std::setw(aFractionalSecondDigits) << fracSeconds;
   }
   iso << "Z";
   return iso.str();
}

/**
Return a UtCalendar representation of aDateTime, which must be in a supported ISO 8601 timestamp format.
Comprehensive support for the ISO 8601 specification is not intended. Supported formats are primarily
derived from the RFC 3339 and W3 subsets. Specifically, formats of YYYY-MM-DD, YYYY-MM-DDTHH:MMZ, YYYY-MM-DDTHH:MM:SSZ,
and YYYY-MM-DDTHH:MM:SS.sssZ are supported, along with the basic versions thereof (without '-' or ':').
Years of 1900 and earlier are not supported.
@note Only UTC/Zulu time is supported (i.e. no time zone offsets).
@return UtCalendar at the time specified by aDateTime
@throws if aDateTime is an unsupported or unrecognized value or format
*/
UtCalendar UtCalendar::ParseISO8601(const std::string& aDateTime)
{
   bool validParse = false;

   int    year    = 0;
   int    month   = 0;
   int    day     = 0;
   int    hours   = 0;
   int    minutes = 0;
   double seconds = 0.0;

   // Define supported ISO 8601 formats
   constexpr const char* ISO8601_SCANFPATTERN_FULL     = "%4d-%2d-%2dT%2d:%2d:%lfZ"; // YYYY-MM-DDTHH:MM:SS.sssZ
   constexpr const char* ISO8601_SCANFPATTERN_BASIC    = "%4d%2d%2dT%2d%2d%lfZ";     // YYYYMMDDTHHMMSS.sssZ
   constexpr const char* ISO8601_SCANFPATTERN_ORDER[2] = {ISO8601_SCANFPATTERN_FULL, ISO8601_SCANFPATTERN_BASIC};

   // Extract the numeric values from the string input.
   // Note that using std::get_time was found insufficient since it doesn't provide standardized error indication,
   // and at least MSVC parsed partial and invalid fields without mechanisms for error checking or handling.

   for (auto format : ISO8601_SCANFPATTERN_ORDER)
   {
      int scannedFields = std::sscanf(aDateTime.c_str(), format, &year, &month, &day, &hours, &minutes, &seconds);

      // Only a full specification of date is supported
      bool foundDate = (scannedFields == 3);
      // Supported time formats: HH:MM, HH:MM:SS, HH:MM:SS.sss (or corresponding basic versions)
      bool foundDateTime = (scannedFields == 5 || scannedFields == 6);
      // If a time is provided, require that the Zulu indicator be provided
      bool foundDateTimeWithZone = (foundDateTime && aDateTime.back() == 'Z');
      if (foundDate || foundDateTimeWithZone)
      {
         validParse = true;
         break;
      }
   }

   if (!validParse)
   {
      throw UtException("Unrecognized ISO 8601 format (" + aDateTime + ")");
   }

   // UtCalendar doesn't support years 1900 or earlier
   if (year <= 1900)
   {
      throw UtException("Year values of 1900 or earlier are not supported (" + aDateTime + ")");
   }

   // Validation of values is provided by SetDate and SetTime
   UtCalendar result;
   if (!result.SetDate(year, month, day))
   {
      throw UtException("Invalid date specified (" + aDateTime + ")");
   }

   if (!result.SetTime(hours, minutes, seconds))
   {
      throw UtException("Invalid time specified (" + aDateTime + ")");
   }
   return result;
}
