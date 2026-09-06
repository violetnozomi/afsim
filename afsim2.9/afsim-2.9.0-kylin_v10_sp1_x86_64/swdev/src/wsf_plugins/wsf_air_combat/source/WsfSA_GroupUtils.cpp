// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_GroupUtils.hpp"

#include "UtEntity.hpp"
#include "UtVec3dX.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"

namespace
{
/** A helper function for Prune which unfocuses a group while ensuring that importance is preserved.
 * @param aGroup  The group do demote.
 */
void DemoteGroup(WsfSA_Group* aGroup)
{
   auto importance = aGroup->GetImportance();
   aGroup->SetFocus(false);
   if (importance == WsfSA_PerceivedItem::IMPORTANT)
   {
      aGroup->SetImportance(importance);
   }
}
} // namespace

int WsfSA_GroupUtils::StringToSplitCriteria(const std::string& aEnumString)
{
   static const std::map<std::string, int> strMap = {{"maximum threat", cMAX_THREAT},
                                                     {"minimum threat", cMIN_THREAT},
                                                     {"maximum target", cMAX_TARGET},
                                                     {"minimum target", cMIN_TARGET},
                                                     {"maximum altitude", cMAX_ALTITUDE},
                                                     {"minimum altitude", cMIN_ALTITUDE},
                                                     {"maximum distance", cMAX_DISTANCE},
                                                     {"minimum distance", cMIN_DISTANCE},
                                                     {"rightmost from heading", cRIGHTMOST_FROM_HEADING},
                                                     {"leftmost from heading", cLEFTMOST_FROM_HEADING}};
   auto                                    ret    = strMap.find(aEnumString);
   if (ret != strMap.end())
   {
      return ret->second;
   }
   ut::log::warning() << "Invalid split criteria: " << aEnumString;
   return -1;
}

int WsfSA_GroupUtils::StringToGroupSorting(const std::string& aEnumString)
{
   static const std::map<std::string, int> strMap = {{"ascending threat", cASCENDING_THREAT},
                                                     {"descending threat", cDESCENDING_THREAT},
                                                     {"ascending target", cASCENDING_TARGET},
                                                     {"descending target", cDESCENDING_TARGET},
                                                     {"ascending altitude", cASCENDING_ALTITUDE},
                                                     {"descending altitude", cDESCENDING_ALTITUDE},
                                                     {"ascending distance", cASCENDING_DISTANCE},
                                                     {"descending distance", cDESCENDING_DISTANCE},
                                                     {"ascending entity count", cASCENDING_ENTITY_COUNT},
                                                     {"descending entity count", cDESCENDING_ENTITY_COUNT},
                                                     {"ascending radius", cASCENDING_RADIUS},
                                                     {"descending radius", cDESCENDING_RADIUS}};
   auto                                    ret    = strMap.find(aEnumString);
   if (ret != strMap.end())
   {
      return ret->second;
   }
   ut::log::warning() << "Invalid group sorting: " << aEnumString;
   return -1;
}

int WsfSA_GroupUtils::CalculateNumPIs(const std::list<WsfSA_PerceivedItem*>& aItemList)
{
   int ret = 0;
   for (auto& item : aItemList)
   {
      if (item->GetItemType() == WsfSA_PerceivedItem::GROUP)
      {
         auto* group = dynamic_cast<WsfSA_Group*>(item);
         if (group->GetNumElements() == 0)
         {
            // This is an empty group, so it's about to be deleted anyway.
            // Also, it doesn't count for any PIs.
            continue;
         }
         if (group->GetFocus())
         {
            // Count the number of elements in focused groups.
            ret += ut::cast_to_int(group->GetNumElements());
         }
         else
         {
            // Count unfocused groups as 1.
            ret += 1;
         }
      }
      else
      {
         // Count entity perceptions as 1.
         ret += 1;
      }
   }
   return ret;
}

int WsfSA_GroupUtils::CalculateNumPIs(const std::list<WsfSA_Group*>& aGroupList)
{
   int ret = 0;
   for (auto group : aGroupList)
   {
      if (group->GetNumElements() == 0)
      {
         // This is an empty group, so it's about to be deleted anyway.
         // Also, it doesn't count for any PIs.
         continue;
      }
      if (group->GetFocus())
      {
         // Count the number of elements in focused groups.
         ret += ut::cast_to_int(group->GetNumElements());
      }
      else
      {
         // Count unfocused groups as 1.
         ret += 1;
      }
   }
   return ret;
}

bool WsfSA_GroupUtils::RenameGroup(const std::list<WsfSA_Group*>& aGroupList, WsfSA_Group* aGroup, const std::string& aName)
{
   if (!aGroup)
   {
      return false;
   }

   // Ensure that there are no groups that already have that name.
   // This is so that one won't mask the other from FindGroup().
   for (auto& group : aGroupList)
   {
      if (group->GetPerceivedName() == aName)
      {
         return false;
      }
   }
   aGroup->SetPerceivedName(aName);
   return true;
}

WsfSA_Group* WsfSA_GroupUtils::CreateGroup(int                                       aPI_Limit,
                                           int                                       aPI_Current,
                                           std::list<WsfSA_Group*>&                  aGroupList,
                                           const WsfPlatform*                        aOwningPlatform,
                                           const std::list<WsfSA_EntityPerception*>& aElements,
                                           const std::string&                        aName,
                                           double                                    aMinGroupRadius_nm,
                                           bool                                      aProtect)
{
   int                     addedPIs = 0;
   std::list<WsfSA_Group*> groupsToDrop;
   bool                    hasEntities = false;
   for (auto element : aElements)
   {
      if (element == nullptr)
      {
         ut::log::warning() << "an entity passed to CreateGroup was null.";
         continue;
      }
      if (element->GetParentGroup())
      {
         if (!(element->GetParentGroup()->GetFocus()))
         {
            ++addedPIs;
            ut::log::warning() << "entity " << element->GetPerceivedName() << " is being removed from an unfocused group.";
         }
         if (element->GetParentGroup()->GetNumElements() == 1)
         {
            // This is a degenerate group, and should be dropped.
            groupsToDrop.push_back(element->GetParentGroup());
         }
         element->GetParentGroup()->RemoveElement(element);
      }
      hasEntities = true;
   }
   if (!hasEntities)
   {
      ut::log::warning() << "CreateGroup failed because groups without any entities cannot be created.";
      return nullptr;
   }
   if (aPI_Limit > -1 && addedPIs > 0 && aPI_Current + addedPIs > aPI_Limit)
   {
      ut::log::warning() << "CreateGroup failed because it would have exceeded the perceived item limit.";
      return nullptr;
   }

   // Create the group
   WsfSA_Group* group = new WsfSA_Group(aOwningPlatform);
   group->SetPerceivedName(aName);
   group->SetElementList(aElements);
   RecalculateGroupCentroid(group, aMinGroupRadius_nm);
   group->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
   group->SetElementProtection(aProtect);

   // If the last element was removed from a group in order to be put in this new one,
   // then that old group should be deleted.
   for (auto& dropped : groupsToDrop)
   {
      aGroupList.remove(dropped);
      delete dropped;
   }

   // Make sure the group ends up in the group list.
   aGroupList.push_back(group);
   return group;
}

bool WsfSA_GroupUtils::DisbandGroup(int                          aPI_Limit,
                                    int                          aPI_Current,
                                    std::list<WsfSA_Group*>&     aGroupList,
                                    WsfSA_Group*                 aGroup,
                                    std::function<std::string()> aNamingFunc,
                                    double                       aMinGroupRadius_nm,
                                    bool                         aProtect)
{
   if (!aGroup)
   {
      ut::log::warning() << "DisbandGroup failed because the given group is null.";
      return false;
   }
   if (aPI_Limit > -1 && !aGroup->GetFocus() && aPI_Current + aGroup->GetNumElements() > ut::cast_to_size_t(aPI_Limit) + 1)
   {
      ut::log::warning() << "DisbandGroup failed because it would have exceeded the perceived item limit.";
      return false;
   }
   auto elements = aGroup->GetElementList();

   // Each element gets its own degenerate group.
   for (auto element : elements)
   {
      aGroup->RemoveElement(element);
      WsfSA_Group* group = new WsfSA_Group(aGroup->OwningPlatform());
      group->AddElement(element);
      RecalculateGroupCentroid(group, aMinGroupRadius_nm);
      group->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
      group->SetPerceivedName(aNamingFunc());
      group->SetElementProtection(aProtect);
      aGroupList.push_back(group);
   }

   // Make sure the old original group gets removed.
   aGroupList.remove(aGroup);
   delete aGroup;
   return true;
}

WsfSA_Group* WsfSA_GroupUtils::MergeGroups(std::list<WsfSA_Group*>&       aGroupList,
                                           const std::list<WsfSA_Group*>& aGroups,
                                           const WsfPlatform*             aOwningPlatform,
                                           double                         aMinGroupRadius_nm,
                                           bool                           aProtect)
{
   std::list<WsfSA_EntityPerception*> elements;
   // Validate input first
   if (aGroups.empty())
   {
      ut::log::warning() << "MergeGroups failed because no groups were given to merge.";
      return nullptr;
   }
   bool focus = aGroups.front() && aGroups.front()->GetFocus();
   for (auto group : aGroups)
   {
      if (group == nullptr || focus != group->GetFocus())
      {
         // Only groups with the same focus can be merged.
         ut::log::warning() << "MergeGroups failed because the list of groups' focus states was non-uniform.";
         return nullptr;
      }
   }
   std::string name = aGroups.front()->GetPerceivedName();
   // Special consideration needs to be taken for coasting groups.
   // Solution: don't allow them to merge (we may just get rid of coasting groups anyway).
   // - Merging a coasting group with one that has entities would basically be the same as deleting it.
   // - Merging multiple coasting groups would defeat the point of their coasting.
   for (auto group : aGroups)
   {
      if (group->GetNumElements() > 0)
      {
         for (auto entity : group->GetElementList())
         {
            elements.push_back(entity);
         }
         aGroupList.remove(group);
         delete group;
      }
   }

   // Create the new group.
   if (elements.size() > 0)
   {
      WsfSA_Group* group = new WsfSA_Group(aOwningPlatform);
      group->SetElementList(elements);
      group->SetFocus(focus);
      group->SetPerceivedName(name);
      RecalculateGroupCentroid(group, aMinGroupRadius_nm);
      group->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
      group->SetElementProtection(aProtect);
      aGroupList.push_back(group);
      return group;
   }
   else
   {
      // all input groups were coasting, so nothing happens.
      return nullptr;
   }
}

std::pair<WsfSA_Group*, WsfSA_Group*> WsfSA_GroupUtils::SplitGroup(int                      aPI_Limit,
                                                                   int                      aPI_Current,
                                                                   std::list<WsfSA_Group*>& aGroupList,
                                                                   WsfSA_Group*             aGroup,
                                                                   int                      aNumElements,
                                                                   int                      aCriterion,
                                                                   WsfPlatform*             aPlatform,
                                                                   const std::string&       aNewGroupName,
                                                                   double                   aMinGroupRadius_nm,
                                                                   bool                     aProtect)
{
   if (aGroup == nullptr)
   {
      ut::log::warning() << "SplitGroup failed because the given group was null.";
      return {nullptr, nullptr};
   }
   if (aPI_Limit > -1 && !aGroup->GetFocus() && aPI_Current + 1 > aPI_Limit)
   {
      ut::log::warning() << "SplitGroup failed because it would have exceeded the perceived item limit.";
      return {nullptr, nullptr};
   }
   if (aNumElements < 1 || ut::cast_to_size_t(aNumElements) >= aGroup->GetNumElements())
   {
      ut::log::warning() << "SplitGroup failed because the number of elements to split off was invalid or would have "
                            "resulted in only one group.";
      return {nullptr, nullptr};
   }
   bool focus    = aGroup->GetFocus();
   auto elements = aGroup->GetElementList();
   // Using std::list for this gave me trouble, hence the existence of 'sortable'
   std::vector<WsfSA_EntityPerception*> sortable{elements.begin(), elements.end()};
   // Would a map<int, std::function> be better?
   switch (aCriterion)
   {
   case cMAX_THREAT:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetThreatLevel() < b->GetThreatLevel(); });
      break;
   }
   case cMIN_THREAT:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetThreatLevel() > b->GetThreatLevel(); });
      break;
   }
   case cMAX_TARGET:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetTargetValue() < b->GetTargetValue(); });
      break;
   }
   case cMIN_TARGET:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetTargetValue() > b->GetTargetValue(); });
      break;
   }
   case cMAX_ALTITUDE:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetAltitude_ft() < b->GetAltitude_ft(); });
      break;
   }
   case cMIN_ALTITUDE:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->GetAltitude_ft() > b->GetAltitude_ft(); });
      break;
   }
   case cMAX_DISTANCE:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [aPlatform](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->CalcRangeForPlatform(aPlatform) < b->CalcRangeForPlatform(aPlatform); });
      break;
   }
   case cMIN_DISTANCE:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [aPlatform](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b)
                       { return a->CalcRangeForPlatform(aPlatform) > b->CalcRangeForPlatform(aPlatform); });
      break;
   }
   case cRIGHTMOST_FROM_HEADING:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [aPlatform](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b) {
                          return a->AngleOffHeadingFromOwningPlatform_deg() < b->AngleOffHeadingFromOwningPlatform_deg();
                       });
      break;
   }
   case cLEFTMOST_FROM_HEADING:
   {
      std::stable_sort(sortable.begin(),
                       sortable.end(),
                       [aPlatform](WsfSA_EntityPerception* a, WsfSA_EntityPerception* b) {
                          return a->AngleOffHeadingFromOwningPlatform_deg() > b->AngleOffHeadingFromOwningPlatform_deg();
                       });
      break;
   }
   default:
      ut::log::warning() << "SplitGroup failed because the given split criteria (" << aCriterion << ") was invalid.";
      break;
   }
   // count backwards until we get the right number of entities to move.
   WsfSA_Group::ElementList elements2;
   elements.clear();
   for (size_t i = 0UL; i < sortable.size(); ++i)
   {
      if (i < sortable.size() - aNumElements)
      {
         elements.push_back(sortable[i]);
      }
      else
      {
         elements2.push_back(sortable[i]);
      }
   }
   std::string name = aGroup->GetPerceivedName();
   aGroupList.remove(aGroup);

   // Group 1 is the "original" (not literally - you can see that the real original
   // gets subsequently deleted)
   // Group 2 is the "new" group (i.e. the one with the number of entities given in the input.)
   WsfSA_Group* group1 = new WsfSA_Group(aGroup->OwningPlatform());
   WsfSA_Group* group2 = new WsfSA_Group(aGroup->OwningPlatform());
   delete aGroup;
   group1->SetElementList(elements);
   group2->SetElementList(elements2);
   group1->SetFocus(focus);
   group2->SetFocus(focus);
   RecalculateGroupCentroid(group1, aMinGroupRadius_nm);
   group1->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
   RecalculateGroupCentroid(group2, aMinGroupRadius_nm);
   group2->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
   group1->SetPerceivedName(name);
   group2->SetPerceivedName(aNewGroupName);
   if (aProtect)
   {
      group1->SetElementProtection(true);
      group2->SetElementProtection(true);
   }
   aGroupList.push_back(group1);
   aGroupList.push_back(group2);
   return {group1, group2};
}

bool WsfSA_GroupUtils::TransferEntity(int                      aPI_Limit,
                                      int                      aPI_Current,
                                      std::list<WsfSA_Group*>& aGroupList,
                                      WsfSA_EntityPerception*  aEntity,
                                      WsfSA_Group*             aGroup,
                                      double                   aMinGroupRadius_nm,
                                      bool                     aProtect)
{
   if (aEntity == nullptr)
   {
      ut::log::warning() << "TransferEntity failed because the given entity was null.";
      return false;
   }
   if (aGroup == nullptr)
   {
      ut::log::warning() << "TransferEntity failed because the given group was null.";
      return false;
   }
   if (aPI_Limit > -1 && aEntity->GetParentGroup() && !aEntity->GetParentGroup()->GetFocus() && aGroup->GetFocus() &&
       aPI_Current + 1 > aPI_Limit)
   {
      ut::log::warning() << "TransferEntity failed because it would have exceeded the perceived item limit.";
      return false;
   }

   auto oldGroup = aEntity->GetParentGroup();
   if (aProtect)
   {
      // The old group is an implicit party in this transaction, so it needs to be element protected.
      if (oldGroup)
      {
         oldGroup->SetElementProtection(true);
      }
      aGroup->SetElementProtection(true);
   }

   if (oldGroup)
   {
      if (oldGroup->GetNumElements() > 1)
      {
         // With one less element, the old group's centroid needs recalculated.
         oldGroup->RemoveElement(aEntity);
         RecalculateGroupCentroid(oldGroup, aMinGroupRadius_nm);
      }
      else
      {
         // If that was the last element in the old group, delete the group.
         // Note that this situation is the same as inserting a loose entity into a group.
         aGroupList.remove(oldGroup);
         delete oldGroup;
      }
   }
   // With one more element, the old group's centroid needs recalculated.
   aGroup->AddElement(aEntity);
   RecalculateGroupCentroid(aGroup, aMinGroupRadius_nm);
   return true;
}

bool WsfSA_GroupUtils::RemoveEntityFromGroup(int                      aPI_Limit,
                                             int                      aPI_Current,
                                             std::list<WsfSA_Group*>& aGroupList,
                                             WsfSA_EntityPerception*  aEntity,
                                             const std::string&       aDegenerateGroupName,
                                             double                   aMinGroupRadius_nm,
                                             bool                     aProtect)
{
   if (aEntity == nullptr)
   {
      ut::log::warning() << "RemoveEntityFromGroup failed because the given entity was null.";
      return false;
   }
   auto oldGroup = aEntity->GetParentGroup();
   if (oldGroup == nullptr)
   {
      ut::log::warning() << "RemoveEntityFromGroup failed because the entity's group was null.";
      return false;
   }
   if (aPI_Limit > -1 && !aEntity->GetParentGroup()->GetFocus() && aPI_Current + 1 > aPI_Limit)
   {
      ut::log::warning() << "RemoveEntityFromGroup failed because it would have exceeded the perceived item limit.";
      return false;
   }
   if (oldGroup->GetNumElements() < 2)
   {
      ut::log::warning() << "RemoveEntityFromGroup failed because the entity's group was degenerate.";
      return false;
   }
   if (aProtect)
   {
      oldGroup->SetElementProtection(true);
   }
   oldGroup->RemoveElement(aEntity);
   // The old group needs recalculated becase it now has one less element.
   RecalculateGroupCentroid(oldGroup, aMinGroupRadius_nm);

   // Create the new degenerate group.
   WsfSA_Group* group = new WsfSA_Group(oldGroup->OwningPlatform());
   group->SetElementProtection(aProtect);
   group->SetPerceivedName(aDegenerateGroupName);
   group->AddElement(aEntity);
   RecalculateGroupCentroid(group, aMinGroupRadius_nm);
   group->SetRadiusMinimum_nm(static_cast<float>(aMinGroupRadius_nm));
   aGroupList.push_back(group);
   return true;
}

bool WsfSA_GroupUtils::SetFocus(int aPI_Limit, int aPI_Current, WsfSA_Group* aGroup, bool aFocus, bool aProtect)
{
   if (aGroup->GetNumElements() < 2 && aFocus == false)
   {
      ut::log::warning() << "SetFocus failed because a degenerate group cannot be unfocused.";
      return false;
   }
   if (aPI_Limit > -1 && !aGroup->GetFocus() && aFocus &&
       ut::cast_to_size_t(aPI_Current) + aGroup->GetNumElements() > ut::cast_to_size_t(aPI_Limit + 1))
   {
      ut::log::warning() << "SetFocus failed because it would have exceeded the perceived item limit.";
      return false;
   }
   aGroup->SetFocus(aFocus);
   if (aProtect)
   {
      aGroup->SetFocusProtection(true);
   }
   return true;
}

void WsfSA_GroupUtils::Prune(int                              aPI_Limit,
                             std::list<WsfSA_PerceivedItem*>& aGroupList,
                             std::list<WsfSA_PerceivedItem*>& aToRemove)
{
   PruneRecursive(aPI_Limit, WsfSA_PerceivedItem::NOMINAL, true, aGroupList, aToRemove);
}

void WsfSA_GroupUtils::PruneRecursive(int                              aPI_Limit,
                                      WsfSA_PerceivedItem::Importance  aImportance,
                                      bool                             aPreserve,
                                      std::list<WsfSA_PerceivedItem*>& aGroupList,
                                      std::list<WsfSA_PerceivedItem*>& aToRemove)
{
   if (aPI_Limit < 0 || CalculateNumPIs(aGroupList) <= aPI_Limit)
   {
      return;
   }
   if (aImportance == WsfSA_PerceivedItem::IGNORED)
   {
      // This should never happen, but just in case...
      ut::log::warning() << "Importance of IGNORE passed to WsfSA_GroupUtils::Prune, using NOMINAL instead.";
      aImportance = WsfSA_PerceivedItem::NOMINAL;
   }

   // Try to demote
   auto it = aGroupList.begin();
   while (it != aGroupList.end() && CalculateNumPIs(aGroupList) > aPI_Limit)
   {
      // Only groups can demote, so we'll ignore individuals for now.
      if ((*it)->GetItemType() == WsfSA_PerceivedItem::GROUP)
      {
         auto group = dynamic_cast<WsfSA_Group*>(*it);
         if (group->GetImportance() == aImportance && !(aPreserve && group->GetFocusProtection()))
         {
            if (group->GetNumElements() > 1)
            {
               DemoteGroup(group);
            }
            ++it;
         }
         else if (group->GetImportance() == WsfSA_PerceivedItem::IGNORED)
         {
            aToRemove.emplace_back(group);
            it = aGroupList.erase(it);
            // A weird detail, but it's here because the group itself cannot
            // police its own focus based on changes to the modifiable element list.
            // Setting it to "true" here results in consistency with groups' usual behavior.
            group->SetFocus(true);
         }
         else
         {
            ++it;
         }
      }
      else
      {
         ++it;
      }
   }

   // Only when we can no longer demote, try to delete
   auto jt = aGroupList.begin();
   while (jt != aGroupList.end() && CalculateNumPIs(aGroupList) > aPI_Limit)
   {
      auto toRemove = jt;
      ++jt;
      if ((*toRemove)->GetItemType() == WsfSA_PerceivedItem::GROUP)
      {
         auto group = dynamic_cast<WsfSA_Group*>(*toRemove);
         if (group->GetImportance() == aImportance && (!group->GetFocus() || group->GetNumElements() < 2) &&
             !(aPreserve && group->GetElementProtection()))
         {
            aToRemove.emplace_back(*toRemove);
            aGroupList.erase(toRemove);
            // A weird detail, but it's here because the group itself cannot
            // police its own focus based on changes to the modifiable element list.
            // Setting it to "true" here results in consistency with groups' usual behavior.
            group->SetFocus(true);
         }
      }
      else
      {
         auto entity = dynamic_cast<WsfSA_EntityPerception*>(*toRemove);
         if (entity->GetImportance() == aImportance)
         {
            aToRemove.emplace_back(entity);
            aGroupList.erase(toRemove);
         }
      }
   }

   if (CalculateNumPIs(aGroupList) > aPI_Limit)
   {
      // We tried preserving "important" items, but still have too many PIs.
      if (aImportance == WsfSA_PerceivedItem::NOMINAL && aPreserve)
      {
         PruneRecursive(aPI_Limit, WsfSA_PerceivedItem::NOMINAL, false, aGroupList, aToRemove);
      }
      else if (aImportance == WsfSA_PerceivedItem::NOMINAL && !aPreserve)
      {
         PruneRecursive(aPI_Limit, WsfSA_PerceivedItem::IMPORTANT, true, aGroupList, aToRemove);
      }
      else if (aImportance == WsfSA_PerceivedItem::IMPORTANT && aPreserve)
      {
         PruneRecursive(aPI_Limit, WsfSA_PerceivedItem::IMPORTANT, false, aGroupList, aToRemove);
      }
   }
}

void WsfSA_GroupUtils::SortedGroups(int                            aSortingEnum,
                                    std::vector<WsfSA_Group*>&     aOutputGroups,
                                    const std::list<WsfSA_Group*>& aGroupList,
                                    const WsfPlatform*             aPlatform)
{
   aOutputGroups.clear();
   switch (aSortingEnum)
   {
   case WsfSA_GroupUtils::cASCENDING_THREAT:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetThreatLevel() < b->GetThreatLevel(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_THREAT:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetThreatLevel() >= b->GetThreatLevel(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cASCENDING_TARGET:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetTargetValue() < b->GetTargetValue(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_TARGET:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetTargetValue() >= b->GetTargetValue(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cASCENDING_ALTITUDE:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetAltitude_ft() < b->GetAltitude_ft(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_ALTITUDE:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetAltitude_ft() >= b->GetAltitude_ft(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cASCENDING_DISTANCE:
   {
      auto comp = [aPlatform](WsfSA_Group* a, WsfSA_Group* b)
      { return a->CalcRangeForPlatform(aPlatform) < b->CalcRangeForPlatform(aPlatform); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_DISTANCE:
   {
      auto comp = [aPlatform](WsfSA_Group* a, WsfSA_Group* b)
      { return a->CalcRangeForPlatform(aPlatform) >= b->CalcRangeForPlatform(aPlatform); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cASCENDING_ENTITY_COUNT:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetNumElements() < b->GetNumElements(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_ENTITY_COUNT:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetNumElements() >= b->GetNumElements(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cASCENDING_RADIUS:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetRadiusCurrent_nm() < b->GetRadiusCurrent_nm(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   case WsfSA_GroupUtils::cDESCENDING_RADIUS:
   {
      auto comp = [](WsfSA_Group* a, WsfSA_Group* b) { return a->GetRadiusCurrent_nm() >= b->GetRadiusCurrent_nm(); };
      for (auto& group : aGroupList)
      {
         aOutputGroups.insert(std::upper_bound(aOutputGroups.begin(), aOutputGroups.end(), group, comp), group);
      }
      break;
   }
   default:
   {
      ut::log::warning() << "SortedGroups failed because the sorting criteria was invalid.";
      break;
   }
   }
}

void WsfSA_GroupUtils::RecalculateGroupCentroid(WsfSA_Group* aGroup, double aMinGroupRadius_nm)
{
   if (aGroup->GetElementList().empty())
   {
      // Empty list, no need to recalculate centroid
      return;
   }

   // These are used below for calculating averages
   int      numLatLonElements = 0;
   int      numAltElements    = 0;
   UtVec3dX centroidWCS(0.0, 0.0, 0.0);
   double   centroidAlt_ft = 0.0;

   // Loop though entities and calculate centroid location and altitude
   for (auto& entity : aGroup->GetElementList())
   {
      // For now, we ignore angle-only entities, which should not be in any group.
      // So, only use entities that are not angle-only
      if (!entity->GetAngleOnly())
      {
         // Get the WCS of the entity, using zero altitude
         UtVec3dX entityWCS;
         UtEntity::ConvertLLAToWCS(entity->GetLat_deg(), entity->GetLon_deg(), 0.0, entityWCS.GetData());

         // Add the entity's WCS to the centroid
         centroidWCS += entityWCS;
         ++numLatLonElements;

         if (entity->GetAltitudeValid())
         {
            centroidAlt_ft += entity->GetAltitude_ft();
            ++numAltElements;
         }
      }
   }

   // Set the centroid Lat/Lon
   if (numLatLonElements > 0)
   {
      // Calc the centroid by dividing my number of elements
      double num = static_cast<double>(numLatLonElements);
      centroidWCS[0] /= num;
      centroidWCS[1] /= num;
      centroidWCS[2] /= num;
   }

   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(centroidWCS.GetData(), lat, lon, alt);
   aGroup->SetLat_deg(lat);
   aGroup->SetLon_deg(lon);

   // Set the centroid altitude
   if (numAltElements > 0)
   {
      // Calc the centroid by dividing my number of elements
      double num = static_cast<double>(numAltElements);
      centroidAlt_ft /= num;
   }
   aGroup->SetAltitude_ft(static_cast<float>(centroidAlt_ft));
   aGroup->SetAltitudeValid(true);

   // Now, calculate the speed and heading centroids. Use vectors for heading -- x is north and y is east
   double sum_x          = 0.0;
   double sum_y          = 0.0;
   int    numSpdHdgItems = 0;
   for (auto& entity : aGroup->GetElementList())
   {
      if (entity->GetSpeedValid() && entity->GetHeadingValid())
      {
         double x = entity->GetSpeed_kts() * cos(entity->GetHeading_deg() * UtMath::cRAD_PER_DEG);
         double y = entity->GetSpeed_kts() * sin(entity->GetHeading_deg() * UtMath::cRAD_PER_DEG);
         sum_x += x;
         sum_y += y;
         ++numSpdHdgItems;
      }
   }

   // Set the centroid speed and heading
   if (numSpdHdgItems > 0)
   {
      double num = static_cast<double>(numSpdHdgItems);

      sum_x /= num;
      sum_y /= num;

      double spd       = sqrt(sum_x * sum_x + sum_y * sum_y);
      double angle_deg = atan2(sum_y, sum_x) * UtMath::cDEG_PER_RAD;

      if (angle_deg < 0.0)
      {
         angle_deg += 360.0;
      }
      if (angle_deg > 360.0)
      {
         angle_deg -= 360.0;
      }

      aGroup->SetSpeed_kts(static_cast<float>(spd));
      aGroup->SetHeading_deg(static_cast<float>(angle_deg));
      aGroup->SetSpeedValid(true);
      aGroup->SetHeadingValid(true);
   }
   else
   {
      // No entities, so no speed or heading
      aGroup->SetSpeed_kts(0.0);
      aGroup->SetHeading_deg(0.0);
      aGroup->SetSpeedValid(false);
      aGroup->SetHeadingValid(false);
   }

   // Now, calculate the radius -- note that we use the min radius as a buffer
   aGroup->SetRadiusCurrent_nm(static_cast<float>(CalcRadiusForGroup_nm(aGroup, aMinGroupRadius_nm)));
}

double WsfSA_GroupUtils::CalcRadiusForGroup_nm(const WsfSA_Group* aGroup, double aMinGroupRadius_nm)
{
   // This is used to determine the max radius below
   double maxRadius_nm = aGroup->GetRadiusMinimum_nm(); // It must be at least the minimum

   if (aGroup->GetElementList().size() == 0)
   {
      // This is coasting, so the radius will be handled in CoastGroups(). Return the current radius.
      return aGroup->GetRadiusCurrent_nm();
   }
   else
   {
      // Loop through all entities (that are not angle-only only) in the group to find the max radius from the centroid
      for (auto& entity : aGroup->GetElementList())
      {
         if (!entity->GetAngleOnly())
         {
            double radius_nm = DistanceFromCentroid(aGroup, entity) * UtMath::cNM_PER_M +
                               aMinGroupRadius_nm; // Note the buffer space (mMinGroupRadius_nm)
            if (radius_nm > maxRadius_nm)
            {
               maxRadius_nm = radius_nm;
            }
         }
      }
   }

   return maxRadius_nm;
}

double WsfSA_GroupUtils::DistanceFromCentroid(const WsfSA_Group* aGroup, const WsfSA_EntityPerception* aEntity)
{
   // Construct WsfGeoPoint for the entity
   WsfGeoPoint entityPt =
      WsfGeoPoint(aEntity->GetLat_deg(), aEntity->GetLon_deg(), aEntity->GetAltitude_ft() * UtMath::cM_PER_FT);

   // Construct WsfGeoPoint for the centroid
   WsfGeoPoint centroidPt =
      WsfGeoPoint(aGroup->GetLat_deg(), aGroup->GetLon_deg(), aGroup->GetAltitude_ft() * UtMath::cM_PER_FT);

   return entityPt.GetDistanceFrom(centroidPt);
}
