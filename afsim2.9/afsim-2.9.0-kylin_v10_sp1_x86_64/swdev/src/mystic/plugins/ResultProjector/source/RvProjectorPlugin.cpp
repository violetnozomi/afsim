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
#include "RvProjectorPlugin.hpp"

#include <QMenu>

#include "RvEnvironment.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "VaCamera.hpp"
#include "VaObserver.hpp"
#include "VaOverlayTexture.hpp"
#include "VaWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfUtils.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvProjector::Plugin, "Projector", "Draws projections onto terrain from platforms", "mystic")

RvProjector::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
}

void RvProjector::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      QString        name = QString::fromStdString(aEntityPtr->GetName());
      wkf::Platform* plat = dynamic_cast<wkf::Platform*>(aEntityPtr);

      // 1. look up the rv::ResultPlatform
      rv::ResultPlatform* resPlat = rvEnv.GetData()->FindPlatform(plat->GetIndex());
      if (resPlat)
      {
         double simTime = rvEnv.GetData()->GetSimTime();
         resPlat->BeginRead();
         auto sparts = resPlat->GetParts(rv::PartType::sensor);
         if (sparts)
         {
            for (auto& sensor : *sparts)
            {
               auto&& platProj = mProjectorMap.find(name);
               if ((platProj == mProjectorMap.end()) ||
                   (platProj->second.find(QString::fromStdString(sensor.first)) == platProj->second.end()))
               {
                  auto statusMsg = sensor.second.FindFirstBefore<rv::MsgPartStatus>(simTime);
                  if (0 != statusMsg)
                  {
                     if (statusMsg->on() && !statusMsg->disabled() && !statusMsg->broken())
                     {
                        QString     sensorName = QString::fromStdString(statusMsg->partName());
                        std::string modeName   = "default";
                        const auto* modeCh     = sensor.second.FindFirstBefore<rv::MsgSensorModeChange>(simTime);
                        if (modeCh != nullptr)
                        {
                           modeName = modeCh->modeName();
                        }

                        const rv::MsgEmitterModeDefinition* mode =
                           resPlat->FindSensorMode(modeName, statusMsg->partName());
                        if (mode != 0)
                        {
                           if (!mode->beamList().empty())
                           {
                              auto&& beam = mode->beamList().begin();
                              if (!beam->fov().points().empty() && (beam->fov().shape() == 0))
                              {
                                 float minaz = beam->fov().points()[0].x();
                                 float minel = beam->fov().points()[0].y();
                                 float maxaz = beam->fov().points()[1].x();
                                 float maxel = beam->fov().points()[1].y();
                                 if ((-minaz == maxaz) && (-minel == maxel) && (maxaz - minaz <= UtMath::cPI * 0.75) &&
                                     (maxel - minel <= UtMath::cPI * 0.75))
                                 {
                                    // this is one we can handle
                                    QAction* projectorAction =
                                       new QAction(QString("Add Projector to %1 : %2").arg(name, sensorName), aMenu);
                                    aMenu->addAction(projectorAction);
                                    connect(projectorAction,
                                            &QAction::triggered,
                                            this,
                                            [=]() { CreateProjector(name, sensorName); });
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
         resPlat->EndRead();
      }
      auto&& sensorList = mProjectorMap.find(name);
      if (sensorList != mProjectorMap.end())
      {
         for (auto&& projector : sensorList->second)
         {
            QAction* remAction = new QAction(QString("Remove Projector from %1 : %2").arg(name, projector.first), aMenu);
            aMenu->addAction(remAction);
            connect(remAction, &QAction::triggered, this, [=]() { RemoveProjector(name, projector.first); });
         }
      }
   }
}

void RvProjector::Plugin::CreateProjector(const QString& aPlatformName, const QString& aSensorName)
{
   wkf::Viewer* mainViewer = vaEnv.GetStandardViewer();
   if (mainViewer)
   {
      vespa::VaEntity* ent = vaEnv.FindEntity(aPlatformName.toStdString());
      if (ent)
      {
         UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(mainViewer->GetDatabase());
         if (terrain != nullptr)
         {
            std::string  name  = QString(aPlatformName + "_" + aSensorName).toStdString();
            UtColor      color = wkf::util::GetHashUtColor(name);
            unsigned int id    = terrain->AddProjector(name, color);
            if (id != 0)
            {
               mProjectorMap[aPlatformName][aSensorName] = id;
            }
         }
      }
   }
}

void RvProjector::Plugin::RemoveProjector(const QString& aPlatformName, const QString& aSensorName)
{
   auto&& platProj = mProjectorMap.find(aPlatformName);
   if (platProj != mProjectorMap.end())
   {
      auto&& sensProj = platProj->second.find(aSensorName);
      if (sensProj != platProj->second.end())
      {
         wkf::Viewer* mainViewer = vaEnv.GetStandardViewer();
         if (mainViewer)
         {
            UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(mainViewer->GetDatabase());
            if (terrain != nullptr)
            {
               terrain->RemoveProjector(sensProj->second);
            }
         }

         platProj->second.erase(sensProj);
      }
      if (platProj->second.empty())
      {
         mProjectorMap.erase(platProj);
      }
   }
}

void RvProjector::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   for (auto&& projplat : mProjectorMap)
   {
      wkf::Platform*      plat    = vaEnv.GetStandardScenario()->FindPlatformByName(projplat.first.toStdString());
      rv::ResultPlatform* resPlat = aData.FindPlatform(plat->GetIndex());

      auto statePtr = resPlat->FindFirstBefore<rv::MsgEntityState>(aData.GetSimTime());
      if (statePtr != nullptr)
      {
         for (auto&& projsens : projplat.second)
         {
            auto sparts = resPlat->GetParts(rv::PartType::sensor);
            if (sparts)
            {
               const rv::MsgPartArticulationStatus* art    = nullptr;
               auto                                 sensor = sparts->find(projsens.first.toStdString());
               if (sensor != sparts->end())
               {
                  art = sensor->second.FindFirstBefore<rv::MsgPartArticulationStatus>(aData.GetSimTime());
               }

               if (art != nullptr)
               {
                  std::string modeName = "default";
                  const auto* modeCh   = sensor->second.FindFirstBefore<rv::MsgSensorModeChange>(aData.GetSimTime());
                  if (modeCh != nullptr)
                  {
                     modeName = modeCh->modeName();
                  }

                  const rv::MsgEmitterModeDefinition* mode =
                     resPlat->FindSensorMode(modeName, projsens.first.toStdString());
                  if (mode != nullptr)
                  {
                     if (!mode->beamList().empty())
                     {
                        if (mode->beamList().begin()->fov().shape() == 0)
                        {
                           float vfov = mode->beamList().begin()->fov().points()[1].y() -
                                        mode->beamList().begin()->fov().points()[0].y();
                           float aspect = (mode->beamList().begin()->fov().points()[1].x() -
                                           mode->beamList().begin()->fov().points()[0].x()) /
                                          vfov;
                           // find the active mode
                           // find the articulation
                           wkf::Viewer* mainViewer = vaEnv.GetStandardViewer();
                           if (mainViewer)
                           {
                              UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(mainViewer->GetDatabase());
                              if (terrain)
                              {
                                 osg::Matrix modelMatrix;
                                 rv::Vec3d   vec = statePtr->state().locationWCS();
                                 rv::Vec3f   ori = statePtr->state().orientationWCS();
                                 UtEntity    ent;
                                 ent.SetLocationWCS(rv::toVec3(vec).GetData());
                                 ent.SetOrientationWCS(ori.x(), ori.y(), ori.z());
                                 double lla[3];
                                 ent.GetLocationLLA(lla[0], lla[1], lla[2]);
                                 double hpr[3];
                                 ent.GetOrientationNED(hpr[0], hpr[1], hpr[2]);

                                 modelMatrix = osg::Matrixd::translate(-vec.x(), -vec.y(), -vec.z()) *
                                               osg::Matrixd::rotate(-osg::PI_2, osg::Vec3d(1, 0, 0)) *
                                               osg::Matrixd::rotate(-osg::PI_2, osg::Vec3d(0, 1, 0)) *
                                               osg::Matrixd::rotate(osg::DegreesToRadians(-lla[1]), 0.0, 1.0, 0.0) *
                                               osg::Matrixd::rotate(osg::DegreesToRadians(lla[0]), 1.0, 0.0, 0.0) *
                                               osg::Matrixd::rotate(hpr[0],
                                                                    osg::Vec3d(0.0, 0.0, 1.0), // heading
                                                                    -hpr[1],
                                                                    osg::Vec3d(1.0, 0.0, 0.0), // pitch
                                                                    -hpr[2],
                                                                    osg::Vec3d(0.0, 1.0, 0.0)); // roll

                                 double az;
                                 double el;
                                 double roll;
                                 rv::GetCombinedOrientation(*art, az, el, roll);
                                 osg::Matrix viewMatrix = osg::Matrixd::rotate(az,
                                                                               osg::Vec3(0, 0, 1),
                                                                               -roll,
                                                                               osg::Vec3(0, 1, 0),
                                                                               -el - osg::PI_2,
                                                                               osg::Vec3(1, 0, 0));

                                 osg::Matrix mv = modelMatrix * viewMatrix;
                                 osg::Matrix projMatrix =
                                    osg::Matrixd::perspective(vfov * UtMath::cDEG_PER_RAD, aspect, 100.0, 7000000.0);
                                 terrain->UpdateProjectorMatrix(projsens.second, mv, projMatrix);
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void RvProjector::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}
