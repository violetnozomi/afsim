# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf)
include(swdev_project)

IF(NOT TOOLS_LIBS)
   MESSAGE(FATAL_ERROR "TOOLS_LIBS must be defined.  Don't build this CMakeLists directly.")
ENDIF()

# A WSF extension should report grammar for new inputs using this macro
# This copies the grammar file to an intermediate directory on build
# and adds the grammar file to a list of all grammar files for installation
macro(wsf_grammar_file SOURCES_VAR GRAMMAR_FILE_PATH)
   get_filename_component(GRAMMAR_FILE_NAME "${GRAMMAR_FILE_PATH}" NAME)
   add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/grammar/${GRAMMAR_FILE_NAME}"
                      COMMAND ${CMAKE_COMMAND} -E copy "${GRAMMAR_FILE_PATH}" "${CMAKE_BINARY_DIR}/grammar/${GRAMMAR_FILE_NAME}"
                      MAIN_DEPENDENCY "${GRAMMAR_FILE_PATH}"
                     )
   set(${SOURCES_VAR} ${${SOURCES_VAR}} ${GRAMMAR_FILE_PATH})
   set_property(GLOBAL APPEND PROPERTY WSF_GRAMMAR_FILES "${GRAMMAR_FILE_PATH}")
endmacro()

# WSF-based projects use this definition to locate files and libraries,
# either wsf/source must be added as a subdirectory first,
# or WSF_LIBS must be defined by the application
if(NOT WSF_LIBS)
   SET(WSF_LIBS wsf ${TOOLS_LIBS} wsf_util PARENT_SCOPE)
endif()

# Set the wsf source directory for other projects to use
# as a method to add/include other files
set(wsf_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source PARENT_SCOPE)

# Configuration for WsfEventPipe.
add_subdirectory(../wsf_util/ut_pack/pack_to_cpp pack_to_cpp)

add_subdirectory(source)
#add_subdirectory(test)

# Add some configuration files to the project for ease of editing
set(CONFIGURATION_FILES CMakeLists.txt optional_projects.cmake)
source_group(configuration FILES ${CONFIGURATION_FILES})

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})
add_wsf_doc_sidebar_ext(${CMAKE_CURRENT_SOURCE_DIR}/wsftheme/wsf_sidebar_core.html)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)
add_wsf_doxygen_exclude("${CMAKE_CURRENT_SOURCE_DIR}/source/WsfEventPipe.cpp ${CMAKE_CURRENT_SOURCE_DIR}/source/WsfEventPipe.hpp")

# Setup install commands for release. Includes WSF source and support
# directories (e.g. tests) under 'wsf'. Calls a swdev_project.cmake macros
install_sources(source core/${PROJECT_NAME})
install_sources_all_files(doc core/${PROJECT_NAME})
install_sources_all_files(event_pipe core/${PROJECT_NAME})
install_sources_all_files(grammar core/${PROJECT_NAME})
install_sources_all_files(test core/${PROJECT_NAME})
install_sources_all_files(tools core/${PROJECT_NAME})
install_sources_all_files(wsftheme core/${PROJECT_NAME})
install_tests(test_mission core/${PROJECT_NAME})

if(WSF_INSTALL_DEMOS)
   set(WSF_DEMO_DIRS
       comm
      )
   install_wsf_data("${WSF_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()

# Install a few files and the misc directory that are required to build WSF but not covered elsewhere
install_source_files(CMakeLists.txt
                     event_pipe.cmake
                     optional_projects.cmake
                     README.md
                     wsf_extensions.hpp.in
                     core/wsf
                    )
install_source_files(../wsf_module core)
