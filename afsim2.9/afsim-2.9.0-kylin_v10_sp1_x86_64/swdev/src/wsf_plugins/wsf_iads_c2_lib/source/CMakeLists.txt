# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include (GenerateExportHeader)
include(swdev_project)

FILE(GLOB SRCS *.?pp)
FILE(GLOB PLUGIN_SRCS WsfIADSC2UnclassProcRegistration.?pp)
list(REMOVE_ITEM SRCS ${PLUGIN_SRCS})

wsf_grammar_file(SRCS "${CMAKE_CURRENT_LIST_DIR}/../grammar/wsf_iads_c2_unclass.ag")

include_directories(${PROJECT_BINARY_DIR}/source)

add_library(${PROJECT_NAME} STATIC ${SRCS})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}/source")
target_link_libraries(${PROJECT_NAME} wsf wsf_mil util util_script iadsLib)
swdev_warning_level(${PROJECT_NAME})
#swdev_lib_install(${PROJECT_NAME})

add_library(${PROJECT_PLUGIN_NAME} ${PLUGIN_SRCS})
generate_export_header(${PROJECT_PLUGIN_NAME})
target_include_directories(${PROJECT_PLUGIN_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}/source")
target_link_libraries(${PROJECT_PLUGIN_NAME} ${PROJECT_NAME})

if(WSF_PLUGIN_BUILD)
   swdev_plugin_install(${PROJECT_PLUGIN_NAME} mission_plugins)
else()
   swdev_lib_install(${PROJECT_PLUGIN_NAME})
endif()
swdev_warning_level(${PROJECT_PLUGIN_NAME})
