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

#include "WsfScriptUCI_ESM_CommandTypes.hpp"

#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptTypes.hpp"
#include "UtScriptRef.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_ESM_CommandTypeClass::UCI_ESM_CommandTypeClass(const std::string& aClassName,
                                                   UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CommandTypeClass, UCI_ESM_CommandTypeClass, Construct_1, 1, "UCI_ESM_Command",
                        "UCI_ESM_CapabilityCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ESM_CapabilityCommandType>();
      auto commandPtr = new UCI_ESM_CommandType(SIMULATION);
      commandPtr->Get().setESM_CommandTypeChoiceOrdinal(uci::type::ESM_CommandType::ESM_COMMANDTYPE_CHOICE_CAPABILITY);
      commandPtr->Get().setCapability(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CommandTypeClass, UCI_ESM_CommandTypeClass, Construct_2, 1, "UCI_ESM_Command",
                        "UCI_ESM_ActivityCommand")
{
  if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ESM_ActivityCommandType>();
      auto commandPtr = new UCI_ESM_CommandType(SIMULATION);
      commandPtr->Get().setESM_CommandTypeChoiceOrdinal(uci::type::ESM_CommandType::ESM_COMMANDTYPE_CHOICE_ACTIVITY);
      commandPtr->Get().setActivity(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//================================================================================================
UCI_ESM_CapabilityCommandTypeClass::UCI_ESM_CapabilityCommandTypeClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<PushBack>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CapabilityCommandTypeClass, UCI_ESM_CapabilityCommandTypeClass, Construct, 2,
                        "UCI_ESM_CapabilityCommand", "UCI_CapabilityId, UCI_SubCapabilitySelection")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_CapabilityID_Type>();
      auto selectionPtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_SubCapabilitySelectionType>();

      auto commandPtr = new UCI_ESM_CapabilityCommandType(WsfScriptContext::GetPLATFORM(aContext));
      commandPtr->Get().setCapabilityID(capabilityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      commandPtr->Get().getRanking().getRank().setPrecedence(0);
      commandPtr->Get().getRanking().getRank().setPriority(0);
      commandPtr->Get().getSubCapabilitySelection().push_back(selectionPtr->Get());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CapabilityCommandTypeClass, UCI_ESM_CapabilityCommandType, PushBack, 1,
                        "void", "UCI_SubCapabilitySelection")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto subCapabilitySelection = aVarArgs[0].GetPointer()->GetAppObject<UCI_SubCapabilitySelectionType>();
      aObjectPtr->Get().getSubCapabilitySelection().push_back(subCapabilitySelection->Get());
   }
}

//================================================================================================
UCI_ESM_ActivityCommandTypeClass::UCI_ESM_ActivityCommandTypeClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_ActivityCommandTypeClass, UCI_ESM_ActivityCommandTypeClass, Construct, 1, "UCI_ESM_ActivityCommand",
                        "UCI_ActivityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto activityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ActivityID_Type>();
      auto commandPtr = new UCI_ESM_ActivityCommandType(SIMULATION);
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

//================================================================================================
UCI_SubCapabilitySelectionTypeClass::UCI_SubCapabilitySelectionTypeClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<PushBack>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_SubCapabilitySelectionTypeClass, UCI_SubCapabilitySelectionTypeClass, Construct, 1,
                        "UCI_SubCapabilitySelection", "UCI_SubCapabilityDetails")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto detailsPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_SubCapabilityDetailsType>();

      auto selectionPtr = new UCI_SubCapabilitySelectionType(WsfScriptContext::GetPLATFORM(aContext));
      selectionPtr->Get().getSubCapabilityDetails().push_back(detailsPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(selectionPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_SubCapabilitySelectionTypeClass, UCI_SubCapabilitySelectionType, PushBack, 1,
                        "void", "UCI_SubCapabilityDetails")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto subCapabilityDetails = aVarArgs[0].GetPointer()->GetAppObject<UCI_SubCapabilityDetailsType>();
      aObjectPtr->Get().getSubCapabilityDetails().push_back(subCapabilityDetails->Get());
   }
}

//================================================================================================
UCI_SubCapabilityDetailsTypeClass::UCI_SubCapabilityDetailsTypeClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_SubCapabilityDetailsTypeClass, UCI_SubCapabilityDetailsTypeClass, Construct, 4,
                        "UCI_SubCapabilityDetails", "UCI_LOS_Reference, UCI_ElevationScanStabilization, double, double")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto azReferenceTypePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_LOS_ReferenceEnumType>();
      auto elReferenceTypePtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_ElevationScanStabilizationEnumType>();
      auto azScanCenter = aVarArgs[2].GetDouble();
      auto elScanCenter = aVarArgs[3].GetDouble();

      auto detailsPtr = new UCI_SubCapabilityDetailsType(WsfScriptContext::GetPLATFORM(aContext));
      detailsPtr->Get().enableESM_Location();
      detailsPtr->Get().getESM_Location().setESM_LocationTypeChoiceOrdinal(uci::type::ESM_LocationType::ESM_LOCATIONTYPE_CHOICE_ESM_AIRVOLUME);
      auto& airVolume = detailsPtr->Get().getESM_Location().getESM_AirVolume();
      airVolume.setAzimuthScanCenter(azScanCenter);
      airVolume.setElevationScanCenter(elScanCenter);
      airVolume.setAzimuthScanStabilization(azReferenceTypePtr->Get());
      airVolume.setElevationScanStabilization(elReferenceTypePtr->Get());

      //set arbitrary values for other required data
      //============================================
      airVolume.setAzimuthScanWidth(0);
      airVolume.setElevationScanWidth(0);
      airVolume.setMaxRangeOfInterest(0);
      airVolume.setMinRangeOfInterest(0);
      airVolume.setRollStabilized(false);
      //============================================

      aReturnVal.SetPointer(new UtScriptRef(detailsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf