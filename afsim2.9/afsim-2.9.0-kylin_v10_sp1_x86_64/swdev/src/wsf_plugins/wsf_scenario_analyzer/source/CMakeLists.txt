# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
include(GenerateExportHeader)
include(swdev_project)

set(PROJECT_STATIC_LIB_NAME ${PROJECT_NAME}_lib)
# if(NOT BUILD_WIZARD_PLUGIN_WizScenarioAnalyzer)
   # message(WARNING "wsf_scenario_analyzer will be built but will not be available in Wizard because WizScenarioAnalyzer is not being built. Select BUILD_WIZARD_PLUGIN_ViPluginScenarioAnalyzer to fix this issue.")
# endif()

# Grab all source except plugin registration (Avoid editing this every time a new file is added)
FILE(GLOB SRCS *.?pp)
LIST(REMOVE_ITEM SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ScenarioAnalyzerPluginRegistration.cpp)

include_directories(${PROJECT_BINARY_DIR}/source)

add_library(${PROJECT_STATIC_LIB_NAME} STATIC ${SRCS})
swdev_warning_level(${PROJECT_STATIC_LIB_NAME})
generate_export_header(${PROJECT_STATIC_LIB_NAME})
target_include_directories(${PROJECT_STATIC_LIB_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${PROJECT_BINARY_DIR}/source")
target_link_libraries(${PROJECT_STATIC_LIB_NAME} util util_script wsf wsf_mil)


add_library(${PROJECT_NAME} ScenarioAnalyzerPluginRegistration.cpp)
generate_export_header(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${PROJECT_STATIC_LIB_NAME})

FILE(GLOB check_suite_files "../data/check_suites/*.txt")
FILE(GLOB session_note_suite_files "../data/session_note_suites/*.txt")

# After building, install SAT checks and session_notes to a directory where they'll
# be found when debugging from Visual Studio.
set(DATA_DIR wsf_scenario_analyzer_data)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}" VERBATIM
				   COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}/check_suites" VERBATIM
				   COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}/session_note_suites" VERBATIM
				   )
				   
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy ${check_suite_files} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}/check_suites VERBATIM
				   COMMAND ${CMAKE_COMMAND} -E copy ${session_note_suite_files} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR}/session_note_suites VERBATIM
				   )
if(WSF_PLUGIN_BUILD)
   swdev_plugin_install(${PROJECT_NAME} mission_plugins)
else()
   swdev_lib_install(${PROJECT_NAME})
endif()
swdev_warning_level(${PROJECT_NAME})

install(
   FILES ${check_suite_files}
   DESTINATION resources/data/wsf_scenario_analyzer/check_suites COMPONENT Resource)

install(
   FILES ${session_note_suite_files}
   DESTINATION resources/data/wsf_scenario_analyzer/session_note_suites COMPONENT Resource)
                  
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_scenario_analyzer")
set_property(TARGET ${PROJECT_STATIC_LIB_NAME} PROPERTY FOLDER "wsf_scenario_analyzer")
