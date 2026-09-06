# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
include(GenerateExportHeader)

set(library_name artificer)

file(GLOB SRCS *.cpp *.hpp)
add_library(${library_name} SHARED ${SRCS})

generate_export_header(${library_name})
swdev_warning_level(${library_name})
target_include_directories(${library_name} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set_property(TARGET ${library_name} PROPERTY FOLDER "tools/artificer")
target_link_libraries(${library_name} util)
swdev_lib_install(${library_name})
