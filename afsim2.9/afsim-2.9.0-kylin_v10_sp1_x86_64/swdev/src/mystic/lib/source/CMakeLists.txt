# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# *****************************************************************************
# CMAKE file: mystic_lib
# *****************************************************************************
project(mystic_lib)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

# *****************************************************************************
# Get the source files
file(GLOB SRCS *.?pp *.hpp)
file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)
FILE(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.ui)

include(qt_project)
configure_qt_project(UI UI_HEADERS ${UIS})

# *****************************************************************************
# Event Pipe
include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(WSF_EVENT_PACK_FILE ${WSF_ROOT}/wsf/event_pipe/wsf.utpack)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
set(MIL_EVENT_PIPE_INCLUDES)
set(MIL_EVENT_PIPE_SOURCES)
add_event_pipe_schema("${WSF_EVENT_PACK_FILE}"
                       ""
                       "rv::MessageBase"
                       "RvEventPipeMessages.hpp"
                       "rv"
                       "RvEventPipe"
                       EVENT_PIPE_INCLUDES
                       EVENT_PIPE_SOURCES)
set(WSF_MIL_EVENT_PACK_FILE ${WSF_ROOT}/wsf_mil/wsf_mil_events.utpack)
add_event_pipe_schema("${WSF_MIL_EVENT_PACK_FILE}"
                      "${WSF_EVENT_PACK_FILE}"
                      "rv::MessageBase"
                      "RvEventPipeClasses.hpp"
                      "rv"
                      "RvMilEventPipe"
                      MIL_EVENT_PIPE_INCLUDES
                      MIL_EVENT_PIPE_SOURCES)

SOURCE_GROUP(utpack_gen FILES ${EVENT_PIPE_INCLUDES} ${EVENT_PIPE_SOURCES})
SOURCE_GROUP(utpack_gen FILES ${MIL_EVENT_PIPE_INCLUDES} ${MIL_EVENT_PIPE_SOURCES})

# *****************************************************************************
# The executable we are trying to build
add_library(${PROJECT_NAME}
            ${SRCS}
            ${EVENT_PIPE_INCLUDES}
            ${EVENT_PIPE_SOURCES}
            ${MIL_EVENT_PIPE_INCLUDES}
            ${MIL_EVENT_PIPE_SOURCES}
            ${UI_HEADERS}
            ${QRC}
           )
           
# *****************************************************************************
# Libraries
swdev_warning_level(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} wkf wkf_common util wsf_util)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${WSF_EXTENSION_HEADER_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR}
                          )
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "mystic")

# *****************************************************************************
# Installation
swdev_lib_install(${PROJECT_NAME})

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)

install_sources_all_files(../ mystic/lib)
