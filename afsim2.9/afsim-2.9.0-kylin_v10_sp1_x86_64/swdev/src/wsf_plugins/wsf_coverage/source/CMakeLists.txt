# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(GenerateExportHeader)
include(swdev_project)

if(MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

FILE(GLOB SRCS *.cpp *.hpp)

wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../grammar/wsf_coverage.ag")

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/source)

add_library(${PROJECT_NAME} ${SRCS})
swdev_warning_level(${PROJECT_NAME})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${WSF_COVERAGE_INCLUDES}
   ${PROJECT_BINARY_DIR}/source)
target_link_libraries(${PROJECT_NAME} ${WSF_LIBS})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_plugins")

if(WSF_PLUGIN_BUILD)
   swdev_plugin_install(${PROJECT_NAME} mission_plugins)
else()
   swdev_lib_install(${PROJECT_NAME})
endif()
