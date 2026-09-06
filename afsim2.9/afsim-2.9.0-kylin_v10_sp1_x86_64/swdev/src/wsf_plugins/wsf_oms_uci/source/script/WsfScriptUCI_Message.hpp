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

#ifndef UCI_MESSAGECLASS_HPP
#define UCI_MESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "script/WsfScriptMessageClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

namespace script
{

//! Define script methods for UCI_Message.
class WSF_OMS_UCI_EXPORT UCI_MessageClass : public WsfScriptMessageClass
{
   public:
      UCI_MessageClass(const std::string& aClassName,
                       UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Header);

};

}

}

#endif
