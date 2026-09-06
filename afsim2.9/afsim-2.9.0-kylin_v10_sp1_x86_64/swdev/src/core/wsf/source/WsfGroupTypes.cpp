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

#include "WsfGroupTypes.hpp"

#include <iostream>
#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfGroupTypes& WsfGroupTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetGroupTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfGroupTypes& WsfGroupTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetGroupTypes();
}

// =================================================================================================
WsfGroupTypes::WsfGroupTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfGroup>(aScenario, "group")
{
   AddCoreType("WSF_GROUP", ut::make_unique<WsfGroup>());
}

// =================================================================================================
//! if exists, clone the given group type and join it.
//!
//! @param aGroupType - the type of the group to join
//! @param aGroupName - the name of the group to join
//! @param aPlatformPtr - the platform that will join the group
//! @return true if successfully joined group
bool WsfGroupTypes::JoinGroupType(const std::string& aGroupType, const std::string& aGroupName, WsfPlatform* aPlatformPtr) const
{
   // find an existing template
   if (Find(aGroupType) != nullptr)
   {
      std::unique_ptr<WsfGroup> instancePtr(Clone(aGroupType));
      if (instancePtr != nullptr)
      {
         instancePtr->SetName(aGroupName);
         aPlatformPtr->JoinGroup(instancePtr.release());
         return true;
      }
   }
   return false;
}

// =================================================================================================
//! if exists, clone the given group type and join it.
//!
//! @param aGroupType - the type of the group to join
//! @param aGroupName - the name of the group to join
//! @param aPlatformPartPtr - the platform part that will join the group
//! @return true if successfully joined group
bool WsfGroupTypes::JoinGroupType(const std::string& aGroupType,
                                  const std::string& aGroupName,
                                  WsfPlatformPart*   aPlatformPartPtr) const
{
   // find an existing template
   if (Find(aGroupType) != nullptr)
   {
      std::unique_ptr<WsfGroup> instancePtr(Clone(aGroupType));
      if (instancePtr != nullptr)
      {
         instancePtr->SetName(aGroupName);
         aPlatformPartPtr->JoinGroup(instancePtr.release());
         return true;
      }
   }
   return false;
}

// =================================================================================================
WsfGroupTypes::LoadResult WsfGroupTypes::LoadType(UtInput& aInput)
{
   return WsfObjectTypeList<WsfGroup>::LoadType(aInput);
}

// =================================================================================================
//! Create a group type that will be cloned later when instantiated
//!
//! @param aInput - stream from input file
//! @param aPlatformPtr - the platform used to get the token replacements from
//! @return true if successfully created group type
bool WsfGroupTypes::LoadType(UtInput& aInput, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr == nullptr)
   {
      return false;
   }

   if (aInput.GetCommand() == "group")
   {
      UtInputBlock inputBlock(aInput);
      std::string  groupName;
      std::string  groupType;

      aInput.ReadValue(groupName);
      aInput.ReadValue(groupType);

      // find and replace any tokens in the string
      std::string lowerGroupName = groupName;
      UtStringUtil::ToLower(lowerGroupName);
      if (aPlatformPtr->GetNameId() > nullptr) // only replace tokens on platforms, not platform types
      {
         size_t tokenPos = lowerGroupName.find("<plat_type>");
         if (tokenPos != std::string::npos)
         {
            groupName.replace(tokenPos, 11, aPlatformPtr->GetType());
            lowerGroupName = groupName;
            UtStringUtil::ToLower(lowerGroupName);
         }
         tokenPos = lowerGroupName.find("<plat_name>");
         if (tokenPos != std::string::npos)
         {
            groupName.replace(tokenPos, 11, aPlatformPtr->GetName());
         }
      }

      // create a template for this group type
      std::unique_ptr<WsfGroup> groupPtr(Clone(groupType));
      if (groupPtr != nullptr)
      {
         inputBlock.ProcessInput(groupPtr.get());
         if (!Add(groupName, std::move(groupPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate group name: " + groupName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown group type: " + groupType);
      }

      return true;
   }

   return false;
}

// =================================================================================================
//! Create a group type that will be cloned later when instantiated
//!
//! @param aInput - stream from input file
//! @param aPlatformPartPtr - the platform part used to get the token replacements from
//! @return true if successfully created group type
bool WsfGroupTypes::LoadType(UtInput& aInput, WsfPlatformPart* aPlatformPartPtr)
{
   if (aPlatformPartPtr == nullptr)
   {
      return false;
   }

   if (aInput.GetCommand() == "group")
   {
      UtInputBlock inputBlock(aInput);
      std::string  groupName;
      std::string  groupType;

      aInput.ReadValue(groupName);
      aInput.ReadValue(groupType);

      std::unique_ptr<WsfGroup> groupPtr(Clone(groupType));
      if (groupPtr != nullptr)
      {
         inputBlock.ProcessInput(groupPtr.get());
         if (!Add(groupName, std::move(groupPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate group type: " + groupName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown group type: " + groupType);
      }

      return true;
   }

   return false;
}
