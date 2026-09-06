# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_parser)

FILE(GLOB HDRS source/*.hpp source/grammar_parse/*.h)
FILE(GLOB SRCS source/*.cpp source/grammar_parse/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
STANDALONE
HDRS
   ${HDRS}
SRCS
   ${SRCS}
NOTICES
   README.md
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/grammar_parse)
target_link_libraries(${PROJECT_NAME} util wsf_util)

#add_subdirectory(test)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/grammar_parse)

# INSTALL
install_sources_all_files(source/grammar_parse ${_project_install_dir}/source)
install_sources_all_files(legacy_test ${_project_install_dir})
install_sources_all_files(scripts ${_project_install_dir})
install_sources(test ${_project_install_dir})
