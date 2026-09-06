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

#ifndef UCIAMTICAPABILITYMESSAGECLASS_HPP
#define UCIAMTICAPABILITYMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptTypes.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{

//! Define script methods for AMTI_CapabilityMessage.
class WSF_OMS_UCI_EXPORT UCI_AMTI_CapabilityMessageClass : public UCI_MessageClass
{
   public:
      UCI_AMTI_CapabilityMessageClass(const std::string& aClassName,
                                      UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Capability);
      UT_DECLARE_SCRIPT_METHOD(Size);

};

}

}

#endif
