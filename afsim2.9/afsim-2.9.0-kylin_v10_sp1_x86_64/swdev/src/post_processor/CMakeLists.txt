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

project(post_processor)
include(swdev_project)

add_subdirectory(lib/source post_processor_lib)
#add_subdirectory(lib/test)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/lib/source)

add_subdirectory(exec/source)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "applications")

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/exec/source)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/lib/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/exec)

install_sources_all_files(. ${PROJECT_NAME})
