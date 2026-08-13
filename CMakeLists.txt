project(wsf_network_resource_manager)

wsf_project_template(${PROJECT_NAME} PLUGIN
   HDRS
      include/nrm/NetworkResourceTypes.hpp
      include/nrm/AssessmentTypes.hpp
      include/nrm/AssessmentEvaluator.hpp
      include/nrm/ConcurrentTaskAssessment.hpp
      include/nrm/DegradationPolicy.hpp
      include/nrm/ContractMetricEnricher.hpp
      include/nrm/InputProvider.hpp
      include/nrm/NetworkTypeUtils.hpp
      include/nrm/RollingMetrics.hpp
      include/nrm/MetricReason.hpp
      include/nrm/MessageLifecycleTypes.hpp
      include/nrm/MessageLifecycleTracker.hpp
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
      include/nrm/NetworkPlanValidator.hpp
      include/nrm/NetworkPlanEvaluationService.hpp
      include/nrm/NetworkPlanSerialization.hpp
      include/nrm/NetworkPlanDistributionService.hpp
      include/nrm/ResourceDemandTypes.hpp
      include/nrm/ResourceDemandRepository.hpp
      include/nrm/PlanningRecommendationEngine.hpp
      include/nrm/ResourceDemandMatchingService.hpp
      include/nrm/ModelServiceTypes.hpp
      include/nrm/ModelServiceFacade.hpp
      include/nrm/ModelRegistry.hpp
      include/nrm/ContractInterfaceAdapter.hpp
      include/nrm/Version.hpp
      source/WsfNetworkResourceManagerPlugin.hpp
   SRCS
      source/WsfNetworkResourceManagerPlugin.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(nrm_framework_types_test EXCLUDE_FROM_ALL
   tests/NetworkResourceTypesTest.cpp
)
target_include_directories(nrm_framework_types_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_framework_types_test COMMAND nrm_framework_types_test)

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

add_executable(nrm_message_lifecycle_tracker_test EXCLUDE_FROM_ALL
   tests/MessageLifecycleTrackerTest.cpp
)
target_include_directories(nrm_message_lifecycle_tracker_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_message_lifecycle_tracker_test COMMAND nrm_message_lifecycle_tracker_test)

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

add_executable(nrm_resource_demand_repository_test EXCLUDE_FROM_ALL
   tests/ResourceDemandRepositoryTest.cpp
)
target_include_directories(nrm_resource_demand_repository_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
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

add_executable(nrm_afsim_navigation_packet_parser_test EXCLUDE_FROM_ALL
   tests/AfsimNavigationPacketParserTest.cpp
)
target_include_directories(nrm_afsim_navigation_packet_parser_test PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_test(NAME nrm_afsim_navigation_packet_parser_test
   COMMAND nrm_afsim_navigation_packet_parser_test)

find_package(Qt5Core REQUIRED)
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
   nrm_snapshot_reporter_test
   nrm_assessment_evaluator_test
   nrm_network_profile_test
   nrm_message_lifecycle_tracker_test
   nrm_resource_event_ledger_test
   nrm_constrained_path_selector_test
   nrm_snapshot_reporter_recovery_test
   nrm_communication_capability_service_test
   nrm_network_plan_repository_test
   nrm_network_plan_evaluation_test
   nrm_resource_demand_repository_test
   nrm_resource_demand_matching_test
   nrm_model_service_facade_test
   nrm_model_registry_test
   nrm_environment_config_repository_test
   nrm_environment_effect_adapter_test
   nrm_afsim_navigation_packet_parser_test
   nrm_customer_json_codec_test
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   foreach(NRM_TEST_TARGET IN LISTS NRM_TEST_TARGETS)
      target_compile_options(${NRM_TEST_TARGET} PRIVATE -UNDEBUG)
   endforeach()
endif()
add_custom_target(nrm_tests DEPENDS ${NRM_TEST_TARGETS} nrm_navigation_packet_tool)
