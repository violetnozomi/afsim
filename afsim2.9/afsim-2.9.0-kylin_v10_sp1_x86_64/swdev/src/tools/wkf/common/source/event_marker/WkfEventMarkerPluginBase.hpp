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
#ifndef WKFEVENTMARKERPLUGINBASE_HPP
#define WKFEVENTMARKERPLUGINBASE_HPP

#include "wkf_common_export.h"

#include "WkfEventMarkerDisplayInterface.hpp"
#include "WkfEventMarkerPrefObject.hpp"
#include "WkfEventMarkerPrefWidget.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
//! An abstract base class for a plugin to a Wkf application that visualizes events
//! Contains a preference object and widget, as well as a list of "platform options" tree widget items.
//! Contains a DisplayInterface that will allow the application plugin to drive the visuals
class WKF_COMMON_EXPORT EventMarkerPluginBase : public Plugin
{
public:
   EventMarkerPluginBase(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles = eNO_ROLE);
   ~EventMarkerPluginBase() override = 0;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

protected:
   EventMarkerDisplayInterface* GetDisplayInterface() { return mDisplayInterfacePtr; }
   const EventMarkerPrefObject* GetPrefObject() const;

private:
   PluginUiPointer<EventMarkerPrefWidget> mPrefWidgetPtr; // Use guarded pointer because objects will be owned by main window
   EventMarkerDisplayInterface*           mDisplayInterfacePtr;
};
} // namespace wkf
#endif
