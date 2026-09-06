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

#ifndef UCISTRIKECOMMANDSTATUSMESSAGECLASS_HPP
#define UCISTRIKECOMMANDSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for StrikeCommandStatusMessage.
      class WSF_OMS_UCI_EXPORT UCI_StrikeCommandStatusMessageClass : public UCI_MessageClass
      {
         public:
            UCI_StrikeCommandStatusMessageClass(const std::string& aClassName,
                                                 UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CommandUUID);
            UT_DECLARE_SCRIPT_METHOD(IsAccepted);
            UT_DECLARE_SCRIPT_METHOD(IsRejected);
            UT_DECLARE_SCRIPT_METHOD(IsRemoved);
            UT_DECLARE_SCRIPT_METHOD(IsReceived);

      };
   }

}

#endif
