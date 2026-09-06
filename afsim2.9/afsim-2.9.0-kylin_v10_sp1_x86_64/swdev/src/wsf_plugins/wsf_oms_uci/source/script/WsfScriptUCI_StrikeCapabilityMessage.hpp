// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UCI_STRIKECAPABILITYMESSAGECLASS_HPP
#define UCI_STRIKECAPABILITYMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{

class WSF_OMS_UCI_EXPORT UCI_StrikeCapabilityMessageClass : public UCI_MessageClass
{
   public:
      UCI_StrikeCapabilityMessageClass(const std::string& aClassName,
                                       UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Capability);
      UT_DECLARE_SCRIPT_METHOD(Size);

};

//! Define script methods for StrikeCapabilityType.
class WSF_OMS_UCI_EXPORT UCI_StrikeCapabilityTypeClass : public UtScriptClass
{
public:
   UCI_StrikeCapabilityTypeClass(const std::string& aClassName,
                                 UtScriptTypes*     aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(CapabilityId);
};

}

}

#endif
