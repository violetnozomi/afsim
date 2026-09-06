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

#ifndef WSFGROUPLIST_HPP
#define WSFGROUPLIST_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
class WsfGroup;
class WsfGroupManager;
class WsfPlatform;
class WsfPlatformPart;
class WsfSimulation;
#include "WsfStringId.hpp"

//! Contains the 'groups' to which platform or platform part belongs

class WSF_EXPORT WsfGroupList
{
public:
   typedef std::vector<WsfStringId> GroupList;

   WsfGroupList();
   WsfGroupList(const WsfGroupList& aSrc);
   WsfGroupList& operator=(const WsfGroupList&) = delete;
   ~WsfGroupList()                              = default;

   const GroupList& GetGroupList() const { return mGroups; }

   bool Initialize(WsfPlatform* aPlatformPtr);
   bool Initialize(WsfPlatformPart* aPlatformPartPtr);

   bool ProcessInput(UtInput& aInput, WsfPlatform* aPlatformPtr);
   bool ProcessInput(UtInput& aInput, WsfPlatformPart* aPlatformPartPtr);

   void LeaveAllGroups();

   void JoinGroup(WsfGroup* aGroupPtr, size_t aPlatformIndex, unsigned int aPlatformPartId = 0);

   void LeaveGroup(WsfGroup* aGroupPtr, size_t aPlatformIndex, unsigned int aPlatformPartId = 0);

   bool IsGroupMember(WsfStringId aGroupId) const;

private:
   bool ProcessInput(UtInput& aInput);

   void JoinGroup(WsfStringId aGroupId);
   void JoinGroup(WsfGroup* aGroupPtr);

   void LeaveGroup(WsfStringId aGroupId);
   void LeaveGroup(WsfGroup* aGroupPtr);

   GroupList                mGroups;
   std::vector<WsfStringId> mGroupsToJoin;
   std::vector<WsfStringId> mGroupsToLeave;
   WsfGroupManager*         mGroupManagerPtr;
};

#endif
