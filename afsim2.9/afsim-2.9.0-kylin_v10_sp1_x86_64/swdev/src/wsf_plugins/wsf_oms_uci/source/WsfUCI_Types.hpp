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

#ifndef WSFUCI_TYPES_HPP
#define WSFUCI_TYPES_HPP

#include "wsf_oms_uci_export.h"

#include "uci/base/accessorType.h"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

#include "WsfUCI_FactoryService.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Utils.hpp"

//types
#include "ismownpro/type/CVEnumISMOwnerProducerValues.h"
#include "uci/type/ActivityID_Type.h"
#include "uci/type/AirTargetVolumeCommandType.h"
#include "uci/type/AMTI_Activity.h"
#include "uci/type/AMTI_ActivityCommandType.h"
#include "uci/type/AMTI_ActivityType.h"
#include "uci/type/AMTI_CapabilityCommandType.h"
#include "uci/type/AMTI_CapabilityType.h"
#include "uci/type/AMTI_CommandType.h"
#include "uci/type/AMTI_SubCapabilityEnum.h"
#include "uci/type/ApprovalEnum.h"
#include "uci/type/CapabilityAvailabilityEnum.h"
#include "uci/type/CapabilityCommandID_Type.h"
#include "uci/type/CapabilityID_Type.h"
#include "uci/type/CapabilityControlInterfacesEnum.h"
#include "uci/type/CapabilityStatusType.h"
#include "uci/type/CapabilityStateType.h"
#include "uci/type/ControleeType.h"
#include "uci/type/ControlRequestEnum.h"
#include "uci/type/ControlStatusCapabilityControlType.h"
#include "uci/type/ControlTypeEnum.h"
#include "uci/type/DMPI_ID_Type.h"
#include "uci/type/DMPI_StoreStatusType.h"
#include "uci/type/ElevationScanStabilizationEnum.h"
#include "uci/type/EntityID_Type.h"
#include "uci/type/EntitySignalSummaryType.h"
#include "uci/type/ESM_Activity.h"
#include "uci/type/ESM_ActivityCommandType.h"
#include "uci/type/ESM_ActivityType.h"
#include "uci/type/ESM_CapabilityCommandType.h"
#include "uci/type/ESM_CapabilityStatus.h"
#include "uci/type/ESM_CommandType.h"
#include "uci/type/ESM_SubCapabilityEnum.h"
#include "uci/type/FrequencyRangeType.h"
#include "uci/type/FixedPointingEnum.h"
#include "uci/type/FrequencyRangeType.h"
#include "uci/type/KinematicsType.h"
#include "uci/type/LOS_VariableA_Type.h"
#include "uci/type/LOS_VariableB_Type.h"
#include "uci/type/LOS_ReferenceEnum.h"
#include "uci/type/HeaderType.h"
#include "uci/type/MessageType.h"
#include "uci/type/PointingType.h"
#include "uci/type/PointingTypeEnum.h"
#include "uci/type/POST_ActivityCommandType.h"
#include "uci/type/POST_AirVolumeSensorReferencedType.h"
#include "uci/type/POST_AirTargetVolumeCommandType.h"
#include "uci/type/POST_AirTargetVolumeType.h"
#include "uci/type/POST_CapabilityCommandType.h"
#include "uci/type/POST_CapabilityType.h"
#include "uci/type/POST_CommandType.h"
#include "uci/type/POST_SubCapabilityType.h"
#include "uci/type/PrimaryControllerType.h"
#include "uci/type/SAR_SubCapabilityType.h"
#include "uci/type/SignalID_Type.h"
#include "uci/type/SMTI_Activity.h"
#include "uci/type/SMTI_ActivityCommandType.h"
#include "uci/type/SMTI_ActivityType.h"
#include "uci/type/SMTI_CapabilityCommandType.h"
#include "uci/type/SMTI_CapabilityType.h"
#include "uci/type/SMTI_CommandType.h"
#include "uci/type/SMTI_SubCapabilityEnum.h"
#include "uci/type/StandardIdentityConfidenceType.h"
#include "uci/type/StoreCategoryEnum.h"
#include "uci/type/StrikeActivityCommandType.h"
#include "uci/type/StrikeActivityType.h"
#include "uci/type/StrikeCapabilityCommandType.h"
#include "uci/type/StrikeCapabilityType.h"
#include "uci/type/StrikeCapabilityStatusType.h"
#include "uci/type/StrikeCommandType.h"
#include "uci/type/StrikeWeaponCommandType.h"
#include "uci/type/SystemID_Type.h"
#include "uci/type/SubsystemStatus.h"
#include "uci/type/SubCapabilityDetailsType.h"
#include "uci/type/SubCapabilitySelectionType.h"

//factories
#include "ismownpro/factory/CVEnumISMOwnerProducerValuesFactory.h"
#include "uci/factory/ActivityID_TypeFactory.h"
#include "uci/factory/AirTargetVolumeCommandTypeFactory.h"
#include "uci/factory/AMTI_ActivityFactory.h"
#include "uci/factory/AMTI_ActivityCommandTypeFactory.h"
#include "uci/factory/AMTI_ActivityTypeFactory.h"
#include "uci/factory/AMTI_CapabilityCommandTypeFactory.h"
#include "uci/factory/AMTI_CapabilityTypeFactory.h"
#include "uci/factory/AMTI_CommandTypeFactory.h"
#include "uci/factory/AMTI_SubCapabilityEnumFactory.h"
#include "uci/factory/ApprovalEnumFactory.h"
#include "uci/factory/CapabilityAvailabilityEnumFactory.h"
#include "uci/factory/CapabilityControlInterfacesEnumFactory.h"
#include "uci/factory/CapabilityCommandID_TypeFactory.h"
#include "uci/factory/CapabilityID_TypeFactory.h"
#include "uci/factory/CapabilityManagerTypeFactory.h"
#include "uci/factory/CapabilityStatusTypeFactory.h"
#include "uci/factory/CapabilityStateTypeFactory.h"
#include "uci/factory/ControleeTypeFactory.h"
#include "uci/factory/ControlRequestEnumFactory.h"
#include "uci/factory/ControlTypeEnumFactory.h"
#include "uci/factory/ControlStatusCapabilityControlTypeFactory.h"
#include "uci/factory/ControlStatusMissionControlTypeFactory.h"
#include "uci/factory/DMPI_ID_TypeFactory.h"
#include "uci/factory/DMPI_StoreStatusTypeFactory.h"
#include "uci/factory/ElevationScanStabilizationEnumFactory.h"
#include "uci/factory/EntityID_TypeFactory.h"
#include "uci/factory/EntitySignalSummaryTypeFactory.h"
#include "uci/factory/ESM_ActivityFactory.h"
#include "uci/factory/ESM_ActivityCommandTypeFactory.h"
#include "uci/factory/ESM_ActivityTypeFactory.h"
#include "uci/factory/ESM_CapabilityCommandTypeFactory.h"
#include "uci/factory/ESM_CapabilityTypeFactory.h"
#include "uci/factory/ESM_CapabilityStatusFactory.h"
#include "uci/factory/ESM_CommandTypeFactory.h"
#include "uci/factory/ESM_SubCapabilityEnumFactory.h"
#include "uci/factory/FrequencyRangeTypeFactory.h"
#include "uci/factory/FixedPointingEnumFactory.h"
#include "uci/factory/FrequencyRangeTypeFactory.h"
#include "uci/factory/HeaderTypeFactory.h"
#include "uci/factory/KinematicsTypeFactory.h"
#include "uci/factory/LOS_VariableA_TypeFactory.h"
#include "uci/factory/LOS_VariableB_TypeFactory.h"
#include "uci/factory/LOS_ReferenceEnumFactory.h"
#include "uci/factory/PointingTypeFactory.h"
#include "uci/factory/PointingTypeEnumFactory.h"
#include "uci/factory/POST_ActivityCommandTypeFactory.h"
#include "uci/factory/POST_ActivityTypeFactory.h"
#include "uci/factory/POST_AirVolumeSensorReferencedTypeFactory.h"
#include "uci/factory/POST_AirTargetVolumeCommandTypeFactory.h"
#include "uci/factory/POST_AirTargetVolumeTypeFactory.h"
#include "uci/factory/POST_CapabilityCommandTypeFactory.h"
#include "uci/factory/POST_CapabilityTypeFactory.h"
#include "uci/factory/POST_CapabilityStatusTypeFactory.h"
#include "uci/factory/POST_CapabilityTypeFactory.h"
#include "uci/factory/POST_CommandTypeFactory.h"
#include "uci/factory/POST_SubCapabilityTypeFactory.h"
#include "uci/factory/PointingTypeEnumFactory.h"
#include "uci/factory/PrimaryControllerTypeFactory.h"
#include "uci/factory/SAR_SubCapabilityTypeFactory.h"
#include "uci/factory/SignalID_TypeFactory.h"
#include "uci/factory/ActivityID_TypeFactory.h"
#include "uci/factory/SecondaryControllerTypeFactory.h"
#include "uci/factory/SMTI_ActivityFactory.h"
#include "uci/factory/SMTI_ActivityCommandTypeFactory.h"
#include "uci/factory/SMTI_ActivityTypeFactory.h"
#include "uci/factory/SMTI_CapabilityCommandTypeFactory.h"
#include "uci/factory/SMTI_CapabilityTypeFactory.h"
#include "uci/factory/SMTI_CommandTypeFactory.h"
#include "uci/factory/SMTI_SubCapabilityEnumFactory.h"
#include "uci/factory/StandardIdentityConfidenceTypeFactory.h"
#include "uci/factory/SecondaryControllerTypeFactory.h"
#include "uci/factory/StoreCategoryEnumFactory.h"
#include "uci/factory/StrikeActivityCommandTypeFactory.h"
#include "uci/factory/StrikeActivityTypeFactory.h"
#include "uci/factory/StrikeCapabilityCommandTypeFactory.h"
#include "uci/factory/StrikeCapabilityTypeFactory.h"
#include "uci/factory/StrikeCapabilityStatusTypeFactory.h"
#include "uci/factory/StrikeCommandTypeFactory.h"
#include "uci/factory/StrikeWeaponCommandTypeFactory.h"
#include "uci/factory/SystemID_TypeFactory.h"
#include "uci/factory/SubCapabilityDetailsTypeFactory.h"
#include "uci/factory/SubCapabilitySelectionTypeFactory.h"

namespace wsf
{

//! Class parameterizing basic UCI types.
template< typename Type,
          typename Factory,
          uci::base::accessorType::AccessorType TypeName >
class WSF_OMS_UCI_EXPORT UCI_Type
{
   public:
      
      explicit UCI_Type(WsfSimulation* aSimulationPtr);
      explicit UCI_Type(WsfPlatform* aPlatformPtr);
      explicit UCI_Type(UCI_Interface& aInterface);
      UCI_Type(UCI_Interface& aInterface,
               const Type&    aType);
      UCI_Type(const UCI_Type& aSrc);
      ~UCI_Type();

      static uci::base::accessorType::AccessorType GetUCI_TypeId() { return TypeName; }

      //! Get the underlying UCI type.
      Type& Get() const { return *mTypePtr; }

      //! Get the associated UCI interface.
      UCI_Interface& GetInterface() const { return *mInterfacePtr; }

   protected:

      UCI_Type() = default;

      UCI_Interface*  mInterfacePtr  { nullptr };
      Type*           mTypePtr       { nullptr };

   private:

      const UCI_FactoryService<Type, Factory>* mTypeFactoryPtr  { nullptr };
};

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::UCI_Type(WsfSimulation* aSimulationPtr)
   : mInterfacePtr(UCI_Util::GetInterface(aSimulationPtr))
{
   // get factory from interface
   // Construct uci message from factory
   mInterfacePtr->GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create());
}

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::UCI_Type(WsfPlatform* aPlatformPtr)
   : mInterfacePtr(UCI_Util::GetInterface(aPlatformPtr))
{
   // get factory from interface
   // Construct uci message from factory
   mInterfacePtr->GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create());
}

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::UCI_Type(UCI_Interface& aInterface)
   : mInterfacePtr(&aInterface)
{
   // get factory from interface
   // Construct uci message from factory
   mInterfacePtr->GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create());
}

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::UCI_Type(wsf::UCI_Interface& aInterface, const Type& aType)
   : mInterfacePtr(&aInterface)
{
   // get factory from interface
   // Construct uci message from factory
   mInterfacePtr->GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create(aType));
}

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::UCI_Type(const UCI_Type& aSrc)
   : mInterfacePtr(aSrc.mInterfacePtr),
     mTypeFactoryPtr(aSrc.mTypeFactoryPtr)
{
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create(*aSrc.mTypePtr));
}

template<typename Type, typename Factory, uci::base::accessorType::AccessorType TypeName>
inline UCI_Type<Type, Factory, TypeName>::~UCI_Type()
{
   mTypeFactoryPtr->GetFactory().destroy(*mTypePtr);
}
//=======================================================================================================
//! Define a WSF type corresponding with a given UCI type.  The WSF type can be constructed and destructed
//! without direct reference to the abstract bus.
//! Arguments:
//! WSF_TYPE : The name of the WSF type to be defined.
//! UCI_TYPE : The name of the existing UCI type.
//! ACCESSOR_NAME : The accessor id corresponding with the existing UCI type (from uci::type::accessorType).
//!
//! For example, the following definition:
//! @code
//! DEFINE_UCI_TYPE(UCI_CapabilityID_Type, CapabilityID_Type, capabilityID_Type)
//! @endcode
//! expands to the following code:
//! @code
//! using UCI_CapabilityID_Type = UCI_Type<uci::type::CapabilityID_Type,
//!                                        uci::factory::CapabilityID_TypeFactory,
//!                                        uci::type::accessorType::capabilityID_Type>;
//! @endcode
#define DEFINE_UCI_TYPE(NAMESPACE, WSF_TYPE, UCI_TYPE, ACCESSOR_NAME)           \
using WSF_TYPE = UCI_Type<NAMESPACE::type::UCI_TYPE,                       \
                          NAMESPACE::factory::UCI_TYPE##Factory,           \
                          NAMESPACE::type::accessorType::ACCESSOR_NAME>;   \
using WSF_TYPE##_StatusFactory = UCI_FactoryService<NAMESPACE::type::UCI_TYPE, \
                                                    NAMESPACE::factory::UCI_TYPE##Factory>; \

//=======================================================================================================

DEFINE_UCI_TYPE(ismownpro, ISM_OwnerProducerType, CVEnumISMOwnerProducerValues, cVEnumISMOwnerProducerValues)
DEFINE_UCI_TYPE(uci, UCI_ActivityID_Type, ActivityID_Type, activityID_Type)
DEFINE_UCI_TYPE(uci, UCI_AirTargetVolumeCommandType, AirTargetVolumeCommandType, airTargetVolumeCommandType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_ActivityCommandType, AMTI_ActivityCommandType, aMTI_ActivityCommandType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_ActivityType, AMTI_ActivityType, aMTI_ActivityType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_CapabilityCommandType, AMTI_CapabilityCommandType, aMTI_CapabilityCommandType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_CapabilityType, AMTI_CapabilityType, aMTI_CapabilityType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_CommandType, AMTI_CommandType, aMTI_CommandType)
DEFINE_UCI_TYPE(uci, UCI_AMTI_SubCapabilityType, AMTI_SubCapabilityEnum, aMTI_SubCapabilityEnum)
DEFINE_UCI_TYPE(uci, UCI_ApprovalType, ApprovalEnum, approvalEnum)
DEFINE_UCI_TYPE(uci, UCI_AvailabilityType, CapabilityAvailabilityEnum, capabilityAvailabilityEnum)
DEFINE_UCI_TYPE(uci, UCI_CapabilityCommandID_Type, CapabilityCommandID_Type, capabilityCommandID_Type)
DEFINE_UCI_TYPE(uci, UCI_CapabilityID_Type, CapabilityID_Type, capabilityID_Type)
DEFINE_UCI_TYPE(uci, UCI_CapabilityStateType, CapabilityStateType, capabilityStateType)
DEFINE_UCI_TYPE(uci, UCI_CapabilityStatusType, CapabilityStatusType, capabilityStatusType)
DEFINE_UCI_TYPE(uci, UCI_ControlRequestType, ControlRequestEnum, controlRequestEnum)
DEFINE_UCI_TYPE(uci, UCI_ControlStatusMissionControlType, ControlStatusMissionControlType, controlStatusMissionControlType)
DEFINE_UCI_TYPE(uci, UCI_CapabilityControlInterfaceEnumType, CapabilityControlInterfacesEnum, capabilityControlInterfacesEnum)
DEFINE_UCI_TYPE(uci, UCI_CapabilityManagerType, CapabilityManagerType, capabilityManagerType)
DEFINE_UCI_TYPE(uci, UCI_ControleeType, ControleeType, controleeType)
DEFINE_UCI_TYPE(uci, UCI_ControlType, ControlTypeEnum, controlTypeEnum)
DEFINE_UCI_TYPE(uci, UCI_ControlStatusCapabilityControlType, ControlStatusCapabilityControlType, controlStatusCapabilityControlType)
DEFINE_UCI_TYPE(uci, UCI_DMPI_ID_Type, DMPI_ID_Type, dMPI_ID_Type)
DEFINE_UCI_TYPE(uci, UCI_DMPI_StoreStatusType, DMPI_StoreStatusType, dMPI_StoreStatusType)
DEFINE_UCI_TYPE(uci, UCI_ElevationScanStabilizationEnumType, ElevationScanStabilizationEnum, elevationScanStabilizationEnum)
DEFINE_UCI_TYPE(uci, UCI_EntityID_Type, EntityID_Type, entityID_Type)
DEFINE_UCI_TYPE(uci, UCI_EntitySignalSummaryType, EntitySignalSummaryType, entitySignalSummaryType)
DEFINE_UCI_TYPE(uci, UCI_ESM_ActivityCommandType, ESM_ActivityCommandType, eSM_ActivityCommandType)
DEFINE_UCI_TYPE(uci, UCI_ESM_ActivityType, ESM_ActivityType, eSM_ActivityType)
DEFINE_UCI_TYPE(uci, UCI_ESM_CapabilityCommandType, ESM_CapabilityCommandType, eSM_CapabilityCommandType)
DEFINE_UCI_TYPE(uci, UCI_ESM_CapabilityType, ESM_CapabilityType, eSM_CapabilityType)
DEFINE_UCI_TYPE(uci, UCI_ESM_CommandType, ESM_CommandType, eSM_CommandType)
DEFINE_UCI_TYPE(uci, UCI_FrequencyRangeType, FrequencyRangeType, frequencyRangeType)
DEFINE_UCI_TYPE(uci, UCI_FixedPointingType, FixedPointingEnum, fixedPointingEnum)
DEFINE_UCI_TYPE(uci, UCI_HeaderType, HeaderType, headerType)
DEFINE_UCI_TYPE(uci, UCI_KinematicsType, KinematicsType, kinematicsType)
DEFINE_UCI_TYPE(uci, UCI_LOS_Type, LOS_VariableB_Type, lOS_VariableB_Type)
DEFINE_UCI_TYPE(uci, UCI_LOS_ReferenceEnumType, LOS_ReferenceEnum, lOS_ReferenceEnum)
DEFINE_UCI_TYPE(uci, UCI_PointingCapabilityType, PointingTypeEnum, pointingTypeEnum)
DEFINE_UCI_TYPE(uci, UCI_PointingType, PointingType, pointingType)
DEFINE_UCI_TYPE(uci, UCI_POST_ActivityCommandType, POST_ActivityCommandType, pOST_ActivityCommandType)
DEFINE_UCI_TYPE(uci, UCI_POST_ActivityType, POST_ActivityType, pOST_ActivityType)
DEFINE_UCI_TYPE(uci, UCI_POST_AirTargetVolumeCommandType, POST_AirTargetVolumeCommandType, pOST_AirTargetVolumeCommandType)
DEFINE_UCI_TYPE(uci, UCI_POST_AirTargetVolumeType, POST_AirTargetVolumeType, pOST_AirTargetVolumeType)
DEFINE_UCI_TYPE(uci, UCI_POST_AirVolumeSensorReferencedType, POST_AirVolumeSensorReferencedType, pOST_AirVolumeSensorReferencedType)
DEFINE_UCI_TYPE(uci, UCI_POST_CapabilityCommandType, POST_CapabilityCommandType, pOST_CapabilityCommandType)
DEFINE_UCI_TYPE(uci, UCI_POST_CapabilityType, POST_CapabilityType, pOST_CapabilityType)
DEFINE_UCI_TYPE(uci, UCI_POST_CapabilityStatusType, POST_CapabilityStatusType, pOST_CapabilityStatusType)
DEFINE_UCI_TYPE(uci, UCI_POST_CommandType, POST_CommandType, pOST_CommandType)
DEFINE_UCI_TYPE(uci, UCI_POST_SubCapabilityType, POST_SubCapabilityType, pOST_SubCapabilityType)
DEFINE_UCI_TYPE(uci, UCI_PrimaryControllerType, PrimaryControllerType, primaryControllerType)
DEFINE_UCI_TYPE(uci, UCI_SecondaryControllerType, SecondaryControllerType, secondaryControllerType)
DEFINE_UCI_TYPE(uci, UCI_SMTI_ActivityCommandType, SMTI_ActivityCommandType, sMTI_ActivityCommandType)
DEFINE_UCI_TYPE(uci, UCI_SMTI_ActivityType, SMTI_ActivityType, sMTI_ActivityType)
DEFINE_UCI_TYPE(uci, UCI_SMTI_CapabilityCommandType, SMTI_CapabilityCommandType, sMTI_CapabilityCommandType)
DEFINE_UCI_TYPE(uci, UCI_SMTI_CapabilityType, SMTI_CapabilityType, sMTI_CapabilityType)
DEFINE_UCI_TYPE(uci, UCI_SMTI_CommandType, SMTI_CommandType, sMTI_CommandType)
DEFINE_UCI_TYPE(uci, UCI_SignalID_Type, SignalID_Type, signalID_Type)
DEFINE_UCI_TYPE(uci, UCI_StandardIdentityConfidenceType, StandardIdentityConfidenceType, standardIdentityConfidenceType)
DEFINE_UCI_TYPE(uci, UCI_StoreCategoryEnumType, StoreCategoryEnum, storeCategoryEnum)
DEFINE_UCI_TYPE(uci, UCI_StrikeActivityType, StrikeActivityType, strikeActivityType)
DEFINE_UCI_TYPE(uci, UCI_StrikeActivityCommandType, StrikeActivityCommandType, strikeActivityCommandType)
DEFINE_UCI_TYPE(uci, UCI_StrikeCapabilityCommandType, StrikeCapabilityCommandType, strikeCapabilityCommandType)
DEFINE_UCI_TYPE(uci, UCI_StrikeCapabilityType, StrikeCapabilityType, strikeCapabilityType)
DEFINE_UCI_TYPE(uci, UCI_StrikeCapabilityStatusType, StrikeCapabilityStatusType, strikeCapabilityStatusType)
DEFINE_UCI_TYPE(uci, UCI_StrikeCommandType, StrikeCommandType, strikeCommandType)
DEFINE_UCI_TYPE(uci, UCI_StrikeWeaponCommandType, StrikeWeaponCommandType, strikeWeaponCommandType)
DEFINE_UCI_TYPE(uci, UCI_SubCapabilityDetailsType, SubCapabilityDetailsType, subCapabilityDetailsType)
DEFINE_UCI_TYPE(uci, UCI_SubCapabilityEnumType, ESM_SubCapabilityEnum, eSM_SubCapabilityEnum)
DEFINE_UCI_TYPE(uci, UCI_SubCapabilitySelectionType, SubCapabilitySelectionType, subCapabilitySelectionType)
DEFINE_UCI_TYPE(uci, UCI_SystemID_Type, SystemID_Type, systemID_Type)

} // namespace wsf
#endif
