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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFSCRIPTCOMMCLASS_HPP
#define WSFSCRIPTCOMMCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptArticulatedPartClass.hpp"
#include "script/WsfScriptEM_InteractionClass.hpp"

//! Define script methods for WsfComm.
class WSF_EXPORT WsfScriptCommClass : public WsfScriptArticulatedPartClass
{
public:
   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   WsfScriptCommClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);

   UT_DECLARE_SCRIPT_METHOD(NetworkName);
   UT_DECLARE_SCRIPT_METHOD(GetAddress);
   UT_DECLARE_SCRIPT_METHOD(GetRouter);

   //! Network Manager pass-through
   UT_DECLARE_SCRIPT_METHOD(GetComm);

   //! Medium script API accessor.
   UT_DECLARE_SCRIPT_METHOD(GetMedium);

   //! Connection methods
   UT_DECLARE_SCRIPT_METHOD(IsConnectedTo_1);
   UT_DECLARE_SCRIPT_METHOD(IsConnectedTo_2);
   UT_DECLARE_SCRIPT_METHOD(PathExistsTo_1);
   UT_DECLARE_SCRIPT_METHOD(PathExistsTo_2);
   UT_DECLARE_SCRIPT_METHOD(IsConnectionEnabled_1);
   UT_DECLARE_SCRIPT_METHOD(IsConnectionEnabled_2);
   UT_DECLARE_SCRIPT_METHOD(EnableConnection_1);
   UT_DECLARE_SCRIPT_METHOD(EnableConnection_2);
   UT_DECLARE_SCRIPT_METHOD(DisableConnection_1);
   UT_DECLARE_SCRIPT_METHOD(DisableConnection_2);
   UT_DECLARE_SCRIPT_METHOD(AddConnection_1);
   UT_DECLARE_SCRIPT_METHOD(AddConnection_2);
   UT_DECLARE_SCRIPT_METHOD(RemoveConnection_1);
   UT_DECLARE_SCRIPT_METHOD(RemoveConnection_2);

   //! Transmission methods
   UT_DECLARE_SCRIPT_METHOD(SendMessage_1);
   UT_DECLARE_SCRIPT_METHOD(SendMessage_2);
   UT_DECLARE_SCRIPT_METHOD(SendMessage_3);
   UT_DECLARE_SCRIPT_METHOD(SendMessageToCommander_1);
   UT_DECLARE_SCRIPT_METHOD(SendMessageToCommander_2);
   UT_DECLARE_SCRIPT_METHOD(SendMessageToPeers);
   UT_DECLARE_SCRIPT_METHOD(SendMessageToSubordinates);

   UT_DECLARE_SCRIPT_METHOD(Xmtr);
   UT_DECLARE_SCRIPT_METHOD(XmtrCount);
   UT_DECLARE_SCRIPT_METHOD(Rcvr);
   UT_DECLARE_SCRIPT_METHOD(RcvrCount);

   UT_DECLARE_SCRIPT_METHOD(CanSendTo);
};

class WSF_EXPORT WsfScriptCommInteractionClass : public WsfScriptEM_InteractionClass
{
public:
   WsfScriptCommInteractionClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
};

#endif
