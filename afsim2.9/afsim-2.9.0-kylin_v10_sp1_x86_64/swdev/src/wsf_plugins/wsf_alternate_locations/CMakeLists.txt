# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_alternate_locations)
include(GenerateExportHeader)
include(swdev_project)

file(GLOB SRCS source/*.?pp)

add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/doc)
wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/grammar/wsf_alternate_locations.ag")

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${WSF_LIBS})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_plugins")

swdev_plugin_install(${PROJECT_NAME} mission_plugins)
swdev_warning_level(${PROJECT_NAME})
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source" "${CMAKE_CURRENT_BINARY_DIR}")

install_sources(source wsf_plugins/${PROJECT_NAME})
install_sources_all_files(grammar wsf_plugins/${PROJECT_NAME})
install_sources_all_files(doc wsf_plugins/${PROJECT_NAME})
install_source_files(README.md
                     wsf_module
                     wsf_cmake_extension.cmake
                     CMakeLists.txt
                     wsf_plugins/${PROJECT_NAME})
install_source_files(wsf_module wsf_plugins)

if (WSF_INSTALL_DEMOS)
   install_wsf_data(alternate_locations ${WSF_DEMOS_ROOT} demos Demo)
endif()
