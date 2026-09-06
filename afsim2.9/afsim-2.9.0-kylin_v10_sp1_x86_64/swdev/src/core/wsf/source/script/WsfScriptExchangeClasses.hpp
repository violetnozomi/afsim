// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTEXCHANGECLASSES_HPP
#define WSFSCRIPTEXCHANGECLASSES_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_EXPORT WsfScriptTenderClass : public UtScriptClass
{
public:
   WsfScriptTenderClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // static
   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(ItemName);
   UT_DECLARE_SCRIPT_METHOD(Quantity);
   UT_DECLARE_SCRIPT_METHOD(Rate);
   UT_DECLARE_SCRIPT_METHOD(IsService);
   UT_DECLARE_SCRIPT_METHOD(IsCommodity);
   UT_DECLARE_SCRIPT_METHOD(Interval);
   UT_DECLARE_SCRIPT_METHOD(Increment);
   UT_DECLARE_SCRIPT_METHOD(Decrement);
};

class WSF_EXPORT WsfScriptContainerClass : public UtScriptClass
{
public:
   WsfScriptContainerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(IsEmpty);
   UT_DECLARE_SCRIPT_METHOD(IsFull);
   UT_DECLARE_SCRIPT_METHOD(CurrentQuantity);
   UT_DECLARE_SCRIPT_METHOD(MaximumQuantity);
   UT_DECLARE_SCRIPT_METHOD(QuantityNeeded);
   UT_DECLARE_SCRIPT_METHOD(IsCompatibleWith);
   UT_DECLARE_SCRIPT_METHOD(TenderNeeded);
   UT_DECLARE_SCRIPT_METHOD(TenderRemaining);
   UT_DECLARE_SCRIPT_METHOD(MaximumTender);
   UT_DECLARE_SCRIPT_METHOD(Increment);
   UT_DECLARE_SCRIPT_METHOD(Decrement);
   UT_DECLARE_SCRIPT_METHOD(SetQuantity);
};

class WSF_EXPORT WsfScriptQueryClass : public UtScriptClass
{
public:
   WsfScriptQueryClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsService);
   UT_DECLARE_SCRIPT_METHOD(IsOfferor);
   UT_DECLARE_SCRIPT_METHOD(IsDone);
   UT_DECLARE_SCRIPT_METHOD(OwningIndex);
   UT_DECLARE_SCRIPT_METHOD(RequestId);
   UT_DECLARE_SCRIPT_METHOD(RespondingIndex);
   UT_DECLARE_SCRIPT_METHOD(RequestingIndex);
   UT_DECLARE_SCRIPT_METHOD(OfferedQuantity);
   UT_DECLARE_SCRIPT_METHOD(DesiredQuantity);
   UT_DECLARE_SCRIPT_METHOD(NegotiatedQuantity);
   UT_DECLARE_SCRIPT_METHOD(ItemName);
   UT_DECLARE_SCRIPT_METHOD(StateName);
   UT_DECLARE_SCRIPT_METHOD(EventName);
};

class WSF_EXPORT WsfScriptTransactorClass : public UtScriptClass
{
public:
   WsfScriptTransactorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(Container);
   UT_DECLARE_SCRIPT_METHOD(Query);

   UT_DECLARE_SCRIPT_METHOD(IsReady);
   UT_DECLARE_SCRIPT_METHOD(IsInProgress);
   UT_DECLARE_SCRIPT_METHOD(ContinueToOffer);
   UT_DECLARE_SCRIPT_METHOD(IsEnabled);
   UT_DECLARE_SCRIPT_METHOD(IsOfferor);
   UT_DECLARE_SCRIPT_METHOD(IsRequester);
   UT_DECLARE_SCRIPT_METHOD(IsHookedToFuel);
   UT_DECLARE_SCRIPT_METHOD(IsHookedToPayload);
   UT_DECLARE_SCRIPT_METHOD(IsExclusiveHook);
   UT_DECLARE_SCRIPT_METHOD(ProximityLimit);
   UT_DECLARE_SCRIPT_METHOD(ReserveTender); // ONLY VALID FOR A SUPPLIER
   UT_DECLARE_SCRIPT_METHOD(ReserveAll);    // ONLY VALID FOR A SUPPLIER
   UT_DECLARE_SCRIPT_METHOD(RequestFrom);   // ONLY VALID FOR A REQUESTOR
   UT_DECLARE_SCRIPT_METHOD(Cancel);
   UT_DECLARE_SCRIPT_METHOD(Disable);
};

#endif
