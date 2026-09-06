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

#include "WkfRouteBrowserInterface.hpp"

#include "UtCast.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfRouteBrowserCreateDialog.hpp"
#include "WkfRouteBrowserDockWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

wkf::RouteBrowserInterface::RouteBrowserInterface(QWidget* aParent, vespa::VaViewer* aViewer)
   : QObject(aParent)
   , mViewer(aViewer)
   , mEntityAnchorInitialized(false)
   , mSelectedWaypoint(-1)
   , mOriginalVisibilityState(true)
{
   mDockWidget = new RouteBrowserDockWidget(aParent);

   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, mDockWidget);
   mDockWidget->hide();

   Connect();

   mCallbacks.Add(vespa::VaObserver::AttachmentSelected.Connect(&RouteBrowserInterface::AttachmentSelected, this));
   mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&RouteBrowserInterface::PlatformsAdded, this));
   mCallbacks.Add(vespa::VaObserver::EntityRemoved.Connect(&RouteBrowserInterface::EntityRemoved, this));
   mCallbacks.Add(wkf::Observer::StandardScenarioAdded.Connect(&RouteBrowserInterface::ScenarioAdded, this));
}

void wkf::RouteBrowserInterface::Clear()
{
   mDockWidget->Clear();
   mPOIToWaypointsMap.clear();
   mSelectedPlatforms.clear();
}

void wkf::RouteBrowserInterface::Connect()
{
   // Environment Connections
   connect(&wkfEnv, &Environment::PlatformSelectionChanged, this, &RouteBrowserInterface::PlatformSelectionChanged);

   // DockWidget Connections --------------------------------------------------------------------------------------
   connect(mDockWidget,
           &wkf::RouteBrowserDockWidget::DockGlobalRouteSelectedChanged,
           this,
           &RouteBrowserInterface::DockGlobalRouteSelectedHandler);
   connect(mDockWidget,
           &wkf::RouteBrowserDockWidget::DockRouteSelectionChanged,
           this,
           &RouteBrowserInterface::DockRouteSelectionChangedHandler);
   connect(mDockWidget,
           &wkf::RouteBrowserDockWidget::ContextMenuRequested,
           this,
           &RouteBrowserInterface::ContextMenuRequestedHandler);

   // Edit Dialog Connections -------------------------------------------------------------------------------------
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::LocalWaypointSelected,
           this,
           &RouteBrowserInterface::DialogLocalWaypointSelectedHandler);
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::GlobalWaypointSelected,
           this,
           &RouteBrowserInterface::DialogGlobalWaypointSelectedHandler);
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::RoutePopulated,
           this,
           [&](const RouteBrowserInterface::RouteInfo& aRouteInfo, const RouteBrowserInterface::RouteInfo& aPrevRoute)
           {
              RouteBrowserInterface::ShowExistingRoute(aPrevRoute);
              RouteBrowserInterface::CreateTemporaryRoute(aRouteInfo);
           });
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::WaypointAdded,
           this,
           [&]() { RouteBrowserInterface::CreateTemporaryRoute(mDockWidget->GetEditDialog()->GetRouteInfo()); });
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::RouteEdited,
           this,
           &RouteBrowserInterface::CreateTemporaryRoute);
   connect(mDockWidget->GetEditDialog(),
           &wkf::RouteBrowserEditDialog::EditDialogCancelled,
           this,
           &RouteBrowserInterface::RemoveTemporaryRoute);

   // Create Dialog Connections -----------------------------------------------------------------------------------
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::GlobalWaypointSelected,
           this,
           &RouteBrowserInterface::CreateDialogGlobalWaypointSelectedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserDialog::GlobalRouteEdited,
           this,
           &RouteBrowserInterface::GlobalRouteEditedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::GlobalRouteEdited,
           this,
           &RouteBrowserInterface::GlobalRouteEditedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::CreateRouteStarted,
           this,
           &RouteBrowserInterface::CreateRouteStartedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::CreateRouteTerminated,
           this,
           &RouteBrowserInterface::CreateRouteTerminatedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::WaypointAdded,
           this,
           &RouteBrowserInterface::WaypointAddedHandler);
   connect(mDockWidget->GetCreateDialog(),
           &wkf::RouteBrowserCreateDialog::WaypointRemoved,
           this,
           &RouteBrowserInterface::WaypointRemovedHandler);

   // redraw existing route
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           this,
           &RouteBrowserInterface::TeamColorChanged);
}

// When a platform is selected in the environment, update the route browser accordingly.
void wkf::RouteBrowserInterface::PlatformSelectionChanged(const wkf::PlatformList& aSelected,
                                                          const wkf::PlatformList& aUnselected)
{
   for (const auto& platform : aSelected)
   {
      unsigned int platformId   = platform->GetUniqueId();
      std::string  platformName = platform->GetName();

      // If the platform is selected, but none of its waypoints are selected
      if (mPOIToWaypointsMap.find(platformId) == mPOIToWaypointsMap.end())
      {
         mSelectedWaypoint              = -1;
         mPOIToWaypointsMap[platformId] = std::vector<unsigned int>();
         SelectRoute(platform->GetName());
      }
      mSelectedPlatforms.push_back(platformId);
   }
   for (const auto& platform : aUnselected)
   {
      unsigned int platformId   = platform->GetUniqueId();
      QString      platformName = QString::fromStdString(platform->GetName());

      auto it = mPOIToWaypointsMap.find(platformId);
      if (it != mPOIToWaypointsMap.end())
      {
         // If the platform is deselected and none of its waypoints are selected
         if (mPOIToWaypointsMap[platformId].empty())
         {
            mPOIToWaypointsMap.erase(it);
            PlatformOfInterestRemovedHandler(platformName);
         }
      }

      // Remove from selected platforms
      auto iter = std::find(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), platformId);
      if (iter != mSelectedPlatforms.end())
      {
         mSelectedPlatforms.erase(iter);
      }

      PlatformDeselectedHandler(platform);
   }
}

void wkf::RouteBrowserInterface::AttachmentSelected(vespa::VaViewer*                aViewer,
                                                    int                             aSelect,
                                                    vespa::VaAttachment*            aAttachmentPtr,
                                                    const vespa::VaAttachmentSubId& aAttachmentId)
{
   wkf::AttachmentRoute* routePtr = dynamic_cast<wkf::AttachmentRoute*>(aAttachmentPtr);
   if (routePtr != nullptr)
   {
      unsigned int platformId    = routePtr->GetParent().GetUniqueId();
      QString      platformName  = QString::fromStdString(routePtr->GetParent().GetName());
      int          waypointIndex = routePtr->FindWaypointIndex(aAttachmentId.mSubId);

      bool global = (platformName.endsWith("_anchor") ? true : false);

      if (aSelect) // a waypoint was selected
      {
         global ? GlobalWaypointSelected(routePtr, waypointIndex) :
                  LocalWaypointSelected(platformName, platformId, waypointIndex);
      }
      else
      {
         if (aAttachmentId == 0) // if the whole attachment was deselected
         {
            global ? GlobalRouteDeselected(routePtr) : LocalRouteDeselected();
         }
         else // if a single waypoint was deselected
         {
            global ? GlobalWaypointDeselected(routePtr, waypointIndex) :
                     LocalWaypointDeselected(platformName, platformId, waypointIndex);
         }
      }
   }
}

void wkf::RouteBrowserInterface::PlatformsAdded(const wkf::PlatformList& aPlatforms)
{
   for (const auto& platform : aPlatforms)
   {
      mDockWidget->GetCreateDialog()->AddPlatform(QString::fromStdString(platform->GetName()));
   }
}

void wkf::RouteBrowserInterface::EntityRemoved(vespa::VaEntity* aEntityPtr)
{
   mDockWidget->GetCreateDialog()->RemovePlatform(QString::fromStdString(aEntityPtr->GetName()));

   // remove platform from the route browser.
   // We could check to see if it's selected or if one of its route's waypoints is selected first,
   // but calling RemoveLocal on a platform that's not in the route browser won't hurt.
   mDockWidget->RemoveLocal(QString::fromStdString(aEntityPtr->GetName()));
}

// When is a platform is deselected, remove as a platform of interest if
// none of its route's waypoints are selected
void wkf::RouteBrowserInterface::PlatformDeselectedHandler(wkf::Platform* aPlatform)
{
   unsigned int platformId   = aPlatform->GetUniqueId();
   QString      platformName = QString::fromStdString(aPlatform->GetName());

   auto it = mPOIToWaypointsMap.find(platformId);
   if (it != mPOIToWaypointsMap.end())
   {
      // If the platform is deselected and none of its waypoints are selected
      if (mPOIToWaypointsMap[platformId].empty())
      {
         mPOIToWaypointsMap.erase(it);
         PlatformOfInterestRemovedHandler(platformName);
      }
   }

   // Remove from selected platforms
   auto iter = std::find(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), platformId);
   if (iter != mSelectedPlatforms.end())
   {
      mSelectedPlatforms.erase(iter);
   }
}

// Called when a platform is added as a platform of interest
// Populates the route browser with the platform's route info and expands the route (and waypoint, if applicable)
// aRouteInfo: the platform's route info that will populate the route browser
void wkf::RouteBrowserInterface::PlatformOfInterestAddedHandler(const RouteInfo& aRouteInfo)
{
   if (!aRouteInfo.mWaypoints.empty())
   {
      mDockWidget->PopulateLocal(aRouteInfo);
      if (mPOIToWaypointsMap.size() == 1)
      {
         mDockWidget->ExpandLocalRoute(aRouteInfo.mPlatformName, mSelectedWaypoint);
      }
      else
      {
         mDockWidget->CollapseAllLocalRoutes();
      }
   }
   vespa::VaEntity* entity = vaEnv.FindEntity(aRouteInfo.mPlatformName.toStdString());
   if (entity != nullptr)
   {
      wkf::AttachmentRoute* routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(entity->FindAttachment("route"));
      if (routeAttachmentPtr != nullptr)
      {
         // routeAttachmentPtr->RemoveWaypointHighlights();
         routeAttachmentPtr->ShowLabels(true);
      }
   }
}

void wkf::RouteBrowserInterface::AddPlatformOfInterest(unsigned int aPlatformIndex, int aWaypointIndex)
{
   if (mPOIToWaypointsMap.find(aPlatformIndex) == mPOIToWaypointsMap.end())
   {
      mPOIToWaypointsMap[aPlatformIndex] = std::vector<unsigned int>();
   }
   if (aWaypointIndex != -1)
   {
      // don't add it again if the waypoint has already been added
      if (std::find(mPOIToWaypointsMap[aPlatformIndex].begin(), mPOIToWaypointsMap[aPlatformIndex].end(), aWaypointIndex) ==
          mPOIToWaypointsMap[aPlatformIndex].end())
      {
         mPOIToWaypointsMap[aPlatformIndex].push_back(aWaypointIndex);
      }
   }
}

void wkf::RouteBrowserInterface::RemovePlatformOfInterest(unsigned int aPlatformIndex)
{
   auto waypointVecIter = mPOIToWaypointsMap.find(aPlatformIndex);
   if (waypointVecIter != mPOIToWaypointsMap.end())
   {
      mPOIToWaypointsMap.erase(waypointVecIter);
   }
}

void wkf::RouteBrowserInterface::RemovePlatformOfInterestWaypoints(unsigned int aPlatformIndex)
{
   auto waypointVecIter = mPOIToWaypointsMap.find(aPlatformIndex);
   if (waypointVecIter != mPOIToWaypointsMap.end())
   {
      waypointVecIter->second.clear();
   }
}

void wkf::RouteBrowserInterface::RemovePlatformOfInterestWaypoint(unsigned int aPlatformIndex, int aWaypointIndex)
{
   auto waypointVecIter = mPOIToWaypointsMap.find(aPlatformIndex);
   if (waypointVecIter != mPOIToWaypointsMap.end())
   {
      auto waypointIter = std::find(waypointVecIter->second.begin(), waypointVecIter->second.end(), aWaypointIndex);
      if (waypointIter != waypointVecIter->second.end())
      {
         waypointVecIter->second.erase(waypointIter);
      }
   }
}

bool wkf::RouteBrowserInterface::IsSelected(unsigned int aPlatformIndex) const
{
   return std::find(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), aPlatformIndex) != mSelectedPlatforms.end();
}

bool wkf::RouteBrowserInterface::IsPlatformOfInterest(unsigned int aPlatformIndex) const
{
   return mPOIToWaypointsMap.find(aPlatformIndex) != mPOIToWaypointsMap.end();
}

int wkf::RouteBrowserInterface::GetWaypointCount(unsigned int aPlatformIndex) const
{
   auto waypointsIter = mPOIToWaypointsMap.find(aPlatformIndex);
   if (waypointsIter != mPOIToWaypointsMap.end())
   {
      return waypointsIter->second.size();
   }
   return -1;
}

// Called when a platform is removed as a platform of interest
// Removes the platform aPlatformName and its route from the dock widget
void wkf::RouteBrowserInterface::PlatformOfInterestRemovedHandler(const QString& aPlatformName)
{
   // Remove any highlights on Waypoints
   if (mViewer != nullptr)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(aPlatformName.toStdString());
      if (entity != nullptr)
      {
         wkf::AttachmentRoute* routeAttachmentPtr =
            dynamic_cast<wkf::AttachmentRoute*>(entity->FindAttachment("route"));
         if (routeAttachmentPtr != nullptr)
         {
            routeAttachmentPtr->RemoveWaypointHighlights();
            routeAttachmentPtr->ShowLabels(false);
         }
      }
   }

   mDockWidget->RemoveLocal(aPlatformName);
}

void wkf::RouteBrowserInterface::LocalWaypointSelected(const QString& aPlatformName,
                                                       unsigned int   aPlatformIndex,
                                                       int            aWaypointIndex)
{
   if (aWaypointIndex != -1)
   {
      AddPlatformOfInterest(aPlatformIndex, aWaypointIndex);

      if (GetWaypointCount(aPlatformIndex) == 1) // if a single waypoint is selected
      {
         mSelectedWaypoint = aWaypointIndex;
         SelectRoute(aPlatformName.toStdString());
      }
      else // if multiple waypoints are selected
      {
         mSelectedWaypoint = -1;
         mDockWidget->CollapseLocalRoute(aPlatformName);
      }
   }
}

void wkf::RouteBrowserInterface::LocalWaypointDeselected(const QString& aPlatformName,
                                                         unsigned int   aPlatformIndex,
                                                         int            aWaypointIndex)
{
   if (aWaypointIndex != -1)
   {
      if (IsPlatformOfInterest(aPlatformIndex))
      {
         RemovePlatformOfInterestWaypoint(aPlatformIndex, aWaypointIndex);

         if (GetPOICount() == 1 && GetWaypointCount(aPlatformIndex) == 1) // if a single waypoint is selected
         {
            mSelectedWaypoint = mPOIToWaypointsMap[aPlatformIndex][0];
            mDockWidget->ExpandLocalRoute(aPlatformName, mPOIToWaypointsMap[aPlatformIndex][0]);
         }
         else
         {
            mSelectedWaypoint = -1;
            mDockWidget->CollapseLocalRoute(aPlatformName);
            if (GetWaypointCount(aPlatformIndex) == 0) // if nothing is selected
            {
               if (!IsSelected(aPlatformIndex))
               {
                  RemovePlatformOfInterest(aPlatformIndex);
                  PlatformOfInterestRemovedHandler(aPlatformName);
               }
               else
               {
                  // Note: This section may need to change if another selectable attachment besides routes is added.
                  // Selecting a waypoint on the map display also simultaneously selects the parent platform
                  // (implicitly). When all waypoints were deselected, the map display handled the subsequent
                  // deselection of platforms in a callback that was triggered on manual selections (mouse clicks) This
                  // was fine because the only way to select and deselect waypoints was via the map display. Since
                  // waypoints can now be programmatically be deselected via visibility changes, a second check needs to
                  // be performed to make sure the parent platform doesn't remain selected unless it was explicitly
                  // selected
                  wkf::Platform* plat = vaEnv.FindEntity<wkf::Platform>(aPlatformName.toStdString());
                  if (plat && !plat->GetSelected(mViewer))
                  {
                     wkf::PlatformList list = wkfEnv.GetSelectedPlatforms();
                     list.erase(std::remove_if(list.begin(),
                                               list.end(),
                                               [aPlatformName](wkf::Platform* aPlat)
                                               { return aPlat->GetName() == aPlatformName.toStdString(); }),
                                list.end());
                     wkfEnv.SetPlatformsSelected(list);
                  }
               }
            }
         }
      }
   }
}

void wkf::RouteBrowserInterface::LocalRouteDeselected()
{
   mSelectedWaypoint = -1;
   std::vector<unsigned int> keysToRemove; // save off elements we want to remove, erasing elements in unordered_map in loop is UB

   for (auto&& p : mPOIToWaypointsMap)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(p.first);
      if (entity != nullptr)
      {
         PlatformOfInterestRemovedHandler(QString::fromStdString(entity->GetName()));
         keysToRemove.push_back(p.first);

         // Remove from selected platforms
         auto iter = std::find(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), p.first);
         if (iter != mSelectedPlatforms.end())
         {
            mSelectedPlatforms.erase(iter);
         }
      }
   }

   for (auto&& i : keysToRemove)
   {
      RemovePlatformOfInterest(i);
   }
}

void wkf::RouteBrowserInterface::GlobalWaypointSelected(wkf::AttachmentRoute* aRoutePtr, int aWaypointIndex)
{
   QString routeName = QString::fromStdString(aRoutePtr->GetName());

   if (routeName == "route") // we are currently creating a new route, and it hasn't been named yet.
   {
      if (mDockWidget->GetCreateDialog()->isVisible())
      {
         mDockWidget->GetCreateDialog()->SelectWaypoint(aWaypointIndex);
      }
   }
   else
   {
      if (aWaypointIndex != -1)
      {
         unsigned int selectedCount = 0;
         for (auto& w : aRoutePtr->GetWaypoints())
         {
            if (w->IsSelected())
               ++selectedCount;
         }
         if (selectedCount == 1)
         {
            SelectRoute(aRoutePtr->GetParent().GetName());
            mSelectedWaypoint = aWaypointIndex;
            mDockWidget->ExpandGlobalRoute(routeName, aWaypointIndex);
         }
         else
         {
            mSelectedWaypoint = -1;
            mDockWidget->CollapseGlobalRoute(routeName);
         }
      }
   }
}

void wkf::RouteBrowserInterface::GlobalWaypointDeselected(wkf::AttachmentRoute* aRoutePtr, int aWaypointIndex)
{
   QString routeName = QString::fromStdString(aRoutePtr->GetName());

   if (aWaypointIndex != -1)
   {
      unsigned int selectedCount = 0;
      int          selectedIndex = -1;
      int          index         = 0;
      for (auto& w : aRoutePtr->GetWaypoints())
      {
         if (w->IsSelected())
         {
            ++selectedCount;
            selectedIndex = index;
         }
         ++index;
      }
      if (selectedCount == 1)
      {
         mSelectedWaypoint = selectedIndex;
         mDockWidget->ExpandGlobalRoute(routeName, selectedIndex);
      }
      else
      {
         mSelectedWaypoint = -1;
         mDockWidget->CollapseGlobalRoute(routeName);
      }
   }
}

void wkf::RouteBrowserInterface::GlobalRouteDeselected(wkf::AttachmentRoute* aRoutePtr)
{
   QString routeName = QString::fromStdString(aRoutePtr->GetName());

   mSelectedWaypoint = -1;
   mDockWidget->CollapseGlobalRoute(routeName);
}

void wkf::RouteBrowserInterface::GlobalRouteCreatedHandler(const RouteInfo& aRouteInfo)
{
   vespa::VaEntity* routeAnchor = new vespa::VaEntity(aRouteInfo.mName.toStdString() + "_anchor");
   CreateRouteOnEntityAnchor(routeAnchor, aRouteInfo);
}

void wkf::RouteBrowserInterface::DeleteRoute(const QString& aRouteName)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(aRouteName.toStdString() + "_anchor");
      if (entity != nullptr)
      {
         wkf::Scenario* scenario = vaEnv.GetStandardScenario();
         if (scenario != nullptr)
         {
            vespa::VaAttachment* attachment = entity->FindAttachment(aRouteName.toStdString());
            if (attachment != nullptr)
            {
               entity->RemoveAttachment(attachment->GetUniqueId());
            }
            scenario->RemoveEntity(entity);
         }
      }
   }
}

void wkf::RouteBrowserInterface::GlobalRouteEditedHandler(const RouteInfo& aRouteInfo)
{
   // NOTE: The only global routes that are editable are those defined by the user during the simulation.
   // These routes do not yet support relative waypoints, so there is currently no need to check the waypoint type.
   mOriginalVisibilityState                 = true; // global routes are always visible when selected
   wkf::AttachmentRoute* routeAttachmentPtr = nullptr;
   if (mViewer != nullptr)
   {
      vespa::VaEntity* realAnchor = vaEnv.FindEntity(aRouteInfo.mName.toStdString() + "_anchor");
      if (realAnchor != nullptr)
      {
         routeAttachmentPtr =
            dynamic_cast<wkf::AttachmentRoute*>(realAnchor->FindAttachment(aRouteInfo.mName.toStdString()));
         if (routeAttachmentPtr)
         {
            routeAttachmentPtr->DeleteWaypoints();
            DrawRoute(aRouteInfo, routeAttachmentPtr);
            routeAttachmentPtr->SetStateVisibility(false);
            routeAttachmentPtr->SetContextMenuEnabled(true);
         }
      }
      vespa::VaEntity* tempAnchor = vaEnv.FindEntity(cTEMPORARY_ROUTE);
      if (tempAnchor != nullptr)
      {
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment("route"));
         if (routeAttachmentPtr != nullptr)
         {
            RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
            ConstructRoute(aRouteInfo, cTEMPORARY_ROUTE);
         }
      }
   }
}

void wkf::RouteBrowserInterface::CreateRouteOnEntityAnchor(vespa::VaEntity* aEntityPtr, const RouteInfo& aRouteInfo)
{
   if (aEntityPtr && !aRouteInfo.mWaypoints.empty())
   {
      bool relative = false; // does the route contain any relative waypoints?
      for (auto& w : aRouteInfo.mWaypoints)
      {
         if (w.mPointType == WaypointInfo::WaypointType::cRELATIVE)
         {
            relative = true;
         }
      }

      if (!relative) // if the route contains any relative waypoints, don't draw it
      {
         // Currently, we don't need this check since we are first checking the route for any relative waypoints
         // if this is a relative route (i.e. the first waypoint is defined as an offset), don't draw anything
         // if (aRouteInfo.mWaypoints[0].mPointType == WaypointInfo::WaypointType::cABSOLUTE)
         //{
         // double previousLat = 0.0f;
         // double previousLon = 0.0f;
         // double previousHeading = 0.0f;
         double previousAlt = 0.0f;

         vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
         if (scenario != nullptr)
         {
            scenario->AddEntity(aEntityPtr);

            auto routeAttachmentPtr = vespa::make_attachment<wkf::AttachmentRoute>(*aEntityPtr, mViewer);
            routeAttachmentPtr->ShowIndices(false);
            vespa::VaAttachment::LoadAttachment(*routeAttachmentPtr);

            routeAttachmentPtr->SetName(aRouteInfo.mName.toStdString());

            // Set entity position to the first waypoint position - otherwise route will be drawn starting at 0n 0e
            if (!aRouteInfo.mWaypoints.empty())
            {
               WaypointInfo waypointInfo = aRouteInfo.mWaypoints[0];
               aEntityPtr->SetPosition(vespa::VaPosition(waypointInfo.mLat, waypointInfo.mLon, waypointInfo.mAlt));
            }

            wkf::AttachmentRoute::WaypointDataList waypoints;
            // build route
            for (auto& w : aRouteInfo.mWaypoints)
            {
               int goToIndex = -1;
               if (!w.mGoTo.isEmpty())
               {
                  auto wypt = std::find_if(aRouteInfo.mWaypoints.begin(),
                                           aRouteInfo.mWaypoints.end(),
                                           [&](const wkf::RouteBrowserInterface::WaypointInfo& waypoint)
                                           { return w.mGoTo == waypoint.mLabel; });

                  if (wypt != aRouteInfo.mWaypoints.end())
                  {
                     goToIndex = std::distance(aRouteInfo.mWaypoints.begin(), wypt);
                  }
               }

               // Currently we only draw Absolute waypoints (not Relative)
               // if (w.mPointType == WaypointInfo::WaypointType::cABSOLUTE)
               //{
               // if (w.mHeading != WaypointValueType::cNOT_SET)
               //{
               //    previousHeading = w.mHeading;
               // }
               double alt = w.mAlt;
               // Use the previous value for all 3 of the following cases.
               if (alt == WaypointValueType::cUSE_DEFAULT || alt == WaypointValueType::cUSE_PREVIOUS ||
                   alt == WaypointValueType::cNOT_SET)
               {
                  alt = previousAlt;
               }

               waypoints.emplace_back(w.mLat, w.mLon, alt, goToIndex, w.mLabel.toStdString());
               // previousLat = w.mLat;
               // previousLon = w.mLon;
               previousAlt = alt;
               //}
               // If we choose to handle drawing of relative waypoints, put the code here:
               // else if (w.mPointType == WaypointInfo::WaypointType::cRELATIVE)
               //{
               //   double x = w.mXOffset;
               //   double y = w.mYOffset;
               //   double range = sqrt(x * x + y * y);
               //   double relBearing = atan2(y, x);
               //   double headingDeg = (relBearing + previousHeading) * UtMath::cDEG_PER_RAD;
               //   UtSphericalEarth::ExtrapolateGreatCirclePosition(previousLat, previousLon, headingDeg, range, lat, lon);

               //   waypoints.emplace_back(lat, lon, previousAlt, goToIndex, w.mLabel.toStdString(), false);
               //   previousLat = lat;
               //   previousLon = lon;
               //   //previousHeading = relBearing + previousHeading;
               //}
            }
            routeAttachmentPtr->AddWaypoints(waypoints);
            routeAttachmentPtr->SetStateVisibility(true);

            mDockWidget->GetCreateDialog()->RemovePlatform(QString::fromStdString(aEntityPtr->GetName()));
         }
         //}
      }
   }
}

// TODO: In 2.5, for Create/Remove Temporary Routes, need to handle global routes as well (in 2.4 cannot create/edit
// global routes from route browser in Wizard)
void wkf::RouteBrowserInterface::CreateTemporaryRoute(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr != nullptr)
   {
      // hide the platform's original route
      QString owningPlatformName = aRouteInfo.mPlatformName;
      if (!owningPlatformName.isEmpty())
      {
         wkf::Platform* owningPlatform = scenarioPtr->FindPlatformByName(owningPlatformName);
         if (owningPlatform)
         {
            auto& attachmentList = owningPlatform->GetAttachments();
            for (const auto& it : attachmentList)
            {
               wkf::AttachmentRoute* route = dynamic_cast<wkf::AttachmentRoute*>(it.get());
               if (route)
               {
                  // if the temporary route has not been created yet, save off the original route visibility state
                  vespa::VaEntity* tempAnchor = vaEnv.FindEntity(cTEMPORARY_ROUTE);
                  if (!tempAnchor)
                  {
                     mOriginalVisibilityState = route->IsVisible();
                  }
                  route->SetStateVisibility(false);
               }
            }
         }
      }
      else
      {
         // if global, also required to hide
         vespa::VaEntity* baseAnchor = scenarioPtr->FindEntity(aRouteInfo.mName.toStdString() + "_anchor");
         if (baseAnchor)
         {
            vespa::VaAttachment*  attachment  = baseAnchor->FindAttachment(aRouteInfo.mName.toStdString());
            wkf::AttachmentRoute* routeAnchor = dynamic_cast<wkf::AttachmentRoute*>(attachment);
            if (routeAnchor)
            {
               routeAnchor->SetStateVisibility(false);
               mOriginalVisibilityState = true;
            }
         }
      }
      RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
      ConstructRoute(aRouteInfo, cTEMPORARY_ROUTE);
   }
}

void wkf::RouteBrowserInterface::DrawRoute(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo,
                                           wkf::AttachmentRoute*                        aRouteAttachmentPtr)
{
   // Correctly draw the temporary route
   if (aRouteAttachmentPtr)
   {
      const wkf::RouteBrowserInterface::RouteInfo& routeInfo = aRouteInfo;
      int                                          infoIndex = 0;
      wkf::AttachmentRoute::WaypointDataList       waypoints;
      while (infoIndex != routeInfo.mWaypoints.size())
      {
         RouteBrowserInterface::WaypointInfo curInfo = routeInfo.mWaypoints[infoIndex];

         // if this waypoint is set to use previous altitude, assign it the previous altitude so it gets drawn correctly
         if (curInfo.mAlt == WaypointValueType::cUSE_PREVIOUS)
         {
            int prevIdx = infoIndex - 1;
            while (prevIdx >= 0)
            {
               if (routeInfo.mWaypoints[prevIdx].mAlt != WaypointValueType::cUSE_PREVIOUS)
               {
                  curInfo.mAlt = routeInfo.mWaypoints[prevIdx].mAlt;
                  break;
               }
               else
               {
                  --prevIdx;
               }
            }
         }

         // check to see if this waypoint is using agl
         if (curInfo.mAltRef == static_cast<int>(RouteBrowserInterface::WaypointInfo::AltitudeReference::AGL))
         {
            float elevationResult;
            if (wkfEnv.GetResourceManager().QueryElevation(wkf::ResourceManager::cDEFAULT_REQUEST_ID,
                                                           curInfo.mLat,
                                                           curInfo.mLon,
                                                           elevationResult))
            {
               curInfo.mAlt = curInfo.mAlt + elevationResult;
            }
         }
         auto gotoIndexIt =
            std::find_if(routeInfo.mWaypoints.begin(),
                         routeInfo.mWaypoints.end(),
                         [&](const RouteBrowserInterface::WaypointInfo& aInfo)
                         { return (!aInfo.mLabel.isEmpty() && aInfo.mLabel == routeInfo.mWaypoints[infoIndex].mGoTo); });
         int gotoIndex = -1;
         if (gotoIndexIt != routeInfo.mWaypoints.end())
         {
            gotoIndex = std::distance(routeInfo.mWaypoints.begin(), gotoIndexIt);
         }
         infoIndex += 1;

         waypoints.emplace_back(curInfo.mLat, curInfo.mLon, curInfo.mAlt, gotoIndex, curInfo.mLabel.toStdString());
      }
      aRouteAttachmentPtr->AddWaypoints(waypoints);
      aRouteAttachmentPtr->ShowLabels(true);
   }
}

void wkf::RouteBrowserInterface::ConstructRoute(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo,
                                                const std::string&                           aEntityName)
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      auto tempAnchor         = ut::make_unique<vespa::VaEntity>(aEntityName);
      auto routeAttachmentPtr = vespa::make_attachment<wkf::AttachmentRoute>(*tempAnchor, mViewer);
      routeAttachmentPtr->ShowIndices(false);
      routeAttachmentPtr->SetName("route");
      scenario->AddEntity(tempAnchor.release());
      DrawRoute(aRouteInfo, routeAttachmentPtr);
      routeAttachmentPtr->SetTeamColor(UtColor(1, 1, 0, 1));
      routeAttachmentPtr->UpdateTeamColor();
      routeAttachmentPtr->SetSelectionEnabled(false);
      routeAttachmentPtr->SetContextMenuEnabled(false);
   }
}

void wkf::RouteBrowserInterface::RemoveTemporaryRouteP(const std::string& aEntityName)
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (mViewer != nullptr)
   {
      vespa::VaEntity* tempAnchor = vaEnv.FindEntity(aEntityName);
      if ((scenario != nullptr) && (tempAnchor != nullptr))
      {
         wkf::AttachmentRoute* route = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment("route"));
         if (route)
         {
            tempAnchor->RemoveAttachment(route->GetUniqueId());
            scenario->RemoveEntity(tempAnchor);
         }
      }
   }
}

void wkf::RouteBrowserInterface::RemoveTemporaryRoute(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
   // show the platform's original route, with edit
   if (!mDockWidget->GetEditDialog()->isVisible())
   {
      ShowExistingRoute(aRouteInfo);
   }
}

void wkf::RouteBrowserInterface::ShowExistingRoute(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   bool    visibility         = false;
   QString owningPlatformName = aRouteInfo.mPlatformName;
   if (mOriginalVisibilityState)
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         if (!owningPlatformName.isEmpty())
         {
            wkf::Platform* owningPlatform = scenarioPtr->FindPlatformByName(owningPlatformName);
            if (owningPlatform)
            {
               auto& attachmentList = owningPlatform->GetAttachments();
               for (const auto& it : attachmentList)
               {
                  wkf::AttachmentRoute* route = dynamic_cast<wkf::AttachmentRoute*>(it.get());
                  if (route)
                  {
                     route->SetStateVisibility(true);
                     visibility = true;
                  }
               }
            }
         }
         else
         {
            vespa::VaEntity* baseAnchor = scenarioPtr->FindEntity(aRouteInfo.mName.toStdString() + "_anchor");
            if (baseAnchor)
            {
               vespa::VaAttachment*  attachment  = baseAnchor->FindAttachment(aRouteInfo.mName.toStdString());
               wkf::AttachmentRoute* routeAnchor = dynamic_cast<wkf::AttachmentRoute*>(attachment);
               if (routeAnchor)
               {
                  routeAnchor->SetStateVisibility(true);
                  visibility = true;
               }
            }
         }
      }
   }
   mOriginalVisibilityState = visibility;
}

void wkf::RouteBrowserInterface::CreateRouteStartedHandler(const QString& aRouteName)
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntity* tempAnchor = vaEnv.FindEntity(cTEMPORARY_ROUTE);
      if (!tempAnchor)
      {
         tempAnchor               = new vespa::VaEntity(cTEMPORARY_ROUTE);
         mEntityAnchorInitialized = false;
         auto newRoute            = vespa::make_attachment<wkf::AttachmentRoute>(*tempAnchor, mViewer);
         newRoute->SetSelectionEnabled(false);
         scenario->AddEntity(tempAnchor);
      }
   }
}

void wkf::RouteBrowserInterface::CreateRouteTerminatedHandler(const QString& aRouteName)
{
   RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
}

void wkf::RouteBrowserInterface::WaypointAddedHandler(const WaypointInfo& aWaypoint,
                                                      int                 aInsertIndex,
                                                      const QString&      aPlatformName,
                                                      const QString&      aRouteName)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointAddedHandler(aWaypoint, aInsertIndex, aRouteName);
   }
   else
   {
      LocalWaypointAddedHandler(aWaypoint, aInsertIndex, aPlatformName);
   }
}

void wkf::RouteBrowserInterface::WaypointRemovedHandler(int aIndex, const QString& aPlatformName, const QString& aRouteName)
{
   if (aPlatformName.isEmpty())
   {
      GlobalWaypointRemovedHandler(aIndex, aRouteName);
   }
   else
   {
      LocalWaypointRemovedHandler(aIndex, aPlatformName);
   }
}

void wkf::RouteBrowserInterface::GlobalWaypointAddedHandler(const WaypointInfo& aWaypoint,
                                                            int                 aInsertIndex,
                                                            const QString&      aRouteName)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity*      tempAnchor         = vaEnv.FindEntity(cTEMPORARY_ROUTE);
      std::string           routeName          = "route";
      wkf::AttachmentRoute* routeAttachmentPtr = nullptr;

      if (tempAnchor == nullptr)
      {
         tempAnchor = vaEnv.FindEntity(aRouteName.toStdString() + "_anchor");
         routeName  = aRouteName.toStdString();
      }
      if (tempAnchor != nullptr)
      {
         if (!mEntityAnchorInitialized)
         {
            tempAnchor->SetPosition(vespa::VaPosition(aWaypoint.mLat, aWaypoint.mLon, aWaypoint.mAlt));
            mEntityAnchorInitialized = true;
         }
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment(routeName));
      }

      if (routeAttachmentPtr != nullptr)
      {
         RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
         ConstructRoute(mDockWidget->GetCreateDialog()->GetRouteInfo(), cTEMPORARY_ROUTE);
         tempAnchor         = vaEnv.FindEntity(cTEMPORARY_ROUTE);
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment(routeName));
         // deselect adjacent waypoints - we don't know if user inserted waypoint "before" or "after", so do both
         if (routeAttachmentPtr != nullptr)
         {
            routeAttachmentPtr->SelectWaypoint(aInsertIndex - 1, false);
            routeAttachmentPtr->SelectWaypoint(aInsertIndex + 1, false);

            // select added waypoint
            routeAttachmentPtr->SelectWaypoint(aInsertIndex);
         }
      }
   }
}

void wkf::RouteBrowserInterface::LocalWaypointAddedHandler(const WaypointInfo& aWaypoint,
                                                           int                 aInsertIndex,
                                                           const QString&      aPlatformName)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(aPlatformName.toStdString());
      if (entity != nullptr)
      {
         wkf::AttachmentRoute* routeAttachmentPtr =
            dynamic_cast<wkf::AttachmentRoute*>(entity->FindAttachment("route"));
         if (routeAttachmentPtr != nullptr)
         {
            routeAttachmentPtr->InsertWaypoint(aWaypoint.mLat,
                                               aWaypoint.mLon,
                                               aWaypoint.mAlt,
                                               -1,
                                               aWaypoint.mLabel.toStdString(),
                                               aInsertIndex);

            // deselect adjacent waypoints - we don't know if user inserted waypoint "before" or "after", so do both
            routeAttachmentPtr->SelectWaypoint(aInsertIndex - 1, false);
            routeAttachmentPtr->SelectWaypoint(aInsertIndex + 1, false);

            // select added waypoint
            routeAttachmentPtr->SelectWaypoint(aInsertIndex);
         }
      }
   }
}


void wkf::RouteBrowserInterface::GlobalWaypointRemovedHandler(int aIndex, const QString& aRouteName)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity*      tempAnchor         = vaEnv.FindEntity(cTEMPORARY_ROUTE);
      std::string           routeName          = "route";
      wkf::AttachmentRoute* routeAttachmentPtr = nullptr;

      if (tempAnchor == nullptr)
      {
         tempAnchor = vaEnv.FindEntity(aRouteName.toStdString() + "_anchor");
         routeName  = aRouteName.toStdString();
      }
      if (tempAnchor != nullptr)
      {
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment(routeName));
      }

      if (routeAttachmentPtr != nullptr)
      {
         RemoveTemporaryRouteP(cTEMPORARY_ROUTE);
         ConstructRoute(mDockWidget->GetCreateDialog()->GetRouteInfo(), cTEMPORARY_ROUTE);
         tempAnchor         = vaEnv.FindEntity(cTEMPORARY_ROUTE);
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(tempAnchor->FindAttachment(routeName));
         if (!routeAttachmentPtr->SelectWaypoint(aIndex))
         {
            routeAttachmentPtr->SelectWaypoint(routeAttachmentPtr->GetWaypoints().size() - 1);
         }
      }
   }
}

void wkf::RouteBrowserInterface::LocalWaypointRemovedHandler(int aIndex, const QString& aPlatformName)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(aPlatformName.toStdString());
      if (entity != nullptr)
      {
         wkf::AttachmentRoute* routeAttachmentPtr =
            dynamic_cast<wkf::AttachmentRoute*>(entity->FindAttachment("route"));
         if (routeAttachmentPtr != nullptr)
         {
            routeAttachmentPtr->DeleteWaypoint(aIndex);
            if (!routeAttachmentPtr->SelectWaypoint(aIndex))
            {
               routeAttachmentPtr->SelectWaypoint(routeAttachmentPtr->GetWaypoints().size() - 1);
            }
         }
      }
   }
}

void wkf::RouteBrowserInterface::DialogLocalWaypointSelectedHandler(const QString& aPlatformName, int aWaypointIndex)
{
   mDockWidget->CollapseLocalRoute(aPlatformName);
   mDockWidget->ExpandLocalRoute(aPlatformName, aWaypointIndex);
   if (mViewer != nullptr)
   {
      vespa::VaEntity* entity = vaEnv.FindEntity(aPlatformName.toStdString());
      if (entity != nullptr)
      {
         wkf::AttachmentRoute* routeAttachmentPtr =
            dynamic_cast<wkf::AttachmentRoute*>(entity->FindAttachment("route"));
         if (routeAttachmentPtr != nullptr)
         {
            routeAttachmentPtr->SetDraggable(false);

            if (aWaypointIndex != -1)
            {
               wkf::Waypoint* waypoint = routeAttachmentPtr->GetWaypoint(aWaypointIndex);
               if (waypoint != nullptr)
               {
                  waypoint->SetMovable(true);
               }
            }
         }
      }
   }
}

void wkf::RouteBrowserInterface::DialogGlobalWaypointSelectedHandler(const QString& aRouteName, int aWaypointIndex)
{
   mDockWidget->CollapseGlobalRoute(aRouteName);
   mDockWidget->ExpandGlobalRoute(aRouteName, aWaypointIndex);
   if (mViewer != nullptr)
   {
      wkf::AttachmentRoute* routeAttachmentPtr = nullptr;
      vespa::VaEntity*      anchor             = vaEnv.FindEntity(aRouteName.toStdString() + "_anchor");
      if (anchor != nullptr)
      {
         routeAttachmentPtr = dynamic_cast<wkf::AttachmentRoute*>(anchor->FindAttachment(aRouteName.toStdString()));
      }

      if (routeAttachmentPtr != nullptr)
      {
         routeAttachmentPtr->SetDraggable(false);

         if (aWaypointIndex != -1)
         {
            wkf::Waypoint* waypoint = routeAttachmentPtr->GetWaypoint(aWaypointIndex);
            if (waypoint != nullptr)
            {
               waypoint->SetMovable(true);
            }
         }
      }
   }
}

void wkf::RouteBrowserInterface::CreateDialogGlobalWaypointSelectedHandler(const QString& aRouteName, int aWaypointIndex)
{
   if (!aRouteName.isEmpty())
   {
      vespa::VaEntity* currEntity = vaEnv.FindEntity(cTEMPORARY_ROUTE);
      if (currEntity != nullptr)
      {
         wkf::AttachmentRoute* currRoute = dynamic_cast<wkf::AttachmentRoute*>(currEntity->FindAttachment("route"));

         if (currRoute != nullptr)
         {
            for (auto& w : currRoute->GetWaypoints())
            {
               w->SetSelected(false);
            }

            if (aWaypointIndex >= 0 && aWaypointIndex < (int)currRoute->GetWaypoints().size())
            {
               currRoute->GetWaypoint(aWaypointIndex)->SetSelected(true);
               mSelectedWaypoint = aWaypointIndex;
            }
         }
      }
   }
}

void wkf::RouteBrowserInterface::DockGlobalRouteSelectedHandler(const QString& aPrevRouteName,
                                                                int            aPrevWaypoint,
                                                                const QString& aCurrRouteName,
                                                                int            aCurrWaypoint)
{
   if (mViewer != nullptr)
   {
      if (QString::compare(aPrevRouteName, aCurrRouteName) != 0)
      {
         wkf::AttachmentRoute* prevRoute = nullptr;
         wkf::AttachmentRoute* currRoute = nullptr;

         if (!aPrevRouteName.isEmpty())
         {
            vespa::VaEntity* prevEntity = vaEnv.FindEntity(aPrevRouteName.toStdString() + "_anchor");
            if (prevEntity != nullptr)
            {
               prevRoute = dynamic_cast<wkf::AttachmentRoute*>(prevEntity->FindAttachment(aPrevRouteName.toStdString()));
            }
         }

         if (!aCurrRouteName.isEmpty())
         {
            vespa::VaEntity* currEntity = vaEnv.FindEntity(aCurrRouteName.toStdString() + "_anchor");
            if (currEntity != nullptr)
            {
               currRoute = dynamic_cast<wkf::AttachmentRoute*>(currEntity->FindAttachment(aCurrRouteName.toStdString()));
            }
         }

         if (prevRoute != nullptr)
         {
            // hide previous route
            prevRoute->SetStateVisibility(false);
         }

         if (currRoute != nullptr)
         {
            // show current route
            currRoute->SetStateVisibility(true);
         }
      }
   }
}

void wkf::RouteBrowserInterface::DockRouteSelectionChangedHandler(const QString& aPrevPlatformName,
                                                                  const QString& aPrevRouteName,
                                                                  int            aPrevWaypoint,
                                                                  const QString& aCurrPlatformName,
                                                                  const QString& aCurrRouteName,
                                                                  int            aCurrWaypoint)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity* prevEntity = nullptr;
      vespa::VaEntity* currEntity = nullptr;

      wkf::AttachmentRoute* prevRoute = nullptr;
      wkf::AttachmentRoute* currRoute = nullptr;

      if (aPrevPlatformName.isEmpty()) // global route
      {
         if (!aPrevRouteName.isEmpty())
         {
            prevEntity = vaEnv.FindEntity(aPrevRouteName.toStdString() + "_anchor");
            if (prevEntity != nullptr)
            {
               prevRoute = dynamic_cast<wkf::AttachmentRoute*>(prevEntity->FindAttachment(aPrevRouteName.toStdString()));
            }
         }
      }
      else // local route
      {
         prevEntity = vaEnv.FindEntity(aPrevPlatformName.toStdString());
         if (prevEntity != nullptr)
         {
            prevRoute = dynamic_cast<wkf::AttachmentRoute*>(prevEntity->FindAttachment("route"));
         }
      }

      if (aCurrPlatformName.isEmpty()) // global route
      {
         if (!aCurrRouteName.isEmpty())
         {
            currEntity = vaEnv.FindEntity(aCurrRouteName.toStdString() + "_anchor");
            if (currEntity != nullptr)
            {
               currRoute = dynamic_cast<wkf::AttachmentRoute*>(currEntity->FindAttachment(aCurrRouteName.toStdString()));
            }
         }
      }
      else // local route
      {
         currEntity = vaEnv.FindEntity(aCurrPlatformName.toStdString());
         if (currEntity != nullptr)
         {
            currRoute = dynamic_cast<wkf::AttachmentRoute*>(currEntity->FindAttachment("route"));
         }
      }

      if (currRoute != nullptr)
      {
         if (aCurrWaypoint != -1) // If we didn't multi-select
         {
            for (auto& w : currRoute->GetWaypoints())
            {
               w->SetSelected(false);
            }
         }
         if (prevEntity != nullptr && prevRoute != nullptr)
         {
            if (aPrevWaypoint >= 0 && aPrevWaypoint < (int)prevRoute->GetWaypoints().size())
            {
               if (!prevRoute->GetSelected(mViewer) && !prevEntity->GetSelected(mViewer))
               {
                  prevRoute->ShowLabels(false);
               }
            }
            mSelectedWaypoint = -1;
         }
      }

      if (currRoute != nullptr)
      {
         if (currRoute->GetStateVisibility()) // only show labels if the route is visible
         {
            currRoute->ShowLabels(true);
            if (aCurrWaypoint >= 0 && aCurrWaypoint < (int)currRoute->GetWaypoints().size())
            {
               currRoute->GetWaypoint(aCurrWaypoint)->SetSelected(true);
               mSelectedWaypoint = aCurrWaypoint;
            }
         }
      }
   }
}

// private
// Called when user clicks on Edit action in the right-click context menu
void wkf::RouteBrowserInterface::EditRoute()
{
   mDockWidget->EditClickedHandler();
}

void wkf::RouteBrowserInterface::TeamColorChanged()
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntityList list;
      scenario->GetEntityList(list);
      for (auto& i : list)
      {
         wkf::Platform* entityPtr = dynamic_cast<wkf::Platform*>(i);
         if (entityPtr != nullptr)
         {
            wkf::AttachmentRoute* route = dynamic_cast<wkf::AttachmentRoute*>(entityPtr->FindAttachment("route"));
            if (route != nullptr)
            {
               route->SetTeamColor(
                  wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(entityPtr->GetSide()));
               route->UpdateTeamColor();
            }
         }
      }
   }
}

int wkf::RouteBrowserInterface::GetCurrentWaypoint() const
{
   return mDockWidget->GetEditDialog()->GetCurrentWaypoint();
}

void wkf::RouteBrowserInterface::RouteEditedHandler(const RouteInfo& aRouteInfo)
{
   if (mViewer != nullptr)
   {
      vespa::VaEntity* currEntity = vaEnv.FindEntity(aRouteInfo.mPlatformName.toStdString());
      if (currEntity != nullptr)
      {
         wkf::AttachmentRoute* route = dynamic_cast<wkf::AttachmentRoute*>(currEntity->FindAttachment("route"));
         if (route != nullptr)
         {
            int routeInfoSize = aRouteInfo.mWaypoints.size();
            int routeSize     = route->GetWaypoints().size();
            if (routeInfoSize == routeSize)
            {
               for (int i = 0; i < aRouteInfo.mWaypoints.size(); ++i)
               {
                  wkf::RouteBrowserInterface::WaypointInfo waypointInfo = aRouteInfo.mWaypoints.at(i);

                  int goToIndex = -1;
                  if (!waypointInfo.mGoTo.isEmpty())
                  {
                     auto wypt = std::find_if(aRouteInfo.mWaypoints.begin(),
                                              aRouteInfo.mWaypoints.end(),
                                              [&](const wkf::RouteBrowserInterface::WaypointInfo& waypoint)
                                              { return waypointInfo.mGoTo == waypoint.mLabel; });

                     if (wypt != aRouteInfo.mWaypoints.end())
                     {
                        goToIndex = std::distance(aRouteInfo.mWaypoints.begin(), wypt);
                     }
                  }
                  route->UpdateWaypoint(ut::cast_to_size_t(i),
                                        waypointInfo.mLat,
                                        waypointInfo.mLon,
                                        waypointInfo.mAlt,
                                        goToIndex,
                                        waypointInfo.mLabel.toStdString());
               }
            }
         }
      }
   }
}

// Called upon a context menu request (right-click) in the route browser
// Creates an edit action on a context menu
// aPoint: point to show the context menu
// aRoute: name of the route selected (empty for a non-global route)
// aPlatform: name of the platform selected (empty for a global route)
void wkf::RouteBrowserInterface::ContextMenuRequestedHandler(QWidget*       aParent,
                                                             const QPoint&  aPoint,
                                                             const QString& aRoute,
                                                             const QString& aPlatform,
                                                             bool           aEditable,
                                                             bool           aDeleteable)
{
   QMenu*  menu       = new QMenu(aParent);
   QString editText   = "";
   QString deleteText = "";
   if (!aPlatform.isEmpty()) // Local Route
   {
      editText = "Edit Route on " + aPlatform;
   }
   else if (!aRoute.isEmpty()) // Global Route
   {
      editText   = "Edit " + aRoute;
      deleteText = "Delete " + aRoute;
   }
   if (aEditable)
   {
      wkf::Action* editAction = new wkf::Action(editText, menu);
      connect(editAction, &QAction::triggered, std::bind(&RouteBrowserInterface::EditRoute, this));
   }
   if (aDeleteable)
   {
      wkf::Action* deleteAction = new wkf::Action(deleteText, menu);
      connect(deleteAction, &QAction::triggered, std::bind(&RouteBrowserInterface::DeleteRoute, this, aRoute));
   }
   if (menu)
   {
      auto triggeredAction = menu->exec(aPoint);
      if (triggeredAction && triggeredAction->text() == deleteText)
      {
         emit DeleteRouteHandler(aRoute);
      }
   }
}
