# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_iads_c2_lib)
set(PROJECT_PLUGIN_NAME "wsf_iads_c2")
include(swdev_project)

add_subdirectory(iadsLib iadsLib)
set_property(TARGET iadsLib PROPERTY FOLDER "wsf_iads_c2")

add_subdirectory(source)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_iads_c2")
set_property(TARGET ${PROJECT_PLUGIN_NAME} PROPERTY FOLDER "wsf_iads_c2")

# Add source directories to doxygen input
add_wsf_doxygen_input(${CMAKE_CURRENT_LIST_DIR}/source)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/doc)

# INSTALL
install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources_all_files(grammar wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})
install_sources_all_files(iadsLib wsf_plugins/${PROJECT_NAME})
install_sources_all_files(test wsf_plugins/${PROJECT_NAME})
install_source_files(CMakeLists.txt
                     LICENSE.md
                     NO_EXPORT.md
                     README.md
                     wsf_cmake_extension.cmake
                     wsf_module
                     wsf_plugins/${PROJECT_NAME})
install_source_files(wsf_module wsf_plugins)

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the IADS C2 build
   set(IADS_C2_DEMO_DIRS
         demos
      )
   install_wsf_data("${IADS_C2_DEMO_DIRS}/" ${CMAKE_CURRENT_SOURCE_DIR} demos/iads_c2_demos Demo)
endif()
