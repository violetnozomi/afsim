// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfEventMarkerPluginBase.hpp"

#include "WkfEventMarkerPrefObject.hpp"
#include "WkfEventMarkerPrefWidget.hpp"

wkf::EventMarkerPluginBase::EventMarkerPluginBase(const QString&       aPluginName,
                                                  const size_t         aUniqueId,
                                                  const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueId, aRoles)
   , mPrefWidgetPtr(new EventMarkerPrefWidget)
   , mDisplayInterfacePtr(new wkf::EventMarkerDisplayInterface(this, mPrefWidgetPtr))
{
}

wkf::EventMarkerPluginBase::~EventMarkerPluginBase() = default;

QList<wkf::PrefWidget*> wkf::EventMarkerPluginBase::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

const wkf::EventMarkerPrefObject* wkf::EventMarkerPluginBase::GetPrefObject() const
{
   return mPrefWidgetPtr->GetPreferenceObject();
}
