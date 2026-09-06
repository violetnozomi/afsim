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

#ifndef UCI_COMMONTYPES_HPP
#define UCI_COMMONTYPES_HPP

#include "wsf_oms_uci_export.h"

#include "script/WsfScriptMessageClass.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class UtScriptTypes;

namespace wsf
{

   namespace script
   {

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_CapabilityIdClass : public UtScriptClass
      {
         public:
            UCI_CapabilityIdClass(const std::string& aClassName,
                                  UtScriptTypes*     aScriptTypesPtr);
            ~UCI_CapabilityIdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_CapabilityIdClass
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_SystemIdClass : public UtScriptClass
      {
         public:
            UCI_SystemIdClass(const std::string& aClassName,
                              UtScriptTypes*     aScriptTypesPtr);
            ~UCI_SystemIdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_SystemIdClass
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_EntityIdClass : public UtScriptClass
      {
         public:
            UCI_EntityIdClass(const std::string& aClassName,
                              UtScriptTypes*     aScriptTypesPtr);
            ~UCI_EntityIdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_EntityIdClass
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_ActivityIdClass : public UtScriptClass
      {
         public:
            UCI_ActivityIdClass(const std::string& aClassName,
                                UtScriptTypes*     aScriptTypesPtr);
            ~UCI_ActivityIdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_ActivityIdClass
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_CommandIdClass : public UtScriptClass
      {
         public:
            UCI_CommandIdClass(const std::string& aClassName,
                                UtScriptTypes*     aScriptTypesPtr);
            ~UCI_CommandIdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_ActivityIdClass
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };

      class WSF_OMS_UCI_EXPORT UCI_MessageHeaderClass : public UtScriptClass
      {
         public:
            UCI_MessageHeaderClass(const std::string& aClassName,
                                   UtScriptTypes*     aScriptTypesPtr);
            ~UCI_MessageHeaderClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_SystemIdClass
            UT_DECLARE_SCRIPT_METHOD(SystemId);
            UT_DECLARE_SCRIPT_METHOD(Timestamp);
      };
      class WSF_OMS_UCI_EXPORT UCI_LOS_ReferenceTypeClass : public UtScriptClass
      {
         public:
            UCI_LOS_ReferenceTypeClass(const std::string& aClassName,
                                       UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(BODY);
            UT_DECLARE_SCRIPT_METHOD(INERTIAL);
      };

      class WSF_OMS_UCI_EXPORT UCI_ElevationScanStabilizationTypeClass : public UtScriptClass
      {
         public:
            UCI_ElevationScanStabilizationTypeClass(const std::string& aClassName,
                                                    UtScriptTypes*     aTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(BODY);
            UT_DECLARE_SCRIPT_METHOD(CENTER_ALTITUDE);
      };

      class WSF_OMS_UCI_EXPORT UCI_CapabilityStatusTypeClass : public UtScriptClass
      {
         public:
            UCI_CapabilityStatusTypeClass(const std::string& aClassName,
                                          UtScriptTypes*     aScriptTypesPtr);

            UT_DECLARE_SCRIPT_METHOD(CapabilityId);
            UT_DECLARE_SCRIPT_METHOD(IsAvailable);
            UT_DECLARE_SCRIPT_METHOD(IsUnavailable);
            UT_DECLARE_SCRIPT_METHOD(IsTemporarilyUnavailable);
            UT_DECLARE_SCRIPT_METHOD(IsExpended);
            UT_DECLARE_SCRIPT_METHOD(IsDisabled);
            UT_DECLARE_SCRIPT_METHOD(IsFaulted);
           
      };

      class WSF_OMS_UCI_EXPORT UCI_CapabilityStateTypeClass : public UtScriptClass
      {
      public:
         UCI_CapabilityStateTypeClass(const std::string& aClassName,
                                      UtScriptTypes*     aScriptTypesPtr);

         UT_DECLARE_SCRIPT_METHOD(ENABLE);
         UT_DECLARE_SCRIPT_METHOD(DISABLE);
      };
   

      //! These objects are meant to be passed as arguments to UCI script messages as parameters.
      class WSF_OMS_UCI_EXPORT UCI_DMPI_IdClass : public UtScriptClass
      {
         public:
            UCI_DMPI_IdClass(const std::string& aClassName,
                             UtScriptTypes*     aScriptTypesPtr);
            ~UCI_DMPI_IdClass() override = default;

            //!@name Script methods documented in the AFSIM Reference Guide:
            // Methods that return UCI_DMPI_ID
            UT_DECLARE_SCRIPT_METHOD(UUID);
            UT_DECLARE_SCRIPT_METHOD(Descriptor);
      };
   }
}

#endif
