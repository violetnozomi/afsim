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

#include "script/WsfScriptExchangeClasses.hpp"

#include "UtMemory.hpp"
#include "WsfExchange.hpp"
#include "script/WsfScriptContext.hpp"

// ==== Tender ===

WsfScriptTenderClass::WsfScriptTenderClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("Tender");
   // mIsScriptAccessible = true;
   mConstructible = true;
   mCloneable     = true;

   AddStaticMethod(ut::make_unique<Construct>()); // Create(string ItemName, double Quantity, double Rate, bool IsService)

   AddMethod(ut::make_unique<ItemName>());
   AddMethod(ut::make_unique<Quantity>());
   AddMethod(ut::make_unique<Rate>());
   AddMethod(ut::make_unique<IsService>());
   AddMethod(ut::make_unique<IsCommodity>());
   AddMethod(ut::make_unique<Interval>());
   AddMethod(ut::make_unique<Increment>());
   AddMethod(ut::make_unique<Decrement>());
}

// virtual
void* WsfScriptTenderClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfExchange::Tender();
}

// virtual
void* WsfScriptTenderClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfExchange::Tender*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptTenderClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfExchange::Tender*>(aObjectPtr);
}

//! Construct(string ItemId, double Quantity, double Rate, bool IsService)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Construct, 4, "Tender", "string,double,double,bool")
{
   // See WsfScriptGeoPointClass for a good template class.
   WsfStringId          itemId(aVarArgs[0].GetString());
   double               quantity  = aVarArgs[1].GetDouble();
   double               rate      = aVarArgs[2].GetDouble();
   bool                 isService = aVarArgs[3].GetBool();
   WsfExchange::Tender* tenderPtr = new WsfExchange::Tender(itemId, quantity, rate, isService);
   aReturnVal.SetPointer(new UtScriptRef(tenderPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, ItemName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Quantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Quantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Rate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Rate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Interval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Interval());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, IsService, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsService());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, IsCommodity, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCommodity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Increment, 1, "void", "double")
{
   aObjectPtr->Increment(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTenderClass, WsfExchange::Tender, Decrement, 1, "void", "double")
{
   aObjectPtr->Decrement(aVarArgs[0].GetDouble());
}

// ==== Query ===

WsfScriptQueryClass::WsfScriptQueryClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("Query");

   AddMethod(ut::make_unique<IsService>());
   AddMethod(ut::make_unique<IsOfferor>());
   AddMethod(ut::make_unique<IsDone>());
   AddMethod(ut::make_unique<OwningIndex>());
   AddMethod(ut::make_unique<RequestId>());
   AddMethod(ut::make_unique<RespondingIndex>());
   AddMethod(ut::make_unique<RequestingIndex>());
   AddMethod(ut::make_unique<OfferedQuantity>());
   AddMethod(ut::make_unique<DesiredQuantity>());
   AddMethod(ut::make_unique<NegotiatedQuantity>());
   AddMethod(ut::make_unique<ItemName>());
   AddMethod(ut::make_unique<StateName>());
   AddMethod(ut::make_unique<EventName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, IsService, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsService());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, IsOfferor, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsOfferor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, OwningIndex, 0, "int", "")
{
   aReturnVal.SetInt((int)aObjectPtr->OwningIndex());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, RequestId, 0, "int", "")
{
   aReturnVal.SetInt((int)aObjectPtr->RequestId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, RespondingIndex, 0, "int", "")
{
   aReturnVal.SetInt((int)aObjectPtr->RespondingIndex());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, RequestingIndex, 0, "int", "")
{
   aReturnVal.SetInt((int)aObjectPtr->RequestingIndex());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, OfferedQuantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->OfferedQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, DesiredQuantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->DesiredQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, NegotiatedQuantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->NegotiatedQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, IsDone, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsDone());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, ItemName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->ItemId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, StateName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->StateId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQueryClass, WsfExchange::Query, EventName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->EventId().GetString());
}

// ==== Container ===

WsfScriptContainerClass::WsfScriptContainerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("Container");

   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<IsEmpty>());
   AddMethod(ut::make_unique<IsFull>());
   AddMethod(ut::make_unique<CurrentQuantity>());
   AddMethod(ut::make_unique<MaximumQuantity>());
   AddMethod(ut::make_unique<QuantityNeeded>());
   AddMethod(ut::make_unique<IsCompatibleWith>());
   AddMethod(ut::make_unique<TenderNeeded>());
   AddMethod(ut::make_unique<TenderRemaining>());
   AddMethod(ut::make_unique<MaximumTender>());
   AddMethod(ut::make_unique<Increment>());
   AddMethod(ut::make_unique<Decrement>());
   AddMethod(ut::make_unique<SetQuantity>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, CurrentQuantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->CurrentQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, MaximumTender, 0, "Tender", "")
{
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->MaximumTender(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, TenderNeeded, 0, "Tender", "")
{
   WsfExchange::Tender* tPtr = new WsfExchange::Tender(aObjectPtr->TenderNeeded());
   aReturnVal.SetPointer(new UtScriptRef(tPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, TenderRemaining, 0, "Tender", "")
{
   WsfExchange::Tender* tPtr = new WsfExchange::Tender(aObjectPtr->TenderRemaining());
   aReturnVal.SetPointer(new UtScriptRef(tPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, MaximumQuantity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MaximumQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, QuantityNeeded, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->QuantityNeeded());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, IsCompatibleWith, 1, "bool", "Tender")
{
   const WsfExchange::Tender* tPtr = (const WsfExchange::Tender*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->IsCompatibleWith(*tPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, IsEmpty, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsEmpty());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, IsFull, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsFull());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, Increment, 1, "double", "double")
{
   aReturnVal.SetDouble(aObjectPtr->Increment(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, Decrement, 1, "double", "double")
{
   aReturnVal.SetDouble(aObjectPtr->Decrement(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, SetQuantity, 1, "bool", "double")
{
   aReturnVal.SetBool(aObjectPtr->SetQuantity(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptContainerClass, WsfExchange::Container, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

// ==== Transactor ===

WsfScriptTransactorClass::WsfScriptTransactorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("Transactor");

   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<Container>());
   AddMethod(ut::make_unique<Query>());

   AddMethod(ut::make_unique<IsReady>());
   AddMethod(ut::make_unique<IsInProgress>());
   AddMethod(ut::make_unique<ContinueToOffer>());
   AddMethod(ut::make_unique<IsEnabled>());
   AddMethod(ut::make_unique<IsOfferor>());
   AddMethod(ut::make_unique<IsRequester>());
   AddMethod(ut::make_unique<IsHookedToFuel>());
   AddMethod(ut::make_unique<IsHookedToPayload>());
   AddMethod(ut::make_unique<IsExclusiveHook>());
   AddMethod(ut::make_unique<ProximityLimit>());
   AddMethod(ut::make_unique<ReserveTender>());
   AddMethod(ut::make_unique<ReserveAll>());
   AddMethod(ut::make_unique<RequestFrom>());
   AddMethod(ut::make_unique<Cancel>());
   AddMethod(ut::make_unique<Disable>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, Container, 0, "Container", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->ContainerPtr(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, Query, 0, "Query", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetQueryPtr(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsOfferor, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsOfferor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsReady, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsReady());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsInProgress, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsInProgress());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsEnabled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsEnabled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsHookedToFuel, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsHookedToFuel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsHookedToPayload, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsHookedToPayload());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsExclusiveHook, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExclusiveHook());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, ContinueToOffer, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->ContinueToOffer());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, IsRequester, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsRequester());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, ProximityLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->ProximityLimit());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, Cancel, 0, "void", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Cancel(simTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, Disable, 0, "void", "")
{
   aObjectPtr->Disable();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, ReserveAll, 0, "bool", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   bool   success = aObjectPtr->ReserveAll(simTime);
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, ReserveTender, 1, "bool", "Tender")
{
   double               simTime = WsfScriptContext::GetTIME_NOW(aContext);
   WsfExchange::Tender* tPtr    = (WsfExchange::Tender*)aVarArgs[0].GetPointer()->GetAppObject();
   bool                 success = aObjectPtr->ReserveTender(simTime, *tPtr);
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, RequestFrom, 2, "bool", "WsfPlatform,Tender")
{
   double                     simTime = WsfScriptContext::GetTIME_NOW(aContext);
   const WsfPlatform*         platPtr = (const WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   const WsfExchange::Tender* tPtr    = (const WsfExchange::Tender*)aVarArgs[1].GetPointer()->GetAppObject();
   bool                       success = aObjectPtr->RequestFrom(simTime, platPtr, *tPtr);
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTransactorClass, WsfExchange::Transactor, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}
