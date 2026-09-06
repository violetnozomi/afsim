# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_oms_uci)
include(swdev_project)

add_subdirectory(source)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_plugins")

# INSTALL
install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources_all_files(data wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})
install_sources_all_files(grammar wsf_plugins/${PROJECT_NAME})
install_sources_all_files(lib wsf_plugins/${PROJECT_NAME})
install_tests(test_mission wsf_plugins/${PROJECT_NAME})
install_source_files(CMakeLists.txt LICENSE.md NO_EXPORT.md README.md wsf_cmake_extension.cmake wsf_module
                     wsf_plugins/${PROJECT_NAME})
install_source_files(wsf_module wsf_plugins)
