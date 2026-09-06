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
#include "MapDockWidget.hpp"

#include <sstream>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMimeData>
#include <QStatusBar>

#include "MapCameraMotion.hpp"
#include "MapRulerPropertiesWidget.hpp"
#include "UtQt.hpp"
#include "UtoShaders.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentQt.hpp"
#include "VaAttachmentWingRibbon.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaLocationReference.hpp"
#include "VaModelDatabase.hpp"
#include "VaOverlayQt.hpp"
#include "VaViewer.hpp"
#include "VaWidget.hpp"
#include "WkfAction.hpp"
#include "WkfAttachmentGreatCircleRuler.hpp"
#include "WkfAttachmentLabel.hpp"
#include "WkfAttachmentModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfMilStdIconPrefObject.hpp"
#include "WkfObserver.hpp"
#include "WkfOverlayUpdater.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfTrack.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "annotation/WkfAttachmentHeadingVector.hpp"
#include "route/WkfAttachmentRoute.hpp"

Map::DockWidget::DockWidget(QMainWindow* parent, Qt::WindowFlags flags)
   : wkf::DockWidget("WkMapDisplay", parent, flags, true)
   , mActiveRulerPtr(nullptr)
   , mRulerPropertiesPtr(new RulerPropertiesWidget(this))
   , mContextMenuActive(false)
   , mActiveMap("")
{
   setWindowTitle("Map Display");
   wkfEnv.GetMainWindow()->AddDockWidgetToViewMenu(this);

   wkf::VtkEnvironment::InitializeOSG_Path();

   mViewerPtr              = new wkf::Viewer(this, vespa::VaViewer::HighlightViewer, 0);
   vespa::VaWidget* widget = new vespa::VaWidget(mViewerPtr, this, true);

   widget->BlockRightClick(true);
   mViewerPtr->SetChooser(widget->GetChooser());
   mCameraMotionPtr = new CameraMotion(mViewerPtr->GetCamera());
   mViewerPtr->GetCamera()->SetCustomCameraMotion(mCameraMotionPtr);
   setWidget(widget);

   mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&DockWidget::PlatformsAddedHandler, this));
   mCallbacks.Add(wkf::Observer::PlatformPropertiesChanged.Connect(&DockWidget::PlatformPropertiesChangedHandler, this));
   mCallbacks.Add(vespa::VaObserver::ButtonClick.Connect(&DockWidget::MouseButtonClickCB, this, mViewerPtr->GetUniqueId()));
   mCallbacks.Add(vespa::VaObserver::EndSelectionChange.Connect(&DockWidget::EndSelectionCB, this));
   mCallbacks.Add(vespa::VaObserver::AttachmentSelected.Connect(&DockWidget::AttachmentSelectedCB, this));
   mCallbacks.Add(vespa::VaObserver::AttachmentDeleted.Connect(&DockWidget::AttachmentDeletedCB, this));

   mCenterOnSelectionPtr = new wkf::Action(QString("Center on Selection"), this, QKeySequence(Qt::ALT + Qt::Key_Home));
   mCenterOnScenarioPtr  = new wkf::Action(QString("Center on Scenario"), this, QKeySequence(Qt::Key_Home));
   mCopyCursorToClipboardPtr = new wkf::Action(QIcon::fromTheme("location_clipboard"),
                                               QString("Copy Location to Clipboard"),
                                               this,
                                               QKeySequence(QKeySequence::Copy));
   mMeasureFromLocationPtr   = new wkf::Action(QIcon::fromTheme("distance"),
                                             QString("Measure from Location..."),
                                             this,
                                             QKeySequence(Qt::CTRL + Qt::Key_M));
   mMeasureFromPlatformPtr   = new wkf::Action(QIcon::fromTheme("distance"),
                                             QString("Measure from Platform..."),
                                             this,
                                             QKeySequence(Qt::ALT + Qt::Key_M));
   connect(mCenterOnSelectionPtr, &QAction::triggered, [this]() { mViewerPtr->CenterCamera(true); });
   connect(mCenterOnScenarioPtr, &QAction::triggered, [this]() { mViewerPtr->CenterCamera(false); });
   connect(mCopyCursorToClipboardPtr, &QAction::triggered, this, &DockWidget::CopyCursorLocationToClipboard);
   connect(mMeasureFromLocationPtr, &QAction::triggered, [this]() { MeasureFrom(vaEnv.GetCurrentCursorPosition()); });
   connect(mMeasureFromPlatformPtr,
           &QAction::triggered,
           [this]()
           {
              auto* platformPtr = wkfEnv.GetPlatformOfInterest();
              if (platformPtr)
              {
                 MeasureFrom(platformPtr);
              }
           });

   connect(&wkfEnv, &wkf::Environment::PlatformSelectionChanged, this, &DockWidget::PlatformSelectionChanged);
   connect(this, &QDockWidget::visibilityChanged, this, &DockWidget::VisibilityChanged);

   connect(&wkfEnv, &wkf::Environment::PlatformVisibilityChanged, this, &DockWidget::PlatformVisibilityChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           this,
           &DockWidget::TeamColorChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>(),
           &wkf::MilStdIconPrefObject::UseSymbologyChanged,
           this,
           &DockWidget::MilStdChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>(),
           &wkf::MilStdIconPrefObject::TeamMappingsChanged,
           this,
           &DockWidget::MilStdChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>(),
           &wkf::MilStdIconPrefObject::ShowVelocityVectorChanged,
           this,
           &DockWidget::ShowVelocityVectorChanged);

   if (!mActiveMap.empty())
   {
      mActiveMap =
         wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMap(mActiveMap, mViewerPtr, mActiveMap);
   }

   QMenu* devMenu = wkfEnv.GetMainWindow()->GetMenuByName("Developer");
   if (devMenu)
   {
      QAction* action = new QAction(QIcon::fromTheme("graph"), "Show Map Window Scene Graph", devMenu);
      connect(action, &QAction::triggered, this, &DockWidget::SceneGraphAction);
      devMenu->addAction(action);

      QAction* shaderReload = new QAction("Reload Shaders", devMenu);
      connect(shaderReload, &QAction::triggered, this, &DockWidget::ReloadShaders);
      devMenu->addAction(shaderReload);

      mToggleStatsPtr = new wkf::Action("Toggle Graphics Stats", devMenu);
      connect(mToggleStatsPtr, &QAction::triggered, this, &DockWidget::ToggleStatsAction);
      devMenu->addAction(mToggleStatsPtr);

      mTogglePolygonModePtr = new wkf::Action("Toggle Polygon Mode", devMenu);
      connect(mTogglePolygonModePtr, &QAction::triggered, this, &DockWidget::ToggleModeAction);
      devMenu->addAction(mTogglePolygonModePtr);
   }
   mCopyCursorToClipboardPtr->setEnabled(false);
   setAcceptDrops(true);
}

void Map::DockWidget::SetPreferences(const PrefObject* aPrefObjectPtr)
{
   // we don't set mActiveMap right away, because SetMap compares to avoid redundant "changes"
   std::string newMap = aPrefObjectPtr->GetPreferences().mActiveMap;
   mActiveMap = wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMap(newMap, mViewerPtr, mActiveMap);
   mCameraMotionPtr->SetZoomOnCursor(aPrefObjectPtr->GetZoomOnCursor());
}

QList<wkf::Action*> Map::DockWidget::GetActions() const
{
   QList<wkf::Action*> list;
   list << mCenterOnSelectionPtr << mCenterOnScenarioPtr << mCopyCursorToClipboardPtr << mMeasureFromLocationPtr
        << mMeasureFromPlatformPtr << mTogglePolygonModePtr << mToggleStatsPtr;
   return list;
}

bool Map::DockWidget::BuildContextMenu(QMenu& aMenu, const QPoint& pos)
{
   mContextMenuActive = true;

   EndRuler(true); // Cancel any active ruler

   int                 highdpiFactor = QApplication::desktop()->devicePixelRatio();
   vespa::VaOverlayQt* overlayPtr =
      mViewerPtr->PickOverlayFromScreen<vespa::VaOverlayQt>(pos.x() * highdpiFactor,
                                                            mViewerPtr->GetHeight() - pos.y() * highdpiFactor);
   if (overlayPtr &&
       overlayPtr->BuildContextMenu(aMenu,
                                    QPoint(pos.x() * highdpiFactor, mViewerPtr->GetHeight() - pos.y() * highdpiFactor)))
   {
      return false;
   }

   vespa::VaHitList hitList;
   mViewerPtr->PickFromScreen(hitList, pos.x() * highdpiFactor, mViewerPtr->GetHeight() - pos.y() * highdpiFactor);

   // Only add Unique (Somehow multiple hits on same platform can occur) instances of wkf::Platform types to the filtered list.
   std::set<unsigned int>                         entityTracker;
   std::list<wkf::Platform*>                      filteredPlatformList;
   std::list<wkf::Track*>                         trackList;
   std::list<wkf::Entity*>                        otherList;
   std::map<unsigned int, vespa::VaAttachmentQt*> attachmentSubPartMap;
   for (auto& hit : hitList)
   {
      vespa::VaAttachment* att = hit.FindAttachment();
      if ((att == nullptr) || (att->GetPickMode() == vespa::VaAttachment::cPICK_ENTITY)) // you could else of this to
                                                                                         // handle other attachments
      {
         vespa::VaEntity* ent = hit.FindEntity();
         if (ent)
         {
            wkf::Platform* platform = dynamic_cast<wkf::Platform*>(ent);
            if (platform)
            {
               if (entityTracker.find(platform->GetUniqueId()) == entityTracker.end())
               {
                  entityTracker.insert(platform->GetUniqueId());
                  filteredPlatformList.push_back(platform);
               }
            }
            else
            {
               wkf::Track* track = dynamic_cast<wkf::Track*>(ent);
               if (track)
               {
                  trackList.push_back(track);
               }
               else if (ent->IsA_TypeOf<wkf::Entity>())
               {
                  wkf::Entity* went = dynamic_cast<wkf::Entity*>(ent);
                  otherList.push_back(went);
               }
            }
         }
      }
      // create a menu when a route waypoint is clicked on
      if ((att != nullptr) && (att->GetPickMode() == vespa::VaAttachment::cPICK_SUBPARTS))
      {
         vespa::VaAttachmentQt* qtAtt = dynamic_cast<vespa::VaAttachmentQt*>(att);
         if (qtAtt != nullptr)
         {
            attachmentSubPartMap[hit.GetSubId().mSubId] = qtAtt;
         }
      }
      else if ((att != nullptr) && att->GetPickMode() == vespa::VaAttachment::cPICK_ATTACHMENT)
      {
         vespa::VaAttachmentQt* qtAtt = dynamic_cast<vespa::VaAttachmentQt*>(att);
         if (qtAtt != nullptr)
         {
            attachmentSubPartMap[0] = qtAtt; // subID of '0' indicates the entire attachment
         }
      }
   }

   // at this point filteredPlatformList contains only valid pointer to WkfPlatforms with no duplicates
   if (!filteredPlatformList.empty())
   {
      if (filteredPlatformList.size() == 1)
      {
         wkfEnv.BuildEntityContextMenu(&aMenu, filteredPlatformList.front());
      }
      else
      {
         for (auto& platform : filteredPlatformList)
         {
            QMenu* subMenu = aMenu.addMenu(QString::fromStdString(platform->GetName()));
            wkfEnv.BuildEntityContextMenu(subMenu, platform);
            UtQtTranslucentMenu(*subMenu);
            connect(subMenu->menuAction(), &QAction::hovered, this, &DockWidget::MenuHovered);
         }
      }
      if (!aMenu.actions().empty())
      {
         aMenu.addSeparator();
      }
   }
   else if (!trackList.empty())
   {
      if (trackList.size() == 1)
      {
         wkfEnv.BuildEntityContextMenu(&aMenu, trackList.front());
      }
      else
      {
         for (auto& track : trackList)
         {
            QMenu* subMenu = aMenu.addMenu(QString::fromStdString(track->GetName()));
            wkfEnv.BuildEntityContextMenu(subMenu, track);
            UtQtTranslucentMenu(*subMenu);
            connect(subMenu->menuAction(), &QAction::hovered, this, &DockWidget::MenuHovered);
         }
      }
      aMenu.addSeparator();
   }
   else if (!otherList.empty())
   {
      wkfEnv.BuildEntityContextMenu(&aMenu, otherList.front());
   }

   if (!attachmentSubPartMap.empty())
   {
      if (attachmentSubPartMap.size() == 1)
      {
         std::map<unsigned int, vespa::VaAttachmentQt*>::iterator mapIter = attachmentSubPartMap.begin();
         mapIter->second->BuildContextMenu(aMenu, mapIter->first);
      }
      else
      {
         for (const auto& attachSubPart : attachmentSubPartMap)
         {
            QMenu* subMenu = aMenu.addMenu(attachSubPart.second->GetName().c_str());
            attachSubPart.second->BuildContextMenu(*subMenu, attachSubPart.first);
            UtQtTranslucentMenu(*subMenu);
            subMenu->setTitle(attachSubPart.second->GetName().c_str());
            connect(subMenu->menuAction(), &QAction::hovered, this, &DockWidget::MenuHovered);
            aMenu.addMenu(subMenu);
         }
      }
      aMenu.addSeparator();
   }
   aMenu.addAction(mCenterOnSelectionPtr);
   aMenu.addAction(mCenterOnScenarioPtr);
   aMenu.addAction(mCopyCursorToClipboardPtr);
   aMenu.addAction(mMeasureFromLocationPtr);

   wkfEnv.BuildViewerContextMenu(&aMenu, mViewerPtr);

   connect(&aMenu, &QMenu::aboutToHide, this, &DockWidget::ContextMenuExeced);

   return true;
}

void Map::DockWidget::ContextMenuExeced()
{
   mContextMenuActive = false;

   for (auto* attachment : mTempHighlightAttachments)
   {
      UtColor color(.5, 1, .5, 0.7f);
      attachment->Highlight(vaEnv.GetStandardViewer(), 0, false, color);
   }
   mTempHighlightAttachments.clear();
}

bool Map::DockWidget::event(QEvent* event)
{
   if (event->type() == QEvent::ShortcutOverride)
   {
      QKeyEvent* ke = static_cast<QKeyEvent*>(event);
      if (ke->key() == Qt::Key_Escape)
      {
         EndRuler(true);
      }
      else if (ke->key() == Qt::Key_Delete)
      {
         vespa::VaHitSet selection = mViewerPtr->GetSelection();
         for (const auto& hit : selection)
         {
            if (hit.IsAttachment())
            {
               wkf::AttachmentGreatCircleRuler* rulerPtr =
                  dynamic_cast<wkf::AttachmentGreatCircleRuler*>(hit.FindAttachment());
               vespa::VaAttachment* a = hit.FindAttachment();
               if (rulerPtr)
               {
                  vaEnv.DeleteEntity(&rulerPtr->GetParent());
               }
               else if ((nullptr != a) && (a->DeleteSelection(mViewerPtr) == vespa::VaAttachment::cDELETE_ATTACHMENT))
               {
                  a->GetParent().RemoveAttachment(a->GetUniqueId(), hit.SubId());
               }
            }
         }
      }
   }
   else if ((event->type() == QEvent::Enter && hasFocus()) || event->type() == QEvent::FocusIn)
   {
      mCopyCursorToClipboardPtr->setEnabled(true);
   }
   else if ((event->type() == QEvent::Leave || event->type() == QEvent::FocusOut) && !mContextMenuActive)
   {
      mCopyCursorToClipboardPtr->setEnabled(false);
   }
   return wkf::DockWidget::event(event);
}

void Map::DockWidget::VisibilityChanged(bool aVisible)
{
   if (aVisible)
   {
      setFocus();
   }
}

void Map::DockWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->accept();
   }
   else if (aEvent->mimeData()->hasUrls())
   {
      auto urls = aEvent->mimeData()->urls();
      if (!urls.empty())
      {
         auto checkUrls = [](const QList<QUrl>& aList, const QString& aSuffix)
         {
            for (const auto& it : aList)
            {
               QString str = it.toLocalFile();
               if (!str.endsWith(aSuffix, Qt::CaseInsensitive))
               {
                  return false;
               }
            }
            return true;
         };

         if (wkfEnv.GetApplicationName() == "mystic")
         {
            aEvent->setAccepted(urls.size() == 1 && checkUrls(urls, ".aer"));
         }
         else if (wkfEnv.GetApplicationName() == "warlock")
         {
            aEvent->setAccepted(urls.size() == 1 && checkUrls(urls, ".txt"));
         }
      }
   }
}

void Map::DockWidget::dropEvent(QDropEvent* aEvent)
{
   int                 highdpiFactor = QApplication::desktop()->devicePixelRatio();
   vespa::VaOverlayQt* overlayPtr =
      mViewerPtr->PickOverlayFromScreen<vespa::VaOverlayQt>(aEvent->pos().x() * highdpiFactor,
                                                            mViewerPtr->GetHeight() - aEvent->pos().y() * highdpiFactor);
   if (overlayPtr && overlayPtr->SupportsDropEvent())
   {
      if (overlayPtr->DropEvent(aEvent))
      {
         return;
      }
   }
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->acceptProposedAction();
      QByteArray   ba = aEvent->mimeData()->data(wkf::Updater::cMIME_IDENTIFIER);
      QDataStream  in(&ba, QIODevice::ReadOnly);
      unsigned int qi;
      size_t       pluginId;
      QString      platformName;
      QString      datum;
      unsigned int context;
      in >> qi;
      in >> platformName;
      in >> datum;
      in >> context;
      pluginId = static_cast<size_t>(qi);

      auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginId);
      if (plugin)
      {
         auto updaterPtr = plugin->GetUpdater(platformName, datum, context);
         if (updaterPtr)
         {
            QPoint p = aEvent->pos();

            int highdpiFactor = QApplication::desktop()->devicePixelRatio();
            new wkf::OverlayUpdater(mViewerPtr,
                                    updaterPtr,
                                    platformName,
                                    p.x() * highdpiFactor,
                                    highdpiFactor * (height() - p.y()));
            // viewer takes ownership
         }
      }
   }
   else if (aEvent->mimeData()->hasUrls())
   {
      auto        urls = aEvent->mimeData()->urls();
      QStringList list;
      for (const auto& it : urls)
      {
         list.push_back(it.toLocalFile());
      }
      vaEnv.GetFactory()->UserOpenFile(list);
   }
}

void Map::DockWidget::PlatformsAddedHandler(const wkf::PlatformList& aPlatforms)
{
   for (auto& platform : aPlatforms)
   {
      if (platform->GetScenario() == vaEnv.GetStandardScenario())
      {
         platform->SetVisibilityContribution(wkfEnv.IsPlatformVisible(platform),
                                             vespa::VaEntity::UserOptionHidden,
                                             mViewerPtr);

         vespa::VaAttachmentModel* modelPtr = GetModel(platform);
         if (!modelPtr)
         {
            modelPtr = vespa::make_attachment<wkf::AttachmentModel>(*platform, mViewerPtr);
            modelPtr->Initialize();
            modelPtr->SetName("mapDisplayModel");
            modelPtr->SetProxy(true);
         }
         // Note, this isn't an else!  We should always have a model here.
         if (modelPtr)
         {
            auto milStdPrefs = wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>();
            if (milStdPrefs->GetUseSymbology())
            {
               WkfMilStd2525d::Side         side = WkfMilStd2525d::GetSideFromTeam(platform->GetSide());
               WkfMilStd2525d::PartialInput input(side, platform->GetSpatialDomain(), platform->GetIcon());
               modelPtr->SetIcon(wkfEnv.GetMilStdIconLoader().GetModelName(input));
               if (milStdPrefs->GetShowVelocityVector())
               {
                  ShowVelocityVector(*platform, true);
               }
            }
            else
            {
               modelPtr->SetColor(
                  wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(platform->GetSide()));
               modelPtr->SetIcon(platform->GetIcon());
            }
         }
      }
   }
}

void Map::DockWidget::PlatformPropertiesChangedHandler(wkf::Platform* aPlatformPtr)
{
   wkf::PlatformList temp{aPlatformPtr};
   PlatformsAddedHandler(temp);
}

void Map::DockWidget::PlatformSelectionChanged(const wkf::PlatformList& aSelected, const wkf::PlatformList& aUnselected)
{
   if (mEntitySelectionMutex.tryLock())
   {
      for (auto& platform : aSelected)
      {
         platform->SetSelected(mViewerPtr, true);
      }
      for (auto& platform : aUnselected)
      {
         platform->SetSelected(mViewerPtr, false);
      }
      mEntitySelectionMutex.unlock();
   }
}

void Map::DockWidget::PlatformVisibilityChanged()
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
            entityPtr->SetVisibilityContribution(wkfEnv.IsPlatformVisible(entityPtr),
                                                 vespa::VaEntity::UserOptionHidden,
                                                 mViewerPtr);
         }
      }
   }
}

void Map::DockWidget::TeamColorChanged()
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
            vespa::VaAttachmentModel* model = GetModel(entityPtr);
            if (model != nullptr)
            {
               model->SetColor(
                  wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(entityPtr->GetSide()));
            }
            wkf::AttachmentLabel* labelPtr = dynamic_cast<wkf::AttachmentLabel*>(entityPtr->FindAttachment("label"));
            if (labelPtr)
            {
               labelPtr->SetColor(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamColor(
                  QString::fromStdString(entityPtr->GetSide())));
            }
         }
      }
   }
}

void Map::DockWidget::MilStdChanged()
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntityList list;
      scenario->GetEntityList(list);
      auto milStdPrefs = wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>();
      for (auto& i : list)
      {
         wkf::Platform* entityPtr = dynamic_cast<wkf::Platform*>(i);
         if (entityPtr != nullptr)
         {
            auto model = GetModel(entityPtr);
            if (milStdPrefs->GetUseSymbology())
            {
               WkfMilStd2525d::Side         side = WkfMilStd2525d::GetSideFromTeam(entityPtr->GetSide());
               WkfMilStd2525d::PartialInput input(side, entityPtr->GetSpatialDomain(), entityPtr->GetIcon());
               model->SetIcon(wkfEnv.GetMilStdIconLoader().GetModelName(input));
               if (milStdPrefs->GetShowVelocityVector())
               {
                  ShowVelocityVector(*entityPtr, true);
               }
            }
            else
            {
               model->SetIcon(entityPtr->GetIcon());
               model->SetColor(
                  wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(entityPtr->GetSide()));
               ShowVelocityVector(*entityPtr, false);
            }
         }
      }
   }
}

void Map::DockWidget::ShowVelocityVectorChanged()
{
   vespa::VaScenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      vespa::VaEntityList list;
      auto                milStdPrefs = wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>();
      scenario->GetEntityList(list);
      bool showVector = milStdPrefs->GetUseSymbology() && milStdPrefs->GetShowVelocityVector();
      for (auto& i : list)
      {
         wkf::Platform* entityPtr = dynamic_cast<wkf::Platform*>(i);
         if (entityPtr != nullptr)
         {
            ShowVelocityVector(*entityPtr, showVector);
         }
      }
   }
}

void Map::DockWidget::ShowVelocityVector(wkf::Platform& aPlatform, bool aInitialState)
{
   wkf::AttachmentHeadingVector* headingVector = aPlatform.FindFirstAttachmentOfType<wkf::AttachmentHeadingVector>();
   if (!headingVector)
   {
      headingVector = vespa::make_attachment<wkf::AttachmentHeadingVector>(aPlatform, vaEnv.GetStandardViewer());
   }

   auto    milStdPrefs = wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>();
   QString side        = QString::fromStdString(aPlatform.GetSide());
   UtColor vectorColor;
   if (milStdPrefs->GetFriendTeams().count(side) > 0)
   {
      vectorColor.Set(0.0f, 1.0f, 1.0f, 1.0f);
   }
   else if (milStdPrefs->GetNeutralTeams().count(side) > 0)
   {
      vectorColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
   }
   else if (milStdPrefs->GetHostileTeams().count(side) > 0)
   {
      vectorColor.Set(1.0f, 0.0f, 0.0f, 0.0f);
   }
   else
   {
      vectorColor.Set(1.0f, 1.0f, 0.0f, 1.0f);
   }
   headingVector->ShowVector(aInitialState, vectorColor);
}

void Map::DockWidget::MeasureFrom(const vespa::VaLocationReference& aLocRef)
{
   if (!mActiveRulerPtr)
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         vespa::VaEntity* anchorPtr = new vespa::VaEntity("ruler_anchor");
         scenarioPtr->AddEntity(anchorPtr);
         mActiveRulerPtr =
            vaEnv.CreateAttachment<wkf::AttachmentGreatCircleRuler>("great_circle_ruler", *anchorPtr, mViewerPtr);
         mActiveRulerPtr->SetMode(mRulerPropertiesPtr->GetRulerMode());
         mActiveRulerPtr->SetLabelMode(mRulerPropertiesPtr->GetLabelMode());
         if (aLocRef.GetEntity())
         {
            mActiveRulerPtr->SetPosition(*aLocRef.GetEntity());
         }
         else
         {
            wkf::PointOfInterest* startPoint = AddPointOfInterest(aLocRef);
            mActiveRulerPtr->SetPosition(*startPoint);
            mActiveRulerPtr->Take(startPoint);
            startPoint->SetManagingAttachment(mActiveRulerPtr);
         }

         mActiveRulerPtr->SetActive(true);
         mActiveRulerPtr->SetProxy(true);
         mActiveRulerPtr->SetSelected(mViewerPtr, true, 0);

         mRulerPropertiesPtr->SetRuler(mActiveRulerPtr);
         mRulerPropertiesPtr->show();
         widget()->setFocus(); // Give keyboard focus to dock widget so 'Esc' key can be used to cancel
         widget()->setCursor(Qt::PointingHandCursor);
         wkfEnv.GetMainWindow()->ShowStatusMessage(
            "Click on destination point to anchor ruler. Press 'Esc' or right click to cancel...");
      }
   }
   else
   {
      // Cancel the existing ruler
      EndRuler(true);
   }
}

void Map::DockWidget::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](GetViewer());
}

void Map::DockWidget::ReloadShaders()
{
   UtoShaders::ReloadShaders();
}

void Map::DockWidget::ToggleModeAction()
{
   GetViewer()->ToggleMode();
}

void Map::DockWidget::ToggleStatsAction()
{
   GetViewer()->ToggleStats();
}

vespa::VaAttachmentModel* Map::DockWidget::GetModel(vespa::VaEntity* aEntityPtr)
{
   auto attachmentVector = aEntityPtr->FindAttachmentsOfType<vespa::VaAttachmentModel>();
   for (auto modelPtr : attachmentVector)
   {
      if ((modelPtr->GetName() == "mapDisplayModel") && modelPtr->IsMyViewer(mViewerPtr))
      {
         return modelPtr;
      }
   }
   return nullptr;
}

void Map::DockWidget::MenuHovered()
{
   UtColor color(.5, 1, .5, 0.7f);
   for (auto* attachment : mTempHighlightAttachments)
   {
      attachment->Highlight(vaEnv.GetStandardViewer(), 0, false, color);
   }
   mTempHighlightAttachments.clear();

   QAction* action = dynamic_cast<QAction*>(QObject::sender());
   if (action)
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         vespa::VaEntity* entity = scenarioPtr->FindEntity(action->text().toStdString());
         if (entity)
         {
            entity->FindAttachments<vespa::VaAttachmentModel>(mTempHighlightAttachments);
            entity->FindAttachments<vespa::VaAttachmentWingRibbon>(mTempHighlightAttachments);
            for (auto* attachment : mTempHighlightAttachments)
            {
               attachment->Highlight(vaEnv.GetStandardViewer(), 0, true, color);
            }
         }
      }
   }
}

void Map::DockWidget::CopyCursorLocationToClipboard()
{
   std::ostringstream oss;
   vespa::VaPosition  curPos(vaEnv.GetCurrentCursorPosition());
   curPos.SetLatFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
   curPos.SetLonFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
   oss << curPos.GetLat() << ' ' << curPos.GetLon();

   QString text(oss.str().c_str());
   qApp->clipboard()->setText(text);
   wkfEnv.GetMainWindow()->ShowStatusMessage(QString("Cursor location %1 copied to clipboard").arg(text), 2000);
}

void Map::DockWidget::EndRuler(bool aCancel)
{
   if (mActiveRulerPtr)
   {
      if (aCancel)
      {
         vaEnv.DeleteEntity(&mActiveRulerPtr->GetParent());
         mRulerPropertiesPtr->hide();
      }

      mActiveRulerPtr = nullptr;
      widget()->unsetCursor();
      wkfEnv.GetMainWindow()->statusBar()->clearMessage();
   }
}

bool Map::DockWidget::MouseButtonClickCB(vespa::VaViewer* aViewerPtr, int aButton, int aMouseX, int aMouseY, int aState)
{
   setFocus();
   if (mActiveRulerPtr && (aButton == 1))
   {
      vespa::VaHitList hits;
      mViewerPtr->PickFromScreen(hits, aMouseX, aMouseY);
      // Filter out the hit on the ruler itself
      hits.remove(vespa::VaHitEntry::FromAttachment(*mActiveRulerPtr));

      vespa::VaHitEntry picked;
      if (mViewerPtr->ChooseHit(hits, aMouseX, aMouseY, picked))
      {
         if (picked.IsEntity())
         {
            vespa::VaEntity* entityPtr = picked.FindEntity();
            if (entityPtr && (entityPtr != mActiveRulerPtr->GetSrcLocationRef().GetEntity()))
            {
               mActiveRulerPtr->SetPosition(*entityPtr);
               EndRuler(false);
            }
         }
      }
      else if (hits.empty())
      {
         wkf::PointOfInterest* endPoint = AddPointOfInterest(vaEnv.GetSavedCursorPosition());
         mActiveRulerPtr->SetPosition(*endPoint);
         endPoint->SetManagingAttachment(mActiveRulerPtr);
         mActiveRulerPtr->Take(endPoint);
         EndRuler(false);
      }

      return true;
   }
   return false;
}

void Map::DockWidget::EndSelectionCB(vespa::VaViewer* aViewerPtr)
{
   if (mViewerPtr->GetUniqueId() == aViewerPtr->GetUniqueId())
   {
      wkf::PlatformList selectedPlatformList;

      auto hitSet = mViewerPtr->GetSelection();
      for (auto&& hit : hitSet)
      {
         wkf::Platform* platform = dynamic_cast<wkf::Platform*>(hit.FindEntity());
         if (platform)
         {
            selectedPlatformList.push_back(platform);
         }
      }

      // Set all the platforms in selectedPlatformList to be selected.
      if (mEntitySelectionMutex.tryLock())
      {
         wkfEnv.SetPlatformsSelected(selectedPlatformList);
         mEntitySelectionMutex.unlock();
      }
   }
}

void Map::DockWidget::AttachmentSelectedCB(vespa::VaViewer*                aViewerPtr,
                                           int                             aSelect,
                                           vespa::VaAttachment*            aAttachmentPtr,
                                           const vespa::VaAttachmentSubId& aSubPartId)
{
   if (mViewerPtr->GetUniqueId() == aViewerPtr->GetUniqueId())
   {
      wkf::AttachmentGreatCircleRuler* rulerPtr = dynamic_cast<wkf::AttachmentGreatCircleRuler*>(aAttachmentPtr);
      if (rulerPtr)
      {
         if (aSelect == 0)
         {
            if (rulerPtr == mActiveRulerPtr)
            {
               EndRuler(false);
            }
            if (rulerPtr == mRulerPropertiesPtr->GetRuler())
            {
               mRulerPropertiesPtr->SetRuler(nullptr);
               mRulerPropertiesPtr->hide();
            }
         }
         else if (aSelect == 1)
         {
            mRulerPropertiesPtr->SetRuler(rulerPtr);
            mRulerPropertiesPtr->show();
         }
      }
   }
}

void Map::DockWidget::AttachmentDeletedCB(vespa::VaAttachment* aAttachmentPtr, const vespa::VaAttachmentSubId& aSubPartId)
{
   // If the attachment that was deleted exists in the mTempHighlightAttachments container, remove it
   for (auto iter = mTempHighlightAttachments.begin(); iter != mTempHighlightAttachments.end(); ++iter)
   {
      if (*iter == aAttachmentPtr)
      {
         mTempHighlightAttachments.erase(iter);
         break;
      }
   }
}

wkf::PointOfInterest* Map::DockWidget::AddPointOfInterest(const vespa::VaLocationReference& aLocRef)
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (mViewerPtr && scenario)
   {
      wkf::PointOfInterest* entity = vaEnv.CreateEntity<wkf::PointOfInterest>("pointOfInterest");
      entity->SetPosition(vespa::VaPosition(aLocRef.GetPosition().GetLat(), aLocRef.GetPosition().GetLon(), 0.0));
      scenario->AddEntity(entity);
      entity->Initialize();
      entity->Load();
      return entity;
   }
   return nullptr;
}

void Map::DockWidget::CenterCamera(wkf::Entity* aEntityPtr)
{
   if (aEntityPtr)
   {
      GetViewer()->CenterOnEntity(*aEntityPtr);
   }
}

void Map::DockWidget::Follow(wkf::Entity* aEntityPtr)
{
   if (aEntityPtr)
   {
      mCameraMotionPtr->Follow(aEntityPtr);
   }
}
