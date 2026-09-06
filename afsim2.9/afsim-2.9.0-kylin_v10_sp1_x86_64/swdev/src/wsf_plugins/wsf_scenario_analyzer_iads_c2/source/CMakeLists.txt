# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
if(NOT BUILD_WITH_wsf_iads_c2 OR NOT BUILD_WITH_wsf_scenario_analyzer)
   message(SEND_ERROR "wsf_scenario_analyzer_iads_c2 requires BUILD_WITH_wsf_iads_c2 AND BUILD_WITH_wsf_scenario_analyzer")
endif()
   
include(GenerateExportHeader)
include(swdev_project)

file(GLOB SRCS *.?pp)

add_library(${PROJECT_NAME} ${SRCS})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${PROJECT_BINARY_DIR}/source")
target_link_libraries(${PROJECT_NAME} util util_script wsf wsf_mil wsf_scenario_analyzer_lib wsf_iads_c2_lib)

if(WSF_PLUGIN_BUILD)
   swdev_plugin_install(${PROJECT_NAME} mission_plugins)
else()
   swdev_lib_install(${PROJECT_NAME})
endif()
swdev_warning_level(${PROJECT_NAME})

FILE(GLOB check_suite_files "../data/check_suites/*.txt")

# After building, install SAT checks and session_notes to a directory where they'll
# be found when debugging from Visual Studio.
set(DATA_DIR wsf_scenario_analyzer_data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		  COMMAND ${CMAKE_COMMAND} -E copy ${check_suite_files} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}/check_suites VERBATIM
		  )

install(
   FILES ${check_suite_files}
   DESTINATION resources/data/wsf_scenario_analyzer/check_suites COMPONENT Resource)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_scenario_analyzer")
