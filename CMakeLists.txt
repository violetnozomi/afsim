project(wsf_network_resource_manager)

wsf_project_template(${PROJECT_NAME} PLUGIN
   HDRS
      include/nrm/NetworkResourceTypes.hpp
      include/nrm/AssessmentTypes.hpp
      include/nrm/AssessmentEvaluator.hpp
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

set(NRM_TEST_TARGETS
   nrm_framework_types_test
   nrm_snapshot_reporter_test
   nrm_assessment_evaluator_test
   nrm_network_profile_test
   nrm_message_lifecycle_tracker_test
   nrm_resource_event_ledger_test
   nrm_constrained_path_selector_test
   nrm_snapshot_reporter_recovery_test
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   foreach(NRM_TEST_TARGET IN LISTS NRM_TEST_TARGETS)
      target_compile_options(${NRM_TEST_TARGET} PRIVATE -UNDEBUG)
   endforeach()
endif()
add_custom_target(nrm_tests DEPENDS ${NRM_TEST_TARGETS})
