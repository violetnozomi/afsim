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

#ifndef WSFSCRIPTGROUPCLASS_HPP
#define WSFSCRIPTGROUPCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptAuxDataUtil.hpp"
#include "WsfScriptObjectClass.hpp"

//! Define script methods for WsfSimulation.
class WSF_EXPORT WsfScriptGroupClass : public WsfScriptObjectClass
{
public:
   WsfScriptGroupClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;
   void  OnNewScriptRef(UtScriptRef& aReference) override;

   // Group management methods
   UT_DECLARE_SCRIPT_METHOD(AddMember_1);    // Add the given platform to the group
   UT_DECLARE_SCRIPT_METHOD(AddMember_2);    // Add the given platform part to the group
   UT_DECLARE_SCRIPT_METHOD(RemoveMember_1); // Remove the given platform from the group
   UT_DECLARE_SCRIPT_METHOD(RemoveMember_2); // Remove the given platform part from the group
   UT_DECLARE_SCRIPT_METHOD(MembersByName);  // Returns a list of member names
   UT_DECLARE_SCRIPT_METHOD(Members);        // Returns a list of member objects
   UT_DECLARE_SCRIPT_METHOD(Member);         // Returns a member given an index
   UT_DECLARE_SCRIPT_METHOD(Size);           // Returns the number of members in the group

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()

   // Static methods
   UT_DECLARE_SCRIPT_METHOD(CreateGroup); // creates a group of the given type with the given name
   UT_DECLARE_SCRIPT_METHOD(Group);       // returns a pointer to the group matching the given name
   UT_DECLARE_SCRIPT_METHOD(Groups);      // returns a list of the names of all groups in the simulation
};

#endif
