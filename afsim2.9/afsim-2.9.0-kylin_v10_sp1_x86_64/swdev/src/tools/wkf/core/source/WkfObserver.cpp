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

#include "WkfObserver.hpp"

#define WKF_SET_TYPE_CALLBACK(CALLBACK_TYPE, CALLBACK_NAME) \
   CALLBACK_NAME.SetCallbackType(#CALLBACK_TYPE);           \
   CALLBACK_NAME.SetName(#CALLBACK_NAME);

bool wkf::Observer::Initialize()
{
   WKF_SET_TYPE_CALLBACK(Observer::PlatformPropertiesChangedCallback, Observer::PlatformPropertiesChanged);
   WKF_SET_TYPE_CALLBACK(Observer::PlatformBrokenCallback, Observer::PlatformBroken);
   WKF_SET_TYPE_CALLBACK(Observer::PlatformPendingDeletionCallback, Observer::PlatformPendingDeletion);
   WKF_SET_TYPE_CALLBACK(Observer::WaypointMovedCallback, Observer::WaypointMoved);
   WKF_SET_TYPE_CALLBACK(Observer::PlatformActiveChangedCallback, Observer::PlatformActiveChanged);
   WKF_SET_TYPE_CALLBACK(Observer::PlatformsAddedCallback, Observer::PlatformsAdded);
   WKF_SET_TYPE_CALLBACK(Observer::PlatformConfiguredCallback, Observer::PlatformConfigured);
   WKF_SET_TYPE_CALLBACK(Observer::TrackAddedCallback, Observer::TrackAdded);
   WKF_SET_TYPE_CALLBACK(Observer::SubpartAddedCallback, Observer::SubpartAdded);
   WKF_SET_TYPE_CALLBACK(Observer::SubpartRemovedCallback, Observer::SubpartRemoved);
   WKF_SET_TYPE_CALLBACK(Observer::SubpartUpdatedCallback, Observer::SubpartUpdated);
   WKF_SET_TYPE_CALLBACK(Observer::EntityDecoratedCallback, Observer::EntityDecorated);
   WKF_SET_TYPE_CALLBACK(Observer::StandardScenarioAddedCallback, Observer::StandardScenarioAdded);
   WKF_SET_TYPE_CALLBACK(Observer::StandardScenarioRemovedCallback, Observer::StandardScenarioRemoved);
   WKF_SET_TYPE_CALLBACK(Observer::StandardViewerChangedCallback, Observer::StandardViewerChanged);
   WKF_SET_TYPE_CALLBACK(Observer::StandardMapChangedCallback, Observer::StandardMapChanged);
   WKF_SET_TYPE_CALLBACK(Observer::StandardWidgetChangedCallback, Observer::StandardWidgetChanged);
   WKF_SET_TYPE_CALLBACK(Observer::LoadSettingsCallback, Observer::LoadSettings);
   WKF_SET_TYPE_CALLBACK(Observer::SaveSettingsCallback, Observer::SaveSettings);

   return vespa::VaObserver::Initialize();
}

// Static instantiations
wkf::Observer::PlatformPropertiesChangedCallback wkf::Observer::PlatformPropertiesChanged;
wkf::Observer::PlatformBrokenCallback            wkf::Observer::PlatformBroken;
wkf::Observer::PlatformPendingDeletionCallback   wkf::Observer::PlatformPendingDeletion;
wkf::Observer::PlatformActiveChangedCallback     wkf::Observer::PlatformActiveChanged;
wkf::Observer::PlatformsAddedCallback            wkf::Observer::PlatformsAdded;
wkf::Observer::PlatformConfiguredCallback        wkf::Observer::PlatformConfigured;
wkf::Observer::WaypointMovedCallback             wkf::Observer::WaypointMoved;
wkf::Observer::TrackAddedCallback                wkf::Observer::TrackAdded;
wkf::Observer::SubpartAddedCallback              wkf::Observer::SubpartAdded;
wkf::Observer::SubpartRemovedCallback            wkf::Observer::SubpartRemoved;
wkf::Observer::SubpartUpdatedCallback            wkf::Observer::SubpartUpdated;
wkf::Observer::EntityDecoratedCallback           wkf::Observer::EntityDecorated;
wkf::Observer::StandardScenarioAddedCallback     wkf::Observer::StandardScenarioAdded;
wkf::Observer::StandardScenarioRemovedCallback   wkf::Observer::StandardScenarioRemoved;
wkf::Observer::StandardViewerChangedCallback     wkf::Observer::StandardViewerChanged;
wkf::Observer::StandardMapChangedCallback        wkf::Observer::StandardMapChanged;
wkf::Observer::StandardWidgetChangedCallback     wkf::Observer::StandardWidgetChanged;
wkf::Observer::LoadSettingsCallback              wkf::Observer::LoadSettings;
wkf::Observer::SaveSettingsCallback              wkf::Observer::SaveSettings;
