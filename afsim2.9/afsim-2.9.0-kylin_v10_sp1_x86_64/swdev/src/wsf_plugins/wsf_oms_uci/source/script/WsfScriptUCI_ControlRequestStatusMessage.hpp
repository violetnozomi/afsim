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

#ifndef UCI_CONTROLREQUESTSTATUSMESSAGECLASS_HPP
#define UCI_CONTROLREQUESTSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for Control Request Status Message.
      class WSF_OMS_UCI_EXPORT UCI_ControlRequestStatusMessageClass : public UCI_MessageClass
      {
         public:
            UCI_ControlRequestStatusMessageClass(const std::string& aClassName,
                                                 UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(GetRemarks);
            UT_DECLARE_SCRIPT_METHOD(IsApproved);
            UT_DECLARE_SCRIPT_METHOD(IsRejected);
            UT_DECLARE_SCRIPT_METHOD(IsPending);
            UT_DECLARE_SCRIPT_METHOD(IsDeleted);
            UT_DECLARE_SCRIPT_METHOD(UUID);

      };

   }

}

#endif
