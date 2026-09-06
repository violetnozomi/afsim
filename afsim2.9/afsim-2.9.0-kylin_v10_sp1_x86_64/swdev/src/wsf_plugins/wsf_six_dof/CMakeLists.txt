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

project(wsf_six_dof)

include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_six_dof_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfSixDOF_EventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)


FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)
FILE(GLOB MANEUVER_HDRS source/maneuvers/*.hpp)
FILE(GLOB MANEUVER_SRCS source/maneuvers/*.cpp)
FILE(GLOB FORMATION_HDRS source/formations/*.hpp)
FILE(GLOB FORMATION_SRCS source/formations/*.cpp)
source_group(Maneuvers FILES ${MANEUVER_HDRS} ${MANEUVER_SRCS})
source_group(Formations FILES ${FORMATION_HDRS} ${FORMATION_SRCS})


wsf_project_template(${PROJECT_NAME} PLUGIN
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
   grammar/wsf_six_dof.ag
NOTICES
   README.md
   wsf_six_dof_events.utpack
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

#add_subdirectory(test)
install_sources(test ${_project_install_dir})

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the plugin build
   set(SIX_DOF_DEMO_DIRS
       six_dof
      )
   if (BUILD_WITH_wsf_brawler)
      set(SIX_DOF_DEMO_DIRS ${SIX_DOF_DEMO_DIRS} six_dof_with_brawler)
   endif()
   install_wsf_data("${SIX_DOF_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
