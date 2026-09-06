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

#ifndef UCIAMTISETTINGSCOMMANDMESSAGECLASS_HPP
#define UCIAMTISETTINGSCOMMANDMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{
   class UCI_MessageClass;

//! Define script methods for AMTI_SettingsCommandMessage.
class WSF_OMS_UCI_EXPORT UCI_AMTI_SettingsCommandMessageClass : public UCI_MessageClass
{
   public:
      UCI_AMTI_SettingsCommandMessageClass(const std::string& aClassName,
                                    UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct);

};

}

}

#endif
