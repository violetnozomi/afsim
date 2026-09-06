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

#ifndef WSFGROUP_HPP
#define WSFGROUP_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class UtInput;
#include "UtReferenceTracked.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfAuxDataEnabled.hpp"
#include "WsfObject.hpp"
#include "WsfStringId.hpp"

//! A class to support the concept of 'groups' and 'group membership'.
class WSF_EXPORT WsfGroup : public WsfObject, public WsfAuxDataEnabled
{
public:
   using GroupPair  = std::pair<size_t, unsigned int>;
   using MemberList = std::vector<GroupPair>;

   WsfGroup() = default;
   WsfGroup(WsfStringId aNameId);
   WsfGroup& operator=(const WsfGroup&) = delete;
   ~WsfGroup() override;

   WsfGroup* Clone() const override;

   const char* GetScriptClassName() const override;

   static UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   MemberList::iterator Find(size_t aPlatformIndex, unsigned int aPlatformPartId);

   virtual void Initialize();

   bool IsGroupMember(size_t aPlatformIndex, unsigned int aPlatformPartId = 0);

   void JoinGroup(size_t aPlatformIndex, unsigned int aPlatformPartId = 0);

   void LeaveGroup(size_t aPlatformIndex, unsigned int aPlatformPartId = 0);

   MemberList&       GetMembers() { return mMembers; }
   const MemberList& GetMembers() const { return mMembers; }

   bool ProcessInput(UtInput& aInput) override;

protected:
   WsfGroup(const WsfGroup& aSrc) = default;

private:
   MemberList mMembers; //!< <platform index, platform part id> platform part id is 0 if it is a platform
};

#endif
