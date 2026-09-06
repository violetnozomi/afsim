# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_scenario_analyzer)
include(swdev_project)

add_subdirectory(source)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# INSTALL
install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources(data wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})
install_sources_all_files(data wsf_plugins/${PROJECT_NAME})
install_source_files(CMakeLists.txt
                     README.md
                     wsf_cmake_extension.cmake
                     wsf_module
                     wsf_plugins/${PROJECT_NAME})
install_source_files(wsf_module wsf_plugins)

