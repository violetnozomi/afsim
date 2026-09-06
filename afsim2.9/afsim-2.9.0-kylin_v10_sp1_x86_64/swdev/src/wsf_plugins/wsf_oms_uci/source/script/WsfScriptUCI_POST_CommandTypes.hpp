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

#ifndef UCI_POSTCOMMANDTYPES_HPP
#define UCI_POSTCOMMANDTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

namespace script
{

//! Define script methods for Pointing type.
class WSF_OMS_UCI_EXPORT UCI_POST_CommandTypeClass : public UtScriptClass
{
   public:
      UCI_POST_CommandTypeClass(const std::string& aClassName,
                                UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct_1);
      UT_DECLARE_SCRIPT_METHOD(Construct_2);
};

class WSF_OMS_UCI_EXPORT UCI_POST_CapabilityCommandTypeClass : public UtScriptClass
{
   public:
      UCI_POST_CapabilityCommandTypeClass(const std::string& aClassName,
                                          UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct);
      UT_DECLARE_SCRIPT_METHOD(SetPointing);
};

class WSF_OMS_UCI_EXPORT UCI_POST_ActivityCommandTypeClass : public UtScriptClass
{
   public:
      UCI_POST_ActivityCommandTypeClass(const std::string& aClassName,
                                        UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct);
      UT_DECLARE_SCRIPT_METHOD(SetPointing);
};

//! Define script methods for Pointing type.
class WSF_OMS_UCI_EXPORT UCI_PointingTypeClass : public UtScriptClass
{
   public:
      UCI_PointingTypeClass(const std::string& aClassName,
                            UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Construct);
      UT_DECLARE_SCRIPT_METHOD(SetLOSAzimuth);
      UT_DECLARE_SCRIPT_METHOD(SetLOSElevation);
      UT_DECLARE_SCRIPT_METHOD(SetLOSReference);
};


}

}

#endif
