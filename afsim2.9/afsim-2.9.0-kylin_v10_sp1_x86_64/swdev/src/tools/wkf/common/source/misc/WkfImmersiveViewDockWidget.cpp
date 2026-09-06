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
#include "WkfImmersiveViewDockWidget.hpp"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QGLWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QMimeData>
#include <QVariant>
#include <osg/Matrix>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtQtColorButton.hpp"
#include "UtoViewer.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentVectors.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "VaModelDatabase.hpp"
#include "VaOverlayTexture.hpp"
#include "VaWidget.hpp"
#include "WkfAngleViewUpdater.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfOverlayUpdater.hpp"
#include "WkfOverlayUpdaterPrefObject.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfVtkEnvironment.hpp"
#include "selectors/WkfGroupingSelector.hpp"

wkf::ImmersiveViewDockWidget::ImmersiveViewDockWidget(const QString&  aObjectName,
                                                      const QString&  aPlatformName,
                                                      QMainWindow*    parent,
                                                      Qt::WindowFlags flags)
   : wkf::DockWidget(aObjectName, parent, flags)
   , mViewerPtr(nullptr)
   , mPlatformPtr(nullptr)
   , mConnectedPlatformName(aPlatformName)
   , mLookAtName("")
   , mLookAtEntity(nullptr)
   , mLookAtCallbackId(0, 0)
   , mDeleteCallbackId(0, 0)
   , mLastDist(400)
   , mDecoratorOverlay(nullptr)
   , mInitialized(false)
{
   wkf::VtkEnvironment::InitializeOSG_Path();
   mFrame          = new QFrame(this);
   QHBoxLayout* lo = new QHBoxLayout(mFrame);
   lo->setContentsMargins(0, 0, 0, 0);
   mFrame->setLayout(lo);
   mFrame->setLineWidth(0);
   mFrame->setObjectName("killFrame");
   setWidget(mFrame);

   vespa::VaWidget* vawidget =
      new vespa::VaWidget(vespa::VaViewer::SimpleViewer, vespa::VaViewer::SmallFeatureCulling, this);
   connect(vawidget, &vespa::VaWidget::Initialized, this, &wkf::ImmersiveViewDockWidget::ViewerInitialized);
   mFrame->layout()->addWidget(vawidget);
   mViewerPtr = vawidget->GetViewer();
   mViewerPtr->SetData("type", UtVariant(std::string("immersive")));
   vaEnv.GetModelDatabase()->SetViewerModelSet(mViewerPtr,
                                               "immersive"); // let the model-set know what types of models we want
   setAcceptDrops(true);

   if (SupportsVectors())
   {
      mVectorManager = ut::make_unique<VectorManager>(this);
   }
}

wkf::ImmersiveViewDockWidget::~ImmersiveViewDockWidget()
{
   if (mViewerPtr)
   {
      mViewerPtr = nullptr;
      if (SupportsVectors())
      {
         mVectorManager->Clear();
      }
   }
}

void wkf::ImmersiveViewDockWidget::LookAt(const QString& aName)
{
   mLookAtName = aName;
   if (vaEnv.GetStandardScenario() && mPlatformPtr)
   {
      vespa::VaEntity* ent = vaEnv.GetStandardScenario()->FindEntity(mLookAtName.toStdString());
      LookAt(ent);
   }
}

void wkf::ImmersiveViewDockWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   if (aEvent->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEvent->accept();
   }
}

void wkf::ImmersiveViewDockWidget::dropEvent(QDropEvent* aEvent)
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
      in >> qi;
      in >> platformName;
      in >> datum;
      pluginId = static_cast<size_t>(qi);

      auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginId);
      if (plugin)
      {
         auto updaterPtr = plugin->GetUpdater(platformName, datum);
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

void wkf::ImmersiveViewDockWidget::ViewerInitialized()
{
   if (mViewerPtr)
   {
      mInitialized = true;
      mViewerPtr->SetTrueScale(true);
      mViewerPtr->SuppressTeamColor(true);

      mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&ImmersiveViewDockWidget::PlatformsAdded, this));
      mCallbacks.Add(wkf::Observer::PlatformPendingDeletion.Connect(&ImmersiveViewDockWidget::PlatformToBeDeletedCB, this));
      mCallbacks.Add(wkf::Observer::ScenarioRemoved.Connect(&ImmersiveViewDockWidget::ScenarioRemovedCB, this));
      mCallbacks.Add(wkf::Observer::ViewerDestroyed.Connect(&ImmersiveViewDockWidget::ViewerShutdownCB, this));
      mCallbacks.Add(wkf::Observer::SubpartAdded.Connect(&ImmersiveViewDockWidget::SubpartAddedCB, this));
      mCallbacks.Add(wkf::Observer::SubpartRemoved.Connect(&ImmersiveViewDockWidget::SubpartRemovedCB, this));
      mCallbacks.Add(wkf::Observer::SubpartUpdated.Connect(&ImmersiveViewDockWidget::SubpartUpdatedCB, this));
      mCallbacks.Add(wkf::Observer::EntityDecorated.Connect(&ImmersiveViewDockWidget::PlatformDecoratedCB, this));

      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         vespa::VaEntityList entitylist;
         scenarioPtr->GetEntityList(entitylist);
         vespa::VaEntityList::iterator it;
         wkf::PlatformList             platforms;
         for (it = entitylist.begin(); it != entitylist.end(); ++it)
         {
            wkf::Platform* platform = dynamic_cast<wkf::Platform*>(*it);
            if (platform)
            {
               platforms.push_back(platform);
            }
         }
         PlatformsAdded(platforms);
      }
      ViewerInitializedPrivate();
      wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMapProfile(wkf::SharedMapPreferencesData::IMMERSIVE,
                                                                                   mViewerPtr);
      mViewerPtr->SetUniform("VA_ModelsSupportLighting", 1.0);

      emit ImmersiveViewInitialized();
   }
}

// if the connection was established during construction, our derived catch may not have been called,
// by calling this at the end of the derived construction we ensure that the object gets the call
void wkf::ImmersiveViewDockWidget::CompleteConstruction()
{
   if (mConnectionEstablished)
   {
      ConnectionEstablishedPrivate();
   }
}

void wkf::ImmersiveViewDockWidget::LookAt(vespa::VaEntity* aEntity)
{
   ClearCallback(mLookAtCallbackId.first);
   ClearCallback(mLookAtCallbackId.second);
   ClearCallback(mDeleteCallbackId.first);
   ClearCallback(mDeleteCallbackId.second);

   mLookAtEntity = aEntity;
   if (mViewerPtr)
   {
      vespa::VaCameraMotionTethered* cam =
         dynamic_cast<vespa::VaCameraMotionTethered*>(mViewerPtr->GetCamera()->GetCameraMotion());
      cam->AllowUserControl(false);
      cam->SetUseRefHeading(false);
      mLookAtCallbackId.first =
         mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&ImmersiveViewDockWidget::LookAtPlatformMovedCB,
                                                           this,
                                                           mLookAtEntity->GetUniqueId()));
      mLookAtCallbackId.second =
         mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&ImmersiveViewDockWidget::LookAtPlatformMovedCB,
                                                           this,
                                                           mPlatformPtr->GetUniqueId()));
      mDeleteCallbackId.first =
         mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&ImmersiveViewDockWidget::LookAtPlatformDeletedCB,
                                                             this,
                                                             mLookAtEntity->GetUniqueId()));
      mDeleteCallbackId.second =
         mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&ImmersiveViewDockWidget::LookAtPlatformDeletedCB,
                                                             this,
                                                             mPlatformPtr->GetUniqueId()));
      LookAtPlatformMovedCB(aEntity, false);
   }
}

bool wkf::ImmersiveViewDockWidget::BuildContextMenu(QMenu& aMenu, const QPoint& pos)
{
   int                 highdpiFactor = QApplication::desktop()->devicePixelRatio();
   vespa::VaOverlayQt* overlayPtr =
      mViewerPtr->PickOverlayFromScreen<vespa::VaOverlayQt>(pos.x() * highdpiFactor,
                                                            mViewerPtr->GetHeight() - pos.y() * highdpiFactor);
   if ((overlayPtr) &&
       (overlayPtr->BuildContextMenu(aMenu,
                                     QPoint(pos.x() * highdpiFactor, mViewerPtr->GetHeight() - pos.y() * highdpiFactor))))
   {
      return false;
   }

   BuildContextMenuPrivate(aMenu, pos);

   if (SupportsVectors())
   {
      mVectorManager->PopulateMenu(aMenu);
   }

   wkfEnv.BuildViewerContextMenu(&aMenu, mViewerPtr);

#if defined(_DEBUG) || defined(SEE_DEBUG_OPTIONS_IN_RELEASE)
   QAction* action = new QAction("Show Scene Graph", &aMenu);
   connect(action, &QAction::triggered, this, &ImmersiveViewDockWidget::SceneGraphAction);
   aMenu.addAction(action);
#endif
   return true;
}

void wkf::ImmersiveViewDockWidget::ActivateKillFrame(bool aState)
{
   if (aState)
   {
      mFrame->setLineWidth(4);
      mFrame->setStyleSheet("#killFrame { border: 10px solid red; }");
   }
   else
   {
      mFrame->setLineWidth(0);
      mFrame->setStyleSheet("#killFrame { border: 0px solid black; }");
   }
}

void wkf::ImmersiveViewDockWidget::ViewerShutdownCB(vespa::VaViewer* aViewerPtr)
{
   // if something else destroyed the viewer (not this plug-in) this is probably game-over
   if (mViewerPtr)
   {
      if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
      {
         mViewerPtr = nullptr;
         ClearData();
      }
   }
}

void wkf::ImmersiveViewDockWidget::ScenarioRemovedCB(vespa::VaScenario* aScenarioPtr)
{
   vespa::VaScenario* stdScenario = vaEnv.GetStandardScenario();
   if ((!stdScenario) || (aScenarioPtr->GetKey() == stdScenario->GetKey()))
   {
      ClearData();
   }
}

void wkf::ImmersiveViewDockWidget::ClearData()
{
   mPlatformPtr = nullptr;
   mShownBoresights.clear();
   if (SupportsVectors())
   {
      mVectorManager->Clear();
   }
   ClearDataPrivate();
}

void wkf::ImmersiveViewDockWidget::LookAtPlatformMovedCB(vespa::VaEntity* aEntityPtr, bool aUiDriven)
{
   if (mLookAtEntity)
   {
      vespa::VaCameraMotionTethered* tetherCam =
         dynamic_cast<vespa::VaCameraMotionTethered*>(mViewerPtr->GetCamera()->GetCameraMotion());
      double src[3];
      if (tetherCam->GetEntity() == nullptr)
      {
         return;
      }
      tetherCam->GetEntity()->GetPosition().GetECEF(src);
      double dst[3];
      mLookAtEntity->GetPosition().GetECEF(dst);
      double   vec[3] = {src[0] - dst[0], src[1] - dst[1], src[2] - dst[2]};
      double   range  = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
      UtEntity srcEnt;
      srcEnt.SetLocationWCS(src);
      double iaz;
      double iel;
      srcEnt.ComputeAspect(vec, iaz, iel);
      iaz -= mPlatformPtr->GetHeading() - UtMath::cPI;

      float  fovy3 = tetherCam->GetFovY() * 0.33f;
      double d     = 30;
      // 0. calculate a pitch that will put the target in the center and the ownship down by a third of the window
      float addPitch = fovy3 * UtMath::cRAD_PER_DEG + asin(d / range * sin(fovy3 * UtMath::cRAD_PER_DEG));

      double h;
      double p;
      double r;
      tetherCam->GetEntity()->GetOrientation(h, p, r);
      // 1. build a matrix that converts from local to ecs
      osg::Matrix m(osg::Quat(h, osg::Vec3d(0, 1, 0), p, osg::Vec3d(0, 0, 1), r, osg::Vec3d(1, 0, 0)));
      // 2. make it convert local to boresight
      m.postMultRotate(osg::Quat(iaz, osg::Vec3d(0, 1, 0)));
      m.postMultRotate(osg::Quat(-iel, osg::Vec3d(0, 0, 1)));
      // 3. get the boresight vector
      osg::Vec3 v = m * osg::Vec3(1, 0, 0);
      // 4. convert that to azimuth elevation
      float az = atan2(v.z(), v.x()) * UtMath::cDEG_PER_RAD;
      float el = atan2(v.y(), sqrt(v.x() * v.x() + v.z() * v.z())) * UtMath::cDEG_PER_RAD;
      // 5. add the calculated pitch to frame the points of interest correctly
      tetherCam->SetAzElDist(az, el + addPitch * UtMath::cDEG_PER_RAD, d);
      // 6. pitch the camera to complete to framing
      tetherCam->SetYawPitchRoll(0, 90 - fovy3, 0);
   }
}

void wkf::ImmersiveViewDockWidget::LookAtPlatformDeletedCB(vespa::VaEntity* aEntityPtr)
{
   ClearCallback(mLookAtCallbackId.first);
   ClearCallback(mLookAtCallbackId.second);
   ClearCallback(mDeleteCallbackId.first);
   ClearCallback(mDeleteCallbackId.second);
   mLookAtEntity = nullptr; // this will suffice to prevent the processing on entity moved
}

void wkf::ImmersiveViewDockWidget::ClearCallback(int& aId)
{
   if (aId)
   {
      mCallbacks.Disconnect(aId);
      aId = 0;
   }
}

void wkf::ImmersiveViewDockWidget::PlatformsAdded(const wkf::PlatformList& aPlatforms)
{
   for (const auto& platform : aPlatforms)
   {
      if (vaEnv.GetStandardScenario())
      {
         if (platform->GetScenarioKey() == vaEnv.GetStandardScenario()->GetKey())
         {
            vespa::VaAttachmentModel* modelPtr =
               vespa::VaEnvironment::CreateAttachment<vespa::VaAttachmentModel>("model", *platform, mViewerPtr);
            if (modelPtr)
            {
               modelPtr->SetIcon(platform->GetIcon());
               vespa::VaAttachment::LoadAttachment(*modelPtr);
            }
            for (auto&& it : platform->GetSubobjectMap())
            {
               SubpartAddedCB(platform, it.first);
            }
            if (platform->GetName() == mConnectedPlatformName.toStdString())
            {
               mViewerPtr->GetCamera()->Tether(platform);
               vespa::VaCameraMotionTethered* tetherCam =
                  (vespa::VaCameraMotionTethered*)(mViewerPtr->GetCamera()->GetCameraMotion());
               tetherCam->SetUseRefHeading(false);
               mPlatformPtr = platform;
               mCallbacks.Add(
                  vespa::VaObserver::EntityDeleted.Connect(&wkf::ImmersiveViewDockWidget::ConnectionDestroyed, this));
               mConnectionEstablished  = true;
               const QIcon& decoration = platform->GetDecoration();
               if (!decoration.pixmap(QSize(32, 32)).isNull())
               {
                  PlatformDecoratedCB(platform, decoration, "");
               }
               ConnectionEstablishedPrivate();
               if (!mLookAtName.isEmpty())
               {
                  LookAt(mLookAtName);
               }
            }
            else if (platform->GetName() == mLookAtName.toStdString())
            {
               if (mPlatformPtr)
               {
                  LookAt(platform);
               }
            }
         }
      }

      if (SupportsVectors())
      {
         mVectorManager->AddPlatform(platform);
      }
      PlatformAddedPrivate(platform);
   }
}

void wkf::ImmersiveViewDockWidget::SubpartAddedCB(wkf::Platform* aPlatformPtr, const std::string& aName)
{
   if (mSubpartLookup.find(std::pair<int, std::string>(aPlatformPtr->GetIndex(), aName)) == mSubpartLookup.end())
   {
      auto si = aPlatformPtr->GetSubobjectMap().find(aName);
      if (si != aPlatformPtr->GetSubobjectMap().end())
      {
         const wkf::Platform::Subobject& so = si->second;
         if (so.mIcon.empty())
         {
            return;
         }
         vespa::VaAttachmentModel* modelPtr =
            vespa::VaEnvironment::CreateAttachment<vespa::VaAttachmentModel>("model", *aPlatformPtr, mViewerPtr);
         if (modelPtr)
         {
            mSubpartLookup[std::pair<int, std::string>(aPlatformPtr->GetIndex(), aName)] = modelPtr->GetUniqueId();
            modelPtr->SetIcon(so.mIcon);
            osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix;
            matrix->preMultTranslate(osg::Vec3(so.mPositionECS[0], so.mPositionECS[1], so.mPositionECS[2]));
            matrix->preMultRotate(osg::Quat(so.mOrientationECS[0],
                                            osg::Vec3(0, 0, 1),
                                            so.mOrientationECS[1],
                                            osg::Vec3(0, 1, 0),
                                            so.mOrientationECS[2],
                                            osg::Vec3(1, 0, 0)));
            // need to multiply rotation in
            modelPtr->SetRelativeToParentTransform(matrix);
         }
      }
   }
   else
   {
      auto sp = mSubpartLookup.find(std::make_pair(aPlatformPtr->GetIndex(), aName));
      if (sp != mSubpartLookup.end())
      {
         vespa::VaAttachment* attach = aPlatformPtr->FindAttachment(sp->second);
         if (attach)
         {
            attach->SetStateVisibility(true);
         }
      }
   }
}

void wkf::ImmersiveViewDockWidget::SubpartRemovedCB(wkf::Platform* aPlatformPtr, const std::string& aName)
{
   auto sp = mSubpartLookup.find(std::make_pair(aPlatformPtr->GetIndex(), aName));
   if (sp != mSubpartLookup.end())
   {
      auto* attachment = aPlatformPtr->FindAttachment(sp->second);
      if (attachment)
      {
         attachment->SetStateVisibility(false);
      }
   }
}

void wkf::ImmersiveViewDockWidget::SubpartUpdatedCB(wkf::Platform* aPlatformPtr, const std::string& aName)
{
   auto si = aPlatformPtr->GetSubobjectMap().find(aName);
   if (si != aPlatformPtr->GetSubobjectMap().end())
   {
      const wkf::Platform::Subobject& so = si->second;
      unsigned int              uid      = mSubpartLookup[std::pair<int, std::string>(aPlatformPtr->GetIndex(), aName)];
      vespa::VaAttachmentModel* modelPtr = dynamic_cast<vespa::VaAttachmentModel*>(aPlatformPtr->FindAttachment(uid));
      if (modelPtr)
      {
         osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix;
         matrix->preMultTranslate(osg::Vec3(so.mPositionECS[0], so.mPositionECS[1], so.mPositionECS[2]));
         matrix->preMultRotate(osg::Quat(so.mOrientationECS[0],
                                         osg::Vec3(0, 0, 1),
                                         so.mOrientationECS[1],
                                         osg::Vec3(0, 1, 0),
                                         so.mOrientationECS[2],
                                         osg::Vec3(1, 0, 0)));
         // need to multiply rotation in
         modelPtr->SetRelativeToParentTransform(matrix);
      }

      for (auto&& model : aPlatformPtr->FindAttachmentsOfType<vespa::VaAttachmentModel>())
      {
         if (model->IsMyViewer(mViewerPtr))
         {
            // do articulations
            osg::Matrix matrix;
            matrix.makeRotate(osg::Quat(so.mOrientationECS[0],
                                        osg::Vec3(0, 0, 1),
                                        so.mOrientationECS[1],
                                        osg::Vec3(0, 1, 0),
                                        so.mOrientationECS[2],
                                        osg::Vec3(1, 0, 0)));
            model->ArticulateModel(aName, matrix);
         }
      }
   }
}

void wkf::ImmersiveViewDockWidget::PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr)
{
   aPlatformPtr->SetVisibilityContribution(false, vespa::VaEntity::ScenarioStateHidden, mViewerPtr);
   if (SupportsVectors())
   {
      mVectorManager->PlatformToBeDeleted(aPlatformPtr);
   }
   PlatformToBeDeletedPrivate(aPlatformPtr);
}

void wkf::ImmersiveViewDockWidget::ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   mVectorManager->ShowBoresight(aLabel, aAzimuth, aElevation);
   mShownBoresights.insert(aLabel);
}

void wkf::ImmersiveViewDockWidget::UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   mVectorManager->UpdateBoresight(aLabel, aAzimuth, aElevation);
}

void wkf::ImmersiveViewDockWidget::HideBoresight(const std::string& aLabel)
{
   mVectorManager->HideBoresight(aLabel);
   mShownBoresights.erase(aLabel);
}

void wkf::ImmersiveViewDockWidget::ResetCamera()
{
   mViewerPtr->GetCamera()->Reset();
}

vespa::VaAttachment* wkf::ImmersiveViewDockWidget::AddAttachment(const std::string& aType)
{
   if (mPlatformPtr)
   {
      return vaEnv.CreateAttachment(aType, *mPlatformPtr, mViewerPtr);
   }
   else
   {
      return nullptr;
   }
}

void wkf::ImmersiveViewDockWidget::AddOverlay(vespa::VaOverlay* aOverlayPtr)
{
   mViewerPtr->AddOverlay(aOverlayPtr);
   aOverlayPtr->SetVisible(false);
}

void wkf::ImmersiveViewDockWidget::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](mViewerPtr);
}

bool wkf::ImmersiveViewDockWidget::IsMyViewer(vespa::VaViewer* aViewerPtr)
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

void wkf::ImmersiveViewDockWidget::SetFirstPerson(bool aState)
{
   double az;
   double el;
   double dist;
   mViewerPtr->GetCamera()->GetCameraMotion()->GetAzElDist(az, el, dist);
   if (aState)
   {
      mLastDist = dist;
      dist      = 0;
      mViewerPtr->GetCamera()->GetCameraMotion()->RequestDist(dist);
   }
   else
   {
      dist = mLastDist;
      mViewerPtr->GetCamera()->GetCameraMotion()->RequestDist(dist);
   }
   SetFirstPersonPrivate(aState);
}

void wkf::ImmersiveViewDockWidget::GetAzElDist(double& aAz, double& aEl, double& aDist)
{
   mViewerPtr->GetCamera()->GetCameraMotion()->GetAzElDist(aAz, aEl, aDist);
}

void wkf::ImmersiveViewDockWidget::SetAzElDist(double aAz, double aEl, double aDist)
{
   mViewerPtr->GetCamera()->GetCameraMotion()->SetAzElDist(aAz, aEl, aDist);
}

void wkf::ImmersiveViewDockWidget::ConnectionDestroyed(vespa::VaEntity* aEntityPtr)
{
   if (mConnectionEstablished)
   {
      if (mPlatformPtr)
      {
         if (aEntityPtr->GetUniqueId() == mPlatformPtr->GetUniqueId())
         {
            ConnectionDestroyedPrivate();
            mConnectionEstablished = false;
            ClearData();
         }
      }
   }
}

void wkf::ImmersiveViewDockWidget::SetLockToHeading(bool aEnabled)
{
   vespa::VaCameraMotionTethered* tetherCam =
      dynamic_cast<vespa::VaCameraMotionTethered*>(mViewerPtr->GetCamera()->GetCameraMotion());
   if (tetherCam)
   {
      tetherCam->SetUseRefHeading(aEnabled);
   }
}

void wkf::ImmersiveViewDockWidget::SetLightingEnabled(bool aEnabled)
{
   mViewerPtr->EnableLighting(aEnabled);
}

bool wkf::ImmersiveViewDockWidget::GetLockToHeading() const
{
   vespa::VaCameraMotionTethered* tetherCam =
      dynamic_cast<vespa::VaCameraMotionTethered*>(mViewerPtr->GetCamera()->GetCameraMotion());
   if (tetherCam)
   {
      return tetherCam->GetUseRefHeading();
   }
   return false;
}

void wkf::ImmersiveViewDockWidget::PlatformDecoratedCB(wkf::Entity*       aEntityPtr,
                                                       const QIcon&       aIcon,
                                                       const std::string& aDescription)
{
   auto platformPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
   if (platformPtr == nullptr)
   {
      return;
   }
   if (mViewerPtr && mPlatformPtr)
   {
      if (platformPtr->GetUniqueId() == mPlatformPtr->GetUniqueId())
      {
         if (aIcon.isNull())
         {
            if (mDecoratorOverlay)
            {
               mViewerPtr->RemoveOverlay(mDecoratorOverlay->GetName());
               mDecoratorOverlay = nullptr;
            }
            // remove overlay
         }
         else
         {
            if (!mDecoratorOverlay)
            {
               mDecoratorOverlay = new vespa::VaOverlayTexture("decorator");
               mViewerPtr->AddOverlay(mDecoratorOverlay);
               mDecoratorOverlay->SetVisible(true);
               mDecoratorOverlay->SetPosition(3 + 32, 3 + 32);
               mDecoratorOverlay->SetShader("VaOverlayTexture", "VaOverlayTextureColor");
               mDecoratorOverlay->EnableTransparency(true);
               mDecoratorOverlay->SetSize(32, 32);
               mDecoratorOverlay->SetJustification(false, false);
               // create overlay
            }
            if (mDecoratorOverlay)
            {
               QImage image        = aIcon.pixmap(32, 32).toImage();
               QImage GL_formatted = QGLWidget::convertToGLFormat(image);
               mDecoratorOverlay->LoadImageBits(32, 32, GL_formatted.bits());
               mDecoratorOverlay->EnableTransparency(true);
            }
         }
      }
   }
}

wkf::ImmersiveViewDockWidget::VectorManager::VectorManager(ImmersiveViewDockWidget* aParent)
   : QObject(aParent)
   , mParentWidget(aParent)
{
   mCallbacks.Add(vespa::VaObserver::EntityRemoved.Connect(&VectorManager::EntityRemovedCB, this));
}

void wkf::ImmersiveViewDockWidget::VectorManager::Clear()
{
   mVectorAttachmentPtr = nullptr;
   mGroupingVectors.clear();
   mVectorState.clear();
   mAnglesAttachmentPtr = nullptr;
}

void wkf::ImmersiveViewDockWidget::VectorManager::PopulateMenu(QMenu& aMenu)
{
   QMenu* vectors = new QMenu("Vectors", &aMenu);
   vectors->setObjectName("Vectors");
   // Context menu to visualize angles.
   if (mVectorAttachmentPtr) // if we have a vector
   {
      int count = mVectorAttachmentPtr->GetCategoryCount();
      for (int i = 0; i < count; ++i)
      {
         if (mVectorAttachmentPtr->GetBoresightId(mVectorAttachmentPtr->GetCategoryText(i)) == -1)
         {
            QAction* vbxaction = new QAction(QString::fromStdString(mVectorAttachmentPtr->GetCategoryText(i)), vectors);
            vbxaction->setCheckable(true);
            vbxaction->setChecked(mVectorState[i]);
            vbxaction->setData(i);
            connect(vbxaction, &QAction::triggered, this, &VectorManager::VectorActionTriggered);
            vectors->addAction(vbxaction);
         }
      }
   }
   else
   {
      // otherwise use the static members(eVECTOR_STATIC_COUNT, GetVectorTextStatic(i))
      for (int i = 0; i < vespa::VaAttachmentVectors::eVECTOR_STATIC_COUNT; ++i)
      {
         QAction* vbxaction = new QAction(vespa::VaAttachmentVectors::GetCategoryTextStatic(i).c_str(), vectors);
         vbxaction->setCheckable(true);
         vbxaction->setChecked(mVectorState[i]);
         vbxaction->setData(i);
         connect(vbxaction, &QAction::triggered, this, &VectorManager::VectorActionTriggered);
         vectors->addAction(vbxaction);
      }
   }
   QAction* newVectorAction = new QAction("New Vector...", &aMenu);
   newVectorAction->setData(-1);
   connect(newVectorAction, &QAction::triggered, this, &VectorManager::VectorActionTriggered);
   vectors->addAction(newVectorAction); // add a new vector to a platform or group
   aMenu.addMenu(vectors);

   // Make sure it comes last in vector menu.
   QAction* angleVisualizationAction = new QAction("Visualize Angles...", &aMenu);
   connect(angleVisualizationAction, &QAction::triggered, this, &VectorManager::VisualizeAnglesClickHandler);
   vectors->addAction(angleVisualizationAction);

   // Angles Menu
   if (mAnglesAttachmentPtr)
   {
      QMenu* angles = new QMenu("Angles", &aMenu);
      for (auto& entry : mAnglesAttachmentPtr->GetEntries())
      {
         std::string uniqueId        = entry.first;
         size_t      positionOfSplit = uniqueId.find("_");
         std::string vectorIdOnes    = uniqueId.substr(0, positionOfSplit);
         int         vectorIdOnei    = std::stoi(vectorIdOnes);
         std::string vectorOneName   = mVectorAttachmentPtr->GetCategoryText(vectorIdOnei);

         std::string vectorIdTwos  = uniqueId.substr(positionOfSplit + 1, uniqueId.length() - (positionOfSplit + 1));
         int         vectorIdTwoi  = std::stoi(vectorIdTwos);
         std::string vectorTwoName = mVectorAttachmentPtr->GetCategoryText(vectorIdTwoi);

         QString angleEntryName = QString::fromStdString(vectorOneName + " ∠ " + vectorTwoName);
         QMenu*  subMenu        = new QMenu(angleEntryName, angles);
         QPixmap pixmap(100, 100);
         pixmap.fill(entry.second.GetColor());
         QIcon icon(pixmap);
         subMenu->setIcon(icon);
         QAction* angleVisibleAction = new QAction("Visible", subMenu);
         angleVisibleAction->setCheckable(true);
         angleVisibleAction->setChecked(entry.second.IsAngleVisible());
         angleVisibleAction->setData(QVariant(QString::fromStdString(uniqueId)));
         connect(angleVisibleAction, &QAction::triggered, this, &VectorManager::HandleAngleVisibilityClick);
         subMenu->addAction(angleVisibleAction);

         QAction* angleFilledAction = new QAction("Filled", subMenu);
         angleFilledAction->setCheckable(true);
         angleFilledAction->setChecked(entry.second.IsModeFill());
         angleFilledAction->setData(QVariant(QString::fromStdString(uniqueId)));
         connect(angleFilledAction, &QAction::triggered, this, &VectorManager::HandleAngleFillClick);
         subMenu->addAction(angleFilledAction);
         angles->addMenu(subMenu);
      }

      aMenu.addMenu(angles);

      // overlay
      QMenu* subMenu = new QMenu("Overlay", angles);
      auto   ou      = GetAnglesOverlay();
      if (ou)
      {
         QAction* overlayAction = new QAction("Visible", subMenu);
         overlayAction->setCheckable(true);
         overlayAction->setChecked(ou->IsVisible());
         connect(overlayAction, &QAction::triggered, this, &VectorManager::HandleOverlayVisibilityClick);
         subMenu->addAction(overlayAction);
      }

      QAction* syncOverlayAction = new QAction("Sync with view", subMenu);
      syncOverlayAction->setCheckable(false);
      connect(syncOverlayAction, &QAction::triggered, this, &VectorManager::HandleOverlaySync);
      subMenu->addAction(syncOverlayAction);
      angles->addMenu(subMenu);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::VectorActionTriggered()
{
   QAction* senderAction = dynamic_cast<QAction*>(sender());
   if (senderAction)
   {
      int data = senderAction->data().toInt();

      if (data >= 0)
      {
         bool state = false;
         auto it    = mVectorState.find(data);
         if (it != mVectorState.end())
         {
            state = it->second;
         }
         state              = !state;
         mVectorState[data] = state;
         ShowVector(data, state);
      }
      else if (data == -1)
      {
         auto* scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr)
         {
            std::map<wkf::PlatformGrouping, std::set<int>> cats = wkfEnv.GetGroupings();

            vespa::VaEntityList entList;
            scenarioPtr->GetEntityList(entList);
            for (auto& jt : entList)
            {
               wkf::Platform* entityPtr = dynamic_cast<wkf::Platform*>(jt);
               if (entityPtr != nullptr)
               {
                  cats[wkf::PlatformGrouping(wkf::PlatformGrouping::eIndividual, jt->GetName())].insert(
                     entityPtr->GetIndex());
               }
            }

            QDialog* selector = new QDialog(mParentWidget);
            selector->setWindowTitle("Add Vector");
            QVBoxLayout*      layout = new QVBoxLayout(selector);
            QDialogButtonBox* dbb    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, selector);
            wkf::GroupingSelector* groupSelect = new wkf::GroupingSelector(selector);
            UtQtColorButton*       colorButton = new UtQtColorButton(selector);
            colorButton->SetColor(Qt::red);
            layout->addWidget(new QLabel("Point vector to:", selector));
            layout->addWidget(groupSelect);
            layout->addWidget(new QLabel("Color:", selector));
            layout->addWidget(colorButton);
            layout->addWidget(dbb);
            connect(dbb, &QDialogButtonBox::accepted, selector, &QDialog::accept);
            connect(dbb, &QDialogButtonBox::rejected, selector, &QDialog::reject);
            if (QDialog::Accepted == selector->exec())
            {
               QColor                       color = colorButton->GetColor();
               const wkf::PlatformGrouping& g     = groupSelect->GetGrouping();
               if (g.Type() == wkf::PlatformGrouping::eIndividual)
               {
                  wkf::Platform* plat = scenarioPtr->FindPlatformByName(g.Name());
                  if (plat)
                  {
                     std::set<int> pset;
                     pset.insert(plat->GetIndex());
                     AddVectorSet(g, pset, color);
                  }
               }
               else
               {
                  const auto& grouppair = wkfEnv.GetGroupings().find(g);
                  AddVectorSet(g, grouppair->second, color);
               }
            }
            delete dbb;
         }
      }
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::VisualizeAnglesClickHandler()
{
   InstantiateAngleAttachment();
   AngleInfo results = RunVisualizerDialogue();
   int       vec1Id  = std::get<0>(results);
   int       vec2Id  = std::get<1>(results);
   if (vec1Id == -1 || vec2Id == -1)
      return; // canceled.
   UtVec3f vec1       = GetVectorPosition(vec1Id);
   UtVec3f vec2       = GetVectorPosition(vec2Id);
   float   vec1Scale  = GetVectorScale(vec1Id);
   float   vec2Scale  = GetVectorScale(vec2Id);
   QColor  angleColor = std::get<2>(results);

   if (mAnglesAttachmentPtr)
   {
      std::string       uniqueEntryId = std::to_string(vec1Id) + "_" + std::to_string(vec2Id);
      const std::string name =
         mVectorAttachmentPtr->GetCategoryText(vec1Id) + " ∠ " + mVectorAttachmentPtr->GetCategoryText(vec2Id);
      // std::min is used so that we draw the smallest radius, otherwise it would be extended past the end of the
      // smallest vector. granted these should never not be the same, you never know.
      this->mAnglesAttachmentPtr->AddEntry(vec1, vec2, angleColor, uniqueEntryId, name, std::min(vec1Scale, vec2Scale));
      const std::string             platformName = mAnglesAttachmentPtr->GetParent().GetName();
      auto                          currentEntry = mAnglesAttachmentPtr->GetEntry(uniqueEntryId);
      std::unique_ptr<wkf::Updater> angle =
         wkf::make_updater<wkf::AngleViewUpdater>(QString::fromStdString(name), platformName, currentEntry);
      auto ou = GetAnglesOverlay();
      if (!ou)
      {
         ou = new wkf::OverlayUpdater(mAnglesAttachmentPtr->GetViewer(), angle, QString::fromStdString(platformName), 1, 1);
      }
      else
      {
         ou->AddUpdater(std::move(angle));
      }
      OverlayUpdaterPrefObject* po     = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
      QColor                    opaque = currentEntry->GetColor();
      opaque.setAlphaF(1.0F);
      po->AddColor(QString::fromStdString(name), opaque);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::HandleAngleVisibilityClick()
{
   QAction*    senderAction = dynamic_cast<QAction*>(sender());
   std::string entryId      = senderAction->data().toString().toStdString();
   if (mAnglesAttachmentPtr)
   {
      mAnglesAttachmentPtr->ToggleEntryVisibility(entryId);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::HandleOverlayVisibilityClick()
{
   auto ou = GetAnglesOverlay();
   if (ou)
   {
      ou->SetVisible(!ou->IsVisible());
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::HandleAngleFillClick()
{
   QAction*    senderAction = dynamic_cast<QAction*>(sender());
   std::string entryId      = senderAction->data().toString().toStdString();
   mAnglesAttachmentPtr->ToggleEntryFill(entryId);
}

void wkf::ImmersiveViewDockWidget::VectorManager::HandleOverlaySync()
{
   auto ou = GetAnglesOverlay();

   if (ou)
   {
      ou->ClearUpdaters();
      if (mAnglesAttachmentPtr)
      {
         const std::string platformName = mAnglesAttachmentPtr->GetParent().GetName();
         for (auto& angleEntry : mAnglesAttachmentPtr->GetEntries())
         {
            if (angleEntry.second.IsAngleVisible())
            {
               std::unique_ptr<wkf::Updater> angle =
                  wkf::make_updater<wkf::AngleViewUpdater>(QString::fromStdString(angleEntry.second.GetName()),
                                                           platformName,
                                                           &angleEntry.second);
               ou->AddUpdater(std::move(angle));
               OverlayUpdaterPrefObject* po     = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
               QColor                    opaque = angleEntry.second.GetColor();
               opaque.setAlphaF(1.0F);
               po->AddColor(QString::fromStdString(angleEntry.second.GetName()), opaque);
            }
         }
      }
   }
   else
   {
      if (mAnglesAttachmentPtr)
      {
         const std::string platformName = mAnglesAttachmentPtr->GetParent().GetName();
         for (auto& angleEntry : mAnglesAttachmentPtr->GetEntries())
         {
            if (angleEntry.second.IsAngleVisible())
            {
               std::unique_ptr<wkf::Updater> angle =
                  wkf::make_updater<wkf::AngleViewUpdater>(QString::fromStdString(angleEntry.second.GetName()),
                                                           platformName,
                                                           &angleEntry.second);
               if (!ou) // check again because second round will not need to instantiate it, unfortunately the
                        // constructor as it stands requires at leas one...
               {
                  ou = new wkf::OverlayUpdater(mAnglesAttachmentPtr->GetViewer(),
                                               angle,
                                               QString::fromStdString(platformName),
                                               1,
                                               1);
               }
               ou->AddUpdater(std::move(angle));
               OverlayUpdaterPrefObject* po     = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
               QColor                    opaque = angleEntry.second.GetColor();
               opaque.setAlphaF(1.0F);
               po->AddColor(QString::fromStdString(angleEntry.second.GetName()), opaque);
            }
         }
      }
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::ShowVector(int aVector, bool aState)
{
   if (aState && !mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr = dynamic_cast<vespa::VaAttachmentVectors*>(mParentWidget->AddAttachment("vectors"));
      if (mVectorAttachmentPtr)
      {
         // This may look funky to those new to the pattern. This just makes sure a method can be used in a
         // functor/functions place.
         std::function<void(int, UtVec3f)> fn =
            std::bind(&VectorManager::PositionUpdated, this, std::placeholders::_1, std::placeholders::_2);
         mVectorAttachmentPtr->SubscribeToPositionUpdate(fn);
      }
   }
   if (mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr->ShowVector(aVector, aState);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::AddPlatform(wkf::Platform* aPlatformPtr)
{
   // if we should point to it, point to it
   std::set<wkf::PlatformGrouping> cat = wkfEnv.GetGroupings(aPlatformPtr->GetIndex()); // get the new platforms groupings
   for (auto& it : cat)
   {
      auto jt = mGroupingVectors.find(it);
      if (jt != mGroupingVectors.end()) // if a grouping is something we are already handling
      {
         mVectorAttachmentPtr->AddVectorToEntity(jt->second, aPlatformPtr); // add the platform to the vector set
      }
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::ShowBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   // Create vector attachment if not already created
   if (!mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr = dynamic_cast<vespa::VaAttachmentVectors*>(mParentWidget->AddAttachment("vectors"));
      std::function<void(int, UtVec3f)> fn =
         std::bind(&VectorManager::PositionUpdated, this, std::placeholders::_1, std::placeholders::_2);
      mVectorAttachmentPtr->SubscribeToPositionUpdate(fn);
   }
   mVectorAttachmentPtr->ShowBoresight(aLabel, aAzimuth, aElevation);
}

void wkf::ImmersiveViewDockWidget::VectorManager::UpdateBoresight(const std::string& aLabel, double aAzimuth, double aElevation)
{
   if (mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr->UpdateBoresight(aLabel, aAzimuth, aElevation);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::HideBoresight(const std::string& aLabel)
{
   if (mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr->HideBoresight(aLabel);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::ConnectionEstablished()
{
   for (auto& it : mVectorState)
   {
      if (it.second)
      {
         ShowVector(it.first, it.second);
      }
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::EntityRemovedCB(vespa::VaEntity* aEntityPtr)
{
   wkf::Platform* entityPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
   if (entityPtr != nullptr)
   {
      RemovePlatform(entityPtr);
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::PlatformToBeDeleted(wkf::Platform* aPlatformPtr)
{
   RemovePlatform(aPlatformPtr);
}

UtVec3f wkf::ImmersiveViewDockWidget::VectorManager::GetVectorPosition(int aVectorId) const
{
   if (mVectorAttachmentPtr)
   {
      return mVectorAttachmentPtr->GetVectorPosition(aVectorId);
   }
   return UtVec3f{};
}

float wkf::ImmersiveViewDockWidget::VectorManager::GetVectorScale(int aVectorId) const
{
   if (mVectorAttachmentPtr)
   {
      return mVectorAttachmentPtr->GetVectorScale(aVectorId);
   }
   return -1;
}

void wkf::ImmersiveViewDockWidget::VectorManager::PositionUpdated(int aVectorId, UtVec3f aNewPosition)
{
   if (mAnglesAttachmentPtr)
   {
      mAnglesAttachmentPtr->UpdatePositionContainingVector(aVectorId, aNewPosition);
   }
}

bool wkf::ImmersiveViewDockWidget::VectorManager::IsMultiVectorEntry(int aVectorId) const
{
   // 0 is the static id of FrameBody, the only multivector that is not covered by the logic below.
   if (aVectorId == 0)
      return true;
   if (mVectorAttachmentPtr)
   {
      const std::string name = mVectorAttachmentPtr->GetCategoryText(aVectorId);
      if (name.find(" = ") != std::string::npos)
      {
         return true;
      }
   }
   return false;
}

void wkf::ImmersiveViewDockWidget::VectorManager::InstantiateAngleAttachment()
{
   if (!mAnglesAttachmentPtr)
   {
      mAnglesAttachmentPtr = dynamic_cast<vespa::VaAttachmentAngles*>(mParentWidget->AddAttachment("angles"));
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::RemovePlatform(wkf::Platform* aPlatformPtr)
{
   if (mVectorAttachmentPtr)
   {
      std::set<wkf::PlatformGrouping> cat = wkfEnv.GetGroupings(aPlatformPtr->GetIndex()); // get the platform's groupings
      for (auto& it : cat)
      {
         auto jt = mGroupingVectors.find(it);
         if (jt != mGroupingVectors.end()) // if a grouping is something we are already handling
         {
            mVectorAttachmentPtr->RemoveVectorToEntity(jt->second, aPlatformPtr); // add the platform to the vector set
         }
      }

      wkf::PlatformGrouping inst(wkf::PlatformGrouping::eIndividual, aPlatformPtr->GetName());

      auto jt = mGroupingVectors.find(inst);
      if (jt != mGroupingVectors.end()) // if a grouping is something we are already handling
      {
         mVectorAttachmentPtr->RemoveVectorToEntity(jt->second, aPlatformPtr); // add the platform to the vector set
      }
   }
}

void wkf::ImmersiveViewDockWidget::VectorManager::AddVectorSet(const wkf::PlatformGrouping& aGrouping,
                                                               const std::set<int>&         aPlatformSet,
                                                               const QColor&                aColor)
{
   if (!mVectorAttachmentPtr)
   {
      mVectorAttachmentPtr = dynamic_cast<vespa::VaAttachmentVectors*>(mParentWidget->AddAttachment("vectors"));
      if (mVectorAttachmentPtr)
      {
         std::function<void(int, UtVec3f)> fn =
            std::bind(&VectorManager::PositionUpdated, this, std::placeholders::_1, std::placeholders::_2);
         mVectorAttachmentPtr->SubscribeToPositionUpdate(fn);
      }
   }

   if (mVectorAttachmentPtr)
   {
      if (mGroupingVectors.find(aGrouping) == mGroupingVectors.end())
      {
         int categoryCount = mVectorAttachmentPtr->GetCategoryCount();

         std::string label;
         if (aGrouping.Type() < wkf::PlatformGrouping::eGroupingTypeCount)
         {
            label += aGrouping.TypeString();
            label += " = ";
         }
         label += aGrouping.Name();

         int index =
            mVectorAttachmentPtr->AddVector(label, aColor.redF(), aColor.greenF(), aColor.blueF(), aColor.alphaF());
         mGroupingVectors[aGrouping] = index;

         auto* scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr)
         {
            for (auto& it : aPlatformSet)
            {
               vespa::VaEntity* ent = scenarioPtr->FindPlatformByIndex(it);
               if (ent)
               {
                  mVectorAttachmentPtr->AddVectorToEntity(index, ent);
                  mVectorState[categoryCount] = true;
               }
            }
         }
      }
   }
}

wkf::ImmersiveViewDockWidget::VectorManager::AngleInfo wkf::ImmersiveViewDockWidget::VectorManager::RunVisualizerDialogue()
{
   // Build Dialogue
   std::unique_ptr<QDialog> vectorSelectionDialogue = ut::make_unique<QDialog>(new QDialog(mParentWidget));
   vectorSelectionDialogue->setWindowTitle("Visualize Angle");
   QVBoxLayout* layout = new QVBoxLayout(vectorSelectionDialogue.get());

   QComboBox* vectorListOne           = new QComboBox(vectorSelectionDialogue.get());
   QComboBox* vectorListTwo           = new QComboBox(vectorSelectionDialogue.get());
   bool       atLeastOneVectorVisible = false;
   if (mVectorAttachmentPtr) // should always be true.
   {
      for (auto vec : mVectorState)
      {
         if (vec.second)
         {
            // Although AttachmentVectors supports multiple vectors per shape, we do not for angles. It wouldnt be useful.
            if (IsMultiVectorEntry(vec.first))
               continue;
            atLeastOneVectorVisible = true;
            vectorListOne->addItem(mVectorAttachmentPtr->GetCategoryText(vec.first).c_str(), vec.first);
            vectorListTwo->addItem(mVectorAttachmentPtr->GetCategoryText(vec.first).c_str(), vec.first);
         }
      }
      for (const auto& vecName : mParentWidget->mShownBoresights)
      {
         atLeastOneVectorVisible = true;
         vectorListOne->addItem(QString::fromStdString(vecName), mVectorAttachmentPtr->GetBoresightId(vecName));
         vectorListTwo->addItem(QString::fromStdString(vecName), mVectorAttachmentPtr->GetBoresightId(vecName));
      }
   }
   layout->addWidget(new QLabel("Vector 1:", vectorSelectionDialogue.get()));
   layout->addWidget(vectorListOne);
   layout->addWidget(new QLabel("Vector 2:", vectorSelectionDialogue.get()));
   layout->addWidget(vectorListTwo);

   layout->addWidget(new QLabel("Select Angle Color", vectorSelectionDialogue.get()));
   UtQtColorButton* colorButton = new UtQtColorButton(vectorSelectionDialogue.get());
   colorButton->SetColor(QColor{255, 0, 0, 64}); // default to Red with alpha to 64
   colorButton->ShowAlpha(true);
   layout->addWidget(colorButton);

   QDialogButtonBox* dialogButtonBox = nullptr;
   // We might be able to do this better, perhaps not having the menu in the context at all if no angles are visualized.
   // My thought is thats not very discoverable though. Ill get the opinions of others.
   if (atLeastOneVectorVisible)
   {
      dialogButtonBox =
         new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, vectorSelectionDialogue.get());
   }
   else
   {
      dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, vectorSelectionDialogue.get());
   }

   layout->addWidget(dialogButtonBox);
   connect(dialogButtonBox, &QDialogButtonBox::accepted, vectorSelectionDialogue.get(), &QDialog::accept);
   connect(dialogButtonBox, &QDialogButtonBox::rejected, vectorSelectionDialogue.get(), &QDialog::reject);
   if (QDialog::Accepted == vectorSelectionDialogue->exec())
   {
      QColor color = colorButton->GetColor();
      int    vec1  = qvariant_cast<int>(vectorListOne->itemData(vectorListOne->currentIndex()));
      int    vec2  = qvariant_cast<int>(vectorListTwo->itemData(vectorListTwo->currentIndex()));
      return std::make_tuple(vec1, vec2, color);
   }
   return std::make_tuple(-1, -1, QColor{});
}

wkf::OverlayUpdater* wkf::ImmersiveViewDockWidget::VectorManager::GetAnglesOverlay() const
{
   return mAnglesAttachmentPtr->GetViewer()->FindFirstOverlayOfType<wkf::OverlayUpdater>();
}
