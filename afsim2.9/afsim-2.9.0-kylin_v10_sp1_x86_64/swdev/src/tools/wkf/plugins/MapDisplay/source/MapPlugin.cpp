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
#include "MapPlugin.hpp"

#include <QEvent>
#include <QMenu>
#include <QStatusBar>

#include "MapCursorStatusUpdater.hpp"
#include "MapCursorStatusWidget.hpp"
#include "MapDockWidget.hpp"
#include "MapPrefWidget.hpp"
#include "UtLog.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaFactory.hpp"
#include "VaLocationReference.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaWidget.hpp"
#include "WkfAction.hpp"
#include "WkfAttachmentLabel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
void AddBoxOverlay(vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr == vaEnv.GetStandardViewer())
   {
      aViewerPtr->AddOverlay("selection_box", "selection_box_overlay");
   }
}
} // namespace

WKF_PLUGIN_DEFINE_SYMBOLS(
   Map::Plugin,
   "Map Display",
   "The Map Display plugin provides a dockable 3D map view. This display can be enabled through the View menu.",
   "all")

Map::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
   , mDockWidgetPtr(new DockWidget(wkfEnv.GetMainWindow()->centralWidget()))
   , mMapPrefWidgetPtr(new PrefWidget)
   , mCursorStatusPtr(nullptr)
   , mCursorDockWidgetPtr(nullptr)
{
   const PrefObject* prefObj = mMapPrefWidgetPtr->GetPreferenceObject();
   connect(prefObj, &PrefObject::PreferencesChanged, this, &Plugin::PreferencesChanged);
   mDockWidgetPtr->SetPreferences(prefObj);

   wkf::Viewer* viewerPtr = mDockWidgetPtr->GetViewer();
   vaEnv.SetStandardViewer(viewerPtr);
   vespa::VaWidget* widgetPtr = dynamic_cast<vespa::VaWidget*>(mDockWidgetPtr->widget());
   vaEnv.SetStandardWidget(widgetPtr);

   if (viewerPtr->IsInitialized())
   {
      // Add the selection box overlay if the viewer is initialized.
      AddBoxOverlay(viewerPtr);
   }
   else
   {
      // If the viewer is not initialized yet, wait and add the selection box later
      mMapCallbacks.Add(vespa::VaObserver::ViewerInitialized.Connect(AddBoxOverlay));
   }

   mCursorDockWidgetPtr = new CursorStatusDockWidget(*viewerPtr, UniqueId(), wkfEnv.GetMainWindow());
   wkfEnv.GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, mCursorDockWidgetPtr);
   mCursorDockWidgetPtr->hide();

   mCursorStatusPtr = new CursorStatusWidget(*viewerPtr);
   wkfEnv.GetMainWindow()->statusBar()->insertPermanentWidget(0, mCursorStatusPtr);
   mDockWidgetPtr->installEventFilter(this);
   mCursorStatusPtr->SetShowAltitude(prefObj->GetPreferences().mShowTerrainAltitudeAtCursor);

   mTimer.start(500);
   connect(&mTimer, &QTimer::timeout, mCursorDockWidgetPtr.data(), &CursorStatusDockWidget::Update);
   connect(&mTimer, &QTimer::timeout, mCursorStatusPtr.data(), &CursorStatusWidget::Update);

   mToolBarPtr = new ToolBar();
   wkfEnv.GetMainWindow()->AddToolBar(mToolBarPtr);

   RegisterOption(nullptr, "Platform Labels");
}

QList<wkf::Action*> Map::Plugin::GetActions() const
{
   QList<wkf::Action*> list;
   list << mDockWidgetPtr->GetActions() << mToolBarPtr->GetCaptureAction();
   return list;
}

QList<wkf::PrefWidget*> Map::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mMapPrefWidgetPtr;
   return prefs;
}

void Map::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   QString  name         = aEntityPtr->GetName().c_str();
   QAction* centerAction = new QAction(QIcon::fromTheme("aim"), QString("Center on %1").arg(name), aMenu);
   aMenu->addAction(centerAction);
   QAction* followAction = new QAction(QIcon::fromTheme("track"), QString("Follow %1").arg(name), aMenu);
   aMenu->addAction(followAction);

   connect(centerAction, &QAction::triggered, this, [=]() { mDockWidgetPtr->CenterCamera(aEntityPtr); });
   connect(followAction, &QAction::triggered, this, [=]() { mDockWidgetPtr->Follow(aEntityPtr); });

   if (!mDockWidgetPtr->HasActiveRuler())
   {
      wkf::Action* measureAction =
         new wkf::Action(QIcon::fromTheme("distance"), QString("Measure from %1...").arg(name), aMenu);
      connect(measureAction, &QAction::triggered, this, [=]() { mDockWidgetPtr->MeasureFrom(aEntityPtr); });
   }
}

void Map::Plugin::ResetOptionStates()
{
   wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
   if (scenPtr)
   {
      const std::map<unsigned int, wkf::Platform*>& platMap = scenPtr->GetIndexPlatformMap();
      for (auto& it : platMap)
      {
         wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(it.second->FindAttachment("label"));
         if (labelPtr)
         {
            it.second->RemoveAttachment(labelPtr->GetUniqueId());
         }
      }
   }
}

void Map::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(aPlatformPtr->FindAttachment("label"));
      // if label attachment does not exist, create it
      if (!labelPtr)
      {
         if (aState)
         {
            try
            {
               labelPtr = vespa::VaEnvironment::CreateAttachment<wkf::AttachmentLabel>("label",
                                                                                       *aPlatformPtr,
                                                                                       mDockWidgetPtr->GetViewer(),
                                                                                       true);
            }
            catch (const vespa::UnknownAttachmentTypeError& e)
            {
               ut::log::error() << e.what() << "in Map::Plugin::SetPlatformOptionState()";
               return;
            }
            // Fortify still warns even if an exception is thrown.
            if (!labelPtr)
            {
               return;
            }
            labelPtr->Setup(QString::fromStdString(aPlatformPtr->GetName()),
                            wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamColor(
                               QString::fromStdString(aPlatformPtr->GetSide())),
                            mMapPrefWidgetPtr->GetPreferenceObject()->GetLabelFont());
         }
      }
      if (labelPtr)
      {
         labelPtr->SetStateVisibility(aState);
      }
   }
}

Qt::CheckState Map::Plugin::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const
{
   if (aPlatformPtr)
   {
      const wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(aPlatformPtr->FindAttachment("label"));

      if (labelPtr)
      {
         return (labelPtr->GetStateVisibility()) ? Qt::Checked : Qt::Unchecked;
      }
   }
   return Qt::Unchecked;
}

std::unique_ptr<wkf::Updater> Map::Plugin::GetUpdater(const QString&     aPlatformName,
                                                      const QString&     aDatum,
                                                      const unsigned int aContext) const
{
   if (aDatum == "Latitude" || aDatum == "Longitude")
   {
      return wkf::make_updater<PositionUpdater>(*mDockWidgetPtr->GetViewer());
   }
   else if (aDatum == "Elevation")
   {
      return wkf::make_updater<ElevationUpdater>(*mDockWidgetPtr->GetViewer(), mCursorDockWidgetPtr->GetDtedId());
   }
   else if (aDatum == "Bearing (Bullseye)")
   {
      return wkf::make_updater<BullseyeBearingUpdater>(*mDockWidgetPtr->GetViewer());
   }
   else if (aDatum == "Range (Bullseye)")
   {
      return wkf::make_updater<BullseyeRangeUpdater>(*mDockWidgetPtr->GetViewer());
   }
   else if (aDatum == "Nearest Bullseye")
   {
      return wkf::make_updater<BullseyeNearestUpdater>(*mDockWidgetPtr->GetViewer());
   }
   return nullptr;
}

void Map::Plugin::PreferencesChanged()
{
   PrefObject* prefObj = dynamic_cast<PrefObject*>(QObject::sender());
   mDockWidgetPtr->SetPreferences(prefObj);

   wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
   if (scenPtr)
   {
      const std::map<unsigned int, wkf::Platform*>& platMap = scenPtr->GetIndexPlatformMap();
      for (auto& it : platMap)
      {
         wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(it.second->FindAttachment("label"));
         if (labelPtr)
         {
            labelPtr->SetFont(prefObj->GetLabelFont());
         }
      }
   }
   mCursorStatusPtr->SetShowAltitude(prefObj->GetShowTerrainAltitudeAtCursor());
}

bool Map::Plugin::eventFilter(QObject* watched, QEvent* event)
{
   if (watched == mDockWidgetPtr)
   {
      if (event->type() == QEvent::Leave)
      {
         mCursorStatusPtr->clear();
      }
      return false; // Don't stop event from being handled further
   }
   else
   {
      // pass the event on to the parent class
      return wkf::Plugin::eventFilter(watched, event);
   }
}

void Map::Plugin::LoadSettings(QSettings& aSettings)
{
   mToolBarPtr->LoadSettings(aSettings);
}

void Map::Plugin::SaveSettings(QSettings& aSettings)
{
   mToolBarPtr->SaveSettings(aSettings);
}
