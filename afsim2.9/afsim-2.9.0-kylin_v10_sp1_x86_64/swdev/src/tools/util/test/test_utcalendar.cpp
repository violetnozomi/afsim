// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//==============================================================================
// This software developed by Stellar Science Ltd Co and the U.S. Government.
// Copyright (C) 2018 Stellar Science; U.S. Government has Unlimited Rights.
// Warning: May contain EXPORT CONTROLLED, FOUO, ITAR, or sensitive information.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtException.hpp"

namespace
{
void testIso8601Match(const std::string& iso8601, int year, int month, int day, int hour, int minute, double seconds)
{
   auto result = UtCalendar::ParseISO8601(iso8601);
   EXPECT_EQ(result.GetYear(), year);
   EXPECT_EQ(result.GetMonth(), month);
   EXPECT_EQ(result.GetDay(), day);
   EXPECT_EQ(result.GetHour(), hour);
   EXPECT_EQ(result.GetMinute(), minute);
   EXPECT_DOUBLE_EQ(result.GetSecond(), seconds);
}
} // namespace

TEST(UtCalendar, EpochConversions)
{
   UtCalendar cal;
   cal.SetEpoch(0061.0); // march 1 2000 (leap year)
   EXPECT_NEAR(cal.GetEpoch(), 0061.0, 1.0e-8);
}

TEST(UtCalendar, ParseISO8601FullTimestamp)
{
   testIso8601Match("2018-01-01T00:00:00Z", 2018, 1, 1, 0, 0, 0.0);
   testIso8601Match("2018-07-12T04:02:23Z", 2018, 7, 12, 4, 2, 23.0);

   // Test valid parsing of leap seconds
   testIso8601Match("2018-06-30T23:59:60Z", 2018, 6, 30, 23, 59, 60.0);
   testIso8601Match("2018-06-30T23:59:60.456Z", 2018, 6, 30, 23, 59, 60.456);
   testIso8601Match("2018-12-31T23:59:60Z", 2018, 12, 31, 23, 59, 60);
   testIso8601Match("2018-12-31T23:59:60.999Z", 2018, 12, 31, 23, 59, 60.999);

   // Test without leading zeros for fields
   testIso8601Match("2018-1-1T0:0:0Z", 2018, 1, 1, 0, 0, 0.0);
   testIso8601Match("2018-7-12T4:2:23Z", 2018, 7, 12, 4, 2, 23.0);

   testIso8601Match("2018-01-01T16:42:16.689Z", 2018, 1, 1, 16, 42, 16.689);
   testIso8601Match("2018-01-01T16:42:16.689012Z", 2018, 1, 1, 16, 42, 16.689012);
}

TEST(UtCalendar, ParseISO8601BasicFormat)
{
   // Full timestamp in basic format (without delimiters)
   testIso8601Match("20180101T012345Z", 2018, 1, 1, 1, 23, 45.0);
   testIso8601Match("19991231T1547Z", 1999, 12, 31, 15, 47, 0.0);
   testIso8601Match("19991231", 1999, 12, 31, 0, 0, 0.0);

   testIso8601Match("20180101T012345.678Z", 2018, 1, 1, 1, 23, 45.678);
   testIso8601Match("20180101T012345.678901Z", 2018, 1, 1, 1, 23, 45.678901);
}

TEST(UtCalendar, ParseISO8601Partial)
{
   // For partially specified format, must ensure that non-specified fields are set to defaults
   testIso8601Match("1999-12-31T15:47Z", 1999, 12, 31, 15, 47, 0.0);
   testIso8601Match("1999-12-31", 1999, 12, 31, 0, 0, 0.0);
}

TEST(UtCalendar, ParseISO8601ValidButUnsupported)
{
   // Years prior to 1901 are unsupported by UtCalendar
   EXPECT_THROW(UtCalendar::ParseISO8601("1900"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("1899"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("1492"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("492"), UtException);

   // Atypical uses of fields are not yet supported
   EXPECT_THROW(UtCalendar::ParseISO8601("1999-12-31T15Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("1999-12"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("1999"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("1901"), UtException);

   // Technically valid, but nonexistent in practice leap second insertion on
   // months other than June and December
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-01-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-02-28T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-03-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-04-30T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-05-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-07-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-08-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-09-30T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-10-31T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-11-30T23:59:60Z"), UtException);
}

TEST(UtCalendar, ParseISO8601Invalid)
{
   // Completely invalid input
   EXPECT_THROW(UtCalendar::ParseISO8601(""), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("bogus"), UtException);

   // Correct except no timezone specified
   EXPECT_THROW(UtCalendar::ParseISO8601("1999-12-31T15:00"), UtException);

   // Out of date range values
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-13-01"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-12-32"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-02-30"), UtException);

   // Leap seconds must appear at the end of a month
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-06-30T00:00:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-06-30T00:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-06-30T23:00:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-06-29T23:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-12-31T00:00:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-12-31T00:59:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-12-31T23:00:60Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2018-12-30T23:59:60Z"), UtException);

   // Out of time range values
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T00:00:61Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T00:00:61.123Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T00:60:00Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T00:61:00Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T24:00:00Z"), UtException);
   EXPECT_THROW(UtCalendar::ParseISO8601("2019-01-01T25:00:00Z"), UtException);
}

TEST(UtCalendar, TestFormatISO8601)
{
   const std::string singleDigitDateZeroTime{"2019-01-01T00:00:00Z"};
   EXPECT_EQ(singleDigitDateZeroTime, UtCalendar::ParseISO8601(singleDigitDateZeroTime).GetISO8601(0));
   EXPECT_EQ("20190101T000000Z", UtCalendar::ParseISO8601(singleDigitDateZeroTime).GetISO8601(0, false));

   const std::string singleDigitDateSingleDigitTime{"2019-01-01T03:04:05Z"};
   EXPECT_EQ(singleDigitDateSingleDigitTime, UtCalendar::ParseISO8601(singleDigitDateSingleDigitTime).GetISO8601(0));
   EXPECT_EQ("20190101T030405Z", UtCalendar::ParseISO8601(singleDigitDateSingleDigitTime).GetISO8601(0, false));

   const std::string fullWidthDateAndTime{"2019-12-21T13:24:35Z"};
   EXPECT_EQ(fullWidthDateAndTime, UtCalendar::ParseISO8601(fullWidthDateAndTime).GetISO8601(0));
   EXPECT_EQ("20191221T132435Z", UtCalendar::ParseISO8601(fullWidthDateAndTime).GetISO8601(0, false));

   const std::string defaultPrecisionZeroSeconds{"2019-12-21T13:24:00.000Z"};
   EXPECT_EQ(defaultPrecisionZeroSeconds, UtCalendar::ParseISO8601(defaultPrecisionZeroSeconds).GetISO8601());
   EXPECT_EQ("20191221T132400.000Z", UtCalendar::ParseISO8601(defaultPrecisionZeroSeconds).GetISO8601(3, false));

   const std::string defaultPrecisionMilliSeconds{"2019-12-21T13:24:35.444Z"};
   EXPECT_EQ(defaultPrecisionMilliSeconds, UtCalendar::ParseISO8601(defaultPrecisionMilliSeconds).GetISO8601());
   EXPECT_EQ("20191221T132435.444Z", UtCalendar::ParseISO8601(defaultPrecisionMilliSeconds).GetISO8601(3, false));
}

TEST(UtCalendar, FormatISO8601VaryingSubsecondPrecision)
{
   const std::string baseTime  = "2019-01-01T03:04:05";
   const std::string calString = baseTime + ".1234567890Z";

   const std::string digits0 = baseTime + "Z";
   const std::string digits1 = baseTime + ".1Z";
   const std::string digits2 = baseTime + ".12Z";
   const std::string digits3 = baseTime + ".123Z";
   const std::string digits4 = baseTime + ".1234Z";
   const std::string digits5 = baseTime + ".12345Z";
   const std::string digits6 = baseTime + ".123456Z";
   const std::string digits7 = baseTime + ".1234567Z";

   const UtCalendar cal = UtCalendar::ParseISO8601(calString);

   EXPECT_EQ(digits0, cal.GetISO8601(0));
   EXPECT_EQ(digits0, cal.GetISO8601(-1));

   EXPECT_EQ(digits1, cal.GetISO8601(1));
   EXPECT_EQ(digits2, cal.GetISO8601(2));
   EXPECT_EQ(digits3, cal.GetISO8601(3));
   EXPECT_EQ(digits4, cal.GetISO8601(4));
   EXPECT_EQ(digits5, cal.GetISO8601(5));
   EXPECT_EQ(digits6, cal.GetISO8601(6));
   EXPECT_EQ(digits7, cal.GetISO8601(7));
}

TEST(UtCalendar, TimeSystemConversions)
{
   // From Vallado 4th Ed., Example 3-7, pages 195-196.
   // Starting point is May 14, 2004 10:43 MST -> Which is 16:43 UTC
   constexpr int    cDELTA_AT  = 32;        // [s] See Astronomical Almanac (as per Vallado)
   constexpr double cDELTA_UT1 = -0.463326; // [s] See USNO IERS Final Data (as per Vallado)
   UtCalendar       exampleEvent{cDELTA_UT1, cDELTA_AT};
   exampleEvent.SetDate(2004, 5, 14);
   exampleEvent.SetTime(16, 43, 0.0);

   // From the example, the results should be as follows:
   constexpr double cJD_TT             = 2453140.19727065;
   constexpr double cJD_TDB            = 2453140.197270667;
   constexpr double cALLOWED_TOLERANCE = 1.e-8; // The precision reported by Vallado

   EXPECT_NEAR(exampleEvent.GetJulianTT_Date(), cJD_TT, cALLOWED_TOLERANCE);
   EXPECT_NEAR(exampleEvent.GetJulianTDB_Date(), cJD_TDB, cALLOWED_TOLERANCE);
}

TEST(UtCalendar, TimeConstants)
{
   constexpr int    cDELTA_AT       = 37;
   constexpr double cDELTA_UT1      = 0.125;
   const double     cDELTA_UT1_DAYS = cDELTA_UT1 / UtCalendar::cSEC_IN_DAY;
   UtCalendar       test{cDELTA_UT1, cDELTA_AT};

   EXPECT_EQ(test.GetDeltaAT(), cDELTA_AT);
   EXPECT_NEAR(test.GetDeltaUT1(), cDELTA_UT1_DAYS, 1.0e-10);

   test.SetDeltaUT1(0);
   EXPECT_EQ(test.GetDeltaAT(), cDELTA_AT);
   EXPECT_NEAR(test.GetDeltaUT1(), 0, 1.0e-10);
}
