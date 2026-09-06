# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_air_combat)

if (NOT BUILD_WITH_wsf_six_dof OR NOT BUILD_WITH_wsf_p6dof OR NOT BUILD_WITH_wsf_brawler)
   message(FATAL_ERROR "wsf_air_combat requires wsf_six_dof, wsf_p6dof, and wsf_brawler to be enabled")
endif()

include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_air_combat_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfAirCombatEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} PLUGIN
HDRS
   ${HDRS}
   ${EVENT_PIPE_INCLUDES}
SRCS
   ${SRCS}
GRAMMARS 
   grammar/wsf_air_combat.ag
NOTICES
   NO_EXPORT.md
   README.md
   wsf_air_combat_events.utpack
)

target_link_libraries(${PROJECT_NAME} wsf_mil wsf_p6dof wsf_six_dof wsf_brawler)

# Unit tests
#add_subdirectory(test)
install_sources(test ${_project_install_dir})

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the air combat build
   set(AIR_TO_AIR_DEMO_DIRS
       air_to_air
      )
   install_wsf_data("${AIR_TO_AIR_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
