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

project(wsf_p6dof)

FILE(GLOB MANEUVER_SRCS source/maneuvers/*.cpp)
FILE(GLOB MANEUVER_HDRS source/maneuvers/*.hpp)
FILE(GLOB FORMATION_SRCS source/formations/*.cpp)
FILE(GLOB FORMATION_HDRS source/formations/*.hpp)
FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

source_group(Manuevers FILES ${MANEUVER_SRCS} ${MANEUVER_HDRS})
source_group(Formations FILES ${FORMATION_SRCS} ${FORMATION_HDRS})

include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_p6dof_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfP6dofEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)
                      
wsf_project_template(${PROJECT_NAME} PLUGIN
SUB_PROJECTS
   p6dof
HDRS
   ${HDRS}
   ${MANEUVER_HDRS}
   ${FORMATION_HDRS}
   ${EVENT_PIPE_INCLUDES}

SRCS
   ${SRCS}
   ${MANEUVER_SRCS}
   ${FORMATION_SRCS}
GRAMMARS
   grammar/wsf_p6dof.ag
NOTICES
   README.md
   wsf_p6dof_events.utpack
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the p6dof build
   set(P6DOF_DEMO_DIRS
       p6dof
      )
   install_wsf_data("${P6DOF_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
