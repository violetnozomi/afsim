// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfGroupList.hpp"

#include <algorithm>
#include <cassert>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupManager.hpp"
#include "WsfGroupTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfGroupList::WsfGroupList()
   : mGroups()
   , mGroupsToJoin()
   , mGroupsToLeave()
   , mGroupManagerPtr(nullptr)
{
}

// =================================================================================================
WsfGroupList::WsfGroupList(const WsfGroupList& aSrc)
   : mGroups(aSrc.mGroups)
   , mGroupsToJoin(aSrc.mGroupsToJoin)
   , mGroupsToLeave(aSrc.mGroupsToLeave)
   , mGroupManagerPtr(nullptr)
{
}

// =================================================================================================
bool WsfGroupList::Initialize(WsfPlatform* aPlatformPtr)
{
   bool ok          = true;
   mGroupManagerPtr = &(aPlatformPtr->GetSimulation()->GetGroupManager());
   for (auto& groupToJoin : mGroupsToJoin)
   {
      if (!mGroupManagerPtr->LoadInstance(groupToJoin, aPlatformPtr))
      {
         auto out = ut::log::error() << "Unable to join group.";
         out.AddNote() << "Group Name: " << groupToJoin;
         ok = false;
      }
   }
   mGroupsToJoin.clear();

   for (auto& groupToLeave : mGroupsToLeave)
   {
      LeaveGroup(mGroupManagerPtr->GetGroup(groupToLeave));
   }
   mGroupsToLeave.clear();
   return ok;
}

// =================================================================================================
bool WsfGroupList::Initialize(WsfPlatformPart* aPlatformPartPtr)
{
   bool ok          = true;
   mGroupManagerPtr = &(aPlatformPartPtr->GetSimulation()->GetGroupManager());
   for (auto& groupToJoin : mGroupsToJoin)
   {
      if (!mGroupManagerPtr->LoadInstance(groupToJoin, aPlatformPartPtr))
      {
         auto out = ut::log::error() << "Unable to join group.";
         out.AddNote() << "Group Name: " << groupToJoin;
         ok = false;
      }
   }
   mGroupsToJoin.clear();

   for (auto& groupToLeave : mGroupsToLeave)
   {
      LeaveGroup(mGroupManagerPtr->GetGroup(groupToLeave));
   }
   mGroupsToLeave.clear();
   return ok;
}

// =================================================================================================
bool WsfGroupList::ProcessInput(UtInput& aInput, WsfPlatform* aPlatformPtr)
{
   bool myCommand(true);
   if (ProcessInput(aInput))
   {
   }
   else if (aPlatformPtr->GetScenario().GetGroupTypes().LoadType(aInput, aPlatformPtr))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool WsfGroupList::ProcessInput(UtInput& aInput, WsfPlatformPart* aPlatformPartPtr)
{
   bool myCommand(true);
   if (ProcessInput(aInput))
   {
   }
   else if (aPlatformPartPtr->GetScenario().GetGroupTypes().LoadType(aInput, aPlatformPartPtr))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool WsfGroupList::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "group_join")
   {
      std::string groupName;
      aInput.ReadValue(groupName);
      mGroupsToJoin.emplace_back(groupName);
   }
   else if (command == "group_leave")
   {
      std::string groupName;
      aInput.ReadValue(groupName);
      mGroupsToLeave.emplace_back(groupName);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Leave all groups this platform/platform part is a member of.
void WsfGroupList::LeaveAllGroups()
{
   if (mGroupManagerPtr != nullptr)
   {
      WsfGroupList::GroupList groups = GetGroupList();
      for (auto& group : groups)
      {
         LeaveGroup(mGroupManagerPtr->GetGroup(group));
      }
   }
}

// =================================================================================================
//! Determine if the associated object is a member of the indicated group.
//!
//!  @param aGroupId String ID number of the group to be checked.
//!  @return 'true' if the object is a member of the indicated group or 'false' if not.
bool WsfGroupList::IsGroupMember(WsfStringId aGroupId) const
{
   return (std::find(mGroups.begin(), mGroups.end(), aGroupId) != mGroups.end());
}

// =================================================================================================
//! Add a platform/platform part to a group.
//! This is the form called by the platform/platform part.
//! @param aGroupPtr     Pointer to the group.
//! @param aPlatformIndex The platform index.
//! @param aPlatformPartId The unique ID of the platform part.
//! The ID will be zero if it is a platform that is being added to the group.
void WsfGroupList::JoinGroup(WsfGroup* aGroupPtr, size_t aPlatformIndex, unsigned int aPlatformPartId /* = 0 */)
{
   if (aGroupPtr != nullptr)
   {
      aGroupPtr->JoinGroup(aPlatformIndex, aPlatformPartId);
      mGroupManagerPtr->AddGroup(aGroupPtr);
      JoinGroup(aGroupPtr);
   }
}

// =================================================================================================
//! Add the given group to the list
//! @param aGroupId String ID number of the group of which the object is a member.
void WsfGroupList::JoinGroup(WsfStringId aGroupId)
{
   if (!IsGroupMember(aGroupId))
   {
      mGroups.push_back(aGroupId);
   }
}

// =================================================================================================
//! Add the given group to the list
//! @param aGroupPtr group object
void WsfGroupList::JoinGroup(WsfGroup* aGroupPtr)
{
   JoinGroup(aGroupPtr->GetNameId());
}

// =================================================================================================
//! Remove the a platform/platform part from group.
//! This is the form called by the platform/platform part.
//! @param aGroupPtr     Pointer to the group.
//! @param aPlatformIndex The platform index.
//! @param aPlatformPartId The unique ID of the platform part.
//! The ID will be zero if it is a platform that is being removed from the group.
void WsfGroupList::LeaveGroup(WsfGroup* aGroupPtr, size_t aPlatformIndex, unsigned int aPlatformPartId /* = 0 */)
{
   if (aGroupPtr != nullptr)
   {
      aGroupPtr->LeaveGroup(aPlatformIndex, aPlatformPartId);
      LeaveGroup(aGroupPtr);
   }
}

// =================================================================================================
//! Remove the given group from the list
//! @param aGroupId String ID number of the group of which the object is a member.
void WsfGroupList::LeaveGroup(WsfStringId aGroupId)
{
   std::vector<WsfStringId>::iterator groupId = std::find(mGroups.begin(), mGroups.end(), aGroupId);
   if (groupId != mGroups.end())
   {
      mGroups.erase(groupId);
   }
}

// =================================================================================================
//! Remove the given group from the list
//! @param aGroupPtr group object
void WsfGroupList::LeaveGroup(WsfGroup* aGroupPtr)
{
   LeaveGroup(aGroupPtr->GetNameId());
}
