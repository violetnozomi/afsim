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
set(WSF_EVENT_PACK_FILE ${WSF_ROOT}/wsf/event_pipe/wsf.utpack)
set(WSF_ANNOTATION_EVENT_PACK_FILE ${WSF_ROOT}/../wsf_plugins/wsf_annotation/wsf_annotation_events.utpack)
add_event_pipe_schema("${WSF_ANNOTATION_EVENT_PACK_FILE}"
                      "${WSF_EVENT_PACK_FILE}"
                      "rv::MessageBase"
                      "RvEventPipeClasses.hpp"
                      "rv"
                      "RvAnnotationEventPipe"
                      ANNOTATION_EVENT_PIPE_INCLUDES
                      ANNOTATION_EVENT_PIPE_SOURCES)
SOURCE_GROUP(utpack_gen FILES ${ANNOTATION_EVENT_PIPE_INCLUDES} ${ANNOTATION_EVENT_PIPE_SOURCES})
SOURCE_GROUP(utpack FILES ${WSF_ANNOTATION_EVENT_PACK_FILE})

plugin_cmakelists_template(ResultDataAnnotation mystic
                           ADDITIONAL_SOURCES ${ANNOTATION_EVENT_PIPE_INCLUDES} ${ANNOTATION_EVENT_PIPE_SOURCES} ${WSF_ANNOTATION_EVENT_PACK_FILE}
                           LIBRARIES mystic_lib)
