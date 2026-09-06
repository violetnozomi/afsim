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
#ifndef RVTIMECONTROLLERINTERFACE_HPP
#define RVTIMECONTROLLERINTERFACE_HPP

#include <memory>
#include <queue>
#include <string>

#include <QMutex>
#include <QObject>

#include "UtCalendar.hpp"

namespace rv
{
class ResultData;
}

namespace RvTimeController
{
class PrefObject;
class Interface : public QObject
{
   Q_OBJECT

public:
   Interface(QObject* parent);
   ~Interface() override;

   float                   GetSimTime() const;
   UtCalendar              GetWallTime() const;
   float                   GetEndTime() const;
   float                   GetClockRate() const;
   std::pair<float, float> GetCacheRange() const;

   void SetWallTime(const UtCalendar& aCalendar);

   void AdvanceTimeRead(const rv::ResultData& aData);

public slots:
   void SendAdvanceToTimeCommand(double aSimTime);
   void SendPauseCommand();
   void SendResumeCommand();
   void SendRestartCommand();
   void SendReverseCommand();
   void SendClockRateCommand(double aClockRate,
                             bool   aIsLogScale); // if log scale, must convert from DB to Linear
   void LoopedPlaybackCommand(bool aState);
   void ExtrapolatePlaybackCommand(bool aState);
   // Commands issued from GUI to Sim, these slots are all executed on GUI thread

private slots:
   // Executed on the simulation thread to read and write data from/to the simulation

private:
   mutable QMutex          mMutex;
   float                   mSimTime;
   UtCalendar              mWallTime;
   float                   mEndTime;
   std::string             mState;
   float                   mClockRate;
   std::pair<float, float> mCacheRange;
};
} // namespace RvTimeController
#endif // RVTIMECONTROLLERINTERFACE_HPP
