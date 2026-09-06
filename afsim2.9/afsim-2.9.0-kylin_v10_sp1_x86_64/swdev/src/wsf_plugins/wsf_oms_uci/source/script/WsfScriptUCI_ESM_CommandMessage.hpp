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

#ifndef UCI_ESM_COMMANDMESSAGECLASS_HPP
#define UCI_ESM_COMMANDMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "UtScriptClassDefine.hpp"
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for ESM_CommandMessage.
      class WSF_OMS_UCI_EXPORT UCI_ESM_CommandMessageClass : public UCI_MessageClass
      {
         public:
            UCI_ESM_CommandMessageClass(const std::string& aClassName,
                                        UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(PushBack);
            UT_DECLARE_SCRIPT_METHOD(CommandUUID);

      };

   }

}

#endif
