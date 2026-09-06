# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_coverage)
include(swdev_project)

include_directories(include ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/include)
add_subdirectory(source)
#add_subdirectory(test)

# Add source directories to doxygen input
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# INSTALL
install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources_all_files(grammar wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})
install_sources(test wsf_plugins/${PROJECT_NAME})
install_tests(test_mission wsf_plugins/${PROJECT_NAME})
install_source_files(CMakeLists.txt
                     README.md
                     wsf_module
                     wsf_cmake_extension.cmake
                     wsf_plugins/${PROJECT_NAME})

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the wsf_coverage build
   set(COVERAGE_DEMO_DIRS
       coverage_demos
      )
   install_wsf_data("${COVERAGE_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
