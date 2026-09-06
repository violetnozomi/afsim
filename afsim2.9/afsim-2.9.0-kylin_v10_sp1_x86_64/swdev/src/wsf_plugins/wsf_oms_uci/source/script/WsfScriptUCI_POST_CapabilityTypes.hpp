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

#ifndef UCI_POST_CAPABILITYTYPECLASS_HPP
#define UCI_POST_CAPABILITYTYPECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{

namespace script
{

//! Define script methods for WsfFuel.
//! todo make script capability and script capability id script classes.
class WSF_OMS_UCI_EXPORT UCI_POST_CapabilityTypeClass : public UtScriptClass
{
   public:
      UCI_POST_CapabilityTypeClass(const std::string& aClassName,
                                   UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(CapabilityId);
};

}

}

#endif
