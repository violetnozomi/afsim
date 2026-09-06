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

#include "WsfScriptUCI_POST_CommandTypes.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_POST_CommandTypeClass::UCI_POST_CommandTypeClass(const std::string& aClassName,
                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandTypeClass, UCI_POST_CommandTypeClass, Construct_1, 1, "UCI_POST_Command", "UCI_POST_CapabilityCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_CapabilityCommandType>();
      auto commandPtr = new UCI_POST_CommandType(SIMULATION);
      commandPtr->Get().setPOST_CommandTypeChoiceOrdinal(uci::type::POST_CommandType::POST_COMMANDTYPE_CHOICE_CAPABILITY);
      commandPtr->Get().setCapability(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandTypeClass, UCI_POST_CommandTypeClass, Construct_2, 1, "UCI_POST_Command", "UCI_POST_ActivityCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_ActivityCommandType>();
      auto commandPtr = new UCI_POST_CommandType(SIMULATION);
      commandPtr->Get().setPOST_CommandTypeChoiceOrdinal(uci::type::POST_CommandType::POST_COMMANDTYPE_CHOICE_ACTIVITY);
      commandPtr->Get().setActivity(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//================================================================================================
UCI_POST_CapabilityCommandTypeClass::UCI_POST_CapabilityCommandTypeClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<SetPointing>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CapabilityCommandTypeClass, UCI_POST_CapabilityCommandTypeClass, Construct, 1, "UCI_POST_CapabilityCommand",
                        "UCI_CapabilityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_CapabilityID_Type>();
      auto commandPtr = new UCI_POST_CapabilityCommandType(SIMULATION);
      commandPtr->Get().setCapabilityID(capabilityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      commandPtr->Get().getRanking().getRank().setPrecedence(0);
      commandPtr->Get().getRanking().getRank().setPriority(0);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CapabilityCommandTypeClass, UCI_POST_CapabilityCommandType, SetPointing, 1, "void", "UCI_Pointing")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto pointingPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_PointingType>();
      aObjectPtr->Get().enablePointing();
      aObjectPtr->Get().setPointing(pointingPtr->Get());
   }
}

//================================================================================================
UCI_POST_ActivityCommandTypeClass::UCI_POST_ActivityCommandTypeClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<SetPointing>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_ActivityCommandTypeClass, UCI_POST_ActivityCommandTypeClass, Construct, 1, "UCI_POST_ActivityCommand",
                        "UCI_ActivityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto activityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ActivityID_Type>();
      auto commandPtr = new UCI_POST_ActivityCommandType(SIMULATION);
      commandPtr->Get().setActivityID(activityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_ActivityCommandTypeClass, UCI_POST_ActivityCommandType, SetPointing, 1, "void", "UCI_Pointing")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto pointingPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_PointingType>();
      aObjectPtr->Get().enablePointing();
      aObjectPtr->Get().setPointing(pointingPtr->Get());
   }
}

//================================================================================================
UCI_PointingTypeClass::UCI_PointingTypeClass(const std::string& aClassName,
                                             UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   
   AddMethod(ut::make_unique<SetLOSAzimuth>());
   AddMethod(ut::make_unique<SetLOSElevation>());
   AddMethod(ut::make_unique<SetLOSReference>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_PointingTypeClass, UCI_PointingTypeClass, Construct, 3, "UCI_Pointing",
                        "UCI_LOS_Reference, double, double")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto pointingPtr = new UCI_PointingType(SIMULATION);
      auto referencePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_LOS_ReferenceEnumType>();
      auto az = aVarArgs[1].GetDouble();
      auto el = aVarArgs[2].GetDouble();
      pointingPtr->Get().setPointingTypeChoiceOrdinal(uci::type::PointingType::POINTINGTYPE_CHOICE_LOSOPTION);
      pointingPtr->Get().getLOSOption().setLOS_D_TypeChoiceOrdinal(uci::type::LOS_D_Type::LOS_D_TYPE_CHOICE_LOS);
      auto& los = pointingPtr->Get().getLOSOption().getLOS();
      los.setReference(referencePtr->Get());
      los.setAzimuth(az);
      los.setElevation(el);
      aReturnVal.SetPointer(new UtScriptRef(pointingPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_PointingTypeClass, UCI_PointingType, SetLOSAzimuth, 1, "void",
                        "double")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      aObjectPtr->Get().getLOSOption().getLOS().setAzimuth(aVarArgs[0].GetDouble());
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_PointingTypeClass, UCI_PointingType, SetLOSElevation, 1, "void",
                        "double")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aObjectPtr->Get().getLOSOption().getLOS().setElevation(aVarArgs[0].GetDouble());
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_PointingTypeClass, UCI_PointingType, SetLOSReference, 1, "void",
                        "UCI_LOS_Reference")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_LOS_ReferenceEnumType>();
      aObjectPtr->Get().getLOSOption().getLOS().setReference(referencePtr->Get());
   }
}

} // script
} // wsf
