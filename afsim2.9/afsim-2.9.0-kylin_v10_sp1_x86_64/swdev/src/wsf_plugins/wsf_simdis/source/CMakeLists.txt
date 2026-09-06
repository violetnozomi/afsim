# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 - 2019 Lockheed Martin Corporation. All Rights Reserved.
# Lockheed Martin Corporation hereby provides the below source code to the
# United States Government with Unlimited Rights as such is defined in
# DFARS 252.227 - 7014.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include (GenerateExportHeader)
include(swdev_project)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

FILE(GLOB SRCS *.?pp)

wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../grammar/wsf_simdis.ag")

include_directories(${PROJECT_BINARY_DIR}/source)

add_library(${PROJECT_NAME} ${SRCS})
swdev_warning_level(${PROJECT_NAME})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}/source")
target_link_libraries(${PROJECT_NAME} wsf wsf_mil util)
swdev_lib_install(${PROJECT_NAME})
