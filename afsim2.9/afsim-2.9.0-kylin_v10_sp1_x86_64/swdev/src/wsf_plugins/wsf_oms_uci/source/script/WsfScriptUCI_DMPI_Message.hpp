// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UCIDMPIMESSAGECLASS_HPP
#define UCIDMPIMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClassDefine.hpp"
class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{

//! Define script methods for UCI_DMPI_Message.
class WSF_OMS_UCI_EXPORT UCI_DMPI_MessageClass : public UCI_MessageClass
{
   public:
      UCI_DMPI_MessageClass(const std::string& aClassName,
                            UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct_1);
      UT_DECLARE_SCRIPT_METHOD(Construct_2);
      UT_DECLARE_SCRIPT_METHOD(DMPI_ID);
      UT_DECLARE_SCRIPT_METHOD(SetWeaponeering);

};

//! Define script methods for UCI_DMPI_StatusMessageClass.
class WSF_OMS_UCI_EXPORT UCI_DMPI_StatusMessageClass : public UCI_MessageClass
{
   public:
      UCI_DMPI_StatusMessageClass(const std::string& aClassName,
                                  UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(DMPI_ID);

};

//! Define script methods for UCI_DMPI_StatusMessageClass.
class WSF_OMS_UCI_EXPORT UCI_StoreCategoryEnumClass : public UtScriptClass
{
   public:
      UCI_StoreCategoryEnumClass(const std::string& aClassName,
                                 UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(AIR);
      UT_DECLARE_SCRIPT_METHOD(GROUND);
      UT_DECLARE_SCRIPT_METHOD(GUN);
      UT_DECLARE_SCRIPT_METHOD(SEA_SUBSURFACE);
      UT_DECLARE_SCRIPT_METHOD(SEA_SURFACE);
      UT_DECLARE_SCRIPT_METHOD(SEA_SURFACE_SAMSSM);

};
}

}

#endif
