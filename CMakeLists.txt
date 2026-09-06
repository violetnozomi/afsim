project(wsf_network_resource_manager)

wsf_project_template(${PROJECT_NAME} PLUGIN
   HDRS
      include/nrm/NetworkResourceTypes.hpp
      include/nrm/AssessmentTypes.hpp
      include/nrm/AssessmentEvaluator.hpp
      include/nrm/ConcurrentTaskAssessment.hpp
      include/nrm/DegradationPolicy.hpp
      include/nrm/ContractMetricEnricher.hpp
      include/nrm/FrequencyCharacteristicRepository.hpp
      include/nrm/ProtocolResourceModel.hpp
      include/nrm/OperationalEnvironmentEvaluator.hpp
      include/nrm/InputProvider.hpp
      include/nrm/TemporaryPathGuard.hpp
      include/nrm/NetworkTypeUtils.hpp
      include/nrm/RollingMetrics.hpp
      include/nrm/LinkMetricsRepository.hpp
      include/nrm/MetricReason.hpp
      include/nrm/MessageLifecycleTypes.hpp
      include/nrm/MessageLifecycleTracker.hpp
      include/nrm/GatewayTypes.hpp
      include/nrm/GatewayPolicyEngine.hpp
      include/nrm/NetworkProfile.hpp
      include/nrm/NetworkProfileRepository.hpp
      include/nrm/ResourceEventTypes.hpp
      include/nrm/ResourceEventLedger.hpp
      include/nrm/ConstrainedPathSelector.hpp
      include/nrm/CommunicationCapabilityTypes.hpp
      include/nrm/CommunicationCapabilityService.hpp
      include/nrm/EnvironmentConfigRepository.hpp
      include/nrm/BuiltInEnvironmentEffectAdapter.hpp
      include/nrm/AfsimNavigationPacketParser.hpp
      include/nrm/NetworkPlanTypes.hpp
      include/nrm/NetworkPlanRepository.hpp
      include/nrm/NetworkPlanCoordinationService.hpp
      include/nrm/NetworkPlanValidator.hpp
      include/nrm/NetworkPlanEvaluationService.hpp
      include/nrm/NetworkPlanSerialization.hpp
      include/nrm/NetworkPlanDistributionService.hpp
      include/nrm/ResourceDemandTypes.hpp
      include/nrm/ResourceDemandRepository.hpp
      include/nrm/ResourceDemandCoordinator.hpp
      include/nrm/PlanningRecommendationEngine.hpp
      include/nrm/ResourceDemandMatchingService.hpp
      include/nrm/ModelServiceTypes.hpp
      include/nrm/ModelServiceFacade.hpp
      include/nrm/ModelRegistry.hpp
      include/nrm/ContractInterfaceAdapter.hpp
      include/nrm/CustomerIngestionState.hpp
      include/nrm/CustomerNrmAdapter.hpp
      include/nrm/CustomerSnapshotAssembler.hpp
      include/nrm/EffectiveSnapshotAssembler.hpp
      include/nrm/ResourceSnapshotValidator.hpp
      include/nrm/Version.hpp
      source/WsfNetworkResourceManagerPlugin.hpp
      source/WsfNrmGatewayExtension.hpp
      source/WsfNrmCrossDomainGatewayProcessor.hpp
   SRCS
      source/WsfNetworkResourceManagerPlugin.cpp
      source/WsfNrmGatewayExtension.cpp
      source/WsfNrmCrossDomainGatewayProcessor.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/source
)

add_executable(nrm_framework_types_test EXCLUDE_FROM_ALL
   tests/NetworkResourceTypesTest.cpp
)
target_include_directories(nrm_framework_types_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_framework_types_test COMMAND nrm_framework_types_test)

add_executable(nrm_ui_text_test EXCLUDE_FROM_ALL
   tests/UiTextTest.cpp
   warlock/source/NrmUiText.cpp
)
target_include_directories(nrm_ui_text_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
add_test(NAME nrm_ui_text_test COMMAND nrm_ui_text_test)

add_executable(nrm_ui_scale_test EXCLUDE_FROM_ALL
   tests/UiScaleTest.cpp
   warlock/source/NrmTacticalViewport.cpp
   warlock/source/NrmUiScale.cpp
   warlock/source/NrmUiStyle.cpp
)
target_include_directories(nrm_ui_scale_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
find_package(Qt5Widgets REQUIRED)
target_link_libraries(nrm_ui_scale_test PRIVATE Qt5::Widgets)
add_test(NAME nrm_ui_scale_test COMMAND nrm_ui_scale_test)

add_executable(nrm_acceptance_demo_panel_test EXCLUDE_FROM_ALL
   tests/AcceptanceDemoPanelTest.cpp
   warlock/source/NrmAcceptanceDemoPanel.cpp
)
set_target_properties(nrm_acceptance_demo_panel_test PROPERTIES AUTOMOC ON)
target_include_directories(nrm_acceptance_demo_panel_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_link_libraries(nrm_acceptance_demo_panel_test PRIVATE Qt5::Widgets)
add_test(NAME nrm_acceptance_demo_panel_test COMMAND nrm_acceptance_demo_panel_test)

add_executable(nrm_demand_detail_tabs_test EXCLUDE_FROM_ALL
   tests/DemandDetailTabsTest.cpp
   warlock/source/NrmDemandDetailTabs.cpp
)
target_include_directories(nrm_demand_detail_tabs_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_link_libraries(nrm_demand_detail_tabs_test PRIVATE Qt5::Widgets)
add_test(NAME nrm_demand_detail_tabs_test COMMAND nrm_demand_detail_tabs_test)

add_executable(nrm_operational_detail_tabs_test EXCLUDE_FROM_ALL
   tests/OperationalDetailTabsTest.cpp
   warlock/source/NrmOperationalDetailTabs.cpp
)
target_include_directories(nrm_operational_detail_tabs_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_link_libraries(nrm_operational_detail_tabs_test PRIVATE Qt5::Widgets)
add_test(NAME nrm_operational_detail_tabs_test COMMAND nrm_operational_detail_tabs_test)

add_executable(nrm_operator_presentation_test EXCLUDE_FROM_ALL
   tests/OperatorPresentationTest.cpp
   warlock/source/NrmOperatorPresentation.cpp
   warlock/source/NrmUiText.cpp
)
target_include_directories(nrm_operator_presentation_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_link_libraries(nrm_operator_presentation_test PRIVATE Qt5::Widgets)
add_test(NAME nrm_operator_presentation_test COMMAND nrm_operator_presentation_test)

add_executable(nrm_tactical_selection_test EXCLUDE_FROM_ALL
   tests/TacticalSelectionTest.cpp
   warlock/source/NrmTacticalSelection.cpp
)
target_include_directories(nrm_tactical_selection_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
add_test(NAME nrm_tactical_selection_test COMMAND nrm_tactical_selection_test)

add_test(NAME nrm_remote_plan_switch_test
   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/RemotePlanSwitchTest.sh
)

add_test(NAME nrm_deployment_contract_test
   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/DeploymentContractTest.sh
)

add_test(NAME nrm_customer_interface_validation_script_test
   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/CustomerInterfaceValidationScriptTest.sh
)

add_executable(nrm_snapshot_reporter_test EXCLUDE_FROM_ALL
   tests/SnapshotReporterTest.cpp
   warlock/source/NrmSnapshotReporter.cpp
)
target_include_directories(nrm_snapshot_reporter_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
if(UNIX)
   target_link_libraries(nrm_snapshot_reporter_test PRIVATE pthread)
endif()
add_test(NAME nrm_snapshot_reporter_test COMMAND nrm_snapshot_reporter_test)

add_executable(nrm_assessment_evaluator_test EXCLUDE_FROM_ALL
   tests/AssessmentEvaluatorTest.cpp
)
target_include_directories(nrm_assessment_evaluator_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_assessment_evaluator_test COMMAND nrm_assessment_evaluator_test)

add_executable(nrm_gateway_policy_engine_test EXCLUDE_FROM_ALL
   tests/GatewayPolicyEngineTest.cpp
)
target_include_directories(nrm_gateway_policy_engine_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_gateway_policy_engine_test COMMAND nrm_gateway_policy_engine_test)

add_test(NAME nrm_operational_gateway_scenario_test
   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/OperationalGatewayScenarioTest.sh
)

add_test(NAME nrm_pdr_diversity_scenario_test
   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/PdrDiversityScenarioTest.sh
)
set_tests_properties(nrm_pdr_diversity_scenario_test PROPERTIES
   ENVIRONMENT "AFSIM_BUILD=${CMAKE_BINARY_DIR}"
)

add_executable(nrm_concurrent_task_assessment_test EXCLUDE_FROM_ALL
   tests/ConcurrentTaskAssessmentTest.cpp
)
target_include_directories(nrm_concurrent_task_assessment_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_concurrent_task_assessment_test
   COMMAND nrm_concurrent_task_assessment_test)

add_executable(nrm_degradation_policy_test EXCLUDE_FROM_ALL
   tests/DegradationPolicyTest.cpp
)
target_include_directories(nrm_degradation_policy_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_degradation_policy_test COMMAND nrm_degradation_policy_test)

add_executable(nrm_contract_metric_enricher_test EXCLUDE_FROM_ALL
   tests/ContractMetricEnricherTest.cpp
)
target_include_directories(nrm_contract_metric_enricher_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_contract_metric_enricher_test COMMAND nrm_contract_metric_enricher_test)

add_executable(nrm_network_profile_test EXCLUDE_FROM_ALL
   tests/NetworkProfileTest.cpp
)
target_include_directories(nrm_network_profile_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(nrm_network_profile_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME nrm_network_profile_test COMMAND nrm_network_profile_test)

add_executable(nrm_frequency_characteristic_test EXCLUDE_FROM_ALL
   tests/FrequencyCharacteristicRepositoryTest.cpp
)
target_include_directories(nrm_frequency_characteristic_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(nrm_frequency_characteristic_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME nrm_frequency_characteristic_test COMMAND nrm_frequency_characteristic_test)

add_executable(nrm_protocol_resource_model_test EXCLUDE_FROM_ALL
   tests/ProtocolResourceModelTest.cpp
)
target_include_directories(nrm_protocol_resource_model_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_protocol_resource_model_test COMMAND nrm_protocol_resource_model_test)

add_executable(nrm_message_lifecycle_tracker_test EXCLUDE_FROM_ALL
   tests/MessageLifecycleTrackerTest.cpp
)
target_include_directories(nrm_message_lifecycle_tracker_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_message_lifecycle_tracker_test COMMAND nrm_message_lifecycle_tracker_test)

add_executable(nrm_link_metrics_repository_test EXCLUDE_FROM_ALL
   tests/LinkMetricsRepositoryTest.cpp
)
target_include_directories(nrm_link_metrics_repository_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_link_metrics_repository_test COMMAND nrm_link_metrics_repository_test)

add_executable(nrm_resource_event_ledger_test EXCLUDE_FROM_ALL
   tests/ResourceEventLedgerTest.cpp
)
target_include_directories(nrm_resource_event_ledger_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_resource_event_ledger_test COMMAND nrm_resource_event_ledger_test)

add_executable(nrm_constrained_path_selector_test EXCLUDE_FROM_ALL
   tests/ConstrainedPathSelectorTest.cpp
)
target_include_directories(nrm_constrained_path_selector_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_constrained_path_selector_test COMMAND nrm_constrained_path_selector_test)

add_executable(nrm_snapshot_reporter_recovery_test EXCLUDE_FROM_ALL
   tests/SnapshotReporterRecoveryTest.cpp
   warlock/source/NrmSnapshotReporter.cpp
)
target_include_directories(nrm_snapshot_reporter_recovery_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
if(UNIX)
   target_link_libraries(nrm_snapshot_reporter_recovery_test PRIVATE pthread)
endif()
add_test(NAME nrm_snapshot_reporter_recovery_test COMMAND nrm_snapshot_reporter_recovery_test)

add_executable(nrm_communication_capability_service_test EXCLUDE_FROM_ALL
   tests/CommunicationCapabilityServiceTest.cpp
)
target_include_directories(nrm_communication_capability_service_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_communication_capability_service_test
   COMMAND nrm_communication_capability_service_test)

add_executable(nrm_network_plan_repository_test EXCLUDE_FROM_ALL
   tests/NetworkPlanRepositoryTest.cpp
)
target_include_directories(nrm_network_plan_repository_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(nrm_network_plan_repository_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME nrm_network_plan_repository_test
   COMMAND nrm_network_plan_repository_test)

add_executable(nrm_network_plan_evaluation_test EXCLUDE_FROM_ALL
   tests/NetworkPlanEvaluationTest.cpp
)
target_include_directories(nrm_network_plan_evaluation_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_network_plan_evaluation_test
   COMMAND nrm_network_plan_evaluation_test)

add_executable(nrm_network_plan_coordination_test EXCLUDE_FROM_ALL
   tests/NetworkPlanCoordinationServiceTest.cpp
)
target_include_directories(nrm_network_plan_coordination_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_network_plan_coordination_test
   COMMAND nrm_network_plan_coordination_test)

add_executable(nrm_resource_demand_repository_test EXCLUDE_FROM_ALL
   tests/ResourceDemandRepositoryTest.cpp
)
target_include_directories(nrm_resource_demand_repository_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(nrm_resource_demand_repository_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME nrm_resource_demand_repository_test
   COMMAND nrm_resource_demand_repository_test)

add_executable(nrm_resource_demand_matching_test EXCLUDE_FROM_ALL
   tests/ResourceDemandMatchingTest.cpp
)
target_include_directories(nrm_resource_demand_matching_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_resource_demand_matching_test
   COMMAND nrm_resource_demand_matching_test)

add_executable(nrm_resource_demand_coordinator_test EXCLUDE_FROM_ALL
   tests/ResourceDemandCoordinatorTest.cpp
)
target_include_directories(nrm_resource_demand_coordinator_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_resource_demand_coordinator_test
   COMMAND nrm_resource_demand_coordinator_test)

add_executable(nrm_model_service_facade_test EXCLUDE_FROM_ALL
   tests/ModelServiceFacadeTest.cpp
)
target_include_directories(nrm_model_service_facade_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_model_service_facade_test
   COMMAND nrm_model_service_facade_test)

add_executable(nrm_model_registry_test EXCLUDE_FROM_ALL
   tests/ModelRegistryTest.cpp
)
target_include_directories(nrm_model_registry_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_model_registry_test
   COMMAND nrm_model_registry_test)

add_executable(nrm_environment_config_repository_test EXCLUDE_FROM_ALL
   tests/EnvironmentConfigRepositoryTest.cpp
)
target_include_directories(nrm_environment_config_repository_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_environment_config_repository_test
   COMMAND nrm_environment_config_repository_test)

add_executable(nrm_environment_effect_adapter_test EXCLUDE_FROM_ALL
   tests/EnvironmentEffectAdapterTest.cpp
)
target_include_directories(nrm_environment_effect_adapter_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_environment_effect_adapter_test
   COMMAND nrm_environment_effect_adapter_test)

add_executable(nrm_operational_environment_evaluator_test EXCLUDE_FROM_ALL
   tests/OperationalEnvironmentEvaluatorTest.cpp
)
target_include_directories(nrm_operational_environment_evaluator_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_operational_environment_evaluator_test
   COMMAND nrm_operational_environment_evaluator_test)

add_executable(nrm_afsim_navigation_packet_parser_test EXCLUDE_FROM_ALL
   tests/AfsimNavigationPacketParserTest.cpp
)
target_include_directories(nrm_afsim_navigation_packet_parser_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_afsim_navigation_packet_parser_test
   COMMAND nrm_afsim_navigation_packet_parser_test)

add_executable(nrm_navigation_accuracy_model_test EXCLUDE_FROM_ALL
   tests/NavigationAccuracyModelTest.cpp
)
target_include_directories(nrm_navigation_accuracy_model_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_navigation_accuracy_model_test
   COMMAND nrm_navigation_accuracy_model_test)

add_executable(nrm_recovery_state_store_test EXCLUDE_FROM_ALL
   tests/RecoveryStateStoreTest.cpp
)
target_include_directories(nrm_recovery_state_store_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_recovery_state_store_test
   COMMAND nrm_recovery_state_store_test)

add_executable(nrm_reference_model_adapter_test EXCLUDE_FROM_ALL
   tests/ReferenceModelAdapterTest.cpp
)
target_include_directories(nrm_reference_model_adapter_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_reference_model_adapter_test
   COMMAND nrm_reference_model_adapter_test)

add_executable(nrm_temporary_path_guard_test EXCLUDE_FROM_ALL
   tests/TemporaryPathGuardTest.cpp
)
target_include_directories(nrm_temporary_path_guard_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_temporary_path_guard_test
   COMMAND nrm_temporary_path_guard_test)

add_executable(nrm_customer_state_lifecycle_test EXCLUDE_FROM_ALL
   tests/CustomerStateLifecycleTest.cpp
)
target_include_directories(nrm_customer_state_lifecycle_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_customer_state_lifecycle_test
   COMMAND nrm_customer_state_lifecycle_test)

add_executable(nrm_customer_nrm_adapter_test EXCLUDE_FROM_ALL
   tests/CustomerNrmAdapterTest.cpp
)
target_include_directories(nrm_customer_nrm_adapter_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_customer_nrm_adapter_test
   COMMAND nrm_customer_nrm_adapter_test)

add_executable(nrm_resource_snapshot_validator_test EXCLUDE_FROM_ALL
   tests/ResourceSnapshotValidatorTest.cpp
)
target_include_directories(nrm_resource_snapshot_validator_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_resource_snapshot_validator_test
   COMMAND nrm_resource_snapshot_validator_test)

add_executable(nrm_effective_snapshot_assembler_test EXCLUDE_FROM_ALL
   tests/EffectiveSnapshotAssemblerTest.cpp
)
target_include_directories(nrm_effective_snapshot_assembler_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_effective_snapshot_assembler_test
   COMMAND nrm_effective_snapshot_assembler_test)

find_package(Qt5Core REQUIRED)
add_executable(nrm_plan_scenario_binding_test EXCLUDE_FROM_ALL
   tests/PlanScenarioBindingTest.cpp
   warlock/source/NrmPlanScenarioBinding.cpp
)
target_include_directories(nrm_plan_scenario_binding_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_link_libraries(nrm_plan_scenario_binding_test PRIVATE Qt5::Core)
add_test(NAME nrm_plan_scenario_binding_test
   COMMAND nrm_plan_scenario_binding_test)

add_executable(nrm_customer_json_codec_test EXCLUDE_FROM_ALL
   tests/CustomerJsonCodecTest.cpp
   warlock/source/NrmCustomerJsonCodec.cpp
)
target_include_directories(nrm_customer_json_codec_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_compile_definitions(nrm_customer_json_codec_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(nrm_customer_json_codec_test PRIVATE Qt5::Core)
add_test(NAME nrm_customer_json_codec_test
   COMMAND nrm_customer_json_codec_test)

add_executable(nrm_customer_data_container_test EXCLUDE_FROM_ALL
   tests/CustomerDataContainerTest.cpp
   warlock/source/NrmDataContainer.cpp
   warlock/source/NrmCustomerJsonCodec.cpp
   warlock/source/NrmSnapshotReporter.cpp
)
set_target_properties(nrm_customer_data_container_test PROPERTIES AUTOMOC ON)
target_include_directories(nrm_customer_data_container_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/warlock/source
)
target_compile_definitions(nrm_customer_data_container_test PRIVATE
   NRM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(nrm_customer_data_container_test PRIVATE Qt5::Core)
add_test(NAME nrm_customer_data_container_test
   COMMAND nrm_customer_data_container_test)

add_executable(nrm_navigation_packet_tool EXCLUDE_FROM_ALL
   tools/NrmNavigationPacketTool.cpp
)
target_include_directories(nrm_navigation_packet_tool PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(NRM_TEST_TARGETS
   nrm_contract_metric_enricher_test
   nrm_concurrent_task_assessment_test
   nrm_degradation_policy_test
   nrm_framework_types_test
   nrm_ui_text_test
   nrm_ui_scale_test
   nrm_acceptance_demo_panel_test
   nrm_demand_detail_tabs_test
   nrm_operational_detail_tabs_test
   nrm_operator_presentation_test
   nrm_plan_scenario_binding_test
   nrm_tactical_selection_test
   nrm_snapshot_reporter_test
   nrm_assessment_evaluator_test
   nrm_gateway_policy_engine_test
   nrm_network_profile_test
   nrm_frequency_characteristic_test
   nrm_protocol_resource_model_test
   nrm_message_lifecycle_tracker_test
   nrm_link_metrics_repository_test
   nrm_resource_event_ledger_test
   nrm_constrained_path_selector_test
   nrm_snapshot_reporter_recovery_test
   nrm_communication_capability_service_test
   nrm_network_plan_repository_test
   nrm_network_plan_evaluation_test
   nrm_network_plan_coordination_test
   nrm_resource_demand_repository_test
   nrm_resource_demand_matching_test
   nrm_resource_demand_coordinator_test
   nrm_model_service_facade_test
   nrm_model_registry_test
   nrm_environment_config_repository_test
   nrm_environment_effect_adapter_test
   nrm_operational_environment_evaluator_test
   nrm_afsim_navigation_packet_parser_test
   nrm_navigation_accuracy_model_test
   nrm_recovery_state_store_test
   nrm_reference_model_adapter_test
   nrm_temporary_path_guard_test
   nrm_customer_state_lifecycle_test
   nrm_customer_nrm_adapter_test
   nrm_resource_snapshot_validator_test
   nrm_effective_snapshot_assembler_test
   nrm_customer_json_codec_test
   nrm_customer_data_container_test
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   foreach(NRM_TEST_TARGET IN LISTS NRM_TEST_TARGETS)
      target_compile_options(${NRM_TEST_TARGET} PRIVATE -UNDEBUG)
   endforeach()
endif()
add_custom_target(nrm_tests DEPENDS ${NRM_TEST_TARGETS} nrm_navigation_packet_tool)
