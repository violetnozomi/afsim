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

#include "WsfScriptUCI_AMTI_CommandTypes.hpp"

#include "script/WsfScriptDefs.hpp"

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_AMTI_CommandTypeClass::UCI_AMTI_CommandTypeClass(const std::string& aClassName,
                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CommandTypeClass, UCI_AMTI_CommandTypeClass, Construct_1, 1, "UCI_AMTI_Command",
                        "UCI_AMTI_CapabilityCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_AMTI_CapabilityCommandType>();
      auto commandPtr = new UCI_AMTI_CommandType(SIMULATION);
      commandPtr->Get().setAMTI_CommandTypeChoiceOrdinal(uci::type::AMTI_CommandType::AMTI_COMMANDTYPE_CHOICE_CAPABILITY);
      commandPtr->Get().setCapability(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CommandTypeClass, UCI_AMTI_CommandTypeClass, Construct_2, 1, "UCI_AMTI_Command",
                        "UCI_AMTI_ActivityCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_AMTI_ActivityCommandType>();
      auto commandPtr = new UCI_AMTI_CommandType(SIMULATION);
      commandPtr->Get().setAMTI_CommandTypeChoiceOrdinal(uci::type::AMTI_CommandType::AMTI_COMMANDTYPE_CHOICE_ACTIVITY);
      commandPtr->Get().setActivity(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//================================================================================================
UCI_AMTI_CapabilityCommandTypeClass::UCI_AMTI_CapabilityCommandTypeClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<PushBack>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityCommandTypeClass, UCI_AMTI_CapabilityCommandTypeClass, Construct, 2,
                        "UCI_AMTI_CapabilityCommand", "UCI_CapabilityId, UCI_AirTargetVolumeCommand")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_CapabilityID_Type>();
      auto volumeCommandPtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_AirTargetVolumeCommandType>();

      auto commandPtr = new UCI_AMTI_CapabilityCommandType(WsfScriptContext::GetPLATFORM(aContext));
      commandPtr->Get().setCapabilityID(capabilityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      commandPtr->Get().getRanking().getRank().setPrecedence(0);
      commandPtr->Get().getRanking().getRank().setPriority(0);
      commandPtr->Get().getTargetVolume().push_back(volumeCommandPtr->Get());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityCommandTypeClass, UCI_AMTI_CapabilityCommandType, PushBack, 1,
                        "void", "UCI_AirTargetVolumeCommand")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto volumeCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_AirTargetVolumeCommandType>();
      aObjectPtr->Get().getTargetVolume().push_back(volumeCommandPtr->Get());
   }
}

//================================================================================================
UCI_AMTI_ActivityCommandTypeClass::UCI_AMTI_ActivityCommandTypeClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_ActivityCommandTypeClass, UCI_AMTI_ActivityCommandTypeClass, Construct, 1, "UCI_AMTI_ActivityCommand",
                        "UCI_ActivityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto activityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ActivityID_Type>();
      auto commandPtr = new UCI_AMTI_ActivityCommandType(SIMULATION);
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
UCI_AirTargetVolumeCommandTypeClass::UCI_AirTargetVolumeCommandTypeClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AirTargetVolumeCommandTypeClass, UCI_AirTargetVolumeCommandTypeClass, Construct, 4,
                        "UCI_AirTargetVolumeCommand", "UCI_LOS_Reference, UCI_ElevationScanStabilization, double, double")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto azReferenceTypePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_LOS_ReferenceEnumType>();
      auto elReferenceTypePtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_ElevationScanStabilizationEnumType>();
      auto azScanCenter = aVarArgs[2].GetDouble();
      auto elScanCenter = aVarArgs[3].GetDouble();

      auto targetVolumePtr = new UCI_AirTargetVolumeCommandType(SIMULATION);
      targetVolumePtr->Get().setAirTargetVolumeCommandTypeChoiceOrdinal(uci::type::AirTargetVolumeCommandType::AIRTARGETVOLUMECOMMANDTYPE_CHOICE_AIRVOLUME);
      targetVolumePtr->Get().getAirVolume().setAirTargetVolumeTypeChoiceOrdinal(uci::type::AirTargetVolumeType::AIRTARGETVOLUMETYPE_CHOICE_AIRVOLUMESENSORREFERENCED);
      auto& sensorReferenced = targetVolumePtr->Get().getAirVolume().getAirVolumeSensorReferenced();
      sensorReferenced.setAzimuthScanCenter(azScanCenter);
      sensorReferenced.setElevationScanCenter(elScanCenter);
      sensorReferenced.setAzimuthScanStabilization(azReferenceTypePtr->Get());
      sensorReferenced.setElevationScanStabilization(elReferenceTypePtr->Get());

      //set arbitrary values for other required data
      //============================================
      sensorReferenced.setAzimuthScanWidth(0);
      sensorReferenced.setElevationScanWidth(0);
      sensorReferenced.setMaxRangeOfInterest(0);
      sensorReferenced.setMinRangeOfInterest(0);
      sensorReferenced.setRollStabilized(false);
      //============================================

      aReturnVal.SetPointer(new UtScriptRef(targetVolumePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
