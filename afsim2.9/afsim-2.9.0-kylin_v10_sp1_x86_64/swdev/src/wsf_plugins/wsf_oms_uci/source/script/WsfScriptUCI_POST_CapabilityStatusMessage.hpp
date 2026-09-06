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

#ifndef UCI_POST_CAPABILITYSTATUSMESSAGECLASS_HPP
#define UCI_POST_CAPABILITYSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for UCI_POST_CapabilityStatusMessage.
      class WSF_OMS_UCI_EXPORT UCI_POST_CapabilityStatusMessageClass : public UCI_MessageClass
      {
         public:
            UCI_POST_CapabilityStatusMessageClass(const std::string& aClassName,
                                                  UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CapabilityStatus);
            UT_DECLARE_SCRIPT_METHOD(Size);
      };

      class WSF_OMS_UCI_EXPORT UCI_POST_CapabilityStatusTypeClass : public UtScriptClass
      {
         public:
            UCI_POST_CapabilityStatusTypeClass(const std::string& aClassName,
                                               UtScriptTypes*     aScriptTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CapabilityId);
            UT_DECLARE_SCRIPT_METHOD(IsAvailable);
            UT_DECLARE_SCRIPT_METHOD(IsUnavailable);
            UT_DECLARE_SCRIPT_METHOD(IsTemporarilyUnavailable);
            UT_DECLARE_SCRIPT_METHOD(IsExpended);
            UT_DECLARE_SCRIPT_METHOD(IsDisabled);
            UT_DECLARE_SCRIPT_METHOD(IsFaulted);
      };
   }

}

#endif
