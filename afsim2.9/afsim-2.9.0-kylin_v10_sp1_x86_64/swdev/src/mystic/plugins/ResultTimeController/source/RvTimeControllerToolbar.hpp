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
#ifndef RVTIMECONTROLLERTOOLBAR_HPP
#define RVTIMECONTROLLERTOOLBAR_HPP

#include <set>

#include <QDoubleSpinBox>
#include <QSlider>
#include <QToolBar>

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "ui_ClockRateSlider.h"

namespace RvTimeController
{
class Toolbar : public QToolBar
{
   friend class Plugin;

   Q_OBJECT

public:
   Toolbar(Interface* aInterfacePtr);
   ~Toolbar() override = default;

   void                MaxTimeChanged(float aMax);
   void                CurrentTimeChanged(double aTime);
   void                TimeInCacheChanged(float aMin, float aMax);
   void                Update(Interface* aInterfacePtr);
   void                UpdateWrite(Interface* aInterfacePtr);
   void                AddBookmark(const rv::MsgBookmark& aBookmark) { mBookmarks.emplace(aBookmark); }
   void                ClearBookmarks() { mBookmarks.clear(); }
   QList<wkf::Action*> GetActions() const;

   void SpeedLimitChanged(int aBase10Exponent);

signals:
   void ClockRateChanged(double aVal, bool aIsLog);
private slots:
   void GuiSimulationResuming();
   void GuiSimulationPausing();
   void GuiSimulationReversing();
   void GuiSimulationClockRateChange(double aClockRate);

   // Toolbar slots
   void AdvanceToTimeTriggered(Interface* aInterfacePtr);
   void SetClockRateTriggered();
   void ClockRateSliderMoved(int aValue);
   void ClockRateSpinChanged(double aValue);
   void TimeSliderPressed();
   void TimeSliderMoved(int aValue);
   void TimeSliderReleased();

private:
   void MovePopup(QWidget* aPopupWidgetPtr, QAction* aActionPtr);

   void PlaybackStatusCallback(rv::PlaybackStatus aStatus);
   void BookmarkAdvancement(Interface* aInterfacePtr, bool aNext);
   void SetupPlaybackShortcuts();

   wkf::Action*  mPauseActionPtr;
   wkf::Action*  mResumeActionPtr;
   wkf::Action*  mRestartActionPtr;
   wkf::Action*  mReverseActionPtr;
   wkf::Action*  mAdvanceTimeActionPtr;
   wkf::Action*  mFastForwardActionPtr;
   wkf::Action*  mPrevBookmarkActionPtr;
   wkf::Action*  mNextBookmarkActionPtr;
   CachedSlider* mSliderPtr;

   wkf::Action*       mReverseTogglePtr;
   wkf::Action*       mResumeTogglePtr;
   rv::PlaybackStatus mCurrentStatus;

   Ui::ClockRateSliderUi mUi;
   double                mTime;
   double                mMax;
   float                 mMinCache;
   float                 mMaxCache;
   bool                  mDeferUpdatesToTime;
   bool                  mPressed;

   struct TimeCompare
   {
      bool operator()(const rv::MsgBookmark& aTime1, const rv::MsgBookmark& aTime2) const
      {
         return aTime1.time() < aTime2.time();
      }
   };
   std::multiset<rv::MsgBookmark, TimeCompare> mBookmarks;
};
} // namespace RvTimeController
#endif // RVTIMECONTROLLERTOOLBAR_HPP
