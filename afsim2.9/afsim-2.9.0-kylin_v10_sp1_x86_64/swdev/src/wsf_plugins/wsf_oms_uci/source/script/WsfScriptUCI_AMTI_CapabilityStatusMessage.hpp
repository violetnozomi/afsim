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

#ifndef UCIAMTICAPABILITYSTATUSMESSAGECLASS_HPP
#define UCIAMTICAPABILITYSTATUSMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;

#include "UtScriptClassDefine.hpp"
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for UCI_AMTI_CapabilityStatusMessage.
      class WSF_OMS_UCI_EXPORT UCI_AMTI_CapabilityStatusMessageClass : public UCI_MessageClass
      {
         public:
            UCI_AMTI_CapabilityStatusMessageClass(const std::string& aClassName,
                                                  UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CapabilityStatus);
            UT_DECLARE_SCRIPT_METHOD(Size);

      };

   }

}

#endif
