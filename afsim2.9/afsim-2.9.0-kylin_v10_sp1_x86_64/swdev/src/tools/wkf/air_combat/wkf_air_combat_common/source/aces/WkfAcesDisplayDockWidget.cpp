// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "aces/WkfAcesDisplayDockWidget.hpp"

#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGLWidget>
#include <QMimeData>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Matrix>

#include "UtEllipsoidalEarth.hpp"
#include "UtQt.hpp"
#include "UtQtColorButton.hpp"
#include "UtQtMemory.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaWidget.hpp"
#include "WkfAction.hpp"
#include "WkfAttachmentModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfOverlayUpdater.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"
#include "aces/WkfAcesDisplayOverlay.hpp"

wkf::AcesDisplay::DockWidget::DockWidget(const QString&  aObjectName,
                                         wkf::Platform*  aPlatformPtr,
                                         QMainWindow*    aParent,
                                         Qt::WindowFlags aFlags)
   : wkf::DockWidget(aObjectName, aParent, aFlags)
   , mPlatformPtr(aPlatformPtr)
{
   // Set the minimum size of the window when opening on a new platform
   setMinimumSize(960, 540);

   mConnectedPlatformIndex = aPlatformPtr->GetIndex();
   wkf::VtkEnvironment::InitializeOSG_Path();

   // Sets up the viewer, as well as the QWidget that it lives in
   vespa::VaWidget* vawidget =
      new vespa::VaWidget(vespa::VaViewer::SimpleViewer, vespa::VaViewer::SmallFeatureCulling, this);

   // Set the threshold check so we don't miss clicks
   vawidget->CheckDragThreshold(true);

   connect(vawidget, &vespa::VaWidget::Initialized, this, &DockWidget::ViewerInitialized);
   mViewerPtr = vawidget->GetViewer();
   mViewerPtr->SetData("type", UtVariant(std::string("aces")));
   mViewerPtr->GetCamera()->SetCustomCameraMotion(nullptr);
   setAcceptDrops(true);
   setWidget(vawidget);

   // based on TetherView
   mCallbacks.Add(vespa::VaObserver::ViewerResized.Connect(&DockWidget::ViewerResizedCB, this));

   setVisible(true);
   setAttribute(Qt::WA_DeleteOnClose);
   setFocusPolicy(Qt::StrongFocus);
}

wkf::AcesDisplay::DockWidget::~DockWidget()
{
   if (mViewerPtr)
   {
      // mViewerPtr is actually owned by the VaWidget, so we only set it to
      // nullptr. The VaWidget should be automatically destroyed because it's
      // a QWidget, and its parent is this, which is also a QWidget.
      mViewerPtr = nullptr;
   }
}

void wkf::AcesDisplay::DockWidget::ViewerResizedCB(vespa::VaViewer* aViewerPtr)
{
   // If this is the viewer and the overlay is valid,
   // resize the overlay, reinitialize any drawables
   // and update(re-draw).
   if (IsMyViewer(aViewerPtr) && mOverlayPtr)
   {
      mOverlayPtr->SetPosition(0, 0);
      mOverlayPtr->SetSize(aViewerPtr->GetWidth(), aViewerPtr->GetHeight());
      mOverlayPtr->InitializeDrawables();
      mOverlayPtr->Update();
   }
}

void wkf::AcesDisplay::DockWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->accept();
   }
}

void wkf::AcesDisplay::DockWidget::dropEvent(QDropEvent* aEvent)
{
   int                 highdpiFactor = QApplication::desktop()->devicePixelRatio();
   vespa::VaOverlayQt* overlayPtr =
      mViewerPtr->PickOverlayFromScreen<vespa::VaOverlayQt>(aEvent->pos().x() * highdpiFactor,
                                                            mViewerPtr->GetHeight() - aEvent->pos().y() * highdpiFactor);
   // If the viewer has a VaOverlayQt (which it probably won't but since the
   // viewer's pointer is available publicly, you never know) then have it deal
   // with it.
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
      in >> qi;
      in >> platformName;
      in >> datum;
      pluginId = static_cast<size_t>(qi);

      auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginId);
      if (plugin)
      {
         std::unique_ptr<wkf::Updater> updaterPtr = plugin->GetUpdater(platformName, datum);
         if (updaterPtr)
         {
            QPoint               p  = aEvent->pos();
            wkf::OverlayUpdater* ou = new wkf::OverlayUpdater(mViewerPtr,
                                                              updaterPtr,
                                                              platformName,
                                                              p.x() * highdpiFactor,
                                                              (height() - p.y()) * highdpiFactor);
            ou->SetVisible(true);
         }
      }
   }
}

void wkf::AcesDisplay::DockWidget::ScenarioRemovedCB(vespa::VaScenario* aScenarioPtr)
{
   vespa::VaScenario* stdScenario = vaEnv.GetStandardScenario();
   if ((!stdScenario) || (aScenarioPtr->GetKey() == stdScenario->GetKey()))
   {
      mViewerPtr = nullptr;
   }
}

void wkf::AcesDisplay::DockWidget::ViewerShutdownCB(vespa::VaViewer* aViewerPtr)
{
   // if something else destroyed the viewer (not this plug-in) this is probably game-over
   if (mViewerPtr)
   {
      if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
      {
         mViewerPtr = nullptr;
      }
   }
}

void wkf::AcesDisplay::DockWidget::ViewerInitialized()
{
   if (mViewerPtr)
   {
      mConnectionEstablished = true;
      mInitialized           = true;

      mCallbacks.Add(wkf::Observer::ScenarioRemoved.Connect(&DockWidget::ScenarioRemovedCB, this));
      mCallbacks.Add(wkf::Observer::ViewerDestroyed.Connect(&DockWidget::ViewerShutdownCB, this));

      // Kludgy fix to get the osg/opengl lines blending and drawing at the correct pixel sizing.  If you remove this
      // everything looks awful.
      wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMapProfile(wkf::SharedMapPreferencesData::NAVIGATIONAL,
                                                                                   mViewerPtr);

      emit DisplayInitialized();

      mOverlayPtr = new Overlay("aces", mViewerPtr, mPlatformPtr->GetIndex());
   }
}

bool wkf::AcesDisplay::DockWidget::IsMyViewer(vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
   {
      return true;
   }
   else
   {
      return false;
   }
}

const bool wkf::AcesDisplay::DockWidget::GetHasFocus()
{
   if (mOverlayPtr != nullptr)
   {
      return mOverlayPtr->GetHasFocus();
   }
   return false;
}

void wkf::AcesDisplay::DockWidget::SetHasFocus(const bool aHasFocus)
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetHasFocus(aHasFocus);
   }
}

void wkf::AcesDisplay::DockWidget::AddOverlay(vespa::VaOverlay* aOverlayPtr)
{
   mViewerPtr->AddOverlay(aOverlayPtr);
   aOverlayPtr->SetVisible(false);
}

void wkf::AcesDisplay::DockWidget::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](mViewerPtr);
}

bool wkf::AcesDisplay::DockWidget::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   BuildContextMenuPrivate(aMenu, aPos);

   wkfEnv.BuildViewerContextMenu(&aMenu, mViewerPtr);

#if defined(_DEBUG) || defined(SEE_DEBUG_OPTIONS_IN_RELEASE)
   QAction* action = new QAction("Show Scene Graph", &aMenu);
   connect(action, &QAction::triggered, this, &DockWidget::SceneGraphAction);
   aMenu.addAction(action);
#endif
   return true;
}

void wkf::AcesDisplay::DockWidget::ActivateKillFrame(bool aState)
{
   mConnectionEstablished = !aState;
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetConnectionDestroyed(aState);
   }
}

void wkf::AcesDisplay::DockWidget::ConnectionDestroyed(vespa::VaEntity* aEntityPtr)
{
   if (mConnectionEstablished)
   {
      if (mPlatformPtr)
      {
         if (aEntityPtr->GetUniqueId() == mPlatformPtr->GetUniqueId())
         {
            mConnectionEstablished = false;
         }
      }
   }
}

void wkf::AcesDisplay::DockWidget::SetAcesData(const AcesData& aData)
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetAcesData(aData);
   }
}

void wkf::AcesDisplay::DockWidget::SetAcesTruthPlatforms(const std::vector<AcesDisplay::EntityTruth>& aData)
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetAcesTruthPlatforms(aData);
   }
}

void wkf::AcesDisplay::DockWidget::ResetCenterPoint()
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->ResetCenterPoint();
   }
}

void wkf::AcesDisplay::DockWidget::SetAcesPlatformComment(const std::string& aPlatformCommentStr)
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetAcesPlatformComment(aPlatformCommentStr);
   }
}

void wkf::AcesDisplay::DockWidget::SetCenterPoint()
{
   if (mOverlayPtr != nullptr)
   {
      mOverlayPtr->SetCenterPoint();
   }
}

void wkf::AcesDisplay::DockWidget::BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos)
{
   auto action = new QAction(QIcon::fromTheme("aim"), QString("Set Center Point"), &aMenu);
   connect(action, &QAction::triggered, this, &DockWidget::SetCenterPoint);
   aMenu.addAction(action);

   auto action2 = new QAction(QIcon::fromTheme("undo"), QString("Reset Center Point"), &aMenu);
   connect(action2, &QAction::triggered, this, &DockWidget::ResetCenterPoint);
   aMenu.addAction(action2);
}

void wkf::AcesDisplay::DockWidget::closeEvent(QCloseEvent* aEvent)
{
   // Tether views should be deleted when they are closed, and recreated on demand
   deleteLater();
}

void wkf::AcesDisplay::DockWidget::changeEvent(QEvent* aEvent)
{
   wkf::DockWidget::changeEvent(aEvent);

   if (aEvent->type() == QEvent::ActivationChange && isActiveWindow())
   {
      emit DisplayGainedFocus(mConnectedPlatformIndex);
      if (mOverlayPtr != nullptr)
      {
         mOverlayPtr->SetHasFocus(true);
      }
   }
}

void wkf::AcesDisplay::DockWidget::ClearCallback(int& aId)
{
   if (aId)
   {
      mCallbacks.Disconnect(aId);
      aId = 0;
   }
}

void wkf::AcesDisplay::DockWidget::SetTargetPlatforms(const std::vector<size_t>& aTargetPlatforms)
{
   if (GetHasFocus())
   {
      mOverlayPtr->SetTargetPlatforms(aTargetPlatforms);
   }
}
