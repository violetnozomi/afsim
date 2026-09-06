// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "TracksPlugin.hpp"

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QTextBrowser>

#include "PlotUpdater.hpp"
#include "PlottingWidget.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "UtColor.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtQt.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentModel.hpp"
#include "VaObserver.hpp"
#include "WkfAttachmentLabel.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfMilStdIconPrefObject.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfTrack.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "misc/WkfTrackDisplayInterface.hpp"
#include "visibility/WkfTrackVisibilityPrefObject.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvTracks::Plugin, "Tracks", "Displays track information on map displays.", "mystic")

const QString RvTracks::Plugin::TRACK_SEPARATOR = " Track:";

//! These functions exist to allow a recent change in local tracks and sensor tracks to not break the
//! PlotTrackChildItems abstraction.
//! {
static bool IsMasterProcessor(const rv::MsgLocalTrackUpdate& aMsg, bool)
{
   return aMsg.masterProcessor();
}

static bool IsMasterProcessor(const rv::MsgSensorTrackUpdate&, bool aDefault)
{
   return aDefault;
}

template<typename T>
static constexpr int WidgetIndex();

template<>
constexpr int WidgetIndex<rv::MsgLocalTrackUpdate>()
{
   return 0;
}

template<>
constexpr int WidgetIndex<rv::MsgSensorTrackUpdate>()
{
   return 1;
}
//! }

// MUST BE DEFINED FIRST!
template<typename MessageUpdateType, typename MessageCreateType>
void RvTracks::Plugin::PlotTrackChildItems(RvTracks::Plugin::TrackChildItems& aTrackChildItems,
                                           const rv::Track_Id&                aTrackId,
                                           const rv::TrackDb::TrackList<MessageUpdateType, MessageCreateType>& aMessages,
                                           double                                                              aSimTime,
                                           bool                                                                aLocal)
{
   int trackId = aTrackId.localTrackNumber();
   if ((aSimTime <= aMessages.mUpdates.GetMinTime()) || (aSimTime > aMessages.mDropTime))
   {
      if (aTrackChildItems.find(trackId) != aTrackChildItems.end())
      {
         mTopLevelWidgets.at(WidgetIndex<MessageUpdateType>())->removeChild(aTrackChildItems.at(trackId).mParentItem);
         aTrackChildItems.erase(trackId);
      }
   }
   else
   {
      MessageUpdateType* trackUpdate = aMessages.mUpdates.FindFirstBefore(aSimTime);
      if (trackUpdate && IsMasterProcessor(*trackUpdate, true))
      {
         if (aTrackChildItems.find(trackId) == aTrackChildItems.end())
         {
            aTrackChildItems.emplace(trackId,
                                     RvTracks::Plugin::TrackItems(mTopLevelWidgets.at(WidgetIndex<MessageUpdateType>()),
                                                                  trackId,
                                                                  QString::fromStdString(aTrackId.owner()),
                                                                  aLocal));
         }

         UtEntity         entity;
         const rv::Track& track         = trackUpdate->track();
         const rv::Vec3d& location      = track.locationWCS();
         const rv::Vec3f& velocity      = track.velocityWCS();
         const bool       locationValid = track.locationWCSValid();
         const bool       velocityValid = track.velocityWCSValid();

         const double lwcs[3] = {location.x(), location.y(), location.z()};
         const double vwcs[3] = {velocity.x(), velocity.y(), velocity.z()};
         entity.SetLocationWCS(lwcs);
         entity.SetVelocityWCS(vwcs);
         double vel[3] = {0, 0, 0};
         entity.GetVelocityNED(vel);

         double heading_rad = atan2(vel[1], vel[0]);
         // For display purposes put the valid heading range in [0, 2*PI]
         if (heading_rad < 0)
         {
            heading_rad += UtMath::cTWO_PI;
         }

         double lat, lon, alt_m;
         entity.GetLocationLLA(lat, lon, alt_m);
         const double  speed_mps = entity.GetSpeed();
         const QString domain =
            QString::fromStdString(wkf::SpatialDomainToString(static_cast<wkf::SpatialDomain>(track.spatialDomain())));

         aTrackChildItems.at(trackId).mLatitude->SetHidden(!locationValid);
         aTrackChildItems.at(trackId).mLongitude->SetHidden(!locationValid);
         aTrackChildItems.at(trackId).mAltitude->SetHidden(!locationValid);
         aTrackChildItems.at(trackId).mSpeed->SetHidden(!velocityValid);
         aTrackChildItems.at(trackId).mHeading->SetHidden(!velocityValid);
         aTrackChildItems.at(trackId).mPositionValid->SetHidden(locationValid);
         aTrackChildItems.at(trackId).mVelocityValid->SetHidden(velocityValid);
         aTrackChildItems.at(trackId).mSide->SetHidden(!track.sideValid());
         aTrackChildItems.at(trackId).mType->SetHidden(!track.typeValid());
         aTrackChildItems.at(trackId).mSpatialDomain->SetHidden(!track.spatialDomainValid());
         aTrackChildItems.at(trackId).mQuality->SetHidden(!track.trackQualityValid());

         aTrackChildItems.at(trackId).mLatitude->SetValue(lat);
         aTrackChildItems.at(trackId).mLongitude->SetValue(lon);
         aTrackChildItems.at(trackId).mAltitude->SetValue(alt_m);
         aTrackChildItems.at(trackId).mPositionValid->setText(1, locationValid ? "true" : "false");
         aTrackChildItems.at(trackId).mVelocityValid->setText(1, velocityValid ? "true" : "false");
         aTrackChildItems.at(trackId).mSpeed->SetValue(speed_mps);
         aTrackChildItems.at(trackId).mHeading->SetValue(heading_rad);
         aTrackChildItems.at(trackId).mSide->setText(1, QString::fromStdString(track.side()));
         aTrackChildItems.at(trackId).mType->setText(1, QString::fromStdString(track.type()));
         aTrackChildItems.at(trackId).mSpatialDomain->setText(1, domain);
         aTrackChildItems.at(trackId).mQuality->SetValue(track.trackQuality());
      }
   }
}

RvTracks::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mTempHighlightTrack(nullptr)
   , mTrackDisplayInterface(new wkf::TrackDisplayInterface(this))
{
   connect(&wkfEnv, &wkf::Environment::PlatformOfInterestChanged, this, &Plugin::PlatformOfInterestChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>(),
           &wkf::TrackVisibilityPrefObject::TrackVisibilityChanged,
           this,
           &Plugin::TrackVisibilityChangedCB);

   wkf::PlatformDataTreeItem* mtl = new wkf::PlatformDataTreeItem();
   mtl->setText(0, "Master Track List");
   wkf::PlatformDataTreeItem* stl = new wkf::PlatformDataTreeItem();
   stl->setText(0, "Sensor Track List");
   mTopLevelWidgets.push_back(mtl);
   mTopLevelWidgets.push_back(stl);
}

void RvTracks::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   DataUpdate(aData);
}

void RvTracks::Plugin::RegularRead(const rv::ResultData& aData)
{
   DataUpdate(aData);
}

QList<QTreeWidgetItem*> RvTracks::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   // Remove track highlighting relating to the previously selected platform
   RemoveTrackHighlight();

   return mTopLevelWidgets;
}

void RvTracks::Plugin::PlatformDataItemSelected(QTreeWidgetItem* aItem)
{
   bool local = aItem->data(0, LOCAL_ID_ROLE).toBool();
   if (local)
   {
      wkf::Platform* platformOfInterest = wkfEnv.GetPlatformOfInterest();
      if (platformOfInterest)
      {
         wkf::Scenario* scenario = vaEnv.GetStandardScenario();
         if (scenario)
         {
            int         trackId = aItem->data(0, TRACK_ID_ROLE).toInt();
            wkf::Track* track   = scenario->FindTrack(wkf::TrackId(platformOfInterest->GetName(), trackId));
            if (track)
            {
               mTempHighlightAttachments.clear();
               track->FindAttachments<vespa::VaAttachmentModel>(mTempHighlightAttachments);

               for (auto* attachment : mTempHighlightAttachments)
               {
                  UtColor color(.5, 1, .5, 0.7f);
                  attachment->Highlight(vaEnv.GetStandardViewer(), 0, true, color);
                  mTempHighlightTrack = track;
               }
            }
         }
      }
   }
}

void RvTracks::Plugin::UpdateTrackLabel(wkf::Track* aTrack, bool aVisible, bool aShowColor) const
{
   wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(aTrack->FindAttachment("label"));

   // if label attachment does not exist, create it
   if (!labelPtr)
   {
      try
      {
         labelPtr =
            vespa::VaEnvironment::CreateAttachment<wkf::AttachmentLabel>("label", *aTrack, vaEnv.GetStandardViewer(), true);
      }
      catch (const vespa::UnknownAttachmentTypeError& e)
      {
         ut::log::error() << e.what() << "in RvTracks::Plugin::UpdateTrackLabel()";
         return;
      }
      // Fortify still warns even if an exception is thrown.
      if (!labelPtr)
      {
         return;
      }

      auto* trackPref = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>();


      auto*  teamPref = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      auto   side     = aTrack->GetSide();
      QColor color;

      if (aShowColor && !side.empty())
      {
         color = teamPref->GetTeamColor(QString::fromStdString(side));
      }
      else
      {
         color = teamPref->GetTeamColor(wkf::TeamVisibilityPrefData::cDEFAULT_TEAM); // set the label color as default team
      }
      labelPtr->Setup(QString::fromStdString(aTrack->GetName()),
                      color,
                      trackPref->GetTrackFont()); // set the label text as the name of the tracking entity
   }

   labelPtr->SetStateVisibility(aVisible);
}

void RvTracks::Plugin::PlatformDataItemUnSelected()
{
   RemoveTrackHighlight();
}

std::unique_ptr<wkf::Updater> RvTracks::Plugin::GetUpdater(const QString&     aPlatformName,
                                                           const QString&     aDatum,
                                                           const unsigned int aContext) const
{
   // we aren't going to address track updaters, yet, since they are dynamic in number and presence.
   return nullptr;
}

void RvTracks::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Track>())
   {
      wkf::Track* track = dynamic_cast<wkf::Track*>(aEntityPtr);
      if (track)
      {
         QAction* traceAction = new QAction(QString("Trace Track"), aMenu);
         aMenu->addAction(traceAction);
         connect(traceAction,
                 &QAction::triggered,
                 [=]() {
                    TraceTrack(QString::fromStdString(track->GetTrackId().GetPlatformName()),
                               track->GetTrackId().GetLocalTrackId());
                 });
      }
   }
}

void RvTracks::Plugin::BuildItemContextMenu(QMenu* aMenu, QTreeWidgetItem* aItemPtr)
{
   bool        local  = aItemPtr->data(0, LOCAL_ID_ROLE).toBool();
   QString     subcat = aItemPtr->data(0, wkf::ID_ROLE).toString();
   QStringList list   = subcat.split(TRACK_SEPARATOR);
   if (list.size() == 2) // this is probably a track related item!
   {
      const QString& platformName = list.at(0);
      int            trackId      = list.at(1).toInt();

      if (local) // we don't trace on sensor tracks
      {
         QAction* traceAction = new QAction(QString("Trace Track"), aMenu);
         aMenu->addAction(traceAction);
         connect(traceAction, &QAction::triggered, [=]() { TraceTrack(platformName, trackId); });
      }

      if (dynamic_cast<wkf::UnitTreeWidgetItem*>(aItemPtr))
      {
         QString  text   = QString("Plot ") + platformName + ":" + QString::number(trackId);
         QAction* action = new QAction(QIcon::fromTheme("plot"), text, aMenu);
         action->setProperty("platform", platformName);
         action->setProperty("trackIdx", trackId);
         action->setProperty("dataType", aItemPtr->text(0));
         action->setProperty("local", local);
         connect(action, &QAction::triggered, this, &Plugin::PlotActionHandler);
         aMenu->addAction(action);
      }
   }
}

void RvTracks::Plugin::TrackVisibilityChangedCB()
{
   ClearTracks();
}

void RvTracks::Plugin::PlatformOfInterestChanged(wkf::Platform* aPlatform)
{
   auto* trackVisPtr = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>();
   auto  trackVis    = trackVisPtr->GetTrackVisibility();

   // create filtered list of tracks based on preferences.
   if (trackVis == wkf::tracks::Visibility::cLOCAL_ONLY)
   {
      ClearTracks();
   }
   mLocalTrackChildItems.clear();
   mSensorTrackChildItems.clear();
   while (mTopLevelWidgets[0]->childCount())
   {
      mTopLevelWidgets[0]->removeChild(mTopLevelWidgets[0]->child(0));
   }
   while (mTopLevelWidgets[1]->childCount())
   {
      mTopLevelWidgets[1]->removeChild(mTopLevelWidgets[1]->child(0));
   }
}

void RvTracks::Plugin::DataUpdate(const rv::ResultData& aData)
{
   double             simTime = aData.GetSimTime();
   const rv::TrackDb* trackDb = aData.GetDb()->LockTrackDb();

   // 1. update the platform data
   wkf::Platform* platformOfInterest = wkfEnv.GetPlatformOfInterest();
   if (platformOfInterest)
   {
      const rv::TrackDb::SourceTracks* tracks = trackDb->FindOwnerTracks(platformOfInterest->GetIndex());
      if (tracks)
      {
         if (!tracks->mLocalTracks.empty())
         {
            wkf::PlatformDataTreeItem* pitem = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(0));
            if (pitem != nullptr)
            {
               pitem->SetHidden(false);
            }
         }
         else
         {
            wkf::PlatformDataTreeItem* pitem = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(0));
            if (pitem != nullptr)
            {
               pitem->SetHidden(true);
            }
         }
         for (auto&& it : tracks->mLocalTracks)
         {
            this->PlotTrackChildItems(mLocalTrackChildItems, it.first, it.second, simTime, true);
         }

         if (!tracks->mSensorTracks.empty())
         {
            wkf::PlatformDataTreeItem* pitem = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(1));
            if (pitem != nullptr)
            {
               pitem->SetHidden(false);
            }
         }
         else
         {
            wkf::PlatformDataTreeItem* pitem = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(1));
            if (pitem != nullptr)
            {
               pitem->SetHidden(true);
            }
         }
         for (auto&& it : tracks->mSensorTracks)
         {
            this->PlotTrackChildItems(mSensorTrackChildItems, it.first, it.second, simTime, false);
         }
      }
      else
      {
         wkf::PlatformDataTreeItem* pitem0 = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(0));
         if (pitem0 != nullptr)
         {
            pitem0->SetHidden(true);
         }

         wkf::PlatformDataTreeItem* pitem1 = dynamic_cast<wkf::PlatformDataTreeItem*>(mTopLevelWidgets.at(1));
         if (pitem1 != nullptr)
         {
            pitem1->SetHidden(true);
         }
      }
   }

   auto* trackVisPtr = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>();
   auto  trackVis    = trackVisPtr->GetTrackVisibility();

   // create filtered list of tracks based on preferences.
   if (trackVis == wkf::tracks::Visibility::cLOCAL_ONLY)
   {
      wkf::PlatformList pl = wkfEnv.GetSelectedPlatforms();
      for (auto&& it : pl)
      {
         if (wkfEnv.IsPlatformVisible(it))
         {
            UpdateTracks(simTime, trackDb, it->GetIndex());
         }
      }
   }
   else if (trackVis == wkf::tracks::Visibility::cSELECTED_TEAM)
   {
      std::string selectedTeam = trackVisPtr->GetSelectedTeamForTracks().toStdString();

      const std::map<wkf::PlatformGrouping, std::set<int>>& catmap = wkfEnv.GetGroupings();

      wkf::PlatformGrouping sideGroup(wkf::PlatformGrouping::eSide, selectedTeam);

      std::map<wkf::PlatformGrouping, std::set<int>>::const_iterator it = catmap.find(sideGroup);
      if (it != catmap.end())
      {
         for (auto&& jt : it->second)
         {
            UpdateTracks(simTime, trackDb, jt);
         }
      }
   }
   else if (trackVis == wkf::tracks::Visibility::cALL_VISIBLE_TEAMS)
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         vespa::VaEntityList elist;
         scenarioPtr->GetEntityList(elist);
         for (auto&& entity : elist)
         {
            wkf::Platform* plat = dynamic_cast<wkf::Platform*>(entity);
            if (plat)
            {
               if (wkfEnv.IsPlatformVisible(plat))
               {
                  UpdateTracks(simTime, trackDb, plat->GetIndex());
               }
            }
         }
      }
   }
   // else, trackVis == wkf::TrackVisibilityPrefData::NONE, nothing to do

   aData.GetDb()->UnlockTrackDb();
}

void RvTracks::Plugin::GuiUpdate() {}

void RvTracks::Plugin::UpdateTracks(double aSimTime, const rv::TrackDb* aTrackDb, int aIndex)
{
   const rv::TrackDb::SourceTracks* tracks = aTrackDb->FindOwnerTracks(aIndex);
   if (tracks)
   {
      for (auto&& it : tracks->mLocalTracks)
      {
         wkf::TrackId trackid(it.first.owner(), it.first.localTrackNumber());

         if ((aSimTime <= it.second.mUpdates.GetMinTime()) || (aSimTime > it.second.mDropTime))
         {
            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               wkf::Track* track = scenarioPtr->FindTrack(trackid);
               if (track)
               {
                  track->SetVisibilityContribution(false, vespa::VaEntity::ScenarioHidden, vaEnv.GetStandardViewer());
               }
            }
         }
         else
         {
            rv::MsgLocalTrackUpdate* trackUpdate = it.second.mUpdates.FindFirstBefore(aSimTime);
            if (trackUpdate && IsMasterProcessor(*trackUpdate, true))
            {
               UpdateTrack(trackid, trackUpdate);
            }
         }
      }
   }
}

void RvTracks::Plugin::UpdateTrack(const wkf::TrackId& aTrackId, rv::MsgLocalTrackUpdate* aTrackMsg)
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario)
   {
      wkf::Track* track    = scenario->FindTrack(aTrackId);
      bool        newTrack = false;
      if (!track)
      {
         track    = new wkf::Track(aTrackId);
         newTrack = true;
      }
      else
      {
         track->SetVisibilityContribution(true, vespa::VaEntity::ScenarioHidden, vaEnv.GetStandardViewer());
      }

      UtEntity         entity;
      const rv::Track& trackInfo = aTrackMsg->track();
      const double     xyz[3] = {trackInfo.locationWCS().x(), trackInfo.locationWCS().y(), trackInfo.locationWCS().z()};
      const double vxyz[3]    = {trackInfo.velocityWCS().x(), trackInfo.velocityWCS().y(), trackInfo.velocityWCS().z()};
      entity.SetLocationWCS(xyz);
      entity.SetVelocityWCS(vxyz);

      const bool useHeading = trackInfo.velocityWCSValid() &&
                              wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>()->GetShowDirection();
      if (useHeading)
      {
         double vel[3];
         entity.GetVelocityNED(vel);
         const double heading_rad = atan2(vel[1], vel[0]);
         entity.SetOrientationNED(heading_rad, 0, 0);
      }
      else
      {
         entity.SetOrientationNED(0, 0, 0);
      }

      double psi, theta, phi;
      entity.GetOrientationWCS(psi, theta, phi);

      auto* target = scenario->FindPlatformByIndex(trackInfo.targetIndex());
      if (target != nullptr)
      {
         track->SetTargetPlatform(target->GetName());
      }
      else
      {
         track->SetTargetPlatform(std::string());
      }

      track->SetPositionValid(trackInfo.locationWCSValid());
      track->SetVelocityValid(trackInfo.velocityWCSValid());
      track->SetPositionOrientation(xyz, psi, theta, phi);
      track->SetVelocityWCS(vxyz);
      track->SetSide(trackInfo.side());
      track->SetTrackType(trackInfo.type());
      track->SetIcon(trackInfo.icon());
      track->SetSpatialDomain(static_cast<wkf::SpatialDomain>(trackInfo.spatialDomain()));

      const bool showLabel = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>()->GetShowLabel();
      const bool showColor = wkfEnv.GetPreferenceObject<wkf::TrackVisibilityPrefObject>()->GetShowColor();
      UpdateTrackLabel(track, showLabel, showColor);


      if (newTrack)
      {
         scenario->AddTrack(track);
         wkf::Observer::TrackAdded(track);
      }
   }
}

void RvTracks::Plugin::ClearTracks()
{
   // JEM TODO this needs to be called when the trackVisibility state changes or platform selection changes with the
   // current selection mode active
   if (vaEnv.GetStandardViewer() && vaEnv.GetStandardScenario())
   {
      vespa::VaEntityList elist;
      vaEnv.GetStandardScenario()->GetEntityList(elist);
      for (auto&& it : elist)
      {
         wkf::Track* track = dynamic_cast<wkf::Track*>(it);
         if (track)
         {
            track->SetVisibilityContribution(false, vespa::VaEntity::ScenarioHidden, vaEnv.GetStandardViewer());
         }
      }
   }
}

void RvTracks::Plugin::RemoveTrackHighlight()
{
   for (auto* attachment : mTempHighlightAttachments)
   {
      const UtColor color(.5, 1, .5, 0.7f);
      attachment->Highlight(vaEnv.GetStandardViewer(), 0, false, color);
      mTempHighlightTrack = nullptr;
   }
   mTempHighlightAttachments.clear();
}

void RvTracks::Plugin::TraceTrack(QString aPlatformName, int aTrackId)
{
   const double simTime = rvEnv.GetData()->GetSimTime();

   std::vector<std::pair<rv::MsgBase*, unsigned int>> eventList;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         QString message;
         // follow the trackid to its origins (local created, sensor created, correlation, decorrelation, engage tasks)
         rv::Track_Id tid;
         tid.owner(aPlatformName.toStdString());
         tid.localTrackNumber(aTrackId);
         db->TraceTrackId(tid, 0 /*wfPtr->GetMessageIndex()*/, eventList, simTime);

         QColor fc = qApp->palette().text().color();
         qreal  hsv[3];
         fc.getHsvF(&(hsv[0]), &(hsv[1]), &(hsv[2]));
         bool darkText = hsv[2] < 0.5;

         std::sort(eventList.begin(), eventList.end(), TraceSort);
         int w = 0;
         for (auto&& it : eventList)
         {
            QString line;
            QString time = QString::number(it.first->simTime(), 'f', 4) + ": ";
            QString pre;
            QString post = "</font><br>";
            // build a message
            switch (it.first->GetMessageId())
            {
            case (rv::MsgWeaponTerminated::cMESSAGE_ID):
            {
               rv::MsgWeaponTerminated* wt = static_cast<rv::MsgWeaponTerminated*>(it.first);
               if (darkText)
               {
                  pre = "<font color=\"#CC0000\">";
               }
               else
               {
                  pre = "<font color=\"#FFAAAA\">";
               }
               line = "Weapon Terminates: " +
                      QString(rv::MsgWeaponTerminated_GeometryResult::ToString(wt->geometryResult()));
               break;
            }
            case (rv::MsgWeaponFired::cMESSAGE_ID):
            {
               rv::MsgWeaponFired* wf = static_cast<rv::MsgWeaponFired*>(it.first);
               QString platformName   = QString::fromStdString(db->FindPlatform(wf->firingPlatformIndex())->GetName());
               if (darkText)
               {
                  pre = "<font color=\"#CC0000\">";
               }
               else
               {
                  pre = "<font color=\"#FFAAAA\">";
               }
               line = platformName + " fires on " + QString::fromStdString(wf->targetTrackId().owner()) + ":" +
                      QString::number(wf->targetTrackId().localTrackNumber());
               break;
            }
            case (rv::MsgTaskUpdate::cMESSAGE_ID):
            {
               const rv::MsgTaskUpdate* tu = static_cast<rv::MsgTaskUpdate*>(it.first);
               const QString assigner = QString::fromStdString(db->FindPlatform(tu->assignerPlatform())->GetName());
               const QString assignee = QString::fromStdString(db->FindPlatform(tu->assigneePlatform())->GetName());
               const QString taskType = QString::fromStdString(tu->taskType());
               const QString owner    = QString::fromStdString(tu->trackId().owner());

               if (darkText)
               {
                  pre = "<font color=\"#AA00CC\">";
               }
               else
               {
                  pre = "<font color=\"#DDAAFF\">";
               }

               if (tu->state() == rv::MsgTaskUpdate_State::assigned)
               {
                  line = assigner + " tasks " + assignee + " to " + taskType + " against " + owner + ":" +
                         QString::number(tu->trackId().localTrackNumber());
               }
               else if (tu->state() == rv::MsgTaskUpdate_State::canceled)
               {
                  line = assigner + " cancels task for " + assignee + " to " + taskType + " against " + owner + ":" +
                         QString::number(tu->trackId().localTrackNumber());
               }
               else
               {
                  line = assignee + " completes task from " + assigner + " to " + taskType + " against " + owner + ":" +
                         QString::number(tu->trackId().localTrackNumber());
               }
               break;
            }
            case (rv::MsgSensorTrackCreated::cMESSAGE_ID):
            {
               rv::MsgSensorTrackCreated* stc = static_cast<rv::MsgSensorTrackCreated*>(it.first);
               const QString platformName     = QString::fromStdString(db->FindPlatform(stc->ownerIndex())->GetName());
               if (darkText)
               {
                  pre = "<font color=\"#888800\">";
               }
               else
               {
                  pre = "<font color=\"#FFFFCC\">";
               }
               line = platformName + "'s " + QString::fromStdString(stc->sensorName()) + " has a new sensor track " +
                      QString::fromStdString(stc->trackId().owner()) + ":" +
                      QString::number(stc->trackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackCreated::cMESSAGE_ID):
            {
               rv::MsgLocalTrackCreated* ltc = static_cast<rv::MsgLocalTrackCreated*>(it.first);
               const QString platformName    = QString::fromStdString(db->FindPlatform(ltc->ownerIndex())->GetName());
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " has a new local track " + QString::fromStdString(ltc->trackId().owner()) + ":" +
                      QString::number(ltc->trackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackCorrelation::cMESSAGE_ID):
            {
               rv::MsgLocalTrackCorrelation* ltc = static_cast<rv::MsgLocalTrackCorrelation*>(it.first);
               const QString platformName = QString::fromStdString(db->FindPlatform(ltc->ownerIndex())->GetName());
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " correlates " + QString::fromStdString(ltc->trackId().owner()) + ":" +
                      QString::number(ltc->trackId().localTrackNumber()) + " with " +
                      QString::fromStdString(ltc->addTrackId().owner()) + ":" +
                      QString::number(ltc->addTrackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackDecorrelation::cMESSAGE_ID):
            {
               rv::MsgLocalTrackDecorrelation* ltd = static_cast<rv::MsgLocalTrackDecorrelation*>(it.first);
               const QString platformName = QString::fromStdString(db->FindPlatform(ltd->ownerIndex())->GetName());
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " decorrelates " + QString::fromStdString(ltd->trackId().owner()) + ":" +
                      QString::number(ltd->trackId().localTrackNumber()) + " from " +
                      QString::fromStdString(ltd->remTrackId().owner()) + ":" +
                      QString::number(ltd->remTrackId().localTrackNumber());
               break;
            }
            }
            int lw = qApp->fontMetrics().width(time + line);
            w      = (w > lw) ? w : lw;
            message += time + pre + line + post;
            // free the memory
            delete it.first;
         }

         // 3rd figure out how to present this
         QDialog dlg(wkfEnv.GetMainWindow());
         dlg.resize(w + 50, static_cast<int>(eventList.size() * qApp->fontMetrics().lineSpacing() + 50));
         dlg.setWindowTitle("Engagement Events");
         dlg.setLayout(new QHBoxLayout(&dlg));
         QTextBrowser* textEdit = new QTextBrowser(&dlg);
         textEdit->setAcceptRichText(true);
         dlg.layout()->addWidget(textEdit);
         textEdit->setReadOnly(true);
         textEdit->setText(message);
         dlg.exec();
      }
   }
}

RvTracks::Plugin::TrackItems::TrackItems(QTreeWidgetItem* aParentItem, int aTrackId, const QString& aPlatformName, bool aLocal)
{
   mParentItem = new wkf::PlatformDataTreeItem(aParentItem, {"Track " + QString::number(aTrackId)});

   mPositionValid = new wkf::PlatformDataTreeItem(mParentItem, {"Position Valid"});
   mLatitude      = new wkf::LatitudeTreeWidgetItem(mParentItem, "Latitude");
   mLongitude     = new wkf::LongitudeTreeWidgetItem(mParentItem, "Longitude");
   mAltitude      = new wkf::AltitudeTreeWidgetItem(mParentItem, "Altitude");
   mVelocityValid = new wkf::PlatformDataTreeItem(mParentItem, {"Velocity Valid"});
   mSpeed         = new wkf::SpeedTreeWidgetItem(mParentItem, "Speed");
   mHeading       = new wkf::AngleTreeWidgetItem(mParentItem, "Heading");
   mSide          = new wkf::PlatformDataTreeItem(mParentItem, {"Side"});
   mType          = new wkf::PlatformDataTreeItem(mParentItem, {"Type"});
   mSpatialDomain = new wkf::PlatformDataTreeItem(mParentItem, {"Spatial Domain"});
   mQuality       = new wkf::UnitlessTreeWidgetItem(mParentItem, "Quality");

   std::array<QTreeWidgetItem*, 12> items = {mParentItem,
                                             mPositionValid,
                                             mLatitude,
                                             mLongitude,
                                             mAltitude,
                                             mVelocityValid,
                                             mSpeed,
                                             mHeading,
                                             mSide,
                                             mType,
                                             mSpatialDomain,
                                             mQuality};

   mParentItem->SetUnhideable();
   mPositionValid->SetUnhideable();
   mLatitude->SetUnhideable();
   mLongitude->SetUnhideable();
   mAltitude->SetUnhideable();
   mVelocityValid->SetUnhideable();
   mSpeed->SetUnhideable();
   mHeading->SetUnhideable();
   mSide->SetUnhideable();
   mType->SetUnhideable();
   mSpatialDomain->SetUnhideable();
   mQuality->SetUnhideable();

   const QString id = aPlatformName + TRACK_SEPARATOR + QString::number(aTrackId);

   for (auto* item : items)
   {
      item->setData(0, TRACK_ID_ROLE, aTrackId);
      item->setData(0, LOCAL_ID_ROLE, aLocal);
      item->setData(0, wkf::ID_ROLE, id);
   }
}

bool RvTracks::Plugin::TraceSort(const std::pair<rv::MsgBase*, unsigned int>& aLeft,
                                 const std::pair<rv::MsgBase*, unsigned int>& aRight)
{
   if (aLeft.first->simTime() == aRight.first->simTime())
   {
      std::map<int, int> prec;
      constexpr int      mstc  = rv::MsgSensorTrackCreated::cMESSAGE_ID;
      constexpr int      mtu   = rv::MsgTaskUpdate::cMESSAGE_ID;
      constexpr int      mltc  = rv::MsgLocalTrackCreated::cMESSAGE_ID;
      constexpr int      mltco = rv::MsgLocalTrackCorrelation::cMESSAGE_ID;
      constexpr int      mltd  = rv::MsgLocalTrackDecorrelation::cMESSAGE_ID;
      constexpr int      mwf   = rv::MsgWeaponFired::cMESSAGE_ID;
      constexpr int      mwt   = rv::MsgWeaponTerminated::cMESSAGE_ID;
      prec[mstc]               = 0;
      prec[mtu]                = 1;
      prec[mltc]               = 2;
      prec[mltco]              = 3;
      prec[mltd]               = 4;
      prec[mwf]                = 5;
      prec[mwt]                = 6;

      return prec[aLeft.first->GetMessageId()] < prec[aRight.first->GetMessageId()];
   }
   else
   {
      return (aLeft.first->simTime() < aRight.first->simTime());
   }
}

void RvTracks::Plugin::PlotActionHandler()
{
   const QString platformName = sender()->property("platform").toString();
   const int     trackId      = sender()->property("trackIdx").toInt();
   const QString xAxis        = "Time";
   const QString yAxis        = sender()->property("dataType").toString();
   const bool    local        = sender()->property("local").toBool();

   PlottingWidget* plotWidget = new PlottingWidget(wkfEnv.GetMainWindow()->centralWidget());

   plotWidget->SetUpdater(ut::make_unique<PlotUpdater>(xAxis, yAxis, platformName, trackId, local));
   plotWidget->show();
}
