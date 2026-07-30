project(wsf_network_resource_manager)

wsf_project_template(${PROJECT_NAME} PLUGIN
   HDRS
      include/nrm/NetworkResourceTypes.hpp
      include/nrm/InputProvider.hpp
      include/nrm/NetworkTypeUtils.hpp
      include/nrm/RollingMetrics.hpp
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
