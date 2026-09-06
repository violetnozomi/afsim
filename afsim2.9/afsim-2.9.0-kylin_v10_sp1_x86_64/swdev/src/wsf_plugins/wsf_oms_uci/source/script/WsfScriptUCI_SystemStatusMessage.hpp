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

#ifndef UCISYSTEMSTATUSMESSAGECLASS_HPP
#define UCISYSTEMSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{
   class UCI_MessageClass;

//! Define script methods for SystemStatusMessage.
class WSF_OMS_UCI_EXPORT UCI_SystemStatusMessageClass : public UCI_MessageClass
{
   public:
      UCI_SystemStatusMessageClass(const std::string& aClassName,
                                   UtScriptTypes*     aTypesPtr);

};

}

}

#endif
