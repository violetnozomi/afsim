// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvPluginRoute.hpp"

#include <QHeaderView>

#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtSphericalEarth.hpp"
#include "VaUtils.hpp"
#include "WkfMainWindow.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "route/WkfAttachmentRoute.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvRoute::Plugin, "Route", "Draws platform routes", "mystic")

namespace
{
const rv::MsgRouteChanged* GetRouteData(wkf::Platform& aPlatform)
{
   rv::ResultData* currentData = rvEnv.GetData();
   if (currentData != nullptr)
   {
      rv::ResultPlatform* rplatform = currentData->FindPlatform(aPlatform.GetIndex());
      if (rplatform != nullptr)
      {
         return rplatform->FindFirstBefore<rv::MsgRouteChanged>(currentData->GetSimTime());
      }
   }
   return nullptr;
}

void AddItem(QTableWidget* aTable, unsigned int aRow, unsigned int aColumn, const QString& aLabel)
{
   QTableWidgetItem* item = new QTableWidgetItem(aLabel);
   item->setFlags(item->flags() & ~Qt::ItemIsEditable);
   aTable->setItem(aRow, aColumn, item);
}
} // namespace

RvRoute::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
   RegisterOption(nullptr, "Route");
}

void RvRoute::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      for (auto& rteData : mRouteMap)
      {
         wkf::Platform* plat = scenarioPtr->FindPlatformByIndex(rteData.first);
         if (plat)
         {
            rteData.second.first = BuildRoute(*plat, *rteData.second.second, rteData.second.first);
         }
      }
   }
}

void RvRoute::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvRoute::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      if (GetRouteData(*static_cast<wkf::Platform*>(aEntityPtr)))
      {
         QAction* projectorAction = new QAction(QIcon::fromTheme("route"), QString("Show Route Information"), aMenu);
         aMenu->addAction(projectorAction);
         connect(projectorAction,
                 &QAction::triggered,
                 this,
                 [=]() { ShowRouteInformation(*static_cast<wkf::Platform*>(aEntityPtr)); });
      }
   }
}

void RvRoute::Plugin::ResetOptionStates()
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      for (auto& it : mRouteMap)
      {
         wkf::Platform* plat = scenarioPtr->FindPlatformByIndex(it.first);
         plat->RemoveAttachment(it.second.second->GetUniqueId());
      }
   }
   mRouteMap.clear();
}

void RvRoute::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aState)
   {
      wkf::AttachmentRoute* routePtr = aPlatformPtr->FindFirstAttachmentOfType<wkf::AttachmentRoute>();
      if (!routePtr)
      {
         auto attPtr = vespa::make_attachment<wkf::AttachmentRoute>(*aPlatformPtr, vaEnv.GetStandardViewer());
         attPtr->ShowIndices(false);

         mRouteMap[aPlatformPtr->GetIndex()] = std::make_pair(BuildRoute(*aPlatformPtr, *attPtr, 0), attPtr);
      }
   }
   else
   {
      wkf::AttachmentRoute* routePtr = aPlatformPtr->FindFirstAttachmentOfType<wkf::AttachmentRoute>();
      if (routePtr) // if there is an attachment
      {
         mRouteMap.erase(aPlatformPtr->GetIndex());
         aPlatformPtr->RemoveAttachment(routePtr->GetUniqueId());
         // remove the attachment
      }
   }
}

Qt::CheckState RvRoute::Plugin::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const
{
   if (mRouteMap.find(aPlatformPtr->GetIndex()) != mRouteMap.end())
   {
      return Qt::Checked;
   }
   return Qt::Unchecked;
}

void RvRoute::Plugin::ClearScenario(bool aFullReset)
{
   if (aFullReset)
   {
      for (auto& dlg : mDialogMap)
      {
         if (!dlg.second.isNull())
         {
            dlg.second->close();
         }
      }
      mDialogMap.clear();
      mRouteMap.clear();
   }
}

int RvRoute::Plugin::BuildRoute(wkf::Platform& aPlatform, wkf::AttachmentRoute& aRoute, int aPreviousMessageIdx)
{
   auto rte = GetRouteData(aPlatform);
   auto dlg = mDialogMap.find(aPlatform.GetIndex());
   if (dlg != mDialogMap.end())
   {
      if (!dlg->second.isNull())
      {
         dlg->second->Populate(GetRouteData(aPlatform));
      }
   }

   if (rte && (rte->GetMessageIndex() != mRouteMap[aPlatform.GetIndex()].first))
   {
      aRoute.Clear();
      if (!rte->route().empty())
      {
         auto     wc     = rte->location();
         double   xyz[3] = {wc.x(), wc.y(), wc.z()};
         UtEntity ent;
         ent.SetLocationWCS(xyz);
         double previousLat;
         double previousLon;
         double previousAlt;
         ent.GetLocationLLA(previousLat, previousLon, previousAlt);
         double                                 heading = rte->heading();
         double                                 lat, lon, alt;
         wkf::AttachmentRoute::WaypointDataList wpd;

         for (const auto& wp : rte->route())
         {
            if (wp.locationType() == rv::WaypointLocationType::LatitiudeAndLongitude)
            {
               heading = 0;
               if (wp.headingValid())
               {
                  heading = wp.heading();
               }

               lat = wp.locationX();
               lon = wp.locationY();

               if (wp.altitudeValid())
               {
                  alt = wp.altitude();
               }
               else
               {
                  alt = previousAlt;
               }
               int goToIndex = -1;
               if (wp.gotoIdValid())
               {
                  auto wypt = std::find_if(rte->route().begin(),
                                           rte->route().end(),
                                           [&](const rv::Waypoint& waypoint) { return wp.gotoId() == waypoint.label(); });

                  if (wypt != rte->route().end())
                  {
                     goToIndex = std::distance(rte->route().begin(), wypt);
                  }
               }
               wpd.emplace_back(lat, lon, alt, goToIndex);
               previousAlt = alt;
            }
            else if (wp.locationType() == rv::WaypointLocationType::RelativeOffset)
            {
               double x = wp.locationX();
               double y = wp.locationY();
               if (wp.altitudeValid())
               {
                  alt = wp.altitude();
               }
               else
               {
                  alt = previousAlt;
               }
               double range      = sqrt(x * x + y * y);
               double relBearing = atan2(y, x);
               double headingDeg = (relBearing + heading) * UtMath::cDEG_PER_RAD;
               UtSphericalEarth::ExtrapolateGreatCirclePosition(previousLat, previousLon, headingDeg, range, lat, lon);

               int goToIndex = -1;
               if (wp.gotoIdValid())
               {
                  auto wypt = std::find_if(rte->route().begin(),
                                           rte->route().end(),
                                           [&](const rv::Waypoint& waypoint) { return wp.gotoId() == waypoint.label(); });

                  if (wypt != rte->route().end())
                  {
                     goToIndex = std::distance(rte->route().begin(), wypt);
                  }
               }
               wpd.emplace_back(lat, lon, alt, goToIndex);
               previousLat = lat;
               previousLon = lon;
               previousAlt = alt;
            }
         }
         aRoute.AddWaypoints(wpd);
      }
      return rte->GetMessageIndex();
   }
   else if (!rte)
   {
      aRoute.Clear();
      return 0;
   }

   return aPreviousMessageIdx;
}

void RvRoute::Plugin::ShowRouteInformation(wkf::Platform& aPlatform)
{
   auto dlg = mDialogMap[aPlatform.GetIndex()];

   if (dlg.isNull())
   {
      wkf::AttachmentRoute* rte = nullptr;
      auto                  rme = mRouteMap.find(aPlatform.GetIndex());
      if (rme != mRouteMap.end())
      {
         rte = rme->second.second;
      }
      auto dlg = mDialogMap[aPlatform.GetIndex()] = new RouteDialog(wkfEnv.GetMainWindow(), rte); // create and file the UI
      dlg->setWindowTitle(QString::fromStdString(aPlatform.GetName()) + " route");
      dlg->Populate(GetRouteData(aPlatform));
   }
   else
   {
      dlg->raise();
   }
}

RvRoute::RouteDialog::RouteDialog(QWidget* aParent, wkf::AttachmentRoute* aRoutePtr)
   : QDialog(aParent)
   , mRoutePtr(aRoutePtr)
{
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowFlag(Qt::WindowContextHelpButtonHint, false);
   setLayout(new QVBoxLayout(this));
   layout()->addWidget(mTable = new QTableWidget(this));
   mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   mTable->setRowCount(0);
   mTable->setColumnCount(6);
   QStringList header{"label", "X", "Y", "Altitude", "Heading", "Goto"};
   mTable->setHorizontalHeaderLabels(header);
   int w = 50 + mTable->verticalHeader()->width();
   for (int i = 0; i < 6; ++i)
   {
      w += mTable->columnWidth(i);
   }

   auto* unitPrefs = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   connect(unitPrefs, &wkf::UnitsObject::UnitsChanged, [this]() { Populate(&mRouteData); });

   if (mRoutePtr)
   {
      connect(mTable,
              &QTableWidget::cellClicked,
              [=](int aRow, int aColumn)
              {
                 mRoutePtr->ClearSelection(mRoutePtr->GetViewer());
                 mRoutePtr->SelectWaypoint(aRow);
              });
      mCallbacks.Add(vespa::VaObserver::AttachmentSelected.Connect(&RouteDialog::AttachmentSelectedCB, this));
   }
   resize(w, 400);
   show();
}

void RvRoute::RouteDialog::Populate(const rv::MsgRouteChanged* aRouteData)
{
   if (aRouteData == nullptr)
   {
      mRouteData.route().Resize(0);
      mTable->setRowCount(0);
   }
   else
   {
      mRouteData      = *aRouteData;
      auto* unitPrefs = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
      mTable->setRowCount(aRouteData->route().size());
      int i = 0;
      for (auto wp : aRouteData->route())
      {
         AddItem(mTable, i, 0, QString::fromStdString(wp.label()));
         if (wp.locationType() == rv::WaypointLocationType::LatitiudeAndLongitude)
         {
            AddItem(mTable, i, 1, unitPrefs->GetFormattedValue(wkf::ValueType::cLATITUDE, wp.locationX()));
            AddItem(mTable, i, 2, unitPrefs->GetFormattedValue(wkf::ValueType::cLONGITUDE, wp.locationY()));
         }
         else
         {
            AddItem(mTable, i, 1, unitPrefs->GetFormattedValue(wkf::ValueType::cLENGTH, wp.locationX()));
            AddItem(mTable, i, 2, unitPrefs->GetFormattedValue(wkf::ValueType::cLENGTH, wp.locationY()));
         }
         if (wp.altitudeValid())
         {
            AddItem(mTable, i, 3, unitPrefs->GetFormattedValue(wkf::ValueType::cALTITUDE, wp.altitude()));
         }
         else
         {
            AddItem(mTable, i, 3, "use previous");
         }
         if (wp.headingValid())
         {
            AddItem(mTable, i, 4, unitPrefs->GetFormattedValue(wkf::ValueType::cANGLE, wp.heading()));
         }
         else
         {
            AddItem(mTable, i, 4, "use previous");
         }
         if (wp.gotoIdValid())
         {
            AddItem(mTable, i, 5, QString::fromStdString(wp.gotoId()));
         }
         ++i;
      }
   }
}

void RvRoute::RouteDialog::AttachmentSelectedCB(vespa::VaViewer*                aViewerPtr,
                                                int                             aState,
                                                vespa::VaAttachment*            aAttachmentPtr,
                                                const vespa::VaAttachmentSubId& aSubPartId)
{
   if ((aAttachmentPtr->GetUniqueId() == mRoutePtr->GetUniqueId()) && (aState != 0) && (aSubPartId.mSubId != 0))
   {
      mTable->selectRow(mRoutePtr->FindWaypointIndex(aSubPartId.mSubId));
   }
}
