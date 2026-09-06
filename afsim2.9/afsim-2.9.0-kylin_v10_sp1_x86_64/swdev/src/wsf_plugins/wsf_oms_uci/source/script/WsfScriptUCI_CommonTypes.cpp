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

#include "WsfScriptUCI_CommonTypes.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtCalendar.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_InterfaceExtension.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_CapabilityIdClass::UCI_CapabilityIdClass(const std::string& aClassName,
                                             UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityIdClass, UCI_CapabilityID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityIdClass, UCI_CapabilityID_Type, Descriptor, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr &&
       aObjectPtr->Get().hasDescriptiveLabel())
   {
      aReturnVal.SetString(aObjectPtr->Get().getDescriptiveLabel());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UCI_CommandIdClass::UCI_CommandIdClass(const std::string& aClassName,
                                       UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_CommandIdClass, UCI_CapabilityCommandID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_CommandIdClass, UCI_CapabilityCommandID_Type, Descriptor, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr && aObjectPtr->Get().hasDescriptiveLabel())
   {
      aReturnVal.SetString(aObjectPtr->Get().getDescriptiveLabel());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

//=============================================================================

UCI_SystemIdClass::UCI_SystemIdClass(const std::string& aClassName,
                                     UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_SystemIdClass, UCI_SystemID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_SystemIdClass, UCI_SystemID_Type, Descriptor, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr && aObjectPtr->Get().hasDescriptiveLabel())
   {
      aReturnVal.SetString(aObjectPtr->Get().getDescriptiveLabel());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

//=============================================================================
UCI_EntityIdClass::UCI_EntityIdClass(const std::string& aClassName,
                                     UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_EntityIdClass, UCI_EntityID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_EntityIdClass, UCI_EntityID_Type, Descriptor, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr && aObjectPtr->Get().hasDescriptiveLabel())
   {
      aReturnVal.SetString(aObjectPtr->Get().getDescriptiveLabel());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

//===========================================================================================
UCI_ActivityIdClass::UCI_ActivityIdClass(const std::string& aClassName,
                                         UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ActivityIdClass, UCI_ActivityID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ActivityIdClass, UCI_ActivityID_Type, Descriptor, 0, "string", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr && aObjectPtr->Get().hasDescriptiveLabel())
   {
      std::string descriptor = aObjectPtr->Get().getDescriptiveLabel();
      aReturnVal.SetString(descriptor);
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

//============================================================================================
UCI_MessageHeaderClass::UCI_MessageHeaderClass(const std::string& aClassName,
                                               UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<SystemId>());
   AddMethod(ut::make_unique<Timestamp>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_MessageHeaderClass, UCI_HeaderType, SystemId, 0, "UCI_SystemId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto systemIdPtr = new UCI_SystemID_Type(*interfacePtr, aObjectPtr->Get().getSystemID());
      aReturnVal.SetPointer(new UtScriptRef(systemIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_MessageHeaderClass, UCI_HeaderType, Timestamp, 0, "Calendar", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto time = UCI_Util::ConvertUCI_DateTimeToDouble(aObjectPtr->Get().getTimestamp());

      UtCalendar timestampStart;
      timestampStart.SetDate(1970, 1, 1);
      timestampStart.SetTime(0);

      timestampStart.AdvanceTimeBy(time);
      aReturnVal.SetPointer(new UtScriptRef(new UtCalendar(timestampStart), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//=============================================================================================================================
UCI_LOS_ReferenceTypeClass::UCI_LOS_ReferenceTypeClass(const std::string& aClassName,
                                                       UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<BODY>());
   AddStaticMethod(ut::make_unique<INERTIAL>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_LOS_ReferenceTypeClass, UCI_LOS_ReferenceTypeClass, BODY, 0, "UCI_LOS_Reference", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = new UCI_LOS_ReferenceEnumType(SIMULATION);
      referencePtr->Get().setValue(uci::type::LOS_ReferenceEnum::UCI_BODY);
      aReturnVal.SetPointer(new UtScriptRef(referencePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_LOS_ReferenceTypeClass, UCI_LOS_ReferenceTypeClass, INERTIAL, 0, "UCI_LOS_Reference", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = new UCI_LOS_ReferenceEnumType(SIMULATION);
      referencePtr->Get().setValue(uci::type::LOS_ReferenceEnum::UCI_INERTIAL);
      aReturnVal.SetPointer(new UtScriptRef(referencePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//=============================================================================================================================
UCI_ElevationScanStabilizationTypeClass::UCI_ElevationScanStabilizationTypeClass(const std::string& aClassName,
                                                                                 UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<BODY>());
   AddStaticMethod(ut::make_unique<CENTER_ALTITUDE>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ElevationScanStabilizationTypeClass, UCI_ElevationScanStabilizationTypeClass, BODY, 0, "UCI_ElevationScanStabilization", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = new UCI_ElevationScanStabilizationEnumType(SIMULATION);
      referencePtr->Get().setValue(uci::type::ElevationScanStabilizationEnum::UCI_BODY);
      aReturnVal.SetPointer(new UtScriptRef(referencePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ElevationScanStabilizationTypeClass, UCI_ElevationScanStabilizationTypeClass, CENTER_ALTITUDE, 0, "UCI_ElevationScanStabilization", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = new UCI_ElevationScanStabilizationEnumType(SIMULATION);
      referencePtr->Get().setValue(uci::type::ElevationScanStabilizationEnum::UCI_CENTER_ALTITUDE);
      aReturnVal.SetPointer(new UtScriptRef(referencePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//========================================================================
UCI_CapabilityStatusTypeClass::UCI_CapabilityStatusTypeClass(const std::string& aClassName,
                                                             UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);
   AddMethod(ut::make_unique<CapabilityId>());
   AddMethod(ut::make_unique<IsAvailable>());
   AddMethod(ut::make_unique<IsUnavailable>());
   AddMethod(ut::make_unique<IsTemporarilyUnavailable>());
   AddMethod(ut::make_unique<IsExpended>());
   AddMethod(ut::make_unique<IsDisabled>());
   AddMethod(ut::make_unique<IsFaulted>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, CapabilityId, 0, "UCI_CapabilityId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityIdPtr = new UCI_CapabilityID_Type(*interfacePtr,
                                                       aObjectPtr->Get().getCapabilityID());
      aReturnVal.SetPointer(new UtScriptRef(capabilityIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsAvailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsUnavailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_UNAVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsTemporarilyUnavailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_TEMPORARILY_UNAVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsExpended, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_EXPENDED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsDisabled, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_DISABLED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStatusTypeClass, UCI_CapabilityStatusType, IsFaulted, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_FAULTED);
}

//========================================================================
UCI_CapabilityStateTypeClass::UCI_CapabilityStateTypeClass(const std::string& aClassName,
                                                             UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);
   AddStaticMethod(ut::make_unique<ENABLE>());
   AddStaticMethod(ut::make_unique<DISABLE>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStateTypeClass, UCI_CapabilityStateType, ENABLE, 0, "UCI_CapabilityState", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto statePtr = new UCI_CapabilityStateType(SIMULATION);
      statePtr->Get().getCommandedState().setValue(uci::type::CapabilityStateCommandEnum::UCI_ENABLE);
      aReturnVal.SetPointer(new UtScriptRef(statePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_CapabilityStateTypeClass, UCI_CapabilityStateType, DISABLE, 0, "UCI_CapabilityState", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto statePtr = new UCI_CapabilityStateType(SIMULATION);
      statePtr->Get().getCommandedState().setValue(uci::type::CapabilityStateCommandEnum::UCI_DISABLE);
      aReturnVal.SetPointer(new UtScriptRef(statePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//===========================================================================================
UCI_DMPI_IdClass::UCI_DMPI_IdClass(const std::string& aClassName,
                                   UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<UUID>());
   AddMethod(ut::make_unique<Descriptor>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_IdClass, UCI_DMPI_ID_Type, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_DMPI_IdClass, UCI_DMPI_ID_Type, Descriptor, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr && aObjectPtr->Get().hasDescriptiveLabel())
   {
      std::string descriptor = aObjectPtr->Get().getDescriptiveLabel();
      aReturnVal.SetString(descriptor);
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

} // script
} // wsf
