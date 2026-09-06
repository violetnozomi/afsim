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

#ifndef UCIAMTICAPABILITYTYPECLASS_HPP
#define UCIAMTICAPABILITYTYPECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! Define script methods for UCI_AMTI_CapabilityType.
      class WSF_OMS_UCI_EXPORT UCI_AMTI_CapabilityTypeClass : public UtScriptClass
      {
         public:
            UCI_AMTI_CapabilityTypeClass(const std::string& aClassName,
                                         UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CapabilityId);
      };

   }

}

#endif
