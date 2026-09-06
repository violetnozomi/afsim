# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#
# WSF CMake Configuration
#
# This file creates the mission executable

project(mission)

# Set the shared library "paths" before calling "add_executable"
# The actual lib names can just be added, in the modules cmake file,
# using 'target_link_libraries'
if (COMMAND set_3rd_party_shared_paths)
   set_3rd_party_shared_paths()
endif()

wsf_project_template(${PROJECT_NAME} EXECUTABLE
HDRS
   ${WSF_EXTENSION_HEADER_DIR}/wsf_extensions.hpp
SRCS
   source/mission.cpp
NOTICES
   README.md
)

# *****************************************************************************
# Configure version header and resource files
include(GenerateVersionInfo)
generate_version_header(${PROJECT_NAME})
if(MSVC)
   generate_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/source/mission_icon.ico")
endif()

link_wsf_optional_projects(${PROJECT_NAME})
link_sockets(${PROJECT_NAME})

# Install all WSF grammar to get "main" grammar file and any files defining
# commands that may be used by mission but not included in mission.ag
get_property(WSF_GRAMMAR_FILES GLOBAL PROPERTY WSF_GRAMMAR_FILES)
install(FILES ${WSF_GRAMMAR_FILES} DESTINATION bin/${PROJECT_NAME}_grammar COMPONENT Runtime)
# Install mission specific grammar
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/grammar/mission.ag DESTINATION bin/${PROJECT_NAME}_grammar COMPONENT Runtime)

install_tests(tests ${PROJECT_NAME})

# Generate this applications AUTO_TEST target.
generate_wsf_auto_test(${PROJECT_NAME})

# Generate this applications REGRESSION_TEST target.
generate_wsf_regression_test(${PROJECT_NAME})
