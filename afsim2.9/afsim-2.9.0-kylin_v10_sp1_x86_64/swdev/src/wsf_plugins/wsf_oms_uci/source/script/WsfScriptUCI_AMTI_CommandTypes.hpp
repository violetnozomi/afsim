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

#ifndef UCI_AMTICOMMANDTYPES_HPP
#define UCI_AMTICOMMANDTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! Define script methods for Pointing type.
      class WSF_OMS_UCI_EXPORT UCI_AMTI_CommandTypeClass : public UtScriptClass
      {
         public:
            UCI_AMTI_CommandTypeClass(const std::string& aClassName,
                                      UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct_1);
            UT_DECLARE_SCRIPT_METHOD(Construct_2);
      };

      class WSF_OMS_UCI_EXPORT UCI_AMTI_CapabilityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_AMTI_CapabilityCommandTypeClass(const std::string& aClassName,
                                                UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(PushBack);
      };

      class WSF_OMS_UCI_EXPORT UCI_AMTI_ActivityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_AMTI_ActivityCommandTypeClass(const std::string& aClassName,
                                              UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
      };

      class WSF_OMS_UCI_EXPORT UCI_AirTargetVolumeCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_AirTargetVolumeCommandTypeClass(const std::string& aClassName,
                                                UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
      };

   }

}

#endif
