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
#ifndef RVPLUGINTIMECONTROLLER_HPP
#define RVPLUGINTIMECONTROLLER_HPP

#include "RvBookmarkBrowser.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "RvTimeControllerPrefWidget.hpp"
#include "RvTimeControllerStatusWidget.hpp"
#include "RvTimeControllerToolbar.hpp"

namespace RvTimeController
{
class Interface;

class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

private slots:
   // Slots executed on Gui thread
   void                GuiUpdate() override;
   void                AdvanceTimeRead(const rv::ResultData& aData) override;
   void                RegularRead(const rv::ResultData& aData) override;
   void                AdvanceTimeWrite(rv::ResultData& aData) override;
   void                SetEpoch(const UtCalendar& aEpoch) override;
   void                ClearScenario(bool aFullReset) override;
   void                BookmarkRead(const rv::MsgBookmark& aBookmark) override;
   QList<wkf::Action*> GetActions() const override;

   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

private:
   // managed by Qt
   Interface* mDataInterfacePtr;

   // Use guarded pointers because objects will be owned by main window
   PluginUiPointer<BookmarkBrowser> mBrowserPtr;
   PluginUiPointer<StatusWidget>    mStatusWidgetPtr;
   PluginUiPointer<Toolbar>         mToolbarPtr;
   PluginUiPointer<PrefWidget>      mPrefWidgetPtr;

   QMutex                       mMutex;
   std::vector<rv::MsgBookmark> mBookmarkCache;
};
} // namespace RvTimeController
#endif
