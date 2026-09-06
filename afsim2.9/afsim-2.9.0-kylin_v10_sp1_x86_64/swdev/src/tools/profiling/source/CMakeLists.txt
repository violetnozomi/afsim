# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(GenerateExportHeader)
include(swdev_project)

# Common directory files added to GLOB in order for headers to be present in IDE folders.
FILE(GLOB SRCS *.cpp *.hpp ../common/*.hpp)

configure_file(ProfilingMacros.hpp.in ProfilingMacros.hpp)

add_library(${PROJECT_NAME} ${SRCS})
# There is no direct dependency on afperf, but for the profiling system to be useful it needs to
# at least have one profiling library implementation available.
add_dependencies(${PROJECT_NAME} afperf)

generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/profiling")
target_include_directories(${PROJECT_NAME}
   PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${PROJECT_BINARY_DIR}/source"
   "${CMAKE_CURRENT_LIST_DIR}/../common")
target_link_libraries(${PROJECT_NAME} util)
swdev_warning_level(${PROJECT_NAME})
swdev_lib_install(${PROJECT_NAME})
