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

#ifndef UCI_INTERFACE_HPP
#define UCI_INTERFACE_HPP

#include "wsf_oms_uci_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_InterfaceClass : public UtScriptClass
      {
         public:
            UCI_InterfaceClass(const std::string& aClassName,
                               UtScriptTypes*     aScriptTypesPtr);
            ~UCI_InterfaceClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return WsfUCI_ControlTypeClass
            UT_DECLARE_SCRIPT_METHOD(GetSensorFromCapabilityID);
            UT_DECLARE_SCRIPT_METHOD(GenerateRandomUUID);
      };

   }

}

#endif
