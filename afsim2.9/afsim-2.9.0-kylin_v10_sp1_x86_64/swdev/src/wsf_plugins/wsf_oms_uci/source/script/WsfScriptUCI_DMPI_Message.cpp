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

#include "WsfScriptUCI_DMPI_Message.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_DMPI_MessageClass::UCI_DMPI_MessageClass(const std::string& aClassName,
                                             UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddMethod(ut::make_unique<DMPI_ID>());
   AddMethod(ut::make_unique<SetWeaponeering>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_MessageClass, UCI_DMPI_Message, Construct_1, 3, "UCI_DMPI_Message", "double, double, double")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto messagePtr = new UCI_DMPI_Message(*interfacePtr);
      auto lat = aVarArgs[0].GetDouble();
      auto lon = aVarArgs[1].GetDouble();
      auto alt = aVarArgs[2].GetDouble();

      ISM_OwnerProducerType ownPro(SIMULATION);
      ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
      messagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());
      auto& data = messagePtr->Get().getMessageData();
      auto& coordinates = data.getDMPI_Location().getDMPI_Coordinates();
      coordinates.setAltitude(alt);
      coordinates.setLatitude(lat);
      coordinates.setLongitude(lon);
      data.getDMPI_ID().setUUID(interfacePtr->GetConnection()->generateUUID());
      aReturnVal.SetPointer(new UtScriptRef(messagePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_MessageClass, UCI_DMPI_Message, Construct_2, 4, "UCI_DMPI_Message", "UCI_DMPI_ID, double, double, double")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto messagePtr = new UCI_DMPI_Message(*interfacePtr);
      auto idPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_DMPI_ID_Type>();
      auto lat = aVarArgs[1].GetDouble();
      auto lon = aVarArgs[2].GetDouble();
      auto alt = aVarArgs[3].GetDouble();

      ISM_OwnerProducerType ownPro(SIMULATION);
      ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
      messagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());
      auto& data = messagePtr->Get().getMessageData();
      auto& coordinates = data.getDMPI_Location().getDMPI_Coordinates();
      coordinates.setAltitude(alt);
      coordinates.setLatitude(lat);
      coordinates.setLongitude(lon);
      data.setDMPI_ID(idPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(messagePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_MessageClass, UCI_DMPI_Message, DMPI_ID, 0, "UCI_DMPI_ID", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto idPtr = new UCI_DMPI_ID_Type(*interfacePtr, aObjectPtr->Get().getMessageData().getDMPI_ID());
      aReturnVal.SetPointer(new UtScriptRef(idPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_MessageClass, UCI_DMPI_Message, SetWeaponeering, 3, "void", "int, UCI_StoreCategory, int")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      aObjectPtr->Get().getMessageData().enableWeaponeering();
      auto& weaponeering = aObjectPtr->Get().getMessageData().getWeaponeering();
      auto category = aVarArgs[1].GetPointer()->GetAppObject<UCI_StoreCategoryEnumType>();
      weaponeering.setStoreCategory(category->Get());
      weaponeering.setStoreQuantity(aVarArgs[2].GetInt());
      weaponeering.setStoreType(aVarArgs[0].GetInt());
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//==================================================================
UCI_DMPI_StatusMessageClass::UCI_DMPI_StatusMessageClass(const std::string& aClassName,
                                                         UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<DMPI_ID>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_StatusMessageClass, UCI_DMPI_StatusMessage, DMPI_ID, 0, "UCI_DMPI_ID", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto idPtr = new UCI_DMPI_ID_Type(*interfacePtr, aObjectPtr->Get().getMessageData().getDMPI_ID());
      aReturnVal.SetPointer(new UtScriptRef(idPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//==================================================================
UCI_StoreCategoryEnumClass::UCI_StoreCategoryEnumClass(const std::string& aClassName,
                                                       UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<AIR>());
   AddStaticMethod(ut::make_unique<GROUND>());
   AddStaticMethod(ut::make_unique<GUN>());
   AddStaticMethod(ut::make_unique<SEA_SUBSURFACE>());
   AddStaticMethod(ut::make_unique<SEA_SURFACE>());
   AddStaticMethod(ut::make_unique<SEA_SURFACE_SAMSSM>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, AIR, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_AIR);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, GROUND, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_GROUND);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, GUN, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_GUN);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, SEA_SUBSURFACE, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_SEA_SUBSURFACE);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, SEA_SURFACE, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_SEA_SURFACE);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StoreCategoryEnumClass, UCI_StoreCategoryEnumClass, SEA_SURFACE_SAMSSM, 0, "UCI_StoreCategory", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto storeCategory = new UCI_StoreCategoryEnumType(*interfacePtr);
      storeCategory->Get().setValue(uci::type::StoreCategoryEnum::UCI_SEA_SURFACE_SAMSSM);
      aReturnVal.SetPointer(new UtScriptRef(storeCategory, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
