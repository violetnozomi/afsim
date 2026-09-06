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

#include "UtScriptCalendar.hpp"

#include <iomanip>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtException.hpp"
#include "UtMemory.hpp"
#include "UtScriptException.hpp"
#include "UtScriptMethodDefine.hpp"

UtScriptCalendar::UtScriptCalendar(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_WRAP_CLASS(UtCalendar);

   // Construct_Epoch
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtCalendar>, Construct, (double aEpoch))
   {
      auto calendarPtr = ut::make_unique<UtCalendar>();
      calendarPtr->SetEpoch(aEpoch);
      return calendarPtr;
   };

   // Construct_Current
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtCalendar>, Construct, ())
   {
      auto calendarPtr = ut::make_unique<UtCalendar>();
      calendarPtr->SetCurrentDateAndTime();
      return calendarPtr;
   };

   // Construct_Iso8601
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtCalendar>, Construct, (const std::string& Iso8601_Date))
   {
      try
      {
         auto calendarPtr = ut::make_unique<UtCalendar>(UtCalendar::ParseISO8601(Iso8601_Date));
         return calendarPtr;
      }
      catch (const UtException& e)
      {
         std::ostringstream oss;
         oss << "Failed to construct Calendar: " << e.what();
         UT_SCRIPT_ABORT(oss.str());
      }
   };

   UT_SCRIPT_WRAP_MEMBER("AdvanceTime", AdvanceTimeBy);
   UT_SCRIPT_WRAP_MEMBER("Epoch", GetEpoch);
   UT_SCRIPT_METHOD(double, SecondsPastMidnight, ()) { return self.GetTime(); };
   UT_SCRIPT_METHOD(int, JulianDay, ()) { return static_cast<int>(self.GetJulianDate()); };
   UT_SCRIPT_METHOD(double, JulianDate, ()) { return self.GetJulianDate(); };
   UT_SCRIPT_METHOD(double, J2000_Date, ()) { return self.GetJ2000_Date(); };
   UT_SCRIPT_WRAP_MEMBER("SetEpoch", SetEpoch);
   UT_SCRIPT_WRAP_MEMBER("SetJulianDate", SetJulianDate);
   UT_SCRIPT_METHOD(void, SetJ2000_Date, (double aJ2000_Date)) { self.SetJ2000_Date(aJ2000_Date); };
   UT_SCRIPT_WRAP_MEMBER("SetToSystemTime", SetCurrentDateAndTime);
   UT_SCRIPT_WRAP_MEMBER("SetDate", SetDate);

   UT_SCRIPT_METHOD(bool, SetTime, (int hour, int minute, double second))
   {
      return self.SetTime(hour, minute, second);
   };


   UT_SCRIPT_METHOD(int, Month, ()) { return self.GetMonth(); };
   UT_SCRIPT_WRAP_MEMBER("Year", GetYear);
   UT_SCRIPT_WRAP_MEMBER("Day", GetDay);
   UT_SCRIPT_WRAP_MEMBER("Hour", GetHour);
   UT_SCRIPT_WRAP_MEMBER("Minute", GetMinute);
   UT_SCRIPT_WRAP_MEMBER("Second", GetSecond);
   UT_SCRIPT_WRAP_MEMBER("GMST", GetGMST);
   UT_SCRIPT_WRAP_MEMBER("GAST", GetGAST);

   UT_SCRIPT_WRAP_MEMBER("TimeSince", GetTimeSince);

   UT_SCRIPT_METHOD(std::string, ToStringLocalTime, (int gmtOffset))
   {
      int    hour, minute;
      double second;
      self.GetLocalMeanTime(gmtOffset, hour, minute, second);
      std::ostringstream oss;
      oss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":"
          << std::setw(2) << std::setfill('0') << static_cast<unsigned>(second);
      return oss.str();
   };

   UT_SCRIPT_METHOD(std::string, ToStringLocalDate, (int gmtOffset))
   {
      int year, month, day;
      self.GetLocalDate(gmtOffset, year, month, day);
      std::ostringstream oss;
      oss << UtCalendar::cMONTH_NAMES[month] << " " << day << " " << year;
      return oss.str();
   };

   UT_SCRIPT_METHOD(std::string, ToStringTime, ())
   {
      int    hour, minute;
      double second;
      self.GetTime(hour, minute, second);
      std::ostringstream oss;
      oss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0') << minute << ":"
          << std::setw(2) << std::setfill('0') << static_cast<unsigned>(second);
      return oss.str();
   };

   UT_SCRIPT_METHOD(std::string, ToStringDate, ())
   {
      std::ostringstream oss;
      oss << UtCalendar::cMONTH_NAMES[self.GetMonth()] << " " << self.GetDay() << " " << self.GetYear();
      return oss.str();
   };

   UT_SCRIPT_METHOD(std::string, ToStringIso8601, ()) { return self.GetISO8601(); };
}

void UtScriptCalendar::Destroy(void* aObjectPtr)
{
   delete static_cast<UtCalendar*>(aObjectPtr);
}

void* UtScriptCalendar::Clone(void* aObjectPtr)
{
   return new UtCalendar(*static_cast<UtCalendar*>(aObjectPtr));
}

void* UtScriptCalendar::Create(const UtScriptContext&)
{
   return static_cast<void*>(new UtCalendar());
}


std::string UtScriptCalendar::ToString(void* aObjectPtr) const
{
   UtCalendar&        cal = *(static_cast<UtCalendar*>(aObjectPtr));
   std::ostringstream oss;
   oss << cal;
   return oss.str();
}
