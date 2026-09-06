// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfSatelliteTetherDockWidget.hpp"

#include <osg/Uniform>

#include "UtMath.hpp"
#include "UtoViewer.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotion.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "WkfPlatform.hpp"
#include "WkfUtils.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::SatelliteTetherDockWidget::SatelliteTetherDockWidget(const QString&  aObjectName,
                                                          const QString&  aPlatformName,
                                                          unsigned int    aId,
                                                          QMainWindow*    aParent /*= nullptr*/,
                                                          Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ImmersiveViewDockWidget(aObjectName, aPlatformName, aParent, aFlags)
   , mId(aId)
{
   setAttribute(Qt::WA_DeleteOnClose);
   show();
}

wkf::SatelliteTetherDockWidget::~SatelliteTetherDockWidget()
{
   emit CleanUp(mId);
}

void wkf::SatelliteTetherDockWidget::BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos)
{
   if (mPlatformPtr)
   {
      const QIcon& icon   = mCartesianGrid ? QIcon::fromTheme("grid") : QIcon::fromTheme("rangering");
      QAction*     ICGrid = new QAction(icon, "Show IC Grid", &aMenu);
      connect(ICGrid, &QAction::triggered, [this]() { SetAlignment(AttachmentSatelliteTetherGrid::IC); });
      QAction* IRGrid = new QAction(icon, "Show IR Grid", &aMenu);
      connect(IRGrid, &QAction::triggered, [this]() { SetAlignment(AttachmentSatelliteTetherGrid::IR); });
      QAction* RCGrid = new QAction(icon, "Show RC Grid", &aMenu);
      connect(RCGrid, &QAction::triggered, [this]() { SetAlignment(AttachmentSatelliteTetherGrid::RC); });
      QAction* noGrid = new QAction(icon, "No Grid", &aMenu);
      connect(noGrid, &QAction::triggered, [this]() { SetAlignment(AttachmentSatelliteTetherGrid::NONE); });
      aMenu.addSeparator()->setText("Grid Alignment");
      aMenu.addAction(ICGrid);
      aMenu.addAction(IRGrid);
      aMenu.addAction(RCGrid);
      aMenu.addAction(noGrid);

      QAction* cartesian = new QAction(QIcon::fromTheme("grid"), "Cartesian Grid", &aMenu);
      connect(cartesian, &QAction::triggered, [this]() { SetCartesian(true); });
      QAction* radial = new QAction(QIcon::fromTheme("rangering"), "Radial Grid", &aMenu);
      connect(radial, &QAction::triggered, [this]() { SetCartesian(false); });
      aMenu.addSeparator()->setText("Grid Type");
      aMenu.addAction(cartesian);
      aMenu.addAction(radial);
      aMenu.addSeparator();

      QMenu* trackMenu   = aMenu.addMenu(QIcon::fromTheme("satellite"), "Add Track");
      auto*  scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         auto platMap = scenarioPtr->GetIndexPlatformMap();
         for (auto&& plat : platMap)
         {
            wkf::Platform* platptr = plat.second;
            if (platptr->GetSpatialDomain() == wkf::SPATIAL_DOMAIN_SPACE)
            {
               if (platptr->GetUniqueId() != mPlatformPtr->GetUniqueId())
               {
                  if (!HaveTrack(platptr->GetUniqueId()))
                  {
                     QAction* platAct = new QAction(platptr->GetName().c_str(), trackMenu);
                     trackMenu->addAction(platAct);
                     connect(platAct, &QAction::triggered, [this, platptr]() { AddTrack(platptr); });
                     // if we port to warlock we need to check platforms for visibility here
                  }
               }
            }
         }
      }
      if (trackMenu->isEmpty())
      {
         delete trackMenu;
      }
      if (!mTrackList.empty())
      {
         QMenu* tether = aMenu.addMenu("Tether");
         if (mTetherId != mPlatformPtr->GetUniqueId())
         {
            connect(tether->addAction(QString("Tether to %1 (origin)").arg(mPlatformPtr->GetName().c_str())),
                    &QAction::triggered,
                    [this]() { TetherTo(*mPlatformPtr); });
         }
         for (auto&& track : mTrackList)
         {
            if (mTetherId != track.first->GetUniqueId())
            {
               vespa::VaEntity* ent = track.first;
               connect(tether->addAction(QString("Tether to %1").arg(ent->GetName().c_str())),
                       &QAction::triggered,
                       [this, ent]() { TetherTo(*ent); });
            }
         }
         QAction* removeTrack = new QAction("Clear Tracks", &aMenu);
         connect(removeTrack, &QAction::triggered, this, &SatelliteTetherDockWidget::ClearTracks);
         aMenu.addAction(removeTrack);
      }
      aMenu.addSeparator();
   }
}

void wkf::SatelliteTetherDockWidget::SetLockToHeading(bool aState)
{
   if (aState && !GetLockToHeading())
   {
      double az = mViewerPtr->GetCamera()->GetCameraMotion()->GetAz();
      ImmersiveViewDockWidget::SetLockToHeading(true);
      double heading = mPlatformPtr ? mPlatformPtr->GetHeading() : 0.0;
      mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(az - UtMath::cDEG_PER_RAD * heading);
   }
   else
   {
      ImmersiveViewDockWidget::SetLockToHeading(aState);
   }
}

void wkf::SatelliteTetherDockWidget::DataUpdate()
{
   for (auto&& track : mTrackList)
   {
      AttachmentSatelliteTetherTrace* trace = track.second.second;
      UpdateTrace(trace);
   }
}

void wkf::SatelliteTetherDockWidget::ShowNames(bool aState)
{
   mShowNames = aState;

   for (auto&& track : mTrackList)
   {
      track.second.first->ShowNames(aState);
   }
}

void wkf::SatelliteTetherDockWidget::ShowFutureTracks(bool aState)
{
   mShowFutureTracks = aState;

   for (auto&& track : mTrackList)
   {
      track.second.second->ShowFutureTracks(aState);
   }
}

void wkf::SatelliteTetherDockWidget::EnableLighting(bool aState)
{
   mViewerPtr->EnableLighting(aState);
}

bool wkf::SatelliteTetherDockWidget::HaveTrack(const unsigned int aUniqueId) const
{
   for (auto&& track : mTrackList)
   {
      if (track.first->GetUniqueId() == aUniqueId)
      {
         return true;
      }
   }
   return false;
}

void wkf::SatelliteTetherDockWidget::ViewerInitializedPrivate()
{
   // anything we add to the viewer that needs to compare depth should accommodate the logarithmic depth buffer
   // we'll use the "VA_LogDepth" uniform to communicate the need to shaders
   // the shader header "logDepth.h" does the conversion and should be applied after multiplying the projection matrix
   // The maps handle this with an osgEarth::VirtualProgram (see UtoCmeTerrainImp)
   // see modelinst_base.vert or wingribbon.geom for attachment examples
   mViewerPtr->SetUniform("VA_LogDepth", 1.0f);
   mViewerPtr->GetViewer()->GetOSGViewer()->getCamera()->getOrCreateStateSet()->addUniform(
      new osg::Uniform("VA_LogDepth", 1.0f));
   SetLockToHeading(true);

   // move the earth to the celestial sphere
   mViewerPtr->GetViewer()->CopyTerrainToCelestial();
}

void wkf::SatelliteTetherDockWidget::ConnectionEstablishedPrivate()
{
   mCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&SatelliteTetherDockWidget::EntityMovedCB, this, mPlatformPtr->GetUniqueId()));
   mViewerPtr->GetCamera()->GetCameraMotion()->SetDist(1500);
   mViewerPtr->GetViewer()->EnableSmallFeatureCulling(false);
   mTetherId = mPlatformPtr->GetUniqueId();
}

void wkf::SatelliteTetherDockWidget::ClearDataPrivate()
{
   ClearTracks();
   ClearGrid();
}

void wkf::SatelliteTetherDockWidget::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aInTime)
{
   UtVec3d  v = aEntityPtr->GetVelocityWCS();
   UtEntity ent;
   double   xyz[3];
   // this magic number (date?) is used to make the UtEntity use an earth angle of 0 for ECI conversions
   // I was told to use the vernal equinox, but the time did not result in an angle of 0
   UtCalendar vequ;
   vequ.SetDate(2019, 3, 20);
   vequ.SetTime(43742.394);
   ent.SetECI_ReferenceEpoch(vequ);
   aEntityPtr->GetPosition().GetECEF(xyz);
   ent.SetLocationWCS(xyz);
   ent.SetVelocityWCS(v.GetData());
   ent.GetVelocityTOD(v.GetData());
   v.Normalize();
   UtVec3d r(xyz[0], xyz[1], xyz[2]);
   r.Normalize();
   UtVec3d c;
   c.CrossProduct(r, v);
   c.Normalize();
   UtVec3d i;
   i.CrossProduct(c, r);
   i.Normalize();
   osg::Matrix mat(r[0], r[1], r[2], 0.0, i[0], i[1], i[2], 0.0, c[0], c[1], c[2], 0.0, 0.0, 0.0, 0.0, 1.0);

   double vned[3];
   ent.SetVelocityWCS(i.GetData());
   ent.GetVelocityNED(vned);
   mHeading = atan2(vned[0], vned[1]) *
              UtMath::cDEG_PER_RAD; // we use this to get an appropriate heading vector for camera purposes

   for (auto&& track : mTrackList)
   {
      track.second.second->UpdateTransform(mat);
   }
   if (mGridPtr)
   {
      mGridPtr->UpdateTransform(mat);
      AdjustCamera();
   }
}

void wkf::SatelliteTetherDockWidget::UpdateColor(wkf::Platform* aPlatformPtr, const QColor& aColor)
{
   if (HaveTrack(aPlatformPtr->GetUniqueId()))
   {
      mTrackList[aPlatformPtr].first->SetColor(aColor);
      mTrackList[aPlatformPtr].first->ShowNames(mShowNames);
      mTrackList[aPlatformPtr].second->SetColor(aColor);
   }
   mPlatformColorList[aPlatformPtr->GetName()] = aColor;
}

std::vector<std::string> wkf::SatelliteTetherDockWidget::GetTrackedPlatforms() const
{
   std::vector<std::string> trackedPlatforms;
   for (const auto& platform : mPlatformColorList)
   {
      trackedPlatforms.push_back(platform.first);
   }
   return trackedPlatforms;
}

void wkf::SatelliteTetherDockWidget::AddTrack(wkf::Platform* aPlatformPtr)
{
   AddTrackToDock(aPlatformPtr);
}

void wkf::SatelliteTetherDockWidget::ClearTracks()
{
   if (mPlatformPtr != nullptr)
   {
      if (mTetherId != mPlatformPtr->GetUniqueId())
      {
         TetherTo(*mPlatformPtr);
      }
      for (auto&& track : mTrackList)
      {
         track.second.first->GetParent().RemoveAttachment(track.second.first->GetUniqueId());
         track.second.second->GetParent().RemoveAttachment(track.second.second->GetUniqueId());
      }
      mTrackList.clear();
   }
}

void wkf::SatelliteTetherDockWidget::ClearGrid()
{
   if (mGridPtr)
   {
      if (mPlatformPtr != nullptr)
      {
         mPlatformPtr->RemoveAttachment(mGridPtr->GetUniqueId());
      }
      mGridPtr = nullptr;
   }
}

QColor wkf::SatelliteTetherDockWidget::GetTrackColor(const std::string& aPlatform)
{
   QColor color;
   auto   it = mPlatformColorList.find(aPlatform);
   if (it != mPlatformColorList.end())
   {
      color = it->second;
   }
   else
   {
      color = wkf::util::GetHashColor(QString::fromStdString(aPlatform));
      mPlatformColorList.emplace(aPlatform, color);
   }
   return color;
}

void wkf::SatelliteTetherDockWidget::AddTrackToDock(wkf::Platform* aPlatformPtr)
{
   if (mPlatformPtr)
   {
      QColor color = GetTrackColor(aPlatformPtr->GetName());
      mTrackList[aPlatformPtr] =
         std::make_pair(vespa::make_attachment<AttachmentSatelliteTetherTrack>(*aPlatformPtr, *mViewerPtr, color),
                        MakeTrace(*mPlatformPtr, *mViewerPtr, *aPlatformPtr, color));

      mTrackList[aPlatformPtr].first->ShowNames(mShowNames);
      mTrackList[aPlatformPtr].second->ShowFutureTracks(mShowFutureTracks);
      mCallbacks.Add(vespa::VaObserver::EntityRemoved.Connect(&SatelliteTetherDockWidget::TrackedEntityDeleted,
                                                              this,
                                                              aPlatformPtr->GetUniqueId()));
   }
}

void wkf::SatelliteTetherDockWidget::UpdateTrace(wkf::AttachmentSatelliteTetherTrace* aTracePtr)
{
   aTracePtr->UpdateData();
}

void wkf::SatelliteTetherDockWidget::TetherTo(vespa::VaEntity& aEntity)
{
   mViewerPtr->GetCamera()->Tether(&aEntity);
   mViewerPtr->GetCamera()->GetCameraMotion()->SetSoftLock(false);
   mTetherId = aEntity.GetUniqueId();
   SetLockToHeading(true);
}

void wkf::SatelliteTetherDockWidget::SetAlignment(const AttachmentSatelliteTetherGrid::Alignment& aAlignment)
{
   if (aAlignment == AttachmentSatelliteTetherGrid::NONE)
   {
      ClearGrid();
      mViewerPtr->GetCamera()->GetCameraMotion()->SetSoftLock(false);
      SetLockToHeading(true);
   }
   else if (mPlatformPtr != nullptr)
   {
      if (!mGridPtr)
      {
         mGridPtr = vespa::make_attachment<AttachmentSatelliteTetherGrid>(*mPlatformPtr, *mViewerPtr);
         mGridPtr->SetCartesian(mCartesianGrid);
      }
      mGridPtr->SetAlignment(aAlignment);
      mViewerPtr->GetCamera()->GetCameraMotion()->SetSoftLock(true,
                                                              vespa::VaCameraMotion::cZOOM,
                                                              vespa::VaCameraMotion::cPRIMARY_ROTATION |
                                                                 vespa::VaCameraMotion::cSECONDARY_ROTATION);
      SetLockToHeading(false);

      switch (mGridPtr->GetAlignment())
      {
      case (AttachmentSatelliteTetherGrid::IC):
         mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-mHeading);
         mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(90);
         break;
      case (AttachmentSatelliteTetherGrid::IR):
         mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-mHeading);
         mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(0);
         break;
      case (AttachmentSatelliteTetherGrid::RC):
         mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-90 - mHeading);
         mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(0);
         break;
      default:
         break;
      }
   }
}

void wkf::SatelliteTetherDockWidget::SetCartesian(bool aState)
{
   if (mCartesianGrid != aState)
   {
      mCartesianGrid = aState;
      if (mGridPtr)
      {
         mGridPtr->SetCartesian(mCartesianGrid);
      }
   }
}

void wkf::SatelliteTetherDockWidget::AdjustCamera()
{
   if (mGridPtr)
   {
      if (mViewerPtr->GetCamera()->GetCameraMotion()->SoftLock())
      {
         switch (mGridPtr->GetAlignment())
         {
         case (AttachmentSatelliteTetherGrid::IC):
            mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-mHeading);
            mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(90);
            break;
         case (AttachmentSatelliteTetherGrid::IR):
            mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-mHeading);
            mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(0);
            break;
         case (AttachmentSatelliteTetherGrid::RC):
            mViewerPtr->GetCamera()->GetCameraMotion()->SetAz(-90 - mHeading);
            mViewerPtr->GetCamera()->GetCameraMotion()->SetEl(0);
            break;
         default:
            break;
         }
      }
      else
      {
         if (!GetLockToHeading())
         {
            SetLockToHeading(true);
         }
      }
   }
}

void wkf::SatelliteTetherDockWidget::TrackedEntityDeleted(vespa::VaEntity* aEntityPtr)
{
   if (mTetherId == aEntityPtr->GetUniqueId())
   {
      if (mPlatformPtr)
      {
         TetherTo(*mPlatformPtr); // revert to the reference entity
      }
   }
   // the attachments take care of themselves
   mTrackList.erase(aEntityPtr);
}
