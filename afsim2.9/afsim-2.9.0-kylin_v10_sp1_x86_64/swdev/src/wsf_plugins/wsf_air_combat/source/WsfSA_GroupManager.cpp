// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_GroupManager.hpp"

#include "WsfSA_Group.hpp"
#include "WsfSA_GroupUtils.hpp"
#include "WsfSA_Processor.hpp"

void WsfSA_GroupManager::InitializeData(WsfPlatform* aPlatform,
                                        double       aMaxGroupingDistanceCentroid,
                                        double       aMaxGroupingDistanceNeighbor,
                                        double       aMaxGroupingHeadingDifference,
                                        double       aMaxGroupingSpeedDifference,
                                        double       aMaxGroupingAngle,
                                        double       aMinGroupRadius,
                                        bool         aUseCentroidGrouping,
                                        bool         aUseNeighborGrouping,
                                        bool         aUseHeadingGrouping,
                                        bool         aUseSpeedGrouping,
                                        bool         aUseTypeGrouping)
{
   mPlatform                     = aPlatform;
   mMaxGroupingDistanceCentroid  = aMaxGroupingDistanceCentroid;
   mMaxGroupingDistanceNeighbor  = aMaxGroupingDistanceNeighbor;
   mMaxGroupingHeadingDifference = aMaxGroupingHeadingDifference;
   mMaxGroupingSpeedDifference   = aMaxGroupingSpeedDifference;
   mMinGroupRadius_nm            = aMinGroupRadius * UtMath::cNM_PER_M;
   mUseCentroidGrouping          = aUseCentroidGrouping;
   mUseNeighborGrouping          = aUseNeighborGrouping;
   mUseHeadingGrouping           = aUseHeadingGrouping;
   mUseSpeedGrouping             = aUseSpeedGrouping;
   mUseTypeGrouping              = aUseTypeGrouping;
}

void WsfSA_GroupManager::UpdateAndAssessGroups(double                                      aSimtime,
                                               const WsfPlatform*                          aPlatform,
                                               std::list<WsfSA_Group*>&                    aCurrentlyPerceivedGroups,
                                               const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                                               const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                                               bool                                        aIgnoreBogies)
{
   double deltaT = aSimtime - mLastSimtime;

   if (deltaT < std::numeric_limits<double>::epsilon())
   {
      mLastSimtime = aSimtime;
      return;
   }

   // TODO -- Support other algorithms besides simple
   switch (mGroupingAlgorithm)
   {
   case SIMPLE_POSITIONAL:
      PositionalAlgorithm(aSimtime, aPlatform, aCurrentlyPerceivedGroups, aPerceivedBogies, aPerceivedBandits, aIgnoreBogies);
      break;
   case SIMPLE_ANGULAR:
   case K_MEANS:
   case HIERARCHICAL_TREE_AVERAGE:
   case HIERARCHICAL_TREE_COMPLETE:
   case HIERARCHICAL_TREE_SINGLE:
   default:
      PositionalAlgorithm(aSimtime, aPlatform, aCurrentlyPerceivedGroups, aPerceivedBogies, aPerceivedBandits, aIgnoreBogies);
      break;
   }

   mLastSimtime = aSimtime;
}

void WsfSA_GroupManager::SetGroupingAlgorithms(GroupingAlgorithm aGroupingAlgorithm, ScoringAlgorithm aScoringAlgorithm)
{
   if ((aGroupingAlgorithm >= K_MEANS) && (aGroupingAlgorithm <= HIERARCHICAL_TREE_SINGLE))
   {
      mGroupingAlgorithm = aGroupingAlgorithm;
   }

   if ((aScoringAlgorithm >= SCORE_2D) && (aScoringAlgorithm <= SCORE_ANG))
   {
      mScoringAlgorithm = aScoringAlgorithm;
   }
}

void WsfSA_GroupManager::PositionalAlgorithm(double                                      aSimtime,
                                             const WsfPlatform*                          aPlatform,
                                             std::list<WsfSA_Group*>&                    aCurrentlyPerceivedGroups,
                                             const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                                             const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                                             bool                                        aIgnoreBogies)
{
   // Create a list of available entities
   std::list<WsfSA_EntityPerception*> availableList;
   FormAvailableList(aPerceivedBogies, aPerceivedBandits, availableList, aIgnoreBogies);

   // Update the entity data within the groups (which persist and need updating each update)
   UpdateGroupEntityData(aCurrentlyPerceivedGroups, availableList);

   // If there are groups present, verify all groups contain valid entities
   if (!aCurrentlyPerceivedGroups.empty())
   {
      for (auto it = aCurrentlyPerceivedGroups.begin(); it != aCurrentlyPerceivedGroups.end();)
      {
         WsfSA_Group* group = *it;

         bool groupIsStillViable = VerifyGroupEntitiesAreValid(group, availableList, true);

         if (!groupIsStillViable)
         {
            // No longer viable, so remove it the group list
            it = aCurrentlyPerceivedGroups.erase(it);
         }
         else
         {
            // Increment
            ++it;
         }
      }
   }

   // If there are groups present, determine if the available entities can join any existing groups
   if (!aCurrentlyPerceivedGroups.empty())
   {
      for (auto it = availableList.begin(); it != availableList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         bool joinedAGroup = AttemptToJoinBestGroup(entity, aCurrentlyPerceivedGroups);
         if (joinedAGroup)
         {
            // No longer available, so remove it from the available list
            it = availableList.erase(it);
         }
         else
         {
            // Increment
            ++it;
         }
      }
   }

   // Now, try to form new groups using the availableList

   // Create a list of entities for consideration -- simply copy the available list
   std::list<WsfSA_EntityPerception*> considerationList;
   for (auto& entity : availableList)
   {
      considerationList.push_back(entity);
   }

   // Attempt to form groups using the available entities
   for (auto& entity : availableList)
   {
      AttemptToFormGroup(entity, considerationList, aCurrentlyPerceivedGroups);
   }

   // Remove from the availableList any items that were brought into groups using inSomeGroup
   for (auto it = availableList.begin(); it != availableList.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      if (EntityHasBeenAddedToSomeGroup(entity))
      {
         // Remove from the available list
         it = availableList.erase(it);
      }
      else
      {
         ++it;
      }
   }

   // Any entities remaining in the available list at this point were not able to join any groups.
   // Form degenerate groups (with single entities) with the remaining availableList entities.
   FormDegenerateGroups(availableList, aCurrentlyPerceivedGroups);

   // Remove any old "coasting" groups (with no entities) that have "timed-out" and expired
   RemoveOldGroups(aCurrentlyPerceivedGroups, aSimtime);

   // Update non-coasting groups
   UpdateCurrentGroups(aCurrentlyPerceivedGroups);

   // Merge Groups
   MergeGroups(aCurrentlyPerceivedGroups);
}

void WsfSA_GroupManager::FormAvailableList(const std::vector<WsfSA_EntityPerception*>& aPerceivedBogies,
                                           const std::vector<WsfSA_EntityPerception*>& aPerceivedBandits,
                                           std::list<WsfSA_EntityPerception*>&         aAvailableList,
                                           bool                                        aIgnoreBogies)
{
   if (!aIgnoreBogies)
   {
      for (auto& entity : aPerceivedBogies)
      {
         aAvailableList.push_back(entity);
      }
   }

   for (auto& entity : aPerceivedBandits)
   {
      aAvailableList.push_back(entity);
   }
}

bool WsfSA_GroupManager::VerifyGroupEntitiesAreValid(WsfSA_Group*                        aGroup,
                                                     std::list<WsfSA_EntityPerception*>& aAvailableList,
                                                     bool                                aTestForAvailable)
{
   // When aTestForAvailable is true, we verify if the entity is in the available list.
   // If not, the entity is removed from the group.
   bool entityRemovedSinceNotAvailable = false;
   if (aTestForAvailable)
   {
      for (auto it = aGroup->ModifiableElementList()->begin(); it != aGroup->ModifiableElementList()->end();)
      {
         WsfSA_EntityPerception* entity = *it;

         if (!EntityIsInList(entity, aAvailableList))
         {
            // Not in the list, we need to remove the item from the group
            entity->SetParentGroup(nullptr);
            it = aGroup->ModifiableElementList()->erase(it);
            if (IsLastItemFriendlyOrNeutral(aGroup))
            {
               // Return false to indicate the group should be removed/deleted
               return false;
            }
            else
            {
               // Only remove the item
               entityRemovedSinceNotAvailable = true;
            }
         }
         else
         {
            // Increment
            ++it;
         }
      }
   }

   // Now, loop through the entities in the group and verify each entity meets criteria to remain in the group,
   // including distance from the centroid, distance from neighbors, or angle from platform to entity and centroid.
   // If it does not meet the criteria, remove it from the group. If an entity is the last entity to be removed,
   // return false to indicate the group should be removed/deleted. Only do this if the group has not been set
   // as protected by script.
   bool entityWasRemovedDueToInvalidity = false;
   if (!aGroup->GetElementProtection())
   {
      for (auto it = aGroup->ModifiableElementList()->begin(); it != aGroup->ModifiableElementList()->end();)
      {
         WsfSA_EntityPerception* entity = *it;

         // Is this entity valid and should it remain in the group
         bool valid = VerifyEntityIsValidForGroup(entity, aGroup);

         if (!valid)
         {
            entity->SetParentGroup(nullptr);
            it = aGroup->ModifiableElementList()->erase(it);
            if (IsLastItemFriendlyOrNeutral(aGroup))
            {
               // Return false to indicate the group should be removed/deleted
               return false;
            }

            // Show that we removed an entity
            entityWasRemovedDueToInvalidity = true;
         }
         else
         {
            // Increment
            ++it;
         }
      }
   }

   // Update the centroid data if any entity was removed
   if (entityRemovedSinceNotAvailable || entityWasRemovedDueToInvalidity)
   {
      WsfSA_GroupUtils::RecalculateGroupCentroid(aGroup, mMinGroupRadius_nm);
   }

   // Now, for each entity that is still retained in the group, remove it from the availableList,
   // since it is no longer valid to join other groups
   for (auto& entity : aGroup->GetElementList())
   {
      RemoveEntityFromList(entity, aAvailableList);
   }

   // Return true to indicate the group should be retained
   return true;
}

bool WsfSA_GroupManager::VerifyEntityIsValidForGroup(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup)
{
   // Check for top-level qualifiers
   if (!MeetsTopLevelValidityGroupQualifiers(aEntity))
   {
      // This does not meet the top-level validity tests, so this in not valid
      return false;
   }

   // Check for a degenerate group (single entity) consisting of the entity itself
   if (aGroup->GetElementList().size() == 1)
   {
      if (EntityIsInList(aEntity, aGroup->GetElementList()))
      {
         // This is a degenerate list of only the entity, so this is valid
         return true;
      }
   }

   return CloseEnoughToStay(aEntity, aGroup);
}

bool WsfSA_GroupManager::VerifyEntityIsValidForJoiningGroup(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup)
{
   // Check for top-level qualifiers
   if (!MeetsTopLevelValidityGroupQualifiers(aEntity))
   {
      // This does not meet the top-level validity tests, so this in not valid
      return false;
   }

   // Check for a degenerate group (single entity) consisting of the entity itself
   if (aGroup->GetElementList().size() == 1)
   {
      if (EntityIsInList(aEntity, aGroup->GetElementList()))
      {
         // This is a degenerate list of only the entity, so this is valid
         return true;
      }
   }

   return CloseEnoughToJoin(aEntity, aGroup);
}

bool WsfSA_GroupManager::AttemptToJoinBestGroup(WsfSA_EntityPerception*        aEntity,
                                                const std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups)
{
   if (aCurrentlyPerceivedGroups.empty())
   {
      // No groups to join, so did not join a group
      return false;
   }

   // Check for top-level qualifiers
   if (!MeetsTopLevelValidityGroupQualifiers(aEntity))
   {
      // Does not meet top-level criteria, so did not join a group
      return false;
   }

   // We seek to join the best group, which is any valid group to join with the closest centroid to the entity

   // Remember the best group
   WsfSA_Group* bestGroup = nullptr;

   // Remember the best distance
   double bestDistance = 2.0e13; // Note: This distance is greater than 100 times the distance from the earth to the sun

   // Loop through the groups
   for (auto& group : aCurrentlyPerceivedGroups)
   {
      if (!group->GetElementProtection() && CloseEnoughToJoin(aEntity, group))
      {
         // This is a valid group to join, so check the range
         double dist = WsfSA_GroupUtils::DistanceFromCentroid(group, aEntity);

         // This group may be joined, but is it the best
         if (dist < bestDistance)
         {
            // It is the best
            bestDistance = dist;
            bestGroup    = group;
         }
      }
   }

   if (bestGroup != nullptr)
   {
      // Join this group -- add the entity to the group
      bestGroup->AddElement(aEntity);

      // Need to recalculate the centroid since the group data is changed by joining
      WsfSA_GroupUtils::RecalculateGroupCentroid(bestGroup, mMinGroupRadius_nm);

      // Joined a group, so return true
      return true;
   }

   return false;
}

void WsfSA_GroupManager::AttemptToFormGroup(WsfSA_EntityPerception*             aEntity,
                                            std::list<WsfSA_EntityPerception*>& aConsiderationList,
                                            std::list<WsfSA_Group*>&            aCurrentlyPerceivedGroups)
{
   // Start by removing the current available entity from the considerationList
   RemoveEntityFromList(aEntity, aConsiderationList);

   // Before considering this entity further, verify that it may be added to a new group
   if (!ItemMayBeAddedToGroup(aEntity))
   {
      // Cannot add this entity, so simply return
      return;
   }

   // Check for top-level qualifiers
   if (!MeetsTopLevelValidityGroupQualifiers(aEntity))
   {
      return;
   }

   // Construct WsfGeoPoint for the entity (this will be used below)
   WsfGeoPoint entityPt =
      WsfGeoPoint(aEntity->GetLat_deg(), aEntity->GetLon_deg(), aEntity->GetAltitude_ft() * UtMath::cM_PER_FT);

   bool                    formedGroup  = false;
   WsfSA_Group*            newGroup     = nullptr;
   WsfSA_EntityPerception* tgtEntityPtr = nullptr;

   // Loop through the list and find the first entity with which to form a group
   for (auto& tgtEntity : aConsiderationList)
   {
      if (MeetsTopLevelValidityGroupQualifiers(tgtEntity))
      {
         if (ItemMayBeAddedToGroup(tgtEntity))
         {
            // Construct WsfGeoPoint for the target entity
            WsfGeoPoint tgtEntityPt = WsfGeoPoint(tgtEntity->GetLat_deg(),
                                                  tgtEntity->GetLon_deg(),
                                                  tgtEntity->GetAltitude_ft() * UtMath::cM_PER_FT);

            // Calc distance
            double dist = entityPt.GetDistanceFrom(tgtEntityPt);
            if ((dist <= (2.0 * mMaxGroupingDistanceCentroid)) || (dist <= mMaxGroupingDistanceNeighbor))
            {
               // These entities can form a group

               // Create a new group
               newGroup = new WsfSA_Group(mPlatform);

               // Set data based on group manager values
               newGroup->SetPerceivedName(GenerateNewGroupName());
               newGroup->SetRadiusCurrent_nm(static_cast<float>(mMinGroupRadius_nm));
               newGroup->SetRadiusMinimum_nm(static_cast<float>(mMinGroupRadius_nm));
               // Add both entities to the group
               newGroup->AddElement(aEntity);
               newGroup->AddElement(tgtEntity);

               // Save the pointer to target entity
               tgtEntityPtr = tgtEntity;

               // Calculate centroid information for the newly formed group of two entities
               WsfSA_GroupUtils::RecalculateGroupCentroid(newGroup, mMinGroupRadius_nm);

               // Set flag
               formedGroup = true;

               // Break out of the loop
               break;
            }
         }
      }
   }

   if (formedGroup)
   {
      // Remove the target entity from the considerationList
      RemoveEntityFromList(tgtEntityPtr, aConsiderationList);

      // This needs to be performed repeatedly, as long as anything gets added
      bool addedToTheGroup = false;
      do
      {
         // Clear the flag
         addedToTheGroup = false;

         // Now, loop through the consideration list and determine if any of the other entities could join the new group
         for (auto& tgtEntity : aConsiderationList)
         {
            if (MeetsTopLevelValidityGroupQualifiers(tgtEntity))
            {
               // Only proceed if not in some group
               if (ItemMayBeAddedToGroup(tgtEntity))
               {
                  bool ableToJoinGroup = VerifyEntityIsValidForJoiningGroup(tgtEntity, newGroup);

                  if (ableToJoinGroup)
                  {
                     // Add the entity to the group
                     newGroup->AddElement(tgtEntity);

                     // Update centroid information, since the entity was added
                     WsfSA_GroupUtils::RecalculateGroupCentroid(newGroup, mMinGroupRadius_nm);

                     // Set the flag
                     addedToTheGroup = true;
                  }
               }
            }
         }
      } while (addedToTheGroup);

      // Add to the full list of groups
      aCurrentlyPerceivedGroups.emplace_back(newGroup);
   }
}

void WsfSA_GroupManager::FormDegenerateGroups(std::list<WsfSA_EntityPerception*>& aAvailableList,
                                              std::list<WsfSA_Group*>&            aCurrentlyPerceivedGroups)
{
   for (const auto& entity : aAvailableList)
   {
      FormDegenerateGroup(entity, aCurrentlyPerceivedGroups);
   }

   // Remove all entities from aAvailableList
   aAvailableList.clear();
}

void WsfSA_GroupManager::FormDegenerateGroup(WsfSA_EntityPerception*  aEntity,
                                             std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups)
{
   // Check for top-level qualifiers
   if (MeetsTopLevelValidityGroupQualifiers(aEntity) && !aEntity->GetParentGroup())
   {
      // Create new degenerate group
      WsfSA_Group* group = new WsfSA_Group(mPlatform);

      // Set data based on group manager values
      group->SetPerceivedName(GenerateNewGroupName());
      group->SetRadiusCurrent_nm(static_cast<float>(mMinGroupRadius_nm));
      group->SetRadiusMinimum_nm(static_cast<float>(mMinGroupRadius_nm));

      // Add the entity
      group->AddElement(aEntity);

      // Calculate centroid information
      WsfSA_GroupUtils::RecalculateGroupCentroid(group, mMinGroupRadius_nm);

      // Add to the full list of groups
      aCurrentlyPerceivedGroups.emplace_back(group);
   }
}

void WsfSA_GroupManager::UpdateGroupEntityData(std::list<WsfSA_Group*>&                  aCurrentlyPerceivedGroups,
                                               const std::list<WsfSA_EntityPerception*>& aAvailableList)
{
   // Loop through the perceived groups
   for (auto itGroup = aCurrentlyPerceivedGroups.begin(); itGroup != aCurrentlyPerceivedGroups.end();)
   {
      WsfSA_Group* group = *itGroup;

      // Loop through the perceived entities in the group
      for (auto it = group->ModifiableElementList()->begin(); it != group->ModifiableElementList()->end();)
      {
         WsfSA_EntityPerception* entity = *it;

         if (!EntityDataAvailable(entity, aAvailableList))
         {
            // We need to remove the item from the group
            entity->SetParentGroup(nullptr);
            it = group->ModifiableElementList()->erase(it);
         }
         else
         {
            ++it;
         }
      }

      if (IsLastItemFriendlyOrNeutral(group))
      {
         // Remove this group
         itGroup = aCurrentlyPerceivedGroups.erase(itGroup);
      }
      else
      {
         ++itGroup;
      }
   }
}

bool WsfSA_GroupManager::EntityDataAvailable(WsfSA_EntityPerception*                   aEntity,
                                             const std::list<WsfSA_EntityPerception*>& aAvailableList)
{
   return std::find(aAvailableList.begin(), aAvailableList.end(), aEntity) != aAvailableList.end();
}

void WsfSA_GroupManager::UpdateCurrentGroups(const std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups)
{
   // Update non-coasting groups
   for (auto& group : aCurrentlyPerceivedGroups)
   {
      // Not coasting if the list is not empty
      if (!group->GetElementList().empty())
      {
         // Update this non-coasting group
         WsfSA_GroupUtils::RecalculateGroupCentroid(group, mMinGroupRadius_nm);
      }
   }
}

void WsfSA_GroupManager::RemoveOldGroups(std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups, double aSimtime)
{
   // For each group in the currently perceived groups, check if each group is coasting
   // and if it has passed the coasting time limit.  If a group passes these criteria,
   // remove it from the currently perceived groups list.

   for (auto it = aCurrentlyPerceivedGroups.begin(); it != aCurrentlyPerceivedGroups.end();)
   {
      WsfSA_Group* group = *it;

      // If the group is coasting (0 platforms)
      if (group->GetElementList().empty())
      {
         // Remove the group from the perceived groups if it has been coasting too long.
         it = aCurrentlyPerceivedGroups.erase(it);
         delete group;
      }
      else
      {
         // Increment
         ++it;
      }
   }
}

void WsfSA_GroupManager::MergeGroups(std::list<WsfSA_Group*>& aCurrentlyPerceivedGroups)
{
   // The set of individual groups which are set to be merged with another.
   std::set<WsfSA_Group*> paired;
   // Each list within this list contains groups which will all be merged together.
   std::list<std::list<WsfSA_Group*>> toMerge;
   for (auto i = aCurrentlyPerceivedGroups.begin(); i != aCurrentlyPerceivedGroups.end(); ++i)
   {
      auto group1 = *i;
      if (group1->GetElementProtection() || paired.find(group1) != paired.end())
      {
         // Groups that are element protected won't be automatically merged.
         // Additionally, groups that are already in the "paired" set are skipped to prevent doubling up on merges.
         continue;
      }
      // Only check groups that haven't already been evaluated.
      for (auto j = i; j != aCurrentlyPerceivedGroups.end(); ++j)
      {
         auto group2 = *j;
         if (j == i || group1->GetFocus() != group2->GetFocus() || group2->GetElementProtection() ||
             paired.find(group2) != paired.end() || group2->GetNumElements() == 0)
         {
            // In addition to the restrictions on the groups chosen for the outer loop,
            // the inner loop groups must also have the same focus as the outer loop group
            // and have at least one element. The result of the latter is that coasting groups
            // will not attempt to merge with other coasting groups.
            continue;
         }
         bool merge = true;
         for (auto entity : group2->GetElementList())
         {
            if (!VerifyEntityIsValidForJoiningGroup(entity, group1))
            {
               // All entities in the second group must be valid for the first, or else they won't merge.
               merge = false;
               break;
            }
         }
         if (merge)
         {
            // If successful, both are inserted to the "paired" set, as well as the list of mergers.
            toMerge.emplace_back(std::list<WsfSA_Group*>{group1, group2});
            paired.insert(group1);
            paired.insert(group2);
            break;
         }
      }
   }
   // Finally, loop through the merges to perform, and perform them.
   for (auto pair : toMerge)
   {
      WsfSA_GroupUtils::MergeGroups(aCurrentlyPerceivedGroups, pair, mPlatform, mMinGroupRadius_nm);
   }
}

std::string WsfSA_GroupManager::GenerateNewGroupName()
{
   // Increment the group number
   ++mGroupNumber;

   // Setup the name using a G (for group) plus the group number
   std::string name = "G" + std::to_string(mGroupNumber);

   return name;
}

bool WsfSA_GroupManager::EntityIsInList(const WsfSA_EntityPerception*             aEntity,
                                        const std::list<WsfSA_EntityPerception*>& aAvailableList) const
{
   for (auto& tstEntity : aAvailableList)
   {
      if (aEntity->IsSame(tstEntity))
      {
         // Match found, so return true
         return true;
      }
   }

   return false;
}

bool WsfSA_GroupManager::EntityIsInList(const WsfSA_EntityPerception*            aEntity,
                                        const std::list<WsfSA_EntityPerception>& aAvailableList) const
{
   for (auto& tstEntity : aAvailableList)
   {
      if (aEntity->IsSame(&tstEntity))
      {
         // Match found, so return true
         return true;
      }
   }

   return false;
}

void WsfSA_GroupManager::RemoveEntityFromList(const WsfSA_EntityPerception*       aEntity,
                                              std::list<WsfSA_EntityPerception*>& aAvailableList)
{
   for (auto it = aAvailableList.begin(); it != aAvailableList.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      if (aEntity->IsSame(entity))
      {
         // This is a match, so remove it
         it = aAvailableList.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

bool WsfSA_GroupManager::ItemMayBeAddedToGroup(const WsfSA_EntityPerception* aEntity) const
{
   return !EntityHasBeenAddedToSomeGroup(aEntity);
}

bool WsfSA_GroupManager::EntityHasBeenAddedToSomeGroup(const WsfSA_EntityPerception* aEntity) const
{
   return aEntity->GetParentGroup() != nullptr;
}

bool WsfSA_GroupManager::EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity, const WsfPlatform* aPlatform) const
{
   WsfSA_Processor* saProc = WsfSA_Processor::GetSA_Processor(*aPlatform);

   if (saProc != nullptr)
   {
      return saProc->EntityIsInAssetGroup(aEntity);
   }

   return false;
}

bool WsfSA_GroupManager::MeetsTopLevelValidityGroupQualifiers(const WsfSA_EntityPerception* aEntity) const
{
   // There are certain immediate disqualifiers for being in a group, so we test for those first
   if (mPlatform != nullptr)
   {
      if (aEntity->GetPerceivedIndex() == mPlatform->GetIndex())
      {
         // The owning platform may not be in a group, so simply return
         return false;
      }

      if (EntityIsInAssetGroup(aEntity, mPlatform))
      {
         // A known asset may not be in a group, so simply return
         return false;
      }
   }

   // Check for being a friendly
   if (aEntity->GetIdentification() == WsfSA_EntityPerception::FRIENDLY)
   {
      // A known friendly may not be in a group, so simply return
      return false;
   }

   // Check for being a neutral
   if (aEntity->GetIdentification() == WsfSA_EntityPerception::NEUTRAL)
   {
      // A known neutral may not be in a group, so simply return
      return false;
   }

   // Check for being an angle-only track
   if (aEntity->GetAngleOnly())
   {
      // An angle-only track may not be in a group, so simply return
      return false;
   }

   return true;
}

bool WsfSA_GroupManager::IsLastItemFriendlyOrNeutral(const WsfSA_Group* aGroup) const
{
   // Must be the last item
   if (aGroup->GetElementList().size() == 1)
   {
      const WsfSA_EntityPerception* entity = *(aGroup->GetElementList().begin());
      if (entity != nullptr && ((entity->GetIdentification() == WsfSA_EntityPerception::FRIENDLY) ||
                                (entity->GetIdentification() == WsfSA_EntityPerception::NEUTRAL)))
      {
         return true;
      }
   }

   return false;
}

WsfSA_GroupManager::GroupingValidity WsfSA_GroupManager::CloseEnoughToCentroid(const WsfSA_EntityPerception* aEntity,
                                                                               const WsfSA_Group* aGroup) const
{
   if (!mUseCentroidGrouping)
   {
      return INCONCLUSIVE;
   }
   // Construct WsfGeoPoint for the entity
   WsfGeoPoint entityPt =
      WsfGeoPoint(aEntity->GetLat_deg(), aEntity->GetLon_deg(), aEntity->GetAltitude_ft() * UtMath::cM_PER_FT);

   // Construct WsfGeoPoint for the centroid
   WsfGeoPoint centroidPt =
      WsfGeoPoint(aGroup->GetLat_deg(), aGroup->GetLon_deg(), aGroup->GetAltitude_ft() * UtMath::cM_PER_FT);

   double dist = entityPt.GetDistanceFrom(centroidPt);
   if (dist <= mMaxGroupingDistanceCentroid)
   {
      // Close enough to centroid - passed
      return VALID;
   }

   // Failed
   return INVALID;
}

WsfSA_GroupManager::GroupingValidity WsfSA_GroupManager::CloseEnoughToNeighbor(const WsfSA_EntityPerception* aEntity,
                                                                               const WsfSA_Group* aGroup) const
{
   if (!mUseNeighborGrouping || aGroup->GetElementList().empty())
   {
      // No elements, so failed
      return INCONCLUSIVE;
   }

   // Construct WsfGeoPoint for the entity
   WsfGeoPoint entityPt =
      WsfGeoPoint(aEntity->GetLat_deg(), aEntity->GetLon_deg(), aEntity->GetAltitude_ft() * UtMath::cM_PER_FT);

   // Test distance to neighbors, we only need to be close enough to one to be valid
   for (auto& entity : aGroup->GetElementList())
   {
      // Only proceed if not the same entity
      if (!aEntity->IsSame(entity))
      {
         // Construct WsfGeoPoint from the neighbor's location
         WsfGeoPoint neighborPt =
            WsfGeoPoint(entity->GetLat_deg(), entity->GetLon_deg(), entity->GetAltitude_ft() * UtMath::cM_PER_FT);

         double dist = entityPt.GetDistanceFrom(neighborPt);
         if (dist <= mMaxGroupingDistanceNeighbor)
         {
            // Close enough to neighbor - passed
            return VALID;
         }
      }
   }

   // Failed
   return INVALID;
}

WsfSA_GroupManager::GroupingValidity WsfSA_GroupManager::CloseEnoughToHeading(const WsfSA_EntityPerception* aEntity,
                                                                              const WsfSA_Group* aGroup) const
{
   if (!mUseHeadingGrouping || !aGroup->GetHeadingValid() || !aEntity->GetHeadingValid())
   {
      return INCONCLUSIVE;
   }
   double difference_rad = UtMath::AngleDifference(aGroup->GetHeading_deg() * UtMath::cRAD_PER_DEG,
                                                   aEntity->GetHeading_deg() * UtMath::cRAD_PER_DEG);
   if (difference_rad < mMaxGroupingHeadingDifference)
   {
      return VALID;
   }
   else
   {
      return INVALID;
   }
}

WsfSA_GroupManager::GroupingValidity WsfSA_GroupManager::CloseEnoughToSpeed(const WsfSA_EntityPerception* aEntity,
                                                                            const WsfSA_Group*            aGroup) const
{
   if (!mUseSpeedGrouping || !aGroup->GetSpeedValid() || !aEntity->GetSpeedValid())
   {
      return INCONCLUSIVE;
   }
   double difference = std::abs(aGroup->GetSpeed_kts() - aEntity->GetSpeed_kts());
   if (difference < mMaxGroupingSpeedDifference)
   {
      return VALID;
   }
   else
   {
      return INVALID;
   }
}

WsfSA_GroupManager::GroupingValidity WsfSA_GroupManager::MatchesType(const WsfSA_EntityPerception* aEntity,
                                                                     const WsfSA_Group*            aGroup) const
{
   if (!mUseTypeGrouping || aGroup->GetElementList().empty())
   {
      return INCONCLUSIVE;
   }

   for (auto entity : aGroup->GetElementList())
   {
      if (aEntity->GetPerceivedType() != entity->GetPerceivedType())
      {
         return INVALID;
      }
   }
   return VALID;
}

bool WsfSA_GroupManager::CloseEnoughToStay(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup)
{
   std::list<GroupingValidity> results;
   results.push_back(CloseEnoughToCentroid(aEntity, aGroup));
   results.push_back(CloseEnoughToNeighbor(aEntity, aGroup));
   // results.push_back(MatchesType(aEntity, aGroup));

   return std::find(results.begin(), results.end(), VALID) != results.end() &&
          std::find(results.begin(), results.end(), INVALID) == results.end();
}

bool WsfSA_GroupManager::CloseEnoughToJoin(const WsfSA_EntityPerception* aEntity, const WsfSA_Group* aGroup)
{
   std::list<GroupingValidity> results;
   results.push_back(CloseEnoughToCentroid(aEntity, aGroup));
   results.push_back(CloseEnoughToNeighbor(aEntity, aGroup));
   results.push_back(CloseEnoughToHeading(aEntity, aGroup));
   results.push_back(CloseEnoughToSpeed(aEntity, aGroup));
   results.push_back(MatchesType(aEntity, aGroup));

   return std::find(results.begin(), results.end(), VALID) != results.end() &&
          std::find(results.begin(), results.end(), INVALID) == results.end();
}
