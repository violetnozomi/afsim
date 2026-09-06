// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptCommRouterClass.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfCommRouterTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptCommAddress.hpp"
#include "script/WsfScriptDefs.hpp"

using Router  = wsf::comm::router::Router;
using Comm    = wsf::comm::Comm;
using Address = wsf::comm::Address;

// ============================================================================
WsfScriptCommRouterClass::WsfScriptCommRouterClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptPlatformPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommRouter");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());

   AddMethod(ut::make_unique<GetMedium>());

   AddMethod(ut::make_unique<HasGateway>());
   AddMethod(ut::make_unique<GetGateway>());
   AddMethod(ut::make_unique<SetGateway>());
   AddMethod(ut::make_unique<GetHopLimit>());
   AddMethod(ut::make_unique<SetHopLimit>());

   AddMethod(ut::make_unique<HasInterface>());
   AddMethod(ut::make_unique<AddInterface>());
   AddMethod(ut::make_unique<RemoveInterface>());
   AddMethod(ut::make_unique<GetInterfaces>());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, WsfScriptCommRouterClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool    isA_TypeOf = false;
   Router* objectPtr  = wsf::comm::router::Types::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptCommRouterClass, Router, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOff(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptCommRouterClass, Router, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOn(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, GetMedium, 0, "WsfCommMedium", "")
{
   auto                       mediumContainer = wsf::comm::medium::ContainerComponent<Router>::Find(*aObjectPtr);
   wsf::comm::medium::Medium* mediumPtr       = nullptr;
   if (mediumContainer)
   {
      mediumPtr = mediumContainer->GetMedium();
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(mediumPtr, aReturnClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, HasGateway, 0, "bool", "")
{
   aReturnVal.SetBool(!aObjectPtr->GetGateway().IsNull());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, GetGateway, 0, "WsfAddress", "")
{
   const auto& gateway = aObjectPtr->GetGateway();
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(gateway));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, SetGateway, 1, "void", "WsfAddress")
{
   auto gatewayPtr = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aObjectPtr->SetGateway(*gatewayPtr);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, GetHopLimit, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetHopLimit()));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, SetHopLimit, 1, "void", "int")
{
   auto hopLimit = aVarArgs[0].GetInt();
   aObjectPtr->SetHopLimit(ut::cast_to_size_t(hopLimit));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, HasInterface, 1, "bool", "WsfAddress")
{
   auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aReturnVal.SetBool(aObjectPtr->HasInterface(*address));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, AddInterface, 1, "void", "WsfAddress")
{
   auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aObjectPtr->AddInterface(TIME_NOW, *address);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, RemoveInterface, 1, "void", "WsfAddress")
{
   auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aObjectPtr->RemoveInterface(TIME_NOW, *address);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommRouterClass, Router, GetInterfaces, 0, "Array<WsfAddress>", "")
{
   auto& interfaces = aObjectPtr->GetInterfaces();
   auto  data       = ut::make_unique<std::vector<UtScriptData>>();

   for (auto interfacePtr : interfaces)
   {
      data->emplace_back(WsfScriptCommAddressClass::Create(interfacePtr->GetAddress()));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}
