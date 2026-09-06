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

#ifndef UCISTRIKECOMMANDTYPES_HPP
#define UCISTRIKECOMMANDTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! Define script methods for Pointing type.
      class WSF_OMS_UCI_EXPORT UCI_StrikeCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_StrikeCommandTypeClass(const std::string& aClassName,
                                       UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct_1);
            UT_DECLARE_SCRIPT_METHOD(Construct_2);
      };

      class WSF_OMS_UCI_EXPORT UCI_StrikeCapabilityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_StrikeCapabilityCommandTypeClass(const std::string& aClassName,
                                                 UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
            UT_DECLARE_SCRIPT_METHOD(SetWeaponCommandType);
      };

      class WSF_OMS_UCI_EXPORT UCI_StrikeActivityCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_StrikeActivityCommandTypeClass(const std::string& aClassName,
                                               UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(Construct);
      };

      class WSF_OMS_UCI_EXPORT UCI_StrikeWeaponCommandTypeClass : public UtScriptClass
      {
         public:
            UCI_StrikeWeaponCommandTypeClass(const std::string& aClassName,
                                             UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(AssignDMPI_ID);
            UT_DECLARE_SCRIPT_METHOD(SelectForJettison);
            UT_DECLARE_SCRIPT_METHOD(SelectForKeyLoad);
            UT_DECLARE_SCRIPT_METHOD(SelectForRelease);
            UT_DECLARE_SCRIPT_METHOD(WeaponArm);
      };

 
   }

}

#endif
