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

#ifndef UCI_POST_CAPABILITYMESSAGECLASS_HPP
#define UCI_POST_CAPABILITYMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "WsfScriptUCI_Message.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

namespace script
{

//! Define script methods for UCI_POST_CapabilityMessage.
class WSF_OMS_UCI_EXPORT UCI_POST_CapabilityMessageClass : public UCI_MessageClass
{
   public:
      UCI_POST_CapabilityMessageClass(const std::string& aClassName,
                                      UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Capability);
      UT_DECLARE_SCRIPT_METHOD(Size);
};

}

}

#endif
