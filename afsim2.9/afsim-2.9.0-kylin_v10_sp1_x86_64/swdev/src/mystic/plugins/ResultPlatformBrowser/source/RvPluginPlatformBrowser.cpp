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
#include "RvPluginPlatformBrowser.hpp"

#include <QLabel>
#include <QMenu>

#include "RvEnvironment.hpp"
#include "RvPlatformBrowserInterface.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "WkfMainWindow.hpp"
#include "platform_browser/WkfPlatformBrowserDockWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvPlatformBrowser::Plugin, "Platform Browser", "Displays a list of platforms in the replay", "mystic")

RvPlatformBrowser::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new PrefWidget)
   , mPlatformBrowserInterfacePtr(new Interface())
{
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &PrefObject::ShowInactiveChanged,
           mPlatformBrowserInterfacePtr,
           &Interface::ShowInactiveChanged);
}

QList<wkf::PrefWidget*> RvPlatformBrowser::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvPlatformBrowser::Plugin::ClearScenario(bool aFullReset)
{
   mPlatformBrowserInterfacePtr->ClearScenario();
}

void RvPlatformBrowser::Plugin::GuiUpdate()
{
   // Don't bother updating dialog if it is not visible
   if (mPlatformBrowserInterfacePtr->GetDockWidget()->isVisible())
   {
      mPlatformBrowserInterfacePtr->Update();
   }
}
