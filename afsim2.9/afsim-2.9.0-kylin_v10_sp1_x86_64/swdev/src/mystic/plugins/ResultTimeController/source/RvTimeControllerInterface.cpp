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
#include "RvTimeControllerInterface.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvTimeControllerUpdater.hpp"
#include "UtMath.hpp"

RvTimeController::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mSimTime(0.0)
   , mWallTime()
   , mEndTime(0.0)
   , mState()
   , mClockRate(1.0)
{
}

RvTimeController::Interface::~Interface()
{
   QMutexLocker locker(&mMutex);
}

float RvTimeController::Interface::GetSimTime() const
{
   QMutexLocker locker(&mMutex);
   return mSimTime;
}

UtCalendar RvTimeController::Interface::GetWallTime() const
{
   QMutexLocker locker(&mMutex);
   return mWallTime;
}

float RvTimeController::Interface::GetEndTime() const
{
   QMutexLocker locker(&mMutex);
   return mEndTime;
}

float RvTimeController::Interface::GetClockRate() const
{
   QMutexLocker locker(&mMutex);
   return mClockRate;
}

std::pair<float, float> RvTimeController::Interface::GetCacheRange() const
{
   QMutexLocker locker(&mMutex);
   return mCacheRange;
}

void RvTimeController::Interface::SetWallTime(const UtCalendar& aCalendar)
{
   mWallTime = aCalendar;
}

void RvTimeController::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   mSimTime = aData.GetSimTime();
   mEndTime = aData.GetEndTime();

   mClockRate  = aData.GetClockRate();
   mCacheRange = aData.GetCacheRange();
}

void RvTimeController::Interface::SendPauseCommand()
{
   rvEnv.SendPauseCommand();
}

void RvTimeController::Interface::SendResumeCommand()
{
   rvEnv.SendResumeCommand();
}

void RvTimeController::Interface::SendReverseCommand()
{
   rvEnv.SendReverseCommand();
}

void RvTimeController::Interface::SendRestartCommand()
{
   rvEnv.SendRestartCommmand();
}

void RvTimeController::Interface::SendAdvanceToTimeCommand(double aSimTime)
{
   QMutexLocker locker(&mMutex);
   // notify the data
   rvEnv.SetTime(aSimTime);
}

// if log scale, must convert from DB to Linear
void RvTimeController::Interface::SendClockRateCommand(double aClockRate, bool aIsLogScale)
{
   if (aIsLogScale)
   {
      aClockRate = UtMath::DB_ToLinear(aClockRate);
   }

   QMutexLocker locker(&mMutex);
   // notify the data
   rvEnv.SetRate(aClockRate);
}

void RvTimeController::Interface::LoopedPlaybackCommand(bool aState)
{
   rvEnv.SetLooped(aState);
}

void RvTimeController::Interface::ExtrapolatePlaybackCommand(bool aState)
{
   rvEnv.SetExtrapolateAtEnd(aState);
}
