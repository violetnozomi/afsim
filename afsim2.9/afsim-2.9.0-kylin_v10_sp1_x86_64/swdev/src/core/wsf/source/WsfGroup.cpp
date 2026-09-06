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

#include "WsfGroup.hpp"

#include "UtInput.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "UtStringUtil.hpp"

// =================================================================================================
WsfGroup::WsfGroup(WsfStringId aNameId)
{
   SetName(aNameId);
}

// =================================================================================================
WsfGroup::~WsfGroup()
{
   UtReferenceTracked::FreeReference();
   mMembers.clear();
}

// =================================================================================================
//! virtual
void WsfGroup::Initialize() {}

// =================================================================================================
//! virtual
WsfGroup* WsfGroup::Clone() const
{
   return new WsfGroup(*this);
}

// =================================================================================================
//! Add a new member to this group
void WsfGroup::JoinGroup(size_t aPlatformIndex, unsigned int aPlatformPartId /*=0*/)
{
   if (!IsGroupMember(aPlatformIndex, aPlatformPartId))
   {
      mMembers.emplace_back(aPlatformIndex, aPlatformPartId);
   }
}

// =================================================================================================
//! Remove a member of this group
void WsfGroup::LeaveGroup(size_t aPlatformIndex, unsigned int aPlatformPartId /*=0*/)
{
   MemberList::iterator it = Find(aPlatformIndex, aPlatformPartId);
   if (it != mMembers.end())
   {
      mMembers.erase(it);
   }
}

// =================================================================================================
//! Find a member of this group
WsfGroup::MemberList::iterator WsfGroup::Find(size_t aPlatformIndex, unsigned int aPlatformPartId)
{
   MemberList::iterator it;
   for (it = mMembers.begin(); it != mMembers.end(); ++it)
   {
      if ((*it).first == aPlatformIndex && (*it).second == aPlatformPartId)
      {
         return it;
      }
   }
   return it;
}

// =================================================================================================
//! determine if the given platform or platform part is a member of the group
bool WsfGroup::IsGroupMember(size_t aPlatformIndex, unsigned int aPlatformPartId)
{
   return (Find(aPlatformIndex, aPlatformPartId) != mMembers.end());
}

// =================================================================================================
//! virtual
bool WsfGroup::ProcessInput(UtInput& aInput)
{
   return WsfAuxDataEnabled::ProcessInput(aInput);
}

const char* WsfGroup::GetScriptClassName() const
{
   return "WsfGroup";
}
