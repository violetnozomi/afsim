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

project(profiling_test_version_greater_library)
include(GenerateExportHeader)
add_library(${PROJECT_NAME} SHARED version_greater_library.cpp)
target_include_directories(${PROJECT_NAME}
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   PRIVATE
   "${CMAKE_CURRENT_LIST_DIR}/../../common")
target_link_libraries(${PROJECT_NAME} util)
generate_export_header(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "UnitTests/profiling")
