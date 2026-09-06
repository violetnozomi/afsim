// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <iostream>

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "WsfDateTime.hpp"


namespace
{
double secPerDay          = 3600.0 * 24.0;
double sixtyYears_Days    = 365.0 * 60.0;
double sixtyYears_Seconds = secPerDay * sixtyYears_Days;

double oneTenthSecond_Seconds = 0.1;
double oneTenthSecond_Days    = oneTenthSecond_Seconds / secPerDay;

// Inputs
double dAT = 18.0; // delta_atomic_time
double dUT = 0.75; // delta_universal_time

// Expected Values
int    deltaAT = static_cast<int>(dAT);
double deltaUT = dUT / secPerDay;
double deltaT  = (32.184 + dAT) / secPerDay - deltaUT;

double j2000DateUTC_Big = sixtyYears_Days;
double j2000Date_Big    = sixtyYears_Days + deltaUT;
double j2000DateTT_Big  = sixtyYears_Days + deltaUT + deltaT;

double j2000DateUTC_Small = oneTenthSecond_Days;
double j2000Date_Small    = oneTenthSecond_Days + deltaUT;
double j2000DateTT_Small  = oneTenthSecond_Days + deltaUT + deltaT;

} // namespace

TEST(DateTime, GetJulianDate)
{
   WsfDateTime  testTime;
   const double twelveHours = 12.0 * 60.0 * 60.0;
   // These two values were taken from the commnents on the function:
   EXPECT_DOUBLE_EQ(2451544.5, testTime.GetJulianDate(2000, 1, 1, 0));
   EXPECT_DOUBLE_EQ(2451545.0, testTime.GetJulianDate(2000, 1, 1, twelveHours));
   // Value of when the Julian calendar starts
   // TODO: Seems this doesn't work
   // EXPECT_DOUBLE_EQ(0.0, testTime.GetJulianDate(-4713, 1, 1, twelveHours));
}
//
TEST(DateTime, GetCurrentTime)
{
   std::stringstream inputStream{};
   inputStream << "delta_atomic_time " << static_cast<int>(dAT) << "\n";
   inputStream << "delta_universal_time " << dUT << " s\n";
   inputStream << "start_date jan 1 2000\n";
   UtInput input;
   input.PushInputString(inputStream.str());

   WsfDateTime testTime;
   std::string command;
   while (input.TryReadCommand(command))
   {
      ASSERT_TRUE(testTime.ProcessInput(input));
   }

   // Large sim time (60 years)
   UtCalendar calendar1 = testTime.GetCurrentTime(sixtyYears_Seconds);

   ASSERT_EQ(calendar1.GetJ2000UTC_Date(), j2000DateUTC_Big);
   ASSERT_EQ(calendar1.GetJ2000_Date(), j2000Date_Big);
   ASSERT_EQ(calendar1.GetJ2000TT_Date(), j2000DateTT_Big);
   ASSERT_EQ(calendar1.GetDeltaAT(), deltaAT);
   ASSERT_EQ(calendar1.GetDeltaUT1(), deltaUT);
   ASSERT_EQ(calendar1.GetDeltaT(), deltaT);

   UtCalendar calendar2;
   testTime.GetCurrentTime(sixtyYears_Seconds, calendar2);

   ASSERT_EQ(calendar2.GetJ2000UTC_Date(), j2000DateUTC_Big);
   ASSERT_EQ(calendar2.GetJ2000_Date(), j2000Date_Big);
   ASSERT_EQ(calendar2.GetJ2000TT_Date(), j2000DateTT_Big);
   ASSERT_EQ(calendar2.GetDeltaAT(), deltaAT);
   ASSERT_EQ(calendar2.GetDeltaUT1(), deltaUT);
   ASSERT_EQ(calendar2.GetDeltaT(), deltaT);

   // Small sim time (0.1 second)
   UtCalendar calendar3 = testTime.GetCurrentTime(oneTenthSecond_Seconds);

   ASSERT_EQ(calendar3.GetJ2000UTC_Date(), j2000DateUTC_Small);
   ASSERT_EQ(calendar3.GetJ2000_Date(), j2000Date_Small);
   ASSERT_EQ(calendar3.GetJ2000TT_Date(), j2000DateTT_Small);
   ASSERT_EQ(calendar3.GetDeltaAT(), deltaAT);
   ASSERT_EQ(calendar3.GetDeltaUT1(), deltaUT);
   ASSERT_EQ(calendar3.GetDeltaT(), deltaT);

   UtCalendar calendar4;
   testTime.GetCurrentTime(oneTenthSecond_Seconds, calendar4);

   ASSERT_EQ(calendar4.GetJ2000UTC_Date(), j2000DateUTC_Small);
   ASSERT_EQ(calendar4.GetJ2000_Date(), j2000Date_Small);
   ASSERT_EQ(calendar4.GetJ2000TT_Date(), j2000DateTT_Small);
   ASSERT_EQ(calendar4.GetDeltaAT(), deltaAT);
   ASSERT_EQ(calendar4.GetDeltaUT1(), deltaUT);
   ASSERT_EQ(calendar4.GetDeltaT(), deltaT);
}
