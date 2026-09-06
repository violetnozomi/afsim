# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include (GenerateExportHeader)
include(swdev_project)
project(post_processor_lib)

file(GLOB SRCS *.?pp)

add_library(${PROJECT_NAME} STATIC ${SRCS})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${PROJECT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} util)
swdev_warning_level(${PROJECT_NAME})
swdev_lib_install(${PROJECT_NAME})