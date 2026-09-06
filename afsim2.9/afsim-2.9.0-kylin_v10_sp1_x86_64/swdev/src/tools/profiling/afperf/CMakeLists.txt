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

set(library_name afperf)

add_library(${library_name} SHARED
   afperf.cpp
   "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingCsvUtil.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingTimeUtil.cpp"
   # impl headers included in order for headers to be present in IDE folders.
   "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingCsvUtil.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingTimeUtil.hpp"
)
set_property(TARGET ${library_name} PROPERTY FOLDER "tools/profiling")
generate_export_header(${library_name})
target_include_directories(${library_name}
   PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../source"
           "${CMAKE_CURRENT_LIST_DIR}/../common"
           "${CMAKE_CURRENT_LIST_DIR}/../impl"
           "${PROJECT_BINARY_DIR}/${library_name}"
   )
target_link_libraries(${library_name} util)
swdev_lib_install(${library_name})