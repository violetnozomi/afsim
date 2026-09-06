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

project(wsf_space)

FILE(GLOB MANEUVERS_HDRS source/maneuvers/*.hpp)
FILE(GLOB MANEUVERS_SRCS source/maneuvers/*.cpp)
FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)
source_group(Maneuvers FILES ${MANEUVERS_HDRS} ${MANEUVERS_SRCS})

# Add in event pipe
include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_space_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfSpaceEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)


wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
   ${MANEUVERS_HDRS}
   ${EVENT_PIPE_INCLUDES}
SRCS
   ${SRCS}
   ${MANEUVERS_SRCS}
GRAMMARS
   grammar/wsf_space.ag
NOTICES
   README.md
   wsf_space_events.utpack
)

#add_subdirectory(test)

# 'wsf_space' is dependent on 'wsf_mil' only until the base signature classes are moved back to 'wsf'
target_link_libraries(${PROJECT_NAME} wsf_mil)

# INSTALL
install_sources_all_files(tools ${_project_install_dir})
install_tests(test ${_project_install_dir})

if(WSF_INSTALL_TRAINING)
   # Training directories included with space build
   set(WSF_SPACE_USER_TRAIN_DIRS
       9_Space
      )
   install_wsf_data("${WSF_SPACE_USER_TRAIN_DIRS}" ${WSF_TRAINING_ROOT}/user training/user Training)
endif()

if(WSF_INSTALL_DEMOS)
   # Demo directories included with space build
   set(WSF_SPACE_DEMO_DIRS
       cislunar
       satellite_demos
       space_operations
      )
   install_wsf_data("${WSF_SPACE_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
