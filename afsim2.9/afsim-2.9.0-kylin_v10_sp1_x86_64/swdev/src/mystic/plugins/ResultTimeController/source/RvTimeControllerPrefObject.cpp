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
#include "RvTimeControllerPrefObject.hpp"

#include <QSettings>

RvTimeController::PrefObject::PrefObject(QObject* parent)
   : wkf::PrefObjectT<RvTimeController::PrefData>(parent, cNAME)
{
}

void RvTimeController::PrefObject::SetShowClockRate(bool aShow)
{
   mCurrentPrefs.mShowClockRate = aShow;
   emit ShowClockRateChanged(aShow);
}

void RvTimeController::PrefObject::SetShowSimTime(bool aShow)
{
   mCurrentPrefs.mShowSimTime = aShow;
   emit ShowSimTimeChanged(aShow);
}

void RvTimeController::PrefObject::SetShowWallTime(bool aShow)
{
   mCurrentPrefs.mShowWallTime = aShow;
   emit ShowWallTimeChanged(aShow);
}

void RvTimeController::PrefObject::SetSpeedLimit(int aBase10Exponent)
{
   mCurrentPrefs.mSpeedLimit = aBase10Exponent;
   emit SpeedLimitChanged(aBase10Exponent);
}

void RvTimeController::PrefObject::SetUTC_OffsetHrs(int aHrs)
{
   mCurrentPrefs.mUTC_OffsetHrs = aHrs;
   emit WallTimeSpecChanged();
}

void RvTimeController::PrefObject::SetUTC_OffsetMins(int aMins)
{
   mCurrentPrefs.mUTC_OffsetMins = aMins;
   emit WallTimeSpecChanged();
}

void RvTimeController::PrefObject::SetWallTimeSpec(Qt::TimeSpec aTimeSpec)
{
   mCurrentPrefs.mWallTimeSpec = aTimeSpec;
   emit WallTimeSpecChanged();
}

void RvTimeController::PrefObject::SetLoopedPlayback(bool aLoopedPlayback)
{
   mCurrentPrefs.mLoopedPlayback = aLoopedPlayback;
   emit LoopedPlaybackChanged(aLoopedPlayback);
}


void RvTimeController::PrefObject::SetExtrapolateAtEnd(bool aExtrapolateAtEnd)
{
   mCurrentPrefs.mExtrapolateAtEnd = aExtrapolateAtEnd;
   emit ExtrapolateAtEndChanged(aExtrapolateAtEnd);
}

void RvTimeController::PrefObject::EmitAllSignals()
{
   emit ShowClockRateChanged(mCurrentPrefs.mShowClockRate);
   emit ShowSimTimeChanged(mCurrentPrefs.mShowSimTime);
   emit ShowWallTimeChanged(mCurrentPrefs.mShowWallTime);
   emit SpeedLimitChanged(mCurrentPrefs.mSpeedLimit);
   emit LoopedPlaybackChanged(mCurrentPrefs.mLoopedPlayback);
   emit ExtrapolateAtEndChanged(mCurrentPrefs.mExtrapolateAtEnd);
   emit WallTimeSpecChanged();
}

void RvTimeController::PrefObject::Apply()
{
   EmitAllSignals();
}

RvTimeController::PrefData RvTimeController::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData pData;
   pData.mShowClockRate = aSettings.value("showClockRate", mDefaultPrefs.mShowClockRate).toBool();
   pData.mShowSimTime   = aSettings.value("showSimTime", mDefaultPrefs.mShowSimTime).toBool();
   pData.mShowWallTime  = aSettings.value("showWallTime", mDefaultPrefs.mShowWallTime).toBool();
   pData.mSpeedLimit    = aSettings.value("speedLimit", mDefaultPrefs.mSpeedLimit).toInt();

   pData.mUTC_OffsetHrs  = aSettings.value("UTC_OffsetHrs", mDefaultPrefs.mUTC_OffsetHrs).toInt();
   pData.mUTC_OffsetMins = aSettings.value("UTC_OffsetMins", mDefaultPrefs.mUTC_OffsetMins).toInt();
   pData.mWallTimeSpec = static_cast<Qt::TimeSpec>(aSettings.value("wallTimeSpec", mDefaultPrefs.mWallTimeSpec).toInt());

   pData.mLoopedPlayback   = aSettings.value("loopedPlayback", mDefaultPrefs.mLoopedPlayback).toBool();
   pData.mExtrapolateAtEnd = aSettings.value("extrapolateAtEnd", mDefaultPrefs.mExtrapolateAtEnd).toBool();
   return pData;
}

void RvTimeController::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("showClockRate", mCurrentPrefs.mShowClockRate);
   aSettings.setValue("showSimTime", mCurrentPrefs.mShowSimTime);
   aSettings.setValue("showWallTime", mCurrentPrefs.mShowWallTime);
   aSettings.setValue("speedLimit", mCurrentPrefs.mSpeedLimit);

   aSettings.setValue("UTC_OffsetHrs", mCurrentPrefs.mUTC_OffsetHrs);
   aSettings.setValue("UTC_OffsetMins", mCurrentPrefs.mUTC_OffsetMins);
   aSettings.setValue("wallTimeSpec", mCurrentPrefs.mWallTimeSpec);

   aSettings.setValue("loopedPlayback", mCurrentPrefs.mLoopedPlayback);
   aSettings.setValue("extrapolateAtEnd", mCurrentPrefs.mExtrapolateAtEnd);
}
