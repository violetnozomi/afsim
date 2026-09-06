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
#include "RvPluginTimeController.hpp"

#include <QStatusBar>

#include "RvTimeControllerInterface.hpp"
#include "RvTimeControllerUpdater.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvTimeController::Plugin, "Time Controller", "Controls the displayed time.", "mystic")

RvTimeController::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mDataInterfacePtr(new Interface(this))
   , mPrefWidgetPtr(new PrefWidget)
{
   mStatusWidgetPtr =
      new RvTimeController::StatusWidget(mDataInterfacePtr, mPrefWidgetPtr->GetPreferenceObject(), UniqueId());
   // Create and add toolbar to main window
   mToolbarPtr                    = new RvTimeController::Toolbar(mDataInterfacePtr);
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mainWindowPtr->AddToolBar(mToolbarPtr);

   // Add status bar widget
   mainWindowPtr->statusBar()->addPermanentWidget(mStatusWidgetPtr);

   mBrowserPtr = new RvTimeController::BookmarkBrowser(mainWindowPtr);
   mainWindowPtr->addDockWidget(Qt::LeftDockWidgetArea, mBrowserPtr);
   mBrowserPtr->hide();

   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &RvTimeController::PrefObject::LoopedPlaybackChanged,
           mDataInterfacePtr,
           &RvTimeController::Interface::LoopedPlaybackCommand);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &RvTimeController::PrefObject::ExtrapolateAtEndChanged,
           mDataInterfacePtr,
           &RvTimeController::Interface::ExtrapolatePlaybackCommand);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &RvTimeController::PrefObject::SpeedLimitChanged,
           mToolbarPtr.data(),
           &RvTimeController::Toolbar::SpeedLimitChanged);
   mDataInterfacePtr->LoopedPlaybackCommand(mPrefWidgetPtr->GetPreferenceObject()->GetLoopedPlayback());
   mDataInterfacePtr->ExtrapolatePlaybackCommand(mPrefWidgetPtr->GetPreferenceObject()->GetExtrapolateAtEnd());

   connect(mBrowserPtr.data(),
           &RvTimeController::BookmarkBrowser::BookmarkSelected,
           this,
           [&](const rv::MsgBookmark& aBookmark) { mDataInterfacePtr->SendAdvanceToTimeCommand(aBookmark.time()); });
}

QList<wkf::PrefWidget*> RvTimeController::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvTimeController::Plugin::GuiUpdate()
{
   mToolbarPtr->Update(mDataInterfacePtr);
   mStatusWidgetPtr->Update();
}

void RvTimeController::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   mDataInterfacePtr->AdvanceTimeRead(aData);
}

void RvTimeController::Plugin::RegularRead(const rv::ResultData& aData)
{
   mDataInterfacePtr->AdvanceTimeRead(aData);

   QMutexLocker l(&mMutex);
   for (auto b : mBookmarkCache)
   {
      mBrowserPtr->AddBookmark(b);
      mToolbarPtr->AddBookmark(b);
   }
   mBookmarkCache.clear();
}

void RvTimeController::Plugin::AdvanceTimeWrite(rv::ResultData& aData)
{
   mToolbarPtr->UpdateWrite(mDataInterfacePtr);
}

void RvTimeController::Plugin::SetEpoch(const UtCalendar& aEpoch)
{
   mDataInterfacePtr->SetWallTime(aEpoch);
   mStatusWidgetPtr->Update();
}

void RvTimeController::Plugin::ClearScenario(bool aFullReset)
{
   if (aFullReset)
   {
      mToolbarPtr->GuiSimulationClockRateChange(1.0);
   }
   mToolbarPtr->ClearBookmarks();
}

void RvTimeController::Plugin::BookmarkRead(const rv::MsgBookmark& aBookmark)
{
   QMutexLocker l(&mMutex);
   mBookmarkCache.emplace_back(aBookmark);
}

QList<wkf::Action*> RvTimeController::Plugin::GetActions() const
{
   return mToolbarPtr->GetActions();
}

std::unique_ptr<wkf::Updater> RvTimeController::Plugin::GetUpdater(const QString&     aPlatformName,
                                                                   const QString&     aDatum,
                                                                   const unsigned int aContext) const
{
   if (aDatum == "mSimTimeWidget")
   {
      return wkf::make_updater<SimTimeUpdater>();
   }
   else if (aDatum == "mWallTimeWidget")
   {
      return wkf::make_updater<WallTimeUpdater>();
   }
   else if (aDatum == "mClockRateWidget")
   {
      return wkf::make_updater<ClockRateUpdater>();
   }
   return nullptr;
}
