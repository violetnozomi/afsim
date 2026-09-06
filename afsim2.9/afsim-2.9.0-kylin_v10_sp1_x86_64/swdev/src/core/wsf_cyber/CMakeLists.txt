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

project(wsf_cyber)

file(GLOB EFFECTS_HDRS source/effects/*.hpp)
file(GLOB EFFECTS_SRCS source/effects/*.cpp)
file(GLOB TRIGGERS_HDRS source/triggers/*.hpp)
file(GLOB TRIGGERS_SRCS source/triggers/*.cpp)
file(GLOB HDRS source/*.hpp)
file(GLOB SRCS source/*.cpp)

source_group(Effects FILES ${EFFECTS_HDRS} ${EFFECTS_SRCS})
source_group(Triggers FILES ${TRIGGERS_HDRS} ${TRIGGERS_SRCS})

# Add in event pipe
include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_cyber_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfCyberEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
   ${EFFECTS_HDRS}
   ${TRIGGERS_HDRS}
   ${EVENT_PIPE_INCLUDES}
SRCS
   ${SRCS}
   ${EFFECTS_SRCS}
   ${TRIGGERS_SRCS}
GRAMMARS
   grammar/wsf_cyber.ag
NOTICES
   README.md
   wsf_cyber_events.utpack
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

#add_subdirectory(test)

# INSTALL
install_sources(test ${_project_install_dir})

if (WSF_INSTALL_DEMOS)
   install_wsf_data(cyber ${WSF_DEMOS_ROOT} demos Demo)
endif()
