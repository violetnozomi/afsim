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

project(wsf_annotation)

include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/wsf_annotation_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfAnnotationEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)


wsf_project_template(${PROJECT_NAME} PLUGIN
HDRS
   source/WsfAnnotation.hpp
   source/WsfAnnotationDataTypes.hpp
   source/WsfAnnotationEventPipe.hpp
   ${EVENT_PIPE_INCLUDES}
SRCS
   source/WsfAnnotation.cpp
   source/WsfAnnotationEventPipe.cpp
GRAMMARS 
   grammar/wsf_annotation.ag
NOTICES
   README.md
   wsf_annotation_events.utpack
)

