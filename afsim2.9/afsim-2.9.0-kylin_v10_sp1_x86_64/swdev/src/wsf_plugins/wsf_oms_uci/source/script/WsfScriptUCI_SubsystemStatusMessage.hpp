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

#ifndef UCI_SUBSYSTEMSTATUSMESSAGECLASS_HPP
#define UCI_SUBSYSTEMSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "script/WsfScriptUCI_Message.hpp"
class UtScriptClass;

namespace wsf
{

namespace script
{

//! Define script methods for UCI_SubsystemStatusMessage.
class WSF_OMS_UCI_EXPORT UCI_SubsystemStatusMessageClass : public UCI_MessageClass
{
   public:
      UCI_SubsystemStatusMessageClass(const std::string& aClassName,
                                      UtScriptTypes*     aTypesPtr);
};

}

}

#endif
