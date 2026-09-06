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

#ifndef UCI_CONTROLSTATUSMESSAGECLASS_HPP
#define UCI_CONTROLSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{

//! Define script methods for ControlStatusMessage.
class WSF_OMS_UCI_EXPORT UCI_ControlStatusMessageClass : public UCI_MessageClass
{
   public:
      UCI_ControlStatusMessageClass(const std::string& aClassName,
                                    UtScriptTypes*     aTypesPtr);
};

}

}

#endif
