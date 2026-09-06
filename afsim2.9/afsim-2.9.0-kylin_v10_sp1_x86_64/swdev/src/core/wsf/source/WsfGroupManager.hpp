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

#ifndef WSFGROUPMANAGER_HPP
#define WSFGROUPMANAGER_HPP

#include "wsf_export.h"

#include <map>
#include <string>

class UtInput;
class WsfGroup;
class WsfGroupManager;
class WsfGroupTypes;
class WsfPlatform;
class WsfPlatformPart;
class WsfSimulation;
#include "WsfStringId.hpp"

//! This is a singleton that contains a list of objects that have been
//! created as 'group objects'.

class WSF_EXPORT WsfGroupManager
{
public:
   WsfGroupManager() = default;
   WsfGroupManager(WsfSimulation* aSimPtr);
   ~WsfGroupManager();

   bool AddGroup(WsfGroup* aNewGroupPtr);

   WsfGroup* CreateGroup(const std::string& aGroupName, const std::string& aGroupType);

   WsfGroup* GetGroup(const WsfStringId aGroupId);

   using GroupMap = std::map<WsfStringId, WsfGroup*>;

   GroupMap& GetGroups() { return mGroups; }

   bool LoadInstance(const std::string& aGroupName, WsfPlatformPart* aPlatformPartPtr);
   bool LoadInstance(const std::string& aGroupName, WsfPlatform* aPlatformPtr);

private:
   const WsfGroupTypes& GetTypes() const;

   WsfSimulation* mSimPtr{nullptr};
   GroupMap       mGroups;
};

#endif
