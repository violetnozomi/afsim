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

#ifndef UCIAMTISTRIKEMESSAGECLASS_HPP
#define UCIAMTISTRIKEMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;
#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

namespace script
{
   class UCI_MessageClass;

//! Define script methods for StrikeActivityMessage.
class WSF_OMS_UCI_EXPORT UCI_StrikeActivityMessageClass : public UCI_MessageClass
{
   public:
      UCI_StrikeActivityMessageClass(const std::string& aClassName,
                                     UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(Activity);
      UT_DECLARE_SCRIPT_METHOD(Size);

};

//! Define script methods for StrikeCapabilityType.
class WSF_OMS_UCI_EXPORT UCI_StrikeActivityTypeClass : public UtScriptClass
{
   public:
      UCI_StrikeActivityTypeClass(const std::string& aClassName,
                                  UtScriptTypes*     aTypesPtr);

      UT_DECLARE_SCRIPT_METHOD(ActivityId);
      UT_DECLARE_SCRIPT_METHOD(CapabilityId);
      UT_DECLARE_SCRIPT_METHOD(Size);
      UT_DECLARE_SCRIPT_METHOD(IsReadyForRelease);
      UT_DECLARE_SCRIPT_METHOD(IsSelectedForJettison);
      UT_DECLARE_SCRIPT_METHOD(IsSelectedForKeyLoad);
      UT_DECLARE_SCRIPT_METHOD(IsSelectedForRelease);
      UT_DECLARE_SCRIPT_METHOD(IsWeaponArmed);

      //Activity State
      UT_DECLARE_SCRIPT_METHOD(HasFailed);
      UT_DECLARE_SCRIPT_METHOD(IsActiveFullyConstrained);
      UT_DECLARE_SCRIPT_METHOD(IsActivePartiallyConstrained);
      UT_DECLARE_SCRIPT_METHOD(IsActiveUnconstrained);
      UT_DECLARE_SCRIPT_METHOD(IsCompleted);
      UT_DECLARE_SCRIPT_METHOD(IsDeleted);
      UT_DECLARE_SCRIPT_METHOD(IsDisabled);
      UT_DECLARE_SCRIPT_METHOD(IsEnabled);
};

}

}

#endif
