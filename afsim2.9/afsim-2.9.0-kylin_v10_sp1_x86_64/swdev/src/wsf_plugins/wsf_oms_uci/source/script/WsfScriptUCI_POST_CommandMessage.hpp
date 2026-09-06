// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UCI_POST_COMMANDMESSAGECLASS_HPP
#define UCI_POST_COMMANDMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{

//! Define script methods for UCI_POST_CommandMessage.
class WSF_OMS_UCI_EXPORT UCI_POST_CommandMessageClass : public UCI_MessageClass
{
   public:
      UCI_POST_CommandMessageClass(const std::string& aClassName,
                                   UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct);
      UT_DECLARE_SCRIPT_METHOD(LOS);
      UT_DECLARE_SCRIPT_METHOD(PushBack);
      UT_DECLARE_SCRIPT_METHOD(CommandUUID);

};

class WSF_OMS_UCI_EXPORT UCI_LOS_TypeClass : public UtScriptClass
{
   public:
      UCI_LOS_TypeClass(const std::string& aClassName,
                        UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Azimuth);
      UT_DECLARE_SCRIPT_METHOD(Elevation);
      UT_DECLARE_SCRIPT_METHOD(Reference);
};

}

}

#endif
