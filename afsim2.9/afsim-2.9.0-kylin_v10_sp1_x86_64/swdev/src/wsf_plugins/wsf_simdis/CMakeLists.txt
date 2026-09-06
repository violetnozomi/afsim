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

project(wsf_simdis)
include(swdev_project)

add_subdirectory(source)

# Add project to Doxygen for code documentation
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# INSTALL
install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources_all_files(grammar wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})

install_source_files(../LICENSE.md
                     ../NO_EXPORT.md
                     ../README.md
                     CMakeLists.txt
                     wsf_module
                     wsf_cmake_extension.cmake
                     wsf_plugins/${PROJECT_NAME})
install_source_files(wsf_module wsf_plugins)
