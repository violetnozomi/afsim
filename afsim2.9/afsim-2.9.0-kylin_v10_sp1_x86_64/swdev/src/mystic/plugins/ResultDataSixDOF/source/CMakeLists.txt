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
set(WSF_SIX_DOF_EVENT_PACK_FILE ${WSF_ROOT}/../wsf_plugins/wsf_six_dof/wsf_six_dof_events.utpack)
add_event_pipe_schema("${WSF_SIX_DOF_EVENT_PACK_FILE}"
                      "${WSF_EVENT_PACK_FILE}"
                      "rv::MessageBase"
                      "RvEventPipeClasses.hpp"
                      "rv"
                      "RvSixDOF_EventPipe"
                      SIX_DOF_EVENT_PIPE_INCLUDES
                      SIX_DOF_EVENT_PIPE_SRCS)
SOURCE_GROUP(utpack_gen FILES ${SIX_DOF_EVENT_PIPE_INCLUDES} ${SIX_DOF_EVENT_PIPE_SRCS})
SOURCE_GROUP(utpack FILES ${WSF_SIX_DOF_EVENT_PACK_FILE})

plugin_cmakelists_template(ResultDataSixDOF mystic
                           ADDITIONAL_SOURCES ${SIX_DOF_EVENT_PIPE_INCLUDES} ${SIX_DOF_EVENT_PIPE_SRCS} ${WSF_SIX_DOF_EVENT_PACK_FILE}
                           LIBRARIES mystic_lib)
