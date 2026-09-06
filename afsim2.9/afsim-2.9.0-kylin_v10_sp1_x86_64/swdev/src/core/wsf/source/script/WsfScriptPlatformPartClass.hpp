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

#ifndef WSFSCRIPTPLATFORMPARTCLASS_HPP
#define WSFSCRIPTPLATFORMPARTCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptAuxDataUtil.hpp"
#include "WsfScriptObjectClass.hpp"

//! Define script methods for WsfPlatformPart.
class WSF_EXPORT WsfScriptPlatformPartClass : public WsfScriptObjectClass
{
public:
   WsfScriptPlatformPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void OnNewScriptRef(UtScriptRef& aReference) override;

   UT_DECLARE_SCRIPT_METHOD(Platform);
   UT_DECLARE_SCRIPT_METHOD(IsNonOperational);
   UT_DECLARE_SCRIPT_METHOD(IsOperational);
   UT_DECLARE_SCRIPT_METHOD(SetOperational);
   UT_DECLARE_SCRIPT_METHOD(IsNonCritical);
   UT_DECLARE_SCRIPT_METHOD(IsCritical);
   UT_DECLARE_SCRIPT_METHOD(SetCritical);
   UT_DECLARE_SCRIPT_METHOD(IsTurnedOff);
   UT_DECLARE_SCRIPT_METHOD(IsTurnedOn);
   UT_DECLARE_SCRIPT_METHOD(InitiallyTurnedOn);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceLastStatusChange);
   UT_DECLARE_SCRIPT_METHOD(SendMessage_1);
   UT_DECLARE_SCRIPT_METHOD(SendMessage_2);
   UT_DECLARE_SCRIPT_METHOD(SendMessageToProcessor); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetDebugEnabled);
   UT_DECLARE_SCRIPT_METHOD(DebugEnabled);
   UT_DECLARE_SCRIPT_METHOD(SetDamageFactor);
   UT_DECLARE_SCRIPT_METHOD(DamageFactor);

   UT_DECLARE_SCRIPT_METHOD(CanBeMadeOperational);    // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(CanBeMadeNonOperational); // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(CanBeTurnedOn);           // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(CanBeTurnedOff);          // NO_DOC | PENDING

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()

   UT_DECLARE_SCRIPT_METHOD(CategoryMemberOf); // CategoryMemberOf(name)

   // Group methods
   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_1); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_2); // GroupMemberOf(string)
   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_3); // GroupMemberOf(string)
   UT_DECLARE_SCRIPT_METHOD(GetGroupByIndex); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GetGroupByName);  // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GroupsByName);    // returns Array<string>
   UT_DECLARE_SCRIPT_METHOD(Groups);          // returns Array<WsfGroup>
   UT_DECLARE_SCRIPT_METHOD(GroupJoin);       // GroupJoin(string)
   UT_DECLARE_SCRIPT_METHOD(GroupLeave);      // GroupLeave(string)
};

#endif
