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

#include "OrbitPreviewManager.hpp"

#include "OrbitPreviewEntity.hpp"
#include "OrbitalMissionModel.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

namespace wsfg
{

namespace spaceg
{

//! Remove all orbit previews.
void OrbitPreviewManager::ClearOrbitPreviews()
{
   auto scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      for (auto& iter : mPreviews)
      {
         scenarioPtr->DeleteEntity(iter.second);
      }
   }
   mPreviews.clear();
}

//! Updates the orbit previews for the events with the given @p aIndex.
void OrbitPreviewManager::UpdateOrbitPreviews(const OrbitalMissionModel& aModel, const QModelIndex& aIndex)
{
   HideOrDeletePreview(aModel);

   if (aIndex.isValid())
   {
      QModelIndex firstCol = aModel.index(aIndex.row(), 0, aIndex.parent());
      if (aModel.hasChildren(firstCol))
      {
         UpdateAllDescendants(aModel, firstCol);
      }
      else
      {
         UpdateSingleOrbitPreview(aModel, firstCol);
      }
   }
}

//! Recreate the orbit previews for all events in the given @p aModel.
void OrbitPreviewManager::RecreateOrbitPreviews(const OrbitalMissionModel& aModel)
{
   ClearOrbitPreviews();
   auto createPreview =
      [this, &aModel](const QModelIndex& aIndex, int aOrderedIndex, int aParent, int aRow, OrbitalMissionItem* aItemPtr)
   {
      auto eventOrbits = aModel.GetEventOrbitDetails(aIndex);
      if (eventOrbits.IsValid() && vaEnv.GetStandardScenario())
      {
         OrbitPreviewEntity* previewPtr = new OrbitPreviewEntity{eventOrbits.mEventName};
         if (!vaEnv.GetStandardScenario()->AddEntity(previewPtr))
         {
            delete previewPtr;
         }
         else
         {
            previewPtr->SetColor(eventOrbits.mPreviewColor);
            previewPtr->UpdateEventDetails(eventOrbits);
            previewPtr->SetVisible(false);
            mPreviews[aIndex] = previewPtr;
         }
      }
   };
   aModel.Traverse(createPreview);
}

void OrbitPreviewManager::HideOrDeletePreview(const OrbitalMissionModel& aModel)
{
   auto scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      auto iter = mPreviews.begin();
      while (iter != mPreviews.end())
      {
         iter->second->SetVisible(false);
         if (!aModel.GetEventOrbitDetails(iter->first).IsValid())
         {
            scenarioPtr->DeleteEntity(iter->second);
            iter = mPreviews.erase(iter);
         }
         else
         {
            ++iter;
         }
      }
   }
}

void OrbitPreviewManager::UpdateSingleOrbitPreview(const OrbitalMissionModel& aModel, const QModelIndex& aIndex)
{
   auto iter = mPreviews.find(aIndex);
   if (iter != mPreviews.end())
   {
      auto eventDetails = aModel.GetEventOrbitDetails(aIndex);
      if (eventDetails.IsValid())
      {
         iter->second->SetVisible(true);
      }
   }
}

void OrbitPreviewManager::UpdateAllDescendants(const OrbitalMissionModel& aModel, const QModelIndex& aParentIndex)
{
   auto action =
      [this, &aModel](const QModelIndex& aIndex, int aOrderedIndex, int aParent, int aRow, OrbitalMissionItem* aItemPtr)
   { UpdateSingleOrbitPreview(aModel, aIndex); };
   aModel.Traverse(action, aParentIndex);
}

} // namespace spaceg

} // namespace wsfg
