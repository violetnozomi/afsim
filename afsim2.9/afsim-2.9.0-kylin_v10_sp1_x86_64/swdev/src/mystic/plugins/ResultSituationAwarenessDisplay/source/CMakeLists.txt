# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
try_add_subdirectory(${TOOLS_DIRECTORY}/wkf/air_combat/wkf_air_combat_common/source wkf_air_combat_common)
plugin_cmakelists_template(ResultSituationAwarenessDisplay mystic
                           REQUIRES ResultDataAirCombat
                           LIBRARIES mystic_lib wkf_air_combat_common
                           THIRD_PARTY_INCLUDE_DIRS osg)
