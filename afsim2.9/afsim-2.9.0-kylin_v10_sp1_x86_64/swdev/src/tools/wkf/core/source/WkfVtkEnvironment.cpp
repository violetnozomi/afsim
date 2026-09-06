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
#include "WkfVtkEnvironment.hpp"

#include <QMenu>

#include "UtPath.hpp"
#include "UtQt.hpp"
#include "VaEphemeris.hpp"
#include "VaScenario.hpp"
#include "VaTimeManager.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "osgDB/Registry"

wkf::VtkEnvironment& wkf::VtkEnvironment::Instance()
{
   assert(mInstancePtr != nullptr);
   return *static_cast<VtkEnvironment*>(mInstancePtr);
}

void wkf::VtkEnvironment::Create(Factory* aFactoryPtr, const std::string& aScenarioType)
{
   assert(mInstancePtr == nullptr);
   if (mInstancePtr == nullptr)
   {
      // mInstancePtr is assigned in vespa::VaEnvironment's constructor
      new VtkEnvironment(aFactoryPtr, aScenarioType);
   }
}

wkf::VtkEnvironment::VtkEnvironment(Factory* aFactoryPtr, const std::string& aScenarioType)
   : vespa::VaEnvironment(nullptr, nullptr, aFactoryPtr, new Observer)
   , mEnableDraggableMovement(false)
   , mScenarioType(aScenarioType)
   , mStandardScenarioPtr(nullptr)
   , mStandardViewerPtr(nullptr)
{
   mCallbacks.Add(Observer::HandleSelectionBox.Connect(&VtkEnvironment::HandleSelectionBoxCB, this));
   mCallbacks.Add(Observer::ScenarioRemoved.Connect(&VtkEnvironment::HandleScenarioRemovedCB, this));
   mCallbacks.Add(Observer::ViewerDestroyed.Connect(&VtkEnvironment::HandleViewerDestroyedCB, this));
}

void wkf::VtkEnvironment::SetStandardScenario(wkf::Scenario* aScenarioPtr)
{
   mStandardScenarioPtr = aScenarioPtr;
   if (mStandardScenarioPtr)
   {
      mStandardScenarioPtr->EnableMovement(mEnableDraggableMovement);
      Observer::StandardScenarioAdded(mStandardScenarioPtr);
   }
}

void wkf::VtkEnvironment::ResetStandardScenario()
{
   if (mStandardScenarioPtr != nullptr)
   {
      RemoveScenario(mStandardScenarioPtr);
   }
   SetStandardScenario(dynamic_cast<Scenario*>(vaEnv.NewScenario(mScenarioType)));
}

void wkf::VtkEnvironment::SetStandardViewer(Viewer* aViewerPtr)
{
   if (mStandardViewerPtr != aViewerPtr)
   {
      mStandardViewerPtr = aViewerPtr;
      wkf::Observer::StandardViewerChanged(mStandardViewerPtr);
   }
}

void wkf::VtkEnvironment::SetStandardWidget(vespa::VaWidget* aWidgetPtr)
{
   if (mStandardWidgetPtr != aWidgetPtr)
   {
      mStandardWidgetPtr = aWidgetPtr;
      wkf::Observer::StandardWidgetChanged(mStandardWidgetPtr);
   }
}

vespa::VaScenario* wkf::VtkEnvironment::NewScenario(const std::string& aScenarioType)
{
   vespa::VaScenario* scenarioPtr = CreateScenario(aScenarioType);
   if (scenarioPtr)
   {
      AddScenario(scenarioPtr);
      scenarioPtr->Load(mStandardViewerPtr);
   }
   return scenarioPtr;
}

// static
void wkf::VtkEnvironment::InitializeOSG_Path()
{
#ifndef _WIN32
   UtPath binPath = UtPath::GetExePath();
   binPath.Up();
   UtPath libPath(binPath + "lib");
   osgDB::Registry::instance()->setLibraryFilePathList(libPath.GetSystemPath());
#endif
}

void wkf::VtkEnvironment::EnableBoxSelection()
{
   if (mStandardViewerPtr)
   {
      mStandardViewerPtr->AddOverlay("selection_box", "selection_box_overlay");
   }
}

void wkf::VtkEnvironment::DisableBoxSelection()
{
   if (mStandardViewerPtr)
   {
      mStandardViewerPtr->RemoveOverlay("selection_box_overlay");
   }
}

void wkf::VtkEnvironment::EnablePlatformsDraggable(bool aDraggable)
{
   mEnableDraggableMovement = aDraggable;
   if (mStandardScenarioPtr)
   {
      mStandardScenarioPtr->EnableMovement(mEnableDraggableMovement);
   }
}

void wkf::VtkEnvironment::HandleSelectionBoxCB(vespa::VaViewer* aViewerPtr, const vespa::VaSelectionBox& aSelectionBox)
{
   vespa::VaObserver::HandleSelectionBoxResult("select_inside", aViewerPtr, aSelectionBox);
}

bool wkf::VtkEnvironment::DeletePlatformByIndex(unsigned int aIndex)
{
   if (mStandardScenarioPtr)
   {
      return mStandardScenarioPtr->DeletePlatformByIndex(aIndex);
   }
   return false;
}

void wkf::VtkEnvironment::HandleScenarioRemovedCB(vespa::VaScenario* aScenario)
{
   if (aScenario == mStandardScenarioPtr)
   {
      SetStandardScenario(nullptr);
      wkf::Scenario* scen = dynamic_cast<Scenario*>(aScenario);
      if (scen)
      {
         wkf::Observer::StandardScenarioRemoved(scen);
      }
   }
}

void wkf::VtkEnvironment::HandleViewerDestroyedCB(vespa::VaViewer* aViewer)
{
   if (aViewer == mStandardViewerPtr)
   {
      SetStandardViewer(nullptr);
      SetStandardWidget(nullptr);
   }
}

void wkf::VtkEnvironment::SetStartTime(const UtCalendar& aCalendar)
{
   vespa::VaEphemeris::SetStartTime(aCalendar);
}

double wkf::VtkEnvironment::GetDisplayedTime()
{
   return GetTime()->GetCurrentTime();
}
