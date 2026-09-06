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

#include "VaScenario.hpp"

#include <algorithm>
#include <assert.h>
#include <memory>

#include "UtEllipsoidalEarth.hpp"
#include "UtLLAPos.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "VaAttachment.hpp"
#include "VaBoundingBox.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaFactory.hpp"
#include "VaObserver.hpp"
#include "VaSelectionBox.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
VaCallbackHolder VaScenario::mGlobalCallbacks;

VaScenario::VaScenario(const std::string& aFormat)
   : mCallbacks()
   , mModified(false)
   , mModifiable(true)
   , mEntityMap()
   , mStartTime(0.0)
   , mEndTime(0.0)
   , mTimeRangeDirty(true)
   , mKey()
   , mFormat(aFormat)
   , mSource()
   , mInitialized(false)
   , mLoaded(false)
   , mAttachmentMap()
   , mPlatformTypes()
   , mLayerShow(true)
   , mAutocenter(true)
{
   mKey.Initialize();

   mCallbacks.Add(VaObserver::EntityDeleted.Connect(&VaScenario::EntityDeletedCB, this));
   mCallbacks.Add(VaObserver::EntityNameChanged.Connect(&VaScenario::EntityNameChangedCB, this));

   SetName("New Scenario");
}

VaScenario::~VaScenario()
{
   // Make a temporary copy of the entity list.
   VaEntityMap tempEntities = mEntityMap;

   for (VaEntityMap::iterator iter = tempEntities.begin(); iter != tempEntities.end(); ++iter)
   {
      if (mEntityMap.find(iter->first) == mEntityMap.end())
      {
         continue;
      }
      VaObserver::EntityDeleted[(*iter).second->GetUniqueId()]((*iter).second);
      if (iter->second)
      {
         iter->second->PrivateBeforeDelete();
         delete (*iter).second;
      }
   }

   mEntityNameMap.clear();
   mEntityMap.clear();

   // Since we just cleared the entity list, we must clear all of the other
   // lists because they wont get cleared now that the entity list is empty.
   mAttachmentMap.clear();
   mPlatformTypes.clear();

   mCallbacks.Clear();
}

void VaScenario::SetName(const std::string& aName)
{
   mModified = false;
}

void VaScenario::SetFormat(const std::string& aFormat)
{
   mFormat   = aFormat;
   mModified = false;
}

void VaScenario::SetSource(const std::string& aSource)
{
   mSource = aSource;
   if (mFormat == "dis")
   {
      size_t firstColon = aSource.find_first_of(':');
      if (firstColon != std::string::npos)
      {
         if (aSource.substr(0, firstColon) == "playback")
         {
            std::string srcFile = aSource.substr(firstColon + 1);
            mSourceFile         = srcFile;
         }
         else
         {
            mSourceFile = "";
         }
      }
      else
      {
         mSourceFile = mSource;
      }
   }
   else
   {
      mSourceFile = mSource;
   }
   mModified = false;
}

bool VaScenario::Initialize()
{
   if (!mInitialized)
   {
      for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
      {
         (*iter).second->Initialize();
      }
      mInitialized = true;
   }
   return mInitialized;
}

void VaScenario::Load(VaViewer* aViewerPtr)
{
   if (!mLoaded)
   {
      for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
      {
         (*iter).second->Load();
      }
      mLoaded = true;
   }
}

bool VaScenario::Reset()
{
   return false;
}

void VaScenario::SetModified(bool aModified)
{
   if (mModified != aModified)
   {
      mModified = aModified;
      VaObserver::ScenarioModified(this);
   }
}

void VaScenario::Update(double aTime)
{
   for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      (*iter).second->Update(aTime);
   }
}

void VaScenario::UpdateFrame(double aTime)
{
   for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      (*iter).second->UpdateFrame(aTime);
   }
}

void VaScenario::TranslateScenario(double aDeltaLat, double aDeltaLon)
{
   for (VaEntityMap::const_iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      iter->second->Translate(aDeltaLat, aDeltaLon);
   }
}

void VaScenario::RotateScenario(const UtLLAPos& aCenterLLA, double aAngle)
{
   for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      iter->second->Rotate(aCenterLLA, aAngle);
   }
}

void VaScenario::GetScenarioBound(VaBoundingBox& aBoundingBox) const
{
   GetBound(mEntityMap, aBoundingBox);
}

void VaScenario::GetBound(const VaEntityMap& aEntities, VaBoundingBox& aBoundingBox) const
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
      VaViewer* currentViewerPtr = VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
      double    tempXYZ[3];
      for (VaEntityMap::const_iterator eli = aEntities.begin(); eli != aEntities.end(); ++eli)
      {
         VaEntity* entityPtr = (*eli).second;
         if ((entityPtr != nullptr) && (entityPtr->IsVisible(currentViewerPtr)))
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

//! Method: AddEntity
//! Description: Adds an entity to the scenario.
//!
//! @param aEntityPtr The entity to add.
//! @return Returns true if the entity was successfully added.

// virtual
bool VaScenario::AddEntity(VaEntity* aEntityPtr)
{
   bool                        added = false;
   VaEntityMap::const_iterator iter  = mEntityMap.find(aEntityPtr->GetUniqueId());
   if (iter == mEntityMap.end())
   {
      aEntityPtr->SetScenario(this); // Add the scenario key to the entity.

      mEntityMap[aEntityPtr->GetUniqueId()] = aEntityPtr;

      AddAttachments(aEntityPtr);
      // If we're adding the entity after the scenario has already been initialized,
      // then initialize the entity.
      if (mInitialized && !aEntityPtr->IsInitialized())
      {
         aEntityPtr->Initialize();
         aEntityPtr->Load();
      }
      VaObserver::EntityAdded(aEntityPtr);
      aEntityPtr->SetVisibilityContribution(mLayerShow,
                                            VaEntity::ScenarioHidden,
                                            nullptr); // note, scenario visibility is currently a trans-viewer setting

      added = true;
      SetModified(true);
   }
   return added;
}

// private
void VaScenario::AddAttachments(VaEntity* aEntityPtr)
{
   // Add the cloned attachments to the attachment map.
   const auto& attachments = aEntityPtr->GetAttachments();
   for (const auto& attachmentPtr : attachments)
   {
      if (attachmentPtr != nullptr)
      {
         mAttachmentMap[attachmentPtr->GetUniqueId()] = attachmentPtr.get();
      }
   }
}

// private
void VaScenario::AddAttachment(VaAttachment* aAttachmentPtr)
{
   VaAttachmentMap::iterator iter = mAttachmentMap.find(aAttachmentPtr->GetUniqueId());
   if (iter == mAttachmentMap.end())
   {
      mAttachmentMap[aAttachmentPtr->GetUniqueId()] = aAttachmentPtr;
      SetModified(true);
   }
}

//! Method: RemoveEntity
//! Description: Removes an entity from the scenario.
//!
//! @param aEntityPtr The entity to remove.
//! @return Returns true if the entity was successfully removed.

// virtual
bool VaScenario::RemoveEntity(VaEntity* aEntityPtr)
{
   bool removed = false;

   VaEntityMap::const_iterator iter = mEntityMap.find(aEntityPtr->GetUniqueId());
   if (iter != mEntityMap.end())
   {
      aEntityPtr->SetSelected(nullptr, false);

      RemoveAttachments(aEntityPtr);

      PrivateRemoveEntity(aEntityPtr);
      mEntityMap.erase(aEntityPtr->GetUniqueId());

      VaObserver::EntityRemoved[aEntityPtr->GetUniqueId()](aEntityPtr);

      aEntityPtr->SetScenario(nullptr);

      removed = true;
      SetModified(true);
   }
   auto range = mEntityNameMap.equal_range(aEntityPtr->GetName());

   for (auto i = range.first; i != range.second; ++i)
   {
      if (i->second->GetUniqueId() == aEntityPtr->GetUniqueId())
      {
         i = mEntityNameMap.erase(i);
         break;
      }
   }
   return removed;
}

// private
void VaScenario::RemoveAttachments(VaEntity* aEntityPtr)
{
   // Remove the entities attachments from the attachment map.
   const auto& attachments = aEntityPtr->GetAttachments();
   for (const auto& attachIter : attachments)
   {
      unsigned int attachId = attachIter->GetUniqueId();
      mAttachmentMap.erase(attachId);
      if (attachIter->GetViewer() == nullptr)
      {
         VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
         for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
         {
            (*it)->RemoveAttachmentSelection(attachIter.get());
         }
      }
      else
      {
         attachIter->GetViewer()->RemoveAttachmentSelection(attachIter.get());
      }
   }
}

// private
void VaScenario::RemoveAttachment(VaAttachment* aAttachmentPtr, int aSubpartId)
{
   mAttachmentMap.erase(aAttachmentPtr->GetUniqueId());
   if (aAttachmentPtr->GetViewer() == nullptr)
   {
      VaViewerManager::Viewers v = VaEnvironment::Instance().GetViewerManager()->GetViewers();
      for (VaViewerManager::Viewers::iterator it = v.begin(); it != v.end(); ++it)
      {
         (*it)->RemoveAttachmentSelection(aAttachmentPtr, aSubpartId);
      }
   }
   else
   {
      aAttachmentPtr->GetViewer()->RemoveAttachmentSelection(aAttachmentPtr, aSubpartId);
   }
}

void VaScenario::DeleteEntity(VaEntity* aEntityPtr)
{
   if (aEntityPtr != nullptr)
   {
      VaEntityMap::const_iterator iter = mEntityMap.find(aEntityPtr->GetUniqueId());
      if (iter != mEntityMap.end())
      {
         PrivateDeleteEntity(aEntityPtr);
         // Only notify if the entity existed in the scenario. When the undo stack is
         // purged entities are deleted and we don't want them triggering the EntityDeleted callback
         VaObserver::EntityDeleted[aEntityPtr->GetUniqueId()](aEntityPtr);
      }
      aEntityPtr->PrivateBeforeDelete();

      auto range = mEntityNameMap.equal_range(aEntityPtr->GetName());

      for (auto i = range.first; i != range.second; ++i)
      {
         if (i->second->GetUniqueId() == aEntityPtr->GetUniqueId())
         {
            i = mEntityNameMap.erase(i);
            break;
         }
      }

      delete aEntityPtr;
      SetModified(true);
   }
}

VaEntity* VaScenario::FindEntity(const std::string& aName) const
{
   auto it = mEntityNameMap.find(aName);
   if (it != mEntityNameMap.end())
   {
      return it->second;
   }
   return nullptr;
}

VaEntity* VaScenario::FindEntity(unsigned int aUniqueId) const
{
   VaEntity* entityPtr = nullptr;

   VaEntityMap::const_iterator iter = mEntityMap.find(aUniqueId);
   if (iter != mEntityMap.end())
   {
      entityPtr = (*iter).second;
   }
   return entityPtr;
}

VaAttachment* VaScenario::FindAttachment(unsigned int aUniqueId) const
{
   VaAttachmentMap::const_iterator iter = mAttachmentMap.find(aUniqueId);
   if (iter != mAttachmentMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

VaAttachment* VaScenario::FindAttachment(const std::string& aName) const
{
   VaAttachment* attachmentPtr = nullptr;

   for (VaAttachmentMap::const_iterator iter = mAttachmentMap.begin(); iter != mAttachmentMap.end(); ++iter)
   {
      if (iter->second->GetName() == aName)
      {
         attachmentPtr = iter->second;
      }
   }
   return attachmentPtr;
}

//! Method: GetEntityList
//! Description: Returns a list of all entities
//!
//! @param aEntityList The returned list of entities.

// public
void VaScenario::GetEntityList(VaEntityList& aEntityList) const
{
   for (auto&& entity : mEntityMap)
   {
      aEntityList.push_back(entity.second);
   }
}

//! Method: GetSelectedEntityList
//! Description: Returns the list of entities that are selected
//!
//! @param aViewerPtr The viewer the entities are in
//! @param aEntityList The returned list of entities.

// public
void VaScenario::GetSelectedEntityList(VaViewer* aViewerPtr, VaEntityList& aEntityList) const
{
   for (VaEntityMap::const_iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      VaEntity* tempEntityPtr = (*iter).second;

      // if the Entity is selected, add it to the list
      if (tempEntityPtr->GetSelected(aViewerPtr))
      {
         aEntityList.push_back(tempEntityPtr);
      }
   }
}

bool VaScenario::BoxSelect(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside, bool aAddSelection)
{
   bool selected = false;
   if (aAddSelection == false)
   {
      // Clear the selection if aAddSelection method flag is false.
      aViewerPtr->ClearSelection();
   }

   std::vector<VaEntity*> selectedEntities;
   for (VaEntityMap::iterator iter = mEntityMap.begin(); iter != mEntityMap.end(); ++iter)
   {
      VaEntity* entityPtr      = (*iter).second;
      bool      entitySelected = false;

      auto& attachments = entityPtr->GetAttachments();
      for (size_t aIndex = 0; aIndex < attachments.size(); ++aIndex)
      {
         if (attachments[aIndex]->IsMyViewer(aViewerPtr))
         {
            VaAttachment* attachmentPtr = attachments[aIndex].get();

            switch (attachmentPtr->GetPickMode() & VaAttachment::cPICK_MODE_MASK)
            {
            case VaAttachment::cPICK_ENTITY:
               entitySelected = true;
               break;
            case VaAttachment::cPICK_ATTACHMENT:
            case VaAttachment::cPICK_SUBPARTS:
               selected = true;
               if (attachmentPtr->IsVisible() && attachmentPtr->BoxPick(aViewerPtr, aBox, aInside))
               {
                  selected = true;
               }
               break;
            case VaAttachment::cPICK_NOTHING:
               // Don't add the attachment to the hit list.
               break;
            default:
               // Don't add the attachment to the hit list.
               break;
            }
         }
      }
      // Pick the entity down here so that it is the last thing "selected"
      // so that the player panel is shown not the waypoint panel.
      if (entitySelected)
      {
         if (entityPtr->BoxPick(aViewerPtr, aBox, aInside))
         {
            selectedEntities.push_back(entityPtr);
            selected = true;
         }
      }
   }
   // Publish a list of selected entity.
   VaObserver::EntitiesSelected(aViewerPtr, 1, selectedEntities);
   return selected;
}

bool VaScenario::DeleteSelection(VaViewer* aViewerPtr, VaEntityMap& aEntityMap, VaAttachmentList& aAttachmentList)
{
   for (auto* attachmentPtr : aAttachmentList)
   {
      VaAttachment::DeleteAction deleteMode = attachmentPtr->DeleteSelection(aViewerPtr);
      if (deleteMode != VaAttachment::cDELETE_NOTHING)
      {
         // The attachment decided that it should be deleted.
         VaEntity& entity(attachmentPtr->GetParent());
         entity.RemoveAttachment(attachmentPtr->GetUniqueId());

         // Add the attachment's entity to the selection list, so that it is deleted.
         if (deleteMode == VaAttachment::cDELETE_ENTITY)
         {
            // if for some reason entity is not in the list, add it to be deleted in next loop
            // also for multiple attachments with same parent, only adds once
            aEntityMap.emplace(entity.GetUniqueId(), &entity);
         }
      }
   }
   for (const auto& entity : aEntityMap)
   {
      VaEntity* entityPtr = entity.second;
      if (entityPtr->IsUserDeletable())
      {
         // Delete the entity through the environment.
         VaEnvironment::Instance().DeleteEntity(entityPtr);
      }
      else
      {
         entityPtr->SetSelected(aViewerPtr, false);
      }
   }
   return false;
}

void VaScenario::MoveSelection(VaViewer*          aViewerPtr,
                               const VaEntityMap& aEntityMap,
                               VaAttachmentList&  aAttachmentList,
                               UtEntity&          aRefBefore,
                               UtEntity&          aRefAfter,
                               int                aFeedbackFormat,
                               double             aFeedbackPrecision,
                               bool               aOverrideModifiable)
{
   if (IsModifiable() || aOverrideModifiable)
   {
      for (auto&& ent : aEntityMap)
      {
         VaEntity* entityPtr = ent.second;

         // Only move entities that can be dragged.
         if (entityPtr->GetDraggable())
         {
            // Request the entity's position in LLA, that way relative entities
            // will do the appropriate conversions to LLA for us.
            VaPosition pos(entityPtr->GetPosition());
            double     entityLLA[3] = {pos.GetLat(), pos.GetLon(), pos.GetAlt()};

            double ned[3];
            aRefBefore.ConvertLLAToNED(entityLLA[0], entityLLA[1], entityLLA[2], ned);
            aRefAfter.ConvertNEDToLLA(ned, entityLLA[0], entityLLA[1], entityLLA[2]);

            // Set the entity's new position.
            pos.SetLat(entityLLA[0]);
            pos.SetLatFormat(VaEnvironment::Instance().GetUnitFormat().mLatLonFormat);
            pos.SetLon(entityLLA[1]);
            pos.SetLonFormat(VaEnvironment::Instance().GetUnitFormat().mLatLonFormat);
            pos.SetAlt(entityLLA[2]);
            pos.SetAltFormat(VaEnvironment::Instance().GetUnitFormat().mAltFormat);
            entityPtr->SetPosition(pos);
         }
      }

      // Only tell the attachment to move once, by keeping track of previous attachment visited
      VaAttachment* previousAttachmentPtr = nullptr;
      for (auto&& att : aAttachmentList)
      {
         if (previousAttachmentPtr == att)
            continue; // only move the attachment once
         previousAttachmentPtr = att;

         // If the parent entity has already been moved, indicate so with the cMR_ENTITY_MOVED mask.
         unsigned int moveRelative = cMR_SELECTED;
         if (aEntityMap.find(att->GetParent().GetUniqueId()) != aEntityMap.end())
         {
            moveRelative |= cMR_ENTITY_MOVED;
         }
         att->MoveRelative(aRefBefore, aRefAfter, aFeedbackFormat, aFeedbackPrecision, (MoveRelativeMask)moveRelative, aViewerPtr);
      }
   }
}

void VaScenario::MoveSelectionComplete(VaViewer* aViewerPtr, VaEntityMap& aEntityMap, VaAttachmentList& aAttachmentList)
{
   for (VaEntityMap::iterator iter = aEntityMap.begin(); iter != aEntityMap.end(); ++iter)
   {
      VaEntity* entityPtr = iter->second;
      if (entityPtr && entityPtr->GetDraggable())
      {
         // Only move entities that can be dragged.
         entityPtr->MoveComplete();
         entityPtr->RotateComplete();
      }
   }
   for (VaAttachmentList::const_iterator iter = aAttachmentList.begin(); iter != aAttachmentList.end(); ++iter)
   {
      VaAttachment* attachmentPtr = *iter;
      if (attachmentPtr)
      {
         attachmentPtr->MoveComplete();
         attachmentPtr->GetParent().MoveComplete();
      }
   }
}

void VaScenario::RotateSelection(VaViewer*         aViewerPtr,
                                 VaEntityMap&      aEntityMap,
                                 VaAttachmentList& aAttachmentList,
                                 const UtLLAPos&   aCenterLLA,
                                 double            aAngle)
{
   if (IsModifiable())
   {
      if (!aEntityMap.empty())
      {
         for (VaEntityMap::iterator it = aEntityMap.begin(); it != aEntityMap.end(); ++it)
         {
            it->second->Rotate(aCenterLLA, aAngle);
         }
      }
   }
}

//! Method: GetPlatformTypes
//! Description: Returns a list of platform types that are available for the current scenario.
//!
//! @param aPlatformTypes The list of platform types (filled by reference).

// public
void VaScenario::GetPlatformTypes(PlatformTypeList& aPlatformTypes) const
{
   aPlatformTypes = mPlatformTypes;
}

bool VaScenario::AddPlatformType(const std::string& aPlatformType)
{
   bool added = false;

   PlatformTypeList::const_iterator iter = std::find(mPlatformTypes.begin(), mPlatformTypes.end(), aPlatformType);
   if (iter == mPlatformTypes.end())
   {
      mPlatformTypes.push_back(aPlatformType);
      added = true;
   }
   return added;
}

// protected
void VaScenario::SetPlatformTypes(const PlatformTypeList& aPlatformTypes)
{
   mPlatformTypes = aPlatformTypes;
}

void VaScenario::AttachmentAdded(VaAttachment& aAttachment)
{
   AddAttachment(&aAttachment);
   VaObserver::AttachmentAdded(&aAttachment);
}

void VaScenario::AttachmentRemoved(VaAttachment& aAttachment)
{
   if (mAttachmentMap.find(aAttachment.GetUniqueId()) != mAttachmentMap.end())
   {
      RemoveAttachment(&aAttachment, 0);
   }
}

void VaScenario::EntityDeletedCB(VaEntity* aEntityPtr)
{
   if ((aEntityPtr != nullptr) && (aEntityPtr->GetScenarioKey() == GetKey()))
   {
      aEntityPtr->SetSelected(nullptr, false);
      RemoveEntity(aEntityPtr);
      DirtyTimeRange();
   }
}

// virtual
void VaScenario::DirtyTimeRange()
{
   mTimeRangeDirty = true;
   VaEnvironment::Instance().DirtyTimeRange();
}

// virtual
void VaScenario::GetTimeRange(double& aStartTime, double& aEndTime)
{
   if (mTimeRangeDirty)
   {
      bool first = true;
      for (VaEntityMap::iterator entityIter = mEntityMap.begin(); entityIter != mEntityMap.end(); ++entityIter)
      {
         double startTime = 0.0;
         double endTime   = 0.0;

         (*entityIter).second->GetTimeRange(startTime, endTime);
         if (first)
         {
            mStartTime = startTime;
            mEndTime   = endTime;
            first      = false;
         }
         else
         {
            if (startTime < mStartTime)
            {
               mStartTime = startTime;
            }
            if (endTime > mEndTime)
            {
               mEndTime = endTime;
            }
         }
      }
      if (first)
      {
         mStartTime = 0;
         mEndTime   = 0;
      }
      mTimeRangeDirty = false;
   }
   aStartTime = mStartTime;
   aEndTime   = mEndTime;
}

void VaScenario::EntityNameChangedCB(const std::string& aOldName, const std::string& aNewName, VaEntity* aEntityPtr)
{
   auto range = mEntityNameMap.equal_range(aOldName);

   for (auto i = range.first; i != range.second; ++i)
   {
      if (i->second->GetUniqueId() == aEntityPtr->GetUniqueId())
      {
         i = mEntityNameMap.erase(i);
         break;
      }
   }
   mEntityNameMap.emplace(std::make_pair(aNewName, aEntityPtr));
}

} // end namespace vespa
