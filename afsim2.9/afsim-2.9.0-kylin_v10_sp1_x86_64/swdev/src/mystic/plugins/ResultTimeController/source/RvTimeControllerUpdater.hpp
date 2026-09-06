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

#ifndef RVTIMECONTROLLERUPDATER_HPP
#define RVTIMECONTROLLERUPDATER_HPP

#include <QObject>

#include "UtCalendar.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfUpdater.hpp"

namespace RvTimeController
{
class SimTimeUpdater : public wkf::Updater
{
public:
   SimTimeUpdater();
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return ""; }

private:
};
class WallTimeUpdater : public wkf::Updater, public QObject
{
public:
   WallTimeUpdater();
   QString       GetValueString() const override;
   const QString GetDisplayName() const override { return ""; }

private:
   void DateFormatChanged(wkf::UnitsData::DateFormat aFormat) { mDateFormat = aFormat; }

   wkf::UnitsData::DateFormat mDateFormat;
};
class ClockRateUpdater : public wkf::Updater
{
public:
   ClockRateUpdater();
   QString GetValueString() const override;

private:
};
}; // namespace RvTimeController

#endif
