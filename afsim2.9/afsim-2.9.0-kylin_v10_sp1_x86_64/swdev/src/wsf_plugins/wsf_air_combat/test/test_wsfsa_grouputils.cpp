//*****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include <list>
#include <random> // I don't need anything high-powered, just to generate some numbers that aren't the same as each other.
#include <sstream>
#include <string>

#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_GroupUtils.hpp"
#include "gtest/gtest.h"

namespace
{
const double       MIN_RADIUS{1.0};   /** The default minimum radius for groups.*/
const WsfPlatform* PLATFORM{nullptr}; /** The platform pointer used for functions which require one.*/

/** Populate a given entity list with entities.
 * @param aEntityList  A reference to the entity list, which should initially be empty, and is filled by this function.
 * @param aCount       The number of entities to add.
 */
void PopulateEntities(WsfSA_Group::ElementList& aEntityList, size_t aCount)
{
   for (size_t i = 0; i < aCount; ++i)
   {
      WsfSA_EntityPerception* entity = new WsfSA_EntityPerception();
      std::ostringstream      oss;
      oss << "ent_" << aEntityList.size();
      entity->SetIdString(oss.str());
      aEntityList.push_back(entity);
   }
}

/** Group name factory.
 * @return  A string with the pattern "G-<group number>" where <group number> increments each time this function is called.
 */
std::string GenerateNewGroupName()
{
   // Increment the group number
   static int groupNumber{0};
   ++groupNumber;

   // Setup the name using a G (for group) plus the group number
   std::string name = "G" + std::to_string(groupNumber);

   return name;
}
} // namespace

/** Test CalculateNumPIs with the following input:
 * - Empty group list
 * - Group list with one empty group
 * - Group list with one focused group of multiple entities
 * - Group list with one unfocused group of multiple entities
 * - Group list with one unfocused and one empty group
 * - Group list with one unfocused and one focused group
 * - Group list with two focused groups
 */
TEST(WsfSA_GroupUtils, CalculateNumPIs)
{
   std::list<WsfSA_Group*> groupList;
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 0);
   WsfSA_Group::ElementList elements1;
   WsfSA_Group::ElementList elements2;
   PopulateEntities(elements1, 3);
   PopulateEntities(elements2, 3);
   groupList.push_back(new WsfSA_Group);
   // An empty group shouldn't happen, but if it does, then that case should still be handled reasonably.
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 0);
   groupList.front()->SetElementList(elements1);
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 3);
   groupList.front()->SetFocus(false);
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 1);
   groupList.push_back(new WsfSA_Group);
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 1);
   groupList.back()->SetElementList(elements2);
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 4);
   groupList.front()->SetFocus(true);
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(groupList), 6);

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : elements1)
   {
      delete entity;
   }
   for (auto entity : elements2)
   {
      delete entity;
   }
}

/** Tests RenameGroup, with both the usual success case, and the failure case of having a duplicate name.*/
TEST(WsfSA_GroupUtils, RenameGroup)
{
   WsfSA_Group             group;
   WsfSA_Group             duplicate;
   std::list<WsfSA_Group*> groups{&group, &duplicate};
   group.SetPerceivedName("Before");
   duplicate.SetPerceivedName("Duplicate");
   std::string newName = "After";
   EXPECT_TRUE(WsfSA_GroupUtils::RenameGroup(groups, &group, newName));
   EXPECT_STREQ(group.GetPerceivedName().c_str(), newName.c_str());
   newName = "Duplicate";
   EXPECT_FALSE(WsfSA_GroupUtils::RenameGroup(groups, &group, newName));
   EXPECT_STREQ(group.GetPerceivedName().c_str(), "After");
}

/** Tests CreateGroup in the following cases:
 * - The usual success case
 * - An empty entity list (failure)
 * - Too many PIs due to taking one from an unfocused group (failure)
 * - A group with a null entity
 * Also tests that, following a call to CreateGroup, all entities' parent group pointer is set to the new group.
 */
TEST(WsfSA_GroupUtils, CreateGroup)
{
   std::list<WsfSA_Group*> groupList;

   // test success
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 4);
   for (auto entity : entityList1)
   {
      EXPECT_FALSE(entity->GetParentGroup());
   }
   EXPECT_EQ(groupList.size(), 0);
   EXPECT_TRUE(WsfSA_GroupUtils::CreateGroup(4, 4, groupList, PLATFORM, entityList1, "creation_test_1", MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 1);
   WsfSA_Group* newGroup = groupList.front();
   EXPECT_TRUE(newGroup);
   EXPECT_EQ(newGroup->GetNumElements(), 4);
   for (auto entity : entityList1)
   {
      EXPECT_EQ(entity->GetParentGroup(), newGroup);
   }

   // test too many PIs
   newGroup->SetFocus(false);
   WsfSA_Group::ElementList entityList2;
   PopulateEntities(entityList2, 3);
   for (auto entity : entityList2)
   {
      EXPECT_FALSE(entity->GetParentGroup());
   }
   entityList2.push_back(entityList1.back());
   entityList1.pop_back();
   EXPECT_FALSE(WsfSA_GroupUtils::CreateGroup(4, 4, groupList, PLATFORM, entityList2, "creation_test_2", MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 1);
   EXPECT_FALSE(entityList2.front()->GetParentGroup());

   // test null entity
   WsfSA_Group::ElementList entityList3;
   entityList3.push_back(nullptr);
   EXPECT_FALSE(WsfSA_GroupUtils::CreateGroup(4, 1, groupList, PLATFORM, entityList3, "creation_test_3", MIN_RADIUS));

   // clean up
   delete newGroup;
   for (auto entity : entityList1)
   {
      delete entity;
   }
   for (auto entity : entityList2)
   {
      delete entity;
   }
}

/** Test DisbandGroup in the following cases:
 * - Too many PIs due to disbanding a large unfocused group (failure)
 * - Disbanding a group successfully in the usual case
 * Also tests that the free entities now have their own focused degenerate groups afterwards.
 */
TEST(WsfSA_GroupUtils, DisbandGroup)
{
   std::list<WsfSA_Group*> groupList;

   // test failure
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 4);
   WsfSA_GroupUtils::CreateGroup(4, 4, groupList, PLATFORM, entityList1, "disband_test", MIN_RADIUS);
   EXPECT_EQ(groupList.size(), 1);
   WsfSA_Group* newGroup = groupList.front();
   newGroup->SetFocus(false);
   EXPECT_EQ(newGroup->GetNumElements(), 4);
   for (auto entity : entityList1)
   {
      EXPECT_EQ(entity->GetParentGroup(), newGroup);
   }
   EXPECT_FALSE(WsfSA_GroupUtils::DisbandGroup(3, 1, groupList, newGroup, GenerateNewGroupName, MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 1);
   EXPECT_EQ(newGroup->GetNumElements(), 4);
   for (auto entity : entityList1)
   {
      EXPECT_EQ(entity->GetParentGroup(), newGroup);
   }

   // test success
   EXPECT_TRUE(WsfSA_GroupUtils::DisbandGroup(4, 1, groupList, newGroup, GenerateNewGroupName, MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 4);
   for (auto group : groupList)
   {
      EXPECT_EQ(group->GetNumElements(), 1);
   }
   for (auto entity : entityList1)
   {
      EXPECT_NE(entity->GetParentGroup(), newGroup);
      EXPECT_TRUE(entity->GetParentGroup()->GetFocus());
   }

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
}

/** Test MergeGroups in the following cases:
 * - Empty list of groups to merge (failure)
 * - List of groups to merge with non-uniform focus states (failure)
 * - List of groups to merge with uniform focus states (success)
 * Also tests that focus remains the same as the input groups after merging.*/
TEST(WsfSA_GroupUtils, MergeGroups)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList1;
   WsfSA_Group::ElementList entityList2;
   PopulateEntities(entityList1, 2);
   PopulateEntities(entityList2, 2);
   WsfSA_GroupUtils::CreateGroup(4, 4, groupList, PLATFORM, entityList1, "merge_test_1", MIN_RADIUS);
   WsfSA_GroupUtils::CreateGroup(4, 4, groupList, PLATFORM, entityList2, "merge_test_2", MIN_RADIUS);
   std::list<WsfSA_Group*> groupsToMerge;

   // test failure
   // empty merge list
   EXPECT_EQ(groupList.size(), 2);
   EXPECT_FALSE(WsfSA_GroupUtils::MergeGroups(groupList, groupsToMerge, PLATFORM, MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 2);

   // different focus states
   groupsToMerge           = groupList; // We just want to merge the two into one.
   WsfSA_Group* firstGroup = groupList.front();
   firstGroup->SetFocus(false);
   EXPECT_FALSE(WsfSA_GroupUtils::MergeGroups(groupList, groupsToMerge, PLATFORM, MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 2);

   // test success
   groupList.back()->SetFocus(false);
   EXPECT_TRUE(WsfSA_GroupUtils::MergeGroups(groupList, groupsToMerge, PLATFORM, MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 1);
   WsfSA_Group* newGroup = groupList.front();
   EXPECT_EQ(newGroup->GetNumElements(), 4);
   EXPECT_FALSE(newGroup->GetFocus());

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
   for (auto entity : entityList2)
   {
      delete entity;
   }
}

/** Test SplitGroup with the following cases:
 * - Successfully splitting according to maximum threat level
 * - Failing due to creating too many PIs when splitting an unfocused group
 * Also tests that the new groups have the same focus as the source group.
 */
TEST(WsfSA_GroupUtils, SplitGroup)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList;
   PopulateEntities(entityList, 5);
   WsfSA_GroupUtils::CreateGroup(-1, 5, groupList, PLATFORM, entityList, "split_test_1", MIN_RADIUS);
   std::default_random_engine             generator(1); // seed it with a literal for test repeatability.
   std::uniform_real_distribution<double> distribution(0.0, 1.0);
   for (auto entity : entityList)
   {
      entity->SetThreatLevel(distribution(generator));
   }

   EXPECT_EQ(groupList.size(), 1);
   auto newGroups = WsfSA_GroupUtils::SplitGroup(-1,
                                                 5,
                                                 groupList,
                                                 groupList.front(),
                                                 2,
                                                 WsfSA_GroupUtils::cMAX_THREAT,
                                                 nullptr,
                                                 "split_test_2",
                                                 MIN_RADIUS);
   EXPECT_NE(newGroups.first, nullptr);
   EXPECT_EQ(groupList.size(), 2);
   // The group with the higher threat level should have 2 entities.
   auto group1 = newGroups.first;
   auto group2 = newGroups.second;
   EXPECT_EQ(group1->GetNumElements(), 3);
   EXPECT_EQ(group2->GetNumElements(), 2);
   double maxLower = 0.0;
   double minUpper = 1.0;
   for (auto entity : group1->GetElementList())
   {
      if (entity->GetThreatLevel() > maxLower)
      {
         maxLower = entity->GetThreatLevel();
      }
   }
   for (auto entity : group2->GetElementList())
   {
      if (entity->GetThreatLevel() < minUpper)
      {
         minUpper = entity->GetThreatLevel();
      }
   }
   EXPECT_LE(maxLower, minUpper);
   EXPECT_TRUE(groupList.front()->GetFocus());
   EXPECT_TRUE(groupList.back()->GetFocus());

   // fail upon exceeding the PI limit
   std::list<WsfSA_Group*> groupsToMerge = groupList;
   WsfSA_GroupUtils::MergeGroups(groupList, groupsToMerge, PLATFORM, MIN_RADIUS);
   groupList.front()->SetFocus(false);
   EXPECT_EQ(groupList.size(), 1);
   newGroups = WsfSA_GroupUtils::SplitGroup(1,
                                            1,
                                            groupList,
                                            groupList.front(),
                                            2,
                                            WsfSA_GroupUtils::cMAX_THREAT,
                                            nullptr,
                                            "split_test_3",
                                            MIN_RADIUS);
   EXPECT_EQ(newGroups.first, nullptr);
   EXPECT_EQ(groupList.size(), 1);

   newGroups = WsfSA_GroupUtils::SplitGroup(-1,
                                            1,
                                            groupList,
                                            groupList.front(),
                                            2,
                                            WsfSA_GroupUtils::cMAX_THREAT,
                                            nullptr,
                                            "split_test_4",
                                            MIN_RADIUS);
   EXPECT_NE(newGroups.first, nullptr);
   EXPECT_EQ(groupList.size(), 2);
   EXPECT_FALSE(newGroups.first->GetFocus());
   EXPECT_FALSE(newGroups.second->GetFocus());

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList)
   {
      delete entity;
   }
}

/** Tests TransferEntity in the following cases:
 * - Exceeding the PI limit by transfering from an unfocused group to a focused one (failure)
 * - Successful transfer
 */
TEST(WsfSA_GroupUtils, TransferEntity)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 3);
   WsfSA_Group::ElementList entityList2;
   PopulateEntities(entityList2, 2);

   auto transferredEntity = entityList1.front();
   WsfSA_GroupUtils::CreateGroup(-1, 5, groupList, PLATFORM, entityList1, "transfer_test_1", MIN_RADIUS);
   WsfSA_GroupUtils::CreateGroup(-1, 5, groupList, PLATFORM, entityList2, "transfer_test_2", MIN_RADIUS);

   // Fail because moving an entity from an unfocused group to a focused one causes the PI limit to be exceeded.
   groupList.front()->SetFocus(false);
   EXPECT_EQ(groupList.front()->GetNumElements(), 3);
   EXPECT_EQ(groupList.back()->GetNumElements(), 2);
   EXPECT_FALSE(WsfSA_GroupUtils::TransferEntity(3, 3, groupList, transferredEntity, groupList.back(), MIN_RADIUS));
   EXPECT_EQ(groupList.front()->GetNumElements(), 3);
   EXPECT_EQ(groupList.back()->GetNumElements(), 2);
   EXPECT_EQ(transferredEntity->GetParentGroup(), groupList.front());
   EXPECT_NE(std::find(groupList.front()->GetElementList().begin(), groupList.front()->GetElementList().end(), transferredEntity),
             groupList.front()->GetElementList().end());
   EXPECT_EQ(std::find(groupList.back()->GetElementList().begin(), groupList.back()->GetElementList().end(), transferredEntity),
             groupList.back()->GetElementList().end());

   groupList.front()->SetFocus(true);
   EXPECT_TRUE(WsfSA_GroupUtils::TransferEntity(3, 3, groupList, transferredEntity, groupList.back(), MIN_RADIUS));
   EXPECT_EQ(groupList.front()->GetNumElements(), 2);
   EXPECT_EQ(groupList.back()->GetNumElements(), 3);
   EXPECT_EQ(transferredEntity->GetParentGroup(), groupList.back());
   EXPECT_EQ(std::find(groupList.front()->GetElementList().begin(), groupList.front()->GetElementList().end(), transferredEntity),
             groupList.front()->GetElementList().end());
   EXPECT_NE(std::find(groupList.back()->GetElementList().begin(), groupList.back()->GetElementList().end(), transferredEntity),
             groupList.back()->GetElementList().end());

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
   for (auto entity : entityList2)
   {
      delete entity;
   }
}

/** Test RemoveEntityFromGroup in the following cases:
 * - Failure when removing exceeding the PI limit when removing an entity from an unfocused group
 * - The usual success case
 * Also tests that the removed entity gets its own degenerate group.
 */
TEST(WsfSA_GroupUtils, RemoveEntityFromGroup)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 3);
   auto removedEntity = entityList1.front();
   WsfSA_GroupUtils::CreateGroup(-1, 3, groupList, PLATFORM, entityList1, "remove_test", MIN_RADIUS);
   groupList.front()->SetFocus(false);

   EXPECT_EQ(groupList.size(), 1);
   EXPECT_FALSE(WsfSA_GroupUtils::RemoveEntityFromGroup(1, 1, groupList, removedEntity, "removed_entity", MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 1);
   EXPECT_TRUE(WsfSA_GroupUtils::RemoveEntityFromGroup(-1, 1, groupList, removedEntity, "removed_entity", MIN_RADIUS));
   EXPECT_EQ(groupList.size(), 2);
   EXPECT_EQ(groupList.front()->GetNumElements(), 2);
   EXPECT_EQ(groupList.back()->GetNumElements(), 1);

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
}

/** Tests SetFocus in the following cases:
 * - Failure to unfocus a degenerate group
 * - Failure when exceeding PI limits when focusing an unfocused group
 * - Success in the general case when focusing or unfocusing
 */
TEST(WsfSA_GroupUtils, SetFocus)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 1);
   WsfSA_GroupUtils::CreateGroup(-1, 1, groupList, PLATFORM, entityList1, "focus_test", MIN_RADIUS);
   auto group = groupList.front();

   EXPECT_TRUE(group->GetFocus());
   EXPECT_FALSE(WsfSA_GroupUtils::SetFocus(1, 1, group, false));
   PopulateEntities(entityList1, 1);
   group->AddElement(entityList1.back());
   EXPECT_TRUE(group->GetFocus());
   EXPECT_TRUE(WsfSA_GroupUtils::SetFocus(1, 2, group, false));
   EXPECT_FALSE(group->GetFocus());
   EXPECT_FALSE(WsfSA_GroupUtils::SetFocus(1, 1, group, true));
   EXPECT_FALSE(group->GetFocus());
   EXPECT_TRUE(WsfSA_GroupUtils::SetFocus(-1, 1, group, true));
   EXPECT_TRUE(group->GetFocus());

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
}

/** Test Prune in the following cases:
 * - 3 groups of 4 (all focused, none focus or element protected, none important), PIL = 8
 * - 3 groups of 4 (2 unfocused, 1 focused, none focus or element protected, none important), PIL = 2
 * - 3 groups of 4 (all focused, 1 focus protected, 1 important), PIL = 8
 * - 2 groups of 4 (all focused, 1 focus protected, 1 important), PIL = 6
 */
TEST(WsfSA_GroupUtils, Prune)
{
   std::list<WsfSA_Group*>         groupList;
   std::list<WsfSA_PerceivedItem*> toRemove;
   WsfSA_Group::ElementList        entityList1;
   PopulateEntities(entityList1, 4);
   WsfSA_Group::ElementList entityList2;
   PopulateEntities(entityList2, 4);
   WsfSA_Group::ElementList entityList3;
   PopulateEntities(entityList3, 4);
   WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList1, "prune_test_1", MIN_RADIUS);
   WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList2, "prune_test_2", MIN_RADIUS);
   WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList3, "prune_test_3", MIN_RADIUS);
   std::list<WsfSA_PerceivedItem*> PI_List{groupList.begin(), groupList.end()};
   std::vector<WsfSA_Group*>       easyAccess{groupList.begin(), groupList.end()};

   // The prune function currently assumes that the list is already sorted according to consideration score.
   EXPECT_EQ(PI_List.size(), 3);
   EXPECT_EQ(toRemove.size(), 0);
   WsfSA_GroupUtils::Prune(8, PI_List, toRemove);
   EXPECT_EQ(PI_List.size(), 3);
   EXPECT_EQ(easyAccess[0]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[1]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[2]->GetNumElements(), 4);
   EXPECT_EQ(toRemove.size(), 0);
   EXPECT_FALSE(easyAccess[0]->GetFocus());
   EXPECT_FALSE(easyAccess[1]->GetFocus());
   EXPECT_TRUE(easyAccess[2]->GetFocus());
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(PI_List), 6);
   WsfSA_GroupUtils::Prune(2, PI_List, toRemove);
   EXPECT_EQ(PI_List.size(), 2);
   EXPECT_EQ(easyAccess[0]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[1]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[2]->GetNumElements(), 4);
   EXPECT_EQ(toRemove.size(), 1);
   EXPECT_TRUE(easyAccess[0]->GetFocus());
   EXPECT_FALSE(easyAccess[1]->GetFocus());
   EXPECT_FALSE(easyAccess[2]->GetFocus());
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(PI_List), 2);

   // easyAccess[0]->SetElementList(toRemove);
   PI_List.push_front(easyAccess[0]);
   toRemove.clear();
   for (auto group : groupList)
   {
      group->SetFocus(true);
   }
   easyAccess[0]->SetFocusProtection(true);
   easyAccess[1]->SetImportance(WsfSA_PerceivedItem::IMPORTANT);
   WsfSA_GroupUtils::Prune(8, PI_List, toRemove);
   EXPECT_EQ(PI_List.size(), 2);
   EXPECT_EQ(easyAccess[0]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[1]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[2]->GetNumElements(), 4);
   EXPECT_EQ(toRemove.size(), 1);
   EXPECT_TRUE(easyAccess[0]->GetFocus());
   EXPECT_TRUE(easyAccess[1]->GetFocus());
   EXPECT_TRUE(easyAccess[2]->GetFocus());
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(PI_List), 8);
   WsfSA_GroupUtils::Prune(6, PI_List, toRemove);
   EXPECT_EQ(PI_List.size(), 2);
   EXPECT_EQ(easyAccess[0]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[1]->GetNumElements(), 4);
   EXPECT_EQ(easyAccess[2]->GetNumElements(), 4);
   EXPECT_EQ(toRemove.size(), 1);
   EXPECT_FALSE(easyAccess[0]->GetFocus());
   EXPECT_TRUE(easyAccess[1]->GetFocus());
   EXPECT_TRUE(easyAccess[2]->GetFocus());
   EXPECT_EQ(WsfSA_GroupUtils::CalculateNumPIs(PI_List), 5);

   // clean up
   for (auto group : easyAccess)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
   for (auto entity : entityList2)
   {
      delete entity;
   }
   for (auto entity : entityList3)
   {
      delete entity;
   }
}

/** TestSortedGroups in the following cases:
 * - invalid sorting enum (failure)
 * - 3 groups of 4, ascending threat level
 * - 3 groups of 4, ascending target value
 * - 3 groups of 4, ascending radius
 * - 3 groups of 4, ascending entity count
 */
TEST(WsfSA_GroupUtils, SortedGroups)
{
   std::list<WsfSA_Group*>  groupList;
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 4);
   WsfSA_Group::ElementList entityList2;
   PopulateEntities(entityList2, 4);
   WsfSA_Group::ElementList entityList3;
   PopulateEntities(entityList3, 4);
   auto g1 = WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList1, "sort_test_1", MIN_RADIUS);
   auto g2 = WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList2, "sort_test_2", MIN_RADIUS);
   auto g3 = WsfSA_GroupUtils::CreateGroup(-1, 12, groupList, PLATFORM, entityList3, "sort_test_3", MIN_RADIUS);

   g1->SetAltitude_ft(10.0);
   g1->SetThreatLevel(0.75);
   g1->SetTargetValue(0.5);
   g1->SetRadiusCurrent_nm(0.5);

   g2->SetAltitude_ft(100.0);
   g2->SetThreatLevel(0.5);
   g2->SetTargetValue(0.25);
   g2->SetRadiusCurrent_nm(0.75);

   g3->SetAltitude_ft(1000.0);
   g3->SetThreatLevel(0.25);
   g3->SetTargetValue(0.75);
   g3->SetRadiusCurrent_nm(0.25);

   std::vector<WsfSA_Group*> output{};

   // Passing nullptr as the platform is fine as long as the metric in use doesn't require a platform to be calculated.
   WsfSA_GroupUtils::SortedGroups(9999, output, groupList, nullptr);
   EXPECT_TRUE(std::equal(output.begin(), output.end(), groupList.begin()));
   output.clear();

   WsfSA_GroupUtils::SortedGroups(WsfSA_GroupUtils::cASCENDING_THREAT, output, groupList, nullptr);
   EXPECT_TRUE(std::equal(output.begin(), output.end(), groupList.rbegin()));
   output.clear();

   WsfSA_GroupUtils::SortedGroups(WsfSA_GroupUtils::cASCENDING_TARGET, output, groupList, nullptr);
   std::vector<WsfSA_Group*> expected{g2, g1, g3};
   EXPECT_TRUE(std::equal(output.begin(), output.end(), expected.begin()));
   output.clear();
   expected.clear();

   WsfSA_GroupUtils::SortedGroups(WsfSA_GroupUtils::cASCENDING_RADIUS, output, groupList, nullptr);
   expected.assign({g3, g1, g2});
   EXPECT_TRUE(std::equal(output.begin(), output.end(), expected.begin()));
   output.clear();
   expected.clear();

   WsfSA_GroupUtils::SortedGroups(WsfSA_GroupUtils::cASCENDING_ENTITY_COUNT, output, groupList, nullptr);
   EXPECT_TRUE(std::equal(output.begin(), output.end(), groupList.begin()));

   // clean up
   for (auto group : groupList)
   {
      delete group;
   }
   for (auto entity : entityList1)
   {
      delete entity;
   }
   for (auto entity : entityList2)
   {
      delete entity;
   }
   for (auto entity : entityList3)
   {
      delete entity;
   }
}

/** Test RecalculateGroupCentroid:
 * - of an empty group
 * - of a group with 4 entities
 */
TEST(WsfSA_GroupUtils, RecalculateGroupCentroid)
{
   WsfSA_Group group;
   double      alt     = group.GetAltitude_ft();
   double      lat     = group.GetLat_deg();
   double      lon     = group.GetLon_deg();
   double      speed   = group.GetSpeed_kts();
   double      heading = group.GetHeading_deg();
   EXPECT_EQ(alt, 0.0);
   EXPECT_EQ(lat, 0.0);
   EXPECT_EQ(lon, 0.0);
   EXPECT_EQ(speed, 0.0);
   EXPECT_EQ(heading, 0.0);
   WsfSA_GroupUtils::RecalculateGroupCentroid(&group, MIN_RADIUS);
   alt     = group.GetAltitude_ft();
   lat     = group.GetLat_deg();
   lon     = group.GetLon_deg();
   speed   = group.GetSpeed_kts();
   heading = group.GetHeading_deg();
   EXPECT_EQ(alt, 0.0);
   EXPECT_EQ(lat, 0.0);
   EXPECT_EQ(lon, 0.0);
   EXPECT_EQ(speed, 0.0);
   EXPECT_EQ(heading, 0.0);
   WsfSA_Group::ElementList entityList1;
   PopulateEntities(entityList1, 4);
   std::vector<WsfSA_EntityPerception*> entityVector; // To have random access to each individual entity without having to iterate.
   entityVector.assign(entityList1.begin(), entityList1.end());
   group.SetElementList(entityList1);
   entityVector[0]->SetAltitude_ft(1000.0);
   entityVector[1]->SetAltitude_ft(2000.0);
   entityVector[2]->SetAltitude_ft(3000.0);
   entityVector[3]->SetAltitude_ft(4000.0);
   entityVector[0]->SetLat_deg(0.1);
   entityVector[1]->SetLat_deg(0.2);
   entityVector[2]->SetLat_deg(0.3);
   entityVector[3]->SetLat_deg(0.4);
   entityVector[0]->SetLon_deg(0.1);
   entityVector[1]->SetLon_deg(0.2);
   entityVector[2]->SetLon_deg(0.3);
   entityVector[3]->SetLon_deg(0.4);
   entityVector[0]->SetSpeed_kts(100.0);
   entityVector[1]->SetSpeed_kts(100.0);
   entityVector[2]->SetSpeed_kts(100.0);
   entityVector[3]->SetSpeed_kts(100.0);
   entityVector[0]->SetHeading_deg(0.0);
   entityVector[1]->SetHeading_deg(90.0);
   entityVector[2]->SetHeading_deg(270.0);
   entityVector[3]->SetHeading_deg(180.0);
   for (auto ent : entityList1)
   {
      ent->SetAltitudeValid(true);
      ent->SetAngleOnly(false);
      ent->SetSpeedValid(true);
      ent->SetHeadingValid(true);
   }
   WsfSA_GroupUtils::RecalculateGroupCentroid(&group, MIN_RADIUS);
   alt   = group.GetAltitude_ft();
   lat   = group.GetLat_deg();
   lon   = group.GetLon_deg();
   speed = group.GetSpeed_kts();
   // heading = group.GetHeading_deg(); // Heading for this test is essentially undefined, since the centroid is stationary.
   EXPECT_NEAR(alt, 2500.0, 0.0001);
   EXPECT_NEAR(lat, 0.25, 0.0001);
   EXPECT_NEAR(lon, 0.25, 0.0001);
   EXPECT_NEAR(speed, 0.0, 0.0001);

   for (auto entity : entityList1)
   {
      delete entity;
   }
}
