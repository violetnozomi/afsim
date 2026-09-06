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

#include "WsfGroupManager.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtStringUtil.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
//! Constructor.
WsfGroupManager::WsfGroupManager(WsfSimulation* aSimPtr)
{
   mSimPtr = aSimPtr;
}

// =================================================================================================
//! Destructor.
WsfGroupManager::~WsfGroupManager()
{
   for (auto& iter : mGroups)
   {
      delete iter.second;
   }
}

// =================================================================================================
//! Adds the given group to the group list
// @ aNewGroup - the group object to add
bool WsfGroupManager::AddGroup(WsfGroup* aNewGroupPtr)
{
   if (aNewGroupPtr != nullptr)
   {
      if (GetGroup(aNewGroupPtr->GetName()) == nullptr)
      {
         mGroups[aNewGroupPtr->GetName()] = aNewGroupPtr;
         return true;
      }
   }
   return false;
}

// =================================================================================================
//! Create a new group
// @ aGroupName - the name of the group
// @ aGroupType - the type of the group
WsfGroup* WsfGroupManager::CreateGroup(const std::string& aGroupName, const std::string& aGroupType)
{
   if (GetGroup(aGroupName) == nullptr)
   {
      std::unique_ptr<WsfGroup> groupPtr(GetTypes().Clone(aGroupType));
      if (groupPtr != nullptr)
      {
         groupPtr->SetName(aGroupName);
         AddGroup(groupPtr.get());
         WsfObserver::GroupAdded(mSimPtr)(aGroupName, aGroupType);
         return groupPtr.release();
      }
      else
      {
         auto out = ut::log::error() << "Unknown group type.";
         out.AddNote() << "Type: " << aGroupType;
      }
   }
   return nullptr;
}

// =================================================================================================
//! Return the group that matches the given group id
WsfGroup* WsfGroupManager::GetGroup(const WsfStringId aGroupId)
{
   auto iter = mGroups.find(aGroupId);
   if (iter != std::end(mGroups))
   {
      return iter->second;
   }

   return nullptr;
}

// =================================================================================================
//! create an instance of a group after replacing any tokens in the string
//!
//! @param aGroupName - the name of the group to instantiate
//! @param aPlatformPtr - the platform that will join the group
//! @return true if successfully created and joined group
bool WsfGroupManager::LoadInstance(const std::string& aGroupName, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr == nullptr)
   {
      return false;
   }

   std::string fixedGroupName = aGroupName;
   std::string lowerGroupName = aGroupName;
   std::string typeGroupName  = aGroupName;

   UtStringUtil::ToLower(lowerGroupName);
   size_t tokenPos = lowerGroupName.find("<plat_type>");
   if (tokenPos != std::string::npos)
   {
      fixedGroupName.replace(tokenPos, 11, aPlatformPtr->GetType());
      typeGroupName  = fixedGroupName;
      lowerGroupName = fixedGroupName;
      UtStringUtil::ToLower(lowerGroupName);
   }
   tokenPos = lowerGroupName.find("<plat_name>");
   if (tokenPos != std::string::npos)
   {
      fixedGroupName.replace(tokenPos, 11, aPlatformPtr->GetName());
   }

   // join the group if it already exists
   WsfGroup* groupPtr = GetGroup(fixedGroupName);
   if (groupPtr != nullptr)
   {
      aPlatformPtr->JoinGroup(groupPtr);
      return true;
   }

   // find an existing template
   if (GetTypes().JoinGroupType(fixedGroupName, fixedGroupName, aPlatformPtr)) // all tokens replaced
   {
      return true;
   }
   else if (GetTypes().JoinGroupType(typeGroupName, fixedGroupName, aPlatformPtr)) // platform type replaced
   {
      return true;
   }
   else if (GetTypes().JoinGroupType(aGroupName, fixedGroupName, aPlatformPtr)) // raw string
   {
      return true;
   }
   else
   {
      auto out = ut::log::error() << "Group not found.";
      out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      out.AddNote() << "Group Name: " << fixedGroupName;
   }

   return false;
}

// =================================================================================================
//! create an instance of a group after replacing any tokens in the string
//!
//! @param aGroupName - the name of the group to instantiate
//! @param aPlatformPartPtr - the platform part that will join the group
//! @return true if successfully created and joined group
bool WsfGroupManager::LoadInstance(const std::string& aGroupName, WsfPlatformPart* aPlatformPartPtr)
{
   if (aPlatformPartPtr == nullptr)
   {
      return false;
   }

   WsfPlatform* platformPtr = aPlatformPartPtr->GetPlatform();
   if (platformPtr == nullptr)
   {
      return false;
   }

   std::string fixedGroupName = aGroupName; // completely fixed string with all tokens replaced
   std::string typeGroupName  = aGroupName; // string that will only contain the fixed platform type token
   std::string lowerGroupName = aGroupName; // only used to allow case-insensitivity

   // find and replace any tokens in the string
   UtStringUtil::ToLower(lowerGroupName);
   size_t tokenPos = lowerGroupName.find("<plat_type>");
   if (tokenPos != std::string::npos)
   {
      fixedGroupName.replace(tokenPos, 11, platformPtr->GetType());
      typeGroupName  = fixedGroupName;
      lowerGroupName = fixedGroupName;
      UtStringUtil::ToLower(lowerGroupName);
   }
   tokenPos = lowerGroupName.find("<plat_name>");
   if (tokenPos != std::string::npos)
   {
      fixedGroupName.replace(tokenPos, 11, platformPtr->GetName());
   }

   // join the group if it already exists
   WsfGroup* groupPtr = GetGroup(fixedGroupName);
   if (groupPtr != nullptr)
   {
      aPlatformPartPtr->JoinGroup(groupPtr);
      return true;
   }

   // find an existing template
   if (GetTypes().JoinGroupType(fixedGroupName, fixedGroupName, aPlatformPartPtr)) // all tokens replaced
   {
      return true;
   }
   else if (GetTypes().JoinGroupType(typeGroupName, fixedGroupName, aPlatformPartPtr)) // platform type replaced
   {
      return true;
   }
   else if (GetTypes().JoinGroupType(aGroupName, fixedGroupName, aPlatformPartPtr)) // raw string
   {
      return true;
   }
   else // no version of the group was found, throw an error
   {
      auto out = ut::log::error() << "Group not found.";
      out.AddNote() << "Platform: " << aPlatformPartPtr->GetPlatform()->GetName();
      out.AddNote() << "Group Name: " << fixedGroupName;
   }

   return false;
}

// =================================================================================================
//! A convenience method to return the group type list.
// private
const WsfGroupTypes& WsfGroupManager::GetTypes() const
{
   return WsfGroupTypes::Get(mSimPtr->GetScenario());
}
