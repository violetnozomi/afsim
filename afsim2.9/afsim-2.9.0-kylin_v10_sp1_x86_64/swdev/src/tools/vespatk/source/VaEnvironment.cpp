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

#include "VaEnvironment.hpp"

#include <algorithm>
#include <assert.h>

#include <osg/Notify>

#include "UtAngle.hpp"
#include "UtAngleR.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInputFile.hpp"
#include "UtLength.hpp"
#include "UtLog.hpp"
#include "UtSpeed.hpp"
#include "UtTime.hpp"
#include "UtoShape.hpp"
#include "UtoViewer.hpp"
#include "VaAttachment.hpp"
#include "VaEntity.hpp"
#include "VaEntityPtr.hpp"
#include "VaEphemeris.hpp"
#include "VaEvent.hpp"
#include "VaEventManager.hpp"
#include "VaModelDatabase.hpp"
#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "VaScenario.hpp"
#include "VaSelectionBox.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
VaEnvironment* VaEnvironment::mInstancePtr = nullptr;

VaEnvironment& VaEnvironment::Instance()
{
   assert(Exists());
   return *mInstancePtr;
}

bool VaEnvironment::Exists()
{
   return (mInstancePtr != nullptr);
}

void VaEnvironment::Shutdown()
{
   if (mInstancePtr != nullptr)
   {
      delete mInstancePtr;
      mInstancePtr = nullptr;
   }
}

VaEnvironment::VaEnvironment(VaTimeManager*   aTimePtr,
                             VaViewerManager* aViewerManagerPtr,
                             VaFactory*       aFactoryPtr,
                             VaObserver*      aObserverPtr)
   : mInitialized(false)
   , mScenarios()
   , mCurrentScenarioKey()
   , mTimePtr(aTimePtr)
   , mViewerManagerPtr(aViewerManagerPtr)
   , mFactoryPtr(aFactoryPtr)
   , mObserverPtr(aObserverPtr)
   , mSimTimeEventManagerPtr(nullptr)
   , mRealTimeEventManagerPtr(nullptr)
   , mSelectionDirty(true)
   , mSelection()
   , mCurrentCursorPosition(0, 0, 0)
   , mCurrentCursorMetersPerPixel(1.0)
   , mSavedCursorPosition(0, 0, 0)
   , mSavedCursorMetersPerPixel(1.0)
   , mUseEphemeris(true)
   , mDirtyTimeRange(true)
   , mStart(-1)
   , mEnd(-1)
   , mDoingMultiselect(0)
   , mApplicationTerminated(false)
   , mProxyHost("")
   , mProxyPort(8080)
   , mCacheLocation("")
   , mCacheMode("Disable")
   , mGL_Version("Unknown")
   , mGLSL_Version("Unknown")
   , mGL_Extensions("Unknown")
{
   assert(mInstancePtr == nullptr); // Something already created a environment object. There should only be one.

// when we updated to osgEarth 2.10.1 and osg 3.6.3 there seemed to be some disagreement on the handling of image
// loading between the two osgEarth attempts to load images in its LOD until it can't load anymore, and then assumes it
// hit the highest LOD at some point, though OSG started complaining about the attempts to load non-existent files,
// resulting in command line spew. We block these messages, but this may result in other useful messages being lost.
#ifndef _DEBUG
   osg::setNotifyLevel(osg::FATAL);
#endif
   UtoShape::SetVespaCoordinateSystem(true);

   // By setting the instance pointer to the this pointer we allow
   // users to derive from VaEnvironment, which is a singleton.
   if (nullptr == mInstancePtr)
   {
      mInstancePtr = this;
   }

   if (nullptr == mTimePtr)
   {
      mTimePtr = new VaTimeManager();
   }

   if (nullptr == mViewerManagerPtr)
   {
      mViewerManagerPtr = new VaViewerManager();
   }

   if (nullptr == mFactoryPtr)
   {
      mFactoryPtr = new VaFactory();
   }

   if (nullptr == mObserverPtr)
   {
      mObserverPtr = new VaObserver();
   }

   mModelDatabasePtr = ut::make_unique<VaModelDatabase>();

   mObserverPtr->Initialize();

   mSimTimeEventManagerPtr  = new VaSimTimeEventManager();
   mRealTimeEventManagerPtr = new VaRealTimeEventManager();

   mUnitFormat.mAltFormat    = UtLength::FmtFT + 2;
   mUnitFormat.mDistFormat   = UtLength::FmtNM + 2;
   mUnitFormat.mSpeedFormat  = UtSpeed::FmtKTS + 0;
   mUnitFormat.mTimeFormat   = UtTime::FmtHMS + 2;
   mUnitFormat.mLatLonFormat = UtAngle::FmtDMS + 2;
   mUnitFormat.mAngleFormat  = UtAngleR::FmtDeg + 2;

   mCallbacks.Add(VaObserver::EntityDeleted.Connect(&VaEnvironment::EntityDeletedCB, this));
   mCallbacks.Add(VaObserver::EntitySelected.Connect(&VaEnvironment::EntitySelectedCB, this));
   mCallbacks.Add(VaObserver::AttachmentDeleted.Connect(&VaEnvironment::AttachmentDeletedCB, this));
   mCallbacks.Add(VaObserver::AttachmentSelected.Connect(&VaEnvironment::AttachmentSelectedCB, this));
   mCallbacks.Add(VaObserver::HandleSelectionBoxResult.Connect(&VaEnvironment::HandleSelectionBoxResultCB, this));
   mCallbacks.Add(VaObserver::SetUnitFormat.Connect(&VaEnvironment::SetUnitFormatCB, this));
   mCallbacks.Add(VaObserver::CameraUpdated.Connect(&VaEnvironment::CameraUpdatedCB, this));
   mCallbacks.Add(VaObserver::TimeUpdated.Connect(&VaEnvironment::TimeUpdatedCB, this));
   mCallbacks.Add(
      VaObserver::RedrawWindow.Connect(&VaEnvironment::RedrawWindowCB, this, VaCallbackGroupId("frame_update")));
}

// virtual
VaEnvironment::~VaEnvironment()
{
   // Remove any remaining scenarios.
   std::vector<VaScenario*> scenarios;
   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      scenarios.push_back(iter->second);
   }
   for (std::vector<VaScenario*>::iterator iter = scenarios.begin(); iter != scenarios.end(); ++iter)
   {
      RemoveScenario(*iter);
   }

   delete mTimePtr;
   mTimePtr = nullptr;
   delete mViewerManagerPtr;
   mViewerManagerPtr = nullptr;
   delete mFactoryPtr;
   mFactoryPtr = nullptr;
   delete mObserverPtr;
   mObserverPtr = nullptr;
   delete mSimTimeEventManagerPtr;
   mSimTimeEventManagerPtr = nullptr;
   delete mRealTimeEventManagerPtr;
   mRealTimeEventManagerPtr = nullptr;

   mCallbacks.Clear();

   mInstancePtr = nullptr;
}

bool VaEnvironment::Initialize()
{
   if (!mInitialized)
   {
      mInitialized = true;

      for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
      {
         mInitialized &= iter->second->Initialize();
      }

      if (mUseEphemeris)
      {
         VaEphemeris::Initialize();
      }
   }

   mModelDatabasePtr->Initialize();

   return mInitialized;
}

void VaEnvironment::Load(VaViewer* aViewerPtr)
{
   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      iter->second->Load(aViewerPtr);
   }
}

//! Updates each of the scenarios.
//! This is called only if the time changes and is not specific to a viewer.

void VaEnvironment::Update(double aTime)
{
   VaObserver::BeginUserStat("VaUpdate");
   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      iter->second->Update(aTime);
   }

   // Dispatch any scheduled events.
   DispatchSimTimeEvents(aTime);

   VaObserver::EndUserStat("VaUpdate");
}

//! Updates each of the scenarios.
//! This is called only once per frame and is not specific to a viewer.

void VaEnvironment::UpdateFrame(double aTime)
{
   VaObserver::BeginUserStat("VaUpdateFrame");
   if (mDirtyTimeRange)
   {
      HandleDirtyTimeRange();
      mDirtyTimeRange = false;
   }

   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      iter->second->UpdateFrame(aTime);
   }

   // Dispatch any scheduled events.
   DispatchRealTimeEvents(GetTime()->GetWallClockTime());

   VaObserver::EndUserStat("VaUpdateFrame");
}

//! Updates each of the scenario's screen based elements.
//! Each viewer calls this just before rendering.
//! @param aViewerPtr The viewer who's requesting the update.

void VaEnvironment::CameraUpdatedCB(VaViewer* aViewerPtr)
{
   /*
   TODO - VESPA_FRAMEWORK
   if (aViewerPtr != 0)
   {
      VaPosition camPos;
      aViewerPtr->GetCameraPosition(camPos);
      if (mRenderBinManagerPtr->SetViewAltitude(camPos.GetAlt())) // a render bin swap has occurred.
      {
         for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
         {
            iter->second->UpdateEntityRenderBins();
         }
      }
   }
   */
}

void VaEnvironment::AddSimTimeEvent(VaEvent* aEventPtr)
{
   mSimTimeEventManagerPtr->AddEvent(aEventPtr);
}

void VaEnvironment::AddRealTimeEvent(VaEvent* aEventPtr)
{
   mRealTimeEventManagerPtr->AddEvent(aEventPtr);
}

VaScenario* VaEnvironment::NewScenario(const std::string& aFormat)
{
   VaScenario* scenarioPtr = new VaScenario(aFormat);
   if (scenarioPtr != nullptr)
   {
      AddScenario(scenarioPtr);
   }
   return scenarioPtr;
}

bool VaEnvironment::AddScenario(VaScenario* aScenarioPtr)
{
   bool added = false;

   VaScenarioList::const_iterator iter = mScenarios.find(aScenarioPtr->GetKey());
   if (iter == mScenarios.end())
   {
      mScenarios[aScenarioPtr->GetKey()] = aScenarioPtr;
      mCurrentScenarioKey                = aScenarioPtr->GetKey();

      if (mInitialized && !aScenarioPtr->IsInitialized())
      {
         aScenarioPtr->Initialize();
      }

      VaObserver::ScenarioAdded(aScenarioPtr);
      VaObserver::ScenarioCurrentChanged(aScenarioPtr);

      added = true;
   }
   return added;
}

bool VaEnvironment::RemoveScenario(VaScenario* aScenarioPtr)
{
   bool removed = false;

   VaScenarioList::iterator iter = mScenarios.find(aScenarioPtr->GetKey());
   if (iter != mScenarios.end())
   {
      VaScenario* scenarioPtr = iter->second;
      bool        isCurrent   = (scenarioPtr->GetKey() == mCurrentScenarioKey);

      VaObserver::ScenarioRemoved(scenarioPtr);

      delete scenarioPtr;
      mScenarios.erase(iter);

      // Since this was the current scenario, pick a new one.
      if (isCurrent)
      {
         if (!mScenarios.empty())
         {
            VaScenarioList::iterator iter = mScenarios.begin();
            SetCurrentScenario(iter->second->GetKey());
         }
      }

      VaObserver::SelectionChanged();
      removed = true;
   }
   return removed;
}

bool VaEnvironment::ClearScenarios()
{
   VaViewerManager::Viewers& v = mViewerManagerPtr->GetViewers();
   for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
   {
      VaViewer* viewer = *it;
      viewer->ClearSelection();
   }

   VaScenarioList scenarioList = mScenarios;
   mScenarios.clear();

   for (VaScenarioList::iterator iter = scenarioList.begin(); iter != scenarioList.end(); ++iter)
   {
      VaScenario* scenarioPtr = iter->second;
      SetCurrentScenario(scenarioPtr->GetKey());

      VaObserver::ScenarioRemoved(scenarioPtr);

      delete scenarioPtr;
   }
   VaScenarioKey emptyKey;
   SetCurrentScenario(emptyKey);

   return true;
}

VaScenario* VaEnvironment::FindScenario(const VaScenarioKey& aKey) const
{
   VaScenario* scenarioPtr = nullptr;

   VaScenarioList::const_iterator iter = mScenarios.find(aKey);
   if (iter != mScenarios.end())
   {
      scenarioPtr = iter->second;
   }
   return scenarioPtr;
}

VaScenario* VaEnvironment::FindScenario(const std::string& aName) const
{
   VaScenarioList scenarioList = mScenarios;
   for (VaScenarioList::iterator iter = scenarioList.begin(); iter != scenarioList.end(); ++iter)
   {
      if (iter->second->GetName() == aName)
      {
         return iter->second;
      }
   }
   return nullptr;
}

VaScenario* VaEnvironment::SetCurrentScenario(const VaScenarioKey& aKey)
{
   VaScenario* scenarioPtr = nullptr;

   VaScenarioList::const_iterator iter = mScenarios.find(aKey);
   if (iter != mScenarios.end())
   {
      scenarioPtr         = iter->second;
      mCurrentScenarioKey = scenarioPtr->GetKey();

      VaObserver::ScenarioCurrentChanged(scenarioPtr);
   }
   return scenarioPtr;
}

VaScenario* VaEnvironment::GetCurrentScenario() const
{
   VaScenario* scenarioPtr = nullptr;

   VaScenarioList::const_iterator iter = mScenarios.find(mCurrentScenarioKey);

   // If the current didn't correspond to an existing scenario, return the first scenario.
   iter = (iter != mScenarios.end()) ? iter : mScenarios.begin();
   if (iter != mScenarios.end())
   {
      scenarioPtr = iter->second;
   }
   return scenarioPtr;
}

VaScenario* VaEnvironment::NextScenario()
{
   VaScenario* scenarioPtr = nullptr;
   if (!mScenarios.empty())
   {
      VaScenarioList::const_iterator iter = mScenarios.find(mCurrentScenarioKey);
      if (iter != mScenarios.end())
      {
         ++iter;

         // Loop back to the beginning.
         if (iter == mScenarios.end())
         {
            iter = mScenarios.begin();
         }

         scenarioPtr         = iter->second;
         mCurrentScenarioKey = iter->second->GetKey();

         VaObserver::ScenarioCurrentChanged(scenarioPtr);
      }
   }
   return scenarioPtr;
}

VaScenario* VaEnvironment::PrevScenario()
{
   VaScenario* scenarioPtr = nullptr;
   if (!mScenarios.empty())
   {
      VaScenarioList::const_iterator iter = mScenarios.find(mCurrentScenarioKey);
      ;

      // Loop back to the end.
      if (iter == mScenarios.begin())
      {
         iter = mScenarios.end();
      }

      if (iter != mScenarios.begin())
      {
         --iter;
         scenarioPtr         = iter->second;
         mCurrentScenarioKey = iter->second->GetKey();

         VaObserver::ScenarioCurrentChanged(scenarioPtr);
      }
   }
   return scenarioPtr;
}

bool VaEnvironment::ResetScenarios()
{
   bool scenariosReset = !mScenarios.empty();
   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      iter->second->Reset();
   }
   return scenariosReset;
}

// static
VaEntity* VaEnvironment::CreateEntity(const std::string& aEntityType)
{
   return Instance().GetFactory()->CreateEntity(aEntityType);
}

//! Builds a new VaAttachment and adds it to the passed parent entity.
//!
//! @param aAttachmentType - A string describing the type of attachment to create
//! @param aEntity - The parent entity.
//! @param aViewerPtr - The viewer in which this attachment will be visible. If NULL is passed
//!                     then the attachment will be visible in all viewers.
//! @param aLoad - If true, then Initialize() and Load() will be called on the newly created attachment
VaAttachment*
VaEnvironment::CreateAttachment(const std::string& aAttachmentType, VaEntity& aEntity, VaViewer* aViewerPtr, bool aLoad)
{
   return Instance().GetFactory()->CreateAttachment(aAttachmentType, aEntity, aViewerPtr, aLoad);
}

VaScenario* VaEnvironment::CreateScenario(const std::string& aScenarioType)
{
   return Instance().GetFactory()->CreateScenario(aScenarioType);
}

//! Returns the aggregate bounding box for all scenarios.
//! @param aBoundingBox The bounding box is returned by reference.
//! @return Returns true if a bound was computed. By default -90, -180, 90, 180 will be returned.

bool VaEnvironment::GetScenarioBound(VaBoundingBox& aBoundingBox) const
{
   bool ok = false;

   for (VaScenarioList::const_iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      VaBoundingBox bb;
      iter->second->GetScenarioBound(bb);
      aBoundingBox.Grow(bb);
      ok = true;
   }
   return ok;
}

void VaEnvironment::GetBound(const VaEntityMap& aEntities, VaBoundingBox& aBoundingBox) const
{
   int    numEntities  = 0;
   double centerXYZ[3] = {0.0, 0.0, 0.0};

   if (aEntities.empty())
   {
      if (VaPosition::GetMapProjection() != nullptr)
      {
         UtVec3d minLLA(VaPosition::GetCenterLat() - 2.5, VaPosition::GetCenterLon() - 2.5, 0.0);
         UtVec3d maxLLA(VaPosition::GetCenterLat() + 2.5, VaPosition::GetCenterLon() + 2.5, 0.0);
         aBoundingBox = VaBoundingBox(minLLA, maxLLA);
      }
   }
   else
   {
      double tempXYZ[3];
      for (VaEntityMap::const_iterator eli = aEntities.begin(); eli != aEntities.end(); ++eli)
      {
         VaEntity* entityPtr = eli->second;
         if (entityPtr != nullptr)
         {
            VaBoundingBox bb;
            entityPtr->GetBoundingBox(bb);
            aBoundingBox.Grow(bb);

            UtEllipsoidalEarth::ConvertLLAToECEF(bb.mCenterLLA[0], bb.mCenterLLA[1], bb.mCenterLLA[2], tempXYZ);
            UtVec3d::Add(centerXYZ, centerXYZ, tempXYZ);
            ++numEntities;
         }
      }

      // Calculate the mean LLA.
      if (numEntities > 0)
      {
         UtVec3d::Divide(centerXYZ, (double)numEntities);
         double meanLLA[3];
         UtEllipsoidalEarth::ConvertECEFToLLA(centerXYZ, meanLLA[0], meanLLA[1], meanLLA[2]);
         aBoundingBox.mMeanLLA = UtVec3d(meanLLA);
      }
   }
}

void VaEnvironment::GetBound(const VaEntityList& aEntities, VaBoundingBox& aBoundingBox) const
{
   int    numEntities  = 0;
   double centerXYZ[3] = {0.0, 0.0, 0.0};

   if (aEntities.empty())
   {
      if (VaPosition::GetMapProjection() != nullptr)
      {
         UtVec3d minLLA(VaPosition::GetCenterLat() - 2.5, VaPosition::GetCenterLon() - 2.5, 0.0);
         UtVec3d maxLLA(VaPosition::GetCenterLat() + 2.5, VaPosition::GetCenterLon() + 2.5, 0.0);
         aBoundingBox = VaBoundingBox(minLLA, maxLLA);
      }
   }
   else
   {
      double tempXYZ[3];
      for (VaEntityList::const_iterator eli = aEntities.begin(); eli != aEntities.end(); ++eli)
      {
         VaEntity* entityPtr = *eli;
         if (entityPtr != nullptr)
         {
            VaBoundingBox bb;
            entityPtr->GetBoundingBox(bb);
            aBoundingBox.Grow(bb);

            UtEllipsoidalEarth::ConvertLLAToECEF(bb.mCenterLLA[0], bb.mCenterLLA[1], bb.mCenterLLA[2], tempXYZ);
            UtVec3d::Add(centerXYZ, centerXYZ, tempXYZ);
            ++numEntities;
         }
      }

      // Calculate the mean LLA.
      if (numEntities > 0)
      {
         UtVec3d::Divide(centerXYZ, (double)numEntities);
         double meanLLA[3];
         UtEllipsoidalEarth::ConvertECEFToLLA(centerXYZ, meanLLA[0], meanLLA[1], meanLLA[2]);
         aBoundingBox.mMeanLLA = UtVec3d(meanLLA);
      }
   }
}

bool VaEnvironment::BoxSelect(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside)
{
   bool selected = false;
   BeginMultiSelect();
   for (VaScenarioList::iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      selected |= iter->second->BoxSelect(aViewerPtr, aBox, aInside);
   }
   EndMultiSelect();
   return selected;
}

bool VaEnvironment::DeleteSelection(VaViewer* aViewerPtr)
{
   std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>> scenarioToEntityAndAttachment;
   GetScenarioToEntityAndAttachmentMap(aViewerPtr->GetSelection(), scenarioToEntityAndAttachment);

   bool selectionDeleted = false;
   for (std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>>::iterator it =
           scenarioToEntityAndAttachment.begin();
        it != scenarioToEntityAndAttachment.end();
        ++it)
   {
      if (it->first->IsModifiable())
      {
         selectionDeleted |= it->first->DeleteSelection(aViewerPtr, it->second.first, it->second.second);
      }
   }

   return selectionDeleted;
}

void VaEnvironment::MoveSelection(VaViewer*       aViewerPtr,
                                  const UtLLAPos& aBefore,
                                  const UtLLAPos& aAfter,
                                  int             aFeedbackFormat,
                                  double          aFeedbackPrecision)
{
   UtEntity ebefore;
   ebefore.SetLocationLLA(aBefore.GetLat(), aBefore.GetLon(), aBefore.GetAlt());
   UtEntity eafter;
   eafter.SetLocationLLA(aAfter.GetLat(), aAfter.GetLon(), aAfter.GetAlt());

   std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>> scenarioToEntityAndAttachment;
   GetScenarioToEntityAndAttachmentMap(aViewerPtr->GetSelection(), scenarioToEntityAndAttachment);

   for (auto&& moves : scenarioToEntityAndAttachment)
   {
      // Checks for modifiability, etc. will be handled by the scenario's MoveSelection method
      moves.first->MoveSelection(aViewerPtr,
                                 moves.second.first,
                                 moves.second.second,
                                 ebefore,
                                 eafter,
                                 aFeedbackFormat,
                                 aFeedbackPrecision);
   }
}

void VaEnvironment::MoveSelectionComplete(VaViewer* aViewerPtr)
{
   std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>> scenarioToEntityAndAttachment;
   GetScenarioToEntityAndAttachmentMap(aViewerPtr->GetSelection(), scenarioToEntityAndAttachment);

   for (std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>>::iterator it =
           scenarioToEntityAndAttachment.begin();
        it != scenarioToEntityAndAttachment.end();
        ++it)
   {
      it->first->MoveSelectionComplete(aViewerPtr, it->second.first, it->second.second);
   }
}

void VaEnvironment::RotateSelection(VaViewer* aViewerPtr, const UtLLAPos& aCenterPt, double aAngle)
{
   std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>> scenarioToEntityAndAttachment;
   GetScenarioToEntityAndAttachmentMap(aViewerPtr->GetSelection(), scenarioToEntityAndAttachment);

   for (std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>>::iterator it =
           scenarioToEntityAndAttachment.begin();
        it != scenarioToEntityAndAttachment.end();
        ++it)
   {
      if (it->first->IsModifiable())
      {
         it->first->RotateSelection(aViewerPtr, it->second.first, it->second.second, aCenterPt, aAngle);
      }
   }
}

void VaEnvironment::DeleteEntity(VaEntity* aEntityPtr)
{
   if (aEntityPtr != nullptr)
   {
      if (!aEntityPtr->mDeleting) // Make sure we don't recursively delete the entity
      {
         aEntityPtr->mDeleting = true;
         VaObserver::EntityDeleted[aEntityPtr->GetUniqueId()](aEntityPtr);
         aEntityPtr->PrivateBeforeDelete();
         delete aEntityPtr;
      }
   }
}

VaEntity* VaEnvironment::FindEntity(const std::string& aName) const
{
   VaEntity* entityPtr = nullptr;
   for (auto&& scen : mScenarios)
   {
      entityPtr = scen.second->FindEntity(aName);
      if (entityPtr != nullptr)
      {
         break;
      }
   }
   return entityPtr;
}

VaAttachment* VaEnvironment::FindAttachment(const std::string& aName) const
{
   VaAttachment* attachmentPtr = nullptr;
   for (VaScenarioList::const_iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      attachmentPtr = iter->second->FindAttachment(aName);
      if (attachmentPtr != nullptr)
      {
         break;
      }
   }
   return attachmentPtr;
}

VaAttachment* VaEnvironment::FindAttachment(unsigned int aUniqueId) const
{
   VaAttachment* attachmentPtr = nullptr;
   for (VaScenarioList::const_iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      attachmentPtr = iter->second->FindAttachment(aUniqueId);
      if (attachmentPtr != nullptr)
      {
         break;
      }
   }
   return attachmentPtr;
}

void VaEnvironment::GetEntityList(VaEntityList& aEntityList) const
{
   for (auto&& scen : mScenarios)
   {
      scen.second->GetEntityList(aEntityList);
   }
}

VaEntity* VaEnvironment::FindEntity(unsigned int aUniqueId) const
{
   VaEntity* entityPtr = nullptr;
   for (VaScenarioList::const_iterator iter = mScenarios.begin(); iter != mScenarios.end(); ++iter)
   {
      entityPtr = iter->second->FindEntity(aUniqueId);
      if (entityPtr != nullptr)
      {
         return entityPtr;
      }
   }
   return nullptr;
}

//! Method: HeightOfTerrain
//! Description: Returns the height of the terrain at the specified location.
//!
//! @param aPosition The location of the desired height lookup.
//! @param aViewerPtr The viewer to use to do the terrain height lookup.
//!        If NULL is provided the default viewer will be used.

// virtual
double VaEnvironment::HeightOfTerrain(const VaPosition& aPosition, VaViewer* aViewerPtr) const
{
   double    hot       = 0.0;
   VaViewer* viewerPtr = (nullptr != aViewerPtr) ? aViewerPtr : mViewerManagerPtr->CurrentViewer();
   if (viewerPtr != nullptr)
   {
      hot = viewerPtr->HeightOfTerrain(aPosition);
   }
   return hot;
}


//! Method: SetCenter
//! Description: Sets the center of the camera.
//!
//! @param aPosition The position to center the camera on.
//! @param aViewerPtr The viewer whose camera will be adjusted.
//!        If NULL is provided the default viewer will be used.

// virtual
void VaEnvironment::SetCenter(const VaPosition& aPosition, VaViewer* aViewerPtr)
{
   /*
   TODO - VESPA_FRAMEWORK
   Viewer* viewerPtr = (0 != aViewerPtr) ? aViewerPtr : mViewerManagerPtr->CurrentViewer();
   viewerPtr->SetCenter(aPosition);
   */
}

// virtual
void VaEnvironment::SetUnitFormat(int aMeasureType, int aUnitType)
{
   bool setFormat = true;

   switch (aMeasureType)
   {
   case (AltitudeMeasure):
   {
      mUnitFormat.mAltFormat = aUnitType;
      break;
   }
   case (DistanceMeasure):
   {
      mUnitFormat.mDistFormat = aUnitType;
      UtLength::SetDefaultFormat(mUnitFormat.mDistFormat);
      break;
   }
   case (SpeedMeasure):
   {
      mUnitFormat.mSpeedFormat = aUnitType;
      break;
   }
   case (LatLonMeasure):
   {
      mUnitFormat.mLatLonFormat = aUnitType;
      break;
   }
   case (TimeMeasure):
   {
      mUnitFormat.mTimeFormat = aUnitType;
      UtTime::SetDefaultFormat(mUnitFormat.mTimeFormat);
      break;
   }
   case (AngleMeasure):
   {
      mUnitFormat.mAngleFormat = aUnitType;
      break;
   }
   default:
      setFormat = false;
      break;
   }

   // Notify subscribers that a unit format has changed.
   if (setFormat)
   {
      VaObserver::ChangedUnitFormat(aMeasureType, aUnitType);
   }
}

void VaEnvironment::EntityDeletedCB(VaEntity* aEntityPtr)
{
   mSelectionDirty = true;
   // If the entity was deselected, then remove it to the VaEnvironment's selection list.
   if (aEntityPtr != nullptr)
   {
      VaHitSet::iterator entityIter = mSelection.find(VaHitEntry::FromEntity(*aEntityPtr));
      if (entityIter != mSelection.end())
      {
         mSelection.erase(entityIter);
      }
   }
}

void VaEnvironment::EntitySelectedCB(VaViewer* aViewerPtr, int aSelected, VaEntity* aEntityPtr)
{
   mSelectionDirty = true;
   if (mDoingMultiselect == 0)
   {
      VaObserver::SelectionChanged();
   }
}

void VaEnvironment::AttachmentDeletedCB(VaAttachment*, const VaAttachmentSubId&)
{
   mSelectionDirty = true;
}

void VaEnvironment::AttachmentSelectedCB(VaViewer*, int, VaAttachment*, const VaAttachmentSubId&)
{
   mSelectionDirty = true;
   if (mDoingMultiselect == 0)
   {
      VaObserver::SelectionChanged();
   }
}

void VaEnvironment::HandleSelectionBoxResultCB(const std::string&    aBoxOperation,
                                               VaViewer*             aViewerPtr,
                                               const VaSelectionBox& aSelectionBox)
{
   if (aBoxOperation == "select_inside")
   {
      BoxSelect(aViewerPtr, aSelectionBox, true);
   }
   else if (aBoxOperation == "select_outside")
   {
      BoxSelect(aViewerPtr, aSelectionBox, false);
   }
}

void VaEnvironment::SetUnitFormatCB(int aMeasureType, int aUnitType)
{
   SetUnitFormat(aMeasureType, aUnitType);
}

void VaEnvironment::TimeUpdatedCB(double aTime, double aRate, double aMinTime, double aMaxTime)
{
   Update(aTime);
}

void VaEnvironment::RedrawWindowCB()
{
   VaObserver::PreUpdateFrame();
   UpdateFrame(GetTime()->GetCurrentTime());
   VaObserver::PostUpdateFrame();
}

void VaEnvironment::SetCurrentCursorData(const VaPosition& aCursorPosition, double aCursorMetersPerPixel)
{
   mCurrentCursorPosition       = aCursorPosition;
   mCurrentCursorMetersPerPixel = aCursorMetersPerPixel;
}

void VaEnvironment::SetSavedCursorData(const VaPosition& aCursorPosition, double aCursorMetersPerPixel)
{
   mSavedCursorPosition       = aCursorPosition;
   mSavedCursorMetersPerPixel = aCursorMetersPerPixel;
}

//! Dispatch any pending events up to and including the indicated time.
//!
//! @param aSimTime  All events whose time is less than or equal to this
//!                  value will be dispatched.
// protected virtual
void VaEnvironment::DispatchSimTimeEvents(double aTime)
{
   // Dispatch pending events up to and including the current time.
   mSimTimeEventManagerPtr->DispatchEvents(aTime);
}

//! Dispatch any pending events up to and including the indicated time.
//!
//! @param aRealTime  All events whose time is less than or equal to this
//!                  value will be dispatched.
// protected virtual
void VaEnvironment::DispatchRealTimeEvents(double aTime)
{
   // Dispatch pending events up to and including the current time.
   mRealTimeEventManagerPtr->DispatchEvents(aTime);
}

VaEnvironment::ErrorTracker::ErrorTracker(UtInput& aInput)
   : mTesting(false)
   , mInput(aInput)
{
}

VaEnvironment::ErrorTracker::~ErrorTracker() {}

void VaEnvironment::ErrorTracker::ThrowError()
{
   auto out = ut::log::error() << "VaEnvironment 'other' exception near token.";
   out.AddNote() << "Token: " << mInput.GetCommand();
   out.AddNote() << "Location: " << mInput.GetLocation();
   throw;
}

void VaEnvironment::ErrorTracker::ThrowError(UtException aException)
{
   ThrowErrorMessageStart(aException);
   auto out = ut::log::error() << "VaEnvironment Exception:";
   out.AddNote() << aException.what();
   out.AddNote() << "Location: " << mInput.GetLocation();
   throw(aException);
}

void VaEnvironment::ErrorTracker::ThrowError(UtInput::ExceptionBase aException)
{
   auto out = ut::log::error() << "VaEnvironment Exception:";
   out.AddNote() << "Exception Message: " << aException.GetMessage();
   out.AddNote() << "Location: " << aException.GetLocation();
   throw(aException);
}

void VaEnvironment::ErrorTracker::ThrowErrorMessageStart(UtException aException)
{
   auto out = ut::log::error() << "VaEnvironment UtException near token.";
   out.AddNote() << "Token: " << mInput.GetCommand();
}

std::string VaEnvironment::ErrorTracker::AdditionalMessage(UtException aException)
{
   std::string value = "processing command '";
   value.append(aException.what());
   value += "'";
   return value;
}

void VaEnvironment::DirtyTimeRange()
{
   mDirtyTimeRange = true;
}

// virtual
void VaEnvironment::HandleDirtyTimeRange()
{
   double gStart = 0.0;
   double gEnd   = 0.0;

   bool first = true;
   for (VaScenarioList::iterator scenarioIter = mScenarios.begin(); scenarioIter != mScenarios.end(); ++scenarioIter)
   {
      if (scenarioIter->second->AffectsTimeRange())
      {
         double startTime = 0.0;
         double endTime   = 0.0;

         scenarioIter->second->GetTimeRange(startTime, endTime);
         if (first)
         {
            gStart = startTime;
            gEnd   = endTime;
            first  = false;
         }
         else
         {
            if (startTime < gStart)
            {
               gStart = startTime;
            }
            if (endTime > gEnd)
            {
               gEnd = endTime;
            }
         }
      }
   }

   if (gStart != mStart)
   {
      mStart = gStart;
      mTimePtr->SetMinTime(gStart);
   }
   if (gEnd != mEnd)
   {
      mEnd = gEnd;
      mTimePtr->SetMaxTime(gEnd);
   }
}

void VaEnvironment::EndMultiSelect()
{
   if (--mDoingMultiselect == 0)
   {
      VaObserver::SelectionChanged();
   }
}

void VaEnvironment::SetGL_Info(const char* aVersion, const char* aShadingVersion, const char* aExtensions)
{
   mGL_Version    = aVersion;
   mGLSL_Version  = aShadingVersion;
   mGL_Extensions = aExtensions;
}

std::string VaEnvironment::GetRecommendedGLSL_ShaderString() const
{
   // two sets of shaders are maintained for important features (platforms, etc...)
   // this will inform which one to use
   std::stringstream ss(mGLSL_Version);
   float             number;
   ss >> number;
   if (number >= 3.3)
   {
      return std::string("330");
   }
   else
   {
      // some mesa implementations report back 1.3, but support 3.0es
      // fall-back to that, and hope for the best
      return std::string("300es");
   }
}

void VaEnvironment::BeginMultiSelect()
{
   ++mDoingMultiselect;
}

VaViewer* VaEnvironment::GetFirstViewer() const
{
   return mViewerManagerPtr->GetViewerByIndex(0);
}

void VaEnvironment::GetScenarioToEntityAndAttachmentMap(VaHitSet& aHitSet,
                                                        std::map<VaScenario*, std::pair<VaEntityMap, VaAttachmentList>>& aMap)
{
   VaEntityMap em;
   CollectEntities(aHitSet, em, true);
   VaEntityMap::iterator it;
   for (it = em.begin(); it != em.end(); ++it)
   {
      aMap[it->second->GetScenario()].first[it->second->GetUniqueId()] = it->second;
   }
   VaAttachmentList am;
   CollectAttachments(aHitSet, am);
   VaAttachmentList::iterator jt;
   for (jt = am.begin(); jt != am.end(); ++jt)
   {
      aMap[(*jt)->GetParent().GetScenario()].second.push_back(*jt);
   }
}

} // end namespace vespa
