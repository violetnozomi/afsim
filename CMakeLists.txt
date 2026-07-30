project(wsf_network_resource_manager)

wsf_project_template(${PROJECT_NAME} PLUGIN
   HDRS
      include/nrm/NetworkResourceTypes.hpp
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
