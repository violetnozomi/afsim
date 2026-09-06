# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
try_add_subdirectory(${TOOLS_DIRECTORY}/wkf/comm_vis/wkf_comm_vis_common/source wkf_comm_vis_common)
plugin_cmakelists_template(ResultCommVis mystic
                           LIBRARIES mystic_lib wkf_comm_vis_common)
