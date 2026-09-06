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

#ifndef WKFVAOBSERVER_HPP
#define WKFVAOBSERVER_HPP

#include "wkf_export.h"

#include <QList>

#include "VaCallback.hpp"
#include "VaObserver.hpp"

class QIcon;
class QSettings;

namespace vespa
{
class VaViewer;
class VaWidget;
} // namespace vespa

namespace wkf
{
class AttachmentRoute;
class Entity;
class Platform;
class Scenario;
class Track;
class Waypoint;

class WKF_EXPORT Observer : public vespa::VaObserver
{
public:
   Observer()           = default;
   ~Observer() override = default;

   using PlatformPropertiesChangedCallback = vespa::VaCallbackListN<void(Platform* aPlatformPtr)>;
   using PlatformBrokenCallback            = vespa::VaCallbackListN<void(Platform* aPlatformPtr)>;
   using PlatformPendingDeletionCallback   = vespa::VaCallbackListN<void(Platform* aPlatformPtr)>;
   using WaypointMovedCallback = vespa::VaCallbackListN<void(const wkf::AttachmentRoute* aRoute, int aWaypointIndex)>;
   using PlatformActiveChangedCallback = vespa::VaCallbackListN<void(Platform* aPlatformPtr, bool aActive)>;
   using PlatformsAddedCallback        = vespa::VaCallbackListN<void(const QList<Platform*>& aPlatformPtrs)>;
   using PlatformConfiguredCallback    = vespa::VaCallbackListN<void(Platform* aPlatformPtr)>;
   using EntityDecoratedCallback =
      vespa::VaCallbackListN<void(Entity* aPlatformPtr, const QIcon& aIcon, const std::string& aDescription)>;
   using TrackAddedCallback            = vespa::VaCallbackListN<void(Track* aTrackPtr)>;
   using SubpartAddedCallback          = vespa::VaCallbackListN<void(Platform* aPlatformPtr, const std::string& aName)>;
   using SubpartRemovedCallback        = vespa::VaCallbackListN<void(Platform* aPlatformPtr, const std::string& aName)>;
   using SubpartUpdatedCallback        = vespa::VaCallbackListN<void(Platform* aPlatformPtr, const std::string& aName)>;
   using StandardScenarioAddedCallback = vespa::VaCallbackListN<void(Scenario* aScenarioPtr)>;
   using StandardScenarioRemovedCallback = vespa::VaCallbackListN<void(Scenario* aScenarioPtr)>;
   using StandardViewerChangedCallback   = vespa::VaCallbackListN<void(vespa::VaViewer* aViewer)>;
   using StandardMapChangedCallback      = vespa::VaCallbackListN<void()>;
   using StandardWidgetChangedCallback   = vespa::VaCallbackListN<void(vespa::VaWidget* aWidgetPtr)>;
   using LoadSettingsCallback            = vespa::VaCallbackListN<void(QSettings&)>;
   using SaveSettingsCallback            = vespa::VaCallbackListN<void(QSettings&)>;

   //! Command listeners that a entities aux-data has changes and may need review
   //! @param aPlatform the platform than changed
   static PlatformPropertiesChangedCallback PlatformPropertiesChanged;

   //! Inform listeners that a platform is broken or disabled, but not currently scheduled for deletion
   //! @param aPlatformPtr the platform that is broken
   static PlatformBrokenCallback PlatformBroken;

   // this gets emitted when a platform is made active or inactive, this is not currently used in warlock
   //! @param aPlatformPtr the platform whose active state changed
   //! @param aActive indicates the platform has changed to active/inactive
   static PlatformActiveChangedCallback PlatformActiveChanged;

   // this gets emitted when a warlock platform is created, this should be used rather than VaObserver::EntityAdded
   // to guarantee that warlock specific data is populated (ie categories).
   static PlatformsAddedCallback PlatformsAdded;

   // this is emitted after all calls to PlatformAdded have been resolved.  It is at this point that the platforms
   // options should be properly configured
   static PlatformConfiguredCallback PlatformConfigured;

   // this gets emitted when a warlock platform receives a new subpart
   //! @param aPlatfromPtr the platform that received a new part
   //! @param aName the name of the added part
   static SubpartAddedCallback SubpartAdded;

   // this gets emitted when a warlock platform loses a subpart
   //! @param aPlatfromPtr the platform that lost a part
   //! @param aName the name of the removed part
   static SubpartRemovedCallback SubpartRemoved;

   // this gets emitted when a platform's subpart changes position/orientation
   //! @param aPlatfromPtr the platform that updated its part
   //! @param aName the name of the part
   static SubpartUpdatedCallback SubpartUpdated;

   // this gets emitted when a warlock track is created
   static TrackAddedCallback TrackAdded;

   //! Inform listeners that a platform is scheduled for deletion
   //! @param aPlatformPtr the platform that is pending deletion
   static PlatformPendingDeletionCallback PlatformPendingDeletion;

   static WaypointMovedCallback WaypointMoved;

   //! Inform listeners that a platform has received a decoration
   //! @param aPlatformPtr the platform receiving a decoration
   //! @param aIcon the QIcon used to decorate the platform
   //! @param aDescription name of the icon, used to save/restore
   static EntityDecoratedCallback EntityDecorated;

   // this gets emitted when a warlock scenario is added.
   static StandardScenarioAddedCallback StandardScenarioAdded;

   //! Inform listeners that the Standard Scenario has been Removed
   static StandardScenarioRemovedCallback StandardScenarioRemoved;

   //! Inform listeners that the Standard Viewer has been Changed
   static StandardViewerChangedCallback StandardViewerChanged;

   //! Inform listeners that a new map has been loaded in the standard viewer
   static StandardMapChangedCallback StandardMapChanged;
   //! Inform listeners that the Standard Widget has been changed
   static StandardWidgetChangedCallback StandardWidgetChanged;

   //! Inform listeners that we want to load settings
   static LoadSettingsCallback LoadSettings;

   //! Inform listeners that we want to save settings
   static SaveSettingsCallback SaveSettings;

   bool Initialize() override;
};
} // namespace wkf
#endif
