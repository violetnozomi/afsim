// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvTimeControllerUpdater.hpp"

#include <sstream>

#include <QDateTime>

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvTimeControllerPrefObject.hpp"
#include "UtTime.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

RvTimeController::SimTimeUpdater::SimTimeUpdater()
   : Updater("Sim Time", "")
{
}

QString RvTimeController::SimTimeUpdater::GetValueString() const
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      float              st = results->GetSimTime();
      std::ostringstream oss;
      oss << UtTime(st, (wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeFormatting()));
      return oss.str().c_str();
   }
   return QString("ERROR");
}

RvTimeController::WallTimeUpdater::WallTimeUpdater()
   : Updater("Wall Time", "")
{
   wkf::UnitsObject* unitPrefs = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   mDateFormat                 = unitPrefs->GetDateFormatting();
   connect(unitPrefs, &wkf::UnitsObject::DateFormatChanged, this, &WallTimeUpdater::DateFormatChanged);
}

QString RvTimeController::WallTimeUpdater::GetValueString() const
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      UtCalendar c = rvEnv.GetEpoch();
      c.AdvanceTimeBy(results->GetSimTime());
      QDate       date(c.GetYear(), c.GetMonth(), c.GetDay());
      QTime       time(c.GetHour(), c.GetMinute(), c.GetSecond());
      QDateTime   wallTime(date, time, Qt::UTC);
      PrefObject* prefObject = wkfEnv.GetPreferenceObject<PrefObject>();
      if (prefObject->GetWallTimeSpec() == Qt::LocalTime)
      {
         wallTime = wallTime.toLocalTime();
      }
      else if (prefObject->GetWallTimeSpec() == Qt::OffsetFromUTC)
      {
         wallTime =
            wallTime.toOffsetFromUtc((3600 * prefObject->GetUTC_OffsetHrs()) + (60 * prefObject->GetUTC_OffsetMins()));
      }
      if (mDateFormat == wkf::UnitsData::DATE_ISO)
      {
         return wallTime.toString(Qt::ISODate);
      }
      else if (mDateFormat == wkf::UnitsData::DATE_SHORT)
      {
         return wallTime.toString("MM/dd/yy hh:mm:ss");
      }
      else
      {
         return wallTime.toString("MMM d yyyy hh:mm:ss");
      }
   }
   return QString("ERROR");
}

RvTimeController::ClockRateUpdater::ClockRateUpdater()
   : Updater("Clock Rate", "")
{
}

QString RvTimeController::ClockRateUpdater::GetValueString() const
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      float cr = results->GetClockRate();
      return QString("x%1").arg(cr, 0, 'f', 2);
   }
   return QString("ERROR");
}
