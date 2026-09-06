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
#include "RvVaScenarioManagerInterface.hpp"

#include <functional>

#include <QCoreApplication>
#include <QTimer>

#include "RvInterpolationPrefObject.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtQuaternion.hpp"
#include "VaEntity.hpp"
#include "VaScenario.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

RvVaScenarioManager::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mScenarioPtr(nullptr)
   , mSimTime(0)
   , mResetCamera(true)
   , mValidEntityFound(false)
{
   mScenarioPtr = (wkf::Scenario*)vaEnv.NewScenario("wkf");
   mScenarioPtr->SetModifiable(false);
   vaEnv.SetStandardScenario(mScenarioPtr);
}

RvVaScenarioManager::Interface::~Interface()
{
   QMutexLocker locker(&mMutex);
}

void RvVaScenarioManager::Interface::PlatformAdded(rv::ResultDb& aData, const rv::MsgPlatformInfo& aInfo)
{
   QMutexLocker locker(&mMutex);

   rv::ResultPlatform* plat                     = aData.FindPlatform(aInfo.platformIndex());
   mPlatforms[aInfo.platformIndex()]            = plat;
   mPlatformStates[aInfo.platformIndex()].mName = aInfo.name();
   mPlatformStates[aInfo.platformIndex()].mIcon = aInfo.icon();
   mPlatformStates[aInfo.platformIndex()].mSide = aInfo.side();
   switch (aInfo.spatialDomain())
   {
   case (rv::SpatialDomain::air):
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_AIR;
      break;
   case (rv::SpatialDomain::land):
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_LAND;
      break;
   case (rv::SpatialDomain::space):
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_SPACE;
      break;
   case (rv::SpatialDomain::subsurface):
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_SUBSURFACE;
      break;
   case (rv::SpatialDomain::surface):
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_SURFACE;
      break;
   default:
      mPlatformStates[aInfo.platformIndex()].mSpatialDomain = wkf::SPATIAL_DOMAIN_UNKNOWN;
      break;
   }
   mPlatformStates[aInfo.platformIndex()].mTypeList = aInfo.types();
   mPlatformStates[aInfo.platformIndex()].mVisible  = false;


   int index = aInfo.platformIndex();
   wkfEnv.ClearPlatformGroupings(index);

   if (aInfo.categoriesValid())
   {
      for (auto& cit : aInfo.categories())
      {
         wkfEnv.SetPlatformGrouping(index, wkf::PlatformGrouping(wkf::PlatformGrouping::eCategory, cit));
      }
   }
   if (aInfo.sideValid())
   {
      wkfEnv.SetPlatformGrouping(index, wkf::PlatformGrouping(wkf::PlatformGrouping::eSide, aInfo.side()));
   }
   if (aInfo.typesValid()) // consider going to the proxy (when available) instead for more accurate data...
   {
      for (auto& tyit : aInfo.types())
      {
         wkfEnv.SetPlatformGrouping(index, wkf::PlatformGrouping(wkf::PlatformGrouping::eType, tyit));
      }
   }
}

void RvVaScenarioManager::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);
   float        time = aData.GetSimTime();
   mSimTime          = time;

   PlatformMap::iterator it;
   for (it = mPlatforms.begin(); it != mPlatforms.end(); ++it)
   {
      rv::ResultPlatform* plat = it->second;
      plat->BeginRead();
      auto       status        = plat->FindFirstBefore<rv::MsgPlatformStatus>(time);
      AliveState newAliveState = cUNKNOWN;
      if (status)
      {
         if (status->broken())
         {
            newAliveState = cDEAD;
         }
         else if (status->removed())
         {
            newAliveState = cREMOVED;
         }
         else
         {
            newAliveState = cALIVE;
         }
      }
      else if (plat->HasData<rv::MsgPlatformInfo>())
      {
         newAliveState = cALIVE;
      }
      bool removed = newAliveState != cALIVE;

      const rv::MsgEntityState* state = plat->FindFirstBefore<rv::MsgEntityState>(time);
      if (!removed)
      {
         if (state == nullptr)
         {
            removed = true;
         }
      }
      PlatformStateMap::iterator jt = mPlatformStates.find(it->first);
      if (jt != mPlatformStates.end())
      {
         if (jt->second.mVisible == removed)
         {
            if (jt->second.mEntityPtr)
            {
               wkf::Observer::PlatformActiveChanged(jt->second.mEntityPtr, !removed);
            }
         }
         jt->second.mVisible = !removed;
      }
      else
      {
         mPlatformStates[it->first].mVisible = !removed;
      }
      if (!removed && state)
      {
         mValidEntityFound              = true;
         const rv::EntityState& eState  = state->state();
         auto                   nState  = plat->FindFirstAfter<rv::MsgEntityState>(time);
         float                  elapsed = time - state->simTime();
         if (wkfEnv.GetPreferenceObject<rv::InterpolationPrefObject>()->GetPositionalExtrapolation())
         {
            mPlatformStates[it->first].mLocationWCS[0] = eState.locationWCS().x() + elapsed * eState.velocityWCS().x() +
                                                         0.5 * elapsed * elapsed * eState.accelerationWCS().x();
            mPlatformStates[it->first].mLocationWCS[1] = eState.locationWCS().y() + elapsed * eState.velocityWCS().y() +
                                                         0.5 * elapsed * elapsed * eState.accelerationWCS().y();
            mPlatformStates[it->first].mLocationWCS[2] = eState.locationWCS().z() + elapsed * eState.velocityWCS().z() +
                                                         0.5 * elapsed * elapsed * eState.accelerationWCS().z();
         }
         else
         {
            mPlatformStates[it->first].mLocationWCS[0] = eState.locationWCS().x();
            mPlatformStates[it->first].mLocationWCS[1] = eState.locationWCS().y();
            mPlatformStates[it->first].mLocationWCS[2] = eState.locationWCS().z();
         }
         if (nState && wkfEnv.GetPreferenceObject<rv::InterpolationPrefObject>()->GetAngularInterpolation() &&
             (nState->simTime() != state->simTime()))
         {
            float        factor = elapsed / (nState->simTime() - state->simTime());
            UtQuaternion startQ(eState.orientationWCS().x(), eState.orientationWCS().y(), eState.orientationWCS().z());
            UtQuaternion nextQ(nState->state().orientationWCS().x(),
                               nState->state().orientationWCS().y(),
                               nState->state().orientationWCS().z());
            UtQuaternion interpQ = UtQuaternion::Slerp(startQ, nextQ, factor);
            interpQ.Get(mPlatformStates[it->first].mOrientationWCS[0],
                        mPlatformStates[it->first].mOrientationWCS[1],
                        mPlatformStates[it->first].mOrientationWCS[2]);
            // build a quaternion and apply it to the orientation
         }
         else
         {
            mPlatformStates[it->first].mOrientationWCS[0] = eState.orientationWCS().x();
            mPlatformStates[it->first].mOrientationWCS[1] = eState.orientationWCS().y();
            mPlatformStates[it->first].mOrientationWCS[2] = eState.orientationWCS().z();
         }
         mPlatformStates[it->first].mVelocityWCS[0]     = eState.velocityWCS().x();
         mPlatformStates[it->first].mVelocityWCS[1]     = eState.velocityWCS().y();
         mPlatformStates[it->first].mVelocityWCS[2]     = eState.velocityWCS().z();
         mPlatformStates[it->first].mAccelerationWCS[0] = eState.accelerationWCS().x();
         mPlatformStates[it->first].mAccelerationWCS[1] = eState.accelerationWCS().y();
         mPlatformStates[it->first].mAccelerationWCS[2] = eState.accelerationWCS().z();
      }
      // identify active parts / disabled parts and add or remove as appropriate apply articulations
      auto vparts = plat->GetParts(rv::PartType::visual);
      if (vparts != nullptr)
      {
         for (auto& visualpart : *vparts)
         {
            const rv::MsgPartStatus* statusMsg = visualpart.second.FindFirstBefore<rv::MsgPartStatus>(mSimTime);
            if (statusMsg)
            {
               // this may not work in cases where the on is prior to our window and we reenter the on period (or vice-versa)
               if (statusMsg->on() && !statusMsg->disabled() && !statusMsg->broken())
               {
                  const rv::MsgVisualPartDefinition* def =
                     visualpart.second.FindFirstBefore<rv::MsgVisualPartDefinition>(mSimTime);
                  if (def)
                  {
                     std::map<std::string, VisualPart>::iterator vpit =
                        mPlatformStates[it->first].mVisualPart.find(statusMsg->partName());
                     if ((vpit == mPlatformStates[it->first].mVisualPart.end()) ||
                         (mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mReadTime != statusMsg->simTime()))
                     {
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mReadTime = statusMsg->simTime();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mDirty    = true;
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mArticulationDirty = true;
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mVisible           = true;
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mIcon = def->partIcon();
                     }
                     // get articulation
                     const rv::MsgPartArticulationStatus* art =
                        visualpart.second.FindFirstBefore<rv::MsgPartArticulationStatus>(mSimTime);
                     if (art)
                     {
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mArticulationDirty = true;
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mLocation[0] =
                           art->locationECS().x();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mLocation[1] =
                           art->locationECS().y();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mLocation[2] =
                           art->locationECS().z();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mOrientation[0] = art->slewECS().x();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mOrientation[1] = art->slewECS().y();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mOrientation[2] = art->slewECS().z();
                     }
                  }
               }
               else
               {
                  // deactivate if necessary
                  if (mPlatformStates[it->first].mVisualPart.find(statusMsg->partName()) !=
                      mPlatformStates[it->first].mVisualPart.end())
                  {
                     if (mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mReadTime != statusMsg->simTime())
                     {
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mReadTime = statusMsg->simTime();
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mDirty    = true;
                        mPlatformStates[it->first].mVisualPart[statusMsg->partName()].mVisible  = false;
                     }
                  }
               }
            }
         }
      }

      plat->EndRead();
   }
}

void RvVaScenarioManager::Interface::GuiUpdate()
{
   QMutexLocker locker(&mMutex);

   vaEnv.GetTime()->SetMaxTime(mSimTime);
   vaEnv.GetTime()->SetTime(mSimTime);

   if (!mScenarioPtr)
   {
      mScenarioPtr = (wkf::Scenario*)vaEnv.NewScenario("wkf");
      mScenarioPtr->SetModifiable(false);
      vaEnv.SetStandardScenario(mScenarioPtr);
   }

   // Update entity states
   for (PlatformStateMap::iterator pit = mPlatformStates.begin(); pit != mPlatformStates.end(); ++pit)
   {
      wkf::Platform*& entityPtr = pit->second.mEntityPtr;
      if (0 == entityPtr)
      {
         bool newOne = false;
         entityPtr   = mScenarioPtr->FindPlatformByIndex(pit->first);
         if (!entityPtr)
         {
            entityPtr = new wkf::Platform(pit->second.mName);
            newOne    = true;
         }
         entityPtr->SetIcon(pit->second.mIcon);
         entityPtr->SetSide(pit->second.mSide);
         entityPtr->SetIndex(pit->first);
         entityPtr->SetSpatialDomain(pit->second.mSpatialDomain);
         entityPtr->SetTypeList(pit->second.mTypeList.GetVector());

         if (newOne)
         {
            mScenarioPtr->AddPlatformWithIndex(entityPtr, pit->first);
            entityPtr->Initialize();
         }
         else // the platform already existed (but was not configured), something needed a platform before a state change
         {
            entityPtr->SetName(pit->second.mName);
            wkf::Observer::PlatformPropertiesChanged(entityPtr);
         }
         wkf::PlatformList temp{entityPtr};
         wkf::Observer::PlatformsAdded(temp);
         wkf::Observer::PlatformConfigured(entityPtr);
         wkf::Observer::PlatformActiveChanged(entityPtr, pit->second.mVisible);
         pit->second.mEntityPtr = entityPtr;
      }
      if (0 != entityPtr)
      {
         entityPtr->SetVisibilityContribution(pit->second.mVisible, vespa::VaEntity::ScenarioStateHidden, 0);

         if (pit->second.mVisible)
         {
            entityPtr->SetPositionOrientation(pit->second.mLocationWCS,
                                              pit->second.mOrientationWCS[0],
                                              pit->second.mOrientationWCS[1],
                                              pit->second.mOrientationWCS[2]);
            entityPtr->SetVelocityWCS(pit->second.mVelocityWCS);
            for (auto&& vit : pit->second.mVisualPart)
            {
               if (vit.second.mVisible)
               {
                  if ((!vit.second.mIcon.empty()) && (vit.second.mDirty))
                  {
                     vit.second.mDirty = false;
                     entityPtr->AddSubobject(vit.first, vit.second.mIcon, vit.second.mLocation, vit.second.mOrientation);
                     wkf::Observer::SubpartAdded(entityPtr, vit.first);
                  }
                  if (vit.second.mArticulationDirty)
                  {
                     vit.second.mArticulationDirty = false;
                     entityPtr->UpdateSubobject(vit.first, vit.second.mLocation, vit.second.mOrientation);
                     wkf::Observer::SubpartUpdated(entityPtr, vit.first);
                  }
               }
               else
               {
                  if ((!vit.second.mIcon.empty()) && (vit.second.mDirty))
                  {
                     vit.second.mDirty = false;
                     entityPtr->RemoveSubobject(vit.first);
                     wkf::Observer::SubpartRemoved(entityPtr, vit.first);
                  }
               }
            }
         }
      }
   }
   if (mValidEntityFound && mResetCamera)
   {
      wkf::Viewer* viewerPtr = vaEnv.GetStandardViewer();
      if (viewerPtr)
      {
         // If CenterOnStart is not specified (default to true) OR
         // If CenterOnStart is specified and it is true then
         // Center the camera on the scenario.
         if (!viewerPtr->DataExists("CenterOnStart") || viewerPtr->GetData("CenterOnStart").ToBool())
         {
            viewerPtr->CenterCamera(false);
         }
         mResetCamera = false;
      }
   }
}

void RvVaScenarioManager::Interface::ClearScenario(bool aFullReset)
{
   if (mScenarioPtr != nullptr)
   {
      if (aFullReset)
      {
         mResetCamera      = true;
         mValidEntityFound = false;
         mSimTime          = 0.0;
      }
      vaEnv.RemoveScenario(mScenarioPtr);
      mScenarioPtr = nullptr;
      mPlatforms.clear();
      mPlatformStates.clear();
      if (!mScenarioPtr)
      {
         mScenarioPtr = (wkf::Scenario*)vaEnv.NewScenario("wkf");
         mScenarioPtr->SetModifiable(false);
         vaEnv.SetStandardScenario(mScenarioPtr);
      }
   }
}
