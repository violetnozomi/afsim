// This file was generated  on 12/5/2018 at 1:1:49 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__MessageTypeEnum_h
#define Uci__Type__MessageTypeEnum_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(StdLibMap)
#  include <map>
//  @cond
#  define StdLibMap 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** List of all message types in UCI */
      class MessageTypeEnum : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  UCI_AIR_SAMPLE_ACTIVITY's ordinal value */
            UCI_AIR_SAMPLE_ACTIVITY,

            //  UCI_AIR_SAMPLE_CAPABILITY's ordinal value */
            UCI_AIR_SAMPLE_CAPABILITY,

            //  UCI_AIR_SAMPLE_CAPABILITY_STATUS's ordinal value */
            UCI_AIR_SAMPLE_CAPABILITY_STATUS,

            //  UCI_AIR_SAMPLE_COMMAND's ordinal value */
            UCI_AIR_SAMPLE_COMMAND,

            //  UCI_AIR_SAMPLE_COMMAND_STATUS's ordinal value */
            UCI_AIR_SAMPLE_COMMAND_STATUS,

            //  UCI_AIR_SAMPLE_SETTINGS_COMMAND's ordinal value */
            UCI_AIR_SAMPLE_SETTINGS_COMMAND,

            //  UCI_AIR_SAMPLE_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_AIR_SAMPLE_SETTINGS_COMMAND_STATUS,

            //  UCI_AMTI_ACTIVITY's ordinal value */
            UCI_AMTI_ACTIVITY,

            //  UCI_AMTI_CAPABILITY's ordinal value */
            UCI_AMTI_CAPABILITY,

            //  UCI_AMTI_CAPABILITY_STATUS's ordinal value */
            UCI_AMTI_CAPABILITY_STATUS,

            //  UCI_AMTI_COMMAND's ordinal value */
            UCI_AMTI_COMMAND,

            //  UCI_AMTI_COMMAND_STATUS's ordinal value */
            UCI_AMTI_COMMAND_STATUS,

            //  UCI_AMTI_SETTINGS_COMMAND's ordinal value */
            UCI_AMTI_SETTINGS_COMMAND,

            //  UCI_AMTI_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_AMTI_SETTINGS_COMMAND_STATUS,

            //  UCI_ANTENNA_STATUS's ordinal value */
            UCI_ANTENNA_STATUS,

            //  UCI_AO_ACTIVITY's ordinal value */
            UCI_AO_ACTIVITY,

            //  UCI_AO_CAPABILITY's ordinal value */
            UCI_AO_CAPABILITY,

            //  UCI_AO_CAPABILITY_STATUS's ordinal value */
            UCI_AO_CAPABILITY_STATUS,

            //  UCI_AO_COMMAND's ordinal value */
            UCI_AO_COMMAND,

            //  UCI_AO_COMMAND_STATUS's ordinal value */
            UCI_AO_COMMAND_STATUS,

            //  UCI_AO_CONSENT_REQUEST's ordinal value */
            UCI_AO_CONSENT_REQUEST,

            //  UCI_AO_CONSENT_REQUEST_STATUS's ordinal value */
            UCI_AO_CONSENT_REQUEST_STATUS,

            //  UCI_AO_SETTINGS_COMMAND's ordinal value */
            UCI_AO_SETTINGS_COMMAND,

            //  UCI_AO_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_AO_SETTINGS_COMMAND_STATUS,

            //  UCI_APPROVAL_AUTHORITY's ordinal value */
            UCI_APPROVAL_AUTHORITY,

            //  UCI_APPROVAL_AUTHORITY_REQUEST's ordinal value */
            UCI_APPROVAL_AUTHORITY_REQUEST,

            //  UCI_APPROVAL_AUTHORITY_REQUEST_STATUS's ordinal value */
            UCI_APPROVAL_AUTHORITY_REQUEST_STATUS,

            //  UCI_APPROVAL_POLICY's ordinal value */
            UCI_APPROVAL_POLICY,

            //  UCI_APPROVAL_REQUEST's ordinal value */
            UCI_APPROVAL_REQUEST,

            //  UCI_APPROVAL_REQUEST_STATUS's ordinal value */
            UCI_APPROVAL_REQUEST_STATUS,

            //  UCI_ASSESSMENT's ordinal value */
            UCI_ASSESSMENT,

            //  UCI_ASSESSMENT_REQUEST's ordinal value */
            UCI_ASSESSMENT_REQUEST,

            //  UCI_ASSESSMENT_REQUEST_STATUS's ordinal value */
            UCI_ASSESSMENT_REQUEST_STATUS,

            //  UCI_AUTHORIZATION's ordinal value */
            UCI_AUTHORIZATION,

            //  UCI_AUTONOMY_SETTING's ordinal value */
            UCI_AUTONOMY_SETTING,

            //  UCI_CAPABILITY_COVERAGE_AREA's ordinal value */
            UCI_CAPABILITY_COVERAGE_AREA,

            //  UCI_CAPABILITY_COVERAGE_AREA_REQUEST's ordinal value */
            UCI_CAPABILITY_COVERAGE_AREA_REQUEST,

            //  UCI_CAPABILITY_COVERAGE_AREA_REQUEST_STATUS's ordinal value */
            UCI_CAPABILITY_COVERAGE_AREA_REQUEST_STATUS,

            //  UCI_CAPABILITY_PLAN's ordinal value */
            UCI_CAPABILITY_PLAN,

            //  UCI_CARGO_DELIVERY_CAPABILITY's ordinal value */
            UCI_CARGO_DELIVERY_CAPABILITY,

            //  UCI_CARGO_DELIVERY_CAPABILITY_STATUS's ordinal value */
            UCI_CARGO_DELIVERY_CAPABILITY_STATUS,

            //  UCI_CARGO_STATUS's ordinal value */
            UCI_CARGO_STATUS,

            //  UCI_COMINT_ACTIVITY's ordinal value */
            UCI_COMINT_ACTIVITY,

            //  UCI_COMINT_CAPABILITY's ordinal value */
            UCI_COMINT_CAPABILITY,

            //  UCI_COMINT_CAPABILITY_STATUS's ordinal value */
            UCI_COMINT_CAPABILITY_STATUS,

            //  UCI_COMINT_COMMAND's ordinal value */
            UCI_COMINT_COMMAND,

            //  UCI_COMINT_COMMAND_STATUS's ordinal value */
            UCI_COMINT_COMMAND_STATUS,

            //  UCI_COMINT_CONSENT_REQUEST's ordinal value */
            UCI_COMINT_CONSENT_REQUEST,

            //  UCI_COMINT_CONSENT_REQUEST_STATUS's ordinal value */
            UCI_COMINT_CONSENT_REQUEST_STATUS,

            //  UCI_COMINT_SETTINGS_COMMAND's ordinal value */
            UCI_COMINT_SETTINGS_COMMAND,

            //  UCI_COMINT_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_COMINT_SETTINGS_COMMAND_STATUS,

            //  UCI_COMM_ASSET's ordinal value */
            UCI_COMM_ASSET,

            //  UCI_COMM_ASSET_ESTIMATION_REQUEST's ordinal value */
            UCI_COMM_ASSET_ESTIMATION_REQUEST,

            //  UCI_COMM_ASSET_ESTIMATION_REQUEST_STATUS's ordinal value */
            UCI_COMM_ASSET_ESTIMATION_REQUEST_STATUS,

            //  UCI_COMM_ASSET_ORBIT's ordinal value */
            UCI_COMM_ASSET_ORBIT,

            //  UCI_COMM_ASSET_POSITION_REPORT's ordinal value */
            UCI_COMM_ASSET_POSITION_REPORT,

            //  UCI_COMM_POINTING's ordinal value */
            UCI_COMM_POINTING,

            //  UCI_COMM_POINTING_COMMAND's ordinal value */
            UCI_COMM_POINTING_COMMAND,

            //  UCI_COMM_POINTING_COMMAND_STATUS's ordinal value */
            UCI_COMM_POINTING_COMMAND_STATUS,

            //  UCI_COMM_POINTING_REQUEST's ordinal value */
            UCI_COMM_POINTING_REQUEST,

            //  UCI_COMM_POINTING_REQUEST_STATUS's ordinal value */
            UCI_COMM_POINTING_REQUEST_STATUS,

            //  UCI_COMM_RELAY_ACTIVITY's ordinal value */
            UCI_COMM_RELAY_ACTIVITY,

            //  UCI_COMM_RELAY_CAPABILITY's ordinal value */
            UCI_COMM_RELAY_CAPABILITY,

            //  UCI_COMM_RELAY_CAPABILITY_STATUS's ordinal value */
            UCI_COMM_RELAY_CAPABILITY_STATUS,

            //  UCI_COMM_RELAY_COMMAND's ordinal value */
            UCI_COMM_RELAY_COMMAND,

            //  UCI_COMM_RELAY_COMMAND_STATUS's ordinal value */
            UCI_COMM_RELAY_COMMAND_STATUS,

            //  UCI_COMM_RELAY_SETTINGS_COMMAND's ordinal value */
            UCI_COMM_RELAY_SETTINGS_COMMAND,

            //  UCI_COMM_RELAY_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_COMM_RELAY_SETTINGS_COMMAND_STATUS,

            //  UCI_COMM_SCHEDULE_ALLOCATION's ordinal value */
            UCI_COMM_SCHEDULE_ALLOCATION,

            //  UCI_COMM_SCHEDULE_ALLOCATION_UPDATE_REQUEST's ordinal value */
            UCI_COMM_SCHEDULE_ALLOCATION_UPDATE_REQUEST,

            //  UCI_COMM_SCHEDULE_ALLOCATION_UPDATE_REQUEST_STATUS's ordinal value */
            UCI_COMM_SCHEDULE_ALLOCATION_UPDATE_REQUEST_STATUS,

            //  UCI_COMM_SCHEDULE_AVAILABILITY's ordinal value */
            UCI_COMM_SCHEDULE_AVAILABILITY,

            //  UCI_COMM_SCHEDULING_REQUEST's ordinal value */
            UCI_COMM_SCHEDULING_REQUEST,

            //  UCI_COMM_SCHEDULING_REQUEST_STATUS's ordinal value */
            UCI_COMM_SCHEDULING_REQUEST_STATUS,

            //  UCI_COMM_SCHEDULING_REQUIREMENT's ordinal value */
            UCI_COMM_SCHEDULING_REQUIREMENT,

            //  UCI_COMM_SYSTEM's ordinal value */
            UCI_COMM_SYSTEM,

            //  UCI_COMM_TERMINAL_ACTIVITY's ordinal value */
            UCI_COMM_TERMINAL_ACTIVITY,

            //  UCI_COMM_TERMINAL_CAPABILITY's ordinal value */
            UCI_COMM_TERMINAL_CAPABILITY,

            //  UCI_COMM_TERMINAL_CAPABILITY_STATUS's ordinal value */
            UCI_COMM_TERMINAL_CAPABILITY_STATUS,

            //  UCI_COMM_TERMINAL_COMMAND's ordinal value */
            UCI_COMM_TERMINAL_COMMAND,

            //  UCI_COMM_TERMINAL_COMMAND_STATUS's ordinal value */
            UCI_COMM_TERMINAL_COMMAND_STATUS,

            //  UCI_COMM_TERMINAL_PLAN's ordinal value */
            UCI_COMM_TERMINAL_PLAN,

            //  UCI_COMM_TERMINAL_PLAN_ACTIVATION_COMMAND's ordinal value */
            UCI_COMM_TERMINAL_PLAN_ACTIVATION_COMMAND,

            //  UCI_COMM_TERMINAL_PLAN_ACTIVATION_COMMAND_STATUS's ordinal value */
            UCI_COMM_TERMINAL_PLAN_ACTIVATION_COMMAND_STATUS,

            //  UCI_COMM_TERMINAL_PLAN_ACTIVATION_STATE's ordinal value */
            UCI_COMM_TERMINAL_PLAN_ACTIVATION_STATE,

            //  UCI_COMM_TERMINAL_SETTINGS_COMMAND's ordinal value */
            UCI_COMM_TERMINAL_SETTINGS_COMMAND,

            //  UCI_COMM_TERMINAL_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_COMM_TERMINAL_SETTINGS_COMMAND_STATUS,

            //  UCI_COMM_UTILIZATION_REQUEST's ordinal value */
            UCI_COMM_UTILIZATION_REQUEST,

            //  UCI_COMM_UTILIZATION_REQUEST_STATUS's ordinal value */
            UCI_COMM_UTILIZATION_REQUEST_STATUS,

            //  UCI_CONTROL_INTERFACES_COMMAND's ordinal value */
            UCI_CONTROL_INTERFACES_COMMAND,

            //  UCI_CONTROL_INTERFACES_COMMAND_STATUS's ordinal value */
            UCI_CONTROL_INTERFACES_COMMAND_STATUS,

            //  UCI_CONTROL_REQUEST's ordinal value */
            UCI_CONTROL_REQUEST,

            //  UCI_CONTROL_REQUEST_STATUS's ordinal value */
            UCI_CONTROL_REQUEST_STATUS,

            //  UCI_CONTROL_SOURCES_COMMAND's ordinal value */
            UCI_CONTROL_SOURCES_COMMAND,

            //  UCI_CONTROL_SOURCES_COMMAND_STATUS's ordinal value */
            UCI_CONTROL_SOURCES_COMMAND_STATUS,

            //  UCI_CONTROL_STATUS's ordinal value */
            UCI_CONTROL_STATUS,

            //  UCI_COORDINATED_TASK_SET's ordinal value */
            UCI_COORDINATED_TASK_SET,

            //  UCI_COORDINATED_TASK_SET_STATUS's ordinal value */
            UCI_COORDINATED_TASK_SET_STATUS,

            //  UCI_CRYPTO_COMMAND's ordinal value */
            UCI_CRYPTO_COMMAND,

            //  UCI_CRYPTO_COMMAND_STATUS's ordinal value */
            UCI_CRYPTO_COMMAND_STATUS,

            //  UCI_CRYPTO_STATUS's ordinal value */
            UCI_CRYPTO_STATUS,

            //  UCI_DAMAGE_ASSESSMENT's ordinal value */
            UCI_DAMAGE_ASSESSMENT,

            //  UCI_DAMAGE_ASSESSMENT_REQUEST's ordinal value */
            UCI_DAMAGE_ASSESSMENT_REQUEST,

            //  UCI_DAMAGE_ASSESSMENT_REQUEST_STATUS's ordinal value */
            UCI_DAMAGE_ASSESSMENT_REQUEST_STATUS,

            //  UCI_DAMAGE_ESTIMATE's ordinal value */
            UCI_DAMAGE_ESTIMATE,

            //  UCI_DAMAGE_ESTIMATE_REQUEST's ordinal value */
            UCI_DAMAGE_ESTIMATE_REQUEST,

            //  UCI_DAMAGE_ESTIMATE_REQUEST_STATUS's ordinal value */
            UCI_DAMAGE_ESTIMATE_REQUEST_STATUS,

            //  UCI_DAMAGE_REPORT's ordinal value */
            UCI_DAMAGE_REPORT,

            //  UCI_DAMAGE_SUMMARY's ordinal value */
            UCI_DAMAGE_SUMMARY,

            //  UCI_DATA_DELETE_REQUEST's ordinal value */
            UCI_DATA_DELETE_REQUEST,

            //  UCI_DATA_DELETE_REQUEST_STATUS's ordinal value */
            UCI_DATA_DELETE_REQUEST_STATUS,

            //  UCI_DATA_MANAGEMENT_REQUEST's ordinal value */
            UCI_DATA_MANAGEMENT_REQUEST,

            //  UCI_DATA_MANAGEMENT_REQUEST_STATUS's ordinal value */
            UCI_DATA_MANAGEMENT_REQUEST_STATUS,

            //  UCI_DATA_PLAN's ordinal value */
            UCI_DATA_PLAN,

            //  UCI_DATA_PLAN_ACTIVATION_COMMAND's ordinal value */
            UCI_DATA_PLAN_ACTIVATION_COMMAND,

            //  UCI_DATA_PLAN_ACTIVATION_COMMAND_STATUS's ordinal value */
            UCI_DATA_PLAN_ACTIVATION_COMMAND_STATUS,

            //  UCI_DATA_PLAN_ACTIVATION_STATE's ordinal value */
            UCI_DATA_PLAN_ACTIVATION_STATE,

            //  UCI_DATA_STORAGE_UTILIZATION_REQUEST's ordinal value */
            UCI_DATA_STORAGE_UTILIZATION_REQUEST,

            //  UCI_DATA_STORAGE_UTILIZATION_REQUEST_STATUS's ordinal value */
            UCI_DATA_STORAGE_UTILIZATION_REQUEST_STATUS,

            //  UCI_DMPI's ordinal value */
            UCI_DMPI,

            //  UCI_DMPI_CANCEL's ordinal value */
            UCI_DMPI_CANCEL,

            //  UCI_DMPI_DESIGNATION's ordinal value */
            UCI_DMPI_DESIGNATION,

            //  UCI_DMPI_DESIGNATION_REQUEST's ordinal value */
            UCI_DMPI_DESIGNATION_REQUEST,

            //  UCI_DMPI_DESIGNATION_REQUEST_STATUS's ordinal value */
            UCI_DMPI_DESIGNATION_REQUEST_STATUS,

            //  UCI_DMPI_PATTERN's ordinal value */
            UCI_DMPI_PATTERN,

            //  UCI_DMPI_STATUS's ordinal value */
            UCI_DMPI_STATUS,

            //  UCI_DOOR_COMMAND's ordinal value */
            UCI_DOOR_COMMAND,

            //  UCI_DOOR_COMMAND_STATUS's ordinal value */
            UCI_DOOR_COMMAND_STATUS,

            //  UCI_DOOR_STATUS's ordinal value */
            UCI_DOOR_STATUS,

            //  UCI_EA_ACTIVITY's ordinal value */
            UCI_EA_ACTIVITY,

            //  UCI_EA_CAPABILITY's ordinal value */
            UCI_EA_CAPABILITY,

            //  UCI_EA_CAPABILITY_STATUS's ordinal value */
            UCI_EA_CAPABILITY_STATUS,

            //  UCI_EA_COMMAND's ordinal value */
            UCI_EA_COMMAND,

            //  UCI_EA_COMMAND_STATUS's ordinal value */
            UCI_EA_COMMAND_STATUS,

            //  UCI_EA_CONSENT_REQUEST's ordinal value */
            UCI_EA_CONSENT_REQUEST,

            //  UCI_EA_CONSENT_REQUEST_STATUS's ordinal value */
            UCI_EA_CONSENT_REQUEST_STATUS,

            //  UCI_EA_SETTINGS_COMMAND's ordinal value */
            UCI_EA_SETTINGS_COMMAND,

            //  UCI_EA_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_EA_SETTINGS_COMMAND_STATUS,

            //  UCI_EEI's ordinal value */
            UCI_EEI,

            //  UCI_ELEVATION_REQUEST's ordinal value */
            UCI_ELEVATION_REQUEST,

            //  UCI_ELEVATION_REQUEST_STATUS's ordinal value */
            UCI_ELEVATION_REQUEST_STATUS,

            //  UCI_ELEVATION_VISIBILITY_REQUEST's ordinal value */
            UCI_ELEVATION_VISIBILITY_REQUEST,

            //  UCI_ELEVATION_VISIBILITY_REQUEST_STATUS's ordinal value */
            UCI_ELEVATION_VISIBILITY_REQUEST_STATUS,

            //  UCI_ENTITY's ordinal value */
            UCI_ENTITY,

            //  UCI_ENTITY_DESIGNATION's ordinal value */
            UCI_ENTITY_DESIGNATION,

            //  UCI_ENTITY_MANAGEMENT_REQUEST's ordinal value */
            UCI_ENTITY_MANAGEMENT_REQUEST,

            //  UCI_ENTITY_MANAGEMENT_REQUEST_STATUS's ordinal value */
            UCI_ENTITY_MANAGEMENT_REQUEST_STATUS,

            //  UCI_ENTITY_METADATA's ordinal value */
            UCI_ENTITY_METADATA,

            //  UCI_ENTITY_PROPAGATION's ordinal value */
            UCI_ENTITY_PROPAGATION,

            //  UCI_EOB_CORRELATION_RECORD's ordinal value */
            UCI_EOB_CORRELATION_RECORD,

            //  UCI_ESM_ACTIVITY's ordinal value */
            UCI_ESM_ACTIVITY,

            //  UCI_ESM_CAPABILITY's ordinal value */
            UCI_ESM_CAPABILITY,

            //  UCI_ESM_CAPABILITY_STATUS's ordinal value */
            UCI_ESM_CAPABILITY_STATUS,

            //  UCI_ESM_COMMAND's ordinal value */
            UCI_ESM_COMMAND,

            //  UCI_ESM_COMMAND_STATUS's ordinal value */
            UCI_ESM_COMMAND_STATUS,

            //  UCI_ESM_CONSENT_REQUEST's ordinal value */
            UCI_ESM_CONSENT_REQUEST,

            //  UCI_ESM_CONSENT_REQUEST_STATUS's ordinal value */
            UCI_ESM_CONSENT_REQUEST_STATUS,

            //  UCI_ESM_SETTINGS_COMMAND's ordinal value */
            UCI_ESM_SETTINGS_COMMAND,

            //  UCI_ESM_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_ESM_SETTINGS_COMMAND_STATUS,

            //  UCI_EW_CONFIGURATION's ordinal value */
            UCI_EW_CONFIGURATION,

            //  UCI_EXTERNAL_DATA_REQUEST's ordinal value */
            UCI_EXTERNAL_DATA_REQUEST,

            //  UCI_EXTERNAL_DATA_REQUEST_STATUS's ordinal value */
            UCI_EXTERNAL_DATA_REQUEST_STATUS,

            //  UCI_FAULT's ordinal value */
            UCI_FAULT,

            //  UCI_FILE_LOCATION's ordinal value */
            UCI_FILE_LOCATION,

            //  UCI_FILE_METADATA's ordinal value */
            UCI_FILE_METADATA,

            //  UCI_FOREIGN_KEY_MAP's ordinal value */
            UCI_FOREIGN_KEY_MAP,

            //  UCI_FUSION_SETTINGS's ordinal value */
            UCI_FUSION_SETTINGS,

            //  UCI_FUSION_SETTINGS_REQUEST's ordinal value */
            UCI_FUSION_SETTINGS_REQUEST,

            //  UCI_FUSION_SETTINGS_REQUEST_STATUS's ordinal value */
            UCI_FUSION_SETTINGS_REQUEST_STATUS,

            //  UCI_GATEWAY_ACTIVITY's ordinal value */
            UCI_GATEWAY_ACTIVITY,

            //  UCI_GATEWAY_CAPABILITY's ordinal value */
            UCI_GATEWAY_CAPABILITY,

            //  UCI_GATEWAY_CAPABILITY_STATUS's ordinal value */
            UCI_GATEWAY_CAPABILITY_STATUS,

            //  UCI_GATEWAY_COMMAND's ordinal value */
            UCI_GATEWAY_COMMAND,

            //  UCI_GATEWAY_COMMAND_STATUS's ordinal value */
            UCI_GATEWAY_COMMAND_STATUS,

            //  UCI_GATEWAY_SETTINGS_COMMAND's ordinal value */
            UCI_GATEWAY_SETTINGS_COMMAND,

            //  UCI_GATEWAY_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_GATEWAY_SETTINGS_COMMAND_STATUS,

            //  UCI_IO_PORT_COMMAND's ordinal value */
            UCI_IO_PORT_COMMAND,

            //  UCI_IO_PORT_COMMAND_STATUS's ordinal value */
            UCI_IO_PORT_COMMAND_STATUS,

            //  UCI_IO_PORT_STATUS's ordinal value */
            UCI_IO_PORT_STATUS,

            //  UCI_LAR's ordinal value */
            UCI_LAR,

            //  UCI_LAR_REPORT's ordinal value */
            UCI_LAR_REPORT,

            //  UCI_LAR_REQUEST's ordinal value */
            UCI_LAR_REQUEST,

            //  UCI_LAR_REQUEST_STATUS's ordinal value */
            UCI_LAR_REQUEST_STATUS,

            //  UCI_LOG's ordinal value */
            UCI_LOG,

            //  UCI_MESSAGE_TRANSMISSION_LIST's ordinal value */
            UCI_MESSAGE_TRANSMISSION_LIST,

            //  UCI_META_TASK's ordinal value */
            UCI_META_TASK,

            //  UCI_META_TASK_CANCEL's ordinal value */
            UCI_META_TASK_CANCEL,

            //  UCI_META_TASK_STATUS's ordinal value */
            UCI_META_TASK_STATUS,

            //  UCI_MISSION_ACTIVATION_REQUEST's ordinal value */
            UCI_MISSION_ACTIVATION_REQUEST,

            //  UCI_MISSION_ACTIVATION_REQUEST_STATUS's ordinal value */
            UCI_MISSION_ACTIVATION_REQUEST_STATUS,

            //  UCI_MISSION_CONTINGENCY_ALERT's ordinal value */
            UCI_MISSION_CONTINGENCY_ALERT,

            //  UCI_MISSION_DEFINITION's ordinal value */
            UCI_MISSION_DEFINITION,

            //  UCI_MISSION_ENVIRONMENT_OVERRIDE's ordinal value */
            UCI_MISSION_ENVIRONMENT_OVERRIDE,

            //  UCI_MISSION_PLAN's ordinal value */
            UCI_MISSION_PLAN,

            //  UCI_MISSION_PLAN_ACTIVATION_COMMAND's ordinal value */
            UCI_MISSION_PLAN_ACTIVATION_COMMAND,

            //  UCI_MISSION_PLAN_ACTIVATION_COMMAND_STATUS's ordinal value */
            UCI_MISSION_PLAN_ACTIVATION_COMMAND_STATUS,

            //  UCI_MISSION_PLAN_ACTIVATION_STATE's ordinal value */
            UCI_MISSION_PLAN_ACTIVATION_STATE,

            //  UCI_MISSION_PLAN_COMMAND's ordinal value */
            UCI_MISSION_PLAN_COMMAND,

            //  UCI_MISSION_PLAN_COMMAND_STATUS's ordinal value */
            UCI_MISSION_PLAN_COMMAND_STATUS,

            //  UCI_MISSION_PLAN_METRICS_REPORT's ordinal value */
            UCI_MISSION_PLAN_METRICS_REPORT,

            //  UCI_MISSION_PLAN_STATUS's ordinal value */
            UCI_MISSION_PLAN_STATUS,

            //  UCI_MISSION_RESPONSE_LIST's ordinal value */
            UCI_MISSION_RESPONSE_LIST,

            //  UCI_MISSION_TASK_STATUS's ordinal value */
            UCI_MISSION_TASK_STATUS,

            //  UCI_NAVIGATION_REPORT's ordinal value */
            UCI_NAVIGATION_REPORT,

            //  UCI_OBSERVATION_REPORT's ordinal value */
            UCI_OBSERVATION_REPORT,

            //  UCI_OPERATOR's ordinal value */
            UCI_OPERATOR,

            //  UCI_OPERATOR_ANNOTATION's ordinal value */
            UCI_OPERATOR_ANNOTATION,

            //  UCI_OPERATOR_LOCATION_OF_INTEREST's ordinal value */
            UCI_OPERATOR_LOCATION_OF_INTEREST,

            //  UCI_OPERATOR_LOCATION_OF_INTEREST_DESIGNATION's ordinal value */
            UCI_OPERATOR_LOCATION_OF_INTEREST_DESIGNATION,

            //  UCI_OPERATOR_ACTION_COMMAND's ordinal value */
            UCI_OPERATOR_ACTION_COMMAND,

            //  UCI_OPERATOR_ACTION_COMMAND_STATUS's ordinal value */
            UCI_OPERATOR_ACTION_COMMAND_STATUS,

            //  UCI_OPERATOR_NOTIFICATION's ordinal value */
            UCI_OPERATOR_NOTIFICATION,

            //  UCI_OPERATOR_NOTIFICATION_ACK's ordinal value */
            UCI_OPERATOR_NOTIFICATION_ACK,

            //  UCI_OPERATOR_NOTIFICATION_OVERRIDE's ordinal value */
            UCI_OPERATOR_NOTIFICATION_OVERRIDE,

            //  UCI_OP_LINE's ordinal value */
            UCI_OP_LINE,

            //  UCI_OP_POINT's ordinal value */
            UCI_OP_POINT,

            //  UCI_OP_ROUTING's ordinal value */
            UCI_OP_ROUTING,

            //  UCI_OP_ZONE's ordinal value */
            UCI_OP_ZONE,

            //  UCI_ORDER_OF_BATTLE's ordinal value */
            UCI_ORDER_OF_BATTLE,

            //  UCI_PO_ACTIVITY's ordinal value */
            UCI_PO_ACTIVITY,

            //  UCI_PO_CAPABILITY's ordinal value */
            UCI_PO_CAPABILITY,

            //  UCI_PO_CAPABILITY_STATUS's ordinal value */
            UCI_PO_CAPABILITY_STATUS,

            //  UCI_PO_COMMAND's ordinal value */
            UCI_PO_COMMAND,

            //  UCI_PO_COMMAND_STATUS's ordinal value */
            UCI_PO_COMMAND_STATUS,

            //  UCI_PO_SETTINGS_COMMAND's ordinal value */
            UCI_PO_SETTINGS_COMMAND,

            //  UCI_PO_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_PO_SETTINGS_COMMAND_STATUS,

            //  UCI_POSITION_REPORT's ordinal value */
            UCI_POSITION_REPORT,

            //  UCI_POSITION_REPORT_DETAILED's ordinal value */
            UCI_POSITION_REPORT_DETAILED,

            //  UCI_POST_ACTIVITY's ordinal value */
            UCI_POST_ACTIVITY,

            //  UCI_POST_CAPABILITY's ordinal value */
            UCI_POST_CAPABILITY,

            //  UCI_POST_CAPABILITY_STATUS's ordinal value */
            UCI_POST_CAPABILITY_STATUS,

            //  UCI_POST_COMMAND's ordinal value */
            UCI_POST_COMMAND,

            //  UCI_POST_COMMAND_STATUS's ordinal value */
            UCI_POST_COMMAND_STATUS,

            //  UCI_POST_SETTINGS_COMMAND's ordinal value */
            UCI_POST_SETTINGS_COMMAND,

            //  UCI_POST_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_POST_SETTINGS_COMMAND_STATUS,

            //  UCI_PRODUCT_CLASSIFICATION_PLAN's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_PLAN,

            //  UCI_PRODUCT_CLASSIFICATION_REPORT's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_REPORT,

            //  UCI_PRODUCT_CLASSIFICATION_REQUEST's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_REQUEST,

            //  UCI_PRODUCT_CLASSIFICATION_REQUEST_STATUS's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_REQUEST_STATUS,

            //  UCI_PRODUCT_CLASSIFICATION_TASK's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_TASK,

            //  UCI_PRODUCT_CLASSIFICATION_TASK_STATUS's ordinal value */
            UCI_PRODUCT_CLASSIFICATION_TASK_STATUS,

            //  UCI_PRODUCT_DISSEMINATION_CONFIGURATION's ordinal value */
            UCI_PRODUCT_DISSEMINATION_CONFIGURATION,

            //  UCI_PRODUCT_DISSEMINATION_DESTINATION's ordinal value */
            UCI_PRODUCT_DISSEMINATION_DESTINATION,

            //  UCI_PRODUCT_DISSEMINATION_PLAN's ordinal value */
            UCI_PRODUCT_DISSEMINATION_PLAN,

            //  UCI_PRODUCT_DISSEMINATION_REPORT's ordinal value */
            UCI_PRODUCT_DISSEMINATION_REPORT,

            //  UCI_PRODUCT_DISSEMINATION_REQUEST's ordinal value */
            UCI_PRODUCT_DISSEMINATION_REQUEST,

            //  UCI_PRODUCT_DISSEMINATION_REQUEST_STATUS's ordinal value */
            UCI_PRODUCT_DISSEMINATION_REQUEST_STATUS,

            //  UCI_PRODUCT_DISSEMINATION_TASK's ordinal value */
            UCI_PRODUCT_DISSEMINATION_TASK,

            //  UCI_PRODUCT_DISSEMINATION_TASK_STATUS's ordinal value */
            UCI_PRODUCT_DISSEMINATION_TASK_STATUS,

            //  UCI_PRODUCT_DOWNLOAD_PLAN's ordinal value */
            UCI_PRODUCT_DOWNLOAD_PLAN,

            //  UCI_PRODUCT_DOWNLOAD_REPORT's ordinal value */
            UCI_PRODUCT_DOWNLOAD_REPORT,

            //  UCI_PRODUCT_DOWNLOAD_REQUEST's ordinal value */
            UCI_PRODUCT_DOWNLOAD_REQUEST,

            //  UCI_PRODUCT_DOWNLOAD_REQUEST_STATUS's ordinal value */
            UCI_PRODUCT_DOWNLOAD_REQUEST_STATUS,

            //  UCI_PRODUCT_DOWNLOAD_TASK's ordinal value */
            UCI_PRODUCT_DOWNLOAD_TASK,

            //  UCI_PRODUCT_DOWNLOAD_TASK_STATUS's ordinal value */
            UCI_PRODUCT_DOWNLOAD_TASK_STATUS,

            //  UCI_PRODUCT_LOCATION's ordinal value */
            UCI_PRODUCT_LOCATION,

            //  UCI_PRODUCT_METADATA's ordinal value */
            UCI_PRODUCT_METADATA,

            //  UCI_PRODUCT_PROCESSING_PLAN's ordinal value */
            UCI_PRODUCT_PROCESSING_PLAN,

            //  UCI_PRODUCT_PROCESSING_REPORT's ordinal value */
            UCI_PRODUCT_PROCESSING_REPORT,

            //  UCI_PRODUCT_PROCESSING_REQUEST's ordinal value */
            UCI_PRODUCT_PROCESSING_REQUEST,

            //  UCI_PRODUCT_PROCESSING_REQUEST_STATUS's ordinal value */
            UCI_PRODUCT_PROCESSING_REQUEST_STATUS,

            //  UCI_PRODUCT_PROCESSING_TASK's ordinal value */
            UCI_PRODUCT_PROCESSING_TASK,

            //  UCI_PRODUCT_PROCESSING_TASK_STATUS's ordinal value */
            UCI_PRODUCT_PROCESSING_TASK_STATUS,

            //  UCI_PULSE_DATA's ordinal value */
            UCI_PULSE_DATA,

            //  UCI_QUERY_REQUEST's ordinal value */
            UCI_QUERY_REQUEST,

            //  UCI_QUERY_REQUEST_STATUS's ordinal value */
            UCI_QUERY_REQUEST_STATUS,

            //  UCI_READINESS_STATUS's ordinal value */
            UCI_READINESS_STATUS,

            //  UCI_REFUEL_ACTIVITY's ordinal value */
            UCI_REFUEL_ACTIVITY,

            //  UCI_REFUEL_CAPABILITY's ordinal value */
            UCI_REFUEL_CAPABILITY,

            //  UCI_REFUEL_CAPABILITY_STATUS's ordinal value */
            UCI_REFUEL_CAPABILITY_STATUS,

            //  UCI_REFUEL_COMMAND's ordinal value */
            UCI_REFUEL_COMMAND,

            //  UCI_REFUEL_COMMAND_STATUS's ordinal value */
            UCI_REFUEL_COMMAND_STATUS,

            //  UCI_REFUEL_SETTINGS_COMMAND's ordinal value */
            UCI_REFUEL_SETTINGS_COMMAND,

            //  UCI_REFUEL_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_REFUEL_SETTINGS_COMMAND_STATUS,

            //  UCI_RELATIONSHIP_DESIGNATION's ordinal value */
            UCI_RELATIONSHIP_DESIGNATION,

            //  UCI_RESEND_REQUEST's ordinal value */
            UCI_RESEND_REQUEST,

            //  UCI_RESEND_REQUEST_STATUS's ordinal value */
            UCI_RESEND_REQUEST_STATUS,

            //  UCI_RF_CONTROL_COMMAND's ordinal value */
            UCI_RF_CONTROL_COMMAND,

            //  UCI_RF_CONTROL_COMMAND_STATUS's ordinal value */
            UCI_RF_CONTROL_COMMAND_STATUS,

            //  UCI_RF_PROFILE's ordinal value */
            UCI_RF_PROFILE,

            //  UCI_RF_REPORT's ordinal value */
            UCI_RF_REPORT,

            //  UCI_RF_RESOURCE_ALLOCATION's ordinal value */
            UCI_RF_RESOURCE_ALLOCATION,

            //  UCI_RF_RESOURCE_REQUEST's ordinal value */
            UCI_RF_RESOURCE_REQUEST,

            //  UCI_RF_RESOURCE_REQUEST_STATUS's ordinal value */
            UCI_RF_RESOURCE_REQUEST_STATUS,

            //  UCI_RF_THREAD_INSTANCE_SETUP_COMMAND's ordinal value */
            UCI_RF_THREAD_INSTANCE_SETUP_COMMAND,

            //  UCI_RF_THREAD_INSTANCE_SETUP_COMMAND_STATUS's ordinal value */
            UCI_RF_THREAD_INSTANCE_SETUP_COMMAND_STATUS,

            //  UCI_RFDC_STATUS's ordinal value */
            UCI_RFDC_STATUS,

            //  UCI_ROUTE's ordinal value */
            UCI_ROUTE,

            //  UCI_RUNWAY's ordinal value */
            UCI_RUNWAY,

            //  UCI_SAR_ACTIVITY's ordinal value */
            UCI_SAR_ACTIVITY,

            //  UCI_SAR_CAPABILITY's ordinal value */
            UCI_SAR_CAPABILITY,

            //  UCI_SAR_CAPABILITY_STATUS's ordinal value */
            UCI_SAR_CAPABILITY_STATUS,

            //  UCI_SAR_COMMAND's ordinal value */
            UCI_SAR_COMMAND,

            //  UCI_SAR_COMMAND_STATUS's ordinal value */
            UCI_SAR_COMMAND_STATUS,

            //  UCI_SAR_SETTINGS_COMMAND's ordinal value */
            UCI_SAR_SETTINGS_COMMAND,

            //  UCI_SAR_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_SAR_SETTINGS_COMMAND_STATUS,

            //  UCI_SECURITY_AUDIT_RECORD's ordinal value */
            UCI_SECURITY_AUDIT_RECORD,

            //  UCI_SELF_DEFENSE_STATUS's ordinal value */
            UCI_SELF_DEFENSE_STATUS,

            //  UCI_SERVICE_CONFIGURATION_CHANGE_REQUEST's ordinal value */
            UCI_SERVICE_CONFIGURATION_CHANGE_REQUEST,

            //  UCI_SERVICE_CONFIGURATION_CHANGE_REQUEST_STATUS's ordinal value */
            UCI_SERVICE_CONFIGURATION_CHANGE_REQUEST_STATUS,

            //  UCI_SERVICE_CONFIGURATION_PARAMS's ordinal value */
            UCI_SERVICE_CONFIGURATION_PARAMS,

            //  UCI_SERVICE_CONFIGURATION_PARAMS_REQUEST's ordinal value */
            UCI_SERVICE_CONFIGURATION_PARAMS_REQUEST,

            //  UCI_SERVICE_CONFIGURATION_PARAMS_REQUEST_STATUS's ordinal value */
            UCI_SERVICE_CONFIGURATION_PARAMS_REQUEST_STATUS,

            //  UCI_SERVICE_ERROR_REPORT's ordinal value */
            UCI_SERVICE_ERROR_REPORT,

            //  UCI_SERVICE_LIFECYCLE_REQUEST's ordinal value */
            UCI_SERVICE_LIFECYCLE_REQUEST,

            //  UCI_SERVICE_LIFECYCLE_REQUEST_STATUS's ordinal value */
            UCI_SERVICE_LIFECYCLE_REQUEST_STATUS,

            //  UCI_SERVICE_SETTINGS_COMMAND's ordinal value */
            UCI_SERVICE_SETTINGS_COMMAND,

            //  UCI_SERVICE_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_SERVICE_SETTINGS_COMMAND_STATUS,

            //  UCI_SERVICE_STATUS's ordinal value */
            UCI_SERVICE_STATUS,

            //  UCI_SERVICE_STATUS_REQUEST's ordinal value */
            UCI_SERVICE_STATUS_REQUEST,

            //  UCI_SERVICE_STATUS_REQUEST_STATUS's ordinal value */
            UCI_SERVICE_STATUS_REQUEST_STATUS,

            //  UCI_SIGNAL_REPORT's ordinal value */
            UCI_SIGNAL_REPORT,

            //  UCI_SMTI_ACTIVITY's ordinal value */
            UCI_SMTI_ACTIVITY,

            //  UCI_SMTI_CAPABILITY's ordinal value */
            UCI_SMTI_CAPABILITY,

            //  UCI_SMTI_CAPABILITY_STATUS's ordinal value */
            UCI_SMTI_CAPABILITY_STATUS,

            //  UCI_SMTI_COMMAND's ordinal value */
            UCI_SMTI_COMMAND,

            //  UCI_SMTI_COMMAND_STATUS's ordinal value */
            UCI_SMTI_COMMAND_STATUS,

            //  UCI_SMTI_SETTINGS_COMMAND's ordinal value */
            UCI_SMTI_SETTINGS_COMMAND,

            //  UCI_SMTI_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_SMTI_SETTINGS_COMMAND_STATUS,

            //  UCI_SPECTRAL_DENSITY_REPORT's ordinal value */
            UCI_SPECTRAL_DENSITY_REPORT,

            //  UCI_STORE_CARRIAGE_CAPABILITY_STATUS's ordinal value */
            UCI_STORE_CARRIAGE_CAPABILITY_STATUS,

            //  UCI_STORE_MANAGEMENT_COMMAND's ordinal value */
            UCI_STORE_MANAGEMENT_COMMAND,

            //  UCI_STORE_MANAGEMENT_COMMAND_STATUS's ordinal value */
            UCI_STORE_MANAGEMENT_COMMAND_STATUS,

            //  UCI_STORE_MANAGEMENT_REPORT's ordinal value */
            UCI_STORE_MANAGEMENT_REPORT,

            //  UCI_STORE_MANAGEMENT_STATUS's ordinal value */
            UCI_STORE_MANAGEMENT_STATUS,

            //  UCI_STRIKE_ACTIVITY's ordinal value */
            UCI_STRIKE_ACTIVITY,

            //  UCI_STRIKE_CAPABILITY's ordinal value */
            UCI_STRIKE_CAPABILITY,

            //  UCI_STRIKE_CAPABILITY_STATUS's ordinal value */
            UCI_STRIKE_CAPABILITY_STATUS,

            //  UCI_STRIKE_COMMAND's ordinal value */
            UCI_STRIKE_COMMAND,

            //  UCI_STRIKE_COMMAND_STATUS's ordinal value */
            UCI_STRIKE_COMMAND_STATUS,

            //  UCI_STRIKE_CONSENT_REQUEST's ordinal value */
            UCI_STRIKE_CONSENT_REQUEST,

            //  UCI_STRIKE_CONSENT_REQUEST_STATUS's ordinal value */
            UCI_STRIKE_CONSENT_REQUEST_STATUS,

            //  UCI_STRIKE_SETTINGS_COMMAND's ordinal value */
            UCI_STRIKE_SETTINGS_COMMAND,

            //  UCI_STRIKE_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_STRIKE_SETTINGS_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_BIT_COMMAND's ordinal value */
            UCI_SUBSYSTEM_BIT_COMMAND,

            //  UCI_SUBSYSTEM_BIT_COMMAND_STATUS's ordinal value */
            UCI_SUBSYSTEM_BIT_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_BIT_CONFIGURATION's ordinal value */
            UCI_SUBSYSTEM_BIT_CONFIGURATION,

            //  UCI_SUBSYSTEM_BIT_STATUS's ordinal value */
            UCI_SUBSYSTEM_BIT_STATUS,

            //  UCI_SUBSYSTEM_CALIBRATION_COMMAND's ordinal value */
            UCI_SUBSYSTEM_CALIBRATION_COMMAND,

            //  UCI_SUBSYSTEM_CALIBRATION_COMMAND_STATUS's ordinal value */
            UCI_SUBSYSTEM_CALIBRATION_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_CALIBRATION_CONFIGURATION's ordinal value */
            UCI_SUBSYSTEM_CALIBRATION_CONFIGURATION,

            //  UCI_SUBSYSTEM_CALIBRATION_STATUS's ordinal value */
            UCI_SUBSYSTEM_CALIBRATION_STATUS,

            //  UCI_SUBSYSTEM_ERASE_COMMAND's ordinal value */
            UCI_SUBSYSTEM_ERASE_COMMAND,

            //  UCI_SUBSYSTEM_ERASE_COMMAND_STATUS's ordinal value */
            UCI_SUBSYSTEM_ERASE_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_SETTINGS_COMMAND's ordinal value */
            UCI_SUBSYSTEM_SETTINGS_COMMAND,

            //  UCI_SUBSYSTEM_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_SUBSYSTEM_SETTINGS_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_STATE_COMMAND's ordinal value */
            UCI_SUBSYSTEM_STATE_COMMAND,

            //  UCI_SUBSYSTEM_STATE_COMMAND_STATUS's ordinal value */
            UCI_SUBSYSTEM_STATE_COMMAND_STATUS,

            //  UCI_SUBSYSTEM_STATUS_DETAILED's ordinal value */
            UCI_SUBSYSTEM_STATUS_DETAILED,

            //  UCI_SUBSYSTEM_STATUS_DETAILED_REQUEST's ordinal value */
            UCI_SUBSYSTEM_STATUS_DETAILED_REQUEST,

            //  UCI_SUBSYSTEM_STATUS_DETAILED_REQUEST_STATUS's ordinal value */
            UCI_SUBSYSTEM_STATUS_DETAILED_REQUEST_STATUS,

            //  UCI_SUBSYSTEM_STATUS's ordinal value */
            UCI_SUBSYSTEM_STATUS,

            //  UCI_SUBSYSTEM_STATUS_REQUEST's ordinal value */
            UCI_SUBSYSTEM_STATUS_REQUEST,

            //  UCI_SUBSYSTEM_STATUS_REQUEST_STATUS's ordinal value */
            UCI_SUBSYSTEM_STATUS_REQUEST_STATUS,

            //  UCI_SUBSYSTEM_STREAM's ordinal value */
            UCI_SUBSYSTEM_STREAM,

            //  UCI_SURVIVABILITY_RISK_LEVEL's ordinal value */
            UCI_SURVIVABILITY_RISK_LEVEL,

            //  UCI_SYSTEM_ESTIMATION_REQUEST's ordinal value */
            UCI_SYSTEM_ESTIMATION_REQUEST,

            //  UCI_SYSTEM_ESTIMATION_REQUEST_STATUS's ordinal value */
            UCI_SYSTEM_ESTIMATION_REQUEST_STATUS,

            //  UCI_SYSTEM_MANAGEMENT_REQUEST's ordinal value */
            UCI_SYSTEM_MANAGEMENT_REQUEST,

            //  UCI_SYSTEM_MANAGEMENT_REQUEST_STATUS's ordinal value */
            UCI_SYSTEM_MANAGEMENT_REQUEST_STATUS,

            //  UCI_SYSTEM_METADATA's ordinal value */
            UCI_SYSTEM_METADATA,

            //  UCI_SYSTEM_STATUS's ordinal value */
            UCI_SYSTEM_STATUS,

            //  UCI_SYSTEM_STATUS_REQUEST's ordinal value */
            UCI_SYSTEM_STATUS_REQUEST,

            //  UCI_SYSTEM_STATUS_REQUEST_STATUS's ordinal value */
            UCI_SYSTEM_STATUS_REQUEST_STATUS,

            //  UCI_SYSTEM_TIME_AT_REFERENCE's ordinal value */
            UCI_SYSTEM_TIME_AT_REFERENCE,

            //  UCI_TASK's ordinal value */
            UCI_TASK,

            //  UCI_TASK_CANCEL's ordinal value */
            UCI_TASK_CANCEL,

            //  UCI_TASK_SET's ordinal value */
            UCI_TASK_SET,

            //  UCI_TASK_STATUS's ordinal value */
            UCI_TASK_STATUS,

            //  UCI_THREAT_TARGET_PRIORITIZATION_LIST's ordinal value */
            UCI_THREAT_TARGET_PRIORITIZATION_LIST,

            //  UCI_TURRET_COMMAND's ordinal value */
            UCI_TURRET_COMMAND,

            //  UCI_TURRET_COMMAND_STATUS's ordinal value */
            UCI_TURRET_COMMAND_STATUS,

            //  UCI_TURRET_REPORT's ordinal value */
            UCI_TURRET_REPORT,

            //  UCI_TURRET_STATUS's ordinal value */
            UCI_TURRET_STATUS,

            //  UCI_WEATHER's ordinal value */
            UCI_WEATHER,

            //  UCI_WEATHER_DATASET's ordinal value */
            UCI_WEATHER_DATASET,

            //  UCI_WEATHER_OBSERVATION's ordinal value */
            UCI_WEATHER_OBSERVATION,

            //  UCI_WEATHER_RADAR_ACTIVITY's ordinal value */
            UCI_WEATHER_RADAR_ACTIVITY,

            //  UCI_WEATHER_RADAR_CAPABILITY's ordinal value */
            UCI_WEATHER_RADAR_CAPABILITY,

            //  UCI_WEATHER_RADAR_CAPABILITY_STATUS's ordinal value */
            UCI_WEATHER_RADAR_CAPABILITY_STATUS,

            //  UCI_WEATHER_RADAR_COMMAND's ordinal value */
            UCI_WEATHER_RADAR_COMMAND,

            //  UCI_WEATHER_RADAR_COMMAND_STATUS's ordinal value */
            UCI_WEATHER_RADAR_COMMAND_STATUS,

            //  UCI_WEATHER_RADAR_OBSERVATION's ordinal value */
            UCI_WEATHER_RADAR_OBSERVATION,

            //  UCI_WEATHER_RADAR_SETTINGS_COMMAND's ordinal value */
            UCI_WEATHER_RADAR_SETTINGS_COMMAND,

            //  UCI_WEATHER_RADAR_SETTINGS_COMMAND_STATUS's ordinal value */
            UCI_WEATHER_RADAR_SETTINGS_COMMAND_STATUS,

            //  UCI_WORKING_EOB's ordinal value */
            UCI_WORKING_EOB,

            //  UCI_WORKING_SOB's ordinal value */
            UCI_WORKING_SOB,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~MessageTypeEnum()
         { }

         /** Returns this accessor's type constant, i.e. messageTypeEnum
           *
           * @return This accessor's type constant, i.e. messageTypeEnum
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::messageTypeEnum;
         }


         /** The assignment operator. Sets the contents of this MessageTypeEnum to the contents of the MessageTypeEnum on the
           * right hand side (rhs) of the assignment operator.MessageTypeEnum
           *
           * @param rhs The MessageTypeEnum on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this MessageTypeEnum
           * @return A constant reference to this MessageTypeEnum.
           */
         const MessageTypeEnum& operator=(const MessageTypeEnum& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this MessageTypeEnum enumeration to the EnumerationItem on the right hand
           * side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this MessageTypeEnum enumeration
           * @return A reference to this MessageTypeEnum enumeration
           */
         const MessageTypeEnum& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this MessageTypeEnum enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this MessageTypeEnum enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this MessageTypeEnum enumeration.
           *
           * @param testForValidity=true Specifies whether this MessageTypeEnum enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this MessageTypeEnum enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this MessageTypeEnum enumeration.
           *
           * @return The number of items in this MessageTypeEnum enumeration.
           */
         int getNumberOfItems() const
         {
            return 387;
         }


         /** Returns whether this MessageTypeEnum enumeration is valid or not. A MessageTypeEnum enumeration is valid if its value
           * is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this MessageTypeEnum enumeration is valid (true) or not (false).
           */
         virtual bool isValid() const
         {
            return isValid(getValue(false));
         }


         /** Returns whether the specified enumeration item is valid or not. A enumeration item is valid if its value is one of
           * the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @param item The EnumerationItem to test
           * @return A flag indicating whether the specified EnumerationItem is valid (true) or not (false).
           */
         static bool isValid(EnumerationItem item)
         {
            return (item > enumNotSet) && (item < enumMaxExclusive);
         }


         /** Returns whether the specified name item is valid or not. A name is valid if it is the name of one of the enumeration
           * items other than enumNotSet and enumMaxExclusive.
           *
           * @param name The name to test
           * @return A flag indicating whether the specified name is valid (true) or not (false).
           */
         static bool isValid(const std::string& name)
            throw(uci::base::UCIException)
         {
            initializeTables();
            FromNameTable::const_iterator iter = m_fromNameTable.find(name);
            return !(iter == m_fromNameTable.end());
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the == operator is equal to the
           * value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is equal to the value of this
           *      MessageTypeEnum enumeration, false otherwise
           */
         bool operator==(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the != operator is not equal to
           * the value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is not equal to the value of this
           *      MessageTypeEnum enumeration, false otherwise
           */
         bool operator!=(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the < operator is less than the
           * value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is less than the value of this
           *      MessageTypeEnum enumeration, false otherwise
           */
         bool operator<(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the <= operator is less than or
           * equal to the value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is less than or equal to the value of
           *      this MessageTypeEnum enumeration, false otherwise
           */
         bool operator<=(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the > operator is greater than
           * the value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is greater than the value of this
           *      MessageTypeEnum enumeration, false otherwise
           */
         bool operator>(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the MessageTypeEnum enumeration on the right hand side of the >= operator is greater than
           * or equal to the value as this MessageTypeEnum enumeration.
           *
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of this MessageTypeEnum
           *      enumeration.
           * @return True if the value of the specified MessageTypeEnum enumeration (rhs) is greater than or equal to the value of
           *      this MessageTypeEnum enumeration, false otherwise
           */
         bool operator>=(const MessageTypeEnum& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is equal to the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this MessageTypeEnum enumeration,
           *      false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified MessageTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the MessageTypeEnum enumeration (rhs),
           *      false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this MessageTypeEnum enumeration,
           *      false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified MessageTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the MessageTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is less than the value of the specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this MessageTypeEnum enumeration,
           *      false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified MessageTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the MessageTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this MessageTypeEnum
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * MessageTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the MessageTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this MessageTypeEnum enumeration,
           *      false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified MessageTypeEnum
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the MessageTypeEnum enumeration
           *      (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this MessageTypeEnum enumeration is greater than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this MessageTypeEnum enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this MessageTypeEnum
           *      enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * MessageTypeEnum enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified MessageTypeEnum
           *      enumeration.
           * @param rhs The MessageTypeEnum enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the MessageTypeEnum
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const MessageTypeEnum& rhs)
         {
            testForValidItem(lhs);
            return lhs >= rhs.getValue();
         }


         /** Returns the name of the specified enumeration item.
           *
           * @param item The enumeration item whose name is to be returned.
           * @return The enumeration item's name if valid
           */
         static const std::string& toName(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            initializeTables();

            ToNameTable::const_iterator iter = m_toNameTable.find(item);
            if (iter == m_toNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to convert enumerated item to its name, item not found in mapping: " << item;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Returns the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item to be returned.
           * @return The enumeration item having the specified name
           */
         static EnumerationItem fromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            initializeTables();

            FromNameTable::const_iterator iter = m_fromNameTable.find(itemName);
            if (iter == m_fromNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to return enumerated item having specified name, name not found in mapping: " << itemName;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Return the name of the enumeration item that the value of this MessageTypeEnum enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this MessageTypeEnum enumeration to the value of the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this MessageTypeEnum enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new MessageTypeEnum, this constructor also makes sure the
           * conversion tables are initialized
           */
         MessageTypeEnum()
         {
            initializeTables();
         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param additionalDescription An additional description to include in the error message should an exception be thrown
           * @param item The enumeration item to test
           */
         static void testForValidItem(const char* additionalDescription, EnumerationItem item)
            throw(uci::base::UCIException)
         {
            if (!isValid(item)) {
               std::ostringstream oStream;
               oStream << additionalDescription << ": " << item << " (item must be between " << enumNotSet << " and "<< enumMaxExclusive << " exclusive)";
               throw uci::base::UCIException(oStream.str(), 0);
            }

         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param item The enumeration item to test
           */
         static void testForValidItem(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            testForValidItem("Invalid enumerated item specified", item);
         }



      private:

         //  The type of table that is used to lookup the name of a enumeration item given that enumeration item.
         typedef std::map<EnumerationItem, const std::string> ToNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         typedef std::map<const std::string, EnumerationItem> FromNameTable;

         //  The table that is used to lookup the name of a enumeration item given that enumeration item.
         static ToNameTable m_toNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         static FromNameTable m_fromNameTable;

         //  Indicates whether the tables has been initialized (true) or not (false).
         static bool m_tablesInitialized;

         //  Initializes the translation tables
         static void initializeTables();


      protected:

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MessageTypeEnum to copy from
           */
         MessageTypeEnum(const MessageTypeEnum& rhs)
         {
            (void)rhs;
         }

      }; // MessageTypeEnum


   } // Namespace: type
} // Namespace: uci
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const uci::type::MessageTypeEnum& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Uci__Type__MessageTypeEnum_h

