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

#ifndef UCI_ESMCOMMANDTYPES_HPP
#define UCI_ESMCOMMANDTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! Define script methods for Pointing type.
      class WSF_OMS_UCI_EXPORT UCI_ESM_CommandTypeClass : public UtScriptClass
      {
         public:
            UCI_ESM_CommandTypeClass(const std::string& aClassName,
                                     UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct_1);
            UT_DECLARE_SCRIPT_METHOD(Construct_2);
      };

      class WSF_OMS_UCI_EXPORT UCI_ESM_CapabilityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_ESM_CapabilityCommandTypeClass(const std::string& aClassName,
                                               UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(PushBack);
      };

      class WSF_OMS_UCI_EXPORT UCI_ESM_ActivityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_ESM_ActivityCommandTypeClass(const std::string& aClassName,
                                             UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
      };

      class WSF_OMS_UCI_EXPORT UCI_SubCapabilitySelectionTypeClass : public UtScriptClass
      {
         public:
            UCI_SubCapabilitySelectionTypeClass(const std::string& aClassName,
                                                UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(PushBack);
      };

      class WSF_OMS_UCI_EXPORT UCI_SubCapabilityDetailsTypeClass : public UtScriptClass
      {
         public:
            UCI_SubCapabilityDetailsTypeClass(const std::string& aClassName,
                                              UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(SetAirVolume);
      };

   }

}

#endif
