// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#ifndef WSFSCRIPTCOMMADDRESS_HPP
#define WSFSCRIPTCOMMADDRESS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"
#include "WsfCommAddress.hpp"

class WSF_EXPORT WsfScriptCommAddressClass : public UtScriptClass
{
public:
   WsfScriptCommAddressClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptCommAddressClass() override;

   static UtScriptRef* Create(const wsf::comm::Address& aAddress);
   void*               Create(const UtScriptContext& aContext) override;
   void                Destroy(void* aObject) override;
   void*               Clone(void* aObject) override;
   std::string         ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(WithinRange);
   UT_DECLARE_SCRIPT_METHOD(IntegerValue);

   UT_DECLARE_SCRIPT_METHOD(SetAddress);
   UT_DECLARE_SCRIPT_METHOD(GetAddress);
   UT_DECLARE_SCRIPT_METHOD(SubnetMaskBitSize);
   UT_DECLARE_SCRIPT_METHOD(RoutingPrefix);
   UT_DECLARE_SCRIPT_METHOD(Subnet);
   UT_DECLARE_SCRIPT_METHOD(PossibleHosts);
   UT_DECLARE_SCRIPT_METHOD(IsNull);
   UT_DECLARE_SCRIPT_METHOD(Increment);
   UT_DECLARE_SCRIPT_METHOD(AddressToken);
   UT_DECLARE_SCRIPT_METHOD(GetBroadcastAddress);

private:
   static WsfScriptCommAddressClass* sClassPtr;
};

#endif
