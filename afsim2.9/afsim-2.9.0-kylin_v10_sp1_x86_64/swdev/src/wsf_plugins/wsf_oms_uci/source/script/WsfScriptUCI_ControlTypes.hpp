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

#ifndef UCI_CONTROLTYPES_HPP
#define UCI_CONTROLTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

namespace script
{

//! These objects are meant to be passed as arguments to UCI script messages as parameters.
class WSF_OMS_UCI_EXPORT UCI_ControlTypeClass : public UtScriptClass
{
   public:
      UCI_ControlTypeClass(const std::string& aClassName,
                           UtScriptTypes*     aScriptTypesPtr);
      ~UCI_ControlTypeClass() override = default;

      //!@name Script methods documented in the AFSIM Reference Guide:
      // Methods that return WsfUCI_ControlTypeClass
      UT_DECLARE_SCRIPT_METHOD(MISSION);
      UT_DECLARE_SCRIPT_METHOD(CAPABILITY_PRIMARY);
      UT_DECLARE_SCRIPT_METHOD(CAPABILITY_SECONDARY);
      UT_DECLARE_SCRIPT_METHOD(CAPABILITY_MANAGER);
};

//! These objects are meant to be passed as arguments to UCI script messages as parameters.
class WSF_OMS_UCI_EXPORT UCI_ControlRequestTypeClass : public UtScriptClass
{
   public:
      UCI_ControlRequestTypeClass(const std::string& aClassName,
                                  UtScriptTypes*     aScriptTypesPtr);
      ~UCI_ControlRequestTypeClass() override = default;

      //!@name Script methods documented in the AFSIM Reference Guide:
      // Methods that return UCI_ControlRequestTypeClass
      UT_DECLARE_SCRIPT_METHOD(ACQUIRE);
      UT_DECLARE_SCRIPT_METHOD(RELEASE);
};

}

}

#endif
