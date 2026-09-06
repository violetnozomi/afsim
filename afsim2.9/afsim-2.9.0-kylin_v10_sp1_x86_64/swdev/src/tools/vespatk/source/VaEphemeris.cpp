// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "VaEphemeris.hpp"

#include <iostream>

#include "UtMath.hpp"
#include "UtRunEnvManager.hpp"
#include "UtSun.hpp"
#include "UtoEphemerisNode.hpp"
#include "UtoMemory.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

// static members
UtCalendar                                                                   vespa::VaEphemeris::mSimTime;
UtCalendar                                                                   vespa::VaEphemeris::mStartTime;
std::map<int, std::pair<osg::ref_ptr<Uto::EphemerisNode>, vespa::VaViewer*>> vespa::VaEphemeris::mNodeMap;
std::map<int, osg::ref_ptr<Uto::EphemerisMoon>>                              vespa::VaEphemeris::mMoonMap;
bool                                                                         vespa::VaEphemeris::mUseSimTime;
vespa::VaCallbackHolder                                                      vespa::VaEphemeris::mCallbacks;

bool vespa::VaEphemeris::mTimeSetManually = false;

vespa::VaEphemeris::VaEphemeris() {}

// virtual
vespa::VaEphemeris::~VaEphemeris()
{
   mCallbacks.Clear();
}

// static
void vespa::VaEphemeris::Initialize()
{
   mCallbacks.Add(VaObserver::ViewerInitialized.Connect(VaEphemeris::ViewerInitializedCB));
   mCallbacks.Add(VaObserver::ViewerDestroyed.Connect(VaEphemeris::ViewerDestroyedCB));
   mCallbacks.Add(VaObserver::TimeUpdated.Connect(VaEphemeris::TimeUpdatedCB));

   mUseSimTime = true;
   if (!mTimeSetManually)
   {
      mStartTime.SetCurrentDateAndTime();
   }
   mSimTime = mStartTime;
}

void vespa::VaEphemeris::ViewerInitializedCB(VaViewer* aViewerPtr)
{
   osg::Group* cegroup = aViewerPtr->GetCelestialNode();
   if (cegroup != nullptr)
   {
      std::string resourceDir;
      vespa::VaUtils::GetPathPrioritizedFilename("starcatalog", resourceDir);
      UtPath path(resourceDir);
      path.Up();
      resourceDir = path.GetSystemPath();

      osg::ref_ptr<Uto::EphemerisNode> node = ut::osg::make_ref<Uto::EphemerisNode>(resourceDir);
      cegroup->addChild(node);
      node->SetTime(mSimTime);
      aViewerPtr->SetSunVector(node->GetSunVector());
      mNodeMap[aViewerPtr->GetUniqueId()] = std::make_pair(node, aViewerPtr);

      osg::Group* group = aViewerPtr->GetSceneRoot(); // and add a moon to the main scene
      if (group != nullptr)
      {
         osg::ref_ptr<Uto::EphemerisMoon> moon = ut::osg::make_ref<Uto::EphemerisMoon>(false, resourceDir);
         group->addChild(moon);
         moon->SetTime(mSimTime);
         mMoonMap[aViewerPtr->GetUniqueId()] = moon;
      }
   }
}

void vespa::VaEphemeris::ViewerDestroyedCB(VaViewer* aViewerPtr)
{
   mNodeMap.erase(aViewerPtr->GetUniqueId());
   mMoonMap.erase(aViewerPtr->GetUniqueId());
}

// static
void vespa::VaEphemeris::TimeUpdatedCB(double aTime, double aRate, double aMax, double aMin)
{
   if (mUseSimTime)
   {
      mSimTime = mStartTime;
      mSimTime.AdvanceTimeBy(aTime);

      for (auto&& node : mNodeMap)
      {
         node.second.first->SetTime(mSimTime);
      }
      for (auto&& moon : mMoonMap)
      {
         moon.second->SetTime(mSimTime);
      }

      Draw();
   }
}

// static
void vespa::VaEphemeris::Draw()
{
   for (auto&& node : mNodeMap)
   {
      node.second.second->SetSunVector(node.second.first->GetSunVector());
   }
}

// static
void vespa::VaEphemeris::SetStartTime(const UtCalendar& aCalendar)
{
   mStartTime       = aCalendar;
   mSimTime         = mStartTime;
   mTimeSetManually = true;

   for (auto&& node : mNodeMap)
   {
      node.second.first->SetTime(mSimTime);
   }
   for (auto&& moon : mMoonMap)
   {
      moon.second->SetTime(mSimTime);
   }

   Draw();
}
