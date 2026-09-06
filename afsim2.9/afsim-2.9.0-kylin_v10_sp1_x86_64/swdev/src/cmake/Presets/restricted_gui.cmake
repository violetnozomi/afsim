# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# GUI plugins that are non-exportable
set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL  "")

include("${CMAKE_CURRENT_LIST_DIR}/restricted.cmake")

set(BUILD_WITH_mover_creator TRUE CACHE BOOL "")

set(BUILD_MYSTIC_PLUGIN_ResultAcesDisplay TRUE CACHE BOOL "")
set(BUILD_MYSTIC_PLUGIN_ResultAirCombatVisualization TRUE CACHE BOOL "")
set(BUILD_MYSTIC_PLUGIN_ResultDataAirCombat TRUE CACHE BOOL "")
set(BUILD_MYSTIC_PLUGIN_ResultHeadDownView TRUE CACHE BOOL "")
set(BUILD_MYSTIC_PLUGIN_ResultHeadUpView TRUE CACHE BOOL "")
set(BUILD_MYSTIC_PLUGIN_ResultSituationAwarenessDisplay TRUE CACHE BOOL "")

set(BUILD_WARLOCK_PLUGIN_AcesDisplay TRUE CACHE BOOL "")
set(BUILD_WARLOCK_PLUGIN_AirToAirVisualization TRUE CACHE BOOL "")
set(BUILD_WARLOCK_PLUGIN_HeadDownView TRUE CACHE BOOL "")
set(BUILD_WARLOCK_PLUGIN_HeadUpView TRUE CACHE BOOL "")
set(BUILD_WARLOCK_PLUGIN_SituationAwarenessDisplay TRUE CACHE BOOL "")

set(BUILD_WIZARD_PLUGIN_WizEngage TRUE CACHE BOOL "")
set(BUILD_WIZARD_PLUGIN_WizSIMDIS TRUE CACHE BOOL "")
