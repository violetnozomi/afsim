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
#ifndef RVTIMECONTROLLERPREFOBJECT_HPP
#define RVTIMECONTROLLERPREFOBJECT_HPP

class QSettings;
#include "WkfPrefObject.hpp"

namespace RvTimeController
{
struct PrefData
{
   bool mShowClockRate{true};
   bool mShowSimTime{true};
   bool mShowWallTime{true};

   int mSpeedLimit{3};

   int          mUTC_OffsetHrs{0};
   int          mUTC_OffsetMins{0};
   Qt::TimeSpec mWallTimeSpec{Qt::UTC};

   bool mLoopedPlayback{true};
   bool mExtrapolateAtEnd{false};
};

class PrefObject : public wkf::PrefObjectT<PrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "TimeControllerPreferences";
   PrefObject(QObject* parent = nullptr);

   bool GetShowClockRate() const { return mCurrentPrefs.mShowClockRate; }
   void SetShowClockRate(bool aShow);

   bool GetShowSimTime() const { return mCurrentPrefs.mShowSimTime; }
   void SetShowSimTime(bool aShow);

   bool GetShowWallTime() const { return mCurrentPrefs.mShowWallTime; }
   void SetShowWallTime(bool aShow);

   int  GetSpeedLimit() const { return mCurrentPrefs.mSpeedLimit; }
   void SetSpeedLimit(int aBase10Exponent);

   int  GetUTC_OffsetHrs() const { return mCurrentPrefs.mUTC_OffsetHrs; }
   void SetUTC_OffsetHrs(int aHrs);

   int  GetUTC_OffsetMins() const { return mCurrentPrefs.mUTC_OffsetMins; }
   void SetUTC_OffsetMins(int aMins);

   Qt::TimeSpec GetWallTimeSpec() const { return mCurrentPrefs.mWallTimeSpec; }
   void         SetWallTimeSpec(Qt::TimeSpec aTimeSpec);

   bool GetLoopedPlayback() const { return mCurrentPrefs.mLoopedPlayback; }
   void SetLoopedPlayback(bool aLoopedPlayback);

   bool GetExtrapolateAtEnd() const { return mCurrentPrefs.mExtrapolateAtEnd; }
   void SetExtrapolateAtEnd(bool aExtrapolate);

signals:
   void ShowClockRateChanged(bool aShow);
   void ShowRunNumberChanged(bool aShow);
   void ShowSimStateChanged(bool aShow);
   void ShowSimTimeChanged(bool aShow);
   void ShowWallTimeChanged(bool aShow);
   void SpeedLimitChanged(int aShow);
   void WallTimeSpecChanged();
   void LoopedPlaybackChanged(bool aState);
   void ExtrapolateAtEndChanged(bool aState);

private:
   void EmitAllSignals();

   void Apply() override;

   PrefData ReadSettings(QSettings& aSettings) const override;
   void     SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace RvTimeController

Q_DECLARE_METATYPE(RvTimeController::PrefData)
#endif // RVTIMECONTROLLERPREFOBJECT_HPP
